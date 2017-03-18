/**
*	\file cBucketHeaderStorage.cpp
*	\author Michal Kratky
*	\version 0.2
*	\date jul 2011
*	\brief A storage of node records
*/

#ifndef __cBucketHeaderStorage_h__
#define __cBucketHeaderStorage_h__

#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "common/memorystructures/cHashTable.h"
#include "common/memorystructures/cLinkedList.h"
#include "dstruct/paged/core/cBucketHeader.h"
#include "common/datatype/cBasicType.h"

using namespace common::memorystructures;

namespace dstruct {
  namespace paged {
	namespace core {

/**
*	A storage of node records
*
*	\author Michal Krátký
*	\version 0.2
*	\date jul 2011
**/

// typedef cHashTable<tNodeIndex, unsigned int> cBucketHashTable;
typedef cHashTable<cUInt, cUInt> cBucketHashTable;

class cBucketHeaderStorage
{
private:
	unsigned int mSize;
	unsigned int mCapacity;           // Capacity of the storage
	// each header holds its own BucketQueueNode, due to the efficiency reason, this node is not returned into the BucketQueue while
	// ItemCount in the queue < 10% of it capacity, see PutBackInBucketQueue.
	unsigned int mEmptyQueueTreshold; 
	cBucketHeader *mBucketHeader;
	cBucketHashTable *mBucketArrayIndex;
	char* mBucketArrayIndex_memory;   // memory for the BucketArrayIndex
	cLinkedList<unsigned int> *mBucketQueue;

public:
	static const unsigned int NOT_FOUND = UINT_MAX;

	cBucketHeaderStorage(unsigned int capacity);
	~cBucketHeaderStorage();

	void Null();
	void Clear();
	void Clear(unsigned int i);
	inline void ClearIndexArray(unsigned int index);

	bool FindBucket(const tNodeIndex &nodeIndex, cBucketHeader **bucketHeader);
	bool FindNode(const tNodeIndex &nodeIndex, cBucketHeader **bucketHeader);
	void PutBackInBucketQueue(const cBucketHeader *bucketHeader);

	void DeleteFromBucketIndex(const tNodeIndex &nodeIndex);
	void AddInBucketIndex(const tNodeIndex &nodeIndex, const unsigned int &bucketOrder);

	inline cBucketHeader* GetBucketHeader(unsigned int bucketOrder);
	
	bool CheckArrays();
	void Print() const;
	void PrintLocks() const;
	unsigned int GetNofLocks() const;
	void CheckQueue() const;
};

void cBucketHeaderStorage::ClearIndexArray(unsigned int index)
{
	assert(mSize > 0);
	// ???
	// mBucketArrayIndex->FreeNode(index);
	mSize--;

	// uncommend for debugging
	// assert(mBucketArrayIndex->CheckHashArray());
	// assert(mSize == mBucketArrayIndex->GetNumberOfUsedItemsInHashArray());
}

/**
* \param order Order of the item in the mBucketHeader
*/
inline cBucketHeader* cBucketHeaderStorage::GetBucketHeader(unsigned int bucketOrder)
{
	return &mBucketHeader[bucketOrder];
}

/*
inline cBucketHeader* cBucketHeaderStorage::GetNodeRecordArray() const
{
	return mBucketHeader;
}*/

/*
inline cBucketHeaderSortedList* cBucketHeaderStorage::GetTimestampSortedArray()
{
	return mTimestampSortedArray;
}*/

}}}
#endif