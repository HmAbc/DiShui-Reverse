#pragma once
#include <Windows.h>

/// @brief ��ӡ������
/// @param pFileBuffer ��ȡ���ļ����ڴ�ĵ�ַ
/// @return 0
DWORD PrintExportTable(IN LPVOID pFileBuffer);

/// @brief ��ӡ�ض�λ��
/// @param pFileBuffer ��ȡ���ļ����ڴ�ĵ�ַ
/// @return 0
DWORD PrintRelocationTable(IN LPVOID pFileBuffer);

/// @brief ���������ƶ����ļ������һ�����У���Ҫ��ǰ���ļ�������һ���ڣ�
/// @param pFileBuffer �����ڵ��ļ��ĵ�ַ
/// @return �ɹ�����TRUE
BOOL MoveExportTable(IN LPVOID pFileBuffer);



