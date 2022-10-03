#include <iostream>
using namespace std;

struct Base
{
	int x;
	int y;

	int Plus()
	{
		return this->x + this->y;
	}
	int Sub()
	{
		return this->x + this->y;
	}
	int Mul()
	{
		return this->x + this->y;
	}
	int Div()
	{
		return this->x + this->y;
	}

};

struct Person
{
	int x;
	void Fn_1()
	{
		printf("Person:Fn_1()\n");
	}

	void Fn_2()
	{
		printf("Person:Fn_2()\n");
	}
};



int main()
{
	Base base;
	base.x = 2;
	base.y = 1;
	base.Plus();

	return 0;
}