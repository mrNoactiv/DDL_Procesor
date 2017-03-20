/**
*	\file cNodeRecordHash.h
*	\author Radim Baca
*	\version 0.1
*	\date oct 2010
*	\brief
*/

#ifndef __cResultSetPhiPhi_h__
#define __cResultSetPhi_h__
#pragma offload_attribute(push, target(mic)
#include <limits>
#include <assert.h>

#include "common/cCommon.h"
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
class cResultSetPhiNode
{
public:
	cResultSetPhiNode<TKey,TData> *Next;
	TKey Key;
	TData Data;
};

template<class TKey, class TData>
class cResultSetPhi
{
	cResultSetPhiNode<TKey, TData> **mHashTable;
	cResultSetPhiNode<TKey, TData> *mHashTableNodes;
	cResultSetPhiNode<TKey, TData> **mFreeNodes;
	unsigned int mFreeNodesPointer;

	unsigned int mSize;      // the size of the hash table
	unsigned int mNodeCount; // the count of the nodes, the maximal number of Keys in the hash table is mNodeCount
	unsigned int mItemCount; // the number of keys in hashtable
	inline unsigned int HashValue(unsigned int value) const;

public:
	static const unsigned int NOT_FOUND = UINT_MAX;

	cResultSetPhi(unsigned int size);
	~cResultSetPhi();

	void Clear();

	bool Find(const TKey &key, TData &data) const;
	void Add(const TKey &key, const TData &data);
	bool Delete(const TKey &key);
	unsigned int Count();
	inline TKey& GetRefItem(unsigned int order) const;
	void Add(const TKey &key); //overloaded method. Does not set any data.
	void Init(char* mem, unsigned int size);
	static unsigned int ComputeHeapSize(unsigned int capacity);
};

template<class TKey, class TData>
cResultSetPhi<TKey, TData>::cResultSetPhi(unsigned int size)
{
	mHashTable = mFreeNodes = NULL;
	mHashTableNodes = NULL;
	mSize = size;
	mItemCount = 0;
	mHashTable = new cResultSetPhiNode<TKey, TData>*[mSize];
	memset(mHashTable, NULL, mSize * sizeof(cResultSetPhiNode<TKey, TData>*));  // empty the hash table

	// try to create 2x more nodes than the size of the hash table
	mNodeCount = 2 * mSize;
	mHashTableNodes = new cResultSetPhiNode<TKey,TData>[mNodeCount];
	mFreeNodes = new cResultSetPhiNode<TKey,TData>*[mNodeCount];
	for (unsigned int i = 0; i < mNodeCount; i++)
	{
		mFreeNodes[i] = &mHashTableNodes[i];
	}
	mFreeNodesPointer = mNodeCount - 1;
}

template<class TKey, class TData>
cResultSetPhi<TKey,TData>::~cResultSetPhi()
{
	if (mHashTable != NULL)
	{
		delete []mHashTable;
		mHashTable = NULL;
	}
	if (mHashTableNodes != NULL)
	{
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
void cResultSetPhi<TKey, TData>::Init(char* mem, unsigned int size)
{
	mHashTable = mFreeNodes = NULL;
	mHashTableNodes = NULL;
	mSize = size;
	mItemCount = 0;
	mHashTable = (cResultSetPhiNode<TKey, TData>**)mem;
	mem += sizeof(cResultSetPhiNode<TKey, TData>*);
	memset(mHashTable, NULL, mSize * sizeof(cResultSetPhiNode<TKey, TData>*));  // empty the hash table
	mem += size * sizeof(cResultSetPhiNode<TKey, TData>*);

	// try to create 2x more nodes than the size of the hash table
	mNodeCount = 2 * mSize;
	mHashTableNodes = (cResultSetPhiNode<TKey, TData>*)mem;
	mem += mNodeCount * sizeof(cResultSetPhiNode<TKey, TData>*);
	mFreeNodes = (cResultSetPhiNode<TKey, TData>**)mem;
	mem += mNodeCount * sizeof(cResultSetPhiNode<TKey, TData>*);
	for (unsigned int i = 0; i < mNodeCount; i++)
	{
		mFreeNodes[i] = &mHashTableNodes[i];
	}
	mFreeNodesPointer = mNodeCount - 1;

}
template<class TKey, class TData>
unsigned int cResultSetPhi<TKey, TData>::ComputeHeapSize(unsigned int capacity)
{
	uint size = 0;
	size += sizeof(cResultSetPhiNode<TKey, TData>) + capacity * sizeof(cResultSetPhiNode<TKey, TData>*); //mHashTable
	size += sizeof(cResultSetPhiNode<TKey, TData>) + 2 * capacity * sizeof(cResultSetPhiNode<TKey, TData>*); //mHashTableNodes
	size += sizeof(cResultSetPhiNode<TKey, TData>) + 2 * capacity * sizeof(cResultSetPhiNode<TKey, TData>*); //mFreeNodes
	return size;
}
/**
 * Clear the hash table.
 */
template<class TKey, class TData>
void cResultSetPhi<TKey,TData>::Clear()
{
	memset(mHashTable, NULL, mSize * sizeof(cResultSetPhiNode<TKey,TData>*));  // empty the hash table
	// and return all nodes to the list of free nodes
	for (unsigned int i = 0; i < mNodeCount; i++)
	{
		mFreeNodes[i] = &mHashTableNodes[i];
	}
	mFreeNodesPointer = mNodeCount - 1;
	mItemCount = 0;
}

template<class TKey, class TData>
bool cResultSetPhi<TKey,TData>::Find(const TKey &key, TData &data) const
{
	unsigned int hashValue = HashValue(key);
	unsigned int ret = false;

	cResultSetPhiNode<TKey,TData> *node = mHashTable[hashValue];

	while (node != NULL)
	{
		if (node->Key == key)
		{
			data = node->Data;
			ret = true;
			break;
		}
		node = node->Next;
	}
	return ret;
}

template<class TKey, class TData>
void cResultSetPhi<TKey, TData>::Add(const TKey &key, const TData &data)
{
	assert(mFreeNodesPointer != UINT_MAX);
	unsigned int hashValue = HashValue(key);
	cResultSetPhiNode<TKey, TData> **node = &(mHashTable[hashValue]);
	cResultSetPhiNode<TKey, TData> *prev = NULL;

	// find the of line related to the hashValue
	while (*node != NULL)
	{
		prev = *node;
		node = &((*node)->Next);
	}

	// get a new node from the list of free nodes
	*node = mFreeNodes[mFreeNodesPointer--];
	cResultSetPhiNode<TKey, TData> *n = *node;
	n->Key = key;
	n->Data = data;
	n->Next = NULL;
	mItemCount++;
}

template<class TKey, class TData>
bool cResultSetPhi<TKey, TData>::Delete(const TKey &Key)
{
	unsigned int hashValue = HashValue(Key);
	cResultSetPhiNode<TKey,TData> **pnode = &(mHashTable[hashValue]);
	bool ret = false;

	while (*pnode != NULL)
	{
		if ((*pnode)->Key == Key)
		{
			// Key found? delete it
			cResultSetPhiNode<TKey,TData> *deletedNode = *pnode;
			// the next pointer of the previous node pointers the next node
			*pnode = (*pnode)->Next;

			// and return the deleted node to free nodes
			mFreeNodes[++mFreeNodesPointer] = deletedNode;
			ret = true;
			break;
		}
		pnode = &((*pnode)->Next);
	}
	if (ret)
		mItemCount--;
	return ret;
}

template<class TKey, class TData>
unsigned int cResultSetPhi<TKey, TData>::HashValue(unsigned int value) const
{
	return value % mSize;
}

template<class TKey, class TData>
unsigned int cResultSetPhi<TKey, TData>::Count()
{
	return mItemCount;
}
/*
template<class TKey, class TData>
inline TKey& cResultSetPhi<TKey, TData>::GetRefItem(unsigned int order) const
{
	//return (&mHashTableNodes[order])->Data;
	return mFreeNodes[mFreeNodesPointer + (mItemCount - order)]->Key;
	//return mFreeNodes[order]->Key;
}
/*
Adds key into hash array if it is not in the hash array already.
*/
/*
template<class TKey, class TData>
void cResultSetPhi<TKey, TData>::Add(const TKey &key)
{
	assert(mFreeNodesPointer != UINT_MAX);
	unsigned int hashValue = HashValue(key);
	if (hashValue != (int)key)
		printf("\nWarning (cResultSetPhi.h): Key %d has probably same hash value as another key!", key);
	cResultSetPhiNode<TKey, TData> **node = &(mHashTable[hashValue]);
	cResultSetPhiNode<TKey, TData> *prev = NULL;

	// find the of line related to the hashValue
	if (*node != NULL) //key already present
	{
		return;
	}

	// get a new node from the list of free nodes
	*node = mFreeNodes[mFreeNodesPointer--];
	//*node = mFreeNodes[mItemCount++];
	cResultSetPhiNode<TKey, TData> *n = *node;
	n->Key = key;
	n->Data = NULL;
	n->Next = NULL;
	//mHashTableNodes[mItemCount++] = n;
	mItemCount++;
}
*/


template<class TKey, class TData>
inline TKey& cResultSetPhi<TKey, TData>::GetRefItem(unsigned int order) const
{
	//return mFreeNodes[mFreeNodesPointer + (mItemCount - order)]->Key;
	return mFreeNodes[order]->Key;
}
template<class TKey, class TData>
void cResultSetPhi<TKey, TData>::Add(const TKey &key)
{
	assert(mFreeNodesPointer != UINT_MAX);
	unsigned int hashValue = HashValue(key);
	if (hashValue != (int)key)
		printf("\nWarning (cResultSetPhi.h): Key %d has probably same hash value as another key!", key);
	cResultSetPhiNode<TKey, TData> **node = &(mHashTable[hashValue]);
	cResultSetPhiNode<TKey, TData> *prev = NULL;

	// find the of line related to the hashValue
	if (*node != NULL) //key already present
	{
		return;
	}

	// get a new node from the list of free nodes
	*node = mFreeNodes[mItemCount++];
	cResultSetPhiNode<TKey, TData> *n = *node;
	n->Key = key;
	n->Data = NULL;
	n->Next = NULL;
}
}}
#pragma offload_attribute(pop)
#endif
