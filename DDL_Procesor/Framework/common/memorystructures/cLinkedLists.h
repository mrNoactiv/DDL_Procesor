/**
*	\file cLinkedLists.h
*	\author Radim Baca
*	\version 0.1
*	\date apr 2008
*	\brief Main memory structure simulating linked lists. Designed for elementar types like integers.
*/


#ifndef __cLinkedLists_h__
#define __cLinkedLists_h__

#include "cArray.h"

/**
* This class hold in the main memory items linked together by pointers. Can contain number of linked lists.
* This class is for example used for blocking the results in cTwigStack and two items can even point to 
* the same item (is that still linked list?).
* Class has to be parametrized by item inherited from cBasicType<Item>. The expected type of is some elementar type.
* This class does NOT resize item, due to that fact types like cTreeTupleType can not be the parameter!
*
*	\author Radim Baca
*	\version 0.1
*	\date apr 2008
**/
template<class TItemType>
class cLinkedLists
{
	typedef typename TItemType::Type ItemType;

	cArray<ItemType>*			mItem;					/// Contains items in the linked lists.
	cArray<unsigned int>*		mPointer;				/// Pointers to the next item in the list. Corespond to the items in the mItem array.

public:
	static const unsigned int EMPTY_POINTER = 0xffffffff;

	cLinkedLists();
	~cLinkedLists();

	void Init();
	void Delete();
	void Clear();

	inline unsigned int AddItem(ItemType& item);
	inline unsigned int AddItem(ItemType& item, unsigned int pointer);
	inline ItemType* GetItem(unsigned int pointer)								{ return mItem->GetItem(pointer); }
	inline void SetPointer(unsigned int itemPosition, unsigned int pointer);
	inline unsigned int GetPointer(unsigned int itemPosition)					{ return mPointer->GetRefItem(itemPosition); }
	inline unsigned int Count()		{ return mItem->Count(); }

};

/// Class constructor
template<class TItemType>
cLinkedLists<TItemType>::cLinkedLists(): mItem(NULL), mPointer(NULL)
{
	Init();
}

/// Destructor
template<class TItemType>
cLinkedLists<TItemType>::~cLinkedLists()
{
	Delete();
}

/// Delete all arrays
template<class TItemType>
void cLinkedLists<TItemType>::Delete()
{
	if (mItem != NULL)
	{
		delete mItem;
		mItem = NULL;
	}
	if (mPointer != NULL)
	{
		delete mPointer;
		mPointer = NULL;
	}
}

/// Delete and create all arrays
template<class TItemType>
void cLinkedLists<TItemType>::Init()
{
	Delete();

	mItem = new cArray<ItemType>();
	mItem->Resize(20);
	mPointer = new cArray<unsigned int>();
	mPointer->Resize(20);
}

/// Reset all counts in the arrays. Reset also the factory.
template<class TItemType>
void cLinkedLists<TItemType>::Clear()
{
	mItem->ClearCount();
	mPointer->ClearCount();
}

/// Add new item into the linked list.
/// \return Unique position of the item, which can be used as a pointer.
template<class TItemType>
unsigned int cLinkedLists<TItemType>::AddItem(ItemType& item)
{
	mItem->AddDouble(item);
	mPointer->AddDouble(EMPTY_POINTER);
	return mItem->Count() - 1;
}

/// Add new item into the linked list and bound the item with item specified by pointer
/// \return Unique position of the item, which can be used as a pointer.
template<class TItemType>
unsigned int cLinkedLists<TItemType>::AddItem(ItemType& item, unsigned int pointer)
{
	mItem->AddDouble(item);
	mPointer->AddDouble(pointer);
	return mItem->Count() - 1;
}

/// Set pointer of the item which is on the 'itemPosition' position.
/// \param itemPosition Order of the source item.
/// \param pointer Order of the destination item.
template<class TItemType>
void cLinkedLists<TItemType>::SetPointer(unsigned int itemPosition, unsigned int pointer)
{
	assert(itemPosition < mItem->Count());
	assert(mItem->Count() == mPointer->Count());

	*mPointer->GetItem(itemPosition) = pointer;
}

#endif