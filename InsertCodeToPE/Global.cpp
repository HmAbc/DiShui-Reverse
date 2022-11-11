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
		printf("(ReadPEFile)文件打开失败,错误代码：%d\n", err);
		return 0;
	}

	fseek(pFile, 0, SEEK_END);
	fileSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	pFileBufferTemp = malloc(fileSize);
	if (!pFileBufferTemp)
	{
		printf("(ReadPEFile)动态分配内存失败\n");
		fclose(pFile);
		return 0;
	}

	DWORD n = fread(pFileBufferTemp, sizeof(BYTE), fileSize, pFile);
	if (!n)
	{
		printf("(ReadPEFile)读取文件失败\n");
		fclose(pFile);
		free(pFileBufferTemp);
		return 0;
	}
	if (*((PWORD)pFileBufferTemp) != IMAGE_DOS_SIGNATURE)
	{
		printf("(ReadPEFile)无效的MZ标识\n");
		free(pFileBufferTemp);
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBufferTemp;
	if (*((LPWORD)((DWORD)pFileBufferTemp + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE)
	{
		printf("(ReadPEFile)无效的PE标记\n");
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
		printf("(CopyFileBufferToImageBuffer)FileBuffer 获取失败\n");
		return 0;
	}

	if (*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("(CopyFileBufferToImageBuffer)无效的MZ标识\n");
		free(pFileBuffer);
		return 0;
	}

	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	if (*((LPWORD)((DWORD)pFileBuffer + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE)
	{
		printf("(CopyFileBufferToImageBuffer)无效的PE标记\n");
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
		printf("(CopyFileBufferToImageBuffer)内存空间申请失败\n");
		free(pFileBuffer);
		return 0;
	}

	memset(pImageBufferTemp, 0, pOptionHeader->SizeOfImage);	//pImageBufferTemp空间
	memcpy(pImageBufferTemp, pDosHeader, pOptionHeader->SizeOfHeaders);	//从pDosHeader拷贝pOptionHeader->SizeOfHeaders大小的数据到pImageBuffer

	//循环拷贝节
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
		printf("(CopyImageBufferToNewBuffer)ImageBuffer 获取失败");
		return 0;
	}

	pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	pNtHeader = (PIMAGE_NT_HEADERS32)((DWORD)pImageBuffer + pDosHeader->e_lfanew);
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	DWORD SecNum = pPEHeader->NumberOfSections;
	FileSize = pSectionHeader[SecNum - 1].PointerToRawData + pSectionHeader[SecNum - 1].SizeOfRawData;

	////按照SizeOfRawData大小计算文件大小
	//FileSize = pOptionHeader->SizeOfHeaders;
	//for (DWORD i = 0; i < pPEHeader->NumberOfSections; i++)
	//{
	//	FileSize += pSectionHeader[i].SizeOfRawData;
	//}

	pNewBufferTemp = malloc(FileSize);
	if (!pNewBufferTemp)
	{
		printf("(CopyImageBufferToNewBuffer)内存空间分配失败\n");
		free(pImageBuffer);
		return 0;
	}

	memset(pNewBufferTemp, 0, FileSize);
	memcpy(pNewBufferTemp, pImageBuffer, pOptionHeader->SizeOfHeaders);	//拷贝文件头

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
		printf("(MemoryToFile)文件创建失败,错误代码：%d\n", err);
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

	printf("(RVAtoFOA)RVA超出文件大小\n");
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
	//如果dosheader和ntheader之间不够一个节大小，不移动
	if (pDosHeader->e_lfanew - 64 < IMAGE_SIZEOF_SECTION_HEADER)
	{
		printf("(MoveHeader)空间不足，无法移动header\n");
		return 0;
	}
	
	pNtHeader = (PIMAGE_NT_HEADERS32)((DWORD)pFileBuffer+pDosHeader->e_lfanew);
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	
	//目的起始地址
	pNewNtHeader = (LPVOID)((DWORD)(pFileBuffer)+ 64);
	//要移动的大小
	size = 4 + IMAGE_SIZEOF_FILE_HEADER + pPEHeader->SizeOfOptionalHeader + (pPEHeader->NumberOfSections) * 40;
	//开始移动
	memcpy(pNewNtHeader, (LPVOID)pNtHeader, size);
	//在最后一个section后面添加一个全0的section
	memset((LPVOID)((DWORD)(pFileBuffer) + 64 + size), 0, 40);
	//设置elfanew的值
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
		printf("(MergeSections)只有一个节，不需要合并\n");
		free(*pImageBuffer);
		return 0;
	}

	//计算拉伸后所有节的大小
	dataSize = pOptionHeader->SizeOfImage - pSectionHeader->VirtualAddress;
	//满足所有数据需要的权限
	characterise = pSectionHeader->Characteristics;
	for (DWORD i = 0; i < pPEHeader->NumberOfSections; i++)
	{
		characterise |= pSectionHeader[i].Characteristics;
	}
	//更改权限
	pSectionHeader->Characteristics = characterise;
	//VirtualSize和SizeOfRawData设置为一样大
	pSectionHeader->Misc.VirtualSize = dataSize;
	pSectionHeader->SizeOfRawData = dataSize;
	//更改section数量为1
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
		printf("(AddNewSection)内存分配失败\n");
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
	//在节表后添加一个全零节表，表示结束
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
		printf("(ExpandLastSection)内存分配失败\n");
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
	//计算最后一节扩大后在内存的大小
	newSectionSize = pOptionHeader->SizeOfImage - pSectionHeader[index - 1].VirtualAddress + addSize;
	//将最后一节的VirtualSize和SizeOfRawData都设置为新的大小
	pSectionHeader[index - 1].Misc.VirtualSize = newSectionSize;
	pSectionHeader[index - 1].SizeOfRawData = newSectionSize;
	//修改SizeOfImage
	pOptionHeader->SizeOfImage += addSize;

	*pNewImageBuffer = pNewImageBufferTemp;
	pNewImageBufferTemp = NULL;
	return TRUE;
}