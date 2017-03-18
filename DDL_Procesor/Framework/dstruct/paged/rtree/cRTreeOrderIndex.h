/**************************************************************************}
{                                                                          }
{    cRTreeOrderIndex.h                                                }
{                                                                          }
{                                                                          }
{    Copyright (c) 2013                      Michal Kratky/Peter Chovanec  }
{                                                                          }
{    VERSION: 0.2                            DATE 15/04/2013               }
{                                                                          }
{    following functionality:                                              }
{                               }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      15/4/2013                                                           }
{                                                                          }
{**************************************************************************/

#ifndef __cRTreeOrderIndex_h__
#define __cRTreeOrderIndex_h__

#include <stdlib.h>
#include <stdio.h>
#include <float.h>

namespace dstruct {
	namespace paged {
		namespace rtree {
  // class cTreeHeader;
  template<class TKey> class cRTreeHeader;
}}}

#include "dstruct/paged/sequentialarray/cSequentialArray.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayHeader.h"
#include "dstruct/paged/b+tree/cB+Tree.h"
#include "dstruct/paged/b+tree/cB+TreeHeader.h"
#include "dstruct/paged/queryprocessing/cRQBuffers.h"
#include "dstruct/paged/rtree/cInsertBuffers.h"
#include "dstruct/paged/rtree/cRTreeHeader.h"
#include "dstruct/paged/rtree/cRTreeLeafNode.h"
#include "dstruct/paged/rtree/cRTreeNodeHeader.h"

using namespace dstruct::paged::sqarray;
using namespace common;

namespace dstruct {
	namespace paged {
		namespace rtree {

template<class TKey>
class cRTreeOrderIndex
{
	typedef cRTreeLeafNode<TKey> TLeafNode;

private:
	static const unsigned int TUPLE_LENGTH = 2; // first value for node index, second for signature chunk order
	static const unsigned int KEY_SIZE = 4;     // size of key in conversion index
	static const unsigned int DATA_SIZE = 8;    // size of data in conversion index

private:
	cSequentialArrayHeader<TKey>* mOrderArrayHeader;
	cBpTreeHeader<cUInt>* mConversionIndexHeader;

	cSpaceDescriptor* mCiSd; // space descriptor for two dimensional tuples in conversion index

	cSequentialArray<TKey>* mOrderArray;
	cBpTree<cUInt>* mConversionIndex;

	cTuple* mConvTuple;  //temporary tuple, zmeni se nazev
	TKey *mTmpTuple;//temporary tuple, zmeni se nazev

	bool mDebug;
	bool mOpenFlag;

private:
	void Init(cRTreeHeader<TKey> *header, cQuickDB *quickDb);

public:
	cRTreeOrderIndex();
	~cRTreeOrderIndex();

	bool Create(cRTreeHeader<TKey> *header, cQuickDB* quickDB);
	bool Open(cRTreeHeader<TKey> *header, cQuickDB* quickDB, bool readOnly);
	bool Close();

	inline bool IsOpen() const;
	inline bool IsEnabled(unsigned int level) const;

	inline void SetTuple(cTuple* tuple, uint firstValue, uint secondValue);
	inline void CharToInt(const char* data, uint& indexID, uint& positionID) const;
	bool InsertOrUpdateTuple(tNodeIndex nodeIndex, const TKey& key);
	cTuple* GetTuple(tNodeIndex nodeIndex, const cSpaceDescriptor* sd) const;
	void GetTupleIndex(tNodeIndex nodeIndex, uint& indexID, uint& positionID) const;
	void PrintFT (tNodeIndex nodeIndex, const cSpaceDescriptor* sd) const;
	void PrintInfo();
};

template<class TKey>
cRTreeOrderIndex<TKey>::cRTreeOrderIndex() 
{
}

template<class TKey>
cRTreeOrderIndex<TKey>::~cRTreeOrderIndex(void)
{
	if (mOrderArrayHeader != NULL)
	{
		delete mOrderArrayHeader;
	}

	if (mOrderArray != NULL)
	{
		delete mOrderArray;
	}

	if (mConversionIndexHeader != NULL)
	{
		delete mConversionIndexHeader;
	}

	if (mConversionIndex != NULL)
	{
		delete mConversionIndex;
	}

	if (mCiSd != NULL)
	{
		delete mCiSd;
		mCiSd = NULL;
	}
}

template<class TKey>
bool cRTreeOrderIndex<TKey>::Create(cRTreeHeader<TKey> *header, cQuickDB* quickDB)
{
	Init(header, quickDB);

	mOrderArray = new cSequentialArray<TKey>();
	if (!mOrderArray->Create(mOrderArrayHeader, quickDB))
	{
		printf("Order array: creation failed\n");
		return false;
	}

	mConversionIndex = new cBpTree<cUInt>();
	if (!mConversionIndex->Create(mConversionIndexHeader, quickDB))
	{
		printf("Conversion index: creation failed!\n");
	}

	return (mOpenFlag = true);
}

template<class TKey>
bool cRTreeOrderIndex<TKey>::Open(cRTreeHeader<TKey> *header, cQuickDB* quickDB, bool readOnly)
{
	Init(header, quickDB);

	mOrderArray = new cSequentialArray<TKey>();
	if (!mOrderArray->Open(mOrderArrayHeader, quickDB))
	{
		printf("Order array: open failed\n");
		return false;
	}

	mConversionIndex = new cBpTree<cUInt>();
	if (!mConversionIndex->Open(mConversionIndexHeader, quickDB, readOnly))
	{
		printf("Conversion index: creation failed!\n");
	}

	return (mOpenFlag = true);
}

template<class TKey>
void cRTreeOrderIndex<TKey>::Init(cRTreeHeader<TKey> *header, cQuickDB *quickDB)
{
	unsigned int blockSize = quickDB->GetNodeCache()->GetBlockSize();

	char orderArrayName[cCommon::STRING_LENGTH];
	strcpy(orderArrayName, "orderArray_");
	strcat(orderArrayName, header->GetName());

	char conversionArrayName[cCommon::STRING_LENGTH];
	strcpy(conversionArrayName, "conversionArray_");
	strcat(conversionArrayName, header->GetName());

	mOrderArrayHeader = new cSequentialArrayHeader<TKey>(orderArrayName, blockSize, header->GetSpaceDescriptor(), cDStructConst::DSMODE_DEFAULT);
	mCiSd = new cSpaceDescriptor(TUPLE_LENGTH, new cTuple(), new cUInt());
	mConversionIndexHeader = new cBpTreeHeader<cUInt>(conversionArrayName, blockSize, mCiSd, KEY_SIZE, DATA_SIZE, false, cDStructConst::DSMODE_DEFAULT);

	mConvTuple = new cTuple(mCiSd);
//	mTmpTuple = new TKey(header->GetSpaceDescriptor());
}

template<class TKey>
bool cRTreeOrderIndex<TKey>::Close()
{
	if (mOpenFlag)
	{
		mConversionIndex->Close();
		
		// mk: mSignatureController? Asi omyl.
		// for (unsigned int i = 0; i < mSignatureController->GetLevelsCount(); i++)
		// {
		//	mOrderArray[i]->Close();
		// }

		mOpenFlag = false;
	}
	return true;
}

template<class TKey>
bool cRTreeOrderIndex<TKey>::IsOpen() const
{
	return mOpenFlag;
}

template<class TKey>
void cRTreeOrderIndex<TKey>::SetTuple(cTuple* tuple, uint firstValue, uint secondValue)
{
	tuple->SetValue(0, firstValue, mCiSd);
	tuple->SetValue(1, secondValue, mCiSd);
}

template<class TKey>
void cRTreeOrderIndex<TKey>::CharToInt(const char* data, uint& indexID, uint& positionID) const
{
	uint* tmpData = (uint*)(data + 4); //fk 4=delka cuint=nodeindex
	indexID = tmpData[0];
	positionID = tmpData[1];
}

template<class TKey>
bool cRTreeOrderIndex<TKey>::InsertOrUpdateTuple(tNodeIndex nodeIndex, const TKey& key)
{
	uint indexID, positionID;
	GetTupleIndex(nodeIndex, indexID, positionID);
	
	if (indexID == -1)
	{
		mOrderArray->AddItem(indexID, positionID, key);  // ... ulozeni do pole., vlozi a vrati x,y
		
		SetTuple(mConvTuple, indexID, positionID);
		//mConvTuple->Print("\n", mCiSd);
		mConversionIndex->Insert(nodeIndex, mConvTuple->GetData());
	}
	else
	{
		mOrderArray->UpdateItem(indexID, positionID, key);
	}

	//mConversionIndex->Print(0);
	//printf("\nNode: %u, indexID: %u, positionID: %u", nodeIndex, indexID, positionID);

	return true;
}

/// Get tuple record.
template<class TKey>
cTuple* cRTreeOrderIndex<TKey>::GetTuple(tNodeIndex nodeIndex, const cSpaceDescriptor* sd) const
{
	uint indexID, positionID;
	GetTupleIndex(nodeIndex, indexID, positionID);
	bool ret = mOrderArray->GetItem(indexID, positionID, mTmpTuple->GetData());
	//mTmpTuple->Print("\n", sd);
	return mTmpTuple;
}

/// Get tuple indexes (indexID and positionID)
template<class TKey>
void cRTreeOrderIndex<TKey>::GetTupleIndex(tNodeIndex nodeIndex, uint& indexID, uint& positionID) const
{
	cTreeItemStream<cUInt>* resultSet = mConversionIndex->PointQuery(nodeIndex);
	if (resultSet->GetItemCount() == 1)
	{
		CharToInt(resultSet->GetItem(), indexID, positionID);
	}
	else
	{
		indexID = -1;
		positionID = -1;
	}
	resultSet->CloseResultSet();
}

/********************************** PRINTS AND SUPPORT METHODS *****************************************************/

template<class TKey>
void cRTreeOrderIndex<TKey>::PrintInfo()
{
}

template<class TKey>
void cRTreeOrderIndex<TKey>::PrintFT (tNodeIndex nodeIndex, const cSpaceDescriptor* sd) const
{
	TKey *firstTuple = new TKey(sd);
	*(firstTuple) = *(GetTuple(nodeIndex, sd));
	printf("FT:");
	firstTuple->Print("\n", sd);
}
/**/
}}}

#endif