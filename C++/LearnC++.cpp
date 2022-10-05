#include "BaseClass.h"
#include <iostream>
#include "VectorImplement.h"

template<class T>
struct Base
{
public:
    T x;
    T y;
    Base(){}

    Base(T x, T y)
    {
        this->x = x;
        this->y = y;
    }

    bool operator> (const Base& A);
};

template<class T>
bool Base<T>::operator> (const Base<T>& A)
{
    return this->x > A.x;
}

template<class T>
void swap(T &x, T &y)
{
    T temp;
    temp = x;
    x = y;
    y = temp;

    std::cout << x << " " << y << std::endl;
}

//Ã°ÅÝÅÅÐò
template<class T>
void sort(T *arr, int length)
{
    T temp;
    for (int i = 0; i < length - 1; i++)
    {
        for (int j = 0; j < length - 1 - i; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                temp = arr[j + 1];
                arr[j + 1] = arr[j];
                arr[j] = temp;
            }
        }
    }
}

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

void TestTemplateOperatorReload()
{
    Base<int> b1(4, 2);
    Base<int> b2(2, 5);
    Base<int> b3(7, 3);
    Base<int> b4(5, 4);
    Base<int> b5(1, 8);

    Base<int> b[] = { b1, b2, b3, b4, b5 };

    sort<Base<int>>(b, 5);

    for (int i = 0; i < 5; i++)
    {
        printf("%d %d\n", b[i].x, b[i].y);
    }
}

void TestFrendOperatorReload()
{
    FrendOperator fo1(3), fo2(6), fo3;
    bool flag;

    fo3 = fo1 + fo2;
    printf("fo1 + fo2 = %lf\n", fo3.x);
    fo3 = fo1 - fo2;
    printf("fo1 - fo2 = %lf\n", fo3.x);
    fo3 = fo1 * fo2;
    printf("fo1 * fo2 = %lf\n", fo3.x);
    fo3 = fo1 / fo2;
    printf("fo1 / fo2 = %lf\n", fo3.x);
    flag = fo1 > fo2;
    printf("fo1 > fo2 : %d\n", flag);
    flag = fo1 < fo2;
    printf("fo1 < fo2 : %d\n", flag);
    flag = fo1 == fo2;
    printf("fo1 == fo2 : %d\n", flag);
}

void TestVector()
{
    Vector<Base<int>>* pVector = new Vector<Base<int>>[5];
    Base<int> base1(1, 2);
    Base<int> base2(3, 4);
    Base<int> base3(5, 6);
    Base<int> base4(7, 8);

    pVector->push_back(base1);
    pVector->push_back(base2);
    pVector->push_back(base3);
    pVector->push_back(base4);

}

int main()
{
	//TestMyString();

    //PrintVirtualTable();

    //TestTemplateOperatorReload();

    //TestFrendOperatorReload();

    TestVector();

	return 0;
}