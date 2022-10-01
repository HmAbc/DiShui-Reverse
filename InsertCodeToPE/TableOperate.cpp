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
		printf("(PrintExportTable)FileBuffer ��ȡʧ��\n");
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
		printf("(PrintExportTable)�ļ�û�е�����\n");
		return 0;
	}

	printf("***********************����������**********************\n");
	printf("Characteristics:	%#x		(���ԣ�δ��)\n", pImageExport->Characteristics);
	printf("TimeDateStamp:		%#x	(ʱ���)\n", pImageExport->TimeDateStamp);
	printf("MajorVersion:		%#x		(δ��)\n", pImageExport->MajorVersion);
	printf("MinorVersion:		%#x		(δ��)\n", pImageExport->MinorVersion);
	printf("Name:			%#x		(�õ�������ļ����ַ���)\n", pImageExport->Name);
	printf("Base:			%d		(������������ʼ���)\n", pImageExport->Base);
	printf("NumberOfFunctions:	%d		(��������������)\n", pImageExport->NumberOfFunctions);
	printf("NumberOfNames:		%d		(�����ֵ����ĺ���)\n", pImageExport->NumberOfNames);
	printf("AddressOfFunctions:	%#x		(����������ַ��RVA)\n", pImageExport->AddressOfFunctions);
	printf("AddressOfNames:		%#x		(��������ַ��RVA)\n", pImageExport->AddressOfNames);
	printf("AddressOfNameOrdinals:	%#x		(������ŵ�ַ��)\n", pImageExport->AddressOfNameOrdinals);
	printf("***********************������ַ��(FOA)**********************\n");
	for (DWORD i = 0; i < pImageExport->NumberOfFunctions; i++)
	{
		tempFOA = RVAtoFOA(pFileBuffer, pImageExport->AddressOfFunctions);
		tempFOA = RVAtoFOA(pFileBuffer, ((PDWORD)(tempFOA + (DWORD)pFileBuffer))[i]);
		printf("������ַ��	%#x\n", tempFOA);
	}
	printf("***********************�������Ʊ�(FOA)**********************\n");
	for (DWORD i = 0; i < pImageExport->NumberOfNames; i++)
	{
		tempFOA = RVAtoFOA(pFileBuffer, pImageExport->AddressOfNames);
		tempFOA = RVAtoFOA(pFileBuffer, ((PDWORD)(tempFOA + (DWORD)pFileBuffer))[i]);
		printf("������ַ��	%#x\n", tempFOA);
		printf("�������ƣ�	%s\n", (PBYTE)(tempFOA + (DWORD)pFileBuffer));
	}
	printf("*************************������ű�*************************\n");
	for (DWORD i = 0; i < pImageExport->NumberOfNames; i++)
	{
		tempFOA = RVAtoFOA(pFileBuffer, pImageExport->AddressOfNameOrdinals);
		printf("������ţ�	%d\n", ((PSHORT)(tempFOA + (DWORD)pFileBuffer))[i]);
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
		printf("(PrintExportTable)FileBuffer ��ȡʧ��\n");
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
		printf("(PrintRelocationTable)û���ҵ��ض�λ��\n");
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
			printf("�� %d ����ַ��%#x	���ԣ�%d\n", i + 1, (tempAddr & 0xFFF) + pImageRelocation->VirtualAddress, (tempAddr >> 12) & 0xF);
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
	PIMAGE_DATA_DIRECTORY pImageData = NULL;
	PIMAGE_EXPORT_DIRECTORY pImageExport = NULL;
	PSHORT exportAddr = 0;
	DWORD exportSize = 0;
	DWORD numberOfItem = 0;
	DWORD index = 0;

	if (!pFileBuffer)
	{
		printf("(PrintExportTable)FileBuffer ��ȡʧ��\n");
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
		printf("(MoveExportTable)û���ҵ�������\n");
		return 0;
	}
	

}
