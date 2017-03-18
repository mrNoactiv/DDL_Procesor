/**
 *	\file cPagedHashTableNode.h
 *	\author Michal Kratky, Radim Baca
 *	\version 0.1
 *	\date jun 2006
 *	\brief Tuple for a tree data structure
 */

#ifndef __cPagedHashTableNode_h__
#define __cPagedHashTableNode_h__

#include "dstruct/paged/core/cTreeNode.h"
#include "dstruct/paged/hashtable/constants.h"

namespace dstruct {
  namespace paged {
	namespace hashtable {

using namespace dstruct::paged::core;

/**
* Represents n-dimensional tuple. Every tuple is bounded with its space descriptor
* which holds meta information about the tuple (dimension, type of each item etc.).
*
* \author Michal Kratky, Radim Baca
* \version 0.1
* \date jun 2006
**/
template<class TItem>
class cPagedHashTableNode: public cTreeNode<TItem>
{
private:
	tNodeIndex mNextNode;

public:
	static const tNodeIndex NODE_NOT_EXIST = (unsigned int)-1;

	cPagedHashTableNode();
	~cPagedHashTableNode();

	int Insert(const TItem& key, char* data);
	bool Find(const TItem& key, char* data);

	void SetNextNode(tNodeIndex nodeIndex);
	inline tNodeIndex GetNextNode() const;

	// for extendible and linear hashing
	inline char* GetKeyPtr(unsigned int lOrder) const;
	inline unsigned int GetItemSize(unsigned int lOrder) const;
	unsigned int Clear();
	unsigned int CopyNodeData(char* &items, tItemOrder* &orders);
	int ExtractLastItem(char* &key);
	int ShiftItemChain(char* &key);
	inline int CompareWithLastKey(const char* key) const;
};

/**
* Modify MBR according to the tuple.
* \param mbr1 Lower tuple of the MBR.
* \param mbr2 Higher tuple of the MBR.
* \return
*		- true if the MBR was modified,
*		- false otherwise.
*/
// bool cTreeTuple::ModifyMbr(cTreeTuple &mbrl, cTreeTuple &mbrh) const
template<class TItem> cPagedHashTableNode<TItem>::cPagedHashTableNode()
{
	//mNextNode = NULL;		//PT//
}

template<class TItem> cPagedHashTableNode<TItem>::~cPagedHashTableNode()
{
}

template<class TItem>
inline void cPagedHashTableNode<TItem>::SetNextNode(tNodeIndex nodeIndex)
{
	SetExtraLink(0, nodeIndex);		//PT//
	//mNextNode = nodeIndex;
}

template<class TItem>
inline tNodeIndex cPagedHashTableNode<TItem>::GetNextNode() const
{
	return GetExtraLink(0);		//PT//
	//return mNextNode;
}

/// Insert term into the node.
template<class TItem> 
int cPagedHashTableNode<TItem>::Insert(const TItem& key, char* data)
{
	return cTreeNode<TItem>::InsertLeafItem(key.GetData(), data, GetNodeHeader()->DuplicatesAllowed(), NULL);  // duplicate keys are allowed
}

/*
 * Find item in the node.
 *
 */
template<class TItem> 
bool cPagedHashTableNode<TItem>::Find(const TItem& key, char* data)
{
	bool ret = false;
	int order = cTreeNode<TItem>::FindOrder(key.GetData(), FIND_E);

	if (order != FIND_NOTEXIST)
	{
		cNodeItem::Copy(data, GetData(order), GetNodeHeader()->GetDataSize());
		ret = true;
	}

	return ret;
}

/*
unsigned int cPagedHashTableNode::HashValue(const wchar_t* term)
{
	unsigned int hashValue = 0;
	for (unsigned int i = 0 ; i < wcslen(term); i++)
	{
		unsigned int tmp = term[i] << (i*4);
		hashValue += tmp;
	}
	return hashValue;
}*/

///////////////////////////////////////////////////////////////////////////////////////////////PT//
template<class TItem>
inline char* cPagedHashTableNode<TItem>::GetKeyPtr(unsigned int lOrder) const
{
	return cTreeNode::GetKeyPtr(lOrder);
}

template<class TItem>
inline unsigned int cPagedHashTableNode<TItem>::GetItemSize(unsigned int lOrder) const
{
	return cTreeNode::GetItemSize(lOrder);
}

template<class TItem>
unsigned int cPagedHashTableNode<TItem>::Clear()
{
	uint count = mItemCount;
	mFreeSize += count * GetItemSize(0);
	mItemCount = 0;
	((cPagedHashTableNodeHeader<TItem>*)mHeader)->DecreaseItemCount(count);
	SetNextNode(C_EMPTY_LINK);

	return count;
}

template<class TItem>
unsigned int cPagedHashTableNode<TItem>::CopyNodeData(char* &items, tItemOrder* &orders)
{
	unsigned int count = mItemCount;

	unsigned int item_size = GetItemSize(0);

	items = new char[count * item_size];
	memcpy(items, GetItems(), count * item_size);

	orders = new tItemOrder[count];
	memcpy(orders, GetPItemPOrder(0), count * sizeof(tItemOrder));

	return count;
}

/// Extract last item to allow insert into data chain
template<class TItem>
int cPagedHashTableNode<TItem>::ExtractLastItem(char*& key)
{
	if (mItemCount == 0)
	{
		key = NULL;
		return C_FIND_NOTEXIST;
	}

	assert(mHeader->GetDStructMode() == cDStructConst::DSMODE_DEFAULT);

	unsigned int item = mItemCount - 1;
	unsigned int item_size = GetItemSize(item);

	// copy data
	key = new char[item_size];
	memcpy(key, GetKeyPtr(item), item_size);

	if (GetItemPOrder(item) != (item * item_size))	// if not stored at last physical position
	{
		unsigned int last = 0;
		tItemOrder lastPO = GetItemPOrder(last);

		for (unsigned int i = 1; i < mItemCount; i++)
		{
			if (GetItemPOrder(i) > lastPO)
			{
				lastPO = GetItemPOrder(i);
				last = i;
			}
		}
		memcpy(GetKeyPtr(item), GetItemPtr(last), item_size);	// move last physical data to extracted position
		SetItemPOrder(last, GetItemPOrder(item));				// fix the PO
	}

	mFreeSize += item_size;
	((cPagedHashTableNodeHeader<TItem>*)mHeader)->DecreaseItemCount(1);
	return --mItemCount;
}

/// insert given item to the front and extract last item
template<class TItem>
int cPagedHashTableNode<TItem>::ShiftItemChain(char*& key)
{
	if (mItemCount < mHeader->GetNodeCapacity())
	{
		TItem key_item = TItem(mHeader->GetKeyDescriptor());
		key_item.SetData(key);
		Insert(key_item, key + GetKeySize(key));
		key_item.SetData(NULL);
		delete key;
		key = NULL;
		return C_FIND_NOTEXIST;
	}

	assert(mHeader->GetDStructMode() == cDStructConst::DSMODE_DEFAULT);

	unsigned int item_size = GetItemSize(0);
	char *new_key = new char[item_size];

	unsigned int last = mItemCount - 1;
	tItemOrder lastPO = GetItemPOrder(last);

	// copy data
	memcpy(new_key, GetKeyPtr(last), item_size);
	memcpy(GetKeyPtr(last), key, item_size);
	delete key;
	key = new_key;
	new_key = NULL;

	mItemCount--;
	InsertItemPOrder(0, lastPO);
	mItemCount++;

	return C_INSERT_YES;
}

template<class TItem>
int cPagedHashTableNode<TItem>::CompareWithLastKey(const char* key) const
{
	if (mItemCount < mHeader->GetNodeCapacity()) return C_COMPARE_SMALLER;
	return TItem::Compare(key, GetKeyPtr(mItemCount - 1), mHeader->GetKeyDescriptor());
}
///////////////////////////////////////////////////////////////////////////////////////////////PT//

}}}
#endif