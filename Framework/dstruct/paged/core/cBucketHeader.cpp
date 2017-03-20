#include "dstruct/paged/core/cBucketHeader.h"

namespace dstruct {
  namespace paged {
	namespace core {

cBucketHeader::cBucketHeader()
{
#ifdef CUDA_ENABLED
	mGpuId=-1;
	mGpuItemOrder = -1;
#endif
}

void cBucketHeader::Clear(unsigned int bucketOrder)
{
	mReadLock = 0;
	mWriteLock = false;
	mModified = false;
	mNodeIndex = UINT_MAX;
	mBucketOrder = bucketOrder;
#ifdef CUDA_ENABLED
	mGpuId=-1;
	mGpuItemOrder = -1;
#endif
}
}}}