#pragma once
#include "Global.h"

/// @brief ��ӡ������
/// @param pFileBuffer ��ȡ���ļ����ڴ�ĵ�ַ
/// @return 0
DWORD PrintExportTable(IN LPVOID pFileBuffer);

/// @brief ��ӡ�ض�λ��
/// @param pFileBuffer ��ȡ���ļ����ڴ�ĵ�ַ
/// @return 0
DWORD PrintRelocationTable(IN LPVOID pFileBuffer);
