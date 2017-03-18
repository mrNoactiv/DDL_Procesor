/**************************************************************************}
{                                                                          }
{    cRTreeHeader.h                                                        }
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

#ifndef __cRTreeHeader_h__
#define __cRTreeHeader_h__

#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "common/datatype/tuple/cMBRectangle.h"
#include "dstruct/paged/rtree/cRTreeNodeHeader.h"
#include "dstruct/paged/rtree/cRTreeLeafNodeHeader.h"
#include "dstruct/paged/core/cTreeHeader.h"
#include "dstruct/paged/rtree/cSignatureController.h"
#include "dstruct/paged/rtree/cRTreeSignatureIndex.h"

using namespace common::datatype::tuple;
using namespace common::utils;

namespace dstruct {
	namespace paged {
		namespace rtree {

template<class TKey>
class cRTreeHeader : public cTreeHeader
{
protected:
	cSpaceDescriptor* mSpaceDescriptor;
	bool mSignatureEnabled;		   // signature enable/disable	
	cSignatureController* mSignatureController;
	bool mOrderingEnabled;         // Ordered R-tree enabled/disabled 
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
	cRTreeHeader(cSpaceDescriptor* sd, 
		const uint leafDataSize, 
		bool varlenData = false, 
		uint dsMode = cDStructConst::DSMODE_DEFAULT,
		uint dsCode = cDStructConst::RTREE,
		uint compressionRatio = 1);
	~cRTreeHeader();

	inline virtual void Init();
	inline virtual bool Write(cStream *stream);
	inline virtual bool Read(cStream *stream);

	inline cSpaceDescriptor* GetSpaceDescriptor() const;

	inline bool GetOrderingEnabled() const;
	inline void SetOrderingEnabled(bool flag);

	inline bool IsSignatureEnabled() const;
	inline bool IsOnlyMemoryProcessing() const;

	inline void SetSignatureEnabled(bool signatureEnabled);
	inline void SetOnlyMemoryProcessing(bool onlyMemoryProcessing);

	inline void SetSignatureController(cSignatureController* pSignatureController);
	inline cRTreeSignatureIndex<TKey>* GetSignatureIndex();

	inline void SetSignatureIndex(cRTreeSignatureIndex<TKey> *signatureIndex);
	inline cSignatureController* GetSignatureController() const;

	void HeaderSetup(unsigned int blockSize);

	inline cRTreeLeafNodeHeader<TKey>* GetLeafNodeHeader();
	inline cMBRectangle<TKey>* GetTreeMBR();
};

template<class TKey>
inline cSpaceDescriptor* cRTreeHeader<TKey>::GetSpaceDescriptor() const
{
	return mSpaceDescriptor;
}

template<class TKey>
inline cRTreeLeafNodeHeader<TKey>* cRTreeHeader<TKey>::GetLeafNodeHeader()
{
	return (cRTreeLeafNodeHeader<TKey>*)(cDStructHeader::GetNodeHeader(cTreeHeader::HEADER_LEAFNODE));
}

template<class TKey>
inline cMBRectangle<TKey>* cRTreeHeader<TKey>::GetTreeMBR()
{
	return mTreeMBR;
}

template<class TKey>
cRTreeHeader<TKey>::cRTreeHeader(cSpaceDescriptor* sd, const uint leafDataSize, bool varlenData, uint dsMode, uint treeCode, uint compressionRatio)
{
	mSpaceDescriptor = (cSpaceDescriptor*)sd;
	Init(sd, cMBRectangle<TKey>::GetMaxSize(NULL, sd), TKey::GetMaxSize(NULL, sd), leafDataSize, varlenData, dsMode, treeCode, compressionRatio);
	AddHeaderSize(mSpaceDescriptor->GetSerialSize() + sizeof(bool));
}

template<class TKey>
cRTreeHeader<TKey>::~cRTreeHeader(void)
{
}

template<class TKey>
bool cRTreeHeader<TKey>::Write(cStream *stream)
{
	bool ret = cTreeHeader::Write(stream);
	ret &= mSpaceDescriptor->Write(stream);
	ret &= stream->Write((char*)&mOrderingEnabled, sizeof(bool));
	return ret;
}

template<class TKey>
bool cRTreeHeader<TKey>::Read(cStream *stream)
{
	bool ret = cTreeHeader::Read(stream);
	ret &= mSpaceDescriptor->Read(stream);
	ret &= stream->Read((char*)&mOrderingEnabled, sizeof(bool));
	return ret;
}

template<class TKey>
void cRTreeHeader<TKey>::Init()
{
	



}

template<class TKey>
void cRTreeHeader<TKey>::Init(cSpaceDescriptor* pSd, const uint innerKeySize, const uint leafKeySize, const uint leafDataSize, bool varlenData, 
	uint dsMode, uint dsCode, uint compressionRatio)
{
	cTreeHeader::Init();
	
	SetTitle("R-tree");
	SetName("R-tree1");
	SetVersion((float)0.20);
	SetBuild(0x20030808);
	SetOrderingEnabled(false);

	unsigned int headerCount = 2;
	SetNodeHeaderCount(headerCount);
	cRTreeNodeHeader<cMBRectangle<TKey>>* nodeHeader = new cRTreeNodeHeader<cMBRectangle<TKey>>(false, innerKeySize, varlenData, dsMode);
	nodeHeader->SetKeyDescriptor(pSd);
	cRTreeLeafNodeHeader<TKey>* leafNodeHeader = new cRTreeLeafNodeHeader<TKey>(true, leafKeySize, leafDataSize, varlenData, dsMode);
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
void cRTreeHeader<TKey>::HeaderSetup(unsigned int blockSize)
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
	
	if (mSignatureEnabled)
	{
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
}

template<class TKey>
inline bool cRTreeHeader<TKey>::GetOrderingEnabled() const
{
	return ((cTreeNodeHeader*)mNodeHeaders[HEADER_LEAFNODE])->GetOrderingEnabled();
}

template<class TKey>
inline void cRTreeHeader<TKey>::SetOrderingEnabled(bool flag)
{
	for (unsigned int i = 0; i < mNodeHeaderCount; i++)
	{
		((cTreeNodeHeader*)mNodeHeaders[i])->SetOrderingEnabled(flag);
	}
}

template<class TKey>
inline bool cRTreeHeader<TKey>::IsSignatureEnabled() const
{
	return mSignatureEnabled;
}

template<class TKey>
inline bool cRTreeHeader<TKey>::IsOnlyMemoryProcessing() const
{
	return mOnlyMemoryProcessing;
}

template<class TKey>
inline void cRTreeHeader<TKey>::SetSignatureEnabled(bool signatureEnabled)
{
	mSignatureEnabled = signatureEnabled;
}

template<class TKey>
inline void cRTreeHeader<TKey>::SetOnlyMemoryProcessing(bool onlyMemoryProcessing)
{
	mOnlyMemoryProcessing = onlyMemoryProcessing;
}

template<class TKey>
inline void cRTreeHeader<TKey>::SetSignatureController(cSignatureController* pSignatureController)
{
	mSignatureController = pSignatureController;
}

template<class TKey>
inline cSignatureController* cRTreeHeader<TKey>::GetSignatureController() const
{
	return mSignatureController;
}

template<class TKey>
inline void cRTreeHeader<TKey>::SetSignatureIndex(cRTreeSignatureIndex<TKey> *signatureIndex)
{
	((cRTreeLeafNodeHeader<TKey>*)mNodeHeaders[0])->SetSignatureIndex(signatureIndex);
	((cRTreeNodeHeader<cMBRectangle<TKey>>*)mNodeHeaders[1])->SetSignatureIndex(signatureIndex);
}

template<class TKey>
inline cRTreeSignatureIndex<TKey> * cRTreeHeader<TKey>::GetSignatureIndex()
{
	return ((cRTreeLeafNodeHeader<TKey>*)mNodeHeaders[0])->GetSignatureIndex();
	// retunr NULL;
}

}}}
#endif