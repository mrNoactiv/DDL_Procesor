/**
*	\file cCommonUBTree.h
*	\author Michal Kratky
*	\version 0.4
*	\date nov 2013
*	\version 0.3
*	\date jul 2011
*	\version 0.2
*	\date 2003
*	\brief It implements the paged UB-Tree
*/

#ifndef __cCommonUBTree_h__
#define __cCommonUBTree_h__

#include <float.h>
#include <mutex>

#include "dstruct/paged/core/cQuickDB.h"
#include "dstruct/paged/core/cPagedTree.h"
#include "dstruct/paged/ubtree/cUBTreeHeader.h"
#include "dstruct/paged/ubtree/cZRegion.h"
#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "dstruct/paged/ubtree/cInsertBuffers.h"

using namespace common::datatype::tuple;
using namespace dstruct::paged::ubtree;

/**
 *  Parameters of the template:
 *		- TMbr - Key of the inner node, it means MBRectangle
 *		- TKey - Key of the leaf node, e.g. TKey, cUniformTuple
 *		- TNode - Inner node
 *		- TLeafNode - Leaf node
 *
 *	\author 
 *	\version 0.1
 *	\date 
 **/
namespace dstruct {
	namespace paged {
		namespace ubtree {

template<class TKey, class TNode, class TLeafNode>
class cCommonUBTree: public cPagedTree<TKey, TNode, TLeafNode>
{
	typedef cPagedTree<TKey, TNode, TLeafNode> parent;

private:

public:
	cCommonUBTree();
	~cCommonUBTree();

	bool Open(cUBTreeHeader<TKey> *header, cQuickDB* quickDB, bool readOnly = true);
	bool Create(cUBTreeHeader<TKey> *header, cQuickDB* quickDB);
	bool Close();
	bool Clear();

	int Insert(const TKey &item, char* leafData);

	inline cUBTreeHeader<TKey>* GetUBTreeHeader();
	inline cUBTreeHeader<TKey>& GetRefUBTreeHeader();
	inline const cSpaceDescriptor* GetSpaceDescriptor() const;

	void PrintIndexSize(uint blockSize);

	void PrintInfo();
};

template<class TKey, class TNode, class TLeafNode>
cCommonUBTree<TKey,TNode,TLeafNode>::cCommonUBTree(): parent()
{
}

template<class TKey, class TNode, class TLeafNode>
cCommonUBTree<TKey,TNode,TLeafNode>::~cCommonUBTree()
{
}

template<class TKey, class TNode, class TLeafNode>
inline cUBTreeHeader<TKey>* cCommonUBTree<TKey,TNode,TLeafNode>::GetUBTreeHeader()
{
	return (cUBTreeHeader<TKey>*)parent::mHeader;
}

template<class TKey, class TNode, class TLeafNode>
inline cUBTreeHeader<TKey>& cCommonUBTree<TKey,TNode,TLeafNode>::GetRefUBTreeHeader()
{
	return *((cUBTreeHeader<TKey>*)parent::mHeader);
}

template<class TKey, class TNode, class TLeafNode>
inline const cSpaceDescriptor* cCommonUBTree<TKey,TNode,TLeafNode>::GetSpaceDescriptor() const
{
	return ((cUBTreeHeader<TKey>*)parent::mHeader)->GetSpaceDescriptor();
}

template<class TKey, class TNode, class TLeafNode>
bool cCommonUBTree<TKey,TNode,TLeafNode>::Open(cUBTreeHeader<TKey> *header, cQuickDB* quickDB, bool readOnly)
{
	if (!parent::Open(header, quickDB, readOnly))
	{
		return false;
	}

	cUBTreeHeader<TKey> **p = &header;
	// vvv mRQProcessor->SetQuickDB(quickDB);
	// vvv mRQProcessor->SetTreeHeader(header);

	return true;
}

/**
 * Insert item into UB-tree.
 **/
template<class TKey, class TNode, class TLeafNode>
int cCommonUBTree<TKey,TNode,TLeafNode>::Insert(const TKey &item, char* data)
{
	if (parent::mReadOnly)
	{
		printf("Critical Error: cCommonUBTree::Insert(), The tree is read only!\n");
		exit(1);
	}

	cUBTreeHeader<TKey>* header = GetUBTreeHeader();
	uint currentLevel = 0;  // counter of acrossing pages
	tNodeIndex nodeIndex = parent::mHeader->GetRootIndex(), insertNodeIndex = 0;
	int ret = 0, numCurrentLow, numPreviousHigh, numItem; // cRTreeConst::INSERT_NO;
	TLeafNode* currentLeafNode = NULL;
	TNode *currentNode = NULL;
	unsigned int *currPath, *itemOrderCurrPath;
	bool leaf;
	cZRegion currentZRegion = NULL;
	cZRegion previousZRegion = NULL;
	cInsertBuffers<TKey> insertBuffers;
	cNodeBuffers<TKey>* nodeBuffers = &insertBuffers.nodeBuffer;
	char* lowAdress;
	char* highAdress;


	if (parent::mDebug)
	{
		item.Print("\n", GetSpaceDescriptor());
	}

	if (header->GetItemCount() == 0)
	{
		TKey::Copy(header->GetTreeMBR()->GetLoTuple()->GetData(), item.GetData(), GetSpaceDescriptor());
		TKey::Copy(header->GetTreeMBR()->GetHiTuple()->GetData(), item.GetData(), GetSpaceDescriptor());
	}
	
	else
	{
		header->GetTreeMBR()->ModifyMbr(item, GetSpaceDescriptor());
	}

	parent::Insert(item);
	
	for (;;)
	{

		
		if ((leaf = TNode::IsLeaf(nodeIndex)))
			{
				currentLeafNode = parent::ReadLeafNodeW(nodeIndex);
			}
			else
			{
				currentNode = parent::ReadInnerNodeR(nodeIndex);
			}

		
		
/*
		- -1 if the first item is lower than the second item.
		- 0 if the items are equal
		- 1 if the first item is higher than the second item.
*/		

		for(int i = 0; i< currentLeafNode->GetItemCount();i++)
		{
		
		currentZRegion = curentLeafNode->GetItem(i);
		lowAdress = currentZRegion.getLowAdress;
		highAdress = currentZRegion.getHighAdress;
				
		if(item.Compare(lowAdress, currentLeafNode->GetNodeHeader()->GetKeyDescriptor()) >= 0 ){
			if(item.Compare(highAdress, currentLeafNode->GetNodeHeader()->GetKeyDescriptor()) <= 0){
				// je v rozsahu Zregionn -> insert 
			}

			else if (item.Compare(highAdress, currentLeafNode->GetNodeHeader()->GetKeyDescriptor())  == 1){
				previousZRegion = currentZRegion;
			
			}
	   		
		}
		else if (item.Compare(lowAdress, currentLeafNode->GetNodeHeader()->GetKeyDescriptor()) == -1 ){
			
			if(previousZRegion != NULL)
			{
				if(item.Compare(previousZRegion.getHighAdress, currentLeafNode->GetNodeHeader()->GetKeyDescriptor()) == 1)
				{
				
					//pri dalsim pruchodu je mensi ale nespada do zadneho 
			
					numCurrentLow = atoi(lowAdress);
					numPreviousHigh = atoi(previousZRegion.getHighAdress);
					numItem = atoi(item);

					if(abs(numItem - numCurrentLow) >= abs(numItem - numPreviousHigh))
					{
						//insert do previous , uprava zregionu
					}
					else
					{
						// insert do current, uprava zregionu
					}

					previousZRegion = NULL;
				}
			}			
			
			currentLeafNode->SetKey(order, cZRegion(item,highAdress));
			// v pripade ze je mensi nez aktuální z region
			//insert
		}
		}


		//insert do LEAF

		if(leaf)
		{
				//test zda je list prázdný
				if(currentLeafNode->HasLeafFreeSpace(item, data))
				{
					currentLeafNode->InsertLeafItem(item, data, parent::mHeader->DuplicatesAllowed());
					currentNode->SetKey(nodeIndex, cZRegion(item,item)//upravit);					
				}
				else
				{
				TNode* newNode = parent::ReadNewInnerNode();
				ar* TNode_mem = parent::mMemoryPool->GetMem(parent::mHeader->GetNodeInMemSize());
				TNode tmpNode(newNode, TNode_mem);

				currentNode->SetKey(order, cZRegion(item,item))//upravit);
				currentNode->Split(*newNode, tmpNode, nodeBuffers);
				
				currentNode->InsertItem(insertBuffers.secondItem, insNodeIndex, parent::mHeader->DuplicatesAllowed());
				
				tmpNode.SetData(NULL);
				parent::mMemoryPool->FreeMem(TNode_mem);

				currentNode->CopyKeyTo(insertBuffers.firstItem, currentNode->GetItemCount() - 1, &nodeBuffers->itemBuffer);
				newNode->CopyKeyTo(insertBuffers.secondItem, newNode->GetItemCount() - 1, &nodeBuffers->itemBuffer);				
				}
		}


	}

	return ret;
}


/**
 * Create Empty UB-Tree
 **/
template<class TKey, class TNode, class TLeafNode>
bool cCommonUBTree<TKey,TNode,TLeafNode>::Create(cUBTreeHeader<TKey> *header, cQuickDB* quickDB)
{
	if (!parent::Create(header, quickDB))
	{
		return false;
	}

	cUBTreeHeader<TKey> **p = &header;

	// create the root node with one leaf node
	TNode* node = parent::ReadNewInnerNode();
	node->SetItemCount(0); // del
	node->SetLeaf(false);  // del

	parent::mHeader->SetRootIndex(node->GetIndex());
	parent::mHeader->SetInnerNodeCount(1);
	parent::mHeader->SetHeight(0);
	parent::mHeader->SetLeafNodeCount(0);
	parent::mSharedCache->UnlockW(node);

	//mRTreeLog.Open(filename);

	return true	mRQProcessor->SetQuickDB(quickDB);
	mRQProcessor->SetTreeHeader(header);

	// create empty root node
	TLeafNode* leafnode = parent::ReadNewLeafNode();
	
	leafnode->SetItemCount(0);
	leafnode->SetLeaf(true);
	leafnode->Init();
	//leafnode->SetExtraLink(0, TNode::EMPTY_LINK);
	//leafnode->SetExtraLink(1, TNode::EMPTY_LINK);
	parent::mHeader = header;
	parent::mHeader->ResetNodeCount();
	parent::mHeader->ResetItemCount();

	parent::mHeader->SetHeight(0);
	parent::mHeader->SetLeafNodeCount(1);
	parent::mHeader->SetRootIndex(leafnode->GetLeafNodeIndex(leafnode->GetIndex()));    // 3.6.2013: nebylo GetLeafNodeIndex, nemohlo to fungovat
	parent::mSharedCache->UnlockW(leafnode);

	mActualNode = TNode::EMPTY_LINK;

	InitInMemCache(header);

	return true;
}

template<class TKey, class TNode, class TLeafNode>
bool cCommonUBTree<TKey,TNode,TLeafNode>::Close()
{
	if (!parent::Close())
	{
		return false;
	}

	return true;
}

/**
 * Create Empty UB-Tree. The tree has to be already opened
 **/
template< class TKey, class TNode, class TLeafNode>
bool cCommonUBTree<TKey,TNode,TLeafNode>::Clear()
{
	if (!cPagedTree<TKey,TNode,TLeafNode>::Clear())
	{
		return false;
	}

	printf("cCommonUBTree<TKey,TNode,TLeafNode>::Clear() - has to be implemented!\n");

	return true;
}

template< class TKey, class TNode, class TLeafNode>
void cCommonUBTree<TKey, TNode, TLeafNode>::PrintIndexSize(uint blockSize)
{
	printf("Index Size: %.2f", parent::GetIndexSizeMB(blockSize));
	if (GetUBTreeHeader()->IsSignatureEnabled())
	{
		mSignatureIndex->PrintIndexSize(blockSize);
	}
	printf(" MB\n");
}

template<class TKey, class TNode, class TLeafNode>
void cCommonUBTree<TKey,TNode,TLeafNode>::PrintInfo()
{
	cPagedTree<TKey,TNode,TLeafNode>::PrintInfo();
	printf("Dimension:             %d\t Type:  %c\n", GetUBtreeHeader()->GetSpaceDescriptor()->GetDimension(), GetUBtreeHeader()->GetSpaceDescriptor()->GetType(0)->GetCode());
	printf("Root region:           ");
	TKey::Print(GetUBTreeHeader()->GetTreeMBR()->GetLoTuple()->GetData(), " x ", GetUBTreeHeader()->GetSpaceDescriptor());
	TKey::Print(GetUBTreeHeader()->GetTreeMBR()->GetHiTuple()->GetData(), "\n\n", GetUBTreeHeader()->GetSpaceDescriptor());

	/*if (((cRTreeHeader<TKey>*)parent::mHeader)->IsSignatureEnabled())
	{
		mSignatureIndex->PrintInfo();
	}*/
}


}}}
#endif
