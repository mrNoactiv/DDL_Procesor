#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "dstruct/paged/cuda/cCudaProcessor.cuh"
#include "dstruct/paged/cuda/cGpuConst.h"
#include "dstruct/paged/cuda/cCudaParams.h"
#include "dstruct/paged/cuda/cCudaTimer.h"
#include "dstruct/paged/cuda/cCudaCommonKernels.cuh"



__global__ void Kernel_RangeQuerySQ(cCudaParams params)
{
	bool isInner = params.NodeType == cGpuConst::NODE_HEADER;
	extern __shared__ int sMem[];
	bool *sResults = (bool*)&sMem[0];
	uint tid = threadIdx.x;
	uint dimOffset;

	uint bucketOrder = params.D_SearchOffsets[blockIdx.x];

	int tuplesCount = params.D_Inputs[bucketOrder * (params.BlockSize / sizeof(uint))];
	uint* D_input = params.D_Inputs + bucketOrder * (params.BlockSize / sizeof(uint)) + 1; //first number is number of items in block
	uint* D_input2;
	if (isInner)
	{
		D_input2 = params.D_Inputs + bucketOrder * (params.BlockSize / sizeof(uint)) + 1 + tuplesCount*params.Dimension;
	}
	uint threadOffset = threadIdx.x;
	while (threadOffset < tuplesCount *params.Dimension)
	{
		dimOffset = threadOffset % params.Dimension;

		if (params.NodeType == cGpuConst::NODE_LEAF)
		{
			sResults[threadOffset] = !(NOTININTERVAL(C_RQElement[dimOffset].minimum, C_RQElement[dimOffset].maximum, D_input[threadOffset]));
		}
		else
		{
			sResults[threadOffset] = !(NOTINTERSECTED(D_input[threadOffset], D_input2[threadOffset], C_RQElement[dimOffset].minimum, C_RQElement[dimOffset].maximum));
			//printf("\ntid;%d;ql;%d;qh;%d;r1;%d;r2;%d;result;%d", tid, C_RQElement[dimOffset].minimum, C_RQElement[dimOffset].maximum, D_input[tid], D_input2[tid], sResults[tid] ? 1 : 0);
		}
		threadOffset += blockDim.x;
	}
	__syncthreads();


	//sumarize
	threadOffset = threadIdx.x;
	while (threadOffset < tuplesCount)
	{
		uint dataOffset = threadOffset * params.Dimension;
		bool threadResult = Device_ComputeThreadResult(sResults, dataOffset, params.Dimension);

		if (cGpuConst::RESULT_STRUCT == cGpuResultStructure::DistinctList)
		{
			if (threadResult)
			{
				Device_AddToResultList(atomicAdd(params.D_ResultList, 1), params.D_ResultList, params.D_ChildIndices, params.NodeCapacity, bucketOrder, isInner, threadOffset);
			}
		}
		else
		{
			params.D_Results[blockIdx.x * params.NodeCapacity + threadOffset] = threadResult;
		}
		threadOffset += blockDim.x;

	}

}
__global__ void Kernel_RangeQuerySequencial(cCudaParams params)
{
	DATATYPE* inputs = params.D_Inputs;
	//unsigned int threadIdx.x = threadIdx.x;
	unsigned int chunkFirstInput = blockIdx.x * params.ThreadsPerBlock * params.SequencialNoChunks;	//Every block should process params.ThreadsPerBlock input vectors in one chunk
	unsigned int chunkDataOffset = chunkFirstInput *  params.Dimension;
	unsigned int threadDataOffset = threadIdx.x;
	unsigned int threadQueryOffset;
	unsigned int noInputsInChunk;
	extern __shared__ unsigned char blockBuffer[];
	bool *rqResults = (bool*)&blockBuffer[0];

	for (unsigned int chunk = 0; chunk < params.SequencialNoChunks; chunk++)
	{
		noInputsInChunk = MINIMUM(params.ThreadsPerBlock, params.SequencialNoInputs - chunkFirstInput);
		threadDataOffset = threadIdx.x;							//Data offset of a thread

		//TPE - ThreadPerElement range C_RQElement test
#pragma unroll 8
		for (unsigned int i = 0; i < params.Dimension; i++)
		{
			if (threadDataOffset < (noInputsInChunk * params.Dimension))
			{
				threadQueryOffset = threadDataOffset % params.Dimension;			//MODULO :-(
				if (NOTININTERVAL(C_RQElement[threadQueryOffset].minimum, C_RQElement[threadQueryOffset].maximum, inputs[chunkDataOffset + threadDataOffset]))
				{
					rqResults[threadDataOffset] = false;
					//printf("\nthread ;%d; process value ;%d; at ;%d; result: ;FALSE",threadIdx.x,inputs[chunkDataOffset+threadDataOffset],chunkDataOffset+threadDataOffset);
				}
				else
				{
					rqResults[threadDataOffset] = true;
					//printf("\nthread ;%d; process value ;%d; at ;%d; result: ;TRUE",threadIdx.x,inputs[chunkDataOffset+threadDataOffset],chunkDataOffset+threadDataOffset);
				}
			}
			threadDataOffset += params.ThreadsPerBlock;							//Shifts thread data offset
		}
		__syncthreads();

		//TPI - ThreadPerInput summarization in shared memory
		threadDataOffset = threadIdx.x * params.Dimension;							//New thread data offset for TPI computation
		bool threadResult = true;
		if (threadIdx.x < noInputsInChunk)
		{
			for (unsigned int i = threadDataOffset; i < threadDataOffset + params.Dimension; i++)
			{
				threadResult &= rqResults[i];
			}
			params.D_Results[chunkFirstInput + threadIdx.x] = threadResult;							//Stores the result to the params.D_ResultVector vector in global memory
			/*if (threadResult)
			printf("\n%d; TRUE",chunkFirstInput+threadIdx.x);
			else
			printf("\n%d; FALSE",chunkFirstInput+threadIdx.x);*/


		}
		//Shift the beginning of a block to the next chunk
		chunkFirstInput += noInputsInChunk;
		chunkDataOffset = chunkFirstInput *  params.Dimension;
	}
}
sKernelSetting cCudaProcessor::PrepareKernelSettingsLevel(cCudaParams params)
{
	sKernelSetting ks;
	if (params.Mode == cGpuQueryType::BATCHQUERY)
	{
		ks.dimGrid = dim3(params.TBCount, 1, 1);
		uint tpb = 256;
		if (cGpuConst::USE_2DKERNEL)
		{
			tpb = params.DeviceProperties.maxThreadsPerBlock / params.QueriesInBatch;
			if (tpb * params.QueriesInBatch > params.DeviceProperties.maxThreadsPerBlock)
				printf("\nCUDA Critical Error! Cannot plan 2D kernel. Maximum of threads per block exceeded.");
			ks.dimBlock = dim3(tpb, params.QueriesInBatch, 1);
		}
		else
		{
			ks.dimBlock = dim3(tpb, 1, 1);
		}

		unsigned int SMemSize = (params.ResultRowSize*params.Dimension + 4)* sizeof(int); //+4 is reserved for smem variables 
		if (params.NodeType == 1) //inner node
			SMemSize *= 2;
		SMemSize += +params.NodeCapacity * params.Dimension * params.QueriesInBatch * sizeof(bool); //results array
		if (SMemSize > params.DeviceProperties.sharedMemPerBlock)
			printf("\nCUDA Critical Error! Shared memory size limit has exceeded maximum amount.");
		ks.sharedMemSize = SMemSize;// (unsigned int)params.DeviceProperties.sharedMemPerBlock;
	}
	else
	{
		ks.dimGrid = dim3(params.TBCount, 1, 1);
		ks.dimBlock = dim3(params.ThreadsPerBlock, 1, 1);
		unsigned int SMemSize = params.NodeCapacity * params.NoChunks * params.Dimension * sizeof(bool);
		if (SMemSize > params.DeviceProperties.sharedMemPerBlock)
			printf("\nCUDA Critical Error! Shared memory size limit has exceeded maximum amount.");
		ks.sharedMemSize = SMemSize;// (unsigned int)params.DeviceProperties.sharedMemPerBlock;
	}
	#if (PRINT==1)
	ks.print();
	#endif
	return ks;
}

sKernelSetting cCudaProcessor::PrepareKernelSettingsSequencial(cCudaParams params)
{
	sKernelSetting ks;
	unsigned int SMemSize = params.ThreadsPerBlock * params.Dimension * sizeof(bool);
	ks.blockSize = params.ThreadsPerBlock;
	ks.dimBlock = dim3(params.ThreadsPerBlock, 1, 1);
	if (SMemSize > params.DeviceProperties.sharedMemPerBlock)
		printf("\nCUDA Critical Error! Shared memory size limit has exceeded maximum amount.");
	ks.sharedMemSize = SMemSize;// (unsigned int)params.DeviceProperties.sharedMemPerBlock;

	ks.noChunks = params.SequencialNoChunks;

	unsigned int noBlocks = getNumberOfParts(params.SequencialNoInputs, params.ThreadsPerBlock * ks.noChunks);
	//noBlocks = 256; //temporary
	if (noBlocks > params.DeviceProperties.maxGridSize[0])
	{
		unsigned int multiplicator = noBlocks / params.DeviceProperties.maxGridSize[0];
		if ((noBlocks % params.DeviceProperties.maxGridSize[0]) != 0)
			multiplicator++;
		ks.noChunks *= multiplicator;
		ks.dimGrid = getNumberOfParts(params.SequencialNoInputs, params.ThreadsPerBlock * ks.noChunks);
	}
	else
	{
		ks.dimGrid = dim3(noBlocks, 1, 1);
	}
	return ks;
}


void cCudaProcessor::CopyRQToConstantMemory(unsigned int dim, unsigned int *pql, unsigned int *pqh, unsigned int count)
{
	RQElement* rqe = new RQElement[dim*count];
	for (unsigned int j = 0; j < dim*count; j++)
	{
		rqe[j].minimum = pql[j];
		rqe[j].maximum = pqh[j];
	}
	CUDA_CHECK(cudaMemcpyToSymbol(C_RQElement, rqe, dim * count * sizeof(RQElement), 0, cudaMemcpyHostToDevice), "Copy RQ to constant Memory");
	delete rqe;
}

void cCudaProcessor::RangeQuery_Level(cCudaParams params)
{
	cudaError_t e;
	sKernelSetting ks = PrepareKernelSettingsLevel(params);
	cudaGetLastError();
	if (params.Mode == cGpuQueryType::BATCHQUERY)
	{
		RangeQuery_LevelBQ(ks,params);
	}
	else
	{
		RangeQuery_LevelSQ(ks,params);
	}
	e = cudaGetLastError();
	if (e != cudaSuccess)
		printf("\nCUDA Critical Error! Exception when launching kernel: %s\n", cudaGetErrorString(e));
}
void cCudaProcessor::RangeQuery_LevelBQ(sKernelSetting ks, cCudaParams params)
{
	/*cCudaParams params = *p;
	if (cGpuConst::USE_2DKERNEL)
	{
		if (cGpuConst::BATCH_ONLY_RELEVANT)
		{
			printf("\nCritical error: GPU search method not implemented!!!");
		}
		else
		{
			if (cGpuConst::RESULT_STRUCT == cGpuResultStructure::DistinctList)
			{
				Data_ClearResultList << <1, 1 >> >(params);
				rq_batch_2d_all<4> << <ks.dimGrid, ks.dimBlock, ks.sharedMemSize >> >(params);
			}
			else
			{
				rq_batch_2d_all<4> << <ks.dimGrid, ks.dimBlock, ks.sharedMemSize >> >(params);
			}
		}
	}
	else
	{
		if (cGpuConst::BATCH_ONLY_RELEVANT)
		{
			printf("\nCritical error: GPU search method not implemented!!!");
		}
		else
		{
			if (cGpuConst::RESULT_STRUCT == cGpuResultStructure::DistinctList)
			{
				Data_ClearResultList << <1, 1 >> >(params);
				rq_batch_1d_all<4> << <ks.dimGrid, ks.dimBlock, ks.sharedMemSize >> >(params);
			}
			else
			{
				rq_batch_1d_all<4> << <ks.dimGrid, ks.dimBlock, ks.sharedMemSize >> >(params);
			}
		}
	}*/

}
void cCudaProcessor::RangeQuery_LevelSQ(sKernelSetting ks, cCudaParams params)
{
	if (cGpuConst::RESULT_STRUCT == cGpuResultStructure::DistinctList)
	{
		Kernel_ClearResultList <<<1, 1 >>>(params);
		Kernel_RangeQuerySQ <<<ks.dimGrid, ks.dimBlock, ks.sharedMemSize >>>(params);
	}
	else
	{
		Kernel_RangeQuerySQ<<<ks.dimGrid, ks.dimBlock, ks.sharedMemSize >>>(params);
	}
}

uint cCudaProcessor::RangeQuery_Sequential(cCudaParams params)
{
	uint resultSize = 0;
	sKernelSetting ks = PrepareKernelSettingsSequencial(params);
	Kernel_RangeQuerySequencial<<<ks.dimGrid, ks.dimBlock, ks.sharedMemSize >>>(params);
	size_t sizeResults = params.SequencialNoInputs * sizeof(bool);
	bool* h_result = (bool*)malloc(sizeResults);
	cudaMemcpy(h_result, params.D_Results, sizeResults, cudaMemcpyDeviceToHost);
	for (uint i = 0; i<params.SequencialNoInputs; i++)
	{
		if (h_result[i])
			resultSize++;
	}
	return resultSize;
}

