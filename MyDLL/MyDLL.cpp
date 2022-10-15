#include "MyDLL.h"
#include "pch.h"

void init()
{
	MessageBox(NULL, TEXT("init"), TEXT("init"), MB_ICONINFORMATION);
}

int WINAPI Plus(int x, int y)
{
	return x + y;
}
int WINAPI Sub(int x, int y)
{
	return x - y;
}
int WINAPI Mul(int x, int y)
{
	return x * y;
}
int WINAPI Div(int x, int y)
{
	return x / y;
}

