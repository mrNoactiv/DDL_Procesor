/*
 * \class cMemoryManagerCuda
 *
 * \brief Class is responsible for storing nodes on GPU.
 *
 * \author Pavel Bednář
 * \date September 2014
 */

#ifdef CUDA_ENABLED

#ifndef __cMemoryManagerCuda_h__
#define __cMemoryManagerCuda_h__

#include "common/datatype/cBasicType.h"
#include "common/datatype/tuple/cMBRectangle.h"
//#include "common/memdatstruct/cMemoryManager.h"
#include "dstruct/paged/cuda/cCudaGlobalDefs.cuh"
#include "dstruct/paged/cuda/cCudaTimer.h"
#include "dstruct/paged/core/cBucketHeaderStorage.h"
#include "dstruct/paged/cuda/cCudaProcessor.cuh"
#include "dstruct/paged/cuda/cGpuConst.h"
#include "dstruct/paged/queryprocessing/cDbfsLevel.h"
//#include "dstruct/paged/core/cTreeNode.h"
//#include "dstruct/paged/core/cNodeCache.h"

using namespace common::datatype::tuple;
using namespace common::datatype;
using namespace dstruct::paged::core;



class cMemoryManagerCuda
{
	//new implementation:
private:
	cBucketHeaderStorage *mNodeRecordStorage;       // storage for headers of the buckets
	__device__ uint* D_Inputs;					//Array of data blocks.
	__device__ uint* D_SearchOffsets;				//Array of offset in GMEM to be searched
	__device__ bool* D_Results;						//2D Array (stored in single array) of query results
	__device__ uint* D_ChildIndices;				//Holds children nodeindexes for each node
	__device__ uint* D_ResultList;					//list of valid node indices for next search level
	__device__ bool* D_RelevantQueries;				//Holds information if specific query in batch should be compared to block.
	size_t mProcessingMemSize;
	uint mWorkNodesCount;							//maximum of nodes stored in working memory
	//buffers
	uint* mBuffer;
	uint mBufferPos;
	uint* mBufferChilds;
	uint mBufferPosChilds;
	static const int mBufferCapacity = 256;

	uint mDimension;
	uint mGpuId;
	uint mCacheCapacity;							//maximum of nodes to be stored in GMEM
	uint mBlockSize;								//memory size of one block
	uint mMaxQueriesInBatch;						//maximum amount of range queries in one batch
	uint mNodeCapacity;
	cudaDeviceProp mDeviceProperties;				//GPU properties
	int cToMB;// = 1048576;
	int cToKB;// = 1024;
    
	//test
public:
	uint *zero_OutCount;
	uint *dev_zero_OutCount;

private:
	inline size_t GetFreeMemory();
	inline void TransferBuffersToGpu();
	inline void TransferNodeToGpu_Immediately(uint bucketOrder, short nodeType, uint nodeIndex, uint* mbr, uint* children, uint sizeMbr, uint sizeChildren, uint childCount);
	inline void TransferNodeToGpu_Buffered(short nodeType, uint nodeIndex, uint* mbr, uint* children, uint sizeMbr, uint sizeChildren, uint childCount);

public:
	cMemoryManagerCuda();
	~cMemoryManagerCuda();
	inline void Init(uint blockSize, uint dim, uint bufferCapacity,uint nodeCapacity);
	inline void TransferNodeToGpu(short nodeType, uint nodeIndex, uint* mbr, uint* children, uint sizeMbr, uint sizeChildren, uint childCount);
	inline void InicializeRangeQuery(uint dim, uint* qls, uint* qhs, uint queriesInBatch);
	inline void CopySearchArrayToGpu(cArray<uint>* buffer);
	inline void CopyResultVectorFromGpu(uint itemCount, bool* H_ResultVector);
	inline uint GetHostResultListCount(uint* src);
	inline void GetHostResultList(uint* list,uint listCount, uint* src);
	inline bool GetBucket(uint nodeIndex, cBucketHeader **bucketHeader);
	inline bool FindNode(uint nodeIndex);
	inline uint* GetD_Inputs();
	inline bool* GetD_Results();
	inline uint* GetD_SearchOffsets();
	inline bool* GetD_RelevantQueries();
	inline uint* GetD_ChildIndices();
	inline uint* GetD_ResultList();
	inline uint GetMaxNodes();
	inline cudaDeviceProp GetDeviceProperties();
	__device__ uint* D_Qls;				//Holds information if specific query in batch should be compared to block.
	__device__ uint* D_Qhs;				//Holds information if specific query in batch should be compared to block.

};



void cMemoryManagerCuda::Init(uint blockSize, uint dim, uint bufferCapacity, uint nodeCapacity)
{
	cToMB=1048576;
	cToKB=1024;
	assert(blockSize > 0);
	assert(dim > 0);
	mBlockSize = blockSize;
	mNodeCapacity = nodeCapacity;
	int devCount;
	CUDA_CHECK(cudaGetDeviceCount(&devCount), "Failed to get CUDA devices count");
	mGpuId = devCount > 1 ? 1 : 0;
	printf("\nGPU using device %d", mGpuId);
	cudaSetDevice(mGpuId);
	cudaError_t	error_id = cudaGetDeviceProperties(&mDeviceProperties, mGpuId);
	if (error_id != CUDA_SUCCESS)
	{
		printf("\nError fetching CUDA device properties for GPU: %d", mGpuId);
		return;
	}
	//Initialize GPU memory

	size_t recordSize = blockSize + (nodeCapacity + 1)*sizeof(int);
	size_t freeMem = GetFreeMemory();
	size_t cacheMem = freeMem * cGpuConst::SPLIT_RATIO;
	mProcessingMemSize = freeMem - cacheMem;
	assert(mProcessingMemSize > 0);
	
	//initialize cache
	mCacheCapacity = 100000;// cacheMem / recordSize;

	mNodeRecordStorage = new cBucketHeaderStorage(mCacheCapacity);
	mNodeRecordStorage->Clear();
	size_t size_Inputs = mCacheCapacity * blockSize;
	size_t size_ChildIndices = mCacheCapacity * (nodeCapacity)* sizeof(int);
	CUDA_CHECK(cudaMalloc((void**)&D_Inputs, size_Inputs), "Initialize D_Inputs");
	if (cGpuConst::STORE_CHILD_REFERENCES)
	{
		CUDA_CHECK(cudaMalloc((void**)&D_ChildIndices, size_ChildIndices), "Initialize D_ChildIndices");
	}
	printf("\nGPU cache buffer: %d MB. Cache peak capacity: %u", (size_Inputs + size_ChildIndices) / cToMB, mCacheCapacity);
	printf("\n\t%d MB\tData blocks.", (size_Inputs ) / cToMB );
	printf("\n\t%d MB\tChildren indices list.", (size_ChildIndices) / cToMB);

	//alocate copybuffer
	if (cGpuConst::BUFFERED_COPY)
	{
		mBuffer = (uint*)malloc(mBufferCapacity*blockSize); //nahradit memory managerem
		mBufferChilds = (uint*)malloc(mBufferCapacity*nodeCapacity);//nahradit memory managerem
		mBufferPos = 0;
		mBufferPosChilds = 0;
	}
	mMaxQueriesInBatch = 10; //temporary fix
	//alocate working memory
	//alokace bude probihat, jako by se jednalo o single query. V případě batch se pak použije zlomek D_SearchOffsets a na D_Results se bude dívat jako na 2D pole.
	size_t nodeSize =sizeof(int) + (nodeCapacity) * sizeof(bool); //1 item in search buffer + node capacity * bool output
	size_t nodeSize_relevantQs = mMaxQueriesInBatch * nodeCapacity *sizeof(bool); //relevant queries array
	size_t nodeSize_resultArray = nodeCapacity * sizeof(int); //output list array
	if (cGpuConst::BATCH_ONLY_RELEVANT)
	{
		nodeSize += nodeSize_relevantQs;
	}
	if (cGpuConst::RESULT_STRUCT == cGpuResultStructure::DistinctList)
	{
		nodeSize += nodeSize_resultArray;
	}
	mWorkNodesCount = mProcessingMemSize / nodeSize;
	size_t sizeSearchOffsets = mWorkNodesCount*sizeof(int);
	size_t sizeResultset = mWorkNodesCount * (nodeCapacity ) * sizeof(bool);
	size_t sizeResultList = mWorkNodesCount * nodeCapacity * sizeof(int);
	CUDA_CHECK(cudaMalloc((void**)&D_SearchOffsets, sizeSearchOffsets), "Initialize D_SearchOffsets");
	CUDA_CHECK(cudaMalloc((void**)&D_Results, sizeResultset), "Initialize D_Results");
	printf("\nGPU working memory: %d MB. Max buffer capacity: %u", (nodeSize*mWorkNodesCount) / cToMB,mWorkNodesCount);
	printf("\n\t%d MB\tSearch buffer", (sizeSearchOffsets) / cToMB );
	printf("\n\t%d MB\tResultArray buffer", (sizeResultset) / cToMB );
	if (cGpuConst::BATCH_ONLY_RELEVANT)
	{
		CUDA_CHECK(cudaMalloc((void**)&D_RelevantQueries, nodeSize_relevantQs*mWorkNodesCount), "Initialize D_RelevantQueries");
		printf("\n\t%d MB\tRelevant queries array.", (sizeResultset) / cToMB );
	}
	if (cGpuConst::RESULT_STRUCT == cGpuResultStructure::DistinctList)
	{
		CUDA_CHECK(cudaMalloc((void**)&D_ResultList, sizeResultList), "Initialize D_ResultListCount");
		printf("\n\t%d MB\tResult List.", (sizeResultList) / cToMB);
	}
	CUDA_CHECK(cudaMalloc((void**)&D_Qls, 1000 * dim * sizeof(uint)), "Initialize D_Qls");
	CUDA_CHECK(cudaMalloc((void**)&D_Qhs, 1000 * dim * sizeof(uint)), "Initialize D_Qhs");


	//test
	//cudaMallocManaged(&zero_OutCount,sizeof(uint));
	cudaSetDeviceFlags(cudaDeviceMapHost);
	cudaHostAlloc( (void**)&zero_OutCount, sizeof(uint), cudaHostAllocMapped | cudaHostAllocWriteCombined );
	cudaHostGetDevicePointer(&dev_zero_OutCount, zero_OutCount, 0);

}
size_t cMemoryManagerCuda::GetFreeMemory()
{
	cudaError_t result;
	size_t free;
	size_t total;
	result = cudaMemGetInfo(&free, &total);
	if (result != cudaSuccess)
		printf("\nCUDA Warning: An error occurred when trying to get amount of free global memory. %s",cudaGetErrorString(result));
	if (cGpuConst::FIXED_MEMORY_SIZE != cGpuConst::NOT_ASSIGNED)
	{
		if (free < cGpuConst::FIXED_MEMORY_SIZE)
		{
			printf("\nCUDA Warning: Fixed GPU memory allocation requested %d MB but there was only %d MB!", cGpuConst::FIXED_MEMORY_SIZE / cToMB, free / cToMB);
			return free;
		}
		else
		{
			return cGpuConst::FIXED_MEMORY_SIZE;
		}
	}
	else
	{
		free = free * 0.5;
	}
	printf("\nGPU:%d free global memory: %d MB", mGpuId, free / cToMB);
	return free;
}
bool cMemoryManagerCuda::GetBucket(uint nodeIndex, cBucketHeader **bucketHeader)
{
	return mNodeRecordStorage->FindBucket(nodeIndex, bucketHeader);
}
bool cMemoryManagerCuda::FindNode(uint nodeIndex)
{
	cBucketHeader *bucketHeader;
	return mNodeRecordStorage->FindNode(nodeIndex,&bucketHeader);
	//return mNodeRecordStorage->FindBucket(nodeIndex, &bucketHeader);

}

void cMemoryManagerCuda::TransferNodeToGpu(short nodeType, uint nodeIndex, uint* mbr,uint* children, uint sizeMbr, uint sizeChildren, uint childCount)
{
	
	cBucketHeader *bucketHeader;
	uint bucketOrder;
	bool nodeFound;
	if (cGpuConst::MEMORY_LOCATION == cGpuMainMemoryLocation::Gpu)
	{
		//bucketOrder references to nodeIndex
		//printf("\rCopying node %d to GPU", nodeIndex);
		nodeFound = false;
		bucketOrder = nodeIndex;
		bucketHeader = mNodeRecordStorage->GetBucketHeader(nodeIndex);
	}
	else
	{
		nodeFound = mNodeRecordStorage->FindBucket(nodeIndex, &bucketHeader); //najdi nebo vytvoř nový buket
		bucketOrder = bucketHeader->GetBucketOrder(); //pořadí v GPU chache
	}
	if (!nodeFound)
	{
		cudaError_t err;
		if (cGpuConst::BUFFERED_COPY)
		{
			//if (!TransferNodeToGpu_Buffered(nodeType, nodeIndex, mbr, children, sizeMbr, sizeChildren, childCount))
				//TransferNodeToGpu_Immediately(bucketOrder, nodeType, nodeIndex, mbr, children, sizeMbr, sizeChildren, childCount);

		}
		else
		{
				TransferNodeToGpu_Immediately(bucketOrder, nodeType, nodeIndex, mbr, children, sizeMbr, sizeChildren, childCount);
		}
		//test - back copy
		/*int* tmp = (int*)malloc(dataSize);
		CUDA_CHECK(cudaMemcpy(tmp, offset + 1, dataSize, cudaMemcpyDeviceToHost), "test: Copy node from GPU");
		int cnt = dataSize / sizeof(int);
		for (uint i = 0; i<cnt; i++)
		{
		if (i%11==0)
		printf("\n");
		if (i == cnt / 2)
		printf("\n\n");
		printf("%d,", tmp[i]);
		}
		printf("\n");*/
		mNodeRecordStorage->AddInBucketIndex(nodeIndex, bucketOrder);
		bucketHeader->SetGpuId(mGpuId);
		bucketHeader->SetGpuItemOrder(bucketOrder);
	}

	bucketHeader->IncrementReadLock();
	//read lock se nastaví, at už je uzel na gpu nebo ne, tak bude potřeba pro následujcí RQ. Po provedení RQ by se měly dekrementovat všechny R locky použité v dané RQ
	//v případě, že by nad cache probíhalo více RQ, dekrementace zajistí, že se nesmaže uzel, který je potřeba.

}
void cMemoryManagerCuda::TransferNodeToGpu_Buffered(short nodeType, uint nodeIndex, uint* mbr, uint* children, uint sizeMbr, uint sizeChildren, uint childCount)
{
	//cBucketHeader **bucketHeader;// = new cBucketHeader[mBufferCapacity];
	//bool nodeFound = mNodeRecordStorage->FindBuckets(mBufferCapacity, bucketHeader); //najdi posloupnost buketů o daném počtu
	//uint bucketOrder = bucketHeader->GetBucketOrder(); //pořadí v GPU chache
	//uint* offset = D_Inputs + (bucketOrder*mBlockSize); //konkrétní uzel v GPU CACHE

	//if (mBufferPos + sizeMbr > mBufferCapacity*sizeof(int))
	//{
	//	TransferBuffersToGpu();
	//}
	//uint *tmp = mBuffer + mBufferPos;
	//tmp[0] = childCount;
	//memcpy(&tmp[1], mbr, sizeMbr);
	//mBufferPos += sizeMbr;

}
void cMemoryManagerCuda::TransferNodeToGpu_Immediately(uint bucketOrder, short nodeType, uint nodeIndex, uint* mbr, uint* children, uint sizeMbr, uint sizeChildren, uint childCount)
{
	//printf("\nCUDA copy nodeindex: %d to GPU, bucketOrder: %d", nodeIndex,bucketOrder);
	cudaError_t err;
	uint* offset = D_Inputs + (bucketOrder*mBlockSize/sizeof(uint)); //konkrétní uzel v GPU CACHE
	err = cudaMemcpy(offset, &childCount, sizeof(int), cudaMemcpyHostToDevice);
	if (err != cudaSuccess)
	{
		printf("\nCUDA error when copy block to GMEM. %s", cudaGetErrorString(err));
	}
	err = cudaMemcpy(offset + 1, mbr, sizeMbr, cudaMemcpyHostToDevice);
	if (err != cudaSuccess)
	{
		printf("\nCUDA error when copy block to GMEM. %s", cudaGetErrorString(err));
	}

	if (cGpuConst::STORE_CHILD_REFERENCES && nodeType == cGpuConst::NODE_HEADER) //not for leaf nodes
	{
		err = cudaMemcpy(D_ChildIndices + (bucketOrder*mNodeCapacity), children, sizeChildren, cudaMemcpyHostToDevice);
		if (err != cudaSuccess)
		{
			printf("\nCUDA error when copy node children to GMEM. %s", cudaGetErrorString(err));
		}

	}
}
//void cMemoryManagerCuda::TransferNodeToGpu(uint nodeIndex, uint* data, uint dataSize, uint childCount, uint* childArray)
//{
//	cBucketHeader *bucketHeader;
//	bool nodeFound = mNodeRecordStorage->FindBucket(nodeIndex, &bucketHeader); //najdi nebo vytvoř nový buket
//	uint bucketOrder = bucketHeader->GetBucketOrder(); //pořadí v GPU chache
//	uint* offset = D_Inputs + (bucketOrder*mBlockSize); //konkrétní uzel v GPU CACHE
//
//	if (!nodeFound)
//	{
//		cudaError_t err;
//		if (cGpuConst::USE_COPY_BUFFER)
//		{
//			if (mBufferPos + dataSize > mBufferCapacity*sizeof(int))
//			{
//				TransferBuffersToGpu();
//			}
//			uint *tmp = mBuffer + mBufferPos;
//			tmp[0] = childCount;
//			memcpy(&tmp[1], data, dataSize);
//			mBufferPos += dataSize;
//		}
//		else
//		{
//			//err = cudaMemcpy(offset, &childCount, sizeof(int), cudaMemcpyHostToDevice);
//			//if (err != cudaSuccess)
//			//{
//			//	printf("\nCUDA error when copy block to GMEM. %s", cudaGetErrorString(err));
//			//}
//			err = cudaMemcpy(offset + 1, data, dataSize, cudaMemcpyHostToDevice);
//			if (err != cudaSuccess)
//			{
//				printf("\nCUDA error when copy block to GMEM. %s", cudaGetErrorString(err));
//			}
//		}
//		//test - back copy
//		/*int* tmp = (int*)malloc(dataSize);
//		CUDA_CHECK(cudaMemcpy(tmp, offset + 1, dataSize, cudaMemcpyDeviceToHost), "test: Copy node from GPU");
//		int cnt = dataSize / sizeof(int);
//		for (uint i = 0; i<cnt; i++)
//		{
//			if (i%11==0)
//				printf("\n");
//			if (i == cnt / 2)
//				printf("\n\n");
//			printf("%d,", tmp[i]);
//		}
//		printf("\n");*/
//		mNodeRecordStorage->AddInBucketIndex(nodeIndex, bucketOrder);
//		bucketHeader->SetGpuId(mGpuId);
//		bucketHeader->SetGpuItemOrder(bucketOrder);
//	}
//
//	bucketHeader->IncrementReadLock();
//	//read lock se nastaví, at už je uzel na gpu nebo ne, tak bude potřeba pro následujcí RQ. Po provedení RQ by se měly dekrementovat všechny R locky použité v dané RQ
//	//v případě, že by nad cache probíhalo více RQ, dekrementace zajistí, že se nesmaže uzel, který je potřeba.
//
//
//
//}
void cMemoryManagerCuda::TransferBuffersToGpu()
{
	//CUDA_CHECK(cudaMemcpy(offset, , sizeof(int), cudaMemcpyHostToDevice),"Copy memory buffer to device.");

}
void cMemoryManagerCuda::InicializeRangeQuery(uint dim, uint* qls, uint* qhs, uint queriesInBatch)
{
#ifdef CUDA_MEASURE
	cCudaTimer* tmrHtoD = new cCudaTimer();
	tmrHtoD->Start();
#endif
	cCudaProcessor::CopyRQToConstantMemory(dim, qls, qhs, queriesInBatch);
#ifdef CUDA_MEASURE
	tmrHtoD->Stop();
	cCudaTimer::TimeHtoD += tmrHtoD->GetTime();
#endif
}
inline void cMemoryManagerCuda::CopySearchArrayToGpu(cArray<uint>* buffer)
{
	//int* arrayItemOreder, uint itemCount
#ifdef CUDA_MEASURE
	cCudaTimer* tmrHtoD = new cCudaTimer();
	tmrHtoD->Start();
#endif
	size_t memSize = buffer->Count() * sizeof(uint);
	CUDA_CHECK(cudaMemcpy(D_SearchOffsets, buffer->GetArray(), memSize, cudaMemcpyHostToDevice),"Copy search array");
	/*int* tmp = (int*)malloc(memSize);
	CUDA_CHECK(cudaMemcpy(tmp, D_SearchOffsets, memSize, cudaMemcpyDeviceToHost),"Copy search array");
	printf("\nSearchArray: ");
	for (uint i=0;i<itemCount;i++)
	{
		printf("%d,",tmp[i]);
	}
	*/
#ifdef CUDA_MEASURE
	tmrHtoD->Stop();
	cCudaTimer::TimeSearchArray += tmrHtoD->GetTime();
	delete tmrHtoD;
#endif
}
void cMemoryManagerCuda::CopyResultVectorFromGpu(uint itemCount, bool* H_ResultVector)
{
#ifdef CUDA_MEASURE
	cCudaTimer* tmrDtoH = new cCudaTimer();
	tmrDtoH->Start();
#endif
	CUDA_CHECK(cudaMemcpy(H_ResultVector, D_Results, itemCount * sizeof(bool), cudaMemcpyDeviceToHost),"\nCUDA copy result array from GPU");
#ifdef CUDA_MEASURE
	tmrDtoH->Stop();
	cCudaTimer::TimeResultVector += tmrDtoH->GetTime();
	delete tmrDtoH;
#endif
}



uint cMemoryManagerCuda::GetHostResultListCount(uint* src)
{
#ifdef CUDA_MEASURE
	cCudaTimer* tmrDtoH = new cCudaTimer();
	tmrDtoH->Start();
#endif
	uint listCount = 0;
	CUDA_CHECK(cudaMemcpy(&listCount, src, sizeof(uint), cudaMemcpyDeviceToHost), "\nCUDA copy result list count from GPU");
	//printf("\nCUDA result list count: %d", listCount);
	return listCount;
#ifdef CUDA_MEASURE
	tmrDtoH->Stop();
	cCudaTimer::TimeResultVector += tmrDtoH->GetTime();
	delete tmrDtoH;
#endif
}
void cMemoryManagerCuda::GetHostResultList(uint* list,uint listCount, uint* src)
{
#ifdef CUDA_MEASURE
	cCudaTimer* tmrDtoH = new cCudaTimer();
	tmrDtoH->Start();
#endif
	CUDA_CHECK(cudaMemcpy(list, src+1, listCount * sizeof(uint), cudaMemcpyDeviceToHost), "\nCUDA copy result list from GPU");
	/*printf("\nResult List: ");
	for (uint i=0;i<listCount;i++)
	{
		printf("%d,",list[i]);
	}*/
#ifdef CUDA_MEASURE
	tmrDtoH->Stop();
	cCudaTimer::TimeResultVector += tmrDtoH->GetTime();
	delete tmrDtoH;
#endif
}
cudaDeviceProp cMemoryManagerCuda::GetDeviceProperties()
{
	return mDeviceProperties;
}
uint* cMemoryManagerCuda::GetD_Inputs()
{
	return D_Inputs;
}
bool* cMemoryManagerCuda::GetD_Results()
{
	return D_Results;
}

uint* cMemoryManagerCuda::GetD_SearchOffsets()
{
	return D_SearchOffsets;
}
uint* cMemoryManagerCuda::GetD_ChildIndices()
{
	return D_ChildIndices;
}
bool* cMemoryManagerCuda::GetD_RelevantQueries()
{
	return D_RelevantQueries;
}
uint* cMemoryManagerCuda::GetD_ResultList()
{
	return D_ResultList;
}
uint cMemoryManagerCuda::GetMaxNodes()
{
	return mWorkNodesCount;
}

#endif
#endif
