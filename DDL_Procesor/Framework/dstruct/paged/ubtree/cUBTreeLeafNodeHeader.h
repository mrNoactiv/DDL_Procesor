/*
*
* cUBTreeLeafNodeHeader.h - hlavicka uzlu
* Radim Bača, David Bednář
* Jan 2011
*
*/

#ifndef __cUBTreeLeafNodeHeader_h__
#define __cUBTreeLeafNodeHeader_h__

namespace dstruct {
	namespace paged {
		namespace ubtree {
  template<class TKey> class cUBTreeLeafNodeHeader;
}}}

#include "dstruct/paged/core/cTreeNodeHeader.h"

using namespace dstruct::paged::core;

namespace dstruct {
	namespace paged {
		namespace ubtree {

template<class TKey>
class cUBTreeLeafNodeHeader : public cTreeNodeHeader
{
public:
	cUBTreeLeafNodeHeader(bool leafNode, const unsigned int leafKeySize, const unsigned int leafDataSize, bool varlenKey, bool varlenData = false, unsigned int dsMode = cDStructConst::DSMODE_DEFAULT);
	cUBTreeLeafNodeHeader(bool leafNode, const unsigned int leafKeySize, const unsigned int leafDataSize, bool varlenData = false, unsigned int dsMode = cDStructConst::DSMODE_DEFAULT); //bas064
	~cUBTreeLeafNodeHeader();

	virtual inline void WriteNode(cNode* block, cStream* stream);
	virtual inline void ReadNode(cNode* block, cStream* stream);
	virtual cNodeHeader* CreateCopy(unsigned int inMemSize);
	// virtual inline void FormatNode(cMemoryBlock* block);		
	virtual inline cNode* CopyNode(cNode* dest, cNode* source);

	inline const cSpaceDescriptor* GetSpaceDescriptor() const;
};

template<class TKey>
cUBTreeLeafNodeHeader<TKey>::cUBTreeLeafNodeHeader(bool leafNode, const unsigned int leafKeySize, const unsigned int leafDataSize, bool varlenKey, bool varlenData, unsigned int dsMode):
cTreeNodeHeader(leafNode, leafKeySize, leafDataSize, varlenKey, varlenData, dsMode)
{	
}

template<class TKey>
cUBTreeLeafNodeHeader<TKey>::cUBTreeLeafNodeHeader(bool leafNode, const unsigned int leafKeySize, const unsigned int leafDataSize, bool varlenData, unsigned int dsMode):
cTreeNodeHeader(leafNode, leafKeySize, leafDataSize, TKey::LengthType == cDataType::LENGTH_VARLEN, varlenData, dsMode)
{	
}

template<class TKey>
cUBTreeLeafNodeHeader<TKey>::~cUBTreeLeafNodeHeader()
{ 
}

template<class TKey>
inline const cSpaceDescriptor* cUBTreeLeafNodeHeader<TKey>::GetSpaceDescriptor() const
{
	return (cSpaceDescriptor*)GetKeyDescriptor();
}

template<class TKey>
void cUBTreeLeafNodeHeader<TKey>::WriteNode(cNode* node, cStream* stream)
{
	((cUBTreeLeafNode<TKey>*)node)->Write(stream);
}

template<class TKey>
void cUBTreeLeafNodeHeader<TKey>::ReadNode(cNode* node, cStream* stream)
{
	((cUBTreeLeafNode<TKey>*)node)->Read(stream);
}

template<class TKey>
cNodeHeader* cUBTreeLeafNodeHeader<TKey>::CreateCopy(unsigned int inMemSize)
{
	cUBTreeLeafNodeHeader<TKey>* newHeader = new cUBTreeLeafNodeHeader<TKey>(true, mKeySize, mDataSize);

	this->SetCopy(newHeader);
	newHeader->ComputeNodeCapacity(inMemSize, true);
	return newHeader;
}

template<class TKey>
cNode* cUBTreeLeafNodeHeader<TKey>::CopyNode(cNode* dest, cNode* source)
{
	cTreeNode<TKey> *d = (cTreeNode<TKey> *) dest;
	cTreeNode<TKey> *s = (cTreeNode<TKey> *) source;
	cUBTreeLeafNodeHeader<TKey> *sourceHeader = (cUBTreeLeafNodeHeader<TKey> *)source->GetHeader();
	
	d->SetLeaf(true);
	d->SetItemCount(s->GetItemCount());
	d->GetHeader()->SetInnerItemCount(s->GetItemCount());
	d->SetIndex(s->GetIndex());
	d->Copy(s);
	
	for (unsigned int i = 0; i < sourceHeader->GetNodeExtraLinkCount(); i++)
	{
		d->SetExtraLink(i, s->GetExtraLink(i));
	}
	
	return (cNode *) d;
}
}}}
#endif