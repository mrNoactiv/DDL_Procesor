/**
*	\file cPagedStaticHashTable.h
*	\author Petr Turecek 2015
*	\brief Static hash table with order preserving and node chaining
*	\version 1.0
*	\date jul 2015
*/

//	based on cPagedHashTable.h by Vaclav Snasel & Michal Krátký

#ifndef __cPagedStaticHashTable_h__
#define __cPagedStaticHashTable_h__

#include "dstruct/paged/core/cQuickDB.h"
#include "dstruct/paged/core/cNodeCache.h"
#include "dstruct/paged/hashtable/cPagedHashTableNode.h"
#include "dstruct/paged/hashtable/cPagedHashTableHeader.h"
#include "dstruct/paged/core/cTreeItemStream.h"
#include "dstruct/paged/core/cQueryStatistics.h"
#include "common/cMemory.h"
#include "dstruct/paged/core/cMemoryPool.h"
#include "dstruct/paged/core/cDStructConst.h"
#include "dstruct/paged/hashtable/constants.h"

#define MAX_FILE_NAME_LENGTH 100

namespace dstruct {
  namespace paged {
	namespace hashtable {

using namespace dstruct::paged::core;

template<class TKey>
class cPagedStaticHashTable
{

protected:
	typedef typename cPagedHashTableNode<TKey> tNode;
	cPagedHashTableHeader<TKey> *mHeader;
	cQuickDB *mQuickDB;
	cNodeCache *mSharedCache;           /// Cache shared by data structures
	cMemoryPool *mMemoryPool;           /// Pool providing temporary variables without a realtime memory allocation
	cQueryStatistics *mQueryStatistics;
	bool mDebug;

	int mStatus;
	bool mIsOpen;
	bool mReadOnly;

	unsigned int mNodeHeaderId;			/// Id of the node. The corresponding value is also stored in the cHeader
	inline unsigned int GetNodeHeaderId() { return mNodeHeaderId; };

	tNodeIndex *mHashArray;
	tNodeIndex	mSize;				// HT base size N
	tNodeIndex	mRootNodes;			// number of nodes attached directly to the hash table
	static const uchar mHashValueBits = 8 * sizeof(tNodeIndex);	// Bit size of value used for hash (32 for uint)
	uchar		mHashValueTrim;
	uchar		mHashValueBonus;

	virtual inline tNodeIndex GetNodeIndex(const TKey &key);
	virtual inline tNodeIndex GetNextNodeIndex(const tNode* node) const;
	virtual inline tNodeIndex GetHashValue(const TKey &key);

	tNode* ReadNewNode();
	tNode* ReadNodeW(unsigned int index);
	tNode* ReadNodeR(unsigned int index);
	inline void GetNodeW(tNodeIndex &nodeIndex, tNode* &node);
	inline int InsertIntoSortedNodeChain(tNodeIndex nodeIndex, unsigned int &chaining, const TKey &key, char* data, bool pInsertOrUpdate = false);

	void Init();

private:
	int FindCacheRow(unsigned int nodeRealSize);

public:
	cPagedStaticHashTable();
	virtual ~cPagedStaticHashTable();

	virtual bool Create(cPagedHashTableHeader<TKey> *header, cQuickDB *quickDB);
	virtual bool Open(cPagedHashTableHeader<TKey> *header, cQuickDB *quickDB, bool readOnly);
	virtual bool Flush();
	virtual bool Close();
	virtual bool Clear();

	virtual int Insert(const TKey &key, char* data, bool insertOrUpdate = false);
	virtual cTreeItemStream<TKey>* Find(const TKey &key, char* data);
	virtual bool PointQuery(const TKey &key, char* pData);

	inline cPagedHashTableHeader<TKey>* GetHeader() const;
	virtual inline unsigned int SetHashTableSize(tNodeIndex newSize);
	virtual inline unsigned int GetRealHashTableSize();
	inline unsigned int GetHashTableSize();
	inline unsigned int GetRootNodes() const;
	inline uchar SetHashValueBonus(uchar newBonus);

	unsigned int mHashBonusFails;	// counter of HashValue overruns due to mHashValueBonus - free to read or set
	unsigned int mMaxNodeChainLength;	// length of the longest chain found during Insert() - free to read or set

	virtual void PrintInfo() const;
	inline void SetDebug(bool debug);
	inline bool IsOpen() const				{ return mIsOpen; }

	void DoHashTableStatistics(uint &TableSize, uint &TotalNodes, uint &RootNodes, uint &TotalItems, uint &LongestChain);
};

/**
 * If createFlag the tree is created according to defined header. Otherwise some properties are initialized
 *   (called before calling Open() method).  cPagedHashTableKey<TKey> tHtKey
 */
template<class TKey>
cPagedStaticHashTable<TKey>::cPagedStaticHashTable() : mDebug(false), mHashArray(NULL)
{
	mQueryStatistics = new cQueryStatistics();
	Init();
}

template<class TKey>
cPagedStaticHashTable<TKey>::~cPagedStaticHashTable()
{
	if (mHashArray != NULL)
	{
		Close();
	}

	delete mQueryStatistics;
	mQueryStatistics = NULL;
}

template<class TKey>
void cPagedStaticHashTable<TKey>::Init()
{
	delete mHashArray;
	mHashArray = NULL;
	
	if (mQueryStatistics == NULL)
		mQueryStatistics = new cQueryStatistics();
	mQueryStatistics->Resize();        // resize of query statistic
	mRootNodes = 0;
	mSize = 0;
	mMaxNodeChainLength = 0;
	mHashValueTrim = 0;
	mHashValueBonus = 0;
	mHashBonusFails = 0;
}


/**
* sharedCache do konstruktoru?
* Create new data structure.
* \param header Data structure header, which is prepared for the data structure creation.
* \param sharedCache Opened cache.
*/
template<class TKey> 
bool cPagedStaticHashTable<TKey>::Create(cPagedHashTableHeader<TKey> *header, cQuickDB *quickDB)
{
	bool ret = false;
	mReadOnly = false;
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

	SetHashTableSize(mHeader->GetHashTableSize());
	mHashArray = new tNodeIndex[mSize];
	for (unsigned int i = 0 ; i < mSize ; i++)
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
bool cPagedStaticHashTable<TKey>::Open(cPagedHashTableHeader<TKey> *header, cQuickDB *quickDB, bool readOnly)
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
	mHashArray = new tNodeIndex[hashTableSize];
	hashArrayFile.Read((char*)mHashArray, sizeof(tNodeIndex)*hashTableSize);
	hashArrayFile.Read((char*)&mHashValueBonus, sizeof(uchar));
	hashArrayFile.Close();

	SetHashTableSize(hashTableSize);

	mIsOpen = true;
	ret = true;

	return ret;
}

template<class TKey> 
bool cPagedStaticHashTable<TKey>::Clear()
{
	bool ret = false;
	mReadOnly = false;
	mStatus = 0;
	Init();

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
bool cPagedStaticHashTable<TKey>::Flush() 
{
	return true;
}

template<class TKey> 
bool cPagedStaticHashTable<TKey>::Close()
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
		unsigned int hashTableSize = mSize;
		hashArrayFile.Write((char*)&hashTableSize, sizeof(tNodeIndex));
		hashArrayFile.Write((char*)mHashArray, sizeof(tNodeIndex)*mSize);
		hashArrayFile.Write((char*)&mHashValueBonus, sizeof(uchar));
		hashArrayFile.Close();

		Init();
	}

	delete mQueryStatistics;
	mQueryStatistics = NULL;
	
	return true;
}


template<class TKey>
int cPagedStaticHashTable<TKey>::InsertIntoSortedNodeChain(tNodeIndex nodeIndex, unsigned int &chaining, const TKey &key, char* data, bool pInsertOrUpdate)
{
	int ret;
	tNodeIndex next;
	tNode *node;
	int comp;
	unsigned int nodeCapacity = (mHeader->GetNodeHeader()->GetNodeCapacity());

	next = nodeIndex;

	while (next != C_EMPTY_LINK)
	{
		nodeIndex = next;
		node = ReadNodeR(nodeIndex);	// just looking, no need to have write access yet

		if ((comp = node->CompareWithLastKey(key.GetData())) <= C_COMPARE_EQUAL)
		{
			mSharedCache->UnlockR(node);
			break;
		}

		next = GetNextNodeIndex(node);
		mSharedCache->UnlockR(node);
		chaining++;
	}

	if (comp == C_COMPARE_EQUAL)		// duplicates handling
	{
		if (!mHeader->DuplicatesAllowed())	// update data not implemented yet - return error
		{
			return C_INSERT_EXIST;
		}							// else insert duplicate
	}

	node = ReadNodeW(nodeIndex);	// now open for write

	if (node->GetItemCount() >= nodeCapacity)	// no space to insert but we have to do it anyway
	{
		char *shift_item = NULL;
		node->ExtractLastItem(shift_item);

		tNode *chainNode;
		next = GetNextNodeIndex(node);
		if (next == C_EMPTY_LINK)
		{
			GetNodeW(next, chainNode);
			node->SetNextNode(next);
		}
		else
		{
			chainNode = ReadNodeW(next);
		}

		// we have the space to insert -> do it
		ret = node->Insert(key, data);
		mSharedCache->UnlockW(node);	// new item stored, we can close node now

		do  // finish the item shifting
		{
			if (chainNode->ShiftItemChain(shift_item) == C_FIND_NOTEXIST)	// inserted and no need to another shift
			{
				mSharedCache->UnlockW(chainNode);
				break;
			}

			next = GetNextNodeIndex(chainNode);
			if (next == C_EMPTY_LINK)
			{
				tNode* new_node;
				GetNodeW(next, new_node);
				chainNode->SetNextNode(next);
				mSharedCache->UnlockW(chainNode);
				chainNode = new_node;
				new_node = NULL;
			}
			else
			{
				mSharedCache->UnlockW(chainNode);
				GetNodeW(next, chainNode);
			}
		} while (true);
	}
	else
	{	// we have the space to insert -> do it
		ret = node->Insert(key, data);
		mSharedCache->UnlockW(node);	// new item stored, we can close node now
	}

	if (chaining > mMaxNodeChainLength)		// do some statistics
		mMaxNodeChainLength = chaining;

	if (ret == C_INSERT_EXIST2)
		ret = C_INSERT_EXIST;

	return ret;
}

/**
 * Insert the key into the hash table.
 *   - INSERT_YES 
 *   - INSERT_EXIST
 */
template<class TKey>
int cPagedStaticHashTable<TKey>::Insert(const TKey &key, char* data, bool pInsertOrUpdate)
{
	int ret = 0;
	tNode* node;
	unsigned int chaining = 0, hashValue = GetHashValue(key);
	tNodeIndex nodeIndex = mHashArray[hashValue];
	
	if (nodeIndex == C_EMPTY_LINK)	// extreme case - first record inserted into this hash table cell
	{	// there is no node for the hash value, you must create the new one
		node = ReadNewNode();
		nodeIndex = node->GetIndex();
		mHeader->GetNodeHeader()->IncrementNodeCount();
		mRootNodes++;	// do some statistics
		mHashArray[hashValue] = nodeIndex;
		// in the new node, there is always space to insert and no duplicates
		ret = node->Insert(key, data);
		mSharedCache->UnlockW(node);
		if (ret == C_INSERT_AT_THE_END)
			ret = C_INSERT_YES;
		return ret;
	}

	// no empty_link do the standard insert
	ret = InsertIntoSortedNodeChain(nodeIndex, chaining, key, data, pInsertOrUpdate);

	if (ret == C_INSERT_AT_THE_END)
		ret = C_INSERT_YES;

	return ret;
}

/**
* Find the key in the hash table. Only one key is in the result, data are returned in data.
*/
template<class TKey>
bool cPagedStaticHashTable<TKey>::PointQuery(const TKey &key, char* pData)
{
	bool ret = false;
	tNode *node;
	int order;
	char* pKey = key.GetData();

	cPagedHashTableNodeHeader<TKey>* nodeHeader = mHeader->GetNodeHeader();
	tNodeIndex nodeIndex = this->GetNodeIndex(key);

	while (nodeIndex != C_EMPTY_LINK)
	{
		node = ReadNodeR(nodeIndex);
		
		if (node->CompareWithLastKey(pKey) <= C_COMPARE_EQUAL)
			break;
		
		nodeIndex = GetNextNodeIndex(node);
		mSharedCache->UnlockR(node);
	}

	if (nodeIndex == C_EMPTY_LINK)
		return false;

	if ((order = node->FindOrder(key, C_FIND_SBE, NULL)) != C_FIND_NOTEXIST)
	{
		cNodeItem::CopyData(pData, node->GetData(order, NULL), nodeHeader);
		mSharedCache->UnlockR(node);
		return true;
	}

	return false;
}


/**
* Find the key in the hash table.
*/
template<class TKey>
cTreeItemStream<TKey>* cPagedStaticHashTable<TKey>::Find(const TKey &key, char* data)
{
	bool ret = false;
	cNodeBuffers<TKey> nodeBuffers;
	tNode *node;
	int order;

	const cDTDescriptor* dtd = mHeader->GetNodeHeader()->GetKeyDescriptor();

	cTreeItemStream<TKey>* itemStream = (cTreeItemStream<TKey>*)mQuickDB->GetResultSet();
	itemStream->SetNodeHeader(mHeader->GetNodeHeader());

	unsigned int nodeIndex = GetNodeIndex(key);

	while (nodeIndex != C_EMPTY_LINK)
	{
		node = ReadNodeR(nodeIndex);

		if (mDebug)
		{
			node->Print(&nodeBuffers.itemBuffer);
		}

		order = node->FindOrder(key, C_FIND_SBE, NULL);
		if (order != C_FIND_NOTEXIST) 
		{
			while (TKey::Compare(key.GetData(), node->GetCKey(order), dtd) >= 0)
			{
				itemStream->Add(node->GetCItem(order));

				if (++order == node->GetItemCount()) 
				{
					break;
				}
			}
		}

		nodeIndex = GetNextNodeIndex(node);
		mSharedCache->UnlockR(node);		
	}

	itemStream->FinishWrite();
	return itemStream;
}


template<class TKey>
inline tNodeIndex cPagedStaticHashTable<TKey>::GetNodeIndex(const TKey &key)
{
	unsigned int idx = GetHashValue(key);
	return mHashArray[idx];
}

template<class TKey>
inline tNodeIndex cPagedStaticHashTable<TKey>::GetNextNodeIndex(const tNode* node) const
{
	return node->GetNextNode();
}

template<class TKey>
inline tNodeIndex cPagedStaticHashTable<TKey>::GetHashValue(const TKey &key)
{
	const cDTDescriptor *dtd = mHeader->GetNodeHeader()->GetKeyDescriptor();
	tNodeIndex hashValue = key.OrderedHashValue(dtd, 1, mHashValueBonus);
	if (hashValue == UINT_MAX)
		mHashBonusFails++;

	hashValue>>=mHashValueTrim;
	return hashValue;
}


/**
* Read new inner node
*/
template<class TKey>
cPagedHashTableNode<TKey>* cPagedStaticHashTable<TKey>::ReadNewNode()
{
	tNode* node;
	
	// try to get entry from unused nodes cache, when no unused node found, return EMPTY_LINK back
	tNodeIndex nodeIndex = GetHeader()->GetUnusedNode();

	if (nodeIndex != C_EMPTY_LINK)
	{
		node = ReadNodeW(nodeIndex);
	}
	else
	{
		node = (tNode*)mSharedCache->ReadNew(GetNodeHeaderId());
	}
	
	node->Init(); 
	node->SetLeaf(true);
	node->SetNextNode(C_EMPTY_LINK);
	return node;
}

template<class TKey>
cPagedHashTableNode<TKey>* cPagedStaticHashTable<TKey>::ReadNodeW(unsigned int index)
{
	tNode* node = (tNode*)mSharedCache->ReadW(index, mNodeHeaderId);
	return node;
}

template<class TKey>
cPagedHashTableNode<TKey>* cPagedStaticHashTable<TKey>::ReadNodeR(unsigned int index)
{
	tNode* node = (tNode*)mSharedCache->ReadR(index, mNodeHeaderId);
	return node;
}

template<class TKey>
void cPagedStaticHashTable<TKey>::GetNodeW(tNodeIndex &nodeIndex, tNode* &node)
{
	if (nodeIndex == C_EMPTY_LINK)
	{
		// there is no node for the hash value, you must create the new one
		node = ReadNewNode();
		nodeIndex = node->GetIndex();
		mHeader->GetNodeHeader()->IncrementNodeCount();
		return;
	}
	// else the node for the hash value exists, you must read the value
	node = ReadNodeW(nodeIndex);
}

template<class TKey>
inline cPagedHashTableHeader<TKey>* cPagedStaticHashTable<TKey>::GetHeader() const
{ 
	return mHeader; 
};

template <class TKey>
inline unsigned int cPagedStaticHashTable<TKey>::SetHashTableSize(tNodeIndex newSize)
{
	if (newSize == 0) return 0;	// invalid value
	if (newSize > (1 << (mHashValueBits - 1))) newSize = 1 << (mHashValueBits - 1);
	mSize = 256;
	mHashValueTrim = mHashValueBits - 8;	// correct for size of 256

	while (mSize < newSize)
	{
		mSize <<= 1;
		mHashValueTrim--;
	}

	mHeader->SetHashTableSize(mSize);
	return mSize;
}

template <class TKey>
inline uchar cPagedStaticHashTable<TKey>::SetHashValueBonus(uchar newBonus)
{
	if (newBonus < mHashValueBits - 2) 	// not too high value
		mHashValueBonus = newBonus;
	
	return mHashValueBonus;
}

template <class TKey>
inline unsigned int cPagedStaticHashTable<TKey>::GetHashTableSize()
{
	mSize = mHeader->GetHashTableSize();
	return mSize;
}

template <class TKey>
inline unsigned int cPagedStaticHashTable<TKey>::GetRealHashTableSize()
{
	mSize = mHeader->GetHashTableSize();
	return mSize;
}

template <class TKey>
inline unsigned int cPagedStaticHashTable<TKey>::GetRootNodes() const
{
	return mRootNodes;
}

template<class TKey> 
inline void cPagedStaticHashTable<TKey>::SetDebug(bool debug)
{
	mDebug = debug;
}

template<class TKey>
void cPagedStaticHashTable<TKey>::DoHashTableStatistics(uint &TableSize, uint &TotalNodes, uint &RootNodes, uint &TotalItems, uint &LongestChain)
{
	TableSize = GetRealHashTableSize();		// used by Linear HT - real size = mSize + mSplit
	TotalItems = mHeader->GetItemCount();
	TotalNodes = 0;
	RootNodes = 0;
	LongestChain = 0;

	unsigned int lastEntry = C_EMPTY_LINK;	// used by Extendible HT - group nodes are used by multiple mHashArray entries
	for (unsigned int i = 0; i < TableSize; i++)
	{
		tNodeIndex idx = mHashArray[i];
		if (idx == EMPTY_LINK || idx == lastEntry) continue;

		RootNodes++;
		lastEntry = idx;
		unsigned int chain = 0;
		while (idx != C_EMPTY_LINK)
		{
			tNode* node = ReadNodeR(idx);
			idx = node->GetNextNode();
			mSharedCache->UnlockR(node);
			chain++;
			TotalNodes++;
		}
		if (chain > LongestChain)
			LongestChain = chain;
	}
}

/**
 * Print base information about tree.
 */
template<class TKey> 
void cPagedStaticHashTable<TKey>::PrintInfo() const
{
	printf("****************************** cPagedStaticHashTable statistics: *******************************\n");
	printf("HashTable Size: %d\n", mSize);
	printf("Item count: %d\t Node count: %d\n", mHeader->GetItemCount(), mHeader->GetNodeCount());
}

}}}
#endif