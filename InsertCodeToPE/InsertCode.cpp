#include "Globle.h"

#define FILEPATH_IN "d:/notepad.exe"
#define FILEPATH_OUT "d:/notepad_new.exe"
#define MESSAGEBOXADDR 0x766C3B90

BYTE SHELLCODE[] =
{
	0x6A, 00, 0x6A, 00, 0x6A, 00, 0x6A, 00,	// push 0, push 0�� push 0�� push 0��
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
	printf("exe->filebuffer ��ȡ�����ļ���СΪ��%#x\n", ReadSize);

	RamSize = CopyFileBufferToImageBuffer(pFileBuffer, &pImageBuffer);
	printf("filebuffer->imagebuffer ���ڴ���չ�����ļ���СΪ��%#x\n", RamSize);

	SaveSize = CopyImageBufferToNewBuffer(pImageBuffer, &pNewBuffer);
	printf("imagebuffer->newbuffer ׼��д���ļ��Ĵ�СΪ��%#x\n", SaveSize);

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
		printf("�ļ���ȡʧ��\n");
		return 0;
	}

	CopyFileBufferToImageBuffer(pFileBuffer, &pImageBuffer);
	if (!pImageBuffer)
	{
		printf("filebuffer -> imagebuffer ʧ��\n");
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
		printf("�������ռ䲻��\n");
		free(pFileBuffer);
		free(pImageBuffer);
		return 0;
	}
	//�������λ��
	codeBegin = (PBYTE)((DWORD)pImageBuffer + pSectionHeader->VirtualAddress + pSectionHeader->Misc.VirtualSize);
	//����SHELLCODE
	memcpy(codeBegin, SHELLCODE, ShellCodeLength);
	//����E8
	DWORD callAddr = MESSAGEBOXADDR - (pOptionHeader->ImageBase + ((DWORD)(codeBegin + 0xD) - (DWORD)pImageBuffer));
	*(PDWORD)(codeBegin + 9) = callAddr;
	//����E9
	DWORD jmpAddr = (pOptionHeader->ImageBase + pOptionHeader->AddressOfEntryPoint) - (pOptionHeader->ImageBase + ((DWORD)(codeBegin + 0x12) - (DWORD)pImageBuffer));
	*(PDWORD)(codeBegin + 0xE) = jmpAddr;
	//����oep
	pOptionHeader->AddressOfEntryPoint = pSectionHeader->VirtualAddress + pSectionHeader->Misc.VirtualSize;

	printf("%#x, %#x, %#x\n", pSectionHeader->VirtualAddress, pSectionHeader->Misc.VirtualSize, pOptionHeader->AddressOfEntryPoint);

	fileSize = CopyImageBufferToNewBuffer(pImageBuffer, &pNewBuffer);
	if (!pNewBuffer)
	{
		printf("pImageBuffer ->pNewBuffer ʧ��\n");
		free(pImageBuffer);
		free(pFileBuffer);
		return 0;
	}

	BOOL isOK = MemoryToFile(pNewBuffer, fileSize, FILEPATH_OUT);
	if (!isOK)
	{
		printf("�ļ�����ʧ��\n");
		free(pFileBuffer);
		free(pImageBuffer);
		free(pNewBuffer);
		return 0;
	}

	printf("����ɹ�\n");
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
		printf("�ļ���ȡʧ��\n");
		return 0;
	}

	CopyFileBufferToImageBuffer(pFileBuffer, &pImageBuffer);
	if (!pImageBuffer)
	{
		printf("filebuffer -> imagebuffer ʧ��\n");
		free(pFileBuffer);
		return 0;
	}

	pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageBuffer + pDosHeader->e_lfanew + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	if (index > pPEHeader->NumberOfSections)
	{
		printf("�ļ����� %d �ڣ�����������\n", pPEHeader->NumberOfSections);
		free(pFileBuffer);
		free(pImageBuffer);
		return 0;
	}
	pSectionHeader += index - 1;
	LONG ShellCodeLength = sizeof(SHELLCODE) / sizeof(BYTE);

	if ((LONG)(pSectionHeader->SizeOfRawData - pSectionHeader->Misc.VirtualSize) < ShellCodeLength)
	{
		printf("�� %d �ڿռ䲻��\n", index);
		free(pFileBuffer);
		free(pImageBuffer);
		return 0;
	}
	//�������λ��
	codeBegin = (PBYTE)((DWORD)pImageBuffer + pSectionHeader->VirtualAddress + pSectionHeader->Misc.VirtualSize);
	//����SHELLCODE
	memcpy(codeBegin, SHELLCODE, ShellCodeLength);
	//����E8
	DWORD callAddr = MESSAGEBOXADDR - (pOptionHeader->ImageBase + ((DWORD)(codeBegin + 0xD) - (DWORD)pImageBuffer));
	*(PDWORD)(codeBegin + 9) = callAddr;
	//����E9
	DWORD jmpAddr = (pOptionHeader->ImageBase + pOptionHeader->AddressOfEntryPoint) - (pOptionHeader->ImageBase + ((DWORD)(codeBegin + 0x12) - (DWORD)pImageBuffer));
	*(PDWORD)(codeBegin + 0xE) = jmpAddr;
	//����oep
	pOptionHeader->AddressOfEntryPoint = pSectionHeader->VirtualAddress + pSectionHeader->Misc.VirtualSize;
	//�����ڶ�д����
	DWORD x = (pSectionHeader + index - 1)->Characteristics;
	DWORD y = pSectionHeader->Characteristics;
	pSectionHeader->Characteristics = x | y;

	fileSize = CopyImageBufferToNewBuffer(pImageBuffer, &pNewBuffer);
	if (!pNewBuffer)
	{
		printf("pImageBuffer ->pNewBuffer ʧ��\n");
		free(pImageBuffer);
		free(pFileBuffer);
		return 0;
	}

	BOOL isOK = MemoryToFile(pNewBuffer, fileSize, FILEPATH_OUT);
	if (!isOK)
	{
		printf("�ļ�����ʧ��\n");
		free(pFileBuffer);
		free(pImageBuffer);
		free(pNewBuffer);
		return 0;
	}

	printf("����ɹ�\n");
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
		printf("(MoveHeaderBehindDosHeader)exe->pFileBufferʧ��\n");
		return 0;
	}

	CopyFileBufferToImageBuffer(pFileBuffer, &pImageBuffer);
	if (!pImageBuffer)
	{
		printf("(MoveHeaderBehindDosHeader)pFileBuffer->pImageBufferʧ��\n");
		free(pFileBuffer);
		return 0;
	}

	isOk = MoveHeader(pFileBuffer, &pImageBuffer);
	if (!isOk)
	{
		printf("(MoveHeaderBehindDosHeader)�ƶ��ļ�ͷʧ��\n");
		free(pFileBuffer);
		free(pImageBuffer);
		return FALSE;
	}

	fileSize = CopyImageBufferToNewBuffer(pImageBuffer, &pNewBuffer);

	MemoryToFile(pNewBuffer, fileSize, FILEPATH_OUT);
	free(pFileBuffer);
	free(pImageBuffer);
	free(pNewBuffer);
	printf("(MoveHeaderBehindDosHeader)�ƶ��ļ�ͷ�ɹ�\n");
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
		printf("(MergeSectionsToOne)exe->pFileBufferʧ��\n");
		return 0;
	}

	CopyFileBufferToImageBuffer(pFileBuffer, &pImageBuffer);
	if (!pImageBuffer)
	{
		printf("(MergeSectionsToOne)pFileBuffer->pImageBufferʧ��\n");
		free(pFileBuffer);
		return 0;
	}

	isOk = MergeSections(&pImageBuffer);
	if (!isOk)
	{
		printf("(MergeSectionsToOne)�ϲ�sectionʧ��\n");
		free(pFileBuffer);
		free(pImageBuffer);
		return 0;
	}

	fileSize = CopyImageBufferToNewBuffer(pImageBuffer, &pNewBuffer);

	MemoryToFile(pNewBuffer, fileSize, FILEPATH_OUT);
	free(pFileBuffer);
	free(pImageBuffer);
	free(pNewBuffer);
	printf("(MergeSectionsToOne)�ϲ�section�ɹ�\n");
	return TRUE;
}

//��������һ��section�����ƶ�header�������ƻ��������ݣ�Ҳ�����Ⱥϲ���
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
		printf("(AddNewSectionAtEnd)exe->pFileBufferʧ��\n");
		return 0;
	}

	fileSize = CopyFileBufferToImageBuffer(pFileBuffer, &pImageBuffer);
	if (!pImageBuffer)
	{
		printf("(AddNewSectionAtEnd)pFileBuffer->pImageBufferʧ��\n");
		free(pFileBuffer);
		return 0;
	}

	isOk = MoveHeader(pFileBuffer, &pImageBuffer);
	if (!isOk)
	{
		printf("(AddNewSectionAtEnd)�ƶ��ļ�ͷʧ��\n");
		free(pFileBuffer);
		free(pImageBuffer);
		return FALSE;
	}

	AddNewSection(pImageBuffer, &pNewImageBuffer, fileSize, 0x1000, name);
	if (!pNewImageBuffer)
	{
		printf("(AddNewSectionAtEnd)��ӽ�ʧ��\n");
		free(pFileBuffer);
		free(pImageBuffer);
		return 0;
	}

	newFileSize = CopyImageBufferToNewBuffer(pNewImageBuffer, &pNewBuffer);
	if (!pNewBuffer)
	{
		printf("(AddNewSectionAtEnd)��С�ļ�ʧ��\n");
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
	printf("(AddNewSectionAtEnd)���section�ɹ�\n");
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
		printf("(ModifyLastSectionSize)exe->pFileBufferʧ��\n");
		return 0;
	}

	fileSize = CopyFileBufferToImageBuffer(pFileBuffer, &pImageBuffer);
	if (!pImageBuffer)
	{
		printf("(ModifyLastSectionSize)pFileBuffer->pImageBufferʧ��\n");
		free(pFileBuffer);
		return 0;
	}

	ExpandLastSection(pImageBuffer, &pNewImageBuffer, fileSize, 0x1000);
	if (!pNewImageBuffer)
	{
		printf("(ModifyLastSectionSize)�������һ��sectionʧ��\n");
		free(pFileBuffer);
		free(pImageBuffer);
		return 0;
	}

	newFileSize = CopyImageBufferToNewBuffer(pNewImageBuffer, &pNewBuffer);
	if (!pNewBuffer)
	{
		printf("(ModifyLastSectionSize)��С�ļ�ʧ��\n");
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
	printf("(ModifyLastSectionSize)�������һ��section�ɹ�\n");
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