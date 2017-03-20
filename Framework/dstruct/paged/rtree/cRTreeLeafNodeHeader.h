/*
*
* cRTreeLeafNodeHeader.h - hlavicka uzlu
* Radim Bača, David Bednář
* Jan 2011
*
*/

#ifndef __cRTreeLeafNodeHeader_h__
#define __cRTreeLeafNodeHeader_h__

namespace dstruct {
	namespace paged {
		namespace rtree {
  template<class TKey> class cRTreeLeafNodeHeader;
  // template<class TKey> class cCommonRTreeNodeHeader;
  template<class TKey> class cRTreeSignatureIndex;
}}}

// #include "dstruct/paged/rtree/cCommonRTreeNodeHeader.h"
#include "dstruct/paged/core/cTreeNodeHeader.h"
#include "dstruct/paged/rtree/cRTreeSignatureIndex.h"

using namespace dstruct::paged::core;

namespace dstruct {
	namespace paged {
		namespace rtree {

template<class TKey>
class cRTreeLeafNodeHeader : public cTreeNodeHeader// cCommonRTreeNodeHeader<TKey>
{
private:
	cRTreeSignatureIndex<TKey> *mSignatureIndex;

public:
	cRTreeLeafNodeHeader(bool leafNode, const unsigned int leafKeySize, const unsigned int leafDataSize, bool varlenKey, bool varlenData = false, unsigned int dsMode = cDStructConst::DSMODE_DEFAULT);
	cRTreeLeafNodeHeader(bool leafNode, const unsigned int leafKeySize, const unsigned int leafDataSize, bool varlenData = false, unsigned int dsMode = cDStructConst::DSMODE_DEFAULT); //bas064
	~cRTreeLeafNodeHeader();

	virtual inline void WriteNode(cNode* block, cStream* stream);
	virtual inline void ReadNode(cNode* block, cStream* stream);
	virtual cNodeHeader* CreateCopy(unsigned int inMemSize);
	// virtual inline void FormatNode(cMemoryBlock* block);		
	virtual inline cNode* CopyNode(cNode* dest, cNode* source);

	inline cRTreeSignatureIndex<TKey>* GetSignatureIndex();
	inline void SetSignatureIndex(cRTreeSignatureIndex<TKey> *signatureIndex);

	inline const cSpaceDescriptor* GetSpaceDescriptor() const;
};

template<class TKey>
cRTreeLeafNodeHeader<TKey>::cRTreeLeafNodeHeader(bool leafNode, const unsigned int leafKeySize, const unsigned int leafDataSize, bool varlenKey, bool varlenData, unsigned int dsMode):
cTreeNodeHeader(leafNode, leafKeySize, leafDataSize, varlenKey, varlenData, dsMode), mSignatureIndex(NULL)
{	
}

template<class TKey>
cRTreeLeafNodeHeader<TKey>::cRTreeLeafNodeHeader(bool leafNode, const unsigned int leafKeySize, const unsigned int leafDataSize, bool varlenData, unsigned int dsMode):
cTreeNodeHeader(leafNode, leafKeySize, leafDataSize, TKey::LengthType == cDataType::LENGTH_VARLEN, varlenData, dsMode), mSignatureIndex(NULL)
{	
}

template<class TKey>
cRTreeLeafNodeHeader<TKey>::~cRTreeLeafNodeHeader()
{ 
}

template<class TKey>
inline const cSpaceDescriptor* cRTreeLeafNodeHeader<TKey>::GetSpaceDescriptor() const
{
	return (cSpaceDescriptor*)GetKeyDescriptor();
}

template<class TKey>
void cRTreeLeafNodeHeader<TKey>::WriteNode(cNode* node, cStream* stream)
{
	((cRTreeLeafNode<TKey>*)node)->Write(stream);
}

template<class TKey>
void cRTreeLeafNodeHeader<TKey>::ReadNode(cNode* node, cStream* stream)
{
	((cRTreeLeafNode<TKey>*)node)->Read(stream);
}

template<class TKey>
cNodeHeader* cRTreeLeafNodeHeader<TKey>::CreateCopy(unsigned int inMemSize)
{
	cRTreeLeafNodeHeader<TKey>* newHeader = new cRTreeLeafNodeHeader<TKey>(true, mKeySize, mDataSize);

	this->SetCopy(newHeader);
	newHeader->ComputeNodeCapacity(inMemSize, true);
	return newHeader;
}

template<class TKey>
cNode* cRTreeLeafNodeHeader<TKey>::CopyNode(cNode* dest, cNode* source)
{
	cTreeNode<TKey> *d = (cTreeNode<TKey> *) dest;
	cTreeNode<TKey> *s = (cTreeNode<TKey> *) source;
	cRTreeLeafNodeHeader<TKey> *sourceHeader = (cRTreeLeafNodeHeader<TKey> *)source->GetHeader();
	
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

template<class TKey>
cRTreeSignatureIndex<TKey>* cRTreeLeafNodeHeader<TKey>::GetSignatureIndex()
{
	return mSignatureIndex;
}

template<class TKey>
void cRTreeLeafNodeHeader<TKey>::SetSignatureIndex(cRTreeSignatureIndex<TKey> *pSignatureIndex)
{
	mSignatureIndex = pSignatureIndex;
}


//template<class TKey>
//void cRTreeLeafNodeHeader<TKey>::FormatNode(cMemoryBlock* block)
//{
//	cRTreeLeafNode<TKey>* node = (cRTreeLeafNode<TKey>*)block->GetMemory(sizeof(cRTreeLeafNode<TKey>));
//	node->Format(this, block, true);
//}
}}}
#endif