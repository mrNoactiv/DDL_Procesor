/**************************************************************************}
{                                                                          }
{    cCommonUBTreeNode.h                                 		      				 }
{                                                                          }
{                                                                          }
{    Copyright (c) 2001 - 2003   			      Michal Kratky                  }
{                                                                          }
{    VERSION: 0.01													DATE 20/11/2003                }
{                                                                          }
{    following functionality:                                              }
{       common node of R-tree                                              }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      xx/xx/xxxx                                                          }
{                                                                          }
{**************************************************************************/

#ifndef __cCommonUBTreeNode_h__
#define __cCommonUBTreeNode_h__

#include "dstruct/paged/core/cTreeHeader.h"
#include "dstruct/paged/core/cTreeNode.h"
#include "dstruct/paged/core/cTreeNodeHeader.h"
#include "common/datatype/tuple/cSpaceDescriptor.h"

using namespace dstruct::paged::core;
using namespace common::datatype::tuple;

namespace dstruct {
	namespace paged {
		namespace ubtree {

template<class TItem> 
class cCommonUBTreeNode : public cTreeNode<TItem>
{
private:
	typedef cTreeNode<TItem> parent;
	
public:
	static bool DIFFERENT_CARDINALITY;            // choose algorithms for the same (<32b)/different cardinalities

	cCommonUBTreeNode(cTreeNodeHeader* header, const char* mem);
	cCommonUBTreeNode(const cCommonUBTreeNode<TItem>* origNode, const char* mem);

	inline void Clear();

	void CreateNewRootNode(cTreeHeader *mHeader, char* tMbr1, tNodeIndex nIndex1, char* tMbr2, tNodeIndex nIndex2);
	bool TestConsistence(const unsigned int maximal_value);

	void Print(const cSpaceDescriptor* pSd, sItemBuffers* buffers = NULL) const; //bas064
	
	// Peter Chovanec 23.1.2012
	inline void UpdateItem(unsigned int order, const char *item, const cSpaceDescriptor* pSd); 
	inline void UpdateMbr(unsigned int order, const char *TKey_item, const cSpaceDescriptor* pSd);

};

template<class TItem> bool cCommonUBTreeNode<TItem>::DIFFERENT_CARDINALITY = false;

/**
 * This contructor is used in the case when the node is created on the pool, this pool creates a memory
 * and the node uses it.
 */template<class TItem> 
cCommonUBTreeNode<TItem>::cCommonUBTreeNode(const cCommonUBTreeNode<TItem>* origNode, const char* mem): parent(origNode, mem)
{
}

 template<class TItem> 
cCommonUBTreeNode<TItem>::cCommonUBTreeNode(cTreeNodeHeader* header, const char* mem): parent(header, mem)
{
}

template<class TItem> 
void cCommonUBTreeNode<TItem>::Clear() 
{ 
	parent::mItemCount = 0; 
	SetFreeSize(parent::mHeader->GetNodeItemsSpaceSize());
	parent::Init();
}

template<class TItem> 
void cCommonUBTreeNode<TItem>::UpdateItem(unsigned int order, const char *item, const cSpaceDescriptor* pSd)
{
	TItem::Copy(parent::GetKeyPtr(order), item, pSd); //fk GetItem->GetKey
}

template<class TItem> 
void cCommonUBTreeNode<TItem>::UpdateMbr(unsigned int order, const char *TKey_item, const cSpaceDescriptor* pSd)
{
	TItem::ModifyMbr(parent::GetKeyPtr(order), TKey_item, pSd);
}

/// Test if the node contains reasonable values.
template<class TItem> 
bool cCommonUBTreeNode<TItem>::TestConsistence(const unsigned int maximal_value)
{
	bool pointToLeaf = parent::IsLeaf(parent::GetLink(0));

	if (parent::mItemCount > parent::mTreeHeader->GetNodeItemCapacity())
	{
		return false;
	}

	for (unsigned int i = 0; i < parent::mItemCount; i++)
	{
		if (!parent::mItemOrder[i]->TestConsistence(maximal_value))
		{
			return false;
		}

		if (pointToLeaf && !parent::IsLeaf(parent::GetLink(i)))
		{
			return false;
		}
		if (!pointToLeaf && parent::IsLeaf(parent::GetLinks(i)))
		{
			return false;
		}
	}

	return true;
}

/**
 * Create the new root node including two MBRs.
 **/
template<class TItem> 
void cCommonUBTreeNode<TItem>::CreateNewRootNode(cTreeHeader *mHeader, char* tMbr1, tNodeIndex nIndex1, char* tMbr2, tNodeIndex nIndex2)
{
	parent::AddItem(tMbr1, nIndex1, true);
	parent::AddItem(tMbr2, nIndex2, true);

	mHeader->SetRootIndex(parent::mIndex);
	mHeader->IncrementHeight();
	mHeader->IncrementInnerNodeCount();
}

template<class TItem> 
void cCommonUBTreeNode<TItem>::Print(const cSpaceDescriptor* pSd, sItemBuffers* buffers) const
{
	tNodeIndex index;
	if (parent::IsLeaf())
	{
		index = parent::GetNodeIndex(cNode::mIndex);
	}
	else
	{
		index = parent::mIndex;
	}
	printf("|| Index: %d | Item Count: %d ||\n", index, parent::mItemCount);

	for (unsigned int i = 0 ; i < parent::mItemCount ; i++) 
	{
		if (!parent::IsLeaf())
		{
			printf(" Child Node: %d |", GetNodeIndex(parent::GetLink(i)));
			// printf(" %d (%d) |", GetIndex(mLinks[i]), mLinks[i]);
		}
		printf(" ");
		printf("%d (%d): ", parent::GetItemPOrder(i), i);
		TItem::Print(parent::GetCKey(i, buffers), "\n", pSd);
		// old: GetRefItem(i).Print(mode);
	}
	printf("|\n");

	/* if (IsLeaf()) 
	{
		printf(" | next: %d | beta: ", mExtraLinks[0]);
	}*/
}

}}}
#endif   //  __cCommonUBTreeNode_h__