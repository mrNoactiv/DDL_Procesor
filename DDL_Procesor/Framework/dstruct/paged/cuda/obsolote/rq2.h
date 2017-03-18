#ifndef __RQ_H
#define __RQ_H

#include "cudaDefs.h"
#include "cudaDefs.h"
#include "utils_Kernel.h"
#include "lib/cuda/cutil_inline.h"
#include "cGpuConst.h"
#include "cudasharedmem.h"
#include "cCudaParams.h"

//--------------------------------------------------------------------------
// TEMPLATE FUNCTIONS
//--------------------------------------------------------------------------

static __device__ inline unsigned int uilog2(unsigned int x) 
{
     unsigned int l=0;
     if(x >= 1<<16) { x>>=16; l|=16; }
     if(x >= 1<<8) { x>>=8; l|=8; }
     if(x >= 1<<4) { x>>=4; l|=4; }
     if(x >= 1<<2) { x>>=2; l|=2; }
     if(x >= 1<<1) l|=1;
     return l;
 }


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Range Query Hypercube Fit Test.</summary>
/// <remarks>	Gajdi, 25.07.2011. </remarks>
/// <param name="inputs">	[in] If non-null, the pointer to the buffered data of input vectors.</param>
/// <param name="noChunks">	[in] Total count of input vectors.</param>
/// <param name="noChunks">	[in] Total count of input data blocks that will be processed by a single CUDA thread block.</param>
/// <param name="query">	[in] If non-null, the pointer to the buffered data of range-query vectors.</param>
/// <param name="output">	[in,out] If non-null, the pointer to the buffered data of boolean values (results). </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

//__global__ void rqHFT(const DATATYPE* inputs, const unsigned int noInputs, const unsigned int noChunks, const RQElement* query,  bool *outputs);

//old implementation for cuda capability < 2.0, body is implemented in cudaRunner.cu
__global__ void rqHFT_v13(const DATATYPE* inputs, const unsigned int noInputs, const unsigned int noChunks, const RQElement* query,  bool *outputs, unsigned int tpb);
//implementation for cuda capability >= 2.0
template<unsigned int wordSize>__global__ void rqHFT(const DATATYPE* inputs, const unsigned int noInputs, const unsigned int noChunks, const RQElement* query,  bool* outputs, unsigned int tpb)
{
	SharedMemory<unsigned char> blockBuffer;	////WORKAROUND FOR TEMPLATES!!!    
	//extern __shared__ unsigned char blockBuffer[];
#ifndef CUDA_CONSTANT_MEM
	RQElement *rq = (RQElement*)&blockBuffer[0]; //shared memory 
	//bool *rqResults = (bool*)&blockBuffer[sizeof(RQElement)*C_dataInfo.dim];		//rqResults is offset in shared memory
	bool *rqResults = (bool*)&rq[C_dataInfo.dim];									//rqResults is offset in shared memory
	//uint1 *rqResultWord = (uint1*)&rqResults[0];									//has the same offset as rqResult
#else
	bool *rqResults = (bool*)&blockBuffer[0];									//rqResults is first in shared memory
#endif

	unsigned int tid = threadIdx.x;
	unsigned int chunkFirstInput = blockIdx.x * tpb * noChunks;	//Every block should process tpb input vectors in one chunk
	unsigned int chunkDataOffset = chunkFirstInput *  C_dataInfo.dim;
	unsigned int threadDataOffset = tid;
	unsigned int threadQueryOffset;
	unsigned int noInputsInChunk;

#ifndef CUDA_CONSTANT_MEM //if we use constant memory there is no need copy QR into shared memory
	if (tid <  C_dataInfo.dim) //copy Query Rectangle into shared memory
	{
		rq[tid] = query[tid];
	}
	__syncthreads();
#endif
	for (unsigned int chunk = 0; chunk<noChunks; chunk++)
	{
		noInputsInChunk = MINIMUM(tpb, noInputs-chunkFirstInput);
		threadDataOffset = tid;							//Data offset of a thread

//TPE - ThreadPerElement range query test
		#pragma unroll 8
		for (unsigned int i=0; i<C_dataInfo.dim; i++)
		{
			if (threadDataOffset < (noInputsInChunk * C_dataInfo.dim))
			{
				//printf("\nThread: ;%d; process input at position;:%d;, stores in: ;%d",tid,chunkDataOffset+threadDataOffset,threadDataOffset);
				//printf("\nThread: ;%d; process value;:%d;",tid,inputs[chunkDataOffset+threadDataOffset]);
				threadQueryOffset = threadDataOffset % C_dataInfo.dim;			//MODULO :-(
#ifndef CUDA_CONSTANT_MEM //if we use constant memory there is no need copy QR into shared memory
				if (NOTININTERVAL(rq[threadQueryOffset].minimum, rq[threadQueryOffset].maximum, inputs[chunkDataOffset+threadDataOffset])) 
#else
				if (NOTININTERVAL(C_RQElement[threadQueryOffset].minimum, C_RQElement[threadQueryOffset].maximum, inputs[chunkDataOffset+threadDataOffset])) 
#endif
				{
					rqResults[threadDataOffset] = false;
				}
				else
					rqResults[threadDataOffset] = true;
			}
			threadDataOffset += tpb;							//Shifts thread data offset
		}
		__syncthreads();

//TPI - ThreadPerInput summarization in shared memory
		threadDataOffset = tid * C_dataInfo.dim;							//New thread data offset for TPI computation
		bool threadResult = true;
		if (tid < noInputsInChunk)
		{
			//printf("\nthreadDataOffset=%d, dim=%d",threadDataOffset,C_dataInfo.dim);
			for (unsigned int i=threadDataOffset; i<threadDataOffset+C_dataInfo.dim; i++)
			{
				threadResult &= rqResults[i];
			}
			outputs[chunkFirstInput+tid] = threadResult;
			//Stores the result to the outputs vector in global memory
		}
		//Shift the beginning of a block to the next chunk
		chunkFirstInput+=noInputsInChunk;
		chunkDataOffset = chunkFirstInput *  C_dataInfo.dim;
	}

	
}
__global__ void rqHFT_PrintMemory(const DATATYPE* inputs,const unsigned int* memOffset, const unsigned int noInputs, const unsigned int noChunks, const RQElement* query,  bool* outputs)
{
	
	//thread 0 prints global memory
	if (threadIdx.x ==0)
	{
					printf("\n");
		for (int i=0;i < noInputs;i++)
		{
				if (i% 11 == 0)
					printf("\n");
			printf("%d,",inputs[i]);
		}
	}
	printf("\n\nhotovo\n\n");
}
/** 
*	Search one block of tuples on GPU. Every ThreadBlock processes few tuples of a scanned block.
*	\param inputs Pointer to GPU's global memory where the particular block starts.
*	\param noInputs Number of tuples in particular block.
*	\param outputs Output variable representing result vector.
**/
template<unsigned int wordSize>__global__ void rqHFT_SearchBlock(DATATYPE* inputs, const unsigned int noInputs, bool* outputs,unsigned int tpb)
{
	//the query rectangle is in constant memory
	//SharedMemory<unsigned char> blockBuffer;	////WORKAROUND FOR TEMPLATES!!!    
	extern __shared__ unsigned char blockBuffer[];
	bool *rqResults = (bool*)&blockBuffer[0];									//rqResults is first in shared memory

	unsigned int tid = threadIdx.x;
	unsigned int chunkFirstInput = blockIdx.x * tpb ;//+ (memOffset/C_dataInfo.dim/sizeof(DATATYPE)) ;	//Every block should process tpb input vectors in one chunk
	unsigned int chunkDataOffset = chunkFirstInput *  C_dataInfo.dim;
	unsigned int threadDataOffset = tid;
	unsigned int threadQueryOffset;
	unsigned int noInputsInChunk;

#if (CUDA_DEBUG >= 1)
	if (threadIdx.x ==0)
	{
		printf("\n\nPrinting GPU's global memory to process:");
		for (int i=0;i < noInputs*C_dataInfo.dim;i++)
		{
			if (i% C_dataInfo.dim == 0)
				printf("\n");
			printf("%d,",(inputs)[i]);
		}
		printf("\n\n");

	}
#endif


	//there is no need to copy query rectangle into shared memory.
	//there is no need to iterate through number of chunks. Every TB proccesses only 1 chunk
	noInputsInChunk = MINIMUM(tpb, noInputs-chunkFirstInput);
	threadDataOffset = tid;							//Data offset of a thread

#pragma unroll 8 //TPE - ThreadPerElement range query test
	for (unsigned int i=0; i<C_dataInfo.dim; i++) 
	{
		if (threadDataOffset < (noInputsInChunk * C_dataInfo.dim))
		{
			//printf("\nThread: ;%d; process input at position;:%d;, stores in: ;%d",tid,chunkDataOffset+threadDataOffset,threadDataOffset);
			threadQueryOffset = threadDataOffset % C_dataInfo.dim;			//MODULO :-(
			if (NOTININTERVAL(C_RQElement[threadQueryOffset].minimum, C_RQElement[threadQueryOffset].maximum, inputs[chunkDataOffset+threadDataOffset])) 
			{
				rqResults[threadDataOffset] = false;
#if (CUDA_DEBUG >= 1)
				printf("\nThread: ;%d; process value:;%d;false",tid,inputs[chunkDataOffset+threadDataOffset]);
#endif
			}
			else
			{
				rqResults[threadDataOffset] = true;
#if (CUDA_DEBUG >= 1)
				printf("\nThread: ;%d; process value:;%d;true",tid,inputs[chunkDataOffset+threadDataOffset]);
#endif
			}
		}
		threadDataOffset += tpb;							//Shifts thread data offset
	}
	__syncthreads();

	//TPI - ThreadPerInput summarization in shared memory
	threadDataOffset = tid * C_dataInfo.dim;							//New thread data offset for TPI computation
	bool threadResult = true;
	if (tid < noInputsInChunk)
	{
		//printf("\nthreadDataOffset=%d, dim=%d",threadDataOffset,C_dataInfo.dim);
		for (unsigned int i=threadDataOffset; i<threadDataOffset+C_dataInfo.dim; i++)
		{
			threadResult &= rqResults[i];
		}
		outputs[chunkFirstInput+tid] = threadResult;
#if (CUDA_DEBUG >= 1)
		if (threadResult)
			printf("\nThread: ;%d; summary for tuple number:;%d;true",tid,chunkFirstInput+tid);
		else
			printf("\nThread: ;%d; summary for tuple number:;%d;false",tid,chunkFirstInput+tid);
#endif

		//Stores the result to the outputs vector in global memory
	}
	//Shift the beginning of a block to the next chunk
	chunkFirstInput+=noInputsInChunk;
	chunkDataOffset = chunkFirstInput *  C_dataInfo.dim;

}
template<unsigned int wordSize>__global__ void rqHFT_SearchBlockOffsets(DATATYPE* g_inputs,unsigned int g_noInputs, unsigned int* offsets,unsigned int* offsetSizes, bool* outputs, unsigned int tpb,bool debugFlag)
{
//#define CUDA_DEBUG 1
	//the query rectangle is in constant memory
	//SharedMemory<unsigned char> blockBuffer;	////WORKAROUND FOR TEMPLATES!!!    
	extern __shared__ unsigned char blockBuffer[];
	bool *rqResults = (bool*)&blockBuffer[0];									//rqResults is first in shared memory

	unsigned int tid = threadIdx.x;
	//unsigned int chunkFirstInput = blockIdx.x * tpb ;//+ (memOffset/C_dataInfo.dim/sizeof(DATATYPE)) ;	//Every block should process tpb input vectors in one chunk
	//unsigned int chunkDataOffset = chunkFirstInput *  C_dataInfo.dim;
	unsigned int threadDataOffset = tid;
	unsigned int threadQueryOffset;
	//unsigned int noTuplesInBlock;
	unsigned int blockId = blockIdx.x;
	unsigned int offsetSize = offsetSizes[blockId];
	unsigned int offset = offsets[blockId];
	DATATYPE* inputs = g_inputs+offset;
	unsigned int noTuplesInBlock = offsetSize / sizeof(DATATYPE)/ C_dataInfo.dim;
	unsigned int tuplesBeforeThisBlock = offset / C_dataInfo.dim; //+ (memOffset/C_dataInfo.dim/sizeof(DATATYPE)) ;	//Every block should process tpb input vectors in one chunk
	unsigned int debugBlockId = 1;
	if (threadIdx.x ==0 && debugFlag )
	{
		//printf("\n\n%d,%d,%dBLOCK: %d. Printing GPU's global memory to process:\nOffset = %d\nOffsetSize = %d\nNoOfTuplesInBlock = %d",C_RQElement[0].minimum,C_RQElement[1].minimum,C_RQElement[2].minimum, blockId,offset,offsetSize,noTuplesInBlock);
		printf("\n\nBLOCK: %d, Offset = %d\nOffsetSize = %d\nNoOfTuplesInBlock = %d, tuplesBefore: %d",blockId,offset,offsetSize,noTuplesInBlock,tuplesBeforeThisBlock);
	}
	__syncthreads();

	//there is no need to copy query rectangle into shared memory.
	//there is no need to iterate through number of chunks. Every TB proccesses only 1 chunk
	//threadDataOffset = tid;							//Data offset of a thread

#pragma unroll 8 //TPE - ThreadPerElement range query test
	for (unsigned int i=0; i<C_dataInfo.dim; i++) 
	{
		if (threadDataOffset < (noTuplesInBlock * C_dataInfo.dim))
		{
			//printf("\nThread: ;%d; process input at position;:%d;, stores in: ;%d",tid,chunkDataOffset+threadDataOffset,threadDataOffset);
			threadQueryOffset = threadDataOffset % C_dataInfo.dim;			//MODULO :-(
			if (NOTININTERVAL(C_RQElement[threadQueryOffset].minimum, C_RQElement[threadQueryOffset].maximum, inputs[threadDataOffset])) 
			{
				rqResults[threadDataOffset] = false;
#if (CUDA_DEBUG >= 1)
				if (blockId == debugBlockId)
				{
					printf("\nBlock: ;%d;Thread: ;%d; process value:;%d;false",blockId,tid,inputs[+threadDataOffset]);
				}
#endif
			}
			else
			{
				rqResults[threadDataOffset] = true;
#if (CUDA_DEBUG >= 1 )
				if (blockId == debugBlockId)
				{
					printf("\nnBlock: ;%d;Thread: ;%d; process value:;%d;true",blockId,tid,inputs[threadDataOffset]);
				}
#endif
			}
		}
		threadDataOffset += tpb;							//Shifts thread data offset
	}
	__syncthreads();

	//TPI - ThreadPerInput summarization in shared memory
	threadDataOffset = tid * C_dataInfo.dim;	//New thread data offset for TPI computation
	//threadDataOffset is threads.idx < dim. Each thread summarize next |dim| values
	bool threadResult = true;
	if (tid < noTuplesInBlock)
	{
		//printf("\nthreadDataOffset=%d, dim=%d",threadDataOffset,C_dataInfo.dim);
		for (unsigned int i=threadDataOffset; i<threadDataOffset+C_dataInfo.dim; i++)
		{
			threadResult &= rqResults[i];
		}
		outputs[tuplesBeforeThisBlock+tid] = threadResult;
		if (debugFlag)
		{
			if (threadResult)
			{
				printf("\nBlock: ;%d;Thread: ;%d; summary for tuple number:;%d;true",blockId,tid,tuplesBeforeThisBlock+tid);
			}
			else
				;//printf("\nBlock: ;%d;Thread: ;%d; summary for tuple number:;%d;false",blockId,tid,tuplesBeforeThisBlock+tid);
		}

		//Stores the result to the outputs vector in global memory
	}
	//Shift the beginning of a block to the next chunk
	//chunkFirstInput+=noTuplesInBlock;
	//chunkDataOffset = chunkFirstInput *  C_dataInfo.dim;
	

}

template<unsigned int wordSize>__global__ void rqHFT_Universal(DATATYPE* g_inputs,unsigned int blocksCount, unsigned int* offsets,unsigned int* offsetSizes,cCudaParams params, bool* outputs)
{
	//the query rectangle is in constant memory
	extern __shared__ unsigned char blockBuffer[];
	bool *rqResults = (bool*)&blockBuffer[0];//rqResults is first in shared memory
	unsigned int tid = threadIdx.x;
	//unsigned int chunkFirstInput = blockIdx.x * tpb ;//+ (memOffset/C_dataInfo.dim/sizeof(DATATYPE)) ;	//Every block should process tpb input vectors in one chunk
	//unsigned int chunkDataOffset = chunkFirstInput *  C_dataInfo.dim;
	unsigned int threadDataOffset = tid;
	unsigned int threadQueryOffset;
	//unsigned int noTuplesInBlock;
	unsigned int blockId = blockIdx.x;

	for (unsigned int ch= 0; ch < params.NoOfChunks;ch++)
	{
		
		unsigned int threadDataOffset = tid;
		unsigned int offsetSize; 
		unsigned int offset; 
		DATATYPE* inputs;
		if (params.AlgorithmType == GPU_ALGORITHM_TYPE::SINGLE_BLOCK)
		{
			offsetSize = params.SingleBlockOffsetSize;
			offset = params.SingleBlockOffset;
			inputs = g_inputs+offset;
			//tuplesBeforeThisBlock=blockIdx.x * params.ThreadsPerBlock;
		}
		else
		{
			offsetSize = offsetSizes[blockId * params.NoOfChunks+ch];
			offset = offsets[blockId * params.NoOfChunks +ch];
			inputs = g_inputs+offset;
		}
		unsigned int tuplesBeforeThisBlock = offset / C_dataInfo.dim;
		unsigned int noTuplesInBlock = offsetSize / sizeof(DATATYPE)/ C_dataInfo.dim;

#pragma unroll 8 //TPE - ThreadPerElement range query test
		for (unsigned int i=0; i<C_dataInfo.dim; i++) 
		{
			if (threadDataOffset < (noTuplesInBlock * C_dataInfo.dim))
			{
				//printf("\nThread: ;%d; process input at position;:%d;, stores in: ;%d",tid,chunkDataOffset+threadDataOffset,threadDataOffset);
				threadQueryOffset = threadDataOffset % C_dataInfo.dim;			//MODULO :-(
				if (NOTININTERVAL(C_RQElement[threadQueryOffset].minimum, C_RQElement[threadQueryOffset].maximum, inputs[threadDataOffset])) 
				{
					rqResults[threadDataOffset] = false;
#if (CUDA_DEBUG >= 1)
					if (blockId == debugBlockId)
					{
						printf("\nBlock: ;%d;Thread: ;%d; process value:;%d;false",blockId,tid,inputs[+threadDataOffset]);
					}
#endif
				}
				else
				{
					rqResults[threadDataOffset] = true;
#if (CUDA_DEBUG >= 1 )
					if (blockId == debugBlockId)
					{
						printf("\nnBlock: ;%d;Thread: ;%d; process value:;%d;true",blockId,tid,inputs[threadDataOffset]);
					}
#endif
				}
			}
			threadDataOffset += params.ThreadsPerBlock;							//Shifts thread data offset
		}
		__syncthreads();
		//TPI - ThreadPerInput summarization in shared memory
		threadDataOffset = tid * C_dataInfo.dim;	//New thread data offset for TPI computation
		//threadDataOffset is threads.idx < dim. Each thread summarize next |dim| values
		bool threadResult = true;
		if (tid < noTuplesInBlock)
		{
			//printf("\nthreadDataOffset=%d, dim=%d",threadDataOffset,C_dataInfo.dim);
			for (unsigned int i=threadDataOffset; i<threadDataOffset+C_dataInfo.dim; i++)
			{
				threadResult &= rqResults[i];
			}
			outputs[tuplesBeforeThisBlock+tid] = threadResult;
			if (params.DebugFlag)
			{
				if (threadResult)
				{
					//printf("\nUkladam TRUE na pozici: %d, tuples in block: %d",tid,offset);
					//printf("\nBlock: ;%d;Thread: ;%d;Chunk: ;%d; summary for tuple number:;%d;true",blockId,tid,ch,tuplesBeforeThisBlock+tid);
				}
				else
					;//printf("\nBlock: ;%d;Thread: ;%d;Chunk: ;%d; summary for tuple number:;%d;false",blockId,tid,ch,tuplesBeforeThisBlock+tid);
			}

			//Stores the result to the outputs vector in global memory
		}
		//Shift the beginning of a block to the next chunk
		//chunkFirstInput+=noTuplesInBlock;
		//chunkDataOffset = chunkFirstInput *  C_dataInfo.dim;
	}
	/*__syncthreads();
	printf("\n GPU ResSize: %d", rs);*/
	/*unsigned int counter;
	if (blockId == 0 && tid == 0)
	{
		for(int i=0;i < sizeofD;i++)
		{
			if (D_bufferResults
		}
	}*/
}
#endif