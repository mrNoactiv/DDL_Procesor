/**
*	\file cDomCursor.h
*	\author Radim Baca
*	\version 0.1
*	\date feb 2009
*	\brief Store all required information for one cursor into a cDomTree
*/


#ifndef __cDomCursor_h__
#define __cDomCursor_h__



#include "dstruct/mmemory/dom/cDomHeader.h"
#include "dstruct/mmemory/dom/cDomNode_Inner.h"
#include "dstruct/mmemory/dom/cDomNode_Leaf.h"
#include "dstruct/mmemory/dom/cDomStackItem.h"
#include "dstruct/mmemory/dom/cDomCache.h"

#include "common/memorystructures/cGeneralStack.h"
#include "common/cTreeTuple.h"
#include "common/datatype/tuple/cTreeTupleType.h"
#include "common/datatype/cTupleSizeInfo.h"
#include "common/cTreeSpaceDescriptor.h"

/**
* Store all required information for one cursor into a cDomTree. 
* We can move cursor in all dirfections in a cDomTree. cDomCursor contains mainly pasive methods,
* which only read DOM state and does not change any attributes values. The only exceptions is method
* SetOptional().
*
* Template parameters:
*	- TKeyItem - Have to be inherited from cBasicType. Represent type of the key value.
*	- TLeafItem - Have to be inherited from cBasicType. Represent type of the leaf value.
*
*	\author Radim Baca
*	\version 0.1
*	\date feb 2009
**/
template<class TKeyItem, class TLeafItem>
class cDomCursor
{
	typedef typename TKeyItem::Type KeyType;
	typedef typename TLeafItem::Type LeafType;

	unsigned int						mIndex;		/// Index of the node in a cache array.
	unsigned int						mOrder;		/// Order of the item in a node.
	unsigned int						mLevel;		/// Level of the actual DOM node in the DOM tree.
	cDomCache<TKeyItem, TLeafItem>*		mCache;		/// Dom tree.
	cTreeTuple*							mStackItem;	/// Stack item used as a auxiliary variable.
	cGeneralStack<cTreeTupleType>*		mStack;		/// Capture mIndex and mOrder informations on a path from root node to cursor node.
	cTupleSizeInfo*						mSizeInfo;

	void PushToStack();
	bool IsLeaf(unsigned int index)	{ return (index & INNER_NODE_FLAG) == 0;}

	/// This constants are duplicated in cDomTree!!
	static const unsigned int INNER_NODE_FLAG = 0x80000000;
	static const unsigned int EMPTY_POINTER = 0xffffffff;

public:
	cDomCursor();
	~cDomCursor();

	void Init();
	void Delete();
	inline void Clear(unsigned int newIndex);

	inline unsigned int GetIndex() const		{ return mIndex; }
	inline unsigned int GetOrder() const		{ return mOrder; }
	inline unsigned int GetLevel() const		{ return mLevel; }
	inline unsigned int GetPointer()			{ return mCache->GetLeafNode(mIndex)->GetPointer(mOrder); }
	inline const LeafType& GetLeaf()			{ return mCache->GetLeafNode(mIndex)->GetLeaf(mOrder); }
	inline const KeyType& GetKey()				{ return mCache->GetLeafNode(mIndex)->GetKey(mOrder); }	
	inline KeyType* GetRefKey()					{ return mCache->GetLeafNode(mIndex)->GetRefKey(mOrder); }	
	inline unsigned int GetNodeCount()			{ return mCache->GetLeafNode(mIndex)->GetNodeCount(mOrder); }
	inline bool IsOptional()					{ return mCache->GetLeafNode(mIndex)->IsOptional(mOrder); }
	inline const cDomCache<TKeyItem, TLeafItem>* GetCache() const	{ return mCache; }
	inline cDomCache<TKeyItem, TLeafItem>* GetRefCache()			{ return mCache; }
	inline const cGeneralStack<cTreeTupleType>* GetStack() const	{ return mStack; }
	inline cGeneralStack<cTreeTupleType>* GetRefStack()				{ return mStack; }

	inline void SetIndex(unsigned int index)	{ mIndex = index; }
	inline void SetOrder(unsigned int order)	{ mOrder = order; }
	inline void SetDOMCache(cDomCache<TKeyItem, TLeafItem>* domCache)	{ mCache = domCache; }
	inline void SetOptional()					{ mCache->GetLeafNode(mIndex)->SetOptional(mOrder); }

	void operator = (const cDomCursor& cursor);
	bool FindFirst(const KeyType* key = NULL, unsigned int root = EMPTY_POINTER);
	bool FindNext();
	bool FindPrevious();
	bool MoveUp();
	unsigned int CountRequiredEdges();
	unsigned int CountAllEdges();

	void PushEmpty();
};

/**
* Constructor.
* \param sizeInfo Tuple size info containing the space descriptor for a tuple with two integer dimensions. The space descriptor is external in order to save some memory.
*/
template<class TKeyItem, class TLeafItem>
cDomCursor<TKeyItem, TLeafItem>::cDomCursor()
	:mCache(NULL), mStack(NULL),
	mStackItem(NULL), mSizeInfo(NULL)
{
	Init();
}

template<class TKeyItem, class TLeafItem>
cDomCursor<TKeyItem, TLeafItem>::~cDomCursor()
{
	Delete();
}

template<class TKeyItem, class TLeafItem>
void cDomCursor<TKeyItem, TLeafItem>::Delete()
{
	if (mStack != NULL)
	{
		delete mStack;
		mStack = NULL;
	}
	if (mStackItem != NULL)
	{
		delete mStackItem;
		mStackItem = NULL;
	}
	if (mSizeInfo != NULL)
	{
		delete mSizeInfo->GetSpaceDescriptor();
		delete mSizeInfo;
		mSizeInfo = NULL;
	}
}

/**
* Initialize mStack
*/
template<class TKeyItem, class TLeafItem>
void cDomCursor<TKeyItem, TLeafItem>::Init()
{
	Delete();

	cTreeSpaceDescriptor* spaceDescriptor = new cTreeSpaceDescriptor(2, new cUIntType());
	mSizeInfo = new cTupleSizeInfo(spaceDescriptor);
	mStack = new cGeneralStack<cTreeTupleType>(mSizeInfo, 20);
	mStackItem = new cTreeTuple(mSizeInfo->GetSpaceDescriptor());
}

/**
* Clear the mStack of the cursor.
*/
template<class TKeyItem, class TLeafItem>
void cDomCursor<TKeyItem, TLeafItem>::Clear(unsigned int newIndex)
{
	mIndex = newIndex;
	mOrder = 0;
	mLevel = (unsigned int)-1;
	mStack->Clear();
}

/**
* Create copy of the cursor in the parameter. This cursor can be move independently to this one
* \param cursor Cursor which is copied into this one.
*/
template<class TKeyItem, class TLeafItem>
void cDomCursor<TKeyItem, TLeafItem>::operator = (const cDomCursor& cursor)
{
	mCache = (cDomCache<TKeyItem, TLeafItem>*)cursor.GetCache();
	mIndex = cursor.GetIndex();
	mOrder = cursor.GetOrder();
	mLevel = cursor.GetLevel();
	mStack->Clear();
	for (unsigned int i = 0; i < cursor.GetStack()->Count(); i++)
	{
		mStackItem->SetValue(0, cursor.GetStack()->GetRefItem(i)->GetUInt(0));
		mStackItem->SetValue(1, cursor.GetStack()->GetRefItem(i)->GetUInt(1));
		mStack->Push(*mStackItem);
	}
}

/**
* Search for a DOM node among the DOM node childs. If the key is NULL, than it search for the first DOM node.
* \param key Key of the DOM node.
* \param root Pointer from the parent DOM node.
*/
template<class TKeyItem, class TLeafItem>
bool cDomCursor<TKeyItem, TLeafItem>::FindFirst(const KeyType* key, unsigned int root)
{
	unsigned int depth = 0;
	bool founded;

	if (root == EMPTY_POINTER)
	{
		mIndex =  mCache->GetLeafNode(mIndex)->GetPointer(mOrder);
	} else
	{
		mIndex = root;
	}

	if (mIndex == EMPTY_POINTER)
	{
		mOrder = (unsigned int)-1;
		mIndex = (unsigned int)-1;
		PushToStack();
		return false;
	}

	while(true) {
		if (!IsLeaf(mIndex))
		{
			if (key != NULL)
			{
				mCache->GetInnerNode(mIndex & (~INNER_NODE_FLAG))->SearchItem(*key, (int&)mOrder);
				if (mOrder == (unsigned int)-1)
				{
					break;
				}
			} else
			{
				mOrder = 0;
			}
			PushToStack();
			depth++;
			mIndex = mCache->GetInnerNode(mIndex & (~INNER_NODE_FLAG))->GetPointer(mOrder);
			mOrder = 0;
		} else
		{
			if (key != NULL)
			{
				founded = mCache->GetLeafNode(mIndex)->SearchItem(*key, (int&)mOrder);
				if (!founded)
				{
					break;
				}
			} else
			{
				mOrder = 0;
			}
			mLevel++;
			PushToStack();
			return true;
		}
	}
	for (unsigned int i = 0; i < depth; i++)
	{
		mStack->Pop();
	}
	if (!mStack->Empty())
	{
		mIndex = mStack->TopRef()->GetUInt(0);
		mOrder = mStack->TopRef()->GetUInt(1);
	} else
	{
		printf("cDomCursor::FindFirst() - strange situation ...\n");
	}

	return false;
}

template<class TKeyItem, class TLeafItem>
bool cDomCursor<TKeyItem, TLeafItem>::FindNext()
{
	assert(!mStack->Empty());

	*mStackItem = mStack->Pop();
	
	assert(mStackItem->GetUInt(0) == mIndex);
	assert(mStackItem->GetUInt(1) == mOrder);

	mIndex = mStackItem->GetUInt(0);
	mOrder = mStackItem->GetUInt(1) + 1;

	while(true)
	{	
		if (!IsLeaf(mIndex))
		{
			//mIndex = GetIndex(mIndex);
			if ((unsigned char)mOrder >= mCache->GetInnerNode(mIndex & (~INNER_NODE_FLAG))->GetItemCount())
			{
				if (mStack->Empty() || IsLeaf(mStack->TopRef()->GetUInt(0)))
				{
					break;
				}
				*mStackItem = mStack->Pop();
				mIndex = mStackItem->GetUInt(0);
				mOrder = mStackItem->GetUInt(1) + 1;
			} else
			{
				PushToStack();
				mIndex = mCache->GetInnerNode(mIndex & (~INNER_NODE_FLAG))->GetPointer(mOrder);
				mOrder = 0;
			}
		} else
		{
			if ((unsigned char)mOrder >= mCache->GetLeafNode(mIndex)->GetItemCount())
			{
				if (mStack->Empty() || IsLeaf(mStack->TopRef()->GetUInt(0)))
				{
					break;
				}
				*mStackItem = mStack->Pop();
				mIndex = mStackItem->GetUInt(0);
				mOrder = mStackItem->GetUInt(1) + 1;
			} else
			{
				PushToStack();
				return true;
			}
		}
	}
	mIndex = (unsigned int)-1;
	mOrder = (unsigned int)-1;
	PushToStack();		
	return false;
}


/**
* Find previous sibling DOM node.
* \param leafIndex Method return the index of the founded leaf node. Parameter is set to -1 if there is no next item.
* \param order Order of the leaf item in the leaf node. Parameter is set to -1 if there is no next item.
* \return
*	- true if we find a previous DOM node,
*	- false otherwise.
*/
template<class TKeyItem, class TLeafItem>
bool cDomCursor<TKeyItem, TLeafItem>::FindPrevious()
{
	assert(!mStack->Empty());

	*mStackItem = mStack->Pop();
	
	assert(mStackItem->GetUInt(0) == mIndex);
	assert(mStackItem->GetUInt(1) == mOrder);

	mIndex = mStackItem->GetUInt(0);
	mOrder = mStackItem->GetUInt(1) - 1;


	while(true)
	{	
		if (!IsLeaf(mIndex))
		{
			//index = GetIndex(index);
			if (mOrder == (unsigned int)-1)
			{
				if (mStack->Empty() || IsLeaf(mStack->TopRef()->GetUInt(0)))
				{
					break;
				}
				*mStackItem = mStack->Pop();
				mIndex = mStackItem->GetUInt(0);
				mOrder = mStackItem->GetUInt(1) - 1;
			} else
			{
				PushToStack();
				mIndex = mCache->GetInnerNode(mIndex & (~INNER_NODE_FLAG))->GetPointer(mOrder);
				if (IsLeaf(mIndex))
				{
					mOrder = mCache->GetLeafNode(mIndex)->GetItemCount() - 1;
				} else
				{
					mOrder = mCache->GetInnerNode(mIndex & (~INNER_NODE_FLAG))->GetItemCount() - 1;
				}
			}
		} else
		{
			if (mOrder == (unsigned int)-1)
			{
				if (mStack->Empty() || IsLeaf(mStack->TopRef()->GetUInt(0)))
				{
					break;
				}
				*mStackItem = mStack->Pop();
				mIndex = mStackItem->GetUInt(0);
				mOrder = mStackItem->GetUInt(1) - 1;
			} else
			{
				PushToStack();
				return true;
			}
		}
	}
	mIndex = (unsigned int)-1;
	mOrder = (unsigned int)-1;
	PushToStack();
	return false;
}

/**
* Move the cursor in the DOM tree up. Pop the corresponding items from the mStack
*/
template<class TKeyItem, class TLeafItem>
bool cDomCursor<TKeyItem, TLeafItem>::MoveUp()
{
	do {
		mStack->Pop();
		if (mStack->Empty())
		{
			mOrder = (unsigned int)-1;
			mIndex = (unsigned int)-1;
		} else
		{
			mOrder = mStack->TopRef()->GetUInt(1);
			mIndex = mStack->TopRef()->GetUInt(0);
		}
	} while(!IsLeaf(mIndex) && !mStack->Empty());
	mLevel--;

	return !mStack->Empty();
}

/**
* Recursive method counting the XML nodes with required edges in the DOM sub-tree.
* \return
*	- Number of the XML nodes with required edge in this DOM sub-tree.
*/
template<class TKeyItem, class TLeafItem>
unsigned int cDomCursor<TKeyItem, TLeafItem>::CountRequiredEdges()
{
	unsigned int counter = 0;

	if (!IsOptional())
	{
		counter = GetNodeCount();
		if (GetPointer() != EMPTY_POINTER)
		{
			FindFirst();
			do {
				counter += CountRequiredEdges();
			} while(FindNext());
			MoveUp();
		}
	}

	return counter;
}

/**
* Recursive method counting the XML nodes in the DOM sub-tree.
* \return
*	- Number of the XML nodes in this DOM sub-tree.
*/
template<class TKeyItem, class TLeafItem>
unsigned int cDomCursor<TKeyItem, TLeafItem>::CountAllEdges()
{
	unsigned int counter = 0;

	counter = GetNodeCount();
	if (GetPointer() != EMPTY_POINTER)
	{
		FindFirst();
		do {
			counter += CountAllEdges();
		} while(FindNext());
		MoveUp();
	}

	return counter;
}

/**
* Supporting method pushing the actual cursor position into the mStack.
*/
template<class TKeyItem, class TLeafItem>
void cDomCursor<TKeyItem, TLeafItem>::PushToStack()
{
	mStackItem->SetValue(0, (unsigned int)mIndex);
	mStackItem->SetValue(1, (unsigned int)mOrder);
	mStack->Push(*mStackItem);
}

/**
* Supporting method pushing the empty cursor values into the mStack.
*/
template<class TKeyItem, class TLeafItem>
void cDomCursor<TKeyItem, TLeafItem>::PushEmpty()
{
	mIndex = (unsigned int)-1;
	mOrder = (unsigned int)-1;
	PushToStack();	
}

#endif