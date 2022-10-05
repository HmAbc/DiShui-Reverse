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

//使用友元函数重载运算符
class FrendOperator
{
public:
    double x;
    FrendOperator() 
    {
        x = 0;
    }
    FrendOperator(double x)
    {
        this->x = x;
    }

    friend FrendOperator operator+(const FrendOperator& fo1, const FrendOperator& fo2);
    friend FrendOperator operator-(const FrendOperator& fo1, const FrendOperator& fo2);
    friend FrendOperator operator*(const FrendOperator& fo1, const FrendOperator& fo2);
    friend FrendOperator operator/(const FrendOperator& fo1, const FrendOperator& fo2);
    friend bool operator>(const FrendOperator& fo1, const FrendOperator& fo2);
    friend bool operator<(const FrendOperator& fo1, const FrendOperator& fo2);
    friend bool operator==(const FrendOperator& fo1, const FrendOperator& fo2);
};