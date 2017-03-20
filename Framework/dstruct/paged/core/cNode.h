/**
*	\file cNode.h
*	\author Radim Bača, David Bednář, Michal Krátký
*	\version 0.2
*	\date jul 2011
*	\brief Node of a paged data structure
*/

#ifndef __cNode_h__
#define __cNode_h__

namespace dstruct {
	namespace paged {
		typedef unsigned int tNodeIndex;
}}

#include "common/stream/cStream.h"
#include "common/cNumber.h"
#include "common/cBitString.h"
#include "dstruct/paged/core/cNodeHeader.h"

namespace dstruct {
  namespace paged {
	namespace core {

class cNodeHeader;

/**
*	Node of a paged data structure
*
*	\author Radim Bača, David Bednář, Michal Krátký
*	\version 0.2
*	\date jul 2011
**/
class cNode
{
protected:
	cNodeHeader* mHeader;
	tNodeIndex mIndex;
	char* mData;                    /// data of the node.
	bool mDebug;
	bool mHasVariableLength;

	unsigned int mItemCount;		/// Number of items in the node.
	unsigned int mRealSize;     /// Size of node space occupied by some data (in bytes). 
	unsigned int mMaxSize;
	unsigned int mFreeSize;			/// Free space for data in the node

	unsigned int mBucketOrder;		/// specify the order of the char block within the cache array.
	unsigned int mHeaderId;

public:
	static const tNodeIndex EMPTY_INDEX       = 0xffffffff;
	static const int REAL_SIZE_CONST = 128;	// for compute real size

	cNode(unsigned int size, unsigned int order);
	cNode();
	~cNode();

	inline void Init(unsigned int size, unsigned int order, char* data);
	inline void Delete();
	inline void Clear(bool leaveHeaderId = false);

	void Write(cStream* stream);
	void Read(cStream* stream);	

	inline void SetIndex(unsigned int index);
	inline void SetItemCount(unsigned int itemCount);
	inline unsigned int IncrementItemCount();
	inline void SetDebug(bool debug);
	inline void IsVariableLength(unsigned int variable);
	inline void SetFreeSize(unsigned int freeSize);

	inline const char* GetDataMemory();
	inline tNodeIndex GetIndex() const;
	inline unsigned int GetItemCount() const;
	inline unsigned int GetSize();
	inline unsigned int GetFreeSize() const;
	inline cNodeHeader* GetHeader();
	inline cNodeHeader* GetCHeader() const;

	inline unsigned int GetBucketOrder();
	inline void ClearBucketOrder();
	inline void SetBucketOrder(uint bucketOrder);
	inline void SetHeader(const cNodeHeader *header);
	inline void SetHeaderId(unsigned int id);
	inline unsigned int GetHeaderId() const;
	inline unsigned int IsVariableLength();

	inline void ClearHeader();
};

/**
* Initialize new node and allocate its data.
* \param size The size of a new persistent node.
* \param order The order of the node in the cache.
* \param data preallocated mdata of the node
*/
inline void cNode::Init(unsigned int size, unsigned int order, char* data)
{
	mHeader = NULL;
	mMaxSize = size;
	mDebug = false;

	if (data != NULL)
	{
		mData = data;
	}
	else
	{
		mData = new char[size];
	}
	mBucketOrder = order;
	mIndex = EMPTY_INDEX;
	Clear();
}

inline void cNode::SetDebug(bool debug)
{
	mDebug = debug;
}

inline void cNode::SetIndex(unsigned int index)
{ 
	mIndex = index; 
}

inline void cNode::IsVariableLength(unsigned int variable)
{
	mHasVariableLength = variable;
}

inline unsigned int cNode::IsVariableLength()
{
	return mHasVariableLength;
}

inline cNodeHeader* cNode::GetHeader()
{ 
	return mHeader; 
}

inline cNodeHeader* cNode::GetCHeader() const
{ 
	return mHeader; 
}

inline void cNode::SetItemCount(unsigned int itemCount)
{ 
	mItemCount = itemCount; 
}

inline unsigned int cNode::IncrementItemCount()
{ 
	return ++mItemCount; 
}

inline const char* cNode::GetDataMemory()
{
	return mData;
}

inline tNodeIndex cNode::GetIndex() const
{ 
	return mIndex; 
}

inline unsigned int cNode::GetItemCount() const
{ 
	return mItemCount; 
}

inline unsigned int cNode::GetBucketOrder()
{
	return mBucketOrder;
}

inline void cNode::ClearBucketOrder()
{
	mBucketOrder = EMPTY_INDEX;
}

inline void cNode::SetBucketOrder(uint bucketOrder)
{
	mBucketOrder = bucketOrder;
}

inline void cNode::SetHeaderId(unsigned int id)
{
	mHeaderId = id;
}

inline void cNode::SetHeader(const cNodeHeader *header)
{
	mHeader = (cNodeHeader*)header;
}

inline unsigned int cNode::GetHeaderId() const
{
	return mHeaderId;
}

/**
* \return Free size in the node (in bytes).
*/
inline void cNode::SetFreeSize(unsigned int freeSize)
{
	mFreeSize = freeSize;
}


/**
* \return Free size in the node (in bytes).
*/
inline unsigned int cNode::GetFreeSize() const
{
	return mFreeSize;
}

inline void cNode::Clear(bool leafNodeHeaderId)
{
	if (!leafNodeHeaderId)
	{
		mHeaderId = (unsigned int)~0; // !!mk!!
	}
	mIndex = cNode::EMPTY_INDEX;
	mItemCount = 0;
}

}}}
#endif 