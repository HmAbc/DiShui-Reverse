// TestDLL.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <Windows.h>
#include <stdio.h>

#define FILEPATH L"MyDLL_new.dll"
typedef int (*Plus)(int x, int y);
Plus MyPlus;

int main()
{
    HMODULE hModule = LoadLibrary(TEXT("d:/MyDLL_new.dll"));

    if (!hModule)
    {
        printf("DLL载入失败\n");
        return 0;
    }

    MyPlus = (Plus)GetProcAddress(hModule, "Plus");
    INT x = MyPlus(2, 3);
    printf("2 + 3 = %d\n", x);
    FreeLibrary(hModule);
    return 0;
}

