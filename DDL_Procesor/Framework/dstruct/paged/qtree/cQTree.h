/*
	File:		cQTree.h
	Author:		Tomas Plinta, pli040
	Version:	0.1
	Date:		2011
	Brief implementation of QuadTree
*/

#ifndef __cQTree_h__
#define __cQTree_h__

#include "dstruct/paged/core/cQuickDB.h"
#include "dstruct/paged/core/cPagedTree.h"
#include "dstruct/paged/qtree/cQTreeNode.h"
#include "common/datatype/tuple/cMBRectangle.h"

#include "dstruct/paged/qtree/cQTreeHeader.h"
#include "common/cBitStringNew.h"
#include "dstruct/paged/core/cMemoryPool.h"
#include "math.h"

using namespace dstruct::paged::core;
using namespace common::datatype::tuple;
using namespace common;

namespace dstruct {
	namespace paged {
		namespace qtree {

typedef cQTreeNode<cTuple> TNode;

class Collection
{
  public: 
	typedef enum Collect { XML = 0, METEO = 1, CARS = 2, WORDS = 3, ELECTRICITY = 4};
};

template<class TKey>
class cQTree: public cPagedTree<TKey, TNode, TNode>, public cQTreeHeader<TKey>
{
private:
	TNode* ReadNewLeafNode();

public:
	static const uint DEBUG_OFF = 0;
	static const uint DEBUG_INDIVIDUALLY = 1;
	static const uint DEBUG_BULK = 2;
	static const uint NODE_INSERTED = 0;
	static const uint NODE_ALREADY_INSERTED = 1;
	static const uint NODE_NOT_INSERTED = 2;
	static const uint MAX_HEIGHT = 10240;
	static const uint DIMENSION = 2;

	static const uint STACK_NODE_ID = 0;
	static const uint STACK_NEXT_QUADRANT_TO_EXAMINE = 1;
	static const uint STACK_LEVEL_ZERO = 0;

	static const int DEBUG = DEBUG_BULK;
	static const int DEBUG_RANGE_QUERY = DEBUG_BULK;

	cQTree();
	~cQTree();

	bool Open(cQTreeHeader<TKey> *header, cQuickDB* quickDB, bool readOnly);
	bool Create(cQTreeHeader<TKey> *header, cQuickDB* quickDB);
	bool Close();
	uint Insert(TKey &item, char* data);
	bool Find(TKey &item, char* data);
	cItemStream* RangeQuery(TKey &leftUpperSearchedItem, TKey &rightLowerSearchedItem);

	bool IsInSpace(TKey &insertedItem);
	bool IsInNode(TNode *currentNode,TKey &insertedItem);
	uint ComputeQuadrant(TNode *currentNode, TKey &insertedItem);

};


template<class TKey>
cQTree<TKey>::cQTree()
{

}

template<class TKey>
cQTree<TKey>::~cQTree()
{

}

template<class TKey>
bool cQTree<TKey>::Open(cQTreeHeader<TKey> *header, cQuickDB* quickDB, bool readOnly)
{
	if (!cPagedTree<TKey,TNode,TNode>::Open(header, quickDB, readOnly))
	{
		return false;
	}

	cRTreeHeader<TKey> **p = &header;
	mSharedCache->CreateHeadersForRows(*p);

	return true;
}

/**
 * Create Empty Q-Tree
 **/
template<class TKey>
bool cQTree<TKey>::Create(cQTreeHeader<TKey> *header, cQuickDB* quickDB)
{
	if (!cPagedTree<TKey,TNode,TNode>::Create(header, quickDB))
	{
		return false;
	}

	cQTreeHeader<TKey> **p = &header;
	mSharedCache->CreateHeadersForRows(*p);

	// create the root node with one leaf node
	TNode* node = ReadNewLeafNode();

	mHeader->SetRootIndex(node->GetIndex());
	mHeader->SetLeafNodeCount(1);
	mSharedCache->UnlockW(node);
	mHeader->SetHeight(0);

	return true;
}

template<class TKey>
bool cQTree<TKey>::Close()
{
	if (!cPagedTree<TKey,TNode,TNode>::Close())
	{
		return false;
	}
	return true;
}
//this method checks, if the inserted node lies in underlying space
template<class TKey>
bool cQTree<TKey>::IsInSpace(TKey &insertedItem)
{
	uint dimension = sd->GetDimension();

	for(int i = 0; i < dimension; i++)
	{
		if(!(0 < TKey::GetUInt(insertedItem.GetData(), i, GetSpaceDescriptor()) < UINT_MAX))
		{
			return false;
		}
	}
	return true;
}

//This method returns true, if the coordinates of current node and inserted node are the same, false if are not the same
template<class TKey>
bool cQTree<TKey>::IsInNode(TNode *currentNode,TKey &insertedItem)
{
	cSpaceDescriptor* spaceDesc = (cSpaceDescriptor*)mHeader->GetNodeHeader(cQTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor();
	uint dimension = sd->GetDimension();

	for(int i = 0; i < dimension; i++)
	{
		if ((TKey::Equal(currentNode->GetItem(0),insertedItem.GetData(),i,spaceDesc) != 0))
		{
			return false;
		}
	}
	return true;
}

//This method returns number of quadrant, where is inserted item located
//0 for NW quadrant, 1 for NE quadrant, 2 for SW quadrant, 3 for SE quadrant for 2-dimension space
template<class TKey>
uint cQTree<TKey>::ComputeQuadrant(TNode *currentNode, TKey &insertedTuple)
{
	uint dimension = sd->GetDimension();
	cSpaceDescriptor* sd = (cSpaceDescriptor*)mHeader->GetNodeHeader(cQTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor();
	unsigned int quadrantOrder = 0;
	char* quadrantOrderMem = (char*)&quadrantOrder;

	if(cQTree::DEBUG == cQTree::DEBUG_INDIVIDUALLY)
	{
		cout << "COMPUTE>Current node: ";
		currentNode->Print();
	}

	uint i = 0;
	for (; i < dimension; i++)
	{
		bool bit = true;
		if (TKey::Equal(insertedTuple.GetData(), currentNode->GetItem(0), i, sd) < 0)
		{
			bit = false;
		} 
		cBitStringNew::SetBit(quadrantOrderMem, i, bit);
	}
	return quadrantOrder;
}

/************************************************************************/
/*This method returns unsigned integer, which represents if the item was correctly inserted:
/*	0 - item was correctly inserted
/*	1 - item was already in the tree - duplicate
/*	2 - insert error
/************************************************************************/

template<class TKey>
uint cQTree<TKey>::Insert(TKey &item, char* data)
{
	uint insertFlag = cQTree::NODE_NOT_INSERTED;

	if (mReadOnly)
	{
		printf("Critical Error: cQTree::Insert(), The tree is read only!\n");
		exit(1);
	}

	uint nextQuadrant = 0;	
	tNodeIndex nodeIndex = mHeader->GetRootIndex();		//first node in the tree
	TNode *currentNode = NULL;	//pointer, which represents current node of the tree

	if (!IsInSpace(item))
	{
		return insertFlag;
	}

	while(insertFlag == cQTree::NODE_NOT_INSERTED)
	{
		currentNode = ReadLeafNodeW(nodeIndex);

		if(cQTree::DEBUG == cQTree::DEBUG_INDIVIDUALLY)
		{
			cout << "INSERT>Current node: ";
			currentNode->Print();
		}

		if (currentNode->GetItemCount() == 0)	//if the node is empty (there is no item), then create new node with inserted item
		{
			currentNode->Insert(item, data, false);
			insertFlag = cQTree::NODE_INSERTED;
		}
		else if (IsInNode(currentNode, item))	//inserted item is the same like item in currently processed node = data are already located in the tree
		{
			insertFlag = cQTree::NODE_ALREADY_INSERTED;
		}
		else //inserted item is different than current item => compute next quadrant
		{
			nextQuadrant = ComputeQuadrant(currentNode,item);
			nodeIndex = currentNode->GetLink(nextQuadrant);
		}

		// Is the link empty? Create new node and insert the item.
		if (insertFlag == cQTree::NODE_NOT_INSERTED && nodeIndex == cNode::EMPTY_INDEX)
		{
			TNode *newNode = ReadNewLeafNode();
			newNode->Insert(item, data, false);
			currentNode->SetLink(nextQuadrant, newNode->GetIndex());
			mSharedCache->UnlockW(newNode);
			insertFlag = cQTree::NODE_INSERTED;
		}
		mSharedCache->UnlockW(currentNode);
	}
	return insertFlag;
}

//This method returns true, if the item was found
template<class TKey>
bool cQTree<TKey>::Find(TKey &item, char* data)
{
	bool IsNodeFound = false;
	uint nextQuadrant = 0;	
	tNodeIndex nodeIndex = mHeader->GetRootIndex();	//first node in the tree (root node)
	TNode *currentNode = NULL;	//pointer, which represents current node of the tree

	if (!IsInSpace(item))
	{
		return IsNodeFound;
	}

	while(!IsNodeFound)
	{
		currentNode = ReadLeafNodeW(nodeIndex);

		if(cQTree::DEBUG == cQTree::DEBUG_INDIVIDUALLY)
		{
			cout << "FIND>Current node: ";
			currentNode->Print();
		}

		if (currentNode->GetItemCount() != 0)
		{
			if (IsInNode(currentNode, item))	//inserted item is the same like item in currently processed node = searched data are located in the tree
			{
				IsNodeFound = true;
			}
			else //searched item is different than current item => compute next quadrant
			{
				nextQuadrant = ComputeQuadrant(currentNode,item);
				nodeIndex = currentNode->GetLink(nextQuadrant);
			}
		}

		mSharedCache->UnlockW(currentNode);

		if (currentNode->GetItemCount() == 0 | nodeIndex == cNode::EMPTY_INDEX)
		{
			break;
		}	
	}

	return IsNodeFound;
}

template<class TKey>
TNode* cQTree<TKey>::ReadNewLeafNode()
{
	TNode *newNode = cPagedTree<TKey,TNode,TNode>::ReadNewLeafNode(0);
	newNode->SetItemCount(0);
	newNode->ClearLinks();
	return newNode;
}

template<class TKey>
cItemStream* cQTree<TKey>::RangeQuery(TKey &leftUpperSearchedItem, TKey &rightLowerSearchedItem)
{
	cItemStream* resultSet = mQuickDB->GetResultSet();
	resultSet->SetNodeHeader(mHeader->GetNodeHeader(0));

	uint counter = 0;
	uint nextQuadrantToExamine = 0;
	uint currentLevel = 0;
	uint dimension = sd->GetDimension();
	const uint stackSize = 2 + (dimension * 2);
	const uint noQuadrant = (uint)pow((double)2,(int)dimension);

	cMemoryPool *pool = mHeader->GetNodeHeader(0)->GetMemoryPool();		
    char* spaceLeftUpper = pool->GetMem(mHeader->GetKeySize());		
	char* spaceRightLower = pool->GetMem(mHeader->GetKeySize());
	char* quadrantLeftUpper = pool->GetMem(mHeader->GetKeySize());
	char* quadrantRightLower = pool->GetMem(mHeader->GetKeySize());

	tNodeIndex nodeIndex = mHeader->GetRootIndex();
	TNode *currentNode = NULL;
	tNodeIndex parentLinksStack[MAX_HEIGHT];
	cBitString bitString = cBitString(dimension);

	uint **stack;			//stack is two dimensional array, where are stored following items: node id, next searched quadrant and coordinate of current space
	stack = new uint*[MAX_HEIGHT];
	for(int i = 0; i < MAX_HEIGHT; i++)
	{
		stack[i] = new uint[stackSize];
	}

	if (!IsInSpace(leftUpperSearchedItem) || !IsInSpace(rightLowerSearchedItem))
	{
		printf("Critical Error: Some of the coordinates of RangeQuery is not in space!\n");
		return resultSet;
	}

	for(int i = 0; i < MAX_HEIGHT; i++)		//initialization of stack, we must fill space coordinates of the tree in first line (STACK_LEVEL_ZERO), otherwise we must fill the stack with zeros
	{
		for(int y = 0; y < stackSize; y++)
		{
			if(i == STACK_LEVEL_ZERO)
			{
				if((y < (stackSize / 2)) && (y > 1))
				{
					stack[STACK_LEVEL_ZERO][y] = 0;		//set left upper coordinates of current space for level 0
				}
				if((y > (stackSize / 2)))
				{
					stack[STACK_LEVEL_ZERO][y] = UINT_MAX;	//set right lower coordinates of current space for level 0
				} else {
					stack[i][y] = 0;
				}
			} else {
				stack[i][y] = 0;
			}
		}
	}

	while(!(currentLevel == 0 && stack[STACK_LEVEL_ZERO][STACK_NEXT_QUADRANT_TO_EXAMINE] == noQuadrant))
	{
		currentNode = ReadLeafNodeW(nodeIndex);
		parentLinksStack[currentLevel] = nodeIndex;
																												
		for(int i = 0; i < dimension; i++)
		{
			TKey::SetValue(spaceLeftUpper, i, stack[currentLevel][(i + 2)], (cSpaceDescriptor*)mHeader->GetNodeHeader(cQTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor());
			TKey::SetValue(spaceRightLower, i, stack[currentLevel][(dimension + i + 2)], (cSpaceDescriptor*)mHeader->GetNodeHeader(cQTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor());
		}

		if(stack[currentLevel][STACK_NEXT_QUADRANT_TO_EXAMINE] != noQuadrant) //if there is no quadrant to examine
		{
			nextQuadrantToExamine = stack[currentLevel][STACK_NEXT_QUADRANT_TO_EXAMINE]; //what quadrant is compared next
			while (nextQuadrantToExamine != noQuadrant) 
			{
				for(int i = 0; i < dimension; i++)		//we must set current quadrant coordinates
				{
					bitString.SetInt(nextQuadrantToExamine);
					bool hod = bitString.GetBit(i);
					if(hod == false)
					{
						TKey::SetValue(quadrantLeftUpper, i, TKey::GetUInt(spaceLeftUpper, i, GetSpaceDescriptor()), (cSpaceDescriptor*)mHeader->GetNodeHeader(cQTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor());
						TKey::SetValue(quadrantRightLower, i, TKey::GetUInt(currentNode->GetItem(0), i, GetSpaceDescriptor()), (cSpaceDescriptor*)mHeader->GetNodeHeader(cQTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor());
					} else {					
						TKey::SetValue(quadrantLeftUpper, i, TKey::GetUInt(currentNode->GetItem(0), i, GetSpaceDescriptor()), (cSpaceDescriptor*)mHeader->GetNodeHeader(cQTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor());
						TKey::SetValue(quadrantRightLower, i, TKey::GetUInt(spaceRightLower, i, GetSpaceDescriptor()), (cSpaceDescriptor*)mHeader->GetNodeHeader(cQTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor());
					}
				}
				if(cMBRectangle<TKey>::IsIntersected(quadrantLeftUpper, quadrantRightLower, leftUpperSearchedItem.GetData(), rightLowerSearchedItem.GetData(), (cSpaceDescriptor*)mHeader->GetNodeHeader(cQTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor())) //if current quadrant and range query window are intersected
				{
					nodeIndex = currentNode->GetLink(nextQuadrantToExamine);
					if(nodeIndex != cNode::EMPTY_INDEX) //if there is child, we go deeper => we write size of actual space(where we go) into next row in stack
					{									//we delete coordinates of next quadrant						
						stack[currentLevel][STACK_NODE_ID] = currentNode->GetHeaderId();
						stack[currentLevel][STACK_NEXT_QUADRANT_TO_EXAMINE] = nextQuadrantToExamine + 1;
						currentLevel++;
						stack[currentLevel][STACK_NEXT_QUADRANT_TO_EXAMINE] = 0;

						for(int i = 0; i < dimension; i++) //write next quadrant's space coordinates
						{
							stack[currentLevel][(i + 2)] = TKey::GetUInt(quadrantLeftUpper, i, GetSpaceDescriptor());
							stack[currentLevel][(dimension + i + 2)] = TKey::GetUInt(quadrantRightLower, i, GetSpaceDescriptor());
						}
						mSharedCache->UnlockW(currentNode);
						break;
					} else
					{ //if there is no child
						nextQuadrantToExamine ++;
						stack[currentLevel][STACK_NEXT_QUADRANT_TO_EXAMINE] = nextQuadrantToExamine ;
					}
				} else	//is fulfilled, when quadrant, which is compared, is not in intersection with range query window
				{
					nextQuadrantToExamine ++;	
					stack[currentLevel][STACK_NEXT_QUADRANT_TO_EXAMINE] = nextQuadrantToExamine;
				}
			}
			if((nextQuadrantToExamine == noQuadrant))
			{	//if there is no child, we ask list node if it is in rectangle
				if(cMBRectangle<TKey>::IsInRectangle(rightLowerSearchedItem.GetData(), leftUpperSearchedItem.GetData(), currentNode->GetItem(0), (cSpaceDescriptor*)mHeader->GetNodeHeader(cQTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor())) //zjistit, zda aktualni uzel lezi v range query 
				{
					counter++;
					resultSet->Add(currentNode->GetItem(0));
					
				}
				if(currentLevel != 0) 
				{
					currentLevel--;
				}
				mSharedCache->UnlockW(currentNode);
				nodeIndex = parentLinksStack[currentLevel];
			}

		} else
		{	//if we went upwards and we immediately find, that there is no quadrant to compare, we go upwards again
			if(cMBRectangle<TKey>::IsInRectangle(rightLowerSearchedItem.GetData(), leftUpperSearchedItem.GetData(), currentNode->GetItem(0), (cSpaceDescriptor*)mHeader->GetNodeHeader(cQTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor())) //zjistit, zda aktualni uzel lezi v range query 
			{
				counter++;
				resultSet->Add(currentNode->GetItem(0));
			}
			if(currentLevel != 0) 
			{
				currentLevel--;
			}
			mSharedCache->UnlockW(currentNode);
			nodeIndex =  parentLinksStack[currentLevel];
		}
	}

	pool->FreeMem(spaceLeftUpper);
	pool->FreeMem(spaceRightLower);
	pool->FreeMem(quadrantLeftUpper);
	pool->FreeMem(quadrantRightLower);

	delete [] *stack;
    delete [] stack;

	//return counter;
	resultSet->FinishWrite();
	return resultSet;
}

}}}
#endif