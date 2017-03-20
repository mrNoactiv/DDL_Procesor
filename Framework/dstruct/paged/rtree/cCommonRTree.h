/**
*	\file cCommonRTree.h
*	\author Michal Kratky
*	\version 0.4
*	\date nov 2013
*	\version 0.3
*	\date jul 2011
*	\version 0.2
*	\date 2003
*	\brief It implements the paged R*-tree
*/

#ifndef __cCommonRTree_h__
#define __cCommonRTree_h__

#include <float.h>
#include <mutex>

#include "dstruct/paged/core/cQuickDB.h"
#include "dstruct/paged/core/cPagedTree.h"
#include "dstruct/paged/rtree/sItemIdRecord.h"
#include "dstruct/paged/rtree/cRTreeHeader.h"
#include "dstruct/paged/rtree/cRTreeLog.h"
#include "dstruct/paged/queryprocessing/cRangeQueryContext.h"
#include "dstruct/paged/queryprocessing/cRangeQueryConfig.h"
#include "dstruct/paged/queryprocessing/cRangeQueryProcessor.h"
#include "dstruct/paged/queryprocessing/cQueryProcStat.h"
#include "dstruct/paged/rtree/cInsertBuffers.h"


#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "dstruct/paged/rtree/cRTreeSignatureIndex.h"
#include "dstruct/paged/rtree/cRTreeOrderIndex.h"

// States of insert procedure
#define INSERT_TRAVERSE_UP_EXTERNDS_CHANGED 16 // changeFT
#define INSERT_TRAVERSE_UP_INDEX_NOT_CHANGED 8 //// #define INSERT_TRAVERSE_UP_FT_ONLY 3
#define INSERT_TRAVERSE_EXIT 4
#define INSERT_TRAVERSE_UP 2
#define INSERT_TRAVERSE_DOWN 1

using namespace common::datatype::tuple;

/**
 * It implements a persistent R*-tree.
 * Parameters of the template:
 *		- TMbr - Key of the inner node, it means MBRectangle
 *		- TKey - Key of the leaf node, e.g. TKey, cUniformTuple
 *		- TNode - Inner node
 *		- TLeafNode - Leaf node
 *
 *	\author Michal Kratky
 *	\version 0.3
 *	\date jul 2011
 **/
namespace dstruct {
	namespace paged {
		namespace rtree {

int compare (const void *a, const void *b);
int compare2 (const void *a, const void *b);

template<class TMbr, class TKey, class TNode, class TLeafNode>
class cCommonRTree: public cPagedTree<TKey, TNode, TLeafNode>
{
	typedef cPagedTree<TKey, TNode, TLeafNode> parent;
	
private:
	int mCurrenTKeyOrder;      // for Update() purpose
	tNodeIndex mCurrentLeafNodeIndex;   // for Update() purpose
	static const unsigned int NUMBER_OF_TIMERS = 3;
	cTimer mTimers[NUMBER_OF_TIMERS];
	cRTreeLog mRTreeLog;
	cRangeQueryProcessor<TKey, TNode, TLeafNode> *mRQProcessor;

	std::mutex mReadWriteMutex;

protected:
	// Metods for setting of state of insert
	void SetINSERT_TRAVERSE_UP_EXTERNDS_CHANGED(int &state);
	void UnSetINSERT_TRAVERSE_UP_EXTERNDS_CHANGED(int &state);
    void SetINSERT_TRAVERSE_UP_INDEX_NOT_CHANGED(int &state);
	void SetINSERT_TRAVERSE_EXIT(int &state);
	void SetINSERT_TRAVERSE_UP(int &state);
	void SetINSERT_TRAVERSE_DOWN(int &state);

	// Metods for get of statees during insert
	bool GetINSERT_TRAVERSE_UP_EXTERNDS_CHANGED(int state);
	bool GetINSERT_TRAVERSE_UP_INDEX_NOT_CHANGED(int state);
	bool GetINSERT_TRAVERSE_EXIT(int state);
	bool GetINSERT_TRAVERSE_UP(int state);
	bool GetINSERT_TRAVERSE_DOWN(int state);

protected:
	cRTreeSignatureIndex<TKey> *mSignatureIndex;
	cRTreeOrderIndex<TKey> *mOrderIndex;

private:
	void Insert_pre(unsigned int** currPath, unsigned int** itemOrderCurrPath, cInsertBuffers<TKey>* insertBuffers);
	void Insert_post(cInsertBuffers<TKey>* insertBuffers);
	int InsertIntoLeafNode(TLeafNode* leafNode, const TKey &item, char* leafData, cInsertBuffers<TKey>* insertBuffers);
	void SplitLeafNode(TLeafNode *currentLeafNode, const TKey &item, tNodeIndex &insertNodeIndex, char* leafData, cInsertBuffers<TKey>* insertBuffers);
	void InsertIntoInnerNode(TNode *currentInnerNode, const TKey &item, uint currentLevel, const tNodeIndex& insertNodeIndex, cInsertBuffers<TKey>* insertBuffers);
	void SplitInnerNode(TNode *currentInnerNode, uint currentLevel, tNodeIndex& insertNodeIndex, cInsertBuffers<TKey>* insertBuffers);
	void InsertNewRootNode(const tNodeIndex& nIndex1, const tNodeIndex& nIndex2, cInsertBuffers<TKey>* insertBuffers);
	tNodeIndex InsertFirstItemIntoRtree(const TKey &item, TNode *currentInnerNode, cInsertBuffers<TKey>* insertBuffers);

	bool FindMbr(const TKey &item, tNodeIndex &nodeIndex, TNode *currentInnerNode, unsigned int counter, unsigned int *itemOrderCurrPath, unsigned int& itemOrder, cInsertBuffers<TKey>* insertBuffers);
	bool FindMbr_MP(const TKey &item, TNode *currentInnerNode, cStack<sItemIdRecord>& curPathStack, cStack<sItemIdRecord>& mbrHitStack);
	void FixPathStack(cStack<sItemIdRecord>& curPathStack, cStack<sItemIdRecord>& mbrHitStack);

	void RQStoreContext(cRangeQueryContext *rqContext, unsigned int currentLevel, const TLeafNode &currentLeafNode, unsigned int orderInLeafNode, unsigned int* currPath, unsigned int* itemOrderCurrPath);

	inline const cSpaceDescriptor* GetSpaceDescriptor() const;
	void CheckReadOnly();

	bool Delete(const tNodeIndex& nodeIndex, const TKey& item, uint level);
public:
	cCommonRTree();
	~cCommonRTree();

	bool Open(cRTreeHeader<TKey> *header, cQuickDB* quickDB, bool readOnly = true);
	bool Create(cRTreeHeader<TKey> *header, cQuickDB* quickDB);
	bool Close();
	bool Clear();

	bool PointQuery(TKey &item);
	bool Find(const TKey &item, char *data, cRangeQueryConfig *config, cQueryProcStat *QueryProcStat = NULL);
	bool FindBatchQuery(TKey *items, char* data, cRangeQueryConfig *rqConfig, unsigned int queriesCount = 1, cQueryProcStat* QueryProcStat = NULL); // for multiquery
	bool FindCartesianQuery(cHNTuple *queryTuple, cSpaceDescriptor* queryDescriptor, char* data, cRangeQueryConfig *rqConfig, unsigned int queriesCount = 1, cQueryProcStat* QueryProcStat = NULL); // for multiquery
	bool Find(const TKey &item, float &data);
	int Insert(const TKey &item, char* leafData);
	int Insert_MP(const TKey &item, char* leafData);
	bool Update(const TKey &item);
	bool Update(const TKey &item, float data);

	cTreeItemStream<TKey>* RangeQuery(cRangeQueryConfig *rqConfig, cRangeQueryContext *rqContext);
	cTreeItemStream<TKey>* RangeQuery(const TKey &ql, const TKey &qh, cRangeQueryConfig *rqConfig = NULL, cRangeQueryContext *rqContext = NULL, cQueryProcStat* QueryProcStat = NULL);
	cTreeItemStream<TKey>* BatchRangeQuery(TKey* qls, TKey* qhs, cRangeQueryConfig *rqConfig, unsigned int queriesCount, cRangeQueryContext *rqContext = NULL, cQueryProcStat *QueryProcStat = NULL);
	cTreeItemStream<TKey>* CartesianRangeQuery(cHNTuple* qls, cHNTuple* qhs, cSpaceDescriptor* queryDescriptor, cRangeQueryConfig *rqConfig, unsigned int queriesCount, cRangeQueryContext *rqContext = NULL, cQueryProcStat *QueryProcStat = NULL);

	uint RangeQuery_preSize(unsigned int *nofTrueBitOrders, sBatchRQ *batchRQ);
	char* RangeQuery_preAlloc(unsigned int *nofTrueBitOrders, sBatchRQ *batchRQ, char* buffer, cRQBuffers<TKey> *rqBuffers);

	inline cRTreeHeader<TKey>* GetRTreeHeader();
	inline cRTreeHeader<TKey>& GetRefRTreeHeader();
	inline cRangeQueryProcessor<TKey, TNode, TLeafNode>* GetRangeQueryProcessor();

	void WriteTuples();

	void PrintIndexSize(uint blockSize);
	void PrintRndPath();

	void PrintQueryProcStatistic();
	void PrintAverageQueryProcStatistic();
	void PrintSumQueryProcStatistic();
	void PrintInfo();

	// for Ordered Rtree purpose
	void FTInsertorUpdate (tNodeIndex nodeIndex, const TKey &item);
	void ExtraInsertIntoLeafNode(TLeafNode *newLeafNode);
	void ExtraInsertIntoLeafNode2(TLeafNode *currentLeafNode, const TKey &item, TKey &tmp_item);
	void ExtraSplitInnerNode(unsigned int currentLevel, TNode *currentInnerNode, TNode *newNode);
	void ExtraInsertNewRootNode(TNode* oldRootNode);
	void ExtraInsertNewRootNode2(TNode *newRootNode);
	void ExtraInsertValidation(TNode *currentInnerNode, unsigned int currentLevel);
	int  ExtraInsertExternDSPropagate(TNode *currentInnerNode, const TKey &item, int state, unsigned int currentLevel);
	int ExtraInsertSetFlags(TLeafNode *currentLeafNode, unsigned int currentLevel, const TKey &item, int state, int ret);
	bool ExtraInsertBreak(int state, unsigned int currentLevel, tNodeIndex nodeIndex);

	// for Signature Rtree purpose
	void CreateSignatureIndex(cQuickDB* quickDB);
	void CreateSignatureRecord(const tNodeIndex& nodeIndex, unsigned int level, cInsertBuffers<TKey>* insertBuffers);
	void ComputeSignatureWeights(const tNodeIndex& nodeIndex, unsigned int level, cRQBuffers<TKey>* buffers);
	void RebuildNodeSignatures(const tNodeIndex& nodeIndex, const tNodeIndex& insertNodeIndex, uint invLevel, bool newRoot, cInsertBuffers<TKey>* insertBuffers);
	void ModifyNodeSignature(const tNodeIndex& nodeIndex, unsigned int invLevel, const tNodeIndex& indexedNodeIndex, bool signatureExists, cInsertSigBuffers* buffers);
	void PrintSignatureInfo(bool structuresStats = false);
	void PrintSignatureInfo(uint** uniqueValues, uint* itemsCount, bool structuresStats = false);

	// for Histogram purpose
	void PrintDimDistribution();

	bool Delete(const TKey &item);

	int TMP_BADWEIGHT;
	int TMP_TOTALLEAFS;
	int TMP_ITEMS;
	int TMP_WEIGHTS;

	//for Bulkloading
	TLeafNode* ReadNewLeafNode();
	void UnlockLeafNode(TLeafNode* leafnode);
	TNode* ReadNewNode();
	void UnlockNode(TNode* node);

#ifdef CUDA_ENABLED
private:
	uint mGpuCopyBufferCapacity;
public:
	inline void InitGpu(uint blockSize, uint dim, uint bufferCapacity, uint nodeCapacity);
	inline uint TransferIndexToGpu(uint blockSize);
	inline void TransferIndexToGpu(tNodeIndex nodeIndex, uint level, const uint height);
	inline void TransferIndexToGpu_FlushBuffer(cDbfsLevel* buffer, char* data, uint nodeType);
#endif
};

template<class TMbr, class TKey, class TNode, class TLeafNode>
cCommonRTree<TMbr,TKey,TNode,TLeafNode>::cCommonRTree(): parent(), mSignatureIndex(NULL), mOrderIndex(NULL)
{
	mRQProcessor = new cRangeQueryProcessor<TKey, TNode, TLeafNode>(this);

}

template<class TMbr, class TKey, class TNode, class TLeafNode>
cCommonRTree<TMbr,TKey,TNode,TLeafNode>::~cCommonRTree()
{
	if (mSignatureIndex != NULL)
	{
		delete mSignatureIndex;
		mSignatureIndex = NULL;
	}

	if (mOrderIndex != NULL)
	{
		delete mOrderIndex;
		mOrderIndex = NULL;
	}

	if (mRQProcessor != NULL)
	{
		delete mRQProcessor;
	}
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
inline cRTreeHeader<TKey>* cCommonRTree<TMbr,TKey,TNode,TLeafNode>::GetRTreeHeader()
{
	return (cRTreeHeader<TKey>*)parent::mHeader;
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
inline cRTreeHeader<TKey>& cCommonRTree<TMbr,TKey,TNode,TLeafNode>::GetRefRTreeHeader()
{
	return *((cRTreeHeader<TKey>*)parent::mHeader);
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
inline const cSpaceDescriptor* cCommonRTree<TMbr,TKey,TNode,TLeafNode>::GetSpaceDescriptor() const
{
	return ((cRTreeHeader<TKey>*)parent::mHeader)->GetSpaceDescriptor();
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
bool cCommonRTree<TMbr,TKey,TNode,TLeafNode>::Open(cRTreeHeader<TKey> *header, cQuickDB* quickDB, bool readOnly)
{
	if (!parent::Open(header, quickDB, readOnly))
	{
		return false;
	}

	cRTreeHeader<TKey> **p = &header;
	mRQProcessor->SetQuickDB(quickDB);
	mRQProcessor->SetTreeHeader(header);

	if (header->IsSignatureEnabled()) 
	{
		if (mSignatureIndex == NULL)
		{
			mSignatureIndex = new cRTreeSignatureIndex<TKey>();
			GetRTreeHeader()->SetSignatureIndex(mSignatureIndex);
		}

		if (!mSignatureIndex->Open(header, quickDB, readOnly))
		{
			return false;
		}
	}

	if (header->GetOrderingEnabled()) 
	{
		if (mOrderIndex == NULL)
		{
			mOrderIndex = new cRTreeOrderIndex<TKey>();
		}

		if (!mOrderIndex->Open(header, quickDB, readOnly))
		{
			return false;
		}
	}

	//mRTreeLog.Open(fileName);
	return true;
}

/**
 * Create Empty R-Tree
 **/
template<class TMbr, class TKey, class TNode, class TLeafNode>
bool cCommonRTree<TMbr,TKey,TNode,TLeafNode>::Create(cRTreeHeader<TKey> *header, cQuickDB* quickDB)
{
	if (!parent::Create(header, quickDB))
	{
		return false;
	}

	cRTreeHeader<TKey> **p = &header;

	mRQProcessor->SetQuickDB(quickDB);
	mRQProcessor->SetTreeHeader(header);

	if (header->IsSignatureEnabled()) 
	{
		if (mSignatureIndex == NULL)
		{
			mSignatureIndex = new cRTreeSignatureIndex<TKey>();
			GetRTreeHeader()->SetSignatureIndex(mSignatureIndex);
		}
		if (!mSignatureIndex->Create(header, quickDB))
		{
			return false;
		}
	}

	if (header->GetOrderingEnabled()) 
	{
		if (mOrderIndex == NULL)
		{
			mOrderIndex = new cRTreeOrderIndex<TKey>();
		}

		if (!mOrderIndex->Create(header, quickDB))
		{
			return false;
		}
	}

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

	return true;
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
bool cCommonRTree<TMbr,TKey,TNode,TLeafNode>::Close()
{
	if (!parent::Close())
	{
		return false;
	}
	mRTreeLog.Close();

	if (mSignatureIndex != NULL)
	{
	 	mSignatureIndex->Close();
	}
	return true;
}

/**
 * Create Empty R-Tree. The tree has to be already opened
 **/
template<class TMbr, class TKey, class TNode, class TLeafNode>
bool cCommonRTree<TMbr,TKey,TNode,TLeafNode>::Clear()
{
	if (!cPagedTree<TKey,TNode,TLeafNode>::Clear())
	{
		return false;
	}

	printf("cCommonRTree<TMbr,TKey,TNode,TLeafNode>::Clear() - has to be implemented!\n");
	//// create empty root node
	//mTreePool->GetNode(0)->ClearItemOrder();
	//mTreePool->GetNode(0)->SetIndex(parent::mHeader->NextNodeIndex());
	//mTreePool->GetNode(0)->SetMbrCount(0);
	//mTreePool->GetNode(0)->SetLeaf(false);
	//parent::mHeader->SetRootIndex(mTreePool->GetNode(0)->GetIndex());
	//parent::mHeader->SetNodeCount(1);
	//mCache->WriteNew(mTreePool->GetRefNode(0));

	//// create and fill child node
	//mTreePool->GetLeafNode(0)->ClearItemOrder();
	//mTreePool->GetLeafNode(0)->SetIndex(parent::mHeader->NextNodeIndex());
	//mTreePool->GetLeafNode(0)->SetMbrCount(0);
	//mTreePool->GetLeafNode(0)->SetExtraLink(0, TNode::EMPTY_LINK);
	//parent::mHeader->SetHeight(1);
	//parent::mHeader->SetLeafNodeCount(1);
	//mCache->WriteNewLeaf(mTreePool->GetRefLeafNode(0));

	return true;
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr,TKey,TNode,TLeafNode>::CheckReadOnly()
{
	if (parent::mReadOnly)
	{
		printf("Critical Error: cCommonRTree::Insert(), The tree is read only!\n");
		exit(1);
	}
}

/**
 * Update data of the tuple to be contained in the tree.
 **/
template<class TMbr, class TKey, class TNode, class TLeafNode>
bool cCommonRTree<TMbr,TKey,TNode,TLeafNode>::Update(const TKey &item)
{
	TLeafNode* leafNode;
	bool ret = false;

	cTreeItemStream<TKey>* resultSet = RangeQuery(item, item);
	if (resultSet->GetItemCount() == 1)
	{
		char* item = resultSet->GeItem();
		leafNode = parent::ReadLeafW(mCurrentLeafNodeIndex);
		leafNode->GetMbr(mCurrenTKeyOrder)->SetData(item + parent::mHeader->GetKeyInMemSize());
		parent::mSharedCache->UnlockW(leafNode->GetMemoryBlock());
		resultSet->Next();
		ret = true;
	}
	else
	{
		resultSet->CloseResultSet();
	}

	return ret;
}

/**
 * Update data of the tuple to be contained in the tree.
 **/
template<class TMbr, class TKey, class TNode, class TLeafNode>
bool cCommonRTree<TMbr,TKey,TNode,TLeafNode>::Update(const TKey &item, float data)
{
	TLeafNode* leafNode;
	bool ret = false;

	cTreeItemStream<TKey>* resultSet = RangeQuery(item, item);
	if (resultSet->GetItemCount() == 1)
	{
		char* item = resultSet->GeItem();
		leafNode = parent::ReadLeafW(mCurrentLeafNodeIndex);
		leafNode->GetMbr(mCurrenTKeyOrder)->SetData(item + parent::mHeader->GetKeyInMemSize());
		parent::mShareCache->UnlockW(leafNode->GetMemoryBlock());
		resultSet->Next();
		ret = true;
	}
	else
	{
		resultSet->CloseResultSet();
	}

	return ret;
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr,TKey,TNode,TLeafNode>::WriteTuples()
{
	printf("Rewrite for the new cache!\n");
}


template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::PrintIndexSize(uint blockSize)
{
	printf("Index Size: %.2f", parent::GetIndexSizeMB(blockSize));
	if (GetRTreeHeader()->IsSignatureEnabled())
	{
		mSignatureIndex->PrintIndexSize(blockSize);
	}
	printf(" MB\n");
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
uint cCommonRTree<TMbr, TKey, TNode, TLeafNode>::RangeQuery_preSize(unsigned int *nofTrueBitOrders, sBatchRQ *batchRQ)
{
	uint bufferSize = 0, bsizeRQCart = 0, bsizeSignatures = 0, nofNarrowDims = 0;
	cRTreeHeader<TKey> *header = ((cRTreeHeader<TKey>*)parent::mHeader);
	uint dim = header->GetSpaceDescriptor()->GetDimension(); 
	
	uint hPlus1 = header->GetHeight() + 1;

	if (hPlus1 == 1)
	{
		hPlus1++;  // a special case: an empty R-tree has the height = 1
	}

	if (batchRQ->mode == QueryType::CARTESIANQUERY)
	{
		uint oneAQis = dim * sizeof(cArray<uint>*);
		for (uint i = 0; i < dim; i++)
		{
			char *nt = batchRQ->ql->GetNTuple(i, batchRQ->sd);
			uint len = cNTuple::GetLength((const char*)nt, batchRQ->sd->GetDimSpaceDescriptor(i));
			oneAQis += len * sizeof(uint);
		}
		bsizeRQCart = hPlus1 * oneAQis;
		bufferSize += bsizeRQCart;
	}

	if (header->IsSignatureEnabled()) // if the signatures are used during query processing
	{
		cSignatureController* sigController = header->GetSignatureController();
		uint nOfInvLevels = sigController->GetLevelsCount();

		// get the number of narrow dimensions
		for (unsigned int i = 0; i < dim; i++)
		{
			if (batchRQ->qls[0].Equal(batchRQ->qhs[0], i, header->GetSpaceDescriptor()) == 0)
			{
				nofNarrowDims++;
			}
		}

		bsizeSignatures += sizeof(cArray<uint>) + (/*nofNarrowDims*/ dim * sizeof(uint)); // for array of narrow dimensions
		bsizeSignatures += sizeof(uint*) + (hPlus1 * sizeof(uint));	// for array of level bit count

		// get the number of all true bits in all signatures for inv. levels 
		if (sigController->GetSignatureType() == cSignatureController::DimensionIndependent)
		{
			(*nofTrueBitOrders) = 0;
			for (uint i = 0; i < nOfInvLevels; i++)
			{
				cSignatureParams* sigParams = sigController->GetSignatureParams(i);
				(*nofTrueBitOrders) += nofNarrowDims * sigParams->GetBitCount();
			}
		}
		else
		{
			nofNarrowDims = 0;
			uint nofRQBits = 1;
			for (uint i = 0; i < dim; i++)
			{
				if (sigController->GetQueryType(sigController->GetCurrentQueryType())[i])
				{
					nofNarrowDims++;
					nofRQBits *= ((batchRQ->qhs[0].GetUInt(i, header->GetSpaceDescriptor()) - batchRQ->qls[0].GetUInt(i, header->GetSpaceDescriptor())) + 1);
				}
			}

			sigController->SetRQBits(nofRQBits);
			(*nofTrueBitOrders) = 0;
			for (uint i = 0; i < nOfInvLevels; i++)
			{
				cSignatureParams* sigParams = sigController->GetSignatureParams(i);
				(*nofTrueBitOrders) += nofRQBits * sigParams->GetBitCount();
			}

		}

		bsizeSignatures += sizeof(cArray<ullong>) + (*nofTrueBitOrders) * sizeof(ullong);
		bufferSize += bsizeSignatures;
		bufferSize += mSignatureIndex->Query_presize();
	}

	return bufferSize;
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
char* cCommonRTree<TMbr, TKey, TNode, TLeafNode>::RangeQuery_preAlloc(unsigned int *nofTrueBitOrders, sBatchRQ *batchRQ, char* buffer, cRQBuffers<TKey> *rqBuffers)
{
	cRTreeHeader<TKey> *header = ((cRTreeHeader<TKey>*)parent::mHeader);
	uint dim = header->GetSpaceDescriptor()->GetDimension();
	// char* pBuffer = buffer; 
	uint hPlus1 = header->GetHeight() + 1;

	if (hPlus1 == 1)
	{
		hPlus1++;  // a special case: an empty R-tree has the height = 1
	}

	if (batchRQ->mode == QueryType::CARTESIANQUERY)
	{
		rqBuffers->aqis = (cArray<uint>***)buffer;
		buffer += sizeof(cArray<uint>**) * hPlus1;
		for (uint i = 0; i < hPlus1; i++)
		{
			rqBuffers->aqis[i] = (cArray<uint>**)buffer;
			buffer += sizeof(cArray<uint>*) * dim;
			for (uint j = 0; j < dim; j++)
			{
				rqBuffers->aqis[i][j] = (cArray<uint>*)buffer;
				buffer += sizeof(cArray<uint>);

				char* nt = batchRQ->ql->GetNTuple(j, batchRQ->sd);
				uint len = cNTuple::GetLength((const char*)nt, batchRQ->sd->GetDimSpaceDescriptor(j));

				rqBuffers->aqis[i][j]->Init(buffer, len);

				if (i == 0)  // init aqis for the first level
				{
					for (uint k = 0; k < len; k++)
					{
						rqBuffers->aqis[0][j]->Add(k);
					}
				}
				buffer += len * sizeof(uint);
			}
		}
	}

	if (header->IsSignatureEnabled()) // if the signatures are used during query processing
	{
		// signatures of queries for particular levels
		cSignatureController* sigController = header->GetSignatureController();
		unsigned int nOfInvLevels = sigController->GetLevelsCount();

		// the array of narrow dimensions order
		cArray<uint>* narrowDims = (cArray<uint>*)buffer;
		buffer += sizeof(cArray<uint>);
		narrowDims->Init(buffer, dim);

		if (sigController->GetSignatureType() == cSignatureController::DimensionIndependent)
		{
			for (uint i = 0; i < dim; i++)
			{
				if (batchRQ->qls[0].Equal(batchRQ->qhs[0], i, header->GetSpaceDescriptor()) == 0)
				{
					narrowDims->Add(i);
				}
			}
		}
		else
		{
			for (uint i = 0; i < dim; i++)
			{
				if (sigController->GetQueryType(sigController->GetCurrentQueryType())[i])
				{
					narrowDims->Add(i);
				}
			}
		}
		rqBuffers->NarrowDimensions = narrowDims;
		buffer += dim * sizeof(uint);

		// the array of true bit orders - a representation of the query signature
		cArray<ullong>* trueBitOrders = (cArray<ullong>*)buffer;
		buffer += sizeof(cArray<ullong>);
		trueBitOrders->Init(buffer, (*nofTrueBitOrders));
		buffer += (*nofTrueBitOrders) * sizeof(ullong);
		rqBuffers->QueryTrueBitOrders = trueBitOrders;

		uint* nOfLevelBits = new(buffer) uint[hPlus1];
		buffer += sizeof(uint*) +(hPlus1 * sizeof(uint));
		rqBuffers->nOfLevelBits = nOfLevelBits;

//		mSignatureIndex->CreateQuerySignature(batchRQ->qls[0], batchRQ->qhs[0], rqBuffers);
		mSignatureIndex->Query_pre(buffer, rqBuffers);
		buffer += mSignatureIndex->Query_presize();
	}
	return buffer;
}

/**
 * Method printing the first path of the R-tree.
 **/
template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr,TKey,TNode,TLeafNode>::PrintRndPath()
{
	printf("Rewrite for the new cache!\n");
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr,TKey,TNode,TLeafNode>::PrintInfo()
{
	cPagedTree<TKey,TNode,TLeafNode>::PrintInfo();
//	printf("Dimension:             %d\t Type:  %c\n", GetRtreeHeader()->GetSpaceDescriptor()->GetDimension(), GetRtreeHeader()->GetSpaceDescriptor()->GetType(0)->GetCode());
	printf("Root region:           ");
	TKey::Print(GetRTreeHeader()->GetTreeMBR()->GetLoTuple()->GetData(), " x ", GetRTreeHeader()->GetSpaceDescriptor());
	TKey::Print(GetRTreeHeader()->GetTreeMBR()->GetHiTuple()->GetData(), "\n\n", GetRTreeHeader()->GetSpaceDescriptor());

	/*if (((cRTreeHeader<TKey>*)parent::mHeader)->IsSignatureEnabled())
	{
		mSignatureIndex->PrintInfo();
	}*/
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr,TKey,TNode,TLeafNode>::PrintQueryProcStatistic()
{
	/*char *countersText[] = { "query result size", "\t", "searched regions", "\n", "first point tests", "\t",
		"match", "\n", "intersect tests", "\t", "match (linear)", "\t", "match (exponencial)", "\n",
		"average intersect", "\t", "average sequence intersect", "\n\n",
		"leaf optim first point", "\t", "match", "\n", "intersect leaf optim", "\t",
		"match (linear)", "\t", "match (exponencial)", "\n",
		"complexity", "\t\t",	"classical complexity", "\t", "naive", "\n \t\t\t", 
		"classical dac", "\t\t", "naive", "\n",
		"intersect (linear)", "\t", "match (linear)", "\n"};

	char *timersText[] = { "search regions", "\n", "first point test", "\n",
		"intersect test (linear)", "\t", "exponencial", "\n", "leaf optim first point", "\n",
		"leaf optim intersect (linear)", "\t", "exponencial", "\n",
		"intersect (linear)", "\n"};

	printf("-------------------------------------------------------------------------\nNumber of:\n");

	unsigned int ind;
	for (unsigned int i = 0 ; i < parent::mHeader->GetQueryProcStatistics()->GetCounterCount() ; i++)
	{
		printf("%s: ", countersText[2*i]);
		if (i == cRTreeConst::Counter_test_glob_intersectCount || i == cRTreeConst::Counter_test_glob_seqIntersectCount)
		{
			if (i == cRTreeConst::Counter_test_glob_intersectCount) 
			{
				ind = cRTreeConst::Counter_test_intersectCount;
			}
			else
			{
				ind = cRTreeConst::Counter_test_seqIntersectCount;
			}
			printf("%g", parent::mHeader->GetQueryProcStatistics()->GetCounter(ind)->GetAverage());
		}
		else
		{
			ind = i;
			printf("%d", parent::mHeader->GetQueryProcStatistics()->GetCounter(ind)->GetValue());
		}
		printf("%s", countersText[2*i+1]);
	}

	unsigned int intrNumber = parent::mHeader->GetQueryProcStatistics()->GetCounter(cRTreeConst::Counter_test_intersect)->GetValue() + 
		parent::mHeader->GetQueryProcStatistics()->GetCounter(cRTreeConst::Counter_leafopt_intersect)->GetValue();
	unsigned int matchNumber = parent::mHeader->GetQueryProcStatistics()->GetCounter(cRTreeConst::Counter_test_intersectLin_ok)->GetValue() + 
		parent::mHeader->GetQueryProcStatistics()->GetCounter(cRTreeConst::Counter_leafopt_intersectLin_ok)->GetValue();

	printf("%s: %d%s", countersText[2*i], intrNumber, countersText[2*i+1]);
	printf("%s: %d%s", countersText[2*(i+1)], matchNumber, countersText[2*(i+1)+1]);

	printf("-------------------------------------------------------------------------\nTime of:\n");

	for (unsigned int i = 0 ; i < parent::mHeader->GetQueryProcStatistics()->GetTimerCount() ; i++)
	{
		printf("%s: %gs", timersText[2*i], parent::mHeader->GetQueryProcStatistics()->GetTimer(i)->GetProcessTime());
		printf("%s", timersText[2*i+1]);
	}

	double intrTime = (parent::mHeader->GetQueryProcStatistics()->GetTimer(cRTreeConst::Timer_leafopt_intersectLin)->GetProcessTime()+
		parent::mHeader->GetQueryProcStatistics()->GetTimer(cRTreeConst::Timer_test_intersectLin)->GetProcessTime()) / 2.0;
	printf("%s: %g%s", timersText[2*i], intrTime, timersText[2*i+1]);*/
	printf("****************************** Query Statistics: ******************************\n");
	printf("Result size: %d\n", parent::mHeader->GetQueryProcStatistics()->GetCounter(cRTreeConst::Counter_resultSize)->GetValue());
	unsigned int searchedRegions = parent::mQueryProcStatistics->GetCounter(cRTreeConst::Counter_searchedRegions)->GetValue();
	unsigned int relevantRegions = parent::mQueryProcStatistics->GetCounter(cRTreeConst::Counter_relevantRegions)->GetValue();
	printf("Number of: searched regions: %d [%.5f%%] \t relevant regions: %d [%.5f%%] \n", searchedRegions, ((float)searchedRegions/parent::mHeader->GetLeafNodeCount())*100.0,
		relevantRegions, ((float)relevantRegions/parent::mHeader->GetLeafNodeCount())*100.0);
	printf("cq=%.5f\n", (float)relevantRegions/searchedRegions);
	printf("Query time: ");
	parent::mQueryProcStatistics->GetTimer(cRTreeConst::Timer_queryTime)->Print("\n");
	printf("Time of searched regions: ");
	parent::mQueryProcStatistics->GetTimer(cRTreeConst::Timer_searchRegions)->Print("\n");

	for (unsigned int i = 0 ; i < NUMBER_OF_TIMERS ; i++)
	{
		if (i == 0)
		{
			printf("Time of reading nodes: ");
		}
		else if (i == 1)
		{
			printf("Time of searching leaf nodes: ");
		}
		else if (i == 2)
		{
			printf("Time of searching relevant MBBs: ");
		}
		mTimers[i].Print("\n");
	}

	printf("Time of copying nodes: ");
	parent::mQueryProcStatistics->mTimer1->Print("\n");
	printf("Time of copying items: ");
	parent::mQueryProcStatistics->mTimer2->Print("\n");
	printf("Time of comparing items: ");
	parent::mQueryProcStatistics->mTimer3->Print("\n");

}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr,TKey,TNode,TLeafNode>::PrintAverageQueryProcStatistic()
{
	/*char *countersText[] = { "query result size", "\t", "search of regions", "\n", "first point tests", "\t",
		"match", "\n", "intersect tests", "\t", "match (linear)", "\t", "match (exponencial)", "\n",
		"average intersect", "\t", "average sequence intersect", "\n\n",
		"leaf optim first point", "\t", "match", "\n", "leaf optim intersect", "\t", 
		"match (linear)", "\t", "match (exponencial)", "\n",
		"complexity", "\t\t",	"classical complexity", " \t", "naive", "\n \t\t\t", 
		"classical dac", "\t\t", "naive", "\n",
		"intersect (linear)", "\t", "match (linear)", "\n"};

	char *timersText[] = { "search of regions", "\n", "first point test", "\n",
		"intersect test (linear)", " \t", "exponencial", "\n", 
		"leaf optim first point", "\n",	"leaf optim intersect (linear)", "\t\t", "exponencial", "\n",
		"intersect (linear)", "\n"};

	printf("-------------------------------------------------------------------------\n");
	printf("Average number of: \n");
	for (unsigned int i = 0 ; i < parent::mHeader->GetQueryProcStatistics()->GetCounterCount() ; i++) {
		printf("%s: %g", countersText[2*i], parent::mHeader->GetQueryProcStatistics()->GetCounter(i)->GetAverage());
		printf("%s", countersText[2*i+1]);
	}

	double intrNumber = parent::mHeader->GetQueryProcStatistics()->GetCounter(cRTreeConst::Counter_test_intersect)->GetAverage() + 
		parent::mHeader->GetQueryProcStatistics()->GetCounter(cRTreeConst::Counter_leafopt_intersect)->GetAverage();
	double matchNumber = parent::mHeader->GetQueryProcStatistics()->GetCounter(cRTreeConst::Counter_test_intersectLin_ok)->GetAverage() + 
		parent::mHeader->GetQueryProcStatistics()->GetCounter(cRTreeConst::Counter_leafopt_intersectLin_ok)->GetAverage();

	printf("%s: %g%s", countersText[2*i], intrNumber, countersText[2*i+1]);
	printf("%s: %g%s", countersText[2*(i+1)], matchNumber, countersText[2*(i+1)+1]);

	printf("-------------------------------------------------------------------------\n");
	printf("Average time of: \n");
	for (unsigned int i = 0 ; i < parent::mHeader->GetQueryProcStatistics()->GetTimerCount() ; i++)
	{
		printf("%s: %gs", timersText[2*i], parent::mHeader->GetQueryProcStatistics()->GetTimer(i)->GetAverageProcessTime());
		printf("%s", timersText[2*i+1]);
	}

	double intrTime = parent::mHeader->GetQueryProcStatistics()->GetTimer(cRTreeConst::Timer_leafopt_intersectLin)->GetAverageProcessTime()+
		parent::mHeader->GetQueryProcStatistics()->GetTimer(cRTreeConst::Timer_test_intersectLin)->GetAverageProcessTime();
	printf("%s: %g%s", timersText[2*i], intrTime, timersText[2*i+1]);*/
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr,TKey,TNode,TLeafNode>::PrintSumQueryProcStatistic()
{
	/*char *countersText[] = { "query result size", "\t", "search of regions", "\n", "first point tests", "\t",
		"match", "\n", "intersect tests", "\t", "match (linear)", "\t", "match (exponencial)", "\n",
		"average intersect", "\t", "average sequence intersect", "\n\n",
		"leaf optim first point", "\t", "match", "\n", "leaf optim intersect", "\t", 
		"match (linear)", "\t", "match (exponencial)", "\n",
		"complexity", "\t\t",	"classical complexity", " \t", "naive", "\n \t\t\t", 
		"classical dac", "\t\t", "naive", "\n",
		"intersect (linear)", "\t", "match (linear)", "\n"};

	char *timersText[] = { "search of regions", "\n", "first point test", "\n",
		"intersect test (linear)", " \t", "exponencial", "\n", 
		"leaf optim first point", "\n",	"leaf optim intersect (linear)", "\t\t", "exponencial", "\n",
		"intersect (linear)", "\n"};

	printf("-------------------------------------------------------------------------\n");
	printf("Sum number of: \n");
	for (unsigned int i = 0 ; i < parent::mHeader->GetQueryProcStatistics()->GetCounterCount() ; i++) {
		printf("%s: %g", countersText[2*i], parent::mHeader->GetQueryProcStatistics()->GetCounter(i)->GetSum());
		printf("%s", countersText[2*i+1]);
	}

	double intrNumber = (parent::mHeader->GetQueryProcStatistics()->GetCounter(cRTreeConst::Counter_test_intersect)->GetSum() + 
		parent::mHeader->GetQueryProcStatistics()->GetCounter(cRTreeConst::Counter_leafopt_intersect)->GetSum()) / 2;
	double matchNumber = (parent::mHeader->GetQueryProcStatistics()->GetCounter(cRTreeConst::Counter_test_intersectLin_ok)->GetSum() + 
		parent::mHeader->GetQueryProcStatistics()->GetCounter(cRTreeConst::Counter_leafopt_intersectLin_ok)->GetSum()) / 2;

	printf("%s: %g%s", countersText[2*i], intrNumber, countersText[2*i+1]);
	printf("%s: %g%s", countersText[2*(i+1)], matchNumber, countersText[2*(i+1)+1]);

	printf("-------------------------------------------------------------------------\n");
	printf("Sum time of: \n");
	for (unsigned int i = 0 ; i < parent::mHeader->GetQueryProcStatistics()->GetTimerCount() ; i++)
	{
		printf("%s: %gs", timersText[2*i], parent::mHeader->GetQueryProcStatistics()->GetTimer(i)->GetSumProcessTime());
		printf("%s", timersText[2*i+1]);
	}

	double intrTime = (parent::mHeader->GetQueryProcStatistics()->GetTimer(cRTreeConst::Timer_leafopt_intersectLin)->GetSumProcessTime()+
		parent::mHeader->GetQueryProcStatistics()->GetTimer(cRTreeConst::Timer_test_intersectLin)->GetSumProcessTime()) / 2.0;
	printf("%s: %g%s", timersText[2*i], intrTime, timersText[2*i+1]);*/
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::PrintDimDistribution()
{
	char* minValues = GetRTreeHeader()->GetTreeMBR()->GetLoTuple()->GetData();
	char* maxValues = GetRTreeHeader()->GetTreeMBR()->GetHiTuple()->GetData();
	parent::PrintDimDistribution(minValues, maxValues);
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::CreateSignatureIndex(cQuickDB* quickDB)
{
	if (mSignatureIndex == NULL)
	{
		mSignatureIndex->Create(GetRTreeHeader(), quickDB);
	}

	cInsertBuffers<TKey> insertBuffers;

	unsigned int bufferSize = sizeof(cArray<uint>) + (parent::mHeader->GetHeight() + 1) * sizeof(uint);
	bufferSize += mSignatureIndex->Insert_presize();

	cMemoryBlock* bufferMemBlock = parent::mQuickDB->GetMemoryManager()->GetMem(bufferSize);
	insertBuffers.bufferMemBlock = bufferMemBlock;
	char* buffer = bufferMemBlock->GetMem();

	insertBuffers.CurrentPath = (cArray<uint>*)buffer;
	buffer += sizeof(cArray<uint>);
	insertBuffers.CurrentPath->Init(buffer, parent::mHeader->GetHeight() + 1);
	buffer += (parent::mHeader->GetHeight() + 1) * sizeof(uint);
	mSignatureIndex->Insert_pre(buffer, &insertBuffers.signatureBuffers);

	CreateSignatureRecord(parent::mHeader->GetRootIndex(), 0, &insertBuffers);

	parent::mQuickDB->GetMemoryManager()->ReleaseMem(bufferMemBlock);
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::CreateSignatureRecord(const tNodeIndex& nodeIndex, unsigned int level, cInsertBuffers<TKey>* insertBuffers)
{
	TNode* currentNode = NULL;
	TLeafNode* currentLeafNode = NULL;

	if (TNode::IsLeaf(nodeIndex))
	{
		currentLeafNode = parent::ReadLeafNodeR(nodeIndex);
		insertBuffers->CurrentPath->Add(currentLeafNode->GetIndex());

		mSignatureIndex->CreateNodeSignature(currentLeafNode, insertBuffers->CurrentPath,  &insertBuffers->signatureBuffers);
		parent::mSharedCache->UnlockR(currentLeafNode);
	}
	else
	{
		currentNode = parent::ReadInnerNodeR(nodeIndex);
		insertBuffers->CurrentPath->Add(currentNode->GetIndex());

		for (unsigned int i = 0; i < currentNode->GetItemCount(); i++)
		{
			CreateSignatureRecord(currentNode->GetLink(i), level + 1, insertBuffers);
		}
		parent::mSharedCache->UnlockR(currentNode);
	}

	insertBuffers->CurrentPath->SetCount(insertBuffers->CurrentPath->Count() - 1);
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::PrintSignatureInfo(uint** uniqueValues, uint* itemsCount, bool structuresStats)
{
	const cDTDescriptor * sd = parent::mHeader->GetNodeHeader(cTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor();
	uint dimension = TKey::GetDimension(sd);

	if (mSignatureIndex->IsOpen())
	{
		cRQBuffers<TKey> rqBuffers;
		uint bufferSize = mSignatureIndex->Query_presize();
		cMemoryBlock* bufferMemBlock = parent::mQuickDB->GetMemoryManager()->GetMem(bufferSize);
		char* buffer = bufferMemBlock->GetMem();
		mSignatureIndex->Query_pre(buffer, &rqBuffers);

		TMP_BADWEIGHT = 0;
		TMP_TOTALLEAFS = 0;
		TMP_ITEMS = 0;
		TMP_WEIGHTS = 0;
		ComputeSignatureWeights(parent::mHeader->GetRootIndex(), 0, &rqBuffers);

		//printf("Leafs:%d -> Bad Leafs:%d -> Total items:%d -> Totals true bits:%d", TMP_TOTALLEAFS, TMP_BADWEIGHT, TMP_ITEMS, TMP_WEIGHTS);
		parent::mQuickDB->GetMemoryManager()->ReleaseMem(bufferMemBlock);
	}

	mSignatureIndex->PrintInfo(uniqueValues, itemsCount, parent::mHeader->GetHeight(), structuresStats);
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::PrintSignatureInfo(bool structuresStats)
{
	const cDTDescriptor * sd = parent::mHeader->GetNodeHeader(cTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor();
	uint dimension = TKey::GetDimension(sd);
	char* minValues = GetRTreeHeader()->GetTreeMBR()->GetLoTuple()->GetData();
	char* maxValues = GetRTreeHeader()->GetTreeMBR()->GetHiTuple()->GetData();

	parent::CreateDimDistribution(minValues, maxValues);

	if (mSignatureIndex->IsOpen())
	{
		cRQBuffers<TKey> rqBuffers;
		uint bufferSize = mSignatureIndex->Query_presize();
		cMemoryBlock* bufferMemBlock = parent::mQuickDB->GetMemoryManager()->GetMem(bufferSize);
		char* buffer = bufferMemBlock->GetMem();
		mSignatureIndex->Query_pre(buffer, &rqBuffers);

		TMP_BADWEIGHT = 0;
		TMP_TOTALLEAFS = 0;
		TMP_ITEMS = 0;
		TMP_WEIGHTS = 0;
		ComputeSignatureWeights(parent::mHeader->GetRootIndex(), 0, &rqBuffers);

		//printf("Leafs:%d -> Bad Leafs:%d -> Total items:%d -> Totals true bits:%d", TMP_TOTALLEAFS, TMP_BADWEIGHT, TMP_ITEMS, TMP_WEIGHTS);
		parent::mQuickDB->GetMemoryManager()->ReleaseMem(bufferMemBlock);
	}

	mSignatureIndex->PrintInfo(parent::mUniqueValuesCount, parent::mItemsCount, parent::mHeader->GetHeight(), structuresStats);

	parent::DeleteDimDistribution();
}


template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::ComputeSignatureWeights(const tNodeIndex& nodeIndex, uint level, cRQBuffers<TKey>* buffers/*cTuple* keyBuffer*/)
{
	TNode* currentNode = NULL;
	TLeafNode* currentLeafNode = NULL;

	if (TNode::IsLeaf(nodeIndex))
	{
		currentLeafNode = parent::ReadLeafNodeR(nodeIndex);
		/*int weight = */mSignatureIndex->ComputeWeight(currentLeafNode->GetIndex(), parent::mHeader->GetHeight() - level, buffers);
		//if (weight < currentLeafNode->GetItemCount())
		//{
		//	TMP_BADWEIGHT++;
		//	//printf("%d != %d\n", weight, currentLeafNode->GetItemCount());
		//	//currentLeafNode->Print2File("D:\\111.txt");
		//}

		TMP_TOTALLEAFS++;
		TMP_ITEMS += currentLeafNode->GetItemCount();
		//TMP_WEIGHTS += weight;
		/*if ((currentLeafNode->GetIndex() == 1200) || (currentLeafNode->GetIndex() == 2219))
		{
			currentLeafNode->Print2File("D:\\128.txt");
		}*/
		/*if (TMP_TOTALLEAFS % 1000 == 0)
		{
			printf("%d; ", TMP_TOTALLEAFS);
		}*/

		parent::mSharedCache->UnlockR(currentLeafNode);
	}
	else
	{
		currentNode = parent::ReadInnerNodeR(nodeIndex);
		
		for (unsigned int i = 0; i < currentNode->GetItemCount(); i++)
		{
			ComputeSignatureWeights(currentNode->GetLink(i), level+1, buffers);
		}

		mSignatureIndex->ComputeWeight(currentNode->GetIndex(), parent::mHeader->GetHeight() - level, buffers);
		parent::mSharedCache->UnlockR(currentNode);
	}
}


template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::RebuildNodeSignatures(const tNodeIndex& nodeIndex, const tNodeIndex& insertNodeIndex, uint invLevel, bool newRoot, cInsertBuffers<TKey>* insertBuffers)
{
	cRTreeHeader<TKey>* header = GetRTreeHeader();

	// DELETE AFTER
	/*if (cTreeNode<tKey>::Inner_Splits_Count >= 1)
	{
		mTree->GetRTreeHeader()->GetSignatureController()->SetSignatureQuality(cSignatureController::PerfectSignature);
	}*/

	if (header->IsSignatureEnabled() && ((header->GetSignatureController()->GetBuildType() == cSignatureController::SignatureBuild_Insert)) && 
		mSignatureIndex->IsEnabled(invLevel))
	{
		if ((header->GetSignatureController()->GetSignatureQuality() == cSignatureController::PerfectSignature) || (newRoot))
		{
			ModifyNodeSignature(nodeIndex, invLevel, nodeIndex, true, &insertBuffers->signatureBuffers);
			ModifyNodeSignature(insertNodeIndex, invLevel, insertNodeIndex, false, &insertBuffers->signatureBuffers);

			// DELETE AFTER
			/*cTreeNode<TKey>::Inner_Splits_Count = 0;
			mTree->GetRTreeHeader()->GetSignatureController()->SetSignatureQuality(cSignatureController::ImperfectSignature);*/
		}
		else
		{
			mSignatureIndex->ReplicateNodeSignature(insertNodeIndex, nodeIndex, invLevel, &insertBuffers->signatureBuffers);

			printf("pruser");
			// DELETE AFTER
			//cTreeNode<TKey>::Inner_Splits_Count++;
		}
	}
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::ModifyNodeSignature(const tNodeIndex& nodeIndex, unsigned int invLevel, const tNodeIndex& indexedNodeIndex, bool signatureExists, cInsertSigBuffers* buffers)
{
	TNode* currentNode = NULL;
	TLeafNode* currentLeafNode = NULL;

	if (TNode::IsLeaf(nodeIndex))
	{
		currentLeafNode = parent::ReadLeafNodeR(nodeIndex);
		mSignatureIndex->CreateNodeSignature(currentLeafNode, invLevel, indexedNodeIndex, buffers);
		parent::mSharedCache->UnlockR(currentLeafNode);
	}
	else
	{
		currentNode = parent::ReadInnerNodeR(nodeIndex);
		if ((signatureExists) && (nodeIndex == indexedNodeIndex)) 
		{
			mSignatureIndex->ClearNodeSignature(nodeIndex, invLevel, buffers);
		}

		for (unsigned int i = 0; i < currentNode->GetItemCount(); i++)
		{
			ModifyNodeSignature(currentNode->GetLink(i), invLevel, indexedNodeIndex, signatureExists, buffers);
		}
		parent::mSharedCache->UnlockR(currentNode);
	}
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::SetINSERT_TRAVERSE_UP_EXTERNDS_CHANGED(int &state)
{
	state = (state | INSERT_TRAVERSE_UP_EXTERNDS_CHANGED);
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
cRangeQueryProcessor<TKey, TNode, TLeafNode>* cCommonRTree<TMbr,TKey,TNode,TLeafNode>::GetRangeQueryProcessor()
{
	return mRQProcessor;
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::UnSetINSERT_TRAVERSE_UP_EXTERNDS_CHANGED(int &state)
{
	state =	state & (~INSERT_TRAVERSE_UP_EXTERNDS_CHANGED);
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::SetINSERT_TRAVERSE_UP_INDEX_NOT_CHANGED(int &state)
{
	state = (INSERT_TRAVERSE_UP_INDEX_NOT_CHANGED | (state & INSERT_TRAVERSE_UP_EXTERNDS_CHANGED));
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::SetINSERT_TRAVERSE_EXIT(int &state)
{
	state = INSERT_TRAVERSE_EXIT;
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::SetINSERT_TRAVERSE_UP(int &state)
{
	state = (INSERT_TRAVERSE_UP | (state & INSERT_TRAVERSE_UP_EXTERNDS_CHANGED));
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::SetINSERT_TRAVERSE_DOWN(int &state)
{
	state = (INSERT_TRAVERSE_DOWN | (state & INSERT_TRAVERSE_UP_EXTERNDS_CHANGED));
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
bool cCommonRTree<TMbr, TKey, TNode, TLeafNode>::GetINSERT_TRAVERSE_UP_EXTERNDS_CHANGED(int state)
{
	return ((state & INSERT_TRAVERSE_UP_EXTERNDS_CHANGED)==INSERT_TRAVERSE_UP_EXTERNDS_CHANGED);
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
bool cCommonRTree<TMbr, TKey, TNode, TLeafNode>::GetINSERT_TRAVERSE_UP_INDEX_NOT_CHANGED(int state)
{
	return ((state & INSERT_TRAVERSE_UP_INDEX_NOT_CHANGED)==INSERT_TRAVERSE_UP_INDEX_NOT_CHANGED);
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
bool cCommonRTree<TMbr, TKey, TNode, TLeafNode>::GetINSERT_TRAVERSE_EXIT(int state)
{
	return ((state & INSERT_TRAVERSE_EXIT)==INSERT_TRAVERSE_EXIT);
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
bool cCommonRTree<TMbr, TKey, TNode, TLeafNode>::GetINSERT_TRAVERSE_UP(int state)
{
	return ((state & INSERT_TRAVERSE_UP)==INSERT_TRAVERSE_UP);
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
bool cCommonRTree<TMbr, TKey, TNode, TLeafNode>::GetINSERT_TRAVERSE_DOWN(int state)
{
	return ((state & INSERT_TRAVERSE_DOWN)==INSERT_TRAVERSE_DOWN);
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::
FTInsertorUpdate (tNodeIndex nodeIndex, const TKey &item)
{
	if (GetRTreeHeader()->GetOrderingEnabled()) 
	{
		mOrderIndex->InsertOrUpdateTuple(nodeIndex, item);
	}
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::
ExtraInsertIntoLeafNode(TLeafNode *newLeafNode)
{
	if (GetRTreeHeader()->GetOrderingEnabled()) 
	{
		mOrderIndex->InsertOrUpdateTuple(newLeafNode->GetIndex(), newLeafNode->GetItem(0)->GetRefTuple());
	}
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::
ExtraInsertIntoLeafNode2(TLeafNode *currentLeafNode, const TKey &item, TKey& tmpItem)
{
	if (GetRTreeHeader()->GetOrderingEnabled()) 
	{
		if (currentLeafNode->GetItemCount() == 1 
			|| 
		   (TKey::Compare(item, currentLeafNode->GetCKey(0), currentLeafNode->GetNodeHeader()->GetKeyDescriptor())) == 0)
		{
			tmpItem.Copy(currentLeafNode->GetCKey(0), currentLeafNode->GetNodeHeader()->GetKeyDescriptor());
			//tmpItem.Print("\n", currentLeafNode->GetNodeHeader()->GetKeyDescriptor());
			mOrderIndex->InsertOrUpdateTuple(currentLeafNode->GetIndex(), tmpItem);
		}
	}
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::
ExtraSplitInnerNode(unsigned int currentLevel, TNode *currentInnerNode, TNode *newNode)
{
	if (GetRTreeHeader()->GetOrderingEnabled()) 
	{
		// new: insert FT of the new inner node
		unsigned int pom = newNode->GetLink(0) & 0x7fffffff;
		mOrderIndex->InsertOrUpdateTuple(newNode->GetIndex(), *(mOrderIndex->GetTuple(pom, GetSpaceDescriptor())));
		if (parent::mDebug)
		{
			mOrderIndex->PrintFT(newNode->GetIndex(), GetSpaceDescriptor());
		}
		/*
		// Kontrola ordering, jina podle toho, zda se pouziva usporadani
		if (!currentInnerNode->IsOrderedFirstTuple(mOrderIndex))
		{
			printf("Critical Error: (IsOrderedFirstTuple - 1) Node is not Ordered after the split operation!");
			currentInnerNode->Print();
		}
		if (!newNode->IsOrderedFirstTuple(mOrderIndex))
		{
			printf("Critical Error: (IsOrderedFirstTuple - 2) Node is not Ordered after the split operation!");
			newNode->Print();
		}
		/**/
	}
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::
ExtraInsertNewRootNode(TNode* oldRootNode)
{
	if (GetRTreeHeader()->GetOrderingEnabled()) 
	{
		// new: insert FT of the old root node
		mOrderIndex->InsertOrUpdateTuple(oldRootNode->GetIndex(), *(mOrderIndex->GetTuple(oldRootNode->GetLink(0), GetSpaceDescriptor())));
		if (parent::mDebug)
		{
			mOrderIndex->PrintFT(oldRootNode->GetIndex(), GetSpaceDescriptor());
		}
	}
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::
ExtraInsertNewRootNode2(TNode *newRootNode)
{
	if (GetRTreeHeader()->GetOrderingEnabled()) 
	{
		/*
		// new: check the ordering the new root node
		if (!newRootNode->IsOrderedFirstTuple(mOrderIndex))
		{
			printf("Critical Error: cCommonRTree::InsertNewRootNode(): The new root node is not ordered!\n");
		}
		/**/
	}
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::
ExtraInsertValidation(TNode *currentInnerNode, unsigned int currentLevel)
{
	if (GetRTreeHeader()->GetOrderingEnabled()) 
	{
		/*
		if (!currentInnerNode->IsOrderedFirstTuple(mOrderIndex))
		{
			printf("Critical Error: (IsOrderedFirstTuple - 3) Node is not ordered!");
			currentInnerNode->Print();
		}

		if (parent::mDebug)
		{
			printf("currentInnerNode:\n");
			currentInnerNode->Print();
			currentInnerNode->PrintFirstTuple(mOrderIndex);
		}
		*/
	}
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
int cCommonRTree<TMbr, TKey, TNode, TLeafNode>::
ExtraInsertExternDSPropagate(TNode *currentInnerNode, const TKey &item, int state, unsigned int currentLevel)
{
	/*
	 mk?: nelze prelozit
	if (GetRTreeHeader()->GetOrderingEnabled()) 
	{
		if (GetINSERT_TRAVERSE_UP_EXTERNDS_CHANGED(state)) //changeFT
		{
			if (currentInnerNode->GetLink(0) == mTreePool->GetNodeIndex(currentLevel+1))
			{
				if (currentInnerNode->GetIndex() != GetRTreeHeader()->GetRootIndex())
				{
					mdOrderIndex->InsertOrUpdateTuple(currentInnerNode->GetIndex(), item.GetRefTuple());
				}
			}
			else
			{
				UnSetINSERT_TRAVERSE_UP_EXTERNDS_CHANGED(state); //changeFT = false;
			}
		}
	}
	return state;
	 */
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
TLeafNode* cCommonRTree<TMbr, TKey, TNode, TLeafNode>::ReadNewLeafNode()
{
	TLeafNode *leafNode = parent::ReadNewLeafNode();
	parent::mHeader->IncrementLeafNodeCount();
	return leafNode;
}


template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::UnlockLeafNode(TLeafNode* leafNode)
{
	parent::mSharedCache->UnlockW(leafNode);
}


template<class TMbr, class TKey, class TNode, class TLeafNode>
TNode* cCommonRTree<TMbr, TKey, TNode, TLeafNode>::ReadNewNode()
{
	TNode *node = parent::ReadNewInnerNode();
	parent::mHeader->IncrementInnerNodeCount();
	return node;
}


template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::UnlockNode(TNode* node)
{
	parent::mSharedCache->UnlockW(node);
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
int cCommonRTree<TMbr, TKey, TNode, TLeafNode>::
ExtraInsertSetFlags(TLeafNode *currentLeafNode, unsigned int currentLevel, const TKey &item, int state, int ret)
{
	if (GetRTreeHeader()->GetOrderingEnabled()) 
	{
		/*
		if (parent::mDebug)
		{
			printf("Porovnavam tuples\n");
			item.GetRefTuple().Print("\n");
			currentLeafNode->GetItem(0)->GetRefTuple().Print("\n");
		}
		*/
		if (item.Equal(currentLeafNode->GetCKey(0), GetSpaceDescriptor()) == 0)
		{
			SetINSERT_TRAVERSE_UP_EXTERNDS_CHANGED(state);  //changeFT = true;

			if (ret != cRTreeConst::INSERT_NO)
			{
				SetINSERT_TRAVERSE_UP_INDEX_NOT_CHANGED(state);
			}
		}
	}
	return state;
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
bool cCommonRTree<TMbr, TKey, TNode, TLeafNode>::
ExtraInsertBreak(int state, unsigned int currentLevel, tNodeIndex nodeIndex)
{
	if (!(GetINSERT_TRAVERSE_UP_EXTERNDS_CHANGED(state)))  //!changeFT
		return true;
	else
		return false;
}
#include "dstruct/paged/rtree/cCommonRTree_Query.h"
#include "dstruct/paged/rtree/cCommonRTree_Insert.h"
#include "dstruct/paged/rtree/cCommonRTree_Delete.h"
#include "dstruct/paged/rtree/cCommonRTree_Gpu.h"

}}}
#endif
