/**
*	\file cMemoryBlock.h
*	\author Radim Baca
*	\version 0.1
*	\date dec 2010
*	\brief One block of memory
*/


#ifndef __cMemoryBlock_h__
#define __cMemoryBlock_h__

#include "assert.h"
#include "common/stream/cStream.h"

/**
*	One block of memory
*
*	\author Radim Baca
*	\version 0.1
*	\date dec 2010
**/
class cMemoryBlock
{
	char* mBlock;
	unsigned int mUsedSize;
	unsigned int mMaxSize;
	unsigned int mCacheOrder;		/// specify the order of the char block within the cache array.
	unsigned int mIndex;
	unsigned int mHeaderId;
	
public:
	cMemoryBlock(unsigned int size, unsigned int order);
	~cMemoryBlock();

	inline void Clear(bool leaveHeaderId = false);

	inline char* GetMemory(unsigned int size);
	inline char* GetChar();
	inline unsigned int GetCacheOrder();
	inline void SetIndex(unsigned int index);
	inline unsigned int GetIndex() const;
	inline void SetHeaderId(unsigned int id);
	inline unsigned int GetHeaderId() const;

};

inline unsigned int cMemoryBlock::GetCacheOrder()
{
	return mCacheOrder;
}

/**
* \param size Amount of the memory requested.
* \return Free memory from this block.
*/
char* cMemoryBlock::GetMemory(unsigned int size)
{
	char *ret = mBlock + mUsedSize;
	mUsedSize += size;
	assert(mUsedSize < mMaxSize);
	return ret;
}

/**
* \return Begining of this memory block
*/
char* cMemoryBlock::GetChar()
{
	return mBlock;
}

void cMemoryBlock::Clear(bool leaveHeaderId)
{
	mUsedSize = 0;
	if (!leaveHeaderId)
	{
		mHeaderId = (unsigned int)~0;
	}
}

void cMemoryBlock::SetIndex(unsigned int index)
{ 
	mIndex = index; 
}

unsigned int cMemoryBlock::GetIndex() const
{ 
	return mIndex; 
}

void cMemoryBlock::SetHeaderId(unsigned int id)
{
	mHeaderId = id;
}

inline unsigned int cMemoryBlock::GetHeaderId() const
{
	return mHeaderId;
}

#endif