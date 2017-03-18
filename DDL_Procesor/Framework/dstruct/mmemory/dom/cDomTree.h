/**
*	\file cDomTree.h
*	\author Radim Baca
*	\version 0.1
*	\date dec 2008
*	\brief Represent DOM tree in a main memory.
*/


#ifndef __cDomTree_h__
#define __cDomTree_h__

#include "dstruct/mmemory/dom/cDomHeader.h"
#include "dstruct/mmemory/dom/cDomNode_Inner.h"
#include "dstruct/mmemory/dom/cDomNode_Leaf.h"
#include "dstruct/mmemory/dom/cDomStackItem.h"
#include "dstruct/mmemory/dom/cDomCache.h"
#include "dstruct/mmemory/dom/cDomCache.h"
#include "dstruct/mmemory/dom/cDomCursor.h"

#include "common/memorystructures/cSortedArrayWithLeaf.h"
#include "common/memorystructures/cGeneralStack.h"
#include "common/memorystructures/cGeneralArray.h"

/**
* Store DOM tree in a main memory. Sibling nodes in a DOM tree are sorted according to a key value. Every key value is unique
* among its siblings. This class does not support usuall DOM operations, however, the structucture is very close to DOM representation.
* Data structure is used as main memory representation of a DataGuide during the index creation.
*
* Template parameters:
*	- TKeyItem - Have to be inherited from cBasicType. Represent type of the key value.
*	- TLeafItem - Have to be inherited from cBasicType. Represent type of the leaf value.
*
* Terms used with connection of this structure:
*	- DOM tree - Whole tree.
*	- Sub-tree - One sub-tree representing one node in a DOM tree.
*	- Node - one node in a Sub-tree. Nodes are fixed-length in order to avoid extensive reallocation. Therefore, one DOM node is represented by a Sub-tree.
*
* DOM tree use two types of nodes:
*	- Inner node - Inner node of a sub-tree. It contains only key values and pointers to child nodes.
*	- Leaf node - Leaf node of a sub-tree. This node contains also leaf values (labeled path ids in the case of DataGuide).
* Level of the root node is 1.
*
*	\author Radim Baca
*	\version 0.1
*	\date dec 2008
**/
template<class TDataStructure, class TKeyItem, class TLeafItem>
class cDomTree
{
protected:
	typedef typename TKeyItem::Type KeyType;
	typedef typename TLeafItem::Type LeafType;
		
	typedef cDomCache<TKeyItem, TLeafItem> tCacheType;
	typedef cDomCursor<TKeyItem, TLeafItem> tCursor;

	cDomHeader<TKeyItem, TLeafItem>*		mHeader;
	tCacheType*								mCache;					/// Cache with inner and leaf node.
	cDomNode_Inner<TKeyItem, TLeafItem>*	mNewInnerNode;			/// New inner inner node.
	cDomNode_Leaf<TKeyItem, TLeafItem>*		mNewLeafNode;			/// New leaf leaf nodes.
	tCursor*								mThisCursor;			/// Cursor used as a main pointer into this DOM tree.
	tCursor*								mParamCursor;			/// Cursor used as a pointer into a parameter DOM tree.
	
	cGeneralStack<cTreeTupleType>*			mStack;					/// Stack used during the tree traversal. Mainly during the insert to achive the parent nodes.
	cDomStackItem*							mStackItem;				/// Stack item used as a auxiliary variable.
	cGeneralStack<cTreeTupleType>*			mStackParam;			/// Stack used during the DOM tree insertion for a traversal of a DOM tree in a parameter.
	cDomStackItem*							mStackItemParam;		/// Stack item used as a auxiliary variable.
	unsigned char							mLevel;					/// Auxiliary variable used for keeping the level we are currently during the search and insert
	cGeneralStack<TKeyItem>*				mKeyStack;				/// Used during the subtree insertion. This key has to move up and we have to remember the position in a auxiliary key.
	KeyType*								mKey;
	unsigned int							mEnd;

	bool									mDebug;
	bool									mCheckConsistency;


	// support methods
	int AfterInsertOfNewItem(int& state, int& action, unsigned int& root, unsigned int& newNodeIndex, KeyType** newNodeKey, unsigned int actualNode, unsigned int height);
	unsigned int CreateNewTree(unsigned char level, bool newTreeIsLeaf);
	void CreateNewRoot(unsigned int firstNodeIndex, const KeyType& firstNodeKey, unsigned int secondNodeIndex, const KeyType& secondNodeKey);
	void SetNewPointer(unsigned int root, const KeyType* key, unsigned int newPointer);
	//void SetNewDescendantCount(unsigned int root, KeyType* key, unsigned short countIncrease);
	void PushToStack(cGeneralStack<cTreeTupleType>* stack, unsigned int index, unsigned int rightIndex, unsigned char level, unsigned int order);
	bool IsLeaf(unsigned int index);
	unsigned int GetIndex(unsigned int index);
	

	unsigned int FindLabeledPath(cArray<unsigned int>* pointerArray, cGeneralArray<cUIntType>* keyArray, unsigned int leaf);

	// insert methods
	void InsertDomR(bool moveNext, tCursor* thisCursor, tCursor* paramCursor, cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf);
	bool ResolveDifferentKeys(tCursor* thisCursor, tCursor* paramCursor, cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf);
	virtual void MoveUp(tCursor* thisCursor, tCursor* paramCursor, cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf);
	void MoveNext(tCursor* thisCursor, tCursor* paramCursor, cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf);

	void Insert(KeyType *keySet, LeafType& leaf, unsigned int size);
	int InsertIntoTree(LeafType& lpNumber, unsigned int& nodeIndex, unsigned int& root, const KeyType* key, unsigned short remainingLength, bool newTreeIsLeaf, bool isOptional, unsigned int nodeCount);
	virtual void InsertAndCopyTree(tCursor* thisCursor, tCursor* paramCursor, cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf, bool isOptional);
	void CopyTree(unsigned int rootParam, unsigned int rootThis, cDomCache<TKeyItem,TLeafItem>* cacheParam, cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf, bool isOptional);
	virtual void CopyLeafItems(unsigned int indexParam, cDomCache<TKeyItem,TLeafItem>* cacheParam, cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf);
	virtual void SetOptional(tCursor* cursor);

	void PrintTreeR(tCursor* cursor, unsigned int level);

	static const int KEY_FOUNDED = 1;
	static const int KEY_INSERTED = 2;
	static const int NEW_ROOT = 3;

	static const unsigned int DEFAULT_START_NODE_COUNT = 2000;		/// Initial number of leaf nodes.

	/// This constants are duplicated in cDomCursor!!
	static const unsigned int EMPTY_POINTER = 0xffffffff;
	static const unsigned int INNER_NODE_FLAG = 0x80000000;

	static const int GO_DOWN = 1;
	static const int GO_UP = 2;

public:
	// these constants are duplicated in a cDomNode_Inner!!! In the case of any update, check also these constants.
	static const int INSERT_OK = 0;
	static const int INSERT_OVERFULL = 1;
	static const int INSERT_FIRSTCHANGED = 2;

	cDomTree(cDomHeader<TKeyItem, TLeafItem>* header, unsigned int startNodeCount = DEFAULT_START_NODE_COUNT);
	~cDomTree();

	void Init(unsigned int startNodeCount);
	void Delete();
	void Clear();

	// DOM tree traversal methods. Interface used only by other DOM.
	cDomCache<TKeyItem, TLeafItem>* GetCache()	{ return mCache; }
	unsigned int GetRootIndex()					{ return mHeader->GetRootIndex(); }


	/**
	* Insert the set of items into the DOM tree
	* \param keySet Root-to-leaf set of items, which are searched in the tree. If the path does not exist then the set is inserted.
	* \param size Length of the set
	*/
	LeafType Insert(KeyType *keySet, unsigned int size)	{ LeafType leaf; Insert(keySet, leaf, size); return leaf; }
	void InsertDom(cDomTree* dom, const KeyType& firstLevel, cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedArray);
	void ProcessTree(TDataStructure* dataStructure, cArray<unsigned int>* pointerArray, cGeneralArray<cUIntType>* keyArray);

	inline unsigned int GetLeafCount() const	{ return mHeader->GetTopLeaf(); }

	void SetDebug(bool debug)					{ mDebug = debug; }
	void SetCheckConsistency(bool check)		{ mCheckConsistency = check; }
	void PrintTree();
	void PrintTree(tCursor* cursor);
	void PrintNodes();
};

/**
* Constructor
* \param header Header of the cDomTree.
* \param startNodeCount Initial number of a main memory array storing the nodes.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
cDomTree<TDataStructure, TKeyItem, TLeafItem>
	::cDomTree(cDomHeader<TKeyItem, TLeafItem>* header, unsigned int startNodeCount)
	:mHeader(header), mCache(NULL),
	mStack(NULL), mStackItem(NULL),
	mStackParam(NULL), mStackItemParam(NULL),
	mKey(NULL), mKeyStack(NULL),
	mThisCursor(NULL), mParamCursor(NULL)
{
	Init(startNodeCount);
}

/**
* Destructor
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
cDomTree<TDataStructure, TKeyItem, TLeafItem>::~cDomTree()
{
	Delete();
}

/** 
* Delete all variables allocated on the heap
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cDomTree<TDataStructure, TKeyItem, TLeafItem>::Delete()
{
	if (mStack != NULL)
	{
		delete ((cTupleSizeInfo*)(mStack->GetSizeInfo()))->GetSpaceDescriptor();
		delete mStack->GetSizeInfo();
		delete mStack;
		mStack = NULL;
	}
	if (mStackItem != NULL)
	{
		delete mStackItem;
		mStackItem = NULL;
	}
	if (mStackParam != NULL)
	{
		delete mStackParam->GetSizeInfo();
		delete mStackParam;
		mStackParam = NULL;
	}
	if (mStackItemParam != NULL)
	{
		delete mStackItemParam;
		mStackItemParam = NULL;
	}
	if (mCache != NULL)
	{
		delete mCache;
		mCache = NULL;
	}
	if (mKeyStack != NULL)
	{
		delete mKeyStack;
		mKeyStack = NULL;
	}
	if (mKey != NULL)
	{
		delete mKey;
		mKey = NULL;
	}
	if (mThisCursor != NULL)
	{	
		delete mThisCursor;
		mThisCursor = NULL;
	}
	if (mParamCursor != NULL)
	{
		delete mParamCursor;
		mParamCursor = NULL;
	}
}

/**
* First free all allocated memory and then allocate new
* \param startNodeCount Initial number of a main memory array storing the nodes
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cDomTree<TDataStructure, TKeyItem, TLeafItem>
	::Init(unsigned int startNodeCount)
{
	Delete();

	mCache = new cDomCache<TKeyItem, TLeafItem>(mHeader, startNodeCount);
	mHeader->SetInnerArraySize(startNodeCount / 4);
	mHeader->SetLeafArraySize(startNodeCount);

	mCache->CreateLeafNode(1);
	mHeader->SetRootIndex(mHeader->GetTopLeafNodeIndex());
	mNewLeafNode = mCache->CreateLeafNode();
	mNewInnerNode = mCache->CreateInnerNode();

	cTreeSpaceDescriptor* stackItemSpaceDescriptor = new cTreeSpaceDescriptor(4, new cUIntType());
	stackItemSpaceDescriptor->SetType(0, new cCharType());
	mStack = new cGeneralStack<cTreeTupleType>(new cTupleSizeInfo(stackItemSpaceDescriptor));
	mStackItem = new cDomStackItem(stackItemSpaceDescriptor);
	mStackParam = new cGeneralStack<cTreeTupleType>(new cTupleSizeInfo(stackItemSpaceDescriptor));
	mStackItemParam = new cDomStackItem(stackItemSpaceDescriptor);
	mParamCursor = new tCursor();
	mThisCursor = new tCursor();
	mThisCursor->SetDOMCache(mCache);

	mKeyStack = new cGeneralStack<TKeyItem>(&(cSizeInfo<KeyType>&)mHeader->GetKeySizeInfo(), 10);
	mKey = new KeyType();
	TKeyItem::Resize((cSizeInfo<KeyType>&)mHeader->GetKeySizeInfo(), *mKey);

	mDebug = false;
	mCheckConsistency = false;
}

/**
* Set all variables in order to have an empty Dom tree.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cDomTree<TDataStructure, TKeyItem, TLeafItem>
	::Clear()
{
	mHeader->Clear();

	mCache->CreateLeafNode(1);
	mHeader->SetRootIndex(mHeader->GetTopLeafNodeIndex());
	mNewLeafNode = mCache->CreateLeafNode();
	mNewInnerNode = mCache->CreateInnerNode();
	mStack->Clear();
}


//*****************************************************************************************************
//****************************************   DOM Insert    ********************************************
//*****************************************************************************************************

/**
* Insert the DOM tree into this DOM tree. Root of the DOM tree in the parameter is inserted into the second level
* of this DOM tree.
* \param dom The DOM tree, which is inserted into this DOM tree.
* \param firstLevel First level key, where the DOM tree is inserted.
* \param sortedLeaf Map the leaf numbers of the inserted DOM tree into leaf numbers of this DOM tree. Created during insertion. Keys are leaf values of the inserted DOM tree.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cDomTree<TDataStructure, TKeyItem, TLeafItem>::InsertDom(
	cDomTree* dom, 
	const KeyType& firstLevel, 
	cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf)
{
	assert(mHeader->GetRootIndex() != EMPTY_POINTER);
	assert(dom->GetRootIndex() != EMPTY_POINTER);

	mParamCursor->SetDOMCache(dom->GetCache());
	mParamCursor->Clear(dom->GetRootIndex());
	mThisCursor->Clear(mHeader->GetRootIndex());
	mStack->Clear();

	if (!mThisCursor->FindFirst(&firstLevel, mHeader->GetRootIndex()))
	{
		printf("cDomTree::InsertDom() - error, key on a first level of DOM not found!\n");
		return;
	}

	if (mThisCursor->GetPointer() != (unsigned int)-1)
	{
		// Case when the root DOM node of this DOM tree has at least one child
		mParamCursor->FindFirst(NULL, dom->GetRootIndex());
		if (mThisCursor->FindFirst(&mParamCursor->GetKey()))
		{			
			mCache->GetLeafNode(mThisCursor->GetIndex())->IncNodeCount(mThisCursor->GetOrder(), mParamCursor->GetNodeCount());
			sortedLeaf->Insert(mParamCursor->GetLeaf(),	mThisCursor->GetLeaf());
			InsertDomR(false, mThisCursor, mParamCursor, sortedLeaf);
		} else
		{
			InsertAndCopyTree(mThisCursor, mParamCursor, sortedLeaf, false);
		}
	} else
	{
		// The root DOM node of this DOM tree does not have any childs.
		unsigned int newIndex = CreateNewTree(mCache->GetLeafNode(mThisCursor->GetIndex())->GetLevel() + 1, IsLeaf(dom->GetRootIndex()));
		if ((newIndex & INNER_NODE_FLAG) == 0)
		{
			mHeader->DecrementTopLeafNodeIndex();
			mNewLeafNode = mCache->GetLeafNode(newIndex);
		} else
		{
			mHeader->DecrementTopInnerNodeIndex();
			mNewInnerNode = mCache->GetInnerNode(GetIndex(newIndex));
		}
		mCache->GetLeafNode(mThisCursor->GetIndex())->SetPointer(mThisCursor->GetIndex(), newIndex);	
		CopyTree(dom->GetRootIndex(), newIndex, dom->GetCache(), sortedLeaf, false);
	}
}


/**
* Recursive method merging two DOM trees.
* \param moveNext Move to the next DOM node after the DOM sub-trees are merged.
* \param thisCursor Cursor of this DOM tree.
* \param paramCursor Cursor of parameter DOM tree.
* \param sortedLeaf Map the leaf numbers of the inserted DOM tree into leaf numbers of this DOM tree. Created during insertion. Keys are leaf values of the inserted DOM tree.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cDomTree<TDataStructure, TKeyItem, TLeafItem>::InsertDomR(
	bool moveNext,
	tCursor* thisCursor, 
	tCursor* paramCursor, 
	cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf)
{
	thisCursor->FindFirst();
	paramCursor->FindFirst();

	// resolve all siblings in this DOM node
	while (thisCursor->GetIndex() != (unsigned int)-1 && paramCursor->GetIndex() != (unsigned int)-1)
	{
		if (ResolveDifferentKeys(thisCursor, paramCursor, sortedLeaf))
		{
			mCache->GetLeafNode(thisCursor->GetIndex())->IncNodeCount(thisCursor->GetOrder(), paramCursor->GetNodeCount());
			sortedLeaf->Insert(paramCursor->GetLeaf(), thisCursor->GetLeaf());
			if (paramCursor->IsOptional() && !thisCursor->IsOptional())
			{
				SetOptional(thisCursor);
			}
			InsertDomR(true, thisCursor, paramCursor, sortedLeaf);
		}
	}

	// move both cursors up
	MoveUp(thisCursor, paramCursor, sortedLeaf);

	// move both cursors next if necessary
	if (moveNext)
	{
		MoveNext(thisCursor, paramCursor, sortedLeaf);
	}
}

/**
* Move both cursors until they have different key values. Method resolve merging of the DOM sub-trees.
* \param thisCursor Cursor into this DOM tree.
* \param paramCursor Cursor into parameter DOM tree.
* \param sortedLeaf Map the leaf numbers of the inserted DOM tree into leaf numbers of this DOM tree. Created during insertion. Keys are leaf values of the inserted DOM tree.
* \return
*	- true in the case that cursors point to the DOM nodes with the same key
*	- false in the case that at least one cursor reached the last DOM sibling on this DOM level.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
bool cDomTree<TDataStructure, TKeyItem, TLeafItem>::ResolveDifferentKeys(
	tCursor* thisCursor, 
	tCursor* paramCursor, 
	cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf)
{
	while(thisCursor->GetKey() != paramCursor->GetKey())
	{
		if (thisCursor->GetKey() < paramCursor->GetKey())
		{
			// mark DOM subtree of this DOM as an optional
			SetOptional(thisCursor);

			if (!thisCursor->FindNext())
			{
				thisCursor->MoveUp();
				// copy remaining branches from parameter DOM into this DOM
				do
				{
					InsertAndCopyTree(thisCursor, paramCursor, sortedLeaf, true);
				} while(paramCursor->FindNext());

				thisCursor->PushEmpty();
				return false;
			}
		} else
		{
			// copy the parameter DOM subtree into this DOM
			mKeyStack->Push(thisCursor->GetKey());
			thisCursor->MoveUp();
			InsertAndCopyTree(thisCursor, paramCursor, sortedLeaf, true);
			if (!thisCursor->FindFirst(&mKeyStack->Top()))
			{
				printf("cDomTree::ResolveDifferentKeys() - key not found in this DOM tree!\n");
				PrintTree(thisCursor);
				printf("Key: ");
				TKeyItem::Print("\n", mKeyStack->Top());
				exit(1);
			}
			mKeyStack->Pop();

			if (!paramCursor->FindNext())
			{
				// mark rest of the branches in this DOM tree as an optional
				do
				{
					SetOptional(thisCursor);
				} while(thisCursor->FindNext());
				return false;
			}
		}
	}
	return true;
}

/**
* Move both cursors up.
* \param thisCursor Cursor into this DOM tree.
* \param paramCursor Cursor into parameter DOM tree.
* \param sortedLeaf Map the leaf numbers of the inserted DOM tree into leaf numbers of this DOM tree. Created during insertion. Keys are leaf values of the inserted DOM tree.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cDomTree<TDataStructure, TKeyItem, TLeafItem>::MoveUp(
	tCursor* thisCursor, 
	tCursor* paramCursor, 
	cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf)
{
	if (thisCursor->GetIndex() != (unsigned int)-1)
	{
		SetOptional(thisCursor);
	} 
	thisCursor->MoveUp();

	if (paramCursor->GetIndex() != (unsigned int)-1)
	{
		paramCursor->MoveUp();

		// copy param DOM subtree into this DOM
		unsigned int newIndex = CreateNewTree(mCache->GetLeafNode(thisCursor->GetIndex())->GetLevel() + 1, IsLeaf(paramCursor->GetPointer()));
		if ((newIndex & INNER_NODE_FLAG) == 0)
		{
			mHeader->DecrementTopLeafNodeIndex();
			mNewLeafNode = mCache->GetLeafNode(newIndex);
		} else
		{
			mHeader->DecrementTopInnerNodeIndex();
			mNewInnerNode = mCache->GetInnerNode(GetIndex(newIndex));
		}
		mCache->GetLeafNode(thisCursor->GetIndex())->SetPointer(thisCursor->GetOrder(), newIndex);
		CopyTree(paramCursor->GetPointer(), newIndex, (cDomCache<TKeyItem, TLeafItem>*)paramCursor->GetCache(), sortedLeaf, true);
	} else
	{
		paramCursor->MoveUp();
	}
}

/**
* Move both cursors and resolve remaining branches
* \param thisCursor Cursor into this DOM tree.
* \param paramCursor Cursor into parameter DOM tree.
* \param sortedLeaf Map the leaf numbers of the inserted DOM tree into leaf numbers of this DOM tree. Created during insertion. Keys are leaf values of the inserted DOM tree.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cDomTree<TDataStructure, TKeyItem, TLeafItem>::MoveNext(
	tCursor* thisCursor, 
	tCursor* paramCursor, 
	cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf)
{
	// move next
	if (thisCursor->GetIndex() != (unsigned int)-1)
	{
		if (!thisCursor->FindNext())
		{	
			// copy rest of the branches from parameter DOM into this DOM
			thisCursor->MoveUp();
			while(paramCursor->FindNext())
			{
				InsertAndCopyTree(thisCursor, paramCursor, sortedLeaf, true);
			}
			thisCursor->PushEmpty();
		}
	}
	if (paramCursor->GetIndex() != (unsigned int)-1)
	{
		if (!paramCursor->FindNext())
		{
			// mark rest of the branches in this DOM tree as an optional
			do
			{
				SetOptional(thisCursor);
			} 
			while(thisCursor->FindNext());
		}
	} 
}

/**
* Insert the root node of the parameter DOM and then copy the rest of the parameter DOM into this DOM.
* \param thisCursor Cursor into this DOM tree.
* \param paramCursor Cursor into parameter DOM tree.
* \param sortedLeaf Map the leaf numbers of the inserted DOM tree into leaf numbers of this DOM tree. Created during insertion. Keys are leaf values of the inserted DOM tree.
* \param isOptional If true all copied DOM nodes are marked as a 'not required'. Otherwise the required status is not changed.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cDomTree<TDataStructure, TKeyItem, TLeafItem>::InsertAndCopyTree(
		tCursor* thisCursor, 
		tCursor* paramCursor, 
		cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf,
		bool isOptional)
{
	unsigned int index = thisCursor->GetIndex();
	unsigned int order = thisCursor->GetOrder();
	unsigned int indexParam = paramCursor->GetIndex();
	unsigned int orderParam = paramCursor->GetOrder();
	unsigned int root = mCache->GetLeafNode(index)->GetPointer(order);
	unsigned int newLeafValue, nodeIndex;
	cDomCache<TKeyItem, TLeafItem>* cacheParam = (cDomCache<TKeyItem, TLeafItem>*)paramCursor->GetCache();
	KeyType* key = (KeyType*)&mCache->GetLeafNode(index)->GetKey(order);
	unsigned int remainingLength;

	remainingLength = cacheParam->GetLeafNode(indexParam)->GetPointer(orderParam) == EMPTY_POINTER ? 0 : 1;

	mStack->Clear();
	// insert root node of the parameter DOM
	if (InsertIntoTree(newLeafValue, nodeIndex, root, &cacheParam->GetLeafNode(indexParam)->GetKey(orderParam), remainingLength, 
		(cacheParam->GetLeafNode(indexParam)->GetPointer(orderParam) & INNER_NODE_FLAG) == 0, isOptional,
		cacheParam->GetLeafNode(indexParam)->GetNodeCount(orderParam)) == NEW_ROOT)
	{
		SetNewPointer(index, key, root);
	}
	sortedLeaf->Insert(cacheParam->GetLeafNode(indexParam)->GetLeaf(orderParam), newLeafValue);
	if (cacheParam->GetLeafNode(indexParam)->GetPointer(orderParam) != EMPTY_POINTER)
	{
		if ((nodeIndex & INNER_NODE_FLAG) == 0)
		{
			mHeader->DecrementTopLeafNodeIndex();
			mNewLeafNode = mCache->GetLeafNode(nodeIndex);
		} else
		{
			mHeader->DecrementTopInnerNodeIndex();
			mNewInnerNode = mCache->GetInnerNode(GetIndex(nodeIndex));
		}

		// insert the rest of the tree
		CopyTree(cacheParam->GetLeafNode(indexParam)->GetPointer(orderParam), nodeIndex, cacheParam, sortedLeaf, isOptional);

	}
}

/**
* Copy the paramater DOM into this DOM.
* \param rootParam Root node index of the DOM tree, which should be copied into this DOM tree.
* \param rootThis Index of the empty node, where the DOM tree will be copied.
* \param cacheParam Cache of the DOM tree, which should be copied into this DOM tree.
* \param sortedLeaf Map the leaf numbers of the inserted DOM tree into leaf numbers of this DOM tree. Created during insertion. Keys are leaf values of the inserted DOM tree.
* \param isOptional If true all copied DOM nodes are marked as a 'not required'. Otherwise the required status is not changed.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cDomTree<TDataStructure, TKeyItem, TLeafItem>::CopyTree(
		unsigned int rootParam, 
		unsigned int rootThis, 
		cDomCache<TKeyItem,TLeafItem>* cacheParam, 
		cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf,
		bool isOptional)
{
	unsigned int indexParam = rootParam;
	unsigned int order = 0;
	unsigned int newIndex = (unsigned int)-1, newLeafIndex = mHeader->GetTopLeafNodeIndex(), newInnerIndex = mHeader->GetTopInnerNodeIndex() | INNER_NODE_FLAG;
	bool newNode = true;
	KeyType* key = NULL;

	if ((rootThis & INNER_NODE_FLAG) == 0)
	{
		newLeafIndex = rootThis;
	} else
	{
		newInnerIndex = rootThis;
	}

	mStackParam->Clear();
	mStack->Clear();
	while(true)
	{
		//if (mDebug)
		//{
		//	PrintNodes();
		//	printf("**********************");
		//}		
		if (newNode)
		{
			// copy node
			if (IsLeaf(indexParam))
			{
				mNewLeafNode->CopyItems(cacheParam->GetLeafNode(indexParam), isOptional);
				mNewLeafNode->SetItemCount(cacheParam->GetLeafNode(indexParam)->GetItemCount());
				mNewLeafNode->SetLevel(cacheParam->GetLeafNode(indexParam)->GetLevel() + 1);
				CopyLeafItems(indexParam, cacheParam, sortedLeaf);
				newIndex = newLeafIndex;
				mNewLeafNode = mCache->CreateLeafNode();
				newLeafIndex = mHeader->GetTopLeafNodeIndex();
			} else
			{
				mNewInnerNode->CopyItems(0, 0, cacheParam->GetInnerNode(GetIndex(indexParam)), cacheParam->GetInnerNode(GetIndex(indexParam))->GetItemCount());
				mNewInnerNode->SetItemCount(cacheParam->GetInnerNode(GetIndex(indexParam))->GetItemCount());
				mNewInnerNode->SetLevel(cacheParam->GetInnerNode(GetIndex(indexParam))->GetLevel() + 1);
				newIndex = newInnerIndex;
				mNewInnerNode = mCache->CreateInnerNode();
				newInnerIndex = mHeader->GetTopInnerNodeIndex() | INNER_NODE_FLAG;
			}

			// set parent pointer
			if (!mStackParam->Empty())
			{
				unsigned int ord = ((cDomStackItem*)mStack->TopRef())->GetNodeOrder();
				if (IsLeaf(((cDomStackItem*)mStack->TopRef())->GetIndex()))
				{
					mCache->GetLeafNode(((cDomStackItem*)mStack->TopRef())->GetIndex())->SetPointer(ord, newIndex);
				} else
				{
					mCache->GetInnerNode(((cDomStackItem*)mStack->TopRef())->GetIndex() & (~INNER_NODE_FLAG))->SetPointer(ord, newIndex);
				}
			}

			newNode = false;
		}

		// find next node
		if (!IsLeaf(indexParam))
		{
			indexParam = GetIndex(indexParam);
			if ((unsigned char)order >= cacheParam->GetInnerNode(indexParam)->GetItemCount())
			{
				if (mStackParam->Empty())
				{
					break;
				}
				*mStackItem = mStackParam->Pop();
				indexParam = mStackItem->GetIndex();
				order = mStackItem->GetNodeOrder() + 1;
				*mStackItem = mStack->Pop();
				newIndex = mStackItem->GetIndex();
			} else
			{
				PushToStack(mStack, newIndex | INNER_NODE_FLAG, EMPTY_POINTER, 0, order);
				PushToStack(mStackParam, indexParam | INNER_NODE_FLAG, EMPTY_POINTER, 0, order);
				indexParam = cacheParam->GetInnerNode(indexParam)->GetPointer(order);
				order = 0;
				newNode = true;
			}
		} else
		{
			if ((unsigned char)order >= cacheParam->GetLeafNode(indexParam)->GetItemCount())
			{
				if (mStackParam->Empty())
				{
					break;
				}
				*mStackItem = mStackParam->Pop();
				indexParam = mStackItem->GetIndex();
				order = mStackItem->GetNodeOrder() + 1;
				*mStackItem = mStack->Pop();
				newIndex = mStackItem->GetIndex();
			} else
			{
				if (cacheParam->GetLeafNode(indexParam)->GetPointer(order) != EMPTY_POINTER)
				{
					PushToStack(mStack, newIndex, EMPTY_POINTER, 0, order);
					PushToStack(mStackParam, indexParam, EMPTY_POINTER, 0, order);
					indexParam = cacheParam->GetLeafNode(indexParam)->GetPointer(order);
					order = 0;
					newNode = true;
				} else
				{
					order++;
				}
			}
		}
	}
}

/**
* Copy leaf items of the leaf node.
* \param cacheParam Cache of the DOM tree, which should be copied into this DOM tree.
* \param sortedLeaf Map the leaf numbers of the inserted DOM tree into leaf numbers of this DOM tree. Created during insertion. Keys are leaf values of the inserted DOM tree.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cDomTree<TDataStructure, TKeyItem, TLeafItem>::CopyLeafItems(
	unsigned int indexParam,
	cDomCache<TKeyItem,TLeafItem>* cacheParam, 
	cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf)
{
	for (unsigned int i = 0; i < mNewLeafNode->GetItemCount(); i++)
	{
		LeafType newLeaf;
		unsigned int aux;
		TLeafItem::Copy(newLeaf, mHeader->GetNextLeaf());
		sortedLeaf->Insert(cacheParam->GetLeafNode(indexParam)->GetLeaf(i), newLeaf, aux);
		mNewLeafNode->SetLeaf(i, newLeaf);
	}
}

/**
* Set whole DOM subtree as a optional.
* \param cursor Cursor which represent position in the DOM tree.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cDomTree<TDataStructure, TKeyItem, TLeafItem>
	::SetOptional(tCursor* cursor)
{
	if (!cursor->IsOptional())
	{
		cursor->SetOptional();
		if (cursor->GetPointer() != EMPTY_POINTER)
		{
			cursor->FindFirst();
			do {
				SetOptional(cursor);
			} while(cursor->FindNext());
			cursor->MoveUp();
		}
	}
}


//*****************************************************************************************************
//****************************************  Normal Insert  ********************************************
//*****************************************************************************************************


/**
* Insert the set of items into the DOM tree
* \param keySet Root-to-leaf set of items, which are searched in the tree. If the path does not exist then the set is inserted.
* \param leaf Return parameter returning a leaf number generated (in the case of new Root-to-leaf path) or founded in a DOM tree.
* \param size Length of the set
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cDomTree<TDataStructure, TKeyItem, TLeafItem>
	::Insert(KeyType *keySet, LeafType& leaf, unsigned int size)
{
	KeyType* key;
	int ret;
	unsigned int nodeCount, firstInsert = 0, index, root = mHeader->GetRootIndex();
	bool firstKeyInserted = true;

	mLevel = 1;
	mStack->Clear();

	for (unsigned int i = 0; i < size; i++)
	{
		key = &keySet[i];
		if (size - 1 == i)
		{
			nodeCount = 1;
		} else
		{
			nodeCount = 0;
		}
		if ((ret = InsertIntoTree(leaf, index, root, key, size - 1 - i, true, false, nodeCount)) == NEW_ROOT)
		{
			if (i > 0)
			{
				SetNewPointer(((cDomStackItem*)(mStack->TopRef()))->GetIndex(), &keySet[i - 1], root);
			} else
			{
				mHeader->SetRootIndex(root);
			}
			PushToStack(mStack, root, EMPTY_POINTER, mLevel, EMPTY_POINTER);
		}
		// in the case that we have inserted first new key into the tree
		if (firstKeyInserted && ret > 1)
		{
			firstKeyInserted = false;
			firstInsert = i;
		}
		mLevel++;
		root = index;
	}

	//// we change descendant counts in all DOM nodes from root node to 'firstInsert' node
	//for (unsigned int i = 0; i < firstInsert; i++)
	//{
	//	SetNewDescendantCount(((cDomStackItem*)(&mStack->GetItem(i)))->GetIndex(), &keySet[i], size - firstInsert);
	//}
}

/**
* Search for a key in a sub-tree. If the key is not founded, than it is inserted.
* \param lpNumber Parameter by reference. Number of the labelled path ending by this key.
* \param nodeIndex Parameter by reference. Index of the child node.
* \param root Index of this tree root node. If the root node changes the new node index of the root is returned in this parameter.
* \param key The key searched in the tree.
* \param remainingLength Length of the remaining keySet.
* \param newTreeIsLeaf If function create new tree the empty leaf node is created (if this variable is true).
* \param nodeCount Number of XML nodes corresponding to this insert.
* \return One of the following constants are returned
*		- KEY_FOUNDED - key was founded.
*		- KEY_INSERTED - key was inserted, but it doesn't lead to the new root creation. Labelled path id is in the mHeader (top - 1).
*		- NEW_ROOT - key was inserted and it lead to new root node creation. The root index is stored in the root parameter.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
int cDomTree<TDataStructure, TKeyItem, TLeafItem>
	::InsertIntoTree(LeafType& lpNumber, 
		unsigned int& nodeIndex, 
		unsigned int& root, 
		const KeyType* key, 
		unsigned short remainingLength, 
		bool newTreeIsLeaf, 
		bool isOptional,
		unsigned int nodeCount)
{
	unsigned int newNodeIndex = EMPTY_POINTER, rightNode = EMPTY_POINTER, actualNode = root;
	KeyType* newNodeKey = NULL;
	int ret, state = GO_DOWN, action = 0;
	unsigned int height = (unsigned int)-1;
	bool founded;
	int order = 0;
	bool leaf;
	bool lastKey = remainingLength == 0; // This flag indicate, that the key is the last in the keySet.

	PushToStack(mStack,actualNode, rightNode, mLevel, EMPTY_POINTER);
	while(true) {
		leaf = IsLeaf(actualNode);
		actualNode = GetIndex(actualNode);
		if (state == GO_DOWN)
		{
			height++;
			if (leaf)
			{
				founded = mCache->GetLeafNode(actualNode)->SearchItem(*key, order);
				if (mDebug)
				{
					mCache->GetLeafNode(actualNode)->Print("\n", actualNode);
				}
			} else
			{
				founded = mCache->GetInnerNode(actualNode)->SearchItem(*key, order);
				if (mDebug)
				{
					mCache->GetInnerNode(actualNode)->Print("\n", actualNode);
				}
			}

			if (leaf)
			{

				if (founded)
				{
					// Key founded in the tree
					nodeIndex = mCache->GetLeafNode(actualNode)->GetPointer(order);
					if (!lastKey && nodeIndex == EMPTY_POINTER)
					{
						nodeIndex = CreateNewTree(mLevel + 1, newTreeIsLeaf);
					}
					lpNumber = mCache->GetLeafNode(actualNode)->GetLeaf(order);
					mCache->GetLeafNode(actualNode)->SetPointer(order, nodeIndex);
					//mCache->GetLeafNode(actualNode)->IncDescendantCount(order, nodeCount);
					mStack->Pop(height);
					return KEY_FOUNDED;
				} else
				{
					// Insert new item into the leaf node
					lpNumber = mHeader->GetNextLeaf();
					nodeIndex = EMPTY_POINTER;
					if (!lastKey)
					{
						nodeIndex = CreateNewTree(mLevel + 1, newTreeIsLeaf);
					}
					action = mCache->GetLeafNode(actualNode)->InsertLeafItem(order + 1, lpNumber, *key, nodeIndex, isOptional, nodeCount);
					ret = AfterInsertOfNewItem(state, action, root, newNodeIndex, &newNodeKey, actualNode, height);
					if (ret != 0)
					{
						return ret;
					}
				}
			} else
			{
				// go down deep to the tree
				if (order != -1)
				{
					rightNode = (order == (mCache->GetInnerNode(actualNode)->GetItemCount() - 1)) ? EMPTY_POINTER : mCache->GetInnerNode(actualNode)->GetPointer(order + 1);
					actualNode = mCache->GetInnerNode(actualNode)->GetPointer(order);
					PushToStack(mStack,actualNode, rightNode, mLevel, order);
				} else
				{
					rightNode = (mCache->GetInnerNode(actualNode)->GetItemCount() == 1) ? EMPTY_POINTER : mCache->GetInnerNode(actualNode)->GetPointer(1);
					actualNode = mCache->GetInnerNode(actualNode)->GetPointer(0);
					PushToStack(mStack,actualNode, rightNode, mLevel, 0);
				}
			}
		}
		if (state == GO_UP)
		{
			// up phase of the traversal in the case of node split or key change
			unsigned int leftNodeIndex = EMPTY_POINTER, rightNodeIndex = EMPTY_POINTER;
			unsigned int auxActualNode = (unsigned int)-1;

			height--;
			//if (mStack->Empty() || mLevel != ((cDomStackItem*)(mStack->TopRef()))->GetLevel())
			if (mStack->Empty() || height == (unsigned int)-1)
			{
				return KEY_INSERTED;
			}
			if (action == INSERT_OVERFULL || action == INSERT_FIRSTCHANGED)
			{
				leftNodeIndex = mStackItem->GetIndex();
				rightNodeIndex = mStackItem->GetRightIndex();
				order = mStackItem->GetNodeOrder();
				actualNode = ((cDomStackItem*)(mStack->TopRef()))->GetIndex();
				if (IsLeaf(actualNode))
				{
					printf("cDomTree::InsertIntoTree - error during the inner node split!\n");
				} else
				{
					auxActualNode = GetIndex(actualNode);
					if (IsLeaf(leftNodeIndex))
					{
						mCache->GetInnerNode(auxActualNode)->SetKey(order, mCache->GetLeafNode(leftNodeIndex)->GetKey(0));
						if (rightNodeIndex != EMPTY_POINTER)
						{
							mCache->GetInnerNode(auxActualNode)->SetKey(order + 1, mCache->GetLeafNode(rightNodeIndex)->GetKey(0));
						}
					} else
					{
						mCache->GetInnerNode(auxActualNode)->SetKey(order, mCache->GetInnerNode(GetIndex(leftNodeIndex))->GetKey(0));
						if (rightNodeIndex != EMPTY_POINTER)
						{
							mCache->GetInnerNode(auxActualNode)->SetKey(order + 1, mCache->GetInnerNode(GetIndex(rightNodeIndex))->GetKey(0));
						}
					}
				}
				if (action == INSERT_FIRSTCHANGED)
				{
					if (order != 0)
					{
						mStack->Pop(height);
						return KEY_INSERTED;
					} else
					{
						*mStackItem = mStack->Pop();
					}
				}
			}
			if (action == INSERT_OVERFULL)
			{				

				unsigned int insertPosition = 1;
				if (rightNodeIndex != EMPTY_POINTER)
				{
					insertPosition = 2;
				}
				if (IsLeaf(actualNode))
				{
					action = mCache->GetLeafNode(actualNode)->InsertPointerItem(order + insertPosition, *newNodeKey, newNodeIndex);
				} else
				{
					action = mCache->GetInnerNode(auxActualNode)->InsertPointerItem(order + insertPosition, *newNodeKey, newNodeIndex);
				}

				ret = AfterInsertOfNewItem(state, action, root, newNodeIndex, &newNodeKey, actualNode, height);
				if (ret != 0)
				{
					return ret;
				}
			}
		}
	} 

	return 0;
}

/**
* Process the split after the insertion of a new item into any node. It can also change the state or change the action.
* \param state Direction of the tree traversal.
* \param action Action parameter returned after the item insertion.
* \param newNodeIndex Index of a new node created in this method
* \param newNodeKey Key of a new node created in this method
* \param root Index of the new root node (if the new root of the sub-tree has been created).
* \param actualNode Index of the actual node. Can be inner or leaf node index.
* \param height Height of the sub-tree
* \return
*		- NEW_ROOT if the new root node was created
*		- KEY_INSERTED if the key was successfuly inserted
*		- 0 otherwise
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
int cDomTree<TDataStructure, TKeyItem, TLeafItem>
	::AfterInsertOfNewItem(int& state, int& action, unsigned int& root, unsigned int& newNodeIndex, KeyType** newNodeKey, 
		unsigned int actualNode, unsigned int height)
{
	unsigned int index = ((cDomStackItem*)(mStack->TopRef()))->GetRightIndex();
	if (action != INSERT_OK)
	{
		*mStackItem = mStack->Pop();
		state = GO_UP;
	} else
	{
		mStack->Pop(height + 1);
		return KEY_INSERTED;
	}
	if (action == INSERT_OVERFULL)
	{	
		if (IsLeaf(actualNode))
		{
			// in the case that we have leaf node
			cDomNode_Leaf<TKeyItem, TLeafItem>* rightNode = (index == EMPTY_POINTER) ? NULL : mCache->GetLeafNode(index);
			if (!mCache->GetLeafNode(actualNode)->SplitNode(rightNode, mNewLeafNode))
			{
				action = INSERT_FIRSTCHANGED;
				return 0;
			} else
			{
				newNodeIndex = mHeader->GetTopLeafNodeIndex();
				*newNodeKey = (KeyType*)&mNewLeafNode->GetKey(0);
				mNewLeafNode = mCache->CreateLeafNode();							
				if (mStack->Empty() || mLevel != ((cDomStackItem*)(mStack->TopRef()))->GetLevel())
				{
					// new root has to be created
					root = mHeader->GetTopInnerNodeIndex() | INNER_NODE_FLAG;
					CreateNewRoot(actualNode, mCache->GetLeafNode(actualNode)->GetKey(0), newNodeIndex, 
						mCache->GetLeafNode(newNodeIndex)->GetKey(0));
					return NEW_ROOT;
				}
			}
		} else
		{
			// in the case that we have inner node we do almast the same
			cDomNode_Inner<TKeyItem, TLeafItem>* rightNode = (index == EMPTY_POINTER) ? NULL : mCache->GetInnerNode(GetIndex(index));
			if (!mCache->GetInnerNode(GetIndex(actualNode))->SplitNode(rightNode, mNewInnerNode))
			{
				action = INSERT_FIRSTCHANGED;
				return 0;
			} else
			{
				newNodeIndex = mHeader->GetTopInnerNodeIndex() | INNER_NODE_FLAG;
				*newNodeKey = (KeyType*)&mNewInnerNode->GetKey(0);
				mNewInnerNode = mCache->CreateInnerNode();							
				if (mStack->Empty() || mLevel != ((cDomStackItem*)(mStack->TopRef()))->GetLevel())
				{
					// new root has to be created
					root = mHeader->GetTopInnerNodeIndex() | INNER_NODE_FLAG;
					CreateNewRoot(actualNode, mCache->GetInnerNode(GetIndex(actualNode))->GetKey(0), 
						newNodeIndex, mCache->GetInnerNode(GetIndex(newNodeIndex))->GetKey(0));
					return NEW_ROOT;
				}
			}
		}
	}
	return 0;
}

/**
* Create new sub-tree
* \param level Level of a new DOM node.
* \param newTreeIsLeaf The new DOM node is a leaf node (if this variable is true).
* \return Index of new created node
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
unsigned int cDomTree<TDataStructure, TKeyItem, TLeafItem>
	::CreateNewTree(unsigned char level, bool newTreeIsLeaf)
{
	unsigned int nodeIndex;
	if (newTreeIsLeaf)
	{
		nodeIndex = mHeader->GetTopLeafNodeIndex();
		mNewLeafNode->SetLevel(level);
		mNewLeafNode = mCache->CreateLeafNode();
	} else
	{
		nodeIndex = mHeader->GetTopInnerNodeIndex() | INNER_NODE_FLAG;
		mNewInnerNode->SetLevel(level);
		mNewInnerNode = mCache->CreateInnerNode();
	}
	return nodeIndex;
}


/**
* Create new root node in the existing sub-tree.  The index of the new created node is the top on in header.
* \param firstNodeIndex Index of the first child node of the new root.
* \param firstNodeKey Key of the the first child node of the new root.
* \param secondNodeIndex Index of the second child node of the new root.
* \param secondNodeIndex Key of the the second child node of the new root.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cDomTree<TDataStructure, TKeyItem, TLeafItem>
	::CreateNewRoot(unsigned int firstNodeIndex, const KeyType& firstNodeKey,
					unsigned int secondNodeIndex, const KeyType& secondNodeKey)
{
	mNewInnerNode->SetLevel(mLevel);
	mNewInnerNode->SetKey(0, firstNodeKey);
	mNewInnerNode->SetPointer(0, firstNodeIndex);
	mNewInnerNode->SetKey(1, secondNodeKey);
	mNewInnerNode->SetPointer(1, secondNodeIndex);
	mNewInnerNode->SetItemCount(2);

	mNewInnerNode = mCache->CreateInnerNode();
}

/**
* Set new pointer value in a leaf item with a key.
* \param root Root of the DOM node, where the leaf item with key is searched.
* \param key Key value.
* \param newPointer new pointer value.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cDomTree<TDataStructure, TKeyItem, TLeafItem>
	::SetNewPointer(unsigned int root, const KeyType* key, unsigned int newPointer)
{
	unsigned int actualNode = root;
	bool founded, leaf;
	int order = 0;

	while(true) {
		leaf = IsLeaf(actualNode);
		actualNode = GetIndex(actualNode);
		if (leaf)
		{
			founded = mCache->GetLeafNode(actualNode)->SearchItem(*key, order);
		} else
		{
			founded = mCache->GetInnerNode(actualNode)->SearchItem(*key, order);
		}

		if (leaf)
		{
			if (founded)
			{
				mCache->GetLeafNode(actualNode)->SetPointer(order, newPointer);
				return;
			} else
			{
				printf("cDomTree::SetNewPointer - error, item with key was not founded!!\n");
				exit(1);
			}
		} else
		{
			if (order != -1)
			{
				actualNode = mCache->GetInnerNode(actualNode)->GetPointer(order);
			} else
			{
				printf("cDomTree::SetNewPointer - error, node was not founded!!\n");
				exit(1);
			}
		}		
	}
}


/**
* \param index Index of a node
* \return
*	- true - node index is leaf node
*	- false - node index is inner node
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
bool cDomTree<TDataStructure, TKeyItem, TLeafItem>
	::IsLeaf(unsigned int index)
{
	return (index & INNER_NODE_FLAG) == 0;
}

/**
* \param index Index of a node
* \return Real index without flag
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
unsigned int cDomTree<TDataStructure, TKeyItem, TLeafItem>
	::GetIndex(unsigned int index)
{
	return index & ~INNER_NODE_FLAG;
}

/**
* Method push the information about current index, right index and level into the stack
* \param index Index of the node.
* \param rightIndex Index of its right node. Can be EMPTY_POINTER.
* \param level Level of the node in the DOM tree
* \param order Order of the node in the parent node. Can be EMPTY_POINTER in a case of root node of the sub-tree.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cDomTree<TDataStructure, TKeyItem, TLeafItem>
	::PushToStack(cGeneralStack<cTreeTupleType>* stack, unsigned int index, unsigned int rightIndex, unsigned char level, unsigned int order)
{
	mStackItem->SetIndex(index);
	mStackItem->SetRightIndex(rightIndex);
	mStackItem->SetLevel(level);
	mStackItem->SetNodeOrder(order);
	stack->Push(*mStackItem);
}

// TODO - ProcessTree() a PrintTree() prepsat s pouzitim kuzoru a rekurzivne (podobne jako SetOptional())

/**
* Traverse whole tree and store nodes in the data structure
* \param dataStructure Data structure storing the elements of a DataGuide.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cDomTree<TDataStructure, TKeyItem, TLeafItem>
	::ProcessTree(TDataStructure* dataStructure, cArray<unsigned int>* pointerArray, cGeneralArray<cUIntType>* keyArray)
{
	unsigned int pointer, lastlevel = 0, order = 0, index = mHeader->GetRootIndex();
	unsigned int mRequiredCount = 0;

	dataStructure->InitializeBeforeParsingOneTree(0); // TODO - zde nemela byt nula ale obecne cislo vkladaneho stromu
	mStack->Clear();
	while(true)
	{		
		if (!IsLeaf(index))
		{
			if (order >= mCache->GetInnerNode(GetIndex(index))->GetItemCount())
			{
				if (mStack->Empty())
				{
					break;
				}
				*mStackItem = mStack->Pop();
				index = mStackItem->GetIndex();
				order = mStackItem->GetNodeOrder() + 1;
			} else
			{
				//mNode[index]->Print(" \n ", index);
				//printf(" order: %d\n", order);

				PushToStack(mStack, index, EMPTY_POINTER, 0, order);
				index = mCache->GetInnerNode(GetIndex(index))->GetPointer(order);
				order = 0;
			}
		} else
		{
			if (order >= mCache->GetLeafNode(index)->GetItemCount())
			{
				if (mStack->Empty())
				{
					break;
				}
				*mStackItem = mStack->Pop();
				index = mStackItem->GetIndex();
				order = mStackItem->GetNodeOrder() + 1;
			} else
			{
				if (false)
				{
					for (unsigned int i = 0; i < (unsigned int)mStack->Count(); i++)
					{
						printf("  ");
					}
					printf("key: %d, lp: %d\n", mCache->GetLeafNode(index)->GetKey(order), mCache->GetLeafNode(index)->GetLeaf(order));
				}
	
				if (!mCache->GetLeafNode(index)->IsOptional(order))
				{
					mRequiredCount++;
				}

				pointer = FindLabeledPath(pointerArray, keyArray, mCache->GetLeafNode(index)->GetLeaf(order));
				if (lastlevel >= mCache->GetLeafNode(index)->GetLevel())
				{
					dataStructure->EndAndStartNode(false, mCache->GetLeafNode(index)->GetLevel(), 
						mCache->GetLeafNode(index)->GetKey(order), lastlevel - mCache->GetLeafNode(index)->GetLevel(), pointer);
				} else
				{
					dataStructure->StartNode(false, mCache->GetLeafNode(index)->GetLevel(), 
						mCache->GetLeafNode(index)->GetKey(order), pointer);
				}
				lastlevel = mCache->GetLeafNode(index)->GetLevel();

				if (mCache->GetLeafNode(index)->GetPointer(order) != EMPTY_POINTER)
				{
					PushToStack(mStack, index, EMPTY_POINTER, 0, order);
					index = mCache->GetLeafNode(index)->GetPointer(order);
					order = 0;
				} else
				{
					order++;
				}
			}
		}
	}
	dataStructure->EndDocumentInserting();

	printf("Required count: %d\n", mRequiredCount);
}


/**
* Search leaf id in the keyArray.
* \param leaf Labeled path id which was generated by cLPTree and which is searched in the keyArray.
* \return Order of the item from the pointerArray, which is on the position of the founded lp in keyArray.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
unsigned int cDomTree<TDataStructure, TKeyItem, TLeafItem>
	::FindLabeledPath(cArray<unsigned int>* pointerArray, cGeneralArray<cUIntType>* keyArray, unsigned int leaf)
{
	unsigned int pointer = (unsigned int)-1;
	int mid = 0;
	int lo = 0;
	int hi = (int)keyArray->Count() - 1;

	do
	{
		mid = (lo + hi) / 2;

		if (keyArray->GetRefItem(mid) > leaf)
		{
			hi = mid-1;
		}
		else
		{
			if (keyArray->GetRefItem(mid) == leaf)
			{
				break;
			}
			lo = mid+1;

			if (lo > hi)
			{
				mid++;
			}
		}
	}
	while(lo <= hi);

	if (keyArray->GetRefItem(mid) != leaf)
	{
		return (unsigned int)-1;
	} else
	{
		pointer = pointerArray->GetRefItem(mid);
		if (mid < (int)keyArray->Count() - 1)
		{
			pointerArray->ShiftLeft(mid + 1);
			keyArray->ShiftLeft(mid + 1);
		} else
		{
			pointerArray->SetCount(pointerArray->Count() - 1);
			keyArray->SetCount(keyArray->Count() - 1);
		}
		return pointer;
	}
}

/**
* Print tree nodes
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cDomTree<TDataStructure, TKeyItem, TLeafItem>
	::PrintNodes()
{
	printf("Leaf nodes:\n");
	for (unsigned int i = 0; i < mHeader->GetTopLeafNodeIndex(); i++)
	{
		mCache->GetLeafNode(i)->Print("\n", i);
	}
	printf("Iner nodes:\n");
	for (unsigned int i = 0; i < mHeader->GetTopInnerNodeIndex(); i++)
	{
		mCache->GetInnerNode(i)->Print("\n", i);
	}
}

/**
* Print DOM tree from the cursor.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cDomTree<TDataStructure, TKeyItem, TLeafItem>
	::PrintTree(tCursor* cursor)
{
	if (!cursor->IsOptional())
	{
		printf("*");
	}
	printf("key: ");
	TKeyItem::Print(", ", cursor->GetKey());
	printf("leaf: %d, node count: %d\n", cursor->GetLeaf(), cursor->GetNodeCount());

	if (cursor->GetPointer() != EMPTY_POINTER)
	{
		cursor->FindFirst();
		do {
			PrintTreeR(cursor, 1);
		} while(cursor->FindNext());
		cursor->MoveUp();
	}
}

/**
* Print DOM tree from the cursor.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cDomTree<TDataStructure, TKeyItem, TLeafItem>
	::PrintTreeR(tCursor* cursor, unsigned int level)
{
	for (unsigned int i = 0; i < level; i++)
	{
		printf("  ");
	}
	if (!cursor->IsOptional())
	{
		printf("*");
	}
	printf("key: ");
	TKeyItem::Print(", ", cursor->GetKey());
	printf("leaf: %d, node count: %d\n", cursor->GetLeaf(), cursor->GetNodeCount());

	if (cursor->GetPointer() != EMPTY_POINTER)
	{
		cursor->FindFirst();
		do {
			PrintTreeR(cursor, level + 1);
		} while(cursor->FindNext());
		cursor->MoveUp();
	}
}

/**
* Print whole DOM tree.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cDomTree<TDataStructure, TKeyItem, TLeafItem>
	::PrintTree()
{
	unsigned int order = 0, index = mHeader->GetRootIndex();

	mStack->Clear();
	while(true)
	{		
		if (!IsLeaf(index))
		{
			index = GetIndex(index);
			if ((unsigned char)order >= mCache->GetInnerNode(index)->GetItemCount())
			{
				if (mStack->Empty())
				{
					break;
				}
				*mStackItem = mStack->Pop();
				index = mStackItem->GetIndex();
				order = mStackItem->GetNodeOrder() + 1;
			} else
			{
				//mNode[index]->Print(" \n ", index);
				//printf(" order: %d\n", order);

				PushToStack(mStack, index | INNER_NODE_FLAG, EMPTY_POINTER, 0, order);
				index = mCache->GetInnerNode(index)->GetPointer(order);
				order = 0;
			}
		} else
		{
			if ((unsigned char)order >= mCache->GetLeafNode(index)->GetItemCount())
			{
				if (mStack->Empty())
				{
					break;
				}
				*mStackItem = mStack->Pop();
				index = mStackItem->GetIndex();
				order = mStackItem->GetNodeOrder() + 1;
			} else
			{
				//mNode[index]->Print(" \n ", index);
				//printf(" order: %d\n", order);
				for (unsigned int i = 0; i < (unsigned int)mStack->Count(); i++)
				{
					if (IsLeaf(((cDomStackItem*)&mStack->GetItem(i))->GetIndex()))
					{
						printf("  ");
					}
				}
				if (!mCache->GetLeafNode(index)->IsOptional(order))
				{
					printf("*");
				}
				printf("key: ");
				TKeyItem::Print(", ", mCache->GetLeafNode(index)->GetKey(order));
				//printf("leaf: %d (%d, %d)\n", mCache->GetLeafNode(index)->GetLeaf(order) , mCache->GetLeafNode(index)->GetDescendantCount(order) & cDomNode_Leaf<TKeyItem,TLeafItem>::MAXIMAL_DESCENDANT_COUNT, mCache->GetLeafNode(index)->GetNodeCount(order));
				printf("leaf: %d, node count: %d\n", mCache->GetLeafNode(index)->GetLeaf(order), mCache->GetLeafNode(index)->GetNodeCount(order));

				if (mCache->GetLeafNode(index)->GetPointer(order) != EMPTY_POINTER)
				{
					PushToStack(mStack, index, EMPTY_POINTER, 0, order);
					index = mCache->GetLeafNode(index)->GetPointer(order);
					order = 0;
				} else
				{
					order++;
				}
			}
		}
	}
}

#endif