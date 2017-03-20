/**
*	\file cMemoryGovernorLarge.h
*	\author Radim Baca
*	\version 0.1
*	\date jun 2011
*	\brief 
*/


#ifndef __cBucketFactory_h__
#define __cBucketFactory_h__

#include "common/memdatstruct/cMemoryManager.h"
#include "common/memdatstruct/cMemoryBlock.h"

#include <assert.h>

namespace common {
	namespace memdatstruct {

		class cMemoryBlock;
		class cMemoryManager;

struct sBookmark
{
	char* pointer;
	cMemoryBlock* blockpointer;
	unsigned int blockorder;
};

/**
* It is a memory manager that works with largest blocks (SYSTEM blocks) in cMemoryManager. 
* It requests necessary blocks according to memory needs during the GetMemory method calls.
* GetMemory method is not synchronized, therefore, cMemoryGovernorLarge has to be used only 
* by one thread.
*
* This class keeps statistics about an average number of used blocks between Clear method calls.
* It remembers last 16 calls. If the memory governor allocate higher number of block than it is 
* ussual during he last 16 calls that this memory is returned to pool during the clear method call.
*
*	\author Radim Baca
*	\version 0.1
*	\date jun 2011
**/
class cMemoryGovernorLarge
{
protected:
	sBookmark* mBookmarkStack;
	unsigned int mAvailableBookmark;
	unsigned int mBookmarkCount;

	cMemoryBlock* mFirstMemoryBlock;		/// First memory block
	cMemoryBlock* mActualMemoryBlock;		/// Actual memory block that we are working with
	cMemoryBlock* mAvgMemoryBlock;			/// We release this block during the clear
	cMemoryManager* mMemoryManager;
	unsigned int mActualBlockOrder;			/// The order of the mActualMemoryBlock in the linked list of blocks


	unsigned int mActualStatLine;			/// We increase this variable during each Clear method call
	unsigned int* mStatArray;				/// Array with statistics. Each number corresponds to the number of blocks used between two Clear method calls
	unsigned int mActualAvg;				/// Actual average number of blocks 
	unsigned int mActualSum;				/// Sumary of numbers in mStatArray
	bool mFirstRound;						/// It is true when we haven't called the Clear method more than 16 times
	static const unsigned int LINE_MASK = 0xf;
	static const unsigned int DIVISION_NUMBER = 4;	/// 2^DIVISION_NUMBER has to be equal to LINE_MASK

public:
	cMemoryGovernorLarge() {}
	cMemoryGovernorLarge(cMemoryManager* memoryManager);
	~cMemoryGovernorLarge();

	void Null();
	void Init(cMemoryManager* memoryManager);
	void Delete();


	inline void Clear();
	inline void ClearToLastBookmark();
	inline void CreateBookmark();
	inline char* GetMemory(unsigned int size);

	inline void PrintGovernorStatistics();
};

/**
* Memory governor returns the memory pointer to the top most bookmark.
*/
void cMemoryGovernorLarge::ClearToLastBookmark()
{
	assert(mAvailableBookmark > 0);

	mAvailableBookmark--;
	mActualMemoryBlock = mBookmarkStack[mAvailableBookmark].blockpointer;
	mActualBlockOrder = mBookmarkStack[mAvailableBookmark].blockorder;
	mActualMemoryBlock->SetMemAddress(mBookmarkStack[mAvailableBookmark].pointer);
}

/**
* This method remembers the current possition and during the next call of ClearToLastBookmark 
* the memory governor returns to the last bookmark remembered by this method.
* We can create stack of bookmarks. Bookmarks are added to the stack by CreateBookmark and
* removed by ClearToLastBookmark.
*/
void cMemoryGovernorLarge::CreateBookmark()
{
	assert(mAvailableBookmark < mBookmarkCount);
	
	mBookmarkStack[mAvailableBookmark].blockpointer = mActualMemoryBlock;
	mBookmarkStack[mAvailableBookmark].pointer = mActualMemoryBlock->GetMem();
	mBookmarkStack[mAvailableBookmark].blockorder = mActualBlockOrder;
	mAvailableBookmark++;
}

/**
* Method set the cursor at the begining of the allocated memory, therefore, it can be allocated again.
* It some block above the average were allocated then they are released.
*/
void cMemoryGovernorLarge::Clear()
{
	unsigned int oldestblockcount = mStatArray[mActualStatLine];
	mStatArray[mActualStatLine] = mActualBlockOrder;
	mActualSum += mActualBlockOrder - oldestblockcount;  // add current block count and decrease the oldest block count

	mActualStatLine = ++mActualStatLine & LINE_MASK;
	if (mActualStatLine == 0)
	{
		mFirstRound = false;
	}
	if (!mFirstRound && mActualBlockOrder > mActualAvg + 2)
	{
		// if the actually allocated number of blocks is higher than average, than the extra blocks are released back to the pool
		cMemoryBlock* firstRedundantBlock = mAvgMemoryBlock->GetPrevious();
		mAvgMemoryBlock->SetPrevious(NULL);
		mMemoryManager->ReleaseLargeMem(firstRedundantBlock, mActualMemoryBlock, mActualBlockOrder - mActualAvg - 2);
	}

	mActualAvg = (mActualSum >> 4) + 1;
	mActualBlockOrder = 0;
	mActualMemoryBlock = mFirstMemoryBlock;
	mFirstMemoryBlock->Clear();
	mAvailableBookmark = 0;
}

/**
* Method returns memory block of a specified size. It automaticaly allocates memory from the pool if necessary.
* \param size The size of a requested memory  block
* \return Memory block of a specified size
*/
char* cMemoryGovernorLarge::GetMemory(unsigned int size)
{
	char* memory;

	assert(size < mMemoryManager->GetSize_SYSTEM());
	if ((memory = mActualMemoryBlock->GetMemory(size)) == NULL)
	{
		if (mActualMemoryBlock->GetPrevious() == NULL)
		{
			cMemoryBlock* block = mMemoryManager->GetMemSystem();
			mActualMemoryBlock->SetPrevious(block);
			block->SetPrevious(NULL);
		} 
		mActualMemoryBlock = mActualMemoryBlock->GetPrevious();
		mActualMemoryBlock->Clear();
		memory = mActualMemoryBlock->GetMemory(size);
		mActualBlockOrder++;
		if (mActualBlockOrder == mActualAvg + 2)
		{
			mAvgMemoryBlock = mActualMemoryBlock;
		}
		assert(memory != NULL);
	}

	return memory;
}

void cMemoryGovernorLarge::PrintGovernorStatistics()
{
	printf("******************* Governor Statistics *******************\n");
	printf("Average number of blocks: %d (%d / %d)\n", mActualAvg, mActualSum, LINE_MASK + 1);

	for (unsigned int i = 0, j = mActualStatLine; i <= LINE_MASK; i++, j = ++j & LINE_MASK)
	{
		printf("Clear %d: %d\n", i, mStatArray[j]);
	}
}

	}}

#endif