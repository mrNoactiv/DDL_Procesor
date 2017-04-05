#pragma once

#include "dstruct/paged/b+tree/cB+Tree.h"
#include "common/random/cGaussRandomGenerator.h"
#include "common/data/cDataCollection.h"
#include "common/data/cTuplesGenerator.h"
#include "dstruct/paged/core/cBulkLoad.h"
#include "common/datatype/tuple/cCommonNTuple.h"



#include <algorithm>
#include <array>
#include <vector>
#include "cTranslatorCreate.h"
#include "cTranslatorIndex.h"
#include "cCompleteRTree.h"
#include "cCompleteBTree.h"
#include "cRecordGeneratorVar.h"
#include "cRecordGeneratorFix.h"
#include "cCompleteSeqArray.h"


class cTable
{
public:



	/*proměnné k  vytvoření stromu*/
	TypeOfCreate typeOfTable;
	std::vector<cDataType*> vHeap;//prázný vektor který se přetvoří na haldu jako rekace na create table
	cCompleteSeqArray<cTuple>*seqHeapFix;
	cCompleteSeqArray<cHNTuple>*seqHeapVar;

	bool varlenData;
	bool implicitKeyVarlen;
	bool homogenous;
	unsigned int sizeOfData = sizeof(uint) + sizeof(short);

	cSpaceDescriptor *keySD;//SD pro klič
	cSpaceDescriptor *varlenKeyColumnSD;//SD sloupce který je v klíči
	cDataType *keyType;//datovy typ kliče
	cSpaceDescriptor * SD;//SD pro záznamy v tabulce

	/*univerzalni proměnné*/
	std::vector<cColumn*>*columns;
	string tableName;

	/*generatory*/
	cRecordGeneratorVar *varGen = NULL;
	cRecordGeneratorFix *fixGen = NULL;

	/*Proměnné k indexu typu rtree*/
	std::vector<cCompleteRTree<cTuple>*>*indexesFixLenRTree = NULL;//indexy fixní delky
	std::vector<cCompleteRTree<cHNTuple>*>*indexesVarLenRTree = NULL;//indexy var delky

	/*Proměnné k indexu typu btree*/
	std::vector<cCompleteBTree<cTuple>*>*indexesFixLenBTree = NULL;//indexy fixní delky
	std::vector<cCompleteBTree<cHNTuple>*>*indexesVarLenBTree = NULL;//indexy var delky


public:

	cTable();
	bool CreateTable(string query, cQuickDB *quickDB, const unsigned int blockSize, const unsigned int cacheSize, uint DSMODE, unsigned int compressionRatio, unsigned int codeType, unsigned int runtimeMode, bool histograms, static const uint inMemCacheSize);
	bool CreateIndex(string query, cQuickDB *quickDB, const unsigned int blockSize, uint DSMODE, unsigned int compressionRatio, unsigned int codeType, unsigned int runtimeMode, bool histograms, static const uint inMemCacheSize);
	bool CreateClusteredTable(cTranslatorCreate *translator, cQuickDB *quickDB, const unsigned int BLOCK_SIZE, uint DSMODE, static const uint structure, unsigned int compressionRatio, unsigned int codeType, unsigned int runtimeMode, bool histograms, static const uint inMemCacheSize);

	void SetValues(cTuple *tuple, cSpaceDescriptor *SD);
	void SetValues(cHNTuple *tuple, cSpaceDescriptor *SD);

	cTuple* FindKey(string column, int searchedValue);
	cTuple* FindKey(float searchedValue);
	
	cTuple * TransportItemFixLen(cTuple *sourceTuple, cSpaceDescriptor *mSd, int columnPosition, cDataType *mType);
	//cHNTuple * TransportItemVarLen(cHNTuple *sourceTuple, cSpaceDescriptor *columnSD, cSpaceDescriptor *keySD, int columnPosition, cDataType *mType);
	cTuple * TransportItemFixLen(cHNTuple *sourceTuple, cSpaceDescriptor *mSd, int columnPosition, cDataType *mType);
	cHNTuple * TransportItemVarLen(cTuple *sourceTuple, cSpaceDescriptor *columnSD, cSpaceDescriptor *keySD, int columnPosition, cDataType *mType);
	cTuple * TransportItemVarLen(cHNTuple *sourceTuple, cSpaceDescriptor *columnSD, cSpaceDescriptor *keySD, int columnPosition, cDataType *mType);
	cTuple *TransportItemHomoVarlen(cTuple *sourceTuple, cSpaceDescriptor *columnSD, cSpaceDescriptor *keySD, int columnPosition, cDataType *mType);
	
	bool ConstructIndexBtree(string indexName, cDataType *indexType, int indexColumnPosition, uint blockSize, cSpaceDescriptor *indexSD, cSpaceDescriptor *indexKeyColumnSD,bool varlenKey, uint dsMode, uint compressionRatio, unsigned int codeType, unsigned int runtimeMode, bool histograms, static const uint inMemCacheSize, cQuickDB *quickDB);
	bool ConstructVarlenHomoIndexBTree(string indexName, cDataType * indexType, cSpaceDescriptor *indexKeyColumnSD, int indexColumnPosition, uint blockSize, cSpaceDescriptor * indexSD, uint dsMode, uint compressionRatio, unsigned int codeType, unsigned int runtimeMode, bool histograms, const uint inMemCacheSize, cQuickDB * quickDB);
	bool ConstructVarlenIndexBTree(string indexName, cDataType * indexType, cSpaceDescriptor *indexKeyColumnSD, int indexColumnPosition, uint blockSize, cSpaceDescriptor * indexSD, uint dsMode, uint compressionRatio, unsigned int codeType, unsigned int runtimeMode, bool histograms, const uint inMemCacheSize, cQuickDB * quickDB);

	bool ConstructIndexRtree(string indexName, cDataType *indexType, int indexColumnPosition, uint blockSize, cSpaceDescriptor *indexSD,cSpaceDescriptor *indexKeyColumnSD, bool varlenKey, uint dsMode, uint compressionRatio, unsigned int codeType, unsigned int runtimeMode, bool histograms,  cQuickDB *quickDB);
	bool ConstructVarlenHomoIndexRTree(string indexName, cDataType * indexType, cSpaceDescriptor *indexKeyColumnSD, int indexColumnPosition, uint blockSize, cSpaceDescriptor * indexSD, uint dsMode,  uint compressionRatio, unsigned int codeType, unsigned int runtimeMode, bool histograms,  cQuickDB * quickDB);
	bool ConstructVarlenIndexRTree(string indexName, cDataType * indexType, cSpaceDescriptor *indexKeyColumnSD, int indexColumnPosition, uint blockSize, cSpaceDescriptor * indexSD, uint dsMode,  uint compressionRatio, unsigned int codeType, unsigned int runtimeMode, bool histograms,  cQuickDB * quickDB);
	
	char* LoadIndexData(unsigned int nodeId, unsigned int position);

};

cTable::cTable():vHeap(NULL),varlenKeyColumnSD(NULL)
{
	
}

inline bool cTable::CreateTable(string query, cQuickDB *quickDB, const unsigned int blockSize, const unsigned int cacheSize, uint DSMODE, unsigned int compressionRatio, unsigned int codeType, unsigned int runtimeMode, bool histograms, static const uint inMemCacheSize)
{
	//bstrom1
	cTranslatorCreate *translator = new cTranslatorCreate();//instance překladače


	translator->TranlateCreate(query);//překladad cretae table

	keySD = translator->keySD;
	keyType = translator->keyDimensionType;
	SD = translator->SD;
	columns = translator->columns;
	tableName = translator->tableName;
	varlenData = translator->varlenData;
	implicitKeyVarlen = translator->keyVarlen;
	typeOfTable = translator->typeOfCreate;
	homogenous = translator->homogenous;
	
	
	
	for (int i = 0; i < columns->size(); i++)
	{
		if (columns->at(i)->primaryKey)
		{
			varlenKeyColumnSD = columns->at(i)->columnSD;
		}

	}

	if (((implicitKeyVarlen && varlenData) && homogenous==false) || (implicitKeyVarlen &&  homogenous == false) || (implicitKeyVarlen == false && homogenous == false))
	{
		varGen = new cRecordGeneratorVar(columns, SD);
		seqHeapVar = new cCompleteSeqArray<cHNTuple>((tableName + "_heap").c_str(), blockSize, cacheSize, SD, quickDB);
	}
	else
	{
		fixGen = new cRecordGeneratorFix(columns, SD);
		seqHeapFix = new cCompleteSeqArray<cTuple>((tableName + "_heap").c_str(), blockSize, cacheSize, SD, quickDB);
	}

	
	
	if (typeOfTable == BTREE)
	{
		indexesFixLenBTree = new vector<cCompleteBTree<cTuple>*>();
		indexesVarLenBTree = new vector<cCompleteBTree<cHNTuple>*>();




		unsigned int indexSD = keySD->GetSize();
		//unsigned int lengthIndexSD = tp->GetLength();//pocet sloupcu
		unsigned int typeSizeIndex = keySD->GetTypeSize();


		unsigned int lengthkeySD = sizeof(int);
		//unsigned int typeSizeKey = varlenKeyColumnSD->GetTypeSize();


		

		if (implicitKeyVarlen)
		{
			cCompleteBTree<cTuple>*index = new cCompleteBTree<cTuple>(tableName.c_str(), translator->keyPosition, blockSize, keySD, keySD->GetDimensionSize(0), sizeOfData, false, DSMODE, cDStructConst::BTREE, compressionRatio, codeType, runtimeMode, histograms, inMemCacheSize, quickDB);
			if (index != NULL)
			{
				indexesFixLenBTree->push_back(index);
				return true;
			}
		}
		else
		{
			cCompleteBTree<cTuple>*index = new cCompleteBTree<cTuple>(tableName.c_str(), translator->keyPosition, blockSize, keySD, keySD->GetSize(), sizeOfData, false, DSMODE, cDStructConst::BTREE, compressionRatio, codeType, runtimeMode, histograms, inMemCacheSize, quickDB);
			if (index != NULL)
			{
				indexesFixLenBTree->push_back(index);
				return true;
			}
		}

		/*if (implicitKeyVarlen == false)
		{*/
		/*cCompleteBTree<cTuple>*index = new cCompleteBTree<cTuple>(tableName.c_str(), translator->keyPosition, blockSize, keySD, keySD->GetDimensionSize(0) , sizeOfData, false, DSMODE, cDStructConst::BTREE, compressionRatio, codeType, runtimeMode, histograms, inMemCacheSize, quickDB);
			if (index != NULL)
			{
				indexesFixLenBTree->push_back(index);
				return true;
			}*/
		//}
		//else
		//{
		//	cNTuple *ntp = new cNTuple(varlenKeyColumnSD);
		//	
		//	cHNTuple *tp = new cHNTuple();
		//	tp->Resize(keySD);
		//	//unsigned int v1 = tp->GetSize(keySD); tohle určitě ne moc velké
		//	
		//	unsigned int indexSD = keySD->GetSize();
		//	unsigned int lengthIndexSD = tp->GetLength();//pocet sloupcu
		//	unsigned int typeSizeIndex = keySD->GetTypeSize();

		//	unsigned int keysd = varlenKeyColumnSD->GetSize();
		//	unsigned int lengthkeySD = ntp->GetLength();
		//	unsigned int typeSizeKey = varlenKeyColumnSD->GetTypeSize();

		//	unsigned int hue = sizeof(ntp);
		//	unsigned int hue2 = sizeof(ntp);

		//	cCompleteBTree<cNTuple>*index = new cCompleteBTree<cNTuple>(tableName.c_str(), translator->keyPosition, blockSize, keySD, keySD->GetTypeSize(), sizeOfData, false, DSMODE, cDStructConst::BTREE, compressionRatio, codeType, runtimeMode, histograms, inMemCacheSize, quickDB);

		//	if (index != NULL)
		//	{
		//		//indexesVarLenBTree->push_back(index);
		//		//indexesFixLen->push_back(index);
		//		return true;
		//	}

		//}

	}
	else if (typeOfTable == RTREE)
	{
		indexesFixLenRTree = new vector<cCompleteRTree<cTuple>*>();
		indexesVarLenRTree = new vector<cCompleteRTree<cHNTuple>*>();
		
		if (implicitKeyVarlen == false)
		{
			cCompleteRTree<cTuple>*index = new cCompleteRTree<cTuple>(tableName.c_str(), translator->keyPosition, blockSize, keySD, keySD->GetSize(), sizeOfData, false, DSMODE, cDStructConst::RTREE, compressionRatio, codeType, runtimeMode, histograms, quickDB);


			if (index != NULL)
			{
				indexesFixLenRTree->push_back(index);

				return true;
			}
		}
		else
		{
			unsigned int gu = keySD->GetDimensionSize(0);
			
			cCompleteRTree<cTuple>*index = new cCompleteRTree<cTuple>(tableName.c_str(), translator->keyPosition, blockSize, keySD, keySD->GetDimensionSize(0), sizeOfData, false, DSMODE, cDStructConst::RTREE, compressionRatio, codeType, runtimeMode, histograms, quickDB);
			if (index != NULL)
			{
				indexesFixLenRTree->push_back(index);
				return true;
			}
		}
		//}
		//else
		//{
		//	
		//	cCompleteRTree<cNTuple>*index = new cCompleteRTree<cNTuple>(tableName.c_str(), translator->keyPosition, blockSize, keySD, keySD->GetSize(), sizeOfData, false, DSMODE, cDStructConst::RTREE, compressionRatio, codeType, runtimeMode, histograms, quickDB);
		//	
		//	if (index != NULL)
		//	{
		//		//indexesVarLenRTree->push_back(index);

		//		return true;
		//	}
		//
		//}

	}
	else if(typeOfTable ==CLUSTERED_TABLE_BTREE)
	{
		CreateClusteredTable(translator, quickDB, blockSize, DSMODE, cDStructConst::RTREE  ,compressionRatio,  codeType,  runtimeMode,  histograms, inMemCacheSize);
	}
	else if (typeOfTable == CLUSTERED_TABLE_BTREE)
	{
		CreateClusteredTable(translator, quickDB, blockSize, DSMODE, cDStructConst::BTREE, compressionRatio, codeType, runtimeMode, histograms, inMemCacheSize);
	}
	else
	{
		cout << "unknow type of table" << endl;
		exit(0);
	}
}

inline bool cTable::CreateIndex(string query, cQuickDB * quickDB, const unsigned int BLOCK_SIZE, uint DSMODE, unsigned int compressionRatio, unsigned int codeType, unsigned int runtimeMode, bool histograms, const uint inMemCacheSize)
{


	cTranslatorIndex *translator = new cTranslatorIndex();//instance překladače
	translator->TranslateCreateIndex(query);



	cDataType *indexType=NULL;
	int size;
	int indexColumnPosition;
	string indexName = translator->indexName;
	bool varlenIndex=false;

	

	cSpaceDescriptor *indexKeyColumnSD=NULL;//SD klíčového sloupce


	if (translator->tableName.compare(tableName) == 0)//porovnání jména tabulky
	{
		bool found=false;
		for (int i = 0; i < columns->size(); i++)
		{
			string name = columns->at(i)->name;
			if (name.compare(translator->columnName) == 0)//pokud sloupec existuje tak..
			{
				found = true;
				indexColumnPosition = columns->at(i)->positionInTable;//pozice v tabulce
				indexType = columns->at(i)->cType;//typ
				size = columns->at(i)->size;
				if (size != 0)
				{
					varlenIndex = true;//yji3t2n9 jestli je slopec fixlen nebo varlenData
					indexKeyColumnSD = columns->at(i)->columnSD;
				}
				else
					indexKeyColumnSD = NULL;
				i = columns->size();//vyzkočení z foru
			}
		}
		if (found == false)
		{
			cout << "column dont exist" << endl;
			exit(0);
		}
	}
	else
	{
		cout << "table dont exist" << endl;
		exit(0);
	}

	cSpaceDescriptor *indexSD = NULL;//SD indexu
	if (varlenIndex)
	{
		indexSD = new cSpaceDescriptor(1, new cHNTuple(), indexType, false);
		indexSD->SetDimSpaceDescriptor(0, indexKeyColumnSD);
		indexSD->SetDimensionType(0, indexType);

		indexSD->Setup();
	}
	else
	{
		indexSD = new cSpaceDescriptor(1, new cTuple(), indexType, false);
	}

	if (typeOfTable == RTREE)
	{
		
			ConstructIndexRtree(indexName.c_str(), indexType, indexColumnPosition, BLOCK_SIZE, indexSD, indexKeyColumnSD,varlenIndex, DSMODE, compressionRatio, codeType, runtimeMode, histograms, quickDB);
			
			
		
	}
	else
	{


		ConstructIndexBtree(indexName.c_str(), indexType, indexColumnPosition, BLOCK_SIZE, indexSD, indexKeyColumnSD, varlenIndex ,DSMODE, compressionRatio, codeType, runtimeMode, histograms, inMemCacheSize, quickDB);

	}
	}

	inline bool cTable::CreateClusteredTable(cTranslatorCreate *translator, cQuickDB *quickDB, const unsigned int BLOCK_SIZE, uint DSMODE, static const uint structure, unsigned int compressionRatio, unsigned int codeType, unsigned int runtimeMode, bool histograms, static const uint inMemCacheSize)
{
		/*setřízení typů, aby na pozici 0 byl pk*/
		cDataType ** ptr;
		ptr = new cDataType*[columns->size()];

		cSpaceDescriptor *clusteredSD = NULL;

		for (int i = 0; i < columns->size(); i++)
		{
			int j = 1;
			if (columns->at(i)->primaryKey)
			{
				ptr[0] = columns->at(i)->cType;
			}
			else
			{
				ptr[j] = columns->at(i)->cType;
				j++;
			}


		}
		


		indexesFixLenBTree = new vector<cCompleteBTree<cTuple>*>();
		indexesVarLenBTree = new vector<cCompleteBTree<cHNTuple>*>();

		
		if (implicitKeyVarlen)
		{
			clusteredSD = new cSpaceDescriptor(columns->size(), new cHNTuple(), ptr);
			
			unsigned int keySize = keySD->GetSize();
			unsigned int pom = keySD->GetDimensionSize(0);

			cCompleteBTree<cTuple>*index = new cCompleteBTree<cTuple>(tableName.c_str(), translator->keyPosition, BLOCK_SIZE, keySD, keySize, SD->GetSize()-keySize, varlenData, DSMODE, structure, compressionRatio, codeType, runtimeMode, histograms, inMemCacheSize, quickDB);


			if (index != NULL)
			{
				indexesFixLenBTree->push_back(index);
				//indexesFixLen->push_back(index);
				return true;
			}

		}
		else if (implicitKeyVarlen == false)
		{
			clusteredSD = new cSpaceDescriptor(columns->size(), new cTuple(), ptr);
			unsigned int keySize = keySD->GetSize();

			cCompleteBTree<cTuple>*index = new cCompleteBTree<cTuple>(tableName.c_str(), translator->keyPosition, BLOCK_SIZE, keySD, keySize, SD->GetSize()-keySize, varlenData, DSMODE, structure, compressionRatio, codeType, runtimeMode, histograms, inMemCacheSize, quickDB);


			if (index != NULL)
			{
				indexesFixLenBTree->push_back(index);
				//indexesFixLen->push_back(index);
				return true;
			}
		}
		else
		{
			return false;
		}
}



	inline void cTable::SetValues(cTuple * tuple, cSpaceDescriptor * SD)
	{

		if (typeOfTable == RTREE)
		{


			cRTree<cTuple> *mKeyIndex = indexesFixLenRTree->at(0)->mIndex;
			cTuple *keyTuple = NULL;
			if (implicitKeyVarlen && homogenous)
			{
				keyTuple = TransportItemHomoVarlen(tuple, varlenKeyColumnSD, keySD, indexesFixLenRTree->at(0)->indexColumnPosition, keyType);
			}
			else
			{
				keyTuple = TransportItemFixLen(tuple, keySD, indexesFixLenRTree->at(0)->indexColumnPosition, keyType);
			}

			unsigned int nodeid, position;
			seqHeapFix->mSeqArray->AddItem(nodeid, position, *tuple);
			mKeyIndex->Insert(*keyTuple, LoadIndexData(nodeid, position));

		}
		else if (typeOfTable == BTREE)
		{

			cBpTree<cTuple> *mKeyIndex = indexesFixLenBTree->at(0)->mIndex;

			cTuple *keyTuple = NULL;
			if (implicitKeyVarlen && homogenous)
			{
				keyTuple = TransportItemHomoVarlen(tuple, varlenKeyColumnSD, keySD, indexesFixLenBTree->at(0)->indexColumnPosition, keyType);
			}
			else
			{
				keyTuple = TransportItemFixLen(tuple, keySD, indexesFixLenBTree->at(0)->indexColumnPosition, keyType);
			}

			unsigned int nodeid, position;
			seqHeapFix->mSeqArray->AddItem(nodeid, position, *tuple);
			mKeyIndex->Insert(*keyTuple, LoadIndexData(nodeid, position));

		}
		else if(typeOfTable==CLUSTERED_TABLE_BTREE)
		{



		}
		else if (typeOfTable == CLUSTERED_TABLE_RTREE)
		{

		}
		else
		{
			cout << "Error in set tuple method" << endl;
			cin.get();
			exit(0);
		}
}

inline void cTable::SetValues(cHNTuple *tuple, cSpaceDescriptor *SD)//nastavení hodnopty záznamu a vložení primárního klíče do b-stromu pro primarni kliče
{
	
	if (typeOfTable == RTREE)
	{


		cRTree<cTuple> *mKeyIndex = indexesFixLenRTree->at(0)->mIndex;
		cTuple *keyTuple = new cTuple();
		if (implicitKeyVarlen == false)
		{
			keyTuple = TransportItemFixLen(tuple, keySD, indexesFixLenRTree->at(0)->indexColumnPosition, keyType);
		}
		else
		{
			keyTuple = TransportItemVarLen(tuple, varlenKeyColumnSD, keySD, indexesFixLenRTree->at(0)->indexColumnPosition, keyType);
		}
		int h = keyTuple->GetInt(0, keySD);

			unsigned int nodeid, position;
			seqHeapVar->mSeqArray->AddItem(nodeid, position, *tuple);
			mKeyIndex->Insert(*keyTuple, LoadIndexData(nodeid, position));
		
			
		
	}
	else if(typeOfTable==BTREE)
	{
		
			cBpTree<cTuple> *mKeyIndex = indexesFixLenBTree->at(0)->mIndex;
			cTuple *keyTuple = NULL;
			
			if (implicitKeyVarlen == false)
			{
				keyTuple = TransportItemFixLen(tuple, keySD, indexesFixLenBTree->at(0)->indexColumnPosition, keyType);
			}
			else
			{
				keyTuple = TransportItemVarLen(tuple, varlenKeyColumnSD, keySD, indexesFixLenBTree->at(0)->indexColumnPosition, keyType);
			}

			int key = keyTuple->GetInt(0,keySD);

			unsigned int nodeid, position;
			seqHeapVar->mSeqArray->AddItem(nodeid, position, *tuple);
			bool hm=mKeyIndex->Insert(*keyTuple, LoadIndexData(nodeid, position));
			if (hm)
			{
				cout << "value inserted"<<endl;
			}
			else
			{
				cout << "not inserted" << endl;
			}
	}
	else if (typeOfTable == CLUSTERED_TABLE_BTREE)
	{
		cBpTree<cTuple> *mKeyIndex = indexesFixLenBTree->at(0)->mIndex;
		cTuple *keyTuple = TransportItemFixLen(tuple, keySD, indexesFixLenBTree->at(0)->indexColumnPosition, keyType);

		
		mKeyIndex->Insert(*keyTuple, NULL);


	}
	else if (typeOfTable == CLUSTERED_TABLE_RTREE)
	{
		cRTree<cTuple> *mKeyIndex = indexesFixLenRTree->at(0)->mIndex;
		cTuple *keyTuple = new cTuple();
		if (implicitKeyVarlen == false)
		{
			keyTuple = TransportItemFixLen(tuple, keySD, indexesFixLenRTree->at(0)->indexColumnPosition, keyType);
		}
		else
		{
			keyTuple = TransportItemVarLen(tuple, varlenKeyColumnSD, keySD, indexesFixLenRTree->at(0)->indexColumnPosition, keyType);
		}
		int h = keyTuple->GetInt(0, keySD);

		mKeyIndex->Insert(*keyTuple, NULL);
	}
	else
	{
		cout << "Error in set tuple method" << endl;
		cin.get();
		exit(0);
	}
}


/*
inline cTuple* cTable::FindKey(string column, int searchedValue)
{
	int searchedColumn;
	bool succes=false;

	for (int i = 0; i < columns->size(); i++)
	{
		if (columns->at(i)->name.compare(column) == 0)
		{
			searchedColumn = columns->at(i)->positionInTable;
		}
	}

	cBpTreeHeader<cTuple> *mKeyHeader;
	cBpTree<cTuple> *mKeyIndex;
	
	for (int i = 0; i < indexesFixLen->size(); i++)
	{
		if (indexesFixLen->at(i)->indexColumnPosition == searchedColumn)
		{
			mKeyHeader = indexesFixLen->at(i)->mIndexHeader;
			mKeyIndex = indexesFixLen->at(i)->mIndex;
		}
		
	}*/
	/*funguje spravně jen pro keyIndex*/
/*
	int rowId;
	int nodeCount = mKeyHeader->GetNodeCount();
	int itemCount = mKeyHeader->GetItemCount();
	cTuple *searchedTuple = NULL;
	
	for (int i = 1; i <= nodeCount; i++)
	{
		cBpTreeNode<cTuple> *node = mKeyIndex->ReadLeafNodeR(i);
		int itemCount = node->GetItemCount();

		for (int i = 0; i < itemCount; i++)
		{
			char *itemData = (char*)node->GetCItem(i);
			int itemValue = cCommonNTuple<int>::GetInt(itemData, 0, keySD);
			if (itemValue == searchedValue)
			{
				rowId = cCommonNTuple<int>::GetInt(itemData, 1, keySD);
				searchedTuple = vHeap.at(rowId - 1);
				i = itemCount;//vyzkočení z foru
			}
		}
	}
	if (searchedTuple != NULL)
	{
		return searchedTuple;
	}
	else
		return 0;
		*/
/*
	int rowId;
	int nodeCount = mKeyHeader->GetNodeCount();
	int itemCount = mKeyHeader->GetItemCount();
	cTuple *searchedTuple = NULL;

	for (int i = 1; i <= nodeCount; i++)//pruchod všech uzlu
	{
		cBpTreeNode<cTuple> *node = mKeyIndex->ReadLeafNodeR(i);//přečteni jednotlivých uzlu
		int itemCount = node->GetItemCount();

		for (int i = 0; i < itemCount; i++)//pruchod jednotlivych itemu v uzlu
		{
			char *itemData = (char*)node->GetCItem(i);//vytahnuti dat z uzlu
			int itemValue = cCommonNTuple<int>::GetInt(itemData, 0, keySD);//překlad na int
			if (itemValue == searchedValue)
			{
				rowId = cCommonNTuple<int>::GetInt(itemData, 1, keySD);//přirazeni spravne rowID
				searchedTuple = (cTuple*)vHeap.at(rowId - 1);//navraceni celého záznamu ktery odpovida rowID
				i = itemCount;//vyzkočení z foru
			}
		}
	}
	if (searchedTuple != NULL)
	{
		return searchedTuple;
	}
	else
		return 0;

		
}*/
/*
inline cTuple * cTable::FindKey(float searchedValue)
{
	cBpTreeHeader<cTuple> *mKeyHeader = indexesFixLen->at(0)->mIndexHeader;
	cBpTree<cTuple> *mKeyIndex = indexesFixLen->at(0)->mIndex;

	int rowId;
	int nodeCount = mKeyHeader->GetNodeCount();
	int itemCount = mKeyHeader->GetItemCount();
	cTuple *searchedTuple = NULL;

	for (int i = 1; i <= nodeCount; i++)
	{
		cBpTreeNode<cTuple> *node = mKeyIndex->ReadLeafNodeR(i);
		int itemCount = node->GetItemCount();

		for (int i = 0; i < itemCount; i++)
		{
			char *itemData = (char*)node->GetCItem(i);
			float itemValue = cCommonNTuple<float>::GetFloat(itemData, 0, keySD);
			if (itemValue == searchedValue)
			{
				rowId = cCommonNTuple<int>::GetInt(itemData, 1, keySD);
				searchedTuple = (cTuple*)vHeap.at(rowId - 1);
			}
		}
	}
	if (searchedTuple != NULL)
	{
		return searchedTuple;
	}
	else
		return 0;
}
*/
inline cTuple * cTable::TransportItemFixLen(cTuple *sourceTuple, cSpaceDescriptor *mSd, int columnPosition, cDataType *mType)
{
	cTuple *destTuple=new cTuple(mSd);
	
	if (mType->GetCode() == 'i')//int
	{
		int key = sourceTuple->GetInt(columnPosition, SD);
		destTuple->SetValue(0, key, mSd);
	}
	else if (mType->GetCode() == 'u')//uint
	{
		unsigned int key = sourceTuple->GetUInt(columnPosition, SD);
		destTuple->SetValue(0, key, mSd);
	}
	else if (mType->GetCode() == 'f')//float, nepodporovan
	{
		float key = sourceTuple->GetFloat(columnPosition, SD);
		destTuple->SetValue(0, key, mSd);
	}
	else if (mType->GetCode() == 'c')//char(nejasny Get)
	{
		char key= sourceTuple->GetWChar(columnPosition, SD);
		destTuple->SetValue(0, key, mSd);
	}
	else if (mType->GetCode() == 's')//short
	{
		short key = sourceTuple->GetUShort(columnPosition, SD);
		destTuple->SetValue(0, key, mSd);
	}
	else if (mType->GetCode() == 'S')//unsigned short
	{
		unsigned short key = sourceTuple->GetUShort(columnPosition, SD);
		destTuple->SetValue(0, key, mSd);
	}
	return destTuple;
}
//inline cHNTuple * cTable::TransportItemVarLen(cHNTuple *sourceTuple, cSpaceDescriptor *columnSD, cSpaceDescriptor *keySD, int columnPosition, cDataType *mType)
//{
//
//	cHNTuple *pokus = new cHNTuple();
//	cNTuple *varlenTuple = new cNTuple(columnSD);
//	cHNTuple *destTuple = new cHNTuple();
//
//	
//
//
//	destTuple->Resize(keySD);
//
//
//
//	if (mType->GetCode() == 'n')
//	{
//
//		char * TEMPTuple = sourceTuple->GetTuple(sourceTuple->GetData(), columnPosition, SD);
//		varlenTuple->SetData(TEMPTuple);
//
//
//		destTuple->SetValue(0, *varlenTuple, keySD);
//
//		//pokus->Resize(keySD);
//
//		///varcharTuple->SetData(TEMPTuple);
//		//destTuple->SetValue(0, *varcharTuple, SD);
//		//char a=destTuple->GetCChar(0, mSd);
//
//	}
//	else if (mType->GetCode() == 'c')//char(nejasny Get)
//	{
//		char key = sourceTuple->GetWChar(columnPosition, SD);
//		destTuple->SetValue(0, key, keySD);
//	}
//
//	return destTuple;
//	
//}

inline cTuple * cTable::TransportItemFixLen(cHNTuple * sourceTuple, cSpaceDescriptor * mSd,  int columnPosition, cDataType * mType)
{
	cTuple *destTuple = new cTuple(mSd);


	if (mType->GetCode() == 'i')//int
	{
		int key = sourceTuple->GetInt(columnPosition, SD);
		destTuple->SetValue(0, key, mSd);
	}
	else if (mType->GetCode() == 'u')//uint
	{
		unsigned int key = sourceTuple->GetUInt(columnPosition, SD);
		destTuple->SetValue(0, key, mSd);
	}
	else if (mType->GetCode() == 'f')//float, nepodporovan
	{
		float key = sourceTuple->GetFloat(columnPosition, SD);
		destTuple->SetValue(0, key, mSd);
	}
	else if (mType->GetCode() == 'c')//char(nejasny Get)
	{
		char key = sourceTuple->GetWChar(columnPosition, SD);
		destTuple->SetValue(0, key, mSd);
	}
	else if (mType->GetCode() == 's')//short
	{
		short key = sourceTuple->GetUShort(columnPosition, SD);
		destTuple->SetValue(0, key, mSd);
	}
	else if (mType->GetCode() == 'S')//unsigned short
	{
		unsigned short key = sourceTuple->GetUShort(columnPosition, SD);
		destTuple->SetValue(0, key, mSd);
	}
	return destTuple;
}

inline cHNTuple * cTable::TransportItemVarLen(cTuple * sourceTuple, cSpaceDescriptor * mSd, cSpaceDescriptor * keySD, int columnPosition, cDataType * mType)
{
	cNTuple *varlenTuple = new cNTuple(mSd);
	cHNTuple *destTuple = new cHNTuple();
	destTuple->Resize(keySD);

	if (mType->GetCode() == 'n')//varchar,neodzkoušeno
	{
		char * TEMPTuple = sourceTuple->GetTuple(sourceTuple->GetData(), columnPosition, SD);
		varlenTuple->SetData(TEMPTuple);
		destTuple->SetValue(0, *varlenTuple, keySD);

		///varcharTuple->SetData(TEMPTuple);
		//destTuple->SetValue(0, *varcharTuple, SD);
		//char a=destTuple->GetCChar(0, mSd);

	}
	else if (mType->GetCode() == 'c')//char(nejasny Get)
	{
		char key = sourceTuple->GetWChar(columnPosition, SD);
		destTuple->SetValue(0, key, mSd);
	}

	return destTuple;
}

inline cTuple * cTable::TransportItemVarLen(cHNTuple * sourceTuple, cSpaceDescriptor * columnSD, cSpaceDescriptor * keySD, int columnPosition, cDataType * mType)
{

		cNTuple *varlenTuple = new cNTuple(columnSD);
		cTuple *destTuple = new cTuple(keySD);
	
		
		if (mType->GetCode() == 'n')
		{

			char * TEMPTuple = sourceTuple->GetTuple(sourceTuple->GetData(), columnPosition, SD);
			varlenTuple->SetData(TEMPTuple);

			char a = varlenTuple->GetCChar(0, columnSD);
			
			destTuple->SetValue(0, *varlenTuple, keySD);
		}
		else if (mType->GetCode() == 'c')//char(nejasny Get)
		{
			char key = sourceTuple->GetWChar(columnPosition, SD);
			destTuple->SetValue(0, key, keySD);
		}
	
		return destTuple;
}

inline cTuple * cTable::TransportItemHomoVarlen(cTuple * sourceTuple, cSpaceDescriptor * columnSD, cSpaceDescriptor * keySD, int columnPosition, cDataType * mType)
{
	cNTuple *varlenTuple = new cNTuple(columnSD);
	cTuple *destTuple = new cTuple();
	destTuple->Resize(keySD);

	if (mType->GetCode() == 'n')//varchar,neodzkoušeno
	{
		char* huihua = sourceTuple->GetData();
		char * TEMPTuple = sourceTuple->GetTuple(sourceTuple->GetData(), columnPosition, SD);
		varlenTuple->SetData(TEMPTuple);
		destTuple->SetValue(0, *varlenTuple, keySD);

		///varcharTuple->SetData(TEMPTuple);
		//destTuple->SetValue(0, *varcharTuple, SD);
		//char a=destTuple->GetCChar(0, mSd);

	}
	else if (mType->GetCode() == 'c')//char(nejasny Get)
	{
		char key = sourceTuple->GetWChar(columnPosition, SD);
		destTuple->SetValue(0, key, columnSD);
	}

	return destTuple;
}

inline bool cTable::ConstructIndexBtree(string indexName,cDataType *indexType, int indexColumnPosition, uint blockSize, cSpaceDescriptor *indexSD, cSpaceDescriptor *indexKeyColumnSD,bool varlenKey, uint dsMode,  uint compressionRatio, unsigned int codeType, unsigned int runtimeMode, bool histograms, static const uint inMemCacheSize, cQuickDB *quickDB)
{
	
	
	
	
	cCompleteBTree<cTuple> *index = new cCompleteBTree<cTuple>(indexName.c_str(), indexColumnPosition, blockSize, indexSD, indexSD->GetSize(), sizeOfData/*indexSD->GetSize()*/, false, dsMode, cDStructConst::BTREE, compressionRatio, codeType, runtimeMode, histograms, inMemCacheSize, quickDB);


	if (index != NULL)
	{
		
		if ((implicitKeyVarlen==false && varlenData==false) || homogenous)
		{
			

				seqHeapFix->mSeqArray->OpenContext(seqHeapFix->mHeader->GetFirstNodeIndex(), 0, seqHeapFix->context);

				do
				{

					cTuple *heapTuple = new cTuple(SD);
					heapTuple->SetData(seqHeapFix->context->GetItem());
					cTuple *tuple = NULL;

					if (implicitKeyVarlen && homogenous)
					{
						tuple = TransportItemHomoVarlen(heapTuple, indexKeyColumnSD, indexSD, indexColumnPosition, indexType);//funguje
					}
					else
					{
						tuple = TransportItemFixLen(heapTuple, indexSD, indexColumnPosition, indexType);//problem s duplicitními záznamy(tomu se asi nevyhneme) jinak funkční
					}

					int a = tuple->GetInt(0, indexSD);

					bool hm=index->mIndex->Insert(*tuple, LoadIndexData(seqHeapFix->context->GetNode()->GetIndex(), seqHeapFix->context->GetPosition()));
					if (hm)
					{
						cout << "value inserted" << endl;
					}
					else
						cout << "value not inserted" << endl;
				} while (seqHeapFix->mSeqArray->Advance(seqHeapFix->context));

				seqHeapFix->mSeqArray->CloseContext(seqHeapFix->context);

				return true;



		}
		else
		{
			seqHeapVar->mSeqArray->OpenContext(seqHeapVar->mHeader->GetFirstNodeIndex(), 0, seqHeapVar->context);

			do
			{
				//(cHNTuple * sourceTuple, cSpaceDescriptor * columnSD, cSpaceDescriptor * keySD, int columnPosition, cDataType * mType)
				cHNTuple *heapTuple = new cHNTuple();
				heapTuple->Resize(SD);
				cTuple *tuple = NULL;

				heapTuple->SetData(seqHeapVar->context->GetItem());
				
				if (varlenKey == false)
				{
					tuple = TransportItemFixLen(heapTuple, indexSD, indexColumnPosition, indexType);//problem s duplicitními záznamy(tomu se asi nevyhneme) jinak funkční
				}
				else
				{
					tuple = TransportItemVarLen(heapTuple, indexKeyColumnSD, indexSD, indexColumnPosition, indexType);//asi funguje
				}

				int a = tuple->GetInt(0, indexSD);

				bool hm=index->mIndex->Insert(*tuple, LoadIndexData(seqHeapVar->context->GetNode()->GetIndex(), seqHeapVar->context->GetPosition()));
				if (hm)
				{
					cout << "value inserted" << endl;
				}
				else
					cout << "value not inserted" << endl;
			} while (seqHeapVar->mSeqArray->Advance(seqHeapVar->context));

			seqHeapVar->mSeqArray->CloseContext(seqHeapVar->context);

			return true;
		}
			
			
	}
	else
	{
		printf("Index: creation failed!\n");
		return false;
	}


}

inline bool cTable::ConstructVarlenHomoIndexBTree(string indexName, cDataType * indexType,cSpaceDescriptor *indexKeyColumnSD, int indexColumnPosition, uint blockSize, cSpaceDescriptor * indexSD, uint dsMode, uint compressionRatio, unsigned int codeType, unsigned int runtimeMode, bool histograms, const uint inMemCacheSize, cQuickDB * quickDB)
{
	cNTuple *ntp = new cNTuple(indexKeyColumnSD);
	
	cHNTuple *tp = new cHNTuple();
	tp->Resize(indexSD);
	//unsigned int v1 = tp->GetSize(keySD); tohle určitě ne moc velké
	unsigned int v2 = indexSD->GetSize();
	unsigned int length = tp->GetLength();
	unsigned int typeSize = indexSD->GetTypeSize();


	cCompleteBTree<cHNTuple>*index = new cCompleteBTree<cHNTuple>(indexName.c_str(), indexColumnPosition, blockSize, indexSD, indexSD->GetTypeSize()/*tp->GetSize(keySD)*/, sizeof(ntp) /*keySD->GetSize()*/, true, dsMode, cDStructConst::BTREE, compressionRatio, codeType, runtimeMode, histograms, inMemCacheSize, quickDB);


	if (index != NULL)
	{
		for (int i = 1; i <= vHeap.size(); i++)
		{

			cTuple *heapTuple = (cTuple*)vHeap.at(i - 1);//modeifikace


			cHNTuple *tuple = TransportItemVarLen(heapTuple, indexKeyColumnSD, indexSD, indexColumnPosition, indexType);

			tuple->SetValue(1, i - 1, indexSD);


			index->mIndex->Insert(*tuple, tuple->GetData());



		}
		indexesVarLenBTree->push_back(index);
		return true;
	}
	else
	{
		printf("Index: creation failed!\n");
		return false;
	}

	return true;
}

//inline bool cTable::ConstructVarlenIndexBTree(string indexName, cDataType * indexType, cSpaceDescriptor * indexKeyColumnSD, int indexColumnPosition, uint blockSize, cSpaceDescriptor * indexSD, uint dsMode,  uint compressionRatio, unsigned int codeType, unsigned int runtimeMode, bool histograms, const uint inMemCacheSize, cQuickDB * quickDB)
//{
//	cNTuple *ntp = new cNTuple(indexKeyColumnSD);
//	
//	cHNTuple *tp = new cHNTuple();
//	tp->Resize(indexSD);
//	//unsigned int v1 = tp->GetSize(keySD); tohle určitě ne moc velké
//	unsigned int v2 = indexSD->GetSize();
//	unsigned int length = tp->GetLength();
//	unsigned int typeSize = indexSD->GetTypeSize();
//
//
//	cCompleteBTree<cHNTuple>*index = new cCompleteBTree<cHNTuple>(indexName.c_str(), indexColumnPosition, blockSize, indexSD, indexSD->GetTypeSize()/*tp->GetSize(keySD)*/, sizeof(ntp) /*keySD->GetSize()*/, true, dsMode, cDStructConst::BTREE, compressionRatio, codeType, runtimeMode, histograms, inMemCacheSize, quickDB);
//
//
//	if (index != NULL)
//	{
//		for (int i = 1; i <= vHeap.size(); i++)
//		{
//
//			cHNTuple *heapTuple = (cHNTuple*)vHeap.at(i - 1);
//
//
//			cHNTuple *tuple = TransportItemVarLen(heapTuple, indexKeyColumnSD, indexSD, indexColumnPosition, indexType);
//
//			tuple->SetValue(1, i - 1, indexSD);
//
//
//			index->mIndex->Insert(*tuple, tuple->GetData());
//
//
//
//		}
//		indexesVarLenBTree->push_back(index);
//		return true;
//	}
//	else
//	{
//		printf("Index: creation failed!\n");
//		return false;
//	}
//
//	return true;
//}

inline bool cTable::ConstructIndexRtree(string indexName, cDataType * indexType, int indexColumnPosition, uint blockSize, cSpaceDescriptor * indexSD, cSpaceDescriptor *indexKeyColumnSD, bool varlenKey,uint dsMode,  uint compressionRatio, unsigned int codeType, unsigned int runtimeMode, bool histograms,  cQuickDB * quickDB)
{
	unsigned int sizeOfData = sizeof(uint) + sizeof(short);
	cCompleteRTree<cTuple> *index = new cCompleteRTree<cTuple>(indexName.c_str(), indexColumnPosition, blockSize, indexSD, indexSD->GetTypeSize(), sizeOfData, false, dsMode, cDStructConst::RTREE, compressionRatio, codeType, runtimeMode, histograms, quickDB);


	if (index != NULL )
	{
		

		if ((implicitKeyVarlen == false && varlenData == false) || homogenous)
		{
			seqHeapFix->mSeqArray->OpenContext(seqHeapFix->mHeader->GetFirstNodeIndex(), 0, seqHeapFix->context);

			do
			{

				cTuple *heapTuple = new cTuple(SD);
				heapTuple->SetData(seqHeapFix->context->GetItem());
				cTuple *tuple=NULL;

				if (implicitKeyVarlen && homogenous)
				{
					tuple = TransportItemHomoVarlen(tuple, varlenKeyColumnSD, keySD, indexesFixLenRTree->at(0)->indexColumnPosition, keyType);
				}
				else
				{
					tuple = TransportItemFixLen(tuple, keySD, indexesFixLenRTree->at(0)->indexColumnPosition, keyType);
				}
				

				int a = tuple->GetInt(0, indexSD);

				index->mIndex->Insert(*tuple, LoadIndexData(seqHeapFix->context->GetNode()->GetIndex(), seqHeapFix->context->GetPosition()));
			} while (seqHeapFix->mSeqArray->Advance(seqHeapFix->context));

			seqHeapFix->mSeqArray->CloseContext(seqHeapFix->context);

			return true;



		}
		else
		{
			seqHeapVar->mSeqArray->OpenContext(seqHeapVar->mHeader->GetFirstNodeIndex(), 0, seqHeapVar->context);

			do
			{
				//(cHNTuple * sourceTuple, cSpaceDescriptor * columnSD, cSpaceDescriptor * keySD, int columnPosition, cDataType * mType)
				cHNTuple *heapTuple = new cHNTuple();
				heapTuple->Resize(SD);
				cTuple *tuple = NULL;
				
				heapTuple->SetData(seqHeapVar->context->GetItem());
				if(varlenKey)
				tuple = TransportItemVarLen(heapTuple, indexKeyColumnSD,indexSD, indexColumnPosition, indexType);
				else
					tuple= TransportItemFixLen(heapTuple, indexSD, indexColumnPosition, indexType);

				int a = tuple->GetInt(0, indexSD);

				index->mIndex->Insert(*tuple, LoadIndexData(seqHeapVar->context->GetNode()->GetIndex(), seqHeapVar->context->GetPosition()));
			} while (seqHeapVar->mSeqArray->Advance(seqHeapVar->context));

			seqHeapVar->mSeqArray->CloseContext(seqHeapVar->context);

			return true;
		}
	}
	else
	{
		printf("Index: creation failed!\n");
		return false;
	}
}

inline bool cTable::ConstructVarlenHomoIndexRTree(string indexName, cDataType * indexType, cSpaceDescriptor * indexKeyColumnSD, int indexColumnPosition, uint blockSize, cSpaceDescriptor * indexSD, uint dsMode, uint compressionRatio, unsigned int codeType, unsigned int runtimeMode, bool histograms,  cQuickDB * quickDB)
{
	cNTuple *ntp = new cNTuple(indexKeyColumnSD);
	
	cHNTuple *tp = new cHNTuple();
	tp->Resize(indexSD);
	//unsigned int v1 = tp->GetSize(keySD); tohle určitě ne moc velké
	unsigned int v2 = indexSD->GetSize();
	unsigned int length = tp->GetLength();
	unsigned int typeSize = indexSD->GetTypeSize();

	
	cCompleteRTree<cHNTuple>*index = new cCompleteRTree<cHNTuple>(indexName.c_str(), indexColumnPosition, blockSize, indexSD, indexSD->GetTypeSize()/*tp->GetSize(keySD)*/, sizeof(ntp) /*keySD->GetSize()*/, true, dsMode, cDStructConst::RTREE, compressionRatio, codeType, runtimeMode, histograms, quickDB);


	if (index != NULL)
	{
		for (int i = 1; i <= vHeap.size(); i++)
		{

			cTuple *heapTuple = (cTuple*)vHeap.at(i - 1);


			cHNTuple *tuple = TransportItemVarLen(heapTuple, indexKeyColumnSD, indexSD, indexColumnPosition, indexType);

			tuple->SetValue(1, i - 1, indexSD);


		//	index->mIndex->Insert(*tuple, tuple->GetData());



		}
		indexesVarLenRTree->push_back(index);
		return true;
	}
	else
	{
		printf("Index: creation failed!\n");
		return false;
	}

	return true;
}

//inline bool cTable::ConstructVarlenIndexRTree(string indexName, cDataType * indexType, cSpaceDescriptor * indexKeyColumnSD, int indexColumnPosition, uint blockSize, cSpaceDescriptor * indexSD, uint dsMode,  uint compressionRatio, unsigned int codeType, unsigned int runtimeMode, bool histograms,  cQuickDB * quickDB)
//{
//	cNTuple *ntp = new cNTuple(indexKeyColumnSD);
//	
//	cHNTuple *tp = new cHNTuple();
//	tp->Resize(indexSD);
//	//unsigned int v1 = tp->GetSize(keySD); tohle určitě ne moc velké
//	unsigned int v2 = indexSD->GetSize();
//	unsigned int length = tp->GetLength();
//	unsigned int typeSize = indexSD->GetTypeSize();
//
//
//	cCompleteRTree<cHNTuple>*index = new cCompleteRTree<cHNTuple>(indexName.c_str(), indexColumnPosition, blockSize, indexSD, indexSD->GetTypeSize()/*tp->GetSize(keySD)*/, sizeof(ntp) /*keySD->GetSize()*/, true, dsMode, cDStructConst::RTREE, compressionRatio, codeType, runtimeMode, histograms, quickDB);
//
//
//	if (index != NULL)
//	{
//		for (int i = 1; i <= vHeap.size(); i++)
//		{
//
//			cHNTuple *heapTuple = (cHNTuple*)vHeap.at(i - 1);
//
//
//			cHNTuple *tuple = TransportItemVarLen(heapTuple, indexKeyColumnSD, indexSD, indexColumnPosition, indexType);
//
//			tuple->SetValue(1, i - 1, indexSD);
//
//
//		//	index->mIndex->Insert(*tuple, tuple->GetData());
//
//
//
//		}
//		indexesVarLenRTree->push_back(index);
//		return true;
//	}
//	else
//	{
//		printf("Index: creation failed!\n");
//		return false;
//	}
//
//	return true;
//
//}

inline char * cTable::LoadIndexData(unsigned int nodeId, unsigned int position)
{
	ushort x = (short)position;
	uint y = nodeId;

	char * data = static_cast<char*>(static_cast<void*>(&x));
	char * data2 = static_cast<char*>(static_cast<void*>(&y));
	char *datas[] = { data, data2 };
	char sizes[] = { sizeof x, sizeof y };
	char bytes[6];
	cDataType *types[] = { new cUShort(),new cUInt() };



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

	return bytes;
}




