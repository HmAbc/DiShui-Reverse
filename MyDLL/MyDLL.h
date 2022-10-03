#pragma once
#include "pch.h"

void init();
extern "C" _declspec(dllexport)  int WINAPI Plus(int x, int y);
extern "C" _declspec(dllexport)  int WINAPI Sub(int x, int y);
extern "C" _declspec(dllexport)  int WINAPI Mul(int x, int y);
extern "C" _declspec(dllexport)  int WINAPI Div(int x, int y);
