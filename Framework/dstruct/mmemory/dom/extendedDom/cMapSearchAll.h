/**
*	\file cMapSearchAll.h
*	\author Radim Baca
*	\version 0.1
*	\date jan 2009
*	\brief Implement a function search algorithm.
*/


#ifndef __cMapSearchAll_h__
#define __cMapSearchAll_h__

#include "dstruct/mmemory/dom/extendedDom/type/cMapSearchPair.h"
#include "dstruct/mmemory/dom/extendedDom/cMapSearchInput.h"
#include "common/memorystructures/cGeneralFactory.h"
#include "cSizeInfo.h"
#include "cBasicType.h"
#include "cStack.h"
#include "cArray.h"

/**
* Implement a mapping search algorithm. 
* Problem definition:
* Input: We have an input oriented K(n,m) graph, where each arrow point from left side to right side. 
* Each side of graph has one X vertex and X vetexes are not connected. Each edge is parametrized by value.
* Output: Sub-graph having minimal edge values sum, where every non-X vertex has only one edge.
*
*	\author Radim Baca
*	\version 0.1
*	\date jan 2009
**/
class cMapSearchAll
{
	unsigned short			mMinimalValue;
	cStack<unsigned int>*	mStack;					/// Edge order for each left node
	cArray<unsigned int>*	mResult;				/// Store edge order for the minimal mapping

	void ResolveRest(unsigned short change, unsigned short maxChangeValue, cMapSearchInput* input);
	void FindMappingR(unsigned int leftNodeOrder, unsigned short change, unsigned short maxChangeValue, cMapSearchInput* input);
public:
	cMapSearchAll();
	~cMapSearchAll();

	inline void Init();
	inline void Delete();

	unsigned short FindMapping(cMapSearchPair* root, unsigned short maxChangeValue, cMapSearchInput* input);
};

#endif