#pragma once
#include <Windows.h>

/// @brief 打印导出表
/// @param pFileBuffer 读取的文件在内存的地址
/// @return 0
DWORD PrintExportTable(IN LPVOID pFileBuffer);

/// @brief 打印重定位表
/// @param pFileBuffer 读取的文件在内存的地址
/// @return 0
DWORD PrintRelocationTable(IN LPVOID pFileBuffer);

/// @brief 将导出表移动到文件的最后一个节中（需要提前在文件最后添加一个节）
/// @param pFileBuffer 添加完节的文件的地址
/// @return 成功返回TRUE
BOOL MoveExportTable(IN LPVOID pFileBuffer);

/// @brief 移动重定位表到最后一个节中（需要提前在文件最后添加一个节）
/// @param pFileBuffer 添加完节的文件的地址
/// @return 成功返回TRUE
BOOL MoveRelocationTable(IN LPVOID pFileBuffer);

/// @brief 修复重定位表，主要用于当DLL的OEP改变后，重定位表失效的修复
/// @param pFileBuffer 文件在内存的地址
/// @param originOEP 原来的入口地址
/// @param newOEP 新的入口地址
/// @return 成功返回TRUE
BOOL RepairRelocationTable(IN LPVOID pFileBuffer, IN LONG originImageBase);

/// @brief 打印导入表IAT
/// @param pFileBuffer 文件在内存的地址
/// @return 成功返回TRUE
BOOL PrintImportTable(IN LPVOID pFileBuffer);

/// @brief 打印PE文件资源表
/// @param pFileBuffer 文件在内存的地址
/// @return 成功返回TRUE
BOOL PrintResourceTable(IN LPVOID pFileBuffer);
