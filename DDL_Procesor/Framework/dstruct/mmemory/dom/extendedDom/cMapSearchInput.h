/**
*	\file cMapSearchInput.h
*	\author Radim Baca
*	\version 0.1
*	\date jan 2009
*	\brief Input of a cMapSearchAll class
*/


#ifndef __cMapSearchInput_h__
#define __cMapSearchInput_h__

#include "dstruct/mmemory/dom/extendedDom/type/cMapSearchPair.h"
#include "cArray.h"
#include "cSortedArrayWithLeaf.h"
#include "cBasicType.h"
#include "cSizeInfo.h"

/**
*	Input of a cMapSearchAll class. Edges must be inserted in the order of the left nodes.
*
*	\author Radim Baca
*	\version 0.1
*	\date jan 2009
**/
class cMapSearchInput
{
	typedef cSortedArrayWithLeaf<cUIntType, cBasicType<cMapSearchPair*>> tSortedArray;
	typedef cArray<tSortedArray*> tLeftMappingType;

	unsigned int							mRightOccupiedCount;	/// Number of right siblings covered by left side.
	unsigned int							mLeftNodeCount;			/// Number of left nodes
	cArray<unsigned short>*					mRight;					/// Auxiliary array, where every right item corespond to one right sibling. If the value of the array item is not equal to EMPTY_VALUE, the right sibling is occupied by an edge from left side.
	tLeftMappingType*						mLeftToRightMapping;	/// Values of mapping from the source DOM to a destination DOM.

	cSizeInfo<unsigned int>*				mKeySizeInfo;			/// Sorted arrays key size info.
	cSizeInfo<cMapSearchPair*>*				mLeafSizeInfo;			/// Sorted arrays leaf size info.

public:

	const static unsigned short EMPTY_VALUE = 0xffff;

	cMapSearchInput();
	~cMapSearchInput();

	void Init();
	void Delete();

	inline void Clear();
	bool AddEdge(unsigned int value, cMapSearchPair* head);
	inline cMapSearchPair* GetEdge(unsigned int leftNode, unsigned int edge)	{ return (*mLeftToRightMapping->GetItem(leftNode))->GetRefSortedLeaf(edge); }
	inline unsigned int GetEdgeValue(unsigned int leftNode, unsigned int edge)	{ return (*mLeftToRightMapping->GetItem(leftNode))->GetRefSortedItem(edge); }
	inline unsigned int GetEdgeCount(unsigned int leftNode) const				{ return (*mLeftToRightMapping->GetItem(leftNode))->GetItemCount(); }
	inline unsigned int GetLeftNodeCount() const								{ return mLeftNodeCount; }
	inline unsigned int GetLeftMappingNodesCount() const						{ return mLeftToRightMapping->Count(); }
	inline unsigned int GetRightOccupiedCount()	const							{ return mRightOccupiedCount; }
	inline cArray<unsigned short>* GetRightArray()								{ return mRight; }

	void Print();
};

/// Clear the arrays
void cMapSearchInput::Clear()
{
	mRight->ClearCount();
	mRightOccupiedCount = 0;
	mLeftNodeCount = 0;
	mLeftToRightMapping->ClearCount();

}


#endif