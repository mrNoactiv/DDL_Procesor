#ifdef CUDA_ENABLED
#ifndef __globalDefs_h__
#define __globalDefs_h__

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <cstdlib>
#include <ctime> 
#include <cmath>
#include "lib/cuda/cutil_inline.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "dstruct/paged/cuda/cCudaParams.h"
#include "common/cCommon.h"
#define MINIMUM(a, b) ((a) < (b) ? (a) : (b))
#define MAXIMUM(a, b) ((a) > (b) ? (a) : (b))
#define ISININTERVAL(first,last,x) ((first<=x)&&(x<=last))
#define NOTININTERVAL(first,last,x) ((x<first)||(last<x))
#define NOTINTERSECTED(ql1,qh1,ql2,qh2) ((qh2<ql1)||(ql2>qh1))
#define CUDA_CHECK(call,msg) if((call) != cudaSuccess) { cudaError_t err = cudaGetLastError(); printf("\nCUDA ERROR: %s (%s)",msg,cudaGetErrorString(err)); exit(EXIT_FAILURE); }
#define GET_FROM_2D(pointer,x,y,rowSize) return pointer[x*rowSize +y];
#define CEILING(X) (X-(unsigned int)(X) > 0 ? (unsigned int)(X+1) : (unsigned int)(X))


typedef unsigned int DATATYPE;
//typedef unsigned int uint;
//typedef unsigned short ushort;


typedef struct /*__align__(8)*/ sKernelSetting
{
public:
	dim3 dimBlock;
	dim3 dimGrid;
	unsigned int blockSize;
	unsigned int sharedMemSize;
	unsigned int noChunks;

	sKernelSetting()
	{
		dimBlock = dim3(1,1,1);
		dimGrid = dim3(1,1,1);
		blockSize = 0;
		sharedMemSize = 0;
		noChunks = 1;
	}

	inline void print()
	{
		printf("\n------------------------------ KERNEL SETTING\n");
		printf("Block dimensions: %u %u %u\n", dimBlock.x, dimBlock.y, dimBlock.z);
		printf("Grid dimensions:  %u %u %u\n", dimGrid.x, dimGrid.y, dimGrid.z);
		printf("BlockSize: %u\n", blockSize);
		printf("Shared Memory Size: %u\n", sharedMemSize);
		printf("Number of chunks: %u\n", noChunks);
	}
}KernelSetting;

typedef struct __align__(8) RQElement //bed157 vyresit align
{
public:
	int minimum;
	int maximum;
};

__device__ __constant__ static RQElement C_RQElement[500]; //WARNING - Range query dimension must be less than 500 - Cannot allocate constant memory dynamically.

#endif
#endif