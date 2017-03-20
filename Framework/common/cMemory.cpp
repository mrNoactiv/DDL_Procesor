#include "common/cMemory.h"

/// Constructor
/// \param block_capacity Set the block capacity. It is provided in bytes. 100kB - 1000kB.
/// \param block_count Number of blocks in the memory array.
cMemory::cMemory(unsigned int block_capacity , unsigned int block_count): mMemory(NULL)
{
	mMemory = new char*[block_count];
	mCurrentBlock = (unsigned int)-1;
	mBlockCount = block_count;
	mCapacity = block_capacity;
	mAllocatedBlockCount = 0;
	NewMemoryBlock();
}

cMemory::~cMemory()
{
	if (mMemory != NULL)
	{
		for (unsigned int i = 0; i < mAllocatedBlockCount; i++)
		{
			delete[] mMemory[i];
		}
		delete[] mMemory;
		mSize = 0;
		mCapacity = 0;
		mCurrentBlock = (unsigned int)-1;
		mAllocatedBlockCount = 0;
	}
}

/**
 * Return pointer to memory, enlarge size of usage memory to size.
 */
char* cMemory::GetMemory(unsigned int size)
{
	char *mem;
	if (mSize + size < mCapacity)
	{
		mem = mMemory[mCurrentBlock] + mSize;
		mSize += size;
	}
	else
	{
		if (!NewMemoryBlock())
		{
			mem = NULL;
		}
		else
		{
			mem = GetMemory(size);
		}
	}

	assert(mem != NULL);

	return mem;
}

/**
 * Allocate new block of memory.
 */
bool cMemory::NewMemoryBlock()
{
	mCurrentBlock++;
	if (mCurrentBlock >= mBlockCount)
	{
		// TODO: resize array
		return false;
	}

	if (mCurrentBlock >= mAllocatedBlockCount)
	{
		mMemory[mCurrentBlock] = new char[mCapacity];
		memset(mMemory[mCurrentBlock], NULL, mCapacity);
		mAllocatedBlockCount++;
	}
	mSize = 0;
	return true;
}