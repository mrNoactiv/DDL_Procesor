#ifdef CUDA_ENABLED
#ifndef __CUDADEFS_H
#define __CUDADEFS_H

#include "cGpuConst.h"
#include "globalDefs.h"
#define OPERATOR >

#ifdef GPU_BLOCK_IS_POW2
	#undef GPU_BLOCK_IS_POW2
#endif
#define GPU_BLOCK_IS_POW2 1				//Do not change!!!    If n is a power of 2, (i/n) is equivalent to (i>>log2(n)) and (i%n) is equivalent to (i&(n-1));
//#define cGpuConst::THREADS_PER_BLOCK 256			//Must be pow2


#define WORD_SIZE 4						//Must be pow2

#ifdef DEFINE_CUDA_EXTERN_VAR
#define EXTERN
#else 
#define EXTERN extern 
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
//VARIABLEs
////////////////////////////////////////////////////////////////////////////////////////////////////

EXTERN KernelSetting ks[1];

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	The range of every dimension of range query vectors.</summary>
/// <remarks>	Gajdi, 25.07.2011. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct __align__(8) RQElement
{
	DATATYPE minimum;
	DATATYPE maximum;
}RQElement;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Basic information on data like dimensions, etc.
/// It is separated from data to be able to make a copy to DEVICE memory.</summary>
/// <remarks>	Gajdi, 25.07.2011. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct __align__(8) DataInfo
{
public:
	unsigned int dim;				//dimension
	unsigned int noRQ;				//number of RangeQueries

	DataInfo& operator=(const DataInfo& other)
	{
		dim = other.dim;
		noRQ = other.noRQ;
		return *this;
	}

	inline void print()
	{
		printf("Dimension: %u\n", dim);
		printf("Number of RQ: %u\n", noRQ);
	}
}DataInfo;

#ifdef CUDA_CONSTANT_MEM
__device__ __constant__ DataInfo C_dataInfo;
__device__ __constant__ RQElement C_RQElement[500]; //WARNING - Range query dimension must be less than 500 - Cannot allocate constant memory dynamically.
__device__ __constant__ short C_Dim;
#endif


#endif
#endif