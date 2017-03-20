/*
*
* cSequentialArrayNodeHeader.h - hlavicka uzlu
* Radim Bača, David Bednář
* Jan 2011
*
*/

#ifndef __cSequentialArrayNodeHeader_h__
#define __cSequentialArrayNodeHeader_h__

namespace dstruct {
	namespace paged {
		namespace sqarray {
template<class TItem> class cSequentialArrayNodeHeader;
}}}

#include "dstruct/paged/core/cNodeHeader.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayNode.h"

using namespace dstruct::paged::core;

namespace dstruct {
	namespace paged {
		namespace sqarray {

template<class TItem>
class cSequentialArrayNodeHeader: public cNodeHeader
{
	unsigned int mNodeExtraLinkCount;
	unsigned int mDataAreaSize;			/// Size of the area in the node, where the data are stored.
	unsigned int mExtraData;			/// Size of the data in the node storing some extra information (link to the next node, mRealSize). Relate to the data stored on the secondary storage.

public:
	static const unsigned int ItemSize_Links = sizeof(tNodeIndex);

	cSequentialArrayNodeHeader();
	cSequentialArrayNodeHeader(const cSequentialArrayNodeHeader &header);
	~cSequentialArrayNodeHeader();

	virtual inline void WriteNode(cNode* node, cStream* stream);
	virtual inline void ReadNode(cNode* node, cStream* stream);
	virtual inline cNode* CopyNode(cNode* dest, cNode* source);
	unsigned int GetDataAreaSize() const { return mDataAreaSize; }

	void ComputeNodeCapacity(unsigned int nodeSerialSize);
	virtual cNodeHeader* CreateCopy(unsigned int inMemSize);
};

template<class TItem>
cSequentialArrayNodeHeader<TItem>::cSequentialArrayNodeHeader() : cNodeHeader()
{
}

template<class TItem>
cSequentialArrayNodeHeader<TItem>::cSequentialArrayNodeHeader(const cSequentialArrayNodeHeader &header)
	:cNodeHeader(header)
{	
}

template<class TItem>
cSequentialArrayNodeHeader<TItem>::~cSequentialArrayNodeHeader()
{ 
}


template<class TItem>
void cSequentialArrayNodeHeader<TItem>::WriteNode(cNode* node, cStream* stream)
{
	((cSequentialArrayNode<TItem>*)node)->Write(stream);
}

template<class TItem>
void cSequentialArrayNodeHeader<TItem>::ReadNode(cNode* node, cStream* stream)
{
	((cSequentialArrayNode<TItem>*)node)->Read(stream);
}

/**
 * Node header set up. Must be called after the mNodeSize is set.
 */
template<class TItem>
void cSequentialArrayNodeHeader<TItem>::ComputeNodeCapacity(unsigned int nodeSerialSize)
{
	mNodeSerialSize = nodeSerialSize;
	mExtraData = cSequentialArrayNode<TItem>::GetNodeExtraSize(); // linkToNextNode + mRealSize + mItemCount
	mDataAreaSize = mNodeSerialSize - mExtraData;
	mNodeInMemSize = mDataAreaSize + sizeof(int); // data + linkToNextNode
	mNodeItemsSpaceSize = mDataAreaSize;
}


template<class TKey>
cNodeHeader* cSequentialArrayNodeHeader<TKey>::CreateCopy(unsigned int inMemSize)
{
	cSequentialArrayNodeHeader<TKey>* newHeader = new cSequentialArrayNodeHeader<TKey>();

	this->SetCopy(newHeader);
	newHeader->ComputeNodeCapacity(inMemSize);

	newHeader->SetKeyDescriptor((cDTDescriptor *) GetKeyDescriptor());
	return newHeader;
}

template<class TKey>
cNode* cSequentialArrayNodeHeader<TKey>::CopyNode(cNode* dest, cNode* source)
{
	return dest;
}

}}}
#endif