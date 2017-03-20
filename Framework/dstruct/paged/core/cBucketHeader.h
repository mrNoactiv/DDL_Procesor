/**
*	\file cBucketHeader.cpp
*	\author Michal Kratky
*	\version 0.2
*	\date jul 2011
*	\brief A record for a node in the paged cache
*/

#ifndef __cBucketHeader_h__
#define __cBucketHeader_h__

#include <atomic>
#include <limits.h>

#include "dstruct/paged/core/cNode.h"
#include "common/memorystructures/cLinkedList.h"

using namespace common::memorystructures;

namespace dstruct {
  namespace paged {
	namespace core {

/**
*	A record for a node in the paged cache
*
*	\author Radim Bača, David Bednář, Michal Krátký
*	\version 0.2
*	\date jul 2011
**/
class cBucketHeader
{
private:
	unsigned int mBucketOrder;
	tNodeIndex mNodeIndex;
	std::atomic<unsigned int> mReadLock;
	std::atomic<bool> mWriteLock;
	std::atomic<bool> mModified;
	cLinkedListNode<unsigned int> *mBucketQueueNode;  // node of the bucket queue related to the backet

#ifdef CUDA_ENABLED
	int mGpuId; // -1 means this node is not on any GPU, else id of GPU
	int mGpuItemOrder; // position in GPU's memory
#endif


public:
	static const int MODE_INDEX = 0;
	static const int MODE_TIMESTAMP = 1;

public:
	// If these two constructors are not defined, I get:
	// cannot access private member declared in class 'std::atomic<bool>
	// cannot access private member declared in class 'std::mutex'
    cBucketHeader();
    cBucketHeader(const cBucketHeader&);              // not defined
    cBucketHeader& operator=(const cBucketHeader&);   // not defined

	void Clear(unsigned int bucketOrder);

	// inline int Equal(const cBucketHeader &record, int mode) const;

	inline uint GetBucketOrder() const;
	inline void SetBucketOrder(uint bucketOrder);
	inline tNodeIndex GetNodeIndex() const;
	inline void SetNodeIndex(const tNodeIndex &nodeIndex);

	inline bool GetModified() const;
	inline void SetModified(bool modified);

	inline unsigned int GetReadLock() const;
	inline void IncrementReadLock();
	inline void DecrementReadLock();
	inline bool GetWriteLock() const;
	inline void SetWriteLock(bool writeLock);

	inline cLinkedListNode<unsigned int>* GetBucketQueueNode() const;
	inline void SetBucketQueueNode(cLinkedListNode<unsigned int> *bucketQueueNode);
#ifdef CUDA_ENABLED
	inline int GetGpuId();
	inline int GetGpuItemOrder();
	inline void SetGpuId(int value) ;
	inline void SetGpuItemOrder(int value);
#endif	
};

//inline int cBucketHeader::Equal(const cBucketHeader &record, int mode) const
//{
//	int ret = -1;
//
//	if (mode == MODE_TIMESTAMP)
//	{
//		if (mTimestamp > record.GetTimestamp())
//		{
//			ret = 1;
//		}
//		else if (mTimestamp == record.GetTimestamp())
//		{
//			ret = 0;
//		}
//	}
//	else // mode = MODE_INDEX
//	{
//		/*
//		if (mDataStructureIndex == record.GetDataStructureIndex())
//		{
//			if (mNodeIndex == record.GetNodeIndex())
//			{
//				ret = 0;
//			}
//			else if (mNodeIndex > record.GetNodeIndex())
//			{
//				ret = 1;
//			}
//		}
//		else if (mDataStructureIndex > record.GetDataStructureIndex())
//		{
//			ret = 1;
//		}*/
//	}
//	return ret;
//}

/*
inline std::mutex* cBucketHeader::GetMutex()
{
	return &mMutex;
}*/

inline unsigned int cBucketHeader::GetBucketOrder() const
{
	return mBucketOrder;
}

inline void cBucketHeader::SetBucketOrder(unsigned int bucketOrder)
{
	mBucketOrder = bucketOrder;
}

inline unsigned int cBucketHeader::GetReadLock() const
{
	return mReadLock;
}

inline void cBucketHeader::IncrementReadLock()
{
	mReadLock++;
}

inline void cBucketHeader::DecrementReadLock()
{
	assert(mReadLock > 0);
	mReadLock--;
}

inline bool cBucketHeader::GetWriteLock() const
{
	return mWriteLock;
}

inline void cBucketHeader::SetWriteLock(bool writeLock)
{
	mWriteLock = writeLock;
}

inline bool cBucketHeader::GetModified() const
{
	return mModified;
}

inline void cBucketHeader::SetModified(bool modified)
{
	mModified = modified;
}

inline tNodeIndex cBucketHeader::GetNodeIndex() const
{
	return mNodeIndex;
}

inline void cBucketHeader::SetNodeIndex(const tNodeIndex &nodeIndex)
{
	mNodeIndex = nodeIndex;
}

inline cLinkedListNode<unsigned int>* cBucketHeader::GetBucketQueueNode() const
{
	return mBucketQueueNode;
}

inline void cBucketHeader::SetBucketQueueNode(cLinkedListNode<unsigned int> *bucketQueueNode)
{
	mBucketQueueNode = bucketQueueNode;
}
#ifdef CUDA_ENABLED
inline int cBucketHeader::GetGpuId()
{
	return mGpuId;
}

inline void cBucketHeader::SetGpuId(int value) 
{
	mGpuId = value;
}
inline void cBucketHeader::SetGpuItemOrder(int value) 
{
	mGpuItemOrder = value;
}
inline int cBucketHeader::GetGpuItemOrder() 
{
	return mGpuItemOrder;
}

#endif

}}}
#endif