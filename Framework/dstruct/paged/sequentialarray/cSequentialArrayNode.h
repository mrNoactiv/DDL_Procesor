#ifndef __cSequentialArrayNode_h__
#define __cSequentialArrayNode_h__

#include "dstruct/paged/sequentialarray/cSequentialArrayHeader.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayNodeHeader.h"
#include "dstruct/paged/core/cNode.h"

using namespace dstruct::paged::core;

namespace dstruct {
	namespace paged {
		namespace sqarray {
/**
* Node with the set of items. Node is implemented for the cSequentialArray data structure.
* Nodes can have variable length.
*	- TItem - type of the item inherited from the cBasicType. Type is expected to have a encode and decode methods.
*
*	\author Radim Baca
*	\version 0.1
*	\date jul 2011
**/
template<class TItem> 
class cSequentialArrayNode: public cNode
{
private:
	typedef cNode parent;
	
protected:
	inline unsigned int GetMaxDataSize() const;
	inline const cSequentialArrayNodeHeader<TItem>* GetSQHeader() const { return (const cSequentialArrayNodeHeader<TItem>*)mHeader; }

public:
	cSequentialArrayNode(unsigned int size, unsigned int order);
	cSequentialArrayNode(cSequentialArrayHeader<TItem>* header);
	~cSequentialArrayNode();
	
	inline void Init(unsigned int size, unsigned int orders);
	inline void Delete();
	inline void Clear();

	inline void SetNextNodeIndex(unsigned int nextNodeIndex);
	inline void IncItemCount()									{ SetItemCount(mItemCount+1); }

	inline const char* GetCItem(unsigned int position) const;
	inline char* GetItem(unsigned int position);
	inline void AddItem(const TItem &item);
	inline void AddMaxItem(const TItem &item);
	inline void RewriteItem(unsigned int position, const char* item);
	//inline bool TestAddItem(const TItem &item, char* storedBuffer, unsigned int maxSize);
	inline void SetUsedSpace(unsigned int usedSpace)	{ mRealSize = usedSpace; }

	//void Resize(const cSequentialArrayHeader<TItem>* cacheHeader);

	bool Write(cStream* mStream) const;
	bool Read(cStream* mStream);

	inline unsigned int GetNextNodeIndex() const;
	static unsigned int GetNodeExtraSize()				{ return sizeof(unsigned int) + sizeof(unsigned int) + sizeof(unsigned int); }
	unsigned int GetUsedSpace()							{ return mRealSize; }
	unsigned int GetNodeFreeSpace()						{ return GetSQHeader()->GetDataAreaSize() - mRealSize; }
	static int GetNodeFreeSpace(unsigned int blockSize) { return blockSize - GetNodeExtraSize(); }
	inline float GetFreeSpaceRatio();

	//cSequentialArrayNode<TItem>& operator = (const cSequentialArrayNode<TItem>& blk);

	void Print(char* str, unsigned int count) const;
};

template<class TItem> 
cSequentialArrayNode<TItem>::cSequentialArrayNode(unsigned int size, unsigned int order): parent()
{
	Init(size, order);
}

template<class TItem> 
cSequentialArrayNode<TItem>::cSequentialArrayNode(cSequentialArrayHeader<TItem> * header): 
	parent() // problem in gcc: , parent::mHeader(header) //, parent::mIndex(EMPTY_INDEX), parent::mData(NULL)
{
	parent::mHeader = header;
	parent::mIndex = EMPTY_INDEX; 
	parent::mData = NULL;
}

template<class TItem> 
cSequentialArrayNode<TItem>::~cSequentialArrayNode()
{
	Delete();
}

template<class TItem> 
void cSequentialArrayNode<TItem>::Init(unsigned int size, unsigned int order)
{
	cNode::Init(size, order);
}

template<class TItem> 
void cSequentialArrayNode<TItem>::Delete()
{
	cNode::Delete();
}

template<class TItem> 
void cSequentialArrayNode<TItem>::Clear()
{
	mRealSize = 0;
	SetNextNodeIndex(EMPTY_INDEX);
}

template<class TItem> 
inline bool cSequentialArrayNode<TItem>::Read(cStream* stream)
{
	bool ok;
	unsigned int nextNodeIndex;
	
	ok = stream->Read((char*)&mRealSize, sizeof(mRealSize));
	ok &= stream->Read((char*)&nextNodeIndex, sizeof(nextNodeIndex));
	ok &= stream->Read((char*)&mItemCount, sizeof(mItemCount));
	ok &= stream->Read(mData, mRealSize);

	SetNextNodeIndex(nextNodeIndex);

	assert(mRealSize <= GetMaxDataSize());

	return ok;
}

template<class TItem> 
inline bool cSequentialArrayNode<TItem>::Write(cStream* stream) const
{
	bool ok;
	unsigned int nextNodeIndex = GetNextNodeIndex();

	assert(mRealSize <= GetMaxDataSize());

	ok = stream->Write((char*)&mRealSize, sizeof(mRealSize));
	ok &= stream->Write((char*)&nextNodeIndex, sizeof(nextNodeIndex));
	ok &= stream->Write((char*)&mItemCount, sizeof(mItemCount));
	ok &= stream->Write(mData, mRealSize);

	return ok;
}

template<class TItem> 
inline void cSequentialArrayNode<TItem>::SetNextNodeIndex(unsigned int nextNodeIndex)	
{ 
	*(unsigned int*)(mData + GetSQHeader()->GetDataAreaSize()) = nextNodeIndex; 
}

template<class TItem> 
inline unsigned int cSequentialArrayNode<TItem>::GetNextNodeIndex() const		
{ 
	return *(unsigned int*)(mData + GetSQHeader()->GetDataAreaSize()); 
}

template<class TItem> 
inline float cSequentialArrayNode<TItem>::GetFreeSpaceRatio()		
{ 
	return 0;
//	return (cNode::mHeader->GetBlockSize() - mRealSize)/(float)cNode::mHeader->GetBlockSize(); 
}

/**
* \param position The position of the item in the byte array of the node.
* \return Data of the item
*/
template<class TItem> 
const char* cSequentialArrayNode<TItem>::GetCItem(unsigned int position) const
{
	return mData + position;
}

/**
* \param position The position of the item in the byte array of the node.
* \return Data of the item
*/
template<class TItem> 
char* cSequentialArrayNode<TItem>::GetItem(unsigned int position)
{
	return mData + position;
}

/**
* Add item into the node. Encode item to the end of the node.
* \param item Item which should be added into the node.
*/
template<class TItem> 
void cSequentialArrayNode<TItem>::AddItem(const TItem &item)
{
	mRealSize += item.CopyTo(mData + mRealSize, mHeader->GetKeyDescriptor());
	assert(mRealSize <= GetSQHeader()->GetDataAreaSize());
}


template<class TItem>
void cSequentialArrayNode<TItem>::AddMaxItem(const TItem &item)
{
	item.CopyTo(mData + mRealSize, mHeader->GetKeyDescriptor());
	mRealSize += item.GetMaxSize(mHeader->GetKeyDescriptor());
	assert(mRealSize <= GetSQHeader()->GetDataAreaSize());
}

/**
* This method is potentialy dangerous, since it rewrite memory from the specified position.
* Caller should make sure that we are not rewriting the following item.
* \param position in the node block where we encode item.
* \param item Item which should be encoded into the node.
*/
template<class TItem> 
void cSequentialArrayNode<TItem>::RewriteItem(unsigned int position, const char* item)
{
	//item.CopyTo(&mData[position], mHeader->GetKeyDescriptor());
	TItem::Copy(&mData[position], item, mHeader->GetKeyDescriptor());
}

/**
* Size in bytes available in node for data
*/
template<class TItem> 
unsigned int cSequentialArrayNode<TItem>::GetMaxDataSize() const
{ 
	return mHeader->GetNodeSerialSize() - cSequentialArrayNode<TItem>::GetNodeExtraSize(); 
}

/// Print the block items
/// \param count Number of items which should be printed
template<class TItem> 
void cSequentialArrayNode<TItem>::Print(char* str, unsigned int count) const
{
	printf("Items count: %d, Length: %d\n", mItemCount, mRealSize);
}

}}}
#endif
