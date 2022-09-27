#include "Globle.h"

#define FILEPATH_IN "d:/notepad.exe"
#define FILEPATH_OUT "d:/notepad_new.exe"
#define MESSAGEBOXADDR 0x766C3B90

BYTE SHELLCODE[] =
{
	0x6A, 00, 0x6A, 00, 0x6A, 00, 0x6A, 00,	// push 0, push 0， push 0， push 0，
	0xE8, 00, 00, 00, 00,					// call xxxxxxxx
	0xE9, 00, 00, 00, 00,					// jmp xxxxxxxx
};

DWORD FileBufferToImageBuffer()
{
	LPVOID pFileBuffer = NULL;
	LPVOID pNewBuffer = NULL;
	LPVOID pImageBuffer = NULL;
	DWORD ReadSize = 0;
	DWORD RamSize = 0;
	DWORD SaveSize = 0;

	ReadSize = ReadPEFile(FILEPATH_IN, &pFileBuffer);
	printf("exe->filebuffer 读取到的文件大小为：%#x\n", ReadSize);

	RamSize = CopyFileBufferToImageBuffer(pFileBuffer, &pImageBuffer);
	printf("filebuffer->imagebuffer 在内存中展开的文件大小为：%#x\n", RamSize);

	SaveSize = CopyImageBufferToNewBuffer(pImageBuffer, &pNewBuffer);
	printf("imagebuffer->newbuffer 准备写回文件的大小为：%#x\n", SaveSize);

	MemoryToFile(pNewBuffer, SaveSize, FILEPATH_OUT);

	free(pFileBuffer);
	free(pNewBuffer);
	free(pImageBuffer);

	return 0;
}

DWORD InsertCodeToFirstSection()
{
	LPVOID pFileBuffer = NULL;
	LPVOID pImageBuffer = NULL;
	LPVOID pNewBuffer = NULL;
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	PBYTE codeBegin = NULL;
	DWORD fileSize = 0;

	DWORD size = ReadPEFile(FILEPATH_IN, &pFileBuffer);
	if (!pFileBuffer)
	{
		printf("文件读取失败\n");
		return 0;
	}

	CopyFileBufferToImageBuffer(pFileBuffer, &pImageBuffer);
	if (!pImageBuffer)
	{
		printf("filebuffer -> imagebuffer 失败\n");
		free(pFileBuffer);
		return 0;
	}

	pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	size_t ShellCodeLength = sizeof(SHELLCODE) / sizeof(BYTE);

	if (pSectionHeader->SizeOfRawData - pSectionHeader->Misc.VirtualSize < ShellCodeLength)
	{
		printf("代码区空间不足\n");
		free(pFileBuffer);
		free(pImageBuffer);
		return 0;
	}
	//计算插入位置
	codeBegin = (PBYTE)((DWORD)pImageBuffer + pSectionHeader->VirtualAddress + pSectionHeader->Misc.VirtualSize);
	//拷贝SHELLCODE
	memcpy(codeBegin, SHELLCODE, ShellCodeLength);
	//修正E8
	DWORD callAddr = MESSAGEBOXADDR - (pOptionHeader->ImageBase + ((DWORD)(codeBegin + 0xD) - (DWORD)pImageBuffer));
	*(PDWORD)(codeBegin + 9) = callAddr;
	//修正E9
	DWORD jmpAddr = (pOptionHeader->ImageBase + pOptionHeader->AddressOfEntryPoint) - (pOptionHeader->ImageBase + ((DWORD)(codeBegin + 0x12) - (DWORD)pImageBuffer));
	*(PDWORD)(codeBegin + 0xE) = jmpAddr;
	//修正oep
	pOptionHeader->AddressOfEntryPoint = pSectionHeader->VirtualAddress + pSectionHeader->Misc.VirtualSize;

	printf("%#x, %#x, %#x\n", pSectionHeader->VirtualAddress, pSectionHeader->Misc.VirtualSize, pOptionHeader->AddressOfEntryPoint);

	fileSize = CopyImageBufferToNewBuffer(pImageBuffer, &pNewBuffer);
	if (!pNewBuffer)
	{
		printf("pImageBuffer ->pNewBuffer 失败\n");
		free(pImageBuffer);
		free(pFileBuffer);
		return 0;
	}

	BOOL isOK = MemoryToFile(pNewBuffer, fileSize, FILEPATH_OUT);
	if (!isOK)
	{
		printf("文件保存失败\n");
		free(pFileBuffer);
		free(pImageBuffer);
		free(pNewBuffer);
		return 0;
	}

	printf("保存成功\n");
	free(pFileBuffer);
	free(pImageBuffer);
	free(pNewBuffer);
	return 1;
}

DWORD InsertCodeToCustomSection(DWORD index)
{
	LPVOID pFileBuffer = NULL;
	LPVOID pImageBuffer = NULL;
	LPVOID pNewBuffer = NULL;
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	PBYTE codeBegin = NULL;
	DWORD fileSize = 0;

	DWORD size = ReadPEFile(FILEPATH_IN, &pFileBuffer);
	if (!pFileBuffer)
	{
		printf("文件读取失败\n");
		return 0;
	}

	CopyFileBufferToImageBuffer(pFileBuffer, &pImageBuffer);
	if (!pImageBuffer)
	{
		printf("filebuffer -> imagebuffer 失败\n");
		free(pFileBuffer);
		return 0;
	}

	pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	if (index > pPEHeader->NumberOfSections)
	{
		printf("文件共有 %d 节，超出节数量\n", pPEHeader->NumberOfSections);
		free(pFileBuffer);
		free(pImageBuffer);
		return 0;
	}
	pSectionHeader += index - 1;
	LONG ShellCodeLength = sizeof(SHELLCODE) / sizeof(BYTE);

	if ((LONG)(pSectionHeader->SizeOfRawData - pSectionHeader->Misc.VirtualSize) < ShellCodeLength)
	{
		printf("第 %d 节空间不足\n", index);
		free(pFileBuffer);
		free(pImageBuffer);
		return 0;
	}
	//计算插入位置
	codeBegin = (PBYTE)((DWORD)pImageBuffer + pSectionHeader->VirtualAddress + pSectionHeader->Misc.VirtualSize);
	//拷贝SHELLCODE
	memcpy(codeBegin, SHELLCODE, ShellCodeLength);
	//修正E8
	DWORD callAddr = MESSAGEBOXADDR - (pOptionHeader->ImageBase + ((DWORD)(codeBegin + 0xD) - (DWORD)pImageBuffer));
	*(PDWORD)(codeBegin + 9) = callAddr;
	//修正E9
	DWORD jmpAddr = (pOptionHeader->ImageBase + pOptionHeader->AddressOfEntryPoint) - (pOptionHeader->ImageBase + ((DWORD)(codeBegin + 0x12) - (DWORD)pImageBuffer));
	*(PDWORD)(codeBegin + 0xE) = jmpAddr;
	//修正oep
	pOptionHeader->AddressOfEntryPoint = pSectionHeader->VirtualAddress + pSectionHeader->Misc.VirtualSize;
	//修正节读写属性
	DWORD x = (pSectionHeader + index - 1)->Characteristics;
	DWORD y = pSectionHeader->Characteristics;
	pSectionHeader->Characteristics = x | y;

	fileSize = CopyImageBufferToNewBuffer(pImageBuffer, &pNewBuffer);
	if (!pNewBuffer)
	{
		printf("pImageBuffer ->pNewBuffer 失败\n");
		free(pImageBuffer);
		free(pFileBuffer);
		return 0;
	}

	BOOL isOK = MemoryToFile(pNewBuffer, fileSize, FILEPATH_OUT);
	if (!isOK)
	{
		printf("文件保存失败\n");
		free(pFileBuffer);
		free(pImageBuffer);
		free(pNewBuffer);
		return 0;
	}

	printf("保存成功\n");
	free(pFileBuffer);
	free(pImageBuffer);
	free(pNewBuffer);
	return 1;
}

BOOL MoveHeaderBehindDosHeader()
{
	LPVOID pFileBuffer = NULL;
	LPVOID pImageBuffer = NULL;
	LPVOID pNewBuffer = NULL;
	BOOL isOk = FALSE;
	DWORD fileSize = 0;

	ReadPEFile(FILEPATH_IN, &pFileBuffer);
	if (!pFileBuffer)
	{
		printf("(MoveHeaderBehindDosHeader)exe->pFileBuffer失败\n");
		return 0;
	}

	CopyFileBufferToImageBuffer(pFileBuffer, &pImageBuffer);
	if (!pImageBuffer)
	{
		printf("(MoveHeaderBehindDosHeader)pFileBuffer->pImageBuffer失败\n");
		free(pFileBuffer);
		return 0;
	}

	isOk = MoveHeader(pFileBuffer, &pImageBuffer);
	if (!isOk)
	{
		printf("(MoveHeaderBehindDosHeader)移动文件头失败\n");
		free(pFileBuffer);
		free(pImageBuffer);
		return FALSE;
	}

	fileSize = CopyImageBufferToNewBuffer(pImageBuffer, &pNewBuffer);

	MemoryToFile(pNewBuffer, fileSize, FILEPATH_OUT);
	free(pFileBuffer);
	free(pImageBuffer);
	free(pNewBuffer);
	printf("(MoveHeaderBehindDosHeader)移动文件头成功\n");
	return TRUE;
}

BOOL MergeSectionsToOne()
{
	LPVOID pFileBuffer = NULL;
	LPVOID pImageBuffer = NULL;
	LPVOID pNewBuffer = NULL;
	BOOL isOk = FALSE;
	DWORD fileSize = 0;

	ReadPEFile(FILEPATH_IN, &pFileBuffer);
	if (!pFileBuffer)
	{
		printf("(MergeSectionsToOne)exe->pFileBuffer失败\n");
		return 0;
	}

	CopyFileBufferToImageBuffer(pFileBuffer, &pImageBuffer);
	if (!pImageBuffer)
	{
		printf("(MergeSectionsToOne)pFileBuffer->pImageBuffer失败\n");
		free(pFileBuffer);
		return 0;
	}

	isOk = MergeSections(&pImageBuffer);
	if (!isOk)
	{
		printf("(MergeSectionsToOne)合并section失败\n");
		free(pFileBuffer);
		free(pImageBuffer);
		return 0;
	}

	fileSize = CopyImageBufferToNewBuffer(pImageBuffer, &pNewBuffer);

	MemoryToFile(pNewBuffer, fileSize, FILEPATH_OUT);
	free(pFileBuffer);
	free(pImageBuffer);
	free(pNewBuffer);
	printf("(MergeSectionsToOne)合并section成功\n");
	return TRUE;
}

//在最后添加一个section，先移动header，避免破坏其它数据，也可以先合并节
BOOL AddNewSectionAtEnd()
{
	LPVOID pFileBuffer = NULL;
	LPVOID pImageBuffer = NULL;
	LPVOID pNewImageBuffer = NULL;
	LPVOID pNewBuffer = NULL;
	BOOL isOk = FALSE;
	DWORD fileSize = 0;
	DWORD newFileSize = 0;
	BYTE name[8] = "newSec";

	ReadPEFile(FILEPATH_IN, &pFileBuffer);
	if (!pFileBuffer)
	{
		printf("(AddNewSectionAtEnd)exe->pFileBuffer失败\n");
		return 0;
	}

	fileSize = CopyFileBufferToImageBuffer(pFileBuffer, &pImageBuffer);
	if (!pImageBuffer)
	{
		printf("(AddNewSectionAtEnd)pFileBuffer->pImageBuffer失败\n");
		free(pFileBuffer);
		return 0;
	}

	isOk = MoveHeader(pFileBuffer, &pImageBuffer);
	if (!isOk)
	{
		printf("(AddNewSectionAtEnd)移动文件头失败\n");
		free(pFileBuffer);
		free(pImageBuffer);
		return FALSE;
	}

	AddNewSection(pImageBuffer, &pNewImageBuffer, fileSize, 0x1000, name);
	if (!pNewImageBuffer)
	{
		printf("(AddNewSectionAtEnd)添加节失败\n");
		free(pFileBuffer);
		free(pImageBuffer);
		return 0;
	}

	newFileSize = CopyImageBufferToNewBuffer(pNewImageBuffer, &pNewBuffer);
	if (!pNewBuffer)
	{
		printf("(AddNewSectionAtEnd)缩小文件失败\n");
		free(pFileBuffer);
		free(pImageBuffer);
		free(pNewImageBuffer);
		return 0;		
	}

	MemoryToFile(pNewBuffer, newFileSize, FILEPATH_OUT);
	free(pFileBuffer);
	free(pImageBuffer);
	free(pNewImageBuffer);
	free(pNewBuffer);
	printf("(AddNewSectionAtEnd)添加section成功\n");
	return TRUE;
}

BOOL ModifyLastSectionSize()
{
	LPVOID pFileBuffer = NULL;
	LPVOID pImageBuffer = NULL;
	LPVOID pNewImageBuffer = NULL;
	LPVOID pNewBuffer = NULL;
	BOOL isOk = FALSE;
	DWORD fileSize = 0;
	DWORD newFileSize = 0;

	ReadPEFile(FILEPATH_IN, &pFileBuffer);
	if (!pFileBuffer)
	{
		printf("(ModifyLastSectionSize)exe->pFileBuffer失败\n");
		return 0;
	}

	fileSize = CopyFileBufferToImageBuffer(pFileBuffer, &pImageBuffer);
	if (!pImageBuffer)
	{
		printf("(ModifyLastSectionSize)pFileBuffer->pImageBuffer失败\n");
		free(pFileBuffer);
		return 0;
	}

	ExpandLastSection(pImageBuffer, &pNewImageBuffer, fileSize, 0x1000);
	if (!pNewImageBuffer)
	{
		printf("(ModifyLastSectionSize)扩大最后一个section失败\n");
		free(pFileBuffer);
		free(pImageBuffer);
		return 0;
	}

	newFileSize = CopyImageBufferToNewBuffer(pNewImageBuffer, &pNewBuffer);
	if (!pNewBuffer)
	{
		printf("(ModifyLastSectionSize)缩小文件失败\n");
		free(pFileBuffer);
		free(pImageBuffer);
		free(pNewImageBuffer);
		return 0;
	}

	MemoryToFile(pNewBuffer, newFileSize, FILEPATH_OUT);
	free(pFileBuffer);
	free(pImageBuffer);
	free(pNewImageBuffer);
	free(pNewBuffer);
	printf("(ModifyLastSectionSize)扩大最后一个section成功\n");
	return TRUE;
}

int main()
{
	//InsertCodeToFirstSection();

	//FileBufferToImageBuffer();

	//InsertCodeToCustomSection(3);

	//MoveHeaderBehindDosHeader();

	//MergeSectionsToOne();

	//AddNewSectionAtEnd();

	ModifyLastSectionSize();

	return 0;
}