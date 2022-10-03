#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string>

struct MyString
{
	char* str = NULL;

	MyString()
	{
		str = (char*)malloc(1024);
	}
	MyString(char* st)
	{
		int len = strlen(st) + 1;
		this->str = (char*)malloc(len);
		if (this->str)
		{
			strcpy(this->str, st);
		}
	}

	int Size()
	{
		if (this->str)
		{
			return strlen(this->str);
		}
		else
		{
			return 0;
		}
	}

	void SetString(char* newStr)
	{
		if (this->str)
		{
			int len = strlen(newStr) + 1;
			char* temp = (char*)realloc(this->str, len);
			if (temp)
			{
				this->str = temp;
				strcpy(this->str, newStr);
			}
		}
		else
		{
			printf("StrÎ´³õÊ¼»¯\n");
		}
	}

	void PrintString()
	{
		if (this->str)
		{
			printf("str: %s\n", this->str);
		}
	}

	void AppendString(char* newStr)
	{
		if (this->str)
		{
		
		}
	}

	~MyString()
	{
		free(str);
	}

};


int main()
{
	char* str = (char*)malloc(sizeof(char)*100);
	char a[] = "helloworld";
	if (str)
	{
		int len = strlen(str);
		memcpy(str, a, strlen(a));
		printf("%d\n", len);
		printf("%s\n", str);
		char* temp = (char*)realloc(str, 30);
		if (temp)
		{
			str = temp;
			len = strlen(str);
			printf("%d\n", len);
			printf("%s\n", str);
			free(str);
		}
		
	}

	return 0;
}