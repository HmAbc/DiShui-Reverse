#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <malloc.h>

struct MyString
{
	char* str = NULL;
	int size = 0;
	int stringLen = 0;

	MyString()
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
	MyString(const char* st)
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

	int Size()
	{
		return this->stringLen;
	}

	void SetString(const char* newStr)
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

	void PrintString()
	{
		if (this->str)
		{
			printf("str: %s\n", this->str);
		}
	}

	void AppendString(const char* newStr)
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

	~MyString()
	{
		free(str);
	}

};

void TestMyString()
{
	MyString myString;
	printf("%d\n", myString.Size());
	myString.SetString("helloworld");
	printf("%d\n", myString.Size());

	myString.AppendString(".com");
	myString.PrintString();

	printf("%d\n", myString.Size());
}


int main()
{
	

	return 0;
}