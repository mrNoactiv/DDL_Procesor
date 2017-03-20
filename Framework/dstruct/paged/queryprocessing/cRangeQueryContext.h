/**
*	\file cRangeQueryContext.h
*	\author Michal Kratky
*	\version 0.1
*	\date sep 2008
*	\brief This class mainly includes the current tree path. This is the starting point of the next range query. 
*/

#ifndef __cRangeQueryContext_h__
#define __cRangeQueryContext_h__

#include "common/datatype/tuple/cTuple.h"
#include "common/stream/cStream.h"
#include "dstruct/paged/core/cNode.h"

using namespace common::datatype::tuple;
using namespace dstruct::paged;

namespace dstruct {
	namespace paged {

/**
*	This class mainly includes the current tree path. This is the starting point of the next range query. 
*   Clear() must be called before the first query of a query sequence.
*
*	\author Michal Kratky
*	\version 0.1
*	\date sep 2008
**/
class cRangeQueryContext
{
private:
	static const unsigned int TREE_MAX_HEIGHT = 32;

	cTuple *mQl;
	cTuple *mQh;
	tNodeIndex mNodeIndexContext[TREE_MAX_HEIGHT];
	unsigned int mNodeOrderContext[TREE_MAX_HEIGHT];
	unsigned int mPathLength;
	unsigned int mOrder;
	bool mTreeEndReached;

public:
	cRangeQueryContext(cTuple *ql, cTuple *qh);

	inline cTuple* GetQlTuple();
	inline cTuple& GetRefQlTuple() const;
	inline cTuple* GetQhTuple();
	inline cTuple& GetRefQhTuple() const;

	inline void SetContext(unsigned int level, tNodeIndex nodeIndex, unsigned int nodeOrder);
	inline void GetContext(unsigned int level, tNodeIndex &nodeIndex, unsigned int &nodeOrder) const;
	inline tNodeIndex GetNodeIndexContext(unsigned int level) const;
	inline unsigned int GetNodeOrderContext(unsigned int level) const;

	inline void SetPathLength(unsigned int pathLength);
	inline unsigned int GetPathLength() const;

	inline void Clear();
	inline void IncrementOrder();
	inline unsigned int GetOrder() const;

	inline void SetTreeEndReached(bool flag);
	inline bool GetTreeEndReached() const;
};

cTuple* cRangeQueryContext::GetQlTuple()
{
	return mQl;
}

cTuple& cRangeQueryContext::GetRefQlTuple() const
{
	return *mQl;
}

cTuple* cRangeQueryContext::GetQhTuple()
{
	return mQh;
}

cTuple& cRangeQueryContext::GetRefQhTuple() const
{
	return *mQh;
}

/// Set the node index and current order in the node at a tree level.
void cRangeQueryContext::SetContext(unsigned int level, tNodeIndex nodeIndex, unsigned int nodeOrder)
{
	assert(level < TREE_MAX_HEIGHT && level <= mPathLength);
	mNodeIndexContext[level] = nodeIndex;
	mNodeOrderContext[level] = nodeOrder;
}

void cRangeQueryContext::GetContext(unsigned int level, tNodeIndex &nodeIndex, unsigned int &nodeOrder) const
{
	assert(level < TREE_MAX_HEIGHT && level <= mPathLength);
	nodeIndex = mNodeIndexContext[level];
	nodeOrder = mNodeOrderContext[level];
}

tNodeIndex cRangeQueryContext::GetNodeIndexContext(unsigned int level) const
{
	assert(level < TREE_MAX_HEIGHT && level <= mPathLength);
	return mNodeIndexContext[level];
}

unsigned int cRangeQueryContext::GetNodeOrderContext(unsigned int level) const
{
	assert(level < TREE_MAX_HEIGHT && level <= mPathLength);
	return mNodeOrderContext[level];
}

/// Set path length of the context
void cRangeQueryContext::SetPathLength(unsigned int pathLength)
{
	mPathLength = pathLength;
}

unsigned int cRangeQueryContext::GetPathLength() const
{
	return mPathLength;
}

/// This method must be called before the first uery of a query sequence
void cRangeQueryContext::Clear()
{
	mOrder = 0;
	mTreeEndReached = false;
}

void cRangeQueryContext::IncrementOrder()
{
	mOrder++;
}

unsigned int cRangeQueryContext::GetOrder() const
{
	return mOrder;
}

/// Set true if the tree's end is reached - it means that this range query should be the last query of a sequence
void cRangeQueryContext::SetTreeEndReached(bool flag)
{
	mTreeEndReached = flag;
}

bool cRangeQueryContext::GetTreeEndReached() const
{
	return mTreeEndReached;
}
}}
#endif