/**
*	\file cGeneralStack.h
*	\author Radim Baca
*	\version 0.1
*	\date jan 2008
*	\brief Stack parametrized by class inheriting from cBasicType
*/

#ifndef __cGeneralStack_h__
#define __cGeneralStack_h__

#include <stdio.h>
#include <assert.h>

#include "common/datatype/cDTDescriptor.h"

/**
*	Stack parametrized by class inheriting from cBasicType
*
*	\author Radim Baca
*	\version 0.1
*	\date jan 2008
**/
template <class ItemType>
class cGeneralStack
{
	cDTDescriptor *m_Descriptor;

public:
	cGeneralStack(cDTDescriptor *descriptor, const int Size = 100);
	~cGeneralStack();

	inline void Push(const ItemType &Pos);
	inline ItemType& Pop()						
	{	
		assert(m_Sp >= 0);	
		return m_Items[m_Sp--]; 
	}
	inline void Pop(unsigned int n)				{	assert(m_Sp >= (int)(n - 1)); m_Sp -= (int)n; }
	inline ItemType& Top() const				
	{
		assert(m_Sp >= 0);	
		return m_Items[m_Sp]; 
	}
	inline ItemType& GetItem(unsigned int order)	
	{
		assert(m_Sp >= (int)order); 
		return m_Items[order]; 
	}
	inline const ItemType* GetRefItem(unsigned int order) const
	{ 
		assert(m_Sp >= (int)order); 
		return &m_Items[order]; 
	}
	inline ItemType* TopRef()					
	{
		assert(m_Sp >= 0);	
		return &m_Items[m_Sp]; 
	}
	inline bool Empty() const;
	inline void Clear();
	inline int Count() const;
	inline int Size() const;

protected:
	ItemType* m_Items;
	int m_Sp;
	int m_Size;

	void m_Resize(const int Size, const bool Move);	
};


/// constructor
/// \param Size maximum size of the stack
template <class ItemType>
cGeneralStack<ItemType>::cGeneralStack(cDTDescriptor *sizeInfo, const int Size): m_Sp(-1), m_Size(0), m_Descriptor(sizeInfo), m_Items(NULL)
{
	m_Resize(Size, false);
	Clear();
}

/// destructor
template <class ItemType>
cGeneralStack<ItemType>::~cGeneralStack()
{
	if (m_Items != NULL)
	{
		delete [] m_Items;
	}
}

/// Push item on the top of stack
/// \param pos Item which should be pushed on the top of the stack
template <class ItemType>
inline void cGeneralStack<ItemType>::Push(const ItemType &pos)
{
	if (++m_Sp == m_Size)
	{
		m_Resize(m_Size + 10, true);
	}
	ItemType::Copy((char*)m_Items[m_Sp].GetData(), (char*)pos.GetData(), m_Descriptor);
}

/// Check if stack is empty
/// \return True if stack is empty
template <class ItemType>
inline bool cGeneralStack<ItemType>::Empty() const
{
   return m_Sp == -1;
}

/// Clear the stack
template <class ItemType>
inline void cGeneralStack<ItemType>::Clear()
{
	m_Sp = -1;
}

/// \return Number of the items in the stack
template <class ItemType>
inline int cGeneralStack<ItemType>::Count() const
{
	return m_Sp + 1;
}

/// \return Maximum size of the stack
template <class ItemType>
inline int cGeneralStack<ItemType>::Size() const
{
	return m_Size;
}


template <class ItemType>
inline void cGeneralStack<ItemType>::m_Resize(const int Size, const bool Move)
{
	if (Size > m_Size)
	{
		// mk: m_Size = (Size & 7) ? Size + (8 - (Size & 7)): Size;
		m_Size = Size;
		ItemType *auxPtr = new ItemType[m_Size];
		for (int i = 0; i < m_Size; i++)
		{
			//ItemType::Resize(*m_Descriptor, auxPtr[i]);
			auxPtr[i].Resize(m_Descriptor);
		}
		if (m_Items != NULL)
		{
			if (Move)
			{
				for (int i = 0; i < m_Sp; i++)
				{
					ItemType::Copy((char*)auxPtr[i].GetData(), (char*)m_Items[i].GetData(), m_Descriptor);
				}
			}
			delete [] m_Items;
		}
		m_Items = auxPtr;
		if (m_Sp > m_Size) //pm
		{
			m_Sp = m_Size;
		}
	} 
	else if (Size == 0)
	{
		if (m_Items != NULL)
		{
			delete [] m_Items;
			m_Items = NULL;
		}
		m_Size = m_Sp = 0;
	}
}

#endif            //    __CSTACK_H__