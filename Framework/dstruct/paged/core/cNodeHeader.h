/**
*	\file cNodeHeader.h
*	\author Radim Bača, David Bednář, Michal Krátký
*	\version 0.2
*	\date jul 2011
*	\brief Header of a node - root class of data structure nodes
*/

#ifndef __cNodeHeader_h__
#define __cNodeHeader_h__

#include "common/cCommon.h"
#include "common/cMemory.h"

namespace dstruct {
  namespace paged {
	namespace core {
class cNode;
}}}

class cMemoryManagerCuda;

#include "dstruct/paged/core/cNode.h"
#include "dstruct/paged/core/cDStructConst.h"
#include "dstruct/paged/core/cMemoryPool.h"
#include "common/memdatstruct/cMemoryManager.h"
#include "common/stream/cStream.h"
#include "common/datatype/cDTDescriptor.h"

using namespace common;
using namespace common::datatype;
using namespace common::memdatstruct;

namespace dstruct {
  namespace paged {
	namespace core {
// class cNode;
// class common::datatype::cDTDescriptor;

/**
*	Header of a node - root class of data structure nodes
*
*	\author Radim Bača, David Bednář, Michal Krátký
*	\version 0.2
*	\date jul 2011
**/
class cNodeHeader
{
protected:

	unsigned int mNodeSerialSize;
	unsigned int mNodeInMemSize;

	// serialized attributes
	unsigned int mCompressionRatio;
	unsigned int mCodeType;         // Code utilized to encoding/decoding items, Fibonacci etc., see cDStructConst::CODE_*
	unsigned int mNodeCapacity;		// Maximal number of items which can be stored in the node.
	unsigned int mNodeItemsSpaceSize;  // Size of the space reserved for items of the node. In other words, room for the node items.
	unsigned int mItemSize;	// Size of one item stored in the node. In the case of tree DS and leaf nodes: it is the size of the key + data.
	unsigned int mNodeCount;		// Statistics; number of nodes.
	unsigned int mItemCount;		// Statistics; number of items.
	bool mDuplicates;					// Are duplicate keys valid?
	uint mDStructMode;              // Mode of data structure, Default, Reference items used etc., see cDStructConst::DSMODE_*
	uint mRuntimeMode;					// Run-time mode of data structure, Debug, validation, etc., see cDStructConst::RTMODE_DEBUG etc.
	uint mDStructCode;                         /// A code of a data structure  

	bool mCacheMeasureCount;
	bool mCacheMeasureTime;
	cMemoryPool *mMemoryPool;       // A pool - pre-allocated memory used by algorithms
	cMemoryManager *mMemoryManager; // A pool - pre-allocated memory used by algorithms
#ifdef CUDA_ENABLED
	cMemoryManagerCuda *mMemoryManagerCuda;
#endif
	cDTDescriptor *mKeyDescriptor;  // Descriptor of the key, for example cSpaceDescriptor for cTuple or cUniformTuple

	virtual inline void SetCopy(cNodeHeader* newHeader);

public:
	cNodeHeader();
	cNodeHeader(const cNodeHeader &header);
	~cNodeHeader();

	virtual inline void WriteNode(cNode* node, cStream* stream) = 0;
	virtual inline void ReadNode(cNode* node, cStream* stream) = 0;	
	virtual inline cNode* CopyNode(cNode* dest, cNode* source) = 0;
	virtual inline cNodeHeader* CreateCopy(unsigned int blockSize) = 0;
	virtual inline void Clear();
	virtual inline void Init();

	inline virtual bool Write(cStream *stream);
	inline virtual bool Read(cStream *stream);
	
	inline void ResetNodeCount();
	inline void SetNodeCount(unsigned int count);
	inline unsigned int IncrementNodeCount();

	inline void ResetItemCount();
	inline void SetInnerItemCount(unsigned int count);
	inline unsigned int IncrementItemCount();
	inline uint IncrementItemCount(uint count);
	inline unsigned int DecrementItemCount();

	// inline void SetNodeSerialSizeAsMul(unsigned int mul);
	inline void SetItemSize(unsigned int defaultItemSize);
	inline void SetNodeCapacity(unsigned int itemCapacity);
	inline void SetCacheMeasureCount(bool b);
	inline void SetCacheMeasureTime(bool b);
	inline void SetNodeSerialSize(unsigned int nodeSerialSize);
	inline void SetNodeInMemSize(unsigned int nodeInMemSize);
	inline void SetCodeType(uint codeType);
	inline void SetMaxCompressionRatio(uint compressionRatio);
	inline void SetDStructCode(uint dsCode);
	inline bool DuplicatesAllowed() const;
	inline void DuplicatesAllowed(bool status);	

	inline unsigned int GetNodeSerialSize() const;
	inline unsigned int GetNodeInMemSize() const;
	inline unsigned int GetNodeCount() const;
	inline unsigned int GetInnerNodeCount() const;
	inline unsigned int GetItemCount() const;
	inline unsigned int GetNodeCapacity() const;
	inline unsigned int GetInnerItemCount() const;
	inline unsigned int GetItemSize() const;
	inline float AverageNodeUtilization() const;
	inline float AverageLeafNodeUtilization() const;
	inline float AverageInnerNodeUtilization() const;
	inline bool GetCacheMeasureCount() const;
	inline bool GetCacheMeasureTime() const;
	inline unsigned int GetCodeType();
	inline unsigned int GetNodeItemsSpaceSize() const;
	inline void SetNodeItemsSpaceSize(unsigned int value);

	inline uint GetDStructMode() const;
	inline uint GetDStructCode() const;
	inline unsigned int GetRuntimeMode() const;
	inline void SetDStructMode(unsigned int dsMode);
	inline void SetRuntimeMode(unsigned int rtMode);

	inline void SetMemoryPool(cMemoryPool *pool);
	inline cMemoryPool* GetMemoryPool();
	inline void SetMemoryManager(cMemoryManager *pool);
	inline cMemoryManager* GetMemoryManager();
	
#ifdef CUDA_ENABLED
	inline void SetMemoryManagerCuda(cMemoryManagerCuda* memMan);
	inline cMemoryManagerCuda* GetMemoryManagerCuda();
#endif	

	inline void SetKeyDescriptor(cDTDescriptor *pDtD);
	inline const cDTDescriptor* GetKeyDescriptor() const;

	inline void Print();
};

inline void cNodeHeader::Clear()
{
	mNodeCount = 0;
	mItemCount = 0;
}

bool cNodeHeader::Write(cStream *stream)
{
	bool ok = stream->Write((char*)&mCompressionRatio, sizeof(unsigned int));
	ok &= stream->Write((char*)&mCodeType, sizeof(unsigned int));
	ok &= stream->Write((char*)&mNodeCapacity, sizeof(unsigned int));
	ok &= stream->Write((char*)&mNodeSerialSize, sizeof(unsigned int));
	ok &= stream->Write((char*)&mNodeInMemSize, sizeof(unsigned int));
	ok &= stream->Write((char*)&mItemSize, sizeof(unsigned int));
	ok &= stream->Write((char*)&mNodeCount, sizeof(unsigned int));
	ok &= stream->Write((char*)&mItemCount, sizeof(unsigned int));
	ok &= stream->Write((char*)&mDStructMode, sizeof(unsigned int));
	ok &= stream->Write((char*)&mRuntimeMode, sizeof(unsigned int));
	
	return ok;
}

bool cNodeHeader::Read(cStream *stream) 
{
	bool ok = stream->Read((char*)&mCompressionRatio, sizeof(unsigned int));
	ok &= stream->Read((char*)&mCodeType, sizeof(unsigned int));
	ok &= stream->Read((char*)&mNodeCapacity, sizeof(unsigned int));
	ok &= stream->Read((char*)&mNodeSerialSize, sizeof(unsigned int));
	ok &= stream->Read((char*)&mNodeInMemSize, sizeof(unsigned int));
	ok &= stream->Read((char*)&mItemSize, sizeof(unsigned int));
	ok &= stream->Read((char*)&mNodeCount, sizeof(unsigned int));
	ok &= stream->Read((char*)&mItemCount, sizeof(unsigned int));
	ok &= stream->Read((char*)&mDStructMode, sizeof(unsigned int));
	ok &= stream->Read((char*)&mRuntimeMode, sizeof(unsigned int));
	
	return ok;
}

inline void cNodeHeader::Init()
{
	mCompressionRatio = 1;
	mCodeType = cDStructConst::CODE_NOCODING;
	mNodeCount = mItemCount = 0;
	mNodeCapacity = cCommon::UNDEFINED_UINT;
	mItemSize = cCommon::UNDEFINED_UINT;
	mNodeItemsSpaceSize = cCommon::UNDEFINED_UINT;
	mDStructMode = cDStructConst::DSMODE_DEFAULT;
	mRuntimeMode = cDStructConst::RTMODE_DEFAULT;
	mCacheMeasureCount = true;
	mCacheMeasureTime = false;

	//AddHeaderSize(28 * sizeof(unsigned int) + sizeof(bool));
}

void cNodeHeader::SetCopy(cNodeHeader* newHeader)
{
	newHeader->SetMaxCompressionRatio(mCompressionRatio);
	newHeader->SetCodeType(mCodeType);
	newHeader->SetNodeCapacity(mNodeCapacity);
	newHeader->SetNodeSerialSize(mNodeSerialSize);
	newHeader->SetNodeInMemSize(mNodeInMemSize);
	newHeader->SetItemSize(mItemSize);
	newHeader->SetNodeCount(mNodeCount);
	newHeader->SetInnerItemCount(mItemCount);
	newHeader->SetDStructMode(mDStructMode);
	newHeader->SetRuntimeMode(mRuntimeMode);
	newHeader->SetCacheMeasureCount(mCacheMeasureCount);
	newHeader->SetCacheMeasureTime(mCacheMeasureTime);

	//db
	newHeader->SetMemoryPool(mMemoryPool);
}

inline void cNodeHeader::ResetNodeCount()
{ 
	mNodeCount = 0;
}


inline void cNodeHeader::SetCodeType(uint codeType)
{
	mCodeType = codeType;
}

inline void cNodeHeader::SetMaxCompressionRatio(uint compressionRatio)
{
	mCompressionRatio = compressionRatio;
}

inline void cNodeHeader::SetDStructCode(uint dsCode)
{
	mDStructCode = dsCode;
}

inline bool cNodeHeader::DuplicatesAllowed() const				
{ 
	return mDuplicates; 
}

inline void cNodeHeader::DuplicatesAllowed(bool status)		
{ 
	mDuplicates=status; 
}

inline unsigned int cNodeHeader::GetCodeType()
{
	return mCodeType;
}

inline void cNodeHeader::SetNodeCount(unsigned int count)
{ 
	mNodeCount = count; 
}

inline unsigned int cNodeHeader::IncrementNodeCount()
{ 
	return ++mNodeCount; 
}

inline void cNodeHeader::ResetItemCount()
{ 
	mItemCount = 0; 
}

inline void cNodeHeader::SetInnerItemCount(unsigned int count)
{ 
	mItemCount = count; 
}

inline unsigned int cNodeHeader::IncrementItemCount()
{ 
	return ++mItemCount; 
}

inline uint cNodeHeader::IncrementItemCount(uint count)
{
	return mItemCount += count;
}

inline unsigned int cNodeHeader::DecrementItemCount()
{ 
	return --mItemCount; 
}

//inline void cNodeHeader::SetNodeSerialSizeAsMul(unsigned int mul)
//{
//	mNodeSerialSize = mul * BLOCK_SIZE; 
//}

inline void cNodeHeader::SetItemSize(unsigned int defaultItemSize)
{ 
	mItemSize = defaultItemSize; 
}

inline void cNodeHeader::SetNodeCapacity(unsigned int itemCapacity)
{ 
	mNodeCapacity = itemCapacity; 
}

inline unsigned int cNodeHeader::GetItemSize() const 
{ 
	return mItemSize; 
}

inline unsigned int cNodeHeader::GetNodeCount() const
{ 
	return mNodeCount; 
}

inline unsigned int cNodeHeader::GetInnerNodeCount() const
{ 
	return mNodeCount; 
}
inline unsigned int cNodeHeader::GetItemCount() const
{ 
	return mItemCount; 
}

inline unsigned int cNodeHeader::GetInnerItemCount() const
{ 
	return mItemCount; 
}

inline float cNodeHeader::AverageNodeUtilization() const
{ 
	return (float)(((float)(mItemCount) / (float)(mNodeCapacity * mNodeCount)) * 100.0);
}

inline float cNodeHeader::AverageInnerNodeUtilization() const
{ 
	return (float)(((float)mItemCount / (float)(mNodeCapacity * mNodeCount))*100.0); 
}

inline unsigned int cNodeHeader::GetNodeCapacity() const
{ 
	return mNodeCapacity; 
}

//inline char* cNodeHeader::GetTitle() const
//{ 
//	return (char *)mTitle; 
//}
//inline float cNodeHeader::GetVersion() const
//{
//	return mVersion; 
//}
//
//inline void cNodeHeader::SetTitle(const char *treeTitle)
//{ 
//	strncpy_s(mTitle, treeTitle, strlen(treeTitle)); 
//}
//
//inline void cNodeHeader::SetVersion(float treeVersion)
//{ 
//	mVersion = treeVersion; 
//}
//
//inline void cNodeHeader::SetBuild(unsigned int treeBuild)
//{ 
//	mBuild = treeBuild; 
//}
//
//inline void cNodeHeader::SetPath(const char* headersPath)
//{ 
//	strncpy_s(mHeadersPath, headersPath, strlen(headersPath));
//}
//
//inline char* cNodeHeader::GetPath() const
//{ 
//	return (char *)mHeadersPath;
//}

inline void cNodeHeader::SetCacheMeasureCount(bool b)
{
	mCacheMeasureCount = b;
}

inline bool cNodeHeader::GetCacheMeasureCount() const
{
	return mCacheMeasureCount;
}

inline void cNodeHeader::SetCacheMeasureTime(bool b)
{
	mCacheMeasureTime = b;
}

inline bool cNodeHeader::GetCacheMeasureTime() const
{
	return mCacheMeasureTime;
}

/**
* \param sizeInfo Size info of the mItems in the node
*/
//void cNodeHeader::SetSizeInfo(void* sizeInfo)
//{
//	mSizeInfo = sizeInfo;
//}

/**
* \return Size info of the mItems in the node
*/
//inline void* cNodeHeader::GetSizeInfo() const
//{
//	return mSizeInfo;
//}

inline unsigned int cNodeHeader::GetNodeSerialSize() const
{
	return mNodeSerialSize;
}

inline unsigned int cNodeHeader::GetNodeInMemSize() const
{
	return mNodeInMemSize;
}

unsigned int cNodeHeader::GetNodeItemsSpaceSize() const
{
	return mNodeItemsSpaceSize;
}


void cNodeHeader::SetNodeItemsSpaceSize(unsigned int value)
{
	mNodeItemsSpaceSize = value;
}

inline void cNodeHeader::SetNodeSerialSize(unsigned int nodeSerialSize)
{ 
	mNodeSerialSize = nodeSerialSize; 
}

inline void cNodeHeader::SetNodeInMemSize(unsigned int nodeInMemSize)
{ 
	mNodeInMemSize = nodeInMemSize; 
}

unsigned int cNodeHeader::GetDStructCode() const
{
	return mDStructCode;
}

unsigned int cNodeHeader::GetDStructMode() const
{
	return mDStructMode;
}

unsigned int cNodeHeader::GetRuntimeMode() const
{
	return mRuntimeMode;
}

void cNodeHeader::SetDStructMode(unsigned int dsMode)
{
	mDStructMode = dsMode;
}

void cNodeHeader::SetRuntimeMode(unsigned int rtMode)
{
	mRuntimeMode = rtMode;
}

inline void cNodeHeader::SetMemoryPool(cMemoryPool* pool)
{
	mMemoryPool = (cMemoryPool*)pool;
}

inline cMemoryPool* cNodeHeader::GetMemoryPool()
{
	return mMemoryPool;
}

inline void cNodeHeader::SetMemoryManager(cMemoryManager* memMan)
{
	mMemoryManager = (cMemoryManager*)memMan;
}

#ifdef CUDA_ENABLED
inline void cNodeHeader::SetMemoryManagerCuda(cMemoryManagerCuda* memMan)
{
	mMemoryManagerCuda = memMan;
}

inline cMemoryManagerCuda* cNodeHeader::GetMemoryManagerCuda()
{
	return mMemoryManagerCuda;
}
#endif

inline cMemoryManager* cNodeHeader::GetMemoryManager()
{
	return mMemoryManager;
}

inline void cNodeHeader::SetKeyDescriptor(cDTDescriptor *pDtD)
{
	mKeyDescriptor = pDtD;
}

inline const cDTDescriptor* cNodeHeader::GetKeyDescriptor() const
{
	return mKeyDescriptor;
}

void cNodeHeader::Print()
{
	printf("Node count: %d\n", mNodeCount);
	printf("Item count: %d\n", mItemCount);
	printf("Node capacity: %d\n", mNodeCapacity);
	printf("Item size: %d\n", mItemSize);
}

}}}
#endif