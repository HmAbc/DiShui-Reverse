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

/// @brief 将PE头文件中的NTHeader处向前移动到DOSHeader->e_lfanew后
/// @param fileBuffer 文件缓冲区地址
/// @return 成功返回TRUE，失败返回FALSE
BOOL MoveHeader(IN LPVOID fileBuffer);

/// @brief 在PE文件最后添加一个节，可以加在拉伸或未拉伸的文件后，默认添加属性为0xE0000060
/// @param pImageBuffer 文件地址
/// @param pNewImageBuffer 添加完section的文件地址
/// @param fileSize 需要添加节的文件的大小
/// @param addSize 要增加的数据大小
/// @param name 要增加节的名字，字节数组，ANSI编码
/// @return 成功返回总的文件大小，pNewImageBuffer保存添加section后的文件地址，失败返回0
DWORD AddNewSection(IN LPVOID pImageBuffer, OUT LPVOID* pNewImageBuffer, IN DWORD fileSize, IN DWORD addSize, IN PCHAR name);

/// @brief 从内存将文件写回磁盘
/// @param pMemBuffer 需要保存的文件在内存中的地址
/// @param size 需要保存的文件的大小
/// @param lpszFile 保存文件路径及文件名
/// @return 成功返回TRUE，失败返回FALSE
BOOL MemoryToFile(IN LPVOID pMemBuffer, IN size_t size, OUT LPCTSTR lpszFile);