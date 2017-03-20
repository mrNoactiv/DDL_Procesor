#ifndef __cBulkLoadArray_h__
#define __cBulkLoadArray_h__

#include "common/datatype/tuple/cTuple.h"
#include <algorithm>
#include "dstruct/paged/sequentialarray/cSequentialArray.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayContext.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayHeader.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayNodeHeader.h"

using namespace common::data;
using namespace common::datatype::tuple;
using namespace dstruct::paged::sqarray;

class cSortType
{
public:
	static const uint Lexicographical = 0;
	static const uint hOrder = 1;
	static const uint zOrder = 2;
	static const uint TaxiOrder = 3;
};

template<class TKey>
class TItem
{
public:
	static void SetKey(char* pArray, uint pOrder, char* pKey, cSpaceDescriptor* pSD, uint pDataSize);
	static void SetMBR(char* pArray, uint pOrder, char* pMBR, cSpaceDescriptor* pSD);
	static void SetIKey(char* pArray, uint pOrder, char* pKey, cSpaceDescriptor* pSD);
	static char* GetKey(char* pArray, uint pOrder, cSpaceDescriptor* pSD, uint pDataSize);
	static void SetData(char* pArray, uint pOrder, char* pData, cSpaceDescriptor* pSD, uint pDataSize);
	static char* GetData(char* pArray, uint pOrder, cSpaceDescriptor* pSD, uint pDataSize);
	static void SetIndex(char* pArray, uint pOrder, uint* pNodeIndex, cSpaceDescriptor* pSD);
	static void SetMBRIndex(char* pArray, uint pOrder, uint* pNodeIndex, cSpaceDescriptor* pSD);
	static uint* GetIndex(char* pArray, uint pOrder, cSpaceDescriptor* pSD);
};


template<class TKey>
void TItem<TKey>::SetKey(char* pArray, uint pOrder, char* pKey, cSpaceDescriptor* pSD, uint pDataSize)
{
	memcpy(&pArray[(pSD->GetSize() + pDataSize) * pOrder], pKey, pSD->GetSize());
}

template<class TKey>
void TItem<TKey>::SetMBR(char* pArray, uint pOrder, char* pMBR, cSpaceDescriptor* pSD)
{
	memcpy(&pArray[((2 * pSD->GetSize() + sizeof(tNodeIndex)) * pOrder) + sizeof(tNodeIndex)], pMBR, 2 * pSD->GetSize());
}

template<class TKey>
void TItem<TKey>::SetIKey(char* pArray, uint pOrder, char* pKey, cSpaceDescriptor* pSD)
{
	memcpy(&pArray[((pSD->GetSize() + sizeof(tNodeIndex)) * pOrder) + sizeof(tNodeIndex)], pKey, pSD->GetSize());
}

template<class TKey>
char* TItem<TKey>::GetKey(char* pArray, uint pOrder, cSpaceDescriptor* pSD, uint pDataSize)
{
	return  &pArray[pOrder * (pSD->GetSize() + pDataSize)];
}

template<class TKey>
void TItem<TKey>::SetData(char* pArray, uint pOrder, char* pData, cSpaceDescriptor* pSD, uint pDataSize)
{
	memcpy(&pArray[((pSD->GetSize() + pDataSize) * pOrder) + pSD->GetSize()], pData, pDataSize);
}

template<class TKey>
char* TItem<TKey>::GetData(char* pArray, uint pOrder, cSpaceDescriptor* pSD, uint pDataSize)
{
	return  &pArray[(pOrder * (pSD->GetSize() + pDataSize)) + pSD->GetSize()];
}

template<class TKey>
void TItem<TKey>::SetIndex(char* pArray, uint pOrder, uint* pNodeIndex, cSpaceDescriptor* pSD)
{
	memcpy(&pArray[((pSD->GetSize() + sizeof(tNodeIndex)) * pOrder)], pNodeIndex, sizeof(tNodeIndex));
}

template<class TKey>
void TItem<TKey>::SetMBRIndex(char* pArray, uint pOrder, uint* pNodeIndex, cSpaceDescriptor* pSD)
{
	memcpy(&pArray[((2 * pSD->GetSize() + sizeof(tNodeIndex)) * pOrder)], pNodeIndex, sizeof(tNodeIndex));
}

template<class TKey>
uint* TItem<TKey>::GetIndex(char* pArray, uint pOrder, cSpaceDescriptor* pSD)
{
	return  &pArray[(pOrder * (pSD->GetSize() + sizeof(tNodeIndex)) + pSD->GetSize())];
}

/**
* Bulkload class for loading and sorting tuples from file or generate array of random tuples
*
* Template parameter:
*   - TKey - class of tuples ( cTuple for BpTree cMBRectangle for RTree )
*	\author Ondrej Prda
*	\date jun 2015
**/
template<class TKey>
class cBulkLoadArray
{
private:
	uint mCapacity;	// Total number of keys
	uint mDataSize;
	uint mInnerNodeCount;	// Total InnerNode count
	uint mCount;	// How many LeafNodes were inserted
	uint mInnerCount;	// How many InnerNodes were inserted
	uint nesting; // used for Introsort
	uint mLeftOverL;
	uint mLeftOverS;
	uint mCodeType;

	char* mLeafItems;
	char* mInnerItems;
	char* mAArray;	// Auxilliary array for SWAP
	char* mName;

	uint mSortType;
	cSpaceDescriptor* mSD;

private:
	void SWAP(uint left, uint right);
	int PartitionL(uint left, uint right);
	int PartitionH(uint left, uint right);
	int PartitionZ(uint left, uint right);
	int PartitionT(uint left, uint right);
	void IntrosortL(uint first, uint max, int nestLimit); // first is left boundary and max is right boundary for array
	void IntrosortH(uint first, uint max, int nestLimit); // first is left boundary and max is right boundary for array
	void IntrosortZ(uint first, uint max, int nestLimit); // first is left boundary and max is right boundary for array
	void IntrosortT(uint first, uint max, int nestLimit); // first is left boundary and max is right boundary for array
	void Heapsort(uint start, uint length);
	void HeapsortUp(uint start, uint max, int i);
	void HeapsortDown(uint start, uint max);
	bool IsSorted(uint first, uint max);
	void Clear();



public:
	cBulkLoadArray(char* pName, uint pCapacity, uint pInnerNodeCount, uint pSortType, cSpaceDescriptor* pSD, uint pDataSize, uint pCodeType);
	~cBulkLoadArray();

	inline void SetKey(uint pOrder, char* pKey);
	inline void SetMBR(uint pOrder, char* pMBR);
	inline char* GetKey(uint pOrder);
	inline void SetData(uint pOrder, char* pData);
	inline char* GetData(uint pOrder);

	void AddLeafItem(char* key, char* data);
	void AddInnerItem(char* key, tNodeIndex index);
	char* GetLeafNodeItems(uint pOrder, uint pAvgLeafItemsCount);
	char* GetLastLeafNodeItem(uint pOrder, uint pAvgLeafItemsCount, uint pRestSpace);
	char* GetInnerNodeItems(uint pOrder, uint pAvgLeafNodeItems, uint pRestSpace, bool leaf, uint keyNumber);
	char* GetInnerLastNodeItem(uint pOrder, uint pAvgInnerNodeItems, uint pRestSpace);
	char* GetInnerNodeItemsPrint(uint pOrder);	// Debug helper
	uint GetMInnerCount();
	void AddLeftOver(uint pOrder);

	void Sort();
	void BinaryFlush();
	void TextFlush();
};

template<class TKey>
cBulkLoadArray<TKey>::cBulkLoadArray(char* pName, uint pCapacity, uint pInnerNodeCount, uint pSortType, cSpaceDescriptor* pSD, uint pDataSize, uint pCodeType)
{
	mCount = 0;
	mLeftOverL = 0;
	mLeftOverS = 0;
	mInnerCount = 0;
	mCapacity = pCapacity;
	mSortType = pSortType;
	mCodeType = pCodeType;
	mInnerNodeCount = pInnerNodeCount;

	mName = pName;
	mSD = pSD;
	mDataSize = pDataSize;

	mLeafItems = new char[mCapacity *(mSD->GetSize() + mDataSize)];
	mAArray = new char[mSD->GetSize() + mDataSize]; // Auxilliary array for SWAP

	if (mCodeType == cDStructConst::BTREE)
	{
		mInnerItems = new char[(sizeof(tNodeIndex) + mSD->GetSize()) * pInnerNodeCount];
	}
	else if (mCodeType == cDStructConst::RTREE)
	{
		mInnerItems = new char[(sizeof(tNodeIndex) + 2 * mSD->GetSize()) * pInnerNodeCount];
	}


}


template<class TKey>
cBulkLoadArray<TKey>::~cBulkLoadArray()
{
	if (mLeafItems != NULL)
	{
		delete[] mLeafItems;
		mLeafItems = NULL;
	}
}


template<class TKey>
void cBulkLoadArray<TKey>::Clear()
{
	mCount = 0;
}

template<class TKey>
inline void cBulkLoadArray<TKey>::SetKey(uint pOrder, char* pKey)
{
	TItem<TKey>::SetKey(mLeafItems, pOrder, pKey, mSD, mDataSize);
}


template<class TKey>
inline void cBulkLoadArray<TKey>::SetMBR(uint pOrder, char* pMBR)
{
	TItem<TKey>::SetMBR(mInnerItems, pOrder, pMBR, mSD);
}

template<class TKey>
inline char* cBulkLoadArray<TKey>::GetKey(uint pOrder)
{
	return TItem<TKey>::GetKey(mLeafItems, pOrder, mSD, mDataSize);
}

template<class TKey>
inline void cBulkLoadArray<TKey>::SetData(uint pOrder, char* pData)
{
	TItem<TKey>::SetData(mLeafItems, pOrder, pData, mSD, mDataSize);
}

template<class TKey>
inline char* cBulkLoadArray<TKey>::GetData(uint pOrder)
{
	return TItem<TKey>::GetData(mLeafItems, pOrder, mSD, mDataSize);
}

template<class TKey>
void cBulkLoadArray<TKey>::AddLeafItem(char* key, char* data)
{
	if (mCount < mCapacity)
	{
		if (&key != NULL)
		{
			SetKey(mCount, key);
		}
		else
		{
			printf("Position %d was not filled up with key because value of key is equal to NULL \n", mCount);
		}

		if (data != NULL)
		{
			SetData(mCount, data);
		}
		else
		{
			printf("Position %d was not filled up with data because value of data is equal to NULL \n", mCount);
		}

		mCount++;
	}
	else
	{
		printf("mCount is equal or bigger than mCapacity \n");
	}
}

template<class TKey>
void cBulkLoadArray<TKey>::AddInnerItem(char* key, tNodeIndex index)
{
	if (mInnerCount < mInnerNodeCount)
	{
		if (&key != NULL)
		{
			if (mCodeType == cDStructConst::BTREE)
			{
				TItem<TKey>::SetIKey(mInnerItems, mInnerCount, key, mSD);
			}
			else if (mCodeType == cDStructConst::RTREE)
			{
				SetMBR(mInnerCount, key);
			}
		}
		else
		{
			printf("Position %d was not filled up with key because value of key is equal to NULL \n", mInnerCount);
		}

		if (index != NULL)
		{
			if (mCodeType == cDStructConst::BTREE)
			{
				TItem<TKey>::SetIndex(mInnerItems, mInnerCount, &index, mSD);
			}
			else if (mCodeType == cDStructConst::RTREE)
			{
				TItem<TKey>::SetMBRIndex(mInnerItems, mInnerCount, &index, mSD);
			}
		}
		else
		{
			printf("Position %d was not filled up with data because value of index is equal to NULL \n", mInnerCount);
		}

		mInnerCount++;
	}
	else
	{
		printf("mCount is equal or bigger than mCapacity \n");
	}
}

template<class TKey>
char* cBulkLoadArray<TKey>::GetLeafNodeItems(uint pOrder, uint pAvgLeafNodeItems)
{
	return &mLeafItems[pOrder * pAvgLeafNodeItems * (mSD->GetSize() + mDataSize)];
}

template<class TKey>
char* cBulkLoadArray<TKey>::GetLastLeafNodeItem(uint pOrder, uint pAvgLeafNodeItems, uint pRestSpace)
{
	return &mLeafItems[(pOrder + 1) * pAvgLeafNodeItems * (mSD->GetSize() + mDataSize) - (1 + pRestSpace) * (mSD->GetSize() + mDataSize)];
}

template<class TKey>
char* cBulkLoadArray<TKey>::GetInnerNodeItems(uint pOrder, uint pAvgInnerNodeItems, uint pRestSpace, bool leaf, uint keyNumber)
{
	if (leaf == true)
	{
		return &mInnerItems[pOrder * pAvgInnerNodeItems * (sizeof(tNodeIndex) + keyNumber * mSD->GetSize())];
	}
	else
	{
		return &mInnerItems[pOrder * pAvgInnerNodeItems * (sizeof(tNodeIndex) + keyNumber * mSD->GetSize()) - mLeftOverS * (sizeof(tNodeIndex) + keyNumber * mSD->GetSize())];
	}
}

// Just for test prints
template<class TKey>
char* cBulkLoadArray<TKey>::GetInnerNodeItemsPrint(uint pOrder)
{
	return &mInnerItems[pOrder * (sizeof(tNodeIndex) + 2 * mSD->GetSize())];
}

template<class TKey>
char* cBulkLoadArray<TKey>::GetInnerLastNodeItem(uint pOrder, uint pAvgInnerNodeItems, uint pRestSpace)
{
	if (pRestSpace != 0)
	{
		mLeftOverL = mLeftOverS + pRestSpace;
	}
	return &mInnerItems[(pOrder + 1) * pAvgInnerNodeItems * (sizeof(tNodeIndex) + mSD->GetSize()) - (1 + mLeftOverL) * (sizeof(tNodeIndex) + mSD->GetSize())];
}

template<class TKey>
uint cBulkLoadArray<TKey>::GetMInnerCount()
{
	return mInnerCount;
}

template<class TKey>
void cBulkLoadArray<TKey>::AddLeftOver(uint pOrder)
{
	mLeftOverS += pOrder;
}

template<class TKey>
void cBulkLoadArray<TKey>::Sort()
{
	printf("Sorting has been started \n");
	switch (mSortType)
	{
		case cSortType::Lexicographical:
			IntrosortL(0, mCount - 1, 0); 
			break;
		case cSortType::hOrder:
			IntrosortH(0, mCount - 1, 0);
			break;
		case cSortType::zOrder:
			IntrosortZ(0, mCount - 1, 0);
			break;
		case cSortType::TaxiOrder:
			IntrosortT(0, mCount - 1, 0);
			break;
	}
}


template<class TKey>
void cBulkLoadArray<TKey>::BinaryFlush()
{
	cFileStream* stream = new cFileStream();
	char* mHelper = new char[100];
	strcpy(mHelper, mName);
	strcat(mHelper, ".dat");

	stream->Open(mHelper, 0, 1);
	for (uint i = 0; i < mCount; i++)
	{
		stream->Write(mLeafItems, mCapacity*(mSD->GetSize() + mDataSize));
	}

	delete mHelper[];
	mHelper = NULL;
	stream->Close();
	stream->~cFileStream();

}

template<class TKey>
void cBulkLoadArray<TKey>::TextFlush()
{
	cFileStream* stream = new cFileStream();
	char* mHelper = new char[100];
	strcpy(mHelper, mName);
	strcat(mHelper, ".txt");

	stream->Open(mHelper, 0, 1);
	for (uint i = 0; i < mCount; i++)
	{
		cTuple::WriteAsText(GetKey(i), stream, mSD);
	}

	delete mHelper;
	mHelper = NULL;
	stream->Close();
	stream->~cFileStream();
}

template<class TKey>
void cBulkLoadArray<TKey>::SWAP(uint left, uint right)
{
	TItem<TKey>::SetKey(mAArray, 0, GetKey(left), mSD, mDataSize);
	TItem<TKey>::SetData(mAArray, 0, GetData(left), mSD, mDataSize);
	SetKey(left, GetKey(right));
	SetData(left, GetData(right));
	SetKey(right, TItem<TKey>::GetKey(mAArray, 0, mSD, mDataSize));
	SetData(right, TItem<TKey>::GetData(mAArray, 0, mSD, mDataSize));
	/*mLeafItems[mCount + 1] = mLeafItems[left];
	mLeafItems[left] = mLeafItems[right];
	mLeafItems[right] = mLeafItems[mCount + 1];*/
}

template<class TKey>
int cBulkLoadArray<TKey>::PartitionL(uint left, uint right)
{
	int index, pivot, mid = (left + right) / 2;

	if (cTuple::CompareLexicographically(GetKey(left), GetKey(mid), mSD, mSD->GetDimension()) == 1)
	{
		pivot = left;
	}
	else
	{
		pivot = mid;
	}

	if (cTuple::CompareLexicographically(GetKey(pivot), GetKey(right), mSD, mSD->GetDimension()) == 1)
	{
		pivot = right;
	}

	SWAP(pivot, right);

	pivot = right;
	index = left;

	while (left < right)
	{
		if (cTuple::CompareLexicographically(GetKey(left), GetKey(right), mSD, mSD->GetDimension()) == -1)
		{
			SWAP(index, left);
			index++;
			left++;
		}
		else
		{
			left++;
		}
	}

	SWAP(index, right);
	pivot = index;

	return pivot;
}

template<class TKey>
int cBulkLoadArray<TKey>::PartitionH(uint left, uint right)
{
	int index, pivot, mid = (left + right) / 2;

	if (cTuple::CompareHOrder(GetKey(left), GetKey(mid), mSD) == 1)
	{
		pivot = left;
	}
	else
	{
		pivot = mid;
	}

	if (cTuple::CompareHOrder(GetKey(pivot), GetKey(right), mSD) == 1)
	{
		pivot = right;
	}

	SWAP(pivot, right);

	pivot = right;
	index = left;

	while (left < right)
	{
		if (cTuple::CompareHOrder(GetKey(left), GetKey(right), mSD) == -1)
		{
			SWAP(index, left);
			index++;
			left++;
		}
		else
		{
			left++;
		}
	}

	SWAP(index, right);
	pivot = index;

	return pivot;
}

template<class TKey>
int cBulkLoadArray<TKey>::PartitionZ(uint left, uint right)
{
	int index, pivot, mid = (left + right) / 2;

	if (cTuple::CompareZOrder(GetKey(left), GetKey(mid), mSD) == 1)
	{
		pivot = left;
	}
	else
	{
		pivot = mid;
	}

	if (cTuple::CompareZOrder(GetKey(pivot), GetKey(right), mSD) == 1)
	{
		pivot = right;
	}

	SWAP(pivot, right);

	pivot = right;
	index = left;

	while (left < right)
	{
		if (cTuple::CompareZOrder(GetKey(left), GetKey(right), mSD) == -1)
		{
			SWAP(index, left);
			index++;
			left++;
		}
		else
		{
			left++;
		}
	}

	SWAP(index, right);
	pivot = index;

	return pivot;
}

template<class TKey>
int cBulkLoadArray<TKey>::PartitionT(uint left, uint right)
{
	int index, pivot, mid = (left + right) / 2;

	if (cTuple::CompareTaxiOrder(GetKey(left), GetKey(mid), mSD) == 1)
	{
		pivot = left;
	}
	else
	{
		pivot = mid;
	}

	if (cTuple::CompareTaxiOrder(GetKey(pivot), GetKey(right), mSD) == 1)
	{
		pivot = right;
	}

	SWAP(pivot, right);

	pivot = right;
	index = left;

	while (left < right)
	{
		if (cTuple::CompareTaxiOrder(GetKey(left), GetKey(right), mSD) == -1)
		{
			SWAP(index, left);
			index++;
			left++;
		}
		else
		{
			left++;
		}
	}

	SWAP(index, right);
	pivot = index;

	return pivot;
}

template<class TKey>
void cBulkLoadArray<TKey>::IntrosortL(uint left, uint right, int nestLimit)
{
	nesting = nestLimit;
	nesting++;
	//printf("Quicksort over these two borders : %i %i\r", left, right);
	if (nesting == 100)
	{
		if (left < right)
		{
			Heapsort(left, right);
		}
	}
	if (left <= right && nesting < 100)
	{
		int pivot;

		pivot = PartitionL(left, right);
		if (pivot > left)
		{
			//printf("\n");
			IntrosortL(left, pivot - 1, nesting);
		}
		if (pivot < right)
		{
			//printf("\n");
			IntrosortL(pivot + 1, right, nesting);
		}
		//}
	}
	nesting--;
}

template<class TKey>
void cBulkLoadArray<TKey>::IntrosortH(uint left, uint right, int nestLimit)
{
	nesting = nestLimit;
	nesting++;
	//printf("Quicksort over these two borders : %i %i\r", left, right);

	if (nesting == 100)
	{
		if (left < right)
		{
			Heapsort(left, right);
		}
	}
	if (left <= right && nesting < 100)
	{
		int pivot;

		pivot = PartitionH(left, right);
		if (pivot > left)
		{
			//printf("\n");
			IntrosortH(left, pivot - 1, nesting);
		}
		if (pivot < right)
		{
			//printf("\n");
			IntrosortH(pivot + 1, right, nesting);
		}
		//}
	}

	nesting--;
}

template<class TKey>
void cBulkLoadArray<TKey>::IntrosortZ(uint left, uint right, int nestLimit)
{
	nesting = nestLimit;
	nesting++;
	//printf("Quicksort over these two borders : %i %i\r", left, right);

	if (nesting == 100)
	{
		if (left < right)
		{
			Heapsort(left, right);
		}
	}
	if (left <= right && nesting < 100)
	{
		int pivot;

		pivot = PartitionZ(left, right);
		if (pivot > left)
		{
			//printf("\n");
			IntrosortZ(left, pivot - 1, nesting);
		}
		if (pivot < right)
		{
			//printf("\n");
			IntrosortZ(pivot + 1, right, nesting);
		}
		//}
	}

	nesting--;
}

template<class TKey>
void cBulkLoadArray<TKey>::IntrosortT(uint left, uint right, int nestLimit)
{
	nesting = nestLimit;
	nesting++;
	//printf("Quicksort over these two borders : %i %i\r", left, right);

	if (nesting == 100)
	{
		if (left < right)
		{
			Heapsort(left, right);
		}
	}
	if (left <= right && nesting < 100)
	{
		int pivot;

		pivot = PartitionT(left, right);
		if (pivot > left)
		{
			//printf("\n");
			IntrosortT(left, pivot - 1, nesting);
		}
		if (pivot < right)
		{
			//printf("\n");
			IntrosortT(pivot + 1, right, nesting);
		}
		//}
	}

	nesting--;
}

template<class TKey>
void cBulkLoadArray<TKey>::Heapsort(uint start, uint end)
{
	for (uint i = start; i < end + 1; i++)
	{
		HeapsortUp(start, end, i - start);
	}

	int index = end;
	while (index > start)
	{
		SWAP(index, start);
		index--;
		HeapsortDown(start, index);
	}
}

template<class TKey>
void cBulkLoadArray<TKey>::HeapsortUp(uint start, uint end, int i)
{
	int child = i;
	int parent, cmp;

	while (child != 0)
	{
		parent = (child - 1) / 2;
		switch (mSortType) {
			case cSortType::Lexicographical: cmp = cTuple::CompareLexicographically(GetKey(start + parent), GetKey(start + child), mSD, mSD->GetDimension());
				break;
			case cSortType::hOrder: cmp = cTuple::CompareHOrder(GetKey(start + parent), GetKey(start + child), mSD);
				break;
			case cSortType::zOrder: cmp = cTuple::CompareZOrder(GetKey(start + parent), GetKey(start + child), mSD);
				break;
			case cSortType::TaxiOrder: cmp = cTuple::CompareTaxiOrder(GetKey(start + parent), GetKey(start + child), mSD);
				break;
		}
		
		if (cmp == -1)
		{
			SWAP(start + parent, start + child);
			child = parent;
		}
		else
		{
			return;
		}
	}
}

template<class TKey>
void cBulkLoadArray<TKey>::HeapsortDown(uint start, uint end)
{
	int parent = 0;
	int child, cmp, cmp2;

	while (parent * 2 + 1 <= end - start)
	{
		switch (mSortType) {
			case cSortType::Lexicographical: cmp = cTuple::CompareLexicographically(GetKey(start + child), GetKey(start + child + 1), mSD, mSD->GetDimension());
				cmp2 = cTuple::CompareLexicographically(GetKey(start + parent), GetKey(start + child), mSD, mSD->GetDimension());
				break;
			case cSortType::hOrder: cmp = cTuple::CompareHOrder(GetKey(start + child), GetKey(start + child + 1), mSD);
				cmp2 = cTuple::CompareHOrder(GetKey(start + parent), GetKey(start + child), mSD);
				break;
			case cSortType::zOrder: cmp = cTuple::CompareZOrder(GetKey(start + child), GetKey(start + child + 1), mSD);
				cmp2 = cTuple::CompareZOrder(GetKey(start + parent), GetKey(start + child), mSD);
				break;
			case cSortType::TaxiOrder: cmp = cTuple::CompareTaxiOrder(GetKey(start + child), GetKey(start + child + 1), mSD);
				cmp2 = cTuple::CompareTaxiOrder(GetKey(start + parent), GetKey(start + child), mSD);
				break;
		}

		child = parent * 2 + 1;
		if (child < end - start && cmp == -1)
		{
			child++;
		}
		if (cmp2 == -1)
		{
			SWAP(start + parent, start + child);
			parent = child;
		}
		else
		{
			return;
		}
	}
}

template<class TKey>
bool cBulkLoadArray<TKey>::IsSorted(uint first, uint max)
{
	int swapnumber = 0;
	for (int i = first; i < max - 1; i++)
	{
		switch (mSortType)
		{
		case 0:
			if (cTuple::CompareZOrder(GetKey(i), GetKey(i + 1), mSD) == 1)
			{
				swapnumber++;
			}
			break;
		case 1:
			if (cTuple::CompareHOrder(GetKey(i), GetKey(i + 1), mSD) == 1)
			{
				swapnumber++;
			}
			break;
		case 2:
			if (cTuple::CompareTaxiOrder(GetKey(i), GetKey(i + 1), mSD) == 1)
			{
				swapnumber++;
			}
			break;
		case 3:
			if (cTuple::CompareLexicographically(GetKey(i), GetKey(i + 1), mSD, mSD->GetDimension()) == 1)
			{
				swapnumber++;
			}
			break;
		}

	}
	if (swapnumber > 0 || swapnumber < 0)
	{
		return false;
	}
	else if (swapnumber == 0)
	{
		return true;
	}
}

#endif