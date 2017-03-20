/**
*	\file cPagedExtendibleHashTable.h
*	\author Petr Turecek 2015
*	\brief Extendible hash table with order preserving
*	\version 1.0
*	\date jul 2015
*/

//	based on cPagedHashTable.h by Vaclav Snasel & Michal Krátký

#ifndef __cPagedExtendibleHashTable_h__
#define __cPagedExtendibleHashTable_h__

#include "dstruct/paged/core/cQuickDB.h"
#include "dstruct/paged/core/cNodeCache.h"
#include "dstruct/paged/hashtable/cPagedHashTableNode.h"
#include "dstruct/paged/core/cTreeItemStream.h"
#include "dstruct/paged/core/cQueryStatistics.h"
#include "common/cMemory.h"
#include "dstruct/paged/core/cMemoryPool.h"
#include "dstruct/paged/core/cDStructConst.h"
#include "dstruct/paged/core/cTreeNode.h"
#include "dstruct/paged/hashtable/constants.h"

#define MAX_FILE_NAME_LENGTH 100

// max 32 bits for hash table index
#define MAX_GLOBAL	31

namespace dstruct {
  namespace paged {
	namespace hashtable {

using namespace dstruct::paged::core;


template<class TKey>
class cPagedExtendibleHashTable : public cPagedStaticHashTable<TKey>
{
protected:
	uchar *mLocal;		// Local level of Extendible HT
	uchar mGlobal;		// Global level of Extendible HT
	uchar mMaxGlobal;	// no more than 32 bits for hash table index

	inline void AddNewNode(tNodeIndex hash, tNodeIndex node);
	inline int SplitNode(tNodeIndex full_hash, tNode* &node);
	inline int RaiseLevel();	// Raises global level of HT by doubling its size
	virtual inline tNodeIndex GetHashValue(const TKey &key, bool trimHash);
	virtual inline tNodeIndex GetHashValue(const TKey &key) { return GetHashValue(key, true); };
	virtual inline tNodeIndex TrimHashValue(tNodeIndex hash);

public:
	cPagedExtendibleHashTable();
	virtual ~cPagedExtendibleHashTable();

	virtual bool Create(cPagedHashTableHeader<TKey> *header, cQuickDB *quickDB);
	virtual bool Open(cPagedHashTableHeader<TKey> *header, cQuickDB *quickDB, bool readOnly);
	virtual bool Close();
	virtual bool Clear();

	virtual int Insert(const TKey &key, char* data, bool insertOrUpdate = false);

	virtual inline unsigned int SetHashTableSize(unsigned int size);
	inline unsigned int SetMaxGlobal(unsigned int maxGlobal);
	inline unsigned int GetMaxGlobal() const;

	virtual inline void GetNodeW(tNodeIndex hash, tNodeIndex &nodeIndex, tNode* &node, bool chaining = false);

	void PrintInfo() const;
};


template <class TKey>
inline unsigned int cPagedExtendibleHashTable<TKey>::SetHashTableSize(unsigned int size)
{
	cPagedStaticHashTable::SetHashTableSize(size);
	mGlobal = mHashValueBits - mHashValueTrim;

	return mSize;
}


template <class TKey>
inline unsigned int cPagedExtendibleHashTable<TKey>::GetMaxGlobal() const
{
	return mMaxGlobal;
}

template <class TKey>
inline unsigned int cPagedExtendibleHashTable<TKey>::SetMaxGlobal(unsigned int maxGlobal)
{
	if ((maxGlobal>mGlobal) && (maxGlobal <= MAX_GLOBAL))
		mMaxGlobal = maxGlobal;
	return mMaxGlobal;
}


/**
 * If createFlag the tree is created according to defined header. Otherwise some properties are initialized
 *   (called before calling Open() method).  cPagedHashTableKey<TKey> tHtKey
 */
template<class TKey>
cPagedExtendibleHashTable<TKey>::cPagedExtendibleHashTable() : mLocal(NULL)
{
	mGlobal = 0;
	mMaxGlobal = MAX_GLOBAL;
}

template<class TKey>
cPagedExtendibleHashTable<TKey>::~cPagedExtendibleHashTable()
{
	delete [] mLocal;
	mLocal = NULL;
}

/**
* sharedCache do konstruktoru?
* Create new data structure.
* \param header Data structure header, which is prepared for the data structure creation.
* \param sharedCache Opened cache.
*/
template<class TKey> 
bool cPagedExtendibleHashTable<TKey>::Create(cPagedHashTableHeader<TKey> *header, cQuickDB *quickDB)
{
	bool ret = false;
	mReadOnly = false;
	mHashArray = NULL;
	mLocal = NULL;
	mStatus = 0;
	if(mMaxGlobal<1 ||mMaxGlobal > MAX_GLOBAL)
		mMaxGlobal = MAX_GLOBAL;

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

	unsigned int hashTableSize = SetHashTableSize(GetHashTableSize());
	mHashArray = new tNodeIndex[hashTableSize];
	for (unsigned int i = 0 ; i < hashTableSize ; i++)
	{
		mHashArray[i] = C_EMPTY_LINK;
	}

	mLocal = new uchar[hashTableSize];
	memset(mLocal, (mGlobal>2 ? mGlobal-2 : mGlobal), hashTableSize);
	
	return ret;
}

/**
* sharedCache do konstruktoru?
* Open existing data structure.
* \param header Data structure header. All values will be read from the secondary storage. Only the node headers has to be preallocated and the data structure name has to be properly set.
* \param sharedCache Opened cache.
*/
template<class TKey>
bool cPagedExtendibleHashTable<TKey>::Open(cPagedHashTableHeader<TKey> *header, cQuickDB *quickDB, bool readOnly)
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

	unsigned int hashTableSize = 0;
	
	hashArrayFile.Read((char*)&hashTableSize, sizeof(tNodeIndex));
	hashTableSize = SetHashTableSize(hashTableSize);
	
	mHashArray = new tNodeIndex[hashTableSize];
	hashArrayFile.Read((char*)mHashArray, sizeof(tNodeIndex)*hashTableSize);
	
	hashArrayFile.Read((char*)&mHashValueBonus, sizeof(uchar));

	hashArrayFile.Read((char*)&mGlobal, sizeof(mGlobal));
	hashArrayFile.Read((char*)&mMaxGlobal, sizeof(mMaxGlobal));
	
	mLocal = new uchar[hashTableSize];
	hashArrayFile.Read((char*)mLocal, sizeof(uchar)*hashTableSize);
	
	hashArrayFile.Close();

	mIsOpen = true;
	ret = true;

	return ret;
}


template<class TKey> 
bool cPagedExtendibleHashTable<TKey>::Clear()
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
bool cPagedExtendibleHashTable<TKey>::Close()
{	
	char str[256];

	if (!mReadOnly)
	{			
		strcpy_s(str, mHeader->GetName());
		strcat_s(str, "_ht.ddt");
			
		cFileStream hashArrayFile;
		if(!hashArrayFile.Open(str, ACCESS_READWRITE, FILE_CREATE, SEQUENTIAL_SCAN))
		{
			printf("cPagedTree::Create - ht file was not created!\n");
		}
		unsigned int hashTableSize = GetHashTableSize();
		hashArrayFile.Write((char*)&hashTableSize, sizeof(tNodeIndex));
		hashArrayFile.Write((char*)mHashArray, sizeof(tNodeIndex)*mSize);
		hashArrayFile.Write((char*)&mHashValueBonus, sizeof(uchar));

		hashArrayFile.Write((char*)&mGlobal, sizeof(uchar));
		hashArrayFile.Write((char*)&mMaxGlobal, sizeof(mMaxGlobal));
		hashArrayFile.Write((char*)mLocal, sizeof(uchar)*mSize);

		hashArrayFile.Close();
	}

	mQueryStatistics->Reset();
	return true;
}


template<class TKey>
void cPagedExtendibleHashTable<TKey>::GetNodeW(tNodeIndex hash, tNodeIndex &nodeIndex, tNode* &node, bool chaining = false)
{
	if (nodeIndex == C_EMPTY_LINK)
	{
		// there is no node for the hash value, you must create the new one
		node = ReadNewNode();
		nodeIndex = node->GetIndex();
		mHeader->GetNodeHeader()->IncrementNodeCount();
		if (!chaining)
		{
			AddNewNode(hash, nodeIndex);
		}
		return;
	}
	// else the node for the hash value exists, you must read the value
	node = ReadNodeW(nodeIndex);
}


/**
* Sets group of hash table entries from EMPTY_LINK to valid node index
**/
template<class TKey>
inline void cPagedExtendibleHashTable<TKey>::AddNewNode(tNodeIndex hash, tNodeIndex node)
{
	unsigned int gl = mGlobal - mLocal[hash];
	unsigned int mask = ((1u) << gl) - (1u);
	unsigned int base = (unsigned int)hash & ~mask;
	
	for (unsigned int iii = (0u); iii < ((1u)<<gl); iii++)
	{
		unsigned int addr = base | iii;
		mHashArray[addr] = node;
	}
	mRootNodes++;
}


/**
* splits overflown node (bucket) and rehashes previously stored values
* does not insert any new value
**/
template<class TKey>
inline int cPagedExtendibleHashTable<TKey>::SplitNode(tNodeIndex full_hash, tNode* &node)
{
	assert(node->GetNodeHeader()->GetDStructMode() == cDStructConst::DSMODE_DEFAULT);	// DEFAULT implemented only

	tNodeIndex source_index = node->GetIndex();		// nodeIndex to split
	tNodeIndex hash = TrimHashValue(full_hash);	// hash value of key to be inserted
	uchar gl = mGlobal - mLocal[hash];			// difference between mGlobal and mLocal

	if (gl == 0)
	{
		if (RaiseLevel() > 0)
		{
			hash = TrimHashValue(full_hash);
			gl = mGlobal - mLocal[hash];
			assert(mLocal[hash] <= mGlobal);
		}
		else
		{	// raising failed
			return C_SPLIT_FAILED;
		}
	}
										//	  gl=	1		2		3
	unsigned int mask = (1 << gl) - 1;	//	mask=	1		11		111
	unsigned int brk = (1 << (gl-1));	//	 brk=	1		2		4
	unsigned int base = hash & (~mask);

	for (unsigned int iii = 0; iii <= mask; iii++)
	{
		mLocal[base|iii]++;
			mHashArray[base|iii] = C_EMPTY_LINK;
	}
	// infrastructure ready, lets divide the records

	///////////////////////////////////////////////////////////////////////////////////////////////

	char* item_buffer;
	tItemOrder* order_buffer;
	TKey key = TKey(mHeader->GetNodeHeader()->GetKeyDescriptor());

	tNode *source_node, *node0, *node1, *temp_node;
	unsigned int item_size, key_size;
	tNodeIndex next_index, node_index0, node_index1;
	unsigned int total_count;	// total count of items in node to be split
	node_index0 = node_index1 = C_EMPTY_LINK;	// no nodes attached yet
	bool first_run = true, used_node0 = false, used_node1 = false;

	brk |= base;	// hash value split treshold

	while (source_index != C_EMPTY_LINK)
	{
		if (first_run)
		{
			source_node = node;
			key_size = source_node->GetNodeHeader()->GetKeySize();
			item_size = source_node->GetItemSize(0);
			first_run = false;
		}
		else
		{
			source_node = ReadNodeW(source_index);
		}

		next_index = GetNextNodeIndex(source_node);
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

			if (hash_value < brk)
			{	// node0
				if (node_index0 == C_EMPTY_LINK)
				{
					GetNodeW(hash_value, node_index0, node0, used_node0);	// update the hash entry only for the first time
					mHashArray[hash_value] = node_index0;
					used_node0 = true;
				}
				if (!node0->HasLeafFreeSpace(key, data))
				{
					node_index0 = node0->GetNextNode();
					assert(node_index0 == C_EMPTY_LINK);
					cPagedStaticHashTable::GetNodeW(node_index0, temp_node);
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
					GetNodeW(hash_value, node_index1, node1, used_node1);
					mHashArray[hash_value] = node_index1;
					used_node1 = true;
				}
				if (!node1->HasLeafFreeSpace(key, data))
				{
					node_index1 = node1->GetNextNode();
					assert(node_index1 == C_EMPTY_LINK);
					cPagedStaticHashTable::GetNodeW(node_index1, temp_node);
					node1->SetNextNode(node_index1);
					mSharedCache->UnlockW(node1);
					node1 = temp_node;
				}
				node1->Insert(key, data);
			}
		}

		delete[] item_buffer;
		delete[] order_buffer;
	}

	key.SetData(NULL);
	if (node_index0 != C_EMPTY_LINK) mSharedCache->UnlockW(node0);
	if (node_index1 != C_EMPTY_LINK) mSharedCache->UnlockW(node1);

	if (!used_node0)	mRootNodes--;
	if (used_node1)		mRootNodes++;

	if (used_node0 ^ used_node1)	// all records in one node chain, no real split
		return C_SPLIT_NO_CHANGE;

	return C_SPLIT_OK;
}

/**
* doubles the size of extendible hash table and updates mGlobal
* does not split any node and does not change any mLocal
**/
template<class TKey>
inline int cPagedExtendibleHashTable<TKey>::RaiseLevel()
{
	if (mGlobal >= mMaxGlobal)
	{
		return C_SPLIT_FAILED;
	}
	if (mHashValueTrim == 0)
	{
		mMaxGlobal = mGlobal;
		printf("\nRaising stopped (hash value bonus limit) - %d is maximum. \n", mGlobal);
		return C_SPLIT_FAILED;
	}
	unsigned int oldSize = mSize;
	unsigned int newSize = oldSize * 2;
	
	printf("- RaiseGlobalLevel to %d (%u roots)\n", mGlobal+1, newSize);

	tNodeIndex *newHash = NULL;
	uchar *newLocal = NULL;
	
	try {
		newHash = new tNodeIndex[newSize];
		newLocal = new uchar[newSize];
	}
	catch (...) {
		printf("\nRaising stopped (out of memory) - %d is maximum. \n", mGlobal);
		mMaxGlobal = mGlobal;
		delete [] newHash;
		return -1;
	}
	
	for (unsigned int iii = 0; iii < oldSize; iii++)	// copy ABCCD sequence to new AABBCCCCDD
	{
		newHash[iii << 1] = newHash[1 + (iii << 1)] = mHashArray[iii];
		newLocal[iii << 1] = newLocal[1 + (iii << 1)] = mLocal[iii];
	}

	delete [] mHashArray;
	mHashArray = newHash;

	delete [] mLocal;
	mLocal = newLocal;

	mSize = SetHashTableSize(newSize);
	return mGlobal;
}

/**
* returns full hash value or hash table index
**/
template<class TKey>
inline tNodeIndex cPagedExtendibleHashTable<TKey>::GetHashValue(
	const TKey &key,		// Key value to count hash from
	bool trimHash)			// default is TRUE - returns value trimmed to mGlobal level
{
	const cDTDescriptor *dtd = mHeader->GetNodeHeader()->GetKeyDescriptor();
	
	tNodeIndex hashValue =	key.OrderedHashValue(dtd, 1, mHashValueBonus);
	
	if (hashValue == UINT_MAX)	// mHashValueBonus too high
		mHashBonusFails++;

	if (!trimHash) return hashValue;
	return TrimHashValue(hashValue);
}


// returns hash table index
template<class TKey>
inline tNodeIndex cPagedExtendibleHashTable<TKey>::TrimHashValue(tNodeIndex hash)
{
	return	hash >> mHashValueTrim;
}

/**
 * Insert the key into the hash table.
 *   - INSERT_YES 
 *   - INSERT_EXIST
 */
template<class TKey>
int cPagedExtendibleHashTable<TKey>::Insert(const TKey &key, char* data, bool pInsertOrUpdate)
{
	int ret;	
	tNode* node;
	tNode* new_node = NULL;
	bool finishf = false;
	unsigned int chaining = 0;
	bool dupl = mHeader->DuplicatesAllowed();
	unsigned int hashValue = GetHashValue(key, false);
	unsigned int hash;
	tNodeIndex nodeIndex;

	nodeIndex = mHashArray[TrimHashValue(hashValue)];

	if (nodeIndex == C_EMPTY_LINK)	// extreme case - first insert into this HT entry
	{
		GetNodeW(TrimHashValue(hashValue), nodeIndex, node, false);
		mRootNodes++;
		ret = node->Insert(key, data);	// new node always has free space
		mSharedCache->UnlockW(node);
		if (ret == C_INSERT_AT_THE_END)
			ret = C_INSERT_YES;
		return ret;
	}

	node = ReadNodeW(nodeIndex);

	if (!dupl)
	{
		ret = node->FindOrderInsert(key, C_FIND_INSERT);
		if (ret == C_INSERT_EXIST)				// record exists in actual node -> update does not work
		{
			mSharedCache->UnlockW(node);
			return ret;
		}
	}

	while(!node->HasLeafFreeSpace(key, data))
	{	/**/
		if ( (mLocal[TrimHashValue(hashValue)] == mGlobal) && (mGlobal>(mMaxGlobal - 8)) && (mRootNodes<(mSize >> 3)) )
		{	// hashtable already big and with root fill < 12.5%
			mSharedCache->UnlockW(node);

			ret = InsertIntoSortedNodeChain(nodeIndex, chaining, key, data, pInsertOrUpdate);

			if (ret == C_INSERT_AT_THE_END)
				ret = C_INSERT_YES;
			return ret;
		}
		
		if (SplitNode(hashValue, node) == C_SPLIT_FAILED)																		
		{	// -1 -> hash table jiz nemuze byt rozdelena -> fallback to chaining
			ret = InsertIntoSortedNodeChain(nodeIndex, chaining, key, data, pInsertOrUpdate);

			if (ret == C_INSERT_AT_THE_END)
				ret = C_INSERT_YES;
			return ret;
		}

		// split performed - UnlockW(node) done in SplitNode(), let's open it again
		nodeIndex = mHashArray[TrimHashValue(hashValue)];
		GetNodeW(TrimHashValue(hashValue), nodeIndex, node, false);
	}

	ret = node->Insert(key, data);	// new node always has free space
	mSharedCache->UnlockW(node);
	if (ret == C_INSERT_AT_THE_END)
		ret = C_INSERT_YES;
	return ret;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	while(!finishf)
	{
		if (!dupl)
		{
			ret = node->FindOrderInsert(key, C_FIND_INSERT);

			if (ret == C_INSERT_EXIST)				// record exists in actual node -> update does not work
			{
				mSharedCache->UnlockW(node);
				break;
			}
		}

		if (!node->HasLeafFreeSpace(key, data))
		{
			if (SplitNode(hashValue, node) == C_SPLIT_FAILED)		// -1 -> hash table jiz nemuze byt rozdelena
			{
				nodeIndex = GetNextNodeIndex(node);	// fallback to chaining
				chaining++;
				if (nodeIndex == C_EMPTY_LINK)
				{
					GetNodeW(hash, nodeIndex, new_node, true);
					node->SetNextNode(nodeIndex);
				}
				else
					GetNodeW(hash, nodeIndex, new_node, true);
				
			}
			else hash = TrimHashValue(hashValue);
		}
		else
		{
			ret = node->Insert(key, data);
			finishf = true;
		}
		mSharedCache->UnlockW(node);
	}

	if (chaining > mMaxNodeChainLength)		// do some statistics
		mMaxNodeChainLength = chaining;

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
void cPagedExtendibleHashTable<TKey>::PrintInfo() const
{
	printf("******************** cPagedExtendibleHashTable statistics: ********************\n");
	printf("HashTable Size: %d\n", mSize);
	printf("Item count: %d\t Node count: %d\n", mHeader->GetItemCount(), mHeader->GetNodeCount());
}

}}}
#endif