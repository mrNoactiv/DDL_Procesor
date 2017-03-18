/*
	File:		cQTreeHeader.h
	Author:		Tomas Plinta, pli040
	Version:	0.1
	Date:		2011
	Brief implementation of QuadTree header
*/

#ifndef __cQTreeHeader_h__
#define __cQTreeHeader_h__

#include "dstruct/paged/core/cTreeHeader.h"
#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "dstruct/paged/qtree/cQTreeNodeHeader.h"

#include "math.h"

using namespace common::datatype::tuple;
using namespace common::utils;

typedef unsigned int uint;

namespace dstruct {
	namespace paged {
		namespace qtree {

template<class TKey>
class cQTreeHeader : public cTreeHeader
{
protected:
	cSpaceDescriptor* mSpaceDescriptor;
	uint mTreeCode;

public:
	static const uint TREECODE_QTREE = 30;
	static const uint HEADER_NODE = 1;
	static const uint HEADER_LEAFNODE = 0;

	cTimer* mCompressionTimer;
	cTimer* mDecompressionTimer;

	cQTreeHeader(cSpaceDescriptor* sd, const uint nodeDataSize);
	cQTreeHeader();
	~cQTreeHeader(void);

	inline virtual void Init();
	inline virtual bool Write(cStream* stream);
	inline virtual bool Read(cStream* stream);

	inline cSpaceDescriptor* GetSpaceDescriptor() const;
	inline unsigned int GetTreeCode() const;

	void HeaderSetup(uint blockSize);
	inline cQTreeNodeHeader<TKey>* GetNodeHeader();
	void ComputeNodeSize(uint &maxNodeInMemSize, uint &blockSize);

private:
	void Init(cSpaceDescriptor* sd,const uint noderKeySize, const uint nodeDataSize);

};

template<class TKey>
cQTreeHeader<TKey>::cQTreeHeader(cSpaceDescriptor* sd, const uint nodeDataSize)
{
	mSpaceDescriptor = (cSpaceDescriptor*)sd;
	Init(sd, TKey::GetSize(NULL, sd), nodeDataSize);
	AddHeaderSize(mSpaceDescriptor->GetSerialSize());
}

template<class TKey>
cQTreeHeader<TKey>::cQTreeHeader()
{
}

template<class TKey>
cQTreeHeader<TKey>::~cQTreeHeader(void)
{
	delete mCompressionTimer;
	delete mDecompressionTimer;
}

//initialization of tree header
template<class TKey>
void cQTreeHeader<TKey>::Init()
{
	cTreeHeader::Init();

	uint nodeSize = cTreeHeader::BLOCK_SIZE;

	char *treestr;

	treestr = "Q-Tree";
	
	SetTitle(treestr);   // title vs name
	SetName(treestr);
	SetVersion((float)0.10);
	SetBuild(0x20030808);

	mCompressionTimer = new cTimer();
	mCompressionTimer->Reset();
	mDecompressionTimer = new cTimer();
	mDecompressionTimer->Reset();
}

template<class TKey>
void cQTreeHeader<TKey>::Init(cSpaceDescriptor* pSd, const uint nodeKeySize, const uint nodeDataSize)
{
	Init();
	SetNodeHeaderCount(1);
	cQTreeNodeHeader<TKey>* nodeHeader = new cQTreeNodeHeader<TKey>(nodeKeySize,nodeDataSize);
	nodeHeader->SetKeyDescriptor(pSd);
	SetNodeHeader(cTreeHeader::HEADER_LEAFNODE,	nodeHeader);
}

template<class TKey>
bool cQTreeHeader<TKey>::Write(cStream *stream)
{
	bool ret = cTreeHeader::Write(stream);
	ret &= mSpaceDescriptor->Write(stream);
	return ret;
}

template<class TKey>
bool cQTreeHeader<TKey>::Read(cStream *stream)
{
	bool ret = cTreeHeader::Read(stream);
	if (mSpaceDescriptor == NULL)
	{
		mSpaceDescriptor = new cSpaceDescriptor(false);
	}
	ret &= mSpaceDescriptor->Read(stream);
	return ret;
}

template<class TKey>
inline cSpaceDescriptor* cQTreeHeader<TKey>::GetSpaceDescriptor() const
{
	return mSpaceDescriptor;
}

template<class TKey>
inline cQTreeNodeHeader<TKey>* cQTreeHeader<TKey>::GetNodeHeader()
{
	return (cQTreeNodeHeader<TKey>*)(cDStructHeader::GetNodeHeader(HEADER_LEAFNODE));
}

/**
 * The Point QTree has only one type of the node, in this framework, we need always to access leaf nodes.
 */
template<class TKey>
void cQTreeHeader<TKey>::HeaderSetup(uint blockSize)
{
	int dimension, fanoutCapacity;
	dimension = GetSpaceDescriptor()->GetDimension();
	fanoutCapacity = (uint)pow((double)2,(int)dimension);

	SetLeafNodeExtraItemCount(0);
	SetLeafNodeItemCapacity(1);		//number of items(tuples) in node
	SetLeafNodeFanoutCapacity(fanoutCapacity);	//number of links to child nodes
	SetLeafNodeExtraLinkCount(0);
	((cQTreeNodeHeader<TKey>*)mNodeHeaders[HEADER_LEAFNODE])->ComputeNodeCapacity(blockSize);
}

template<class TKey>
void cQTreeHeader<TKey>::ComputeNodeSize(uint &maxNodeInMemSize, uint &blockSize)
{
	uint linkCapacity = 0;
	int dimension;
	dimension = GetSpaceDescriptor()->GetDimension();
	linkCapacity = (uint)pow((double)2,(int)dimension);

	uint linkSize = sizeof(tNodeIndex) * linkCapacity;
	uint itemSize = sizeof(TKey) * (dimension + 1);

	blockSize = itemSize + linkSize * 1.1;//1.15//1.3;
	maxNodeInMemSize = blockSize * 1.1;
}

template<class TKey>
inline uint cQTreeHeader<TKey>::GetTreeCode() const
{
	return mTreeCode;
}

}}}
#endif


