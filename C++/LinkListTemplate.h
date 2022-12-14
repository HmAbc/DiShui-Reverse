#pragma once
#include <Windows.h>
#define SUCCESS           1 // 执行成功											
#define ERROR_            -1 // 执行失败											
#define INDEX_IS_ERROR   -2 // 错误的索引号											
#define BUFFER_IS_EMPTY  -3 // 缓冲区已空											


template <class T_ELE>
class LinkedList
{
public:
	LinkedList();
	~LinkedList();
public:
	BOOL  IsEmpty();											//判断链表是否为空 空返回1 非空返回0				
	void  Clear();												//清空链表				
	DWORD GetElement(IN DWORD dwIndex, OUT T_ELE& Element);		//根据索引获取元素				
	DWORD GetElementIndex(IN T_ELE& Element);					//根据元素获取链表中的索引				
	DWORD Insert(IN T_ELE Element);								//新增元素				
	DWORD Insert(IN DWORD dwIndex, IN T_ELE Element);			//根据索引新增元素				
	DWORD Delete(IN DWORD dwIndex);								//根据索引删除元素				
	DWORD GetSize();											//获取链表中元素的数量				
private:
	typedef struct _NODE
	{
		T_ELE  Data;
		_NODE* pNext;
	}NODE, * PNODE;
	PNODE GetIndexCurrentNode(DWORD dwIndex);					//获取索引为dwIndex的指针				
	PNODE GetIndexPreviousNode(DWORD dwIndex);					//获取索引为dwIndex的前一个节点指针				
	PNODE GetIndexNextNode(DWORD dwIndex);						//获取索引为dwIndex的后一个节点指针				
private:
	PNODE m_pList;												//链表头指针，指向第一个节点				
	DWORD m_dwLength;											//元素的数量				
};

//无参构造函数 初始化成员											
template<class T_ELE> LinkedList<T_ELE>::LinkedList()
	:m_pList(NULL), m_dwLength(0)
{
}

//析构函数 清空元素			
template<class T_ELE> LinkedList<T_ELE>::~LinkedList()
{
	Clear();
}
//判断链表是否为空											
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
//清空链表											
template<class T_ELE> void LinkedList<T_ELE>::Clear()
{
	// 1. 判断链表是否为空										
	if (IsEmpty())
	{
		return;
	}
	// 2. 循环删除链表中的节点		
	PNODE pPreviousNode = NULL, pCurrentNode = m_pList;
	while (pCurrentNode->pNext)
	{
		pPreviousNode = pCurrentNode;
		pCurrentNode = pCurrentNode->pNext;
		delete pPreviousNode;
	}
	// 3. 删除最后一个节点并将链表长度置为0										
	delete pCurrentNode;
	m_pList = NULL;
	m_dwLength = 0;
}
//根据索引获取元素											
template<class T_ELE> DWORD LinkedList<T_ELE>::GetElement(IN DWORD dwIndex, OUT T_ELE& Element)
{
	// 1. 判断索引是否有效										
	if (dwIndex < 0 || dwIndex >= m_dwLength)
	{
		return INDEX_IS_ERROR;
	}
	// 2. 取得索引指向的节点										
	PNODE pCurrentNode = GetIndexCurrentNode(dwIndex);
	// 3. 将索引指向节点的值复制到OUT参数				
	Element = pCurrentNode->Data;
	return SUCCESS;

}
//根据元素内容获取索引											
template<class T_ELE> DWORD LinkedList<T_ELE>::GetElementIndex(IN T_ELE& Element)
{
	// 1. 判断链表是否为空										
	if (m_pList == NULL || m_dwLength == 0)
	{
		return BUFFER_IS_EMPTY;
	}
	// 2. 循环遍历链表，找到与Element相同的元素		
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
//在链表尾部新增节点											
template<class T_ELE> DWORD LinkedList<T_ELE>::Insert(IN T_ELE Element)
{
	// 1. 判断链表是否为空			
	PNODE pNewNode = new NODE;
	memset(pNewNode, 0, sizeof(NODE));
	memcpy(&pNewNode->Data, &Element, sizeof(T_ELE));
	if (m_pList == NULL || m_dwLength == 0)
	{
		m_pList = pNewNode;
		m_dwLength++;
		return SUCCESS;
	}
	// 2. 如果链表中已经有元素			
	PNODE pTempNode = m_pList;
	for (DWORD i = 0; i < m_dwLength - 1; i++)
	{
		pTempNode = pTempNode->pNext;
	}
	pTempNode->pNext = pNewNode;
	m_dwLength++;
	return SUCCESS;
}
//将节点新增到指定索引的位置						0 1 2 3 4					
template<class T_ELE> DWORD LinkedList<T_ELE>::Insert(IN DWORD dwIndex, IN T_ELE Element)
{
	PNODE pPreviousNode = NULL, pCurrentNode = NULL;
	PNODE pNewNode = new NODE;
	memset(pNewNode, 0, sizeof(NODE));
	memcpy(&pNewNode->Data, &Element, sizeof(T_ELE));
	//  1. 判断链表是否为空										
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
	//  2. 判断索引值是否有效										
	if (dwIndex < 0 || dwIndex > m_dwLength)
	{
		delete pNewNode;
		return INDEX_IS_ERROR;
	}
	//  3. 如果索引为0										
	if (dwIndex == 0)
	{
		pNewNode->pNext = m_pList;
		m_pList = pNewNode;
		m_dwLength++;
		return SUCCESS;
	}
	//  4. 如果索引为链表尾										
	if (dwIndex == m_dwLength)
	{
		pPreviousNode = GetIndexPreviousNode(dwIndex);
		pPreviousNode->pNext = pNewNode;
		m_dwLength++;
		return SUCCESS;
	}
	//  5. 如果索引为链表中
	pPreviousNode = GetIndexPreviousNode(dwIndex);
	pNewNode->pNext = pPreviousNode->pNext;
	pPreviousNode->pNext = pNewNode;
	m_dwLength++;
	return SUCCESS;
}
//根据索引删除节点											
template<class T_ELE> DWORD LinkedList<T_ELE>::Delete(IN DWORD dwIndex)
{
	PNODE pPreviousNode = NULL, pCurrentNode = NULL;
	//  1. 判断链表是否为空										
	if (m_pList == NULL || m_dwLength == 0)
	{
		return BUFFER_IS_EMPTY;
	}
	//  2. 判断索引值是否有效										
	if (dwIndex < 0 || dwIndex >= m_dwLength)
	{
		return INDEX_IS_ERROR;
	}
	//  3. 如果链表中只有头节点，且要删除头节点										
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
	//  4. 如果要删除头节点										
	if (dwIndex == 0)
	{
		pCurrentNode = GetIndexCurrentNode(dwIndex);
		m_pList = m_pList->pNext;
		delete pCurrentNode;
		m_dwLength--;
		return SUCCESS;
	}
	//  5. 如果是其他情况			
	pPreviousNode = GetIndexPreviousNode(dwIndex);
	pCurrentNode = GetIndexCurrentNode(dwIndex);
	pPreviousNode->pNext = pCurrentNode->pNext;
	delete pCurrentNode;
	m_dwLength--;
	return SUCCESS;
}
//获取链表中节点的数量											
template<class T_ELE> DWORD LinkedList<T_ELE>::GetSize()
{
	return m_dwLength;
}
//获取dwIndex前面节点的地址											
template<class T_ELE>
typename LinkedList<T_ELE>::PNODE LinkedList<T_ELE>::GetIndexPreviousNode(DWORD dwIndex)
{
	// 就是一个循环										
	PNODE pTempNode = m_pList;
	for (DWORD i = 0; i < dwIndex - 1; i++)
	{
		pTempNode = pTempNode->pNext;
	}
	return pTempNode;
}
//获取dwIndex节点的地址											
template<class T_ELE>
typename LinkedList<T_ELE>::PNODE LinkedList<T_ELE>::GetIndexCurrentNode(DWORD dwIndex)
{
	// 就是一个循环										
	PNODE pTempNode = m_pList;
	for (DWORD i = 0; i < dwIndex; i++)
	{
		pTempNode = pTempNode->pNext;
	}
	return pTempNode;
}
//获取dwIndex后面节点的地址											
template<class T_ELE>
typename LinkedList<T_ELE>::PNODE LinkedList<T_ELE>::GetIndexNextNode(DWORD dwIndex)
{
	// 就是一个循环										
	PNODE pTempNode = m_pList;
	for (DWORD i = 0; i <= dwIndex; i++)
	{
		pTempNode = pTempNode->pNext;
	}
	return pTempNode;
}
