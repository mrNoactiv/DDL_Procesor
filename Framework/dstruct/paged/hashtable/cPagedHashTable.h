/**
*	\file cPagedHashTable.h
*	\author Vaclav Snasel 1998-2011, Michal Krátký 2001-2011
*	\version 0.2
*	\date jul 2011
*	\brief A root class of the paged tree
*/

#ifndef __cPagedHashTable_h__
#define __cPagedHashTable_h__

#include "dstruct/paged/core/cQuickDB.h"
#include "dstruct/paged/core/cNodeCache.h"
#include "dstruct/paged/hashtable/cPagedHashTableNode.h"
#include "dstruct/paged/hashtable/cPagedHashTableHeader.h"
#include "dstruct/paged/core/cTreeItemStream.h"
#include "dstruct/paged/core/cQueryStatistics.h"
#include "common/cMemory.h"
#include "dstruct/paged/core/cMemoryPool.h"
#include "dstruct/paged/core/cDStructConst.h"

#define MAX_FILE_NAME_LENGTH 100

namespace dstruct {
  namespace paged {
	namespace hashtable {

using namespace dstruct::paged::core;

/**
*	The paged hash table
*
*	\author Michal Krátký
*	\version 0.2
*	\date mar 2014
**/
template<class TKey>
class cPagedHashTable
{
	typedef typename cPagedHashTableNode<TKey> tNode;

protected:
	tNodeIndex *mHashArray;
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

private:
	void Init();
	int FindCacheRow(unsigned int nodeRealSize);

public:
	cPagedHashTable();
	~cPagedHashTable();

	bool Create(cPagedHashTableHeader<TKey> *header, cQuickDB *quickDB);
	bool Open(cPagedHashTableHeader<TKey> *header, cQuickDB *quickDB, bool readOnly);
	bool Flush();
	bool Close();
	bool Clear();

	int Insert(const TKey &key, char* data, bool insertOrUpdate = false);
	cTreeItemStream<TKey>* Find(const TKey &key, char* data);
	bool PointQuery(const TKey &key, char* pData);

	tNode* ReadNewNode();
	tNode* ReadNodeW(unsigned int index);
	tNode* ReadNodeR(unsigned int index);

	inline cPagedHashTableHeader<TKey>* GetHeader();

	void PrintInfo() const;
	inline void SetDebug(bool debug);
	inline bool IsOpen() const				{ return mIsOpen; }
};

/**
 * If createFlag the tree is created according to defined header. Otherwise some properties are initialized
 *   (called before calling Open() method).  cPagedHashTableKey<TKey> tHtKey
 */
template<class TKey>
cPagedHashTable<TKey>::cPagedHashTable() : mDebug(false), mHashArray(NULL)
{
	mQueryStatistics = new cQueryStatistics();
}

template<class TKey>
cPagedHashTable<TKey>::~cPagedHashTable()
{
	Close();
	// mQueryResult->Close();

	if (mHashArray != NULL)
	{
		delete mHashArray;
	}
	mHashArray == NULL;

	delete mQueryStatistics;
}

/**
* sharedCache do konstruktoru?
* Create new data structure.
* \param header Data structure header, which is prepared for the data structure creation.
* \param sharedCache Opened cache.
*/
template<class TKey> 
bool cPagedHashTable<TKey>::Create(cPagedHashTableHeader<TKey> *header, cQuickDB *quickDB)
{
	bool ret = false;
	mReadOnly = false;
	mStatus = 0;
	char str[256];

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

	unsigned int hashTableSize = mHeader->GetHashTableSize();
	mHashArray = new tNodeIndex[hashTableSize];
	for (int i = 0 ; i < hashTableSize ; i++)
	{
		mHashArray[i] = tNode::EMPTY_LINK;
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
bool cPagedHashTable<TKey>::Open(cPagedHashTableHeader<TKey> *header, cQuickDB *quickDB, bool readOnly)
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
	strcpy(str, mHeader->GetName());
	strcat(str, "_ht.ddt");
	cFileStream hashArrayFile;
	if(!hashArrayFile.Open(str,ACCESS_READ,FILE_OPEN,SEQUENTIAL_SCAN)){
		printf("cPagedTree::Create - ht file was not opened!\n");
	}

	unsigned int hashTableSize = 0;
	hashArrayFile.Read((char*)&hashTableSize, sizeof(tNodeIndex));
	mHashArray = new tNodeIndex[hashTableSize];
	hashArrayFile.Read((char*)mHashArray, sizeof(tNodeIndex)*hashTableSize);
	mHeader->SetHashTableSize(hashTableSize);
	hashArrayFile.Close();

	mIsOpen = true;
	ret = true;

	return ret;
}

template<class TKey>
void cPagedHashTable<TKey>::Init()
{
	mQueryStatistics->Resize();        // resize of query statistic
	// mHeader->SetQueryStatistics(mQueryStatistics);
	// mHeader->SetMemoryPool(mMemoryPool);

}

template<class TKey> 
bool cPagedHashTable<TKey>::Clear()
{
	// TODO - neni jasne co ma tato metoda delat
	bool ret = false;
	mReadOnly = false;
	mStatus = 0;

	// clear the data structure header
	mHeader->SetMaxStringLength(0);
	mHeader->SetTopNodeIndex((unsigned int)~0);
	mHeader->ReseItemCount();
	mHeader->ResetNodeCount();
	// TODO - actualize the header in the secondary storage

	mQueryStatistics->Reset();
	mSharedCache->Clear(); // TODO clear only for this data structure

	mIsOpen = true;
	ret = true;
	return ret;
}

template<class TKey> 
bool cPagedHashTable<TKey>::Flush() 
{
	return true;
}

template<class TKey> 
bool cPagedHashTable<TKey>::Close()
{	
	char str[256];

	if (!mReadOnly)
	{			
		strcpy(str, mHeader->GetName());
		strcat(str, "_ht.ddt");
			
		cFileStream hashArrayFile;
		if(!hashArrayFile.Open(str, ACCESS_READWRITE, FILE_CREATE, SEQUENTIAL_SCAN))
		{
			printf("cPagedTree::Create - ht file was not created!\n");
		}
		unsigned int hashTableSize = mHeader->GetHashTableSize();
		hashArrayFile.Write((char*)&hashTableSize, sizeof(tNodeIndex));
		hashArrayFile.Write((char*)mHashArray, sizeof(tNodeIndex)*mHeader->GetHashTableSize());
		hashArrayFile.Close();
	}

	// Todo - zavreni pouze datove struktury, tedy odstraneni z cache tech uzlu, ktere nalezeji teto datove strukture.
	mQueryStatistics->Reset();
	return true;
}

/**
 * Insert the key into the hash table.
 *   - INSERT_YES 
 *   - INSERT_EXIST
 */
template<class TKey>
int cPagedHashTable<TKey>::Insert(const TKey &key, char* data, bool pInsertOrUpdate)
{
	const cDTDescriptor *dtd = mHeader->GetNodeHeader()->GetKeyDescriptor();
	int ret;
	tNode* node;
	bool finishf = false;
	unsigned int hashValue = key.HashValue(mHeader->GetHashTableSize(), dtd);
	tNodeIndex nodeIndex = mHashArray[hashValue];

	while(!finishf)
	{
		if (nodeIndex == tNode::EMPTY_LINK)
		{
			// there is no node for the hash value, you must create the new one
			node = ReadNewNode();
			nodeIndex = node->GetIndex();
			mHeader->GetNodeHeader()->IncrementNodeCount();
			mHashArray[hashValue] = nodeIndex;
		}
		else
		{
			// the node for the hash value exists, you must read the value
			node = ReadNodeW(nodeIndex);
		}

		if (!node->HasLeafFreeSpace(key, data))
		{
			nodeIndex = node->GetNextNode();
		}
		else
		{
			ret = node->Insert(key, data);
			finishf = true;
		}
		mSharedCache->UnlockW(node);
	}

	if (ret == tNode::INSERT_AT_THE_END)
	{
		ret = tNode::INSERT_YES;
	}

	return ret;
}

/**
* Find the key in the hash table. Only one key is in the result, data are returned in data.
*/
template<class TKey>
bool cPagedHashTable<TKey>::PointQuery(const TKey &key, char* pData)
{
	bool ret = false;
	tNode *node;
	int order;

	cPagedHashTableNodeHeader<TKey>* nodeHeader = mHeader->GetNodeHeader();
	const cDTDescriptor *dtd = nodeHeader->GetKeyDescriptor();
	cNodeBuffers<TKey> nodeBuffers;

	unsigned int hashValue = key.HashValue(mHeader->GetHashTableSize(), dtd);
	tNodeIndex nodeIndex = mHashArray[hashValue];

	while(nodeIndex != node->EMPTY_LINK)
	{
		node = ReadNodeR(nodeIndex);

		if ((order = node->FindOrder(key, node->FIND_SBE, &nodeBuffers.itemBuffer)) != tNode::FIND_NOTEXIST)
		{
			cNodeItem::CopyData(pData, node->GetData(order, &nodeBuffers.itemBuffer), nodeHeader);
			nodeIndex = node->EMPTY_LINK;
			ret = true;
		} else
		{
			nodeIndex = node->GetExtraLink(0);
		}
		mSharedCache->UnlockR(node);
	}

	return ret;
}


/**
* Find the key in the hash table.
*/
template<class TKey>
cTreeItemStream<TKey>* cPagedHashTable<TKey>::Find(const TKey &key, char* data)
{
	// unsigned int length = (unsigned int)wcslen(term);
	bool ret = false;
	const cDTDescriptor *dtd = mHeader->GetNodeHeader()->GetKeyDescriptor();
	cNodeBuffers<TKey> nodeBuffers;
	tNode *node;
	int order;

	cTreeItemStream<TKey>* itemStream = (cTreeItemStream<TKey>*)mQuickDB->GetResultSet();
	itemStream->SetNodeHeader(mHeader->GetNodeHeader());

	unsigned int arrayIndex = key.HashValue(mHeader->GetHashTableSize(), dtd);

	if (mHashArray[arrayIndex] != tNode::NODE_NOT_EXIST)
	{
		tNodeIndex nodeIndex = mHashArray[arrayIndex];

		while(nodeIndex != node->EMPTY_LINK)
		{
			node = ReadNodeR(nodeIndex);

			if (mDebug)
			{
				node->Print(&nodeBuffers.itemBuffer);
			}

			order = node->FindOrder(key, node->FIND_SBE, &nodeBuffers.itemBuffer);
			if (order != tNode::FIND_NOTEXIST) 
			{
				while (TKey::Compare(key.GetData(), node->GetCKey(order, &nodeBuffers.itemBuffer), mHeader->GetNodeHeader()->GetKeyDescriptor()) >= 0)
				{
					itemStream->Add(node->GetCItem(order, &nodeBuffers.itemBuffer));

					if (++order == node->GetItemCount()) 
					{
						break;
					}
				}
			}

			nodeIndex = node->GetExtraLink(0);
			mSharedCache->UnlockR(node);
			//if (mDebug) {currentLeafNode->Print(buffer);}				
		}
	}

	itemStream->FinishWrite();

	return itemStream;
}

/**
* Read new inner node
*/
template<class TKey>
cPagedHashTableNode<TKey>* cPagedHashTable<TKey>::ReadNewNode()
{
	tNode* node = (tNode*)mSharedCache->ReadNew(mNodeHeaderId);
	
	node->Init(); 
	node->SetLeaf(true);
	node->SetExtraLink(0, cPagedHashTableNode<TKey>::NODE_NOT_EXIST);
	//node->SetRealSize(node->GetInMemorySize());
	return node;
}

template<class TKey>
cPagedHashTableNode<TKey>* cPagedHashTable<TKey>::ReadNodeW(unsigned int index)
{
	tNode* node = (tNode*)mSharedCache->ReadW(index, mNodeHeaderId);
	//node->SetRealSize(node->GetInMemorySize());
	return node;
}

template<class TKey>
cPagedHashTableNode<TKey>* cPagedHashTable<TKey>::ReadNodeR(unsigned int index)
{
	tNode* node = (tNode*)mSharedCache->ReadR(index, mNodeHeaderId);
	//node->SetRealSize(node->GetInMemorySize());
	return node;
}

template<class TKey>
inline cPagedHashTableHeader<TKey>* cPagedHashTable<TKey>::GetHeader() 
{ 
	return mHeader; 
};

template<class TKey> 
inline void cPagedHashTable<TKey>::SetDebug(bool debug)
{
	mDebug = debug;
}

/**
 * Print base information about tree.
 */
template<class TKey> 
void cPagedHashTable<TKey>::PrintInfo() const
{
	printf("****************************** cPagedHashTable statistics: *******************************\n");
	printf("HashTable Size: %d\n",mHeader->GetHashTableSize());
	printf("Item count: %d\t Node count: %d\n", mHeader->GetItemCount(), mHeader->GetNodeCount());
}

//	printf("Height:                %d\n", mHeader->GetHeight());
//	printf("Inner item capacity:   %d\t Leaf item capacity:   %d\n", mHeader->GetNodeItemCapacity(), mHeader->GetLeafNodeItemCapacity());
//	printf("Inner fanout capacity: %d\t Leaf fanout capacity: %d\n", mHeader->GetNodeFanoutCapacity(), mHeader->GetLeafNodeFanoutCapacity());
//	printf("Item count:            %d\t (inner items: %d\t+  leaf items: %d)\n", mHeader->GeTKeyCount(), mHeader->GetInnerItemCount(), mHeader->GetLeafItemCount());
//	printf("Node count:            %d\t (inner nodes: %d\t+  leaf nodes: %d)\n", mHeader->GetNodeCount(), mHeader->GetInnerNodeCount(), mHeader->GetLeafNodeCount());
//	printf("Average utilization:   %2.1f%%\t (inner nodes: %2.1f%% \t;  leaf nodes: %2.1f%%)\n", mHeader->AverageNodeUtilization(), mHeader->AverageInnerNodeUtilization(), mHeader->AverageLeafNodeUtilization());
//	// printf("Node size:             %dB\n", mHeader->GetNodeSize());
//	printf("Inner node Serial size:  %dB\t Leaf node Serial size:  %dB\n", mHeader->GetNodeSerialSize(), mHeader->GetLeafNodeSerialSize());
//	printf("Inner node InMem size:  %dB\t Leaf node InMem size:  %dB\n", mHeader->GetNodeInMemSize(), mHeader->GetLeafNodeInMemSize());
//	printf("Inner node item InMem size:  %dB\t Leaf node item InMem size:  %dB\n", mHeader->GetNodeItemInMemSize(), mHeader->GetLeafNodeItemInMemSize());
//	printf("Cache size [node]:     %d\n", mSharedCache->GetCacheSize());
//}
}}}
#endif