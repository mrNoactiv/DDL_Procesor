/*
*
* cRTreeNodeHeader.h - hlavicka uzlu
* Radim Bača, David Bednář
* Jan 2011
*
*/

#ifndef __cRTreeNodeHeader_h__
#define __cRTreeNodeHeader_h__

// #include "dstruct/paged/rtree/cCommonRTreeNodeHeader.h"
#include "dstruct/paged/core/cTreeNodeHeader.h"
#include "dstruct/paged/rtree/cRTreeSignatureIndex.h"

using namespace dstruct::paged::core;

namespace dstruct {
	namespace paged {
		namespace rtree {

template<class TMbr>
class cRTreeNodeHeader : public cTreeNodeHeader // cCommonRTreeNodeHeader<TMbr>
{
	typedef typename TMbr::Tuple TKey;

private:
	cRTreeSignatureIndex<TKey> *mSignatureIndex;

public:
	cRTreeNodeHeader(bool leafNode, const unsigned int innerKeySize, bool varlenData = false, unsigned int dsMode = cDStructConst::DSMODE_DEFAULT);
	~cRTreeNodeHeader();

	virtual inline void WriteNode(cNode* block, cStream* stream);
	virtual inline void ReadNode(cNode* block, cStream* stream);
	virtual inline cNode* CopyNode(cNode* dest, cNode* source);
	virtual cNodeHeader* CreateCopy(unsigned int inMemSize);

	// virtual inline void FormatNode(cMemoryBlock* block);

	inline cRTreeSignatureIndex<TKey>* GetSignatureIndex() { return mSignatureIndex; }
	inline void SetSignatureIndex(cRTreeSignatureIndex<TKey> *pSignatureIndex) { mSignatureIndex = pSignatureIndex; }

	inline const cSpaceDescriptor* GetSpaceDescriptor() const;
};

template<class TMbr>
cRTreeNodeHeader<TMbr>::cRTreeNodeHeader(bool leafNode, const unsigned int innerKeySize, bool varlenData, unsigned int dsMode): 
cTreeNodeHeader(leafNode, innerKeySize, 0, TMbr::LengthType == cDataType::LENGTH_VARLEN, varlenData, dsMode), mSignatureIndex(NULL)
{	
}

template<class TMbr>
cRTreeNodeHeader<TMbr>::~cRTreeNodeHeader()
{ 
}

template<class TMbr>
inline const cSpaceDescriptor* cRTreeNodeHeader<TMbr>::GetSpaceDescriptor() const
{
	return (cSpaceDescriptor*)GetKeyDescriptor();
}

template<class TMbr>
void cRTreeNodeHeader<TMbr>::WriteNode(cNode* node, cStream* stream)
{
	((cRTreeNode<TMbr>*)node)->Write(stream);
}

template<class TMbr>
void cRTreeNodeHeader<TMbr>::ReadNode(cNode* node, cStream* stream)
{
	((cRTreeNode<TMbr>*)node)->Read(stream);
}


template<class TMbr>
cNodeHeader* cRTreeNodeHeader<TMbr>::CreateCopy(unsigned int inMemSize)
{
	cRTreeNodeHeader<TMbr>* newHeader = new cRTreeNodeHeader<TMbr>(false, mKeySize);

	this->SetCopy(newHeader);
	newHeader->ComputeNodeCapacity(inMemSize, false);

	return newHeader;
}

template<class TMbr>
cNode* cRTreeNodeHeader<TMbr>::CopyNode(cNode* dest, cNode* source)
{
	cTreeNode<TKey> *d = (cTreeNode<TKey> *) dest;
	cTreeNode<TKey> *s = (cTreeNode<TKey> *) source;
	cRTreeNodeHeader<TMbr> *sourceHeader = (cRTreeNodeHeader<TMbr> *) source->GetHeader();
	
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