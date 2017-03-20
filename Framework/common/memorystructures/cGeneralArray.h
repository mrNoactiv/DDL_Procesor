/**
*	\file cGeneralArray.h
*	\author Radim Baca
*	\version 0.1
*	\date jan 2008
*	\brief Main memory array, which can be parametrized by any cBasictType
*/

#ifndef __cGeneralArray_h__
#define __cGeneralArray_h__

#include <stdlib.h>
#include <string.h>
#include <assert.h> 

#include "common/datatype/cBasicType.h"
#include "common/datatype/cDTDescriptor.h"

#pragma warning(push)
#pragma warning(disable : 4710)


/**
*	Main memory array. Compared to the cArray it is more general type, however for elementar types
* can be cArray faster than this array. Particulary the Resize() methods will be definitely faster 
* in the case of cArray.
*
* Template parameters:
*		- ItemType - Class inherited from cBasicType
*
*	\author Radim Baca
*	\version 0.1
*	\date jan 2008
**/
template<class ItemType>
class cGeneralArray
{

	//cSizeInfo<ItemType>* m_SizeInfo;
	cDTDescriptor* m_Descr;
	unsigned int m_Size;
	unsigned int m_Count;
	ItemType* m_Array;

	void m_Resize(const unsigned int Size, const bool Move);

public:
	cGeneralArray(cDTDescriptor* desc);
	~cGeneralArray();

	const cGeneralArray<ItemType>& operator = (const cGeneralArray& other);
	inline ItemType& operator[](const int Index)								{ return m_Array[Index]; }
	inline ItemType& operator[](const unsigned int Index)						{ return m_Array[Index]; }
	inline const ItemType operator[](const int Index) const						{ return m_Array[Index]; }
	inline const ItemType operator[](const unsigned int Index) const			{ return m_Array[Index]; }
	inline operator ItemType* (void)											{ return m_Array; }
	inline unsigned int Size(void) const;
	inline unsigned int Count(void) const;

	inline void Clear(void);
	inline void ClearCount(void);
	inline void SetCount(unsigned int k);
	inline void Resize(const unsigned int Size, const bool Move = false);
	inline void Resize(const unsigned int Size, const unsigned int Count);

	inline unsigned int GetItemSize();
	inline void SetMem(char* mem);
	//void Move(const ItemType* Array, const unsigned int Count = 1);
	//void Append(const ItemType* Array, const unsigned int Position, const unsigned int Count = 1);
	//inline void Shift(unsigned int From, unsigned int Count = 1);
	inline void ShiftLeft(unsigned int From, unsigned int Count = 1);
	void Add(const ItemType* Array, const unsigned int Count = 1);
	inline void Add(const ItemType& Array);
	inline void AddDouble(const ItemType& Value);
	inline void Left(const unsigned int Count);
	//inline void Right(const unsigned int Count);
	//inline void Mid(const unsigned int Left, const unsigned int Right);
	inline void Append(const ItemType& Value);
	inline ItemType* GetArray(const unsigned int Index = 0)						{ return &m_Array[Index]; }
	inline ItemType* GetArray() const											{ return m_Array; }
	void Fill(const ItemType ch, const unsigned int Count);
	void BubbleSort();

	inline ItemType& GetRefItem(unsigned int order) const						{ return m_Array[order]; }
	inline ItemType* GetItem(unsigned int order)								{ return &(m_Array[order]); }
	inline ItemType& GetRefLastItem() const										{ return m_Array[Count() - 1]; }
	inline ItemType* GetLastItem()												{ return &(m_Array[Count() - 1]); }

	//inline cSizeInfo<ItemType>* GetSizeInfo()									{ return m_SizeInfo; }

};

template<class ItemType>
cGeneralArray<ItemType>::cGeneralArray(cDTDescriptor* descr)
	: m_Descr(descr),  m_Size(0), m_Count(0), m_Array(NULL)	
{
}

template<class ItemType>
cGeneralArray<ItemType>::~cGeneralArray()
{
	m_Resize(0, false);
}


template<class ItemType>
inline const cGeneralArray<ItemType>& cGeneralArray<ItemType>::operator = (const cGeneralArray& other)
{
	if (this != &other)
	{
		Move((ItemType*)((*(const_cast<cGeneralArray<ItemType>*>(&other)))->GetArray()), other.Count());
	}
	return *this;
}

//
//template<class ItemType>
//inline (ItemType::ItemType)& cGeneralArray<ItemType>::operator[](const int Index)
//{
//	assert(Index < (int)m_Count);
//	return m_Array[Index];
//}

//
//template<class ItemType>
//inline ItemType& cGeneralArray<ItemType>::operator[](const unsigned int Index)
//{
//	assert(Index < (int)m_Count);
//	return m_Array[Index];
//}

//
//template<class ItemType>
//inline const ItemType cGeneralArray<ItemType>::operator[](const int Index) const
//{
//	assert(Index < (int)m_Count);
//	return m_Array[Index];
//}
//
//
//template<class ItemType>
//inline const ItemType cGeneralArray<ItemType>::operator[](const unsigned int Index) const
//{
//	assert(Index < (int)m_Count);
//	return m_Array[Index];
//}

//template<class ItemType>
//inline ItemType& cGeneralArray<ItemType>::GetRefItem(unsigned int order) const
//{
//	assert(order < (int)m_Count);
//	return m_Array[order];
//}
//
//template<class ItemType>
//inline ItemType* cGeneralArray<ItemType>::GetItem(unsigned int order)
//{
//	assert(order < (int)m_Count);
//	return &(m_Array[order]);
//}
//
//template<class ItemType>
//inline cGeneralArray<ItemType>::operator ItemType* (void)
//{
//	return m_Array;
//}


template<class ItemType>
inline unsigned int cGeneralArray<ItemType>::Size(void) const
{
	return m_Size;
}


template<class ItemType>
inline unsigned int cGeneralArray<ItemType>::Count(void) const
{
	return m_Count;
}


template<class ItemType>
inline void cGeneralArray<ItemType>::Clear(void)
{
	m_Resize(0, false);
}


/// Reset the number of items in the array. Does not reset real array size.
template<class ItemType>
inline void cGeneralArray<ItemType>::ClearCount(void)
{
	m_Count = 0;
}

template<class ItemType>
inline void cGeneralArray<ItemType>::SetCount(unsigned int k)
{
	m_Count = k;
}


template<class ItemType>
inline void cGeneralArray<ItemType>::Resize(const unsigned int Size, const bool Move)
{
	m_Resize(Size, Move);
}


template<class ItemType>
inline void cGeneralArray<ItemType>::Resize(const unsigned int Size, const unsigned int Count)
{
	m_Resize(Size, false);
	m_Count = Count < m_Size? Count: m_Size;
}

// TODO finish all these methods

//
///// Rewrite this array by array in 'Array' argument. It's memory safe, therefore it expand this array if it is necessary
///// \param Array Source data
///// \param Count The size of the source data
//template<class ItemType>
//void cGeneralArray<ItemType>::Move(const ItemType* Array, const unsigned int Count)
//{
//	if (m_Size <= Count)
//	{
//		m_Resize(Count, false);
//	}
//	
//	if (Count == 1)
//	{
//		m_Array[0] = Array[0];
//	}
//	else
//	{
//		memcpy(m_Array, Array, sizeof(ItemType) * Count);
//	}
//	m_Count = Count;
//}
//
///// Append the block of data to the array. It does not append it at the end, but on 'Position'. 
///// After this method the length of the array is 'Count' + 'Position'.
///// It is more general version of 'Move' method.
///// It's memory safe, therefore it expand this array if it is necessary.
///// \param Array Source data
///// \param Position The position where the data should coupied
///// \param Count The size of the 'Array'
//template<class ItemType>
//void cGeneralArray<ItemType>::Append(const ItemType* Array, const unsigned int Position, const unsigned int Count = 1)
//{
//	if (m_Size <= Count + Position)
//	{
//		m_Resize(Count + Position, false);
//	}
//
//	if (Count == 1)
//	{
//		m_Array[Position] = Array[0];
//	}
//	else
//	{
//		memcpy(&m_Array[Position], Array, sizeof(ItemType) * Count);
//	}
//	m_Count = Count + Position;
//}
//
///// Move the whole block of the memory. It increases the count of items by Count 
///// and create empty space in the middle of the array of size Count. The method doesn't resize the array by itself.
///// You should check the size of the array before calling this method and resize it if it is necessary.
///// \param From The first item the move will start from
///// \param Count Number of empty items
//template<class ItemType>
//void cGeneralArray<ItemType>::Shift(unsigned int From, unsigned int Count = 1)
//{
//	assert(From < m_Count);
//	assert(Count + m_Count < m_Size);
//
//	memmove(&m_Array[From + Count], &m_Array[From], sizeof(ItemType) * (m_Count - From));
//	m_Count += Count;
//}
//

/// Move the whole block of the memory to the left, therefore it delete 'Count' items from the array.
/// \param From It points into the first item the move will start from. It's the first item which is not deleted. Variable 'From' cannot be smaller than 'Count'.
/// \param Count How many items will be deleted. The length of the move.
template<class ItemType>
void cGeneralArray<ItemType>::ShiftLeft(unsigned int From, unsigned int Count = 1)
{
	assert(From >= Count);

	if (m_Count - From > 0)
	{
		//memmove(&m_Array[From - Count], &m_Array[From], sizeof(ItemType) * (m_Count - From));
		//ItemType::MoveBlock(&m_Array[From - Count], &m_Array[From], m_Count - From, *m_SizeInfo);
		printf("cGeneralArray<ItemType>::ShiftLeft - error, not implemented!");
	}
	m_Count -= Count;
}

template<class ItemType>
void cGeneralArray<ItemType>::Add(const ItemType* Array, const unsigned int Count)
{
	if (m_Count + Count > m_Size)
	{
		m_Resize(m_Count + Count, true);
	}
	if (Count == 1)
	{
		ItemType::Copy(m_Array[m_Count++], m_Array[0]);
	}
	else
	{
		for (int i = m_Count; i < m_Count + Count; i++)
		{
			ItemType::Copy(m_Array[m_Count], Array[i - m_Count]);
		}
		m_Count += Count;
	}
}

template<class ItemType>
void cGeneralArray<ItemType>::Add(const ItemType& Value)
{
	if (m_Count + 1 > m_Size)
	{
		m_Resize(m_Count + 1, true);
	}
	
	ItemType::Copy(m_Array[m_Count++], Value);
}

template<class ItemType>
void cGeneralArray<ItemType>::AddDouble(const ItemType& Value)
{
	if (m_Count + 1 > m_Size)
	{
		m_Resize(m_Count * 2, true);
	}
	
	ItemType::Copy(m_Array[m_Count++], Value);
}

template<class ItemType>
inline void cGeneralArray<ItemType>::Left(const unsigned int Count)
{
	m_Count = Count < m_Count ? Count : m_Count;
}


//template<class ItemType>
//inline void cGeneralArray<ItemType>::Right(const unsigned int Count)
//{
//	int tmp = (m_Count < Count? 0: m_Count - Count);
//	m_Count = (m_Count < Count? m_Count: Count); 
//	memmove(m_Array, (char* )&m_Array[tmp], sizeof(ItemType) * m_Count);
//}
//
//
//template<class ItemType>
//inline void cGeneralArray<ItemType>::Mid(const unsigned int Left, const unsigned int Right)
//{
//	if (Right >= Left && m_Count >= Right - Left)
//	{
//		m_Count = Right - Left;
//		memmove(m_Array, (char* )&m_Array[Left], sizeof(ItemType) * m_Count);
//	}
//}


/// Replace the item which is right behind the last item in the array
/// \param Value item which added into the item in the array
template<class ItemType>
inline void cGeneralArray<ItemType>::Append(const ItemType& Value)
{
	if (!(m_Count < m_Size))
	{
		m_Resize(m_Size + 1, true);
	}
	ItemType::Copy(m_Array[m_Count], Value);
}

//
//template<class ItemType>
//inline ItemType* cGeneralArray<ItemType>::GetArray(const unsigned int Index)
//{
//	return &m_Array[Index];
//}
//
//template<class ItemType>
//inline ItemType* cGeneralArray<ItemType>::GetArray() const // mk
//{
//	return m_Array;
//}

/// Fill the array with the same item
/// \param ch Item which should be inserted into every item of the array
/// \param count Number of the items which should set with 'ch' item
template<class ItemType>
void cGeneralArray<ItemType>::Fill(const ItemType ch, const unsigned int Count)
{
	m_Resize(Count, false);
	for (unsigned int i = 0; i < Count; i++)
	{
		ItemType::Copy(m_Array[i], ch);
	}
	m_Count = Count;
}

/// This method set the pointer of m_Array. You should do it very caerfully, because you are in fact losing pointer the old.
/// You should also take care about the type
template<class ItemType>
void cGeneralArray<ItemType>::SetMem(char* mem)
{
	m_Array = (ItemType*)mem;
}

template<class ItemType>
void cGeneralArray<ItemType>::m_Resize(const unsigned int Size, const bool Move)
{
	// TODO resize s pouzitim cMemory
	if (Size > m_Size)
	{
		// mk: m_Size = (Size & 7) ? Size + (8 - (Size & 7)): Size;
		m_Size = Size;
		ItemType *auxPtr = new ItemType[m_Size];
		for (unsigned int i = 0; i < m_Size; i++)
		{
			auxPtr[i].Resize(m_Descr);
			//printf("cGeneralArray<ItemType>::m_Resize - error, not implemented!");
		}
		if (m_Array != NULL)
		{
			if (Move)
			{
				for (unsigned int i = 0; i < m_Count; i++)
				{
					ItemType::Copy((char*)&auxPtr[i], (char*)&m_Array[i], m_Descr);
				}
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

template<class ItemType>
inline unsigned int cGeneralArray<ItemType>::GetItemSize()
{
	//return m_SizeInfo->GetSize();
	printf("cGeneralArray<ItemType>::GetItemSize - error, not implemented!");
	return -1;
}

/// Sort the array by bubble sort algorithm
template<class ItemType>
void cGeneralArray<ItemType>::BubbleSort()
{
	printf("cGeneralArray<ItemType>::BubbleSort - error, not implemented!");
	//bool end;
	//ItemType help_variable;

	//for (unsigned int i = 1; i < m_Count; i++)
	//{
	//	end = true;
	//	for (unsigned int j = 0; j < m_Count - i; j++)
	//	{
	//		if (m_Array[j] > m_Array[j + 1])
	//		{
	//			ItemType::Copy(help_variable, m_Array[j]);
	//			ItemType::Copy(m_Array[j], m_Array[j + 1]);
	//			ItemType::Copy(m_Array[j + 1], help_variable);
	//			end = false;
	//		}
	//	}
	//	if (end) 
	//	{
	//		break;
	//	}
	//}
}

#pragma warning(pop)

#endif            //    __CARRAY_H__
