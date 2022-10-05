#include "VectorImplement.h"

template <class T_ELE>
Vector<T_ELE>::Vector()
:m_dwInitSize(100), m_dwIncrement(5)
{
	//1.创建长度为m_dwInitSize个T_ELE对象						
	m_pVector = new T_ELE[m_dwInitSize];
	//2.将新创建的空间初始化						
	memset(m_pVector, 0, sizeof(T_ELE) * m_dwInitSize);
	//3.设置其他值				
	m_dwLen = m_dwInitSize;
	m_dwIndex = 0;
}

template <class T_ELE>
Vector<T_ELE>::Vector(DWORD dwSize)
:m_dwIncrement(5)
{
	//1.创建长度为dwSize个T_ELE对象						
	m_pVector = new T_ELE[dwSize];
	//2.将新创建的空间初始化						
	memset(m_pVector, 0, sizeof(T_ELE) * dwSize);
	//3.设置其他值				
	m_dwLen = dwSize;
	m_dwIndex = 0;
}

template <class T_ELE>
Vector<T_ELE>::~Vector()
{
	//释放空间 delete[]						
	delete[] m_pVector;
	m_pVector = NULL;
}

template <class T_ELE>
BOOL Vector<T_ELE>::expand()
{
	// 1. 计算增加后的长度						
	DWORD afterLen = m_dwLen + m_dwIncrement;
	// 2. 申请空间						
	T_ELE* newVector = new T_ELE[afterLen];
	if (!newVector)
	{
		return ERROR;
	}
	// 3. 将数据复制到新的空间						
	memcpy(newVector, m_pVector, sizeof(T_ELE) * m_dwLen);
	// 4. 释放原来空间						
	delete[] m_pVector;
	// 5. 为各种属性赋值						
	m_pVector = newVector;
	m_dwLen = afterLen;
	return SUCCESS;
}

template <class T_ELE>
DWORD  Vector<T_ELE>::push_back(T_ELE Element)
{
	//1.判断是否需要增容，如果需要就调用增容的函数						
	if (m_dwIndex == m_dwLen)
	{
		expand();
	}
	//2.将新的元素复制到容器的最后一个位置						
	m_pVector[m_dwIndex] = Element;
	//3.修改属性值						
	m_dwIndex++;
	return SUCCESS;
}

template <class T_ELE>
DWORD  Vector<T_ELE>::insert(DWORD dwIndex, T_ELE Element)
{
	//1.判断是否需要增容，如果需要就调用增容的函数						
	if (m_dwIndex == m_dwLen)
	{
		expand();
	}
	//2.判断索引是否在合理区间						
	if (dwIndex < 0 || dwIndex > m_dwIndex)
	{
		return INDEX_ERROR;
	}
	//3.将dwIndex只后的元素后移						
	for (DWORD i = m_dwIndex; i > dwIndex; i--)
	{
		m_pVector[i] = m_pVector[i - 1];
	}
	//4.将Element元素复制到dwIndex位置						
	m_pVector[dwIndex] = Element;
	//5.修改属性值				
	m_dwIndex++;
	return SUCCESS;
}

template <class T_ELE>
DWORD Vector<T_ELE>::at(DWORD dwIndex, T_ELE* pEle)
{
	//判断索引是否在合理区间						
	if (dwIndex < 0 || dwIndex > m_dwIndex)
	{
		return INDEX_ERROR;
	}
	//将dwIndex的值复制到pEle指定的内存						
	*pEle = m_pVector[dwIndex];
	return SUCCESS;
}

//其他函数。。自己实现	
template<class T_ELE>
VOID Vector<T_ELE>::pop_back()
{
	//判断是否为空
	if (!empty())
	{
		//修改属性值
		m_dwIndex--;
	}
}

template<class T_ELE>
BOOL Vector<T_ELE>::empty()
{
	if (m_dwIndex == 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

template<class T_ELE>
DWORD Vector<T_ELE>::capacity()
{
	return m_dwLen - m_dwIndex;
}

template<class T_ELE>
VOID Vector<T_ELE>::clear()
{
	m_dwIndex = 0;
}

template<class T_ELE>
VOID Vector<T_ELE>::erase(DWORD dwIndex)
{
	//判断索引是否在合理区间
	if (dwIndex < 0 || dwIndex >= m_dwIndex)
	{
		return INDEX_ERROR;
	}
	//将dwIndex之后的元素前移
	for (DWORD i = dwIndex; i < m_dwIndex - 1; i++)
	{
		m_pVector[i] = m_pVector[i + 1];
	}
	//修改属性值
	m_dwIndex--;
}

template<class T_ELE>
DWORD Vector<T_ELE>::size()
{
	return m_dwIndex;
}