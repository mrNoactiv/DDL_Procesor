/**
*	\file cDomCache.h
*	\author Radim Baca
*	\version 0.1
*	\date dec 2008
*	\brief Cache of the DOM tree.
*/


#ifndef __cDomCache_h__
#define __cDomCache_h__

#include "dstruct/mmemory/dom/cDomHeader.h"
#include "dstruct/mmemory/dom/cDomNode_Inner.h"
#include "dstruct/mmemory/dom/cDomNode_Leaf.h"

/**
* Cache of the DOM tree.
* Template parameters:
*	- TKeyItem - Have to be inherited from cBasicType. Represent type of the key value.
*	- TLeafItem - Have to be inherited from cBasicType. Represent type of the leaf value.
*
*	\author Radim Baca
*	\version 0.1
*	\date dec 2008
**/
template<class TKeyItem, class TLeafItem>
class cDomCache
{
	cDomHeader<TKeyItem, TLeafItem>*		mHeader;
	cDomNode_Inner<TKeyItem, TLeafItem>**	mNodeCache_Inner;		/// Array of inner nodes.
	cDomNode_Leaf<TKeyItem, TLeafItem>**	mNodeCache_Leaf;		/// Array of leaf nodes.

	static const unsigned int MAXIMAL_NODE_COUNT = 25000000;
public:
	cDomCache(cDomHeader<TKeyItem, TLeafItem>* header, unsigned int startNodeCount);
	~cDomCache();

	void Init(cDomHeader<TKeyItem, TLeafItem>* header, unsigned int startNodeCount);
	void Delete();

	cDomNode_Inner<TKeyItem, TLeafItem>* CreateInnerNode(unsigned char level = 0);
	cDomNode_Leaf<TKeyItem, TLeafItem>* CreateLeafNode(unsigned char level = 0);

	cDomNode_Inner<TKeyItem, TLeafItem>* GetInnerNode(unsigned int pointer)	{ return mNodeCache_Inner[pointer]; }
	cDomNode_Leaf<TKeyItem, TLeafItem>* GetLeafNode(unsigned int pointer)	{ return mNodeCache_Leaf[pointer]; }
};

template<class TKeyItem, class TLeafItem>
cDomCache<TKeyItem,TLeafItem>::cDomCache(cDomHeader<TKeyItem, TLeafItem>* header, unsigned int startNodeCount)
	:mNodeCache_Inner(NULL), mNodeCache_Leaf(NULL),
	mHeader(NULL)
{
	Init(header, startNodeCount);
}

template<class TKeyItem, class TLeafItem>
cDomCache<TKeyItem,TLeafItem>::~cDomCache()
{
	Delete();
}

template<class TKeyItem, class TLeafItem>
void cDomCache<TKeyItem,TLeafItem>::Delete()
{
	if (mNodeCache_Inner != NULL)
	{
		delete mNodeCache_Inner;
		mNodeCache_Inner = NULL;
	}
	if (mNodeCache_Leaf != NULL)
	{
		delete mNodeCache_Leaf;
		mNodeCache_Leaf = NULL;
	}
}

template<class TKeyItem, class TLeafItem>
void cDomCache<TKeyItem,TLeafItem>
	::Init(cDomHeader<TKeyItem, TLeafItem>* header, unsigned int startNodeCount)
{
	Delete();

	mHeader = header;
	mNodeCache_Inner = new cDomNode_Inner<TKeyItem, TLeafItem>*[startNodeCount / 4];
	mNodeCache_Leaf = new cDomNode_Leaf<TKeyItem, TLeafItem>*[startNodeCount];

}

/**
* Create new inner node.
* \param level Level of a new inner node.
* \return New inner dom node.
*/
template<class TKeyItem, class TLeafItem>
cDomNode_Inner<TKeyItem, TLeafItem>* cDomCache<TKeyItem,TLeafItem>
	::CreateInnerNode(unsigned char level)
{
	cDomNode_Inner<TKeyItem, TLeafItem>* newInnerNode;

	if (mHeader->GetTopInnerNodeIndex() == mHeader->GetMaximalAllocatedInnerNodeIndex())
	{
		newInnerNode = (cDomNode_Inner< TKeyItem, TLeafItem>*)mHeader->GetMemory()->GetMemory(sizeof(cDomNode_Inner< TKeyItem, TLeafItem>));
		newInnerNode->Init(mHeader, level);
		unsigned int newNodeIndex = mHeader->GetNextInnerNodeIndex();
		if (newNodeIndex >= mHeader->GetInnerArraySize())
		{
			if (mHeader->GetInnerArraySize() == MAXIMAL_NODE_COUNT)
			{
				printf("cDomCache::CreateInnerNode - Index exceeded the maximal number of nodes in the main memory!\n");
				exit(1);
			} 
			unsigned int newNodeSize = mHeader->GetInnerArraySize() * 2;
			if (newNodeSize > MAXIMAL_NODE_COUNT)
			{
				newNodeSize = MAXIMAL_NODE_COUNT;
			}
			cDomNode_Inner<TKeyItem, TLeafItem>** newNodeArray = new cDomNode_Inner<TKeyItem, TLeafItem>*[newNodeSize];
			for (unsigned int i = 0; i < mHeader->GetInnerArraySize(); i++)
			{
				newNodeArray[i] = mNodeCache_Inner[i];
			}
			delete mNodeCache_Inner;
			mNodeCache_Inner = newNodeArray;
			mHeader->SetInnerArraySize(newNodeSize);
		} else
		{
			//if ((newNodeIndex % 10000) == 0) printf("New node: %d\n", mNewNodeIndex);
		}
		mNodeCache_Inner[newNodeIndex] = newInnerNode;
	} else
	{
		unsigned int newNodeIndex = mHeader->GetNextInnerNodeIndex();
		newInnerNode = mNodeCache_Inner[newNodeIndex];
		newInnerNode->Clear();
	}

	return newInnerNode;
}

/**
* Create new leaf node.
* \param level Level of a new leaf node.
* \return New leaf dom node.
*/
template<class TKeyItem, class TLeafItem>
cDomNode_Leaf<TKeyItem, TLeafItem>* cDomCache<TKeyItem,TLeafItem>
	::CreateLeafNode(unsigned char level)
{
	cDomNode_Leaf<TKeyItem, TLeafItem>* newLeafNode;
	if (mHeader->GetTopLeafNodeIndex() == mHeader->GetMaximalAllocatedLeafNodeIndex())
	{
		newLeafNode = (cDomNode_Leaf< TKeyItem, TLeafItem>*)mHeader->GetMemory()->GetMemory(sizeof(cDomNode_Leaf< TKeyItem, TLeafItem>));
		newLeafNode->Init(mHeader, level);
		unsigned int newNodeIndex = mHeader->GetNextLeafNodeIndex();
		if (newNodeIndex >= mHeader->GetLeafArraySize())
		{
			if (mHeader->GetLeafArraySize() == MAXIMAL_NODE_COUNT)
			{
				printf("cDomCache::CreateLeafNode - Index exceeded the maximal number of nodes in the main memory!\n");
				exit(1);
			} 
			unsigned int newNodeSize = mHeader->GetLeafArraySize() * 2;
			if (newNodeSize > MAXIMAL_NODE_COUNT)
			{
				newNodeSize = MAXIMAL_NODE_COUNT;
			}
			cDomNode_Leaf<TKeyItem, TLeafItem>** newNodeArray = new cDomNode_Leaf<TKeyItem, TLeafItem>*[newNodeSize];
			for (unsigned int i = 0; i < mHeader->GetLeafArraySize(); i++)
			{
				newNodeArray[i] = mNodeCache_Leaf[i];
			}
			delete mNodeCache_Leaf;
			mNodeCache_Leaf = newNodeArray;
			mHeader->SetLeafArraySize(newNodeSize);
		} else
		{
			//if ((newNodeIndex % 10000) == 0) printf("New node: %d\n", mNewNodeIndex);
		}
		mNodeCache_Leaf[newNodeIndex] = newLeafNode;
	} else
	{
		unsigned int newNodeIndex = mHeader->GetNextLeafNodeIndex();
		newLeafNode = mNodeCache_Leaf[newNodeIndex];
		newLeafNode->Clear();
	}
	return newLeafNode;
}

#endif