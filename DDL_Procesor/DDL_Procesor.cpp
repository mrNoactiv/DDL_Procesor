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



cTable* ValidationTest(cTable *table,string query, cQuickDB *quckdb, const unsigned int BLOCK_SIZE, uint DSMODE, unsigned int compressionRatio, unsigned int codeType, unsigned int runtimeMode, bool histograms, static const uint inMemCacheSize);
int main()
{
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





	mQuickDB = new cQuickDB();
	if (!mQuickDB->Create(dbPath, CACHE_SIZE, MAX_NODE_INMEM_SIZE, BLOCK_SIZE))
	{
		printf("Critical Error: Cache Data File was not open!\n");
		exit(1);
	}


	//string query = "create table ahoj(ID INT PRIMARY KEY,AGE INT NOT NULL) option:BTREE";
	//string query = "create table ahoj(ID VARCHAR(50) PRIMARY KEY,AGE INT NOT NULL) option:BTREE";
	

	cTable *table1=NULL;
	
	//string query = "create table ahoj(ID INT PRIMARY KEY,AGE VARCHAR(20) NOT NULL) option:BTREE";
	//string query = "create table ahoj(ID INT PRIMARY KEY,AGE INT NOT NULL) option:BTREE";
	//string query = "create table ahoj(ID VARCHAR(35) PRIMARY KEY,AGE INT NOT NULL) option:BTREE";
	string query = "create table ahoj(ID VARCHAR(20) PRIMARY KEY,AGE VARCHAR(10) NOT NULL) option:BTREE";
	table1=ValidationTest(table1,query, mQuickDB, BLOCK_SIZE, DSMODE, COMPRESSION_RATIO, CODETYPE, RUNTIME_MODE, HISTOGRAMS, INMEMCACHE_SIZE);
	
	query = "create index index_name ON ahoj(AGE)";
	table1=ValidationTest(table1, query, mQuickDB, BLOCK_SIZE, DSMODE, COMPRESSION_RATIO, CODETYPE, RUNTIME_MODE, HISTOGRAMS, INMEMCACHE_SIZE);
	
	
	

	
	


	 //pro 2x int
	/*int j = 100;
	int k = 500;
	for (int i = 0; i < 10000; i++)
	{


		cTuple* haldaTuple = new cTuple(SD);

		haldaTuple->SetValue(0, i, SD);
		haldaTuple->SetValue(1, j + i, SD);


		table1->SetValues(haldaTuple, SD);

	}*/
	 
	
	/*  generator pro varchar*/
	/*
	int randNumber;
	srand(time(NULL));

	for (int i = 0; i < 1000; i++)
	{


		cNTuple * varcharTuple = new cNTuple(table1->columns->at(0)->columnSD);

		for (int i = 0; i < table1->columns->at(0)->size/2; i++)
		{
			randNumber = rand() % 94 + 20;
			varcharTuple->SetValue(i, (char)randNumber, table1->columns->at(1)->columnSD);
		}

		cHNTuple* haldaTuple = new cHNTuple();
		haldaTuple->Resize(SD);


	haldaTuple->SetValue(0, *varcharTuple, SD);
	haldaTuple->SetValue(1, i, SD);

	table1->SetValues(haldaTuple, SD);

	}
	
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
	



	return 0;
}


cTable* ValidationTest(cTable *table,string query, cQuickDB * quckdb, const unsigned int BLOCK_SIZE, uint DSMODE, unsigned int compressionRatio, unsigned int codeType, unsigned int runtimeMode, bool histograms, const uint inMemCacheSize)
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
		table1->CreateTable(query, quckdb, BLOCK_SIZE, DSMODE, compressionRatio, codeType, runtimeMode, histograms, inMemCacheSize);
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
