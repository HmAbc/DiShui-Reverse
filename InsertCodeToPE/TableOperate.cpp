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
		printf("��������ַ��	%#x\n", tempFOA);
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
	PIMAGE_BASE_RELOCATION pBaseRelocation = NULL;
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
	pBaseRelocation = (PIMAGE_BASE_RELOCATION)(RVAtoFOA(pFileBuffer, pImageData[5].VirtualAddress) + (DWORD)pFileBuffer);

	if (!pBaseRelocation)
	{
		printf("(PrintRelocationTable)û���ҵ��ض�λ��\n");
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
			printf("�� %d ����ַ��%#x	���ԣ�%d\n", i + 1, (tempAddr & 0xFFF) + pBaseRelocation->VirtualAddress, (tempAddr >> 12) & 0xF);
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
	DWORD tempAddr = 0;			//��ʱ�����������м��ַ
	DWORD tempName = 0;			//��ʱ���������������ַ������ڴ��ʵ�ʵ�ַ
	DWORD dstNameAddr = 0;		//����pExportDirectory->AddressOfNames���Ŀ���ַ
	DWORD dstNameStr = 0;		//pExportDirectory->AddressOfNames���еĵ�ַָ����ַ������ƶ���Ŀ�ĵ�ַ
	DWORD lastSection = 0;		//���һ�����ڴ��ʵ�ʵ�ַ
	DWORD index = 0;

	if (!pFileBuffer)
	{
		printf("(MoveExportTable)FileBuffer ��ȡʧ��\n");
		return 0;
	}

	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	pDataDirectory = pOptionHeader->DataDirectory;
	//���������ڴ��ʵ�ʵ�ַ
	pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)(RVAtoFOA(pFileBuffer, pDataDirectory->VirtualAddress) + (DWORD)pFileBuffer);

	if (!pExportDirectory)
	{
		printf("(MoveExportTable)û���ҵ�������\n");
		return 0;
	}
	
	index = pPEHeader->NumberOfSections;
	//���һ�����ڴ��ʵ�ʵ�ַ��PointerToRawData��FOA������Ҫת��
	lastSection = (pSectionHeader + index - 1)->PointerToRawData + (DWORD)pFileBuffer;
	//����AddressOfFunctions�����һ����ʼ��ַ
	tempAddr = RVAtoFOA(pFileBuffer, pExportDirectory->AddressOfFunctions) + (DWORD)pFileBuffer;
	memcpy((LPVOID)lastSection, (LPVOID)tempAddr, pExportDirectory->NumberOfFunctions * 4);
	//����AddressOfNameOrdinals
	tempAddr = RVAtoFOA(pFileBuffer, pExportDirectory->AddressOfNameOrdinals) + (DWORD)pFileBuffer;
	memcpy((LPVOID)(lastSection + pExportDirectory->NumberOfFunctions * 4), (LPVOID)tempAddr, pExportDirectory->NumberOfNames * 2);

	//����AddressOfNames
	//ԭ��������AddressOfNames��FOA����һ�����飬ÿһ���ʾһ�����ֵĵ�ַ��ָ��һ���ַ���
	tempAddr = RVAtoFOA(pFileBuffer, pExportDirectory->AddressOfNames) + (DWORD)pFileBuffer;
	//�¶��б���AddressOfNames����ʼFOA�������Ǻ�����������*4��ÿһ����һ����ַ
	dstNameAddr = lastSection + pExportDirectory->NumberOfFunctions * 4 + pExportDirectory->NumberOfNames * 2;
	//�¶��������ַ�����ʼ��ַ��FOA
	dstNameStr = dstNameAddr + pExportDirectory->NumberOfNames * 4;
	for (DWORD i = 0; i < pExportDirectory->NumberOfNames; i++)
	{
		//����pExportDirectory->AddressOfFunctionsΪ�µĵ�ַ�����µĵ�ַת��ΪRVA
		((PDWORD)dstNameAddr)[i] = dstNameStr - (DWORD)pFileBuffer - (pSectionHeader + index - 1)->PointerToRawData + (pSectionHeader + index - 1)->VirtualAddress;
		//��������ÿһ�����ڴ��ʵ�ʵ�ַ
		tempName = RVAtoFOA(pFileBuffer, ((PDWORD)tempAddr)[i]) + (DWORD)pFileBuffer;
		strcpy((PCHAR)dstNameStr, (PCHAR)tempName);
		//�ƶ������ַ�����ַ����һ��λ��
		dstNameStr += strlen((PCHAR)tempName) + 1;
	}
	//����dll����
	tempAddr = RVAtoFOA(pFileBuffer, pExportDirectory->Name) + (DWORD)pFileBuffer;
	strcpy((PCHAR)dstNameStr, (PCHAR)tempAddr);
	//�Ȱ����ֵ�ַ����
	pExportDirectory->Name = dstNameStr - lastSection + (pSectionHeader + index - 1)->VirtualAddress;
	
	//�ƶ��������µĵ�ַ
	dstNameStr += strlen((PCHAR)tempAddr) + 1;
	memcpy((LPVOID)dstNameStr, (LPVOID)pExportDirectory, sizeof(IMAGE_EXPORT_DIRECTORY));

	pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)dstNameStr;
	//�޸���������Ҫʹ�õ���RVA
	tempAddr = (pSectionHeader + index - 1)->VirtualAddress;
	pExportDirectory->AddressOfFunctions = tempAddr;
	pExportDirectory->AddressOfNameOrdinals = tempAddr + pExportDirectory->NumberOfFunctions * 4;
	pExportDirectory->AddressOfNames = tempAddr + pExportDirectory->NumberOfFunctions * 4 + pExportDirectory->NumberOfNames * 2;
	//�޸��ļ�ͷ
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
	DWORD newRelocation = 0;		//���ض�λ����ʼ���������һ�����ڴ��ʵ�ʵ�ַ
	DWORD tempAddr = 0;
	DWORD sizeOfRelocation = 0;		//�ض�λ��Ĵ�С�����У�

	if (!pFileBuffer)
	{
		printf("(MoveRelocationTable)FileBuffer ��ȡʧ��\n");
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
		printf("(PrintRelocationTable)û���ҵ��ض�λ��\n");
		return 0;
	}

	while (pBaseRelocationTemp->VirtualAddress && pBaseRelocationTemp->SizeOfBlock)
	{
		sizeOfRelocation += pBaseRelocationTemp->SizeOfBlock;
		pBaseRelocationTemp = (PIMAGE_BASE_RELOCATION)((DWORD)pBaseRelocationTemp + pBaseRelocationTemp->SizeOfBlock);
	}

	newRelocation = (pSectionHeader + pPEHeader->NumberOfSections - 1)->PointerToRawData + (DWORD)pFileBuffer;
	memcpy((LPVOID)newRelocation, (LPVOID)pBaseRelocation, sizeOfRelocation);

	//�޸��ض�λ���ַ��RVA
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
		printf("(MoveRelocationTable)FileBuffer ��ȡʧ��\n");
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
			//�жϸ�4λ�Ƿ���3
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
		printf("(MoveRelocationTable)FileBuffer ��ȡʧ��\n");
		return 0;
	}

	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + 4);
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + pDosHeader->e_lfanew);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);

	pDataDirectory = pOptionHeader->DataDirectory;
	pImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(RVAtoFOA(pFileBuffer, pDataDirectory[1].VirtualAddress) + (DWORD)pFileBuffer);

	//ѭ���жϵ�����Ƿ�Ϸ����Ϸ��ٽ��д�ӡ
	while (pImportDescriptor->OriginalFirstThunk)
	{
		printf("*******************************************\n");
		//��ӡDLL���֣���ַתΪFOA
		printf("DLL����%s\n", (PCHAR)(RVAtoFOA(pFileBuffer, pImportDescriptor->Name) + (DWORD)pFileBuffer));
		//����OriginalFirstThunk��ת��ΪFOA
		originalFirstThunk = (PDWORD)(RVAtoFOA(pFileBuffer, pImportDescriptor->OriginalFirstThunk) + (DWORD)pFileBuffer);
		printf("OriginalFirstThunk:\n");
		while (*originalFirstThunk)
		{
			//�жϵ�һλ�Ƿ�Ϊ1������ǣ���ô��ȥ���λ��ֵ���Ǻ����ĵ������
			if (*originalFirstThunk & IMAGE_ORDINAL_FLAG)	//IMAGE_ORDINAL_FLAG = 0x80000000
			{
				printf("\t����ŵ��룺%d\n", *originalFirstThunk & 0x7FFFFFFF);
			}
			else
			{
				//������ǣ����ֵ����һ��RVA��ָ�� IMAGE_IMPORT_BY_NAME
				pImportByName = (PIMAGE_IMPORT_BY_NAME)(RVAtoFOA(pFileBuffer, *originalFirstThunk) + (DWORD)pFileBuffer);
				printf("\t�����ֵ��룺HINT %#x\t%s\n", pImportByName->Hint, pImportByName->Name);
			}
			originalFirstThunk++;
		}
		//�ж� pImportDescriptor->TimeDateStamp ��ֵ������Ϊ0ʱ��ʾδ�󶨵����originalFirstThunk���FirstThunk������һ��
		//����Ϊ-1ʱ����ʾ���˵����FirstThunk�м�¼���ǵ�ֵַ
		if (!pImportDescriptor->TimeDateStamp)
		{
			//����FirstThunk��ת��ΪFOA
			firstThunk = (PDWORD)(RVAtoFOA(pFileBuffer, pImportDescriptor->FirstThunk) + (DWORD)pFileBuffer);
			printf("FirstThunk:\n");
			while (*firstThunk)
			{
				//�жϵ�һλ�Ƿ�Ϊ1������ǣ���ô��ȥ���λ��ֵ���Ǻ����ĵ������
				if (*firstThunk & IMAGE_ORDINAL_FLAG)
				{
					printf("\t����ŵ��룺%d\n", *firstThunk & 0x7FFFFFFF);
				}
				else
				{
					//������ǣ����ֵ����һ��RVA��ָ�� IMAGE_IMPORT_BY_NAME
					pImportByName = (PIMAGE_IMPORT_BY_NAME)(RVAtoFOA(pFileBuffer, *firstThunk) + (DWORD)pFileBuffer);
					printf("\t�����ֵ��룺HINT %#x\t%s\n", pImportByName->Hint, pImportByName->Name);
				}
				firstThunk++;
			}
		}
		else
		{
			printf("���˵����ַ��FirstThunk�����ǵ�ַ��Ϣ\n");
		}
		
		pImportDescriptor++;
	}
	return TRUE;
}