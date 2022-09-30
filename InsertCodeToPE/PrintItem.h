#pragma once
#include "Global.h"

/// @brief 打印导出表
/// @param pFileBuffer 读取的文件在内存的地址
/// @return 0
DWORD PrintExportTable(IN LPVOID pFileBuffer);

/// @brief 打印重定位表
/// @param pFileBuffer 读取的文件在内存的地址
/// @return 0
DWORD PrintRelocationTable(IN LPVOID pFileBuffer);
