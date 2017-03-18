/**
*	\file cDomNode_Inner.h
*	\author Radim Baca
*	\version 0.1
*	\date dec 2008
*	\brief Inner node of the sub-tree.
*/


#ifndef __cDomNode_Inner_h__
#define __cDomNode_Inner_h__

#include "common/stream/cStream.h"
#include "dstruct/mmemory/dom/cDomHeader.h"

/**
* Inner node of the sub-tree. This node contains only key values and pointers to child nodes.
* Template parameters:
*	- TKeyItem - Have to be inherited from cBasicType. Represent type of the key value.
*	- TLeafItem - Have to be inherited from cBasicType. Represent type of the leaf value (not declared in this class).
*
* Level of the root node is 1.
*
*	\author Radim Baca
*	\version 0.1
*	\date dec 2008
**/
template<class TKeyItem, class TLeafItem>
class cDomNode_Inner
{
protected:
	typedef typename TKeyItem::Type KeyType;
	typedef typename TLeafItem::Type LeafType;
	
	KeyType *mKey;
	unsigned int *mPointers;

	cDomHeader<TKeyItem, TLeafItem> *mHeader;	
	//char mType;
	unsigned char mLevel;
	unsigned char mCount;

	inline int InsertReturnCode(unsigned int position);
	inline unsigned char GetNodeSize();
	inline void Resize(cMemory *memory);
public:

	// These constants are duplicated in the cDomTree!!! In the case of any update, check also these constants.
	static const int INSERT_OK = 0;
	static const int INSERT_OVERFULL = 1;
	static const int INSERT_FIRSTCHANGED = 2;

	cDomNode_Inner(cDomHeader<TKeyItem, TLeafItem> *header, unsigned char level);
	~cDomNode_Inner();

	inline void Init(cDomHeader<TKeyItem, TLeafItem> *header, unsigned char level);
	void Clear();

	bool SearchItem(const KeyType& key, int& position);
	inline void DeleteItem(unsigned int position);
	inline void MoveItems(unsigned int position, unsigned int moveLength);
	inline void CopyItems(unsigned int destination_position, unsigned int source_position, cDomNode_Inner<TKeyItem, TLeafItem>* node, unsigned int count );
	int InsertPointerItem(unsigned int position, const KeyType& key, unsigned int pointer);
	bool SplitNode(cDomNode_Inner<TKeyItem, TLeafItem>* rightNode, cDomNode_Inner<TKeyItem, TLeafItem>* nextNode);

	// Get, Set
	inline const KeyType* GetKeys(unsigned int order) const		{ return &mKey[order]; }
	inline const unsigned int* GetPointers(unsigned int order) const { return &mPointers[order]; }
	inline const KeyType& GetKey(unsigned int order) const		{ return mKey[order]; }
	inline KeyType* GetRefKey(unsigned int order) const			{ return &mKey[order]; }
	inline unsigned int GetPointer(unsigned int order) const	{ return mPointers[order]; }
	inline unsigned char GetLevel() const						{ return mLevel; }
	inline unsigned char GetItemCount() const					{ return mCount; }

	inline void SetKey(unsigned int order, const KeyType& key)	{ mKey[order] = key; }
	inline void SetPointer(unsigned int order, unsigned int pointer)	{ mPointers[order] = pointer; }
	inline void SetLevel(unsigned char level)					{ mLevel = level; }
	inline void SetItemCount(unsigned char count)				{ mCount = count; }

	void Print(char *str, unsigned int index) const;

};

/**
* Constructor
*/
template<class TKeyItem, class TLeafItem>
cDomNode_Inner<TKeyItem, TLeafItem>
	::cDomNode_Inner(cDomHeader<TKeyItem, TLeafItem> *header, unsigned char level)
	:mHeader(header),
	mLevel(level)
{
	Init(header, level);
}

template<class TKeyItem, class TLeafItem>
cDomNode_Inner<TKeyItem, TLeafItem>::~cDomNode_Inner()
{
}

/// Has the same purpose as the constructor. In some cases, when the cMemory is used for a object creation this method
/// has to be called, to set all necessary attributes in the class
/// \param header header of the tree
/// \param level level of the DataGuide which the node cover
template<class TKeyItem, class TLeafItem>
void cDomNode_Inner<TKeyItem, TLeafItem>::Init(cDomHeader<TKeyItem, TLeafItem> *header, unsigned char level)
{
	mHeader = header;
	mLevel = level;
	mCount = 0;

	Resize(mHeader->GetMemory());
}


/**
* Resize the leaf node
*/
template<class TKeyItem, class TLeafItem>
void cDomNode_Inner<TKeyItem, TLeafItem>::Resize(cMemory *memory)
{
	mKey = (KeyType*)memory->GetMemory((mHeader->GetInnerNodeCapacity() + 1) * sizeof(KeyType));
	for (unsigned int i = 0; i < mHeader->GetInnerNodeCapacity() + 1; i++)
	{
		TKeyItem::Resize((cSizeInfo<KeyType>&)mHeader->GetKeySizeInfo(), memory, mKey[i]);
	}
	mPointers = (unsigned int*)memory->GetMemory((mHeader->GetInnerNodeCapacity() + 1) * sizeof(unsigned int));
}

/// Reset the number of items in the node
template<class TKeyItem, class TLeafItem>
void cDomNode_Inner<TKeyItem, TLeafItem>::Clear()
{
	mCount = 0;
}


/**
* Delete one item on a specified position from this node.
* \param position Position of the item which should be removed from this node.
*/
template<class TKeyItem, class TLeafItem>
void cDomNode_Inner<TKeyItem, TLeafItem>
	::DeleteItem(unsigned int position)
{
	if (position < mCount - 1)
	{
		TKeyItem::MoveBlock(&mKey[position], &mKey[position + 1], mCount - position - 1, mHeader->GetKeySizeInfo());
		memmove(&mPointers[position], &mPointers[position + 1], (mCount - position - 1) * sizeof(unsigned int));
	}
	mCount--;
}

/// Create space in the array of items for new items. It also increase the mCount of the node.
/// \param position position where the space is created
/// \param moveLength size of the space
template<class TKeyItem, class TLeafItem>
void cDomNode_Inner<TKeyItem, TLeafItem>::MoveItems(unsigned int position, unsigned int moveLength)
{
	if (position < mCount)
	{
		TKeyItem::MoveBlock(&mKey[position + moveLength], &mKey[position], mCount - position, mHeader->GetKeySizeInfo());
		memmove(&mPointers[position + moveLength], &mPointers[position], (mCount - position) * sizeof(unsigned int));
	}
	mCount = mCount + (unsigned char)moveLength;
}

/// Copy items from the node in the parameter into this node
/// \param destination_position Position on which the items are copied in this node
/// \param source_position Position where the items start in the source node
/// \param node Source node. The source node and this node can not be the same! It could lead to failure. Use MoveItems instead.
/// \param count Number of items which are copied from the source node into this node
template<class TKeyItem, class TLeafItem>
void cDomNode_Inner<TKeyItem, TLeafItem>::CopyItems(unsigned int destination_position, unsigned int source_position, cDomNode_Inner<TKeyItem, TLeafItem>* node, unsigned int count )
{
	TKeyItem::CopyBlock(&mKey[destination_position], node->GetKeys(source_position), count, mHeader->GetKeySizeInfo());
	memmove(&mPointers[destination_position], node->GetPointers(source_position), count * sizeof(unsigned int));
}

/// Auxiliary method returning the size of the node
/// \return size of the node depending on a node type
template<class TKeyItem, class TLeafItem>
unsigned char cDomNode_Inner<TKeyItem, TLeafItem>::GetNodeSize()
{
	return (unsigned char)mHeader->GetInnerNodeCapacity();
}

/// Auxiliary method returning the status of the insert.
/// \return
///		- INSERT_OK the insert was ok without any significant change.
///		- INSERT_OVERFULL the node is overfull.
///		- INSERT_FIRSTCHANGED first item in the node changed
template<class TKeyItem, class TLeafItem>
int cDomNode_Inner<TKeyItem, TLeafItem>::InsertReturnCode(unsigned int position)
{
	if (GetNodeSize() < mCount)
	{
		return INSERT_OVERFULL;
	} 
	if (position == 0)
	{
		return INSERT_FIRSTCHANGED;
	} else
	{
		return INSERT_OK;
	}
}

/// Method insert the pointer item into specified position and shift all items from this position
/// \param position the position of the item where we put the new item
/// \param key the key of the inserted item
/// \param pointer pointer of the inserted item
/// \return See the InsertReturnCode() method for retun codes
template<class TKeyItem, class TLeafItem>
int cDomNode_Inner<TKeyItem, TLeafItem>::InsertPointerItem(unsigned int position, const KeyType& key, unsigned int pointer)
{
	assert(mCount <= mHeader->GetInnerNodeCapacity());

	MoveItems(position, 1);
	TKeyItem::Copy(mKey[position], key);
	mPointers[position] = pointer;

	return InsertReturnCode(position);
}

/// Search for an item in the node
/// \param key We search for item with this specific key
/// \param position Method return position of the founded item, or the position of maximal lower item (-1 is returned when there is no lower item in the node)
/// \param 
///		- true if the method founded an item with the parameter 'key'
///		- false if such an item was not found
template<class TKeyItem, class TLeafItem>
bool cDomNode_Inner<TKeyItem, TLeafItem>::SearchItem(const KeyType& key, int& position)
{
	int lo = 0, mid = 0, hi = mCount - 1;
	int result;

	if (mCount == 0)
	{
		position = -1;
		return false;
	}
	do
	{
		mid = (lo + hi) / 2;

		if ((result = TKeyItem::Compare(mKey[mid], key)) > 0)
		{
			hi = mid-1;
		}
		else
		{
			if (result == 0)
			{
				break;
			}
			lo = mid+1;
		}
	}
	while(lo <= hi);

	position = mid;
	if ((result = TKeyItem::Compare(mKey[mid], key)) == 0)
	{		
		return true;
	}
	if (result == 1)
	{
		position--;		
	}
	return false;
}

/// Before spliting method first check if some items can not be moved to the next node.
/// If not, three nodes are created from this node and the right node.
/// \param rightNode Node on the right side from this one. The node can be NULL if this node is the last.
/// \param newNode Newly created node. If no split is necessary, the node is left untouched.
/// \return
///		- true If the nodes was splited
///		- false Otherwise
template<class TKeyItem, class TLeafItem>
bool cDomNode_Inner<TKeyItem, TLeafItem>::SplitNode(cDomNode_Inner<TKeyItem, TLeafItem>* rightNode, cDomNode_Inner<TKeyItem, TLeafItem>* newNode)
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
void cDomNode_Inner<TKeyItem, TLeafItem>::Print(char *str, unsigned int index) const
{
	char aux1[64];
	printf("Inner node | index: %d | count:%d\nKey\tPoint\n", index, mCount);
	for (unsigned int i = 0; i < mCount; i++)
	{
		printf("%s\t%d%s", TKeyItem::ToString(aux1, mKey[i]), mPointers[i] , str);
	}
}

#endif