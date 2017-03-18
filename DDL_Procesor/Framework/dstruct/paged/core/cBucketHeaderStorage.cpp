#include "dstruct/paged/core/cBucketHeaderStorage.h"

namespace dstruct {
  namespace paged {
	namespace core {

/// Constructor
/// \param size the number of Nodes
cBucketHeaderStorage::cBucketHeaderStorage(unsigned int capacity)
: mBucketHeader(NULL), mBucketArrayIndex(NULL), mBucketArrayIndex_memory(NULL), mBucketQueue(NULL)
{
	Null();

	mCapacity = capacity;
	mSize = 0;
	mEmptyQueueTreshold = 0.5 * capacity;
	mBucketHeader = new cBucketHeader[capacity];

	mBucketArrayIndex_memory = new char[cBucketHashTable::GetSize(capacity)];
	mBucketArrayIndex = new (mBucketArrayIndex_memory)cBucketHashTable(capacity, mBucketArrayIndex_memory);

	mBucketQueue = new cLinkedList<unsigned int>(capacity);
}

/// Destructor
cBucketHeaderStorage::~cBucketHeaderStorage()
{
	if (mBucketArrayIndex_memory != NULL)
	{
		delete mBucketArrayIndex_memory;
		mBucketArrayIndex_memory = NULL;
		mBucketArrayIndex = NULL;
	}
	if (mBucketHeader != NULL)
	{
		delete []mBucketHeader;
		mBucketHeader = NULL;
	}
	if (mBucketQueue != NULL)
	{
		delete mBucketQueue;
		mBucketQueue = NULL;
	}
}

void cBucketHeaderStorage::Null()
{
	mBucketArrayIndex = NULL;
	mBucketQueue = NULL;
	mBucketHeader = NULL;
}

/// Reset all Node records
void cBucketHeaderStorage::Clear()
{
	mBucketQueue->Clear();

	for (unsigned int i = 0 ; i < mCapacity ; i++)
	{
		mBucketHeader[i].Clear(i);
		mBucketQueue->AddItem(i);
	}

	mBucketArrayIndex->Clear();
	mSize = 0;
}

void cBucketHeaderStorage::Clear(unsigned int i)
{
	mBucketHeader[i].Clear(i);
}

/*
 * \return true if the node is in the index (and cache), otherwise return false
 */
bool cBucketHeaderStorage::FindBucket(const tNodeIndex &nodeIndex, cBucketHeader **bucketHeader)
{
	// try to find the node in the index
	bool nodeFound = true;
	unsigned int bucketOrder;
	cLinkedListNode<unsigned int> *bucketQueueNode = NULL;
	
	// the node is not found, get the first bucket in the queue
	// it an empty bucket or the oldest bucket
	if (!mBucketArrayIndex->Find(nodeIndex, bucketOrder))
	{
		bucketQueueNode = mBucketQueue->GetDeleteHeadNode();
		bucketOrder = bucketQueueNode->Item;
		nodeFound = false;
	}

	// get the header for the bucket found
	*bucketHeader = &(mBucketHeader[bucketOrder]);
	if (!nodeFound)
	{
		// set the new bucket queue node
		(*bucketHeader)->SetBucketQueueNode(bucketQueueNode);
	}
	else
	{
		// if the bucket queue node is in the queue then delete it from the queue
		// since this node has not to be rewrittten by another node
		mBucketQueue->DeleteNode((*bucketHeader)->GetBucketQueueNode());
	}
	
	return nodeFound;
}

/*
 * \return true if the node is in the index (and cache), otherwise return false
 */
bool cBucketHeaderStorage::FindNode(const tNodeIndex &nodeIndex, cBucketHeader **bucketHeader)
{
	// try to find the node in the index
	bool nodeFound = false;
	unsigned int bucketOrder;
	cLinkedListNode<unsigned int> *bucketQueueNode = NULL;
	
	// the node is not found, get the first bucket in the queue
	// it an empty bucket or the oldest bucket
	if (mBucketArrayIndex->Find(nodeIndex, bucketOrder))
	{
		// get the header for the bucket found
		*bucketHeader = &(mBucketHeader[bucketOrder]);
		nodeFound = true;
	}

	return nodeFound;
}

void cBucketHeaderStorage::PutBackInBucketQueue(const cBucketHeader *bucketHeader)
{
	// only if the number of items in the queue < treshold, return the node of the bucket into the queue
	if (mBucketQueue->GetItemCount() < mEmptyQueueTreshold)
	{
		mBucketQueue->AddNode(bucketHeader->GetBucketQueueNode());
	}
}

/*
unsigned int cBucketHeaderStorage::GetFreeNodeOrder(const tNodeIndex &nodeIndex)
{
	unsigned int nodeOrder;

	if (mSize < mCapacity)
	{
		// a bucket is free?
		nodeOrder = mFreeNodes->GetFreeNodeRecordOrder();

		if (nodeOrder < mCapacity && nodeOrder != -1)
		{
			mSize++;
			mTimestampSortedArray->FindNode(&mBucketHeader[nodeOrder]);  // set the first unlock bucket for ChangeNodeIndexOfBucket()
		} 
		else 
		{
			nodeOrder = mTimestampSortedArray->GetFirstUnlockNode()->GetOrder();
		}
	}
	else
	{
		// no, you must find the bucket with the oldest unlock bucket
		nodeOrder = mTimestampSortedArray->GetFirstUnlockNode()->GetOrder();
	}

	mBucketHeader[nodeOrder].SetNodeIndex(nodeIndex);

	return nodeOrder;
}*/


/// Set the timestamp for the Node, the data structures must be sorted
/*
void cBucketHeaderStorage::SetTimestamp(unsigned int bucketOrder, ullong timestamp)
{
	mBucketHeader[bucketOrder].SetTimestamp(timestamp);
	mTimestampSortedArray->MoveAccessNodeToEnd(bucketOrder);
}
*/

void cBucketHeaderStorage::DeleteFromBucketIndex(const tNodeIndex &nodeIndex)
{
	mBucketArrayIndex->Delete(nodeIndex);
}

void cBucketHeaderStorage::AddInBucketIndex(const tNodeIndex &nodeIndex, const unsigned int &bucketOrder)
{
	mBucketArrayIndex->Add(nodeIndex, bucketOrder);
}

/// Find Node record for the Node.
/*
cBucketHeader* cBucketHeaderStorage::GetPNodeRecordIndex(const tNodeIndex &nodeIndex)
{
	cBucketHeader *record = NULL;
	unsigned int nodeOrder = mBucketArrayIndex->GetNodeOrder(nodeIndex);
	// nodeOrder = mIndexSortedArray->GetNodeOrder(nodeIndex, mIndexInSortedArray, mSize);

	if (nodeOrder != cBucketHeaderSortedArray::NOT_FOUND)
	{
		record = &mBucketHeader[nodeOrder];
	}
	return record;
}
*/

// bool cBucketHeaderStorage::CheckArrays()
// {
	// return mBucketArrayIndex->CheckHashArray();
// }

void cBucketHeaderStorage::CheckQueue() const
{
	// mBucketQueue->Check();
	
	for (unsigned int i = 0 ; i < mCapacity ; i++)
	{
		cLinkedListNode<unsigned int> *bucketQueueNode = mBucketHeader[i].GetBucketQueueNode();
		
		if (bucketQueueNode != NULL)
		{
			if (bucketQueueNode->Item > mCapacity)
			{
				int bla = 0;
			}
		}
	}
}

void cBucketHeaderStorage::Print() const
{
	// mBucketArrayIndex->Print();
}

void cBucketHeaderStorage::PrintLocks() const
{
	// mTimestampSortedArray->Print();
}

unsigned int cBucketHeaderStorage::GetNofLocks() const
{
	// return mTimestampSortedArray->GetNofLocks();
	return 0;
}

}}}