#ifndef __cRQBuffers_h__
#define __cRQBuffers_h__

#include "common/memorystructures/cStack.h"
#include "common/memdatstruct/cMemoryBlock.h"
#include "dstruct/paged/core/cNodeBuffers.h"
#include "dstruct/paged/queryprocessing/cRangeQueryConfig.h"
#include "common/memorystructures/cHashTable.h"
#include "dstruct/paged/rtree/cRTreeConst.h"
#include "dstruct/paged/queryprocessing/cDbfsLevel.h"
#include "dstruct/paged/rtree/cSignatureKey.h"

using namespace common::memdatstruct;
using namespace dstruct::paged::core;
using namespace dstruct::paged::rtree;
using namespace dstruct::paged;

namespace dstruct {
	namespace paged {

template<class TKey>
class cRQBuffers
{
private:
	cDbfsLevel** mDbfsLevels;    // an array of indices for the depth-breadth search
public:
	cMemoryBlock* bufferMemBlock;    // memblock for all buffers
	cNodeBuffers<TKey> nodeBuffer;	// buffer used for compression and ri

	cArray<unsigned int> **qrs;      // indices for Batch RQ
	cLinkedList<unsigned int> **qrs_ll;      // indices for B-Tree RQ - duplication - try to merge with the previous one (qrs)
	unsigned int *resultSizes;       // result sizes for individual queries of a batch

	cArray<unsigned int> ***aqis;            // indices for Cartesian RQ

	unsigned int mIndexArrayCount;   // current leaf indices count in LiMatrix 
	cArray<unsigned int>** LiArray;  // two dimensional array of leaf indices and relevant queries for each of them

	unsigned int* ResultSizes;       // number of satisfied records for particular queries

	cArray<unsigned int>** CurrentQueryPath;   // array of stacks storing query paths

	cArray<unsigned int>*** CurrentPath;	   // two dimensional array of stacks storing query paths for cartesian range query

	cArray<unsigned int> *NarrowDimensions;    // array of narrow dimensions
	// it includes orders of true bits in a query signature, order: bits of signature, other signatures, dimension, level
	cArray<ullong> *QueryTrueBitOrders; 
	cSignatureKey** ConvIndexKeys; // keys of conversion index in the case of signatures
	uint* nOfLevelBits;			   // number of describing bits per level	

	cDbfsLevel* GetBreadthSearchArray();
	cDbfsLevel* GetBreadthSearchArray(cRangeQueryConfig *rqConfig, uint level);
	static uint GetSize(int structType, uint capacity, uint noLevels);
	void InitLevels(cRangeQueryConfig *rqConfig, uint capacity, char* buffer, uint noLevels);
#ifdef CUDA_ENABLED
	cArray<uint>** mGpuSearchArray;
	cArray<uint>* GetGpuSearchArray(cRangeQueryConfig *rqConfig,unsigned int level);
#endif
};

template <class TKey>
uint cRQBuffers<TKey>::GetSize(int structType, uint capacity, uint noLevels)
{
	uint size = sizeof(cDbfsLevel*) * noLevels + sizeof(cDbfsLevel) * noLevels + cDbfsLevel::GetSize(structType, capacity, noLevels);
#ifdef CUDA_ENABLED //gpu arrays
	size += sizeof(cArray<uint>*) * noLevels + sizeof(cArray<uint>)*noLevels + cDbfsLevel::GetSize(cRangeQueryConfig::SEARCH_STRUCT_ARRAY, capacity, noLevels);
#endif
	return size;
}

template <class TKey>
void cRQBuffers<TKey>::InitLevels(cRangeQueryConfig *rqConfig, uint capacity, char* buffer,uint noLevels)
{
	mDbfsLevels = (cDbfsLevel**)buffer;
	buffer += sizeof(cDbfsLevel*) * noLevels;
	for (unsigned int i = 0; i < noLevels; i++)
	{
		mDbfsLevels[i] = (cDbfsLevel*)buffer;
		buffer += sizeof(cDbfsLevel);
		mDbfsLevels[i]->Init(rqConfig->GetSearchStruct(), buffer, capacity);
		buffer += cDbfsLevel::GetSize(rqConfig->GetSearchStruct(), capacity, 1);
	}
#ifdef CUDA_ENABLED //gpu arrays
	mGpuSearchArray = (cArray<uint>**)buffer;
	buffer += sizeof(cArray<uint>*) * noLevels;
	for (unsigned int i = 0; i < noLevels; i++)
	{
		mGpuSearchArray[i] = (cArray<uint>*)buffer;
		buffer += sizeof(cArray<uint>);
		mGpuSearchArray[i]->Init(buffer, capacity);
		buffer += cDbfsLevel::GetSize(cRangeQueryConfig::SEARCH_STRUCT_ARRAY, capacity, 1);
	}
#endif
}


template <class TKey> 
cDbfsLevel* cRQBuffers<TKey>::GetBreadthSearchArray()
{
	return mDbfsLevels[0];
}
template <class TKey> 
cDbfsLevel* cRQBuffers<TKey>::GetBreadthSearchArray(cRangeQueryConfig *rqConfig, unsigned int level)
{
	if (rqConfig->GetSearchMethod() == cRangeQueryConfig::SEARCH_BFS)
	{
		if (level % 2 == 0)
			return mDbfsLevels[0];
		else 
			return mDbfsLevels[1];
	}
	else
	{
		return mDbfsLevels[level];
	}
}
#ifdef CUDA_ENABLED
template <class TKey> 
cArray<uint>* cRQBuffers<TKey>::GetGpuSearchArray(cRangeQueryConfig *rqConfig,unsigned int level)
{
	return mGpuSearchArray[0];
	/*if (rqConfig->GetSearchMethod() == cRangeQueryConfig::SEARCH_BFS)
	{
	if (level % 2 == 0)
	return mGpuSearchArray[0];
	else
	return mGpuSearchArray[1];
	}
	else
	{
	return mGpuSearchArray[level];
	}*/
}
#endif
}}
#endif