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
		printf("函数地址：	%#x\n", tempFOA);
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
	PIMAGE_BASE_RELOCATION pImageRelocation = NULL;
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
	pImageRelocation = (PIMAGE_BASE_RELOCATION)(RVAtoFOA(pFileBuffer, pImageData[5].VirtualAddress) + (DWORD)pFileBuffer);

	if (!pImageRelocation)
	{
		printf("(PrintRelocationTable)没有找到重定位表\n");
		return 0;
	}

	//relocationAddr = RVAtoFOA(pFileBuffer, pImageRelocation->VirtualAddress);
	while (pImageRelocation->VirtualAddress && pImageRelocation->SizeOfBlock)
	{
		relocationAddr = (PSHORT)((DWORD)pImageRelocation + 8);
		relocationSize = pImageRelocation->SizeOfBlock;
		
		printf("**********************************\n");
		printf("RVA: %#x\n", pImageRelocation->VirtualAddress);
		numberOfItem = (relocationSize - 8) / 2;
		printf("%d\n", numberOfItem);
		for (size_t i = 0; i < numberOfItem; i++)
		{
			tempAddr = relocationAddr[i];
			printf("第 %d 个地址：%#x	属性：%d\n", i + 1, (tempAddr & 0xFFF) + pImageRelocation->VirtualAddress, (tempAddr >> 12) & 0xF);
		}

		pImageRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)pImageRelocation + relocationSize);
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
		printf("(PrintExportTable)FileBuffer 获取失败\n");
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
	printf("%#x\n", dstNameAddr);
	//新段中名字字符串起始地址，FOA
	dstNameStr = dstNameAddr + pExportDirectory->NumberOfNames * 4;
	for (DWORD i = 0; i < pExportDirectory->NumberOfNames; i++)
	{
		//设置pExportDirectory->AddressOfFunctions为新的地址，将新的地址转换为RVA
		((PDWORD)dstNameAddr)[i] = dstNameStr - (DWORD)pFileBuffer - (pSectionHeader + index - 1)->PointerToRawData + (pSectionHeader + index - 1)->VirtualAddress;
		printf("%#x\n", ((PDWORD)dstNameAddr)[i]);
		//计算名字每一项在内存的实际地址
		tempName = RVAtoFOA(pFileBuffer, ((PDWORD)tempAddr)[i]) + (DWORD)pFileBuffer;
		strcpy((PCHAR)dstNameStr, (PCHAR)tempName);
		//移动名字字符串地址到下一个位置
		dstNameStr += strlen((PCHAR)tempName) + 1;
	}

	//修复导出表，需要使用的是RVA
	tempAddr = (pSectionHeader + index - 1)->VirtualAddress;
	pExportDirectory->AddressOfFunctions = tempAddr;
	pExportDirectory->AddressOfNameOrdinals = tempAddr + pExportDirectory->NumberOfFunctions * 4;
	pExportDirectory->AddressOfNames = tempAddr + pExportDirectory->NumberOfFunctions * 4 + pExportDirectory->NumberOfNames * 2;

	return TRUE;
}
