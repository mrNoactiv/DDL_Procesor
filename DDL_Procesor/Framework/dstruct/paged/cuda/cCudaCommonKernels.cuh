/*!
* \class cCudaProcessor35
*
* \brief Common CUDA kernels
*
* \author Pavel Bednar
* \date 2015-02-02
*/
#ifndef __cCudaCommonKernels_cuh__
#define __cCudaCommonKernels_cuh__
#include "dstruct/paged/cuda/cCudaGlobalDefs.cuh"
#include "dstruct/paged/cuda/cCudaParams.h"
#include "common/cCommon.h"

using namespace common;

inline __host__ unsigned int getNumberOfParts(const unsigned int totalSize, const unsigned int partSize)
{
	unsigned int tmp = totalSize / partSize;
	if ((totalSize % partSize) != 0)
		tmp++;
	return tmp;
}


inline __global__ void Kernel_ClearResultList(cCudaParams params)
{
	params.D_ResultList[0] = 0;
	//printf("\nGPU result list cleared.");
}

inline __device__ bool Device_ComputeThreadResult(bool* pResults, uint &threadOffset, ushort &dimension)
{
	bool threadResult = true;
	for (int i = 0; i < dimension; i++)
	{
		threadResult &= pResults[threadOffset + i];
	}
	return threadResult;
}

inline __device__ void Device_AddToResultList(uint newIndex, uint* D_ResultList, uint* D_ChildIndices, uint &nodeCapacity, uint &bucketOrder, bool &isInner, uint &itemOrder)
{

	uint childIndex = D_ChildIndices[bucketOrder*nodeCapacity + itemOrder];
	if (isInner)
	{
		D_ResultList[newIndex + 1] = childIndex;
		//printf("\n\tChild index:; %d; newIndex: %d, block: %d, bucketOrder: %d", childIndex, newIndex + 1,blockIdx.x,bucketOrder);
	}
	else
	{
		int bitIndex = /*cGpuConst::MEMORY_LOCATION == cGpuMainMemoryLocation::Gpu ? childIndex << 8 :*/ blockIdx.x << 8;
		D_ResultList[newIndex + 1] = bitIndex | itemOrder;
		//printf( "\nChild index:; %d; value: %d", cGpuConst::MEMORY_LOCATION == cGpuMainMemoryLocation::Gpu ? childIndex : blockIdx.x, bitIndex | childIndex, newIndex + 1);
	}
}
#endif