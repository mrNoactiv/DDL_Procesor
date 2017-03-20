/**
*	\file cSortedArrayWithLeaf.h
*	\author Radim Baca
*	\version 0.1
*	\date sep 2007
*	\brief Main memory array, which is always sorted. 
* This class extend the cSortedArray and store also leaf information.
*/

#ifndef __cSortedArrayWithLeaf_h__
#define __cSortedArrayWithLeaf_h__

#include "common/memorystructures/cSortedArray.h"
#include "common/memorystructures/cArray.h"



/**
*	Sorted main memory array. Items in the array are always sorted. 
* This class extend the cSortedArray and store also leaf information. Type of the leaf item is unsigned int.
* Template parameters:
*		- KeyType - class inherited from cBasicType. Type of the key item.
*		- LeafType - class inherited from cBasicType. Type of the leaf item.
*
*	\author Radim Baca
*	\version 0.1
*	\date sep 2007
**/
template<class KeyType, class LeafType>
class cSortedArrayWithLeaf: public cSortedArray<KeyType>
{
private:
	cArray<LeafType>*		mLeafs;

public:
	cSortedArrayWithLeaf(cDTDescriptor* keyDescriptor, cDTDescriptor* leafDescriptor, bool duplicates = false, unsigned int size = cSortedArray<KeyType>::DEFAULT_CAPACITY);
	~cSortedArrayWithLeaf();

	virtual inline void Clear();
	virtual void Init(cDTDescriptor* keyDescriptor, cDTDescriptor* leafDescriptor, unsigned int size = cSortedArray<KeyType>::DEFAULT_CAPACITY);
	virtual void Resize(unsigned int size);

	inline LeafType& GetRefLeaf(unsigned int order)	{ return mLeafs->GetRefItem(order); }
	inline LeafType* GetLeaf(unsigned int order)	{ return mLeafs->GetItem(order); }
	inline LeafType* GetSortedLeaf(unsigned int order) const { assert(order < this->mOrder->Count()); return mLeafs->GetItem((*this->mOrder)[order]); }
	inline LeafType& GetRefSortedLeaf(unsigned int order) const { assert(order < this->mOrder->Count()); return mLeafs->GetRefItem((*this->mOrder)[order]); }

	bool Insert(const KeyType &key, const LeafType& leaf, unsigned int &order);
	bool Insert(const KeyType &key, const LeafType& leaf);
	bool BinnarySearch(const KeyType &key, LeafType& leaf);

	void PrintSorted() const;
};

/// Constructor
template<class KeyType, class LeafType>
cSortedArrayWithLeaf<KeyType, LeafType>
	::cSortedArrayWithLeaf(
		cDTDescriptor* keyDescriptor, 
		cDTDescriptor* leafDescriptor, 
		bool duplicates, 
		unsigned int size)
	:cSortedArray<KeyType>(keyDescriptor, duplicates, size)
{
	Init(keyDescriptor, leafDescriptor, size);
}

/// Destructor
template<class KeyType, class LeafType>
cSortedArrayWithLeaf<KeyType, LeafType>::~cSortedArrayWithLeaf()
{
	delete mLeafs;
}	

template<class KeyType, class LeafType>
void cSortedArrayWithLeaf<KeyType, LeafType>::Init(cDTDescriptor* keyDescriptor, cDTDescriptor* leafDescriptor, unsigned int size)
{
	mLeafs = new cArray<LeafType>(leafDescriptor);

	Resize(size);
}

template<class KeyType, class LeafType>
void cSortedArrayWithLeaf<KeyType, LeafType>::Resize(unsigned int size)
{
	cSortedArray<KeyType>::Resize(size);
	mLeafs->Resize(size);
}

template<class KeyType, class LeafType>
void cSortedArrayWithLeaf<KeyType, LeafType>::Clear()
{
	cSortedArray<KeyType>::Clear();
	mLeafs->ClearCount();
}

/// Insert item into the array
/// \param item Key of new item to be inserted
/// \param leaf Leaf item to be inserted
/// \param order Return position of item in array
/// \return 
///		- true if new item was inserted
///		- false if the item already exist and duplications are not alowed
template<class KeyType, class LeafType>
bool cSortedArrayWithLeaf<KeyType, LeafType>::Insert(const KeyType &item, const LeafType& leaf, unsigned int &order)
{
	bool ret = cSortedArray<KeyType>::Insert(item, order);

	if (ret)
	{
		mLeafs->AddDouble(leaf);
	}
	return ret;
}

/// Insert item into the array
/// \param item Key of new item to be inserted
/// \param leaf Leaf item to be inserted
/// \return 
///		- true if new item was inserted
///		- false if the item already exist and duplications are not alowed
template<class KeyType, class LeafType>
bool cSortedArrayWithLeaf<KeyType, LeafType>::Insert(const KeyType &item, const LeafType& leaf)
{
	unsigned int order;
	bool ret = cSortedArray<KeyType>::Insert(item, order);

	if (ret)
	{
		mLeafs->AddDouble(leaf);
	}
	return ret;
}

/// Perform binnary search in a sorted array.
/// \param item The item which is searched in the array
/// \param leaf Return value of an item with this key
/// \return
///		- true If we find the item in the array
///		- false If the item is not in the array
template<class KeyType, class LeafType>
bool cSortedArrayWithLeaf<KeyType, LeafType>::BinnarySearch(const KeyType &item, LeafType& leaf)
{
	int mid = 0;
	int lo = 0;
	int hi = (int)this->mOrder->Count() - 1;
	int ret;

	//leaf = (unsigned int)-1;
	if (this->mOrder->Count() > 0)
	{
		do
		{
			mid = (lo + hi) / 2;

			if ((ret =  KeyType::Compare(GetRefSortedItem(mid), item)) > 0)
			{
				hi = mid - 1;
			} else if (ret == 0)
			{
				leaf = mLeafs->GetRefItem((*this->mOrder)[mid]);
				return true;
			} else
			{
				lo = mid + 1;
			}
		} while(lo <= hi);

	}
	return false;
}


/// Print sorted items
template<class KeyType, class LeafType>
void cSortedArrayWithLeaf<KeyType, LeafType>::PrintSorted() const
{
	for (unsigned int i = 0; i < this->mArray->Count(); i++)
	{
		KeyType::Print("\n", GetRefSortedItem(i));
	}
}
#endif