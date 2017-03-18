/*
	File:		cQTreeNode.h
	Author:		Tomas Plinta, pli040
	Version:	0.1
	Date:		2011
	Brief implementation of QuadTree node
*/

#ifndef __cQTreeNode_h__
#define __cQTreeNode_h__

#include "dstruct/paged/core/cFCNode.h"
#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "dstruct/paged/qtree/cQTreeNodeHeader.h"

using namespace dstruct::paged::core;
using namespace common::datatype::tuple;

typedef unsigned int uint;

namespace dstruct {
	namespace paged {
		namespace qtree {

template<class TKey>
class cQTreeNode: public cFCNode<TKey>
{

public:
	cQTreeNode(void);
	~cQTreeNode(void);

protected:
};

template<class TKey>
cQTreeNode<TKey>::cQTreeNode()
{
}

template<class TKey>
cQTreeNode<TKey>::~cQTreeNode()
{
}



}}}
#endif