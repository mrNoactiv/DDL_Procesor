/**
*	\file cGeneralQueue.h
*	\author Radim Baca
*	\version 0.1
*	\date jan 2008
*	\brief Queue parametrized by class inheriting from cBasicType
*/

#ifndef __cGeneralQueue_h__
#define __cGeneralQueue_h__

#include <stdio.h>
#include <assert.h>
#include "cSizeInfo.h"
#include "cGeneralArray.h"

/**
*	Queue parametrized by class inheriting from cBasicType.
*
*	\author Radim Baca
*	\version 0.1
*	\date jan 2008
**/
template <class TItemType>
class cGeneralQueue
{
	typedef typename TItemType::Type ItemType;

	cGeneralArray<TItemType>*	mArray;
	unsigned int				mStart;
	int							mEnd;

	static const unsigned int DEFAULT_SIZE = 500;	/// Default starting size of the array.
	static const unsigned int INCREASE = 20;		/// Incease used when the buffer is not large enough.
public:
	cGeneralQueue(cSizeInfo<ItemType> *sizeInfo, const int size = DEFAULT_SIZE);
	~cGeneralQueue();

	void Delete();
	void Init(cSizeInfo<ItemType> *sizeInfo, const int size);
	inline void Clear();

	inline ItemType* Head()				{ assert(Count() > 0); return mArray->GetItem(mStart); }
	inline ItemType* Get()				{ unsigned  int actual = mStart; assert(Count() > 0); if (++mStart == mArray->Count()) mStart = 0; return mArray->GetItem(actual); }
	inline bool Empty()					{ return mStart == mEnd; }
	inline void Put(const ItemType& item);

	inline unsigned int Count()			{ if (mEnd >= mStart) return mEnd - mStart; else return mArray->Count() - mStart + mEnd; }
};


/// constructor
/// \param Size maximum size of the stack
template <class TItemType>
cGeneralQueue<TItemType>::cGeneralQueue(cSizeInfo<ItemType> *sizeInfo, const int size)
	:mArray(NULL)
{
	Init(sizeInfo, size);
}

/// destructor
template <class TItemType>
cGeneralQueue<TItemType>::~cGeneralQueue()
{
	Delete();
}

/// Function free heap memory used by this object.
template <class TItemType>
void cGeneralQueue<TItemType>::Delete()
{
	if (mArray != NULL)
	{
		delete mArray;
	}
}

/// Set default values.
template <class TItemType>
void cGeneralQueue<TItemType>::Clear()
{
	mStart = 0;
	mEnd = 0;
}

/// Initialize array inside queue.
template <class TItemType>
void cGeneralQueue<TItemType>::Init(cSizeInfo<ItemType> *sizeInfo, const int size)
{
	Delete();

	mArray = new cGeneralArray<TItemType>(sizeInfo);
	mArray->Resize(size);
	mArray->SetCount(size);
	mStart = 0;
	mEnd = 0;
}

/// Put item into the queue. If the queue is not large enough, the array is extended.
/// \param item New item.
template <class TItemType>
void cGeneralQueue<TItemType>::Put(const ItemType& item)
{
	*mArray->GetItem(mEnd) = item;
	if ((mStart == 0 && mEnd + 1 == mArray->Count()) || mEnd + 1 == mStart)
	{
		printf("cGeneralQueue::Put - array need to be extended. Who is gona implement this? :)\n");
	} else if (++mEnd == mArray->Count())
	{
		mEnd = 0;
	}
}

#endif