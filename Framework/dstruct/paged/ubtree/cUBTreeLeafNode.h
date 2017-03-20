/**
*	\file cUBTreeLeafNode.h
*	\author Michal Kratky
*	\version 0.1
*	\date 2001 - 2008
*	\brief Implementation of the R-tree's leaf node.
*/

#ifndef __cUBTreeLeafNode_h__
#define __cUBTreeLeafNode_h__

namespace dstruct {
	namespace paged {
		namespace ubtree {
template<class TKey> class cUBTreeLeafNode;
}}}

#include "dstruct/paged/rtree/cRTreeConst.h"
#include "dstruct/paged/ubtree/cCommonUBTreeNode.h"
#include "dstruct/paged/core/cTreeItemStream.h"
#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "common/datatype/tuple/cMBRectangle.h"
#include "common/datatype/tuple/cMbrSideSizeOrder.h"
#include "dstruct/paged/ubtree/cCommonUBTreeNode.h"
#include "dstruct/paged/ubtree/cUBTreeLeafNodeHeader.h"
#include "dstruct/paged/queryprocessing/cRangeQueryConfig.h"
#include "dstruct/paged/queryprocessing/cQueryProcStat.h"
#include "dstruct/paged/queryprocessing/sBatchRQ.h"
#include "dstruct/paged/queryprocessing/cRangeQueryProcessorConstants.h"
#include "dstruct/paged/queryprocessing/cRQBuffers.h"

using namespace common::datatype::tuple;

/**
* Class is parametrized:
*		- TKey - Inherited from cBasicType. Must be type inherited from TKey. This type must implement operator = with TKey as a parameter.
*		- TLeafData - Inherited from cBasicType. Type of an unindexed item.
*
* Implementation of the R-tree's leaf node.
*
*	\author Michal Kratky
*	\version 0.1
*	\date 2001 - 2008
**/
namespace dstruct {
	namespace paged {
		namespace ubtree {

template<class TKey>
class cUBTreeLeafNode : public cCommonUBTreeNode<TKey>
{
  typedef cCommonUBTreeNode<TKey> parent;
  typedef cMBRectangle<TKey> TMbr;

private:
	void SplitDivideMbr_pre(cMemoryBlock **memBlock, char **cRTLeafNode_mem, char **TKey_ql1, char **TKey_qh1);
	void DivideMbr_pre(cMemoryBlock **memBlock, char **TKey_mbrl1, char **TKey_mbrh1, char **TKey_mbrl2, char **TKey_mbrh2);
	void Split_pre(cMemoryBlock **memBlock, char **cRTLeafNode_mem, char **TKey_ql1, char **TKey_qh1, tMbrSideSizeOrder **mbrSide, char **mbr1Lo, char **mbr1Hi, char **mbr2Lo, char **mbr2Hi);
	inline void Split_pre(uint itemCount, cNodeBuffers<TKey>* buffers);

	void Split_CutLongest(cUBTreeLeafNode<TKey> &newNode, char* TKey_mbrLo, char* TKey_mbrHi, cNodeBuffers<TKey>* buffers);
	bool FindTwoDisjMbrs(char *mbr1Lo, char *mbr1Hi, char *mbr2Lo, char *mbr2Hi, unsigned int dimOrder, unsigned int loOrder, unsigned int hiOrder, float pUtilization, cNodeBuffers<TKey>* buffers);
	void BuildTwoDisjNodes(cUBTreeLeafNode<TKey> &newNode, cUBTreeLeafNode<TKey> &tmpNode, char *mbr1Lo, char *mbr1Hi, cNodeBuffers<TKey>* buffers, unsigned int loOrder, unsigned int hiOrder);

	void Split_Ordering(cUBTreeLeafNode<TKey> &newNode, char* TKey_mbrLo, char* TKey_mbrHi, cNodeBuffers<TKey>* nodeBuffers);
	void BuildTwoOrderNodes(cUBTreeLeafNode<TKey> &newNode, cUBTreeLeafNode<TKey> &tmpNode, char *mbr1Lo, char *mbr1Hi, char* itemBuffer, char *itemBuffer2, char *dataBuffer, unsigned int loOrder, unsigned int hiOrder);
	bool FindTwoOrderMbrs(char *mbr1Lo, char *mbr1Hi, char *mbr2Lo, char *mbr2Hi, unsigned int dimOrder, unsigned int loOrder, unsigned int hiOrder, float pUtilization, char* itemBuffer, char* itemBuffer2);
	bool Split_Ordering(char* TKey_mbrLo, char* TKey_mbrHi, unsigned int loOrder, unsigned int hiOrder, tMbrSideSizeOrder *mbrSide, char *mbr1Lo, char *mbr1Hi, char *mbr2Lo, char *mbr2Hi, char* itemBuffer, char* itemBuffer2);

protected:
	void Split_DivideMbr(cUBTreeLeafNode<TKey> &newNode, cNodeBuffers<TKey>* buffers = NULL);

	bool DivideMbr(char* TKey_ql, char* TKey_qh, unsigned int minimal_count, cNodeBuffers<TKey>* buffers = NULL);
	bool SortBy(unsigned int dimension, cNodeBuffers<TKey>* buffers = NULL);
	void SpaceOrderingSort();
	inline int ItemQcmp(const void* p1, const void* p2);
	void CreateMbr(unsigned int startOrder, unsigned int finishOrder, char* TKey_ql, char* TKey_qh, cNodeBuffers<TKey>* buffers = NULL) const; //bas064

public:
	static const int NO_ITEM_FIND = -1;
	static int TMP_SearchInBlock_COUNT;
	static int TMP_RelevantSearchInBlock_COUNT;

	static int ITEM_WRITE;
	static int ITEM_READ;
	static unsigned long long IR_Compares;

public:
	cUBTreeLeafNode(const cUBTreeLeafNode<TKey>* origNode, const char* mem);
	cUBTreeLeafNode(void);
	~cUBTreeLeafNode(void);

	bool Split(char* TKey_mbrLo, char* TKey_mbrHi, unsigned int loOrder, unsigned int hiOrder, tMbrSideSizeOrder *mbrSide, char *mbr1Lo, char *mbr1Hi, char *mbr2Lo, char *mbr2Hi, cNodeBuffers<TKey>* buffers);
	void Split(cUBTreeLeafNode<TKey> &newNode, char* TKey_mbrLo, char* TKey_mbrHi, cNodeBuffers<TKey>* buffers);
	int SearchInBlock(const TKey &ql, const TKey &qh, cTreeItemStream<TKey>* resultSet, unsigned int finishResultSize = 0, unsigned int currentOrder = UINT_MAX, cNodeBuffers<TKey>* buffers = NULL) const; //bas064
	void BatchSearchInBlock(const TKey* qls, const TKey* qhs, cTreeItemStream<TKey>* resultSet, cRangeQueryConfig *rqConfig, cNodeBuffers<TKey>* buffers,  cArray<unsigned int>* queryIndices, unsigned int* resultSizes) const; 
	
	static void FindRelevantQueries(unsigned int currentLevel, cArray<unsigned int>** currentQueryPath, cArray<unsigned int>* queryIndices); 
	void CreateMbr(char* TKey_ql, char* TKey_qh, cNodeBuffers<TKey>* buffers = NULL) const; //bas064

	inline cUBTreeLeafNodeHeader<TKey>* GetUBTreeLeafNodeHeader() const;
	inline cSpaceDescriptor* GetSpaceDescriptor() const;

	static void InsertTuple(cUBTreeLeafNode<TKey>* node1, cUBTreeLeafNode<TKey>* node2, const TKey &tuple, cUBTreeLeafNodeHeader<TKey>* nodeHeader, char* leafData, cNodeBuffers<TKey>* buffers = NULL); //bas064
	static void InsertTuple_Ordered(cUBTreeLeafNode<TKey>* node1, cUBTreeLeafNode<TKey>* node2, const TKey &tuple, cUBTreeLeafNodeHeader<TKey>* nodeHeader, char* leafData, char* buffer = NULL);
	int CheckDuplicity(const TKey &item, char* buffer = NULL);

#ifdef CUDA_ENABLED
	inline void TransferLeafNodeToGpu();
	inline void SerializeKeys(uint* mbr);
#endif
};


template<class TKey>
int cUBTreeLeafNode<TKey>::TMP_SearchInBlock_COUNT = 0;

template<class TKey>
int cUBTreeLeafNode<TKey>::TMP_RelevantSearchInBlock_COUNT = 0;

template<class TKey>
unsigned long long cUBTreeLeafNode<TKey>::IR_Compares = 0;

/**
 * This constructor is used in the case when the node is created on the pool, this pool creates a memory
 * and the node uses it.
 */
template<class TKey>
cUBTreeLeafNode<TKey>::cUBTreeLeafNode(const cUBTreeLeafNode<TKey>* origNode, const char* mem): parent(origNode, mem)
{
}

template<class TKey>
cUBTreeLeafNode<TKey>::cUBTreeLeafNode(): parent()
{
}

template<class TKey>
cUBTreeLeafNode<TKey>::~cUBTreeLeafNode()
{
}


// Allocates memory for temporary variables
template<class TKey>
inline void cUBTreeLeafNode<TKey>::Split_pre(uint itemCount, cNodeBuffers<TKey>* buffers)
{
	// for decompressed items
	uint size = itemCount * (parent::mHeader->GetItemSize() + sizeof(tItemOrder));

	// for node mbr and for splitted mbr1, mbr2
	uint mbrSize = 2 * parent::GetNodeHeader()->GetItemSize();
	size += 2 * parent::RTREE_SN_COUNT * mbrSize;

	uint maskSize = parent::TMask::ByteSize(((cSpaceDescriptor*)parent::GetNodeHeader()->GetKeyDescriptor())->GetDimension());
	size += parent::RTREE_SN_COUNT * maskSize;

	uint size_mbrSide = ((cSpaceDescriptor*)parent::GetNodeHeader()->GetKeyDescriptor())->GetDimension() * sizeof(tMbrSideSizeOrder);
	size += size_mbrSide;

	size += parent::mHeader->GetNodeInMemSize();

	size += 8 * parent::GetNodeHeader()->GetItemSize();

	// get the memory from the mem pool
	buffers->riMemBlock = parent::mHeader->GetMemoryManager()->GetMem(size);
	char* buffer = buffers->riMemBlock->GetMem();

	buffers->tmpNode = buffer;
	buffer += itemCount * (parent::mHeader->GetItemSize() + sizeof(tItemOrder));

	buffers->mbrs = buffer;
	buffer += 2 * parent::RTREE_SN_COUNT * mbrSize;

	buffers->mbrSide = (tMbrSideSizeOrder*)buffer;
	buffer += size_mbrSide;

	buffers->masks = buffer;
	buffer += parent::RTREE_SN_COUNT * maskSize;

	buffers->tmpNode2 = buffer;
	buffer += parent::mHeader->GetNodeInMemSize();

	buffers->itemBuffer.riBuffer = buffer;
	buffers->itemBuffer.codingBuffer = buffer + (2 * parent::GetNodeHeader()->GetItemSize());
	buffer += 4 * parent::GetNodeHeader()->GetItemSize();

	buffers->itemBuffer2.riBuffer = buffer;
	buffers->itemBuffer2.codingBuffer = buffer + (2 * parent::GetNodeHeader()->GetItemSize());
	buffer += 4 * parent::GetNodeHeader()->GetItemSize();
}

template<class TKey> 
inline cUBTreeLeafNodeHeader<TKey>* cUBTreeLeafNode<TKey>::GetUBTreeLeafNodeHeader() const
{
	return (cUBTreeLeafNodeHeader<TKey>*)parent::mHeader;
}

template<class TKey> 
inline cSpaceDescriptor* cUBTreeLeafNode<TKey>::GetSpaceDescriptor() const
{
	return (cSpaceDescriptor*)((cUBTreeLeafNodeHeader<TKey>*)parent::mHeader)->GetKeyDescriptor();
}

/*
 * \return cRTreeConst::INSERT_DUPLICATE or cRTreeConst::INSERT_YES;
 */
template<class TKey>
int cUBTreeLeafNode<TKey>::CheckDuplicity(const TKey &item, char* buffer)
{
	const cSpaceDescriptor *sd = GetSpaceDescriptor();
	int ret = cRTreeConst::INSERT_YES;

	if (!parent::GetNodeHeader()->DuplicatesAllowed())
	{
		for (unsigned int i = 0 ; i < parent::mItemCount ; i++)
		{
			if (item.Equal(parent::GetCKey(i, buffer), sd) == 0)
			{
				ret = cRTreeConst::INSERT_DUPLICATE;
				break;
			}
		}
	}
	return ret;
}

/**
 * Split node into this and newNode.
 */
template<class TKey>
void cUBTreeLeafNode<TKey>::Split(cUBTreeLeafNode<TKey> &newNode, char* TKey_mbrLo, char* TKey_mbrHi, cNodeBuffers<TKey>* buffers)
{
	if (!GetUBTreeLeafNodeHeader()->GetOrderingEnabled())
	{
		if (cRTreeConst::Node_Split == Split::CUT_LONGEST)
		{
			if ((parent::mHeader->GetDStructMode() == cDStructConst::DSMODE_DEFAULT) || (parent::mHeader->GetDStructMode() == cDStructConst::DSMODE_CODING))
			{
				Split_CutLongest(newNode, TKey_mbrLo, TKey_mbrHi, buffers);
			}
		}
		else if (cRTreeConst::Node_Split == Split::COMPUTE_VOLUME)
		{
			Split_DivideMbr(newNode, buffers);
		}
	}
	else 
	{
		Split_Ordering(newNode, TKey_mbrLo, TKey_mbrHi, buffers);
	}
}

/**
 * Split node into this and newNode.
 */
template<class TKey>
void cUBTreeLeafNode<TKey>::Split_CutLongest(cUBTreeLeafNode<TKey> &newNode, char* TKey_mbrLo, char* TKey_mbrHi, cNodeBuffers<TKey>* buffers)
{
	cMemoryBlock *memBlock;
	char *cRTLeafNode_mem, *TKey_ql1, *TKey_qh1, *mbr1Lo, *mbr1Hi, *mbr2Lo, *mbr2Hi = NULL;
	const cSpaceDescriptor *sd = GetSpaceDescriptor();
	//cNodeBuffers splitBuffers;
	tMbrSideSizeOrder *mbrSide;

	Split_pre(&memBlock, &cRTLeafNode_mem, &TKey_ql1, &TKey_qh1, &mbrSide, &mbr1Lo, &mbr1Hi, &mbr2Lo, &mbr2Hi);
	cUBTreeLeafNode<TKey> tmpNode(this, cRTLeafNode_mem);
	tmpNode.Init();
	tmpNode.SetLeaf(true);

	Split(TKey_mbrLo, TKey_mbrHi, 0, parent::mItemCount-1, mbrSide, mbr1Lo, mbr1Hi, mbr2Lo, mbr2Hi, buffers);
	BuildTwoDisjNodes(newNode, tmpNode, mbr1Lo, mbr1Hi, buffers, 0, parent::mItemCount - 1);

	// free temporary memory
	tmpNode.SetData(NULL);
	parent::mHeader->GetMemoryManager()->ReleaseMem(memBlock);
}

/**
 * Split node into this and newNode.
 */
template<class TKey>
void cUBTreeLeafNode<TKey>::Split_DivideMbr(cUBTreeLeafNode<TKey> &newNode, cNodeBuffers<TKey>* buffers)
{
	char *key, *data;
	unsigned int order1 = 0, order2 = 0;
	unsigned minimal_count = parent::mItemCount * 0.30;
	cMemoryBlock *memBlock;
	char *cRTLeafNode_mem, *TKey_ql1, *TKey_qh1 = NULL;
	const cSpaceDescriptor *sd = GetSpaceDescriptor();

	SplitDivideMbr_pre(&memBlock, &cRTLeafNode_mem, &TKey_ql1, &TKey_qh1);

	cUBTreeLeafNode<TKey> tmpNode(this, cRTLeafNode_mem);
	tmpNode.Init();
	tmpNode.SetLeaf(true);

	if (DivideMbr(TKey_ql1, TKey_qh1, minimal_count, buffers))     // divide leaf node's mbr into two mbrs
	{
		if (parent::mDebug)
		{
			TKey::Print(TKey_ql1, "\n", sd);
			TKey::Print(TKey_qh1, "\n", sd);
			printf("\n");
		}

		tmpNode.Clear();
		newNode.Clear();

		// now copy items into first or second nodes
		for (unsigned int i = 0 ; i < parent::mItemCount ; i++)   
		{
			if (parent::mDebug)
			{
				TKey::Print(parent::GetCKey(i, &buffers->itemBuffer), "\n", sd);
			}

			GetKeyData(i, &key, &data, &buffers->itemBuffer2);
			if (cMBRectangle<TKey>::IsInRectangle(TKey_ql1, TKey_qh1, key, sd))
			{
				tmpNode.parent::AddLeafItem(key, data, false, buffers);
				order1++;
			}
			else
			{
				newNode.parent::AddLeafItem(key, data, false, buffers);
				order2++;
			}

			/*
			if (cMBRectangle<TKey>::IsInRectangle(TKey_ql1, TKey_qh1, parent::GetCKey(i, buffers),  sd))
			{
				// mk: 29.3.2012: Is it possible to use a method adding the complete item
				tmpNode.ParentLeafNode::AddLeafItem(parent::GetCKey(i, buffers), false, GetData(i, dataBuffer), buffers2);
				order1++;
			}
			else
			{
				newNode.ParentLeafNode::AddLeafItem(parent::GetCKey(i, buffers), false, GetData(i, dataBuffer), buffers2);
				order2++;
			}
			*/
		}
	} 
	else
	{
		// all items are the same. Split the leaf node on two halfs
		for (unsigned int i = 0 ; i < parent::mItemCount ; i++)   
		{
			if (i < parent::mItemCount / 2)
			{
				tmpNode.SetItemPOrder(order1, order1);
				tmpNode.SetItem(order1++, GetCItem(i, &buffers->itemBuffer));
				// tato metoda koprije data za klic, reseni je pouzit GetKeyData(i, key, data) SetKeyData(i, key, data)
				// GetCItem je kvuli pridavani do ResultSet, reseni je mit metodu Add(key, data) v result set
			}
			else
			{
				newNode.SetItemPOrder(order2, order2);
				newNode.SetItem(order2++, GetCItem(i, &buffers->itemBuffer));
			}
		}
	}

	parent::Clear();

	// copy items from tmp node to original node
	for (unsigned int i = 0 ; i < order1 ; i++)
	{
		tmpNode.GetKeyData(i, &key, &data, &buffers->itemBuffer2);
		parent::AddLeafItem(key, data, false, buffers);
	}

	if (parent::mDebug)
	{
		printf("\n\n");
		parent::Print(sd);
		printf("\n\n");
		newNode.Print(sd, &buffers->itemBuffer);
		printf("\n\n");
	}

	// free temporary memory
	tmpNode.SetData(NULL);
	parent::mHeader->GetMemoryManager()->ReleaseMem(memBlock);

	parent::mHeader->IncrementNodeCount();
}

template<class TKey>
void cUBTreeLeafNode<TKey>::SplitDivideMbr_pre(cMemoryBlock **memBlock, char **cRTLeafNode_mem, char **TKey_ql1, 
	char **TKey_qh1)
{
	unsigned int size = 0;
	unsigned int size_cRTLeafNode_mem = parent::GetNodeHeader()->GetNodeInMemSize();
	size += size_cRTLeafNode_mem;
	unsigned int size_TKey_ql1 = parent::GetNodeHeader()->GetKeySize();
	size += size_TKey_ql1;
	unsigned int size_TKey_qh1 = size_TKey_ql1;
	size += size_TKey_qh1;

	// get the memory from the mem pool
	*memBlock = parent::mHeader->GetMemoryManager()->GetMem(size);
	char *buffer = (*memBlock)->GetMem();

	*cRTLeafNode_mem = buffer;
	buffer += size_cRTLeafNode_mem;
	*TKey_ql1 = buffer;
	buffer += size_TKey_ql1;
	*TKey_qh1 = buffer;
	buffer += size_TKey_qh1;
}


/**
 * Split leaf nodes's mbr into two mbrs.
 */
template<class TKey>
bool cUBTreeLeafNode<TKey>::Split(char* TKey_mbrLo, char* TKey_mbrHi, unsigned int loOrder, unsigned int hiOrder, tMbrSideSizeOrder *mbrSide,
	char *mbr1Lo, char *mbr1Hi, char *mbr2Lo, char *mbr2Hi, cNodeBuffers<TKey>* buffers)
{
	const cSpaceDescriptor *sd = GetSpaceDescriptor();
	unsigned int dim = sd->GetDimension();

	cMbrSideSizeOrder<TKey>::ComputeSidesSize(TKey_mbrLo, TKey_mbrHi, mbrSide, sd);
	cMbrSideSizeOrder<TKey>::QSortUInt(mbrSide, dim);

	// First, preserve 50:50 utilization
	//for (unsigned int i = 0 ; i < dim ; i++)
	//{
	//	unsigned int dimOrder = mbrSide[i].Order;
	//	if (FindTwoDisjMbrsUtil(mbr1Lo, mbr1Hi, mbr2Lo, mbr2Hi, dimOrder, loOrder, hiOrder, itemBuffer, itemBuffer2))
	//	{
	//		find = true;
	//		break;
	//	}
	//}

	float utilization = 0.49;
	const float minUtilization = 0.30;
	bool disjMbrsFound = false;

	while(!disjMbrsFound)
	{
		// 50:50 utilization is not preserved
		for (unsigned int i = 0 ; i < dim ; i++)
		{
			unsigned int dimOrder = mbrSide[i].Order;
			if (FindTwoDisjMbrs(mbr1Lo, mbr1Hi, mbr2Lo, mbr2Hi, dimOrder, loOrder, hiOrder, utilization, buffers))
			{
				disjMbrsFound = true;
				break;
			}
		}

		if (!disjMbrsFound)
		{
			utilization -= 0.05;
			if (utilization < 0.0)
			{
				printf("Critical Error: cUBTreeLeafNode<TKey>::Split: utilization < 0.0!\n");
				break;
			}
		}
	}

	if (utilization < minUtilization)
	{
		printf("Warning: cUBTreeLeafNode<TKey>::Split(): Two disjunctive mbrs are found but utilization < %.2f (%.2f)!\n", minUtilization, utilization);
	}

	return 0;
}

/**
 * Split leaf nodes's mbr into two mbrs.
 */
template<class TKey>
bool cUBTreeLeafNode<TKey>::Split_Ordering(char* TKey_mbrLo, char* TKey_mbrHi, unsigned int loOrder, unsigned int hiOrder, tMbrSideSizeOrder *mbrSide,
	char *mbr1Lo, char *mbr1Hi, char *mbr2Lo, char *mbr2Hi, char* itemBuffer, char* itemBuffer2)
{
	const cSpaceDescriptor *sd = GetSpaceDescriptor();
	unsigned int dim = sd->GetDimension();

	if (parent::mDebug)
	{
		TKey::Print(TKey_mbrLo, "\n", sd);
		TKey::Print(TKey_mbrHi, "\n", sd);
	}

	//cMbrSideSizeOrder<TKey>::ComputeSidesSize(TKey_mbrLo, TKey_mbrHi, mbrSide, sd);

	if (parent::mDebug)
	{
		TKey::Print(mbr1Lo, " x ", sd);
		TKey::Print(mbr1Hi, "\n", sd);
		TKey::Print(mbr2Lo, " x ", sd);
		TKey::Print(mbr2Hi, "\n", sd);

		cMbrSideSizeOrder<TKey>::Print(mbrSide, dim);
	}
	
	//cMbrSideSizeOrder<TKey>::QSortUInt(mbrSide, dim);
	
	if (parent::mDebug)
	{
		TKey::Print(mbr1Lo, " x ", sd);
		TKey::Print(mbr1Hi, "\n", sd);
		TKey::Print(mbr2Lo, " x ", sd);
		TKey::Print(mbr2Hi, "\n", sd);

		cMbrSideSizeOrder<TKey>::Print(mbrSide, dim);
	}

	FindTwoOrderMbrs(mbr1Lo, mbr1Hi, mbr2Lo, mbr2Hi, 0, loOrder, hiOrder, 0, itemBuffer, itemBuffer2);

	return 0;
}

/**
 * Find two disjunctive mbrs.
 */
template<class TKey>
void cUBTreeLeafNode<TKey>::BuildTwoDisjNodes(cUBTreeLeafNode<TKey> &newNode, cUBTreeLeafNode<TKey> &tmpNode, char *mbr1Lo, char *mbr1Hi,
	cNodeBuffers<TKey>* buffers, unsigned int loOrder, unsigned int hiOrder)
{
	char *key, *data;

	tmpNode.Clear();
	newNode.Clear();
	unsigned int order1 = 0, order2 = 0;
	unsigned minimal_count = parent::mItemCount * 0.30;
	const cSpaceDescriptor *sd = GetSpaceDescriptor();

	// now copy items into first or second nodes
	for (unsigned int i = loOrder ; i <= hiOrder ; i++)   
	{
		GetKeyData(i, &key, &data, &buffers->itemBuffer2);
		if (cMBRectangle<TKey>::IsInRectangle(mbr1Lo, mbr1Hi, key, sd))
		{
			// mk: 29.3.2012: Is it possible to use a method adding the complete item
			tmpNode.parent::AddLeafItem(key, data, false, buffers);
			order1++;
		}
		else
		{
			newNode.parent::AddLeafItem(key, data, false, buffers);
			order2++;
		}
	}

	parent::Clear();

	// copy items from tmp node to original node
	for (unsigned int i = 0 ; i < order1 ; i++)
	{
		tmpNode.GetKeyData(i, &key, &data, &buffers->itemBuffer2);
		parent::AddLeafItem(key, data, false, buffers);
	}

	if (parent::mDebug)
	{
		printf("\n\n");
		parent::Print(sd);
		printf("\n\n");
		newNode.Print(sd, &buffers->itemBuffer);
		printf("\n\n");
	}

	parent::mHeader->IncrementNodeCount();
}

/**
 * Find two ordered mbrs.
 */
template<class TKey>
void cUBTreeLeafNode<TKey>::BuildTwoOrderNodes(cUBTreeLeafNode<TKey> &newNode, cUBTreeLeafNode<TKey> &tmpNode, char *mbr1Lo, char *mbr1Hi,
	char* itemBuffer, char *itemBuffer2, char *dataBuffer, unsigned int loOrder, unsigned int hiOrder)
{
	tmpNode.Clear();
	newNode.Clear();
	unsigned int order1 = 0, order2 = 0;
	const cSpaceDescriptor *sd = GetSpaceDescriptor();

	// now copy items into first or second nodes
	for (unsigned int i = loOrder ; i <= hiOrder ; i++)   
	{
		if (parent::mDebug)
		{
			TKey::Print(parent::GetCKey(i, itemBuffer), "\n", sd);
		}

		if (i<(hiOrder/2))
		{
			// mk: 29.3.2012: Is it possible to use a method adding the complete item
			tmpNode.AddLeafItem(parent::GetCKey(i, itemBuffer), parent::GetData(i, dataBuffer), false, itemBuffer2);
			order1++;
		}
		else
		{
			newNode.AddLeafItem(parent::GetCKey(i, itemBuffer), parent::GetData(i, dataBuffer), false, itemBuffer2);
			order2++;
		}
	}

	parent::Clear();

	// copy items from tmp node to original node
	for (unsigned int i = 0 ; i < order1 ; i++)
	{
		parent::AddLeafItem(tmpNode.GetCItem(i, itemBuffer), tmpNode.GetData(i, dataBuffer), false, itemBuffer2);
	}

	if (parent::mDebug)
	{
		printf("\n\n");
		parent::Print(sd);
		printf("\n\n");
		newNode.Print(sd, itemBuffer);
		printf("\n\n");
	}

	parent::mHeader->IncrementNodeCount();
}



/**
 * Find two disjunctive mbrs.
 * pUtilization -- 1st value: 0.45 can lead to nondisjunctive mbrs then it is necessary to use 0.4 and so on.
 */
template<class TKey>
bool cUBTreeLeafNode<TKey>::FindTwoDisjMbrs(char *mbr1Lo, char *mbr1Hi, char *mbr2Lo, char *mbr2Hi, 
	unsigned int dimOrder, unsigned int loOrder, unsigned int hiOrder, float pUtilization, cNodeBuffers<TKey>* buffers)
{
	unsigned int order, state, index;
	unsigned minimal_count = parent::mItemCount * pUtilization;
	float diff;//, itemCount = (float)((float)parent::mItemCount * 0.5);    // mk: node utilization
	unsigned int itemCount = parent::mItemCount / 2;
	bool disjmbr = false;
	const cSpaceDescriptor *sd = GetSpaceDescriptor();
	unsigned int dim = GetSpaceDescriptor()->GetDimension();

	bool ret = false;

	// for solving of the same values problem
	if (parent::mItemCount % 2 == 0)
	{
		diff = 0.5;
		itemCount -= diff;
	} else 
	{
		diff = 0.0;
	}

	bool sameValues = SortBy(dimOrder, buffers);

	if (sameValues)
	{
		return false;
	}

	// solve problem with the same values in dimension
	if (parent::mItemCount % 2 == 0)
	{
		state = 1;
	} else 
	{
		state = 0;
	}

	index = 0;
	bool probSameValues = false;

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

		if (index != 0 && (order < minimal_count || order >= parent::mItemCount-1))    // if all values in dimension are the same, then continue with next dimension
		{
			probSameValues = true;
			break;
		}

		if (TKey::Equal(parent::GetCKey(order, &buffers->itemBuffer), parent::GetCKey(order + 1, &buffers->itemBuffer2), dimOrder, sd) != 0)
		{
			break;
		}
	}

	if (!probSameValues)
	{
		ret = true;
		CreateMbr(0, order, mbr1Lo, mbr1Hi, buffers);
		CreateMbr(order+1, parent::mItemCount-1, mbr2Lo, mbr2Hi, buffers);
	}

	if (parent::mDebug)
	{
		printf("\n");
		this->Print(sd);
	}

	return ret;
}

/**
 * Find two ordered mbrs.
  */
template<class TKey>
bool cUBTreeLeafNode<TKey>::FindTwoOrderMbrs(char *mbr1Lo, char *mbr1Hi, char *mbr2Lo, char *mbr2Hi, 
	unsigned int dimOrder, unsigned int loOrder, unsigned int hiOrder, float pUtilization, char *itemBuffer, char* itemBuffer2)
{
	unsigned int order, state, index;
	unsigned int itemCount = parent::mItemCount / 2;
	parent::mDebug = false;
	const cSpaceDescriptor *sd = GetSpaceDescriptor();
	unsigned int dim = GetSpaceDescriptor()->GetDimension();

	*(mbr1Lo) = *(parent::GetCKey(0, itemBuffer));
	*(mbr1Hi) = *(parent::GetCKey(0, itemBuffer));
	*(mbr2Lo) = *(parent::GetCKey((parent::mItemCount/2)+1, itemBuffer));
	*(mbr2Hi) = *(parent::GetCKey((parent::mItemCount/2)+1, itemBuffer));

	CreateMbr(0, parent::mItemCount/2, mbr1Lo, mbr1Hi, itemBuffer);
	CreateMbr((parent::mItemCount/2)+1, parent::mItemCount-1, mbr2Lo, mbr2Hi, itemBuffer);

	if (parent::mDebug)
	{
		printf("\n");
		this->Print(sd);
		TKey::Print(mbr1Lo, "\n", sd);
		TKey::Print(mbr1Hi, "\n", sd);
		TKey::Print(mbr2Lo, "\n", sd);
		TKey::Print(mbr2Hi, "\n", sd);
	}

	return false;
}


/**
 * Split leaf nodes's mbr into two mbrs.
 */
/*
template<class TKey>
bool cUBTreeLeafNode<TKey>::Split(char* TKey_mbrLo, char* TKey_mbrHi, unsigned int loOrder, unsigned int hiOrder)
{
	unsigned int order, state, index;
	double volume, minVolume = DBL_MAX;
	float diff;//, itemCount = (float)((float)parent::mItemCount * 0.5);    // mk: node utilization
	unsigned int itemCount = parent::mItemCount / 2;
	bool disjmbr = false;
	parent::mDebug = false;
	cMemoryBlock *memBlock;
	char *TKey_mbrl1, *TKey_mbrh1, *TKey_mbrl2, *TKey_mbrh2;
	const cSpaceDescriptor *sd = GetSpaceDescriptor();

	DivideMbr_pre(&memBlock, &TKey_mbrl1, &TKey_mbrh1, &TKey_mbrl2, &TKey_mbrh2);

	if (parent::mDebug)
	{
		Print(sd);
	}

	// for solving of the same values problem
	if (parent::mItemCount % 2 == 0)
	{
		diff = 0.5;
		itemCount -= diff;
	}
	else
	{
		diff = 0.0;
	}

	if (parent::mDebug)
	{
		this->Print(sd);
	}

	unsigned int dim = GetSpaceDescriptor()->GetDimension();

	for (unsigned int i = 0 ; i < dim ; i++)
	{
		bool sameValues = SortBy(i, itemBuffer, itemBuffer2);

		if (parent::mDebug)
		{
			this->Print(sd);
		}

		if (sameValues)
		{
			continue;
		}

		// solve problem with the same values in dimension
		if (parent::mItemCount % 2 == 0)
		{
			state = 1;
		}
		else
		{
			state = 0;
		}

		index = 0;
		bool probSameValues = false;

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

			if (index != 0 && (order < minimal_count || order >= parent::mItemCount-1))    // if all values in dimension are the same, then continue with next dimension
			{
				probSameValues = true;
				break;
			}

			if (TKey::Equal(parent::GetCKey(order, itemBuffer), parent::GetCKey(order+1, itemBuffer2), i, sd) != 0)  //fk ??? GetCItem->parent::GetCKey
			{
				break;
			}
		}

		if (probSameValues)
		{
			continue;
		}

		CreateMbr(0, order, TKey_mbrl1, TKey_mbrh1, itemBuffer);
		CreateMbr(order+1, parent::mItemCount-1, TKey_mbrl2, TKey_mbrh2, itemBuffer);
		volume = TMbr::Volume(TKey_mbrl1, TKey_mbrh1, sd) + TMbr::Volume(TKey_mbrl2, TKey_mbrh2, sd);

		if (volume < minVolume)
		{
			minVolume = volume;
			TKey::Copy(TKey_ql, TKey_mbrl1, sd);
			TKey::Copy(TKey_qh, TKey_mbrh1, sd);
			disjmbr = true;

			//parent::mDebug = true;  //fk
			if (parent::mDebug)
			{
				printf("\n");
				TKey::Print(TKey_ql, "\n", sd);
				TKey::Print(TKey_qh, "\n", sd);
			}
		}
	}

	if (!disjmbr)
	{
		if (GetRTreeLeafNodeHeader()->DuplicatesAllowed())
		{
			return false;
		}
		printf("Critical Error: cUBTreeLeafNode::DivideMbr(): Disjunctive splitting of MBRs wasn't found!\n");
		this->Print(cObject::MODE_DEC, itemBuffer);
		exit(1);
	}

	if (parent::mDebug)
	{
		this->Print(sd);
	}

	if (parent::mDebug)
	{
		printf("\n");
		TKey::Print(TKey_ql, "\n", sd);
		TKey::Print(TKey_qh, "\n", sd);
		TKey::Print(TKey_mbrl1, "\n", sd);
		TKey::Print(TKey_mbrh1, "\n", sd);
		TKey::Print(TKey_mbrl2, "\n", sd);
		TKey::Print(TKey_mbrh2, "\n", sd);
	}

	// free temporary memory
	parent::mHeader->GetMemoryManager()->ReleaseMem(memBlock);

	return true;
}*/

/**
 * Split leaf nodes's mbr into two mbrs.
 */
template<class TKey>
bool cUBTreeLeafNode<TKey>::DivideMbr(char* TKey_ql, char* TKey_qh, unsigned int minimal_count, cNodeBuffers<TKey>* buffers)
{
	unsigned int order, state, index;
	double volume, minVolume = DBL_MAX;
	float diff;//, itemCount = (float)((float)parent::mItemCount * 0.5);    // mk: node utilization
	unsigned int itemCount = parent::mItemCount / 2;
	bool disjmbr = false;
	cMemoryBlock *memBlock;
	char *TKey_mbrl1, *TKey_mbrh1, *TKey_mbrl2, *TKey_mbrh2;
	const cSpaceDescriptor *sd = GetSpaceDescriptor();

	DivideMbr_pre(&memBlock, &TKey_mbrl1, &TKey_mbrh1, &TKey_mbrl2, &TKey_mbrh2);

	// for solving of the same values problem
	if (parent::mItemCount % 2 == 0)
	{
		diff = 0.5;
		itemCount -= diff;
	}
	else
	{
		diff = 0.0;
	}

	unsigned int dim = GetSpaceDescriptor()->GetDimension();

	for (unsigned int i = 0 ; i < dim ; i++)
	{
		bool sameValues = SortBy(i, buffers);

		if (sameValues)
		{
			continue;
		}

		// solve problem with the same values in dimension
		if (parent::mItemCount % 2 == 0)
		{
			state = 1;
		}
		else
		{
			state = 0;
		}

		index = 0;
		bool probSameValues = false;

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

			if (index != 0 && (order < minimal_count || order >= parent::mItemCount-1))    // if all values in dimension are the same, then continue with next dimension
			{
				probSameValues = true;
				break;
			}

			if (TKey::Equal(parent::GetCKey(order, &buffers->itemBuffer), parent::GetCKey(order + 1, &buffers->itemBuffer2), i, sd) != 0)  //fk ??? GetCItem->parent::GetCKey
			{
				break;
			}
		}

		if (probSameValues)
		{
			continue;
		}

		CreateMbr(0, order, TKey_mbrl1, TKey_mbrh1, buffers);
		CreateMbr(order+1, parent::mItemCount-1, TKey_mbrl2, TKey_mbrh2, buffers);
		volume = TMbr::Volume(TKey_mbrl1, TKey_mbrh1, sd) + TMbr::Volume(TKey_mbrl2, TKey_mbrh2, sd);

		if (volume < minVolume)
		{
			minVolume = volume;
			TKey::Copy(TKey_ql, TKey_mbrl1, sd);
			TKey::Copy(TKey_qh, TKey_mbrh1, sd);
			disjmbr = true;
		}
	}

	if (!disjmbr)
	{
		if (GetUBTreeLeafNodeHeader()->DuplicatesAllowed())
		{
			return false;
		}
		printf("Critical Error: cUBTreeLeafNode::DivideMbr(): Disjunctive splitting of MBRs wasn't found!\n");
		this->Print(cObject::MODE_DEC, &buffers->itemBuffer);
		exit(1);
	}

	if (parent::mDebug)
	{
		this->Print(sd);
		printf("\n");
		TKey::Print(TKey_ql, "\n", sd);
		TKey::Print(TKey_qh, "\n", sd);
		TKey::Print(TKey_mbrl1, "\n", sd);
		TKey::Print(TKey_mbrh1, "\n", sd);
		TKey::Print(TKey_mbrl2, "\n", sd);
		TKey::Print(TKey_mbrh2, "\n", sd);
	}

	// free temporary memory
	parent::mHeader->GetMemoryManager()->ReleaseMem(memBlock);

	return true;
}

template<class TKey>
void cUBTreeLeafNode<TKey>::Split_pre(cMemoryBlock **memBlock, char **cRTLeafNode_mem, char **TKey_ql1, 
	char **TKey_qh1, tMbrSideSizeOrder **mbrSide, char **mbr1Lo, char **mbr1Hi, char **mbr2Lo, char **mbr2Hi)
{
	unsigned int size = 0;
	unsigned int size_cRTLeafNode_mem = parent::GetNodeHeader()->GetNodeInMemSize();
	size += size_cRTLeafNode_mem;
	unsigned int size_tuple = parent::GetNodeHeader()->GetKeySize();
	const unsigned int tupleCount = 6;
	size += tupleCount * size_tuple;
	unsigned int size_mbrSide = ((cSpaceDescriptor*)parent::GetNodeHeader()->GetKeyDescriptor())->GetDimension() * sizeof(tMbrSideSizeOrder);
	size += size_mbrSide;

	// get the memory from the mem pool
	*memBlock = parent::mHeader->GetMemoryManager()->GetMem(size);
	char *buffer = (*memBlock)->GetMem();

	*cRTLeafNode_mem = buffer;
	buffer += size_cRTLeafNode_mem;
	*TKey_ql1 = buffer;
	buffer += size_tuple;
	*TKey_qh1 = buffer;
	buffer += size_tuple;
	*mbr1Lo = buffer;
	buffer += size_tuple;
	*mbr1Hi = buffer;
	buffer += size_tuple;
	*mbr2Lo = buffer;
	buffer += size_tuple;
	*mbr2Hi = buffer;
	buffer += size_tuple;

	*mbrSide = (tMbrSideSizeOrder*)buffer;
	buffer += size_mbrSide;
}


template<class TKey>
void cUBTreeLeafNode<TKey>::DivideMbr_pre(cMemoryBlock **memBlock, char **TKey_mbrl1, char **TKey_mbrh1, 
	char **TKey_mbrl2, char **TKey_mbrh2)
{
	unsigned int size = 0;
	unsigned int size_key = parent::GetNodeHeader()->GetKeySize();
	size += 4 * size_key;

	// get the memory from the mem pool
	*memBlock = parent::mHeader->GetMemoryManager()->GetMem(size);
	char *buffer = (*memBlock)->GetMem();

	*TKey_mbrl1 = buffer;
	buffer += size_key;
	*TKey_mbrh1 = buffer;
	buffer += size_key;
	*TKey_mbrl2 = buffer;
	buffer += size_key;
	*TKey_mbrh2 = buffer;
}

/**
 * Split node into this and newNode.
 */
template<class TKey>
void cUBTreeLeafNode<TKey>::Split_Ordering(cUBTreeLeafNode<TKey> &newNode, char* TKey_mbrLo, char* TKey_mbrHi, cNodeBuffers<TKey>* buffers)
{
	cMemoryBlock *memBlock;
	char *cRTLeafNode_mem, *TKey_ql1, *TKey_qh1, *mbr1Lo, *mbr1Hi, *mbr2Lo, *mbr2Hi;
	const cSpaceDescriptor *sd = GetSpaceDescriptor();
	tMbrSideSizeOrder *mbrSide;

	Split_pre(&memBlock, &cRTLeafNode_mem, &TKey_ql1, &TKey_qh1, &mbrSide, &mbr1Lo, &mbr1Hi, &mbr2Lo, &mbr2Hi);

	cUBTreeLeafNode<TKey> tmpNode(this, cRTLeafNode_mem);
	tmpNode.Init();
	tmpNode.SetLeaf(true);

	cTreeNode<TKey>::Split(newNode, tmpNode, buffers);

	/*
	Split_Ordering(TKey_mbrLo, TKey_mbrHi, 0, parent::mItemCount-1, mbrSide, mbr1Lo, mbr1Hi, mbr2Lo, mbr2Hi, itemBuffer, itemBuffer2);
	
	TKey::Print(mbr1Lo, " x ", sd);
	TKey::Print(mbr1Hi, "\n", sd);
	TKey::Print(mbr2Lo, " x ", sd);
	TKey::Print(mbr2Hi, "\n", sd);

	BuildTwoOrderNodes(newNode, tmpNode, mbr1Lo, mbr1Hi, itemBuffer, itemBuffer2, dataBuffer, 0, parent::mItemCount-1);

	TKey::Print(mbr1Lo, " x ", sd);
	TKey::Print(mbr1Hi, "\n", sd);
	TKey::Print(mbr2Lo, " x ", sd);
	TKey::Print(mbr2Hi, "\n", sd);
	*/

	// free temporary memory
	tmpNode.SetData(NULL);
	parent::mHeader->GetMemoryManager()->ReleaseMem(memBlock);
}


/**
 * Split node into this and newNode.
 */
//template<class TKey>
//void cUBTreeLeafNode<TKey>::Split_Ordering(cUBTreeLeafNode<TKey> &newNode, int ordering)
//{
	//cUBTreeLeafNode<TKey> *tmpNode = (cUBTreeLeafNode<TKey>*)((cTreePool<cRTreeItem<TKey>, TKey, cRTreeNode<TKey, cRTreeItem<TKey>>, cUBTreeLeafNode<TKey>>*)(parent::GetNodeHeader()->GetTreePool()))->GetLeafNode(0);
	//tmpNode->ClearItemOrder();

	//// SpaceOrderingSort();

	//// set true if the item was the minimal item
	//bool* minIndexArray = (bool*)parent::GetNodeHeader()->GetPool()->GetMem(parent::mItemCount);
	//int index = 0;

	//for (unsigned int i = 0 ; i < parent::mItemCount ; i++)
	//{
	//	int min;

	//	for (unsigned int j = 0 ; j < parent::mItemCount ; j++)
	//	{
	//		if (!minIndexArray[j])
	//		{
	//			min = j;
	//			break;
	//		}
	//	}

	//	for (unsigned int j = 0 ; j < parent::mItemCount-1 ; j++)
	//	{
	//		int res;
	//		if (!minIndexArray[j+1])
	//		{
	//			switch (ordering)
	//			{
	//				case cRTreeConst::Node_Split_HilbertOrdering:
	//					res = parent::GetItem(min)->GetTuple()->CompareHOrder(parent::GetItem(j+1)->GetRefTuple());
	//					break;
	//				case cRTreeConst::Node_Split_ZOrdering:
	//					res = parent::GetItem(min)->GetTuple()->CompareZOrder(parent::GetItem(j+1)->GetRefTuple());
	//					break;
	//				default: /* cRTreeConst::Node_Split_TaxiOrdering */
	//					res = parent::GetItem(min)->GetTuple()->CompareTaxiOrder(parent::GetItem(j+1)->GetRefTuple());
	//					break;
	//			}
	//			if (res == 1)
	//			{
	//				min = j+1;
	//			}
	//		}
	//	}
	//	minIndexArray[min] = true;
	//	tmpNode->SetItem(index++, GetRefItem(min));
	//}

	//// free memory
	//parent::GetNodeHeader()->GetPool()->FreeMem(minIndexArray);

	//tmpNode->SetItemCount(index);

	//if (parent::mDebug)
	//{
	//	printf("\n\n");
	//	tmpNode->Print();
	//	printf("\n\n");
	//}

	///*tmpNode->ClearItemOrder();
	//for (int i = 0 ; i < parent::mItemCount ; i++)
	//{
	//	tmpNode->SetItem(i, GetRefItem(i));
	//}*/

	//// now, tuples are sorted in this node => divide
	//unsigned int cnt = parent::mItemCount/2;
	//ClearItemOrder();
	//newNode.ClearItemOrder();
	//int order1 = 0, order2 = 0;

	//for (unsigned int i = 0 ; i < parent::mItemCount ; i++)
	//{
	//	if (i < cnt)
	//	{
	//		SetItem(order1++, tmpNode->GetRefItem(i));
	//	}
	//	else
	//	{
	//		newNode.SetItem(order2++, tmpNode->GetRefItem(i));
	//	}
	//}

	//// set item count
	//newNode.SetItemCount(order2);
	//SetItemCount(order1);

	//if (parent::mDebug)
	//{
	//	printf("\n\n");
	//	Print();
	//	printf("\n\n");
	//	newNode.Print();
	//	printf("\n\n");
	//}

	//mTreeHeader->IncrementNodeCount();
//}

/// Sort nodes's tuples according values in dimension. Select-Sort is applied.
template<class TKey>
bool cUBTreeLeafNode<TKey>::SortBy(unsigned int dimension, cNodeBuffers<TKey>* buffers)
{
	bool sortedFlag = true;
	bool sameValues = true;
	typedef TKey TItem;
	const cSpaceDescriptor *sd = GetSpaceDescriptor();

	// check if the sequence is sorted
	for (unsigned int i = 0 ; i < parent::mItemCount-1 ; i++)
	{
		int cmp = TKey::Equal(GetCItem(i, &buffers->itemBuffer), GetCItem(i + 1, &buffers->itemBuffer2), dimension, sd);
		if (cmp > 0)
		{
			sortedFlag = false;
			sameValues = false;
			break;
		}
		else if (cmp < 0)
		{
			sameValues = false;
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
				if (TKey::Equal(GetCItem(j, &buffers->itemBuffer), GetCItem(min, &buffers->itemBuffer2), dimension, sd) < 0)
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

	return sameValues;
}

/// Sort according to H-ordering.
template<class TKey>
void cUBTreeLeafNode<TKey>::SpaceOrderingSort()
{
	bool sortedFlag = true;

	// check if the sequence is sorted
	for (unsigned int i = 0 ; i < parent::mItemCount-1 ; i++)
	{
		if (parent::GetItem(i)->GetTuple()->CompareHOrder(parent::GetItem(i+1)->GetRefTuple()) > 0)
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
				if (parent::GetItem(j)->GetTuple()->CompareHOrder(parent::GetItem(min)->GetRefTuple()) < 0)
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

/// Create the MBR of tuples from order interval in the node.
template<class TKey>
void cUBTreeLeafNode<TKey>::CreateMbr(unsigned int startOrder, unsigned int finishOrder, char* TKey_ql, char* TKey_qh, cNodeBuffers<TKey>* buffers) const
{
	const cSpaceDescriptor *sd = GetSpaceDescriptor();
	sItemBuffers* itemBuffer = &buffers->itemBuffer;

	TKey::Copy(TKey_ql, parent::GetCKey(startOrder, itemBuffer), sd); 
	TKey::Copy(TKey_qh, parent::GetCKey(startOrder, itemBuffer), sd); 

	for (unsigned int i = startOrder+1 ; i <= finishOrder ; i++)
	{
		cMBRectangle<TKey>::ModifyMbr(TKey_ql, TKey_qh, parent::GetCKey(i, itemBuffer), sd); 
	}
}

/// Create the MBR of tuples in the node.
template<class TKey>
void cUBTreeLeafNode<TKey>::CreateMbr(char* TKey_ql, char* TKey_qh, cNodeBuffers<TKey>* buffers) const //bas064
{
	const cSpaceDescriptor *sd = GetSpaceDescriptor();
	sItemBuffers* itemBuffer = &buffers->itemBuffer;

	TKey::Copy(TKey_ql, parent::GetCKey(0, itemBuffer), sd);
	TKey::Copy(TKey_qh, parent::GetCKey(0, itemBuffer), sd);

	for (unsigned int i = 1 ; i <= parent::mItemCount - 1 ; i++) 
	{
		cMBRectangle<TKey>::ModifyMbr(TKey_ql, TKey_qh, parent::GetCKey(i, itemBuffer), sd);
	}
}

// in the case of buffered read we need to pop the leaf indices, that are added to array leafIndices
template<class TKey>
void cUBTreeLeafNode<TKey>::FindRelevantQueries(unsigned int currentLevel, cArray<unsigned int>** currentQueryPath, cArray<unsigned int>* queryIndices) 
{
	unsigned int count = currentQueryPath[currentLevel]->Count();

	for (unsigned int i = 0 ; i < count; i++)
	{
		queryIndices->Add(currentQueryPath[currentLevel]->GetRefItem(i));
	}
}

// Multi seach applied in the case of buffered read (after tree exploration)
template<class TKey>
void cUBTreeLeafNode<TKey>::BatchSearchInBlock(const TKey* qls, const TKey* qhs, cTreeItemStream<TKey>* resultSet, cRangeQueryConfig *rqConfig, cNodeBuffers<TKey>* buffers,  cArray<unsigned int>* queryIndices, unsigned int* resultSizes) const 
{
	const cSpaceDescriptor *sd = GetSpaceDescriptor();

	unsigned int count = queryIndices->Count();
	for (unsigned int i = 1 ; i < count; i++)
	{
		unsigned int queryOrder = queryIndices->GetRefItem(i);

		if (rqConfig->GetFinalResultSize() == 0 ||  rqConfig->GetFinalResultSize() != resultSizes[queryOrder])
		{
			for (unsigned int j = 0 ; j < parent::mItemCount ; j++)
			{
				if (cMBRectangle<TKey>::IsInRectangle(qls[queryOrder], qhs[queryOrder], GetCItem(j, buffers), sd))
				{
					resultSet->Add(GetCItem(j, buffers));
					resultSizes[queryOrder]++;

					if (rqConfig->GetFinalResultSize() != 0 &&  rqConfig->GetFinalResultSize() == resultSizes[queryOrder])
					{
						break;
					}
				}
			}
		}
	}
}

///
/// \param finishResultSize Finish this search after finding the number of tuples
/// \param currentOrder The starting point (tuple order) of the search
template<class TKey>
int cUBTreeLeafNode<TKey>::SearchInBlock(const TKey &ql, const TKey &qh, cTreeItemStream<TKey>* resultSet, unsigned int finishResultSize, unsigned int currentOrder, cNodeBuffers<TKey>* buffers) const //bas064
{
	TMP_SearchInBlock_COUNT++;

	const cSpaceDescriptor *sd = GetSpaceDescriptor();

	/*if (nSigDebug)
	{
		if (GetSignatureQuality(ql, qh, nQBConst))
		{
			nSigQCounter++;
		}
	}*/

	int ret = NO_ITEM_FIND;
	unsigned int startingOrder;
	if (currentOrder != UINT_MAX)
	{
		startingOrder = currentOrder;
	}
	else
	{
		startingOrder = 0;
	}

	for (unsigned int i = startingOrder ; i < parent::mItemCount ; i++)
	{
		if (cMBRectangle<TKey>::IsInRectangle(ql.parent::GetData(), qh.parent::GetData(), GetCItem(i, buffers), sd))
		{
			resultSet->Add(GetCItem(i, buffers));
			//mTreeHeader->GetQueryStatistics()->GetCounter(cRTreeConst::Counter_resultSize)->Increment();
			ret = i;

			if (finishResultSize != 0 && finishResultSize == resultSet->GetItemCount())
			{
				break;
			}
		} 
	}

	if (ret != NO_ITEM_FIND)
	{
		TMP_RelevantSearchInBlock_COUNT++;
		// old: mTreeHeader->GetQueryStatistics()->GetCounter(cRTreeConst::Counter_relevantRegions)->Increment();
	}

	//if (mTreeHeader->GetMeasureTime())
	//{
	//	mTreeHeader->GetQueryStatistics()->GetTimer(cRTreeConst::Timer_searchRegions)->Stop();
	//}

	return ret;
}

/**
 * Insert the tuple into the more more appropriate leaf node.
 * This method is invoced after the split operation.
 **/
template<class TKey>
void cUBTreeLeafNode<TKey>::InsertTuple(cUBTreeLeafNode<TKey>* node1, cUBTreeLeafNode<TKey>* node2, const TKey &tuple, cUBTreeLeafNodeHeader<TKey>* nodeHeader, char* leafData, cNodeBuffers<TKey>* buffers)
{
	cMemoryPool* pool = nodeHeader->GetMemoryPool();
	// Warning: It works only for TMbr = cMBRectangle, TKey = cTuple
	unsigned int mbrSize = 2 * nodeHeader->GetKeySize();
	char* cMbr_mbr1 = pool->GetMem(mbrSize);
	char* cMbr_mbr2 = pool->GetMem(mbrSize);
	cUBTreeLeafNode<TKey>* node = NULL;
	char* cTuple_tuple = tuple.GetData();
	const cSpaceDescriptor* sd = nodeHeader->GetSpaceDescriptor();

	node1->CreateMbr(TMbr::GetLoTuple(cMbr_mbr1), TMbr::GetHiTuple(cMbr_mbr1, sd), buffers);
	node2->CreateMbr(TMbr::GetLoTuple(cMbr_mbr2), TMbr::GetHiTuple(cMbr_mbr2,  sd), buffers);

	if (!TMbr::IsInRectangle(cMbr_mbr1, cTuple_tuple, sd))
	{
		if (TMbr::IsInRectangle(cMbr_mbr2, cTuple_tuple, sd))
		{
			node = node2;
		}
	}
	else
	{
		node = node1;
	}

	// unfortunately, tuple is not matched by any of two MBRs
	if (node == NULL)
	{
		char* cMbr_newMbr1 = pool->GetMem(mbrSize);
		char* cMbr_newMbr2 = pool->GetMem(mbrSize);

		cNodeItem::Copy(cMbr_newMbr1, cMbr_mbr1, mbrSize);
		cNodeItem::Copy(cMbr_newMbr2, cMbr_mbr2, mbrSize);

		// insert the tuple into both Mbrs
		TMbr::ModifyMbr(cMbr_newMbr1, cTuple_tuple, sd);
		bool mbr1Flag = TMbr::IsIntersected(cMbr_newMbr1, cMbr_mbr2, sd);

		TMbr::ModifyMbr(cMbr_newMbr2, cTuple_tuple, sd);
		bool mbr2Flag = TMbr::IsIntersected(cMbr_newMbr2, cMbr_mbr1, sd);

		if (mbr1Flag & mbr2Flag)
		{
			printf("Critical Error: cUBTreeLeafNode<TKey>::InsertTuple(): MBRs are intersected!\n");
		}
		else if (!(mbr1Flag | mbr2Flag))
		{
			// in both cases MBRs are not intersected, compute volumes and set the right MBR
			double volume1 = TMbr::Volume(cMbr_mbr1, sd);
			double volume2 = TMbr::Volume(cMbr_mbr2, sd);

			double volume1_new = TMbr::Volume(cMbr_newMbr1, sd);
			double volume2_new = TMbr::Volume(cMbr_newMbr2, sd);

			if (volume1_new - volume1 < volume2_new - volume2)
			{
				node = node1;
			}
			else
			{
				node = node2;
			}
		}
		else if (mbr1Flag)
		{
			node = node2;  // the mbr of modified MBR2 is not intersected
		}
		else
		{
			node = node1;  // the mbr of modified MBR1 is not intersected
		}

		// free temporary memory
		pool->FreeMem(cMbr_newMbr1);
		pool->FreeMem(cMbr_newMbr2);
	}

	if (nodeHeader->GetOrderingEnabled())
	{
		node->InsertLeafItem(tuple, leafData, nodeHeader->DuplicatesAllowed(), buffers);
	}
	else
	{
		node->AddLeafItem(tuple, leafData, true, buffers);
	}

	// free temporary memory
	pool->FreeMem(cMbr_mbr1);
	pool->FreeMem(cMbr_mbr2);
}

/**
 * Insert the tuple into the more more appropriate leaf node.
 * This method is invoced after the split operation.
 **/

template<class TKey>
void cUBTreeLeafNode<TKey>::InsertTuple_Ordered(cUBTreeLeafNode<TKey>* node1, cUBTreeLeafNode<TKey>* node2, const TKey &tuple 
	, cUBTreeLeafNodeHeader<TKey>* nodeHeader, char* leafData, char* buffer)
{
	cMemoryPool* pool = nodeHeader->GetMemoryPool();
	// Warning: It works only for TMbr = cMBRectangle, TKey = cTuple
	unsigned int mbrSize = 2 * nodeHeader->GetKeySize();
	char* cMbr_mbr1 = pool->GetMem(mbrSize);
	char* cMbr_mbr2 = pool->GetMem(mbrSize);
	cUBTreeLeafNode<TKey>* node = NULL;
	char* cTuple_tuple = tuple.parent::GetData();
	const cSpaceDescriptor* sd = nodeHeader->GetSpaceDescriptor();
	bool debug = false;

	node1->CreateMbr(TMbr::GetLoTuple(cMbr_mbr1), TMbr::GetHiTuple(cMbr_mbr1, sd), buffer);
	node2->CreateMbr(TMbr::GetLoTuple(cMbr_mbr2), TMbr::GetHiTuple(cMbr_mbr2,  sd), buffer);

	if (!TMbr::IsInRectangle(cMbr_mbr1, cTuple_tuple, sd))
	{
		if (TMbr::IsInRectangle(cMbr_mbr2, cTuple_tuple, sd))
		{
			node = node2;
		}
	}
	else
	{
		node = node1;
	}

	// unfortunately, tuple is not matched by any of two MBRs
	if (node == NULL)
	{
		char* cMbr_newMbr1 = pool->GetMem(mbrSize);
		char* cMbr_newMbr2 = pool->GetMem(mbrSize);

		cNodeItem::Copy(cMbr_newMbr1, cMbr_mbr1, mbrSize);
		cNodeItem::Copy(cMbr_newMbr2, cMbr_mbr2, mbrSize);

		// insert the tuple into both Mbrs
		TMbr::ModifyMbr(cMbr_newMbr1, cTuple_tuple, sd);
		bool mbr1Flag = TMbr::IsIntersected(cMbr_newMbr1, cMbr_mbr2, sd);

		TMbr::ModifyMbr(cMbr_newMbr2, cTuple_tuple, sd);
		bool mbr2Flag = TMbr::IsIntersected(cMbr_newMbr2, cMbr_mbr1, sd);

		if (mbr1Flag & mbr2Flag)
		{
			printf("Critical Error: cUBTreeLeafNode<TKey>::InsertTuple(): MBRs are intersected!\n");
		}
		else if (!(mbr1Flag | mbr2Flag))
		{
			// in both cases MBRs are not intersected, compute volumes and set the right MBR
			double volume1 = TMbr::Volume(cMbr_mbr1, sd);
			double volume2 = TMbr::Volume(cMbr_mbr2, sd);

			double volume1_new = TMbr::Volume(cMbr_newMbr1, sd);
			double volume2_new = TMbr::Volume(cMbr_newMbr2, sd);

			if (volume1_new - volume1 < volume2_new - volume2)
			{
				node = node1;
			}
			else
			{
				node = node2;
			}
		}
		else if (mbr1Flag)
		{
			node = node2;  // the mbr of modified MBR2 is not intersected
		}
		else
		{
			node = node1;  // the mbr of modified MBR1 is not intersected
		}

		// free temporary memory
		pool->FreeMem(cMbr_newMbr1);
		pool->FreeMem(cMbr_newMbr2);
	}

	//fk pro standard rtree na konec: node->AddLeafItem(tuple, leafData, buffer);
	node->InsertLeafItem(tuple, leafData, buffer);

	// free temporary memory
	pool->FreeMem(cMbr_mbr1);
	pool->FreeMem(cMbr_mbr2);
}



#ifdef CUDA_ENABLED
#include "dstruct/paged/rtree/cUBTreeLeafNode_Gpu.h"
#endif

}}}
#endif