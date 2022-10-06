#pragma once
#include <Windows.h>
#define SUCCESS           1 // ִ�гɹ�											
#define ERROR_            -1 // ִ��ʧ��											
#define INDEX_IS_ERROR   -2 // �����������											
#define BUFFER_IS_EMPTY  -3 // �������ѿ�											


template <class T_ELE>
class LinkedList
{
public:
	LinkedList();
	~LinkedList();
public:
	BOOL  IsEmpty();											//�ж������Ƿ�Ϊ�� �շ���1 �ǿշ���0				
	void  Clear();												//�������				
	DWORD GetElement(IN DWORD dwIndex, OUT T_ELE& Element);		//����������ȡԪ��				
	DWORD GetElementIndex(IN T_ELE& Element);					//����Ԫ�ػ�ȡ�����е�����				
	DWORD Insert(IN T_ELE Element);								//����Ԫ��				
	DWORD Insert(IN DWORD dwIndex, IN T_ELE Element);			//������������Ԫ��				
	DWORD Delete(IN DWORD dwIndex);								//��������ɾ��Ԫ��				
	DWORD GetSize();											//��ȡ������Ԫ�ص�����				
private:
	typedef struct _NODE
	{
		T_ELE  Data;
		_NODE* pNext;
	}NODE, * PNODE;
	PNODE GetIndexCurrentNode(DWORD dwIndex);					//��ȡ����ΪdwIndex��ָ��				
	PNODE GetIndexPreviousNode(DWORD dwIndex);					//��ȡ����ΪdwIndex��ǰһ���ڵ�ָ��				
	PNODE GetIndexNextNode(DWORD dwIndex);						//��ȡ����ΪdwIndex�ĺ�һ���ڵ�ָ��				
private:
	PNODE m_pList;												//����ͷָ�룬ָ���һ���ڵ�				
	DWORD m_dwLength;											//Ԫ�ص�����				
};

//�޲ι��캯�� ��ʼ����Ա											
template<class T_ELE> LinkedList<T_ELE>::LinkedList()
	:m_pList(NULL), m_dwLength(0)
{
}

//�������� ���Ԫ��			
template<class T_ELE> LinkedList<T_ELE>::~LinkedList()
{
	Clear();
}
//�ж������Ƿ�Ϊ��											
template<class T_ELE> BOOL LinkedList<T_ELE>::IsEmpty()
{
	if (m_pList == NULL || m_dwLength == 0)
	{
		return SUCCESS;
	}
	else
	{
		return 0;
	}
}
//�������											
template<class T_ELE> void LinkedList<T_ELE>::Clear()
{
	// 1. �ж������Ƿ�Ϊ��										
	if (IsEmpty())
	{
		return;
	}
	// 2. ѭ��ɾ�������еĽڵ�		
	PNODE pPreviousNode = m_pList, pCurrentNode = m_pList;
	while (pCurrentNode->pNext)
	{
		pCurrentNode = pCurrentNode->pNext;

	}
	// 3. ɾ�����һ���ڵ㲢����������Ϊ0										

}
//����������ȡԪ��											
template<class T_ELE> DWORD LinkedList<T_ELE>::GetElement(IN DWORD dwIndex, OUT T_ELE& Element)
{
	// 1. �ж������Ƿ���Ч										
	if (dwIndex < 0 || dwIndex >= m_dwLength)
	{
		return INDEX_IS_ERROR;
	}
	// 2. ȡ������ָ��Ľڵ�										
	PNODE pCurrentNode = GetIndexCurrentNode(dwIndex);
	// 3. ������ָ��ڵ��ֵ���Ƶ�OUT����				
	Element = pCurrentNode->Data;
	return SUCCESS;

}
//����Ԫ�����ݻ�ȡ����											
template<class T_ELE> DWORD LinkedList<T_ELE>::GetElementIndex(IN T_ELE& Element)
{
	// 1. �ж������Ƿ�Ϊ��										
	if (m_pList == NULL || m_dwLength == 0)
	{
		return BUFFER_IS_EMPTY;
	}
	// 2. ѭ�����������ҵ���Element��ͬ��Ԫ��		
	PNODE pTempNode = m_pList;
	for (DWORD i = 0; i < m_dwLength; i++)
	{
		if (Element == pTempNode->Data)
		{
			return i;
		}
		pTempNode = pTempNode->pNext;
	}
	return ERROR_;
}
//������β�������ڵ�											
template<class T_ELE> DWORD LinkedList<T_ELE>::Insert(IN T_ELE Element)
{
	// 1. �ж������Ƿ�Ϊ��			
	PNODE pNewNode = new NODE;
	memset(pNewNode, 0, sizeof(NODE));
	memcpy(&pNewNode->Data, &Element, sizeof(T_ELE));
	if (m_pList == NULL || m_dwLength == 0)
	{
		m_pList = pNewNode;
		m_dwLength++;
		return SUCCESS;
	}
	// 2. ����������Ѿ���Ԫ��			
	PNODE pTempNode = m_pList;
	for (DWORD i = 0; i < m_dwLength - 1; i++)
	{
		pTempNode = pTempNode->pNext;
	}
	pTempNode->pNext = pNewNode;
	m_dwLength++;
	return SUCCESS;
}
//���ڵ�������ָ��������λ��						0 1 2 3 4					
template<class T_ELE> DWORD LinkedList<T_ELE>::Insert(IN DWORD dwIndex, IN T_ELE Element)
{
	PNODE pPreviousNode = NULL, pCurrentNode = NULL;
	PNODE pNewNode = new NODE;
	memset(pNewNode, 0, sizeof(NODE));
	memcpy(&pNewNode->Data, &Element, sizeof(T_ELE));
	//  1. �ж������Ƿ�Ϊ��										
	if (m_pList == NULL || m_dwLength == 0)
	{
		if (dwIndex == 0)
		{
			m_pList = pNewNode;
			m_dwLength++;
			return SUCCESS;
		}
		else
		{
			delete pNewNode;
			return INDEX_IS_ERROR;
		}
	}
	//  2. �ж�����ֵ�Ƿ���Ч										
	if (dwIndex < 0 || dwIndex > m_dwLength)
	{
		delete pNewNode;
		return INDEX_IS_ERROR;
	}
	//  3. �������Ϊ0										
	if (dwIndex == 0)
	{
		pNewNode->pNext = m_pList;
		m_pList = pNewNode;
		m_dwLength++;
		return SUCCESS;
	}
	//  4. �������Ϊ����β										
	if (dwIndex == m_dwLength)
	{
		pPreviousNode = GetIndexPreviousNode(dwIndex);
		pPreviousNode->pNext = pNewNode;
		m_dwLength++;
		return SUCCESS;
	}
	//  5. �������Ϊ������
	pPreviousNode = GetIndexPreviousNode(dwIndex);
	pNewNode->pNext = pPreviousNode->pNext;
	pPreviousNode->pNext = pNewNode;
	m_dwLength++;
	return SUCCESS;
}
//��������ɾ���ڵ�											
template<class T_ELE> DWORD LinkedList<T_ELE>::Delete(IN DWORD dwIndex)
{
	PNODE pPreviousNode = NULL, pCurrentNode = NULL;
	//  1. �ж������Ƿ�Ϊ��										
	if (m_pList == NULL || m_dwLength == 0)
	{
		return BUFFER_IS_EMPTY;
	}
	//  2. �ж�����ֵ�Ƿ���Ч										
	if (dwIndex < 0 || dwIndex >= m_dwLength)
	{
		return INDEX_IS_ERROR;
	}
	//  3. ���������ֻ��ͷ�ڵ㣬��Ҫɾ��ͷ�ڵ�										
	if (m_pList->pNext == NULL && m_dwLength == 1)
	{
		if (dwIndex == 0)
		{
			delete m_pList;
			m_pList = NULL;
			m_dwLength--;
			return SUCCESS;
		}
	}
	//  4. ���Ҫɾ��ͷ�ڵ�										
	if (dwIndex == 0)
	{
		pCurrentNode = GetIndexCurrentNode(dwIndex);
		m_pList = m_pList->pNext;
		delete pCurrentNode;
		m_dwLength--;
		return SUCCESS;
	}
	//  5. ������������			
	pPreviousNode = GetIndexPreviousNode(dwIndex);
	pCurrentNode = GetIndexCurrentNode(dwIndex);
	pPreviousNode->pNext = pCurrentNode->pNext;
	delete pCurrentNode;
	m_dwLength--;
	return SUCCESS;
}
//��ȡ�����нڵ������											
template<class T_ELE> DWORD LinkedList<T_ELE>::GetSize()
{
	return m_dwLength;
}
//��ȡdwIndexǰ��ڵ�ĵ�ַ											
template<class T_ELE>
LinkedList<T_ELE>::PNODE LinkedList<T_ELE>::GetIndexPreviousNode(DWORD dwIndex)
{
	// ����һ��ѭ��										
	PNODE pTempNode = m_pList;
	for (DWORD i = 0; i < dwIndex - 1; i++)
	{
		pTempNode = pTempNode->pNext;
	}
	return pTempNode;
}
//��ȡdwIndex�ڵ�ĵ�ַ											
template<class T_ELE>
LinkedList<T_ELE>::PNODE LinkedList<T_ELE>::GetIndexCurrentNode(DWORD dwIndex)
{
	// ����һ��ѭ��										
	PNODE pTempNode = m_pList;
	for (DWORD i = 0; i < dwIndex; i++)
	{
		pTempNode = pTempNode->pNext;
	}
	return pTempNode;
}
//��ȡdwIndex����ڵ�ĵ�ַ											
template<class T_ELE>
LinkedList<T_ELE>::PNODE LinkedList<T_ELE>::GetIndexNextNode(DWORD dwIndex)
{
	// ����һ��ѭ��										
	PNODE pTempNode = m_pList;
	for (DWORD i = 0; i <= dwIndex; i++)
	{
		pTempNode = pTempNode->pNext;
	}
	return pTempNode;
}
