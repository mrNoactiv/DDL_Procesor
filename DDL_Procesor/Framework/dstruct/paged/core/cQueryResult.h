/**
*	\file cQueryResult.h
*	\author Michal Krátký
*	\version 0.1
*	\date feb 2002
*	\version 0.2
*	\date jul 2011
*	\brief Result - collection of tuples
*/

#ifndef __cQueryResult_h__
#define __cQueryResult_h__

#include "cObject.h"
//#include "common/datatype/tuple/cTuple.h"

namespace dstruct {
  namespace paged {
	namespace core {

/**
*	Result - collection of tuples
*
*	\author Michal Krátký
*	\version 0.2
*	\date jul 2011
**/
template<class TLeafItem> 
class cQueryResult
{
private:
	TLeafItem *mItems;
	unsigned int mCapacity;
	unsigned int mSize;

	static const unsigned int DEFAULT_CAPACITY = 1024;

public:
	cQueryResult(unsigned int capacity=cObject::NONDEFINED);
	~cQueryResult();

	void Resize(const cTreeHeader *header);
	inline void Clear();
	inline unsigned int GetSize() const;
	inline unsigned int GetCapacity() const;
	inline void AddItem(const TLeafItem &item);
	inline void ReplaceItem(unsigned int index, TLeafItem &item);
	inline TLeafItem* GetItem(unsigned int index) const;
	bool Check(cQueryResult &result);
};

/**
 * Create the array of leaf items.
 */
template <class TLeafItem> 
cQueryResult<TLeafItem>::cQueryResult(unsigned int capacity)
{
	if (capacity == cObject::NONDEFINED)
	{
		capacity = DEFAULT_CAPACITY;
	}
	mItems = new TLeafItem[capacity];
	mCapacity = capacity;
	mSize = 0;
}

template <class TLeafItem> cQueryResult<TLeafItem>::~cQueryResult()
{
	if (mItems != NULL)
	{
		delete []mItems;
	}
}

/**
 * Resize of items from result.
 */
template <class TLeafItem> 
void cQueryResult<TLeafItem>::Resize(const cTreeHeader *header)
{
	if (mCapacity > 0 && mItems[0].GetSize() != header->GetLeafNodeItemSize())
	{
		for (unsigned int i = 0 ; i < mCapacity ; i++)
		{
			mItems[i].Resize(header);
		}
	}
}

template <class TLeafItem> inline void cQueryResult<TLeafItem>::Clear()
{
	mSize = 0;
}

template <class TLeafItem>
inline void cQueryResult<TLeafItem>::AddItem(const TLeafItem &item)
{ 
	if (mSize < mCapacity)
	{
		mItems[mSize] = item;
	}
	mSize++;
}

template <class TLeafItem>
inline void cQueryResult<TLeafItem>::ReplaceItem(unsigned int pos, TLeafItem &item)
{ 
	if (pos < mSize)
	{
		mItems[pos] = item;
	}
}

template <class TLeafItem> 
inline TLeafItem* cQueryResult<TLeafItem>::GetItem(unsigned int index) const
{
	TLeafItem *item = NULL;
	if (index < mSize && index < mCapacity)
	{
		item = &mItems[index];
	}
	return item;
}

template <class TLeafItem>
inline unsigned int cQueryResult<TLeafItem>::GetSize() const 
{ 
	return mSize; 
}

template <class TLeafItem>
inline unsigned int cQueryResult<TLeafItem>::GetCapacity() const 
{
	return mCapacity; 
}

/**
 * Check if the two result contain the same items.
 */
template <class TLeafItem>
bool cQueryResult<TLeafItem>::Check(cQueryResult &result)
{
	unsigned int *indexes = NULL, count = 0;   
	bool ret = false, flag = false;

	if (mSize != result.GetSize())
	{
		printf("*** Critical Error: The size isn't the same! ***\n");
	}
	else
	{
		if (mSize > mCapacity)
		{
			printf("Sorry: the size is the same, but size > capacity ... I can't check!\n");
		}
		else
		{
			indexes = new unsigned int[mSize];  // dynamic allocation nevermind here
			for (unsigned int i = 0 ; i < mSize ; i++)
			{
				indexes[i] = (unsigned int)~0;
				for (unsigned int j = 0 ; j < result.GetSize() ; j++)
				{
					flag = false;
					if (GeTLeafItem(i)->Equal(*result.GeTLeafItem(j)) == 0)
					{
						indexes[i] = j;
						flag = true;
						for (unsigned int k = 0 ; k < i ; k++)  // for the more equaled items
						{
							if (indexes[k] == j)
							{
								flag = true;
								indexes[i] = (unsigned int)~0;
							}
						}
						break;
					}
				}

				if (!flag)
				{
					printf("\n%d. item not finded!\n");
					count++;
				}
			}
			if (count == 0)
			{
				printf(" OK\n");
			}
			else
			{
				printf(" Error: %d items not founded!\n", count);
			}
		}
	}

	if (indexes != NULL)
		delete []indexes ;

	return ret;
}
}}}
#endif