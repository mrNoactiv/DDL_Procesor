/*
	File:		cQTreeNodeHeader.h
	Author:		Tomas Plinta, pli040
	Version:	0.1
	Date:		2011
	Brief implementation of QuadTree node header
*/

#ifndef __cQTreeNodeHeader_h__
#define __cQTreeNodeHeader_h__

#include "dstruct/paged/core/cFCNodeHeader.h"
#include "math.h"

using namespace dstruct::paged::core;

typedef unsigned int uint;

namespace dstruct {
	namespace paged {
		namespace qtree {

template<class TKey>
class cQTreeNodeHeader: public cFCNodeHeader
{

public:
	cQTreeNodeHeader(unsigned int keyInMemSize, unsigned int dataInMemSize = 0);
	~cQTreeNodeHeader();

	inline cSpaceDescriptor* GetSpaceDescriptor() const;

	virtual inline void WriteNode(cNode* block, cStream* stream);
	virtual inline void ReadNode(cNode* block, cStream* stream);
	virtual inline cNode* CopyNode(cNode* dest, cNode* source);
	virtual cNodeHeader* CreateCopy(unsigned int inMemSize);

	void ComputeNodeCapacity(unsigned int blockSize);


};

template<class TKey>
cQTreeNodeHeader<TKey>::cQTreeNodeHeader(unsigned int keyInMemSize, unsigned int dataInMemSize = 0): cFCNodeHeader(keyInMemSize, dataInMemSize)
{
}

template<class TKey>
cQTreeNodeHeader<TKey>::~cQTreeNodeHeader()
{
}

template<class TKey>
inline cSpaceDescriptor* cQTreeNodeHeader<TKey>::GetSpaceDescriptor() const
{
	return (cSpaceDescriptor*)GetKeyDescriptor();
}

// Computing real size of node in memory.
template<class TKey>
void cQTreeNodeHeader<TKey>::ComputeNodeCapacity(unsigned int blockSize)
{

	uint linkSize = sizeof(tNodeIndex);
	uint itemSize = GetItemSize();
	uint capacity = 1;
	uint linkCapacity = 0;
	int dimension;

	dimension = GetSpaceDescriptor()->GetDimension();
	linkCapacity = (uint)pow((double)2,(int)dimension);

	uint basSize = sizeof(char) +		// information about the real size of the node (related to the cache rows)
		sizeof(uint) +			//item count
		sizeof(uint) +			//free size
		itemSize * capacity  +		
		linkSize * linkCapacity;
	uint size = blockSize - basSize;

	mIsLeaf = true;
	SetNodeCapacity(capacity);
	mNodeItemsSpaceSize = size - capacity * sizeof(tNodeIndex);
	SetNodeSerialSize(basSize + capacity * sizeof(tNodeIndex) + GetNodeItemsSpaceSize());
	SetInMemOrders();
}

template<class TKey>
void cQTreeNodeHeader<TKey>::WriteNode(cNode* node, cStream* stream)
{
	((cQTreeNode<TKey>*)node)->Write(stream);
}

template<class TKey>
void cQTreeNodeHeader<TKey>::ReadNode(cNode* node, cStream* stream)
{
	((cQTreeNode<TKey>*)node)->Read(stream);
}

template<class TKey>
cNodeHeader* cQTreeNodeHeader<TKey>::CreateCopy(unsigned int size)
{
	cQTreeNodeHeader<TKey>* newHeader = new cQTreeNodeHeader<TKey>(mKeySize);

	this->SetCopy(newHeader);
	newHeader->ComputeNodeCapacity(size);

	return newHeader;
}

template<class TKey>
cNode* cQTreeNodeHeader<TKey>::CopyNode(cNode* dest, cNode* source)
{
	cFCNode<TKey> *d = (cFCNode<TKey> *) dest;
	cFCNode<TKey> *s = (cFCNode<TKey> *) source;
	cQTreeNodeHeader<TKey> *sourceHeader = (cQTreeNodeHeader<TKey> *) source->GetHeader();
	
	d->SetItemCount(s->GetItemCount());
	d->GetHeader()->SetInnerItemCount(s->GetItemCount());
	d->SetIndex(s->GetIndex());
	d->Copy(s);

	return (cNode *) d;
}

}}}
#endif