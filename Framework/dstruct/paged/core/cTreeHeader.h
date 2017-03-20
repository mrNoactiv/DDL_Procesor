/**
*	\file cTreeHeader.h
*	\author Radim Bača, David Bednář, Michal Krátký
*	\version 0.1
*	\date jul 2002
*	\version 0.2
*	\date jul 2011
*	\brief Header of paged tree
*/

#ifndef __cTreeHeader_h__
#define __cTreeHeader_h__

#include "common/cCommon.h"

namespace dstruct {
  namespace paged {
    namespace core {
		class cTreeHeader;
		class cHeader;
		class cDStructHeader;
}}}

#include "common/cCRCStatic.h"
#include "common/stream/cStream.h"
#include "dstruct/paged/core/cDStructHeader.h"
#include "dstruct/paged/core/cTreeNodeHeader.h"

using namespace common;

// #define TITLE_SIZE 128

namespace dstruct {
  namespace paged {
	namespace core {

/**
*	Header of paged tree
*
*	\author Radim Bača, David Bednář, Michal Krátký
*	\version 0.2
*	\date jul 2011
**/
class cTreeHeader: public cDStructHeader
{
public:
	static const unsigned int TITLE_SIZE = 128;

	static const unsigned int HEADER_NODE = 1;
	static const unsigned int HEADER_LEAFNODE = 0;
	static const unsigned int EXTRA_HEADER_NODE = 3;
	static const unsigned int EXTRA_HEADER_LEAFNODE = 2;

protected:

	uint mStructureId;			// unique id for structure
	uint mHeight;
	uint mRootIndex;

	bool mHistogramEnabled;
	uint mLastGpuLevel; //maximum of levels copyied in GPU's memory
public:

	cTreeHeader();
	cTreeHeader(const cTreeHeader &header);
	~cTreeHeader();

	virtual inline void Clear();
	virtual inline void Init();

	inline unsigned int GetCodeType() const;

	inline virtual bool Write(cStream *stream);
	inline virtual bool Read(cStream *stream);

	// void ComputeNodeSize(bool multiply = true);
	//void ComputeNodeCapacity(unsigned int maxNodeSerialSize);
	// void ComputeNodeCapacity(unsigned int compressionRate);

	void SetTreeCount(unsigned int count);
	inline void SetCurrentTree(unsigned int index);
	inline void SetHeight(unsigned int treeHeight);
	inline unsigned int IncrementHeight();

	inline void ResetNodeCount();
	inline void SetLeafNodeCount(unsigned int count);
	inline void SetInnerNodeCount(unsigned int count);
	inline unsigned int IncrementLeafNodeCount();
	inline unsigned int IncrementInnerNodeCount();

	inline void ResetItemCount();
	inline void SetLeafItemCount(unsigned int count);
	inline void SetInnerItemCount(unsigned int count);
	inline unsigned int IncrementLeafItemCount();
	inline unsigned int IncrementInnerItemCount();

	inline void SetRootIndex(unsigned int rootIndex);
	inline unsigned int NextNodeIndex();
	inline void SetMeasureTime(bool value);
	inline void SetMeasureCount(bool value);
	inline void SetCacheMeasureTime(bool value);
	inline void SetCacheMeasureCount(bool value);

	inline void SetNodeSerialSize(unsigned int nodeSerialSize);
	inline void SetLeafNodeSerialSize(unsigned int nodeSerialSize);
	inline void SetNodeInMemSize(unsigned int nodeInMemSize);
	inline void SetLeafNodeInMemSize(unsigned int nodeInMemSize);
	inline void SetKeySize(unsigned int inMemSize);
	inline void SetLeafDataSize(unsigned int inMemSize);

	inline void SetCodeType(uint codeType);
	inline void SetMaxCompressionRatio(uint compresionRatio);

	// inline void SetNodeSerialSizeAsMul(unsigned int mul);
	// inline void SetNodeItemSerialSize(unsigned int defaultItemSize);
	// inline void SetLeafNodeItemSerialSize(unsigned int defaultItemSize);
	inline void SetNodeItemSize(unsigned int defaultItemSize);
	inline void SetLeafNodeItemSize(unsigned int defaultItemSize);

	inline void SetNodeItemCapacity(unsigned int itemCapacity);
	inline void SetLeafNodeItemCapacity(unsigned int itemCapacity);
	inline void SetNodeFanoutCapacity(unsigned int fanoutCapacity);
	inline void SetLeafNodeFanoutCapacity(unsigned int fanoutCapacity);
	inline void SetNodeDeltaCapacity(int deltaCapacity);
	inline void SetLeafNodeDeltaCapacity(int deltaCapacity);

	inline void SetNodeExtraItemCount(unsigned int extraItemCount);
	inline void SetLeafNodeExtraItemCount(unsigned int extraItemCount);
	inline void SetNodeExtraLinkCount(unsigned int extraLinkCount);
	inline void SetLeafNodeExtraLinkCount(unsigned int extraLinkCount);

	inline void SetDStructMode(unsigned int dsMode);
	inline void SetRuntimeMode(unsigned int dsMode);
	inline void ComputeTmpBufferSize();

	inline unsigned int GetHeight() const;
	inline unsigned int GetNodeSerialSize() const;
	inline unsigned int GetLeafNodeSerialSize() const;
	inline unsigned int GetNodeInMemSize() const;
	inline unsigned int GetLeafNodeInMemSize() const;
	inline unsigned int GetNodeCount() const;
	inline unsigned int GetLeafNodeCount() const;
	inline unsigned int GetInnerNodeCount() const;
	inline unsigned int GetItemCount() const;
	inline unsigned int GetLeafItemCount() const;
	inline unsigned int GetInnerItemCount() const;
	// inline unsigned int GetNodeItemSerialSize() const;
	// inline unsigned int GetLeafNodeItemSerialSize() const;
	inline unsigned int GetNodeItemSize() const;
	inline unsigned int GetLeafNodeItemSize() const;
	inline unsigned int GetKeySize() const;
	inline unsigned int GetLeafDataSize() const;

	inline unsigned int GetRootIndex() const;
	inline float AverageNodeUtilization() const;
	inline float AverageLeafNodeUtilization() const;
	inline float AverageInnerNodeUtilization() const;

	inline unsigned int GetNodeItemCapacity() const;
	inline virtual unsigned int GetLeafNodeItemCapacity() const;
	inline unsigned int GetNodeFanoutCapacity() const;
	inline unsigned int GetLeafNodeFanoutCapacity() const;
	inline unsigned int GetLeafNodeDeltaCapacity() const;
	inline unsigned int GetNodeDeltaCapacity() const;

	inline unsigned int GetNodeExtraItemCount() const;
	inline unsigned int GetLeafNodeExtraItemCount() const;
	inline unsigned int GetNodeExtraLinkCount() const;
	inline unsigned int GetLeafNodeExtraLinkCount() const;
	inline unsigned int GetLeafNodeTmpItemCount() const;

	inline unsigned int GetDStructMode() const;
	inline unsigned int GetRuntimeMode() const;
	inline unsigned int GetTmpBufferSize() const;

	inline void SetGroupCounterCount(unsigned int value);
	inline void SetCounterCount(unsigned int value);
	inline void SetTimerCount(unsigned int value);

	inline unsigned int GetGroupCounterCount() const;
	inline unsigned int GetCounterCount() const;
	inline unsigned int GetTimerCount() const;

	inline void SetStructureId(unsigned int value);		// unique Id for structure
	inline unsigned int GetStructureId() const;

	inline cTreeNodeHeader* GetNodeHeader(unsigned int i) const;

	void Print();

	// Create histogram of inserting data
	inline bool IsHistogramEnabled() const;
	inline void SetHistogramEnabled(bool histogramEnabled);
#ifdef CUDA_ENABLED
	inline void SetLastGpuLevel(uint level); //maximum of levels copyied in GPU's memory
	inline uint GetLastGpuLevel(); //maximum of levels copyied in GPU's memory
#endif
};

inline void cTreeHeader::Clear()
{
	mStructureId = cCommon::UNDEFINED_UINT;
}

bool cTreeHeader::Write(cStream *stream)
{
	bool ok = cDStructHeader::Write(stream);
	
	ok &= stream->Write((char*)&mHeight, sizeof(unsigned int));
	ok &= stream->Write((char*)&mRootIndex, sizeof(unsigned int));
	return ok;
}

bool cTreeHeader::Read(cStream *stream) 
{
	bool ok = cDStructHeader::Read(stream);

	ok &= stream->Read((char*)&mHeight, sizeof(unsigned int));
	ok &= stream->Read((char*)&mRootIndex, sizeof(unsigned int));

	ComputeTmpBufferSize();

	return ok;
}

inline void cTreeHeader::Init()
{
	cDStructHeader::Init();

	AddHeaderSize(28 * sizeof(unsigned int) + sizeof(bool)); /* MK: ?28? */
	mStructureId = cCommon::UNDEFINED_UINT;
	SetNodeHeaderCount(0);
}

inline unsigned int cTreeHeader::IncrementHeight()
{ 
	return ++mHeight; 
}

inline void cTreeHeader::SetHeight(unsigned int treeHeight)
{ 
	mHeight = treeHeight; 
}

inline void cTreeHeader::ResetNodeCount()
{ 
	//mLeafNodeCount = mInnerNodeCount = 0;
	mNodeHeaders[0]->ResetNodeCount();
	mNodeHeaders[1]->ResetNodeCount();
}

inline void cTreeHeader::SetLeafNodeCount(unsigned int count)
{ 
	//mLeafNodeCount = count; 
	mNodeHeaders[0]->SetNodeCount(count);
}
inline void cTreeHeader::SetInnerNodeCount(unsigned int count)
{ 
	//mInnerNodeCount = count;
	mNodeHeaders[1]->SetNodeCount(count);
}
inline unsigned int cTreeHeader::IncrementLeafNodeCount()
{ 
	//return ++mLeafNodeCount; 
	return mNodeHeaders[0]->IncrementNodeCount();
}
inline unsigned int cTreeHeader::IncrementInnerNodeCount()
{ 
	//return ++mInnerNodeCount; 
	return mNodeHeaders[1]->IncrementNodeCount();
}

inline void cTreeHeader::ResetItemCount()
{ 
	//mLeafItemCount = mInnerItemCount = 0; 
	mNodeHeaders[0]->ResetItemCount();
}
inline void cTreeHeader::SetLeafItemCount(unsigned int count)
{ 
	//mLeafItemCount = count; 
	mNodeHeaders[0]->SetInnerItemCount(count);
}
inline void cTreeHeader::SetInnerItemCount(unsigned int count)
{ 
	//mInnerItemCount = count; 
	mNodeHeaders[1]->SetInnerItemCount(count);
}
inline unsigned int cTreeHeader::IncrementLeafItemCount()
{ 
	//return ++mLeafItemCount; 
	return mNodeHeaders[0]->IncrementItemCount();
}
inline unsigned int cTreeHeader::IncrementInnerItemCount()
{ 
	//return ++mInnerItemCount; 
	return mNodeHeaders[1]->IncrementItemCount();
}

inline void cTreeHeader::SetNodeSerialSize(unsigned int nodeSerialSize)
{ 
	//mNodeRealSize = nodeSize; 
	((cTreeNodeHeader*)mNodeHeaders[1])->SetNodeSerialSize(nodeSerialSize);
}
inline void cTreeHeader::SetLeafNodeSerialSize(unsigned int nodeSerialSize)
{ 
	//mLeafNodeRealSize = nodeSize; 
	((cTreeNodeHeader*)mNodeHeaders[0])->SetNodeSerialSize(nodeSerialSize);
}

inline void cTreeHeader::SetNodeInMemSize(unsigned int inMemSize)
{ 
	//mNodeRealSize = nodeSize; 
	((cTreeNodeHeader*)mNodeHeaders[1])->SetNodeInMemSize(inMemSize);
}
inline void cTreeHeader::SetLeafNodeInMemSize(unsigned int nodeInMemSize)
{ 
	//mLeafNodeRealSize = nodeSize; 
	((cTreeNodeHeader*)mNodeHeaders[0])->SetNodeInMemSize(nodeInMemSize);
}

//inline void cTreeHeader::SetNodeSerialSizeAsMul(unsigned int mul)
//{
//	//mNodeSize = mul * BLOCK_SIZE; 
//	((cTreeNodeHeader*)mNodeHeaders[0])->SetNodeSerialSizeAsMul(mul);
//	((cTreeNodeHeader*)mNodeHeaders[1])->SetNodeSerialSizeAsMul(mul);
//}

//inline void cTreeHeader::SetNodeItemSerialSize(unsigned int defaultItemSize)
//{ 
//	//mNodeItemSize = defaultItemSize;
//	//((cTreeNodeHeader*)mNodeHeaders[0])->SetNodeItemSize(defaultItemSize);
//	((cTreeNodeHeader*)mNodeHeaders[1])->SetItemSerialSize(defaultItemSize);
//}

//inline void cTreeHeader::SetLeafNodeItemSerialSize(unsigned int defaultItemSize)
//{ 
//	//mLeafNodeItemSize = defaultItemSize; 
//	//((cTreeNodeHeader*)mNodeHeaders[1])->SetNodeItemSize(defaultItemSize);
//	((cTreeNodeHeader*)mNodeHeaders[0])->SetItemSerialSize(defaultItemSize);
//}

inline void cTreeHeader::SetNodeItemSize(unsigned int defaultItemSize)
{ 
	//mNodeItemMainMemorySize = defaultItemSize; 
	((cTreeNodeHeader*)mNodeHeaders[1])->SetItemSize(defaultItemSize);
}

inline void cTreeHeader::SetLeafNodeItemSize(unsigned int defaultItemSize)
{ 
	//mLeafNodeItemMainMemorySize = defaultItemSize; 
	((cTreeNodeHeader*)mNodeHeaders[0])->SetItemSize(defaultItemSize);
}

inline void cTreeHeader::SetRootIndex(unsigned int rootIndex)
{ 
	mRootIndex = rootIndex; 
}

inline void cTreeHeader::SetMeasureTime(bool value)
{ 
	mMeasureTime = value; 
}
inline void cTreeHeader::SetMeasureCount(bool value){ 
	mMeasureCount = value; 
}
inline void cTreeHeader::SetCacheMeasureTime(bool value)
{ 
	mCacheMeasureTime = value; 
}
inline void cTreeHeader::SetCacheMeasureCount(bool value){ 
	mCacheMeasureCount = value; 
}

inline void cTreeHeader::SetNodeItemCapacity(unsigned int itemCapacity)
{ 
	mNodeHeaders[cTreeHeader::HEADER_NODE]->SetNodeCapacity(itemCapacity);
}

inline void cTreeHeader::SetLeafNodeItemCapacity(unsigned int itemCapacity)
{ 
	mNodeHeaders[cTreeHeader::HEADER_LEAFNODE]->SetNodeCapacity(itemCapacity);
}
inline void cTreeHeader::SetNodeFanoutCapacity(unsigned int fanoutCapacity)
{ 
	((cTreeNodeHeader*)mNodeHeaders[1])->SetNodeFanoutCapacity(fanoutCapacity);
}
inline void cTreeHeader::SetLeafNodeFanoutCapacity(unsigned int fanoutCapacity)
{
	((cTreeNodeHeader*)mNodeHeaders[0])->SetNodeFanoutCapacity(fanoutCapacity);
}
inline void cTreeHeader::SetNodeDeltaCapacity(int deltaCapacity)
{
	((cTreeNodeHeader*)mNodeHeaders[1])->SetNodeDeltaCapacity(deltaCapacity);
}

inline void cTreeHeader::SetLeafNodeDeltaCapacity(int deltaCapacity)
{
	//mLeafNodeDeltaCapacity = deltaCapacity;
	((cTreeNodeHeader*)mNodeHeaders[0])->SetNodeDeltaCapacity(deltaCapacity);
}

inline void cTreeHeader::SetNodeExtraItemCount(unsigned int extraItemCount)
{ 
	//mNodeExtraItemCount = extraItemCount; 
	((cTreeNodeHeader*)mNodeHeaders[1])->SetNodeExtraItemCount(extraItemCount);
}
inline void cTreeHeader::SetLeafNodeExtraItemCount(unsigned int extraItemCount)
{ 
	//mLeafNodeExtraItemCount = extraItemCount; 
	((cTreeNodeHeader*)mNodeHeaders[0])->SetNodeExtraItemCount(extraItemCount);
}
inline void cTreeHeader::SetNodeExtraLinkCount(unsigned int extraLinkCount)
{ 
	//mNodeExtraLinkCount = extraLinkCount; 
	((cTreeNodeHeader*)mNodeHeaders[1])->SetNodeExtraLinkCount(extraLinkCount);
}
inline void cTreeHeader::SetLeafNodeExtraLinkCount(unsigned int extraLinkCount)
{ 
	//mLeafNodeExtraLinkCount = extraLinkCount; 
	((cTreeNodeHeader*)mNodeHeaders[0])->SetNodeExtraLinkCount(extraLinkCount);
}

// ?? Proc ne LeafKey
inline void cTreeHeader::SetKeySize(unsigned int inMemSize)
{ 
	((cTreeNodeHeader*)mNodeHeaders[HEADER_LEAFNODE])->SetKeySize(inMemSize);
}

inline void cTreeHeader::SetLeafDataSize(unsigned int inMemSize)
{ 
	((cTreeNodeHeader*)mNodeHeaders[HEADER_LEAFNODE])->SetDataSize(inMemSize);
}

inline void cTreeHeader::SetCodeType(uint codeType)
{ 
	for (uint i = 0; i < mNodeHeaderCount; i++)
	{
		mNodeHeaders[i]->SetCodeType(codeType);
	}
}

inline void cTreeHeader::SetMaxCompressionRatio(uint compresionRatio)
{
	for (uint i = 0; i < mNodeHeaderCount; i++)
	{
		mNodeHeaders[i]->SetMaxCompressionRatio(compresionRatio);
	}
}

inline unsigned int cTreeHeader::GetHeight() const
{
	return mHeight;
}


inline void cTreeHeader::SetDStructMode(unsigned int dsMode)
{
	for (unsigned int i = 0; i < mNodeHeaderCount; i++)
	{
		mNodeHeaders[i]->SetDStructMode(dsMode);
	}
}

inline void cTreeHeader::SetRuntimeMode(unsigned int rtMode)
{
	for (unsigned int i = 0; i < mNodeHeaderCount; i++)
	{
		mNodeHeaders[i]->SetRuntimeMode(rtMode);
	}
}

inline void cTreeHeader::ComputeTmpBufferSize()
{ 
	unsigned int size = 0;

	// create a temporary buffer if necessary
	if (GetDStructMode() == cDStructConst::DSMODE_RI)
	{
		size = 2 * GetKeySize();
	}
	else if (GetDStructMode() == cDStructConst::DSMODE_CODING) 
	{
		size = (2*GetKeySize() + GetLeafDataSize()) * 2;
	}
	else if (GetDStructMode() == cDStructConst::DSMODE_RICODING)
	{
		size = (2*GetKeySize() + GetLeafDataSize()) * 4;
	}

	for (unsigned int i = 0 ; i < mNodeHeaderCount ; i++)
	{
		((cTreeNodeHeader*)mNodeHeaders[i])->SetTmpBufferSize(size);
	}
}



inline unsigned int cTreeHeader::GetCodeType() const
{ 
	return mNodeHeaders[HEADER_LEAFNODE]->GetCodeType();
}

//inline unsigned int cTreeHeader::GetNodeItemSerialSize() const 
//{ 
//	//return mNodeItemSize; 
//	return ((cTreeNodeHeader*)mNodeHeaders[1])->GetItemSerialSize();
//}

//inline unsigned int cTreeHeader::GetLeafNodeItemSerialSize() const 
//{ 
//	//return mLeafNodeItemSize; 
//	return ((cTreeNodeHeader*)mNodeHeaders[0])->GetItemSerialSize();
//}

inline unsigned int cTreeHeader::GetNodeItemSize() const 
{ 
	//return mNodeItemMainMemorySize; 
	return ((cTreeNodeHeader*)mNodeHeaders[1])->GetItemSize();
}

inline unsigned int cTreeHeader::GetLeafNodeItemSize() const 
{ 
	//return mLeafNodeItemMainMemorySize; 
	return ((cTreeNodeHeader*)mNodeHeaders[0])->GetItemSize();
}

inline unsigned int cTreeHeader::GetNodeCount() const
{ 
	//return mLeafNodeCount+mInnerNodeCount; 
	return ((cTreeNodeHeader*)mNodeHeaders[1])->GetNodeCount() + ((cTreeNodeHeader*)mNodeHeaders[0])->GetNodeCount();
}

inline unsigned int cTreeHeader::GetLeafNodeCount() const
{ 
	//return mLeafNodeCount; 
	return ((cTreeNodeHeader*)mNodeHeaders[0])->GetNodeCount();
}

inline unsigned int cTreeHeader::GetInnerNodeCount() const
{ 
	//return mInnerNodeCount; 
	return ((cTreeNodeHeader*)mNodeHeaders[1])->GetInnerNodeCount();
}

inline unsigned int cTreeHeader::GetItemCount() const
{ 
	//return mLeafItemCount+mInnerItemCount; 
	return ((cTreeNodeHeader*)mNodeHeaders[0])->GetItemCount() + ((cTreeNodeHeader*)mNodeHeaders[1])->GetItemCount();
}
inline unsigned int cTreeHeader::GetLeafItemCount() const
{ 
	//return mLeafItemCount; 
	return ((cTreeNodeHeader*)mNodeHeaders[0])->GetItemCount();
}
inline unsigned int cTreeHeader::GetInnerItemCount() const
{ 
	//return mInnerItemCount; 
	return ((cTreeNodeHeader*)mNodeHeaders[1])->GetInnerItemCount();
}

inline unsigned int cTreeHeader::GetNodeSerialSize() const
{
	//return mNodeRealSize;
	return ((cTreeNodeHeader*)mNodeHeaders[1])->GetNodeSerialSize();
}

inline unsigned int cTreeHeader::GetLeafNodeSerialSize() const
{
	//return mLeafNodeRealSize;
	return ((cTreeNodeHeader*)mNodeHeaders[0])->GetNodeSerialSize();
}

inline unsigned int cTreeHeader::GetNodeInMemSize() const
{
	//return mNodeRealSize;
	return ((cTreeNodeHeader*)mNodeHeaders[1])->GetNodeInMemSize();
}
inline unsigned int cTreeHeader::GetLeafNodeInMemSize() const
{
	//return mLeafNodeRealSize;
	return ((cTreeNodeHeader*)mNodeHeaders[0])->GetNodeInMemSize();
}

inline unsigned int cTreeHeader::GetRootIndex() const
{ 
	return mRootIndex; 
}

inline float cTreeHeader::AverageNodeUtilization() const
{ 
	/*return (float)(((float)(mLeafItemCount + mInnerItemCount) / (float)(mLeafNodeItemCapacity * mLeafNodeCount +
		mNodeItemCapacity * mInnerNodeCount)) * 100.0);*/
	return ((cTreeNodeHeader*)mNodeHeaders[0])->AverageNodeUtilization();
}
inline float cTreeHeader::AverageLeafNodeUtilization() const
{ 
	//return (float)(((float)mLeafItemCount / (float)(mLeafNodeItemCapacity * mLeafNodeCount))*100.0); 
	return ((cTreeNodeHeader*)mNodeHeaders[0])->AverageInnerNodeUtilization();
}
inline float cTreeHeader::AverageInnerNodeUtilization() const
{ 
	//return (float)(((float)mInnerItemCount / (float)(mNodeItemCapacity * mInnerNodeCount))*100.0); 
	return ((cTreeNodeHeader*)mNodeHeaders[1])->AverageInnerNodeUtilization();
}

inline unsigned int cTreeHeader::GetNodeItemCapacity() const
{ 
	//return mNodeItemCapacity; 
	return ((cTreeNodeHeader*)mNodeHeaders[1])->GetNodeCapacity();
}

inline unsigned int cTreeHeader::GetLeafNodeItemCapacity() const
{ 
	//return mLeafNodeItemCapacity; 
	return ((cTreeNodeHeader*)mNodeHeaders[0])->GetNodeCapacity();
}
inline unsigned int cTreeHeader::GetNodeFanoutCapacity() const
{ 
	//return mNodeFanoutCapacity; 
	return ((cTreeNodeHeader*)mNodeHeaders[1])->GetNodeFanoutCapacity();
}
inline unsigned int cTreeHeader::GetLeafNodeFanoutCapacity() const
{ 
	//return mLeafNodeFanoutCapacity; 
	return ((cTreeNodeHeader*)mNodeHeaders[0])->GetNodeFanoutCapacity();
}
inline unsigned int cTreeHeader::GetNodeDeltaCapacity() const
{ 
	//return mNodeDeltaCapacity; 
	return ((cTreeNodeHeader*)mNodeHeaders[1])->GetNodeDeltaCapacity();
}
inline unsigned int cTreeHeader::GetLeafNodeDeltaCapacity() const
{ 
	//return mLeafNodeDeltaCapacity;
	return ((cTreeNodeHeader*)mNodeHeaders[0])->GetNodeDeltaCapacity();
}

inline unsigned int cTreeHeader::GetNodeExtraItemCount() const
{ 
	//return mNodeExtraItemCount; 
	return ((cTreeNodeHeader*)mNodeHeaders[1])->GetNodeExtraItemCount();
}
inline unsigned int cTreeHeader::GetLeafNodeExtraItemCount() const
{ 
	//return mLeafNodeExtraItemCount; 
	return ((cTreeNodeHeader*)mNodeHeaders[0])->GetNodeExtraItemCount();
}
inline unsigned int cTreeHeader::GetNodeExtraLinkCount() const
{ 
	//return mNodeExtraLinkCount; 
	return ((cTreeNodeHeader*)mNodeHeaders[1])->GetNodeExtraLinkCount();
}
inline unsigned int cTreeHeader::GetLeafNodeExtraLinkCount() const
{ 
	//return mLeafNodeExtraLinkCount; 
	return ((cTreeNodeHeader*)mNodeHeaders[0])->GetNodeExtraLinkCount();
}

inline unsigned int cTreeHeader::GetKeySize() const
{
	return ((cTreeNodeHeader*)mNodeHeaders[HEADER_LEAFNODE])->GetKeySize();
}

inline unsigned int cTreeHeader::GetLeafDataSize() const
{
	return ((cTreeNodeHeader*)mNodeHeaders[HEADER_LEAFNODE])->GetDataSize();
}

inline void cTreeHeader::SetStructureId(unsigned int value)
{ 
	mStructureId = value; 
}
inline unsigned int cTreeHeader::GetStructureId() const
{ 
	return mStructureId;
}

inline unsigned int cTreeHeader::GetDStructMode() const
{
	return mNodeHeaders[0]->GetDStructMode();
}

inline unsigned int cTreeHeader::GetRuntimeMode() const
{
	return mNodeHeaders[0]->GetRuntimeMode();
}

inline unsigned int cTreeHeader::GetTmpBufferSize() const
{
	return ((cTreeNodeHeader*)mNodeHeaders[0])->GetTmpBufferSize();
}

inline cTreeNodeHeader* cTreeHeader::GetNodeHeader(unsigned int order) const
{
	return (cTreeNodeHeader*)mNodeHeaders[order];
}

inline void cTreeHeader::Print()		
{ 
	cDStructHeader::Print();
	//printf("cTreeHeader::Print()\n");
	for (unsigned int i = 0; i < mNodeHeaderCount; i++)
	{
		((cTreeNodeHeader*)mNodeHeaders[i])->Print();
	}
}

inline bool cTreeHeader::IsHistogramEnabled() const
{
	return mHistogramEnabled;
}

inline void cTreeHeader::SetHistogramEnabled(bool histogramEnabled)
{
	mHistogramEnabled = histogramEnabled;
}
#ifdef CUDA_ENABLED

inline void cTreeHeader::SetLastGpuLevel(uint level)
{
	mLastGpuLevel = level;
}


inline uint cTreeHeader::GetLastGpuLevel()
{
	return mLastGpuLevel;
}
#endif
}}}
#endif
