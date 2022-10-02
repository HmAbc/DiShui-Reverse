#pragma once
#include <Windows.h>
#include <stdio.h>

#define TEXT 0
#define EXPORT 1

/// @brief ��Ӳ���е��ļ���ȡ���ڴ��У���������ַָ��ΪpFileBuffer
/// @param lpszFile �ļ�·��
/// @param pFileBuffer �ļ�����������ȡ�ɹ������ļ����ڴ����ʼ��ַ
/// @return ��ȡ�ɹ������ļ���С��ʧ�ܷ���0
DWORD ReadPEFile(IN LPCSTR lpszFile, OUT LPVOID* pFileBuffer);

/// @brief ����SizeOfImage��С���ڴ�pImageBuffer����pFileBuffer�����ļ����ݽڱ�����pImageBuffer����
/// @param pFileBuffer �ļ���������δ����״̬
/// @param pImageBuffer �ļ��������ڴ���ʼ��ַ
/// @return �ɹ�����������СSizeOfImage��ʧ�ܷ���0
DWORD CopyFileBufferToImageBuffer(IN LPVOID pFileBuffer, OUT LPVOID* pImageBuffer);

/// @brief ���ڴ��о���������ļ����������µĵ�ַ������ѹ��
/// @param pImageBuffer ��������ļ���ַ
/// @param pNewBuffer �µ��ļ����ڴ�ĵ�ַ
/// @return �ɹ�����ѹ�����ļ��Ĵ�С��ʧ�ܷ���0
DWORD CopyImageBufferToNewBuffer(IN LPVOID pImageBuffer, OUT LPVOID* pNewBuffer);

/// @brief ���ڴ潫�ļ�д�ش���
/// @param pMemBuffer ��Ҫ������ļ����ڴ��еĵ�ַ
/// @param size ��Ҫ������ļ��Ĵ�С
/// @param lpszFile �����ļ�·�����ļ���
/// @return �ɹ�����TRUE��ʧ�ܷ���FALSE
BOOL MemoryToFile(IN LPVOID pMemBuffer, IN size_t size, OUT LPCSTR lpszFile);

/// @brief ��������ļ���RVA���ļ��е�FOA
/// @param pFileBuffer �ļ���������ַ
/// @param dwRVA RVAֵ
/// @return ����ת����FOAֵ��ʧ�ܷ���0
DWORD RVAtoFOA(IN LPVOID pFileBuffer, IN DWORD dwRVA);

/// @brief ��PEͷ�ļ��е�NTHeader����ǰ�ƶ���DOSHeader->e_lfanew��
/// @param pFileBuffer �ļ���������ַ
/// @return �ɹ�����TRUE��ʧ�ܷ���FALSE
BOOL MoveHeader(IN LPVOID pFileBuffer);

/// @brief �ϲ�����sectionΪһ��
/// @param pImageBuffer ����������ļ���ַ
/// @return �ɹ�����TRUE��pImageBuffer����ϲ�section����ļ����ڴ�ĵ�ַ��ʧ�ܷ���FALSE
BOOL MergeSections(OUT LPVOID *pImageBuffer);

/// @brief ��PE�ļ�������һ���ڣ����Լ��������δ������ļ���
/// @param pImageBuffer �ļ���ַ
/// @param pNewImageBuffer �����section���ļ���ַ
/// @param fileSize ��Ҫ��ӽڵ��ļ��Ĵ�С
/// @param addSize Ҫ���ӵĽڴ�С�����ڴ����Ҫ��
/// @param name Ҫ���ӽڵ����֣��ֽ�����
/// @param characteristic ָ����ӽڵ����ԣ��鿴ͷ�ļ���ȡ�궨��
/// @return �ɹ�����TRUE��pNewImageBuffer�������section����ļ���ַ��ʧ�ܷ���FALSE
BOOL AddNewSection(IN LPVOID pImageBuffer, OUT LPVOID* pNewImageBuffer, IN DWORD fileSize, IN DWORD addSize, IN PCHAR name);

/// @brief ����PE�ļ����һ����
/// @param pImageBuffer ����������ļ���ַ
/// @param pNewImageBuffer ������ɺ���ļ���ַ
/// @param fileSize ����������ļ��Ĵ�С
/// @param addSize Ҫ���ӵĴ�С�����ڴ�����Ҫ��
/// @return �ɹ�����TRUE��pNewImageBuffer�����������ļ���ַ��ʧ�ܷ���FALSE
BOOL ExpandLastSection(IN LPVOID pImageBuffer, OUT LPVOID* pNewImageBuffer, IN DWORD fileSize, IN DWORD addSize);

