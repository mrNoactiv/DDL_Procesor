/**
*	\file cPagedLinearHashTable.h
*	\author Petr Turecek 2015
*	\brief Linear hash table with order preserving
*	\version 1.0
*	\date jul 2015
*/

//	based on cPagedHashTable.h by Vaclav Snasel & Michal Krátký

#ifndef __cPagedLinearHashTable_h__
#define __cPagedLinearHashTable_h__

#include "dstruct/paged/core/cQuickDB.h"
#include "dstruct/paged/core/cNodeCache.h"
#include "dstruct/paged/hashtable/cPagedHashTableNode.h"
#include "dstruct/paged/core/cTreeItemStream.h"
#include "dstruct/paged/core/cQueryStatistics.h"
#include "common/cMemory.h"
#include "dstruct/paged/core/cMemoryPool.h"
#include "dstruct/paged/core/cDStructConst.h"
#include "dstruct/paged/core/cTreeNode.h"

#define MAX_FILE_NAME_LENGTH 100

namespace dstruct {
  namespace paged {
	namespace hashtable {

using namespace dstruct::paged::core;


template<class TKey>
class cPagedLinearHashTable : public cPagedStaticHashTable<TKey>
{
protected:
	tNodeIndex mSplit;		// S pointer - points to the next bucket to split (zero at begining)
	bool       mForceSplit; // forces split when set to true - for example when too long chaining found

	inline unsigned int GetNodeHeaderId() { return mNodeHeaderId; };

	inline int SplitNode();
	inline int RaiseLevel();	// Raises level of HT by doubling its size
	virtual inline tNodeIndex GetHashValue(const TKey &key);
	
	void Init();
	
public:
	cPagedLinearHashTable();

	virtual bool Create(cPagedHashTableHeader<TKey> *header, cQuickDB *quickDB);
	virtual bool Open(cPagedHashTableHeader<TKey> *header, cQuickDB *quickDB, bool readOnly);
	virtual bool Close();
	virtual bool Clear();

	virtual int Insert(const TKey &key, char* data, bool insertOrUpdate = false);
	virtual inline unsigned int GetRealHashTableSize();

	virtual inline unsigned int GetRealHashTableSize() const	{ return mSize + mSplit; }	// returns real count of root entries available for hashing (including split ones)

	virtual void PrintInfo() const;
};

/**
 * If createFlag the tree is created according to defined header. Otherwise some properties are initialized
 *   (called before calling Open() method).  cPagedHashTableKey<TKey> tHtKey
 */
template<class TKey>
cPagedLinearHashTable<TKey>::cPagedLinearHashTable()
{
	mSplit = 0;
	mForceSplit = false;
}

/**
* sharedCache do konstruktoru?
* Create new data structure.
* \param header Data structure header, which is prepared for the data structure creation.
* \param sharedCache Opened cache.
*/
template<class TKey> 
bool cPagedLinearHashTable<TKey>::Create(cPagedHashTableHeader<TKey> *header, cQuickDB *quickDB)
{
	bool ret = false;
	mReadOnly = false;
	mHashArray = NULL;
	mStatus = 0;

	mQuickDB = quickDB;
	if (!mQuickDB->IsOpened())
	{
		printf("cPagedTree::Create - quickDB is not opened!\n");
		exit(1);
	}
	mSharedCache = mQuickDB->GetNodeCache();
	mMemoryPool = mQuickDB->GetMemoryPool();
	mHeader = header;
	mHeader->SetMaxStringLength(0);

	Init();  // init pools, results, and so on
	mSharedCache->Register(mHeader);
	mNodeHeaderId = mHeader->GetNodeType(0);

	mQueryStatistics->Reset();
	mIsOpen = true;
	ret = true;

	unsigned int hashTableSize = 2 * SetHashTableSize(GetHashTableSize());	// allocate twice the size to allow growing
	mHashArray = new tNodeIndex[hashTableSize];
	for (unsigned int i = 0 ; i < hashTableSize ; i++)
	{
		mHashArray[i] = C_EMPTY_LINK;
	}
		
	return ret;
}

/**
* sharedCache do konstruktoru?
* Open existing data structure.
* \param header Data structure header. All values will be read from the secondary storage. Only the node headers has to be preallocated and the data structure name has to be properly set.
* \param sharedCache Opened cache.
*/
template<class TKey>
bool cPagedLinearHashTable<TKey>::Open(cPagedHashTableHeader<TKey> *header, cQuickDB *quickDB, bool readOnly)
{
	bool ret;
	char str[256];

	mReadOnly = readOnly;
	mQuickDB = quickDB;
	if (!mQuickDB->IsOpened())
	{
		printf("cPagedTree::Open - quickDB is not opened!\n");
		exit(1);
	}
	mSharedCache = mQuickDB->GetNodeCache();
	mMemoryPool = mQuickDB->GetMemoryPool();
	mHeader = header;
	mHeader->SetMaxStringLength(0);
	ret = mSharedCache->LookForHeader(mHeader);

	Init();  // resize pools, results, and so on

	mNodeHeaderId = mHeader->GetNodeType(0);
	mQueryStatistics->Reset();


	// read the hash table
	strcpy_s(str, mHeader->GetName());
	strcat_s(str, "_ht.ddt");
	cFileStream hashArrayFile;
	if(!hashArrayFile.Open(str,ACCESS_READ,FILE_OPEN,SEQUENTIAL_SCAN)){
		printf("cPagedTree::Create - ht file was not opened!\n");
	}

	hashArrayFile.Read((char*)&mRootNodes, sizeof(mRootNodes));

	tNodeIndex savedSize = 0;
	hashArrayFile.Read((char*)&savedSize, sizeof(savedSize));
	tNodeIndex hashTableSize = SetHashTableSize((savedSize+2) / 2);
	mSplit = savedSize - hashTableSize;

	mHashArray = new tNodeIndex[2*hashTableSize];
	hashArrayFile.Read((char*)mHashArray, sizeof(tNodeIndex)*savedSize);
	
	hashArrayFile.Read((char*)&mHashValueBonus, sizeof(uchar));
	hashArrayFile.Close();

	for (unsigned int iii = savedSize; iii < hashTableSize * 2; iii++)
		mHashArray[iii] = C_EMPTY_LINK;

	mIsOpen = true;
	ret = true;

	return ret;
}

template<class TKey>
void cPagedLinearHashTable<TKey>::Init()
{
	cPagedStaticHashTable<TKey>::Init();
	mSplit = 0;
	mForceSplit = false;
}

template<class TKey> 
bool cPagedLinearHashTable<TKey>::Clear()
{
	// TODO - neni jasne co ma tato metoda delat
	bool ret = false;
	mReadOnly = false;
	mStatus = 0;

	// clear the data structure header
	mHeader->SetMaxStringLength(0);
	((cNodeHeader*)mHeader)->ResetItemCount();
	((cNodeHeader*)mHeader)->ResetNodeCount();

	mQueryStatistics->Reset();
	mSharedCache->Clear(); // TODO clear only for this data structure

	mIsOpen = true;
	ret = true;
	return ret;
}


template<class TKey> 
bool cPagedLinearHashTable<TKey>::Close()
{	
	char str[256];

	if (!mReadOnly && mHashArray != NULL)
	{			
		strcpy_s(str, mHeader->GetName());
		strcat_s(str, "_ht.ddt");
			
		cFileStream hashArrayFile;
		if(!hashArrayFile.Open(str, ACCESS_READWRITE, FILE_CREATE, SEQUENTIAL_SCAN))
		{
			printf("cPagedTree::Create - ht file was not created!\n");
		}

		hashArrayFile.Write((char*)&mRootNodes, sizeof(mRootNodes));

		tNodeIndex hashTableSize = GetHashTableSize() + mSplit;
		hashArrayFile.Write((char*)&hashTableSize, sizeof(hashTableSize));
		hashArrayFile.Write((char*)mHashArray, sizeof(tNodeIndex)*hashTableSize);
		hashArrayFile.Write((char*)&mHashValueBonus, sizeof(uchar));


		hashArrayFile.Close();
		delete [] mHashArray;
		mHashArray = NULL;
		mSize = 0;
		mRootNodes = 0;
	}

	mQueryStatistics->Reset();
	return true;
}


/**
* splits node (bucket) and rehashes previously stored values
* does not insert any new value
**/
template<class TKey>
inline int cPagedLinearHashTable<TKey>::SplitNode()
{	
	while (mSplit < mSize && mHashArray[mSplit] == C_EMPTY_LINK)	// find first non-empty node to split
	{
		mSplit++;
	}
	mSplit++;	// mSplit points to the next bucket to split.
				// Needs to be shifted before split to make hash value calculation work properly
	if (mSplit > mSize)
	{
		RaiseLevel();
		return -1;
	}

	char* item_buffer;
	tItemOrder* order_buffer;
	TKey key = TKey(mHeader->GetNodeHeader()->GetKeyDescriptor());

	tNodeIndex source_index = mHashArray[mSplit - 1];	// node to split
	mHashArray[mSplit - 1] = C_EMPTY_LINK;	// start from zero

	tNode *source_node, *node0, *node1, *temp_node;
	unsigned int item_size, key_size;
	tNodeIndex next_index, node_index0, node_index1;
	unsigned int total_count;	// total count of items in node to be split
	node_index0 = node_index1 = C_EMPTY_LINK;	// no nodes attached yet
	bool first_run = true, used_node0 = false, used_node1 = false;

	while (source_index != C_EMPTY_LINK)
	{
		GetNodeW(source_index, source_node);
		next_index = GetNextNodeIndex(source_node);

		if (first_run)
		{
			assert(source_node->GetNodeHeader()->GetDStructMode() == cDStructConst::DSMODE_DEFAULT);	// DEFAULT implemented only
			key_size = source_node->GetNodeHeader()->GetKeySize();
			item_size = source_node->GetItemSize(0);
			first_run = false;
		}

		source_node->CopyNodeData(item_buffer, order_buffer);
		total_count = source_node->Clear();
		mSharedCache->UnlockW(source_node);
		mHeader->SetUnusedNode(source_index);
		source_index = next_index;
		
		for (unsigned int source_item = 0; source_item < total_count; source_item++)
		{
			char* raw_key = (char*)(item_buffer + order_buffer[source_item]);				// current item KEY data
			key.SetData(raw_key);
			char* data = raw_key + key_size;	// current item DATA

			tNodeIndex hash_value = GetHashValue(key);
			
			if (hash_value < mSplit)	// or < mSize
			{	// node0
				if (node_index0 == C_EMPTY_LINK)
				{
					GetNodeW(node_index0, node0);
					mHashArray[hash_value] = node_index0;
					used_node0 = true;
				}
				if (!node0->HasLeafFreeSpace(key, data))
				{
					node_index0 = node0->GetNextNode();
					assert(node_index0 == C_EMPTY_LINK);
					GetNodeW(node_index0, temp_node);
					node0->SetNextNode(node_index0);
					mSharedCache->UnlockW(node0);
					node0 = temp_node;
				}
				node0->Insert(key, data);
			}
			else
			{	// node1
				if (node_index1 == C_EMPTY_LINK)
				{
					GetNodeW(node_index1, node1);
					mHashArray[hash_value] = node_index1;
					used_node1 = true;
				}
				if (!node1->HasLeafFreeSpace(key, data))
				{
					node_index1 = node1->GetNextNode();
					assert(node_index1 == C_EMPTY_LINK);
					GetNodeW(node_index1, temp_node);
					node1->SetNextNode(node_index1);
					mSharedCache->UnlockW(node1);
					node1 = temp_node;
				}
				node1->Insert(key, data);
			}
		}

		delete [] item_buffer;
		delete [] order_buffer;
	}

	key.SetData(NULL);
	if (node_index0 != C_EMPTY_LINK) mSharedCache->UnlockW(node0);
	if (node_index1 != C_EMPTY_LINK) mSharedCache->UnlockW(node1);

	if (!used_node0)	mRootNodes--;
	if (used_node1)		mRootNodes++;

	mForceSplit = false;

	return C_SPLIT_OK;
}

/**
* doubles the size of extendible hash table and updates mGlobal
* does not split any node and does not change any mLocal
**/
template<class TKey>
inline int cPagedLinearHashTable<TKey>::RaiseLevel()
{
	if (mHashValueTrim <= 1) return C_SPLIT_FAILED;	// too big hash table or too big hash value bonus
	if (mSplit < mSize) return C_SPLIT_FAILED;

	tNodeIndex oldSize = mSize;
	tNodeIndex newSize = oldSize * 2;
	printf("- Raise HT size to %u \n", newSize);

	tNodeIndex *newHash = NULL;
	
	try {
		newHash = new tNodeIndex[newSize*2];
	}
	catch (...) {
		printf("\nRaising stopped (out of memory) - %d is maximum. \n", oldSize);
		delete [] newHash;
		return C_SPLIT_FAILED;
	}

	// the new data copy as needed by the order preserving
	//memcpy(newHash, mHashArray, sizeof(tNodeIndex)*newSize);	// the old way
	for (unsigned int iii = 0; iii < oldSize; iii++)
	{
		newHash[iii<<1] = mHashArray[iii];
		newHash[(iii << 1) + 1] = mHashArray[iii + mSize];
	}

	for (unsigned int iii = newSize; iii < newSize*2; iii++)
	{
		newHash[iii] = C_EMPTY_LINK;
	}

	delete [] mHashArray;
	mHashArray = newHash;

	SetHashTableSize(newSize);
	mSplit = 0;

	return newSize;
}

template <class TKey>
inline unsigned int cPagedLinearHashTable<TKey>::GetRealHashTableSize()
{
	mSize = mHeader->GetHashTableSize();
	return mSize + mSplit;
}

/**
* returns full hash value or hash table index
**/
template<class TKey>
inline tNodeIndex cPagedLinearHashTable<TKey>::GetHashValue(
	const TKey &key)		// Key value to count hash from
{
	const cDTDescriptor *dtd = mHeader->GetNodeHeader()->GetKeyDescriptor();
	tNodeIndex hashValue =	key.OrderedHashValue(dtd, 1, mHashValueBonus);
	
	if (hashValue == UINT_MAX)
	{
		mHashBonusFails++;
		if (mSplit<mSize)
			return mSize - 1;	// last non-split node
		return (mSize << 1) - 1;	// the very last node
	}

	tNodeIndex ret = hashValue >> mHashValueTrim;
	
	if (ret < mSplit)
	{	// already split nodes

		if ((hashValue>>(mHashValueTrim-1)) & 1 == 1)	// and this one heads to the new nodes zone
		{   // ret is < mSize and mSize is power of 2 - we can use bitwise OR instead of ADD
			ret |= mSize;	
		}
	}
	return ret;
}

/**
 * Insert the key into the hash table.
 *   - INSERT_YES 
 *   - INSERT_EXIST
 */
template<class TKey>
int cPagedLinearHashTable<TKey>::Insert(const TKey &key, char* data, bool pInsertOrUpdate)
{
	int ret;	
	tNode *node;
	bool finishf = false;
	unsigned int chaining = 0;
	bool dupl = mHeader->DuplicatesAllowed();
	/*
	tNodeIndex node_count = mHeader->GetNodeCount();
	if ((node_count > 1024 ? (100 * mRootNodes) / node_count : 100) < 65)
	{
		mForceSplit = true;	// how much percet are root nodes of total nodes
	}*/
	
	if (mForceSplit)
		SplitNode();

	unsigned int hashValue = GetHashValue(key);
	tNodeIndex nodeIndex = mHashArray[hashValue];

	if (nodeIndex == C_EMPTY_LINK)	// extreme case - hash value used for first time
	{
		GetNodeW(nodeIndex, node);
		mHashArray[hashValue] = nodeIndex;
		mRootNodes++;
		ret = node->Insert(key, data);	// new node always has free space
		mSharedCache->UnlockW(node);
		if (ret == C_INSERT_AT_THE_END)
			ret = C_INSERT_YES;
		return ret;
	}

	// no empty_link do the standard insert
	ret = InsertIntoSortedNodeChain(nodeIndex, chaining, key, data, pInsertOrUpdate);
	
	/* dynamic chaining treshold to limit rapid raising hashtable size - not precisely this version implemented
	8k  16k 32k 64k 128k  256k  512k   1M   root fill
	4    8  16  32   64   128   256   512   < 25%		(extreme case)
	1    2   4   8   16    32    64   128   25% - 50%
	2    2   2   2    2     2     2     2   50% - 75%
	1    1   1   1    1     1     1     1   > 75%		(ideal scenario)
	*/

	if ((chaining > (mSize>>13)) || // chaining > 4 for 32k table
		((mRootNodes>(mSize>>1)) && (chaining >= (mSize>>15)) ) || // root fill rate > 50% && chaining >= 1 for 32k table 
		((4*mRootNodes >= 3*mSize) && (chaining >=1) )) // fill rate > 70% && chaining
	{
		mForceSplit = true;	// if above conditions met -> force split
	}

	if (ret == C_INSERT_AT_THE_END)
	{
		ret = C_INSERT_YES;
	}

	return ret;
}


/**
 * Print base information about tree.
 */
template<class TKey> 
void cPagedLinearHashTable<TKey>::PrintInfo() const
{
	printf("******************** cPagedLinearHashTable statistics: ********************\n");
	printf("HashTable Size: %d\n", mSize);
	printf("Item count: %d\t Node count: %d\n", mHeader->GetItemCount(), mHeader->GetNodeCount());
}

}}}
#endif