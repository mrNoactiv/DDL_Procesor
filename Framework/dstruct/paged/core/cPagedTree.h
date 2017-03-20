/**
*	\file cPagedTree.h
*	\author Michal Kr├ítk├Ż 2001-2011
*	\version 0.2
*	\date jul 2011
*	\brief A root class of the paged tree
*/

#ifndef __cPagedTree_h__
#define __cPagedTree_h__

#include "dstruct/paged/core/cTreeHeader.h"
#include "dstruct/paged/core/cNodeCache.h"
#include "common/cMemory.h"
#include "dstruct/paged/core/cMemoryPool.h"
#include "dstruct/paged/core/cQuickDB.h"
#include "common/utils/cHistogram.h"
#include "dstruct/paged/queryprocessing/cRQBuffers.h"
#include "dstruct/paged/queryprocessing/sBatchRQ.h"

using namespace common::utils;

#define MAX_FILE_NAME_LENGTH 100

namespace dstruct {
  namespace paged {
	namespace core {

/**
*	A root class of the paged tree
*
*	\author Radim Ba─Źa, David Bedn├í┼Ö, Michal Kr├ítk├Ż
*	\version 0.2
*	\date jul 2011
**/
template<class TItem, class TNode, class TLeafNode>
class cPagedTree
{
protected:
	cMemory* mMemory;
	cCharStream mCharStream;
	char mFileName[MAX_FILE_NAME_LENGTH];
	int mStatus;
	bool mIsOpen;
	bool mReadOnly;

	unsigned int mInnerNodeId;			/// Id of the inner node. The corresponding value is also stored in the cHeader.
	unsigned int mLeafNodeId;			/// Id of the leaf node. The corresponding value is also stored in the cHeader.

	cTreeHeader *mHeader;
	cNodeCache *mSharedCache;           /// Cache shared by data structures
	cMemoryPool *mMemoryPool;           /// Pool providing temporary variables without a realtime memory allocation
	cMemoryManager *mMemoryManager;     /// Pool providing temporary variables without a realtime memory allocation
#ifdef CUDA_ENABLED
	cMemoryManagerCuda* mMemoryManagerCuda;
#endif
	cQuickDB* mQuickDB;					/// Represents the database (cache, pool, result sets)
	bool mDebug;

	cHistogram **mTreeHistogram;		/// Histogram of dimension distributions of the index
	cHistogram *mNodeHistogram;		    /// Histogram of nodes on individual tree levels
	cHistogram ***mItemHistogram;		/// Average histogram of dimension distributions of the nodes (for each level)
	uint** mUniqueValuesCount;			/// Number of unique values on the tree level (for each dimension)
	uint* mItemsCount;                  /// Number of items on the tree level
	static const unsigned int RESULT_CACHE_SIZE = 128;

public:
	/// methods accessing both inner and leaf nodes of the tree
	inline TNode* ReadNewInnerNode();
	inline TLeafNode* ReadNewLeafNode();
	inline TNode* ReadInnerNodeW(unsigned int index);
	inline TNode* ReadInnerNodeR(unsigned int index);
	inline TLeafNode* ReadLeafNodeW(unsigned int index);
	inline TLeafNode* ReadLeafNodeR(unsigned int index);

private:
	void Init();

protected:
	void CreateDimDistribution(char* minValues, char* maxValues);
	void CreateItemDistribution();

public:

	uint** CreateDimDistribution(uint** uniqueValues, char* minValues, char* maxValues);
	uint* CreateItemDistribution(uint* uniqueItems);
	void DeleteDimDistribution();

	static const tNodeIndex TREE_ROOT_INDEX_INIT = 0;
	static const int TREE_NOT_A_TREE_FILE = 1;
	static const int TREE_FILE_NOT_OPEN = 2;	
	static const int MAX_TREE_HEIGHT = 24;
	
	cPagedTree();
	~cPagedTree();

	bool Create(cTreeHeader *header, cQuickDB* quickDB);
	bool Open(cTreeHeader *header, cQuickDB* quickDB, bool readOnly);
	bool Flush();
	bool Close();
	bool Clear();

	void Preload();
	void Preload(const tNodeIndex& nodeIndex);

	void Insert(const TItem &item);
	bool Delete(const TItem &item);
	bool Update(const TItem &item, const TItem &newItem);
	bool Find(const TItem &item) const;

	inline cTreeHeader* GetHeader();

	void PrintInfo() const;
	inline void SetDebug(bool debug);
	inline bool IsOpen() const				{ return mIsOpen; }

	inline uint GetIndexSize(uint blockSize);
	inline float GetIndexSizeMB(uint blockSize);

	// for ri purpose
	void PrintSubNodesDistribution(char* fileName);
	void ComputeSubNodesDistribution(const tNodeIndex& nodeIndex, uint level, cHistogram* hist);

	// for dimension distribution histograms
	void ComputeDimDistribution(const tNodeIndex& nodeIndex, uint level, uint dimension);
	//void PrintDimDistribution();
	void PrintDimDistribution(char* minValues, char* maxValues);
	
	void ComputeItemDistribution(const tNodeIndex& nodeIndex, uint level);

	virtual uint RangeQuery_preSize(unsigned int *nofTrueBitOrders, sBatchRQ *batchRQ);
	virtual char* RangeQuery_preAlloc(unsigned int *nofTrueBitOrders, sBatchRQ *batchRQ, char* buffer, cRQBuffers<TItem>* rqBuffers);
};

/**
 * If createFlag the tree is created according to defined header. Otherwise some properties are initialized
 *   (called before calling Open() method).
 */
template<class TItem, class TNode, class TLeafNode>
cPagedTree<TItem,TNode,TLeafNode>::cPagedTree()
  : mMemory(NULL), mStatus(0), mIsOpen(false), mDebug(false)
{
}

template<class TItem, class TNode, class TLeafNode>
cPagedTree<TItem,TNode,TLeafNode>::~cPagedTree()
{
	Close();
}

/**
* Create new data structure.
* \param header Data structure header, which is prepared for the data structure creation.
* \param sharedCache Opened cache.
*/
template<class TItem, class TNode, class TLeafNode> 
bool cPagedTree<TItem,TNode,TLeafNode>::Create(cTreeHeader *header, cQuickDB* quickDB)
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
	mMemoryManager = mQuickDB->GetMemoryManager();
#ifdef CUDA_ENABLED
	mMemoryManagerCuda = mQuickDB->GetMemoryManagerCuda();
#endif
	mHeader = header;

	Init();  // init pools, results, and so on
	mSharedCache->Register(mHeader);
	mLeafNodeId = mHeader->GetNodeType(cTreeHeader::HEADER_LEAFNODE);
	mInnerNodeId = mHeader->GetNodeType(cTreeHeader::HEADER_NODE);

	mIsOpen = true;
	ret = true;

/*	if (header->IsHistogramEnabled())
	{
		uint dimension = TItem::GetDimension(mHeader->GetNodeHeader(cTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor());
		mTreeHistogram = new cHistogram*[dimension];
		for (uint i = 0; i < dimension; i++)
		{
			mTreeHistogram[i] = new cHistogram(cDStructConst::MAX_HIST_VALUE, true);
		}
	}*/
	return ret;
}

/**
* Open existing data structure.
* \param header Data structure header. All values will be read from the secondary storage. Only the node headers has to be preallocated and the data structure name has to be properly set.
* \param sharedCache Opened cache.
*/
template<class TItem, class TNode, class TLeafNode>
bool cPagedTree<TItem,TNode,TLeafNode>::Open(cTreeHeader *header, cQuickDB* quickDB, bool readOnly)
{
	bool ret;

	mQuickDB = quickDB;

	if (!mQuickDB->IsOpened())
	{
		printf("cPagedTree::Open - quickDB is not opened!\n");
		exit(1);
	}
	mReadOnly = readOnly;
	mSharedCache = mQuickDB->GetNodeCache();
	mMemoryPool = mQuickDB->GetMemoryPool();
	mMemoryManager = mQuickDB->GetMemoryManager();
#ifdef CUDA_ENABLED
	mMemoryManagerCuda = mQuickDB->GetMemoryManagerCuda();
#endif
	mHeader = header;
	ret = mSharedCache->LookForHeader(mHeader);

	Init();  // resize pools, results, and so on

	mLeafNodeId = mHeader->GetNodeType(0);
	mInnerNodeId = mHeader->GetNodeType(1);

/*	if (header->IsHistogramEnabled())
	{
		uint dimension = TItem::GetDimension(mHeader->GetNodeHeader(cTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor());
		mTreeHistogram = new cHistogram*[dimension];
		for (uint i = 0; i < dimension; i++)
		{
			mTreeHistogram[i] = new cHistogram(cDStructConst::MAX_HIST_VALUE, true);
		}
	}*/

	return ret;
}

template<class TItem, class TNode, class TLeafNode>
void cPagedTree<TItem,TNode,TLeafNode>::Init()
{
	mHeader->SetMemoryPool(mMemoryPool);
	mHeader->SetMemoryManager(mMemoryManager);
#ifdef CUDA_ENABLED
	mHeader->SetMemoryManagerCuda(mMemoryManagerCuda);
#endif
}

/**
 * UseCase: In the case of a temporary DS (B-tree for example), we want to sort a set of records, after this,
 *          we want to store these sorted record on disk, clear this DS, and sort another set of records.
 *          And this is the method for clearing DS. Evidently, It is not possible to combine QuickDB instance 
 *          with the temporary DS with another QucikDB instance with non-temporary DS.
 */
template<class TItem, class TNode, class TLeafNode> 
bool cPagedTree<TItem,TNode,TLeafNode>::Clear()
{
	// TODO - neni jasne co ma tato metoda delat. ODPOVED: Ale napsal jsi to dobre, gratuluji!
	bool ret = false;
	mReadOnly = false;
	mStatus = 0;

	// clear the data structure header
	// mHeader->SetTopNodeIndex((unsigned int)~0);
	mHeader->ResetItemCount();
	mHeader->ResetNodeCount();
	// TODO - actualize the header in the secondary storage

	// mQueryStatistics->Reset();
	mSharedCache->Clear(); // TODO clear only for this data structure

	mIsOpen = true;
	ret = true;
	return ret;
}


template<class TItem, class TNode, class TLeafNode> 
bool cPagedTree<TItem,TNode,TLeafNode>::Flush() 
{
	//mCharStream.Seek(0);
	////mHeader->DefaultWrite(&mCharStream);
	//mHeader->Write(&mCharStream);
	//mStream->Seek(0);
	//mCharStream.Write(mStream, mHeader->GetSize());
	//mCache->Flush();


	return true;
}

template<class TItem, class TNode, class TLeafNode> 
bool cPagedTree<TItem,TNode,TLeafNode>::Close()
{	
	// Todo - zavreni pouze datove struktury, tedy odstraneni z cache tech uzlu, ktere nalezeji teto datove strukture.
	//mSharedCache->Close();
	return true;
}


template<class TItem, class TNode, class TLeafNode>
void cPagedTree<TItem, TNode, TLeafNode>::Preload()
{
	Preload(mHeader->GetRootIndex());
}

template<class TItem, class TNode, class TLeafNode>
void cPagedTree<TItem, TNode, TLeafNode>::Preload(const tNodeIndex& nodeIndex)
{
	TNode* currentNode = NULL;
	TLeafNode* currentLeafNode = NULL;

	if (TNode::IsLeaf(nodeIndex))
	{
		currentLeafNode = ReadLeafNodeR(nodeIndex);
		mSharedCache->UnlockR(currentLeafNode);
	}
	else
	{
		currentNode = ReadInnerNodeR(nodeIndex);

		for (unsigned int i = 0; i < currentNode->GetItemCount(); i++)
		{
			Preload(currentNode->GetLink(i));
		}
		mSharedCache->UnlockR(currentNode);
	}
}

template<class TItem, class TNode, class TLeafNode>
void cPagedTree<TItem, TNode, TLeafNode>::Insert(const TItem &item)
{
/*	if (mHeader->IsHistogramEnabled())
	{
		item.AddToHistogram(mTreeHistogram, mHeader->GetNodeHeader(cTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor());
	}*/

}

/*
 * Return the size of the index in bytes.
 */
template<class TItem, class TNode, class TLeafNode>
uint cPagedTree<TItem,TNode,TLeafNode>::GetIndexSize(uint blockSize)
{
	return mHeader->GetNodeCount() * blockSize;
}

/*
 * Return the size of the index in MB.
 */
template<class TItem, class TNode, class TLeafNode>
float cPagedTree<TItem,TNode,TLeafNode>::GetIndexSizeMB(uint blockSize)
{
	const uint mb = 1024 * 1024;
	return (float)GetIndexSize(blockSize) / mb;
}

/**
* Read new inner node
*/
template<class TItem, class TNode, class TLeafNode>
TNode* cPagedTree<TItem,TNode,TLeafNode>::ReadNewInnerNode()
{
	TNode* node = (TNode*)mSharedCache->ReadNew(mInnerNodeId);
	node->SetLeaf(false);
	node->Init(); 
	return node;
}

/**
* Read new leaf node
*/
template<class TItem, class TNode, class TLeafNode>
TLeafNode* cPagedTree<TItem,TNode,TLeafNode>::ReadNewLeafNode()
{
	TLeafNode* node = (TLeafNode*)mSharedCache->ReadNew(mLeafNodeId);
	node->SetLeaf(true);
	node->Init();
	return node;
}

template<class TItem, class TNode, class TLeafNode>
TNode* cPagedTree<TItem,TNode,TLeafNode>::ReadInnerNodeW(unsigned int index)
{
	TNode* node = (TNode*)mSharedCache->ReadW(index, mInnerNodeId);
	return node;
}

template<class TItem, class TNode, class TLeafNode>
TNode* cPagedTree<TItem,TNode,TLeafNode>::ReadInnerNodeR(unsigned int index)
{
	//val644 - start - increment readNodesInLevel
	cTuple::readNodesInLevel[cTuple::levelTree]++;
	//val644 - end - increment readNodesInLevel

	TNode* node = (TNode*)mSharedCache->ReadR(index, mInnerNodeId);
	return node;
}

template<class TItem, class TNode, class TLeafNode>
TLeafNode* cPagedTree<TItem,TNode,TLeafNode>::ReadLeafNodeW(unsigned int index)
{
	TLeafNode* node = (TLeafNode*)mSharedCache->ReadW(TNode::GetNodeIndex(index), mLeafNodeId);
	return node;
}

template<class TItem, class TNode, class TLeafNode>
TLeafNode* cPagedTree<TItem,TNode,TLeafNode>::ReadLeafNodeR(unsigned int index)
{
	//val644 - start - increment readNodesInLevel
	cTuple::readNodesInLevel[cTuple::levelTree]++;
	//val644 - end - increment readNodesInLevel

	TLeafNode* node = (TLeafNode*)mSharedCache->ReadR(TNode::GetNodeIndex(index), mLeafNodeId);
	//node->SetRealSize(node->GetInMemorySize());
	return node;
}

template<class TItem, class TNode, class TLeafNode>
inline cTreeHeader* cPagedTree<TItem,TNode,TLeafNode>::GetHeader() 
{ 
	return mHeader; 
};

template<class TItem, class TNode, class TLeafNode> 
inline void cPagedTree<TItem,TNode,TLeafNode>::SetDebug(bool debug)
{
	mDebug = debug;
}

/**
 * Print base information about tree.
 */
template<class TItem, class TNode, class TLeafNode> 
void cPagedTree<TItem,TNode,TLeafNode>::PrintInfo() const
{
	printf("****************************** Tree statistics: *******************************\n");
	printf("Height:                %d\n", mHeader->GetHeight());
	printf("Inner item capacity:   %d\t Leaf item capacity:   %d\n", mHeader->GetNodeItemCapacity(), mHeader->GetLeafNodeItemCapacity());
	printf("Inner fanout capacity: %d\t Leaf fanout capacity: %d\n", mHeader->GetNodeFanoutCapacity(), mHeader->GetLeafNodeFanoutCapacity());
	printf("Item count:            %d\t (inner items: %d\t+  leaf items: %d)\n", mHeader->GetItemCount(), mHeader->GetInnerItemCount(), mHeader->GetLeafItemCount());
	printf("Node count:            %d\t (inner nodes: %d\t+  leaf nodes: %d)\n", mHeader->GetNodeCount(), mHeader->GetInnerNodeCount(), mHeader->GetLeafNodeCount());
	printf("Average utilization:   %2.1f%%\t (inner nodes: %2.1f%% \t;  leaf nodes: %2.1f%%)\n", mHeader->AverageNodeUtilization(), mHeader->AverageInnerNodeUtilization(), mHeader->AverageLeafNodeUtilization());
	// printf("Node size:             %dB\n", mHeader->GetNodeSize());
	printf("Inner node Serial size:  %dB\t Leaf node Serial size:  %dB\n", mHeader->GetNodeSerialSize(), mHeader->GetLeafNodeSerialSize());
	printf("Inner node InMem size:  %dB\t Leaf node InMem size:  %dB\n", mHeader->GetNodeInMemSize(), mHeader->GetLeafNodeInMemSize());
	printf("Inner node item size:  %dB\t Leaf node item size:  %dB\n", mHeader->GetNodeItemSize(), mHeader->GetLeafNodeItemSize());
	printf("Cache size [node]:     %d\n", mSharedCache->GetCacheNodeSize());
}


template<class TItem, class TNode, class TLeafNode>
uint** cPagedTree<TItem, TNode, TLeafNode>::CreateDimDistribution(uint** uniqueValues, char* minValues, char* maxValues)
{
	const cDTDescriptor * sd = mHeader->GetNodeHeader(cTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor();
	uint dimension = TItem::GetDimension(sd);

	// create histograms
	mTreeHistogram = new cHistogram*[dimension];
	for (uint i = 0; i < dimension; i++)
	{
		mTreeHistogram[i] = new cHistogram(TItem::GetUInt(minValues, i, (cSpaceDescriptor*)sd), TItem::GetUInt(maxValues, i, (cSpaceDescriptor*)sd), mHeader->GetItemCount(), cHistogram::BIT_HISTOGRAM_DENSE);
	}

	mNodeHistogram = new cHistogram(mHeader->GetHeight() + 1);
	mItemHistogram = new cHistogram**[mHeader->GetHeight() + 1];
	uniqueValues = new uint*[mHeader->GetHeight() + 1];
	for (uint i = 0; i < mHeader->GetHeight() + 1; i++)
	{
		uniqueValues[i] = new uint[dimension];
		mItemHistogram[i] = new cHistogram*[dimension];
		for (uint j = 0; j < dimension; j++)
		{
			mItemHistogram[i][j] = new cHistogram(TItem::GetUInt(minValues, j, (cSpaceDescriptor*)sd), TItem::GetUInt(maxValues, j, (cSpaceDescriptor*)sd), mHeader->GetItemCount(), cHistogram::BIT_HISTOGRAM_DENSE);
			uniqueValues[i][j] = 0;
		}
	}

	mUniqueValuesCount = uniqueValues;

	// compute histograms
	ComputeDimDistribution(mHeader->GetRootIndex(), 0, dimension);
	
	return uniqueValues;
}

template<class TItem, class TNode, class TLeafNode>
uint* cPagedTree<TItem, TNode, TLeafNode>::CreateItemDistribution(uint* uniqueItems)
{
	const cDTDescriptor * sd = mHeader->GetNodeHeader(cTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor();
	uint dimension = TItem::GetDimension(sd);

	uniqueItems = new uint[mHeader->GetHeight() + 1];
	for (uint i = 0; i < mHeader->GetHeight() + 1; i++)
	{
		uniqueItems[i] = 0;
	}

	mItemsCount = uniqueItems;

	// compute histograms
	ComputeItemDistribution(mHeader->GetRootIndex(), 0);
	
	return uniqueItems;
}


template<class TItem, class TNode, class TLeafNode>
void cPagedTree<TItem, TNode, TLeafNode>::CreateDimDistribution(char* minValues, char* maxValues)
{
	const cDTDescriptor * sd = mHeader->GetNodeHeader(cTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor();
	uint dimension = TItem::GetDimension(sd);

	// create histograms
	mTreeHistogram = new cHistogram*[dimension];
	for (uint i = 0; i < dimension; i++)
	{
		mTreeHistogram[i] = new cHistogram(TItem::GetUInt(minValues, i, (cSpaceDescriptor*) sd), TItem::GetUInt(maxValues, i, (cSpaceDescriptor*) sd), mHeader->GetItemCount(), cHistogram::BIT_HISTOGRAM_DENSE);
	}

	mNodeHistogram = new cHistogram(mHeader->GetHeight() + 1);
	mItemHistogram = new cHistogram**[mHeader->GetHeight() + 1];
	mUniqueValuesCount = new uint*[mHeader->GetHeight() + 1];
	mItemsCount = new uint[mHeader->GetHeight() + 1];
	for (uint i = 0; i < mHeader->GetHeight() + 1; i++)
	{
		mUniqueValuesCount[i] = new uint[dimension];
		mItemHistogram[i] = new cHistogram*[dimension];
		mItemsCount[i] = 0;
		for (uint j = 0; j < dimension; j++)
		{
			mItemHistogram[i][j] = new cHistogram(TItem::GetUInt(minValues, j, (cSpaceDescriptor*) sd), TItem::GetUInt(maxValues, j, (cSpaceDescriptor*) sd), mHeader->GetItemCount(), cHistogram::BIT_HISTOGRAM_DENSE);
			mUniqueValuesCount[i][j] = 0;
		}
	}

	// compute histograms
	ComputeDimDistribution(mHeader->GetRootIndex(), 0, dimension);
}

template<class TItem, class TNode, class TLeafNode>
void cPagedTree<TItem, TNode, TLeafNode>::CreateItemDistribution()
{
	mItemsCount = new uint[mHeader->GetHeight() + 1];
	for (uint i = 0; i < mHeader->GetHeight() + 1; i++)
	{
		mItemsCount[i] = 0;
	}

	// compute histograms
	ComputeItemDistribution(mHeader->GetRootIndex(), 0);
}

template<class TItem, class TNode, class TLeafNode>
void cPagedTree<TItem, TNode, TLeafNode>::DeleteDimDistribution()
{
	const cDTDescriptor * sd = mHeader->GetNodeHeader(cTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor();
	uint dimension = TItem::GetDimension(sd);

	// delete histograms
	for (uint i = 0; i < dimension; i++)
	{
		delete mTreeHistogram[i];
	}
	delete mTreeHistogram;

	for (uint i = 0; i < mHeader->GetHeight() + 1; i++)
	{
		for (uint j = 0; j < dimension; j++)
		{
			delete mItemHistogram[i][j];
		}
		delete mItemHistogram[i];
	}
	delete mItemHistogram;
	delete mNodeHistogram;
}

template<class TItem, class TNode, class TLeafNode>
void cPagedTree<TItem, TNode, TLeafNode>::PrintDimDistribution(char* minValues, char* maxValues)
{
	if (mHeader->GetDStructMode() == cDStructConst::DSMODE_CODING ||
		mHeader->GetDStructMode() == cDStructConst::DSMODE_RI ||
		mHeader->GetDStructMode() == cDStructConst::DSMODE_RICODING)
	{
		printf("******************************** Histograms: **********************************\n");
		printf("Error:: Histograms are implemented only for non - compressed tree structures !!\n\n");
		return;
	}

	CreateDimDistribution(minValues, maxValues);

	const cDTDescriptor * sd = mHeader->GetNodeHeader(cTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor();
	uint dimension = TItem::GetDimension(sd);

	// print histograms
	printf("******************************** Histograms: **********************************\n");
	printf("#Total Dim Distribution: ");
	for (uint i = 0; i < dimension; i++)
	{
		//mHistogram[i]->Print2File("test.txt", "\nHistogram\n");
		printf("%d; ", mTreeHistogram[i]->GetUniqueValuesCount());
	}
	printf("\n");

	for (uint i = 0; i < mHeader->GetHeight() + 1; i++)
	{
		printf("#L %d:	#Nodes: %d	Avg D.:", i, mNodeHistogram->GetCount(i));
		for (uint j = 0; j < dimension; j++)
		{
			printf("%.2f; ", mUniqueValuesCount[i][j] / (float) mNodeHistogram->GetCount(i));
		}
		printf("\n");
	}
	printf("\n");

	DeleteDimDistribution();
}

template<class TItem, class TNode, class TLeafNode>
void cPagedTree<TItem, TNode, TLeafNode>::ComputeDimDistribution(const tNodeIndex& nodeIndex, uint level, uint dimension)
{
	TNode* currentNode = NULL;
	TLeafNode* currentLeafNode = NULL;

	mNodeHistogram->AddValue(level);
	if (TNode::IsLeaf(nodeIndex))
	{
		currentLeafNode = ReadLeafNodeR(nodeIndex);
		currentLeafNode->ComputeDimDistribution(mTreeHistogram);
		
		for (uint i = 0; i <= level; i++)
		{
			currentLeafNode->ComputeDimDistribution(mItemHistogram[i]);
		}

		for (uint i = 0; i < dimension; i++)
		{
			mUniqueValuesCount[level][i] += mItemHistogram[level][i]->GetUniqueValuesCount();
			mItemHistogram[level][i]->ClearHistogram();
		}

		//if (mNodeHistogram->GetCount(level) % 1000 == 0)
		{
			printf("Processed leaf nodes: %d\r", mNodeHistogram->GetCount(level));
			fflush(stdout);
		}
		mSharedCache->UnlockR(currentLeafNode);
	}
	else
	{
		currentNode = ReadInnerNodeR(nodeIndex);

		for (uint i = 0; i < currentNode->GetItemCount(); i++)
		{
			ComputeDimDistribution(currentNode->GetLink(i), level + 1, dimension);
		}

		for (uint i = 0; i < dimension; i++)
		{
			mUniqueValuesCount[level][i] += mItemHistogram[level][i]->GetUniqueValuesCount();
			mItemHistogram[level][i]->ClearHistogram();
		}
		mSharedCache->UnlockR(currentNode);
	}
}

template<class TItem, class TNode, class TLeafNode>
void cPagedTree<TItem, TNode, TLeafNode>::ComputeItemDistribution(const tNodeIndex& nodeIndex, uint level)
{
	TNode* currentNode = NULL;
	TLeafNode* currentLeafNode = NULL;

	if (TNode::IsLeaf(nodeIndex))
	{
		currentLeafNode = ReadLeafNodeR(nodeIndex);
		for (uint i = 0; i <= level; i++)
			mItemsCount[i] += currentLeafNode->GetItemCount(); // DO NOT COUNT WITH DUPLICITIES
		mSharedCache->UnlockR(currentLeafNode);
	}
	else
	{
		currentNode = ReadInnerNodeR(nodeIndex);
		for (uint i = 0; i < currentNode->GetItemCount(); i++)
		{
			ComputeItemDistribution(currentNode->GetLink(i), level + 1);
		}
		mSharedCache->UnlockR(currentNode);
	}
}

/*template<class TItem, class TNode, class TLeafNode>
void cPagedTree<TItem, TNode, TLeafNode>::PrintDimDistribution()
{
	if (mHeader->IsHistogramEnabled())
	{
		uint dimension = TItem::GetDimension(mHeader->GetNodeHeader(cTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor());
		//uint dimension = GetSpaceDescriptor()->GetDimension();

		printf("#Dim Distribution: ");
		for (uint i = 0; i < dimension; i++)
		{
			//mHistogram[i]->Print2File("test.txt", "\nHistogram\n");
			printf("%d; ", mTreeHistogram[i]->GetUniqueValuesCount());
		}
		printf("\n");
	}
}*/

template<class TItem, class TNode, class TLeafNode>
void cPagedTree<TItem, TNode, TLeafNode>::PrintSubNodesDistribution(char* fileName)
{
	cHistogram hist(200);

	ComputeSubNodesDistribution(mHeader->GetRootIndex(), 0, &hist);

	hist.Print2File(fileName);
}


template<class TItem, class TNode, class TLeafNode>
void cPagedTree<TItem, TNode, TLeafNode>::ComputeSubNodesDistribution(const tNodeIndex& nodeIndex, uint level, cHistogram* hist)
{
	TNode* currentNode = NULL;
	TLeafNode* currentLeafNode = NULL;

	if (TNode::IsLeaf(nodeIndex))
	{
		currentLeafNode = ReadLeafNodeR(nodeIndex);
		currentLeafNode->ComputeSubNodesDistribution(hist);
		mSharedCache->UnlockR(currentLeafNode);
	}
	else
	{
		currentNode = ReadInnerNodeR(nodeIndex);

		for (uint i = 0; i < currentNode->GetItemCount(); i++)
		{
			ComputeSubNodesDistribution(currentNode->GetLink(i), level + 1, hist);
		}

		mSharedCache->UnlockR(currentNode);
	}
}

template<class TItem, class TNode, class TLeafNode>
uint cPagedTree<TItem, TNode, TLeafNode>::RangeQuery_preSize(unsigned int *nofTrueBitOrders, sBatchRQ *batchRQ)
{
	//printf("cPagedTree<TItem, TNode, TLeafNode>::uintRangeQuery_preSize: Not implemented!");
	return 0;
}

template<class TItem, class TNode, class TLeafNode>
char* cPagedTree<TItem, TNode, TLeafNode>::RangeQuery_preAlloc(unsigned int *nofTrueBitOrders, sBatchRQ *batchRQ, char* buffer, cRQBuffers<TItem>* rqBuffers)
{
	//printf("cPagedTree<TItem, TNode, TLeafNode>::RangeQuery_preAlloc: Not implemented!");
	return buffer;
}

}}}
#endif
