/**
*	\file cUBTreeNode.h
*	\author Michal Kratky
*	\version 0.1
*	\date 2001 - 2008
*	\brief Implementation of the R-tree's inner node.
*/

#ifndef __cUBTreeNode_h__
#define __cUBTreeNode_h__

#include <float.h>

namespace dstruct {
	namespace paged {
		namespace ubtree {
template<class TInnerKey> class cUBTreeNodeHeader;
}}}

#include "common/memorystructures/cStack.h"
#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "dstruct/paged/rtree/sItemIdRecord.h"
#include "dstruct/paged/ubtree/cCommonUBTreeNode.h"
#include "dstruct/paged/ubtree/cUBTreeNodeHeader.h"
#include "dstruct/paged/queryprocessing/cRangeQueryConfig.h"
#include "dstruct/paged/queryprocessing/cQueryProcStat.h"

using namespace common::datatype::tuple;

/**
* Class is parametrized:
*		- TMbr - Inherited from cBasicType. Inner type must be type inherited from cTuple. This type must implement operator = with cTuple as a parameter.
*		- TItem - Class representing the inner item of the inner node (for example cUBTreeItem<TMbr>).
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
		namespace ubtree {

template<class TInnerKey>
class cUBTreeNode : public cCommonUBTreeNode<TInnerKey>
{
	typedef cCommonUBTreeNode<TInnerKey> parent;

private:
	typedef typename cBitAddress TKey;
	void SplitCommon(cUBTreeNode<TInnerKey> &newNode); //fk

public:
	static const unsigned int RQ_NOINTERSECTION = (unsigned int)~0;
	cUBTreeNode(const cTreeNodeHeader* header, const char* mem);
	cUBTreeNode(const cUBTreeNode<TInnerKey>* origNode, const char* mem);
	cUBTreeNode(const cUBTreeNodeHeader* header, const char* mem);
	cUBTreeNode();
	cUBTreeNode(cTreeHeader* treeHeader);

	bool Insert(unsigned int itemOrder, const char* TMbr_item1, const char* TMbr_item2, tNodeIndex insNodeIndex);
	void Update(unsigned int itemOrder, const char* item1);
	void SortBy(unsigned int dimension);
	static void InsertTuple(cUBTreeNode<TInnerKey>* node1, cUBTreeNode<TInnerKey>* node2, char* TMbr_mbr, const tNodeIndex& insNodeIndex, cUBTreeNodeHeader<TInnerKey>* nodeHeader);
	inline const cSpaceDescriptor* GetSpaceDescriptor() const;

#ifdef CUDA_ENABLED
	inline void TransferInnerNodeToGpu();
	inline void SerializeKeys(uint* mbr, uint* children);
	inline void SerializeKeys(uint* mbr);
#endif
	void Print2File(FILE *streamInfo, uint order, bool relevant);

};

template<class TInnerKey>
void cUBTreeNode<TInnerKey>::Print2File(FILE *streamInfo, uint order, bool relevant)
{
	if (relevant)
		fprintf(streamInfo, "rel:  ");
	TKey::Print2File(streamInfo, TInnerKey::GetLoTuple(this->GetCKey(order)), "x", this->GetSpaceDescriptor());
	TKey::Print2File(streamInfo, TInnerKey::GetHiTuple(this->GetCKey(order), this->GetSpaceDescriptor()), "\n", this->GetSpaceDescriptor());
}

/**
 * This contructor is used in the case when the node is created on the pool, this pool creates a memory
 * and the node uses it.
 */
template<class TInnerKey>
cUBTreeNode<TInnerKey>::cUBTreeNode(const cTreeNodeHeader* header, const char* mem):  parent(header, mem)
{
}

template<class TInnerKey>
cUBTreeNode<TInnerKey>::cUBTreeNode(const cUBTreeNode<TInnerKey>* origNode, const char* mem): parent(origNode, mem)
{
}

template<class TInnerKey>
cUBTreeNode<TInnerKey>::cUBTreeNode():cCommonUBTreeNode<TInnerKey>() { }

/**
 * Update the item (some children have been updated).
 */
template <class TInnerKey>
void cUBTreeNode<TInnerKey>::Update(unsigned int itemOrder, const char* item1)
{
	const cSpaceDescriptor *sd = GetSpaceDescriptor();
	parent::UpdateItem(itemOrder, item1, sd);
}

template<class TInnerKey>
inline const cSpaceDescriptor* cUBTreeNode<TInnerKey>::GetSpaceDescriptor() const
{
	return (cSpaceDescriptor*)((cUBTreeNodeHeader<TInnerKey>*)parent::mHeader)->GetKeyDescriptor();
}


/// Split node into this and newNode.
template<class TInnerKey>
void cUBTreeNode<TInnerKey>::SplitCommon(cUBTreeNode &newNode)
{
	unsigned int order1 = 0, order2 = 0;
	cMemoryPool *pool = parent::GetNodeHeader()->GetMemoryPool();
	char* cRTNode_mem = pool->GetMem(parent::GetNodeHeader()->GetNodeInMemSize());
	cUBTreeNode tmpNode(this, cRTNode_mem);
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


/// Sort nodes's tuples according values in dimension.
template<class TInnerKey>
void cUBTreeNode<TInnerKey>::SortBy(unsigned int dimension)
{
	bool sortedFlag = true;
	const cSpaceDescriptor *sd = GetSpaceDescriptor();

	// check if the sequence is sorted
	for (unsigned int i = 0 ; i < parent::mItemCount-1 ; i++)
	{
		if (TKey::Equal(TMbr::GetHiTuple(parent::GetCKey(i), sd), TInnerKey::GetHiTuple(parent::GetCKey(i+1), sd), dimension, sd) > 0)
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
				if (TKey::Equal(TInnerKey::GetHiTuple(parent::GetCKey(j), sd), TInnerKey::GetHiTuple(parent::GetCKey(min), sd), dimension, sd) < 0)
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


/**
 * Insert the tuple into the more more appropriate node.
 * This method is invoced after the split operation.
 * \param cMbr_mbr, insNodeIndex - inserted item of the inner node
 **/
template<class TInnerKey>
void cUBTreeNode<TInnerKey>::InsertTuple(cUBTreeNode<TInnerKey>* node1, cUBTreeNode<TInnerKey>* node2, 
	char* TMbr_mbr, const tNodeIndex& insNodeIndex /*, char* TMbr_mbr2*/, cUBTreeNodeHeader<TInnerKey>* nodeHeader)
{
	cMemoryPool* pool = nodeHeader->GetMemoryPool();
	const cSpaceDescriptor* sd = nodeHeader->GetSpaceDescriptor();
	char* TMbr_mbr1 = pool->GetMem(nodeHeader->GetKeySize());
	char* TMbr_mbr2 = pool->GetMem(nodeHeader->GetKeySize());
	cUBTreeNode<TInnerKey>* node = NULL;

	node1->CreateMbr(TMbr_mbr1);
	node2->CreateMbr(TMbr_mbr2);

	if (!TInnerKey::IsContained(TMbr_mbr1, TMbr_mbr, sd))
	{
		if (TInnerKey::IsContained(TMbr_mbr2, TMbr_mbr, sd))
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
		double volume1 = TInnerKey::Volume(TMbr_mbr1, sd);
		double volume2 = TInnerKey::Volume(TMbr_mbr2, sd);

		TInnerKey::ModifyMbr(TMbr_mbr1, TMbr_mbr, sd);
		TInnerKey::ModifyMbr(TMbr_mbr2, TMbr_mbr, sd);

		double volume1_new = TInnerKey::Volume(TMbr_mbr1, sd);
		double volume2_new = TInnerKey::Volume(TMbr_mbr2, sd);

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


#ifdef CUDA_ENABLED
#include "dstruct/paged/ubtree/cUBTreeNode_Gpu.h"
#endif

}}}
#endif