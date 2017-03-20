/**
*	\file cRangeQueryConfigGpu.h
*	\author Pavel Bednar
*	\version 0.1
*	\date 2015-02-05
*	\brief Configurator of Range Query Processing on Gpu
*/

#include "dstruct/paged/queryprocessing/cRangeQueryConfig.h"

#ifdef CUDA_ENABLED
/*!
* Sets the GPU number of Threads Per Block.
*/
inline void cRangeQueryConfig::SetGpuAlgorithm(unsigned int value)
{
	mGpuAlgorithm = value;
}
/*!
* Sets the GPU Compute Capability.
*/
inline void cRangeQueryConfig::SetGpuCapability(float value)
{
	mCudaCapability = value;
}
/*!
* Sets the GPU Algorithm.
*/
inline void cRangeQueryConfig::SetGpuThreads(unsigned int value)
{
	mThreadsPerBlock = value;
}

/*!
* Sets the GPU Algorithm.
*/
inline unsigned int cRangeQueryConfig::GetGpuAlgorithm() const
{
	return mGpuAlgorithm;
}

/*!
* Sets the GPU Compute Capability.
*/
inline float cRangeQueryConfig::GetGpuCapability() const
{
	return mCudaCapability;
}

/*!
* Gets the GPU number of Threads Per Block.
*/
inline unsigned int cRangeQueryConfig::GetGpuThreads() const
{
	return mThreadsPerBlock;
}

/*
Returns if buckerOrder on GPU is same as node index.
*/
inline bool cRangeQueryConfig::IsBucketOrderNodeIndex() const
{
	return cGpuConst::MEMORY_LOCATION == cGpuMainMemoryLocation::Gpu;
}
#endif
