#include "Global.h"

DWORD ReadPEFile(IN LPCSTR lpszFile, OUT LPVOID* pFileBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	FILE *pFile = NULL;
	errno_t err = 0;
	DWORD fileSize = 0;
	PVOID pFileBufferTemp = NULL;

	err = fopen_s(&pFile, lpszFile, "rb");
	if (!pFile)
	{
		printf("(ReadPEFile)�ļ���ʧ��,������룺%d\n", err);
		return 0;
	}

	fseek(pFile, 0, SEEK_END);
	fileSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	pFileBufferTemp = malloc(fileSize);
	if (!pFileBufferTemp)
	{
		printf("(ReadPEFile)��̬�����ڴ�ʧ��\n");
		fclose(pFile);
		return 0;
	}

	DWORD n = fread(pFileBufferTemp, sizeof(BYTE), fileSize, pFile);
	if (!n)
	{
		printf("(ReadPEFile)��ȡ�ļ�ʧ��\n");
		fclose(pFile);
		free(pFileBufferTemp);
		return 0;
	}
	if (*((PWORD)pFileBufferTemp) != IMAGE_DOS_SIGNATURE)
	{
		printf("(ReadPEFile)��Ч��MZ��ʶ\n");
		free(pFileBufferTemp);
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBufferTemp;
	if (*((LPWORD)((DWORD)pFileBufferTemp + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE)
	{
		printf("(ReadPEFile)��Ч��PE���\n");
		free(pFileBufferTemp);
		return 0;
	}

	*pFileBuffer = pFileBufferTemp;
	pFileBufferTemp = NULL;
	fclose(pFile);
	return fileSize;
}

DWORD CopyFileBufferToImageBuffer(IN LPVOID pFileBuffer, OUT LPVOID* pImageBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS32 pNtHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;

	LPVOID pImageBufferTemp = NULL;

	if (!pFileBuffer)
	{
		printf("(CopyFileBufferToImageBuffer)FileBuffer ��ȡʧ��\n");
		return 0;
	}

	if (*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("(CopyFileBufferToImageBuffer)��Ч��MZ��ʶ\n");
		free(pFileBuffer);
		return 0;
	}

	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	if (*((LPWORD)((DWORD)pFileBuffer + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE)
	{
		printf("(CopyFileBufferToImageBuffer)��Ч��PE���\n");
		free(pFileBuffer);
		return 0;
	}

	pNtHeader = (PIMAGE_NT_HEADERS32)((DWORD)pDosHeader + pDosHeader->e_lfanew);
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	pImageBufferTemp = malloc(pOptionHeader->SizeOfImage);
	if (!pImageBufferTemp)
	{
		printf("(CopyFileBufferToImageBuffer)�ڴ�ռ�����ʧ��\n");
		free(pFileBuffer);
		return 0;
	}

	memset(pImageBufferTemp, 0, pOptionHeader->SizeOfImage);	//pImageBufferTemp�ռ�
	memcpy(pImageBufferTemp, pDosHeader, pOptionHeader->SizeOfHeaders);	//��pDosHeader����pOptionHeader->SizeOfHeaders��С�����ݵ�pImageBuffer

	//ѭ��������
	PIMAGE_SECTION_HEADER pSectionTemp = pSectionHeader;
	for (DWORD i = 0; i < pPEHeader->NumberOfSections; i++, pSectionTemp++)
	{
		memcpy((PDWORD)((DWORD)pImageBufferTemp + pSectionTemp->VirtualAddress), (PDWORD)((DWORD)pFileBuffer + pSectionTemp->PointerToRawData), pSectionTemp->SizeOfRawData);
	}

	*pImageBuffer = pImageBufferTemp;
	pImageBufferTemp = NULL;

	return pOptionHeader->SizeOfImage;
}

DWORD CopyImageBufferToNewBuffer(IN LPVOID pImageBuffer, OUT LPVOID* pNewBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS32 pNtHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;

	DWORD FileSize = 0;
	LPVOID pNewBufferTemp = NULL;

	if (!pImageBuffer)
	{
		printf("(CopyImageBufferToNewBuffer)ImageBuffer ��ȡʧ��");
		return 0;
	}

	pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	pNtHeader = (PIMAGE_NT_HEADERS32)((DWORD)pImageBuffer + pDosHeader->e_lfanew);
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	DWORD SecNum = pPEHeader->NumberOfSections;
	FileSize = pSectionHeader[SecNum - 1].PointerToRawData + pSectionHeader[SecNum - 1].SizeOfRawData;

	////����SizeOfRawData��С�����ļ���С
	//FileSize = pOptionHeader->SizeOfHeaders;
	//for (DWORD i = 0; i < pPEHeader->NumberOfSections; i++)
	//{
	//	FileSize += pSectionHeader[i].SizeOfRawData;
	//}

	pNewBufferTemp = malloc(FileSize);
	if (!pNewBufferTemp)
	{
		printf("(CopyImageBufferToNewBuffer)�ڴ�ռ����ʧ��\n");
		free(pImageBuffer);
		return 0;
	}

	memset(pNewBufferTemp, 0, FileSize);
	memcpy(pNewBufferTemp, pImageBuffer, pOptionHeader->SizeOfHeaders);	//�����ļ�ͷ

	PIMAGE_SECTION_HEADER pSectionTemp = pSectionHeader;
	for (DWORD i = 0; i < pPEHeader->NumberOfSections; i++, pSectionTemp++)
	{
		memcpy((PDWORD)((DWORD)pNewBufferTemp + pSectionTemp->PointerToRawData), (PDWORD)((DWORD)pImageBuffer + pSectionTemp->VirtualAddress), pSectionTemp->SizeOfRawData);
	}
	*pNewBuffer = pNewBufferTemp;
	pNewBufferTemp = NULL;
	return FileSize;
}

BOOL MemoryToFile(IN LPVOID pMemBuffer, IN size_t size, OUT LPCSTR lpszFile)
{
	FILE* pFile = NULL;
	errno_t err = 0;
	err = fopen_s(&pFile, lpszFile, "wb+");
	if (!pFile)
	{
		printf("(MemoryToFile)�ļ�����ʧ��,������룺%d\n", err);
		free(pMemBuffer);
		return FALSE;
	}

	fwrite(pMemBuffer, sizeof(BYTE), size, pFile);
	fclose(pFile);
	return TRUE;
}

DWORD RVAtoFOA(IN LPVOID pFileBuffer, IN DWORD dwRVA)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS32 pNtHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	DWORD dwFOA = 0;
	DWORD maxSize = 0;

	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pNtHeader = (PIMAGE_NT_HEADERS32)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	if (dwRVA < pOptionHeader->SizeOfHeaders)
	{
		return dwRVA;
	}

	for (DWORD i = 0; i < pPEHeader->NumberOfSections; i++)
	{
		maxSize = pSectionHeader[i].Misc.VirtualSize > pSectionHeader[i].SizeOfRawData ? pSectionHeader[i].Misc.VirtualSize : pSectionHeader[i].SizeOfRawData;
		if (pSectionHeader[i].VirtualAddress + maxSize > dwRVA)
		{
			dwFOA = dwRVA - pSectionHeader[i].VirtualAddress + pSectionHeader[i].PointerToRawData;
			return dwFOA;
		}
	}

	printf("(RVAtoFOA)RVA�����ļ���С\n");
	return 0;
}

BOOL MoveHeader(IN LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS32 pNtHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;

	LPVOID pNewNtHeader = NULL;
	DWORD size = 0;

	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	//���dosheader��ntheader֮�䲻��һ���ڴ�С�����ƶ�
	if (pDosHeader->e_lfanew - 64 < IMAGE_SIZEOF_SECTION_HEADER)
	{
		printf("(MoveHeader)�ռ䲻�㣬�޷��ƶ�header\n");
		return 0;
	}
	
	pNtHeader = (PIMAGE_NT_HEADERS32)((DWORD)pFileBuffer+pDosHeader->e_lfanew);
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	
	//Ŀ����ʼ��ַ
	pNewNtHeader = (LPVOID)((DWORD)(pFileBuffer)+ 64);
	//Ҫ�ƶ��Ĵ�С
	size = 4 + IMAGE_SIZEOF_FILE_HEADER + pPEHeader->SizeOfOptionalHeader + (pPEHeader->NumberOfSections) * 40;
	//��ʼ�ƶ�
	memcpy(pNewNtHeader, (LPVOID)pNtHeader, size);
	//�����һ��section�������һ��ȫ0��section
	memset((LPVOID)((DWORD)(pFileBuffer) + 64 + size), 0, 40);
	//����elfanew��ֵ
	((PIMAGE_DOS_HEADER)(pFileBuffer))->e_lfanew = 64;

	return TRUE;
}

BOOL MergeSections(OUT LPVOID* pImageBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS32 pNtHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;

	DWORD dataSize = 0;
	DWORD characterise = 0;


	pDosHeader = (PIMAGE_DOS_HEADER)(*pImageBuffer);
	pNtHeader = (PIMAGE_NT_HEADERS32)((DWORD)(*pImageBuffer) + pDosHeader->e_lfanew);
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	if (pPEHeader->NumberOfSections == 1)
	{
		printf("(MergeSections)ֻ��һ���ڣ�����Ҫ�ϲ�\n");
		free(*pImageBuffer);
		return 0;
	}

	//������������нڵĴ�С
	dataSize = pOptionHeader->SizeOfImage - pSectionHeader->VirtualAddress;
	//��������������Ҫ��Ȩ��
	characterise = pSectionHeader->Characteristics;
	for (DWORD i = 0; i < pPEHeader->NumberOfSections; i++)
	{
		characterise |= pSectionHeader[i].Characteristics;
	}
	//����Ȩ��
	pSectionHeader->Characteristics = characterise;
	//VirtualSize��SizeOfRawData����Ϊһ����
	pSectionHeader->Misc.VirtualSize = dataSize;
	pSectionHeader->SizeOfRawData = dataSize;
	//����section����Ϊ1
	pPEHeader->NumberOfSections = 1;
	return TRUE;
}

BOOL AddNewSection(IN LPVOID pImageBuffer, OUT LPVOID* pNewImageBuffer, IN DWORD fileSize, IN DWORD addSize, IN PCHAR name)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS32 pNtHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	DWORD index = 0;
	DWORD newFileSize = 0;
	PIMAGE_SECTION_HEADER newSectionHeader = NULL;
	LPVOID pNewImageBufferTemp = NULL;

	newFileSize = fileSize + addSize;
	pNewImageBufferTemp = malloc(newFileSize);
	if (!pNewImageBufferTemp)
	{
		printf("(AddNewSection)�ڴ����ʧ��\n");
		free(pImageBuffer);
		return 0;
	}

	memset(pNewImageBufferTemp, 0, newFileSize);
	memcpy(pNewImageBufferTemp, pImageBuffer, fileSize);

	pDosHeader = (PIMAGE_DOS_HEADER)pNewImageBufferTemp;
	pNtHeader = (PIMAGE_NT_HEADERS32)((DWORD)pNewImageBufferTemp + pDosHeader->e_lfanew);
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	index = pPEHeader->NumberOfSections;
	newSectionHeader = pSectionHeader + index;
	memcpy(newSectionHeader->Name, name, 8);
	newSectionHeader->Misc.VirtualSize = addSize;
	newSectionHeader->VirtualAddress = pOptionHeader->SizeOfImage;
	newSectionHeader->SizeOfRawData = addSize;
	newSectionHeader->PointerToRawData = pSectionHeader[index - 1].PointerToRawData + pSectionHeader[index - 1].SizeOfRawData;
	newSectionHeader->PointerToRelocations = 0;
	newSectionHeader->PointerToLinenumbers = 0;
	newSectionHeader->NumberOfRelocations = 0;
	newSectionHeader->NumberOfLinenumbers = 0;
	newSectionHeader->Characteristics = 0xE0000060;
	//�ڽڱ�����һ��ȫ��ڱ���ʾ����
	memset(newSectionHeader + 1, 0, sizeof(IMAGE_SECTION_HEADER));

	pPEHeader->NumberOfSections += 1;
	pOptionHeader->SizeOfImage += addSize;

	*pNewImageBuffer = pNewImageBufferTemp;
	pNewImageBufferTemp = NULL;
	return newFileSize;	
}

BOOL ExpandLastSection(IN LPVOID pImageBuffer, OUT LPVOID* pNewImageBuffer, IN DWORD fileSize, IN DWORD addSize)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS32 pNtHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	DWORD index = 0;
	DWORD newFileSize = 0;
	DWORD newSectionSize = 0;
	LPVOID pNewImageBufferTemp = NULL;

	newFileSize = fileSize + addSize;
	pNewImageBufferTemp = malloc(newFileSize);
	if (!pNewImageBufferTemp)
	{
		printf("(ExpandLastSection)�ڴ����ʧ��\n");
		free(pImageBuffer);
		return 0;
	}

	memset(pNewImageBufferTemp, 0, newFileSize);
	memcpy(pNewImageBufferTemp, pImageBuffer, fileSize);

	pDosHeader = (PIMAGE_DOS_HEADER)pNewImageBufferTemp;
	pNtHeader = (PIMAGE_NT_HEADERS32)((DWORD)pNewImageBufferTemp + pDosHeader->e_lfanew);
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	index = pPEHeader->NumberOfSections;
	//�������һ����������ڴ�Ĵ�С
	newSectionSize = pOptionHeader->SizeOfImage - pSectionHeader[index - 1].VirtualAddress + addSize;
	//�����һ�ڵ�VirtualSize��SizeOfRawData������Ϊ�µĴ�С
	pSectionHeader[index - 1].Misc.VirtualSize = newSectionSize;
	pSectionHeader[index - 1].SizeOfRawData = newSectionSize;
	//�޸�SizeOfImage
	pOptionHeader->SizeOfImage += addSize;

	*pNewImageBuffer = pNewImageBufferTemp;
	pNewImageBufferTemp = NULL;
	return TRUE;
}