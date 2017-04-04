// DDL_Procesor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "dstruct/paged/b+tree/cB+Tree.h"
#include "common/random/cGaussRandomGenerator.h"
#include "common/data/cDataCollection.h"
#include "common/data/cTuplesGenerator.h"
#include "dstruct/paged/core/cBulkLoad.h"
#include "common/datatype/tuple/cCommonNTuple.h"
#include "cTable.h"
#include "cTypeOfTranslator.h"



cTable* ValidationTest(cTable *table,string query, cQuickDB *quckdb, const unsigned int BLOCK_SIZE, const unsigned int cacheSize, uint DSMODE, unsigned int compressionRatio, unsigned int codeType, unsigned int runtimeMode, bool histograms, static const uint inMemCacheSize);
int main()
{
	/*
	ushort x = 256;
	uint y = 2;

	char * data = static_cast<char*>(static_cast<void*>(&x));
	char * data2 = static_cast<char*>(static_cast<void*>(&y));
	char *datas[] = { data, data2 };
	char sizes[] = { sizeof x, sizeof y };
	char bytes[6];
	cDataType *types[] = { new cUShort(),new cUInt() };
	


	cSpaceDescriptor *ajujiaji = new cSpaceDescriptor(2, NULL, types);

	char *myData = new char();

	cTuple::SetValue(myData, 0, x, ajujiaji);
	cTuple::SetValue(myData, 1, y, ajujiaji);

	for (size_t i = 0; i < 2; i++)
	{
		int offset = 0;
		for (size_t k = 0; k < i; k++)
		{
			offset += types[k]->GetSize();
		}
		for (size_t j = 0; j < types[i]->GetSize(); j++)
		{
			char * tmp = datas[i];

			bytes[i *  offset + j] = tmp[j];
		}
	}
	
	for (size_t i = 0; i < 6; i++)
	{
		printf("%d\n", bytes[i]);
	}
	


	

		return 0;*/
	/*proměnné*/
	char dbPath[1024] = "quickdb";
	uint CACHE_SIZE = 20000;
	uint BLOCK_SIZE = 2048 /*8192*/;
	uint MAX_NODE_INMEM_SIZE = 1.25 * BLOCK_SIZE;
	cQuickDB *mQuickDB;

	// Histograms for each dimension will be created during build (works only on conventional R-tree version)
	bool HISTOGRAMS = false;


	// defines the type of data structure mode
	const uint DSMODES[] = { cDStructConst::DSMODE_DEFAULT,
		cDStructConst::DSMODE_CODING,
		cDStructConst::DSMODE_RI,
		cDStructConst::DSMODE_RICODING,
		cDStructConst::DSMODE_SIGNATURES,
		cDStructConst::DSMODE_ORDERED
	};

	// defines the type of coding in the case of CODING and RICODING
	uint CODETYPE = FIXED_LENGTH_CODING;
	// ELIAS_DELTA 1		FIBONACCI2 2		FIBONACCI3  3		ELIAS_FIBONACCI 4
	// ELIAS_DELTA_FAST 5	FIBONACCI2_FAST 6	FIBONACCI3_FAST 7	ELIAS_FIBONACCI_FAST 8
	// FIXED_LENGTH_CODING 9	FIXED_LENGTH_CODING_ALIGNED 10

	// defines maximal compression ratio in RI, CODING and RICODING mode
	uint COMPRESSION_RATIO = 2;

	uint RUNTIME_MODE = cDStructConst::RTMODE_DEFAULT; // defines the mode (with or without validation) RTMODE_DEFAULT		RTMODE_VALIDATION

	uint DSMODE = DSMODES[0];

	static const uint INMEMCACHE_SIZE = 0;
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------------*/



	/*BpTreeHeader<TKey>::cBpTreeHeader(const char* uniqueName, uint blockSize, cDTDescriptor *dd, uint keySize, uint dataSize,  bool variableLenData, uint dsMode, uint treeCode, uint compressionRatio)*/





	unsigned int a = sizeof(cDataType);

	mQuickDB = new cQuickDB();
	if (!mQuickDB->Create(dbPath, CACHE_SIZE, MAX_NODE_INMEM_SIZE, BLOCK_SIZE))
	{
		printf("Critical Error: Cache Data File was not open!\n");
		exit(1);
	}


	//string query = "create table ahoj(ID INT PRIMARY KEY,AGE INT NOT NULL) option:BTREE";
	//string query = "create table ahoj(ID VARCHAR(50) PRIMARY KEY,AGE INT NOT NULL) option:BTREE";


	cTable *table1 = NULL;
	cTable *table2 = NULL;
	cTable *table3 = NULL;
	cTable *table4 = NULL;
	cTable *table5 = NULL;


	
	/*string query1 = "create table ahoj(ID INT PRIMARY KEY,AGE VARCHAR(20) NOT NULL) OPTION:BTREE";
	table1 = ValidationTest(table1, query1, mQuickDB, BLOCK_SIZE, CACHE_SIZE, DSMODE, COMPRESSION_RATIO, CODETYPE, RUNTIME_MODE, HISTOGRAMS, INMEMCACHE_SIZE);
	query1 = "create index index_name ON ahoj(AGE)";
	table1 = ValidationTest(table1, query1, mQuickDB, BLOCK_SIZE, CACHE_SIZE, DSMODE, COMPRESSION_RATIO, CODETYPE, RUNTIME_MODE, HISTOGRAMS, INMEMCACHE_SIZE);

	table1->indexesFixLenBTree->at(0)->mIndex->PrintInfo();*/


	/*
	string query2 = "create table ahoj2(ID INT PRIMARY KEY,AGE INT NOT NULL,length INT NOT NULL,weight INT NOT NULL) OPTION:MD_TABLE";
	table2 = ValidationTest(table2, query2, mQuickDB, BLOCK_SIZE, CACHE_SIZE, DSMODE, COMPRESSION_RATIO, CODETYPE, RUNTIME_MODE, HISTOGRAMS, INMEMCACHE_SIZE);
	query2 = "create index index_name2 ON ahoj2(AGE)";
	table2 = ValidationTest(table2, query2, mQuickDB, BLOCK_SIZE, CACHE_SIZE, DSMODE, COMPRESSION_RATIO, CODETYPE, RUNTIME_MODE, HISTOGRAMS, INMEMCACHE_SIZE);
	*/


	string query3 = "create table ahoj3(ID VARCHAR(35) PRIMARY KEY,AGE INT NOT NULL) OPTION:MD_TABLE";
	table3 = ValidationTest(table3, query3, mQuickDB, BLOCK_SIZE, CACHE_SIZE, DSMODE, COMPRESSION_RATIO, CODETYPE, RUNTIME_MODE, HISTOGRAMS, INMEMCACHE_SIZE);
	query3 = "create index index_name3 ON ahoj3(AGE)";
	table3 = ValidationTest(table3, query3, mQuickDB, BLOCK_SIZE, CACHE_SIZE, DSMODE, COMPRESSION_RATIO, CODETYPE, RUNTIME_MODE, HISTOGRAMS, INMEMCACHE_SIZE);


	/*
	string query4 = "create table ahoj4(ID VARCHAR(20) PRIMARY KEY,AGE VARCHAR(10) NOT NULL) OPTION:BTREE";
	table4 = ValidationTest(table4, query4, mQuickDB, BLOCK_SIZE, CACHE_SIZE, DSMODE, COMPRESSION_RATIO, CODETYPE, RUNTIME_MODE, HISTOGRAMS, INMEMCACHE_SIZE);
	query4 = "create index index_name4 ON ahoj4(AGE)";
	table4 = ValidationTest(table4, query4, mQuickDB, BLOCK_SIZE, CACHE_SIZE, DSMODE, COMPRESSION_RATIO, CODETYPE, RUNTIME_MODE, HISTOGRAMS, INMEMCACHE_SIZE);


	string query5 = "create table ahoj5(ID INT NOT NULL,AGE VARCHAR(10) PRIMARY KEY) OPTION:BTREE";
	table5 = ValidationTest(table5, query5, mQuickDB, BLOCK_SIZE, CACHE_SIZE, DSMODE, COMPRESSION_RATIO, CODETYPE, RUNTIME_MODE, HISTOGRAMS, INMEMCACHE_SIZE);
	*/
	
	/*
	cTable *table5 = NULL;

	 string query5 = "create table ahoj5(ID INT PRIMARY KEY,AGE INT NOT NULL) OPTION:CLUSTERED_TABLE";
	 table2 = ValidationTest(table5, query5, mQuickDB, BLOCK_SIZE, DSMODE, COMPRESSION_RATIO, CODETYPE, RUNTIME_MODE, HISTOGRAMS, INMEMCACHE_SIZE);
	*/



	





   //table1->indexesVarLenBTree->at(0)->mIndex->PrintInfo();
   //table1->indexesVarLenRTree->at(0)->mIndex->PrintInfo();
   //table1->indexesFixLenBTree->at(0)->mIndex->PrintInfo();
   //table1->indexesFixLenRTree->at(0)->mIndex->PrintInfo();
   /*cTuple *findTuple = new cTuple(SD);
   findTuple->SetValue(0, 1, SD);

   table1->indexesFixLenBTree->at(0)->mIndex->PrintInfo();
   table1->indexesFixLenBTree->at(0)->mIndex->Find(1);
   */

	mQuickDB->PrintDataStructureStatistics();
	mQuickDB->PrintMemoryStatistics();

	
	bool aa = mQuickDB->Open("ahoj", CACHE_SIZE, MAX_NODE_INMEM_SIZE, BLOCK_SIZE);
	

	return 0;
}


cTable* ValidationTest(cTable *table,string query, cQuickDB * quckdb, const unsigned int BLOCK_SIZE, const unsigned int cacheSize, uint DSMODE, unsigned int compressionRatio, unsigned int codeType, unsigned int runtimeMode, bool histograms, const uint inMemCacheSize)
{
	cTable *table1 = table;
	if (table1 == NULL)
	{
		table1 = new cTable();
	}

	
	cTypeOfTranslator *typeofTranslator = new cTypeOfTranslator();
	typeofTranslator->SetType(query);

	cSpaceDescriptor *SD = NULL;


	if (typeofTranslator->type == Type::CREATE)
	{
		table1->CreateTable(query, quckdb, BLOCK_SIZE, cacheSize, DSMODE, compressionRatio, codeType, runtimeMode, histograms, inMemCacheSize);
		SD = table1->SD;

		/*generování záznamů*/
		if (table1->varlen && table1->homogenous == false)
		{
			for (int i = 0; i < 1000; i++)
			{
				table1->SetValues(table1->varGen->CreateNewRecord(), SD);
			}
		}
		else
		{
			for (int i = 0; i < 1000; i++)
			{
				table1->SetValues(table1->fixGen->CreateNewRecord(), SD);
			}
		}



	}
	else if (typeofTranslator->type == Type::INDEX)
	{
		table1->CreateIndex(query, quckdb, BLOCK_SIZE, DSMODE, compressionRatio, runtimeMode, codeType, histograms, inMemCacheSize);
	}
	else
	{
		cout << "command not found" << endl;
	}


	

	return table1;
}
