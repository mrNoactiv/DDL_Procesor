/**
*	\file cTreeBulkLoad.h
*	\author Ondrej Prda
*	\date jun 2015
*	\brief Bulk load of a tree structure from sorted array from cBulkLoading.h.
*/

#ifndef __cBulkLoad_h__
#define __cBulkLoad_h__

#include "cItemStream.h"
#include "cTreeHeader.h"
#include "cTreeNode.h"
#include "cBulkLoadArray.h"
#include "common/data/cTuplesGenerator.h"
#include "common/datatype/tuple/cTuple.h"

/**
* Bulk load of a tree structure from sorted array from cBulkLoading.h.
*
* Template parameters:
*	- Tree - Data structure class type,
*	- TNode - class of the inner item.
*	- TLeafNode - class of the leaf item.
*	- TKey - class for tuplesGenerator
*	- DataType - also used as domain for tuplesGenerators
*	\author Ondrej Prda
*	\date jun 2015
**/

namespace common {
namespace data {

using namespace common::datatype::tuple;



template<class TTree, class TNode, class TLeafNode, class TKey, class THeader, class DataType>
class cBulkLoad
{
private:
	typedef cMBRectangle<TKey> TMbr;

	uint mSortType;

	TTree* mTree;
	THeader* mTreeHeader;

	uint mAvgLeafItemsCount;
	uint mAvgInnerItemsCount;

	tItemOrder* mLeafItemOrders;
	tItemOrder* mInnerItemOrders;

	uint mInnerNodesCount;
	uint mInnerNodeShift;
	uint mCapacity;
	uint mKeySize;

	char* mTmpMbr;
	bool mLeaf;

	cSpaceDescriptor* mSD;
	cBulkLoadArray<TKey>* mNodes;

public:
	cBulkLoad(THeader* pTreeHeader, TTree *pTree, float pMinUtilization, float pMaxUtilization, uint pTuplesCount, uint pSortType, uint pDataSize, cSpaceDescriptor* pSD);
	~cBulkLoad();

	inline void Add(const TKey &key, char* data);
	inline void Sort();
	void CreateBpTree();
	void CreateRTree();

private:
	uint GetInnerNodeCount();
	void CreateLeafMbr(TLeafNode* pLeafNode);
	void CreateInnerMbr(TNode* pNode);

	void CreateBpTreeInnerNodes(uint pItemCount);
	void CreateRTreeInnerNodes(uint pItemCount);
};

template<class TTree, class TNode, class TLeafNode, class TKey, class THeader, class DataType>
cBulkLoad<TTree, TNode, TLeafNode, TKey, THeader, DataType>::cBulkLoad(THeader* pTreeHeader, TTree *pTree, float pMinUtilization, float pMaxUtilization, uint pTuplesCount, uint pSortType, uint pDataSize, cSpaceDescriptor* pSD)
{
	mTree = pTree;
	mTreeHeader = pTreeHeader;
	mCapacity = pTuplesCount;
	mSortType = pSortType;
	mSD = pSD;
	mInnerNodeShift = 0;
	uint value = sizeof(tNodeIndex);

	mAvgLeafItemsCount = ((mTreeHeader->GetLeafNodeItemCapacity() * pMinUtilization) + (mTreeHeader->GetLeafNodeItemCapacity() * pMaxUtilization)) / 2;
	mAvgInnerItemsCount = ((mTreeHeader->GetNodeItemCapacity() * pMinUtilization) + (mTreeHeader->GetNodeItemCapacity() * pMaxUtilization)) / 2;

	mLeafItemOrders = new tItemOrder[mAvgLeafItemsCount];
	for (uint i = 0; i < mAvgLeafItemsCount; i++)
	{
		mLeafItemOrders[i] = i * (pSD->GetSize() + pDataSize);
	}

	mKeySize = (mTreeHeader->GetDStructCode() == cDStructConst::BTREE) ? pSD->GetSize() : (2 * pSD->GetSize());
	mInnerItemOrders = new tItemOrder[mAvgInnerItemsCount];
	for (uint i = 0; i < mAvgInnerItemsCount; i++)
	{
		mInnerItemOrders[i] = i * (mKeySize + sizeof(tNodeIndex));
	}

	mNodes = new cBulkLoadArray<TKey>(mTreeHeader->GetName(), mCapacity, GetInnerNodeCount() + ((mCapacity / mAvgLeafItemsCount) + 1), pSortType, pSD, pDataSize, mTreeHeader->GetDStructCode());
	mTmpMbr = new char[mKeySize];
}

template<class TTree, class TNode, class TLeafNode, class TKey, class THeader, class DataType>
cBulkLoad<TTree, TNode, TLeafNode, TKey, THeader, DataType>::~cBulkLoad()
{
	mNodes->~cBulkLoadArray();
	if (mLeafItemOrders != NULL)
	{
		delete[] mLeafItemOrders;
		mLeafItemOrders = NULL;
	}
	if (mInnerItemOrders != NULL)
	{
		delete[] mInnerItemOrders;
		mInnerItemOrders = NULL;
	}
	if (mTmpMbr != NULL)
	{
		delete mTmpMbr;
		mTmpMbr = NULL;
	}
}


template<class TTree, class TNode, class TLeafNode, class TKey, class THeader, class DataType>
void cBulkLoad<TTree, TNode, TLeafNode, TKey, THeader, DataType>::Add(const TKey &key, char* data)
{
	mNodes->AddLeafItem(key.GetData(), data);
}

template<class TTree, class TNode, class TLeafNode, class TKey, class THeader, class DataType>
void cBulkLoad<TTree, TNode, TLeafNode, TKey, THeader, DataType>::Sort()
{
	mNodes->Sort();
}

template<class TTree, class TNode, class TLeafNode, class TKey, class THeader, class DataType>
uint cBulkLoad<TTree, TNode, TLeafNode, TKey, THeader, DataType>::GetInnerNodeCount()
{
	uint leafNodesCount = 0;
	uint levelInnerNodeCount = 0;
	mInnerNodesCount = 0;
	uint changeLink = 0;

	if (mCapacity > mAvgLeafItemsCount)
	{
		leafNodesCount = mCapacity / mAvgLeafItemsCount;
		if (mCapacity % mAvgLeafItemsCount != 0) // There is a remain which should be inserted into last node
		{
			leafNodesCount++;
		}

		if (leafNodesCount > mAvgInnerItemsCount) // If there is more leaf nodes that could be inserted into one inner node
		{
			do
			{
				levelInnerNodeCount = leafNodesCount / mAvgInnerItemsCount;
				if (leafNodesCount % mAvgInnerItemsCount != 0)
				{
					levelInnerNodeCount++;
				}
				mInnerNodesCount += levelInnerNodeCount;
				leafNodesCount = levelInnerNodeCount;
			} while (levelInnerNodeCount > 1);	// Check for root
			return mInnerNodesCount;
		}
		else
		{
			return mInnerNodesCount = 1;
		}
	}
	else
	{
		return mInnerNodesCount = 0;
	}
}

template<class TTree, class TNode, class TLeafNode, class TKey, class THeader, class DataType>
void cBulkLoad<TTree, TNode, TLeafNode, TKey, THeader, DataType>::CreateBpTree()
{
	mLeaf = true;
	TLeafNode *leafNode, *previousLeafNode;
	tNodeIndex leafNodeIndex;
	uint leafNodesCount = (mCapacity / mAvgLeafItemsCount) + 1;
	uint lastNodeItems = mCapacity % mAvgLeafItemsCount;

	if (lastNodeItems == 0){ leafNodesCount--; }

	printf("LeafNodes creation starts \n");

	for (uint i = 0; i < leafNodesCount; i++)
	{
		leafNode = mTree->ReadNewLeafNode();

		if (i == 0) // the first leaf node
		{
			leafNode->SetExtraLink(0, TLeafNode::EMPTY_LINK);
			leafNode->SetExtraLink(1, TLeafNode::EMPTY_LINK);
		}
		else
		{
			previousLeafNode = mTree->ReadLeafNodeW(leafNodeIndex);
			previousLeafNode->SetExtraLink(1, TNode::GetLeafNodeIndex(leafNode->GetIndex()));
			mTree->UnlockLeafNode(previousLeafNode);

			leafNode->SetExtraLink(0, leafNodeIndex);
			leafNode->SetExtraLink(1, TLeafNode::EMPTY_LINK);
		}

		uint itemsCount = (i < leafNodesCount - 1) ? mAvgLeafItemsCount : lastNodeItems;
		if (i == (leafNodesCount - 1) && lastNodeItems == 0){ itemsCount = mAvgLeafItemsCount; }

		leafNode->CopyItems(mNodes->GetLeafNodeItems(i, mAvgLeafItemsCount), itemsCount);
		leafNode->CopyItemOrders(mLeafItemOrders, itemsCount);
		leafNode->SetItemCount(itemsCount);

		mNodes->AddInnerItem(mNodes->GetLastLeafNodeItem(i, mAvgLeafItemsCount, mAvgLeafItemsCount - itemsCount), TNode::GetLeafNodeIndex(leafNode->GetIndex()));

		leafNodeIndex = TNode::GetLeafNodeIndex(leafNode->GetIndex());
		mTree->UnlockLeafNode(leafNode);
		leafNode = NULL;
	}
	printf("InnerNodes creation is to be started \n");
	CreateBpTreeInnerNodes(leafNodesCount);
}

template<class TTree, class TNode, class TLeafNode, class TKey, class THeader, class DataType>
void cBulkLoad<TTree, TNode, TLeafNode, TKey, THeader, DataType>::CreateBpTreeInnerNodes(uint pItemCount)
{
	TNode *innerNode;
	tNodeIndex innerNodeIndex;
	uint innerNodesCount = (pItemCount / mAvgInnerItemsCount) + 1;
	uint lastNodeItems = pItemCount % mAvgInnerItemsCount;

	if (lastNodeItems == 0){ innerNodesCount--; }

	if (pItemCount < mAvgInnerItemsCount)
	{
		innerNode = mTree->ReadNewNode();

		innerNode->CopyItems(mNodes->GetInnerNodeItems(mInnerNodeShift, mAvgInnerItemsCount, mAvgInnerItemsCount - pItemCount, mLeaf, 1), pItemCount);
		innerNode->CopyItemOrders(mInnerItemOrders, pItemCount);
		innerNode->SetItemCount(pItemCount);

		mTreeHeader->SetRootIndex(innerNode->GetIndex());
		mTree->UnlockNode(innerNode);
	}
	else
	{
		for (uint i = 0; i < innerNodesCount; i++)
		{
			innerNode = mTree->ReadNewNode();

			uint itemsCount = (i < innerNodesCount - 1) ? mAvgInnerItemsCount : lastNodeItems;
			if (i == (innerNodesCount - 1) && lastNodeItems == 0){ itemsCount = mAvgInnerItemsCount; }

			innerNode->CopyItems(mNodes->GetInnerNodeItems(i + mInnerNodeShift, mAvgInnerItemsCount, mAvgInnerItemsCount - itemsCount, mLeaf, 1), itemsCount);
			innerNode->CopyItemOrders(mInnerItemOrders, itemsCount);
			innerNode->SetItemCount(itemsCount);

			mNodes->AddInnerItem(mNodes->GetInnerLastNodeItem(i + mInnerNodeShift, mAvgInnerItemsCount, mAvgInnerItemsCount - itemsCount) + sizeof(tNodeIndex), TNode::GetNodeIndex(innerNode->GetIndex()));

			innerNodeIndex = TNode::GetNodeIndex(innerNode->GetIndex());
			mTree->UnlockNode(innerNode);
		}
		//last node correction for another innerNode get
		lastNodeItems == 0 ? mNodes->AddLeftOver(mAvgInnerItemsCount - mAvgInnerItemsCount) : mNodes->AddLeftOver(mAvgInnerItemsCount - lastNodeItems);

		mLeaf = false;
		mInnerNodeShift += innerNodesCount;
		CreateBpTreeInnerNodes(innerNodesCount);
	}

}

template<class TTree, class TNode, class TLeafNode, class TKey, class THeader, class DataType>
void cBulkLoad<TTree, TNode, TLeafNode, TKey, THeader, DataType>::CreateRTree()
{
	mLeaf = true;
	TLeafNode *leafNode;
	tNodeIndex leafNodeIndex;
	uint leafNodesCount = (mCapacity / mAvgLeafItemsCount) + 1;
	uint lastNodeItems = mCapacity % mAvgLeafItemsCount;

	printf("LeafNodes creation starts \n");

	if (lastNodeItems == 0){ leafNodesCount--; }

	for (uint i = 0; i < leafNodesCount; i++)
	{
		leafNode = mTree->ReadNewLeafNode();
		mTmpMbr = new char[mKeySize];
		uint itemsCount = (i < leafNodesCount - 1) ? mAvgLeafItemsCount : lastNodeItems;
		if (i == (leafNodesCount - 1) && lastNodeItems == 0){ itemsCount = mAvgLeafItemsCount; }

		leafNode->CopyItems(mNodes->GetLeafNodeItems(i, mAvgLeafItemsCount), itemsCount);
		leafNode->CopyItemOrders(mLeafItemOrders, itemsCount);
		leafNode->SetItemCount(itemsCount);

		CreateLeafMbr(leafNode); // MBR je ulozene v premennej mTmpMbr

		//change 1st parameter
		mNodes->AddInnerItem(mTmpMbr, TNode::GetLeafNodeIndex(leafNode->GetIndex()));

		leafNodeIndex = TNode::GetLeafNodeIndex(leafNode->GetIndex());
		mTree->UnlockLeafNode(leafNode);
		leafNode = NULL;
		mTmpMbr = NULL;
	}

	printf("InnerNodes creation is to be started \n");
	CreateRTreeInnerNodes(leafNodesCount);
}

template<class TTree, class TNode, class TLeafNode, class TKey, class THeader, class DataType>
void cBulkLoad<TTree, TNode, TLeafNode, TKey, THeader, DataType>::CreateRTreeInnerNodes(uint pItemCount)
{
	TNode* innerNode;
	tNodeIndex innerNodeIndex;
	uint innerNodesCount = (pItemCount / mAvgInnerItemsCount) + 1;
	uint lastNodeItems = pItemCount % mAvgInnerItemsCount;

	mTreeHeader->IncrementHeight();

	if (lastNodeItems == 0){ innerNodesCount--; }

	if (pItemCount < mAvgInnerItemsCount)
	{
		innerNode = mTree->ReadNewNode();

		innerNode->CopyItems(mNodes->GetInnerNodeItems(mInnerNodeShift, mAvgInnerItemsCount, mAvgInnerItemsCount - pItemCount, mLeaf, 2), pItemCount);
		innerNode->CopyItemOrders(mInnerItemOrders, pItemCount);
		innerNode->SetItemCount(pItemCount);

		mTreeHeader->SetRootIndex(innerNode->GetIndex());
		mTree->UnlockNode(innerNode);

	}
	else
	{
		for (uint i = 0; i < innerNodesCount; i++)
		{
			mTmpMbr = new char[mKeySize];
			innerNode = mTree->ReadNewNode();

			uint itemsCount = (i < innerNodesCount - 1) ? mAvgInnerItemsCount : lastNodeItems;
			if (i == (innerNodesCount - 1) && lastNodeItems == 0){ itemsCount = mAvgInnerItemsCount; }

			innerNode->CopyItems(mNodes->GetInnerNodeItems(i + mInnerNodeShift, mAvgInnerItemsCount, mAvgInnerItemsCount - itemsCount, mLeaf, 2), itemsCount);
			innerNode->CopyItemOrders(mInnerItemOrders, itemsCount);
			innerNode->SetItemCount(itemsCount);

			CreateInnerMbr(innerNode);
			mNodes->AddInnerItem(mTmpMbr, TNode::GetNodeIndex(innerNode->GetIndex()));

			innerNodeIndex = TNode::GetNodeIndex(innerNode->GetIndex());
			mTree->UnlockNode(innerNode);
			mTmpMbr = NULL;
		}

		//last node correction for another innerNode get
		lastNodeItems == 0 ? mNodes->AddLeftOver(mAvgInnerItemsCount - mAvgInnerItemsCount) : mNodes->AddLeftOver(mAvgInnerItemsCount - lastNodeItems);

		mLeaf = false;
		mInnerNodeShift += innerNodesCount;
		CreateRTreeInnerNodes(innerNodesCount);
	}
}


/// Create the Mbr of leaf node.
template<class TTree, class TNode, class TLeafNode, class TKey, class THeader, class DataType>
void cBulkLoad<TTree, TNode, TLeafNode, TKey, THeader, DataType>::CreateLeafMbr(TLeafNode* pLeafNode)
{
	TKey::Copy(TMbr::GetLoTuple(mTmpMbr), pLeafNode->GetCKey(0), mSD);
	TKey::Copy(TMbr::GetHiTuple(mTmpMbr, mSD), pLeafNode->GetCKey(0), mSD);

	for (uint i = 1; i < pLeafNode->GetItemCount(); i++)
	{
		TMbr::ModifyMbr(TMbr::GetLoTuple(mTmpMbr), TMbr::GetHiTuple(mTmpMbr, mSD), pLeafNode->GetCKey(i), mSD);
	}

}

/// Create the Mbr of inner node.
template<class TTree, class TNode, class TLeafNode, class TKey, class THeader, class DataType>
void cBulkLoad<TTree, TNode, TLeafNode, TKey, THeader, DataType>::CreateInnerMbr(TNode* pNode)
{
	TKey::Copy(TMbr::GetLoTuple(mTmpMbr), pNode->GetCKey(0), mSD);
	TKey::Copy(TMbr::GetHiTuple(mTmpMbr, mSD), pNode->GetCKey(0) + mSD->GetSize(), mSD);

	for (uint i = 1; i < pNode->GetItemCount(); i++)
	{
		TMbr::ModifyMbrByMbr(mTmpMbr, pNode->GetCKey(i), mSD);
	}
}

}
}
#endif