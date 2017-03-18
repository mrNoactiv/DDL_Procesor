/**
*	\file cNodeRecordHash.h
*	\author Radim Baca
*	\version 0.1
*	\date oct 2010
*	\brief
*/

#ifndef __cHashTable_h__
#define __cHashTable_h__

#include "common/cCommon.h"
#include "common/datatype/cDTDescriptor.h"
#include "common/memdatstruct/cMemoryBlock.h"

#include <limits>
#include <assert.h>

using namespace common;
using namespace common::datatype;
using namespace common::memdatstruct;

/**
*
*
*	\author Radim Baca
*	\version 0.1
*	\date oct 2010
**/
namespace common {
	namespace memorystructures {

template<class TKey, class TData>
class cHashTableNode
{
public:
	typedef typename TKey::T TKeyT;
	typedef typename TData::T TDataT;

	cHashTableNode<TKey,TData> *Next;
	TKeyT Key;
	TDataT Data;

	cHashTableNode() { }
};

template<class TKey, class TData>
class cHashTable
{
	typedef typename TKey::T TKeyT;
	typedef typename TData::T TDataT;

	cHashTableNode<TKey, TData> **mHashTable;
	cHashTableNode<TKey, TData> *mHashTableNodes;
	cHashTableNode<TKey, TData> **mFreeNodes;
	unsigned int mFreeNodesPointer;

	unsigned int mItemCount;       // the size of the hash table
	unsigned int mCapacity;        // the capacity of the hash tables
	unsigned int mMaxStringLength; // the maximal length of a string of nodes to one hash value

	cDTDescriptor *mKeyDtDesc;
	cDTDescriptor *mDataDtDesc;
	cMemoryBlock* mMemBlock;

	inline unsigned int HashValue(const TKeyT &key) const;

public:
	static const unsigned int NOT_FOUND = UINT_MAX;

	cHashTable(unsigned int capacity, char* memory = NULL, const cDTDescriptor* mKeyDtDesc = NULL, const cDTDescriptor* mDataDtDesc = NULL);
	cHashTable(unsigned int capacity, cMemoryBlock *memBlock, const cDTDescriptor* mKeyDtDesc = NULL, const cDTDescriptor* mDataDtDesc = NULL);
	~cHashTable();
	static inline uint GetSize(unsigned int capacity, const cDTDescriptor* keyDtDesc = NULL, const cDTDescriptor* dataDtDesc = NULL);

	void Clear();

	bool Find(const TKey &key, TDataT& data) const;
	bool Add(const TKeyT &key, const TDataT &data);
	bool Delete(const TKeyT &key);
	inline cHashTableNode<TKey,TData>* GetNextNode(cHashTableNode<TKey,TData>* node = NULL);

	inline uint GetItemCount() const;
	inline cHashTableNode<TKey,TData>* GetNode(uint order) const;

	void PrintInfo() const;
};

template<class TKey, class TData>
cHashTable<TKey, TData>::cHashTable(unsigned int capacity, char* memory, const cDTDescriptor* keyDtDesc, const cDTDescriptor* dataDtDesc):
	mMemBlock(NULL), mHashTable(NULL), mFreeNodes(NULL), mHashTableNodes(NULL)
{
	mCapacity = capacity;

	char* memlo = memory;

	memory += sizeof(cHashTable<TKey, TData>);
	mHashTable = new (memory) cHashTableNode<TKey,TData>*[mCapacity];
	memory += mCapacity * sizeof(cHashTableNode<TKey,TData>*);

	mKeyDtDesc = (cDTDescriptor*)keyDtDesc;
	mDataDtDesc = (cDTDescriptor*)dataDtDesc;

	// try to create 2x more nodes than the size of the hash table
	mHashTableNodes = new (memory) cHashTableNode<TKey,TData>[mCapacity];
	memory += mCapacity * sizeof(cHashTableNode<TKey,TData>);

	mFreeNodes = new (memory) cHashTableNode<TKey,TData>*[mCapacity];
	memory += mCapacity * sizeof(cHashTableNode<TKey,TData>*);

	uint memSize = (memory - memlo);

	Clear();
}

template<class TKey, class TData>
cHashTable<TKey, TData>::cHashTable(unsigned int capacity, cMemoryBlock *memBlock, const cDTDescriptor* keyDtDesc, const cDTDescriptor* dataDtDesc):
	mHashTable(NULL), mFreeNodes(NULL), mHashTableNodes(NULL)
{
	mCapacity = capacity;
	mMemBlock = memBlock;

	mHashTable = new cHashTableNode<TKey,TData>*[mCapacity];
	mKeyDtDesc = (cDTDescriptor*)keyDtDesc;
	mDataDtDesc = (cDTDescriptor*)dataDtDesc;

	// try to create 2x more nodes than the size of the hash table
	mCapacity = capacity;
	mHashTableNodes = new cHashTableNode<TKey,TData>[mCapacity];
	mFreeNodes = new cHashTableNode<TKey,TData>*[mCapacity];

	Clear();
}

/**
 * Return the size of the hash table.
 */ 
template<class TKey, class TData>
inline uint cHashTable<TKey, TData>::GetSize(unsigned int capacity, const cDTDescriptor* keyDtDesc, const cDTDescriptor* dataDtDesc)
{
	uint size = 0;
	size += sizeof(cHashTable<TKey, TData>);
	size += sizeof(cHashTableNode<TKey,TData>*) * capacity;
	size += sizeof(cHashTableNode<TKey,TData>) * capacity;
	size += sizeof(cHashTableNode<TKey,TData>*) * capacity;

	// if key allocates any memory
	if (keyDtDesc != NULL)
	{
		// size += keyDtDesc->GetSize() * capacity;
	}

	// if data allocates any memory
	if (dataDtDesc != NULL)
	{
		// size += dataDtDesc->GetSize() * capacity;
	}
	return size;
}

template<class TKey, class TData>
cHashTable<TKey,TData>::~cHashTable()
{
	if (mHashTable != NULL)
	{
		delete []mHashTable;
		mHashTable = NULL;
	}
	if (mHashTableNodes != NULL)
	{
		if (mMemBlock != NULL)
		{
			for (unsigned int i = 0; i < mCapacity; i++)
			{
				TKey::Free(mHashTableNodes[i].Key, mMemBlock);
			}
		}

		delete []mHashTableNodes;
		mHashTableNodes = NULL;
	}
	if (mFreeNodes != NULL)
	{
		delete[]mFreeNodes;
		mFreeNodes = NULL;
	}
}

template<class TKey, class TData>
inline uint cHashTable<TKey,TData>::GetItemCount() const
{
	return mItemCount;
}

/**
 * Clear the hash table.
 */
template<class TKey, class TData>
void cHashTable<TKey,TData>::Clear()
{
	memset(mHashTable, NULL, mCapacity * sizeof(cHashTableNode<TKey,TData>*));  // empty the hash table

	// and return all nodes to the list of free nodes
	for (unsigned int i = 0; i < mCapacity; i++)
	{
		mFreeNodes[i] = &mHashTableNodes[i];
	}

	mFreeNodesPointer = mCapacity - 1; 
	mMaxStringLength = 0;
	mItemCount = 0;
}

template<class TKey, class TData>
bool cHashTable<TKey,TData>::Find(const TKey &key, TDataT &data) const
{
	unsigned int hashValue = HashValue((const TKeyT&)key);
	unsigned int ret = false;

	cHashTableNode<TKey,TData> *node = mHashTable[hashValue];

	while (node != NULL)
	{
		if (TKey::IsEqual(node->Key, (const TKeyT&)key, mKeyDtDesc))
		{
			TData::SetValue(data, node->Data, mDataDtDesc);
			ret = true;
			break;
		}
		node = node->Next;
	}
	return ret;
}

template<class TKey, class TData>
bool cHashTable<TKey, TData>::Add(const TKeyT &key, const TDataT &data)
{
	if (mFreeNodesPointer >= mCapacity)
	{
		return false;
	}

	uint stringLength = 1;
	unsigned int hashValue = HashValue(key);

	cHashTableNode<TKey, TData> **node = &(mHashTable[hashValue]);
	cHashTableNode<TKey, TData> *prev = NULL;

	// find the of line related to the hashValue
	while (*node != NULL)
	{
		prev = *node;
		node = &((*node)->Next);
		stringLength++;
	}

	if (stringLength > mMaxStringLength)
	{
		mMaxStringLength = stringLength;
	}

	// get a new node from the list of free nodes
	*node = mFreeNodes[mFreeNodesPointer--];
	cHashTableNode<TKey, TData> *n = *node;

	bool ret = TKey::ResizeSet(n->Key, key, mKeyDtDesc, mMemBlock);
	if (ret)
	{
		ret &= TData::ResizeSet(&(n->Data), data, mDataDtDesc, mMemBlock);
	}
	n->Next = NULL;

	if (!ret)
	{
		*node = NULL;
		mFreeNodesPointer++;
	}
	else
	{
		mItemCount++;
	}

	return ret;
}

template<class TKey, class TData>
bool cHashTable<TKey, TData>::Delete(const TKeyT &key)
{
	unsigned int hashValue = HashValue(key);
	cHashTableNode<TKey,TData> **pnode = &(mHashTable[hashValue]);
	bool ret = false;

	while (*pnode != NULL)
	{
		if (TKey::IsEqual((*pnode)->Key, (const TKeyT&)key, mKeyDtDesc))
		{
			// Key found? delete it
			cHashTableNode<TKey,TData> *deletedNode = *pnode;
			// the next pointer of the previous node pointers the next node
			*pnode = (*pnode)->Next;

			// and return the deleted node to free nodes
			mFreeNodes[++mFreeNodesPointer] = deletedNode;
			mItemCount--;

			ret = true;
			break;
		}
		pnode = &((*pnode)->Next);
	}
	return ret;
}

/**
 * A simple iterator throughout nonempty nodes.
 */
/*
template<class TKey,class TData>
inline cHashTableNode<TKey,TData>* cHashTable<TKey, TData>::GetNextNode(cHashTableNode<TKey,TData>* node)
{
	cHashTableNode<TKey,TData>* resultNode = NULL;
	if (node == NULL)
	{
		resultNode = mFreeNodes[mCapacity - 1];
	}
	else
	{
		resultNode = node->Next;
	}
	return resultNode;
}*/

/**
 * It is a kind of iterator - it iterates throughout all nodes used in the hash table. It returns NULL if the node does not exist.
 */
template<class TKey,class TData>
inline cHashTableNode<TKey,TData>* cHashTable<TKey, TData>::GetNode(uint order) const
{
	return mFreeNodes[mCapacity - 1 - order];
}

template<class TKey, class TData>
unsigned int cHashTable<TKey, TData>::HashValue(const TKeyT &key) const
{
	return TKey::HashValue(key, mCapacity, mKeyDtDesc);
}

template<class TKey, class TData>
void cHashTable<TKey, TData>::PrintInfo() const
{
	printf("\nHashTable Info: ItemCount: %u, Capacity: %u, MaxStringLength: %u\n.", mItemCount, mCapacity, mMaxStringLength);
}
}}
#endif