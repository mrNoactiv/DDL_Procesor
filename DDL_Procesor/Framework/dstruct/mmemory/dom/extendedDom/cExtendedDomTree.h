/**
*	\file cExtendedDomTree.h
*	\author Radim Baca
*	\version 0.1
*	\date jan 2009
*	\brief Represent DOM tree in a main memory.
*/


#ifndef __cExtendedDomTree_h__
#define __cExtendedDomTree_h__

#include "dstruct/mmemory/dom/cDomTree.h"
#include "dstruct/mmemory/dom/extendedDom/cExtendedDomTree.h"
#include "dstruct/mmemory/dom/extendedDom/type/cXDGKey.h"
#include "dstruct/mmemory/dom/extendedDom/type/cXDGKeySpaceDescriptor.h"
#include "dstruct/mmemory/dom/extendedDom/type/cXDGKeySizeInfo.h"
#include "dstruct/mmemory/dom/extendedDom/type/cXDGKeyType.h"
#include "dstruct/mmemory/dom/cDomHeader.h"
#include "dstruct/mmemory/dom/extendedDom/cMapSearchAll.h"
#include "dstruct/mmemory/dom/extendedDom/cMapSearchInput.h"
#include "dstruct/mmemory/dom/extendedDom/type/cMapSearchPair.h"
#include "dstruct/mmemory/dom/extendedDom/type/cMapSearchPairSizeInfo.h"

#include "cXMLIndexConst.h"

/**
* Class extends the DataGuide and implement the insert operation of an extended DataGuide.
* Template parameters:
*	- TKeyItem - Have to be inherited from cBasicType. Represent type of the key value.
*	- TLeafItem - Have to be inherited from cBasicType. Represent type of the leaf value.
*
* Terms used with connection of this structure:
*	- DOM tree - Whole tree.
*	- Sub-tree - One sub-tree representing one node in a DOM tree.
*	- DOM sub-tree - DOM tree, which is a sub-tree of DOM tree.
*	- Node - one node in a Sub-tree. Nodes are fixed-length in order to avoid extensive reallocation. Therefore, one DOM node is represented by a Sub-tree.
*
* DOM tree use two types of nodes:
*	- Inner node - Inner node of a sub-tree. It contains only key values and pointers to child nodes.
*	- Leaf node - Leaf node of a sub-tree. This node contains also leaf values (labeled path ids in the case of DataGuide).
* Level of the root node is 1.
*
*	\author Radim Baca
*	\version 0.1
*	\date jan 2009
**/
template<class TDataStructure, class TKeyItem, class TLeafItem>
class cExtendedDomTree: public cDomTree<TDataStructure, TKeyItem, TLeafItem>
{
	cMapSearchPairSizeInfo*							mMapSearchSizeInfo;
	cGeneralFactory<cBasicType<cMapSearchPair>>*	mFactory;

	// variables resolving the mapping
	cMapSearchAll*									mMapSearch;
	cArray<cMapSearchInput*>*						mMapInputArray;			/// Buffer of cMapSearchInput classes.
	unsigned int									mMapInputCount;
	cMapSearchPair*									mPair;
	unsigned int									mMapSearchAlgorithm;
	
	// variables resolving merging of the optinal branches
	cSortedArray<cUIntType>**						mMergeArray;			/// Store information about optional siblings. If there is more than one optional sibling with the same key, then they are merged.
	unsigned int									mMergeArrayStack;		/// Used in order to create the stack from mMergeArray. Point to the top item in the stack.
	cArray<tCursor*>*								mCursors;				/// Array of cursors used during the optional branches merge. Array is used as a stack.
	unsigned int									mCursorsCount;			/// Number of used cursors in the mCursors array (simulate stack).
	cSortedArrayWithLeaf<TLeafItem, TLeafItem>*		mSortedLeaf1;			/// Used by SetOptional() method.
	bool											mHasSiblings;			/// If true after InsertAndCopyTree() method, then the inserted DOM sub-tree has a DOM nodes with the same key.

	unsigned int									mRequiredCount;
	unsigned int									mRequiredEdgeCount;

	// methods supporting DOM tree insertion and DOM sub-tree merge
	void InsertDomExtendR(bool moveNext, bool branchesMerge, cMapSearchPair** pair, tCursor* thisCursor, tCursor* paramCursor, cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf1,	cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf2);
	void SameKey( cMapSearchPair** pair, tCursor* thisCursor, tCursor* paramCursor, cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf1, cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf2);
	void MergeOptionalBranches(tCursor* thisCursor, tCursor* paramCursor, cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf1, cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf2);
	void MergeSiblings(tCursor* thisCursor, cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf1);
	void MergeSiblingsInSubtree(tCursor* cursor);
	virtual void InsertAndCopyTree(tCursor* thisCursor, tCursor* paramCursor, cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf, bool isOptional);
	virtual void CopyLeafItems(unsigned int indexParam, cDomCache<TKeyItem,TLeafItem>* cacheParam, cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf);
	virtual void MoveUp(tCursor* thisCursor, tCursor* paramCursor, cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf);
	virtual void SetOptional(tCursor* cursor);
	void DeleteItem(KeyType* key, tCursor* thisCursor);
	inline tCursor* GetNextCursor() {
		if (mCursorsCount > mCursors->Count()) {
			mCursors->Resize(mCursors->Count() * 2, true);
			mCursors->SetCount(mCursors->Size());
		}
		if (mCursors->GetRefItem(mCursorsCount) == NULL) 
			mCursors->GetRefItem(mCursorsCount) = new tCursor();
		return mCursors->GetRefItem(mCursorsCount++);
	}
	inline void LocateOrder(tCursor* thisCursor, unsigned int order);
	void ProcessTreeR(tCursor* cursor, TDataStructure* dataStructure, cArray<unsigned int>* pointerArray, cGeneralArray<cUIntType>* keyArray);

	bool CheckDOMSubtree(bool moveNext, cMapSearchPair* pair, unsigned int maximalChange, unsigned int& change, tCursor* thisCursor, tCursor* paramCursor);
	unsigned int MoveNextAndCount(tCursor* thisCursor, tCursor* paramCursor);
	bool FindLowestMapping_Alg1(cMapSearchPair* pair, unsigned int maximalChange, unsigned int& change, tCursor* thisCursor, tCursor* paramCursor);

	void PrintMapping(cMapSearchPair** pair, unsigned int level);
	bool CheckMapping(cMapSearchPair** pair);

public:
	// these constants are duplicated in a cDomNode_Inner!!! In the case of any update, check also these constants.
	static const int INSERT_OK = 0;
	static const int INSERT_OVERFULL = 1;
	static const int INSERT_FIRSTCHANGED = 2;

	// algorithms available for a DOM merge. Different algorithms use different mapping search strategies.
	static const unsigned int ALG_NOT_INTO_SAME_SIBLING = 1;

	cExtendedDomTree(cDomHeader<TKeyItem, TLeafItem>* header, unsigned int startNodeCount = DEFAULT_START_NODE_COUNT);
	~cExtendedDomTree();

	virtual void Init();
	virtual void Delete();

	//bool FindPrevious(unsigned int& leafIndex, unsigned int& leafNodeOrder);
	unsigned int CountRequiredEdges(unsigned int index, unsigned int order);

	void InsertDom(unsigned int maximalChange, cExtendedDomTree* dom, const KeyType& firstLevel, cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf1, cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedArray2);
	void ProcessTree(TDataStructure* dataStructure, cArray<unsigned int>* pointerArray, cGeneralArray<cUIntType>* keyArray);

	void SetAlgorithm(unsigned int mapSearchAlgorithm)	{ mMapSearchAlgorithm = mapSearchAlgorithm; }
};

/**
* Constructor
* \param header Header of the cExtendedDomTree.
* \param startNodeCount Initial number of a main memory array storing the nodes.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
cExtendedDomTree<TDataStructure, TKeyItem, TLeafItem>
	::cExtendedDomTree(cDomHeader<TKeyItem, TLeafItem>* header, unsigned int startNodeCount)
	:cDomTree<TDataStructure, TKeyItem, TLeafItem>(header, startNodeCount),
	mMapSearchSizeInfo(NULL), mFactory(NULL),
	mMapSearch(NULL),
	mMapInputArray(NULL), mPair(NULL),
	mMergeArray(NULL), mCursors(NULL)
{
	Init();
}

/**
* Destructor
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
cExtendedDomTree<TDataStructure, TKeyItem, TLeafItem>::~cExtendedDomTree()
{
	Delete();
}

/** 
* Delete all variables allocated on the heap
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cExtendedDomTree<TDataStructure, TKeyItem, TLeafItem>::Delete()
{
	if (mMapSearchSizeInfo != NULL)
	{
		delete mMapSearchSizeInfo;
		mMapSearchSizeInfo = NULL;
	}
	if (mFactory != NULL)
	{
		delete mFactory;
		mFactory = NULL;
	}
	if (mMapSearch != NULL)
	{
		delete mMapSearch;
		mMapSearch = NULL;
	}
	if (mMapInputArray != NULL)
	{
		for (unsigned int i = 0; i < mMapInputArray->Count(); i++)
		{
			delete mMapInputArray->GetRefItem(i);
		}
		delete mMapInputArray;
		mMapInputArray = NULL;
	}
	if (mPair != NULL)
	{
		delete mPair;
		mPair = NULL;
	}
	if (mMergeArray != NULL)
	{
		for (unsigned int i = 0; i < cXMLIndexConst::DEFAULT_MAX_TUPLE_SIZE; i++)
		{
			delete mMergeArray[i];
		}
		delete mMergeArray;
		mMergeArray = NULL;
	}
	if (mCursors != NULL)
	{
		delete mCursors;
		mCursors = NULL;
	}
}

/**
* First free all allocated memory and then allocate new
* \param startNodeCount Initial number of a main memory array storing the nodes
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cExtendedDomTree<TDataStructure, TKeyItem, TLeafItem>
	::Init()
{
	Delete();

	mMapSearchSizeInfo = new cMapSearchPairSizeInfo();
	mFactory = new cGeneralFactory<cBasicType<cMapSearchPair>>(mMapSearchSizeInfo);
	mMapSearch = new cMapSearchAll();
	mMapInputArray = new cArray<cMapSearchInput*>();
	mMapInputArray->Resize(cXMLIndexConst::DEFAULT_MAX_TUPLE_SIZE);
	for (unsigned int i = 0; i < cXMLIndexConst::DEFAULT_MAX_TUPLE_SIZE; i++)
	{
		mMapInputArray->Add(new cMapSearchInput());
	}

	cSizeInfo<unsigned int>* intSizeInfo = new cSizeInfo<unsigned int>();
	mMergeArray = new cSortedArray<cUIntType>*[cXMLIndexConst::DEFAULT_MAX_TUPLE_SIZE];
	for (unsigned int i = 0; i < cXMLIndexConst::DEFAULT_MAX_TUPLE_SIZE; i++)
	{
		mMergeArray[i] = new cSortedArray<cUIntType>(intSizeInfo);
	}

	mCursors = new cArray<tCursor*>(true);
	mCursors->Resize(20);
	mCursors->SetCount(20);

	mDebug = false;
}


//*****************************************************************************************************
//****************************************   DOM Insert    ********************************************
//*****************************************************************************************************


/**
* Insert the DOM tree into this DOM tree. Root of the DOM tree in the parameter is inserted into the second level
* of this DOM tree.
* \param maximalChange Maximal change possible during the insert.
* \param dom The DOM tree, which is inserted into this DOM tree.
* \param firstLevel First level key, where the DOM tree is inserted.
* \param sortedLeaf1 Sometimes are branches in this tree merged. This array capture leaf number change during the merge.
* \param sortedLeaf2 Map the leaf numbers of the inserted DOM tree into leaf numbers of this DOM tree. Created during insertion. Keys are leaf values of the inserted DOM tree.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cExtendedDomTree<TDataStructure, TKeyItem, TLeafItem>::InsertDom(
		unsigned int maximalChange, 
		cExtendedDomTree* dom, 
		const KeyType& firstLevel, 
		cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf1,
		cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf2)
{
	unsigned int change = 0;
	cMapSearchPair* pair;
	cMapSearchPair* minimalMapping;

	mParamCursor->SetDOMCache(dom->GetCache());
	mParamCursor->Clear(dom->GetRootIndex());
	mThisCursor->Clear(mHeader->GetRootIndex());
	mCursorsCount = 0;
	mSortedLeaf1 = sortedLeaf1;

	mStack->Clear();
	// we move thisCursor into the first level
	if (!mThisCursor->FindFirst(&firstLevel, mHeader->GetRootIndex()))
	{
		printf("cExtendedDomTree::InsertDom() - error, key on a first level of DOM not found!\n");
		return;
	}
	if (mThisCursor->GetPointer() != (unsigned int)-1)
	{
		mParamCursor->FindFirst(NULL, dom->GetRootIndex());
		if (mThisCursor->FindFirst(&mParamCursor->GetKey()))
		{		
			bool hasNext, subtreeExist = false;
			unsigned int maximalChangeComputed, minimalChange = (unsigned int)-1, minimalChangeIndex = (unsigned int)-1;
			unsigned int allNodes;

			mFactory->Clear();
			// find DOM sub-tree with minimal change
			do {
				change = 0;
				mMapInputCount = 0;
				mMapInputArray->GetRefItem(mMapInputCount)->Clear();
				pair = mFactory->GetNext();
				maximalChangeComputed = mThisCursor->CountRequiredEdges() + mParamCursor->CountRequiredEdges();
				allNodes = mThisCursor->CountAllEdges() + mParamCursor->CountAllEdges();
				allNodes *= (float)maximalChange/(float)100;
				if (allNodes < maximalChangeComputed)
				{
					maximalChangeComputed -= allNodes;
				} else
				{
					maximalChangeComputed = 0;
				}
				if (CheckDOMSubtree(false, pair, maximalChangeComputed, change, mThisCursor, mParamCursor))
				{
					if (mCheckConsistency)
					{
						cMapSearchPair* auxPair = pair;
						if (!CheckMapping(&auxPair))
						{
							printf("cExtendedDomTree::InsertDom() - mapping pairs are not consistent!\n");
							PrintMapping(&auxPair, 0);
							PrintTree();
							dom->PrintTree();
							exit(0);
						}
					}

					subtreeExist = true;
					if (change < minimalChange)
					{ 
						minimalChange = change;
						minimalChangeIndex = mThisCursor->GetKey().GetOrder();
						minimalMapping = pair;
					}
				}

				hasNext = mThisCursor->GetKey().HasNext();
				if (hasNext)
				{
					mThisCursor->FindNext();
				}
			} while(hasNext);

			if (subtreeExist)
			{
				// insert into existing subtree with minimal change
				while (minimalChangeIndex != mThisCursor->GetKey().GetOrder())
				{
					mThisCursor->FindPrevious();
					if(mParamCursor->GetKey().GetKey() != mThisCursor->GetKey().GetKey())
					{
						printf("error");
					}
				}

				if (mDebug)
				{
					PrintMapping(&minimalMapping, 0);
				}

				mMergeArrayStack = 0;
				mCache->GetLeafNode(mThisCursor->GetIndex())->IncNodeCount(mThisCursor->GetOrder(), mParamCursor->GetNodeCount());
				sortedLeaf2->Insert(mParamCursor->GetLeaf(), mThisCursor->GetLeaf());
				InsertDomExtendR(false, false, &minimalMapping, mThisCursor, mParamCursor, sortedLeaf1, sortedLeaf2);
			} else
			{
				// create new DOM sub-tree with the same key
				while (mThisCursor->GetKey().HasNext())
				{
					// find the key order
					mThisCursor->FindNext();
				} 
				mThisCursor->GetRefKey()->HasNext(true);
				unsigned int keyOrder = mThisCursor->GetKey().GetOrder() + 1;
				
				mParamCursor->GetRefKey()->SetOrder(keyOrder);
				mThisCursor->MoveUp();
				InsertAndCopyTree(mThisCursor, mParamCursor, sortedLeaf2, false);
			}
		} else
		{
			InsertAndCopyTree(mThisCursor, mParamCursor, sortedLeaf2, false);
		}
	} else
	{
		// copy param DOM subtree into this DOM
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
		CopyTree(dom->GetRootIndex(), newIndex, dom->GetCache(), sortedLeaf2, false);
	}
}

/**
* Check if the DOM tree in the parameter can be inserted into this DOM tree.
* \param moveNext Move the cursor next after this subtree is finnised
* \param pair Mapping of the nodes.
* \param maximalChange Maximal change allowed.
* \param change Parameter by reference, where the change of the subtree is returned.
* \param thisCursor Cursor of this DOM tree.
* \param paramCursor Cursor of the parameter DOM tree.
* \return
*		- true if the paramater DOM can be inserted,
*		- false otherwise.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
bool cExtendedDomTree<TDataStructure, TKeyItem, TLeafItem>::CheckDOMSubtree(
	bool moveNext,
	cMapSearchPair* pair,
	unsigned int maximalChange,
	unsigned int& change,
	tCursor* thisCursor, 
	tCursor* paramCursor)
{
	unsigned int thisChange = 0;

	thisCursor->FindFirst();
	paramCursor->FindFirst();

	while (thisCursor->GetIndex() != (unsigned int)-1 && paramCursor->GetIndex() != (unsigned int)-1)
	{

		bool sameKey = true;
		while (thisChange <= maximalChange && thisCursor->GetKey() != paramCursor->GetKey())
		{
			if (thisCursor->GetKey() < paramCursor->GetKey())
			{
				thisChange += thisCursor->CountRequiredEdges();
				if (!thisCursor->FindNext())
				{
					// count rest of the branches from parameter DOM
					do 
					{
						thisChange += paramCursor->CountRequiredEdges();
					} while(paramCursor->FindNext());
					sameKey = false;
					break;
				}
			} else
			{
				thisChange += paramCursor->CountRequiredEdges();
				if (!paramCursor->FindNext())
				{
					// count the rest of branches in this DOM
					do
					{
						thisChange += thisCursor->CountRequiredEdges();
					} while(thisCursor->FindNext());
					sameKey = false;
					break;
				}
			}
		}

		if (thisChange > maximalChange)
		{
			thisCursor->MoveUp();
			paramCursor->MoveUp();
			return false;
		}

		if (sameKey)
		{
			if (paramCursor->GetKey().HasNext() || thisCursor->GetKey().HasNext())
			{
				bool result = true;

				if (mMapSearchAlgorithm == ALG_NOT_INTO_SAME_SIBLING)
				{
					result = FindLowestMapping_Alg1(pair, maximalChange, thisChange, thisCursor, paramCursor);
				} else
				{
					printf("Algorithm not set!!!\n");
				}
				if (!result)
				{					
					thisCursor->MoveUp();
					paramCursor->MoveUp();
					return false;
				}
			} else
			{
				if (paramCursor->IsOptional() && !thisCursor->IsOptional())
				{
					thisChange += thisCursor->GetNodeCount();
				}
				if (!paramCursor->IsOptional() && thisCursor->IsOptional())
				{
					thisChange += paramCursor->GetNodeCount();
				}

				if (!CheckDOMSubtree(true, pair, maximalChange, thisChange, thisCursor, paramCursor))
				{
					thisCursor->MoveUp();
					paramCursor->MoveUp();
					return false;
				}
			}
		}
	}

	// move up
	if (thisCursor->GetIndex() != (unsigned int)-1)
	{
		thisChange += thisCursor->CountRequiredEdges();
		thisCursor->MoveUp();
	} else
	{
		thisCursor->MoveUp();
	}
	if (paramCursor->GetIndex() != (unsigned int)-1)
	{
		thisChange += paramCursor->CountRequiredEdges();
		paramCursor->MoveUp();
	} else
	{
		paramCursor->MoveUp();
	}

	// move next
	if (moveNext)
	{
		thisChange += MoveNextAndCount(thisCursor, paramCursor);
	}

	if (change + thisChange > maximalChange)
	{
		return false;
	} else
	{
		change += thisChange;
		return true;
	}
}

/**
* Generate posible mapping (if there exist any). This algorithm does not allows the siblings to map into the same node
* in the destination DOM. Check if the DOM tree in the parameter can be inserted into this DOM tree.
* \param pair Mapping of the nodes.
* \param maximalChange Maximal change allowed.
* \param change Parameter by reference, where the change of the founded mapping is returned.
* \param thisCursor Cursor of this DOM tree.
* \param paramCursor Cursor of the parameter DOM tree.
* \return
*		- true if the mapping has been found,
*		- false otherwise.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
bool cExtendedDomTree<TDataStructure, TKeyItem, TLeafItem>::FindLowestMapping_Alg1(
	cMapSearchPair* pair, 
	unsigned int maximalChange, 
	unsigned int& thisChange, 
	tCursor* thisCursor, 
	tCursor* paramCursor)
{
	cMapSearchPair* newPair;
	unsigned int newChange, pom;
	bool hasNext1, hasNext2, newChangeZero;
	bool hasOptionalRight;

	// cMapSearchInput creation
	mMapInputArray->GetRefItem(mMapInputCount)->Clear();
	do
	{
		do
		{
			assert(thisCursor->GetKey().GetKey() == paramCursor->GetKey().GetKey());

			//unsigned int rightOrder = mCache->GetLeafNode(index)->GetRefKey(order)->GetOrder();
			unsigned int rightOrder = thisCursor->GetKey().GetOrder();
			newChange = 0;
			newChangeZero = false;
			hasOptionalRight = false;
			mMapInputCount++;
			newPair = mFactory->GetNext();

			if ((mMapInputArray->GetRefItem(mMapInputCount - 1)->GetRightArray()->Count() <= rightOrder ||
				mMapInputArray->GetRefItem(mMapInputCount - 1)->GetRightArray()->GetRefItem(rightOrder) == cMapSearchInput::EMPTY_VALUE) &&
				CheckDOMSubtree(false, newPair, maximalChange, newChange, thisCursor, paramCursor))
			{

				newPair->SetLeft(paramCursor->GetKey().GetOrder());
				newPair->SetRight(rightOrder);
				newPair->SetIsRightSubtreeOptional(thisCursor->IsOptional());
				mMapInputArray->GetRefItem(mMapInputCount - 1)->AddEdge(newChange, newPair);
				if (newChange == 0 && !thisCursor->IsOptional())
				{
					newChangeZero = true;
					mMapInputCount--;
					break;
				}
				if (thisCursor->IsOptional())
				{
					hasOptionalRight = true;
				}
			}
			mMapInputCount--;

			hasNext2 = thisCursor->GetKey().HasNext();
			if (hasNext2)
			{					
				thisCursor->FindNext();
			}
		} while(hasNext2);
		
		while(thisCursor->GetKey().GetOrder() > 0)
		{
			thisCursor->FindPrevious();
		}
		if (!newChangeZero)
		{
			newPair = mFactory->GetNext();
			newChange = paramCursor->CountRequiredEdges();
			newPair->SetLeft(paramCursor->GetKey().GetOrder());
			newPair->SetRight(0);
			newPair->SetIsRightOptional(true);
			mMapInputArray->GetRefItem(mMapInputCount)->AddEdge(newChange, newPair);
		}

		hasNext1 = paramCursor->GetKey().HasNext();
		if (hasNext1)
		{
			paramCursor->FindNext();
		}
	} while(hasNext1);

	do
	{
		//unsigned int rightOrder = mCache->GetLeafNode(index)->GetRefKey(order)->GetOrder();
		unsigned int rightOrder = thisCursor->GetKey().GetOrder();
		if (mMapInputArray->GetRefItem(mMapInputCount)->GetRightArray()->Count() <= rightOrder ||
			mMapInputArray->GetRefItem(mMapInputCount)->GetRightArray()->GetRefItem(rightOrder) == cMapSearchInput::EMPTY_VALUE)
		{
			newPair = mFactory->GetNext();
			newChange = thisCursor->CountRequiredEdges();
			newPair->SetLeft(cMapSearchInput::EMPTY_VALUE);
			newPair->SetRight(thisCursor->GetKey().GetOrder());
			mMapInputArray->GetRefItem(mMapInputCount)->AddEdge(newChange, newPair);
		}
		hasNext2 = thisCursor->GetKey().HasNext();
		if (hasNext2)
		{					
			thisCursor->FindNext();						
		}
	} while(hasNext2);

	// find mapping
	if (mDebug)
	{
		mMapInputArray->GetRefItem(mMapInputCount)->Print();
	}
	if ((pom = mMapSearch->FindMapping(pair, maximalChange - thisChange, mMapInputArray->GetRefItem(mMapInputCount))) != cMapSearchInput::EMPTY_VALUE)
	{
		if (mDebug)
		{
			cMapSearchPair* auxPair = pair;
			PrintMapping(&auxPair, 0);
		}
		thisChange += pom;
		thisChange += MoveNextAndCount(thisCursor, paramCursor);
		return true;
	} else
	{
		return false;
	}
}

/**
* Move both cursors to the left and compute possible change
* \param thisCursor Cursor of this DOM tree.
* \param paramCursor Cursor of the parameter DOM tree.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
unsigned int cExtendedDomTree<TDataStructure, TKeyItem, TLeafItem>::MoveNextAndCount(
	tCursor* thisCursor, 
	tCursor* paramCursor)
{
	unsigned int thisChange = 0;

	if (thisCursor->GetIndex() != (unsigned int)-1)
	{
		if (!thisCursor->FindNext())
		{						
			// count rest of the branches from parameter DOM
			while(paramCursor->FindNext())
			{
				thisChange += paramCursor->CountRequiredEdges();
			} 
		}
	}
	if (paramCursor->GetIndex()  != (unsigned int)-1) 
	{
		if (!paramCursor->FindNext())
		{
			// count rest of the branches in this DOM tree
			do
			{
				thisChange += thisCursor->CountRequiredEdges();
			} while(thisCursor->FindNext());
		}
	} 

	return thisChange;
}


/**
* Recursive method merging two DOM trees. One call of this method resolve one level of the DOM trees.
* \param moveNext If true, method also resolve DOM siblings of the current cursor position.
* \param branchesMerge If true, method is used to merge optional branches in this DOM tree.
* \param pair Actual mapping pair.
* \param thisCursor Cursor of this DOM tree.
* \param paramCursor Cursor of parameter DOM tree.
* \param sortedLeaf1 Sometimes are branches in this tree merged. This array capture leaf number change during the merge.
* \param sortedLeaf2 Map the leaf numbers of the inserted DOM tree into leaf numbers of this DOM tree. Created during insertion. Keys are leaf values of the inserted DOM tree.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cExtendedDomTree<TDataStructure, TKeyItem, TLeafItem>::InsertDomExtendR(
	bool moveNext,
	bool branchesMerge,
	cMapSearchPair** pair,
	tCursor* thisCursor, 
	tCursor* paramCursor, 
	cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf1,
	cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf2)
{
	cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf = sortedLeaf2;
	unsigned int stackLevel;

	thisCursor->FindFirst();
	paramCursor->FindFirst();

	stackLevel = thisCursor->GetStack()->Count();

	if (branchesMerge)
	{
		sortedLeaf = sortedLeaf1;
	}

	while (thisCursor->GetIndex() != (unsigned int)-1 && paramCursor->GetIndex() != (unsigned int)-1)
	{
		if (ResolveDifferentKeys(thisCursor, paramCursor, sortedLeaf))
		{
			if (paramCursor->GetKey().HasNext() || thisCursor->GetKey().HasNext())
			{
				// this resolve situation when one or both DOM trees have siblings with the same key
				if (branchesMerge)
				{
					MergeOptionalBranches(thisCursor, paramCursor, sortedLeaf1, sortedLeaf2);
				} else
				{
					SameKey(pair, thisCursor, paramCursor, sortedLeaf1, sortedLeaf2);
				}
			} else
			{
				mCache->GetLeafNode(thisCursor->GetIndex())->IncNodeCount(thisCursor->GetOrder(), paramCursor->GetNodeCount());
				sortedLeaf->Insert(paramCursor->GetLeaf(), thisCursor->GetLeaf());
				if (paramCursor->IsOptional() && !thisCursor->IsOptional())
				{
					SetOptional(thisCursor);
				}
				InsertDomExtendR(true, branchesMerge, pair, thisCursor, paramCursor, sortedLeaf1, sortedLeaf2);
			}
		}

		if(thisCursor->GetIndex() != (unsigned int)-1 && stackLevel > thisCursor->GetStack()->Count())
		{
			printf("error!");
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
* This key merges the DOM trees with the same sibling keys. This merge is driven by a pair mapping, 
* which is created by a CheckDOMSubtree method. This method can also initiate branches merging in the same DOM tree in the case
* that more than one sibling DOM nodes with the same key is optional.
* \param pair Actual mapping pair.
* \param thisCursor Cursor of this DOM tree.
* \param paramCursor Cursor of parameter DOM tree.
* \param sortedLeaf1 Sometimes are branches in this tree merged. This array capture leaf number change during the merge.
* \param sortedLeaf2 Map the leaf numbers of the inserted DOM tree into leaf numbers of this DOM tree. Created during insertion. Keys are leaf values of the inserted DOM tree.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cExtendedDomTree<TDataStructure, TKeyItem, TLeafItem>::SameKey(
	cMapSearchPair** pair,
	tCursor* thisCursor, 
	tCursor* paramCursor, 
	cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf1,
	cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf2)
{
	bool isLast;
	unsigned int lastRequiredOrder, requiredCount = 0;
	mMergeArray[mMergeArrayStack]->Clear();

	do
	{
		*pair = (*pair)->GetRefNext();
		isLast = (*pair)->GetIsLastInMapping();
		if (!(*pair)->GetIsRightOptional())
		{
			LocateOrder(thisCursor, (*pair)->GetRight());

			if ((*pair)->GetChangeToHasNext())
			{
				thisCursor->GetRefKey()->HasNext(true);
			}

			// insert the parameter DOM subtree or mark this DOM subtree as an optional
			if ((*pair)->GetLeft() != cMapSearchInput::EMPTY_VALUE)
			{
				assert(paramCursor->GetKey().GetOrder() == (*pair)->GetLeft());

				mCache->GetLeafNode(thisCursor->GetIndex())->IncNodeCount(thisCursor->GetOrder(), paramCursor->GetNodeCount());
				sortedLeaf2->Insert(paramCursor->GetLeaf(), thisCursor->GetLeaf());
				if (paramCursor->IsOptional() && !thisCursor->IsOptional())
				{
					SetOptional(thisCursor);
				}
				mMergeArrayStack++;
				InsertDomExtendR(false, false, pair, thisCursor, paramCursor, sortedLeaf1, sortedLeaf2);
				mMergeArrayStack--;
			} else
			{
				SetOptional(thisCursor);
			}

			if (thisCursor->IsOptional())
			{
				mMergeArray[mMergeArrayStack]->Insert(thisCursor->GetKey().GetOrder());
			} else
			{
				requiredCount++;
				lastRequiredOrder = thisCursor->GetKey().GetOrder();
			}
		} else
		{
			paramCursor->GetRefKey()->SetOrder((*pair)->GetRight());
			bool rememberHasNext = paramCursor->GetKey().HasNext();
			if (!(*pair)->GetLastAmongSibling())
			{
				paramCursor->GetRefKey()->HasNext(true);
			} else
			{
				paramCursor->GetRefKey()->HasNext(false);
			}
			//KeyType *key = thisCursor->GetRefKey();
			//thisCursor->MoveUp();
			//InsertAndCopyTree(thisCursor, paramCursor, sortedLeaf2, true);
			//bool ret = thisCursor->FindFirst(key);
			//assert(ret);
			mKeyStack->Push(thisCursor->GetKey());
			thisCursor->MoveUp();
			InsertAndCopyTree(thisCursor, paramCursor, sortedLeaf2, true);
			bool ret = thisCursor->FindFirst(&mKeyStack->Top());
			assert(ret);
			mKeyStack->Pop();

			paramCursor->GetRefKey()->HasNext(rememberHasNext);
			mMergeArray[mMergeArrayStack]->Insert((*pair)->GetRight());
		}

		if (paramCursor->GetKey().HasNext())
		{
			paramCursor->FindNext();
		}
	} while(!isLast);


	if (mMergeArray[mMergeArrayStack]->GetItemCount() > 1)
	{
		// merge optional branches in this DOM tree
		LocateOrder(thisCursor, mMergeArray[mMergeArrayStack]->GetRefSortedItem(0));

		tCursor* cursor = GetNextCursor();
		*cursor = *thisCursor;
		for (unsigned int i = 1; i < mMergeArray[mMergeArrayStack]->GetItemCount(); i++)
		{
			LocateOrder(cursor, mMergeArray[mMergeArrayStack]->GetRefSortedItem(i));
			sortedLeaf1->Insert(cursor->GetLeaf(), thisCursor->GetLeaf());
			mCache->GetLeafNode(thisCursor->GetIndex())->IncNodeCount(thisCursor->GetOrder(), cursor->GetNodeCount());
			InsertDomExtendR(false, true, NULL, thisCursor, cursor, sortedLeaf1, sortedLeaf2);
		}

		// delete optional branches which has been copied
		for (unsigned int i = 1; i < mMergeArray[mMergeArrayStack]->GetItemCount(); i++)
		{
			LocateOrder(cursor, mMergeArray[mMergeArrayStack]->GetRefSortedItem(i));
			DeleteItem(thisCursor->GetRefKey(), cursor);
		}

		// locate last DOM node with the same key and set HasNext(false)
		unsigned int lastOrder = cursor->GetKey().GetOrder();
		while (cursor->GetKey().GetKey() == thisCursor->GetKey().GetKey())
		{
			lastOrder = cursor->GetKey().GetOrder();
			if (!cursor->FindNext())
			{
				break;
			}
		}
		LocateOrder(thisCursor, lastOrder);
		thisCursor->GetRefKey()->HasNext(false);

		// set new order values
		for (int newOrder = requiredCount; newOrder >= 0; newOrder--)
		{
			thisCursor->GetRefKey()->SetOrder(newOrder);
			if (newOrder > 0) 
			{
				thisCursor->FindPrevious();
			}
		}		
		mCursorsCount--;
	}

	// find the last DOM nodes with the same key and then move to the next key in both DOM trees
	assert(!paramCursor->GetKey().HasNext());
	while(thisCursor->GetKey().HasNext())
	{
		thisCursor->FindNext();
	}
	MoveNext(thisCursor, paramCursor, sortedLeaf2);	
}


/**
* First merge siblings defined by thisCursor into one DOM node and then insert all DOM nodes into this DOM node.
* \param thisCursor Cursor of this DOM tree.
* \param paramCursor Cursor of parameter DOM tree.
* \param sortedLeaf1 Sometimes are branches in this tree merged. This array capture leaf number change during the merge.
* \param sortedLeaf2 Map the leaf numbers of the inserted DOM tree into leaf numbers of this DOM tree. Created during insertion. Keys are leaf values of the inserted DOM tree.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cExtendedDomTree<TDataStructure, TKeyItem, TLeafItem>::MergeOptionalBranches(
	tCursor* thisCursor, 
	tCursor* paramCursor, 
	cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf1,
	cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf2)
{
	bool hasNext;

	// merge all sibling DOM nodes in this DOM tree
	if (thisCursor->GetKey().HasNext())
	{
		MergeSiblings(thisCursor, sortedLeaf1);
	}

	do
	{
		sortedLeaf1->Insert(paramCursor->GetLeaf(), thisCursor->GetLeaf());
		mCache->GetLeafNode(thisCursor->GetIndex())->IncNodeCount(thisCursor->GetOrder(), paramCursor->GetNodeCount());
		InsertDomExtendR(false, true, NULL, thisCursor, paramCursor, sortedLeaf1, sortedLeaf2);
		hasNext = paramCursor->GetKey().HasNext();
		if (paramCursor->GetKey().HasNext())
		{
			paramCursor->FindNext();
		}
	} while(hasNext);

	assert(!paramCursor->GetKey().HasNext());
	assert(!thisCursor->GetKey().HasNext());
	MoveNext(thisCursor, paramCursor, sortedLeaf2);	
}

/**
* Merge all sibling DOM nodes in this DOM tree with the same key value.
* \param thisCursor Cursor which represent position in the DOM tree.
* \param sortedLeaf1 Sometimes are branches in this DOM tree merged. This array capture leaf number change during the merge.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cExtendedDomTree<TDataStructure, TKeyItem, TLeafItem>::MergeSiblings(
	tCursor* thisCursor, 
	cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf1)
{
	unsigned int counter = 0;

	tCursor* cursor = GetNextCursor();
	*cursor = *thisCursor;
	do
	{
		assert(cursor->GetKey().HasNext());

		cursor->FindNext();
		sortedLeaf1->Insert(cursor->GetLeaf(), thisCursor->GetLeaf());
		mCache->GetLeafNode(thisCursor->GetIndex())->IncNodeCount(thisCursor->GetOrder(), cursor->GetNodeCount());
		InsertDomExtendR(false, true, NULL, thisCursor, cursor, sortedLeaf1, sortedLeaf1);
		counter++;
	} while(cursor->GetKey().HasNext());
	*cursor = *thisCursor;
	for (unsigned int i = 0; i < counter; i++)
	{
		cursor->FindNext();
		DeleteItem(thisCursor->GetRefKey(), cursor);
	}
	thisCursor->GetRefKey()->HasNext(false);
	mCursorsCount--;
}

/**
* Set whole DOM subtree as a optional and merge all siblings with the same key.
* \param cursor Cursor which represent position in the DOM tree.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cExtendedDomTree<TDataStructure, TKeyItem, TLeafItem>
	::SetOptional(tCursor* cursor)
{
	if (!cursor->IsOptional())
	{
		cursor->SetOptional();
		if (cursor->GetPointer() != EMPTY_POINTER)
		{
			cursor->FindFirst();
			do {
				if (cursor->GetKey().HasNext())
				{
					MergeSiblings(cursor, mSortedLeaf1);
					SetOptional(cursor);
				} else
				{
					SetOptional(cursor);
				}
			} while(cursor->FindNext());
			cursor->MoveUp();
		}
	}
}

/**
* Merge all siblings with the same key in the whole DOM sub-tree.
* \param cursor Cursor which represent position in the DOM tree.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cExtendedDomTree<TDataStructure, TKeyItem, TLeafItem>
	::MergeSiblingsInSubtree(tCursor* cursor)
{
	if (cursor->GetPointer() != EMPTY_POINTER)
	{
		cursor->FindFirst();
		do {
			if (cursor->GetKey().HasNext())
			{
				MergeSiblings(cursor, mSortedLeaf1);
				MergeSiblingsInSubtree(cursor);
			} else
			{
				MergeSiblingsInSubtree(cursor);
			}
		} while(cursor->FindNext());
		cursor->MoveUp();
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
void cExtendedDomTree<TDataStructure, TKeyItem, TLeafItem>::InsertAndCopyTree(
	tCursor* thisCursor, 
	tCursor* paramCursor, 
	cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf, 
	bool isOptional)
{
	mHasSiblings = false;
	cDomTree<TDataStructure, TKeyItem, TLeafItem>::InsertAndCopyTree(thisCursor, paramCursor, sortedLeaf, isOptional);

	if (mHasSiblings && isOptional)
	{
		thisCursor->FindFirst(paramCursor->GetRefKey());
		MergeSiblingsInSubtree(thisCursor);
		thisCursor->MoveUp();
	}

}


/**
* Move both cursors up.
* \param thisCursor Cursor into this DOM tree.
* \param paramCursor Cursor into parameter DOM tree.
* \param sortedLeaf Map the leaf numbers of the inserted DOM tree into leaf numbers of this DOM tree. Created during insertion. Keys are leaf values of the inserted DOM tree.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cExtendedDomTree<TDataStructure, TKeyItem, TLeafItem>::MoveUp(
	tCursor* thisCursor, 
	tCursor* paramCursor, 
	cSortedArrayWithLeaf<TLeafItem, TLeafItem>* sortedLeaf)
{
	mHasSiblings = false;
	cDomTree<TDataStructure, TKeyItem, TLeafItem>::MoveUp(thisCursor, paramCursor, sortedLeaf);

	if (mHasSiblings)
	{
		MergeSiblingsInSubtree(thisCursor);
	}
}

/**
* Copy leaf items of the leaf node.
* \param cacheParam Cache of the DOM tree, which should be copied into this DOM tree.
* \param sortedLeaf Map the leaf numbers of the inserted DOM tree into leaf numbers of this DOM tree. Created during insertion. Keys are leaf values of the inserted DOM tree.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cExtendedDomTree<TDataStructure, TKeyItem, TLeafItem>::CopyLeafItems(
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

		if (cacheParam->GetLeafNode(indexParam)->GetKey(i).HasNext())
		{
			mHasSiblings = true;
		}
	}
}

/**
* Delete the item on the cursor's position.
* \param key thisCursor argument has to be set on a new value with the key value after delete. We suppose that there is at least one such a item among siblings.
* \param thisCursor DOM cursor
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cExtendedDomTree<TDataStructure, TKeyItem, TLeafItem>::DeleteItem(
	KeyType* key, 
	tCursor* thisCursor)
{
	cGeneralStack<cTreeTupleType>* stack = thisCursor->GetRefStack();
	cDomCache<TKeyItem, TLeafItem>* cache = thisCursor->GetRefCache();
	unsigned int index, order;
	unsigned int count;
	bool end;
	
	do {
		end = true;
		index = stack->Top().GetUInt(0);
		order = stack->Top().GetUInt(1);
		stack->Pop();
		if (IsLeaf(index))
		{
			cache->GetLeafNode(index)->DeleteItem(order);
			count = cache->GetLeafNode(index)->GetItemCount();
		} else
		{
			cache->GetInnerNode(index & (~INNER_NODE_FLAG))->DeleteItem(order);
			count = cache->GetInnerNode(index & (~INNER_NODE_FLAG))->GetItemCount();
		}
		if (count == 0)
		{
			if (IsLeaf(stack->Top().GetUInt(0)))
			{
				printf("cExtendedDomTree::DeleteItem - root node is empty!!\n");
			}
			end = false;
		}
	} while(!end);

	while (!IsLeaf(stack->Top().GetUInt(0))) {
		index = stack->Top().GetUInt(0);
		stack->Pop();
	}

	thisCursor->FindFirst(key, index);
}

/**
* Find the nearest DOM node with this order.
* \param thisCursor DOM cursor into a DOM tree.
* \param order Order of the DOM node among the siblings with the same key.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cExtendedDomTree<TDataStructure, TKeyItem, TLeafItem>::LocateOrder(
	tCursor* thisCursor, 
	unsigned int order)
{
	while(thisCursor->GetKey().GetOrder() < order)
	{
		thisCursor->FindNext();
	}
	while(thisCursor->GetKey().GetOrder() > order)
	{
		thisCursor->FindPrevious();
	}
}


/**
* Traverse whole tree and store nodes in the data structure
* \param cursor Store position of the cursor in this DOM tree.
* \param dataStructure Data structure storing the elements of a DataGuide.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cExtendedDomTree<TDataStructure, TKeyItem, TLeafItem>
	::ProcessTreeR(tCursor* cursor, TDataStructure* dataStructure, cArray<unsigned int>* pointerArray, cGeneralArray<cUIntType>* keyArray)
{

	unsigned int pointer, counter = 0;

	if (cursor->GetPointer() != EMPTY_POINTER)
	{
		cursor->FindFirst();
		do {
			if (!mThisCursor->IsOptional())
			{
				mRequiredCount += cursor->GetNodeCount();
				mRequiredEdgeCount++;
			}
			pointer = FindLabeledPath(pointerArray, keyArray, mThisCursor->GetLeaf());
			dataStructure->StartNode(false, cursor->GetLevel(), cursor->GetKey().GetKey(), pointer);
			ProcessTreeR(cursor, dataStructure, pointerArray, keyArray);
			dataStructure->EndNode();
		} while(cursor->FindNext());
		cursor->MoveUp();
	}
}

/**
* Traverse whole tree and store nodes in the data structure
* \param dataStructure Data structure storing the elements of a DataGuide.
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cExtendedDomTree<TDataStructure, TKeyItem, TLeafItem>
	::ProcessTree(TDataStructure* dataStructure, cArray<unsigned int>* pointerArray, cGeneralArray<cUIntType>* keyArray)
{
	unsigned int pointer, counter = 0;
		
	mRequiredCount = 0;
	mRequiredEdgeCount = 0;

	dataStructure->InitializeBeforeParsingOneTree(0); // TODO - zde nemela byt nula ale obecne cislo vkladaneho stromu
	mThisCursor->Clear(mHeader->GetRootIndex());

	if (mThisCursor->GetPointer() != EMPTY_POINTER)
	{
		mThisCursor->FindFirst(NULL, mHeader->GetRootIndex());
		do {
			if (!mThisCursor->IsOptional())
			{
				mRequiredCount += mThisCursor->GetNodeCount();
				mRequiredEdgeCount++;
			}
			pointer = FindLabeledPath(pointerArray, keyArray, mThisCursor->GetLeaf());
			dataStructure->StartNode(false, mThisCursor->GetLevel(), mThisCursor->GetKey().GetKey(), pointer);
			ProcessTreeR(mThisCursor, dataStructure, pointerArray, keyArray);
			dataStructure->EndNode();
		} while(mThisCursor->FindNext());
		mThisCursor->MoveUp();
	}

	printf("Number of required nodes: %d\n", mRequiredCount);
	printf("Number of required edges: %d\n", mRequiredEdgeCount);
}

/**
* \param pair Actual mapping pair to print
* \param level Level of the mapping
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
bool cExtendedDomTree<TDataStructure, TKeyItem, TLeafItem>::CheckMapping(cMapSearchPair** pair)
{
	return true;
}

/**
* \param pair Actual mapping pair to print
* \param level Level of the mapping
*/
template<class TDataStructure, class TKeyItem, class TLeafItem>
void cExtendedDomTree<TDataStructure, TKeyItem, TLeafItem>::PrintMapping(cMapSearchPair** pair, unsigned int level)
{
	do
	{
		(*pair)->Print("\n");		
	} while((*pair = (*pair)->GetRefNext()) != NULL);
}

#endif