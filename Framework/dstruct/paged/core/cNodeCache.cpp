#include "dstruct/paged/core/cNodeCache.h"

namespace dstruct {
	namespace paged {
		namespace core {

/// Real write of node in the secondary storage.
void cNodeCache::RealNodeWrite(cNode &node, unsigned int nodeHeaderId)
{
	bool debug = false;
	tNodeIndex loNodeIndex = node.GetIndex(), hiNodeIndex = node.GetIndex();

	StartCollectStatistic(false, nodeHeaderId, false);

	StoreAdjacentNodes(node.GetIndex(), mMemStream, loNodeIndex, hiNodeIndex);

	//cCacheStatistics::BufferedWrite += (hiNodeIndex - loNodeIndex + 1);
	//cCacheStatistics::BufferedWriteCount++;
	//printf("Avg. Buffered Write: %.2f\n", (float)cCacheStatistics::BufferedWrite / cCacheStatistics::BufferedWriteCount);

	// TODO: mNodesHeadersArray[mNodeType]->GetNodeSize() se musí nahradit velikostí uzlu v cache
	// assert(mMemStream->GetSize() <= nodeHeader->GetNodeSerialSize());

	if (debug)
	{
		printf("cNodeCache::RealNodeWrite()\n");
		mMemStream->Print();
	}

	// Seek(node.GetIndex(), nodeHeaderId);
	if (!Seek(loNodeIndex))
	{
		throw("Critical Error: cNodeCache::RealNodeWrite(): Seek Failed!");
	}

	//if (mTreeHeader->GetCacheMeasureTime())
	//{
	//mCacheStatistics.GetRealWriteTimer()->Run();
	//}

	// bool ret = mMemStream->Write(mStream, nodeHeader->GetNodeSerialSize()); // second, store the char stream into stream
	// second, store the char stream into stream
	if (!mMemStream->Write(mStream, mBlockSize * (hiNodeIndex - loNodeIndex + 1)))
	{
		throw("Critical Error: cNodeCache::RealNodeWrite(): Node Writting Failed!\n");
	}
}

// try to find adjacent modified nodes
void cNodeCache::StoreAdjacentNodes(tNodeIndex nodeIndex, cCharStream* memStream, tNodeIndex &loNodeIndex, tNodeIndex &hiNodeIndex)
{
	int maxCount = 1; // MEMSTREAM_SIZE / mBlockSize;
	int count = 1;
	tNodeIndex currentNodeIndex = nodeIndex;
	bool loNodeReach = false, hiNodeReach = false;

	while (count < maxCount && loNodeIndex >= 1 && !(loNodeReach || hiNodeReach))
	{
		if (!loNodeReach)
		{
			if (IsNodeStorable(--loNodeIndex))
			{
				count++;
			}
			else
			{
				loNodeIndex++;
				loNodeReach = true;
			}
		}
		if (count < maxCount && !hiNodeReach)
		{
			if (IsNodeStorable(++hiNodeIndex))
			{
				count++;
			}
			else
			{
				hiNodeIndex--;
				hiNodeReach = true;
			}
		}
	}

	memStream->Seek(0);
	for (tNodeIndex i = loNodeIndex ; i <= hiNodeIndex ; i++)
	{
		unsigned int bucketOrder;
		cBucketHeader *bucketHeader;

		mNodeRecordStorage->FindNode(i, &bucketHeader);
		bucketOrder = bucketHeader->GetBucketOrder();
		bucketHeader->SetModified(false);
		cNode* node = mNodes[bucketOrder];
		cNodeHeader* nodeHeader = mNodesHeadersArray[node->GetHeaderId()];

		// nodeHeader->WriteNode(&node, mMemStream);
		nodeHeader->WriteNode(node, memStream);
		// anlignment to the block size
		uint seek = mBlockSize - (memStream->GetOffset() % mBlockSize);
		memStream->SeekAdd(seek);
		memStream->IncreaseSize(seek);
	}
}

bool cNodeCache::IsNodeStorable(tNodeIndex nodeIndex)
{
	bool ret = false;
	cBucketHeader *bucketHeader;

	bool bucketFound = mNodeRecordStorage->FindNode(nodeIndex, &bucketHeader);
	if (bucketFound && !bucketHeader->GetReadLock() && !bucketHeader->GetReadLock() && bucketHeader->GetModified())
	{
		ret = true;
	}
	return ret;
}

/// Real read of node from secondary storage.
void cNodeCache::RealNodeRead(const tNodeIndex index, unsigned int nodeHeaderId)
{
	cNodeHeader *nodeHeader = mNodesHeadersArray[nodeHeaderId];

	StartCollectStatistic(true, nodeHeaderId, false);

	Seek(index);

	if (!mNodeMemStream->Read(mStream, nodeHeader->GetNodeSerialSize()))
	{
		mNodeRecordStorage->Print();
		throw new cDSCriticalException("DSCriticalException: cNodeCache::RealNodeRead(): The whole node has not been read!");
	}

	if (mDebug)
	{
		printf("cNodeCache::RealNodeRead()\n");
		mNodeMemStream->Print();
	}

	// volani Read s Formatem na prislusne hlavicce	
	//node.SetHeader(mNodesHeadersArray[mNodeType]);
	//mNodesHeadersArray[mNodeType]->ReadNode(&node, mMemStream);
	//node.Read(mMemStream);
}
}}}