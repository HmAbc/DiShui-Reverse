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



