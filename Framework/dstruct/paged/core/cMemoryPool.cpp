#include "cMemoryPool.h"

namespace dstruct {
  namespace paged {
	namespace core {

cMemoryPool::cMemoryPool(unsigned int size)
{
	mSize = size;
	mData = new char[size];
	mAllocTable = new sAllocTableRecord[ALLOCTABLE_SIZE];
	mMaxIndexInAllocTable = 0;
	mCurrentIndex = 0;
}

cMemoryPool::~cMemoryPool()
{
	if (mData != NULL)
	{
		delete mData;
	}
	if (mAllocTable != NULL)
	{
		delete mAllocTable;
	}
}
}}}