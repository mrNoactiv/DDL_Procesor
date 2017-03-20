/**
*	\file cBpTreeNode.h
*	\author Radim Baca
*	\version 0.1
*	\date feb 2008
*	\brief Header of the cBpTree.
*/


#ifndef __cBpTreeNode_h__
#define __cBpTreeNode_h__

#include "dstruct/paged/b+tree/cB+TreeConst.h"
#include "dstruct/paged/core/cTreeHeader.h"
#include "dstruct/paged/core/cTreeNode.h"
#include "dstruct/paged/core/cTreeItemStream.h"
#include "dstruct/paged/core/cItemStream.h"
#include "dstruct/paged/b+tree/cB+TreeNodeHeader.h"
#include "dstruct/paged/queryprocessing/cRangeQueryConfig.h"
#include "dstruct/paged/queryprocessing/cQueryProcStat.h"
#include "dstruct/paged/queryprocessing/cRangeQueryProcessorConstants.h"
#include "dstruct/paged/queryprocessing/sBatchRQ.h"
#include "dstruct/paged/queryprocessing/cRQBuffers.h"
#include "common/memorystructures/cLinkedList.h"

using namespace dstruct::paged;
using namespace dstruct::paged::core;

/**
*	Header of the cBpTree.
* It is parameterized by the key type being stored in the tree. 
*
*	\author Radim Baca, David Bednář, Michal Krátký
*	\version 0.1
*	\date feb 2008
**/
namespace dstruct {
	namespace paged {
		namespace bptree {

// static const unsigned int MAX_STATES = 46000;
static const unsigned int Query_Count = 40000;
static const unsigned int LeafNode_Count = 24000;

struct cLeafIndexItem
{
	tNodeIndex leafIndex;
	char *states;
};

class cLeafIndices
{
public:
		cLeafIndexItem** mIndexItems;

public:
		cLeafIndices(char* memory);

		static unsigned int GetMemSize();

		static int SortingPredicate (const void* item1, const void* item2) 
		{
#ifdef DEBUG
			cLeafIndexItem item1t = (*(cLeafIndexItem*)item1);
			cLeafIndexItem item2t = (*(cLeafIndexItem*)item2);
			return item1t.leafIndex - item2t.leafIndex; 
#else
			return (*(cLeafIndexItem*)item1).leafIndex - (*(cLeafIndexItem*)item2).leafIndex; 
#endif
		}
};

//class dstruct::paged::core::cTreeHeader;

//using namespace dstruct::paged::core;
			
template<class TKey>
class cBpTreeNode : public cTreeNode<TKey>
{
	typedef cTreeNode<TKey> parent;

public:
	cBpTreeNode(const cBpTreeNode<TKey>* origNode, const char *mem);

	public:
		static const char INTPOS_1 = 1;
		static const char INTPOS_2 = 2;
		static const char INTPOS_3 = 3;
		static const char INTPOS_4 = 4;
		static const char INTPOS_5 = 5;
		static const char INTPOS_6 = 6;
		static const char STATE_1 = 1;
		static const char STATE_2 = 2;
		static const char STATE_3 = 3;
		static const char STATE_4 = 4;
		static const char STATE_5 = 5;
		static const char STATE_6 = 6;
		static const int SUBTREE_NOT_RELEVANT = -2;
		static const int SUBTREE_RELEVANT = -3;

		static unsigned int ScanLeafNodes(const cTreeNode<TKey>& startLeafNode, char* il, char *ih, unsigned int finishResultSize, 
			cTreeItemStream<TKey>* itemStream, cNodeBuffers<TKey>* buffers = NULL);
		static int SearchNode(const cTreeNode<TKey>& node, char* il, char *ih, cNodeBuffers<TKey>* buffers = NULL);

		static int FindRelevantSubtrees(cTreeNode<TKey>& node, const char* ils, const char *ihs, unsigned int intsCount, char* srmRow, char* srmParentRow, unsigned int *startOrder, cNodeBuffers<TKey>* buffers = NULL);
		static unsigned int ScanLeafNode(cTreeNode<TKey>& leafNode, char* ils, char *ihs, unsigned int intsCount, unsigned int finishResultSize, 
			cTreeItemStream<TKey>* itemStream, char* intervalState, cNodeBuffers<TKey>* buffers = NULL);
		static unsigned int ComputeIntervalPosition(const char* lbound, const char* hbound, const char* il, const char *ihm, const cDTDescriptor* pSd, cNodeBuffers<TKey>* buffers = NULL);

		void ScanNode(unsigned int level, unsigned int treeHeight, int& itemOrder, sBatchRQ* batchRQ, cTreeItemStream<TKey> *resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey>* rqBuffers, cQueryProcStat *QueryProcStat);
		void ScanNode_Btree_seq(int& itemOrder, sBatchRQ *batchRQ, cLinkedList<uint> *qrs, cLinkedList<uint> *nqrs, cRangeQueryConfig *rqConfig, unsigned int* resultSizes);
		void ScanNode_Btree_bin(int& itemOrder, sBatchRQ *batchRQ, cLinkedList<uint> *qrs, cLinkedList<uint> *nqrs, cRangeQueryConfig *rqConfig, unsigned int* resultSizes);
		bool ScanLeafNode(unsigned int level, sBatchRQ *batchRQ, cTreeItemStream<TKey> *resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey>* rqBuffers, cQueryProcStat* QueryProcStat);
		bool ScanLeafNode_Btree_seq(unsigned int level, sBatchRQ *batchRQ, cLinkedList<uint> *qrs, cTreeItemStream<TKey> *resultSet, unsigned int finalResultSize, cRQBuffers<TKey>* rqBuffers, unsigned int* resultSizes, cNodeBuffers<TKey>* buffers);
		bool ScanLeafNode_Btree_bin(unsigned int level, sBatchRQ *batchRQ, cLinkedList<uint> *qrs, cTreeItemStream<TKey> *resultSet, unsigned int finalResultSize, cRQBuffers<TKey>* rqBuffers, unsigned int* resultSizes, cNodeBuffers<TKey>* buffers);
		bool ScanLeafNode_Btree_bin_lo(sBatchRQ *batchRQ, cLinkedList<uint> *qrs, cItemStream<TKey> *resultSet, unsigned int finalResultSize, unsigned int* resultSizes, cNodeBuffers<TKey>* buffers);
};

template<class TKey>
cBpTreeNode<TKey>::cBpTreeNode(const cBpTreeNode<TKey>* origNode, const char *mem) :
	cTreeNode<TKey>::cTreeNode(origNode, mem)
{
};

/**
 * It returns the number of items found.
 */
template<class TKey>
unsigned int cBpTreeNode<TKey>::ScanLeafNodes(const cTreeNode<TKey>& startLeafNode, char* il, char *ih, unsigned int finishResultSize, 
	cTreeItemStream<TKey>* itemStream, cNodeBuffers<TKey>* buffers)
{
	// mk: does not tested
	// it is not possible to compile it without an error
	/*
	int order = startLeafNode.FindOrder(il, startLeafNode.FIND_SBE, buffers->itemBuffer);
	unsigned int count = 0;

	if (order != cBpTreeNode<TKey>::FIND_NOTEXIST) 
	{
		while(TKey::Compare(ih, startLeafNode.GetCKey(order, buffers->itemBuffer), GetKeyDescriptor()) >= 0)
		{
			itemStream->Add(startLeafNode.GetCItem(order, buffers->itemBuffer));
			if (++order == leafNode.GetItemCount()) 
			{
				order = 0;
				if (startLeafNode->GetExtraLink(1) == startLeafNode->EMPTY_LINK) 
				{
					break;
				}
				mSharedCache->UnlockR(startLeafNode);
				startLeafNode = ReadLeafNodeR(startLeafNode->GetExtraLink(1));
				//if (mDebug) {currentLeafNode->Print(buffer);}				
			}
			if (finishResultSize > 0 && itemStream->GetItemCount() == finishResultSize)
			{
				break;
			}
		}
		count = itemStream->GetItemCount();
	}
	return count;*/
	return 0;
}

/**
 * It returns:
 *  - FIND_NOTEXIST: no another key is found
 *  - otherwise: some key is found
 */
template<class TKey>
int cBpTreeNode<TKey>::SearchNode(const cTreeNode<TKey>& node, char* il, char *ih, cNodeBuffers<TKey>* buffers)
{
	// mk: does not tested
	// it is not possible to compile it without an error
	/*
	int chld;

	if ((chld = node.FindOrder(il, cTreeNode<TKey>::FIND_SBE, buffers->itemBuffer)) != cTreeNode<TKey>::FIND_NOTEXIST)
	{
		if (mHeader->DuplicatesAllowed())
		{
			//while (chld >= 0 && currentNode->GetItem(chld)->Equal(il, true) >= 0)
			// while (chld >= 0 && currentNode->Compare(il, chld) < 0)
			while (chld >= 0 && TKeu::Compare(il, currentNode->GetCKey(chld, buffers->itemBuffer), GetKeyDescriptor()) < 0)
			{
				if (chld-- == 0) {break;}
			}
			chld++;
		}
	}
	return chld;
	*/
	return 0;
}

/**
 * srmRow - current row of the subtree relevance matrix
 * Interval Position between intervals Ia and Ib:
 * old
 *  - 1: Ib < Ia
 *  - 2: Ib_l < Ia_l && Ib_h in Ia
 *  - 3: Ib in Ia
 *  - 4: Ib_l in Ia && Ib_h > Ia_h
 *  - 5: Ib > Ia - it is the stop state
 *  - 6: Ia in Ib
 * new
 *  - 1: Ib < Ia
 *  - 2: Ib_l < Ia_l && Ib_h in Ia
 *  - 3: Ia in Ib
 *  - 4: Ib in Ia
 *  - 5: Ib_l in Ia && Ib_h > Ia_h
 *  - 6: Ib > Ia - it is the stop state
 */
template<class TKey>
int cBpTreeNode<TKey>::FindRelevantSubtrees(cTreeNode<TKey>& node, const char* ils, const char *ihs, 
	unsigned int intsCount, char* srmRow, char* srmParentRow, unsigned int *startOrder, cNodeBuffers<TKey>* buffers)
{
	// mk: does not tested
	// it is not possible to compile it without an error

	/*
	int minOrder = -1;
	unsigned int allStates = 0, tmpOrder, order;
	int ret = 0;
	//const char *startItem = node.GetCItem(startOrder);
	const cDTDescriptor* sd = node.GetHeader()->GetKeyDescriptor();
	unsigned int keySize = ((cTreeNodeHeader*)node.GetCHeader())->GetKeySize();

	const char* il = ils, *ih = ihs;

	//node.Print();

	//printf("\n");
	
	for (unsigned int i = 0; i < intsCount; i++)
	{

		//TKey::Print(il, "; ", sd);
		//TKey::Print(ih, "; ", sd);

		//printf("\n");

		if (srmParentRow == NULL || (srmParentRow[i] >= STATE_2 && srmParentRow[i] <= STATE_5))
		{
			tmpOrder = (minOrder == -1 ? node.GetItemCount() - 1 : minOrder);

			if (minOrder == *startOrder)
			{
				break;
			}
			
			if (*startOrder == tmpOrder)
			{
				order = tmpOrder;
			}
			else
			{
				//if(*startOrder == 4 && tmpOrder == 201)
				//{
				//	int neco = 0;
				//}
				order = node.FindOrder(il, node.FIND_SBE, &buffers->itemBuffer, *startOrder, tmpOrder);
				//if(order < *startOrder)
				//{
				//	int neco = 0;
				//}
			}

			//TKey::Print(il, "; ", sd);
			//if (order != cTreeNode::FIND_NOTEXIST)
			//{
			//	TKey::Print(node.GetCKey(order-1), "; ", sd);
			//	TKey::Print(node.GetCKey(order), "; ", sd);
			//}

			if (order != cTreeNode::FIND_NOTEXIST && order <= tmpOrder)
			{
				// this line solves the problem of node.FindOrder(il, node.FIND_SBE, ...)
				// we need find a interval function instead of this method
				//if ((order > 0) && (TKey::Compare(node.GetCKey(order-1), il, sd) > 0))
				//{
				//	continue;
				//}
				//minOrder = order;

				if (!((order > 0) && (TKey::Compare(node.GetCKey(order-1), il, sd) > 0)))
				{
					minOrder = order;
				}
			}
		}
		//il += TKey::GetSize(il, node.GetCHeader()->GetKeyDescriptor());
		//ih += TKey::GetSize(ih, node.GetCHeader()->GetKeyDescriptor());
		il += keySize;
		ih += keySize;
	}
	
	if (minOrder == -1) return SUBTREE_NOT_RELEVANT;

	il = ils, ih = ihs;
	
	//node.Print();

	const char *minItem = node.GetCKey(minOrder);
	const char *beforeMinItem = minOrder == 0 ? NULL : node.GetCKey(minOrder - 1);

	//TKey::Print(minItem, "; ", sd);
	//TKey::Print(il, "; ", sd);


	// we have the min order, compute the interval positions of the I^{minOrder}
	//
	for (unsigned int i = 0 ; i < intsCount ; i++)
	{
		if (srmParentRow != NULL && (srmParentRow[i] == STATE_1 || srmParentRow[i] == STATE_6))
		{
			srmRow[i] = srmParentRow[i];
		}
		else
		{
			srmRow[i] = ComputeIntervalPosition(beforeMinItem, minItem, il, ih, sd);
			//allStates += srmRow[i];
			//printf("%d \t", srmRow[i]);
		}
		//il += TKey::GetSize(il, node.GetCHeader()->GetKeyDescriptor());
		//ih += TKey::GetSize(ih, node.GetCHeader()->GetKeyDescriptor());
		il += keySize;
		ih += keySize;
	}

	//printf("\n");

	*startOrder = minOrder;

	//float tmp = allStates / intsCount;

	////if all states are 1
	//if(tmp == 1.0 || tmp == 6.0)
	//{
	//	ret = SUBTREE_NOT_RELEVANT;
	//}

	return ret;*/
	return 0;
}

/**
 * It returns the number of items found.
 */
template<class TKey>
unsigned int cBpTreeNode<TKey>::ScanLeafNode(cTreeNode<TKey>& leafNode, char* ils, char* ihs, unsigned int intsCount, unsigned int finishResultSize, 
	cTreeItemStream<TKey>* itemStream, char* intervalState, cNodeBuffers<TKey>* buffers)
{
	// mk: does not tested
	// it is not possible to compile it without an error

	/*
	int test;
	const unsigned int ordersSize = 30000;
	char* il = ils, *ih = ihs;
	unsigned int minOrder = leafNode.GetItemCount();
	const unsigned int leafNodeItemCount = leafNode.GetItemCount();
	const char *firstItem = leafNode.GetCItem(0);
	bool debug = false;
	int order;
	int orders[ordersSize];
	unsigned int count = 0;
	const cDTDescriptor* sd = leafNode.GetHeader()->GetKeyDescriptor();
	unsigned int keySize = ((cTreeNodeHeader*)leafNode.GetCHeader())->GetKeySize();
	assert(ordersSize > intsCount);
	
	for (unsigned int i = 0 ; i < intsCount ; i++)
	{
		if (intervalState[i] > STATE_1 && intervalState[i] < STATE_6)
		{
			order = leafNode.FindOrder(il, leafNode.FIND_SBE);
			// order zapsat order do pole
			orders[i] = order;
			//ordersMax[i] = leafNode.FindOrder(ih, leafNode.FIND_SBE);
			if (minOrder != 0 && (order != cTreeNode::FIND_NOTEXIST && order < minOrder))
			{
				//if (order == 0 && TKey::Compare(leafNode.GetCKey(order), il, sd) > 0)
				//{
				//	continue;
				//}
				minOrder = order;
			}
		}
		else
		{
			//interval not relevant to node
			orders[i] = -1;
		}
		
		il += keySize;
		ih += keySize;
	}

	order = minOrder;
	il = ils, ih = ihs;

	//qsort(orders, intsCount, sizeof(int), cNgramCommons::intSort);

	for (unsigned int i = 0; i < intsCount ; i++)
	{
		// if (orders[i] >= order && orders[i] != FIND_NOTEXIST)
		if (orders[i] != FIND_NOTEXIST)
		{
			if(orders[i] > order)
			{
				order = orders[i];
			}
			//while(ih.Compare(leafNode.GetCKey(order, buffer), leafNode->GetHeader()->GetKeyDescriptor()) >= 0)
			while ((test = TKey::Compare(ih, leafNode.GetCKey(order, &buffers->itemBuffer), sd)) >= 0)
			{
				itemStream->Add(leafNode.GetCItem(order, &buffers->itemBuffer));
				if (++order == leafNodeItemCount || (finishResultSize > 0 && itemStream->GetItemCount() == finishResultSize))
				{
					break;
				}
				count = itemStream->GetItemCount();
			}
			
			//minOrder = order;

			if (order >= leafNodeItemCount) 
			{
				break;
			}
		}
				
		il += keySize;
		ih += keySize;
	}

	return count;*/
	return 0;
}

/**
 * We have three states: Ib < Ia (1), Ib in Ia (2), Ib > Ia (6)
 * comparing the separator keys value with query intervals
 */
template<class TKey>
unsigned int cBpTreeNode<TKey>::ComputeIntervalPosition(const char* lbound, const char* hbound, const char* il, const char *ih, const cDTDescriptor* pSd, cNodeBuffers<TKey>* buffers)
{
	//TKey::Print(ih, "; ", pSd);
	//TKey::Print(lbound, "; ", pSd);

	unsigned int intpos;

	//if (lbound != NULL)
	//{
	//	if (TKey::Compare(ih, lbound, pSd) < 0)
	//	{
	//		intpos = INTPOS_1;
	//	}
	//	else if (TKey::Compare(hbound, il, pSd) < 0)
	//	{
	//		intpos = INTPOS_6;
	//	}
	//	else if(TKey::Compare(il, lbound, pSd) < 0)
	//	{
	//		if(TKey::Compare(ih, hbound, pSd) > 0)
	//		{
	//			intpos = INTPOS_3;
	//		}
	//		else
	//		{
	//			intpos = INTPOS_2;
	//		}
	//	}
	//	else //if(TKey::Compare(il, lbound, pSd) > 0)
	//	{
	//		if(TKey::Compare(ih, hbound, pSd) > 0)
	//		{
	//			intpos = INTPOS_5;
	//		}
	//		else
	//		{
	//			intpos = INTPOS_4;
	//		}
	//	}
	//}
	//else
	//{
	//	if (TKey::Compare(hbound, il, pSd) < 0)
	//	{
	//		intpos = INTPOS_6;
	//	}
	//	else if(TKey::Compare(ih, hbound, pSd) < 0)
	//	{
	//		intpos = INTPOS_4;
	//	}
	//	else
	//	{
	//		intpos = INTPOS_5;
	//	}
	//}

	if (lbound != NULL && TKey::Compare(ih, lbound, pSd) < 0)
	{
		intpos = INTPOS_1;
	}
	else if (TKey::Compare(hbound, il, pSd) < 0)
	{
		intpos = INTPOS_6;
	}
	else
	{
		intpos = INTPOS_2;
	}

	return intpos;
}

template<class TKey>
void cBpTreeNode<TKey>::ScanNode(unsigned int level, unsigned int treeHeight, int& itemOrder, sBatchRQ* batchRQ, cTreeItemStream<TKey> *resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey>* rqBuffers, cQueryProcStat *QueryProcStat)
{
	switch (batchRQ->mode)
	{
	case cRangeQueryProcessorConstants::RQ_BTREE_SEQ:
		ScanNode_Btree_seq(itemOrder, batchRQ, rqBuffers->qrs_ll[level], rqBuffers->qrs_ll[level + 1], rqConfig, rqBuffers->resultSizes);
		break;
	case cRangeQueryProcessorConstants::RQ_BTREE_BIN:
	case cRangeQueryProcessorConstants::RQ_BTREE_BIN_LAST_ORDER:
		ScanNode_Btree_bin(itemOrder, batchRQ, rqBuffers->qrs_ll[level], rqBuffers->qrs_ll[level + 1], rqConfig, rqBuffers->resultSizes);
		break;
	case cRangeQueryProcessorConstants::RQ_BTREE_L_LAST_BIN:
		ScanNode_Btree_seq(itemOrder, batchRQ, rqBuffers->qrs_ll[level], rqBuffers->qrs_ll[level + 1], rqConfig, rqBuffers->resultSizes);
		break;
	case cRangeQueryProcessorConstants::RQ_BTREE_L0_SEQ:
		if (cTuple::levelTree == 0)
		{
			ScanNode_Btree_seq(itemOrder, batchRQ, rqBuffers->qrs_ll[level], rqBuffers->qrs_ll[level + 1], rqConfig, rqBuffers->resultSizes);
		}
		else
		{
			ScanNode_Btree_bin(itemOrder, batchRQ, rqBuffers->qrs_ll[level], rqBuffers->qrs_ll[level + 1], rqConfig, rqBuffers->resultSizes);
		}
		break;
	default:
		exit(0);
	}
}

template<class TKey>
void cBpTreeNode<TKey>::ScanNode_Btree_seq(int& itemOrder, sBatchRQ* batchRQ,
	cLinkedList<unsigned int> *qrs, cLinkedList<unsigned int> *nqrs, cRangeQueryConfig *rqConfig, unsigned int* resultSizes)
{
	bool proceed = false, lastToDelete = false;
	uint qrsCount = qrs->GetItemCount();
	uint ind;
	const char *hItem, *lItem;
	char *hQuery, *lQuery;

	cLinkedListNode<unsigned int>* lastFoundNode = NULL;
	unsigned int lastFoundNodeOrder = 0;

	cSpaceDescriptor* sd = (cSpaceDescriptor*)parent::GetHeader()->GetKeyDescriptor();
	nqrs->Clear();

	int itemCount = (int)parent::GetItemCount();

	for (itemOrder++; itemOrder < itemCount; itemOrder++)
	{
		if (qrsCount == 0 || itemCount == 0)
		{
			itemOrder = itemCount;
			break;
		}

		proceed = false;
		lastToDelete = false;

		hItem = parent::GetCKey(itemOrder);
		lItem = itemOrder == 0 ? NULL : parent::GetCKey(itemOrder - 1);

		//val644 - start - pro nesetrizene rozsahove dotazy
		if (!cTuple::typeRQordered)
		{
			lastFoundNode = NULL;
			lastFoundNodeOrder = 0;
		}
		//val644 - end - pro nesetrizene rozsahove dotazy

		qrs->GetRefItem(&lastFoundNode, lastFoundNodeOrder, 0);
		ind = lastFoundNode->Item;

		/*if (itemOrder != 0)
		{
		hQuery = batchRQ->qhs[ind].GetData();
		//val644 - start
		printf("\thQuery: ");
		cTuple::Print(hQuery, "\n", mSD_FixedLen);
		//val644 - end

		while (TKey::Compare(hQuery, lItem, sd) < 0)
		{
		qrs->DeleteNode(lastFoundNode); //val644 - lastFoundNode, Prev a Next jsou NULL a pak se kvuli toho zacykli na LOCKu

		if (--qrsCount == 0) break;

		qrs->GetRefItem(&lastFoundNode, lastFoundNodeOrder, 0);
		ind = lastFoundNode->Item;

		hQuery = batchRQ->qhs[ind].GetData();
		}
		}
		*/
		for (int j = 0; j < qrsCount; j++)
		{
			qrs->GetRefItem(&lastFoundNode, lastFoundNodeOrder, j);
			ind = lastFoundNode->Item;

			//if (rqConfig->GetFinalResultSize() != 0 && rqConfig->GetFinalResultSize() <= resultSizes[ind])
			//{
			//	if (qrs->GetItemCount() == 1)
			//	{
			//		//it is only a simple optimization, it should be solved for general qrsCount by
			//		//removing irrelevant range queries in qrs
			//		itemOrder = itemCount;
			//	}
			//	continue;  // the finalResultSize for this query has been already reached, do not process this query
			//}

			if (TKey::Compare(hItem, batchRQ->qls[ind].GetData(), sd) >= 0)
			{
				nqrs->AddItem(ind);
				//nqrs->Print(); //val644
				proceed = true;

				hQuery = batchRQ->qhs[ind].GetData();
				if (TKey::Compare(hQuery, hItem, sd) <= 0)
				{
					qrs->DeleteNode(lastFoundNode);
					lastFoundNode = NULL;
					j--;
					qrsCount--;
				}
			}
			else
			{
				//val644 - start - pro setrizene rozsahove dotazy
				if (cTuple::typeRQordered)
				{
					break;
				}
				//val644 - end - pro setrizene rozsahove dotazy
			}
		}
		if (proceed)
		{
			break;
		}
	}
}

template<class TKey>
void cBpTreeNode<TKey>::ScanNode_Btree_bin(int& itemOrder, sBatchRQ* batchRQ,
	common::memorystructures::cLinkedList<unsigned int> *qrs, common::memorystructures::cLinkedList<unsigned int> *nqrs, cRangeQueryConfig *rqConfig, unsigned int* resultSizes)
{
	bool proceed = false;
	uint qrsCount = qrs->GetItemCount();
	uint ind;
	const char *hItem, *lItem;
	char *hQuery, *lQuery;
	cLinkedListNode<unsigned int>* lastFoundNode = NULL;
	unsigned int lastFoundNodeOrder = 0;

	cSpaceDescriptor* sd = (cSpaceDescriptor*)parent::GetHeader()->GetKeyDescriptor();
	nqrs->Clear();

	int itemCount = (int)parent::GetItemCount();
	int lastIdx = itemCount - 1;

	for (itemOrder++; itemOrder < itemCount; itemOrder++)
	{
		proceed = false;

		if (itemCount == 0 || qrsCount == 0)
		{
			itemOrder = itemCount;
			break;
		}
		//val644 - start - pro setrizene rozsahove dotazy
		if (cTuple::typeRQordered)
		{
			lQuery = batchRQ->qls[qrs->GetRefItem(0)].GetData();

			itemOrder = parent::FindOrder(batchRQ->qls[qrs->GetRefItem(0)].GetData(), parent::FIND_SBE, NULL, itemOrder, lastIdx);
		}
		//val644 - end - pro setrizene rozsahove dotazy
		//val644 - start - pro nesetrizene rozsahove dotazy
		else
		{
			lastFoundNodeOrder = 0;
			lastFoundNode = NULL;

			qrs->GetRefItem(&lastFoundNode, lastFoundNodeOrder, 0);

			qrsCount = qrs->GetItemCount();
			unsigned int nejmensi = 0;
			lQuery = batchRQ->qls[nejmensi].GetData();
			for (unsigned int i = 0; i < qrsCount; i++)
			{
				qrs->GetRefItem(&lastFoundNode, lastFoundNodeOrder, i);
				if (TKey::Compare(batchRQ->qls[lastFoundNode->Item].GetData(), lQuery, sd) < 0)
				{
					nejmensi = lastFoundNode->Item;
					lQuery = batchRQ->qls[nejmensi].GetData();
				}
			}
			itemOrder = parent::FindOrder(lQuery, parent::FIND_SBE, NULL, itemOrder, lastIdx);
		}
		//val644 - end - pro nesetrizene rozsahove dotazy

		hItem = parent::GetCKey(itemOrder);
		lItem = itemOrder == 0 ? NULL : parent::GetCKey(itemOrder - 1);

		//val644 - start - pro nesetrizene rozsahove dotazy
		if (!cTuple::typeRQordered)
		{
			lastFoundNode = NULL;
			lastFoundNodeOrder = 0;
		}
		//val644 - end - pro nesetrizene rozsahove dotazy

		qrs->GetRefItem(&lastFoundNode, lastFoundNodeOrder, 0);
		ind = lastFoundNode->Item;

		/*if (itemOrder != 0)
		{
		hQuery = batchRQ->qhs[ind].GetData();

		while (TKey::Compare(hQuery, lItem, sd) < 0)
		{
		qrs->DeleteNode(lastFoundNode);

		if (--qrsCount == 0) break;

		qrs->GetRefItem(&lastFoundNode, lastFoundNodeOrder, 0);
		ind = lastFoundNode->Item;

		hQuery = batchRQ->qhs[ind].GetData();
		}
		}*/

		for (int j = 0; j < qrsCount; j++)
		{
			qrs->GetRefItem(&lastFoundNode, lastFoundNodeOrder, j);
			ind = lastFoundNode->Item;

			//if (rqConfig->GetFinalResultSize() != 0 && rqConfig->GetFinalResultSize() <= resultSizes[ind])
			//{
			//	if (qrs->GetItemCount() == 1)
			//	{
			//		//it is only a simple optimization, it should be solved for general qrsCount by
			//		//removing irrelevant range queries in qrs
			//		itemOrder = itemCount;
			//	}
			//	continue;  // the finalResultSize for this query has been already reached, do not process this query
			//}

			if (TKey::Compare(hItem, batchRQ->qls[ind].GetData(), sd) >= 0)
			{
				nqrs->AddItem(ind);
				//nqrs->Print(); //val644
				proceed = true;

				hQuery = batchRQ->qhs[ind].GetData();
				if (TKey::Compare(hQuery, hItem, sd) <= 0)
				{
					qrs->DeleteNode(lastFoundNode);
					lastFoundNode = NULL;
					j--;
					qrsCount--;
				}
			}
			else
			{
				if (cTuple::typeRQordered)
				{
					break;
				}
				//break;
			}
		}
		//val644 - end
		if (proceed)
		{
			break;
		}
	}
}

template<class TKey>
bool cBpTreeNode<TKey>::ScanLeafNode(unsigned int level, sBatchRQ *batchRQ, cTreeItemStream<TKey> *resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey>* rqBuffers, cQueryProcStat* QueryProcStat)
{
	bool ret = false;
	if (QueryProcStat != NULL)
	{
		QueryProcStat->IncLarLnQuery();
	}

	switch (batchRQ->mode)
	{
	case cRangeQueryProcessorConstants::RQ_BTREE_SEQ:
		ret = ScanLeafNode_Btree_seq(level, batchRQ, rqBuffers->qrs_ll[level], resultSet, rqConfig->GetFinalResultSize(), rqBuffers, rqBuffers->resultSizes, &rqBuffers->nodeBuffer);
		break;
	case cRangeQueryProcessorConstants::RQ_BTREE_BIN:
		ret = ScanLeafNode_Btree_bin(level, batchRQ, rqBuffers->qrs_ll[level], resultSet, rqConfig->GetFinalResultSize(), rqBuffers, rqBuffers->resultSizes, &rqBuffers->nodeBuffer);
		break;
	case cRangeQueryProcessorConstants::RQ_BTREE_BIN_LAST_ORDER:
		ret = ScanLeafNode_Btree_bin_lo(batchRQ, rqBuffers->qrs_ll[level], resultSet, rqConfig->GetFinalResultSize(), rqBuffers->resultSizes, &rqBuffers->nodeBuffer);
		break;
	case cRangeQueryProcessorConstants::RQ_BTREE_L_LAST_BIN:
		ret = ScanLeafNode_Btree_bin(level, batchRQ, rqBuffers->qrs_ll[level], resultSet, rqConfig->GetFinalResultSize(), rqBuffers, rqBuffers->resultSizes, &rqBuffers->nodeBuffer);
		break;
	case cRangeQueryProcessorConstants::RQ_BTREE_L0_SEQ:
		ret = ScanLeafNode_Btree_bin(level, batchRQ, rqBuffers->qrs_ll[level], resultSet, rqConfig->GetFinalResultSize(), rqBuffers, rqBuffers->resultSizes, &rqBuffers->nodeBuffer);
		break;
	}

	return ret;
}

//val644 zamena cItemStream<TKey> *resultSet za cTreeItemStream
template<class TKey>
bool cBpTreeNode<TKey>::ScanLeafNode_Btree_seq(unsigned int level, sBatchRQ *batchRQ, common::memorystructures::cLinkedList<unsigned int> *qrs,
	cTreeItemStream<TKey> *resultSet, unsigned int finalResultSize, cRQBuffers<TKey>* rqBuffers, unsigned int* resultSizes, cNodeBuffers<TKey>* buffers)
{
	bool endf = false;
	int itemOrder = 0;
	int endItemOrder = 0;
	uint ind;
	const char* item;
	uint qrsCount = qrs->GetItemCount();

	cSpaceDescriptor* sd = (cSpaceDescriptor*)parent::GetHeader()->GetKeyDescriptor();

	uint itemCount = parent::GetItemCount();
	int qrsStart = 0;

	cLinkedListNode<unsigned int>* lastFoundNode = NULL;
	unsigned int lastFoundNodeOrder = 0;

	/*if (batchRQ->qhs[qrs->GetRefItem(0)].Compare(GetCItem(0, &buffers->itemBuffer), sd) < 0)
	{
	qrsStart++;
	}*/

	//val644 - start - nastaveni jaky druh vraceni v rozsahovych dotazech se pouzije
	int previousStartOrder = 0;
	int previousEndOrder = 0;
	//val644 - end
	for (int j = qrsStart; j < qrsCount; j++)
	{
		//val644 - start, typ prochazeni rozsahovych dotazu.
		switch (cTuple::typeRQ)
		{
		case 0:
			//0 = Vzdy se nastavi na zacatek
			itemOrder = 0;
			break;
		case 1:
			//1 = na zacatek predesleho rozsahoveho dotazu
			itemOrder = previousStartOrder;
			break;
		case 2:
			//2 = konec predesleho rozsahoveho dotazu (tento typ odstrani duplicity ve vysledku)
			itemOrder = previousEndOrder;
			break;
		default:
			break;
		}
		//val644 - end
		qrs->GetRefItem(&lastFoundNode, lastFoundNodeOrder, j);
		ind = lastFoundNode->Item;
		
		int compareResult = 1;
		if (batchRQ->qls[ind].flagRQ == 0)
		{
			while ((itemOrder < itemCount) && ((compareResult = TKey::Compare(item = GetCItem(itemOrder, &buffers->itemBuffer), batchRQ->qls[ind].GetData(), sd)) < 0))
			{
				itemOrder++;
				previousStartOrder = itemOrder; //val644
			}
		}
		else if (batchRQ->qls[ind].flagRQ == 1 && cTuple::typeRQ != 2)
		{
			itemOrder = 0;
		}

		//if (compareResult == 0)
		//{
		//	resultSet->Add(item);
		//	resultSizes[ind]++;
		//	itemOrder++;
		//	previousEndOrder = itemOrder; //val644
		//	compareResult = 1;
		//}
		endItemOrder = 0;
		if (compareResult = TKey::Compare(item = GetCItem(itemCount - 1, &buffers->itemBuffer), batchRQ->qhs[ind].GetData(), sd) <= 0)
		{
			while (itemOrder < itemCount)
			{
				resultSet->Add(item);
				resultSizes[ind]++;

				itemOrder++;
				previousEndOrder = itemOrder; //val644
			}
			//batchRQ->qls[ind].flagRQ = 1;
			const int *ptr = &batchRQ->qls[ind].flagRQ;
			int *ptr1 = const_cast <int *>(ptr);
			*ptr1 = *ptr1 + 1;
			//batchRQ->qls[ind].SetFlagRQ(1);
		}
		else if (itemOrder < itemCount)
		{
			endItemOrder = parent::FindOrder(batchRQ->qhs[ind].GetData(), parent::FIND_SBE, &buffers->itemBuffer, itemOrder, itemCount);
			endItemOrder++;
		}

		//while ((itemOrder < itemCount) && ((compareResult = TKey::Compare(item = GetCItem(itemOrder, &buffers->itemBuffer), batchRQ->qhs[ind].GetData(), sd)) <= 0))
		while (itemOrder < endItemOrder)
		{
			resultSet->Add(item);
			resultSizes[ind]++;

			itemOrder++;
			previousEndOrder = itemOrder; //val644
		}
		bool removeRQ = false;
		if (itemOrder == itemCount)
		{
			if (compareResult == 0)
			{
				//endf = true;
				removeRQ = true;
			}
		}
		else if (itemOrder < itemCount)
		{
			removeRQ = true;
		}
		if (removeRQ)
		{
			qrs->DeleteNode(lastFoundNode);
			lastFoundNode = NULL;
			if (qrsCount > 1)
			{
				j--;
				qrsCount = qrs->GetItemCount();
			}
			else if (qrsCount == 1 & qrs->GetItemCount() == 1)
			{
				qrs->Clear();
				rqBuffers->qrs_ll[level]->Clear();
			}
			cLinkedListNode<unsigned int>* deleteNode = NULL;

			for (int i_level = level - 1; i_level > -1; i_level--)
			{
				unsigned int delete_order = 0;
				deleteNode = NULL;
				for (unsigned int item_level = 0; item_level < rqBuffers->qrs_ll[i_level]->GetItemCount(); item_level++)
				{
					if (rqBuffers->qrs_ll[i_level]->GetItemCount() > 1)
					{
						rqBuffers->qrs_ll[i_level]->GetRefItem(&deleteNode, delete_order, item_level);
						if (deleteNode->Item == ind)
						{
							rqBuffers->qrs_ll[i_level]->DeleteNode(deleteNode);
							deleteNode = NULL;
							item_level--;
						}
						else if (deleteNode->Item > ind)
						{
							item_level = rqBuffers->qrs_ll[i_level]->GetItemCount();
						}
					}
					else
					{
						delete_order = 0;
						item_level - 0;
						rqBuffers->qrs_ll[i_level]->GetRefItem(&deleteNode, delete_order, item_level);
						if (deleteNode->Item == ind)
						{
							rqBuffers->qrs_ll[i_level]->Clear();
						}
					}
				}
			}
		}
	}
	return endf;
}

template<class TKey>
bool cBpTreeNode<TKey>::ScanLeafNode_Btree_bin(unsigned int level, sBatchRQ *batchRQ, common::memorystructures::cLinkedList<unsigned int> *qrs,
	cTreeItemStream<TKey> *resultSet, unsigned int finalResultSize, cRQBuffers<TKey>* rqBuffers, unsigned int* resultSizes, cNodeBuffers<TKey>* buffers)
{
	bool endf = false;
	int itemOrder = 0;
	int endItemOrder = 0;
	uint ind;
	const char* item;
	cSpaceDescriptor* sd = (cSpaceDescriptor*)parent::GetHeader()->GetKeyDescriptor();

	cLinkedListNode<unsigned int>* lastFoundNode = NULL;
	unsigned int lastFoundNodeOrder = 0;

	uint itemCount = parent::GetItemCount();
	//val644 - start 
	uint lastIdx = itemCount;
	//val644 - end
	//uint lastIdx = itemCount - 1; //val644 - zakomentovano
	uint qrsCount = qrs->GetItemCount();
	int qrsStart = 0;

	//val644 - start - nastaveni jaky druh vraceni v rozsahovych dotazech se pouzije
	int previousStartOrder = 0;
	int previousEndOrder = 0;
	//val644 - end
	for (int j = qrsStart; j < qrsCount; j++)
	{
		qrs->GetRefItem(&lastFoundNode, lastFoundNodeOrder, j);
		ind = lastFoundNode->Item;

		//if (finalResultSize != 0 && finalResultSize <= resultSizes[ind])
		//{
		//	continue;  // the finalResultSize for this query has been already reached
		//}
		//val644 - start, typ prochazeni rozsahovych dotazu.
		switch (cTuple::typeRQ)
		{
		case 0:
			//0 = Vzdy se nastavi na zacatek
			itemOrder = 0;
			break;
		case 1:
			//1 = na zacatek predesleho rozsahoveho dotazu
			itemOrder = previousStartOrder;
			break;
		case 2:
			//2 = konec predesleho rozsahoveho dotazu (tento typ odstrani duplicity ve vysledku)
			itemOrder = previousEndOrder;
			break;
		default:
			break;
		}
		//val644 - end

		if (batchRQ->qls[ind].flagRQ == 0)
		{
			itemOrder = parent::FindOrder(batchRQ->qls[ind].GetData(), parent::FIND_SBE, &buffers->itemBuffer, itemOrder, lastIdx);
			previousStartOrder = itemOrder;
		}
		else if (batchRQ->qls[ind].flagRQ == 1 && cTuple::typeRQ != 2)
		{
			itemOrder = 0;
		}
		//itemOrder = parent::FindOrder(batchRQ->qls[ind].GetData(), parent::FIND_SBE, &buffers->itemBuffer, itemOrder, lastIdx);
		//previousStartOrder = itemOrder;

		if (itemOrder == cTreeNode<TKey>::FIND_NOTEXIST)
		{
			continue;
		}

		int compareResult = 1;
		endItemOrder = 0;
		if (compareResult = TKey::Compare(item = GetCItem(itemCount - 1, &buffers->itemBuffer), batchRQ->qhs[ind].GetData(), sd) <= 0)
		{
			while (itemOrder < itemCount)
			{
				resultSet->Add(item);
				resultSizes[ind]++;

				itemOrder++;
				previousEndOrder = itemOrder; //val644
			}
			//batchRQ->qls[ind].flagRQ = 1;
			const int *ptr = &batchRQ->qls[ind].flagRQ;
			int *ptr1 = const_cast <int *>(ptr);
			*ptr1 = *ptr1 + 1;
			//batchRQ->qls[ind].SetFlagRQ(1);
		}
		else if (itemOrder < itemCount)
		{
			endItemOrder = parent::FindOrder(batchRQ->qhs[ind].GetData(), parent::FIND_SBE, &buffers->itemBuffer, itemOrder, itemCount);
			endItemOrder++;
		}

		//while ((itemOrder < itemCount) && ((compareResult = TKey::Compare(item = GetCItem(itemOrder, &buffers->itemBuffer), batchRQ->qhs[ind].GetData(), sd)) <= 0))
		while (itemOrder < endItemOrder)
		{
			resultSet->Add(item);
			resultSizes[ind]++;

			itemOrder++;
			previousEndOrder = itemOrder;
		}

		bool removeRQ = false;
		if (itemOrder == itemCount)
		{
			if (compareResult == 0)
			{
				//endf = true;
				removeRQ = true;
			}
		}
		else if (itemOrder < itemCount)
		{
			removeRQ = true;
		}
		if (removeRQ)
		{
			qrs->DeleteNode(lastFoundNode);
			lastFoundNode = NULL;
			if (qrsCount > 1)
			{
				j--;
				qrsCount = qrs->GetItemCount();
			}
			else if (qrsCount == 1 & qrs->GetItemCount() == 1)
			{
				qrs->Clear();
				rqBuffers->qrs_ll[level]->Clear();
			}
			cLinkedListNode<unsigned int>* deleteNode = NULL;

			for (int i_level = level - 1; i_level > -1; i_level--)
			{
				unsigned int delete_order = 0;
				deleteNode = NULL;
				for (unsigned int item_level = 0; item_level < rqBuffers->qrs_ll[i_level]->GetItemCount(); item_level++)
				{
					if (rqBuffers->qrs_ll[i_level]->GetItemCount() > 1)
					{
						rqBuffers->qrs_ll[i_level]->GetRefItem(&deleteNode, delete_order, item_level);
						if (deleteNode->Item == ind)
						{
							rqBuffers->qrs_ll[i_level]->DeleteNode(deleteNode);
							deleteNode = NULL;
							item_level--;
						}
						else if (deleteNode->Item > ind)
						{
							item_level = rqBuffers->qrs_ll[i_level]->GetItemCount();
						}
					}
					else
					{
						delete_order = 0;
						item_level - 0;
						rqBuffers->qrs_ll[i_level]->GetRefItem(&deleteNode, delete_order, item_level);
						if (deleteNode->Item == ind)
						{
							rqBuffers->qrs_ll[i_level]->Clear();
						}
					}
				}
			}
		}
		/*if (endf)
		{
		break;
		}*/
	}
	return endf;
}

template<class TKey>
bool cBpTreeNode<TKey>::ScanLeafNode_Btree_bin_lo(sBatchRQ *batchRQ, common::memorystructures::cLinkedList<unsigned int> *qrs,
	cItemStream<TKey> *resultSet, unsigned int finalResultSize, unsigned int* resultSizes, cNodeBuffers<TKey>* buffers)
{
	bool endf = false;
	int startOrder = 0;
	uint ind;
	const char* item;
	cSpaceDescriptor* sd = (cSpaceDescriptor*)parent::GetHeader()->GetKeyDescriptor();
	unsigned int complfinalResultSize = batchRQ->queriesCount * finalResultSize;

	cLinkedListNode<unsigned int>* lastFoundNode = NULL;
	unsigned int lastFoundNodeOrder = 0;

	uint itemCount = parent::GetItemCount();
	uint lastIdx = itemCount - 1;
	uint qrsCount = qrs->GetItemCount();
	int qrsStart = 0; //-1;

	lastIdx = parent::FindOrder(batchRQ->qls[qrs->GetRefLastItem()].GetData(), parent::FIND_SBE, &buffers->itemBuffer, startOrder, lastIdx);

	for (int j = qrsStart; j < qrsCount; j++)
	{
		qrs->GetRefItem(&lastFoundNode, lastFoundNodeOrder, j);
		ind = lastFoundNode->Item;

		if (finalResultSize != 0 && finalResultSize <= resultSizes[ind])
		{
			continue;  // the finalResultSize for this query has been already reached
		}

		if (j < qrsCount - 1)
		{
			startOrder = FindOrder(batchRQ->qls[ind].GetData(), parent::FIND_SBE, &buffers->itemBuffer, startOrder, lastIdx);
			if (startOrder == cTreeNode<TKey>::FIND_NOTEXIST)
			{
				continue;
			}
		}
		else
		{
			startOrder = lastIdx;
		}

		while ((startOrder != itemCount) && (TKey::Compare(item = GetCItem(startOrder, &buffers->itemBuffer), batchRQ->qhs[ind].GetData(), sd) <= 0))
		{
			resultSet->Add(item);
			resultSizes[ind]++;

			if (finalResultSize != 0 && complfinalResultSize == resultSet->GetItemCount())
			{
				endf = true;   // if the finalResultSize is reached, set the range query should be finished
				break;
			}

			startOrder++;
		}

		if (endf)
		{
			break;
		}
	}
	return endf;
}

}}}
#endif