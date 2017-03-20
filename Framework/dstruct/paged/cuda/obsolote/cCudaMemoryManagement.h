//If GPU's memory allocation type = simple memory or pinned, allocation is exactly same because we do not need to allocate host memory
//In case of using zero-copy memory we have to create host buffer accrording to allocated GPU's global memory because GPU will access host memory instead its own.

#ifndef __cCudaMemoryManagement_h__
#define __cCudaMemoryManagement_h__

#include <dstruct/paged/cuda/globalDefs.h>
#include <dstruct/paged/cuda/dataDefs.h>
#include <dstruct/paged/cuda/dataManager.h>
#include <dstruct/paged/cuda/utils_Kernel.h>

extern "C" void startProcessingOnCUDA();

#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "common/datatype/tuple/cTuple.h"
#include "common/datatype/cBasicType.h"
#include "common/datatype/cDataType.h"
#include "common/stream/cStream.h"
#include "dstruct/paged/core/cNodeCache.h"
#include "dstruct/paged/rtree/cRTreeLeafNode.h"
#include "dstruct/paged/rtree/cRTreeLeafNodeHeader.h"
#include "common/utils/cTimer.h"
#include "dstruct/paged/rtree/compression/cTuplesCompressor.h"
#include "common/random/cGaussRandomGenerator.h"
#include "common/data/cTuplesGenerator.h"
#include "dstruct/paged/rtree/cRTreeConst.h"
#include "cGpuConst.h"
//#include "test\range_query\sequence_scan\cDataManager.h" //Contains paths for Collections and methods for loading.
#include <cuda_runtime.h>
#include <cuda.h>
using namespace common::data;
using namespace common::datatype::tuple;
using namespace dstruct::paged::rtree;

#define EXTERN extern ;
//EXTERN __device__ unsigned int *D_globalMem;				//Outputs

template<class TKey, class TLeafNode>
class cCudaMemoryManagement
{
private:
    CUdevice cudaDevice;
	cudaDeviceProp deviceProp;
	unsigned int inputsMemSize;
	unsigned int currentOffset;
	void GetFirstCudaDevice();
	unsigned int AllocateAvailableMemory(unsigned int reserve);
	unsigned int AllocateMemory(unsigned int amount);
    __device__ unsigned int *D_inputsMem;				//Outputs
    __device__ unsigned int *D_Offsets;				//Outputs
    __device__ unsigned int *D_OffsetSizes;				//Outputs
    unsigned int *H_inputsMem;				//when zero copy memory is used
	__device__ bool *D_ResultsVector;				//Outputs
protected:
public:

	cCudaMemoryManagement();
	~cCudaMemoryManagement();
	unsigned int GetFreeMemory(CUdevice device);
	unsigned int GetCudaDeviceId();
	unsigned int GetInputsMemorySize();
	unsigned int GetUsedInputsSize();
	unsigned int* GetD_InputsMemory();
	bool* GetD_ResultVector();
	unsigned int* GetD_Offsets();
	unsigned int* GetD_OffsetSizes();
	cudaDeviceProp GetDeviceProp();
	void FreeAllocatedMemory();
	unsigned int CopyBlockToGpu(const cRTreeLeafNode<TKey>* currentLeafNode,unsigned int &dataSize);
	void PrintMemory(unsigned int dim);
	bool* InicializeRangeQuery(unsigned int dim, unsigned int* ql, unsigned int* qh);
	void CopyOffsetsToGpu(unsigned int* offset, unsigned int* offsetSizes, unsigned int count);
	void FinalizeRangeQuery();
};

template<class TKey, class TLeafNode>
cCudaMemoryManagement<TKey,TLeafNode>::cCudaMemoryManagement()
{
	currentOffset=0;
	GetFirstCudaDevice();
#if (USE_MAPPED_MEMORY==2)													//Using simple CUDA pinned memory system with mapping (zero copy memory)
	cutilSafeCall( cudaSetDeviceFlags(cudaDeviceMapHost));				//Must be called befor any data is allocated on GPU
#endif
	if (!checkDeviceProperties(deviceProp)) return ;
	if (!deviceProp.canMapHostMemory) exit(0);

	if (cGpuConst::ALLOCATE_ALL_MEMORY)
		AllocateAvailableMemory(cGpuConst::MEMORY_RESERVE);
	else
		AllocateMemory(cGpuConst::ALLOCATED_MEMORY_SIZE);
	currentOffset=0;
}

template<class TKey, class TLeafNode>
cCudaMemoryManagement<TKey,TLeafNode>::~cCudaMemoryManagement()
{

}

template<class TKey, class TLeafNode>
bool* cCudaMemoryManagement<TKey,TLeafNode>::InicializeRangeQuery(unsigned int dim, unsigned int* ql, unsigned int* qh)
{
	cCudaWorker<TKey,TLeafNode>::CopyRangeQueryToConstantMem(dim,ql,qh);

	//initialize output buffer
#if (USE_MAPPED_MEMORY==1) //Using simple CUDA pinned memory system
	cutilSafeCall (cudaMalloc((void**)&D_ResultsVector, sizeof(bool) * cGpuConst::MAX_ITEMS));
#elif (USE_MAPPED_MEMORY==2) //Using CUDA Zero-Copy memory system - mapped memory
	cutilSafeCall( cudaHostGetDevicePointer((void**)&D_ResultsVector, (void*)bufferResults, 0) );
#else //Cuda simple memory
	cutilSafeCall (cudaMalloc((void**)&D_ResultsVector, sizeof(bool) * cGpuConst::MAX_ITEMS_IN_BLOCK));
#endif
	cudaMemset(D_ResultsVector,false,sizeof(bool) * cGpuConst::MAX_ITEMS);
	return D_ResultsVector;
}

template<class TKey, class TLeafNode>
void cCudaMemoryManagement<TKey,TLeafNode>::FinalizeRangeQuery()
{
#if (USE_MAPPED_MEMORY==1)  //Using simple CUDA pinned memory system without mapping
	cudaFree(D_ResultsVector);
#elif (USE_MAPPED_MEMORY==2)  //Using CUDA Zero-Copy memory system with mapping
	cudaFree(D_ResultsVector);
#else //cuda simple memory
	cudaFree(D_ResultsVector);
#endif
}

template<class TKey, class TLeafNode>
bool* cCudaMemoryManagement<TKey,TLeafNode>::GetD_ResultVector()
{
	return D_ResultsVector;
}
template<class TKey, class TLeafNode>
unsigned int* cCudaMemoryManagement<TKey,TLeafNode>::GetD_Offsets()
{
	return D_Offsets;
}
template<class TKey, class TLeafNode>
unsigned int* cCudaMemoryManagement<TKey,TLeafNode>::GetD_OffsetSizes()
{
	return D_OffsetSizes;
}
template<class TKey, class TLeafNode>
cudaDeviceProp cCudaMemoryManagement<TKey,TLeafNode>::GetDeviceProp()
{
	return deviceProp;
}

template<class TKey, class TLeafNode>
void cCudaMemoryManagement<TKey,TLeafNode>::GetFirstCudaDevice()
{
    //get device count  
    int deviceCount = 0;  
    cudaError_t error_id = cudaGetDeviceCount(&deviceCount);  
    if(deviceCount == 0)  
    {  
		printf( "\nError no cuda devices");  
    }  
    //get the first cuda device  
    CUresult result = cuDeviceGet(&cudaDevice, 0);  
    if(result!= CUDA_SUCCESS)  
    {  
       printf("\nError fetching cuda device");  
    }  
}
template<class TKey, class TLeafNode>
void cCudaMemoryManagement<TKey,TLeafNode>::CopyOffsetsToGpu(unsigned int* offset, unsigned int* offsetSizes, unsigned int count)
{
	unsigned int size = sizeof(unsigned int) * count;
	//cudaMalloc is expensive, so we use zero-copy memory in this case
#if (USE_MAPPED_MEMORY==1) //Using simple CUDA pinned memory system
	cutilSafeCall (cudaMalloc((void**)&D_Offsets, size));
	cutilSafeCall (cudaMalloc((void**)&D_OffsetSizes, size));
	cudaMemcpy(D_Offsets,offset,size, cudaMemcpyHostToDevice);
	cudaMemcpy(D_OffsetSizes,offsetSizes,size, cudaMemcpyHostToDevice);

	/*unsigned int* test = new unsigned int[count];
	(cudaMemcpy(test,D_Offsets,size, cudaMemcpyDeviceToHost));
	for (int i=0; i < count;i++)
	{
		printf("\t %d", test[i]);
	}*/

#elif (USE_MAPPED_MEMORY==2) //Using CUDA Zero-Copy memory system - mapped memory
	cutilSafeCall( cudaHostGetDevicePointer((void**)&D_Offsets, (void*)offset, 0) );
	cutilSafeCall( cudaHostGetDevicePointer((void**)&D_OffsetSizes, (void*)offsetSizes, 0) );
#else //Cuda simple memory
	cutilSafeCall (cudaMalloc((void**)&D_Offsets, sizeof(unsigned int) * count));
	cutilSafeCall (cudaMalloc((void**)&D_OffsetSizes, sizeof(unsigned int) * count));
#endif
}

template<class TKey, class TLeafNode>
unsigned int cCudaMemoryManagement<TKey,TLeafNode>::GetInputsMemorySize()
{
	return inputsMemSize;
}
template<class TKey, class TLeafNode>
unsigned int cCudaMemoryManagement<TKey,TLeafNode>::GetUsedInputsSize()
{
	return currentOffset * sizeof(DATATYPE);
}
template<class TKey, class TLeafNode>
unsigned int cCudaMemoryManagement<TKey,TLeafNode>::GetCudaDeviceId()
{
	return cudaDevice;
}
template<class TKey, class TLeafNode>
unsigned int* cCudaMemoryManagement<TKey,TLeafNode>::GetD_InputsMemory()
{
	return D_inputsMem;
}

template<class TKey, class TLeafNode>
unsigned int cCudaMemoryManagement<TKey,TLeafNode>::AllocateAvailableMemory(unsigned int reserve)
{
	size_t free = cCudaMemoryManagement::GetFreeMemory(cudaDevice);
	free = free - reserve;
#if (USE_MAPPED_MEMORY==2) //Using CUDA Zero-Copy memory system - mapped memory
		cutilSafeCall( cudaHostAlloc((void**)&H_globalMem, free, cudaHostAllocMapped));
		cutilSafeCall( cudaHostGetDevicePointer((void**)&D_globalMem, (void*)H_globalMem, 0) );
#else
	cutilSafeCall(cudaMalloc((void**)&D_inputsMem,free));
#endif
	//cutilSafeCall( cudaHostAlloc((void**)&D_globalMem, free, cudaHostAllocWriteCombined));
	inputsMemSize = free;
	printf("\nAllocated %d MB of GPU's global memory.\n",free/1024/1024);
	return free;
}
template<class TKey, class TLeafNode>
unsigned int cCudaMemoryManagement<TKey,TLeafNode>::AllocateMemory(unsigned int amount)
{
#if (USE_MAPPED_MEMORY==2) //Using CUDA Zero-Copy memory system - mapped memory
		cutilSafeCall( cudaHostAlloc((void**)&H_globalMem, amount, cudaHostAllocMapped));
		cutilSafeCall( cudaHostGetDevicePointer((void**)&D_globalMem, (void*)H_globalMem, 0) );
#else
	cutilSafeCall(cudaMalloc((void**)&D_inputsMem,amount));
#endif
	//cutilSafeCall( cudaHostAlloc((void**)&D_globalMem, free, cudaHostAllocWriteCombined));
	inputsMemSize = amount;
	printf("\nAllocated %d MB of GPU's global memory.\n",amount/1024/1024);
	return amount;
}
template<class TKey, class TLeafNode>
void cCudaMemoryManagement<TKey,TLeafNode>::FreeAllocatedMemory()
{
#if (USE_MAPPED_MEMORY==2) //Using CUDA Zero-Copy memory system - mapped memory
	cudaFree(D_inputsMem);
	cudaFreeHost(H_inputsMem);
#else
	cudaFree(D_inputsMem);
#endif
	unsigned int free = cCudaMemoryManagement::GetFreeMemory(cudaDevice);
	currentOffset=0;
	printf("\nGPU memory freed. New free global memory %d MB.",free/1024/1024);
}

template<class TKey, class TLeafNode>
unsigned int cCudaMemoryManagement<TKey,TLeafNode>::GetFreeMemory(CUdevice device)
{
  
    //create cuda context  
    CUcontext cudaContext;    
    CUresult result = cuCtxCreate(&cudaContext, CU_CTX_SCHED_AUTO, device);  
    if(result != CUDA_SUCCESS)  
    {  
        printf("\nError creating cuda context");  
        return 1;         
    }  
  
    //get the amount of free memory on the graphics card  
    size_t free;  
    size_t total;  
    result = cuMemGetInfo(&free, &total);  
    printf("\n GPU's free global memory: %d MB", free/1024/1024);
    return free;  
}

template<class TKey, class TLeafNode>
unsigned int cCudaMemoryManagement<TKey,TLeafNode>::CopyBlockToGpu(const cRTreeLeafNode<TKey>* currentLeafNode, unsigned int &dataSize)
{
	cMemoryPool* pool = currentLeafNode->GetNodeHeader()->GetMemoryPool();
	//MemoryBuffer* buffer = new MemoryBuffer();
	unsigned int size = currentLeafNode->GetItemCount()*currentLeafNode->GetRTreeLeafNodeHeader()->GetSpaceDescriptor()->GetDimension() * sizeof(DATATYPE);
	MemoryBuffer* buffer((MemoryBuffer*)pool->GetMem(size));
	buffer->Allocate(currentLeafNode->GetItemCount(),currentLeafNode->GetRTreeLeafNodeHeader()->GetSpaceDescriptor()->GetDimension(),currentLeafNode->GetRTreeLeafNodeHeader()->GetDataSize());
	for (unsigned int i = 0 ; i < currentLeafNode->GetItemCount() ; i++)
	{
		buffer->Append((unsigned int*)currentLeafNode->GetCKey(i));
	}
	//buffer->PrintBuffer();
	buffer->Finalize();
	unsigned int arraySize = buffer->GetNumberOfItems();
	dataSize = sizeof(DATATYPE) * arraySize;
	//copy buffer to global memory
	if ((currentOffset + dataSize) > inputsMemSize)
	{
		printf("Critical Error: Trying to copy outside range of allocated GPU's memory");
		return 0;
	}
#if (USE_MAPPED_MEMORY==2) //Using CUDA Zero-Copy memory system - mapped memory
	//data must be copyied into host memory
	memcpy(H_globalMem+currentOffset,buffer->GetItemArray(), dataSize);
#else
	cudaMemcpy(D_inputsMem+currentOffset,buffer->GetItemArray(), dataSize, cudaMemcpyHostToDevice );
#endif
	unsigned int ret= currentOffset;
	currentOffset+=arraySize;
	pool->FreeMem((char*)buffer);
	return  ret;
}

template<class TKey, class TLeafNode>
void cCudaMemoryManagement<TKey,TLeafNode>::PrintMemory(unsigned int dim)
{
	unsigned int *H_temp_globalMem;				//when zero copy memory is used
	cutilSafeCall( cudaHostAlloc((void**)&H_temp_globalMem, currentOffset*sizeof(DATATYPE), cudaHostAllocMapped));
	cutilSafeCall(cudaMemcpy(H_temp_globalMem,D_globalMem,currentOffset*sizeof(DATATYPE), cudaMemcpyDeviceToHost));
	printf("\nPrinting GPU's global memory.\n");
	for(int i = 0;  i < currentOffset;i++)
	{
		if (i% dim == 0)
			printf("\n");
		printf("%d,",H_temp_globalMem[i]);
	}
	cudaFreeHost(H_temp_globalMem);
}
#endifdif