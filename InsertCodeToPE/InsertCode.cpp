#include <math.h>
#include "Global.h"
#include "TableOperate.h"


//#define FILEPATH_IN "d:/notepad.exe"
#define FILEPATH_IN "d:/MyDLL.dll"
//#define FILEPATH_OUT "d:/notepad_new.exe"
#define FILEPATH_OUT "d:/MyDLL_new.dll"
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
		printf("(InsertCodeToFirstSection)文件读取失败\n");
		return 0;
	}

	CopyFileBufferToImageBuffer(pFileBuffer, &pImageBuffer);
	if (!pImageBuffer)
	{
		printf("(InsertCodeToFirstSection)filebuffer -> imagebuffer 失败\n");
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
		printf("(InsertCodeToFirstSection)代码区空间不足\n");
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
		printf("(InsertCodeToFirstSection)pImageBuffer ->pNewBuffer 失败\n");
		free(pImageBuffer);
		free(pFileBuffer);
		return 0;
	}

	BOOL isOK = MemoryToFile(pNewBuffer, fileSize, FILEPATH_OUT);
	if (!isOK)
	{
		printf("(InsertCodeToFirstSection)文件保存失败\n");
		free(pFileBuffer);
		free(pImageBuffer);
		free(pNewBuffer);
		return 0;
	}

	printf("(InsertCodeToFirstSection)保存成功\n");
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

	isOk = MoveHeader(pImageBuffer);
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
	BOOL isOK = FALSE;
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

	isOK = MergeSections(&pImageBuffer);
	if (!isOK)
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
	BOOL isOK = FALSE;
	DWORD fileSize = 0;
	DWORD newFileSize = 0;
	CHAR name[8] = "newSec";

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

	isOK = MoveHeader(pImageBuffer);
	if (!isOK)
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

VOID PrintExport()
{
	LPVOID pFileBuffer = NULL;

	ReadPEFile(FILEPATH_IN, &pFileBuffer);
	if (!pFileBuffer)
	{
		printf("(ReadPEFile)文件读取失败\n");
		return;
	}

	PrintExportTable(pFileBuffer);
	free(pFileBuffer);
}

VOID PrintRelocation()
{
	LPVOID pFileBuffer = 0;

	ReadPEFile(FILEPATH_IN, &pFileBuffer);
	if (!pFileBuffer)
	{
		printf("(PrintRelocation)文件读取失败\n");
		return;
	}

	PrintRelocationTable(pFileBuffer);
	free(pFileBuffer);
}

BOOL TestMoveExportTable()
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNtHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER pOptionHeader = NULL;
	PIMAGE_EXPORT_DIRECTORY pExportDirectory = NULL;
	LPVOID pFileBuffer = NULL;
	LPVOID pNewFileBuffer = NULL;
	CHAR name[8] = "NewSec";
	DWORD addrOfName = 0;
	DWORD nameStrAddr = 0;
	DWORD sizeOfExport = 0;
	DWORD fileSize = 0;
	DWORD addSize = 0;
	BOOL isOK = FALSE;

	fileSize = ReadPEFile(FILEPATH_IN, &pFileBuffer);
	if (!pFileBuffer)
	{
		printf("(TestMoveExportTable)文件读取失败\n");
		return FALSE;
	}
	
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);

	//将导出表RVA转换成FOA，再加上起始地址，算出在内存实际地址
	pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)(RVAtoFOA(pFileBuffer, pOptionHeader->DataDirectory->VirtualAddress) + (DWORD)pFileBuffer);
	//计算名字表在内存实际地址
	addrOfName = RVAtoFOA(pFileBuffer, pExportDirectory->AddressOfNames) + (DWORD)pFileBuffer;
	sizeOfExport = pExportDirectory->NumberOfFunctions * 4 + pExportDirectory->NumberOfNames * 6;
	for (DWORD i = 0; i < pExportDirectory->NumberOfNames; i++)
	{
		//计算名字每一项在内存的实际地址
		nameStrAddr = RVAtoFOA(pFileBuffer, ((PDWORD)addrOfName)[i]) + (DWORD)pFileBuffer;
		sizeOfExport += strlen((PCHAR)nameStrAddr) + 1;
		//printf("%s 长度：%d，sizeOfExport: %#x\n", (PCHAR)nameStrAddr, strlen((PCHAR)nameStrAddr) + 1, sizeOfExport);
	}
	addSize = ceil(sizeOfExport / (FLOAT)0x1000) * 0x1000;
	printf("导出表大小：%#x，需要新增节的大小：%#x\n", sizeOfExport, addSize);

	isOK = MoveHeader(pFileBuffer);
	if (!isOK)
	{
		printf("(TestMoveExportTable)移动头文件失败\n");
		free(pFileBuffer);
		return FALSE;
	}
	isOK = FALSE;

	AddNewSection(pFileBuffer, &pNewFileBuffer, fileSize, addSize, name);
	if (!pNewFileBuffer)
	{
		printf("(TestMoveExportTable)新增节失败\n");
		free(pFileBuffer);
		return FALSE;
	}

	isOK = MoveExportTable(pNewFileBuffer);
	if (!isOK)
	{
		printf("(TestMoveExportTable)移动导出表失败\n");
		free(pFileBuffer);
		free(pNewFileBuffer);
		return FALSE;
	}

	MemoryToFile(pNewFileBuffer, fileSize + addSize, FILEPATH_OUT);
	printf("(TestMoveExportTable)移动导出成功\n");
	free(pFileBuffer);
	free(pNewFileBuffer);
	return TRUE;
}

BOOL TestMoveRelocationTable()
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNtHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER pOptionHeader = NULL;
	PIMAGE_BASE_RELOCATION pBaseRelocation = NULL;
	PIMAGE_BASE_RELOCATION pBaseRelocationTemp = NULL;
	LPVOID pFileBuffer = NULL;
	LPVOID pNewFileBuffer = NULL;
	CHAR name[8] = "NewSec";
	DWORD sizeOfRelocation = 0;
	DWORD fileSize = 0;
	DWORD addSize = 0;
	BOOL isOK = FALSE;

	fileSize = ReadPEFile(FILEPATH_IN, &pFileBuffer);
	if (!pFileBuffer)
	{
		printf("(TestMoveRelocationTable)文件读取失败\n");
		return FALSE;
	}

	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pBaseRelocation = (PIMAGE_BASE_RELOCATION)(RVAtoFOA(pFileBuffer, (pOptionHeader->DataDirectory[5]).VirtualAddress) + (DWORD)pFileBuffer);
	//printf("%#x\n", RVAtoFOA(pFileBuffer, (pOptionHeader->DataDirectory[5]).VirtualAddress));

	while (pBaseRelocation->VirtualAddress && pBaseRelocation->SizeOfBlock)
	{
		sizeOfRelocation += pBaseRelocation->SizeOfBlock;
		pBaseRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)pBaseRelocation + pBaseRelocation->SizeOfBlock);
	}

	addSize = ceil(sizeOfRelocation / (FLOAT)0x1000) * 0x1000;
	printf("重定位表大小：%#x，需要新增节的大小：%#x\n", sizeOfRelocation, addSize);

	/*isOK = MoveHeader(pFileBuffer);
	if (!isOK)
	{
		printf("(TestMoveExportTable)移动头文件失败\n");
		free(pFileBuffer);
		return FALSE;
	}
	isOK = FALSE;*/

	AddNewSection(pFileBuffer, &pNewFileBuffer, fileSize, addSize, name);
	if (!pNewFileBuffer)
	{
		printf("(TestMoveRelocationTable)新增节失败\n");
		free(pFileBuffer);
		return FALSE;
	}

	isOK = MoveRelocationTable(pNewFileBuffer);
	if (!isOK)
	{
		printf("(TestMoveRelocationTable)移动重定位表失败\n");
		free(pFileBuffer);
		free(pNewFileBuffer);
		return FALSE;
	}

	MemoryToFile(pNewFileBuffer, fileSize + addSize, FILEPATH_OUT);
	printf("(TestMoveRelocationTable)移动重定位表成功\n");
	free(pFileBuffer);
	free(pNewFileBuffer);
	return TRUE;
}

BOOL TestRepairRelocationTable()
{
	LPVOID pFileBuffer = NULL;

	ReadPEFile("d:/MyDLL_new.dll", &pFileBuffer);
	if (!pFileBuffer)
	{
		printf("(TestRepairRelocationTable)文件读取失败\n");
		return FALSE;
	}

	RepairRelocationTable(pFileBuffer, 0x10000000);
	return 0;
}

int main()
{
	//InsertCodeToFirstSection();

	//FileBufferToImageBuffer();

	//InsertCodeToCustomSection(3);

	//MoveHeaderBehindDosHeader();

	//MergeSectionsToOne();

	//AddNewSectionAtEnd();

	//ModifyLastSectionSize();

	//PrintExport();

	//PrintRelocation();

	TestMoveExportTable();

	//TestMoveRelocationTable();

	return 0;
}