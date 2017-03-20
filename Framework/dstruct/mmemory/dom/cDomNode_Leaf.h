/**
*	\file cDomNode_Leaf.h
*	\author Radim Baca
*	\version 0.1
*	\date dec 2008
*	\brief Leaf node of the sub-tree.
*/


#ifndef __cDomNode_Leaf_h__
#define __cDomNode_Leaf_h__

#include "dstruct/mmemory/dom/cDomNode_Inner.h"

/**
* Leaf node of the sub-tree. This node contains only key values, pointers to child nodes and leaf values (labeled path ids in the case of DataGuide).
* Template parameters:
*	- TKeyItem - Have to be inherited from cBasicType. Represent type of the key value.
*	- TLeafItem - Have to be inherited from cBasicType. Represent type of the leaf value.
*
* Level of the root node is 1.
*
*	\author Radim Baca
*	\version 0.1
*	\date dec 2008
**/
template<class TKeyItem, class TLeafItem>
class cDomNode_Leaf: public cDomNode_Inner<TKeyItem, TLeafItem>
{
	typedef typename TKeyItem::Type KeyType;
	typedef typename TLeafItem::Type LeafType;
	
	LeafType*	mLeaf;						/// Leaf items
	unsigned int		mIsOptional;		/// Flags indicating that the DOM nodes are optional.
	unsigned int*		mNodeCount;			/// Number of XML nodes corresponding to this node.

	inline unsigned char GetNodeSize();
	inline void Resize(cMemory *memory);
public:

	cDomNode_Leaf(cDomHeader<TKeyItem, TLeafItem> *header, unsigned char level);
	~cDomNode_Leaf();

	inline void Init(cDomHeader<TKeyItem, TLeafItem> *header, unsigned char level);
	void Clear();

	inline void DeleteItem(unsigned int position);
	inline void MoveItems(unsigned int position, unsigned int moveLength);
	inline void CopyItems(unsigned int destination_position, unsigned int source_position, cDomNode_Leaf<TKeyItem, TLeafItem>* node, unsigned int count );
	inline void CopyItems(cDomNode_Leaf<TKeyItem, TLeafItem>* node, bool isOptional);
	int InsertLeafItem(unsigned int position, const LeafType& leaf, const KeyType& key, unsigned int pointer, bool isOptional, unsigned int nodeCount);
	bool SplitNode(cDomNode_Leaf<TKeyItem, TLeafItem>* rightNode, cDomNode_Leaf<TKeyItem, TLeafItem>* newNode);

	// Get
	inline const LeafType* GetLeaves(unsigned int order) const			{ return &mLeaf[order]; }
	inline const LeafType& GetLeaf(unsigned int order) const			{ return mLeaf[order]; }
	inline const unsigned int* GetNodesCount(unsigned int order) const	{ return &mNodeCount[order]; }
	inline const unsigned int& GetNodeCount(unsigned int order) const	{ return mNodeCount[order]; }
	inline const bool IsOptional(unsigned int order) const				{ return (mIsOptional & (1 << order)) > 0; }
	inline const unsigned int GetOptional()	const						{ return mIsOptional; }

	// Set
	inline void SetLeaf(unsigned int order, const LeafType& leaf)		{ mLeaf[order] = leaf; }
	inline void SetOptional(unsigned int order)							{ mIsOptional |= (1 << order); }
	inline void IncNodeCount(unsigned int order, const unsigned int nodeCountIncrease)	{ mNodeCount[order] += nodeCountIncrease; }

	void Print(char *str, unsigned int index) const;

};

/**
* Constructor
*/
template<class TKeyItem, class TLeafItem>
cDomNode_Leaf<TKeyItem, TLeafItem>
	::cDomNode_Leaf(cDomHeader<TKeyItem, TLeafItem> *header, unsigned char level)
	:cDomNode_Inner<TKeyItem, TLeafItem>::cDomNode_Inner(header, level)
	:mNodeCount(NULL)
{
	Init(header, level);
}

template<class TKeyItem, class TLeafItem>
cDomNode_Leaf<TKeyItem, TLeafItem>::~cDomNode_Leaf()
{
}

/// Has the same purpose as the constructor. In some cases, when the cMemory is used for a object creation this method
/// has to be called, to set all necessary attributes in the class
/// \param header header of the tree
/// \param level level of the DataGuide which the node cover
template<class TKeyItem, class TLeafItem>
void cDomNode_Leaf<TKeyItem, TLeafItem>::Init(cDomHeader<TKeyItem, TLeafItem> *header, unsigned char level)
{
	mHeader = header;
	mLevel = level;
	mCount = 0;
	mIsOptional = 0;

	Resize(mHeader->GetMemory());
}

/**
* Resize the leaf node
*/
template<class TKeyItem, class TLeafItem>
void cDomNode_Leaf<TKeyItem, TLeafItem>::Resize(cMemory *memory)
{
	mKey = (KeyType*)memory->GetMemory((mHeader->GetLeafNodeCapacity() + 1) * sizeof(KeyType));
	for (unsigned int i = 0; i < mHeader->GetLeafNodeCapacity() + 1; i++)
	{
		TKeyItem::Resize((cSizeInfo<KeyType>&)mHeader->GetKeySizeInfo(), memory, mKey[i]);
	}
	mPointers = (unsigned int*)memory->GetMemory((mHeader->GetLeafNodeCapacity() + 1) * sizeof(unsigned int));

	mLeaf = (LeafType*)memory->GetMemory((mHeader->GetLeafNodeCapacity() + 1) * TLeafItem::GetSerSize(mHeader->GetLeafSizeInfo()));
	mNodeCount = (unsigned int*)memory->GetMemory((mHeader->GetLeafNodeCapacity() + 1) * sizeof(unsigned int));
}

/**
* Reset the number of items in the node and optional flags.
*/
template<class TKeyItem, class TLeafItem>
void cDomNode_Leaf<TKeyItem, TLeafItem>::Clear()
{
	cDomNode_Inner<TKeyItem, TLeafItem>::Clear();
	mIsOptional = 0;
}

/**
* Delete one item on a specified position from this node.
* \param position Position of the item which should be removed from this node.
*/
template<class TKeyItem, class TLeafItem>
void cDomNode_Leaf<TKeyItem, TLeafItem>
	::DeleteItem(unsigned int position)
{
	if (position < mCount - 1)
	{
		TKeyItem::MoveBlock(&mKey[position], &mKey[position + 1], mCount - position - 1, mHeader->GetKeySizeInfo());
		TLeafItem::MoveBlock(&mLeaf[position], &mLeaf[position + 1], mCount - position - 1, mHeader->GetLeafSizeInfo());
		memmove(&mPointers[position], &mPointers[position + 1], (mCount - position - 1) * sizeof(unsigned int));
		memmove(&mNodeCount[position ], &mNodeCount[position + 1], (mCount - position - 1) * sizeof(unsigned int));
		
		unsigned int mask;
		if (position == 0)
		{
			mask = 0;
		} else
		{
			mask = 0xffffffff >> (32 - position);
		}
		mIsOptional = (mIsOptional & mask) + ((mIsOptional >> 1) & (~mask));
	}
	mCount--;
}

/// Create space in the array of items for new items. It also increase the mCount of the node.
/// \param position position where the space is created
/// \param moveLength size of the space
template<class TKeyItem, class TLeafItem>
void cDomNode_Leaf<TKeyItem, TLeafItem>
	::MoveItems(unsigned int position, unsigned int moveLength)
{
	if (position < mCount)
	{
		TKeyItem::MoveBlock(&mKey[position + moveLength], &mKey[position], mCount - position, mHeader->GetKeySizeInfo());
		TLeafItem::MoveBlock(&mLeaf[position + moveLength], &mLeaf[position], mCount - position, mHeader->GetLeafSizeInfo());
		memmove(&mPointers[position + moveLength], &mPointers[position], (mCount - position) * sizeof(unsigned int));
		memmove(&mNodeCount[position + moveLength], &mNodeCount[position], (mCount - position) * sizeof(unsigned int));
		
		unsigned int mask;
		if (position == 0)
		{
			mask = 0;
		} else
		{
			mask = 0xffffffff >> (32 - position);
		}
		mIsOptional = (mIsOptional & mask) + ((mIsOptional & (~mask)) << moveLength);
	}
	mCount = mCount + (unsigned char)moveLength;
}

/// Copy items from the node in the parameter into this node
/// \param destination_position Position on which the items are copied in this node
/// \param source_position Position where the items start in the source node
/// \param node Source node. The source node and this node can not be the same! It could lead to failure. Use MoveItems instead.
/// \param count Number of items which are copied from the source node into this node
template<class TKeyItem, class TLeafItem>
void cDomNode_Leaf<TKeyItem, TLeafItem>::CopyItems(unsigned int destination_position, unsigned int source_position, cDomNode_Leaf<TKeyItem, TLeafItem>* node, unsigned int count )
{
	cDomNode_Inner<TKeyItem, TLeafItem>::CopyItems(destination_position, source_position, node, count);

	TLeafItem::CopyBlock(&mLeaf[destination_position], node->GetLeaves(source_position), count, mHeader->GetLeafSizeInfo());
	memmove(&mNodeCount[destination_position], node->GetNodesCount(source_position), count * sizeof(unsigned int));
	for (unsigned int i = 0; i < count; i++ )
	{
		if (node->IsOptional(source_position + i))
		{
			SetOptional(destination_position + i);
		}
	}
}

/// Copy items from the node in the parameter into this node
/// \param node Source node. The source node and this node can not be the same! It could lead to failure. Use MoveItems instead.
/// \param isOptional If the parameter is true, all items are set as optional.
template<class TKeyItem, class TLeafItem>
void cDomNode_Leaf<TKeyItem, TLeafItem>::CopyItems(cDomNode_Leaf<TKeyItem, TLeafItem>* node, bool isOptional)
{
	cDomNode_Inner<TKeyItem, TLeafItem>::CopyItems(0, 0, node, node->GetItemCount());

	TLeafItem::CopyBlock(&mLeaf[0], node->GetLeaves(0), node->GetItemCount(), mHeader->GetLeafSizeInfo());
	memmove(&mNodeCount[0], node->GetNodesCount(0), node->GetItemCount() * sizeof(unsigned int));
	if (isOptional)
	{
		mIsOptional = 0xffffffff >> (32 - node->GetItemCount());
	} else
	{
		mIsOptional = node->GetOptional();
	}
}

/// Auxiliary method returning the size of the node
/// \return size of the node depending on a node type
template<class TKeyItem, class TLeafItem>
unsigned char cDomNode_Leaf<TKeyItem, TLeafItem>::GetNodeSize()
{
	return (unsigned char)mHeader->GetLeafNodeCapacity();
}

/// Method insert new item into this node.
/// \param position Position of the item where we put the new item.
/// \param leaf Leaf value of the inserted item.
/// \param key Key value of the inserted item.
/// \param pointer Pointer value of the inserted item.
/// \param isOptional If the parameter is true the inserted item is set as optional.
/// \param nodeCount Number of corresponding XML nodes
/// \return See the InsertReturnCode() method for retun codes
template<class TKeyItem, class TLeafItem>
int cDomNode_Leaf<TKeyItem, TLeafItem>::InsertLeafItem(unsigned int position, 
													const LeafType& leaf, 
													const KeyType& key, 
													unsigned int pointer, 
													//unsigned short descendantCount, 
													bool isOptional,
													unsigned int nodeCount)
{
	assert(mCount <= mHeader->GetLeafNodeCapacity());

	MoveItems(position, 1);
	TKeyItem::Copy(mKey[position], key);
	TLeafItem::Copy(mLeaf[position], leaf);
	mPointers[position] = pointer;
	//mDescendantCount[position] = descendantCount;
	if (isOptional)
	{
		SetOptional(position);
	}
	mNodeCount[position] = nodeCount;

	return InsertReturnCode(position);
}


/// Before spliting method first check if some items can not be moved to the next node.
/// If not, three nodes are created from this node and the right node.
/// \param rightNode Node on the right side from this one. The node can be NULL if this node is the last.
/// \param newNode Newly created node. If no split is necessary, the node is left untouched.
/// \return
///		- true If the nodes was splited
///		- false Otherwise
template<class TKeyItem, class TLeafItem>
bool cDomNode_Leaf<TKeyItem, TLeafItem>::SplitNode(cDomNode_Leaf<TKeyItem, TLeafItem>* rightNode, cDomNode_Leaf<TKeyItem, TLeafItem>* newNode)
{
	if (rightNode == NULL)
	{
		// in the case that this node does not have the right sibling node this node is splited on two halves
		unsigned char newSize = mCount / 2;
		newNode->SetLevel(mLevel);
		newNode->CopyItems(0, newSize, this, mCount - newSize);
		newNode->SetItemCount(mCount - newSize);
		mCount = newSize;
		return true;
	} else
	{
		assert(mLevel == rightNode->GetLevel());

		if (mCount + rightNode->GetItemCount() < 2 * GetNodeSize())
		{
			// only some items are shifted to the rightNode
			unsigned char newSize = (mCount + rightNode->GetItemCount()) / 2;
			rightNode->MoveItems(0, mCount - newSize);
			rightNode->CopyItems(0, newSize, this, mCount - newSize);
			mCount = newSize;
			return false;
		} else
		{
			// split two nodes into three
			unsigned char aux, newSize = (mCount + rightNode->GetItemCount()) / 3;
			newNode->SetLevel(mLevel);
			newNode->CopyItems(0, rightNode->GetItemCount() - newSize, rightNode, newSize);
			newNode->SetItemCount(newSize);
			rightNode->SetItemCount(rightNode->GetItemCount() - newSize);

			aux = rightNode->GetItemCount();
			rightNode->MoveItems(0, newSize - aux);
			rightNode->CopyItems(0, mCount - (newSize - aux), this, newSize - aux);
			mCount = mCount - (newSize - aux);
			return true;
		}
	}
}

/// Print the node
template<class TKeyItem, class TLeafItem>
void cDomNode_Leaf<TKeyItem, TLeafItem>::Print(char *str, unsigned int index) const
{
	char aux1[64], aux2[64];
	printf("Leaf node | index: %d | count:%d\nKey\tPoint\tLeaf(corresponding XML nodes count)\n", index, mCount);
	for (unsigned int i = 0; i < mCount; i++)
	{
		printf("%s\t%d\t%s (%d,", TKeyItem::ToString(aux1, mKey[i]), mPointers[i], TLeafItem::ToString(aux2, mLeaf[i]), mNodeCount[i], str);
		if (IsOptional(i))
		{
			printf(" true)%s", str);
		} else
		{
			printf(" false)%s", str);
		}
	}
}

#endif