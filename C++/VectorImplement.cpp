#include "VectorImplement.h"

template <class T_ELE>
Vector<T_ELE>::Vector()
:m_dwInitSize(100), m_dwIncrement(5)
{
	//1.��������Ϊm_dwInitSize��T_ELE����						
	m_pVector = new T_ELE[m_dwInitSize];
	//2.���´����Ŀռ��ʼ��						
	memset(m_pVector, 0, sizeof(T_ELE) * m_dwInitSize);
	//3.��������ֵ				
	m_dwLen = m_dwInitSize;
	m_dwIndex = 0;
}

template <class T_ELE>
Vector<T_ELE>::Vector(DWORD dwSize)
:m_dwIncrement(5)
{
	//1.��������ΪdwSize��T_ELE����						
	m_pVector = new T_ELE[dwSize];
	//2.���´����Ŀռ��ʼ��						
	memset(m_pVector, 0, sizeof(T_ELE) * dwSize);
	//3.��������ֵ				
	m_dwLen = dwSize;
	m_dwIndex = 0;
}

template <class T_ELE>
Vector<T_ELE>::~Vector()
{
	//�ͷſռ� delete[]						
	delete[] m_pVector;
	m_pVector = NULL;
}

template <class T_ELE>
BOOL Vector<T_ELE>::expand()
{
	// 1. �������Ӻ�ĳ���						
	DWORD afterLen = m_dwLen + m_dwIncrement;
	// 2. ����ռ�						
	T_ELE* newVector = new T_ELE[afterLen];
	if (!newVector)
	{
		return ERROR;
	}
	// 3. �����ݸ��Ƶ��µĿռ�						
	memcpy(newVector, m_pVector, sizeof(T_ELE) * m_dwLen);
	// 4. �ͷ�ԭ���ռ�						
	delete[] m_pVector;
	// 5. Ϊ�������Ը�ֵ						
	m_pVector = newVector;
	m_dwLen = afterLen;
	return SUCCESS;
}

template <class T_ELE>
DWORD  Vector<T_ELE>::push_back(T_ELE Element)
{
	//1.�ж��Ƿ���Ҫ���ݣ������Ҫ�͵������ݵĺ���						
	if (m_dwIndex == m_dwLen)
	{
		expand();
	}
	//2.���µ�Ԫ�ظ��Ƶ����������һ��λ��						
	m_pVector[m_dwIndex] = Element;
	//3.�޸�����ֵ						
	m_dwIndex++;
	return SUCCESS;
}

template <class T_ELE>
DWORD  Vector<T_ELE>::insert(DWORD dwIndex, T_ELE Element)
{
	//1.�ж��Ƿ���Ҫ���ݣ������Ҫ�͵������ݵĺ���						
	if (m_dwIndex == m_dwLen)
	{
		expand();
	}
	//2.�ж������Ƿ��ں�������						
	if (dwIndex < 0 || dwIndex > m_dwIndex)
	{
		return INDEX_ERROR;
	}
	//3.��dwIndexֻ���Ԫ�غ���						
	for (DWORD i = m_dwIndex; i > dwIndex; i--)
	{
		m_pVector[i] = m_pVector[i - 1];
	}
	//4.��ElementԪ�ظ��Ƶ�dwIndexλ��						
	m_pVector[dwIndex] = Element;
	//5.�޸�����ֵ				
	m_dwIndex++;
	return SUCCESS;
}

template <class T_ELE>
DWORD Vector<T_ELE>::at(DWORD dwIndex, T_ELE* pEle)
{
	//�ж������Ƿ��ں�������						
	if (dwIndex < 0 || dwIndex > m_dwIndex)
	{
		return INDEX_ERROR;
	}
	//��dwIndex��ֵ���Ƶ�pEleָ�����ڴ�						
	*pEle = m_pVector[dwIndex];
	return SUCCESS;
}

//�������������Լ�ʵ��	
template<class T_ELE>
VOID Vector<T_ELE>::pop_back()
{
	//�ж��Ƿ�Ϊ��
	if (!empty())
	{
		//�޸�����ֵ
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
	//�ж������Ƿ��ں�������
	if (dwIndex < 0 || dwIndex >= m_dwIndex)
	{
		return INDEX_ERROR;
	}
	//��dwIndex֮���Ԫ��ǰ��
	for (DWORD i = dwIndex; i < m_dwIndex - 1; i++)
	{
		m_pVector[i] = m_pVector[i + 1];
	}
	//�޸�����ֵ
	m_dwIndex--;
}

template<class T_ELE>
DWORD Vector<T_ELE>::size()
{
	return m_dwIndex;
}