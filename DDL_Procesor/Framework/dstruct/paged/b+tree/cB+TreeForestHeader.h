/**
*	\file cB+TreeForestHeader.h
*	\author Radim Baca
*	\version 0.1
*	\date aug 2007
*	\brief agregate all B-tree headers for B-tree forest. Also store some necessary information about this trees.
*/


#ifndef __cBpTreeForestHeader_h__
#define __cBpTreeForestHeader_h__

#include "cStream.h"
#include "cCharStream.h"
#include "cDataType.h"
#include "cB+TreeHeader.h"
#include "cSizeInfoSerialize.h"

#include <crtdbg.h>

/**
*	Agregate all B-tree headers for B-tree forest. 
*	Also store some usefull and necessary information about this trees.
*
*	\author Radim Baca
*	\version 0.1
*	\date aug 2007
**/

namespace dstruct {
	namespace paged {
		namespace bptree {

template<class TKey, class TItemType>
class cBpTreeForestHeader
{
private:
	typedef typename TKey::Type KeyType;
	typedef typename TItemType::Type ItemType;

	cCharStream mCharStream;
	cStream *mStream;
	char mFileName[256];
	bool mReadOnly;
	int mStatus;

	unsigned int mForestType;
	unsigned int mNumberOfTrees;
	unsigned int *mTreeLimits;
	unsigned int *mNodeCount;
	unsigned int mWholeNodeCount;
	unsigned int mPageSize;
	cBpTreeHeader<TKey, TItemType> **mHeaders;

	void TreeInit(cSizeInfo<KeyType> **keySizeInfo, cSizeInfo<ItemType> **leafSizeInfo,  bool duplicate);
	void ComputeNodeCounts();

	static const unsigned int MAXIMAL_HEADER_SIZE = 8192;
public:
	static const unsigned int VARIABLE_KEY = 0;
	static const unsigned int VARIABLE_LEAF = 1;

	static const int HEADER_OPEN = 0;
	static const int HEADER_NOT_A_TREE_FILE = 1;
	static const int HEADER_FILE_NOT_OPEN = 2;

	static const unsigned int TREE_NODE_CAPACITY = 25;
	static const unsigned int TREE_NODE_SIZE = 4096;
	static const unsigned int TREE_NOT_FOUND = (unsigned int)-1;

	cBpTreeForestHeader();
	~cBpTreeForestHeader();

	bool Create(char *name);
	bool Open(char *name, bool readOnly);
	void Close();

	void Resize();
	void Init(unsigned int forestType, unsigned int numberOfTrees, unsigned int *treeLimits, cSizeInfo<KeyType> **keySizeInfo, 
		cSizeInfo<ItemType> **leafSizeInfo, bool duplicate, unsigned int page_size = 4096);

	inline cBpTreeHeader<TKey, TItemType> *GetHeader(unsigned int order) { return mHeaders[order]; }
	inline unsigned int GetTreeLimit(unsigned int order) { return mTreeLimits[order]; }
	inline unsigned int GetNumberOfTrees() { return mNumberOfTrees; }
	inline unsigned int GetForestType()  { return mForestType; }
	inline unsigned int FindCorrespondingTree(unsigned int tupleSize);
	inline double GetTreeSize(unsigned int order);

	inline bool Read();
	inline bool Write();

	void PrintInfo(char *string) const;
};


template<class TKey, class TItemType> 
cBpTreeForestHeader<TKey, TItemType>::cBpTreeForestHeader() :mStream(NULL)
{
	mStatus = HEADER_FILE_NOT_OPEN;
	mTreeLimits = NULL;
	mHeaders = NULL;
	mReadOnly = false;
}

template<class TKey, class TItemType> 
cBpTreeForestHeader<TKey, TItemType>::~cBpTreeForestHeader()
{
	if (mStatus == HEADER_OPEN)
	{
		Close();
	}
}

/// Write the header attributes into header file and close the file stream
template<class TKey, class TItemType> 
void cBpTreeForestHeader<TKey, TItemType>::Close()
{
	if (mStatus == HEADER_OPEN)
	{
		Write();
		mStream->Close();
		mStatus = HEADER_FILE_NOT_OPEN;
	}
}

/// Create header file and open it
template<class TKey, class TItemType> 
bool cBpTreeForestHeader<TKey, TItemType>::Create(char *fileName)
{
	mReadOnly = false;
	if (mStream == NULL)
	{
		mStream = new cIOStream();
	}

	if (!mStream->Open(fileName, CREATE_ALWAYS))
	{		
		mStatus = HEADER_FILE_NOT_OPEN;
	}
	else
	{
		mStatus = HEADER_OPEN;
		strcpy(mFileName, fileName);
	}

	return mStatus == HEADER_OPEN;
}

/// Open the header file, read the forest header information from the disk and create the headers of every tree.
/// \param fileName Path to the header file
/// \param readOnly If the trees will be opened as a read only
/// \return True, when the file was correctly opened and headers created
template<class TKey, class TItemType> 
bool cBpTreeForestHeader<TKey, TItemType>::Open(char *fileName, bool readOnly)
{
	mReadOnly = readOnly;
	if (mStream == NULL)
	{
		mStream = new cIOStream();
	}

	if (!mStream->Open(fileName, OPEN_EXISTING))
	{		
		mStatus = HEADER_FILE_NOT_OPEN;
	}
	else
	{
		mStatus = HEADER_OPEN;
		strcpy(mFileName, fileName);
		Read();
	}

	return mStatus == HEADER_OPEN;
}

template<class TKey, class TItemType> 
void cBpTreeForestHeader<TKey, TItemType>::Resize()
{
}

/// Initialize B-tree headers
/// \param forestType indicates if the forest have varible length key items (0) or leaf items (1).
/// \param numberOfTrees Count of the trees
/// \param treeLimits Array having size equal to numberOfTrees. It has to be increasing sequence, which hold the maximal size of item of every tree in a forest
/// \param keySizeInfo Array of key size info
/// \param leafSizeInfo Array of leaf size info
/// \param duplicate If the keys can be the same (duplicate)
/// \param page_size Specify the size of the tree pages. If not specified, than it has value TREE_NODE_SIZE
template<class TKey, class TItemType> 
void cBpTreeForestHeader<TKey, TItemType>::Init(unsigned int forestType, unsigned int numberOfTrees, unsigned int *treeLimits, 
													cSizeInfo<KeyType> **keySizeInfo, cSizeInfo<ItemType> **leafSizeInfo, bool duplicate, 
													unsigned int page_size)
{
	assert(!mReadOnly);

	mNumberOfTrees = numberOfTrees;
	mForestType = forestType;
	mPageSize = page_size;

	mNodeCount = new unsigned int[mNumberOfTrees];
	mTreeLimits = new unsigned int[mNumberOfTrees];
	for (unsigned int i = 0; i < mNumberOfTrees; i++)
	{
		mTreeLimits[i] = treeLimits[i];
	}

	TreeInit(keySizeInfo, leafSizeInfo, duplicate);

}

/// Find the appropriate tree.
/// \param tupleSize The size of the tuple in bytes
/// \return Order of the tree which stores the tuples with size 'tupleSize'
template<class TKey, class TItemType> 
unsigned int cBpTreeForestHeader<TKey, TItemType>::FindCorrespondingTree(unsigned int tupleSize)
{
	for(unsigned int i = 0; i < mNumberOfTrees; i++)
	{
		if (tupleSize <= mTreeLimits[i])
		{
			return i;
		}
	}

	return TREE_NOT_FOUND;
}

/// Create all headers of the forest
template<class TKey, class TItemType> 
void cBpTreeForestHeader<TKey, TItemType>::TreeInit(cSizeInfo<KeyType> **keySizeInfo, cSizeInfo<ItemType> **leafSizeInfo,  bool duplicate)
{	
	mHeaders = new cBpTreeHeader<TKey, TItemType>*[mNumberOfTrees];

	for (unsigned int i = 0; i < mNumberOfTrees; i++)
	{
		if (duplicate)
		{
			mHeaders[i] = new cBpTreeHeader<TKey, TItemType>(keySizeInfo[i], leafSizeInfo[i]);
		} else
		{
			mHeaders[i] = new cBpTreeHeader<TKey, TItemType>(keySizeInfo[i], leafSizeInfo[i], cBpTreeHeader<TKey, TItemType>::TREECODE_BPTREE);
		}


		mHeaders[i]->SetNodeItemSize(TKey::GetSerSize(*(keySizeInfo[i])));
		mHeaders[i]->SetLeafNodeItemSize(TKey::GetSerSize(*(keySizeInfo[i])) + TItemType::GetSerSize(*(leafSizeInfo[i])));

		mHeaders[i]->SetNodeSize(mPageSize);
		mHeaders[i]->ComputeNodeCapacity();
		mHeaders[i]->ComputeNodeSize(true);
	}	
}

/// Not implemented
template<class TKey, class TItemType> 
void cBpTreeForestHeader<TKey, TItemType>::PrintInfo(char *string) const
{
}

/// Retrieve node count information from headers.
template<class TKey, class TItemType> 
void cBpTreeForestHeader<TKey, TItemType>::ComputeNodeCounts()
{
	mWholeNodeCount = 0;

	for (unsigned int i = 0; i < mNumberOfTrees; i++)
	{
		mNodeCount[i] = mHeaders[i]->GetNodeCount();
		mWholeNodeCount += mNodeCount[i];
	}
}

/// Read header values from stream and create the headers from this informations
template<class TKey, class TItemType> 
bool cBpTreeForestHeader<TKey, TItemType>::Read()
{	
	bool ret, duplicates;
	cSizeInfo<KeyType> **keySizeInfo;
	cSizeInfo<ItemType> **itemSizeInfo;

	assert(mStatus == HEADER_OPEN);

	mStream->Seek(0);
	mCharStream.Read(mStream, MAXIMAL_HEADER_SIZE);
	mCharStream.Seek(0);
	ret = mCharStream.Read((char*)&mForestType, sizeof(mForestType));
	ret &= mCharStream.Read((char*)&mNumberOfTrees, sizeof(mNumberOfTrees));
	ret &= mCharStream.Read((char*)&mPageSize, sizeof(mPageSize));
	ret &= mCharStream.Read((char*)&duplicates, sizeof(char));
	ret &= mCharStream.Read((char*)&mWholeNodeCount, sizeof(unsigned int));

	if (mTreeLimits != NULL)
	{
		delete mTreeLimits;
	}
	mTreeLimits = new unsigned int[mNumberOfTrees];
	mNodeCount = new unsigned int[mNumberOfTrees];
	for (unsigned int i = 0; i < mNumberOfTrees; i++)
	{
		ret &= mCharStream.Read((char*)&mTreeLimits[i], sizeof(unsigned int));
		ret &= mCharStream.Read((char*)&mNodeCount[i], sizeof(unsigned int));
	}

	keySizeInfo = new cSizeInfo<KeyType>*[mNumberOfTrees];
	itemSizeInfo = new cSizeInfo<ItemType>*[mNumberOfTrees];
	for (unsigned int i = 0; i < mNumberOfTrees; i++)
	{
		ret &= cSizeInfoSerialize<KeyType>::Read(&mCharStream, &keySizeInfo[i]);
		ret &= cSizeInfoSerialize<ItemType>::Read(&mCharStream, &itemSizeInfo[i]);
	}	

	TreeInit(keySizeInfo, itemSizeInfo, duplicates);
	return ret;
}

/// Write header into stream. Write all values in header apart the header themselves.
template<class TKey, class TItemType> 
bool cBpTreeForestHeader<TKey, TItemType>::Write()
{
	bool ret, duplicates = mHeaders[0]->DuplicatesAllowed();

	assert(mStatus == HEADER_OPEN);


	mCharStream.Seek(0);
	ret = mCharStream.Write((char*)&mForestType, sizeof(mForestType));
	ret &= mCharStream.Write((char*)&mNumberOfTrees, sizeof(mNumberOfTrees));
	ret &= mCharStream.Write((char*)&mPageSize, sizeof(mPageSize));
	ret &= mCharStream.Write((char*)&duplicates, sizeof(char));
	ComputeNodeCounts();
	ret &= mCharStream.Write((char*)&mWholeNodeCount, sizeof(unsigned int));
	for (unsigned int i = 0; i < mNumberOfTrees; i++)
	{
		ret &= mCharStream.Write((char*)&mTreeLimits[i], sizeof(unsigned int));
		ret &= mCharStream.Write((char*)&mNodeCount[i], sizeof(unsigned int));
	}

	for (unsigned int i = 0; i < mNumberOfTrees; i++)
	{
		ret &= cSizeInfoSerialize<KeyType>::Write(&mCharStream, mHeaders[i]->GetKeySizeInfo());
		ret &= cSizeInfoSerialize<ItemType>::Write(&mCharStream, mHeaders[i]->GetItemSizeInfo());
	}	

	assert(mCharStream.GetPos() < MAXIMAL_HEADER_SIZE);

	mStream->Seek(0);
	mCharStream.Write(mStream, MAXIMAL_HEADER_SIZE);

	return ret;
}

/// This method is used during the opening for estimating the necessary cache for every tree
/// \param order Order of the tree in the forest
/// \return The size of the tree compared to other trees. Return the double value from 0 to 1
template<class TKey, class TItemType> 
inline double cBpTreeForestHeader<TKey, TItemType>::GetTreeSize(unsigned int order)
{
	return ((double)mNodeCount[order]/ (double)mWholeNodeCount);
}
}}}
#endif