/*
*
* cBpTreeNodeHeader.h - hlavicka uzlu
* Radim Bača, David Bednář
* Jan 2011
*
*/


#ifndef __cBpTreeNodeHeader_h__
#define __cBpTreeNodeHeader_h__

#include "dstruct/paged/core/cTreeNode.h"
#include "dstruct/paged/core/cTreeNodeHeader.h"

using namespace dstruct::paged::core;

namespace dstruct {
	namespace paged {
		namespace bptree {

template<class TKey>
class cBpTreeNodeHeader: public cTreeNodeHeader
{
public:
	cBpTreeNodeHeader() {}
	cBpTreeNodeHeader(bool leafNode, unsigned int keyInMemSize, unsigned int dataInMemSize = 0, bool varKey = false, bool varData = false, unsigned int dsMode = cDStructConst::DSMODE_DEFAULT);
	cBpTreeNodeHeader(const cBpTreeNodeHeader &header);
	~cBpTreeNodeHeader();

	virtual inline void WriteNode(cNode* node, cStream* stream);
	virtual inline void ReadNode(cNode* node, cStream* stream);
	virtual inline cNode* CopyNode(cNode* dest, cNode* source);
	virtual inline cNodeHeader* CreateCopy(unsigned int inMemSize);

	//virtual inline void FormatNode(cMemoryBlock* block);
};

/**
* Construktor
* \param keyInMemSize Size of an item.
* \param dataInMemSize Size of an data.
*/
template<class TKey>
cBpTreeNodeHeader<TKey>::cBpTreeNodeHeader(bool leafNode, unsigned int keyInMemSize, unsigned int dataInMemSize, bool varKey, bool varData, unsigned int dsMode) 
	: cTreeNodeHeader(leafNode, keyInMemSize, dataInMemSize, varKey, varData, dsMode)
{
}

template<class TKey>
cBpTreeNodeHeader<TKey>::cBpTreeNodeHeader(const cBpTreeNodeHeader &header) 
	: cTreeNodeHeader(header)
{	
}

template<class TKey>
cBpTreeNodeHeader<TKey>::~cBpTreeNodeHeader()
{ 
}

/**
* Call the write method on the cTreeNode.
*/
template<class TKey>
void cBpTreeNodeHeader<TKey>::WriteNode(cNode* node, cStream* stream)
{
	((cTreeNode<TKey>*)node)->Write(stream);
}

/**
* Call the read method on the cTreeNode.
*/
template<class TKey>
void cBpTreeNodeHeader<TKey>::ReadNode(cNode* node, cStream* stream)
{
	((cTreeNode<TKey>*)node)->Read(stream);
}

/**
* Copy a node from source to dest. Used by the row-cache for the node copying.
* \param dest Destination node
* \param source Source node
*/
template<class TKey>
cNode* cBpTreeNodeHeader<TKey>::CopyNode(cNode* dest, cNode* source)
{
	cTreeNode<TKey> *d = (cTreeNode<TKey> *) dest;
	cTreeNode<TKey> *s = (cTreeNode<TKey> *) source;
	cBpTreeNodeHeader<TKey> *sourceHeader = (cBpTreeNodeHeader<TKey> *) source->GetHeader();
	
	d->SetItemCount(s->GetItemCount());
	d->GetHeader()->SetInnerItemCount(s->GetItemCount());
	d->SetIndex(s->GetIndex());
	d->Copy(s);

	return (cNode *) d;
}

/**
* Create copy of this header for nodes with a different memory size.
* Used by the row-cache, since it needs node header for each cache row.
* \param inMemsize Size of the node in the main memory
*/
template<class TKey>
cNodeHeader* cBpTreeNodeHeader<TKey>::CreateCopy(unsigned int inMemSize)
{
	// I don't know if it is correct to set all nodes as leaf nodes
	cBpTreeNodeHeader<TKey>* newHeader = new cBpTreeNodeHeader<TKey>(true, mKeySize, mDataSize);

	this->SetCopy(newHeader);
	newHeader->VariableLenKeyEnabled(mVariableLenKey);
	newHeader->VariableLenDataEnabled(mVariableLenData);
	newHeader->ComputeNodeCapacity(inMemSize, mIsLeaf);

	return newHeader;
}

}}}
#endif
