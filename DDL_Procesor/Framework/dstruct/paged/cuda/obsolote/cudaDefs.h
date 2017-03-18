#ifndef __CUDADEFS_H
#define __CUDADEFS_H

#include "dataDefs.h"
#include "dataManager.h"
#include "fileManager.h"
#include "cGpuConst.h"
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
EXTERN cudaError_t error;
EXTERN cudaDeviceProp deviceProp;

static DataManager *dm = DataManager::Instance();
static DataStorage &dataStorage = dm->dataStorage;

EXTERN KernelSetting ks[1];

EXTERN unsigned int maxInputs;				//Max N input vectors will be stored in D_bufferInputVectors.
EXTERN unsigned int processedInputs;		//Number of already processed input vectors;
EXTERN unsigned int currentInputs;			//Current count of input vectors stored in D_bufferInputVectors;
EXTERN unsigned int bufferOffset;

__device__ __constant__ DataInfo C_dataInfo;
#ifdef CUDA_CONSTANT_MEM
__device__ __constant__ RQElement C_RQElement[11]; //Range query element stored in the constant memory
#endif
EXTERN __device__ DATATYPE *D_bufferInputVectors;		//Inputs

EXTERN __device__ bool *D_bufferResults;				//Outputs
EXTERN bool *H_bufferResults;

EXTERN __device__ RQElement *D_bufferRQ;				//Range Queries

#endif