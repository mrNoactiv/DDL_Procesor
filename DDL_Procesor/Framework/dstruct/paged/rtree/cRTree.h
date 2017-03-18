/**
*	\file cRTree.h
*	\author Michal Kratky
*	\version 0.1
*	\date 2001
*	\brief Implementation of the R-tree.
*/

#ifndef __cRTree_h__
#define __cRTree_h__

#include "dstruct/paged/rtree/cCommonRTree.h"
#include "dstruct/paged/rtree/cRTreeNode.h"
#include "dstruct/paged/rtree/cRTreeLeafNode.h"
#include "common/datatype/tuple/cMBRectangle.h"
#include "dstruct/paged/rtree/cRTreeConst.h"

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
		namespace rtree {

template<class TKey>
class cRTree: public cCommonRTree<cMBRectangle<TKey>, TKey, cRTreeNode<cMBRectangle<TKey>>, cRTreeLeafNode<TKey>>
{
	typedef cCommonRTree<cMBRectangle<TKey>, TKey, cRTreeNode<cMBRectangle<TKey>>, cRTreeLeafNode<TKey>> parent;
private:
	virtual void PreRangeQuery(const TKey &ql, const TKey &qh);
	virtual void PreComplexRangeQuery(unsigned int qbCount);

public:
	cRTree();
	~cRTree();
};

/**
 * Create the R-tree according to header.
 */
template<class TKey>
cRTree<TKey>::cRTree() : parent()
{
	parent::mDebug = false;
}

template<class TKey>
cRTree<TKey>::~cRTree() 
{
	// cCommonRTree<cRTreeItem<TKey>,cRTreeLeafItem,cRTreeNode<cRTreeItem<TKey>, TLeafData>,cRTreeLeafNode>::~cCommonRTree<cRTreeItem<TKey>,cRTreeLeafItem,cRTreeNode<cRTreeItem<TKey>, TLeafData>,cRTreeLeafNode>();
}

template<class TKey>
void cRTree<TKey>::PreRangeQuery(const TKey &ql, const TKey &qh)
{
	UNUSED(ql);
	UNUSED(qh);
}

template<class TKey>
void cRTree<TKey>::PreComplexRangeQuery(unsigned int qbCount)
{
	UNUSED(qbCount);
}
}}}
#endif