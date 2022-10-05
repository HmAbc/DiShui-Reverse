#include "BaseClass.h"

MyString::MyString()
{
	this->str = (char*)malloc(1024);
	if (this->str)
	{
		memset(this->str, 0, 1024);
		this->size = 1024;
	}
	else
	{
		printf("初始化失败\n");
	}
}
MyString::MyString(const char* st)
{
	int len = strlen(st) + 1;
	this->str = (char*)malloc(len);
	if (this->str)
	{
		strcpy(this->str, st);
		this->size = len;
		this->stringLen = len;
	}
}

int MyString::Size()
{
	return this->stringLen;
}

void MyString::SetString(const char* newStr)
{
	if (this->str)
	{
		int len = strlen(newStr) + 1;
		if (this->size >= len)
		{
			strcpy(this->str, newStr);
			this->stringLen = len;
		}
		else
		{
			char* temp = (char*)realloc(this->str, len);
			if (temp)
			{
				this->str = temp;
				strcpy(this->str, newStr);
				this->size = len;
				this->stringLen = len;
			}
		}
	}
	else
	{
		printf("str未初始化\n");
	}
}

void MyString::PrintString()
{
	if (this->str)
	{
		printf("str: %s\n", this->str);
	}
}

void MyString::AppendString(const char* newStr)
{
	if (this->str)
	{
		if (this->size >= strlen(newStr) + this->stringLen)
		{
			strcat(this->str, newStr);
			this->stringLen = strlen(newStr) + this->stringLen;
		}
		else
		{
			char* temp = (char*)realloc(this->str, this->stringLen + strlen(newStr));
			if (temp)
			{
				this->str = temp;
				strcat(this->str, newStr);
				this->size = this->stringLen + strlen(newStr);
				this->stringLen = this->stringLen + strlen(newStr);
			}
		}
	}
	else
	{
		printf("str未初始化\n");
	}
}

MyString::~MyString()
{
	free(str);
}

//使用友元函数重载运算符
FrendOperator operator+(const FrendOperator& fo1, const FrendOperator& fo2)
{
	FrendOperator fo;
	fo.x = fo1.x + fo2.x;
	return fo;

}

FrendOperator operator-(const FrendOperator& fo1, const FrendOperator& fo2)
{
	FrendOperator fo;
	fo.x = fo1.x - fo2.x;
	return fo;
}
FrendOperator operator*(const FrendOperator& fo1, const FrendOperator& fo2)
{
	FrendOperator fo;
	fo.x = fo1.x * fo2.x;
	return fo;
}

FrendOperator operator/(const FrendOperator& fo1, const FrendOperator& fo2)
{
	FrendOperator fo;
	if (fo2.x)
	{
		fo.x = fo1.x / fo2.x;
		return fo;
	}
	else
	{
		return NULL;
	}
	
}

bool operator>(const FrendOperator& fo1, const FrendOperator& fo2)
{
	return fo1.x > fo2.x;
}

bool operator<(const FrendOperator& fo1, const FrendOperator& fo2)
{
	return fo1.x < fo2.x;
}

bool operator==(const FrendOperator& fo1, const FrendOperator& fo2)
{
	return fo1.x == fo2.x;
}
