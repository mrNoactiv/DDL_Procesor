#ifndef __RQ_H
#define __RQ_H

#include "cudaDefs.h"
#include "cudaDefs.h"
#include "utils_Kernel.h"
#include "lib/cuda/cutil_inline.h"

#include "cudasharedmem.h"


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
__global__ void rqHFT_v13(const DATATYPE* inputs, const unsigned int noInputs, const unsigned int noChunks, const RQElement* query,  bool *outputs);
//implementation for cuda capability >= 2.0
template<unsigned int wordSize>__global__ void rqHFT(const DATATYPE* inputs, const unsigned int noInputs, const unsigned int noChunks, const RQElement* query,  bool *outputs,int* resultSize)
{
	SharedMemory<unsigned char> blockBuffer;	////WORKAROUND FOR TEMPLATES!!!    
	//extern __shared__ unsigned char blockBuffer[];
	

	RQElement *rq = (RQElement*)&blockBuffer[0];
	//bool *rqResults = (bool*)&blockBuffer[sizeof(RQElement)*C_dataInfo.dim];		//rqResults is offset in shared memory
	bool *rqResults = (bool*)&rq[C_dataInfo.dim];									//rqResults is offset in shared memory
	//uint1 *rqResultWord = (uint1*)&rqResults[0];									//has the same offset as rqResult

	unsigned int tid = threadIdx.x;
	unsigned int chunkFirstInput = blockIdx.x * THREADS_PER_BLOCK * noChunks;	//Every block should process THREADS_PER_BLOCK input vectors in one chunk
	unsigned int chunkDataOffset = chunkFirstInput *  C_dataInfo.dim;
	unsigned int threadDataOffset = tid;
	unsigned int threadQueryOffset;
	unsigned int noInputsInChunk;


	if (tid <  C_dataInfo.dim) //copy range query rectangle into shared memory
	{
		rq[tid] = query[tid];
	}
	__syncthreads();

	for (unsigned int chunk = 0; chunk<noChunks; chunk++)
	{
		noInputsInChunk = MINIMUM(THREADS_PER_BLOCK, noInputs-chunkFirstInput);
		threadDataOffset = tid;							//Data offset of a thread

//TPE - ThreadPerElement range query test
		#pragma unroll 8
		for (unsigned int i=0; i<C_dataInfo.dim; i++)
		{
			if (threadDataOffset < (noInputsInChunk * C_dataInfo.dim))
			{
				threadQueryOffset = threadDataOffset % C_dataInfo.dim;			//MODULO :-(
				if (NOTININTERVAL(rq[threadQueryOffset].minimum, rq[threadQueryOffset].maximum, inputs[chunkDataOffset+threadDataOffset])) 
				{
					rqResults[threadDataOffset] = false;
				}
				else
					rqResults[threadDataOffset] = true;
			}
			threadDataOffset += THREADS_PER_BLOCK;							//Shifts thread data offset
		}
		__syncthreads();

//TPI - ThreadPerInput summarization in shared memory
		threadDataOffset = tid * C_dataInfo.dim;							//New thread data offset for TPI computation
		bool threadResult = true;
		if (tid < noInputsInChunk)
		{
			for (unsigned int i=threadDataOffset; i<threadDataOffset+C_dataInfo.dim; i++)
			{
				threadResult &= rqResults[i];
			}
			outputs[chunkFirstInput+tid] = threadResult;							//Stores the result to the outputs vector in global memory
		}
		//Shift the beginning of a block to the next chunk
		chunkFirstInput+=noInputsInChunk;
		chunkDataOffset = chunkFirstInput *  C_dataInfo.dim;
	}

	
}

#endif