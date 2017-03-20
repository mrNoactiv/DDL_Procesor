/**************************************************************************}
{                                                                          }
{    cPersistentArray.cpp                                         		     }	
{                                                                          }
{                                                                          }
{    Copyright (c) 2001							        Vaclav Snasel                  }
{    Copyright (c) 2001, 2002				        Michal Kratky                  }
{                                                                          }
{    VERSION: 0.2														DATE 21/11/2002                }
{                                                                          }
{    following functionality:                                              }
{       persistent array                                                   }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      09/12/2002 root node isn't stored                                   }
{                                                                          }
{                                                                          }
{**************************************************************************/

#ifndef __cPersistentArray_h__
#define __cPersistentArray_h__

#include <math.h>
#include <assert.h>

#include "cObject.h"
#include "cStream.h"
#include "cCharStream.h"

template<class TItem> 
class cPersistentArray
{
private:
	bool mDebug;
	bool mOpenFlag;
	TItem* mItems;
	cStream* mStream;
	unsigned int mSize;
	unsigned int mLoIndex;
	int mHiIndex;

public:
	unsigned int mCacheSize;

private:
	bool FlushCache();
	bool ReadItems(unsigned int index);

public:
	static const unsigned int DEFAULT_CACHE_SIZE = 110000;

public:
	cPersistentArray();
	cPersistentArray(unsigned int cacheSize);
	~cPersistentArray();

	bool Create(const char* fileName);
	// bool Open(char* fileName);

	void Close(void);
	void Flush(void);
	TItem* GetItem(unsigned int index);
	TItem& GetRefItem(unsigned int index);
	void SwapItems(unsigned int index1, unsigned int index2);

	// bool SetItem(unsigned int index, const TItem &item);
	bool AddItem(const TItem &item);
	TItem* GetCacheItem(unsigned int index);
	void Clear();

	inline unsigned int GetSize() const;
};

template<class TItem> cPersistentArray<TItem>::cPersistentArray()
{
	mCacheSize = DEFAULT_CACHE_SIZE;
	mItems = new TItem[mCacheSize];
	mOpenFlag = false;
}

template<class TItem> cPersistentArray<TItem>::cPersistentArray(unsigned int cacheSize)
{
	mCacheSize = cacheSize;
	mItems = new TItem[mCacheSize];
	mOpenFlag = false;
}

template<class TItem> cPersistentArray<TItem>::~cPersistentArray()
{
	Close();
	if (mItems != NULL)
	{
		delete []mItems;
		mItems = NULL;
	}
}

/**
 * Create persistent tree.
 */
template<class TItem>
bool cPersistentArray<TItem>::Create(const char *fileName)
{
	mStream = new cIOStream;

	if (!mStream->Open(fileName, CREATE_ALWAYS))
	{
		return false;
	}

	mOpenFlag = true;
	mSize = 0;
	mLoIndex = 0;
	mHiIndex = -1;
	return true;
}

template<class TItem>
void cPersistentArray<TItem>::Close(void)
{
	Flush();

	if (mStream != NULL) 
	{
		if (mOpenFlag)
		{
			mStream->Close();
		}
	}
}

template<class TItem>
bool cPersistentArray<TItem>::AddItem(const TItem &item)
{
	if (!mOpenFlag)
	{
		return false;
	}

	if (mHiIndex + 1 - mLoIndex >= mCacheSize-1) // -1 for special item
	{
		FlushCache();
		mLoIndex = mHiIndex+1;
	}
	mItems[++mHiIndex % (mCacheSize-1)] = item;  // -1 for special item
	mSize++;
	return true;
}

template<class TItem>
bool cPersistentArray<TItem>::FlushCache()
{
	unsigned int index = mItems[0].GetSerialSize() * mLoIndex;
	mStream->Seek(index);
	for (unsigned int i = 0 ; i <= (mHiIndex - mLoIndex) ; i++)
	{
		mItems[i].Write(mStream);
	}
	return true;
}

template<class TItem>
TItem* cPersistentArray<TItem>::GetItem(unsigned int index)
{
	return &GetRefItem(index);
}

template<class TItem>
TItem& cPersistentArray<TItem>::GetRefItem(unsigned int index)
{
	assert(mOpenFlag && index < GetSize());

	if (index < mLoIndex || index > (unsigned int)mHiIndex)
	{
		FlushCache();
		ReadItems(index);
	}
	return mItems[index % (mCacheSize-1)]; // -1 for special item
}

template<class TItem>
bool cPersistentArray<TItem>::ReadItems(unsigned int index)
{
	unsigned int j = mItems[0].GetSerialSize() * index;
	mStream->Seek(j);
	mLoIndex = index;
	mHiIndex = mLoIndex + mCacheSize - 2;  // -1 for special item
	if ((unsigned int)mHiIndex >= mSize)
	{
		mHiIndex = mSize-1;
	}

	for (unsigned int i = 0 ; i <= (mHiIndex - mLoIndex) ; i++)
	{
		mItems[i].Read(mStream);
	}
	return true;
}

/**
 * Flush items onto secondary storage.
 */
template<class TItem> 
void cPersistentArray<TItem>::Flush()
{
}

template<class TItem>
TItem* cPersistentArray<TItem>::GetCacheItem(unsigned int index)
{
	if (index < mCacheSize)
	{
		return &(mItems[index]);
	}
	return NULL;
}

/// Swap items
template<class TItem>
void cPersistentArray<TItem>::SwapItems(unsigned int index1, unsigned int index2)
{
	TItem *item = &(mItems[mCacheSize-1]);
	*item = GetRefItem(index1);

	TItem *item1 = GetItem(index1);
	*item1 = GetRefItem(index2);

	TItem *item2 = GetItem(index2);
	*item2 = *item;
}

template<class TItem> 
inline unsigned int cPersistentArray<TItem>::GetSize() const
{
	return mSize;
}

/**
 * Clear array.
 */
template<class TItem> 
void cPersistentArray<TItem>::Clear()
{
	mSize = mLoIndex = 0;
	mHiIndex = -1;
}
#endif
