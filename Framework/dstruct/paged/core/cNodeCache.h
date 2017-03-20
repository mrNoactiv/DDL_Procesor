/**
*	\file cNodeCache.cpp
*	\author Radim Baca, David Bednar, Michal Kratky
*	\version 0.2
*	\date jul 2011
*	\brief Cache for a persistent data structure - cache is presented by char blocks.
*/

#ifndef __cNodeCache_h__
#define __cNodeCache_h__

#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <mutex>

#include "dstruct/paged/core/cTreeHeader.h"
#include "dstruct/paged/core/cNode.h"
#include "dstruct/paged/core/cNodeHeader.h"
#include "dstruct/paged/core/cDStructHeader.h"
#include "dstruct/paged/core/cCacheStatistics.h"
#include "dstruct/paged/core/cBucketHeaderStorage.h"
#include "dstruct/paged/core/cDSFalseSetupException.h"
#include "dstruct/paged/core/cDSCriticalException.h"
#include "common/memorystructures/cLinkedList.h"

#include "common/cObject.h"
#include "common/stream/cFileStream.h"
#include "common/stream/cCharStream.h"
#include "common/utils/cTimer.h"
#include "common/datatype/cBasicType.h"
#include "dstruct/paged/queryprocessing/cDbfsLevel.h"

using namespace common::stream;
using namespace common::utils;
using namespace common::memorystructures;

//#define MAINMEMORY_OPTIMIZATION
//#define WITHOUT_LOCKS

/**
* Cache for a persistent data structure - cache is presented by char blocks.
* Cache contains one data file where the nodes of data structured are stored.
*
* Cache takes care about the data structure headers as well. 
*
*  \author Radim Baca, David Bednar, Michal Kratky
*  \version 0.2
*  \date jul 2011
**/

namespace dstruct {
  namespace paged {
	namespace core {

class cNodeCache
{
protected:
	static const unsigned int MAX_DATA_FILE_NAME = 512;

	const static unsigned int MIN_CHACHE_SIZE = 3;
	const static unsigned int BLOCK_SIZE_CONST = 1024;
	const static unsigned int NODE_BUFFER_SIZE = 64;						// [nodes]
	const static unsigned int MEMSTREAM_SIZE = /* 16384 */ 65536; 			// [bytes]
	const static unsigned int HEADERS_ARRAY_SIZE = 5000;					// velikost pole pro hlavicky struktur
	const static unsigned int TREE_NODES_HEADERS_ARRAY_SIZE = 10000;		// velikost pole pro hlavicky uzlu

	typedef struct {
		char *mName;								/// Unique name of the data structure.
		cDStructHeader *mHeader;					/// Header of a data structure.
		unsigned int mSeek;							/// Pointer to the file where the mHeader is stored.
		unsigned int mSerializedSize;				/// Size of serialized mHeader.
	} sHeader;

	cNode** mNodes;                                 /// nodes of the cache buffer (buckets)
	cBucketHeaderStorage *mNodeRecordStorage;       /// storage for headers of the buckets
	char* mNodeMemory;                              /// memory of the buckets

	char mDataFileName[MAX_DATA_FILE_NAME];			/// Name of a data file.
	cCharStream* mMemStream;						/// Auxiliary buffer used during the node and headers serialization.
	cCharStream* mNodeMemStream;
	cFileStream *mStream;							/// The data file stream.
	unsigned int mCacheNodeSize;					/// Number of cache nodes in the main memory.
	unsigned int mMaxIndex;							/// Last node index used by a data structure
	unsigned int mBlockSize;						/// Size of a block on a secondary storage.
	unsigned int mMaxNodeInMemSize;                 /// The maximal size of each node in the main memory 

	sHeader *mHeaders;
	unsigned int mHeadersCount;						/// Number of data sructure headers currently mapped to the cache.
	unsigned int mHeadersSize;						/// Size of the mHeaders array.
	cNodeHeader **mNodesHeadersArray;				/// Array of node headers.
	unsigned int mNodesHeadersArrayCount;			/// Number of node headers in the cache.

	cCacheStatistics mCacheStatistics;

	bool mDebug;
	bool mVerbose;
	bool mIsOpened;								/// Is true of the node cache is opened.
	bool mReopened;
	bool mReadDebug;

	std::mutex mReadWriteMutex;

	// unsigned int SearchNode(const tNodeIndex nodeIndex, bool nodeFlag) const;
	inline virtual bool AdditionalTest(unsigned int index) const		{ return true; }
	// inline virtual cBucketHeader* GetNodeRecords(bool nodeFlag) const		{ return mNodeRecordStorage->GetNodeRecordArray(); }
	inline virtual unsigned int GetNodeIndex(bool nodeFlag, unsigned int order) const;
	// inline virtual unsigned int GetNodeTimestamp(bool nodeFlag, unsigned int order) const;
	inline bool Seek(tNodeIndex nodeIndex);

	void RealNodeWrite(cNode &node, unsigned int mNodeType);
	void RealNodeRead(const tNodeIndex index, unsigned int mNodeType);

	inline cNode* Read(const tNodeIndex index, bool readFlag, unsigned int mNodeType, cCharStream* bufferedNodes = NULL);
	inline void Unlock(cNode *node, bool readFlag);

	inline void ReadHeader(unsigned int order);
	inline void ReadHeaders();
	inline void WriteHeaders();
	inline void Open(const unsigned int cacheSize, unsigned int maxNodeInMemSize, unsigned int blockSize = cDStructHeader::DEFAULT_BLOCK_SIZE);

	inline void StartCollectStatistic(bool readFlag, unsigned int nodeType, bool logicalAccess);
	inline void StopCollectStatistic(unsigned int nodeType);
	inline void SetStatistic(bool readFlag, unsigned int nodeType, unsigned int accessType);

	void StoreAdjacentNodes(tNodeIndex nodeIndex, cCharStream* memStream, tNodeIndex &loNodeIndex, tNodeIndex &hiNodeIndex);
	bool IsNodeStorable(tNodeIndex nodeIndex);

public:
	inline cNodeCache();
	inline ~cNodeCache();

	inline bool Open(const char* fileName, unsigned int cacheSize, unsigned int maxNodeInMemSize, unsigned int blockSize = cDStructHeader::DEFAULT_BLOCK_SIZE);
	inline bool Create(const char* fileName, const unsigned int cacheSize, unsigned int maxNodeInMemSize, unsigned int blockSize = cDStructHeader::DEFAULT_BLOCK_SIZE);
	inline void Close(bool flushCache = true);
	inline void Clear(void);
	inline void Flush(void);

	inline cNode* ReadR(const tNodeIndex nodeIndex, unsigned int nodeHeaderId);
	inline cNode* ReadW(const tNodeIndex nodeIndex, unsigned int nodeHeaderId);
	inline cNode* ReadNew(unsigned int nodeHeaderId);
	inline void WriteNew(const cNode &node, unsigned int nodeHeaderId);
	inline void BulkRead(const cArray<tNodeIndex>* nodeIndexArray, unsigned int startIndex, unsigned int endIndex, unsigned int nodeHeaderId);
	inline void BulkRead(const cDbfsLevel* nodeIndexArray, unsigned int startIndex, unsigned int endIndex, unsigned int nodeHeaderId);
	inline void BulkRead(cDbfsLevel* buffer, cRangeQueryConfig* rqConfig, uint nodeHeaderId);
	// header related methods
	inline bool Register(cDStructHeader* treeHeader);
	inline bool LookForHeader(cDStructHeader* header);
	inline void Close(unsigned int mNodeType);
	inline unsigned int GetBlockSize() { return mBlockSize; }

	inline unsigned int GetMaxNodeInMemSize();

	inline virtual void UnlockR(cNode *node);
	inline virtual void UnlockW(cNode *node);
	inline cBucketHeader* GetBucketHeader(cNode *node);

	unsigned int GetNewIndex();
	inline unsigned int ActualNodeCountUsed();
	inline cCacheStatistics* GetCacheStatistics();
	inline unsigned int GetCacheNodeSize() const;
	inline int CheckLocks();
	inline void PrintLocks() const;
	inline unsigned int GetNofLocks() const;
	
	inline void Print() const;
	inline void PrintDataStructureStatistics();
	inline void PrintMemoryStatistics();

	inline bool IsReadDebug() const;
};

/// Constructor
cNodeCache::cNodeCache() :  mNodes(NULL), mNodeMemory(NULL), 
	mNodeRecordStorage(NULL), mCacheNodeSize(0), mMemStream(NULL), mStream(NULL),
	mDebug(false), mVerbose(false)
{
	mIsOpened = false;
}

cNodeCache::~cNodeCache()
{
	if (mIsOpened)
	{
		Close();
	}
}

/**
 * Open an existing cache. Allocate nodes of the cache and read the data structure headers.
 * \param fileName Name of the data file.
 * \param cacheSize Number of main memory pages preallocated for the cache.
 * \param maxNodeSize Main memory size of the node.
 **/
bool cNodeCache::Open(const char* fileName, 
					  const unsigned int cacheSize, 
					  unsigned int maxNodeInMemSize,
					  unsigned int blockSize)
{
	bool ret = true;
	mStream = new cFileStream();
	strcpy(mDataFileName, fileName);
	if (!mStream->Open(mDataFileName, ACCESS_READWRITE, FILE_OPEN, DIRECT_IO))
	{
		printf("cNodeCache::Open - Cache se nepodarilo otevrit!!\n");
		return false;
	}

	mIsOpened =	mReopened = true;
	mMaxNodeInMemSize = maxNodeInMemSize;

	mHeaders = new sHeader[HEADERS_ARRAY_SIZE];
	for (unsigned int i = 0; i < HEADERS_ARRAY_SIZE; i++)
	{
		mHeaders[i].mName = new char[cDStructHeader::HEADER_PATHSIZE];
		mHeaders[i].mHeader = NULL;
		mHeaders[i].mSeek = 0;
		mHeaders[i].mSerializedSize = 0;
	}
	mHeadersCount = 0;
	mNodesHeadersArray = new cNodeHeader*[TREE_NODES_HEADERS_ARRAY_SIZE];
	mNodesHeadersArrayCount = 0;

	ReadHeaders();
	Open(cacheSize, maxNodeInMemSize, blockSize);
	
	return ret;
}

/**
 * Open a new cache. Allocate nodes of the cache.
 * \param fileName Name of the data file.
 * \param cacheSize Number of main memory pages preallocated for the cache.
 * \param maxNodeSize Main memory size of the node.
 * \param blockSize Secondary storage size of the node (size of the page).
 **/
bool cNodeCache::Create(const char* fileName, 
						const unsigned int cacheSize, 
						unsigned int maxNodeInMemSize, 
						unsigned int blockSize)
{
	bool ret = true;
	mStream = new cFileStream();
	strcpy(mDataFileName, fileName);

	if (!mStream->Open(mDataFileName, ACCESS_READWRITE, FILE_CREATE, DIRECT_IO))
	{
		printf("cNodeCache::Create - Cache creation failed!!\n");
		return false;
	}

	mMaxIndex = 0;
	mIsOpened = true;
	mReopened = false;
	mMaxNodeInMemSize = maxNodeInMemSize;

	// vytvoreni jednotlivych poli hlavicek
	mHeaders = new sHeader[HEADERS_ARRAY_SIZE];
	for (unsigned int i = 0; i < HEADERS_ARRAY_SIZE; i++)
	{
		mHeaders[i].mName = new char[cDStructHeader::HEADER_PATHSIZE];
		mHeaders[i].mHeader = NULL;
		mHeaders[i].mSeek = 0;
		mHeaders[i].mSerializedSize = 0;
	}
	mHeadersCount = 0;
	mNodesHeadersArray = new cNodeHeader*[TREE_NODES_HEADERS_ARRAY_SIZE];
	mNodesHeadersArrayCount = 0;

	Open(cacheSize, maxNodeInMemSize, blockSize);
	return ret;
}


/**
 * Open a cache. Allocate nodes of the cache.
 * \param cacheSize Number of main memory pages preallocated for the cache.
 * \param maxNodeInMemSize Main memory size of the node.
 * \param blockSize Size of the block (page) on the secondary storage
 **/
void cNodeCache::Open(unsigned int cacheNodeSize, unsigned int maxNodeInMemSize, unsigned int blockSize)
{
	if (cacheNodeSize < MIN_CHACHE_SIZE)
	{
		cacheNodeSize = MIN_CHACHE_SIZE;
	}

	mBlockSize = blockSize;
	mMemStream = new cCharStream(MEMSTREAM_SIZE);
	mNodeMemStream = new cCharStream(mBlockSize * NODE_BUFFER_SIZE);

	mDebug = false;

	bool reallocCache = false;

	if (mNodes == NULL || mNodeMemory == NULL || mNodeRecordStorage == NULL || cacheNodeSize != mCacheNodeSize)
	{
		// clear memory if it is necessary
		if (mNodes != NULL)
		{
			delete []mNodes;
			mNodes = NULL;
		}
		if (mNodeMemory != NULL)
		{
			delete mNodeMemory;
			mNodeMemory = NULL;
		}
		if (mNodeRecordStorage != NULL)
		{
			delete mNodeRecordStorage;
			mNodeRecordStorage = NULL;
		}
		reallocCache = true;
	}

	mCacheNodeSize = cacheNodeSize;
	mMaxNodeInMemSize = maxNodeInMemSize;

	if (reallocCache)
	{
		int order = 0;

		mNodes = new cNode*[mCacheNodeSize];
		ullong completeNodeInMemSize = sizeof(cNode) + mMaxNodeInMemSize;
		ullong completeCacheInMemSize = completeNodeInMemSize * mCacheNodeSize;

		mNodeMemory = new char[completeCacheInMemSize];
		char* memptr = mNodeMemory;

		for (unsigned int j = 0; j < mCacheNodeSize; j++)
		{
			mNodes[j] = (cNode*)memptr;
			mNodes[j]->Init(mMaxNodeInMemSize, order++, memptr + sizeof(cNode));
			memptr += completeNodeInMemSize;
			mNodes[j]->SetIndex(cNode::EMPTY_INDEX);
		}
		mNodeRecordStorage = new cBucketHeaderStorage(mCacheNodeSize);
	}

	mNodeRecordStorage->Clear();
	mCacheStatistics.SetBlockSize(blockSize);
	mCacheStatistics.Reset();
}

/**
* Register the data structure
* \param treeHeader Header of the data structure being registered.
* \return 
*   - true if the header was successfuly registered and nodeIds are set.
*	- false if the header with the name already exist in the cache
*/
bool cNodeCache::Register(cDStructHeader* treeHeader)
{
	bool ret = false;

	if (!LookForHeader(treeHeader))
	{
		// pridani hlavicky datove struktury a hlavicek uzlu, nastaveni id uzlu
		mHeaders[mHeadersCount].mHeader = treeHeader;

		// TODO - it is necessary to implement own strcpy_s, there is a problem with the MS implementation
		int l = strlen(treeHeader->GetName());
		if (l > cDStructHeader::HEADER_PATHSIZE)
		{
			l = cDStructHeader::HEADER_PATHSIZE-1;
		}
		strncpy(mHeaders[mHeadersCount].mName, treeHeader->GetName(), l);
		*(mHeaders[mHeadersCount].mName + l) = '\0';
		mHeadersCount++;

		for (unsigned int i = 0; i < treeHeader->GetNodeHeaderCount(); i++)
		{
			treeHeader->SetNodeType(i, mNodesHeadersArrayCount);
			mNodesHeadersArray[mNodesHeadersArrayCount++] = treeHeader->GetNodeHeader(i);
		}
		ret = true;
	}

	// check the Serial and InMem Sizes
	for (unsigned int i = 0; i < treeHeader->GetNodeHeaderCount(); i++)
	{
		// validation
		if (treeHeader->GetNodeHeader(i)->GetNodeItemsSpaceSize() == (unsigned int)-1)
		{
			printf("cNodeCache::Register(): cNodeHeader::mNodeItemsSpaceSize is not defined!\n");
			throw new cDSFalseSetupException("DSFalseSetupException: cNodeCache::Register(): mNodeItemsSpaceSize of a NodeHeader is not defined!");
		}

		// if the block size >= Serial Size of the ds's nodes
		if (treeHeader->GetNodeHeader(i)->GetNodeSerialSize() > mBlockSize)
		{
			printf("cNodeCache::Register(): cNodeHeader::mNodeSerialSize (%d) > BlockSize (%d)!\n", treeHeader->GetNodeHeader(i)->GetNodeSerialSize(), mBlockSize);
			throw new cDSFalseSetupException("DSFalseSetupException: cNodeCache::Register(): SerialSize of %dth Node Header (%dB) >= the block size (%dB)!");
		}

		// if the maximal InMem size >= InMem Size of the ds's nodes
		int nims = treeHeader->GetNodeHeader(i)->GetNodeInMemSize();
		if (nims > mMaxNodeInMemSize)
		{
			printf("cNodeCache::Register(): cNodeHeader::mNodeInMemSize (%d) > MaxNodeInMemSize (%d)!\n", nims, mMaxNodeInMemSize);
			throw new cDSFalseSetupException("DSFalseSetupException: cNodeCache::Register(): InMemSize of %dth Node Header (%dB) >= the maximal node size (%dB)!");
		}
	}
	return ret;
}

/**
* Search for the data structure header in the cache.
* \param header Data structure header.
* \return
*  - true if the header with such a name already exist in the cache. The header is read from the file if it is necessary.
*  - false if the data structure name is not in the cache yet.
*/
inline bool cNodeCache::LookForHeader(cDStructHeader* header)
{
	for (unsigned int i = 0 ; i < mHeadersCount ; i++)
	{
		if (strcmp(header->GetName(), mHeaders[i].mName) == 0)
		{
			if (mHeaders[i].mHeader == NULL)
			{
				// read header from the header file
				mHeaders[i].mHeader = header;
				ReadHeader(i);
			} else
			{
				// TODO kontrola, ze se jedna skutecne o identicke hlavicky
				assert(header->GetNodeHeaderCount() == mHeaders[i].mHeader->GetNodeHeaderCount());
				printf("cNodeCache::LookForHeader - pokus o znovu otevření indexu\n");
			}			
			return true;
		}
	}

	return false;
}

/**
* Flushing of cache during closing
* - provede ulozeni a odstraneni vsech uzlu odpovidajicich nejakemu nodeHeaderId
*/
void cNodeCache::Close(unsigned int nodeHeaderId)
{
	if (mNodes != NULL && mStream != NULL)
	{
		for (unsigned int j = 0; j < mCacheNodeSize; j++)
		{
			// write only if node was modified
			if (mNodes[j]->GetHeaderId() == nodeHeaderId && mNodes[j]->GetIndex() != cNode::EMPTY_INDEX && 
				mNodeRecordStorage->GetBucketHeader(j)->GetModified())
			{
				// mCacheStatistics.GetTimer()->Run();
				mCacheStatistics.GetNodeDACWrite(cCacheStatistics::DAC_Logical).Increment();

				RealNodeWrite(*mNodes[j], nodeHeaderId);
				mNodeRecordStorage->GetBucketHeader(j)->SetModified(false);

				// mCacheStatistics.GetTimer()->Stop();
			}

			if (mNodes[j]->GetHeaderId() == nodeHeaderId)
			{
				mNodes[j]->SetIndex(cNode::EMPTY_INDEX);
			}
		}
	}
}

void cNodeCache::Close(bool flushCache)
{
	if (flushCache)
	{
		for (unsigned int i = mNodesHeadersArrayCount ; i > 0; i--)
		{
			cNodeCache::Close(i-1);
		}
		WriteHeaders();
	}
	mStream->Close();
	if (mStream != NULL)
	{
		delete mStream;
		mStream = NULL;
	}

	if (mMemStream != NULL)
	{
		delete mMemStream;
		mMemStream = NULL;
	}
	if (mNodeMemStream != NULL)
	{
		delete mNodeMemStream;
		mNodeMemStream = NULL;
	}
	if (mNodeRecordStorage != NULL)
	{
		delete mNodeRecordStorage;
		mNodeRecordStorage = NULL;
	}
	if (mHeaders != NULL)
	{
		for (unsigned int i = 0; i < mHeadersCount; i++)
		{
			mHeaders[i].mHeader = NULL;
		}

		for (unsigned int i = 0; i < HEADERS_ARRAY_SIZE; i++)
		{
			delete mHeaders[i].mName;
		}
		delete []mHeaders;
		mHeaders = NULL;
	}

	if (mNodesHeadersArray != NULL)
	{
		for (uint i = 0 ; i < TREE_NODES_HEADERS_ARRAY_SIZE ; i++)
		{
			mNodesHeadersArray[i] = NULL;
		}
		delete []mNodesHeadersArray;
		mNodesHeadersArray = NULL;
	}

	if (mNodes != NULL)
	{
		delete []mNodes;
		delete mNodeMemory;
		mNodes = NULL;
		mNodeMemory = NULL;
	}
	mIsOpened = false;
}

/**
* Write data structure headers into a headers file. Headers file is a separate file.
* All headers are written.
*/
void cNodeCache::WriteHeaders()
{
	cFileOutputStream *mHeaderStream = new cFileOutputStream();
	char headersFile[516];

	strcpy(headersFile, mDataFileName);
	strcat(headersFile, ".hdr");

	if (mHeaderStream->Open(headersFile, FILE_CREATE, FLAGS_NORMAL))
	{
		mHeaderStream->Seek(0);
		mHeaderStream->Write((char*)&mBlockSize, sizeof(mBlockSize));
		mHeaderStream->Write((char*)&mMaxIndex, sizeof(mMaxIndex));
		mHeaderStream->Write((char*)&mHeadersCount, sizeof(mHeadersCount));
		mHeaderStream->Write((char*)&mNodesHeadersArrayCount, sizeof(mNodesHeadersArrayCount));

		for (unsigned int i = 0 ; i < mHeadersCount; i++)
		{
			if (mHeaders[i].mHeader != NULL)
			{
				mMemStream->Seek(0);
				mHeaders[i].mHeader->Write(mMemStream);

				mMemStream->Seek(0);
				unsigned int size = mMemStream->GetSize();
				mHeaderStream->Write((char*)&size, sizeof(size));
				mHeaderStream->Write(mMemStream->GetCharArray(), size);
			} else 
			{
				mHeaderStream->Seek(mHeaders[i].mSerializedSize);
			}
		}
	} else
	{
		printf("cNodeCache::WriteHeaders - file %s could not be opened. Headers was not stored!\n", headersFile);
	}

	mHeaderStream->Close();
	delete mHeaderStream;
}

/**
* Read headers from the headers file. Read only the name and size of each data structure header.
* Each header is really read from the headers file when we call the LookForHeader method.
*/
void cNodeCache::ReadHeaders()
{
	cFileInputStream *mHeaderStream = new cFileInputStream();
	char headersFile[516];

	// read headers
	strcpy(headersFile, mDataFileName);
	strcat(headersFile, ".hdr");

	if (mHeaderStream->Open(headersFile, FLAGS_NORMAL /* FILE_FLAG_RANDOM_ACCESS*/))
	{
		mHeaderStream->Seek(0);
		mHeaderStream->Read((char*)&mBlockSize, sizeof(mBlockSize));
		mHeaderStream->Read((char*)&mMaxIndex, sizeof(mMaxIndex));
		mHeaderStream->Read((char*)&mHeadersCount, sizeof(mHeadersCount));
		mHeaderStream->Read((char*)&mNodesHeadersArrayCount, sizeof(mNodesHeadersArrayCount));

		for (unsigned int i = 0 ; i < mHeadersCount; i++)
		{
			mHeaders[i].mSeek = mHeaderStream->GetOffset();
			mHeaderStream->Read((char*)&mHeaders[i].mSerializedSize, sizeof(mHeaders[i].mSerializedSize));
			mHeaderStream->Read((char*)mHeaders[i].mName, cDStructHeader::HEADER_PATHSIZE);
			mHeaderStream->Seek(mHeaders[i].mSeek + mHeaders[i].mSerializedSize + sizeof(mHeaders[i].mSerializedSize));
		}

	} else
	{
		printf("cNodeCache::WriteHeaders - file %s could not be opened. Headers was not read!\n", headersFile);
	}

	mHeaderStream->Close();
	delete mHeaderStream;
}

void cNodeCache::ReadHeader(unsigned int order)
{
	cFileInputStream *mHeaderStream = new cFileInputStream();
	char headersFile[516];

	strcpy(headersFile, mDataFileName);
	strcat(headersFile, ".hdr");
	if (mHeaderStream->Open(headersFile, FLAGS_NORMAL /* FILE_FLAG_RANDOM_ACCESS*/)) 
	{
		mHeaderStream->Seek(mHeaders[order].mSeek);
		mHeaderStream->Read((char*)&mHeaders[order].mSerializedSize, sizeof(mHeaders[order].mSerializedSize));
		mHeaders[order].mHeader->Read(mHeaderStream);

		for (unsigned int i = 0; i < mHeaders[order].mHeader->GetNodeHeaderCount(); i++)
		{
			mNodesHeadersArray[mHeaders[order].mHeader->GetNodeType(i)] = mHeaders[order].mHeader->GetNodeHeader(i);
		}
	} else
	{
		printf("cNodeCache::WriteHeaders - file %s could not be opened. Headers was not read!\n", headersFile);
	}
	mHeaderStream->Close();
	delete mHeaderStream;

}

void cNodeCache::Clear(void)
{
	for (unsigned int i = 0; i < mCacheNodeSize; i++)
	{
		mNodes[i]->Clear(true);
	}
	mNodeRecordStorage->Clear();
	mMaxIndex = 0;
}

/**
 * Flush modified nodes onto secondary storage.
 */
void cNodeCache::Flush(void)
{
	// body moved into cNodeCache::Close(unsigned int) method and chabged according to new functionality...
}

//********************************************************************************
//********************************* Main Logic ***********************************
//********************************************************************************

void cNodeCache::BulkRead(const cArray<tNodeIndex>* nodeIndexArray, unsigned int startIndex, unsigned int endIndex, unsigned int nodeHeaderId)
{
	cBucketHeader *bucketHeader;
	cNodeHeader *nodeHeader = mNodesHeadersArray[nodeHeaderId];
	tNodeIndex startNodeIndex, endNodeIndex;

	// find the first node not to be in the cache
	for (; startIndex <= endIndex; startIndex++)
	{
		tNodeIndex nodeIndex = nodeIndexArray->GetRefItem(startIndex);

		if  (!mNodeRecordStorage->FindBucket(nodeIndex, &bucketHeader))
		{
			startNodeIndex = nodeIndex;
			break;
		}
	}

	// find the last node not to be in the cache
	if (startIndex <= endIndex) // it means, some nodes are neccessary to read
	{
		for (; endIndex >= startIndex; endIndex--)
		{
			tNodeIndex nodeIndex = nodeIndexArray->GetRefItem(endIndex);

			// problem
			if  (!mNodeRecordStorage->FindBucket(nodeIndex, &bucketHeader))
			{
				endNodeIndex = nodeIndex;
				break;
			}
		}

		Seek(startNodeIndex);

		if (!mNodeMemStream->Read(mStream, nodeHeader->GetNodeSerialSize() * (endNodeIndex - startNodeIndex + 1)))
		{
			throw new cDSCriticalException("DSCriticalException: cNodeCache::RealNodeRead(): It is not possible to read the complete node!");
			mNodeRecordStorage->Print();
		}

		char* origMem = mNodeMemStream->GetCharArray();
		char* currentMem = origMem;
		unsigned int nodeSerialSize = nodeHeader->GetNodeSerialSize();

		unsigned int j = startIndex;
		for (tNodeIndex i = startNodeIndex ; i <= endNodeIndex; i++)
		{
			if (nodeIndexArray->GetRefItem(j) == i)
			{
				Read(i, true, nodeHeaderId, mNodeMemStream);
				j++;
			}
			currentMem += nodeHeader->GetNodeSerialSize();
			mNodeMemStream->SetCharArray(currentMem, mNodeMemStream->GetSize() - nodeSerialSize);
		}
		mNodeMemStream->SetCharArray(origMem, mNodeMemStream->GetSize() - nodeSerialSize);
	}
}

/**
* Reads all nodes in buffer. 
* bed157: use cArray version
*/
void cNodeCache::BulkRead(const cDbfsLevel* nodeIndexArray, unsigned int startIndex, unsigned int endIndex, unsigned int nodeHeaderId)
{
	cBucketHeader *bucketHeader;
	cNodeHeader *nodeHeader = mNodesHeadersArray[nodeHeaderId];
	tNodeIndex startNodeIndex, endNodeIndex;

	// find the first node not to be in the cache
	for (; startIndex <= endIndex; startIndex++)
	{
		tNodeIndex nodeIndex = nodeIndexArray->GetRefItem(startIndex);

		if  (!mNodeRecordStorage->FindBucket(nodeIndex, &bucketHeader))
		{
			startNodeIndex = nodeIndex;
			break;
		}
	}

	// find the last node not to be in the cache
	if (startIndex <= endIndex) // it means, some nodes are neccessary to read
	{
		for (; endIndex >= startIndex; endIndex--)
		{
			tNodeIndex nodeIndex = nodeIndexArray->GetRefItem(endIndex);

			// problem
			if  (!mNodeRecordStorage->FindBucket(nodeIndex, &bucketHeader))
			{
				endNodeIndex = nodeIndex;
				break;
			}
		}

		Seek(startNodeIndex);

		if (!mNodeMemStream->Read(mStream, nodeHeader->GetNodeSerialSize() * (endNodeIndex - startNodeIndex + 1)))
		{
			throw new cDSCriticalException("DSCriticalException: cNodeCache::RealNodeRead(): It is not possible to read the complete node!");
			mNodeRecordStorage->Print();
		}

		char* origMem = mNodeMemStream->GetCharArray();
		char* currentMem = origMem;
		unsigned int nodeSerialSize = nodeHeader->GetNodeSerialSize();

		unsigned int j = startIndex;
		for (tNodeIndex i = startNodeIndex ; i <= endNodeIndex; i++)
		{
			if (nodeIndexArray->GetRefItem(j) == i)
			{
				Read(i, true, nodeHeaderId, mNodeMemStream);
				j++;
			}
			currentMem += nodeHeader->GetNodeSerialSize();
			mNodeMemStream->SetCharArray(currentMem, mNodeMemStream->GetSize() - nodeSerialSize);
		}
		mNodeMemStream->SetCharArray(origMem, mNodeMemStream->GetSize() - nodeSerialSize);
	}
}
/**
* Reads all nodes in buffer. 
*/
void cNodeCache::BulkRead(cDbfsLevel* buffer, cRangeQueryConfig* rqConfig, uint nodeHeaderId)
{
	uint count = buffer->Count();
	uint count_1 = count - 1;

	unsigned int startNodeIndex = buffer->GetRefItem(0);
	unsigned int startIndex = 0;
	bool firstItemOfBuffer = true;
	for (unsigned int i = 0; i < count; i++)
	{
		unsigned int indexDiff = 0;
		if (i != count_1 && firstItemOfBuffer)
		{
			firstItemOfBuffer = false;
			continue;  // the first item is the start item
		}

		if (i != startIndex)
		{
			indexDiff = buffer->GetRefItem(i) - startNodeIndex;
			if (indexDiff > rqConfig->GetMaxIndexDiff_BulkRead())
			{
				i--;
			}
		}

		if (indexDiff > rqConfig->GetMaxIndexDiff_BulkRead() || i == count_1)
		{
			BulkRead(buffer, startIndex, i, nodeHeaderId);

			if (i != count_1)
			{
				startIndex = i + 1;
				startNodeIndex = buffer->GetRefItem(startIndex);
				firstItemOfBuffer = true;
			}
		}
	}
}
// Read node for reading
cNode* cNodeCache::ReadR(const tNodeIndex index, unsigned int nodeHeaderId)
{
	return Read(index, true, nodeHeaderId);
}

// Read node for writing
cNode* cNodeCache::ReadW(const tNodeIndex index, unsigned int nodeHeaderId)
{
	return Read(index, false, nodeHeaderId);
}

// Read node
cNode* cNodeCache::Read(const tNodeIndex nodeIndex, bool readFlag, unsigned int nodeHeaderId, cCharStream* bufferedNodes)
{
	cNode *node;
	unsigned int bucketOrder;
	cNodeHeader *nodeHeader = mNodesHeadersArray[nodeHeaderId];
	cBucketHeader *bucketHeader;

	//StartCollectStatistic(readFlag, nodeHeaderId, true);

#ifdef MAINMEMORY_OPTIMIZATION
	if (nodeIndex < mCacheNodeSize)
	{
		bucketHeader = mNodeRecordStorage->GetBucketHeader(nodeIndex - 1);
		bucketOrder = nodeIndex - 1;
		node = mNodes[bucketOrder];
	} else
	{
		printf("cNodeCache::Read - not enough memory for %s\n", mDataFileName);
#endif
		bool bucketFound = mNodeRecordStorage->FindBucket(nodeIndex, &bucketHeader);
		bucketOrder = bucketHeader->GetBucketOrder();

		node = mNodes[bucketOrder];
		node->SetBucketOrder(bucketOrder);

		if (!bucketFound)
		{
			// node is not in the cache (in the selected bucket), the old node has to be written if is has been modified
			tNodeIndex oldNodeIndex = node->GetIndex();
			if (oldNodeIndex != cNode::EMPTY_INDEX)
			{
				if (bucketHeader->GetModified())
				{
					RealNodeWrite(*node, node->GetHeaderId());   // write node in the disk - do a place for the node read
				}
				mNodeRecordStorage->DeleteFromBucketIndex(oldNodeIndex);
			}

			// prepare backer for the new node
			node->Clear();
			node->SetHeaderId(nodeHeaderId);
			node->SetIndex(nodeIndex);
			node->SetHeader(mNodesHeadersArray[nodeHeaderId]);

			// now, read the node from the disk, if it is not in the case
			char size;

			if (bufferedNodes == NULL)
			{
				RealNodeRead(nodeIndex, nodeHeaderId);
				mNodeMemStream->Seek(0);
				mNodeMemStream->Read((char *)&size, sizeof(char));
				mNodeMemStream->Seek(0);
				nodeHeader->ReadNode(node, mNodeMemStream);
			}
			else
			{
				bufferedNodes->Read((char *)&size, sizeof(char));
				bufferedNodes->Seek(0);
				nodeHeader->ReadNode(node, bufferedNodes);
			}

			mNodeRecordStorage->AddInBucketIndex(nodeIndex, bucketOrder);
		}
#ifdef MAINMEMORY_OPTIMIZATION
	}
#endif
	
#ifndef WITHOUT_LOCKS
	if (readFlag)
	{
		if (bufferedNodes == NULL)
		{
			bucketHeader->IncrementReadLock();
		}
	} 
	else
	{
		if (bucketHeader->GetReadLock() > 0)
		{
			node = NULL;
			printf("Warning: cNodeCache::ReadW(): R/W conflict when a node is accessed!\n");
		}
		else
		{
			bucketHeader->SetWriteLock(true);
		}
	}
#endif

	//StopCollectStatistic(nodeHeaderId);
	
	return node;
}

/**
* Read node for writing from the cache. This node is not read from the secondary storage.
* Method should be used when we want to create new node.
* \param nodeIndex index of the node read
* \param nodeHeaderId id of the node header
*/
cNode* cNodeCache::ReadNew(unsigned int nodeHeaderId)
{
	tNodeIndex nodeIndex = GetNewIndex();
	cBucketHeader *bucketHeader;
	cNode *node;
	uint bucketOrder;

	//StartCollectStatistic(false, nodeHeaderId, true);

#ifdef MAINMEMORY_OPTIMIZATION
	if (nodeIndex < mCacheNodeSize - 1)
	{
		bucketOrder = nodeIndex - 1;
		bucketHeader = mNodeRecordStorage->GetBucketHeader(bucketOrder);
		node = mNodes[bucketOrder];
	} else
	{
		printf("cNodeCache::Read - not enough memory for %s\n", mDataFileName);
#endif
		bool nodeFound = mNodeRecordStorage->FindBucket(nodeIndex, &bucketHeader);
		assert(!nodeFound);
		bucketOrder = bucketHeader->GetBucketOrder();

		node = mNodes[bucketOrder];
		node->SetBucketOrder(bucketOrder);

		// if the old has been modified then write it
		tNodeIndex oldNodeIndex = node->GetIndex();
		if (oldNodeIndex != cNode::EMPTY_INDEX)
		{
			if (bucketHeader->GetModified())
			{
				RealNodeWrite(*node, node->GetHeaderId());
			}
			mNodeRecordStorage->DeleteFromBucketIndex(oldNodeIndex);
		}

		mNodeRecordStorage->AddInBucketIndex(nodeIndex, bucketOrder);
#ifdef MAINMEMORY_OPTIMIZATION
	}
#endif

	// prepare the bucket
	node->Clear(); // del !!mk!! ale zaradit node->SetItemCount(0);
	node->SetIndex(nodeIndex);
	node->SetHeaderId(nodeHeaderId);
	node->SetHeader(mNodesHeadersArray[nodeHeaderId]);
	node->SetFreeSize(node->GetHeader()->GetNodeItemsSpaceSize());


#ifndef WITHOUT_LOCKS
	bucketHeader->SetWriteLock(true);
#endif

	//StopCollectStatistic(nodeHeaderId);

	return node;
}

// Write new node into the cache
void cNodeCache::WriteNew(const cNode &node, unsigned int mNodeType)
{
	// NOT IMPLEMENTED!! -- please use ReadNew
}

void cNodeCache::StartCollectStatistic(bool readFlag, unsigned int nodeHeaderId, bool logicalAccess)
{
	//if (mTreeHeader->GetCacheMeasureTime())
	//{
	// mCacheStatistics.GetTimer()->Run();
	//}
	//if (mTreeHeader->GetCacheMeasureCount())
	//{

	//if (mTreeHeader->GetCacheMeasureTime())
	// is it necessary to measure time?
	// if(mNodesHeadersArray[nodeHeaderId]->GetCacheMeasureTime())
	// {
	//	mCacheStatistics.GetTimer()->Run();
	// }
	//if (mTreeHeader->GetCacheMeasureCount())
	//if(mNodesHeadersArray[mNodeType]->GetCacheMeasureTime())
	//{
	// mCacheStatistics.GetNodeDACWrite(cCacheStatistics::DAC_Logical).Increment();
	//}

	if (logicalAccess)
	{
		SetStatistic(readFlag, nodeHeaderId, cCacheStatistics::DAC_Logical);
	}
	else
	{
		SetStatistic(readFlag, nodeHeaderId, cCacheStatistics::DAC_Physical);
	}
	//}
}

void cNodeCache::SetStatistic(bool readFlag, unsigned int nodeHeaderId, unsigned int accessType)
{
	mCacheStatistics.IncrementDAC(readFlag, nodeHeaderId, accessType);
}

void cNodeCache::StopCollectStatistic(unsigned int nodeHeaderId)
{
	// if(mNodesHeadersArray[nodeHeaderId]->GetCacheMeasureTime())
	//{
	//		mCacheStatistics.GetTimer()->Stop();
	//}
}

/**
* Get node index from the cache. Node flag is not important in this type of cache.
* \param order Order of the node in the cache line.
* \return Node index.
*/
unsigned int cNodeCache::GetNodeIndex(bool nodeFlag, unsigned int order) const
{
	UNUSED(nodeFlag);
	return mNodes[order]->GetIndex();
}

/**
* Get node timestamp from the cache. Node flag is not important in this type of cache.
* \param order Order of the node in the cache line.
* \return Node timestamp.
*/
/*
unsigned int cNodeCache::GetNodeTimestamp(bool nodeFlag, unsigned int order) const
{
	UNREFERENCED_PARAMETER(nodeFlag);
	return (unsigned int)mNodeRecordStorage->GetPNodeRecordOrder(order)->GetTimestamp();
}*/

/**
* Unlock the lock the node
*/
void cNodeCache::Unlock(cNode *node, bool readFlag)
{
#ifndef WITHOUT_LOCKS
	assert(node->GetBucketOrder() != cNode::EMPTY_INDEX);

	cBucketHeader* bucketHeader = mNodeRecordStorage->GetBucketHeader(node->GetBucketOrder());
	
	assert(bucketHeader != NULL);
	assert(bucketHeader->GetReadLock() > 0 || bucketHeader->GetWriteLock());
	
	if (readFlag)
	{
		bucketHeader->DecrementReadLock();
		if (bucketHeader->GetReadLock() == 0)
		{
			mNodeRecordStorage->PutBackInBucketQueue(bucketHeader);
			node->ClearBucketOrder();
		}
	}
	else
	{
		bucketHeader->SetWriteLock(false);
		bucketHeader->SetModified(true);
		mNodeRecordStorage->PutBackInBucketQueue(bucketHeader);
		node->ClearBucketOrder();
	}
#endif
}

/// Unlock read lock for the node
void cNodeCache::UnlockR(cNode *node)
{
	Unlock(node, true);
}

/// Unlock write lock for the node
void cNodeCache::UnlockW(cNode *node)
{
	Unlock(node, false);
}

/**
 * Return the header of the bucket for the node.
 */
inline cBucketHeader* cNodeCache::GetBucketHeader(cNode *node)
{
	return mNodeRecordStorage->GetBucketHeader(node->GetBucketOrder());
}

/**
 * Seek at node nodeNum in file.
 */
inline bool cNodeCache::Seek(const tNodeIndex nodeIndex)
{
	bool ret = false;
	if (mStream != NULL) 
	{
		ret = mStream->Seek((llong)nodeIndex * (llong)mBlockSize);
	}
	return ret;
}

/**
* \return unused node index
*/
inline unsigned int cNodeCache::GetNewIndex() 
{ 
	return ++mMaxIndex; 
}

/**
* \return unused node index
*/
inline unsigned int cNodeCache::ActualNodeCountUsed() 
{ 
	return mMaxIndex; 
}

inline cCacheStatistics* cNodeCache::GetCacheStatistics()
{ 
	return &mCacheStatistics;
}

inline unsigned int cNodeCache::GetCacheNodeSize() const
{
	return mCacheNodeSize;
}

void cNodeCache::Print() const
{
	mNodeRecordStorage->Print();
}

void cNodeCache::PrintDataStructureStatistics()
{
	for (unsigned int i = 0 ; i < mHeadersCount ; i++)
	{
		if (mHeaders[i].mHeader != NULL)
		{
			printf("Data structure: %s, (", mHeaders[i].mName);
			unsigned int size, sum = 0;
			for (unsigned int j = 0; j < mHeaders[i].mHeader->GetNodeHeaderCount(); j++)
			{
				size = mHeaders[i].mHeader->GetNodeHeader(j)->GetNodeCount() * mBlockSize;
				if (j + 1 == mHeaders[i].mHeader->GetNodeHeaderCount())
				{
					printf("%d", size);
				} else
				{
					printf("%d + ", size);
				}
				sum += size;
			}
			printf(") = %d\n", sum);
		} else
		{
			printf("Header %d is not opened\n", i);
		}
	}
}

inline void cNodeCache::PrintMemoryStatistics()
{
	ullong memory = 0;
	unsigned int completeNodeInMemSize = sizeof(cNode) + mMaxNodeInMemSize;
	memory += completeNodeInMemSize * mCacheNodeSize;

	printf("Nodes memory size: %f[MB]\n", (float)memory / (float)(1024 * 1024));
}


inline unsigned int cNodeCache::GetNofLocks() const
{
	return mNodeRecordStorage->GetNofLocks();
}

/// \return the number of locked nodes
int cNodeCache::CheckLocks()
{
	int noflocks = 0;

	for (unsigned int i = 0 ; i < 2 ; i++)
	{
		for (unsigned int j = 0 ; j < mCacheNodeSize ; j++)
		{			
			if (i == 0)
			{
				noflocks += mNodeRecordStorage->GetBucketHeader(j)->GetReadLock();
			}
			else if (i == 1)
			{
				noflocks +=  mNodeRecordStorage->GetBucketHeader(j)->GetWriteLock();
			}
		}
	}

	return noflocks;
}

void cNodeCache::PrintLocks() const
{
	mNodeRecordStorage->PrintLocks();
}

bool cNodeCache::IsReadDebug() const
{
	return mReadDebug;
}

unsigned int cNodeCache::GetMaxNodeInMemSize()
{
	return mMaxNodeInMemSize;
}

}}}
#endif
