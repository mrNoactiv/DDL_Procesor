/**
*	\file cRangeQueryProcessor.h
*	\author Michal Kratky, Peter Chovanec
*	\version 0.2
*	\aug 2013
*	\brief Implements a processor of range queries in the R-tree
*/

#ifndef __cRangeQueryProcessor_h__
#define __cRangeQueryProcessor_h__

/**
* Implements a processor of range queries in the R-tree
*
*	\author Michal Kratky, Peter Chovanec
*	\version 0.2
*	\aug 2013
**/

#include "common/cCommon.h"
#include "dstruct/paged/queryprocessing/cRangeQueryConfig.h"
#include "dstruct/paged/core/cTreeHeader.h"
#include "dstruct/paged/core/cTreeItemStream.h"
#include "dstruct/paged/queryprocessing/cQueryProcStat.h"
#include "dstruct/paged/queryprocessing/cRangeQueryProcessorConstants.h"
#include "dstruct/paged/queryprocessing/cRQBuffers.h"

#ifdef CUDA_ENABLED
#include "dstruct/paged/cuda/cMemoryManagerCuda.h"
#include "dstruct/paged/cuda/cCudaParams.h"
#define CEILING(X) (X-(unsigned int)(X) > 0 ? (unsigned int)(X+1) : (unsigned int)(X))
#endif

using namespace common;

namespace dstruct {
	namespace paged {

template <class TKey, class TNode, class TLeafNode>
class cRangeQueryProcessor
{
private:
	cQuickDB *mQuickDB;
	cPagedTree<TKey, TNode, TLeafNode> *mTree;
	cTreeHeader *mTreeHeader;
	unsigned short mMRQBTreeType;

private:
	cTreeItemStream<TKey>* RangeQuery(sBatchRQ *batchRQ, cRangeQueryConfig *rqConfig, cRangeQueryContext *rqContext, cQueryProcStat* QueryProcStat);
	bool RangeQuery_DFS(const tNodeIndex& nodeIndex, uint level, sBatchRQ *batchRQ, cRangeQueryConfig *rqConfig, cRangeQueryContext *rqContext, cTreeItemStream<TKey>* resultSet, cRQBuffers<TKey>* rqBuffers, cQueryProcStat* QueryProcStat);

	// R-tree methods
	/*
	void ScanNode(TNode* node, unsigned int level, int& itemOrder, sBatchRQ* batchRQ, cItemStream<TKey> *resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey>* rqBuffers, cQueryProcStat *QueryProcStat);
	void ScanNode_Batch(TNode *node, int& itemOrder, unsigned int level, sBatchRQ *batchRQ, cItemStream<TKey> *resultSet, cArray<uint> *qrs, cArray<uint> *nqrs, cRangeQueryConfig *rqConfig, cRQBuffers<TKey> *rqBuffers, cQueryProcStat *QueryProcStat, unsigned int* resultSizes);
	void ScanNode_Cartesian(TNode* node, int& itemOrder, sBatchRQ* batchRQ, cArray<uint> **aqis, cArray<uint> **naqis, cRangeQueryConfig *rqConfig);

	bool ScanLeafNode(TLeafNode* leafNode, unsigned int level, sBatchRQ *batchRQ, cItemStream<TKey> *resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey>* rqBuffers, cQueryProcStat* QueryProcStat);
	bool ScanLeafNode_Batch(TLeafNode* leafNode, sBatchRQ *batchRQ, cArray<uint> *qrs, cItemStream<TKey> *resultSet, unsigned int finalResultSize, unsigned int* resultSizes, cNodeBuffers<TKey>* buffers, cQueryProcStat *QueryProcStat);
	void ScanLeafNode_Cartesian(TLeafNode* node, sBatchRQ* batchRQ, cArray<uint> **aqis, cItemStream<TKey> *resultSet, cNodeBuffers<TKey>* buffers, cQueryProcStat *QueryProcStat);
	 */

	void RangeQuery_pre(sBatchRQ *batchRQ, cRangeQueryConfig *rqConfig, cRQBuffers<TKey>* rqBuffers);
	void RangeQuery_post(cRQBuffers<TKey>* rqBuffers);

	//device dependent search methods
	void RangeQuery_Cpu(sBatchRQ *batchRQ, cRangeQueryConfig *rqConfig, cRangeQueryContext *rqContext, cTreeItemStream<TKey>* resultSet, cRQBuffers<TKey>* rqBuffers, cQueryProcStat* QueryProcStat);
	void RangeQuery_Gpu(sBatchRQ *batchRQ, cRangeQueryConfig *rqConfig, cRangeQueryContext *rqContext, cTreeItemStream<TKey>* resultSet, cRQBuffers<TKey>* rqBuffers, cQueryProcStat* QueryProcStat);
	void RangeQuery_Phi(sBatchRQ *batchRQ, cRangeQueryConfig *rqConfig, cRangeQueryContext *rqContext, cTreeItemStream<TKey>* resultSet, cRQBuffers<TKey>* rqBuffers, cQueryProcStat* QueryProcStat);
	//Depth-Breadth search methods
	void RangeQuery_DBFS(unsigned int level, sBatchRQ *batchRQ, cTreeItemStream<TKey> *resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey> *rqBuffers, cQueryProcStat *QueryProcStat);
	void RangeQuery_DBFS_Init(cDbfsLevel* buffer, cQueryProcStat *QueryProcStat,bool isInner);
	void DBFS_Cpu_ScanLevel(uint level, uint nodeType, sBatchRQ *batchRQ, cTreeItemStream<TKey> *resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey> *rqBuffers, cQueryProcStat *QueryProcStat);
	void DBFS_Gpu_ScanLevel(uint level, uint nodeType, sBatchRQ *batchRQ, cTreeItemStream<TKey> *resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey> *rqBuffers, cQueryProcStat *QueryProcStat);
	void DBFS_Phi_ScanLevel(uint level, uint nodeType, sBatchRQ *batchRQ, cTreeItemStream<TKey> *resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey> *rqBuffers, cQueryProcStat *QueryProcStat);
	void DBFS_EnqueueNode(unsigned int nodeIndex,unsigned int level,sBatchRQ *batchRQ, cTreeItemStream<TKey> *resultSet,cRangeQueryConfig *rqConfig, cRQBuffers<TKey>* rqBuffers, cQueryProcStat *QueryProcStat);

	// B-tree methods
	/*
	void ScanNode_Btree(TNode *node, int& itemOrder, sBatchRQ *batchRQ, common::memorystructures::cLinkedList<uint> *qrs, common::memorystructures::cLinkedList<uint> *nqrs, cRangeQueryConfig *rqConfig, unsigned int* resultSizes);
	void ScanNode_Btree_seq(TNode *node, int& itemOrder, sBatchRQ *batchRQ, common::memorystructures::cLinkedList<uint> *qrs, common::memorystructures::cLinkedList<uint> *nqrs, cRangeQueryConfig *rqConfig, unsigned int* resultSizes);
	void ScanNode_Btree_bin(TNode *node, int& itemOrder, sBatchRQ *batchRQ, common::memorystructures::cLinkedList<uint> *qrs, common::memorystructures::cLinkedList<uint> *nqrs, cRangeQueryConfig *rqConfig, unsigned int* resultSizes);
	bool ScanLeafNode_Btree(TLeafNode* leafNode, sBatchRQ *batchRQ, common::memorystructures::cLinkedList<uint> *qrs, cItemStream<TKey> *resultSet, unsigned int finalResultSize, unsigned int* resultSizes, cNodeBuffers<TKey>* buffers);
	bool ScanLeafNode_Btree_seq(TLeafNode* leafNode, sBatchRQ *batchRQ, common::memorystructures::cLinkedList<uint> *qrs, cItemStream<TKey> *resultSet, unsigned int finalResultSize, unsigned int* resultSizes, cNodeBuffers<TKey>* buffers);
	bool ScanLeafNode_Btree_bin(TLeafNode* leafNode, sBatchRQ *batchRQ, common::memorystructures::cLinkedList<uint> *qrs, cItemStream<TKey> *resultSet, unsigned int finalResultSize, unsigned int* resultSizes, cNodeBuffers<TKey>* buffers);
	bool ScanLeafNode_Btree_bin_lo(TLeafNode* leafNode, sBatchRQ *batchRQ, common::memorystructures::cLinkedList<uint> *qrs, cItemStream<TKey> *resultSet, unsigned int finalResultSize, unsigned int* resultSizes, cNodeBuffers<TKey>* buffers);
	 */

#ifdef CUDA_ENABLED
	void InitializeGpuQuery(sBatchRQ *batchRQ);
	//void DBFS_PrepareInnerNode(TNode* currentNode,cRQBuffers<TKey> *rqBuffers,cRangeQueryConfig *rqConfig,unsigned int level,unsigned int itemOrder);
	//void DBFS_PrepareLeafNode(TLeafNode* currentLeafNode,unsigned int level, sBatchRQ *batchRQ, cTreeItemStream<TKey> *resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey> *rqBuffers, cQueryProcStat *QueryProcStat,unsigned int itemOrder);
	//void DBFS_ScanNodes_Gpu(unsigned int level,sBatchRQ *batchRQ,cTreeItemStream<TKey> *resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey> *rqBuffers, cQueryProcStat *QueryProcStat,unsigned int nodeType);
	void DBFS_Gpu_ProcessOutput(cCudaParams &params, uint nodeType, uint level, sBatchRQ *batchRQ, cTreeItemStream<TKey>* resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey> *rqBuffers, cQueryProcStat *queryStat);
	void DBFS_Gpu_FillResultSet(unsigned int level, sBatchRQ *batchRQ, cTreeItemStream<TKey>* resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey> *rqBuffers, bool* resultVector);
	void DBFS_Gpu_FillResultSet(unsigned int level, sBatchRQ *batchRQ, cTreeItemStream<TKey>* resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey> *rqBuffers, uint* resultList, uint resultListCount);
	//void DBFS_FillResultSet(cTreeItemStream<TKey>* resultSet,uint* resultList, uint resultListCount);
	void DBFS_Gpu_FillNextLevel(unsigned int level, sBatchRQ *batchRQ, cTreeItemStream<TKey>* resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey> *rqBuffers, cQueryProcStat *QueryProcStat, bool* resultVector);
	void DBFS_Gpu_FillNextLevel(unsigned int level, sBatchRQ *batchRQ, cTreeItemStream<TKey>* resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey> *rqBuffers, cQueryProcStat *QueryProcStat, uint* resultList, uint resultListCount);
	//void GetValidChildrenFromVector( bool* resultVector,cRQBuffers<TKey> *rqBuffers,unsigned int level);
	//void GetResultsFromVector(cTreeItemStream<TKey>* resultSet, bool* resultVector,tNodeIndex* nodesLinks,unsigned int blockCount);
	cCudaParams CreateSearchParams(cMemoryManagerCuda* mmc, sBatchRQ *batchRQ, cRangeQueryConfig *rqConfig, cDbfsLevel* currentLevel, unsigned int nodeType);
	//void RangeQuery_GpuScan(uint rootIndex,sBatchRQ *batchRQ, cTreeItemStream<TKey> *resultSet, cRangeQueryConfig *rqConfig, cQueryProcStat *QueryProcStat);
#endif
public:
	cRangeQueryProcessor(cPagedTree<TKey, TNode, TLeafNode> *tree);
	~cRangeQueryProcessor();

	cTreeItemStream<TKey>* RangeQuery(const TKey *ql, const TKey *qh, cRangeQueryConfig *rqConfig, cRangeQueryContext *rqContext, cQueryProcStat* QueryProcStat = NULL);
	cTreeItemStream<TKey>* RangeQuery(const TKey *qls, const TKey *qhs, const unsigned int count, cRangeQueryConfig *rqConfig, cRangeQueryContext *rqContext, cQueryProcStat* QueryProcStat = NULL);
	cTreeItemStream<TKey>* RangeQuery(cHNTuple* ql, cHNTuple* qh, cSpaceDescriptor* querySD, cRangeQueryConfig *rqConfig, cRangeQueryContext *rqContext, cQueryProcStat* QueryProcStat = NULL);

	inline void SetQuickDB(cQuickDB *quickDB);
	inline void SetTreeHeader(cTreeHeader *treeHeader);
};

template <class TKey, class TNode, class TLeafNode> 
cRangeQueryProcessor<TKey, TNode, TLeafNode>::cRangeQueryProcessor(cPagedTree<TKey, TNode, TLeafNode> *tree) 
{
	mTree = tree;
	//mMRQBTreeType = MRQ_BTREE_BIN;
}

template <class TKey, class TNode, class TLeafNode> 
cRangeQueryProcessor<TKey, TNode, TLeafNode>::~cRangeQueryProcessor()
{
}

template <class TKey, class TNode, class TLeafNode> 
inline void cRangeQueryProcessor<TKey, TNode, TLeafNode>::SetQuickDB(cQuickDB *quickDB)
{
	mQuickDB = quickDB;
}

template <class TKey, class TNode, class TLeafNode> 
inline void cRangeQueryProcessor<TKey, TNode, TLeafNode>::SetTreeHeader(cTreeHeader *treeHeader)
{
	mTreeHeader = treeHeader;
}

template <class TKey, class TNode, class TLeafNode>
cTreeItemStream<TKey>* cRangeQueryProcessor<TKey, TNode, TLeafNode>::RangeQuery(const TKey *ql, const TKey *qh, cRangeQueryConfig *rqConfig, cRangeQueryContext *rqContext, cQueryProcStat* QueryProcStat) 
{
	sBatchRQ batchRQ = { (ushort)QueryType::BATCHQUERY, ql, qh, 1, NULL, NULL, NULL };
	return RangeQuery(&batchRQ, rqConfig, rqContext, QueryProcStat);
}

template <class TKey, class TNode, class TLeafNode>
cTreeItemStream<TKey>* cRangeQueryProcessor<TKey, TNode, TLeafNode>::RangeQuery(const TKey *qls, const TKey *qhs, const unsigned int queriesCount, cRangeQueryConfig *rqConfig, cRangeQueryContext *rqContext, cQueryProcStat* QueryProcStat) 
{
	//sBatchRQ batchRQ = { cRangeQueryProcessorConstants::RQ_BATCH, qls, qhs, queriesCount, NULL, NULL, NULL };
	sBatchRQ batchRQ = { (ushort)rqConfig->GetQueryProcessingType(), qls, qhs, queriesCount, NULL, NULL, NULL };
	return RangeQuery(&batchRQ, rqConfig, rqContext, QueryProcStat);
}

template <class TKey, class TNode, class TLeafNode>
cTreeItemStream<TKey>* cRangeQueryProcessor<TKey, TNode, TLeafNode>::RangeQuery(cHNTuple* ql, cHNTuple* qh, cSpaceDescriptor* querySD, cRangeQueryConfig *rqConfig, cRangeQueryContext *rqContext, cQueryProcStat* QueryProcStat)
{
	sBatchRQ batchRQ = { (ushort)QueryType::CARTESIANQUERY, NULL, NULL, 0, ql, qh, querySD };
	return RangeQuery(&batchRQ, rqConfig, rqContext, QueryProcStat);
}

template <class TKey, class TNode, class TLeafNode>
cTreeItemStream<TKey>* cRangeQueryProcessor<TKey, TNode, TLeafNode>::RangeQuery(sBatchRQ *batchRQ, cRangeQueryConfig *rqConfig, 
	cRangeQueryContext *rqContext, cQueryProcStat* QueryProcStat) 
{
	cTreeItemStream<TKey>* resultSet = (cTreeItemStream<TKey>*)mQuickDB->GetResultSet();
	resultSet->SetNodeHeader(mTreeHeader->GetNodeHeader(cTreeHeader::HEADER_LEAFNODE));

	uint  c1;
	cRQBuffers<TKey> rqBuffers;

	if (QueryProcStat != NULL)
	{
		QueryProcStat->ResetQuery();
#ifdef RTREE_QPROC
		c1 = cMBRectangle<TKey>::Computation_Compare;
#endif
	}
	RangeQuery_pre(batchRQ, rqConfig, &rqBuffers);
	if (rqConfig->GetDevice() == cRangeQueryConfig::DEVICE_GPU)
	{
		RangeQuery_Gpu(batchRQ,rqConfig,rqContext,resultSet,&rqBuffers,QueryProcStat);
	}
	else if (rqConfig->GetDevice() == cRangeQueryConfig::DEVICE_PHI)
	{
		RangeQuery_Phi(batchRQ, rqConfig, rqContext, resultSet, &rqBuffers, QueryProcStat);
	}
	else //CPU
	{
		RangeQuery_Cpu(batchRQ, rqConfig, rqContext, resultSet, &rqBuffers, QueryProcStat);
	}
	RangeQuery_post(&rqBuffers);

	if (QueryProcStat != NULL)
	{
		// it utilizes a static variable of cMBRectangle, it means it does not work
		// in the case of a multithread environment
#ifdef RTREE_QPROC
		QueryProcStat->IncComputCompareQuery(cMBRectangle<TKey>::Computation_Compare - c1);
#endif
		QueryProcStat->AddQueryProcStat();
	}

	resultSet->FinishWrite();
	return resultSet;
}
/*
* Range Query Depth First Search. 
*/
template <class TKey, class TNode, class TLeafNode> 
bool cRangeQueryProcessor<TKey, TNode, TLeafNode>::RangeQuery_DFS(const tNodeIndex& nodeIndex, unsigned int level, 
	sBatchRQ *batchRQ, cRangeQueryConfig *rqConfig, cRangeQueryContext *rqContext, cTreeItemStream<TKey>* resultSet, cRQBuffers<TKey>* rqBuffers, cQueryProcStat* QueryProcStat)
{
	uint currentResultSize;
	bool endf = false;
	TNode* node = NULL;
	TLeafNode* leafNode = NULL;

	//val644 - start - Nastaveni v jake urovni stromu se nachazime
	cTuple::levelTree = level;
	//val644 - end - Nastaveni v jake urovni stromu se nachazime

	if (TNode::IsLeaf(nodeIndex))
	{
		if (rqConfig->IsBulkReadEnabled())
		{
			DBFS_EnqueueNode(TNode::GetNodeIndex(nodeIndex), 0, batchRQ, resultSet, rqConfig, rqBuffers, QueryProcStat);
		}
		else
		{
			leafNode = mTree->ReadLeafNodeR(nodeIndex);			

			// ZWI0009
			cTuple::itemsCountForLevel[cTuple::levelTree] += leafNode->GetItemCount();

			//endf = ScanLeafNode(leafNode, level, batchRQ, resultSet, rqConfig, rqBuffers, QueryProcStat);
			endf = leafNode->ScanLeafNode(level, batchRQ, resultSet, rqConfig, rqBuffers, QueryProcStat);
			mQuickDB->GetNodeCache()->UnlockR(leafNode);
		}
	}
	else
	{
		node = mTree->ReadInnerNodeR(nodeIndex);
		if (QueryProcStat != NULL)
		{
			QueryProcStat->IncLarInQuery(level);
			QueryProcStat->IncSiInQuery();
		}

		int itemOrder = -1;
		int itemCount = (int)node->GetItemCount();

		// ZWI0009
		cTuple::itemsCountForLevel[cTuple::levelTree] += node->GetItemCount();

		while (true)
		{
			node->ScanNode(level, mTreeHeader->GetHeight(), itemOrder, batchRQ, resultSet, rqConfig, rqBuffers, QueryProcStat);
			//ScanNode(node, level, itemOrder, batchRQ, resultSet, rqConfig, rqBuffers, QueryProcStat);

			if (itemOrder < itemCount)
			{
				currentResultSize = resultSet->GetItemCount();
				endf = RangeQuery_DFS(node->GetLink(itemOrder), level + 1, batchRQ, rqConfig, rqContext, resultSet, rqBuffers, QueryProcStat);
				
				if ((resultSet->GetItemCount() > currentResultSize) && (!TNode::IsLeaf(node->GetLink(itemOrder))) && (QueryProcStat != NULL))
				{
					QueryProcStat->IncRelevantInQuery();
				}

				if (endf) // if the finalResultSize is reached finish the range query batch
				{
					break;
				}

				// val644 - start - Nastaveni v jake urovni stromu se nachazime
				cTuple::levelTree = level;
				//val644 - end - Nastaveni v jake urovni stromu se nachazime	
			}
			else
			{
				break;
			}
		}
		mQuickDB->GetNodeCache()->UnlockR(node);
	}
	return endf;
}


/*
template <class TKey, class TNode, class TLeafNode> 
void cRangeQueryProcessor<TKey, TNode, TLeafNode>::ScanNode(TNode* node, unsigned int level, int& itemOrder, sBatchRQ* batchRQ, cTreeItemStream<TKey> *resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey>* rqBuffers,cQueryProcStat *QueryProcStat)
{
	if (batchRQ->mode == RQ_BATCH)
	{
		if (rqConfig->GetSearchMethod() == cRangeQueryConfig::SEARCH_DBFS || rqConfig->GetSearchMethod() == cRangeQueryConfig::SEARCH_BFS)
		{		
			node->ScanNode_Batch(itemOrder, level, batchRQ, resultSet, rqBuffers->qrs[0], rqBuffers->qrs[1], rqConfig, rqBuffers,QueryProcStat, rqBuffers->resultSizes);
		}
		else
		{
			node->ScanNode_Batch(itemOrder, level, batchRQ, resultSet, rqBuffers->qrs[level], rqBuffers->qrs[level+1], rqConfig, rqBuffers, QueryProcStat, rqBuffers->resultSizes);
		}
	}
	else if (batchRQ->mode == RQ_CARTESIAN) // Cartesian RQ
	{
		node->ScanNode_Cartesian(itemOrder, batchRQ, rqBuffers->aqis[level], rqBuffers->aqis[level + 1], rqConfig);
	}
	else // B-tree
	{
		ScanNode_Btree(node, itemOrder, batchRQ, rqBuffers->qrs_ll[level], rqBuffers->qrs_ll[level + 1], rqConfig, rqBuffers->resultSizes);
	}
}
*/

/*
template <class TKey, class TNode, class TLeafNode> 
bool cRangeQueryProcessor<TKey, TNode, TLeafNode>::ScanLeafNode(TLeafNode* leafNode, unsigned int level, sBatchRQ *batchRQ, cItemStream<TKey> *resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey>* rqBuffers, cQueryProcStat *QueryProcStat)
{
	if (QueryProcStat != NULL)
	{
		QueryProcStat->IncLarLnQuery();
	}
	bool ret = false;

	/*
	if (batchRQ->mode == RQ_BATCH)
	{
		ret = leafNode->ScanLeafNode_Batch(batchRQ, rqBuffers->qrs[level], resultSet, rqConfig->GetFinalResultSize(), rqBuffers->resultSizes, &rqBuffers->nodeBuffer, QueryProcStat);
	}
	else if (batchRQ->mode == RQ_CARTESIAN)  // Cartesian RQ
	{
		// in the case of Cartesian RQ no final size of the result set is not defined
		leafNode->ScanLeafNode_Cartesian(batchRQ, rqBuffers->aqis[level], resultSet, &rqBuffers->nodeBuffer, QueryProcStat);
	}
	else // B-tree
	{
		ret = ScanLeafNode_Btree(leafNode, batchRQ, rqBuffers->qrs_ll[level], resultSet, rqConfig->GetFinalResultSize(), rqBuffers->resultSizes, &rqBuffers->nodeBuffer);
	}
	return ret;
}
*/

/*
 * Return true if the finalResultSize is reached (and the range query should be finished).
 * Params:
 * \param resultSizes includes the result size for each query of the batch
 */
 /*
template <class TKey, class TNode, class TLeafNode> 
bool cRangeQueryProcessor<TKey, TNode, TLeafNode>::ScanLeafNode_Batch(TLeafNode* leafNode, sBatchRQ *batchRQ, cArray<unsigned int> *qrs,
	cItemStream<TKey> *resultSet, unsigned int finalResultSize, unsigned int* resultSizes, cNodeBuffers<TKey>* buffers, cQueryProcStat *QueryProcStat)
{
	bool endf = false;
	bool emptyResult = true;
#ifdef RTREE_QPROC
	cSpaceDescriptor* sd = ((cRTreeHeader<TKey>*)mTreeHeader)->GetSpaceDescriptor();
	unsigned int complfinalResultSize = batchRQ->queriesCount * finalResultSize;

	uint itemCount = leafNode->GetItemCount();
	uint qrsCount = qrs->Count();

	for (unsigned int i = 0 ; i < itemCount ; i++)
	{
		for (unsigned int j = 0 ; j < qrsCount ; j++)
		{
			uint ind = qrs->GetRefItem(j);

			if (finalResultSize != cRangeQueryConfig::FINAL_RESULTSIZE_UNDEFINED && finalResultSize <= resultSizes[ind])
			{
				continue;  // the finalResultSize for this query has been already reached
			}

			if (cMBRectangle<TKey>::IsInRectangle(batchRQ->qls[ind], batchRQ->qhs[ind], leafNode->GetCKey(i, &buffers->itemBuffer), sd))
			{
				resultSet->Add(leafNode->GetCItem(i, &buffers->itemBuffer));
				resultSizes[ind]++;
				emptyResult = false;

				if (finalResultSize != cRangeQueryConfig::FINAL_RESULTSIZE_UNDEFINED && complfinalResultSize == resultSet->GetItemCount())
				{
					endf = true;   // if the finalResultSize is reached, set the range query should be finished
					break;
				}
				// break; // break is not possible due to finalResultSize
			}
		}
		if (endf)
		{	
			break;
		}
	}

	if (QueryProcStat != NULL && !emptyResult)
	{
		QueryProcStat->IncRelevantLnQuery();
	}
#endif

	return endf;
}
*/ 

/*
 */
/* 
template <class TKey, class TNode, class TLeafNode> 
void cRangeQueryProcessor<TKey, TNode, TLeafNode>::ScanNode_Cartesian(TNode* node, int& itemOrder, sBatchRQ* batchRQ, 
	cArray<uint> **aqis, cArray<uint> **naqis, cRangeQueryConfig *rqConfig)
{
#ifdef RTREE_QPROC
	cSpaceDescriptor* sd = ((cRTreeHeader<TKey>*)mTreeHeader)->GetSpaceDescriptor();
	uint dim = sd->GetDimension();
	bool isIntersected;

	int itemCount = (int)node->GetItemCount();
	for (itemOrder++ ; itemOrder < itemCount ; itemOrder++)
	{
		const char* mbr = node->GetCKey(itemOrder);

		for (uint j = 0 ; j < dim ; j++)
		{
			cArray<uint> *qis = aqis[j];
			uint qisCount = qis->Count();
			cArray<uint> *nqis = naqis[j];
			nqis->ClearCount();
			isIntersected = false;  // (... OR ...) AND (... OR ...) ...

			char *ql = batchRQ->ql->GetNTuple(j, batchRQ->sd);
			char *qh = batchRQ->qh->GetNTuple(j, batchRQ->sd);
			cSpaceDescriptor *ntSD = batchRQ->sd->GetInnerSpaceDescriptor(j);
			assert(batchRQ->sd->GetTypeCode(j) == cLNTuple::CODE);

			// temp print
			//const char* mbr = node->GetCKey(itemOrder);
			//cMBRectangle<TKey>::Print(mbr, "\n", sd);
			//cLNTuple::Print(ql, "\n", ntSD);
			//cLNTuple::Print(qh, "\n", ntSD);
			// -----

			for (uint k = 0 ; k < qisCount ; k++)
			{
				uint ind = qis->GetRefItem(k);
				if (cMBRectangle<TKey>::IsIntersected(mbr, j, sd, ql, qh, ind, ntSD))
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
#endif
}
*/

/*
template <class TKey, class TNode, class TLeafNode> 
void cRangeQueryProcessor<TKey, TNode, TLeafNode>::ScanLeafNode_Cartesian(TLeafNode* node, sBatchRQ* batchRQ, 
	cArray<uint> **aqis, cItemStream<TKey> *resultSet, cNodeBuffers<TKey>* buffers, cQueryProcStat *QueryProcStat)
{
#ifdef RTREE_QPROC
	bool emptyResult = true;
	// printf("LN: %d\t", node->GetIndex());
	cSpaceDescriptor* sd = ((cRTreeHeader<TKey>*)mTreeHeader)->GetSpaceDescriptor();
	uint dim = sd->GetDimension();
	bool isIntersected;

	int itemCount = (int)node->GetItemCount();
	for (uint i = 0 ; i < itemCount ; i++)
	{
		const char *key = node->GetCKey(i, &buffers->itemBuffer);

		for (uint j = 0 ; j < dim ; j++)
		{
			cArray<uint> *qis = aqis[j];
			uint qisCount = qis->Count();
			isIntersected = false;      // (... OR ...) AND (... OR ...) ...

			char *ql = batchRQ->ql->GetNTuple(j, batchRQ->sd);
			char *qh = batchRQ->qh->GetNTuple(j, batchRQ->sd);
			cSpaceDescriptor *ntSD = batchRQ->sd->GetInnerSpaceDescriptor(j);
			assert(batchRQ->sd->GetTypeCode(j) == cLNTuple::CODE);

			for (uint k = 0 ; k < qisCount ; k++)
			{
				uint ind = qis->GetRefItem(k);

				if (cMBRectangle<TKey>::IsInInterval(key, j, sd, ql, qh, ind, ntSD))
				{
					isIntersected = true;
					break;
				}
			}
			if (!isIntersected)
			{
				break;
			}
		}
		if (isIntersected)
		{
			resultSet->Add(key);
			emptyResult = false;
		}
	}

	if (QueryProcStat != NULL && !emptyResult)
	{
		QueryProcStat->IncRelevantLnQuery();
	}
#endif
}
*/

/**
* Range Query method for Bread First Search or Depth-Breadth First Search tree traversal.
*/
template <class TKey, class TNode, class TLeafNode> 
void cRangeQueryProcessor<TKey, TNode, TLeafNode>::RangeQuery_DBFS(
	unsigned int level, sBatchRQ *batchRQ, cTreeItemStream<TKey> *resultSet, cRangeQueryConfig *rqConfig, 
	cRQBuffers<TKey> *rqBuffers, cQueryProcStat *QueryProcStat)
{
	cNodeCache *nodeCache = mQuickDB->GetNodeCache();
	bool isInner = level < mTreeHeader->GetHeight();
	uint nodeType = isInner ? mTree->GetHeader()->GetNodeType(cTreeHeader::HEADER_NODE) : mTree->GetHeader()->GetNodeType(cTreeHeader::HEADER_LEAFNODE);
	uint level_p1 = level + 1;
	cDbfsLevel *currentLevel = rqBuffers->GetBreadthSearchArray(rqConfig, level);
	cDbfsLevel *nextLevel = rqBuffers->GetBreadthSearchArray(rqConfig, level_p1);
	RangeQuery_DBFS_Init(currentLevel, QueryProcStat,isInner);
	if (isInner)
		nextLevel->ClearCount();
 	if (level == 0)
	{
		currentLevel->Add(mTreeHeader->GetRootIndex());
	}
	else
	{
		//currentLevel->Sort();
		nodeCache->BulkRead(currentLevel, rqConfig, isInner ? 1 : 0);
	}
	//printf("\nlevel: %d, items: %d.\t", level, currentLevel->Count());
	
	switch (rqConfig->GetDevice()) //search current level
	{
		case cRangeQueryConfig::DEVICE_CPU:
		default:
			DBFS_Cpu_ScanLevel(level, nodeType, batchRQ, resultSet, rqConfig, rqBuffers, QueryProcStat);
			break;
		case cRangeQueryConfig::DEVICE_GPU:
			DBFS_Gpu_ScanLevel(level, nodeType, batchRQ, resultSet, rqConfig, rqBuffers, QueryProcStat);
			break;
		case cRangeQueryConfig::DEVICE_PHI:
			DBFS_Phi_ScanLevel(level, nodeType, batchRQ, resultSet, rqConfig, rqBuffers, QueryProcStat);
			break;
	}
	//unlock nodes read by bulk read
	for (uint i = 0; i < currentLevel->Count(); i++)
	{
		if (isInner)
		{
			nodeCache->UnlockR(mTree->ReadInnerNodeR(currentLevel->GetRefItem(i)));
		}
		else
		{
			nodeCache->UnlockR(mTree->ReadLeafNodeR(currentLevel->GetRefItem(i)));
		}
	}
	if (level_p1 <= mTreeHeader->GetHeight()) //search next level
	{
		RangeQuery_DBFS(level + 1, batchRQ, resultSet, rqConfig, rqBuffers, QueryProcStat);
	}
}

/**
* Updates the query processing statistics for Bread First Search or Depth-Breadth First Search tree traversal.
*/
template <class TKey, class TNode, class TLeafNode> 
void cRangeQueryProcessor<TKey, TNode, TLeafNode>::RangeQuery_DBFS_Init(cDbfsLevel* buffer, cQueryProcStat *QueryProcStat, bool isInner)
{
	//query statistics
	if (!isInner)
	{
		if (QueryProcStat != NULL)
		{
			QueryProcStat->IncSiLnQuery();
			QueryProcStat->AddLarLnQuery(buffer->Count());
		}
	}
	else
	{
		if (QueryProcStat != NULL)
		{
			QueryProcStat->IncSiInQuery();
			QueryProcStat->AddLarInQuery(buffer->Count());
		}
	}

}

/**
* Scans single tree level on Cpu.
*/
template <class TKey, class TNode, class TLeafNode>
void cRangeQueryProcessor<TKey, TNode, TLeafNode>::DBFS_Cpu_ScanLevel(uint level, uint nodeType, sBatchRQ *batchRQ, cTreeItemStream<TKey> *resultSet, cRangeQueryConfig *rqConfig,
	cRQBuffers<TKey> *rqBuffers, cQueryProcStat *QueryProcStat)
{
	TLeafNode *currentLeafNode = NULL;
	TNode* currentNode = NULL;
	cDbfsLevel* currentLevel = rqBuffers->GetBreadthSearchArray(rqConfig,level);
	//cDbfsLevel* nextLevel
	for (uint i=0;i<currentLevel->Count();i++)
	{
		if (nodeType == 1/*cRTreeHeader<TKey>::HEADER_NODE*/)
		{
			currentNode = mTree->ReadInnerNodeR(currentLevel->GetRefItem(i));
			int itemOrder_p1 = -1;
			currentNode->ScanNode(level, mTreeHeader->GetHeight(), itemOrder_p1, batchRQ, resultSet, rqConfig, rqBuffers, QueryProcStat);
		}
		else
		{
			currentLeafNode = mTree->ReadLeafNodeR(currentLevel->GetRefItem(i));
			currentLeafNode->ScanLeafNode(0, batchRQ, resultSet, rqConfig, rqBuffers, QueryProcStat);
		}
	}
}

template<class TKey, class TNode, class TLeafNode>
void cRangeQueryProcessor<TKey, TNode, TLeafNode>::RangeQuery_pre(sBatchRQ *batchRQ, cRangeQueryConfig *rqConfig, cRQBuffers<TKey>* rqBuffers)
{
	unsigned int bufferSize = 0, bsizeCodingRI = 0, bsizeRQBatch = 0, bsizeSignatures = 0, bsizeBulkRead = 0;
	unsigned int nofNarrowDims = 0, nofTrueBitOrders = 0;

	//cSpaceDescriptor *sd;
//#ifdef RTREE_QPROC
//	cRTreeHeader<TKey>* header = ((cRTreeHeader<TKey>*)mTreeHeader);
//	sd = header->GetSpaceDescriptor();
//#else
//	sd = (cSpaceDescriptor*)mTreeHeader->GetNodeHeader(0)->GetKeyDescriptor();
//#endif

	//uint dim = sd->GetDimension();
	uint hPlus1 = mTreeHeader->GetHeight() + 1;
	uint dbfsNoLevels = 0;
	uint dbfsBufferCapacity;
	if (hPlus1 == 1)
	{
		hPlus1++;  // a special case: an empty R-tree has the height = 1
	}

	if (batchRQ->mode == QueryType::BATCHQUERY)
	{
		bsizeRQBatch = sizeof(cArray<unsigned int>*) * hPlus1 +
			hPlus1 * (sizeof(cArray<unsigned int>) + batchRQ->queriesCount * sizeof(unsigned int)) +
			batchRQ->queriesCount * sizeof(unsigned int);  // resultSizes
		bufferSize += bsizeRQBatch;
	}
	else if (batchRQ->mode >= cRangeQueryProcessorConstants::RQ_BTREE_SEQ) //BAS064
	{
		bsizeRQBatch = sizeof(cLinkedList<unsigned int>*) * hPlus1 +
			hPlus1 * (sizeof(cLinkedList<unsigned int>) + sizeof(cLinkedListNode<unsigned int>*) +
			(batchRQ->queriesCount * (sizeof(unsigned int)+(3 * sizeof(cLinkedListNode<unsigned int>*))))) +
			batchRQ->queriesCount * sizeof(unsigned int);  // resultSizes
		bufferSize += bsizeRQBatch;
	}

	if (mTreeHeader->GetDStructMode() == cDStructConst::DSMODE_CODING 
		|| mTreeHeader->GetDStructMode() == cDStructConst::DSMODE_RI
		|| mTreeHeader->GetDStructMode() == cDStructConst::DSMODE_RICODING) 
	{
		bsizeCodingRI = 2 * mTreeHeader->GetTmpBufferSize();
		bufferSize += bsizeCodingRI;
		
		//what about variable length? bas064
		/*bsizeCodingRI = ((mTreeHeader->GetKeySize()*2) + mTreeHeader->GetLeafDataSize()) * 2;
		bufferSize += bsizeCodingRI;*/
	}

	bufferSize += mTree->RangeQuery_preSize(&nofTrueBitOrders, batchRQ);

	if (rqConfig->IsBulkReadEnabled())
	{
		bsizeBulkRead += sizeof(cArray<unsigned short>) + rqConfig->GetNodeIndexCapacity_BulkRead() * sizeof(tNodeIndex);
		bufferSize += bsizeBulkRead;
	}

	//Breadth First Search and Depth-Breadth First Search buffer sizes
	unsigned int bsizeDBFS;
	if (rqConfig->GetSearchMethod() == cRangeQueryConfig::SEARCH_DFS && rqConfig->IsBulkReadEnabled())
	{
		dbfsBufferCapacity = rqConfig->GetNodeIndexCapacity_BulkRead();
		dbfsNoLevels = 1;
	}
	else if (rqConfig->GetSearchMethod() == cRangeQueryConfig::SEARCH_BFS) //only two arrays with size = leaf nodes count in tree
	{
		dbfsBufferCapacity = rqConfig->GetSearchStruct() == cRangeQueryConfig::SEARCH_STRUCT_ARRAY ? mTreeHeader->GetLeafNodeCount() : mTreeHeader->GetNodeCount() + 1;
		dbfsNoLevels = 2;
	}
	else
	{
		dbfsBufferCapacity = rqConfig->GetNodeIndexCapacity_BulkRead();
		dbfsNoLevels = mTreeHeader->GetHeight();
	}
	bsizeDBFS = cRQBuffers<TKey>::GetSize(rqConfig->GetSearchStruct(), dbfsBufferCapacity, dbfsNoLevels);
	bufferSize += bsizeDBFS; 

	// char *buffer = mMemoryPool->GetMem(bufferSize);
	cMemoryBlock* bufferMemBlock = mQuickDB->GetMemoryManager()->GetMem(bufferSize);
	rqBuffers->bufferMemBlock = bufferMemBlock;
	char* buffer = bufferMemBlock->GetMem();

	if (batchRQ->mode == (ushort)QueryType::BATCHQUERY)
	{
		//rqBuffers->qrs = (cArray<unsigned int>**)buffer;
		rqBuffers->qrs = new (buffer)cArray<unsigned int>*[hPlus1];
		buffer += sizeof(cArray<unsigned int>*) * hPlus1;
		for (unsigned int i = 0 ; i < hPlus1 ; i++)
		{
			//rqBuffers->qrs[i] = (cArray<unsigned int>*)buffer;
			rqBuffers->qrs[i] = new (buffer) cArray<unsigned int>(false, batchRQ->queriesCount);
			
			buffer += rqBuffers->qrs[i]->GetListBufferSize();
			buffer += rqBuffers->qrs[i]->GetItemsBufferSize();

			//buffer += sizeof(cArray<unsigned int>);			
			//buffer += batchRQ->queriesCount;
			//buffer += batchRQ->queriesCount * sizeof(unsigned int);
		}
		rqBuffers->resultSizes = (unsigned int*)buffer;
		buffer += batchRQ->queriesCount * sizeof(unsigned int);
		// copy indices of all qls and qhs to the qrs for level = 0
		for (unsigned int i = 0 ; i < batchRQ->queriesCount ; i++)
		{
			rqBuffers->qrs[0]->Add(i);
			rqBuffers->resultSizes[i] = 0;
		}
	}
	else if (batchRQ->mode >= cRangeQueryProcessorConstants::RQ_BTREE_SEQ) //bas064
	{
		rqBuffers->qrs_ll = new (buffer)cLinkedList<unsigned int>*[hPlus1];		
		buffer += sizeof(cLinkedList<unsigned int>*) * hPlus1;

		for (unsigned int i = 0; i < hPlus1; i++)
		{
			rqBuffers->qrs_ll[i] = new (buffer)cLinkedList<unsigned int>(batchRQ->queriesCount);
						
			buffer += rqBuffers->qrs_ll[i]->GetListBufferSize();						
			buffer += rqBuffers->qrs_ll[i]->GetItemsBufferSize();
		}

		rqBuffers->resultSizes = (unsigned int*)buffer;
		buffer += batchRQ->queriesCount * sizeof(unsigned int);
		
		// copy indices of all qls and qhs to the qrs for level = 0
		cLinkedList<unsigned int> *list = rqBuffers->qrs_ll[0];				
		
		memset(rqBuffers->resultSizes, 0, batchRQ->queriesCount * sizeof(unsigned int));
		for (unsigned int i = 0; i < batchRQ->queriesCount; i++)
		{
			list->AddItem(i);
		}
	}

	/*
	// bas064 - buffer for compression
	if (mTreeHeader->GetDStructMode() == cDStructConst::DSMODE_CODING) 
	{
		rqBuffers->nodeBuffer.itemBuffer.codingBuffer = buffer;
		buffer += bsizeCodingRI;
	}*/

	if (mTreeHeader->GetDStructMode() == cDStructConst::DSMODE_RI)
	{
		rqBuffers->nodeBuffer.itemBuffer.riBuffer = buffer;
	}
	else if (mTreeHeader->GetDStructMode() == cDStructConst::DSMODE_CODING)
	{
		rqBuffers->nodeBuffer.itemBuffer.codingBuffer = buffer;
	}
	else  if (mTreeHeader->GetDStructMode() == cDStructConst::DSMODE_RICODING)
	{
		rqBuffers->nodeBuffer.itemBuffer.riBuffer = buffer;
		rqBuffers->nodeBuffer.itemBuffer.codingBuffer = buffer + (mTreeHeader->GetTmpBufferSize() / 2);
	}

	if (mTreeHeader->GetDStructMode() != cDStructConst::DSMODE_DEFAULT)
	{
		buffer += mTreeHeader->GetTmpBufferSize();
	}

	if (mTreeHeader->GetDStructMode() == cDStructConst::DSMODE_RI)
	{
		rqBuffers->nodeBuffer.itemBuffer2.riBuffer = buffer;
	}
	else if (mTreeHeader->GetDStructMode() == cDStructConst::DSMODE_CODING)
	{
		rqBuffers->nodeBuffer.itemBuffer2.codingBuffer = buffer;
	}
	else  if (mTreeHeader->GetDStructMode() == cDStructConst::DSMODE_RICODING)
	{
		rqBuffers->nodeBuffer.itemBuffer2.riBuffer = buffer;
		rqBuffers->nodeBuffer.itemBuffer2.codingBuffer = buffer + (mTreeHeader->GetTmpBufferSize() / 2);
	}

	if (mTreeHeader->GetDStructMode() != cDStructConst::DSMODE_DEFAULT)
	{
		buffer += mTreeHeader->GetTmpBufferSize();
	}

	if (dbfsNoLevels > 0) //
	{
		rqBuffers->InitLevels(rqConfig, dbfsBufferCapacity, buffer, dbfsNoLevels);
	}

	buffer = mTree->RangeQuery_preAlloc(&nofTrueBitOrders, batchRQ, buffer, rqBuffers);
}

template<class TKey, class TNode, class TLeafNode>
void cRangeQueryProcessor<TKey, TNode, TLeafNode>::RangeQuery_post(cRQBuffers<TKey>* rqBuffers)
{
	mQuickDB->GetMemoryManager()->ReleaseMem(rqBuffers->bufferMemBlock);
}

/**
* Enqueues the node for Breath First Search or Depth-Breadth First Search in specific level buffer.
*/
template <class TKey, class TNode, class TLeafNode> 
void cRangeQueryProcessor<TKey, TNode, TLeafNode>::DBFS_EnqueueNode(unsigned int nodeIndex,unsigned int level,sBatchRQ *batchRQ, cTreeItemStream<TKey> *resultSet,cRangeQueryConfig *rqConfig, cRQBuffers<TKey>* rqBuffers, cQueryProcStat *QueryProcStat)
{
	//printf("\nLevel: ;%d;nodeindex: ;%d",level,nodeIndex);
	cDbfsLevel* dbfsArray = rqBuffers->GetBreadthSearchArray(rqConfig, level);
	dbfsArray->Add(nodeIndex);
	if (rqConfig->GetSearchMethod() != cRangeQueryConfig::SEARCH_BFS && dbfsArray->Count() == rqConfig->GetNodeIndexCapacity_BulkRead())
	{
		RangeQuery_DBFS(level,batchRQ,resultSet,rqConfig,rqBuffers,QueryProcStat);
		dbfsArray->ClearCount();
	}
}

/**
* Common range query method for Cpu.
*/
template <class TKey, class TNode, class TLeafNode>
void cRangeQueryProcessor<TKey, TNode, TLeafNode>::RangeQuery_Cpu(sBatchRQ *batchRQ, cRangeQueryConfig *rqConfig, cRangeQueryContext *rqContext, cTreeItemStream<TKey>* resultSet, cRQBuffers<TKey>* rqBuffers, cQueryProcStat* QueryProcStat)
{
	uint level = 0;
	if (rqConfig->GetSearchMethod() == cRangeQueryConfig::SEARCH_DBFS || rqConfig->GetSearchMethod() == cRangeQueryConfig::SEARCH_BFS)
	{
		RangeQuery_DBFS(level, batchRQ,resultSet, rqConfig, rqBuffers, QueryProcStat);
	}
	else
	{
		if (QueryProcStat != NULL)
		{
			//QueryProcStat->IncRelevantInQuery();
		}
		
		RangeQuery_DFS(mTreeHeader->GetRootIndex(), level, batchRQ, rqConfig, rqContext, resultSet, rqBuffers, QueryProcStat);
		if (rqConfig->IsBulkReadEnabled() && rqBuffers->GetBreadthSearchArray()->Count())
		{
			RangeQuery_DBFS(level, batchRQ, resultSet, rqConfig, rqBuffers, QueryProcStat);
		}
	}
}
#include "dstruct/paged/queryprocessing/cRangeQueryProcessor_Gpu.h"
#include "dstruct/paged/queryprocessing/cRangeQueryProcessor_Phi.h"
}}
#endif
