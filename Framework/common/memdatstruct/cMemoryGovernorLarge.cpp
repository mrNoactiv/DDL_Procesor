#include "common/memdatstruct/cMemoryGovernorLarge.h"

using namespace common::memdatstruct;

cMemoryGovernorLarge::cMemoryGovernorLarge(cMemoryManager* memoryManager)
{
	Null();
	Init(memoryManager);
}

cMemoryGovernorLarge::~cMemoryGovernorLarge()
{
	Delete();
}

void cMemoryGovernorLarge::Null()
{
	mActualMemoryBlock = mFirstMemoryBlock = NULL;
    mStatArray = NULL;
	mBookmarkStack = NULL;
}

void cMemoryGovernorLarge::Delete()
{
	cMemoryBlock* nextblock;
	mActualMemoryBlock = mFirstMemoryBlock;
	while (mActualMemoryBlock != NULL)
	{
		nextblock = mActualMemoryBlock->GetPrevious();
		mMemoryManager->ReleaseMem(mActualMemoryBlock);
		mActualMemoryBlock = nextblock;
	}
	mActualMemoryBlock = mFirstMemoryBlock = NULL;

	if (mStatArray != NULL)
	{
        delete[] mStatArray;
		mStatArray = NULL;
	}

	if (mBookmarkStack != NULL)
	{
		delete[] mBookmarkStack;
		mBookmarkStack = NULL;
	}

}

void cMemoryGovernorLarge::Init(cMemoryManager* memoryManager)
{
	Delete();

	mMemoryManager = memoryManager;
	mActualMemoryBlock = mFirstMemoryBlock = memoryManager->GetMemSystem();
	mActualMemoryBlock->SetPrevious(NULL);
    mActualBlockOrder = 0;
    mActualStatLine = 0;
    mFirstRound = true;
    mActualSum = 0;
    mStatArray = new unsigned int[LINE_MASK + 1];
    for (unsigned int i = 0; i <= LINE_MASK; i++)
	{
        mStatArray[i] = 0;
	}
	mBookmarkCount = 10;
	mBookmarkStack = new sBookmark[mBookmarkCount];
	mAvailableBookmark = 0;
}






