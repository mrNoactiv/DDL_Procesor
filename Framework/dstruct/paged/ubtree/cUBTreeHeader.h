/**************************************************************************}
{                                                                          }
{    cUBTreeHeader.h                                                        }
{                                                                          }
{                                                                          }
{    Copyright (c) 2001                      Michal Kratky                 }
{                                                                          }
{    VERSION: 0.01                           DATE 18/11/2003               }
{                                                                          }
{    following functionality:                                              }
{       R-Tree header                                                      }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      xx/xx/xxxx                                                          }
{                                                                          }
{**************************************************************************/

#ifndef __cUBTreeHeader_h__
#define __cUBTreeHeader_h__

#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "common/datatype/tuple/cMBRectangle.h"
#include "dstruct/paged/ubtree/cUBTreeNodeHeader.h"
#include "dstruct/paged/ubtree/cUBTreeLeafNodeHeader.h"
#include "dstruct/paged/core/cTreeHeader.h"

using namespace common::datatype::tuple;
using namespace common::utils;

namespace dstruct {
	namespace paged {
		namespace ubtree {

template<class TKey>
class cUBTreeHeader : public cTreeHeader
{
protected:
	cSpaceDescriptor* mSpaceDescriptor;
	bool mOnlyMemoryProcessing;	   // set true, when we want to preload all pages into memory before query processing (neccessary for signature rtree)
	cMBRectangle<TKey>* mTreeMBR; // MBR of root node

private:
	void Init(cSpaceDescriptor *pSd, 
		const uint innerKeySize, 
		const uint leafKeySize, 
		const uint leafDataSize, 
		bool varlenData = false, 
		uint dsMode = cDStructConst::DSMODE_DEFAULT, 
		uint dsCode = cDStructConst::RTREE, 
		uint compressionRatio = 1);

public:
	cUBTreeHeader(cSpaceDescriptor* sd, 
		const uint leafDataSize, 
		bool varlenData = false, 
		uint dsMode = cDStructConst::DSMODE_DEFAULT,
		uint dsCode = cDStructConst::RTREE,
		uint compressionRatio = 1);
	~cUBTreeHeader();

	inline virtual void Init();
	inline virtual bool Write(cStream *stream);
	inline virtual bool Read(cStream *stream);

	inline cSpaceDescriptor* GetSpaceDescriptor() const;

	inline bool IsOnlyMemoryProcessing() const;
	inline void SetOnlyMemoryProcessing(bool onlyMemoryProcessing);

	void HeaderSetup(unsigned int blockSize);

	inline cUBTreeLeafNodeHeader<TKey>* GetLeafNodeHeader();
	inline cMBRectangle<TKey>* GetTreeMBR();
};

template<class TKey>
inline cSpaceDescriptor* cUBTreeHeader<TKey>::GetSpaceDescriptor() const
{
	return mSpaceDescriptor;
}

template<class TKey>
inline cUBTreeLeafNodeHeader<TKey>* cUBTreeHeader<TKey>::GetLeafNodeHeader()
{
	return (cUBTreeLeafNodeHeader<TKey>*)(cDStructHeader::GetNodeHeader(cTreeHeader::HEADER_LEAFNODE));
}

template<class TKey>
inline cMBRectangle<TKey>* cUBTreeHeader<TKey>::GetTreeMBR()
{
	return mTreeMBR;
}

template<class TKey>
cUBTreeHeader<TKey>::cUBTreeHeader(cSpaceDescriptor* sd, const uint leafDataSize, bool varlenData, uint dsMode, uint treeCode, uint compressionRatio)
{
	mSpaceDescriptor = (cSpaceDescriptor*)sd;
	Init(sd, cMBRectangle<TKey>::GetMaxSize(NULL, sd), TKey::GetMaxSize(NULL, sd), leafDataSize, varlenData, dsMode, treeCode, compressionRatio);
	AddHeaderSize(mSpaceDescriptor->GetSerialSize() + sizeof(bool));
}

template<class TKey>
cUBTreeHeader<TKey>::~cUBTreeHeader(void)
{
}

template<class TKey>
bool cUBTreeHeader<TKey>::Write(cStream *stream)
{
	bool ret = cTreeHeader::Write(stream);
	ret &= mSpaceDescriptor->Write(stream);
	return ret;
}

template<class TKey>
bool cUBTreeHeader<TKey>::Read(cStream *stream)
{
	bool ret = cTreeHeader::Read(stream);
	ret &= mSpaceDescriptor->Read(stream);
	return ret;
}

template<class TKey>
void cUBTreeHeader<TKey>::Init()
{
}

template<class TKey>
void cUBTreeHeader<TKey>::Init(cSpaceDescriptor* pSd, const uint innerKeySize, const uint leafKeySize, const uint leafDataSize, bool varlenData, 
	uint dsMode, uint dsCode, uint compressionRatio)
{
	cTreeHeader::Init();
	
	SetTitle("R-tree");
	SetName("R-tree1");
	SetVersion((float)0.20);
	SetBuild(0x20030808);

	unsigned int headerCount = 2;
	SetNodeHeaderCount(headerCount);
	cUBTreeNodeHeader<cMBRectangle<TKey>>* nodeHeader = new cUBTreeNodeHeader<cMBRectangle<TKey>>(false, innerKeySize, varlenData, dsMode);
	nodeHeader->SetKeyDescriptor(pSd);
	cUBTreeLeafNodeHeader<TKey>* leafNodeHeader = new cUBTreeLeafNodeHeader<TKey>(true, leafKeySize, leafDataSize, varlenData, dsMode);
	leafNodeHeader->SetKeyDescriptor(pSd);

	SetNodeHeader(cTreeHeader::HEADER_NODE,	nodeHeader);
	SetNodeHeader(cTreeHeader::HEADER_LEAFNODE, leafNodeHeader);
	GetNodeHeader(cTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor();
	mDStructCode = dsCode;
	SetDStructCode(dsCode);

	((cTreeNodeHeader*)mNodeHeaders[HEADER_LEAFNODE])->SetMaxCompressionRatio(compressionRatio);
	((cTreeNodeHeader*)mNodeHeaders[HEADER_NODE])->SetMaxCompressionRatio(compressionRatio);
	ComputeTmpBufferSize();

	mTreeMBR = new cMBRectangle<TKey>(pSd);
}

template<class TKey>
void cUBTreeHeader<TKey>::HeaderSetup(unsigned int blockSize)
{
	SetNodeDeltaCapacity(0);
	SetNodeExtraLinkCount(0);

	SetNodeExtraItemCount(0);
	SetLeafNodeFanoutCapacity(0);
	SetLeafNodeExtraItemCount(0);
	SetLeafNodeExtraLinkCount(1);

	SetCacheMeasureTime(true);
	SetCacheMeasureCount(true);

	DuplicatesAllowed(false);

	((cTreeNodeHeader*)mNodeHeaders[HEADER_LEAFNODE])->ComputeNodeCapacity(blockSize, true);
	((cTreeNodeHeader*)mNodeHeaders[HEADER_NODE])->ComputeNodeCapacity(blockSize, false);

	assert((mSignatureEnabled && mSignatureController != NULL) || !mSignatureEnabled);
	const cDTDescriptor* dt = GetNodeHeader(cTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor();
	cSpaceDescriptor* sd = (cSpaceDescriptor*)GetNodeHeader(cTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor();
	
	switch (mSignatureController->GetSignatureType())
	{
	case cSignatureController::DimensionIndependent:
		mSignatureController->Setup_DIS(sd->GetDimension(), GetNodeItemCapacity(), GetLeafNodeItemCapacity(), cSequentialArrayNode<cUInt>::GetNodeFreeSpace(blockSize));
		break;
	case cSignatureController::DimensionDependent:
		mSignatureController->Setup_DDS(sd->GetDimension(), GetNodeItemCapacity(), GetLeafNodeItemCapacity(), cSequentialArrayNode<cUInt>::GetNodeFreeSpace(blockSize));
		break;
	case cSignatureController::DimensionDependent_Orders:
		mSignatureController->Setup_DDO(sd->GetDimension(), GetNodeItemCapacity(), GetLeafNodeItemCapacity(), cSequentialArrayNode<cUInt>::GetNodeFreeSpace(blockSize));
		break;
	}
}

template<class TKey>
inline bool cUBTreeHeader<TKey>::IsOnlyMemoryProcessing() const
{
	return mOnlyMemoryProcessing;
}

template<class TKey>
inline void cUBTreeHeader<TKey>::SetOnlyMemoryProcessing(bool onlyMemoryProcessing)
{
	mOnlyMemoryProcessing = onlyMemoryProcessing;
}

}}}
#endif