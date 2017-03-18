/*
*
* cRTreeNodeHeader.h - hlavicka uzlu
* Radim Bača, David Bednář
* Jan 2011
*
*/

#ifndef __cUBTreeNodeHeader_h__
#define __cUBTreeNodeHeader_h__

#include "dstruct/paged/core/cTreeNodeHeader.h"

using namespace dstruct::paged::core;

namespace dstruct {
	namespace paged {
		namespace ubtree {

template<class TMbr>
class cUBTreeNodeHeader : public cTreeNodeHeader
{
	typedef typename TMbr::Tuple TKey;

public:
	cUBTreeNodeHeader(bool leafNode, const unsigned int innerKeySize, bool varlenData = false, unsigned int dsMode = cDStructConst::DSMODE_DEFAULT);
	~cUBTreeNodeHeader();

	virtual inline void WriteNode(cNode* block, cStream* stream);
	virtual inline void ReadNode(cNode* block, cStream* stream);
	virtual inline cNode* CopyNode(cNode* dest, cNode* source);
	virtual cNodeHeader* CreateCopy(unsigned int inMemSize);

	inline const cSpaceDescriptor* GetSpaceDescriptor() const;
};

template<class TMbr>
cUBTreeNodeHeader<TMbr>::cUBTreeNodeHeader(bool leafNode, const unsigned int innerKeySize, bool varlenData, unsigned int dsMode): 
	cTreeNodeHeader(leafNode, innerKeySize, 0, TMbr::LengthType == cDataType::LENGTH_VARLEN, varlenData, dsMode)
{	
}

template<class TMbr>
cUBTreeNodeHeader<TMbr>::~cUBTreeNodeHeader()
{ 
}

template<class TMbr>
inline const cSpaceDescriptor* cUBTreeNodeHeader<TMbr>::GetSpaceDescriptor() const
{
	return (cSpaceDescriptor*)GetKeyDescriptor();
}

template<class TMbr>
void cUBTreeNodeHeader<TMbr>::WriteNode(cNode* node, cStream* stream)
{
	((cUBTreeNode<TMbr>*)node)->Write(stream);
}

template<class TMbr>
void cUBTreeNodeHeader<TMbr>::ReadNode(cNode* node, cStream* stream)
{
	((cUBTreeNode<TMbr>*)node)->Read(stream);
}


template<class TMbr>
cNodeHeader* cUBTreeNodeHeader<TMbr>::CreateCopy(unsigned int inMemSize)
{
	cUBTreeNodeHeader<TMbr>* newHeader = new cUBTreeNodeHeader<TMbr>(false, mKeySize);

	this->SetCopy(newHeader);
	newHeader->ComputeNodeCapacity(inMemSize, false);

	return newHeader;
}

template<class TMbr>
cNode* cUBTreeNodeHeader<TMbr>::CopyNode(cNode* dest, cNode* source)
{
	cTreeNode<TKey> *d = (cTreeNode<TKey> *) dest;
	cTreeNode<TKey> *s = (cTreeNode<TKey> *) source;
	cUBTreeNodeHeader<TMbr> *sourceHeader = (cUBTreeNodeHeader<TMbr> *) source->GetHeader();
	
	d->SetItemCount(s->GetItemCount());
	d->GetHeader()->SetInnerItemCount(s->GetItemCount());
	d->SetIndex(s->GetIndex());
	d->Copy(s);

	return (cNode *) d;
}

//template<class TMbr>
//void cRTreeNodeHeader<TMbr>::FormatNode(cMemoryBlock* block)
//{
//	cRTreeNode<TMbr, cRTreeItem<TMbr>>* node = (cRTreeNode<TMbr, cRTreeItem<TMbr>>*)block->GetMemory(sizeof(cRTreeNode<TMbr, cRTreeItem<TMbr>>));
//	node->Format(this, block);
//}
}}}
#endif