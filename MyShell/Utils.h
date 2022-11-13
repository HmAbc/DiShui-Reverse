#pragma once
#include <Windows.h>
#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")
#include <stdio.h>
#include <Tlhelp32.h>
#include <string>
#include <tchar.h>

void __cdecl OutputDebugStringF(const char* format, ...);

#ifdef _DEBUG  
#define DbgPrintf   OutputDebugStringF  
#else  
#define DbgPrintf  
#endif 

#ifdef _UNICODE
#define tstring wstring
#define to_tstring to_wstring
#else
#define tstring string
#define to_tstring to_string
#endif

//声明全局变量
extern HINSTANCE hAppInstance;
extern TCHAR fileName[256];
extern LPVOID fileBuffer;

/// @brief 设置进程访问权限，针对系统进程
/// @param lpName 需要的权限的名称
/// @param opt 选择是否设置权限
/// @return 成功返回TRUE
BOOL SetProcessPrivilege(IN LPCWSTR lpName, IN BOOL opt);

/// @brief 打开PE文件，读取PE信息，显示在IDD_DIALOG_PE PE信息查看 对话框
/// @param filePath PE文件路径
/// @param fileBuffer 指针，打开成功后指向PE文件在内存的地址
/// @return 成功返回0，失败返回一个错误码
DWORD ReadPEFile(IN LPCTSTR filePath, OUT LPVOID* fileBuffer);

/// @brief 将PE文件执行时的RVA转换为FOA
/// @param fileBuffer PE文件读取后在内存中的地址
/// @param rva 文件执行后的RVA
/// @return 返回rva对应的foa，超出文件大小的rva返回0
DWORD RVA2FOA(IN LPVOID fileBuffer, IN DWORD rva);

/// @brief 将ASCII码字符串转换成UNICODE
/// @param source ASCII码字符串地址
/// @param result 接收转换后字符串的缓冲区地址
VOID ANSIToUnicode(IN PCSTR source, OUT LPWSTR result);

/// @brief 将UNICODE字符串转换成ASCII码字符串
/// @param source UNICODE字符串地址
/// @param result 接收转换后字符串的缓冲区地址
VOID UnicodeToANSI(IN LPCWSTR source, OUT LPSTR result);

/// @brief 申请SizeOfImage大小的内存imageBuffer，从fileBuffer处将文件根据节表拷贝到imageBuffer处，
/// @param fileBuffer 文件缓冲区，未拉伸状态
/// @param imageBuffer 文件拉伸后的内存起始地址
/// @return 成功返回拉伸后大小SizeOfImage，失败返回0
DWORD CopyFileBufferToImageBuffer(IN LPVOID fileBuffer, OUT LPVOID* imageBuffer);

/// @brief 获取pe文件的 imageBase，sizeOfImage，oep
/// @param fileBuffer pe文件在内存的地址
/// @param imageBase 
/// @param sizeOfImage 
/// @param oep 
VOID GetProcessBase(IN LPVOID fileBuffer, OUT DWORD& imageBase, OUT DWORD& sizeOfImage, OUT DWORD& oep);

/// @brief 判断pe文件有没有重定位表
/// @param fileBuffer pe文件在内存的地址
/// @return 有重定位表返回TRUE
BOOL IsRelocation(IN LPVOID fileBuffer);

/// @brief 修复拉伸后pe文件的重定位表
/// @param imageBuffer 拉伸后pe文件在内存的地址
/// @param newImageBase 新的imageBase
VOID RepaireRelocation(IN LPVOID imageBuffer, IN DWORD newImageBase);
