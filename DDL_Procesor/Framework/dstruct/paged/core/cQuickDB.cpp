#include "cQuickDB.h"

namespace dstruct {
  namespace paged {
	namespace core {

/**
* Constructor
* \param memoryManager Optional parameter. We can pass the memory manager (memory pool) from outside using this parameter. Otherwise the memory manager is created by cQuickDB.
*/ 
cQuickDB::cQuickDB(cMemoryManager* memoryManager): mDatabaseName(NULL)
{
	mNodeCache = new cNodeCache();
	mMemoryPool = new cMemoryPool();
	if (memoryManager == NULL)
	{
		mMemoryManager = new cMemoryManager(MEMORY_MANAGER_SIZE);
		mMemoryManagerOutside = false;
	} else
	{
		mMemoryManager = memoryManager;
		mMemoryManagerOutside = true;
	}
	mIsOpened = false;
#ifdef CUDA_ENABLED
	mMemoryManagerCuda = new cMemoryManagerCuda();
#endif
}

/// Destructor
cQuickDB::~cQuickDB()
{
	if (mNodeCache != NULL)
	{
		delete mNodeCache;
		mNodeCache = NULL;
	}
	if (mMemoryPool != NULL)
	{
		delete mMemoryPool;
		mMemoryPool = NULL;
	}
	if (!mMemoryManagerOutside && mMemoryManager != NULL)
	{
		delete mMemoryManager;
		mMemoryManager = NULL;
	}
	if (mDatabaseName != NULL)
	{
		delete[] mDatabaseName;
		mDatabaseName = NULL;
	}
}

/**
* Create persistent file on a secondary storage, it creates a node cache (data cache) 
* and prepare QuickDB for the usage. 
* \param databaseName Specify the database name and it is used to create a file on a secondary storage (an extension '.dat' is added)
* \param cacheSize Number of blocks of a node cache
* \param maxNodeInMemSize Main memory size of one node cache
* \param blockSize Persistent size of one block
* \return Returns true if everything was created successfully
*/
bool cQuickDB::Create(const char* databaseName, unsigned int cacheSize, unsigned int maxNodeInMemSize, unsigned int blockSize)
{
	const uint STR_LENGTH = 256;
	char fileName[STR_LENGTH];
	char number[4];

	assert(strlen(databaseName) < STR_LENGTH);

	InitDatabaseName(databaseName);

	strcpy(fileName, databaseName);
	strcat(fileName, ".dat");

	if (!mNodeCache->Create(fileName, cacheSize, maxNodeInMemSize, blockSize))
	{
		mIsOpened = false;
		printf("cQuickDB::Create - Critical Error, Cache Data File was not created!\n");
		return false;
	} else
	{
		mIsOpened = true;
	}

	// result creation
	mResultSetPointer = -1;
	mResultSetSize = RESULTSET_MAX_SIZE;
	mResultSet = new cItemStream<void>*[mResultSetSize];
	for (unsigned int i = 0; i < mResultSetSize; i++)
	{
		// snprintf is better but vs does not support it
		sprintf(fileName, "%s%s%u%s", databaseName, "_ResultSet", i, ".dat");

		/*
		itoa(i, number, 10);
		strcpy(fileName, databaseName);
		strcat(fileName, "_ResultSet");
		strcat(fileName, number);
		strcat(fileName, ".dat");*/
		AddResultSet(new cItemStream<void>(fileName, this));
		//AddResultSet(new cItemStream(fileName));
	}

	return true;
}

bool cQuickDB::Open(const char* databaseName, unsigned int cacheSize, unsigned int maxNodeInMemSize, unsigned int blockSize)
{
	const uint STR_LENGTH = 256;
	char fileName[STR_LENGTH];
	char number[4];

	assert(strlen(databaseName) < STR_LENGTH);

	strcpy(fileName, databaseName);
	strcat(fileName, ".dat");

	if (!mNodeCache->Open(fileName, cacheSize, maxNodeInMemSize, blockSize))
	{
		mIsOpened = false;
		printf("cQuickDB::Open - Critical Error, Cache Data File was not open!\n");
		return false;
	} else
	{
		mIsOpened = true;
	}

	mResultSetPointer = -1;
	mResultSetSize = RESULTSET_MAX_SIZE;
	mResultSet = new cItemStream<void>*[mResultSetSize];
	for (unsigned int i = 0; i < mResultSetSize; i++)
	{
		// snprintf is better but vs does not support it
		sprintf(fileName, "%s%s%u%s", databaseName, "_ResultSet", i, ".dat");

		/*
		itoa(i, number, 10);
		strcpy(fileName, databaseName);
		strcat(fileName, "_ResultSet");
		strcat(fileName, number);
		strcat(fileName, ".dat"); */

		AddResultSet(new cItemStream<void>(fileName, this));
		//AddResultSet(new cItemStream(resultSetFileName));
	}

	return true;
}

bool cQuickDB::Close(bool flushCache)
{
	mIsOpened = false;
	mNodeCache->Close(flushCache); // mk: bylo zapoznamkovano
	for (unsigned int i = 0; i <= mResultSetPointer; i++)
	{
		delete mResultSet[i];
		// TODO - smazat soubory
	}
	delete mResultSet;
	return true;
}

/**
 * Create string and copy database name.
 */
void cQuickDB::InitDatabaseName(const char* databaseName)
{
	unsigned int len = strlen(databaseName);

	if (mDatabaseName != NULL)
	{
		unsigned int oldlen = strlen(mDatabaseName);

		if (oldlen < len)
		{
			delete[] mDatabaseName;
		}
	}

	mDatabaseName = new char[len + 1];
	strcpy(mDatabaseName, databaseName);
}

cItemStream<void>* cQuickDB::GetResultSet()
{
	assert (mResultSetPointer >= 0);
	std::lock_guard<std::mutex> lock(mResultSetLock);
	return mResultSet[mResultSetPointer--];
}

void cQuickDB::AddResultSet(cItemStream<void>* stream)
{
	assert (mResultSetPointer+1 < mResultSetSize);
	std::lock_guard<std::mutex> lock(mResultSetLock);
	mResultSet[++mResultSetPointer] = stream;
}
#ifdef CUDA_ENABLED
cMemoryManagerCuda* cQuickDB::GetMemoryManagerCuda()
{
	return mMemoryManagerCuda;
}
#endif
}}}