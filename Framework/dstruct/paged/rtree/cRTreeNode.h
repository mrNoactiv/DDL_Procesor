/**
*	\file cRTreeNode.h
*	\author Michal Kratky
*	\version 0.1
*	\date 2001 - 2008
*	\brief Implementation of the R-tree's inner node.
*/

#ifndef __cRTreeNode_h__
#define __cRTreeNode_h__

#include <float.h>

namespace dstruct {
	namespace paged {
		namespace rtree {
template<class TMbr> class cRTreeNodeHeader;
}}}

#include "common/memorystructures/cStack.h"
#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "dstruct/paged/rtree/sItemIdRecord.h"
#include "dstruct/paged/rtree/cCommonRTreeNode.h"
#include "dstruct/paged/rtree/cRTreeNodeHeader.h"
#include "dstruct/paged/queryprocessing/cRangeQueryConfig.h"
#include "common/datatype/tuple/cHNTuple.h"
#include "dstruct/paged/rtree/cRTreeOrderIndex.h"

using namespace common::datatype::tuple;

/**
* Class is parametrized:
*		- TMbr - Inherited from cBasicType. Inner type must be type inherited from cTuple. This type must implement operator = with cTuple as a parameter.
*		- TItem - Class representing the inner item of the inner node (for example cRTreeItem<TMbr>).
*		- TLeafType - Inherited from cBasicType. Type of the unindexed data.
*
* Implementation of the R-tree's inner node.
*
*	\author Michal Kratky
*	\version 0.1
*	\date 2001 - 2008
**/

namespace dstruct {
	namespace paged {
		namespace rtree {

template<class TMbr>
class cRTreeNode: public cCommonRTreeNode<TMbr>
{
  typedef cCommonRTreeNode<TMbr> parent;

private:
	static bool my;

	void SplitDisMbrs(cRTreeNode<TMbr> &newNode);
	void SplitCommon(cRTreeNode<TMbr> &newNode); //fk
	// void Find2DisjunctiveMbrs(cTuple &tuplel, cTuple &tupleh) const;
	void SortBy(unsigned int dimension);

	typedef typename TMbr::Tuple TKey;

	static unsigned int FIND_MBR_COUNT;
	static unsigned int FIND_MBR_ISINMBR_COUNT;
	static unsigned int FIND_MBR_ISINMBR_COUNT_0;
	static unsigned int FIND_MBR_ISINMBR_COUNT_1;

public:
	static unsigned int II_Compares;

protected:
	void Find2DisjMbrs(char* pTMbr, int minimal_count);

private:
	// inline cRTreeNodeHeader<TMbr>* GetRTreeNodeHeader() const;

	bool FindMbr_MinimalVolume(const TKey &tuple, unsigned int &itemOrder);
	bool FindMbr_MinimalIntrVolume(const TKey &tuple, unsigned int &itemOrder);
	bool FindMbr_MinMaxTaxiDist(const TKey &tuple, unsigned int &itemOrder);

	unsigned int FindMbr_MinimalVolume_MP(const TKey &tuple);
	unsigned int FindMbr_MinimalIntrVolume_MP(const TKey &tuple);

public:
	static const unsigned int RQ_NOINTERSECTION = (unsigned int)~0;
	static int TMP_FindNextRelevantMbr_COUNT;

	static const unsigned int FINDMBR_OK = 0;
	static const unsigned int FINDMBR_MBRS = 1;
	static const unsigned int RECTANGLE_MODIFIED = 2;
	static const unsigned int FINDMBR_NONE = 3;

	cRTreeNode(const cRTreeNode<TMbr>* origNode, const char* mem);
	cRTreeNode(const cTreeNodeHeader* header, const char* mem);
	cRTreeNode();
	cRTreeNode(cTreeHeader* treeHeader);

	// bool Insert(unsigned int itemOrder, const char* TMbr_item1, const char* TMbr_item2, tNodeIndex insNodeIndex);
	void UpdateMbr(unsigned int itemOrder, const char* TMbr_item1);

	void Split(cRTreeNode &newNode);
	unsigned int FindNextRelevantMbr(unsigned int currentOrder, const TKey &ql, const TKey &qh);
	bool IsRegionRelevant(unsigned int currentOrder, const TKey &ql, const TKey &qh);

	void CreateMbr(char* pTMbr) const;
	void CreateMbr(unsigned int startOrder, unsigned int finishOrder, char* pTMbr, cNodeBuffers<TMbr>* buffers = NULL) const;
	// void CreateMbr(cTuple &ql, cTuple &qh) const;
	// void CreateMbr(unsigned int startOrder, unsigned int finishOrder, cTuple &ql, cTuple &qh) const;

	bool FindMbr(const TKey &tuple, unsigned int &itemOrder);
	int FindMbr_MP(const TKey &tuple, cStack<sItemIdRecord>& curPathStack, cStack<sItemIdRecord>& mbrHitStack);
	unsigned int FindModifyMbr_MP(const TKey &tuple);
	bool FindMbr_Ordered(const TKey &tuple, unsigned int &itemOrder, const cRTreeOrderIndex<TKey> *mOrderIndex);

	unsigned int FindNextRelevantMbr(unsigned int currentOrder, const TKey &ql, const TKey &qh, int currentLevel, cRangeQueryConfig *rqConfig);

	static void InsertTuple(cRTreeNode<TMbr>* node1, cRTreeNode<TMbr>* node2, char* TMbr_mbr, const tNodeIndex& insNodeIndex, cRTreeNodeHeader<TMbr>* nodeHeader);
	inline const cSpaceDescriptor* GetSpaceDescriptor() const;

	void ScanNode(unsigned int level, unsigned int treeHeight, int& itemOrder, sBatchRQ* batchRQ, cItemStream<TKey> *resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey>* rqBuffers, cQueryProcStat *QueryProcStat);
	void ScanNode_Batch(int& itemOrder, unsigned int level, unsigned int treeHeight, sBatchRQ* batchRQ, cItemStream<TKey> *resultSet,
		cArray<unsigned int> *qrs, cArray<unsigned int> *nqrs, cRangeQueryConfig *rqConfig, cRQBuffers<TKey> *rqBuffers, cQueryProcStat *QueryProcStat,
		unsigned int* resultSizes);
	void ScanNode_Cartesian(int& itemOrder, sBatchRQ* batchRQ, cArray<uint> **aqis, cArray<uint> **naqis, cRangeQueryConfig *rqConfig);

	void DBFS_EnqueueNode(unsigned int nodeIndex,unsigned int level,sBatchRQ *batchRQ, cItemStream<TKey> *resultSet,cRangeQueryConfig *rqConfig, cRQBuffers<TKey>* rqBuffers, cQueryProcStat *QueryProcStat);

	void DeleteScan(int& itemOrder, const TKey& item);
	void ModifyMbr(char* pMbr) const;

#ifdef CUDA_ENABLED
	inline void TransferInnerNodeToGpu();
	inline void SerializeKeys(uint* mbr, uint* children);
	inline void SerializeKeys(uint* mbr);
#endif

	static void InsertTupleOrder(cRTreeNode<TMbr>* node1, cRTreeNode<TMbr>* node2, char* TMbr_mbr, const tNodeIndex& insNodeIndex, cRTreeNodeHeader<TMbr>* nodeHeader);
	bool IsOrderedFirstTuple(cRTreeOrderIndex<TKey> *mOrderIndex);
	void Print2File(FILE *streamInfo, uint order, bool relevant);

};

template<class TMbr>
void cRTreeNode<TMbr>::Print2File(FILE *streamInfo, uint order, bool relevant)
{
	if (relevant)
		fprintf(streamInfo, "rel:  ");
	TKey::Print2File(streamInfo, TMbr::GetLoTuple(this->GetCKey(order)), "x", this->GetSpaceDescriptor());
	TKey::Print2File(streamInfo, TMbr::GetHiTuple(this->GetCKey(order), this->GetSpaceDescriptor()), "\n", this->GetSpaceDescriptor());
}

template<class TMbr>
unsigned int cRTreeNode<TMbr>::FIND_MBR_COUNT = 0;

template<class TMbr>
unsigned int cRTreeNode<TMbr>::FIND_MBR_ISINMBR_COUNT = 0;

template<class TMbr>
unsigned int cRTreeNode<TMbr>::FIND_MBR_ISINMBR_COUNT_0 = 0;

template<class TMbr>
unsigned int cRTreeNode<TMbr>::FIND_MBR_ISINMBR_COUNT_1 = 0;

template<class TMbr>
int cRTreeNode<TMbr>::TMP_FindNextRelevantMbr_COUNT = 0;

template<class TMbr>
unsigned int cRTreeNode<TMbr>::II_Compares = 0;

/**
 * This contructor is used in the case when the node is created on the pool, this pool creates a memory
 * and the node uses it.
 */
template<class TMbr>
cRTreeNode<TMbr>::cRTreeNode(const cTreeNodeHeader* header, const char* mem):  parent(header, mem)
{
}

template<class TMbr>
cRTreeNode<TMbr>::cRTreeNode(const cRTreeNode<TMbr>* origNode, const char* mem): parent(origNode, mem)
{
}

template<class TMbr>
cRTreeNode<TMbr>::cRTreeNode():cCommonRTreeNode<TMbr>() { }

template<class TMbr>
void cRTreeNode<TMbr>::ScanNode(unsigned int level, unsigned int treeHeight, int& itemOrder, sBatchRQ* batchRQ, cItemStream<TKey> *resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey>* rqBuffers, cQueryProcStat *QueryProcStat)
{
	if (batchRQ->mode == QueryType::BATCHQUERY)
	{
		if (rqConfig->GetSearchMethod() == cRangeQueryConfig::SEARCH_DBFS || rqConfig->GetSearchMethod() == cRangeQueryConfig::SEARCH_BFS)
		{
			//ScanNode_Batch(itemOrder, level, treeHeight, batchRQ, resultSet, rqBuffers->qrs[level], rqBuffers->qrs[level + 1], rqConfig, rqBuffers, QueryProcStat, rqBuffers->resultSizes);
			ScanNode_Batch(itemOrder, level, treeHeight, batchRQ, resultSet, rqBuffers->qrs[0], rqBuffers->qrs[1], rqConfig, rqBuffers, QueryProcStat, rqBuffers->resultSizes);
		}
		else
		{
			ScanNode_Batch(itemOrder, level, treeHeight, batchRQ, resultSet, rqBuffers->qrs[level], rqBuffers->qrs[level + 1], rqConfig, rqBuffers, QueryProcStat, rqBuffers->resultSizes);
		}
	}
	else if (batchRQ->mode == QueryType::CARTESIANQUERY) // Cartesian RQ
	{
		ScanNode_Cartesian(itemOrder, batchRQ, rqBuffers->aqis[level], rqBuffers->aqis[level + 1], rqConfig);
	}
}

template<class TMbr>
void cRTreeNode<TMbr>::ScanNode_Batch(int& itemOrder, uint level, uint treeHeight, sBatchRQ* batchRQ, cItemStream<TKey> *resultSet,
	cArray<uint> *qrs, cArray<uint> *nqrs, cRangeQueryConfig *rqConfig, cRQBuffers<TKey> *rqBuffers, cQueryProcStat *QueryProcStat, 
	uint* resultSizes)
{
	uint nodeIndex;
	uint invLevel;
	bool isIntersected = false;
	uint qrsCount = qrs->Count();

	cRTreeNodeHeader<TMbr>* header = (cRTreeNodeHeader<TMbr>*)parent::mHeader;
	const cSpaceDescriptor* sd = GetSpaceDescriptor();

	nqrs->ClearCount();
	int itemCount = (int)parent::GetItemCount();
	for (itemOrder++; itemOrder < itemCount; itemOrder++)
	{
		for (uint j = 0; j < qrsCount; j++)
		{
			uint ind = qrs->GetRefItem(j);

			if (rqConfig->GetFinalResultSize() != cRangeQueryConfig::FINAL_RESULTSIZE_UNDEFINED && rqConfig->GetFinalResultSize() <= resultSizes[ind])
			{
				if (qrsCount == 1)
				{
					// it is only a simple optimization, it should be solved for general qrsCount by
					// removing irrelevant range queries in qrs
					itemOrder = itemCount;
				}
				continue;  // the finalResultSize for this query has been already reached, do not process this query
			}

			if (TMbr::IsIntersected(parent::GetCKey(itemOrder), batchRQ->qls[ind], batchRQ->qhs[ind], sd))
			{
				nodeIndex = parent::GetNodeIndex(parent::GetLink(itemOrder));
				invLevel = treeHeight - (level + 1);

				if (rqConfig->GetSearchMethod() == cRangeQueryConfig::SEARCH_DBFS || rqConfig->GetSearchMethod() == cRangeQueryConfig::SEARCH_BFS)
				{
					DBFS_EnqueueNode(nodeIndex, level + 1, batchRQ, resultSet, rqConfig, rqBuffers, QueryProcStat);
				}
				else
				{
					bool isMatched = true;
					if (header->GetSignatureIndex() != NULL && header->GetSignatureIndex()->IsEnabled(invLevel) &&
						!header->GetSignatureIndex()->IsMatched(nodeIndex, invLevel, QueryProcStat, rqBuffers))
					{
						isMatched = false;
					}

					if (isMatched)
					{
						nqrs->Add(ind);
						isIntersected = true;
					}
				}
			}
		}
		if (isIntersected)
		{
			break;
		}
	}
}

template<class TMbr>
void cRTreeNode<TMbr>::DeleteScan(int& itemOrder, const TKey& item)
{
	uint nodeIndex;
	bool isIntersected = false;

	cRTreeNodeHeader<TMbr>* header = (cRTreeNodeHeader<TMbr>*)parent::mHeader;
	const cSpaceDescriptor* sd = GetSpaceDescriptor();

	int itemCount = (int) parent::GetItemCount();
	for (itemOrder++; itemOrder < itemCount; itemOrder++)
	{
		if (TMbr::IsIntersected(parent::GetCKey(itemOrder), item, item, sd))
		{
			break;
		}
	}
}

template<class TMbr>
void cRTreeNode<TMbr>::DBFS_EnqueueNode(unsigned int nodeIndex, unsigned int level, sBatchRQ *batchRQ, cItemStream<TKey> *resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey>* rqBuffers, cQueryProcStat *QueryProcStat)
{
	cDbfsLevel* dbfsArray = rqBuffers->GetBreadthSearchArray(rqConfig, level);
	dbfsArray->Add(nodeIndex);
}

template<class TMbr>
void cRTreeNode<TMbr>::ScanNode_Cartesian(int& itemOrder, sBatchRQ* batchRQ,
	cArray<uint> **aqis, cArray<uint> **naqis, cRangeQueryConfig *rqConfig)
{
	const cSpaceDescriptor* sd = GetSpaceDescriptor();
	uint dim = sd->GetDimension();
	bool isIntersected;

	int itemCount = (int)parent::GetItemCount();
	for (itemOrder++; itemOrder < itemCount; itemOrder++)
	{
		const char* mbr = parent::GetCKey(itemOrder);

		for (uint j = 0; j < dim; j++)
		{
			cArray<uint> *qis = aqis[j];
			uint qisCount = qis->Count();
			cArray<uint> *nqis = naqis[j];
			nqis->ClearCount();
			isIntersected = false;  // (... OR ...) AND (... OR ...) ...

			char *ql = batchRQ->ql->GetNTuple(j, batchRQ->sd);
			char *qh = batchRQ->qh->GetNTuple(j, batchRQ->sd);
			cSpaceDescriptor *ntSD = batchRQ->sd->GetDimSpaceDescriptor(j);
			assert(batchRQ->sd->GetDimensionTypeCode(j) == cLNTuple::CODE);

			// temp print
			//const char* mbr = node->GetCKey(itemOrder);
			//cMBRectangle<TKey>::Print(mbr, "\n", sd);
			//cLNTuple::Print(ql, "\n", ntSD);
			//cLNTuple::Print(qh, "\n", ntSD);
			// -----

			for (uint k = 0; k < qisCount; k++)
			{
				uint ind = qis->GetRefItem(k);
				if (TMbr::IsIntersected(mbr, j, sd, ql, qh, ind, ntSD))
				{
					nqis->Add(ind);
					isIntersected = true;
				}
			}
			if (!isIntersected)
			{
				break;
			}
		}
		if (isIntersected)
		{
			break;
		}
	}
}


/**
 * Update the item (some children have been updated).
 */
template <class TMbr>
void cRTreeNode<TMbr>::UpdateMbr(unsigned int itemOrder, const char* TMbr_item1)
{
	const cSpaceDescriptor *sd = GetSpaceDescriptor();
	parent::UpdateItem(itemOrder, TMbr_item1, sd);
}

template<class TMbr>
inline const cSpaceDescriptor* cRTreeNode<TMbr>::GetSpaceDescriptor() const
{
	return (cSpaceDescriptor*)((cRTreeNodeHeader<TMbr>*)parent::mHeader)->GetKeyDescriptor();
}

/**
 * Split node into this and newNode.
 */
template<class TMbr>
void cRTreeNode<TMbr>::Split(cRTreeNode &newNode)
{
	if (parent::GetNodeHeader()->GetOrderingEnabled())
	{
		SplitCommon(newNode);
	}
	else
	{
		SplitDisMbrs(newNode);
	}
}

/// Split node into this and newNode.
template<class TMbr>
void cRTreeNode<TMbr>::SplitCommon(cRTreeNode &newNode)
{
	unsigned int order1 = 0, order2 = 0;
	cMemoryPool *pool = parent::GetNodeHeader()->GetMemoryPool();
	char* cRTNode_mem = pool->GetMem(parent::GetNodeHeader()->GetNodeInMemSize());
	cRTreeNode tmpNode(this, cRTNode_mem);
	tmpNode.Init();
	tmpNode.SetLeaf(false);
	char* TMbr_mbr = pool->GetMem(parent::GetNodeHeader()->GetKeySize());
	const cSpaceDescriptor *sd = GetSpaceDescriptor();

	tmpNode.parent::Clear();
	newNode.parent::Clear();

	for (unsigned int i = 0 ; i < parent::mItemCount ; i++)
	{
		if (i < parent::mItemCount/2)
		{
			tmpNode.AddItem(parent::GetCKey(i), parent::GetLink(i), false);
			order1++;
		}
		else
		{
			newNode.AddItem(parent::GetCKey(i), parent::GetLink(i), false);
			order2++;
		}
	}

	parent::Clear();

	for (unsigned int i = 0 ; i < order1 ; i++)
	{
		parent::AddItem(tmpNode.GetCKey(i), tmpNode.parent::GetLink(i) , false);
	}

	// free temporary variables
	tmpNode.SetData(NULL);
	pool->FreeMem(TMbr_mbr);
	pool->FreeMem(cRTNode_mem);

	parent::mHeader->IncrementNodeCount();
}

/// Split node into this and newNode.
template<class TMbr>
void cRTreeNode<TMbr>::SplitDisMbrs(cRTreeNode &newNode)
{
	unsigned int order1 = 0, order2 = 0;
	cMemoryPool *pool = parent::GetNodeHeader()->GetMemoryPool();
	char* cRTNode_mem = pool->GetMem(parent::GetNodeHeader()->GetNodeInMemSize());
	cRTreeNode tmpNode(this, cRTNode_mem);
	tmpNode.Init();
	tmpNode.SetLeaf(false);
	char* TMbr_mbr = pool->GetMem(parent::GetNodeHeader()->GetKeySize());
	const cSpaceDescriptor *sd = GetSpaceDescriptor();

	Find2DisjMbrs(TMbr_mbr, -1);

	tmpNode.parent::Clear();//ItemOrder();
	newNode.parent::Clear();//ItemOrder();

	for (unsigned int i = 0 ; i < parent::mItemCount ; i++)
	{
		if (TMbr::IsContained(TMbr_mbr, parent::GetCKey(i), sd))
		{
			tmpNode.AddItem(parent::GetCKey(i), parent::GetLink(i), false);
			order1++;
		}
		else
		{
			newNode.AddItem(parent::GetCKey(i), parent::GetLink(i), false);
			order2++;
		}
	}

	if (order1 == 0 || order2 == 0)
	{
		if (parent::GetNodeHeader()->DuplicatesAllowed() && order2 == 0)
		{
			for (unsigned int i = parent::mItemCount / 2; i < parent::mItemCount; i++)
			{
				newNode.SetItemPOrder(order2, order2);
				newNode.SetKey(order2, parent::GetCKey(i));  //fk cFCNode::parent::GetKey => cTreeNode::parent::GetKey
				newNode.SetLink(order2, parent::GetLink(i));
				order2++;
			}			
			order1 = parent::mItemCount - order2;
		} else
		{
			printf("Critical Error: cRTreeNode::SplitDisMbrs():one new node has only 0 items! (%d,%d)", order1, order2);
			// ql->Print(" - ql\n");
			// qh->Print(" - qh\n");
			printf("New node:\n");
			newNode.Print(sd);
			printf("This node:\n");
			parent::Print(sd);
			exit(0);
		}
	}

	parent::Clear();

	for (unsigned int i = 0 ; i < order1 ; i++)
	{
		parent::AddItem(tmpNode.GetCKey(i), tmpNode.parent::GetLink(i) , false);
	}

	// free temporary variables
	tmpNode.SetData(NULL);
	pool->FreeMem(TMbr_mbr);
	pool->FreeMem(cRTNode_mem);

	parent::mHeader->IncrementNodeCount();
}

/// Create the MBR of mbrs in the node.
template<class TMbr>
void cRTreeNode<TMbr>::CreateMbr(unsigned int startOrder, unsigned int finishOrder, char* pTMbr, cNodeBuffers<TMbr>* buffers) const
{
	const cSpaceDescriptor *sd =  GetSpaceDescriptor();
	sItemBuffers* itemBuffer = &buffers->itemBuffer;

	TMbr::Copy(pTMbr, parent::GetCKey(startOrder, itemBuffer), sd);  

	for (unsigned int i = startOrder+1 ; i <= finishOrder ; i++)
	{
		TMbr::ModifyMbrByMbr(pTMbr, parent::GetCKey(i, itemBuffer), sd); //parent::GetCItem->GetCKey
	}
}

/// Create the MBR of mbrs in the node.
template<class TMbr>
void cRTreeNode<TMbr>::CreateMbr(char* pTMbr) const
{
	//CreateMbr(0, parent::mItemCount-1, pTMbr);

	const cSpaceDescriptor *sd =  GetSpaceDescriptor();

	TMbr::Copy(pTMbr, parent::GetCKey(0), sd);  //parent::GetCItem->GetCKey

	for (unsigned int i = 1 ; i <= parent::mItemCount-1 ; i++)
	{
		TMbr::ModifyMbrByMbr(pTMbr, parent::GetCKey(i), sd);  //parent::GetCItem->GetCKey
	}
}

/// Modify MBR by the children items in the case of delete 
template<class TMbr>
void cRTreeNode<TMbr>::ModifyMbr(char* pMbr) const
{
	const cSpaceDescriptor *sd = GetSpaceDescriptor();

	TMbr::Copy(pMbr, parent::GetCKey(0), sd);  

	for (uint i = 1; i <= parent::mItemCount - 1; i++)
	{
		TMbr::ModifyMbrByMbr(pMbr, parent::GetCKey(i), sd);
	}
}

/* Find MBB containg the tuple.
 *  \param tuple Tuple being inserted.
 *  \param itemOrder Parameter by reference. Return order of the item in the node.
 *  \return
 *		- true if the MBR (mItems) in this node were modified,
 *		- false otherwise.
 * ISIN_RECTANGLE
 * ISIN_MODE_RECTANGLES
 * RECTANGLE_MODIFIED
 */
template<class TMbr>
bool cRTreeNode<TMbr>::FindMbr(const TKey &tuple, unsigned int &itemOrder)
{
	if (cRTreeConst::Find_MBR == Find::MINMAX_TAXIDIST)
	{
		return FindMbr_MinMaxTaxiDist(tuple, itemOrder);
	}

	const cSpaceDescriptor *sd = GetSpaceDescriptor();
	bool ret = true, isinf = false;

	for (unsigned int i = 0 ; i < parent::mItemCount ; i++)
	{
		if (TMbr::IsInRectangle(parent::GetCKey(i), tuple.GetData(), sd))
		{
			if (!isinf)
			{
				itemOrder = i;
				isinf = true;//false;
				break;
			}
		}
	}

	if (!isinf)
	{
		switch (cRTreeConst::Find_MBR)
		{
			case Find::MINIMAL_VOLUME:       ret = FindMbr_MinimalVolume(tuple, itemOrder); break;
			case Find::MINIMAL_INTERSECTION: ret = FindMbr_MinimalIntrVolume(tuple, itemOrder); break;
		}
	}

	return ret;
}

/* Find MBB containg the tuple.
 *  \param tuple Tuple being inserted.
 *  \param itemOrder Parameter by reference. Return order of the item in the node.
 *  \return
 *     - FINDMBR_OK
 *     - FINDMBR_MORE
 *     - (RECTANGLE_MODIFIED)
 *     - FINDMBR_NONE
 */
template<class TMbr>
int cRTreeNode<TMbr>::FindMbr_MP(const TKey &tuple, cStack<sItemIdRecord>& curPathStack, cStack<sItemIdRecord>& mbrHitStack)
{
	/*
	const cSpaceDescriptor *sd = GetSpaceDescriptor();
	int ret = FINDMBR_NONE;
	unsigned int mbrHit = 0;
	bool debug = false, firstHit = false;
	ItemIdRecord itemIdRec;
	itemIdRec.Level = curPathStack.Count(); // set the level of the current node's children
	itemIdRec.ItemOrder = -1;

	for (unsigned int i = 0 ; i < parent::mItemCount ; i++)
	{
		if (TMbr::IsInRectangle(parent::GetCItem(i), tuple.GetData(), sd))
		{
			mbrHit++;

			itemIdRec.NodeIndex = parent::GetLink(i);
			itemIdRec.ParentItemOrder = i;

			if (!firstHit)
			{
				curPathStack.TopRef()->ItemOrder = i;
				curPathStack.Push(itemIdRec);
				firstHit = true;
				ret = FINDMBR_OK;
			}
			else
			{
				mbrHitStack.Push(itemIdRec);
			}
		}
	}
	return ret;
	 */
}

/* Find MBR more relevant to the tuple. Since there is not MBR cotaining the tuple
 * you must must modify a more relevant MBR (there are some heuristics:
 *   choose MBR with the minimal volume, choose MBR with the minimal volume
 *   of an intersection with another MBR).
 */
template<class TMbr>
unsigned int cRTreeNode<TMbr>::FindModifyMbr_MP(const TKey &tuple)
{
  unsigned int itemOrder;

	switch (cRTreeConst::Find_MBR)
	{
		case Find::MINIMAL_VOLUME:       
			itemOrder = FindMinimalVolume_MP(tuple);
			break;
		default: /* Find::MINIMAL_INTERSECTION: */ 
			itemOrder = FindMinimalIntrVolume_MP(tuple);
			break;
	}
	return itemOrder;
}

/// Find the best MBR(MBR with minimal volume increase) for tuple 
/// \param tuple Tuple being inserted.
/// \param itemOrder Parameter by reference. Return order of the item in the node.
/// \return
///		- true if the MBR (mItems) in this node were modified (and tuple matches the MBR),
///		- false otherwise.

/*
  Results (1mil. randomly generated points, 3D)
  V1: intersections are checked
  V2: only minimal enhancement is found

  Insert (10^3 i/s): V1: 44.4, V2: 54.4

  Query (10^3 q/s): V1: 81.5, V2: 90.1
  LAR: 
  - V1: 
    #Logical Access Read: 3.44 (#IN: 2.22, #LN: 1.21, #Rel. LN: 1.00)
    #Compare: 705.47
  - V2: 
    #Logical Access Read: 3.46 (#IN: 2.20, #LN: 1.26, #Rel. LN: 1.00)
    #Compare: 701.80

  However for dim=4 a BlockSize=8192 V1 outperforms V2.
*/
template<class TMbr>
bool cRTreeNode<TMbr>::FindMbr_MinimalVolume(const TKey &tuple, unsigned int &itemOrder)
{
	bool ret = true, disjmbrf = false;
	const cSpaceDescriptor *sd = GetSpaceDescriptor();
	cMemoryPool *pool = parent::GetNodeHeader()->GetMemoryPool();
	const unsigned int mbrInMemSize = parent::GetNodeHeader()->GetKeySize();
	double volumeBefore, volumeAfter, minVolumeDiffNonDisj = DBL_MAX, minVolumeDiffDisj = DBL_MAX;
	unsigned int mindiffMbrOrderNonDisj = UINT_MAX, mindiffMbrOrderDisj = UINT_MAX;
	char* cMbr_mbr = pool->GetMem(mbrInMemSize);
	char* cMbr_tmp = pool->GetMem(mbrInMemSize);
	const unsigned int V1 = 0;
	const unsigned int V2 = 1;
	const unsigned int variant = V2;

	for (unsigned int i = 0 ; i < parent::mItemCount ; i++)
	{
		TMbr::Copy(cMbr_mbr, parent::GetCKey(i), sd);
		volumeBefore = TMbr::Volume(cMbr_mbr, sd);
		TMbr::ModifyMbr(cMbr_mbr, tuple.GetData(), sd);
		volumeAfter = TMbr::Volume(cMbr_mbr, sd);
		double volumeDiff = volumeAfter - volumeBefore;
		bool intersectedf = false;

		if (variant == V1)
		{
			if (volumeDiff < minVolumeDiffNonDisj)
			{
				// begin: now you must check if the new rectangle does not intersect other rectangles
				for (unsigned int j = 0 ; j < parent::mItemCount ; j++)
				{
					if (j != i)
					{
						if (TMbr::IsIntersected(parent::GetCKey(j), cMbr_mbr, sd))
						{
							// in my opinion, it is not necessary to scan all rectangles, can I use a {min,max} distance for it?
							intersectedf = true;
							break;
						}
					}
				}

				if (!intersectedf)
				{
					// if the new rectangle does not interstect other mbrs, write it
					minVolumeDiffNonDisj = volumeDiff;
					mindiffMbrOrderNonDisj = i;
					disjmbrf = true;
				}
			}
		}

		// The second variant: only the minimal enlargement is considered
		// there are not disjunctive rectangles? choose the rectangle with the minimal volume difference
		if (variant == V1)
		{
			if (!disjmbrf & intersectedf & (volumeDiff < minVolumeDiffDisj))
			{
				minVolumeDiffDisj = volumeDiff;
				mindiffMbrOrderDisj = i;
			}
		} else
		{
			if (volumeDiff < minVolumeDiffDisj)
			{
				minVolumeDiffDisj = volumeDiff;
				itemOrder = i;
			}
		}
	}

	if (variant == V1)
	{
		if (!disjmbrf)
		{
			itemOrder = mindiffMbrOrderDisj;
		} else
		{
			itemOrder = mindiffMbrOrderNonDisj;
		}
	}

	//TMbr::ModifyMbr(GetItem(itemOrder), tuple.GetData(), sd);
	parent::UpdateMbr(itemOrder, tuple.GetData(), sd);

	pool->FreeMem(cMbr_mbr);
	pool->FreeMem(cMbr_tmp);

	return true;
}

template<class TMbr>
bool cRTreeNode<TMbr>::FindMbr_MinMaxTaxiDist(const TKey &tuple, unsigned int &itemOrder)
{
	unsigned int minDistanceToSide = UINT_MAX;
	unsigned int minDistanceToCentre = UINT_MAX;
	bool isInMbr;
	unsigned int d;
	const cSpaceDescriptor *sd = GetSpaceDescriptor();
	bool verbose = true;

	if (parent::mDebug)
	{
		TKey::Print(tuple.GetData(), "\n", sd);
		parent::Print(sd);
	}

	int count = 0;
	bool find = false;

	for (unsigned int i = 0 ; i < parent::mItemCount ; i++)
	{
		d = TMbr::DistanceToSide(parent::GetCKey(i), tuple.GetData(), sd, isInMbr, count > 0, minDistanceToSide);

		if (isInMbr)
		{
			unsigned int d2Centre = TMbr::DistanceToCentre(parent::GetCKey(i), tuple.GetData(), sd, minDistanceToCentre);
			if (d2Centre < minDistanceToCentre)
			{
				minDistanceToCentre = d2Centre;
				itemOrder = i;
			}

			count++;

			if (!verbose)
			{
				break;
			}
		} 
		else if (count == 0)
		{
			if (d < minDistanceToSide)
			{
				minDistanceToSide = d;
				itemOrder = i;
			}
		}
	}

	if (verbose)
	{
		FIND_MBR_COUNT++;
		FIND_MBR_ISINMBR_COUNT += count;
		if (count == 0)
		{
			FIND_MBR_ISINMBR_COUNT_0++;
		}
		else if (count == 1)
		{
			FIND_MBR_ISINMBR_COUNT_1++;
		}
		//printf("c: %.2f, a: %d, 0: %d, 1: %d					\r", (float)FIND_MBR_ISINMBR_COUNT/FIND_MBR_COUNT,
		//	FIND_MBR_ISINMBR_COUNT, FIND_MBR_ISINMBR_COUNT_0, FIND_MBR_ISINMBR_COUNT_1);
	}

	parent::UpdateMbr(itemOrder, tuple.GetData(), sd);

	if (count > 1)
	{
		int bla = 0;
	}

	return true;
}

template<class TMbr>
unsigned int cRTreeNode<TMbr>::FindMbr_MinimalVolume_MP(const TKey &tuple)
{
	bool disjmbrf = false;
	const cSpaceDescriptor *sd = GetSpaceDescriptor();
	cMemoryPool *pool = parent::GetNodeHeader()->GetMemoryPool();
	const unsigned int mbrInMemSize = parent::GetNodeHeader()->parent::GetKeySize();
	double volumeBefore, volumeAfter, minVolumeDiffNonDisj = DBL_MAX, minVolumeDiffDisj = DBL_MAX;
  unsigned int mindiffMbrOrderNonDisj = UINT_MAX, mindiffMbrOrderDisj = UINT_MAX;
	char* cMbr_mbr = pool->GetMem(mbrInMemSize);
	char* cMbr_tmp = pool->GetMem(mbrInMemSize);
	unsigned int itemOrder;

	for (unsigned int i = 0 ; i < parent::mItemCount ; i++)
	{
		TMbr::Copy(cMbr_mbr, parent::GetCItem(i), sd);
		volumeBefore = TMbr::Volume(cMbr_mbr, sd);
		TMbr::ModifyMbr(cMbr_mbr, tuple.GetData(), sd);
		volumeAfter = TMbr::Volume(cMbr_mbr, sd);
		double volumeDiff = volumeAfter - volumeBefore;
		bool intersectedf = false;

		if (volumeDiff < minVolumeDiffNonDisj)
		{
			// begin: now you must check if the new rectangle does not intersect other rectangles
			for (unsigned int j = 0 ; j < parent::mItemCount ; j++)
			{
				if (j != i)
				{
					if (TMbr::IsIntersected(parent::GetCItem(j), cMbr_mbr, sd))
					{
						// in my opinion, it is not necessary to scan all rectangles, can I use a {min,max} distance for it?
						intersectedf = true;
						break;
					}
				}
			}

			if (!intersectedf)
			{
				// if the new rectangle does not interstect other mbrs, write it
				minVolumeDiffNonDisj = volumeDiff;
				mindiffMbrOrderNonDisj = i;
				disjmbrf = true;
			}
		}

		// there are not disjunctive rectangles? choose the rectangle with the minimal volume difference
		if (!disjmbrf & intersectedf & volumeDiff < minVolumeDiffDisj)
		{
				minVolumeDiffDisj = volumeDiff;
				mindiffMbrOrderDisj = i;
		}
	}

	if (!disjmbrf)
	{
		itemOrder = mindiffMbrOrderDisj;
	}
	else
	{
		itemOrder = mindiffMbrOrderNonDisj;
	}

	//TMbr::ModifyMbr(GetItem(itemOrder), tuple.GetData(), sd);
	parent::UpdateMbr(itemOrder, tuple.GetData(), sd);

	pool->FreeMem(cMbr_mbr);
	pool->FreeMem(cMbr_tmp);

	return itemOrder;
}

/// Find the best MBR(MBR with minimal intersection volume) for tuple 
/// \param tuple Tuple being inserted.
/// \param itemOrder Parameter by reference. Return order of the item in the node.
/// \return
///		- true if the MBR (mItems) in this node were modified,
///		- false otherwise.
template<class TMbr>
bool cRTreeNode<TMbr>::FindMbr_MinimalIntrVolume(const TKey &tuple, unsigned int &itemOrder)
{
	bool ret = true, disjmbrf = false, interf;
	double volume, minVolume = DBL_MAX, intersectionVolume, minIntersectionVolume = DBL_MAX;
	cMemoryPool *pool = parent::GetNodeHeader()->GetMemoryPool();
	const unsigned int mbrInMemSize = parent::GetNodeHeader()->GetKeySize();
	const cSpaceDescriptor *sd = GetSpaceDescriptor();

	char* cMbr_mbr = pool->GetMem(mbrInMemSize);
	char* cMbr_tmp = pool->GetMem(mbrInMemSize);

	for (unsigned int i = 0 ; i < parent::mItemCount ; i++)
	{
		TMbr::Copy(cMbr_mbr, parent::GetCItem(i), sd);
		TMbr::ModifyMbr(cMbr_mbr, tuple.GetData(), sd);
		interf = true;

		intersectionVolume = 0.0;  // reset volume

		for (unsigned int j = 0 ; j < parent::mItemCount ; j++)
		{
			if (j != i)
			{
				if (TMbr::IsIntersected(cMbr_mbr, parent::GetCKey(j), sd))  //fk cFCNode::parent::GetKey => cTreeNode::parent::GetKey
				{
					interf = false;
					if (!disjmbrf)
					{
						// if disjunctive MBRs were not find, use MBRs with minimal intersection
						intersectionVolume += TMbr::IntersectionVolume(cMbr_mbr, parent::GetCKey(j), sd);  //fk cFCNode::parent::GetKey => cTreeNode::parent::GetKey
					}
				}
			}
		}

		if (interf)   // disjunctive mbrs finded?
		{
			disjmbrf = true;
			volume = TMbr::Volume(cMbr_mbr, sd);
			if (volume < minVolume)
			{
				minVolume = volume;
				TMbr::Copy(cMbr_tmp, cMbr_mbr, sd);
				itemOrder = i;
			}
		}
		else if (!disjmbrf)
		{
			// if disjunctive MBRs will not find use MBRs with minimal intersection
			if (intersectionVolume < minIntersectionVolume)
			{
				minIntersectionVolume = intersectionVolume;
				TMbr::Copy(cMbr_tmp, cMbr_mbr, sd);
				itemOrder = i;
			}
		}
	}

	if (!disjmbrf)
	{
		// old parent::mHeader->GetQueryStatistics()->GetCounter(cRTreeConst::Counter_intersectMBRs)->Increment();
	}

	//TMbr::Copy(GetItem(itemOrder), cMbr_tmp, sd);
	parent::UpdateItem(itemOrder, cMbr_tmp, sd);

	// GetItem(itemOrder)->SetTuple(cTuple_tmpQl);
	// GetItem(itemOrder)->SetSecondTuple(cTuple_tmpQh);

	pool->FreeMem(cMbr_mbr);
	pool->FreeMem(cMbr_tmp);

	return true;
}

template<class TMbr>
unsigned int cRTreeNode<TMbr>::FindMbr_MinimalIntrVolume_MP(const TKey &tuple)
{
	bool disjmbrf = false, interf;
	double volume, minVolume = DBL_MAX, intersectionVolume, minIntersectionVolume = DBL_MAX;
	cMemoryPool *pool = parent::GetNodeHeader()->GetMemoryPool();
	const unsigned int mbrInMemSize = parent::GetNodeHeader()->parent::GetKeySize();
	const cSpaceDescriptor *sd = GetSpaceDescriptor();
	unsigned int itemOrder;

	char* cMbr_mbr = pool->GetMem(mbrInMemSize);
	char* cMbr_tmp = pool->GetMem(mbrInMemSize);

	for (unsigned int i = 0 ; i < parent::mItemCount ; i++)
	{
		TMbr::Copy(cMbr_mbr, parent::GetCItem(i), sd);

		TMbr::ModifyMbr(cMbr_mbr, tuple.GetData(), sd);
		// tuple.ModifyMbr(mbr, GetSpaceDescriptor());
		interf = true;

		intersectionVolume = 0.0;  // reset volume

		for (unsigned int j = 0 ; j < parent::mItemCount ; j++)
		{
			if (j != i)
			{
				if (TMbr::IsIntersected(cMbr_mbr, parent::GetKey(j), sd))  //fk cFCNode::parent::GetKey => cTreeNode::parent::GetKey
				{
					interf = false;
					if (!disjmbrf)
					{
						// if disjunctive MBRs were not find, use MBRs with minimal intersection
						intersectionVolume += TMbr::IntersectionVolume(cMbr_mbr, parent::GetKey(j), sd);  //fk cFCNode::parent::GetKey => cTreeNode::parent::GetKey
					}
				}
			}
		}

		if (interf)   // disjunctive mbrs finded?
		{
			disjmbrf = true;
			volume = TMbr::Volume(cMbr_mbr, sd);
			if (volume < minVolume)
			{
				minVolume = volume;
				TMbr::Copy(cMbr_tmp, cMbr_mbr, sd);
				itemOrder = i;
			}
		}
		else if (!disjmbrf)
		{
			// if disjunctive MBRs will not find use MBRs with minimal intersection
			if (intersectionVolume < minIntersectionVolume)
			{
				minIntersectionVolume = intersectionVolume;
				TMbr::Copy(cMbr_tmp, cMbr_mbr, sd);
				itemOrder = i;
			}
		}
	}

	if (!disjmbrf)
	{
		// old parent::mHeader->GetQueryStatistics()->GetCounter(cRTreeConst::Counter_intersectMBRs)->Increment();
	}

	//TMbr::Copy(GetItem(itemOrder), cMbr_tmp, sd);
	parent::UpdateItem(itemOrder, cMbr_tmp, sd);

	// GetItem(itemOrder)->SetTuple(cTuple_tmpQl);
	// GetItem(itemOrder)->SetSecondTuple(cTuple_tmpQh);

	pool->FreeMem(cMbr_mbr);
	pool->FreeMem(cMbr_tmp);

	return itemOrder;
}

/// Find item in the node where the new tuple belongs. 
/// This method if variant of a FindMbr() function for an ordered R-tree.
/// \param tuple Tuple being inserted.
/// \param itemOrder Parameter by reference. Return order of the item in the node.
/// \return
///		- true if the MBR (mItems) in this node were modified,
///		- false otherwise.
template<class TMbr>
bool cRTreeNode<TMbr>::FindMbr_Ordered(const TKey &tuple, unsigned int &itemOrder, const cRTreeOrderIndex<TKey> *mOrderIndex)
{
	int mid = 0;
	int lo = 0;
	int hi = parent::mItemCount - 1;
	int ret;

	if (tuple.Equal(*(mOrderIndex->GetTuple(parent::GetIndex(), GetSpaceDescriptor())), GetSpaceDescriptor()) > 0)
	{
		do
		{
			mid = (lo + hi) / 2;
			tNodeIndex tmpNodeIndex = (parent::GetLink(mid) & 0x7fffffff);
			if ((ret = tuple.Equal(*(mOrderIndex->GetTuple(tmpNodeIndex, GetSpaceDescriptor())), GetSpaceDescriptor())) == -1)
			{
				hi = mid-1;

				if (lo > hi)
				{
					mid--;
				}
			}
			else
			{
				lo = mid+1;
			}
		}
		while(lo <= hi);
	}

	if (mid >= parent::mItemCount)
	{
		mid = parent::mItemCount - 1;
	}

	//TMbr::Print(parent::GetCKey(mid), "\n", sd);
	//tuple.Print("\n", GetSpaceDescriptor());

	parent::UpdateMbr(mid, tuple.GetData(), GetSpaceDescriptor());

	//TMbr::Print(parent::GetCKey(mid), "\n", sd);
	itemOrder = mid;
	return true;	
}

/// Find two dijunctive mbrs. Find a plane splited space into two half-space with minimal volume
/// of course, the best case is if two nodes have the same number of items
template<class TMbr>
void cRTreeNode<TMbr>::Find2DisjMbrs(char* pTMbr, int minimal_count)
{
	unsigned int state, index, order;
	double volume, minVolume = DBL_MAX, intersectionVolume, minIntersectionVolume = DBL_MAX;
	float diff, itemCount = (float)parent::mItemCount / 2;
	bool findDisj = false;
	const cSpaceDescriptor *sd = GetSpaceDescriptor();

	if (minimal_count < 0)
	{
		minimal_count = (parent::mItemCount / 2) - 2;
	}

	cMemoryPool *pool = parent::GetNodeHeader()->GetMemoryPool();
	const unsigned int mbrInMemSize = parent::GetNodeHeader()->GetKeySize();

	char* TMbr_mbr1 = pool->GetMem(mbrInMemSize);
	char* TMbr_mbr2 = pool->GetMem(mbrInMemSize);

	if (parent::mItemCount > 0)
	{
		TMbr::Copy(pTMbr, parent::GetCKey(0), sd);  //fk cFCNode::parent::GetKey => cTreeNode::parent::GetKey
	}

	// find a plane splited space into two half-space with minimal volume
	// of course, the best case is if two nodes have the same number of items
	if (parent::mItemCount % 2 == 0)
	{
		diff = 0.5;
		itemCount -= diff;
	}
	else
	{
		diff = 0.0;
	}

	unsigned int dim = sd->GetDimension();

	for (unsigned int i = 0 ; i < dim ; i++)
	{
		SortBy(i);
		index = 0;

		if (parent::mItemCount % 2 == 0)
		{
			state = 1;
		}
		else
		{
			state = 0;
		}

		for ( ; ; )
		{
			if (state == 0)
			{
				order = (unsigned int)itemCount;
				state = 1;
				index++;
			}
			else if (state == 1)
			{
				order = (unsigned int)(itemCount - diff - index);
				state = 2;
			}
			else
			{
				order = (unsigned int)(itemCount + diff + index);
				index++;
				state = 1;
			}

			CreateMbr(0, order, TMbr_mbr1);
			CreateMbr(order+1, parent::mItemCount-1, TMbr_mbr2);

			if (parent::mDebug)
			{
				printf("\n\n");
				TMbr::Print(TMbr_mbr1, "\n", sd);
				TMbr::Print(TMbr_mbr2, "\n\n", sd);
			}

			if (!TMbr::IsIntersected(TMbr_mbr1, TMbr_mbr2, sd))
			{
				volume = TMbr::Volume(TMbr_mbr1, sd) + 
					TMbr::Volume(TMbr_mbr2, sd);

				if (volume < minVolume)
				{
					minVolume = volume;
					TMbr::Copy(pTMbr, TMbr_mbr1, sd);
					findDisj = true;
				}
				break; 
			}
			else if (!findDisj)  // find the minimal intersection only if disjunctive MBRs wasn't still found
			{
				// if the disjunctive MBRs will not find, use MBRs with minimal intersection volume
				if (!TMbr::IsContained(TMbr_mbr1, TMbr_mbr2, sd))
				{
					if ((intersectionVolume = TMbr::IntersectionVolume(TMbr_mbr1, TMbr_mbr2, sd)) 
						< minIntersectionVolume)
					{
						minIntersectionVolume = intersectionVolume;
						TMbr::Copy(pTMbr, TMbr_mbr1, sd);
					}
				}
			}

			if (order <= minimal_count)
			{
				break;
			}
		}
	}

	if (!findDisj)
	{
		// old: mTreeHeader->GetQueryStatistics()->GetCounter(cRTreeConst::Counter_intersectMBRs)->Increment();
	}

	/*TMbr::Print(pTMbr, "\n", GetSpaceDescriptor());
	TMbr::Print(TMbr_mbr1, "\n", GetSpaceDescriptor());
	TMbr::Print(TMbr_mbr2, "\n\n", GetSpaceDescriptor());*/

	pool->FreeMem(TMbr_mbr1);
	pool->FreeMem(TMbr_mbr2);
}

/// Sort nodes's tuples according values in dimension.
template<class TMbr>
void cRTreeNode<TMbr>::SortBy(unsigned int dimension)
{
	bool sortedFlag = true;
	const cSpaceDescriptor *sd = GetSpaceDescriptor();

	// check if the sequence is sorted
	for (unsigned int i = 0 ; i < parent::mItemCount-1 ; i++)
	{
		if (TKey::Equal(TMbr::GetHiTuple(parent::GetCKey(i), sd), TMbr::GetHiTuple(parent::GetCKey(i+1), sd), dimension, sd) > 0)
		{
			sortedFlag = false;
			break;
		}
	}

	if (!sortedFlag)
	{
		// select-sort
		unsigned int min;

		for (unsigned int i = 0 ; i < parent::mItemCount ; i++)
		{
			min = i;
			for (unsigned int j = i ; j < parent::mItemCount ; j++)
			{
				if (TKey::Equal(TMbr::GetHiTuple(parent::GetCKey(j), sd), TMbr::GetHiTuple(parent::GetCKey(min), sd), dimension, sd) < 0)
				{
					min = j;
				}
			}
			if (i != min)
			{
				parent::SwapItemOrder(i, min);
			}
		}
	}
}

template<class TMbr>
unsigned int cRTreeNode<TMbr>::FindNextRelevantMbr(unsigned int currentOrder, const TKey &ql, const TKey &qh)
{
	unsigned int ret = RQ_NOINTERSECTION;

	for (unsigned int i = currentOrder ; i < parent::mItemCount ; i++)
	{
		if (parent::GetRefItem(i).IsIntersected(ql, qh))
		{
			ret = i;
			break;
		}
	}
	return ret;
}

/// Return true if the current MBB is relevant to the query box.
template<class TMbr>
bool cRTreeNode<TMbr>::IsRegionRelevant(unsigned int currentOrder, const TKey &ql, const TKey &qh)
{
	return parent::GetRefItem(currentOrder).IsIntersected(ql, qh);
}


template<class TMbr>
unsigned int cRTreeNode<TMbr>::FindNextRelevantMbr(unsigned int currentOrder, const TKey &ql, const TKey &qh, int currentLevel, cRangeQueryConfig *rqConfig)
{
	parent::mDebug = false;
	unsigned int ret = RQ_NOINTERSECTION;

	cRTreeHeader<TKey> *header = (cRTreeHeader<TKey>*)parent::mHeader;
	const cSpaceDescriptor *sd = GetSpaceDescriptor();

	if (parent::mDebug)
	{
		parent::Print(sd);
		ql.Print("\n", sd);
		qh.Print("\n", sd);
	}

	for (unsigned int i = currentOrder ; i < parent::mItemCount ; i++)
	{
		if (TMbr::IsIntersected(parent::GetCKey(i), ql, qh, sd))
		{
			bool sigFlag = true;

			tNodeIndex nodeIndex = parent::GetLink(i);

			if (!parent::IsLeaf(nodeIndex))
			{
				/* mm if (currentLevel == header->GetHeight()-2) // super-leaf level will be processed
				{
					if (!mdSignatureIndex.IsMatched(nodeIndex, ql, qh))
					{
						sigFlag = false;
					}
				}*/
			}
			else if (parent::IsLeaf(nodeIndex))
			{
				tNodeIndex nIndex = parent::GetNodeIndex(nodeIndex);

				// if (signatureEnabled && mdSignatureIndex.IsOpen() && !mdSignatureIndex.IsMatched(nIndex, ql, qh))
				// {
				// 	sigFlag = false;
				// }
				// else
				// {
					sigFlag = true;
				// }
			}

			if (sigFlag)
			{
				ret = i;
				break;
			}
		}
	}

	return ret;
}

/**
 * Insert the tuple into the more more appropriate node.
 * This method is invoced after the split operation.
 * \param cMbr_mbr, insNodeIndex - inserted item of the inner node
 **/
template<class TMbr>
void cRTreeNode<TMbr>::InsertTuple(cRTreeNode<TMbr>* node1, cRTreeNode<TMbr>* node2, 
	char* TMbr_mbr, const tNodeIndex& insNodeIndex /*, char* TMbr_mbr2*/, cRTreeNodeHeader<TMbr>* nodeHeader)
{
	cMemoryPool* pool = nodeHeader->GetMemoryPool();
	const cSpaceDescriptor* sd = nodeHeader->GetSpaceDescriptor();
	char* TMbr_mbr1 = pool->GetMem(nodeHeader->GetKeySize());
	char* TMbr_mbr2 = pool->GetMem(nodeHeader->GetKeySize());
	cRTreeNode<TMbr>* node = NULL;

	node1->CreateMbr(TMbr_mbr1);
	node2->CreateMbr(TMbr_mbr2);

	if (!TMbr::IsContained(TMbr_mbr1, TMbr_mbr, sd))
	{
		if (TMbr::IsContained(TMbr_mbr2, TMbr_mbr, sd))
		{
			node = node2;
		}
	}
	else
	{
		node = node1;
	}

	// unfortunately, mbr is not matched by any of two MBRs
	if (node == NULL)
	{
		double volume1 = TMbr::Volume(TMbr_mbr1, sd);
		double volume2 = TMbr::Volume(TMbr_mbr2, sd);

		TMbr::ModifyMbr(TMbr_mbr1, TMbr_mbr, sd);
		TMbr::ModifyMbr(TMbr_mbr2, TMbr_mbr, sd);

		double volume1_new = TMbr::Volume(TMbr_mbr1, sd);
		double volume2_new = TMbr::Volume(TMbr_mbr2, sd);

		if (volume1_new - volume1 < volume2_new - volume2)
		{
			node = node1;
		}
		else
		{
			node = node2;
		}
	}

	node->AddItem(TMbr_mbr, insNodeIndex, true);

	// free temporary memory
	pool->FreeMem(TMbr_mbr1);
	pool->FreeMem(TMbr_mbr2);
}

/**
 * Insert the tuple into the more more appropriate node.
 * This method is invoced after the split operation.
 * \param cMbr_mbr, insNodeIndex - inserted item of the inner node
 **/
template<class TMbr>
void cRTreeNode<TMbr>::InsertTupleOrder(cRTreeNode<TMbr>* node1, cRTreeNode<TMbr>* node2, 
	char* TMbr_mbr, const tNodeIndex& insNodeIndex /*, char* TMbr_mbr2*/, cRTreeNodeHeader<TMbr>* nodeHeader)
{
	cMemoryPool* pool = nodeHeader->GetMemoryPool();
	const cSpaceDescriptor* sd = nodeHeader->GetSpaceDescriptor();
	char* TMbr_mbr1 = pool->GetMem(nodeHeader->GetKeySize());
	char* TMbr_mbr2 = pool->GetMem(nodeHeader->GetKeySize());
	cRTreeNode<TMbr>* node = NULL;

	node1->CreateMbr(TMbr_mbr1);
	node2->CreateMbr(TMbr_mbr2);

	unsigned int pom_order;
	for (int i = 0; i<node1->GetItemCount(); i++)
	{
		if (TKey::Compare(TMbr_mbr, node1->GetCKey(i), sd) < 0)
		{
			node = node1;
			pom_order = i;
			break;
		}
	}
	if (node == NULL)
	{
		for (int i = 0; i<node2->GetItemCount(); i++)
		{
			if (TKey::Compare(TMbr_mbr, node2->GetCKey(i), sd) < 0)
			{
				node = node2;
				pom_order = i;
				break;
			}
		}
	}
	if (node == NULL)
	{
		node = node2;
		pom_order = node2->GetItemCount();
	}
	
	node->InsertItem(pom_order, TMbr_mbr, insNodeIndex);

	// free temporary memory
	pool->FreeMem(TMbr_mbr1);
	pool->FreeMem(TMbr_mbr2);
}

template<class TMbr>
bool cRTreeNode<TMbr>::IsOrderedFirstTuple(cRTreeOrderIndex<TKey> *mdOrderIndex)
{
	/*
	 * mk?: nejde prelozit: 
	bool ret = true;
	cSpaceDescriptor *sd = GetSpaceDescriptor();
	cTuple *tuple1 = new cTuple(sd);
	cTuple *tuple2 = new cTuple(sd);

	*currentTuple = *(mdOrderIndex->GetTuple(parent::GetIndex(), sd));

	for (unsigned int i = 0 ; i < parent::mItemCount-1; i++)
	{
		*tuple1 = *(mdOrderIndex->GetTuple(parent::GetLink(i), sd));
		*tuple2 = *(mdOrderIndex->GetTuple(parent::GetLink(i+1), sd));

		if (tuple1->Equal(*tuple2) > 0)
		{
			Print();
			tuple1->Print("\n");
			tuple2->Print("\n");
			printf("Critical Error: cRTreeNode::IsOrderedFirstTuple(): Node is not ordered!\n");
			ret = false;
			break;
		}
	}
	return ret;
	 */
	 return true;
}

#ifdef CUDA_ENABLED
#include "dstruct/paged/rtree/cRTreeNode_Gpu.h"
#endif

}}}
#endif