#ifndef __cMemory_h__
#define __cMemory_h__

#include "common/memorystructures/cArray.h"

class cMemory
{
private:
	char **mMemory;
	unsigned int mSize;
	unsigned int mCapacity;				/// Size of the one block in the mMemory array
	unsigned int mBlockCount;			/// Size of the mMemory array
	unsigned int mAllocatedBlockCount;	/// Number of allocated blocks
	unsigned int mCurrentBlock;

private:
	bool NewMemoryBlock();

public:
	static const unsigned int DEFAULT_BLOCK_NUMBER = 2048;
	static const unsigned int DEFAULT_BLOCK_CAPACITY = 1048576;

	cMemory(unsigned int block_capacity = DEFAULT_BLOCK_CAPACITY, unsigned int block_count = DEFAULT_BLOCK_NUMBER);
	~cMemory();

	char* GetMemory(unsigned int size);

	inline void Clear() { mCurrentBlock = 0; mSize = 0; }
	inline void Free(unsigned int size);

	inline unsigned int GetMemoryUsage() const		{ return mCurrentBlock * DEFAULT_BLOCK_CAPACITY + mSize; }
	inline unsigned int GetMemoryCapacity() const	{ return mCapacity; }
	inline unsigned int GetMemoryBlockCount() const	{ return mBlockCount; }
};  // cMemory

/// Can free memory allocated during last GetMemory call.
/// \param size Have to be the same with the last GetMemory call.
void cMemory::Free(unsigned int size)
{
	assert(size > mSize);

	mSize -= size;
}

#endif  // __cMemory_h__
