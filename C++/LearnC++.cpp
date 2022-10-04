#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <malloc.h>

struct MyString
{
	char* str = NULL;
	int size = 0;
	int len;

	MyString()
	{
		this->str = (char*)malloc(1024);
		if (this->str)
		{
			memset(this->str, 0, 1024);
			this->len = 1024;
		}
		else
		{
			printf("��ʼ��ʧ��\n");
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
		}
	}

	int Size()
	{
		return this->size;
	}

	void SetString(const char* newStr)
	{
		if (this->str)
		{
			int len = strlen(newStr);
			if (this->Size() > len)
			{
				strcpy(this->str, newStr);
			}
			else
			{
				char* temp = (char*)realloc(this->str, len + 1);
				if (temp)
				{
					this->str = temp;
					strcpy(this->str, newStr);
				}
			}
		}
		else
		{
			printf("strδ��ʼ��\n");
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
			if (this->Size() > strlen(newStr) + strlen(this->str))
			{
				strcat(this->str, newStr);
			}
			else
			{
				char* temp = (char*)realloc(this->str, strlen(this->str) + strlen(newStr) + 1);
				if (temp)
				{
					this->str = temp;
					strcat(this->str, newStr);
				}
			}
		}
		else
		{
			printf("strδ��ʼ��\n");
		}
	}

	~MyString()
	{
		free(str);
	}

};


int main()
{
	MyString myString;
	printf("%d\n", myString.Size());
	myString.SetString("helloworld");
	printf("%d\n", myString.Size());

	return 0;
}