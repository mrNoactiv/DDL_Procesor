#pragma once

#include "common/memdatstruct/cMemoryManager.h"
#include "common/memdatstruct/cMemoryBlock.h"

namespace common {
	namespace memdatstruct {


/**
*	Implement a queue ADT. This class allocate blocks from memory manager.
*	It is capable to easily allocate new blocks if the queue is full.
*	Interface is oriented on a regular classes implementing the cBasicType interface, therefore,
*	it has char* as an input of enqueue.
*
*	Example:
*	cMemoryManager* manager = new cMemoryManager();	
*	cQueue<cUInt>* queue = new cQueue<cUInt>(manager, NULL);
*	unsigned int num = 1;
*	queue->Enqueue((const char*)&num);
*	memcpy(&num, queue->Dequeue(), sizeof(int));
*	delete queue;
*
*
*	\author Radim Baca
*	\version 0.1
*	\date jul 2013
**/
struct sQueueNode
{
	cMemoryBlock * this_block;
	sQueueNode* next;
	unsigned int count;
};

template <class T>
class cQueue
{
private:
	cMemoryManager *mMemoryManager;
	unsigned int	mBlockSize_indicator;
	unsigned int	mBlockCount;

	sQueueNode*		mHeadQueueBlock;
	char*			mHeadMem;
	unsigned int	mHeadPos;

	sQueueNode*		mTailQueueBlock;
	char*			mTailMem;
	unsigned int	mTailPos;

	bool			mIsEmpty;
	unsigned int	mMemSize;
	cDTDescriptor*	mDesc; 

	unsigned int	mCount;

	sQueueNode* NewQueueNode();

public:
	cQueue(cMemoryManager * mmanager, cDTDescriptor* desc, unsigned int block_size = cMemoryManager::SMALL_SIZE);
	~cQueue(void);

	void Enqueue(const char* item);
	char* Dequeue();
	char* FrontItem();
	void Clear();
	bool IsEmpty();
	unsigned int GetCount() { return mCount; }
};


/**
* \param mmanager Memory manager that will be used by this class 
* \param desc Descriptor for the T data type, can be NULL 
* \param block_size Size of the block
*/
template<class T>
cQueue<T>::cQueue(cMemoryManager * mmanager, cDTDescriptor* desc, unsigned int block_size)
{
	mMemoryManager = mmanager;
	mMemoryManager = mmanager;
	mDesc = desc;
	mBlockSize_indicator  = block_size;

	sQueueNode* block1 = NewQueueNode();
	sQueueNode* block2 = NewQueueNode();
	if (mBlockSize_indicator == cMemoryManager::SMALL_SIZE)
	{
		mMemSize = mMemoryManager->GetSize_SMALL();
	} else if (mBlockSize_indicator == cMemoryManager::BIG_SIZE)
	{
		mMemSize = mMemoryManager->GetSize_BIG();
	} else if (mBlockSize_indicator == cMemoryManager::SYSTEM_SIZE)
	{
		mMemSize = mMemoryManager->GetSize_SYSTEM();
	}

	block1->next = block2;
	block2->next = block1;

	mHeadQueueBlock = block1;
	mHeadMem = mHeadQueueBlock->this_block->GetMem();
	mHeadPos = 0;

	mTailQueueBlock = block1;
	mTailMem = mTailQueueBlock->this_block->GetMem();
	mTailPos = 0;

	mIsEmpty = true;
	mBlockCount = 2;

	mCount = 0;
}

/**
* Destructor which release all blocks allocated from memory manager
*/
template<class T>
cQueue<T>::~cQueue()
{
	sQueueNode* qnode = mHeadQueueBlock;
	for (unsigned int i = 0; i < mBlockCount; i++)
	{
		mMemoryManager->ReleaseMem(qnode->this_block);
		sQueueNode* nnode = qnode->next;
		delete qnode;
		qnode = nnode;
	}
}

/**
* Read a new block from the memory manager
*/
template<class T>
sQueueNode* cQueue<T>::NewQueueNode()
{
	sQueueNode* block = new sQueueNode();
	block->count = 0;
	if (mBlockSize_indicator == cMemoryManager::SMALL_SIZE)
	{
		block->this_block = mMemoryManager->GetMemSmall();
	} else if (mBlockSize_indicator == cMemoryManager::BIG_SIZE)
	{
		block->this_block = mMemoryManager->GetMemBig();
	} else if (mBlockSize_indicator == cMemoryManager::SYSTEM_SIZE)
	{
		block->this_block = mMemoryManager->GetMemSystem();
	}
	return block;
}

template<class T>
void cQueue<T>::Enqueue(const char* item)
{
	if (T::GetSize(item, mDesc) > mMemSize - mTailPos)
	{
		if (mTailQueueBlock->next->count > 0)
		{
			// we need to allocate new blocks
			sQueueNode* block = NewQueueNode();
			sQueueNode* next = mTailQueueBlock->next;
			mTailQueueBlock->next = block;
			block->next = next;
			mBlockCount++;
		}

		// switch tail to a new block
		mTailQueueBlock = mTailQueueBlock->next;
		mTailMem = mTailQueueBlock->this_block->GetMem();
		T::Copy(mTailMem, item, mDesc);
		mTailPos = T::GetSize(item, mDesc);
		mTailQueueBlock->count = 1;
		if (mIsEmpty)
		{
			mHeadQueueBlock = mTailQueueBlock;
			mHeadMem = mHeadQueueBlock->this_block->GetMem();
			mHeadPos = 0;
		}
		mIsEmpty = false;
	} else
	{
		// if there is enough space
		T::Copy(mTailMem + mTailPos, item, mDesc);
		mTailPos += T::GetSize(item, mDesc);
		mTailQueueBlock->count++;
		mIsEmpty = false;
	}
	mCount++;
}


template<class T>
char* cQueue<T>::Dequeue()
{
	char* mem = mHeadMem + mHeadPos;
	assert(mHeadQueueBlock->count > 0 && !mIsEmpty);
	mHeadQueueBlock->count--;
	mHeadPos += T::GetSize(mem, mDesc);
	if (mHeadQueueBlock->count == 0)
	{
		if (mHeadQueueBlock == mTailQueueBlock)
		{
			mIsEmpty = true;
		} else
		{
			// we need to switch to another block
			mHeadQueueBlock = mHeadQueueBlock->next;
			mHeadMem = mHeadQueueBlock->this_block->GetMem();
			mHeadPos = 0;
		}
	}
	mCount--;
	return mem;
}


template<class T>
char* cQueue<T>::FrontItem()
{
	assert(mHeadQueueBlock->count > 0 && !mIsEmpty);
	return mHeadMem + mHeadPos;
}

/**
* \return true if the queue is empty
*/
template<class T>
bool cQueue<T>::IsEmpty()
{
	return mIsEmpty;
}

/**
* Set the queue as an empty
*/
template<class T>
void cQueue<T>::Clear()
{
	mHeadPos = 0;
	mHeadQueueBlock->count = 0;
	mTailQueueBlock = mHeadQueueBlock;
	mTailMem = mTailQueueBlock->this_block->GetMem();
	mTailPos = 0;
	mIsEmpty = true;
	mCount = 0;
}

}}
