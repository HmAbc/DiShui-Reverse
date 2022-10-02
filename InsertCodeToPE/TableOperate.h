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

/// @brief �ƶ��ض�λ�����һ�����У���Ҫ��ǰ���ļ�������һ���ڣ�
/// @param pFileBuffer �����ڵ��ļ��ĵ�ַ
/// @return �ɹ�����TRUE
BOOL MoveRelocationTable(IN LPVOID pFileBuffer);

/// @brief �޸��ض�λ����Ҫ���ڵ�DLL��OEP�ı���ض�λ��ʧЧ���޸�
/// @param pFileBuffer �ļ����ڴ�ĵ�ַ
/// @param originOEP ԭ������ڵ�ַ
/// @param newOEP �µ���ڵ�ַ
/// @return �ɹ�����TRUE
BOOL RepairRelocationTable(IN LPVOID pFileBuffer, IN DWORD originImageBase);
