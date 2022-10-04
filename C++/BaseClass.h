#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <malloc.h>

class MyString
{
private:
	char* str = NULL;
	int size = 0;
	int stringLen = 0;

public:
	MyString();
	MyString(const char* st);
	int Size();
	void SetString(const char* newStr);
	void PrintString();
	void AppendString(const char* newStr);
	~MyString();
};