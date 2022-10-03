#define _CRT_SECURE_NO_WARNINGS
#include "Global.h"

DWORD PrintExportTable(IN LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNtHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER pOptionHeader = NULL;
	PIMAGE_DATA_DIRECTORY pImageData = NULL;
	PIMAGE_EXPORT_DIRECTORY pImageExport = NULL;
	DWORD tempFOA = 0;

	if (!pFileBuffer)
	{
		printf("(PrintExportTable)FileBuffer 获取失败\n");
		return 0;
	}

	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);

	pImageData = pOptionHeader->DataDirectory;
	pImageExport = (PIMAGE_EXPORT_DIRECTORY)(RVAtoFOA(pFileBuffer, pImageData->VirtualAddress) + (DWORD)pFileBuffer);
	if (!pImageExport)
	{
		printf("(PrintExportTable)文件没有导出表\n");
		return 0;
	}

	printf("***********************函数导出表**********************\n");
	printf("Characteristics:	%#x		(属性，未用)\n", pImageExport->Characteristics);
	printf("TimeDateStamp:		%#x	(时间戳)\n", pImageExport->TimeDateStamp);
	printf("MajorVersion:		%#x		(未用)\n", pImageExport->MajorVersion);
	printf("MinorVersion:		%#x		(未用)\n", pImageExport->MinorVersion);
	printf("Name:			%#x		(该导出表的文件名字符串)\n", pImageExport->Name);
	printf("Base:			%d		(导出函数的起始序号)\n", pImageExport->Base);
	printf("NumberOfFunctions:	%d		(导出函数的数量)\n", pImageExport->NumberOfFunctions);
	printf("NumberOfNames:		%d		(以名字导出的函数)\n", pImageExport->NumberOfNames);
	printf("AddressOfFunctions:	%#x		(导出函数地址表RVA)\n", pImageExport->AddressOfFunctions);
	printf("AddressOfNames:		%#x		(函数名地址表RVA)\n", pImageExport->AddressOfNames);
	printf("AddressOfNameOrdinals:	%#x		(函数序号地址表)\n", pImageExport->AddressOfNameOrdinals);
	printf("***********************函数地址表(FOA)**********************\n");
	for (DWORD i = 0; i < pImageExport->NumberOfFunctions; i++)
	{
		tempFOA = RVAtoFOA(pFileBuffer, pImageExport->AddressOfFunctions);
		tempFOA = RVAtoFOA(pFileBuffer, ((PDWORD)(tempFOA + (DWORD)pFileBuffer))[i]);
		printf("函数地址：	%#x\n", tempFOA);
	}
	printf("***********************函数名称表(FOA)**********************\n");
	for (DWORD i = 0; i < pImageExport->NumberOfNames; i++)
	{
		tempFOA = RVAtoFOA(pFileBuffer, pImageExport->AddressOfNames);
		tempFOA = RVAtoFOA(pFileBuffer, ((PDWORD)(tempFOA + (DWORD)pFileBuffer))[i]);
		printf("函数名地址：	%#x\n", tempFOA);
		printf("函数名称：	%s\n", (PBYTE)(tempFOA + (DWORD)pFileBuffer));
	}
	printf("*************************函数序号表*************************\n");
	for (DWORD i = 0; i < pImageExport->NumberOfNames; i++)
	{
		tempFOA = RVAtoFOA(pFileBuffer, pImageExport->AddressOfNameOrdinals);
		printf("函数序号：	%d\n", ((PSHORT)(tempFOA + (DWORD)pFileBuffer))[i]);
	}
	return 0;
}

DWORD PrintRelocationTable(IN LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNtHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER pOptionHeader = NULL;
	PIMAGE_DATA_DIRECTORY pImageData = NULL;
	PIMAGE_BASE_RELOCATION pBaseRelocation = NULL;
	PSHORT relocationAddr = 0;
	DWORD relocationSize = 0;
	DWORD numberOfItem = 0;
	SHORT tempAddr = 0;

	if (!pFileBuffer)
	{
		printf("(PrintExportTable)FileBuffer 获取失败\n");
		return 0;
	}

	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);

	pImageData = pOptionHeader->DataDirectory;
	pBaseRelocation = (PIMAGE_BASE_RELOCATION)(RVAtoFOA(pFileBuffer, pImageData[5].VirtualAddress) + (DWORD)pFileBuffer);

	if (!pBaseRelocation)
	{
		printf("(PrintRelocationTable)没有找到重定位表\n");
		return 0;
	}

	//relocationAddr = RVAtoFOA(pFileBuffer, pBaseRelocation->VirtualAddress);
	while (pBaseRelocation->VirtualAddress)
	{
		relocationAddr = (PSHORT)((DWORD)pBaseRelocation + 8);
		relocationSize = pBaseRelocation->SizeOfBlock;
		
		printf("**********************************\n");
		printf("RVA: %#x\n", pBaseRelocation->VirtualAddress);
		numberOfItem = (relocationSize - 8) / 2;
		printf("%d\n", numberOfItem);
		for (size_t i = 0; i < numberOfItem; i++)
		{
			tempAddr = relocationAddr[i];
			printf("第 %d 个地址：%#x	属性：%d\n", i + 1, (tempAddr & 0xFFF) + pBaseRelocation->VirtualAddress, (tempAddr >> 12) & 0xF);
		}

		pBaseRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)pBaseRelocation + relocationSize);
	}

	return 0;
}

BOOL MoveExportTable(IN LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNtHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	PIMAGE_DATA_DIRECTORY pDataDirectory = NULL;
	PIMAGE_EXPORT_DIRECTORY pExportDirectory = NULL;
	DWORD tempAddr = 0;			//临时变量，保存中间地址
	DWORD tempName = 0;			//临时变量，保存名字字符串在内存的实际地址
	DWORD dstNameAddr = 0;		//保存pExportDirectory->AddressOfNames表的目标地址
	DWORD dstNameStr = 0;		//pExportDirectory->AddressOfNames表中的地址指向的字符串被移动的目的地址
	DWORD lastSection = 0;		//最后一节在内存的实际地址
	DWORD index = 0;

	if (!pFileBuffer)
	{
		printf("(MoveExportTable)FileBuffer 获取失败\n");
		return 0;
	}

	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	pDataDirectory = pOptionHeader->DataDirectory;
	//导出表在内存的实际地址
	pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)(RVAtoFOA(pFileBuffer, pDataDirectory->VirtualAddress) + (DWORD)pFileBuffer);

	if (!pExportDirectory)
	{
		printf("(MoveExportTable)没有找到导出表\n");
		return 0;
	}
	
	index = pPEHeader->NumberOfSections;
	//最后一节在内存的实际地址，PointerToRawData是FOA，不需要转换
	lastSection = (pSectionHeader + index - 1)->PointerToRawData + (DWORD)pFileBuffer;
	//拷贝AddressOfFunctions到最后一节起始地址
	tempAddr = RVAtoFOA(pFileBuffer, pExportDirectory->AddressOfFunctions) + (DWORD)pFileBuffer;
	memcpy((LPVOID)lastSection, (LPVOID)tempAddr, pExportDirectory->NumberOfFunctions * 4);
	//拷贝AddressOfNameOrdinals
	tempAddr = RVAtoFOA(pFileBuffer, pExportDirectory->AddressOfNameOrdinals) + (DWORD)pFileBuffer;
	memcpy((LPVOID)(lastSection + pExportDirectory->NumberOfFunctions * 4), (LPVOID)tempAddr, pExportDirectory->NumberOfNames * 2);

	//拷贝AddressOfNames
	//原导出表中AddressOfNames的FOA，是一个数组，每一项表示一个名字的地址，指向一个字符串
	tempAddr = RVAtoFOA(pFileBuffer, pExportDirectory->AddressOfNames) + (DWORD)pFileBuffer;
	//新段中保存AddressOfNames的起始FOA，长度是函数名字项数*4，每一项是一个地址
	dstNameAddr = lastSection + pExportDirectory->NumberOfFunctions * 4 + pExportDirectory->NumberOfNames * 2;
	//新段中名字字符串起始地址，FOA
	dstNameStr = dstNameAddr + pExportDirectory->NumberOfNames * 4;
	for (DWORD i = 0; i < pExportDirectory->NumberOfNames; i++)
	{
		//设置pExportDirectory->AddressOfFunctions为新的地址，将新的地址转换为RVA
		((PDWORD)dstNameAddr)[i] = dstNameStr - (DWORD)pFileBuffer - (pSectionHeader + index - 1)->PointerToRawData + (pSectionHeader + index - 1)->VirtualAddress;
		//计算名字每一项在内存的实际地址
		tempName = RVAtoFOA(pFileBuffer, ((PDWORD)tempAddr)[i]) + (DWORD)pFileBuffer;
		strcpy((PCHAR)dstNameStr, (PCHAR)tempName);
		//移动名字字符串地址到下一个位置
		dstNameStr += strlen((PCHAR)tempName) + 1;
	}
	//拷贝dll名字
	tempAddr = RVAtoFOA(pFileBuffer, pExportDirectory->Name) + (DWORD)pFileBuffer;
	strcpy((PCHAR)dstNameStr, (PCHAR)tempAddr);
	//先把名字地址更新
	pExportDirectory->Name = dstNameStr - lastSection + (pSectionHeader + index - 1)->VirtualAddress;
	
	//移动导出表到新的地址
	dstNameStr += strlen((PCHAR)tempAddr) + 1;
	memcpy((LPVOID)dstNameStr, (LPVOID)pExportDirectory, sizeof(IMAGE_EXPORT_DIRECTORY));

	pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)dstNameStr;
	//修复导出表，需要使用的是RVA
	tempAddr = (pSectionHeader + index - 1)->VirtualAddress;
	pExportDirectory->AddressOfFunctions = tempAddr;
	pExportDirectory->AddressOfNameOrdinals = tempAddr + pExportDirectory->NumberOfFunctions * 4;
	pExportDirectory->AddressOfNames = tempAddr + pExportDirectory->NumberOfFunctions * 4 + pExportDirectory->NumberOfNames * 2;
	//修复文件头
	pDataDirectory->VirtualAddress = dstNameStr - lastSection + tempAddr;

	return TRUE;
}

BOOL MoveRelocationTable(IN LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNtHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	PIMAGE_DATA_DIRECTORY pDataDirectory = NULL;
	PIMAGE_BASE_RELOCATION pBaseRelocation = NULL;
	PIMAGE_BASE_RELOCATION pBaseRelocationTemp = NULL;
	DWORD index = 0;
	DWORD newRelocation = 0;		//新重定位表起始，就是最后一节在内存的实际地址
	DWORD tempAddr = 0;
	DWORD sizeOfRelocation = 0;		//重定位表的大小（所有）

	if (!pFileBuffer)
	{
		printf("(MoveRelocationTable)FileBuffer 获取失败\n");
		return 0;
	}

	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + 4);
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + pDosHeader->e_lfanew);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	pDataDirectory = pOptionHeader->DataDirectory;
	pBaseRelocation = (PIMAGE_BASE_RELOCATION)(RVAtoFOA(pFileBuffer, pDataDirectory[5].VirtualAddress) + (DWORD)pFileBuffer);
	pBaseRelocationTemp = pBaseRelocation;

	if (!pBaseRelocation)
	{
		printf("(PrintRelocationTable)没有找到重定位表\n");
		return 0;
	}

	while (pBaseRelocationTemp->VirtualAddress && pBaseRelocationTemp->SizeOfBlock)
	{
		sizeOfRelocation += pBaseRelocationTemp->SizeOfBlock;
		pBaseRelocationTemp = (PIMAGE_BASE_RELOCATION)((DWORD)pBaseRelocationTemp + pBaseRelocationTemp->SizeOfBlock);
	}

	newRelocation = (pSectionHeader + pPEHeader->NumberOfSections - 1)->PointerToRawData + (DWORD)pFileBuffer;
	memcpy((LPVOID)newRelocation, (LPVOID)pBaseRelocation, sizeOfRelocation);

	//修复重定位表地址，RVA
	pDataDirectory[5].VirtualAddress = (pSectionHeader + pPEHeader->NumberOfSections - 1)->VirtualAddress;

	return TRUE;
}

BOOL RepairRelocationTable(IN LPVOID pFileBuffer, IN LONG originImageBase)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNtHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER pOptionHeader = NULL;
	PIMAGE_DATA_DIRECTORY pDataDirectory = NULL;
	PIMAGE_BASE_RELOCATION pBaseRelocation = NULL;
	PSHORT pBaseRelocationTemp = NULL;
	LONG newImageBase = 0;
	LONG diff = 0;
	DWORD tempAddr = 0;
	DWORD numOfAddr = 0;
	DWORD tempNumber = 0;

	if (!pFileBuffer)
	{
		printf("(MoveRelocationTable)FileBuffer 获取失败\n");
		return 0;
	}

	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + 4);
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + pDosHeader->e_lfanew);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);

	newImageBase = pOptionHeader->ImageBase;
	pDataDirectory = pOptionHeader->DataDirectory;
	pBaseRelocation = (PIMAGE_BASE_RELOCATION)(RVAtoFOA(pFileBuffer, pDataDirectory[5].VirtualAddress) + (DWORD)pFileBuffer);
	diff = newImageBase - originImageBase;

	while (pBaseRelocation->VirtualAddress)
	{
		tempAddr = RVAtoFOA(pFileBuffer, pBaseRelocation->VirtualAddress) + (DWORD)pFileBuffer;
		pBaseRelocationTemp = (PSHORT)((DWORD)pBaseRelocation + 8);
		numOfAddr = (pBaseRelocation->SizeOfBlock - 8) / 2;
		for (DWORD i = 0; i < numOfAddr; i++)
		{
			//判断高4位是否是3
			if ((pBaseRelocationTemp[i] & 0x3000) == 0x3000)
			{
				tempNumber = pBaseRelocationTemp[i] & 0xFFF;
				*(PLONG)(tempNumber + tempAddr) += diff;
			}
		}
		pBaseRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)pBaseRelocation + pBaseRelocation->SizeOfBlock);
	}

	return TRUE;
}

BOOL PrintImportTable(IN LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNtHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER pOptionHeader = NULL;
	PIMAGE_DATA_DIRECTORY pDataDirectory = NULL;
	PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = NULL;
	PIMAGE_IMPORT_BY_NAME pImportByName = NULL;
	PDWORD originalFirstThunk = 0;
	PDWORD firstThunk = 0;

	if (!pFileBuffer)
	{
		printf("(MoveRelocationTable)FileBuffer 获取失败\n");
		return 0;
	}

	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + 4);
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + pDosHeader->e_lfanew);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);

	pDataDirectory = pOptionHeader->DataDirectory;
	pImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(RVAtoFOA(pFileBuffer, pDataDirectory[1].VirtualAddress) + (DWORD)pFileBuffer);

	//循环判断导入表是否合法，合法再进行打印
	while (pImportDescriptor->OriginalFirstThunk)
	{
		printf("*******************************************\n");
		//打印DLL名字，地址转为FOA
		printf("DLL名：%s\n", (PCHAR)(RVAtoFOA(pFileBuffer, pImportDescriptor->Name) + (DWORD)pFileBuffer));
		//遍历OriginalFirstThunk，转换为FOA
		originalFirstThunk = (PDWORD)(RVAtoFOA(pFileBuffer, pImportDescriptor->OriginalFirstThunk) + (DWORD)pFileBuffer);
		printf("OriginalFirstThunk:\n");
		while (*originalFirstThunk)
		{
			//判断第一位是否为1，如果是，那么除去最高位的值就是函数的导出序号
			if (*originalFirstThunk & IMAGE_ORDINAL_FLAG)	//IMAGE_ORDINAL_FLAG = 0x80000000
			{
				printf("\t按序号导入：%d\n", *originalFirstThunk & 0x7FFFFFFF);
			}
			else
			{
				//如果不是，这个值就是一个RVA，指向 IMAGE_IMPORT_BY_NAME
				pImportByName = (PIMAGE_IMPORT_BY_NAME)(RVAtoFOA(pFileBuffer, *originalFirstThunk) + (DWORD)pFileBuffer);
				printf("\t按名字导入：HINT %#x\t%s\n", pImportByName->Hint, pImportByName->Name);
			}
			originalFirstThunk++;
		}
		//判断 pImportDescriptor->TimeDateStamp 的值，当它为0时表示未绑定导入表，originalFirstThunk表和FirstThunk表内容一样
		//当它为-1时，表示绑定了导入表，FirstThunk中记录的是地址值
		if (!pImportDescriptor->TimeDateStamp)
		{
			//遍历FirstThunk，转换为FOA
			firstThunk = (PDWORD)(RVAtoFOA(pFileBuffer, pImportDescriptor->FirstThunk) + (DWORD)pFileBuffer);
			printf("FirstThunk:\n");
			while (*firstThunk)
			{
				//判断第一位是否为1，如果是，那么除去最高位的值就是函数的导出序号
				if (*firstThunk & IMAGE_ORDINAL_FLAG)
				{
					printf("\t按序号导入：%d\n", *firstThunk & 0x7FFFFFFF);
				}
				else
				{
					//如果不是，这个值就是一个RVA，指向 IMAGE_IMPORT_BY_NAME
					pImportByName = (PIMAGE_IMPORT_BY_NAME)(RVAtoFOA(pFileBuffer, *firstThunk) + (DWORD)pFileBuffer);
					printf("\t按名字导入：HINT %#x\t%s\n", pImportByName->Hint, pImportByName->Name);
				}
				firstThunk++;
			}
		}
		else
		{
			printf("绑定了导入地址表，FirstThunk表中是地址信息\n");
		}
		
		pImportDescriptor++;
	}
	return TRUE;
}