/**
*	\file cMemoryPool.h
*	\author Michal Krátký
*	\version 0.1
*	\date jul 2011
*	\brief General pool of paged data structures
*/

#ifndef __cMemoryPool_h__
#define __cMemoryPool_h__

#include <stdlib.h>
#include <stdio.h>

/**
* Pool
*
*	\version 0.1
*	\date 2011
**/
namespace dstruct {
  namespace paged {
	namespace core {

/**
*	General pool of paged data structures
*
*	\author Michal Krátký
*	\version 0.1
*	\date jul 2011
**/

struct sAllocTableRecord
{
	char* mPointer;
	unsigned int mSize;
};

class cMemoryPool
{
private:
	static const unsigned int ALLOCTABLE_SIZE = (unsigned int)10e4;
	static const unsigned int DATA_SIZE = (unsigned int)10e6;

	char* mData;                         // A memory for temporary variables
	sAllocTableRecord* mAllocTable;      // An array of pointers to used blocks
	unsigned int mMaxIndexInAllocTable;  // The highest item of the array used
	unsigned int mSize;                  // The size of the memory
	unsigned int mCurrentIndex;          // A pointer to the lowest unused memory 

public:
	cMemoryPool(unsigned int size = DATA_SIZE);
	~cMemoryPool();

	inline char* GetMem(unsigned int bytes);
	inline void FreeMem(char *mem);
	inline bool IsMemUsed();
	inline unsigned int GetNumberOfObjects();
};

/**
 * Get memory from the pool.
 * \param bytes The size of the memory [B]
 * \return Pointer to the memory
 */
inline char* cMemoryPool::GetMem(unsigned int bytes)
{
	char *mem = mData + mCurrentIndex;

	if ((unsigned int)(mCurrentIndex + bytes) >= mSize)
	{
		printf("Critical Error: cMemoryPool::GetMem(): There is no memory for this allocation!");
	}

	mAllocTable[mMaxIndexInAllocTable].mPointer = mem;
	mAllocTable[mMaxIndexInAllocTable++].mSize = bytes;
	mCurrentIndex += bytes;

	return mem;
}

/**
 * Free memory in the pool. This memory is not returned to Operating System, pool only 
 * knows that this memory is not used.
 *
 * \param mem Pointer to the memory which will be free
 */
inline void cMemoryPool::FreeMem(char *mem)
{
	for (unsigned int i = 0 ; i < mMaxIndexInAllocTable ; i++)
	{
		if (mAllocTable[i].mPointer == mem)
		{
			mAllocTable[i].mPointer = NULL;
			if (i == mMaxIndexInAllocTable-1)
			{
				// find the minimal item of the array with an allocated memory
				while(mMaxIndexInAllocTable > 0 && mAllocTable[mMaxIndexInAllocTable-1].mPointer == NULL)
				{
					mMaxIndexInAllocTable--;
				}

				// It is also necessary to change the pointer to the lowest unused memory
				if (mMaxIndexInAllocTable == 0)
				{
					mCurrentIndex = 0;
				}
				else
				{
					mCurrentIndex = (mAllocTable[mMaxIndexInAllocTable - 1].mPointer - mData) +  mAllocTable[mMaxIndexInAllocTable - 1].mSize;
				}
			}
			break;
		}
	}
}

inline bool cMemoryPool::IsMemUsed()
{
	return mMaxIndexInAllocTable != 0;
}

inline unsigned int cMemoryPool::GetNumberOfObjects()
{
	return mMaxIndexInAllocTable;
}

}}}
#endif