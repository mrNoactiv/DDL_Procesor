#include "cMemoryBlock.h"

cMemoryBlock::cMemoryBlock(unsigned int size, unsigned int order)
{
	mMaxSize = size;
	mBlock = new char[size];
	mCacheOrder = order;

	Clear();
}

cMemoryBlock::~cMemoryBlock()
{
	delete mBlock;
}
