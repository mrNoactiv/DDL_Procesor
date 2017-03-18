/**
*	\file cB+TreeForest.h
*	\author Radim Baca
*	\version 0.1
*	\date aug 2007
*	\brief implements b-tree forest 
*/


#ifndef __cBpTreeForest_h__
#define __cBpTreeForest_h__

#include "cStream.h"
#include "b+tree/cB+TreeForestHeader.h"
#include "cTreeTuple.h"
#include "cPersistentArray.h"

#define TREE_COUNT 5

/**
*	Represents B-tree forest. Basicaly, there can be two types of btree forest. 
* One which has variable key and one having variable leaf item. 
*
*	\author Radim Baca
*	\version 0.1
*	\date aug 2007
**/

namespace dstruct {
	namespace paged {
		namespace bptree {

template<class cBpTreeType, class BII, class BLI, class TKey, class TLeafType>
class cBpTreeForest
{
	typedef typename TKey::Type BKey;
	typedef typename TLeafType::Type BLeaf;

	cBpTreeForestHeader<TKey, TLeafType> *mHeader;
	cBpTreeType **mTrees;
	BLI **mItems;

	unsigned int mLastTreeRQ;

	static const unsigned int FILENAME_SIZE = 512;
public:
	static const unsigned int RANGEQUERY_TREE_UKNOWN = (unsigned int)-1;

	cBpTreeForest(cBpTreeForestHeader<TKey, TLeafType> *header);
	cBpTreeForest();
	~cBpTreeForest();

	void Init();
	inline void Clear();
	inline void Flush();

	bool Create(const char* filename, unsigned int cacheSize);
	bool Open(const char* fileName, bool readOnly, unsigned int cacheSize);
	void Close();

	bool Find(const BLI  &item) { UNREFERENCED_PARAMETER(item); return false; }
	inline bool PointQuery(const BLI &item, unsigned int realSize) { UNREFERENCED_PARAMETER(item); UNREFERENCED_PARAMETER(realSize); return false; }
	int RangeQuery(const BLI & il, const BLI & ih, unsigned int realSize, unsigned int finninsh_size = 0);
	inline cPersistentArray<BLI>* GetQueryResult();
	inline void GetMaxIds(unsigned int *maxIds);

	//tNodeIndex LocateLeaf(const LIT &item);
	inline bool Insert(const BKey &key, const BLeaf &leaf, unsigned int realSize);
	inline bool InsertIntoTree(const BKey &key, const BLeaf &leaf, unsigned int tree);
	inline bool UpdateOrInsert(const BKey &key, const BLeaf &leaf, unsigned int realSize);
	inline unsigned int FindCorrespondingTree(unsigned int size);

	void Print() const {}
	void PrintInfo() const;
	void SetDebug(bool value);
};

/// Constructor
template<class cBpTreeType, class BII, class BLI, class TKey, class TLeafType>
cBpTreeForest<cBpTreeType, BII, BLI, TKey, TLeafType>::cBpTreeForest(cBpTreeForestHeader<TKey, TLeafType> *header)
	:mHeader(header), mTrees(NULL), mItems(NULL)
{
}

/// Constructor
template<class cBpTreeType, class BII, class BLI, class TKey, class TLeafType>
cBpTreeForest<cBpTreeType, BII, BLI, TKey, TLeafType>::cBpTreeForest():mHeader(NULL), mTrees(NULL), mItems(NULL)
{
}

/// Destructor. The header must be deleted after the tree!
template<class cBpTreeType, class BII, class BLI, class TKey, class TLeafType>
cBpTreeForest<cBpTreeType, BII, BLI, TKey, TLeafType>::~cBpTreeForest()
{
	if (mTrees != NULL)
	{
		for (unsigned int i = 0; i < mHeader->GetNumberOfTrees(); i++)
		{
			delete mTrees[i];
		}
		delete[] mTrees;
		mTrees = NULL;
	}
	if (mItems != NULL)
	{
		for (unsigned int i = 0; i < mHeader->GetNumberOfTrees(); i++)
		{
			delete mItems[i];
		}
		delete[] mItems;
		mItems = NULL;
	}
	mHeader = NULL;
}

/// Initialize the object. Set some default values
template<class cBpTreeType, class BII, class BLI, class TKey, class TLeafType>
void cBpTreeForest<cBpTreeType, BII, BLI, TKey, TLeafType>::Init()
{
	mTrees = new cBpTreeType*[mHeader->GetNumberOfTrees()];
	mItems = new BLI*[mHeader->GetNumberOfTrees()];
	for (unsigned int i = 0; i < mHeader->GetNumberOfTrees(); i++)
	{
		mTrees[i] = new cBpTreeType(mHeader->GetHeader(i));
		mItems[i] = new BLI(mHeader->GetHeader(i));
	}
}

/// Create the forest tree files and forest header file.
/// \param filename The whole path of the forest files. Header gets ".bfheader" extension to this filename and trees gets ".bf"+number
/// \param cacheSize The cache size of the files
/// \return True if everything was succesfully opened
template<class cBpTreeType, class BII, class BLI, class TKey, class TLeafType>
bool cBpTreeForest<cBpTreeType, BII, BLI, TKey, TLeafType>::Create(const char* filename, unsigned int cacheSize)
{
	char name[FILENAME_SIZE], number[10];

	Init();
	strcpy_s(name, FILENAME_SIZE ,filename);
	strcat_s(name, FILENAME_SIZE, ".bfheader");
	if (!mHeader->Create(name))
	{
		printf("cBpTreeForest::Create() - header creation failed!\n");
		return false;
	}

	for (unsigned int i = 0; i < mHeader->GetNumberOfTrees(); i++)
	{
		strcpy_s(name, FILENAME_SIZE ,filename);
		itoa(i, number, 10);
		strcat_s(name, FILENAME_SIZE, ".bf");
		strcat_s(name, FILENAME_SIZE, number);

		if (!mTrees[i]->Create(name, cacheSize))
		{
			printf("cBpTreeForest::Create() - tree %d creation failed!\n", i);
			return false;
		}
	}

	return true;
}

/// Open the forest tree files and forest header file.
/// \param filename The whole path of the forest files. Header gets ".bfheader" extension to this filename and trees gets ".bf"+number
/// \param cacheSize The cache size of the files
/// \return True if everything was succesfully opened
template<class cBpTreeType, class BII, class BLI, class TKey, class TLeafType>
bool cBpTreeForest<cBpTreeType, BII, BLI, TKey, TLeafType>::Open(const char* filename, bool readOnly, unsigned int cacheSize)
{
	char name[FILENAME_SIZE], number[10];
	bool isNull = false;

	strcpy_s(name, FILENAME_SIZE ,filename);
	strcat_s(name, FILENAME_SIZE, ".bfheader");
	if (!mHeader->Open(name, readOnly))
	{
		printf("cBpTreeForest::Open() - header creation failed!\n");
		return false;
	}
	Init();

	if (mTrees == NULL)
	{
		mTrees = new cBpTreeType*[mHeader->GetNumberOfTrees()];
		isNull = true;
	}
	for (unsigned int i = 0; i < mHeader->GetNumberOfTrees(); i++)
	{
		strcpy_s(name, FILENAME_SIZE ,filename);
		itoa(i, number, 10);
		strcat_s(name, FILENAME_SIZE, ".bf");
		strcat_s(name, FILENAME_SIZE, number);

		if (isNull)
		{
			mTrees[i] = new cBpTreeType(mHeader->GetHeader(i));
		}
		if (!mTrees[i]->Open(name, readOnly, (unsigned int)(cacheSize * mHeader->GetTreeSize(i))))
		{
			printf("cBpTreeForest::Open() - tree %d creation failed!\n", i);
			return false;
		}
	}

	return true;
}

/// Write and close the header of the forest. Close all trees of the forest
template<class cBpTreeType, class BII, class BLI, class TKey, class TLeafType>
void cBpTreeForest<cBpTreeType, BII, BLI, TKey, TLeafType>::Close()
{
	mHeader->Close();
	for (unsigned int i = 0; i < mHeader->GetNumberOfTrees(); i++)
	{
		mTrees[i]->Close();
	}
}

/// Method find appropriate tree depending of the realSize of the variable item. After that it insert the item into the tree.
/// \param key Value of the key of the item
/// \param leaf Leaf value of the item
/// \param realSize Real size of the variable length tuple in number of numbers (chars, integers)
/// \return True if the item was inserted succesfuly
template<class cBpTreeType, class BII, class BLI, class TKey, class TLeafType>
bool cBpTreeForest<cBpTreeType, BII, BLI, TKey, TLeafType>::Insert(const BKey &key, const BLeaf &leaf, unsigned int realSize)
{
	unsigned int tree = mHeader->FindCorrespondingTree(realSize);

	return InsertIntoTree(key, leaf, tree);
}

/// Insert the item into the tree.
/// \param key Value of the key of the item
/// \param leaf Leaf value of the item
/// \param tree Order of the tree into which the item should be inserted
/// \return True if the item was inserted succesfuly
template<class cBpTreeType, class BII, class BLI, class TKey, class TLeafType>
bool cBpTreeForest<cBpTreeType, BII, BLI, TKey, TLeafType>::InsertIntoTree(const BKey &key, const BLeaf &leaf, unsigned int tree)
{
	mItems[tree]->SetKey(key);
	mItems[tree]->SetLeafItem(leaf);
	return mTrees[tree]->Insert(*mItems[tree]);
}

/// Insert the item into the tree.
/// \param key Value of the key of the item
/// \param leaf Leaf value of the item
/// \param realSize The number of numbers in the tuple
/// \return True if the item was inserted succesfuly
template<class cBpTreeType, class BII, class BLI, class TKey, class TLeafType>
bool cBpTreeForest<cBpTreeType, BII, BLI, TKey, TLeafType>::UpdateOrInsert(const BKey &key, const BLeaf &leaf, unsigned int realSize)
{
	unsigned int tree = mHeader->FindCorrespondingTree(realSize);

	mItems[tree]->SetKey(key);
	mItems[tree]->SetLeafItem(leaf);
	return mTrees[tree]->UpdateOrInsert(*mItems[tree]);
}

/// \return Order of the tree where a tuple with length 'size' belongs 
template<class cBpTreeType, class BII, class BLI, class TKey, class TLeafType>
unsigned int cBpTreeForest<cBpTreeType, BII, BLI, TKey, TLeafType>::FindCorrespondingTree(unsigned int size)
{
	return mHeader->FindCorrespondingTree(size);
}

/// Perform range query on appropriate tree in the forest. Appropriate tree is find depending on the realSize variable.
/// \param il Lower value of the range query.
/// \param ih Higher value of the range query.
/// \param realSize Real size of the tuple in the key, or the concrete tree if the key is only number. If the key is only number, the real size can have also RANGEQUERY_TREE_UKNOWN. This means that the forest will try search in all trees.
template<class cBpTreeType, class BII, class BLI, class TKey, class TLeafType>
int cBpTreeForest<cBpTreeType, BII, BLI, TKey, TLeafType>::RangeQuery(const BLI & il, const BLI & ih, unsigned int realSize, unsigned int finnish_size)
{
	unsigned int tree, ret;

	if (mHeader->GetForestType() == cBpTreeForestHeader<TKey, TLeafType>::VARIABLE_KEY)
	{
		tree = mHeader->FindCorrespondingTree(realSize);
	} else
	{
		if (realSize == RANGEQUERY_TREE_UKNOWN)
		{
			/// TODO search all trees
			for (unsigned int i = 0; i < mHeader->GetNumberOfTrees(); i++)
			{
				ret = mTrees[i]->RangeQuery(il, ih, finnish_size);
				if (ret != 0)
				{
					mLastTreeRQ = ret;
					return ret;
				}
			}
			mLastTreeRQ = (unsigned int)-1;
			return 0;
		} else
		{
			tree = realSize;
		}
	}

	mLastTreeRQ = tree;
	return mTrees[tree]->RangeQuery(il, ih, finnish_size);
}

/// \return Result of the last range query.
template<class cBpTreeType, class BII, class BLI, class TKey, class TLeafType>
cPersistentArray<BLI>* cBpTreeForest<cBpTreeType, BII, BLI, TKey, TLeafType>::GetQueryResult()
{
	assert(mLastTreeRQ < mHeader->GetNumberOfTrees());

	return mTrees[mLastTreeRQ]->GetQueryResult();
}

/// Retrieve the maximal key value of every tree and return it as a unsigned int.
/// \param maxIds Method store the results into maxIds. The memory has to be alocated. If tree is empty than return 0 in corresponding 'maxIds'
template<class cBpTreeType, class BII, class BLI, class TKey, class TLeafType>
void cBpTreeForest<cBpTreeType, BII, BLI, TKey, TLeafType>::GetMaxIds(unsigned int *maxIds)
{
	const BKey *ret;

	for (unsigned int i = 0; i < mHeader->GetNumberOfTrees(); i++)
	{
		if (mTrees[i]->GetHeader()->GetLeafItemCount() > 0 && (ret = mTrees[i]->GetMaxKeyValue()) != NULL)
		{
			maxIds[i] = *ret;
		}
	}
}

/// Clear all trees from all values.
template<class cBpTreeType, class BII, class BLI, class TKey, class TLeafType>
inline void cBpTreeForest<cBpTreeType, BII, BLI, TKey, TLeafType>::Clear()
{
	for (unsigned int i = 0; i < mHeader->GetNumberOfTrees(); i++)
	{
		mTrees[i]->Clear();
	}
}

/// Flush all changes in cache nodes onto disk.
template<class cBpTreeType, class BII, class BLI, class TKey, class TLeafType>
inline void cBpTreeForest<cBpTreeType, BII, BLI, TKey, TLeafType>::Flush()
{
	for (unsigned int i = 0; i < mHeader->GetNumberOfTrees(); i++)
	{
		mTrees[i]->Flush();
	}
}

template<class cBpTreeType, class BII, class BLI, class TKey, class TLeafType>
void cBpTreeForest<cBpTreeType, BII, BLI, TKey, TLeafType>::SetDebug(bool value)
{
	for (unsigned int i = 0; i < mHeader->GetNumberOfTrees(); i++)
	{
		mTrees[i]->SetDebug(value);
	}
}

/// Print the info about the forest trees
template<class cBpTreeType, class BII, class BLI, class TKey, class TLeafType>
void cBpTreeForest<cBpTreeType, BII, BLI, TKey, TLeafType>::PrintInfo() const
{
	printf("\n****************************  Forest Info  ************************************\n");
	for (unsigned int i = 0; i < mHeader->GetNumberOfTrees(); i++)
	{
		mTrees[i]->PrintInfo();
	}
}
}}}
#endif