#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "dstruct/paged/cuda/cCudaProcessor35.cuh"
#include "dstruct/paged/cuda/cCudaCommonKernels.cuh"
#include "dstruct/paged/cuda/cGpuConst.h"
#include "dstruct/paged/cuda/cCudaParams.h"
#include "dstruct/paged/cuda/cCudaTimer.h"




/*
__global__ void rq_dbfs_35(uint* nodeIndices,uint nodeCount,uint level, cCudaParamsRecursive params)
{
	extern __shared__ int sMem[];
	bool *sRelevant = (bool*)&sMem[0];
	bool *sResults = sRelevant + params.NodeCapacity;
	uint tid = threadIdx.x;
	uint dimOffset;
	uint bucketOrder = nodeIndices[blockIdx.x];
	bool isInner = level < params.NoLevels-1;

	const int tuplesCount = params.D_Inputs[bucketOrder * params.BlockSize / sizeof(uint)];
	uint* D_input = params.D_Inputs + (bucketOrder * params.BlockSize / sizeof(uint)) + 1; //first number is number of items in block
	uint* D_input2;
	if (isInner)
	{
		D_input2 = params.D_Inputs + (bucketOrder * params.BlockSize / sizeof(uint)) + 1 + tuplesCount*params.Dimension;
	}

	uint threadOffset = threadIdx.x;
	while (threadOffset < tuplesCount *params.Dimension)
	{
		if (threadOffset < params.NodeCapacity)
			sRelevant[threadOffset] = false; //clear array of relevant children
		dimOffset = threadOffset % params.Dimension;
		for (short b = 0; b < params.QueriesInBatch; b++)
		{
			uint sOffset = b* tuplesCount *params.Dimension + threadOffset;
			if (!isInner)
			{
				sResults[sOffset] = !(NOTININTERVAL(C_RQElement[dimOffset].minimum, C_RQElement[dimOffset].maximum, D_input[threadOffset]));
			}
			else
			{
				sResults[sOffset] = !(NOTINTERSECTED(D_input[threadOffset], D_input2[threadOffset], C_RQElement[dimOffset].minimum, C_RQElement[dimOffset].maximum));
				//printf("\ntid;%d;ql;%d;qh;%d;r1;%d;r2;%d;result;%d", tid, C_RQElement[dimOffset].minimum, C_RQElement[dimOffset].maximum, D_input[tid], D_input2[tid], sResults[tid] ? 1 : 0);
			}
			dimOffset += params.Dimension;
		}
		threadOffset += blockDim.x;
	}
	__syncthreads();

	//prepare relevant result into single row

	//sumarize all batch results into single row of tuples.
	threadOffset = threadIdx.x;
	while (threadOffset < tuplesCount * params.QueriesInBatch)
	{
		uint dataOffset = threadOffset * params.Dimension;
		bool threadResult = Device_ComputeThreadResult(sResults, dataOffset, params.Dimension);

		if (threadResult)
		{
			uint tupleOffset = threadOffset % tuplesCount;
			//if (gridDim.x ==1)
			//printf("\nGPU found relevant %d. child for batch: %d", tupleOffset, threadOffset / tuplesCount);
			sRelevant[tupleOffset] = true;
		}
		threadOffset += blockDim.x;
	}
	threadOffset = threadIdx.x;
	__syncthreads();
	if (isInner && tid == 0)
	{
		uint cnt =0;
		uint *indices = new uint[30];
		for (uint r = 0; r < tuplesCount;r++)
		{
			if (sRelevant[r])
			{
				indices[++cnt] = params.D_ChildIndices[bucketOrder*params.NodeCapacity + r];
			}
		}
		rq_dbfs_35<<<cnt,1024,24000>>>(indices,cnt,level+1,params);

	}
	else
	{
		while (threadOffset < tuplesCount)
		{
			if (sRelevant[threadOffset]) //tuple is relevant for at least one query in batch
			{
				uint newIndex = atomicAdd(params.D_ResultList, 1);
				int bitIndex = nodeIndices[blockIdx.x] << 8;
				params.D_ResultList[newIndex + 1] = bitIndex | threadOffset;
				//printf("\nChild index:; %d; value: %d", nodeIndex, bitIndex | nodeIndex, newIndex + 1);
				threadOffset += blockDim.x;
			}
		}
	}
}
__global__ void rq_dbfs_35(uint nodeIndex,uint level, cCudaParamsRecursive params)
{
	extern __shared__ int sMem[];
		bool *sRelevant = (bool*)&sMem[0];
		bool *sResults = sRelevant + params.NodeCapacity;
		uint tid = threadIdx.x;
		uint dimOffset;
		uint bucketOrder = nodeIndex;
		bool isInner = level < params.NoLevels-1;

		int tuplesCount = params.D_Inputs[bucketOrder * params.BlockSize / sizeof(uint)];
		uint* D_input = params.D_Inputs + (bucketOrder * params.BlockSize / sizeof(uint)) + 1; //first number is number of items in block
		uint* D_input2;
		if (isInner)
		{
			D_input2 = params.D_Inputs + (bucketOrder * params.BlockSize / sizeof(uint)) + 1 + tuplesCount*params.Dimension;
		}

		uint threadOffset = threadIdx.x;
		while (threadOffset < tuplesCount *params.Dimension)
		{
			if (threadOffset < params.NodeCapacity)
				sRelevant[threadOffset] = false; //clear array of relevant children
			dimOffset = threadOffset % params.Dimension;
			for (short b = 0; b < params.QueriesInBatch; b++)
			{
				uint sOffset = b* tuplesCount *params.Dimension + threadOffset;
				if (!isInner)
				{
					sResults[sOffset] = !(NOTININTERVAL(C_RQElement[dimOffset].minimum, C_RQElement[dimOffset].maximum, D_input[threadOffset]));
				}
				else
				{
					sResults[sOffset] = !(NOTINTERSECTED(D_input[threadOffset], D_input2[threadOffset], C_RQElement[dimOffset].minimum, C_RQElement[dimOffset].maximum));
					//printf("\ntid;%d;ql;%d;qh;%d;r1;%d;r2;%d;result;%d", tid, C_RQElement[dimOffset].minimum, C_RQElement[dimOffset].maximum, D_input[tid], D_input2[tid], sResults[tid] ? 1 : 0);
				}
				dimOffset += params.Dimension;
			}
			threadOffset += blockDim.x;
		}
		__syncthreads();

		//prepare relevant result into single row

		//sumarize all batch results into single row of tuples.
		threadOffset = threadIdx.x;
		while (threadOffset < tuplesCount * params.QueriesInBatch)
		{
			uint dataOffset = threadOffset * params.Dimension;
			bool threadResult = Device_ComputeThreadResult(sResults, dataOffset, params.Dimension);

			if (threadResult)
			{
				uint tupleOffset = threadOffset % tuplesCount;
				//if (gridDim.x ==1)
				printf("\nGPU found relevant %d. child for batch: %d", tupleOffset, threadOffset / tuplesCount);
				sRelevant[tupleOffset] = true;
			}
			threadOffset += blockDim.x;
		}
		threadOffset = threadIdx.x;
	__syncthreads();
	if (isInner)
	{
		if (threadIdx.x == 0)
		{
		uint cnt =0;
		uint *indices = new uint[30];
		for (uint r = 0; r < tuplesCount;r++)
		{
			if (sRelevant[r])
			{
				indices[cnt++] = params.D_ChildIndices[bucketOrder*params.NodeCapacity + r];
			}
		}
		if (cnt>0)
		rq_dbfs_35<<<cnt,1024,24000>>>(indices,cnt,level+1,params);
		}
	}
	else
	{
		while (threadOffset < tuplesCount)
		{
			if (sRelevant[threadOffset]) //tuple is relevant for at least one query in batch
			{
				uint newIndex = atomicAdd(params.D_ResultList, 1);
				int bitIndex = nodeIndex << 8;
				params.D_ResultList[newIndex + 1] = bitIndex | threadOffset;
				//printf("\nChild index:; %d; value: %d", nodeIndex, bitIndex | nodeIndex, newIndex + 1);
				threadOffset += blockDim.x;
			}
		}
	}
}
__global__ void rq_dfs_35(uint nodeIndex,uint level, cCudaParamsRecursive params)
{
	extern __shared__ int sMem[];
	bool *sRelevant = (bool*)&sMem[0];
	bool *sResults = sRelevant + params.NodeCapacity;
	uint tid = threadIdx.x;
	uint dimOffset;
	uint bucketOrder = nodeIndex;
	bool isInner = level < params.NoLevels-1;

	int tuplesCount = params.D_Inputs[bucketOrder * params.BlockSize / sizeof(uint)];
	uint* D_input = params.D_Inputs + (bucketOrder * params.BlockSize / sizeof(uint)) + 1; //first number is number of items in block
	uint* D_input2;
	if (isInner)
	{
		D_input2 = params.D_Inputs + (bucketOrder * params.BlockSize / sizeof(uint)) + 1 + tuplesCount*params.Dimension;
	}

	uint threadOffset = threadIdx.x;
	while (threadOffset < tuplesCount *params.Dimension)
	{
		if (threadOffset < params.NodeCapacity)
			sRelevant[threadOffset] = false; //clear array of relevant children
		dimOffset = threadOffset % params.Dimension;
		for (short b = 0; b < params.QueriesInBatch; b++)
		{
			uint sOffset = b* tuplesCount *params.Dimension + threadOffset;
			if (!isInner)
			{
				sResults[sOffset] = !(NOTININTERVAL(C_RQElement[dimOffset].minimum, C_RQElement[dimOffset].maximum, D_input[threadOffset]));
			}
			else
			{
				sResults[sOffset] = !(NOTINTERSECTED(D_input[threadOffset], D_input2[threadOffset], C_RQElement[dimOffset].minimum, C_RQElement[dimOffset].maximum));
				//printf("\ntid;%d;ql;%d;qh;%d;r1;%d;r2;%d;result;%d", tid, C_RQElement[dimOffset].minimum, C_RQElement[dimOffset].maximum, D_input[tid], D_input2[tid], sResults[tid] ? 1 : 0);
			}
			dimOffset += params.Dimension;
		}
		threadOffset += blockDim.x;
	}
	__syncthreads();

	//prepare relevant result into single row

	//sumarize all batch results into single row of tuples.
	threadOffset = threadIdx.x;
	while (threadOffset < tuplesCount * params.QueriesInBatch)
	{
		uint dataOffset = threadOffset * params.Dimension;
		bool threadResult = Device_ComputeThreadResult(sResults, dataOffset, params.Dimension);

		if (threadResult)
		{
			uint tupleOffset = threadOffset % tuplesCount;
			//if (gridDim.x ==1)
			//printf("\nGPU found relevant %d. child for batch: %d", tupleOffset, threadOffset / tuplesCount);
			sRelevant[tupleOffset] = true;
		}
		threadOffset += blockDim.x;
	}
	threadOffset = threadIdx.x;
	__syncthreads();
	while (threadOffset < tuplesCount)
	{
		if (sRelevant[threadOffset]) //tuple is relevant for at least one query in batch
		{

			if (isInner)
			{
				uint childIndex = params.D_ChildIndices[bucketOrder*params.NodeCapacity + threadOffset];
				rq_dfs_35<<<1,512,24000>>>(childIndex,level+1,params);
				//printf("\n\tNodeIndex:; %d; child NodeIndex: %d, block: %d, bucketOrder: %d",nodeIndex, childIndex,blockIdx.x,bucketOrder);
			}
			else
			{
				uint newIndex = atomicAdd(params.D_ResultList, 1);

				int bitIndex = nodeIndex << 8;
				params.D_ResultList[newIndex + 1] = bitIndex | threadOffset;
				//printf("\nChild index:; %d; value: %d", nodeIndex, bitIndex | nodeIndex, newIndex + 1);
			}
		}
		threadOffset += blockDim.x;
	}
}
__global__ void rq_bfs_35_controller(uint level,uint nodeIndex, cCudaParamsBasic_List params,uint sMemSize)
{
	uint itemsCount;
	uint threadOffset = threadIdx.x;
	bool isLeaf = level == params.NoLevels - 1;
	if (level == 0)
	{
		 itemsCount = 1;
		 params.D_SearchOffsets[0] = nodeIndex; //items count
		 params.D_ResultList[0] = 0;
	}
	else
	{
		itemsCount = params.D_ResultList[0];
		while (threadOffset < itemsCount)
		{
			params.D_SearchOffsets[threadOffset] = params.D_ResultList[threadOffset+1];
			//printf("\n#%d\ttid: %d\t sets: %d\t on %d",level,threadIdx.x,params.D_ResultList[threadOffset+1],threadOffset);
			threadOffset += blockDim.x;
		}
	}
	if (threadIdx.x == 0)
	{
		params.D_ResultList[0]=0;
		//printf("\nLevel: %d, items: %d",level, itemsCount);
		gpuscan_kernel_bq<<<itemsCount,blockDim.x,sMemSize>>>(!isLeaf,params);
	}
}
*/

__global__ void Kernel_RangeQueryBFS(bool isInner, cCudaParamsBasic_List params)
{
#if __CUDA_ARCH__ >= 350
	extern __shared__ int sMem[];
	bool *sRelevant = (bool*)&sMem[0];
	bool *sResults = sRelevant + params.NodeCapacity;
	uint tid = threadIdx.x;
	uint dimOffset;

	uint bucketOrder = params.D_SearchOffsets[blockIdx.x];


	int tuplesCount = params.D_Inputs[bucketOrder * params.BlockSize / sizeof(uint)];
	uint* D_input = params.D_Inputs + (bucketOrder * params.BlockSize / sizeof(uint)) + 1; //first number is number of items in block
	uint* D_input2;
	if (isInner)
	{
		D_input2 = params.D_Inputs + (bucketOrder * params.BlockSize / sizeof(uint)) + 1 + tuplesCount*params.Dimension;
	}

	uint threadOffset = threadIdx.x;
	while (threadOffset < tuplesCount *params.Dimension)
	{
		if (threadOffset < params.NodeCapacity)
			sRelevant[threadOffset] = false; //clear array of relevant children
		dimOffset = threadOffset % params.Dimension;
		for (short b = 0; b < params.QueriesInBatch; b++)
		{
			uint sOffset = b* tuplesCount *params.Dimension + threadOffset;
			if (!isInner)
			{
				sResults[sOffset] = !(NOTININTERVAL(C_RQElement[dimOffset].minimum, C_RQElement[dimOffset].maximum, D_input[threadOffset]));
			}
			else
			{
				sResults[sOffset] = !(NOTINTERSECTED(D_input[threadOffset], D_input2[threadOffset], C_RQElement[dimOffset].minimum, C_RQElement[dimOffset].maximum));
				//printf("\ntid;%d;ql;%d;qh;%d;r1;%d;r2;%d;result;%d", tid, C_RQElement[dimOffset].minimum, C_RQElement[dimOffset].maximum, D_input[tid], D_input2[tid], sResults[tid] ? 1 : 0);
			}
			dimOffset += params.Dimension;
		}
		threadOffset += blockDim.x;
	}
	__syncthreads();

	//prepare relevant result into single row

	//sumarize all batch results into single row of tuples.
	threadOffset = threadIdx.x;
	while (threadOffset < tuplesCount * params.QueriesInBatch)
	{
		uint dataOffset = threadOffset * params.Dimension;
		bool threadResult = Device_ComputeThreadResult(sResults, dataOffset, params.Dimension);

		if (threadResult)
		{
			uint tupleOffset = threadOffset % tuplesCount;
			/*if (gridDim.x ==1)
			printf("\nGPU found relevant %d. child for batch: %d", tupleOffset, threadOffset / tuplesCount);*/
			sRelevant[tupleOffset] = true;
		}
		threadOffset += blockDim.x;
	}
	threadOffset = threadIdx.x;
	__syncthreads();
	while (threadOffset < tuplesCount)
	{
		if (sRelevant[threadOffset]) //tuple is relevant for at least one query in batch
		{
			uint newIndex = atomicAdd(params.D_ResultList, 1);
			if (isInner)
			{
				uint childIndex = params.D_ChildIndices[bucketOrder*params.NodeCapacity + threadOffset];
				params.D_ResultList[newIndex + 1] = childIndex;
				//printf("\n\tChild index:; %d; newIndex: %d, block: %d, bucketOrder: %d", childIndex, newIndex + 1,blockIdx.x,bucketOrder);
			}
			else
			{
				uint childIndex = params.D_SearchOffsets[blockIdx.x];
				int bitIndex = childIndex << 8;
				params.D_ResultList[newIndex + 1] = bitIndex | threadOffset;
				//printf("\nChild index:; %d; value: %d", childIndex, bitIndex | childIndex, newIndex + 1);
			}
		}
		threadOffset += blockDim.x;
	}
#else
	printf("\nCritical Error. Insufficient CUDA CAPABILITY.");
#endif
}
__global__ void Kernel_RangeQueryBFS_Controller(uint level, uint nodeIndex, cCudaParamsBasic_List params, uint sMemSize)
{
#if __CUDA_ARCH__ >= 350
	uint itemsCount;
	uint threadOffset = threadIdx.x;
	bool isLeaf = level == params.NoLevels - 1;
	if (level == 0)
	{
		itemsCount = 1;
		params.D_SearchOffsets[0] = nodeIndex; //items count
		params.D_ResultList[0] = 0;
	}
	else
	{
		itemsCount = params.D_ResultList[0];
		while (threadOffset < itemsCount)
		{
			params.D_SearchOffsets[threadOffset] = params.D_ResultList[threadOffset+1];
			//printf("\n#%d\ttid: %d\t sets: %d\t on %d",level,threadIdx.x,params.D_ResultList[threadOffset+1],threadOffset);
			threadOffset += blockDim.x;
		}
	}
	if (threadIdx.x == 0)
	{
		params.D_ResultList[0]=0;
		//printf("\nLevel: %d, items: %d",level, itemsCount);
		Kernel_RangeQueryBFS<<<itemsCount,blockDim.x,sMemSize>>>(!isLeaf,params);
	}
#else
printf("\nCritical Error. Insufficient CUDA CAPABILITY.");
#endif
}

void cCudaProcessor35::RangeQuery_DBFS_35(uint* nodeIndices, uint nodeCount, uint level, cCudaParamsRecursive params)
{

}
void cCudaProcessor35::RangeQuery_BFS_35(cCudaParamsBasic_List params, uint &outCount, uint** items, bool batch, uint rootIndex)
{
	assert(cGpuConst::RESULT_STRUCT == cGpuResultStructure::DistinctList);
	uint sMemSize = 0;
	uint SearchArrayCount = 1;
	cudaError_t err;
	if (batch)
		sMemSize = params.QueriesInBatch * (params.Dimension * params.NodeCapacity * sizeof(uint)) + params.NodeCapacity; //array of relevant nodes
	else
		sMemSize = params.Dimension * params.NodeCapacity * sizeof(uint);
	assert(sMemSize > 0);

	for (uint i = 0; i < params.NoLevels; i++)
	{
		Kernel_RangeQueryBFS_Controller<<<1, cGpuConst::THREADS_PER_BLOCK >>>(i, rootIndex, params, sMemSize);
	}

	err = cudaMemcpy(&SearchArrayCount, params.D_ResultList, sizeof(uint), cudaMemcpyDeviceToHost);
	*items = (uint*)malloc(SearchArrayCount*sizeof(uint));
	CUDA_CHECK(cudaMemcpy(*items, params.D_ResultList + 1, SearchArrayCount*sizeof(uint), cudaMemcpyDeviceToHost), "Copying resultset from GPU");
	outCount = SearchArrayCount;
}
void cCudaProcessor35::RangeQuery_DFS_35(uint* nodeIndices, uint nodeCount, uint level, cCudaParamsRecursive params)
{

}
