/**
*	\file cStack.h
*	\author Vaclav Snasel
*	\version 0.2
*	\date 1999, 2002, jun 2006, feb 2012 (Michal Kratky, memory pool extension)
*	\brief stack
*
* Example: We can use the stack with a memory pool.
*
* char* sps = mMemoryPool->GetMem(PathStackByteSize);
* ItemIdRecord itemIdRec = {mHeader->GetRootIndex(), -1};
* cStack<ItemIdRecord> curPathStack(sps, PathStackByteSize);
* curPathStack.Push(itemIdRec);
* ...
* mMemoryPool->FreeMem(sps);
* 
*/

#ifndef __CSTACK_H__
#define __CSTACK_H__

#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
*	Template of common stack. Template is parametrized with type of stack items.
*
*	\author Vaclav Snasel
*	\version 0.2
*	\date 1999, 2002, jun 2006
**/
template <class T>
class cStack
{
public:
	cStack(const int Size = 100);
	cStack(char* memory, const int byteSize);

	~cStack();
	inline void Init(char* memory, const int byteSize);
	inline void Push(const T &Pos);
	inline T &Pop();
	inline void Pop(unsigned int n);
	inline T &Top() const;
	inline T* TopRef()							{ assert(m_Sp >= 0); return &m_Items[m_Sp]; }
	inline void Insert(unsigned int position, const T& item);
	inline T &GetItem(unsigned int order)		{ return m_Items[order]; }
	inline T *GetRefItem(unsigned int order)	{ return &(m_Items[order]);}
	inline bool Empty() const;
	inline void Clear();
	inline int Count() const;
	inline int Size() const;

protected:
	T* m_Items;
	int m_Sp;
	int m_Size;
	bool m_MemoryPoolUsed;

	void m_Resize(const unsigned int Size, const bool Move);
};


/// constructor
/// \param Size maximum size of the stack
template <class T> cStack<T>::cStack(const int Size): m_Sp(-1)
{
	m_Size = Size;
	m_Items = new T[Size];
	m_MemoryPoolUsed = false;
}

/// constructor
/// \param Size maximum size of the stack
template <class T> cStack<T>::cStack(char* memory, const int byteSize): m_Sp(-1)
{
	m_Size = byteSize/sizeof(T);
	m_Items = (T*)memory;
	m_MemoryPoolUsed = true;
}

/// destructor
template <class T>
cStack<T>::~cStack()
{
	if (!m_MemoryPoolUsed)
	{
		delete [] m_Items;
	}
}

/// constructor
/// \param Size maximum size of the stack
template <class T> 
inline void cStack<T>::Init(char* memory, const int byteSize)
{
	m_Sp = -1;
	m_Size = byteSize/sizeof(T);
	m_Items = (T*)memory;
	m_MemoryPoolUsed = true;
}

/// Push item on the top of stack
/// \param pos Item which should be pushed on the top of the stack
template <class T>
inline void cStack<T>::Push(const T &pos)
{
	if (m_Sp + 1 >= m_Size)
	{
		m_Resize(m_Size + 10, true);
	}
	assert(m_Sp + 1 < m_Size);
	m_Items[++m_Sp] = pos;
}

/// Remove an item from the top of the stack
/// \returns Item on the top of the stack
template <class T>
inline T &cStack<T>::Pop()
{
	assert(m_Sp >= 0);
	return m_Items[m_Sp--];
}

/**
* \param n Number od items that should be removed from the stack.
* Remove n items from the stack.
*/
template<class T>
void cStack<T>::Pop(unsigned int n)				
{	
	assert(m_Sp >= (int)(n - 1)); 
	m_Sp -= (int)n; 
}

/// Insert item not to the top but to some other position.
/// \param position Position from the bottom, where the item should be inserted.
/// \param item Item which is inserted.
template<class T>
void cStack<T>::Insert(unsigned int position, const T &item)
{
	assert(position < m_Sp + 1);
	assert(m_Sp + 1 < m_Size);

	memmove(&m_Items[position + 1], &m_Items[position], sizeof(T) * (m_Sp - position + 1));
	m_Items[position] = item;
	m_Sp++;
}

/// Get the reference on the item from the top of the stack
/// \returns Item on the top of the stack
template <class T>
inline T &cStack<T>::Top() const // rb - return by reference
{
	assert(m_Sp >= 0);
	return m_Items[m_Sp];
}

/// Check if stack is empty
/// \returns True if stack is empty
template <class T>
inline bool cStack<T>::Empty() const
{
   return m_Sp == -1;
}

/// Clear the stack
template <class T>
inline void cStack<T>::Clear()
{
	m_Sp = -1;
}

/// \returns Number of the items in the stack
template <class T>
inline int cStack<T>::Count() const
{
	return m_Sp + 1;
}

/// \returns Maximum size of the stack
template <class T>
inline int cStack<T>::Size() const
{
	return m_Size;
}

/// Resize the stack.
/// \param Size Maximal size of the stack
/// \param Move Copy current values
template<class T>
void cStack<T>::m_Resize(const unsigned int Size, const bool Move)
{
	if (m_MemoryPoolUsed)
	{
		printf("Critical Error: cStack<T>::m_Resize(): There is no memory from a memory pool!");
		return;
	}

	if (Size > (unsigned int)m_Size)
	{
		// mk: m_Size = (Size & 7) ? Size + (8 - (Size & 7)): Size;
		m_Size = Size;
		T *auxPtr = new T[m_Size];
		if (m_Items != NULL)
		{
			if (Move)
			{
				memcpy(auxPtr, m_Items, sizeof(T) * (m_Sp + 1));
			}
			delete [] m_Items;
		}
		m_Items = auxPtr;
		if (Count() > m_Size) //pm
		{
			m_Sp = m_Size - 1;
		}
	} 
}
#endif            //    __CSTACK_H__