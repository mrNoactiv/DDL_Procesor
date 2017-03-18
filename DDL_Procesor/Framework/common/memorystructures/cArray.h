/**************************************************************************}
{                                                                          }
{    cArray.h                                            		      	       }
{                                                                          }
{                                                                          }
{                 Copyright (c) 1999, 2003					Vaclav Snasel          }
{                                                                          }
{    VERSION: 2.0							        DATE 20/2/1999         }
{                                                                          }
{             following functionality:                                     }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{                                                                          }
{                                                                          }
{**************************************************************************/

#ifndef __cArray_h__
#define __cArray_h__

#include <stdlib.h>
#include <string.h>
#include <assert.h>

int compare3(const void *a, const void *b);

template<class T>
class cArray
{
public:
	cArray(bool resetNewMemory = false, unsigned int size = 0);
	~cArray();
	const cArray<T>& operator = (const cArray& other);
	inline T& operator[](const int Index);
	inline T& operator[](const unsigned int Index);
	inline const T operator[](const int Index) const;
	inline const T operator[](const unsigned int Index) const;
	inline operator T* (void);
	inline unsigned int Size(void) const;
	inline unsigned int Count(void) const;
	inline void Clear(void);
	inline void ClearCount(void);
	inline void Init(void);
	void Init(char* mem, unsigned int size);
	inline void SetCount(unsigned int k);
	inline void SetSize(unsigned int k);
	inline void Resize(const unsigned int Size, const bool Move = false);
	inline void Resize(const unsigned int size, char *memory);
	inline unsigned int GetItemSize();
	inline void Resize(const unsigned int Size, const unsigned int Count);
	inline void SetMem(char* mem);
	void Move(const T* Array, const unsigned int Count = 1);
	void Append(const T* Array, const unsigned int Position, const unsigned int Count = 1);
	inline void Shift(unsigned int From, unsigned int Count = 1);
	inline void ShiftLeft(unsigned int From, unsigned int Count = 1);
	inline void ShiftBlockLeft(unsigned int From, unsigned int To, unsigned int Count = 1);
	inline void ShiftBlockRight(unsigned int From, unsigned int To, unsigned int Count = 1);
	void Add(const T* Array, const unsigned int Count = 1);
	inline void Add(const T& Array);
	void AddDouble(const T& Array);
	inline void FastAdd(const T* Array, const unsigned int Count = 1);	
	inline void Left(const unsigned int Count);
	inline void Right(const unsigned int Count);
	inline void Mid(const unsigned int Left, const unsigned int Right);
	inline void Append(const T& Value);
	inline T* GetArray(const unsigned int Index = 0);
	inline T* GetArray() const;  // mk
	void Fill(const char ch, const unsigned int Count);
	void BubbleSort();
	void QSortUInt();

	inline T& GetRefItem(unsigned int order) const;
	inline T* GetItem(unsigned int order);
	inline T& GetRefLastItem() const				{ return GetRefItem(Count() - 1); }
	inline T* GetLastItem()							{ return GetItem(Count() - 1); }

	// BUFFER SIZE
	inline int GetListBufferSize() { return sizeof(cArray<T>); }
	inline int GetItemsBufferSize() { return m_Size * (sizeof(T)); }

	const static unsigned int MAXIMAL_INCREASE = 4096;
private:

	unsigned int m_Size;
	unsigned int m_Count;
	bool m_ResetFlag;
	T* m_Array;
	void m_Resize(const unsigned int Size, const bool Move);
};


template<class T>
cArray<T>::cArray(bool resetNewMemory, unsigned int size = 0)
	:m_Count(0), m_ResetFlag(resetNewMemory), m_Array(NULL)
{
	m_Array = new T();
	m_Size = size;
}

template<class T>
cArray<T>::~cArray()
{
	m_Resize(0, false);
}


template<class T>
inline const cArray<T>& cArray<T>::operator = (const cArray& other)
{
	if (this != &other)
	{
		Move((T*)(*(const_cast<cArray<T>*>(&other))), other.Count());
	}
	return *this;
}


template<class T>
inline T& cArray<T>::operator[](const int Index)
{
	assert(Index < m_Count);
	return m_Array[Index];
}


template<class T>
inline T& cArray<T>::operator[](const unsigned int Index)
{
	assert(Index < m_Count);
	return m_Array[Index];
}


template<class T>
inline const T cArray<T>::operator[](const int Index) const
{
	assert(Index < m_Count);
	return m_Array[Index];
}


template<class T>
inline const T cArray<T>::operator[](const unsigned int Index) const
{
	assert(Index < m_Count);
	return m_Array[Index];
}

template<class T>
inline T& cArray<T>::GetRefItem(unsigned int order) const
{
	assert(order < m_Count);
	return m_Array[order];
}

template<class T>
inline T* cArray<T>::GetItem(unsigned int order)
{
	assert(order < m_Count);
	return &(m_Array[order]);
}

template<class T>
inline cArray<T>::operator T* (void)
{
	return m_Array;
}


template<class T>
inline unsigned int cArray<T>::Size(void) const
{
	return m_Size;
}


template<class T>
inline unsigned int cArray<T>::Count(void) const
{
	return m_Count;
}


template<class T>
inline void cArray<T>::Clear(void)
{
	m_Resize(0, false);
}

/**
* Reset the number of items in the array. Does not reset real array size.
*/
template<class T>
inline void cArray<T>::ClearCount(void)
{
	m_Count = 0;
}

template<class T>
inline void cArray<T>::Init(void)
{
	m_Array = NULL;
	m_Count = 0;
	m_Size = 0;
}

template<class T>
inline void cArray<T>::Init(char* mem, unsigned int size)
{
	m_Array = (T*)mem;
	m_Count = 0;
	m_Size = size;
}


template<class T>
inline void cArray<T>::SetCount(unsigned int k)
{
	m_Count = k;
}

template<class T>
inline void cArray<T>::SetSize(unsigned int k)
{
	m_Size = k;
}

template<class T>
inline void cArray<T>::Resize(const unsigned int Size, const bool Move)
{
	m_Resize(Size, Move);
}


template<class T>
inline void cArray<T>::Resize(const unsigned int Size, const unsigned int Count)
{
	m_Resize(Size, false);
	m_Count = Count < m_Size? Count: m_Size;
}


/**
* Rewrite this array by array in 'Array' argument. It's memory safe, therefore it expand this array if it is necessary
* \param Array Source data
* \param Count The size of the source data
*/
template<class T>
void cArray<T>::Move(const T* Array, const unsigned int Count)
{
	if (m_Size <= Count)
	{
		m_Resize(Count, false);
	}
	
	if (Count == 1)
	{
		m_Array[0] = Array[0];
	}
	else
	{
		memcpy(m_Array, Array, sizeof(T) * Count);
	}
	m_Count = Count;
}

/**
* Append the block of data to the array. It does not append it at the end, but on 'Position'. 
* After this method the length of the array is 'Count' + 'Position'.
* It is more general version of 'Move' method.
* It's memory safe, therefore it expand this array if it is necessary.
* \param Array Source data
* \param Position The position where the data should coupied
* \param Count The size of the 'Array'
*/
template<class T>
void cArray<T>::Append(const T* Array, const unsigned int Position, const unsigned int Count)
{
	if (m_Size <= Count + Position)
	{
		m_Resize(Count + Position, false);
	}

	if (Count == 1)
	{
		m_Array[Position] = Array[0];
	}
	else
	{
		memcpy(&m_Array[Position], Array, sizeof(T) * Count);
	}
	m_Count = Count + Position;
}

/**
* Move the whole block of the memory. It increases the count of items by Count 
* and create empty space in the middle of the array of size Count. The method doesn't resize the array by itself.
* You should check the size of the array before calling this method and resize it if it is necessary.
* \param From The first item the move will start from
* \param Count Number of empty items
*/
template<class T>
void cArray<T>::Shift(unsigned int From, unsigned int Count)
{
	assert(From < m_Count);
 	assert(Count + m_Count < m_Size);

	memmove(&m_Array[From + Count], &m_Array[From], sizeof(T) * (m_Count - From));
	m_Count += Count;
}

/**
* Move the whole block of the memory to the left, therefore it delete 'Count' items from the array.
* \param From It points into the first item the move will start from. It's the first item which is not deleted. Variable 'From' cannot be smaller than 'Count'.
* \param Count How many items will be deleted. The length of the move.
*/
template<class T>
void cArray<T>::ShiftLeft(unsigned int From, unsigned int Count)
{
	assert(From >= Count);

	if (m_Count - From > 0)
	{
		memmove(&m_Array[From - Count], &m_Array[From], sizeof(T) * (m_Count - From));
	}
	m_Count -= Count;
}


/**
* Move the block of the memory in the array to the left. It does not decrease the size of the array at all.
* The moved block is specified by the range <From, To>. 
* \param From The first item the move will start from
* \param To Last moved item
* \param Count Number of items which will be replaced. Items lyies left from the item on the 'From' position.
*/
template<class T>
void cArray<T>::ShiftBlockLeft(unsigned int From, unsigned int To, unsigned int Count)
{
	assert(From >= Count);

	if (m_Count - From > 0)
	{
		memmove(&m_Array[From - Count], &m_Array[From], sizeof(T) * (To - From));
	}
}

/**
* Move the block of the memory in the array to the right. It does not alter the size of the array at all.
* The moved block is specified by the range <From, To>. 
* \param From The first item the move will start from
* \param To Last moved item
* \param Count Number of items which will be replaced. Items lyies right from the item on the 'To' position.
*/
template<class T>
void cArray<T>::ShiftBlockRight(unsigned int From, unsigned int To, unsigned int Count)
{
	assert(From >= Count);

	if (m_Count - From > 0)
	{
		memmove(&m_Array[From + Count], &m_Array[From], sizeof(T) * (To - From));
	}
}

template<class T>
void cArray<T>::Add(const T* Array, const unsigned int Count)
{
	if (m_Count + Count > m_Size)
	{
		m_Resize(m_Count + Count, true);
	}
	if (Count == 1)
	{
		m_Array[m_Count++] = Array[0];
	}
	else
	{
		memcpy((char *)&m_Array[m_Count], Array, sizeof(T) * Count);
				m_Count += Count;
	}
}

template<class T>
inline void cArray<T>::Add(const T& Value)
{
	if (m_Count + 1 > m_Size)
	{
		m_Resize(m_Count + 1, true);
	}
	
	m_Array[m_Count++] = Value;
}

/**
* Double the array size if the count exceeds the size
*/
template<class T>
void cArray<T>::AddDouble(const T& Value)
{
	if (m_Count + 1 > m_Size)
	{
		if (m_Count > MAXIMAL_INCREASE)
		{
			m_Resize(m_Count + MAXIMAL_INCREASE, true);
		} else
		{
			m_Resize(m_Count * 2, true);
		}
	}
	
	m_Array[m_Count++] = Value;
}


template<class T>
inline void cArray<T>::FastAdd(const T* Array, const unsigned int Count)
{
	//if (m_Count + Count > m_Size)
	//{
	//	m_Resize(m_Count + Count, true);
	//}
	if (Count == 1)
	{
		m_Array[m_Count++] = Array[0];
	}
	else
	{
		memcpy((char *)&m_Array[m_Count], Array, sizeof(T) * Count);
		m_Count += Count;
	}
}

template<class T>
inline void cArray<T>::Left(const unsigned int Count)
{
	m_Count = Count < m_Count ? Count : m_Count;
}


template<class T>
inline void cArray<T>::Right(const unsigned int Count)
{
	int tmp = (m_Count < Count? 0: m_Count - Count);
	m_Count = (m_Count < Count? m_Count: Count); 
	memmove(m_Array, (char* )&m_Array[tmp], sizeof(T) * m_Count);
}


template<class T>
inline void cArray<T>::Mid(const unsigned int Left, const unsigned int Right)
{
	if (Right >= Left && m_Count >= Right - Left)
	{
		m_Count = Right - Left;
		memmove(m_Array, (char* )&m_Array[Left], sizeof(T) * m_Count);
	}
}



template<class T>
inline void cArray<T>::Append(const T& Value)
{
	if (!(m_Count < m_Size))
	{
		m_Resize(m_Size + 1, true);
	}
	m_Array[m_Count] = Value;
}


template<class T>
inline T* cArray<T>::GetArray(const unsigned int Index)
{
	return &m_Array[Index];
}

template<class T>
inline T* cArray<T>::GetArray() const // mk
{
	return m_Array;
}

template<class T>
void cArray<T>::Fill(const char ch, const unsigned int Count)
{
	m_Resize(Count, false);
	memset(m_Array, ch, Count);
	m_Count = Count;
}

// Dangerous methods, mk.
template<class T>
void cArray<T>::SetMem(char* mem)
{
	m_Array = (T*)mem;
}

template<class T>
void cArray<T>::m_Resize(const unsigned int Size, const bool Move)
{
	if (Size > m_Size)
	{
		// mk: m_Size = (Size & 7) ? Size + (8 - (Size & 7)): Size;
		m_Size = Size;
		T *auxPtr = new T[m_Size];
		if (m_ResetFlag)
		{
			memset((void *)auxPtr, 0, sizeof(T) * m_Size);
		}
		if (m_Array != NULL)
		{
			if (Move)
			{
				memcpy(auxPtr, m_Array, sizeof(T) * m_Count);
			}
			delete [] m_Array;
		}
		m_Array = auxPtr;
		if (m_Count > m_Size) //pm
		{
			m_Count = m_Size;
		}
	} 
	else if (Size == 0)
	{
		if (m_Array != NULL)
		{
			delete [] m_Array;
			m_Array = NULL;
		}
		m_Size = m_Count = 0;
	}
}

/**
 * Use allocated memory for allocation. !!! Realocation doesn't run. !!!
 */
template<class T>
inline void cArray<T>::Resize(const unsigned int size, char* memory)
{
	m_Array = (T*)memory;
	m_Size = size;
}

template<class T>
inline unsigned int cArray<T>::GetItemSize()
{
	return sizeof(T);
}

/**
* Sort the array by bubble sort algorithm
*/
template<class T>
void cArray<T>::BubbleSort()
{
	bool end;
	T help_variable;

	for (unsigned int i = 1; i < m_Count; i++)
	{
		end = true;
		for (unsigned int j = 0; j < m_Count - i; j++)
		{
			if (m_Array[j] > m_Array[j + 1])
			{
				help_variable = m_Array[j];
				m_Array[j] = m_Array[j + 1];
				m_Array[j + 1] = help_variable;
				end = false;
			}
		}
		if (end) 
		{
			break;
		}
	}
}

/**
* Sort the array by bubble sort algorithm
*/
template<class T>
void cArray<T>::QSortUInt()
{
	// qsort(m_Array, m_Count, sizeof(T), compare3);
}

#endif            //    __CARRAY_H__
