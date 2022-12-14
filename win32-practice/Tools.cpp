#define _CRT_SECURE_NO_WARNINGS
#include "Tools.h"

void __cdecl OutputDebugStringF(const char* format, ...)
{
    va_list vlArgs;
    char* strBuffer = (char*)GlobalAlloc(GPTR, 4096);
    
    va_start(vlArgs, format);
    _vsnprintf(strBuffer, 4096 - 1, format, vlArgs);
    va_end(vlArgs);
    strcat(strBuffer, "\n");
    OutputDebugStringA(strBuffer);
    GlobalFree(strBuffer);
    return;
}
