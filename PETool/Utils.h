#pragma once
#include <Windows.h>
#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")
#include <stdio.h>
#include "resource.h"
#include <Tlhelp32.h>
#include <string>
#include <tchar.h>

void __cdecl OutputDebugStringF(const char* format, ...);

#ifdef _DEBUG  
#define DbgPrintf   OutputDebugStringF  
#else  
#define DbgPrintf  
#endif 

//����ȫ�ֱ���
extern HINSTANCE hAppInstance;
extern TCHAR fileName[256];
extern LPVOID fileBuffer;

/// @brief ���ý��̷���Ȩ�ޣ����ϵͳ����
/// @param lpName ��Ҫ��Ȩ�޵�����
/// @param opt ѡ���Ƿ�����Ȩ��
/// @return �ɹ�����TRUE
BOOL SetProcessPrivilege(IN LPCWSTR lpName, IN BOOL opt);

/// @brief ��PE�ļ�����ȡPE��Ϣ����ʾ��IDD_DIALOG_PE PE��Ϣ�鿴 �Ի���
/// @param filePath PE�ļ�·��
/// @param fileBuffer ָ�룬�򿪳ɹ���ָ��PE�ļ����ڴ�ĵ�ַ
/// @return �ɹ�����0��ʧ�ܷ���һ��������
DWORD ReadPEFile(IN LPCTSTR filePath, OUT LPVOID* fileBuffer);

/// @brief ��PE�ļ�ִ��ʱ��RVAת��ΪFOA
/// @param fileBuffer PE�ļ���ȡ�����ڴ��еĵ�ַ
/// @param rva �ļ�ִ�к��RVA
/// @return ����rva��Ӧ��foa�������ļ���С��rva����0
DWORD RVA2FOA(IN LPVOID fileBuffer, IN DWORD rva);

/// @brief ��ASCII���ַ���ת����UNICODE
/// @param source ASCII���ַ�����ַ
/// @param result ����ת�����ַ����Ļ�������ַ
VOID ANSIToUnicode(IN PCSTR source, OUT LPWSTR result);

/// @brief ��UNICODE�ַ���ת����ASCII���ַ���
/// @param source UNICODE�ַ�����ַ
/// @param result ����ת�����ַ����Ļ�������ַ
VOID UnicodeToANSI(IN LPCWSTR source, OUT LPSTR result);
