/**
*	\file cPagedHashTableNodeHeader.h
*	\author Radim Bača, David Bednář, Michal Krátký
*	\version 0.1
*	\date jan 2011
*	\brief Header of fixed-capacity inner node
*/

#ifndef __cPagedHashTableNodeHeader_h__
#define __cPagedHashTableNodeHeader_h__

#include "dstruct/paged/core/cTreeNode.h"
#include "dstruct/paged/core/cTreeNodeHeader.h"
#include "dstruct/paged/hashtable/cPagedHashTableNode.h"

using namespace dstruct::paged::core;

namespace dstruct {
  namespace paged {
	namespace hashtable {
/**
*	Header of fixed-capacity inner node
*
*	\author Radim Bača, David Bednář, Michal Krátký
*	\version 0.1
*	\date jan 2011
**/
template<class TKey>
class cPagedHashTableNodeHeader: public cTreeNodeHeader
{
public:
	cPagedHashTableNodeHeader(unsigned int keyInMemSize, unsigned int dataInMemSize, bool varlenKey, bool varlenData);

	virtual inline void WriteNode(cNode* block, cStream* stream);
	virtual inline void ReadNode(cNode* block, cStream* stream);
	virtual inline cNode* CopyNode(cNode* dest, cNode* source);
	virtual cNodeHeader* CreateCopy(unsigned int blockSize);
	
	// for extendible and linear hashing
	unsigned int DecreaseItemCount(unsigned int count);
};

template<class TKey>
cPagedHashTableNodeHeader<TKey>::cPagedHashTableNodeHeader(unsigned int keyInMemSize, unsigned int dataInMemSize, bool varlenKey, bool varlenData): 
  cTreeNodeHeader(true /* leaf node */, keyInMemSize, dataInMemSize, varlenKey, varlenData)
{
}

template<class TKey>
void cPagedHashTableNodeHeader<TKey>::WriteNode(cNode* node, cStream* stream)
{
	((cPagedHashTableNode<TKey>*)node)->Write(stream);
}

template<class TKey>
void cPagedHashTableNodeHeader<TKey>::ReadNode(cNode* node, cStream* stream)
{
	((cPagedHashTableNode<TKey>*)node)->Read(stream);
}

// ?? do nadtridy ???
template<class TKey>
cNodeHeader* cPagedHashTableNodeHeader<TKey>::CreateCopy(unsigned int blockSize)
{
	cPagedHashTableNodeHeader<TKey>* newHeader = new cPagedHashTableNodeHeader<TKey>(mKeySize, mDataSize, 
		VariableLenKeyEnabled(), VariableLenKeyEnabled());
	this->SetCopy(newHeader);
	return newHeader;
}

// ?? do nadtridy ???
template<class TKey>
cNode* cPagedHashTableNodeHeader<TKey>::CopyNode(cNode* dest, cNode* source)
{
	cTreeNode<TKey> *d = (cTreeNode<TKey> *) dest;
	cTreeNode<TKey> *s = (cTreeNode<TKey> *) source;
	cPagedHashTableNodeHeader<TKey> *sourceHeader = (cPagedHashTableNodeHeader<TKey>*)source->GetHeader();
	
	d->SetItemCount(s->GetItemCount());
	d->GetHeader()->SetInnerItemCount(s->GetItemCount());
	d->SetIndex(s->GetIndex());
	d->Copy(s);

	return (cNode *)d;
}

// for extendible and linear hashing
template<class TKey>
unsigned int cPagedHashTableNodeHeader<TKey>::DecreaseItemCount(unsigned int count)
{
	if (mItemCount >= count)
		mItemCount -= count;

	return mItemCount;
}

}}}
#endif