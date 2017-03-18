/**
*	\file cGeneralLinkedLists.h
*	\author Radim Baca
*	\version 0.1
*	\date apr 2008
*	\brief Main memory structure simulating linked lists. Designed for more complex classes.
*/


#ifndef __cGeneralLinkedLists_h__
#define __cGeneralLinkedLists_h__

#include "cArray.h"
#include "cFactoryAbstract.h"

/**
* This class hold in the main memory items linked together by pointers. Can contain number of linked lists.
* This class is for example used for blocking the results in cTwigStack and two items can even point to 
* the same item (is that still linked list?).
* Class has to be parametrized by item inherited from cBasicType<Item>. The expected type of Item is some more complex class.
*
*	\author Radim Baca
*	\version 0.1
*	\date apr 2008
**/
template<class TItemType>
class cGeneralLinkedLists
{
	typedef typename TItemType::Type ItemType;

	cFactoryAbstract<TItemType>* mFactory;
	cArray<ItemType*>*			mItem;					/// Contains items in the linked lists.
	cArray<unsigned int>*		mPointer;				/// Pointers to the next item in the list. Corespond to the items in the mItem array.

public:
	static const unsigned int EMPTY_POINTER = 0xffffffff;

	cGeneralLinkedLists(cFactoryAbstract<TItemType>* factory);
	~cGeneralLinkedLists();

	void Init(cFactoryAbstract<TItemType>* factory);
	void Delete();
	void Clear();

	inline unsigned int AddItem(ItemType* item);
	inline ItemType* GetItem(unsigned int pointer)								{ return mItem->GetRefItem(pointer); }
	inline void SetPointer(unsigned int itemPosition, unsigned int pointer);
	inline unsigned int GetPointer(unsigned int itemPosition)					{ return mPointer->GetRefItem(itemPosition); }
	inline unsigned int Count()		{ return mItem->Count(); }


};

/// Class constructor
template<class TItemType>
cGeneralLinkedLists<TItemType>::cGeneralLinkedLists(cFactoryAbstract<TItemType> *factory):mFactory(NULL), mItem(NULL), mPointer(NULL)
{
	Init(factory);
}

/// Destructor
template<class TItemType>
cGeneralLinkedLists<TItemType>::~cGeneralLinkedLists()
{
	Delete();
}

/// Delete all arrays
template<class TItemType>
void cGeneralLinkedLists<TItemType>::Delete()
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
void cGeneralLinkedLists<TItemType>::Init(cFactoryAbstract<TItemType> *factory)
{
	Delete();

	mFactory = factory;
	mItem = new cArray<ItemType*>();
	mItem->Resize(20);
	mPointer = new cArray<unsigned int>();
	mPointer->Resize(20);
}

/// Reset all counts in the arrays. Reset also the factory.
template<class TItemType>
void cGeneralLinkedLists<TItemType>::Clear()
{
	mFactory->Clear();
	mItem->ClearCount();
	mPointer->ClearCount();
}

/// Add new item into the linked list.
/// \return Unique position of the item, which can be used as a pointer.
template<class TItemType>
unsigned int cGeneralLinkedLists<TItemType>::AddItem(ItemType *item)
{
	ItemType *newItem = mFactory->GetNext();
	*newItem = *item;
	mItem->AddDouble(newItem);
	mPointer->AddDouble(EMPTY_POINTER);
	return mItem->Count() - 1;
}

/// Set pointer of the item which is on the 'itemPosition' position.
/// \param itemPosition Order of the source item.
/// \param pointer Order of the destination item.
template<class TItemType>
void cGeneralLinkedLists<TItemType>::SetPointer(unsigned int itemPosition, unsigned int pointer)
{
	assert(itemPosition < mItem->Count());
	assert(mItem->Count() == mPointer->Count());

	*mPointer->GetItem(itemPosition) = pointer;
}

#endif