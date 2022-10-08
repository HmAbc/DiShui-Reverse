#include "BaseClass.h"
#include <iostream>
#include "VectorImplement.h"
#include "LinkListTemplate.h"
#include "BinaryTreePractice.h"

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

//冒泡排序
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

struct Person
{
    int x;
    int y;
    Person()
    {
        x = 5;
        y = 5;
    }
    Person(int x, int y)
    {
        this->x = x;
        this->y = y;
    }
};

void TestVector()
{
    Vector<Person>* pVector = new Vector<Person>(5);
    Person p1(1, 2);
    Person p2(3, 4);
    Person p3(5, 6);

    pVector->push_back(p1);
    pVector->push_back(p2);
    pVector->insert(2, p3);

    pVector->clear();
}

void TestLinkedList()
{
    bool isEmpty = 0;
    int isOk = 0;
    int temp = 0;
    LinkedList<int>* pLinkedList = new LinkedList<int>;

    pLinkedList->Insert(3);
    pLinkedList->Insert(2);
    pLinkedList->Insert(4);
    pLinkedList->Insert(5);

    pLinkedList->Insert(0, 10);
    pLinkedList->Insert(2, 14);
    isOk = pLinkedList->Insert(9, 14);
    printf("插入结果：%d\n", isOk);

    isEmpty = pLinkedList->IsEmpty();
    printf("链表为空：%d\n", isEmpty);

    printf("链表中元素个数：%d\n", pLinkedList->GetSize());
    for (int i = 0; i < pLinkedList->GetSize(); i++)
    {
        pLinkedList->GetElement(i, temp);
        printf("第 %d 个元素为 %d\n", i + 1, temp);
    }

    pLinkedList->Delete(0);
    pLinkedList->Delete(3);
    isOk = pLinkedList->Delete(9);
    printf("删除结果：%d\n", isOk);

    printf("链表中元素个数：%d\n", pLinkedList->GetSize());
    for (int i = 0; i < pLinkedList->GetSize(); i++)
    {
        pLinkedList->GetElement(i, temp);
        printf("第 %d 个元素为 %d\n", i + 1, temp);
    }

    pLinkedList->Clear();
    isEmpty = pLinkedList->IsEmpty();
    printf("链表为空：%d\n", isEmpty);
}

void TestBinaryTree()
{
    BSortTree<Monster>* pBSortTree = new BSortTree<Monster>;

    TreeNode<Monster>* root = pBSortTree->GetRoot();

    int depth = pBSortTree->GetDepth(root);
    printf("depth = %d\n", depth);

    pBSortTree->InOrderTraverse(root);
    pBSortTree->PreOrderTraverse(root);
    pBSortTree->PostOrderTraverse(root);

    pBSortTree->Clear(root);
}

int main()
{
	//TestMyString();

    //PrintVirtualTable();

    //TestTemplateOperatorReload();

    //TestFrendOperatorReload();

    //TestVector();

    //TestLinkedList();

    TestBinaryTree();

	return 0;
}