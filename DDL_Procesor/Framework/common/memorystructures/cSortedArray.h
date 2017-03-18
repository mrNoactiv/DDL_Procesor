/**
*	\file cSortedArray.h
*	\author Radim Baca
*	\version 0.1
*	\date sep 2007
*	\brief Main memory array, which is always sorted.
*/

#ifndef __cSortedArray_h__
#define __cSortedArray_h__

#include "common/stream/cStream.h"
#include "common/memorystructures/cArray.h"
#include "common/datatype/cDTDescriptor.h"

using namespace common::datatype;

/**
*	Sorted main memory array. Items in the array are always sorted. Array has only keys no leaf items.
* Template parameters:
*		- ItemType - class inherited from cBasicType.
*
*	\author Radim Baca
*	\version 0.1
*	\date sep 2007
**/
template<class ItemType>
class cSortedArray
{
protected:

	cArray<ItemType> *mArray;
	cArray<unsigned int> *mOrder;
	bool mDuplicates;
	static const int DEFAULT_CAPACITY = 200;
public:
	cSortedArray(cDTDescriptor* descr, bool duplicates = false, unsigned int size = DEFAULT_CAPACITY);
	~cSortedArray();

	virtual inline void Clear();
	virtual void Init(cDTDescriptor* descr, unsigned int size = DEFAULT_CAPACITY);
	virtual void Resize(unsigned int size);
	inline unsigned int GetItemCount() const;

	bool Insert(const ItemType &item, unsigned int &order);
	bool Insert(const ItemType &item);
	bool Update(unsigned int order, const ItemType &item);
	bool BinnarySearch(const ItemType &item);

	inline ItemType* GetSortedItem(unsigned int order) const 
	{ 
		assert(order < mOrder->Count()); 
		return mArray->GetItem((*mOrder)[order]); 
	}
	inline ItemType& GetRefSortedItem(unsigned int order) const 
	{
		//unsigned int count = mOrder->Count();
		assert(order < mOrder->Count()); 
		return mArray->GetRefItem((*mOrder)[order]); 
	}
	inline ItemType* GetItem(unsigned int order) const	
	{ 
		return mArray->GetItem(order); 
	}
	inline ItemType& GetRefItem(unsigned int order) const	
	{ 
		return mArray->GetRefItem(order); 
	}
	inline void SetItem(unsigned int order, const ItemType &item);

	void Print() const;
	void PrintSorted() const;
	void PrintInfo() const;
};

/// Constructor
template<class ItemType>
cSortedArray<ItemType>::cSortedArray(cDTDescriptor* descr, bool duplicates, unsigned int size)
	:mDuplicates(duplicates)
{
	Init(descr, size);
}

/// Destructor
template<class ItemType>
cSortedArray<ItemType>::~cSortedArray()
{
	delete mArray;
	delete mOrder;
}

template<class ItemType>
void cSortedArray<ItemType>::Init(cDTDescriptor* descr, unsigned int size)
{
	mOrder = new cArray<unsigned int>();
	mArray = new cArray<ItemType>(descr);
	Resize(size);
}

template<class ItemType>
void cSortedArray<ItemType>::Resize(unsigned int size)
{
	mArray->Resize(size);
	mOrder->Resize(size);
}

/// Update value of the existing item in the array
/// \param order Order of the item in the sortet array
/// \param item New value of the item
/// \return 
///		- true if new item was updated
///		- false if the item already exist and duplications are not alowed
template<class ItemType>
bool cSortedArray<ItemType>::Update(unsigned int order, const ItemType &item)
{
	//int ret;
	//int mid = 0;
	//int lo = 0;
	//int hi = (int)mOrder->Count() - 1;
	//int newPosition;

	//if ((ret = ItemType::Compare(GetRefSortedItem(mArray->Count() - 1), item)) < 0)
	//{
	//	newPosition = mArray->Count() - 1;
	//} 
	//else if (ret == 0)
	//{
	//	if (!mDuplicates)
	//	{
	//		return false;
	//	}
	//	newPosition = mArray->Count() - 1;
	//} else
	//{
	//	do
	//	{
	//		mid = (lo + hi) / 2;

	//		if ((ret =  ItemType::Compare(GetRefSortedItem(mid), item)) > 0)
	//		{
	//			hi = mid-1;
	//		}
	//		else if (!mDuplicates && ret == 0)
	//		{
	//			order = mid;
	//			return false;
	//		} 
	//		else
	//		{
	//			lo = mid+1;

	//			if (lo > hi)
	//			{
	//				mid++;
	//			}
	//		}
	//	}
	//	while(lo <= hi);

	//	if (mid < (int)mOrder->Count())
	//	{
	//		if ( ItemType::Compare(GetRefSortedItem(mid), item) == 0) 
	//		{
	//			if (!mDuplicates)
	//			{
	//				return false;
	//			}
	//			newPosition = mid;
	//		} 
	//	}
	//	
	//	if (mid == (int)mOrder->Count())
	//	{
	//		newPosition = mArray->Count() - 1;
	//	} else
	//	{
	//		newPosition = mid;
	//	}
	//}	
	//ItemType::Copy(GetRefSortedItem(order), item);
	//if (order != newPosition)
	//{
	//	if (order > newPosition)
	//	{
	//		mOrder->ShiftBlockRight(newPosition, order - 1);
	//	} else
	//	{
	//		mOrder->ShiftBlockLeft(order + 1, newPosition);
	//	}
	//	*(mOrder->GetItem(newPosition)) = order;
	//}

	return true;
}

/// Insert item into the array
/// \param item New item to be inserted
/// \param order Return position of item in array
/// \return 
///		- true if new item was inserted
///		- false if the item already exist and duplications are not alowed
template<class ItemType>
bool cSortedArray<ItemType>::Insert(const ItemType &item, unsigned int &order)
{
	//int ret;
	//int mid = 0;
	//int lo = 0;
	//int hi = (int)mOrder->Count() - 1;

	//if (!mOrder->Count())
	//{
	//	mArray->Add(item);
	//	mOrder->Add(mArray->Count() - 1);
	//	order = mArray->Count() - 1;
	//	return true;
	//} else
	//{
	//	if (mOrder->Count() + 1 == mOrder->Size())
	//	{
	//		mOrder->Resize(mOrder->Size() * 2, true);
	//		mArray->Resize(mOrder->Size() * 2, true);
	//	}

	//	if ((ret = ItemType::Compare(GetRefSortedItem(mArray->Count() - 1), item)) < 0)
	//	{
	//		mArray->Add(item);
	//		mOrder->Add(mArray->Count() - 1);
	//		order = mArray->Count() - 1;
	//		return true;
	//	} 
	//	else if (!mDuplicates && ret == 0)
	//	{
	//		order = mArray->Count() - 1;
	//		return false;
	//	}
	//	//hi--;

	//	do
	//	{
	//		mid = (lo + hi) / 2;

	//		if ((ret =  ItemType::Compare(GetRefSortedItem(mid), item)) > 0)
	//		{
	//			hi = mid-1;
	//		}
	//		else if (!mDuplicates && ret == 0)
	//		{
	//			order = mid;
	//			return false;
	//		} 
	//		else
	//		{
	//			lo = mid+1;

	//			if (lo > hi)
	//			{
	//				mid++;
	//			}
	//		}
	//	}
	//	while(lo <= hi);

	//	if (mid < (int)mOrder->Count())
	//	{
	//		if ( ItemType::Compare(GetRefSortedItem(mid), item) == 0) 
	//		{
	//			order = mid;
	//			return false;
	//		} 
	//		mOrder->Shift(mid);
	//	}
	//	
	//	if (mid == (int)mOrder->Count())
	//	{
	//		mArray->Add(item);
	//		mOrder->Add(mArray->Count() - 1);
	//		order = mArray->Count() - 1;
	//	} else
	//	{
	//		mArray->Add(item);
	//		(*mOrder)[mid] = mArray->Count() - 1;
	//		order = mid;
	//	}
	//}

	return true;
}

/// Insert item into the array
/// \param item New item to be inserted
/// \return 
///		- true if new item was inserted
///		- false if the item already exist and duplications are not alowed
template<class ItemType>
bool cSortedArray<ItemType>::Insert(const ItemType &item)
{
	//int ret;
	//int mid = 0;
	//int lo = 0;
	//int hi = (int)mOrder->Count() - 1;

	//if (!mOrder->Count())
	//{
	//	mArray->Add(item);
	//	mOrder->Add(mArray->Count() - 1);
	//	return true;
	//} else
	//{
	//	if (mOrder->Count() + 1 == mOrder->Size())
	//	{
	//		mOrder->Resize(mOrder->Size() * 2, true);
	//		mArray->Resize(mOrder->Size() * 2, true);
	//	}

	//	if ((ret = ItemType::Compare(GetRefSortedItem(mArray->Count() - 1), item)) < 0)
	//	{
	//		mArray->Add(item);
	//		mOrder->Add(mArray->Count() - 1);
	//		return true;
	//	} 
	//	else if (!mDuplicates && ret == 0)
	//	{
	//		return false;
	//	}
	//	//hi--;

	//	do
	//	{
	//		mid = (lo + hi) / 2;

	//		if ((ret =  ItemType::Compare(GetRefSortedItem(mid), item)) > 0)
	//		{
	//			hi = mid-1;
	//		}
	//		else if (!mDuplicates && ret == 0)
	//		{
	//			return false;
	//		} 
	//		else
	//		{
	//			lo = mid+1;

	//			if (lo > hi)
	//			{
	//				mid++;
	//			}
	//		}
	//	}
	//	while(lo <= hi);

	//	if (mid < (int)mOrder->Count())
	//	{
	//		if ( ItemType::Compare(GetRefSortedItem(mid), item) == 0) 
	//		{
	//			return false;
	//		} 
	//		mOrder->Shift(mid);
	//	}
	//	
	//	if (mid == (int)mOrder->Count())
	//	{
	//		mArray->Add(item);
	//		mOrder->Add(mArray->Count() - 1);
	//	} else
	//	{
	//		mArray->Add(item);
	//		(*mOrder)[mid] = mArray->Count() - 1;
	//	}
	//}

	return true;
}

/// Perform binnary search in a sorted array.
/// \param item The item which is searched in the array
/// \return
///		- true If we find the item in the array
///		- false If the item is not in the array
template<class ItemType>
bool cSortedArray<ItemType>::BinnarySearch(const ItemType &item)
{
	//int mid = 0;
	//int lo = 0;
	//int hi = (int)mOrder->Count() - 1;
	//int ret;

	//if (mOrder->Count() > 0)
	//{
	//	do
	//	{
	//		mid = (lo + hi) / 2;

	//		if ((ret =  ItemType::Compare(GetRefSortedItem(mid), item)) > 0)
	//		{
	//			hi = mid - 1;
	//		} else if (ret == 0)
	//		{
	//			return true;
	//		} else
	//		{
	//			lo = mid + 1;
	//		}
	//	} while(lo <= hi);
	//}
	return false;
}

/// Print
template<class ItemType>
void cSortedArray<ItemType>::PrintSorted() const
{
	//for (unsigned int i = 0; i < mArray->Count(); i++)
	//{
	//	ItemType::Print("\n", GetRefSortedItem(i));
	//}
}

template<class ItemType>
void cSortedArray<ItemType>::SetItem(unsigned int order, const ItemType &item)
{
	mArray[order] = item;
}

/// \return number of items in a result
template<class ItemType>
unsigned int cSortedArray<ItemType>::GetItemCount() const
{
	return mArray->Count();
}

/// Clear all values
template<class ItemType>
void cSortedArray<ItemType>::Clear()
{
	mOrder->ClearCount();
	mArray->ClearCount();
}

/// Print
template<class ItemType>
void cSortedArray<ItemType>::Print() const
{
	//for (unsigned int i = 0; i < mArray->Count(); i++)
	//{
	//	ItemType::Print("\n", *(GetItem(i)));
	//}
}

template<class ItemType>
void cSortedArray<ItemType>::PrintInfo() const
{
	//printf("\n******************************   Dewey Node Array   **************************\n");
	//printf(" Number of items: %d",mArray->Count());
}

#endif