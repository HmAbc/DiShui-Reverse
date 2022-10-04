#include "BaseClass.h"


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
	TestMyString();

	return 0;
}