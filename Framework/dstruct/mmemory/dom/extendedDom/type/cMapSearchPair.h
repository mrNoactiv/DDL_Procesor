/**
*	\file cMapSearchPair.h
*	\author Radim Baca
*	\version 0.1
*	\date jan 2009
*	\brief 
*/

#include <stdio.h>

#ifndef __cMapSearchPair_h__
#define __cMapSearchPair_h__

/**
* Item used by cMapSearchAll class as one item. Linked list of mapping search items create mapping tree.
* Mapping tree is created by a cExtendedDomTree::CheckDOMSubtree() method. Every mapping tree has its weight,
* which is the value mChangesCount of its root mapping seach item.
*
*	\author Radim Baca
*	\version 0.1
*	\date jan 2009
**/
class cMapSearchPair
{
	unsigned short		mLeft;
	unsigned short		mRight;
	cMapSearchPair*		mNext;						/// pointer to the next mapping pair in a preorder.
	cMapSearchPair*		mLastChild;					/// pointer to the last descentant (preorder) in this subtree.
	//unsigned short		mChildCount;			/// number of child mappings.
	//unsigned short		mRightSiblingsCount;	/// number of right siblings in the child mapping.
	unsigned char				mFlags;

	const static unsigned char RIGHT_IS_OPTIONAL = 0x01;			/// flag indicate that the mRight DOM node does not exist. Therefore, the whole DOM subtree on the left is inserted as optional.
	const static unsigned char CHANGE_TO_HASNEXT = 0x02;			/// flag indicate that the sibling on the right side should be changed from hasNext false into hasNext true.
	const static unsigned char LAST_IN_THE_MAPPING = 0x04;			/// flag indicate that the pair is the last in mapping group of pairs.
	const static unsigned char LAST_AMONG_SIBLINGS = 0x08;			/// flag indicate that the pair does not have next sibling.
	const static unsigned char RIGHT_SUBTREE_IS_OPTIONAL = 0x10;	/// flag indicate that the mRight subtree does not have required edges.
public:
	cMapSearchPair();
	~cMapSearchPair();

	inline void Clear();

	inline unsigned short GetLeft() const			{ return mLeft; }
	inline unsigned short GetRight() const			{ return mRight; }
	inline cMapSearchPair* GetRefNext()				{ return mNext; }
	inline cMapSearchPair& GetNext() const			{ return *mNext; }
	inline cMapSearchPair* GetRefLastChild() const	{ return mLastChild; }
	inline cMapSearchPair& GetLastChild() const		{ return *mLastChild; }
	inline bool GetIsRightOptional() const			{ return (mFlags & RIGHT_IS_OPTIONAL) > 0; }
	inline bool GetChangeToHasNext() const			{ return (mFlags & CHANGE_TO_HASNEXT) > 0; }
	inline bool GetIsLastInMapping() const			{ return (mFlags & LAST_IN_THE_MAPPING) > 0; }
	inline bool GetLastAmongSibling() const			{ return (mFlags & LAST_AMONG_SIBLINGS) > 0;}
	inline bool GetIsRightSubtreeOptional() const	{ return (mFlags & RIGHT_SUBTREE_IS_OPTIONAL) > 0; }
	//inline unsigned short GetChildCount() const		{ return mChildCount; }
	//inline unsigned short GetRightSiblingCount() const	{ return mRightSiblingsCount; }

	inline void SetLeft(unsigned short left)		{ mLeft = left; }
	inline void SetRight(unsigned short right)		{ mRight = right; }
	inline void SetNext(cMapSearchPair* next)		{ mNext = next; }
	inline void SetLastChild(cMapSearchPair* last)	{ mLastChild = last; }
	inline void SetLeftRight(unsigned short left, unsigned short right)	{ mLeft = left; mRight = right; }
	inline void SetIsRightOptional(bool optional);
	inline void SetChangeToHasNext(bool hasNext);
	inline void SetIsLastInMapping(bool lastInMapping);
	inline void SetLastAmongSiblings(bool last);
	inline void SetIsRightSubtreeOptional(bool optional);
	//inline void SetChildCount(unsigned short count)	{ mChildCount = count; }
	//inline void SetRightSiblingCount(unsigned short rightSibling)		{ mRightSiblingsCount = rightSibling; }

	inline void operator = (const cMapSearchPair &pair);
	//inline bool operator == (const cMapSearchPair &pair) const;
	//inline bool operator != (const cMapSearchPair &pair) const;

	void Print(char *string) const;
};


void cMapSearchPair::Clear()
{
	mLeft = 0;
	mRight = 0;
	mNext = NULL;
	mLastChild = this;
	mFlags = (unsigned char)0;
}

void cMapSearchPair::operator = (const cMapSearchPair &pair)
{
	mLeft = pair.GetLeft();
	mRight = pair.GetRight();
	mNext = ((cMapSearchPair&)pair).GetRefNext();
	mLastChild = ((cMapSearchPair&)pair).GetRefLastChild();
	//mChildCount = pair.GetChildCount();
	//mRightSiblingsCount = pair.GetRightSiblingCount();
}

void cMapSearchPair::SetIsRightOptional(bool optional)
{
	if (optional)
	{
		mFlags |= RIGHT_IS_OPTIONAL;
	} else
	{
		mFlags &= ~RIGHT_IS_OPTIONAL;
	}
}

void cMapSearchPair::SetChangeToHasNext(bool hasNext)
{
	if (hasNext)
	{
		mFlags |= CHANGE_TO_HASNEXT;
	} else
	{
		mFlags &= ~CHANGE_TO_HASNEXT;
	}
}

void cMapSearchPair::SetIsLastInMapping(bool lastInMapping)
{
	if (lastInMapping)
	{
		mFlags |= LAST_IN_THE_MAPPING;
	} else
	{
		mFlags &= ~LAST_IN_THE_MAPPING;
	}
}

void cMapSearchPair::SetLastAmongSiblings(bool last)
{
	if (last)
	{
		mFlags |= LAST_AMONG_SIBLINGS;
	} else
	{
		mFlags &= ~LAST_AMONG_SIBLINGS;
	}
}


void cMapSearchPair::SetIsRightSubtreeOptional(bool optional)
{
	if (optional)
	{
		mFlags |= RIGHT_SUBTREE_IS_OPTIONAL;
	} else
	{
		mFlags &= ~RIGHT_SUBTREE_IS_OPTIONAL;
	}
}

#endif