/**
*	\file cQuickDB.h
*	\author Radim Baca
*	\version 0.1
*	\date apr 2007
*	\brief index 
*/


#ifndef __cQuickDB_h__
#define __cQuickDB_h__

#include <mutex>

#include "dstruct/paged/core/cDStructConst.h"
#include "dstruct/paged/core/cNodeCache.h"
#include "dstruct/paged/core/cMemoryPool.h"
#include "common/memdatstruct/cMemoryManager.h"

#ifdef CUDA_ENABLED
#include "dstruct/paged/cuda/cMemoryManagerCuda.h"
#include "dstruct/paged/cuda/cCudaParams.h"
#include "dstruct/paged/cuda/cGpuConst.h"
#include "dstruct/paged/core/cTreeNode.h"
#endif

using namespace common::memdatstruct;

namespace dstruct {
  namespace paged {
	namespace core {

template <class TKey> class cItemStream;

/**
* Contains main components of every persistent tree data structure such as data cache, memory pool, result set.
* Instance of this object should be shared between threads working with the database.
*
*	\author Radim Baca
*	\version 0.1
*	\date oct 2011
**/
class cQuickDB
{
private:
	bool mIsOpened;					/// Is true of the database is opened.
	cItemStream<void> **mResultSet;		/// Stack of available result sets.
	int mResultSetSize;	/// Size of the mResultSet array.
	int mResultSetPointer;	/// Pointer to the mResultSet array, which marks the top available result set.
	cNodeCache*	mNodeCache;
	cMemoryPool* mMemoryPool;
	cMemoryManager *mMemoryManager;
	bool mMemoryManagerOutside;		/// Flag is true, when the memory manager was passed from outside
	char* mDatabaseName;
	std::mutex mResultSetLock;
	
#ifdef CUDA_ENABLED
	cMemoryManagerCuda* mMemoryManagerCuda;
#endif

	const static unsigned int RESULTSET_MAX_SIZE = 10;
	static const unsigned int MEMORY_MANAGER_SIZE = 1000;  /// the number of block in the memory manager

	void InitDatabaseName(const char* databaseName);

public:
	cQuickDB(cMemoryManager* memoryManager = NULL);
	~cQuickDB();

	bool Create(const char* databaseFileName, unsigned int cacheSize, unsigned int maxNodeInMemSize, unsigned int blockSize);
	bool Open(const char* databaseFileName, unsigned int cacheSize, unsigned int maxNodeInMemSize, unsigned int blockSize);
	bool Close(bool flushCache = true);

	inline cNodeCache* GetNodeCache();
	inline cMemoryPool* GetMemoryPool();
	inline cMemoryManager* GetMemoryManager();
	cItemStream<void>* GetResultSet();
	void AddResultSet(cItemStream<void>* stream);
	inline unsigned int GetNofFreeResultSets();
	inline char* GetDatabaseName() const;

	inline void PrintDataStructureStatistics();
	inline void PrintMemoryStatistics();

	inline bool IsOpened();
	
#ifdef CUDA_ENABLED
	cMemoryManagerCuda* GetMemoryManagerCuda();
#endif
};
}}}

#include "dstruct/paged/core/cItemStream.h"

namespace dstruct {
  namespace paged {
	namespace core {

/**
* \return Cache of the database
*/
cNodeCache* cQuickDB::GetNodeCache()
{
	return mNodeCache;
}

cMemoryPool* cQuickDB::GetMemoryPool()
{
	return mMemoryPool;
}

cMemoryManager* cQuickDB::GetMemoryManager()
{
	return mMemoryManager;
}

unsigned int cQuickDB::GetNofFreeResultSets()
{
	return mResultSetPointer + 1;
}

/**
* \return If the database is opened (by create or open method).
*/
bool cQuickDB::IsOpened()
{
	return mIsOpened;
}

inline char* cQuickDB::GetDatabaseName() const
{
	return mDatabaseName;
}

inline void cQuickDB::PrintDataStructureStatistics()
{
	mNodeCache->PrintDataStructureStatistics();
}

void cQuickDB::PrintMemoryStatistics()
{
	mNodeCache->PrintMemoryStatistics();
}

}}}
#endif