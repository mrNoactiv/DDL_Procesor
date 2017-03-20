/**
*	\file cTreeNodeHeader.h
*	\author Radim Bača, David Bednář, Michal Krátký
*	\version 0.1
*	\date jan 2011
*	\brief Header of cTreeNode
*/

#ifndef __cTreeNodeHeader_h__
#define __cTreeNodeHeader_h__

#include "dstruct/paged/core/cDStructConst.h"
#include "dstruct/paged/core/cNodeHeader.h"
#include "common/cMemory.h"
#include "common/stream/cStream.h"
#include "dstruct/paged/core/cMemoryPool.h"
#include "common/datatype/tuple/cTuple.h"

using namespace std;
using namespace common;
using namespace common::datatype;
using namespace common::datatype::tuple;

namespace dstruct {
  namespace paged {
	namespace core {

#define USEITEMORDER // if defined then the cTreeNode use the ItemOrder to sort the items in the node

/**
*	Header of cTreeHeader
*
*	\author Radim Bača, David Bednář, Michal Krátký
*	\version 0.1
*	\date oct 2011
**/

class cTreeNodeHeader: public cNodeHeader
{
private:
	static const bool ORDER_PRESERVING = true;       // preserves ordering of items (in Btree)

protected:
	// serialized attributes
	bool mOrderingEnabled;              /// It is only for the R-tree, it has to be moved to R-tree's classes.
	bool mVariableLenKey;				/// Whether a variable length keys are suported or not.
	bool mVariableLenData;				/// Whether a variable length data are suported or not.
	bool mIsLeaf;						/// Is true when the node header corresponds to the leaf node.
	
	unsigned int mKeySize;				/// Size of the key.
	unsigned int mDataSize;				/// Size of the data, e.g. values in leaf nodes, i.e. item = key + data.
	unsigned int mLinkSize;				/// Size of the link. 
	unsigned int mNodeFanoutCapacity;   // not used
	int mNodeDeltaCapacity;             // Number of links in the node, if the value is -1 it means that the links are not used at all.
	unsigned int mNodeExtraItemCount;
	unsigned int mNodeExtraLinkCount;
	unsigned int mTmpBufferSize;

	// computed attributes
	unsigned short mOffsetItems;
	unsigned short mOffsetItemOrder;
	// unsigned short mOffsetItemByteOrder;
	unsigned short mOffsetLinks;
	unsigned short mOffsetExtraItems;
	unsigned short mOffsetExtraLinks;

	// attributes for reference items
	ushort mOffsetSubNodesCount;
	ushort mOffsetSubNodesCapacity;
	ushort mOffsetUpdatesCount;
	ushort mOffsetSubNodesHeaders;
public:
	static const unsigned int ItemSize_ItemOrder = sizeof(unsigned short);
	static const unsigned int ItemSize_Links = sizeof(tNodeIndex);

	static const unsigned int NODE_PREFIX_SERIAL =  // the size of each node's prefix on the disk, see cTreeNode::Read and Write
		sizeof(unsigned int) +  // item count
		sizeof(unsigned int);   // free size

protected:
	void SetInMemOrders(bool isLeaf);
	virtual inline void SetCopy(cNodeHeader* header);
	// void ComputeOptimCapacity(unsigned int compressionRate = 1);

public:
	cTreeNodeHeader();
	cTreeNodeHeader(bool leafNode, unsigned int keySize, unsigned int dataSize = 0, bool varKey = false, bool varData = false, unsigned int dsMode = cDStructConst::DSMODE_DEFAULT);
	cTreeNodeHeader(const cTreeNodeHeader &header);
	~cTreeNodeHeader();

	virtual inline cNode* CopyNode(cNode* dest, cNode* source) = 0;
	// void ComputeNodeSize(bool multiply = true);
	virtual void ComputeNodeCapacity(unsigned int blockSize, bool isLeaf);
	// virtual void ComputeNodeCapacity(unsigned int compressionRate);

	//inline void AddHeaderSize(unsigned int userHeaderSerSize);

	virtual inline void Clear();
	virtual inline void Init(bool leafNode, unsigned int keyInMemSize, unsigned int dataInMemSize = 0, bool varKey = false, bool varData = false, unsigned int dsMode = cDStructConst::DSMODE_DEFAULT);

	inline virtual bool Write(cStream *stream);
	inline virtual bool Read(cStream *stream);
		
	inline void SetNodeItemSize(unsigned int defaultItemSize);
	inline void SetNodeFanoutCapacity(unsigned int fanoutCapacity);
	inline void SetNodeDeltaCapacity(int deltaCapacity);
	inline void SetNodeExtraItemCount(unsigned int extraItemCount);
	inline void SetNodeExtraLinkCount(unsigned int extraLinkCount);
	inline void SetKeySize(unsigned int value);
	inline void SetDataSize(unsigned int value);
	inline void SetLinkSize(unsigned int linksize);
	inline void SetTmpBufferSize(unsigned int size);

	inline unsigned int GetNodeFanoutCapacity() const;
	inline unsigned int GetNodeDeltaCapacity() const;
	inline unsigned int GetNodeExtraItemCount() const;
	inline unsigned int GetNodeExtraLinkCount() const;
	inline unsigned int GetKeySize() const;
	inline unsigned int GetDataSize() const;
	inline unsigned int GetLinkSize() const;
	inline unsigned int GetTmpBufferSize() const;

	inline bool GetOrderingEnabled() const;
	inline void SetOrderingEnabled(bool flag);
	inline bool VariableLenKeyEnabled() const;
	inline void VariableLenKeyEnabled(bool variableLen);
	inline bool VariableLenDataEnabled() const;
	inline void VariableLenDataEnabled(bool variableLen);

	inline unsigned short GetItemsOffset() const;
	inline unsigned short GetItemOrderOffset() const;
	// inline unsigned short GetItemByteOrderOffset() const;
	inline unsigned short GetLinksOffset() const;
	inline unsigned short GetExtraItemsOffset() const;
	inline unsigned short GetExtraLinksOffset() const;
	inline ushort GetSubNodesCountOffset() const;
	inline ushort GetSubNodesCapacityOffset() const;
	inline ushort GetUpdatesCountOffset() const;
	inline ushort GetSubNodesHeadersOffset() const;

	inline void SetItemsOffset(unsigned short value);
	inline void SetItemOrderOffset(unsigned short value);
	// inline void SetItemByteOrderOffset(unsigned short value);
	inline void SetLinksOffset(unsigned short value);
	inline void SetExtraItemsOffset(unsigned short value);
	inline void SetExtraLinksOffset(unsigned short value);
	inline void SetSubNodesCountOffset(unsigned short value);

	inline void Print();

	inline unsigned int GetInMemSize(unsigned int count);
	inline bool OrderPreserving() const;
};

/// ?? Chybi mDataInMemSize ??
unsigned int cTreeNodeHeader::GetInMemSize(unsigned int count)
{
	unsigned int size = count * GetKeySize();
	size += mNodeFanoutCapacity * sizeof(tNodeIndex);
	size += (mNodeExtraItemCount * sizeof(tNodeIndex)) + (mNodeExtraLinkCount * sizeof(tNodeIndex));

	return size;
}

inline void cTreeNodeHeader::Clear()
{
	//mLeafNodeCount = 0;
	//mNodeCount = 0;
	//mLeafItemCount = 0;
	//mItemCount = 0;
}

bool cTreeNodeHeader::Write(cStream *stream)
{
	bool ok = cNodeHeader::Write(stream);

	ok &= stream->Write((char*)&mKeySize, sizeof(unsigned int));
	ok &= stream->Write((char*)&mDataSize, sizeof(unsigned int));
	ok &= stream->Write((char*)&mNodeFanoutCapacity, sizeof(unsigned int));
	ok &= stream->Write((char*)&mNodeDeltaCapacity, sizeof(int));
	ok &= stream->Write((char*)&mNodeExtraItemCount, sizeof(unsigned int));
	ok &= stream->Write((char*)&mNodeExtraLinkCount, sizeof(unsigned int));
	ok &= stream->Write((char*)&mVariableLenKey, sizeof(bool));
	ok &= stream->Write((char*)&mVariableLenData, sizeof(bool));
	ok &= stream->Write((char*)&mIsLeaf, sizeof(bool));
	return ok;
}

bool cTreeNodeHeader::Read(cStream *stream) 
{
	bool ok = cNodeHeader::Read(stream);

	ok &= stream->Read((char*)&mKeySize, sizeof(unsigned int));
	ok &= stream->Read((char*)&mDataSize, sizeof(unsigned int));
	ok &= stream->Read((char*)&mNodeFanoutCapacity, sizeof(unsigned int));
	ok &= stream->Read((char*)&mNodeDeltaCapacity, sizeof(int));
	ok &= stream->Read((char*)&mNodeExtraItemCount, sizeof(unsigned int));
	ok &= stream->Read((char*)&mNodeExtraLinkCount, sizeof(unsigned int));
	ok &= stream->Read((char*)&mVariableLenKey, sizeof(bool));
	ok &= stream->Read((char*)&mVariableLenData, sizeof(bool));
	ok &= stream->Read((char*)&mIsLeaf, sizeof(bool));

	ComputeNodeCapacity(mNodeSerialSize, mIsLeaf);

	return ok;
}

inline void cTreeNodeHeader::SetCopy(cNodeHeader* newHeader)
{
	cNodeHeader::SetCopy(newHeader);

	((cTreeNodeHeader*)newHeader)->SetKeySize(mKeySize);
	((cTreeNodeHeader*)newHeader)->SetDataSize(mDataSize);
	((cTreeNodeHeader*)newHeader)->SetNodeFanoutCapacity(mNodeFanoutCapacity);
	((cTreeNodeHeader*)newHeader)->SetNodeDeltaCapacity(mNodeDeltaCapacity);
	((cTreeNodeHeader*)newHeader)->SetNodeExtraItemCount(mNodeExtraItemCount);
	((cTreeNodeHeader*)newHeader)->SetNodeExtraLinkCount(mNodeExtraLinkCount);

	//db
	((cTreeNodeHeader*)newHeader)->SetKeyDescriptor(mKeyDescriptor);
	((cTreeNodeHeader*)newHeader)->SetItemsOffset(mOffsetItems);
	((cTreeNodeHeader*)newHeader)->SetItemOrderOffset(mOffsetItemOrder);
	((cTreeNodeHeader*)newHeader)->SetLinksOffset(mOffsetLinks);
	((cTreeNodeHeader*)newHeader)->SetExtraItemsOffset(mOffsetExtraItems);
	((cTreeNodeHeader*)newHeader)->SetExtraLinksOffset(mOffsetExtraLinks);
	((cTreeNodeHeader*)newHeader)->VariableLenDataEnabled(mVariableLenData);
	((cTreeNodeHeader*)newHeader)->VariableLenKeyEnabled(mVariableLenKey);
}

/**
* Construktor
* \param itemInMemSize Size of an item in main memory.
* \param itemSerialSize Size of an item when serialized on the secondary storage.
*/

inline void cTreeNodeHeader::Init(bool leafNode, unsigned int keySize, unsigned int dataSize, bool varKey, bool varData, unsigned int dsMode)
{
	cNodeHeader::Init();

	mNodeSerialSize = cCommon::UNDEFINED_UINT;
	mNodeInMemSize = cCommon::UNDEFINED_UINT;
	mNodeDeltaCapacity = cCommon::UNDEFINED_INT;

	mNodeExtraItemCount = mNodeExtraLinkCount = cCommon::UNDEFINED_UINT;

	if (!leafNode)
	{
		SetItemSize(keySize + sizeof(tNodeIndex));
	} else
	{
		SetItemSize(keySize + dataSize);
	}
	SetKeySize(keySize);
	SetDataSize(dataSize);
	VariableLenKeyEnabled(varKey);
	VariableLenDataEnabled(varData);
	SetDStructMode(dsMode);
	//AddHeaderSize(28 * sizeof(unsigned int) + sizeof(bool));
}

inline void cTreeNodeHeader::SetNodeFanoutCapacity(unsigned int fanoutCapacity)
{ 
	mNodeFanoutCapacity = fanoutCapacity; 
}

inline void cTreeNodeHeader::SetNodeDeltaCapacity(int deltaCapacity)
{
	mNodeDeltaCapacity = deltaCapacity;
}

inline void cTreeNodeHeader::SetNodeExtraItemCount(unsigned int extraItemCount)
{ 
	mNodeExtraItemCount = extraItemCount; 
}

inline void cTreeNodeHeader::SetNodeExtraLinkCount(unsigned int extraLinkCount)
{ 
	mNodeExtraLinkCount = extraLinkCount; 
}

inline void cTreeNodeHeader::SetKeySize(unsigned int pValue)
{
	mKeySize = pValue;
}

inline void cTreeNodeHeader::SetDataSize(unsigned int pValue)
{
	mDataSize = pValue;
}

inline void cTreeNodeHeader::SetLinkSize(unsigned int linksize)
{
	mLinkSize = linksize;
}

inline void cTreeNodeHeader::SetTmpBufferSize(unsigned int size)
{
	mTmpBufferSize = size;
}

inline bool cTreeNodeHeader::OrderPreserving() const
{
	return ORDER_PRESERVING;
}
inline unsigned int cTreeNodeHeader::GetNodeFanoutCapacity() const
{ 
	return mNodeFanoutCapacity; 
}

inline unsigned int cTreeNodeHeader::GetNodeDeltaCapacity() const
{ 
	return mNodeDeltaCapacity; 
}

inline unsigned int cTreeNodeHeader::GetNodeExtraItemCount() const
{ 
	return mNodeExtraItemCount; 
}

inline unsigned int cTreeNodeHeader::GetNodeExtraLinkCount() const
{ 
	return mNodeExtraLinkCount; 
}

inline unsigned short cTreeNodeHeader::GetItemsOffset() const 
{
	return mOffsetItems;
}

inline unsigned short cTreeNodeHeader::GetItemOrderOffset() const 
{
	return mOffsetItemOrder;
}

/*
inline unsigned short cTreeNodeHeader::GetItemByteOrderOffset() const 
{
	return mOffsetItemByteOrder;
}*/

inline unsigned short cTreeNodeHeader::GetLinksOffset() const 
{
	return mOffsetLinks;
}

inline unsigned short cTreeNodeHeader::GetExtraItemsOffset() const 
{
	return mOffsetExtraItems;
}

inline unsigned short cTreeNodeHeader::GetExtraLinksOffset() const 
{
	return mOffsetExtraLinks;
}

inline ushort cTreeNodeHeader::GetSubNodesCountOffset() const 
{
	return mOffsetSubNodesCount;
}

inline ushort cTreeNodeHeader::GetSubNodesCapacityOffset() const
{
	return mOffsetSubNodesCapacity;
}

inline ushort cTreeNodeHeader::GetUpdatesCountOffset() const
{
	return mOffsetUpdatesCount;
}

inline ushort cTreeNodeHeader::GetSubNodesHeadersOffset() const
{
	return mOffsetSubNodesHeaders;
}


inline bool cTreeNodeHeader::GetOrderingEnabled() const
{
	return mOrderingEnabled;
}

bool cTreeNodeHeader::VariableLenKeyEnabled() const
{
	return mVariableLenKey;
}

void cTreeNodeHeader::VariableLenKeyEnabled(bool variableLen)
{
	mVariableLenKey = variableLen;
}

bool cTreeNodeHeader::VariableLenDataEnabled() const
{
	return mVariableLenData;
}

void cTreeNodeHeader::VariableLenDataEnabled(bool variableLen)
{
	mVariableLenData = variableLen;
}

inline unsigned int cTreeNodeHeader::GetKeySize() const
{
	return mKeySize;
}

inline unsigned int cTreeNodeHeader::GetDataSize() const
{
	return mDataSize;
}

inline unsigned int cTreeNodeHeader::GetLinkSize() const
{
	return mLinkSize;
}

inline unsigned int cTreeNodeHeader::GetTmpBufferSize() const
{
	return mTmpBufferSize;
}

inline void cTreeNodeHeader::SetOrderingEnabled(bool flag)
{
	mOrderingEnabled = flag;
}

inline void cTreeNodeHeader::SetItemsOffset(unsigned short value)
{
	mOffsetItems = value;
}

inline void cTreeNodeHeader::SetItemOrderOffset(unsigned short value)
{
	mOffsetItemOrder = value;
}

inline void cTreeNodeHeader::SetLinksOffset(unsigned short value)
{
	mOffsetLinks = value;
}

inline void cTreeNodeHeader::SetExtraItemsOffset(unsigned short value)
{
	mOffsetExtraItems = value;
}

inline void cTreeNodeHeader::SetExtraLinksOffset(unsigned short value)
{
	mOffsetExtraLinks = value;
}

inline void cTreeNodeHeader::SetSubNodesCountOffset(unsigned short value)
{
	mOffsetSubNodesCount = value;
}

/*inline void cTreeNodeHeader::SetFirstRIBlockOffset(unsigned short value)
{
	mOffsetFirstRIBlock = value;
}*/

inline void cTreeNodeHeader::Print()
{
	// TODO, stream bude novy soubor, specialne pro hlavicky uzlu

	cNodeHeader::Print();
	//printf("%d\n", mNodeFanoutCapacity);
	printf("Delta (links segment) capacity: %d\n", mNodeDeltaCapacity);
	//printf("%d\n", mNodeExtraItemCount);
	printf("Extra link count: %d\n", mNodeExtraLinkCount);
	if (VariableLenKeyEnabled())
	{
		printf("Variable length key enabled;\t");
	} else
	{
		printf("Fixed length key of size %d;\t", GetKeySize());
	}
	if (VariableLenDataEnabled())
	{
		printf("Variable length data enabled\n");
	} else
	{
		printf("Fixed length data of size %d\n", GetDataSize());
	}
}

///**
//* Call the write method on the cTreeNode. It should be overloaded by the inherited class
//* if it uses specific node with a specific write method.
//*/
//void cTreeNodeHeader::WriteNode(cNode* node, cStream* stream)
//{
//	((cTreeNode<cUniformTuple>*)node)->Write(stream);
//}
//
///**
//* Call the read method on the cTreeNode. It should be overloaded by the inherited class
//* if it uses specific node with a specific read method.
//*/
//void cTreeNodeHeader::ReadNode(cNode* node, cStream* stream)
//{
//	((cTreeNode<cUniformTuple>*)node)->Read(stream);
//}

}}}
#endif