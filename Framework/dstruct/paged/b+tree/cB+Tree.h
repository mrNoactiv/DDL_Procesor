/**
*	\file cBpTree.h
*	\author Radim Baca
*	\version 0.1
*	\date 2007
*	\brief Implement persistent B-tree
*/

#ifndef __cBpTree_h__
#define __cBpTree_h__

#include "dstruct/paged/b+tree/cB+TreeConst.h"
#include "dstruct/paged/b+tree/cCommonB+Tree.h"
#include "dstruct/paged/b+tree/cB+TreeNode.h"

/**
* Implement persistent B-tree with fixed size of key and leaf data.
* Parameters of the template:
*		- TKey - Type of the key value.
*
*	\author Radim Baca
*	\version 0.1
*	\date may 2007
**/
namespace dstruct {
	namespace paged {
		namespace bptree {

template <class TKey> 
class cBpTree : public cCommonBpTree<cBpTreeNode<TKey>, cBpTreeNode<TKey>, TKey>
{
public:
	cBpTree();	
	~cBpTree();
};  

template <class TKey> 
cBpTree<TKey>::cBpTree() :cCommonBpTree<cBpTreeNode<TKey>, cBpTreeNode<TKey>, TKey>()
{
}

template <class TKey> 
cBpTree<TKey>::~cBpTree()
{
}
}}}
#endif
