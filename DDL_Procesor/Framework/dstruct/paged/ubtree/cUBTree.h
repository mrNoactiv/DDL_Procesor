/**
*	\file cUBTree.h
*	\author Michal Kratky
*	\version 0.1
*	\date 2001
*	\brief Implementation of the UB-tree.
*/

#ifndef __cUBTree_h__
#define __cUBTree_h__

#include "dstruct/paged/ubtree/cCommonUBTree.h"
#include "dstruct/paged/ubtree/cUBTreeNode.h"
#include "dstruct/paged/ubtree/cUBTreeLeafNode.h"
#include "common/datatype/tuple/cMBRectangle.h"
#include "dstruct/paged/rtree/cRTreeConst.h"

#include "dstruct/paged/ubtree/cBitAddress.h"
#include "dstruct/paged/ubtree/cZRegion.h"

// using namespace common::datatype::tuple;

/**
* It implements a persistent R*-tree.
* Parameters of the template:
*		- TNodeItem - 
*		- TLeafNodeItem - 
*
*	\author Michal Kratky
*	\version 0.3
*	\date jul 2011
**/

namespace dstruct {
	namespace paged {
		namespace ubtree {

//template<class TKey>
/*class cUBTree: public cCommonUBTree<cMBRectangle<TKey>, TKey, cUBTreeNode<cMBRectangle<TKey>>, cUBTreeLeafNode<TKey>>
{
	typedef cCommonUBTree<cMBRectangle<TKey>, TKey, cUBTreeNode<cMBRectangle<TKey>>, cUBTreeLeafNode<TKey>> parent;
*/

typedef cBitAddress TKey;
typedef cZRegion TInnerKey;

template <class TKey, class TInnerKey>
class cUBTree : public cCommonUBTree<TKey, cUBTreeNode<TInnerKey>, cUBTreeLeafNode<TKey>>
{
	typedef cCommonUBTree<TKey, cUBTreeNode<TInnerKey>, cUBTreeLeafNode<TKey>> parent;
private:
	virtual void PreRangeQuery(const TKey &ql, const TKey &qh);
	virtual void PreComplexRangeQuery(unsigned int qbCount);
public:
	cUBTree();
	~cUBTree();
};

/**
 * Create the UB-tree according to header.
 */
template <class TKey, class TInnerKey>
cUBTree<TKey,TInnerKey>::cUBTree() : parent()
{
	parent::mDebug = false;
}

template<class TKey, class TInnerKey>
cUBTree<TKey,TInnerKey>::~cUBTree() 
{
	// cCommonRTree<cRTreeItem<TKey>,cRTreeLeafItem,cRTreeNode<cRTreeItem<TKey>, TLeafData>,cRTreeLeafNode>::~cCommonRTree<cRTreeItem<TKey>,cRTreeLeafItem,cRTreeNode<cRTreeItem<TKey>, TLeafData>,cRTreeLeafNode>();
}

template<class TKey, class TInnerKey>
void cUBTree<TKey,TInnerKey>::PreRangeQuery(const TKey &ql, const TKey &qh)
{
	UNUSED(ql);
	UNUSED(qh);
}

template<class TKey, class TInnerKey>
void cUBTree<TKey,TInnerKey>::PreComplexRangeQuery(unsigned int qbCount)
{
	UNUSED(qbCount);
}
}}}
#endif