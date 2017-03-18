/*!
* \class cCudaProcessor35
*
* \brief Range query processor for CUDA 
*
* \author Pavel Bednar
* \date 2015-01-26
*/
#ifndef __cCudaProcessor_cuh__
#define __cCudaProcessor_cuh__
#include "dstruct/paged/cuda/cCudaGlobalDefs.cuh"
#include "dstruct/paged/cuda/cCudaParams.h"
#include "common/cCommon.h"

using namespace common;

class cCudaProcessor
{
private:
	static sKernelSetting PrepareKernelSettingsLevel(cCudaParams params);
	static sKernelSetting PrepareKernelSettingsSequencial(cCudaParams params);
	static void RangeQuery_LevelSQ(sKernelSetting ks, cCudaParams params);
	static void RangeQuery_LevelBQ(sKernelSetting ks, cCudaParams params);

private: 
	//GPU Kernels
	//static void Kernel_ClearResultList(cCudaParams params);
	//static void Kernel_RangeQuerySQ(cCudaParams params);
	//static bool Device_ComputeThreadResult(bool* pResults, uint &threadOffset, ushort &dimension);
	//static bool Device_AddToResultList(uint newIndex, uint* D_ResultList, uint* D_ChildIndices, uint &nodeCapacity, uint &bucketOrder, bool &isInner, uint &itemOrder);
public:
	static void CopyRQToConstantMemory(unsigned int dim, unsigned int* pql, unsigned int* pqh, unsigned int count);
	static void RangeQuery_Level(cCudaParams params);
	static uint RangeQuery_Sequential(cCudaParams params);
};

#endif
