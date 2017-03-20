/*
 * \class cMemoryManagerCuda
 *
 * \brief Class is responsible for storing nodes on GPU.
 *
 * \author Pavel Bednář
 * \date September 2014
 */

#ifdef CUDA_ENABLED

#include "dstruct/paged/cuda/cMemoryManagerCuda.h"

cMemoryManagerCuda::cMemoryManagerCuda()
{

}
cMemoryManagerCuda::~cMemoryManagerCuda()
{
	if (mNodeRecordStorage != NULL)
	{
		delete mNodeRecordStorage;
		mNodeRecordStorage = NULL;
	}
}

#endif
