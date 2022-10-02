#pragma once
#include <Windows.h>
#include <stdio.h>

#define TEXT 0
#define EXPORT 1

/// @brief 将硬盘中的文件读取到内存中，缓冲区地址指针为pFileBuffer
/// @param lpszFile 文件路径
/// @param pFileBuffer 文件缓冲区，读取成功后是文件在内存的起始地址
/// @return 读取成功返回文件大小，失败返回0
DWORD ReadPEFile(IN LPCSTR lpszFile, OUT LPVOID* pFileBuffer);

/// @brief 申请SizeOfImage大小的内存pImageBuffer，从pFileBuffer处将文件根据节表拷贝到pImageBuffer处，
/// @param pFileBuffer 文件缓冲区，未拉伸状态
/// @param pImageBuffer 文件拉伸后的内存起始地址
/// @return 成功返回拉伸后大小SizeOfImage，失败返回0
DWORD CopyFileBufferToImageBuffer(IN LPVOID pFileBuffer, OUT LPVOID* pImageBuffer);

/// @brief 将内存中经过拉伸的文件，拷贝到新的地址并进行压缩
/// @param pImageBuffer 拉伸过的文件地址
/// @param pNewBuffer 新的文件在内存的地址
/// @return 成功返回压缩后文件的大小，失败返回0
DWORD CopyImageBufferToNewBuffer(IN LPVOID pImageBuffer, OUT LPVOID* pNewBuffer);

/// @brief 从内存将文件写回磁盘
/// @param pMemBuffer 需要保存的文件在内存中的地址
/// @param size 需要保存的文件的大小
/// @param lpszFile 保存文件路径及文件名
/// @return 成功返回TRUE，失败返回FALSE
BOOL MemoryToFile(IN LPVOID pMemBuffer, IN size_t size, OUT LPCSTR lpszFile);

/// @brief 计算给定文件中RVA在文件中的FOA
/// @param pFileBuffer 文件缓冲区地址
/// @param dwRVA RVA值
/// @return 返回转换后FOA值，失败返回0
DWORD RVAtoFOA(IN LPVOID pFileBuffer, IN DWORD dwRVA);

/// @brief 将PE头文件中的NTHeader处向前移动到DOSHeader->e_lfanew后
/// @param pFileBuffer 文件缓冲区地址
/// @return 成功返回TRUE，失败返回FALSE
BOOL MoveHeader(IN LPVOID pFileBuffer);

/// @brief 合并所有section为一个
/// @param pImageBuffer 经过拉伸的文件地址
/// @return 成功返回TRUE，pImageBuffer保存合并section后的文件在内存的地址，失败返回FALSE
BOOL MergeSections(OUT LPVOID *pImageBuffer);

/// @brief 在PE文件最后添加一个节，可以加在拉伸或未拉伸的文件后
/// @param pImageBuffer 文件地址
/// @param pNewImageBuffer 添加完section的文件地址
/// @param fileSize 需要添加节的文件的大小
/// @param addSize 要增加的节大小，有内存对齐要求
/// @param name 要增加节的名字，字节数组
/// @param characteristic 指定添加节的属性，查看头文件获取宏定义
/// @return 成功返回TRUE，pNewImageBuffer保存添加section后的文件地址，失败返回FALSE
BOOL AddNewSection(IN LPVOID pImageBuffer, OUT LPVOID* pNewImageBuffer, IN DWORD fileSize, IN DWORD addSize, IN PCHAR name);

/// @brief 扩大PE文件最后一个节
/// @param pImageBuffer 经过拉伸的文件地址
/// @param pNewImageBuffer 扩大完成后的文件地址
/// @param fileSize 经过拉伸的文件的大小
/// @param addSize 要增加的大小，有内存对其的要求
/// @return 成功返回TRUE，pNewImageBuffer保存扩大后的文件地址，失败返回FALSE
BOOL ExpandLastSection(IN LPVOID pImageBuffer, OUT LPVOID* pNewImageBuffer, IN DWORD fileSize, IN DWORD addSize);

