/**
*	\file cBpTreeBulkLoad.h
*	\author Radim Baca
*	\version 0.1
*	\date may 2007
*	\brief Bulk loading of B-tree.
*/


#ifndef __cBpTreeBulkLoad_h__
#define __cBpTreeBulkLoad_h__

#include "cStream.h"

#include "cTreeItem_SizeInfo.h"
#include "cSortedLeafItemArray.h"
#include "cTreeBulkLoad.h"

/**
*	Bulk loading of B+-tree. Implement abstract method cTreeBulkLoad.
*
*	\author Radim Baca
*	\version 0.1
*	\date may 2007
**/

namespace dstruct {
	namespace paged {
		namespace bptree {

template<class BTree, class LIType, class II>
class cBpTreeBulkLoad: public cTreeBulkLoad<BTree, LIType, II>
{
	typedef typename LIType::Type LI;
private:
	virtual inline void SetInnerItem(II &innerItem, LI &leafItem);
public:
	cBpTreeBulkLoad(cTreeItem_SizeInfo<LI> *sizeInfo);
	~cBpTreeBulkLoad();

};

/// Constructor
template<class BTree, class LIType, class II>
cBpTreeBulkLoad<BTree, LIType, II>::cBpTreeBulkLoad(cTreeItem_SizeInfo<LI> *sizeInfo) :cTreeBulkLoad(sizeInfo)
{
}

/// Destructor
template<class BTree, class LIType, class II>
cBpTreeBulkLoad<BTree, LIType, II>::~cBpTreeBulkLoad()
{
}

template<class BTree, class LIType, class II>
void cBpTreeBulkLoad<BTree, LIType, II>::SetInnerItem(II &innerItem, LI &leafItem)
{
	innerItem = leafItem;
}
}}}
#endif