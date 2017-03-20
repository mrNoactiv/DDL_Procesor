/**************************************************************************}
{                                                                          }
{    cRTreeSignatureIndex.h                                                }
{                                                                          }
{                                                                          }
{    Copyright (c) 2013                      Michal Kratky/Peter Chovanec  }
{                                                                          }
{    VERSION: 0.2                            DATE 15/04/2013               }
{                                                                          }
{    following functionality:                                              }
{       implementation of signature index for RTree                        }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      15/4/2013                                                           }
{                                                                          }
{**************************************************************************/

#ifndef __cRTreeSignatureIndex_h__
#define __cRTreeSignatureIndex_h__

#include <stdlib.h>
#include <stdio.h>
#include <float.h>

#include "cRTreeHeader.h"
#include "cRTreeLeafNode.h"
#include "cRTreeNode.h"
#include "cSignatureStorage.h"
#include "cSignatureController.h"
#include "cSignatureRecord.h"
#include "cSignatureKey.h"
#include "dstruct/paged/sequentialarray/cSequentialArray.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayHeader.h"
#include "dstruct/paged/b+tree/cB+Tree.h"
#include "dstruct/paged/b+tree/cB+TreeHeader.h"
#include "dstruct/paged/queryprocessing/cRQBuffers.h"
#include "dstruct/paged/rtree/cInsertBuffers.h"

using namespace dstruct::paged::sqarray;
using namespace common;

namespace dstruct {
	namespace paged {
		namespace rtree {

template<class TKey>
class cRTreeSignatureIndex
{
	typedef cRTreeLeafNode<TKey> TLeafNode; 

private:
	static const uint LEAF_LEVEL = 0;   // level of leaf signatures
	static const uint KEY_TYPES = 10;    // number of signature storages

	static const uint CHUNK_ORDER_UNSPECIFIED = cUInt::MAX;

	cSignatureController* mSignatureController;

	cSpaceDescriptor*** mSignatureSDs;
	cSignatureStorage** mSignatureStorages;
	uint* mNodesCount;

	// Dimension Independent Signatures
	double** mWeights_DIS;
	uint** mZeros_DIS;   // counters for signatures with only false bits

	// Dimension Dependent Signatures
	double* mWeights_DDS;
	uint* mZeros_DDS;   // counters for signatures with only false bits

	bool mDebug;
	bool mOpenFlag;

	sMapItem** mMapTable;
	uint* mMapTableCounter;

private:
	// Common Methods
	void Init();
	void Delete();

	// Dimension Independent Signatures
	void Init_DIS();
	void Delete_DIS();
	void CreateQuerySignature_DIS(const char* ql, cRQBuffers<TKey> *rqBuffers);

	// Dimension Dependent Signatures
	void Init_DDS_DDO();
	void Delete_DDS_DDO();
	void CreateQuerySignature_DDS(const char* ql, const char* qh, cRQBuffers<TKey> *rqBuffers);
	void CreateQuerySignature_DDO(const char* ql, const char* qh, cRQBuffers<TKey> *rqBuffers);

public:
	cRTreeSignatureIndex();
	~cRTreeSignatureIndex();

	bool Create(cRTreeHeader<TKey> *header, cQuickDB* quickDB);
	bool Open(cRTreeHeader<TKey> *header, cQuickDB* quickDB, bool readOnly);
	bool Close();

	uint Insert_presize();
	void Insert_pre(char* buffer, cInsertSigBuffers* buffers);
	uint Query_presize();
	void Query_pre(char* buffer, cRQBuffers<TKey>* buffers);

	void ClearNodeSignature(uint nodeIndex, uint level, cInsertSigBuffers* buffers);
	void CreateItemSignature(uint nodeIndex, const char* item, uint level, cInsertSigBuffers* buffers);

	void CreateNodeSignature(TLeafNode *node, cArray<uint>* path, cInsertSigBuffers* buffers);
	void CreateLeafNodeSignature(TLeafNode *node, bool exists, cInsertSigBuffers* buffers);
	void CreateNodeSignature(TLeafNode *node, uint level, uint nodeIndex, cInsertSigBuffers* buffers);
	void ModifyNodeSignature(uint nodeIndex, const TKey &item, uint level, cInsertSigBuffers* buffers);
	void ReplicateNodeSignature(uint destNodeIndex, uint srcNodeIndex, uint level, cInsertSigBuffers* buffers);

	void CreateQuerySignature(const TKey &ql, const TKey &qh, cRQBuffers<TKey> *rqBuffers);
	bool IsMatched(tNodeIndex nodeIndex, uint invLevel, cQueryProcStat *queryProcStat, cRQBuffers<TKey>* buffers);

	inline bool IsOpen() const;
	inline bool IsEnabled(unsigned int level) const;
	inline cSpaceDescriptor* GetSpaceDescriptor(unsigned int level);

	void ComputeWeight(tNodeIndex nodeIndex, uint level, cRQBuffers<TKey>* buffers);
	void PrintInfo(uint** pUniqueValues, uint* pItemsCount, uint treeHeight, bool treeStatistics = true);
	void PrintIndexSize(uint blockSize);
};

template<class TKey>
cRTreeSignatureIndex<TKey>::cRTreeSignatureIndex() 
{
}

template<class TKey>
cRTreeSignatureIndex<TKey>::~cRTreeSignatureIndex(void)
{
	switch (mSignatureController->GetSignatureType())
	{
		case cSignatureController::DimensionIndependent: 
			Delete_DIS();
			break;
		case cSignatureController::DimensionDependent:
			Delete_DDS_DDO();
			break;
		case cSignatureController::DimensionDependent_Orders:
			Delete_DDS_DDO();
			break;
	}

	if (mNodesCount != NULL)
	{
		delete mNodesCount;
	}
}

// Dimension Independent Signatures
template<class TKey>
bool cRTreeSignatureIndex<TKey>::Create(cRTreeHeader<TKey> *header, cQuickDB* quickDB)
{
	bool result = true;

	mSignatureController = header->GetSignatureController();

	switch (mSignatureController->GetSignatureType())
	{
		case cSignatureController::DimensionIndependent:
			Init_DIS();
			break;
		case cSignatureController::DimensionDependent:
			Init_DDS_DDO();
			break;
		case cSignatureController::DimensionDependent_Orders:
			Init_DDS_DDO();
			break;
		default:
			printf("Signature type is not defined !!!\n");
	}

	for (uint i = 0; i < KEY_TYPES; i++)
	{
		uint index = UINT_MAX;
		for (uint j = 0; j < mSignatureController->GetLevelsCount(); j++)
		{
			if (mSignatureController->IsLevelEnabled(j))
			{
				if (mSignatureController->GetSignatureParams(j)->GetKeyType() == i)
				{
					index = j;
				}
			}
		}

		if (index != UINT_MAX)
		{
			mSignatureStorages[i]->Init(quickDB, mSignatureController->GetSignatureParams(index), mSignatureSDs[index]);
			result &= mSignatureStorages[i]->Create(quickDB);
		}
	}

	return (mOpenFlag = result);
}

template<class TKey>
bool cRTreeSignatureIndex<TKey>::Open(cRTreeHeader<TKey> *header, cQuickDB* quickDB, bool readOnly)
{
	bool result = true;

	mSignatureController = header->GetSignatureController();

	switch (mSignatureController->GetSignatureType())
	{
		case cSignatureController::DimensionIndependent:
			Init_DIS();
			break;
		case cSignatureController::DimensionDependent:
			Init_DDS_DDO();
			break;
		case cSignatureController::DimensionDependent_Orders:
			Init_DDS_DDO();
			break;
		default:
			printf("Signature type is not defined !!!\n");
	}


	for (uint i = 0; i < KEY_TYPES; i++)
	{
		uint index = UINT_MAX;
		for (uint j = 0; j < mSignatureController->GetLevelsCount(); j++)
		{
			if (mSignatureController->IsLevelEnabled(j))
			{
				if (mSignatureController->GetSignatureParams(j)->GetKeyType() == i)
				{
					index = j;
				}
			}
		}

		if (index != UINT_MAX)
		{
			mSignatureStorages[i]->Init(quickDB, mSignatureController->GetSignatureParams(index), mSignatureSDs[index]);
			result &= mSignatureStorages[i]->Open(quickDB, readOnly);
		}
	}

	if (header->IsOnlyMemoryProcessing())
	{
		/*mConversionIndex->Preload();
		for (unsigned int i = 0; i < mSignatureController->GetLevelsCount(); i++)
		{
		mSignatureArrays[i]->Preload();
		}*/
	}

	return (mOpenFlag = result);
}


template<class TKey>
void cRTreeSignatureIndex<TKey>::Init()
{
	uint levelsCount = mSignatureController->GetLevelsCount();

	mSignatureSDs = new cSpaceDescriptor**[levelsCount];
	mSignatureStorages = new cSignatureStorage*[KEY_TYPES];
	for (uint i = 0; i < KEY_TYPES; i++)
	{
		mSignatureStorages[i] = NULL;
	}

	uint previousKeyType = UINT_MAX;
	for (uint i = 0; i < levelsCount; i++)
	{
		if (mSignatureController->IsLevelEnabled(i))
		{
			uint keyType = mSignatureController->GetSignatureParams(i)->GetKeyType();
			uint nOfDims = mSignatureController->GetSignatureParams(i)->GetDimension();
			if (keyType != previousKeyType)
			{
				switch (keyType)
				{
					case cKeyType::DIS_NODEINDEX: mSignatureStorages[keyType] = new cSignatureStorage_1();
						break;
					case cKeyType::DIS_NODEINDEX_DIMENSION: mSignatureStorages[keyType] = new cSignatureStorage_2();
						break;
					case cKeyType::DIS_NODEINDEX_DIMENSION_CHUNKORDER: mSignatureStorages[keyType] = new cSignatureStorage_3();
						break;
					case cKeyType::DDS_NODEINDEX: mSignatureStorages[keyType] = new cSignatureStorage_4();
						break;
					case cKeyType::DDS_NODEINDEX_CHUNKORDER: mSignatureStorages[keyType] = new cSignatureStorage_5();
						break;
					case cKeyType::DDO_NODEINDEX: mSignatureStorages[keyType] = new cSignatureStorage_6();
						break;
					case cKeyType::DDO_NODEINDEX_CHUNKORDER: mSignatureStorages[keyType] = new cSignatureStorage_7();
						break;
				}
				previousKeyType = keyType;
			}

			mSignatureSDs[i] = new cSpaceDescriptor*[nOfDims];
			for (uint j = 0; j < nOfDims; j++)
			{
				mSignatureSDs[i][j] = mSignatureStorages[keyType]->CreateSignatureSD(mSignatureController->GetSignatureParams(i), j);
			}
		}
	}

	// support variables
	mNodesCount = new uint[levelsCount];
	for (uint i = 0; i < levelsCount; i++)
	{
		if (mSignatureController->IsLevelEnabled(i))
		{
			mNodesCount[i] = 0;
		}
	}
}

template<class TKey>
void cRTreeSignatureIndex<TKey>::Delete()
{
	for (uint i = 0; i < mSignatureController->GetLevelsCount(); i++)
	{
		if (mSignatureController->IsLevelEnabled(i))
		{
			uint keyType = mSignatureController->GetSignatureParams(i)->GetKeyType();
			if (mSignatureStorages[keyType] != NULL) // neccessary, since storage can be delete only once for several tree levels
			{
				delete mSignatureStorages[keyType];
				mSignatureStorages[keyType] = NULL;
			}
			
			for (uint j = 0; j < mSignatureController->GetSignatureParams(i)->GetDimension(); j++)
			{
				delete mSignatureSDs[i][j];
			}
			delete mSignatureSDs[i];
		}
	}
	delete mSignatureStorages;
	delete mSignatureSDs;
}


template<class TKey>
uint cRTreeSignatureIndex<TKey>::Insert_presize()
{
	uint levelsCount = mSignatureController->GetLevelsCount();
	
	uint bufferSize = sizeof(cSignatureRecord**);
	bufferSize += levelsCount * sizeof(cSignatureRecord*);
	for (uint i = 0; i < levelsCount; i++)
	{
		if (mSignatureController->IsLevelEnabled(i))
		{
			cSignatureParams* sigParams = mSignatureController->GetSignatureParams(i);
			for (uint j = 0; j < sigParams->GetDimension(); j++)
			{
				bufferSize += mSignatureStorages[sigParams->GetKeyType()]->GetSignatureObjectSize(mSignatureSDs[i][j]);
			}
		}
	}

	bufferSize += sizeof(cSignatureKey**);
	bufferSize += levelsCount * sizeof(cSignatureKey*);
	for (uint i = 0; i < levelsCount; i++)
	{
		if (mSignatureController->IsLevelEnabled(i))
		{
			cSignatureParams* sigParams = mSignatureController->GetSignatureParams(i);
			bufferSize += mSignatureStorages[sigParams->GetKeyType()]->GetConversionItemObjectSize();
		}
	}

	return bufferSize;
}

template<class TKey>
void cRTreeSignatureIndex<TKey>::Insert_pre(char* buffer, cInsertSigBuffers* buffers)
{
	uint levelsCount = mSignatureController->GetLevelsCount();
	
	buffers->SigNodes = (cSignatureRecord***) buffer;
	buffer += levelsCount * sizeof(cSignatureRecord**);
	for (uint i = 0; i < levelsCount; i++)
	{
		if (mSignatureController->IsLevelEnabled(i))
		{
			cSignatureParams* sigParams = mSignatureController->GetSignatureParams(i);
			buffers->SigNodes[i] = (cSignatureRecord**)buffer;
			buffer += sigParams->GetDimension() * sizeof(cSignatureRecord*);

			for (uint j = 0; j < sigParams->GetDimension(); j++)
			{
				buffers->SigNodes[i][j] = new (buffer) cSignatureRecord(buffer, mSignatureSDs[i][j]);
				buffer += mSignatureStorages[sigParams->GetKeyType()]->GetSignatureObjectSize(mSignatureSDs[i][j]);
			}
		}
	}

	buffers->ConvIndexKeys = (cSignatureKey**) buffer;
	buffer += levelsCount * sizeof(cSignatureKey*);
	for (uint i = 0; i < levelsCount; i++)
	{
		if (mSignatureController->IsLevelEnabled(i))
		{
			cSignatureParams* sigParams = mSignatureController->GetSignatureParams(i);

			buffers->ConvIndexKeys[i] = mSignatureStorages[sigParams->GetKeyType()]->CreateConversionItem(buffer);
			buffer += mSignatureStorages[sigParams->GetKeyType()]->GetConversionItemObjectSize();
		}
	}
}


template<class TKey>
unsigned int cRTreeSignatureIndex<TKey>::Query_presize()
{
	uint levelsCount = mSignatureController->GetLevelsCount();

	int bufferSize = sizeof(cSignatureKey**);
	bufferSize += levelsCount * sizeof(cSignatureKey*);
	for (uint i = 0; i < levelsCount; i++)
	{
		if (mSignatureController->IsLevelEnabled(i))
		{
			cSignatureParams* sigParams = mSignatureController->GetSignatureParams(i);
			bufferSize += mSignatureStorages[sigParams->GetKeyType()]->GetConversionItemObjectSize();
		}
	}

	return bufferSize;
}

template<class TKey>
void cRTreeSignatureIndex<TKey>::Query_pre(char* buffer, cRQBuffers<TKey>* buffers)
{
	uint levelsCount = mSignatureController->GetLevelsCount();
	
	buffers->ConvIndexKeys = (cSignatureKey**) buffer;
	buffer += levelsCount * sizeof(cSignatureKey*);
	for (uint i = 0; i < levelsCount; i++)
	{
		if (mSignatureController->IsLevelEnabled(i))
		{
			cSignatureParams* sigParams = mSignatureController->GetSignatureParams(i);

			buffers->ConvIndexKeys[i] = mSignatureStorages[sigParams->GetKeyType()]->CreateConversionItem(buffer);
			buffer += mSignatureStorages[sigParams->GetKeyType()]->GetConversionItemObjectSize();
		}
	}
}

template<class TKey>
bool cRTreeSignatureIndex<TKey>::Close()
{
	if (mOpenFlag)
	{
		for (uint i = 0; i < mSignatureController->GetLevelsCount(); i++)
		{
			if (mSignatureController->IsLevelEnabled(i))
			{
				cSignatureParams* sigParams = mSignatureController->GetSignatureParams(i);
				mSignatureStorages[sigParams->GetKeyType()]->Close();
			}
		}
		
		mOpenFlag = false;
	}

	return true;
}

template<class TKey>
bool cRTreeSignatureIndex<TKey>::IsOpen() const
{
	return mOpenFlag;
}

template<class TKey>
bool cRTreeSignatureIndex<TKey>::IsEnabled(uint level) const
{
	return (level < mSignatureController->GetLevelsCount()) && (mSignatureController->IsLevelEnabled(level));
}

/// Clear signature record
template<class TKey>
void cRTreeSignatureIndex<TKey>::ClearNodeSignature(uint nodeIndex, uint level, cInsertSigBuffers* buffers)
{
	if (IsEnabled(level))
	{
		cSignatureParams* sigParams = mSignatureController->GetSignatureParams(level);
		mSignatureStorages[sigParams->GetKeyType()]->ClearNodeSignature(nodeIndex, sigParams, mSignatureSDs[level], buffers->ConvIndexKeys[level]);
	}
}

/// Create signature record for specified level
template<class TKey>
void cRTreeSignatureIndex<TKey>::CreateItemSignature(uint nodeIndex, const char* item, uint level, cInsertSigBuffers* buffers)
{
	cSignatureParams* sigParams = mSignatureController->GetSignatureParams(level);
	mSignatureStorages[sigParams->GetKeyType()]->CreateItemSignature(nodeIndex, level, item, sigParams, mSignatureSDs[level], buffers, mSignatureController, mMapTable, mMapTableCounter);
}

/// Modify signature of node with index NODEINDEX on level LEVEL with signature of inserting item ITEM
template<class TKey>
void cRTreeSignatureIndex<TKey>::ModifyNodeSignature(uint nodeIndex, const TKey &item, uint level, cInsertSigBuffers* buffers)
{
	if (IsEnabled(level))
	{
		CreateItemSignature(nodeIndex, item, level, buffers);
	}
}

/// Modify signature of node with index NODEINDEX on level LEVEL with signature of leaf node NODE
template<class TKey>
void cRTreeSignatureIndex<TKey>::CreateNodeSignature(TLeafNode *node, uint level, uint nodeIndex, cInsertSigBuffers* buffers)
{
	uint itemCount = node->GetItemCount();

	if (IsEnabled(level))
	{
		for (uint i = 0; i < itemCount; i++)
		{
			CreateItemSignature(nodeIndex, node->GetCKey(i), level, buffers);
		}
	}
}

/// Modify signatures of nodes of all allowed levels with indices stored in array PATH with signature of leaf node NODE
template<class TKey>
void cRTreeSignatureIndex<TKey>::CreateNodeSignature(TLeafNode *node, cArray<uint>* path, cInsertSigBuffers* buffers)
{
	// node signature is created before generating of signature from all leaf children nodes
	for (uint i = 0; i < mSignatureController->GetLevelsCount(); i++)
	{
		CreateNodeSignature(node, i, path->GetRefItem(path->Count() - i - 1), buffers);
	}
}

/// If signature of leaf node exists, clear it (set all bits to false)
/// Create new signature of the leaf node
template<class TKey>
void cRTreeSignatureIndex<TKey>::CreateLeafNodeSignature(TLeafNode *node, bool exists, cInsertSigBuffers* buffers)
{
	uint nodeIndex = node->GetIndex();

	if (exists)
	{
		ClearNodeSignature(nodeIndex, LEAF_LEVEL, buffers);
	}

	CreateNodeSignature(node, LEAF_LEVEL, nodeIndex, buffers);
}

/// Signature is not built, but replicated (during operation split) 
template<class TKey>
void cRTreeSignatureIndex<TKey>::ReplicateNodeSignature(uint destNodeIndex, uint srcNodeIndex, uint level, cInsertSigBuffers* buffers)
{
	cSignatureParams* sigParams = mSignatureController->GetSignatureParams(level);
	mSignatureStorages[sigParams->GetKeyType()]->ReplicateNodeSignature(destNodeIndex, srcNodeIndex, level, sigParams, buffers);
}

/// Compute the signature of the query for each enabled tree level
template<class TKey>
void cRTreeSignatureIndex<TKey>::CreateQuerySignature(const TKey &ql, const TKey &qh, cRQBuffers<TKey> *rqBuffers)
{
	switch (mSignatureController->GetSignatureType())
	{
		case cSignatureController::DimensionIndependent:
			CreateQuerySignature_DIS(ql.GetData(), rqBuffers);
			break;

		case cSignatureController::DimensionDependent:
			CreateQuerySignature_DDS(ql.GetData(), qh.GetData(), rqBuffers);
			break;

	    case cSignatureController::DimensionDependent_Orders:
			CreateQuerySignature_DDO(ql.GetData(), qh.GetData(), rqBuffers);
			break;
	}
}

/// Tests if the signature of the node matches the signature of the signature
template<class TKey>
bool cRTreeSignatureIndex<TKey>::IsMatched(tNodeIndex nodeIndex, uint invLevel, cQueryProcStat *queryProcStat, cRQBuffers<TKey>* buffers)
{
	uint orderIndex = 0;

	for (uint i = 0; i < invLevel; i++)
	{
		if (IsEnabled(i))
		{
			orderIndex += buffers->nOfLevelBits[i];
		}
	}

	cSignatureParams* sigParams = mSignatureController->GetSignatureParams(invLevel);
	return mSignatureStorages[sigParams->GetKeyType()]->IsMatched(nodeIndex, invLevel, orderIndex, sigParams, mSignatureSDs[invLevel], queryProcStat, buffers, mSignatureController);
}


/********************************** PRINTS AND SUPPORT METHODS *****************************************************/
template<class TKey>
void cRTreeSignatureIndex<TKey>::ComputeWeight(tNodeIndex nodeIndex, uint level, cRQBuffers<TKey>* buffers)
{
	cSignatureKey* convKey = buffers->ConvIndexKeys[level];

	if (IsOpen() && IsEnabled(level))
	{
		cSignatureParams* sigParams = mSignatureController->GetSignatureParams(level);
		mNodesCount[level]++;

		if (mSignatureController->GetSignatureType() == cSignatureController::DimensionIndependent)
		{
			mSignatureStorages[sigParams->GetKeyType()]->ComputeWeight(nodeIndex, sigParams, mSignatureSDs[level], mWeights_DIS[level], mZeros_DIS[level], convKey);
		}
		else
		{
			mSignatureStorages[sigParams->GetKeyType()]->ComputeWeight(nodeIndex, sigParams, mSignatureSDs[level], &mWeights_DDS[level], &mZeros_DDS[level], convKey);
		}
	}
}


template<class TKey>
void cRTreeSignatureIndex<TKey>::PrintIndexSize(uint blockSize)
{
	printf("\n#Signature Storages [MB] :");
	for (uint i = 0; i < mSignatureController->GetLevelsCount(); i++)
	{
		if (mSignatureController->IsLevelEnabled(i))
		{
			uint keyType = mSignatureController->GetSignatureParams(i)->GetKeyType();
			printf(" %d: %.2f (%.2f + %.2f)", keyType, mSignatureStorages[keyType]->GetConversionTableSizeMB(blockSize) + mSignatureStorages[keyType]->GetSignatureArraySizeMB(blockSize), mSignatureStorages[keyType]->GetConversionTableSizeMB(blockSize), mSignatureStorages[keyType]->GetSignatureArraySizeMB(blockSize));
		}
		if (i < mSignatureController->GetLevelsCount() - 1)
			printf(";");
	}
}

template<class TKey>
void cRTreeSignatureIndex<TKey>::PrintInfo(uint** pUniqueValues, uint* pItemsCount, uint treeHeight, bool treeStatistics)
{
	if (treeStatistics)
	{
		printf("\n************************* R-Tree Signature Statistics: ************************\n");
		for (uint i = 0; i < mSignatureController->GetLevelsCount(); i++)
		{
			if (mSignatureController->IsLevelEnabled(i))
			{
				cSignatureParams* sigParams = mSignatureController->GetSignatureParams(i);
				mSignatureStorages[sigParams->GetKeyType()]->PrintStructuresInfo();
			}
		}
	}

	printf("\n#Signature statistics:\n");
	for (uint i = 0; i < mSignatureController->GetLevelsCount(); i++)
	{
		if (mSignatureController->IsLevelEnabled(i))
		{
			cSignatureParams* sigParams = mSignatureController->GetSignatureParams(i);
			if (mSignatureController->GetSignatureType() == cSignatureController::DimensionIndependent)
			{
				mSignatureStorages[sigParams->GetKeyType()]->PrintSignaturesInfo(i, pUniqueValues[treeHeight - i], mNodesCount[i], mWeights_DIS[i], mZeros_DIS[i], sigParams);
			}
			else
			{
				mSignatureStorages[sigParams->GetKeyType()]->PrintSignaturesInfo(i, pItemsCount[treeHeight - i], mNodesCount[i], mWeights_DDS[i], mZeros_DDS[i], sigParams, mSignatureController);
			}
		}
	}


	if (treeStatistics)
	{
		printf("********************** End: R-Tree Signature Statistics: **********************\n");
	}
}

template<class TKey>
void cRTreeSignatureIndex<TKey>::Init_DIS()
{
	Init();

	uint levelsCount = mSignatureController->GetLevelsCount();
	mWeights_DIS = new double*[levelsCount];
	mZeros_DIS = new uint*[levelsCount];
	for (uint i = 0; i < levelsCount; i++)
	{
		if (mSignatureController->IsLevelEnabled(i))
		{
			uint nOfDims = mSignatureController->GetSignatureParams(i)->GetDimension();

			mWeights_DIS[i] = new double[nOfDims];
			mZeros_DIS[i] = new uint[nOfDims];
			for (uint j = 0; j < nOfDims; j++)
			{
				mWeights_DIS[i][j] = 0;
				mZeros_DIS[i][j] = 0;
			}
		}
	}
}

template<class TKey>
void cRTreeSignatureIndex<TKey>::Delete_DIS()
{
	Delete();

	for (uint i = 0; i < mSignatureController->GetLevelsCount(); i++)
	{
		if (mSignatureController->IsLevelEnabled(i))
		{
			delete mWeights_DIS[i];
			delete mZeros_DIS[i];
		}
	}
	delete mWeights_DIS;
	delete mZeros_DIS;
}


template<class TKey>
void cRTreeSignatureIndex<TKey>::CreateQuerySignature_DIS(const char* ql, cRQBuffers<TKey> *rqBuffers)
{
	uint levelCount = mSignatureController->GetLevelsCount();

	for (uint i = 0; i < levelCount; i++)
	{
		if (IsEnabled(i))
		{
			rqBuffers->nOfLevelBits[i] = cSignatureRecord::GenerateQuerySignature_DIS(ql, mSignatureController->GetSignatureParams(i), rqBuffers->QueryTrueBitOrders, rqBuffers->NarrowDimensions);
		}
	}
}

template<class TKey>
void cRTreeSignatureIndex<TKey>::Init_DDS_DDO()
{
	Init();

	uint levelsCount = mSignatureController->GetLevelsCount();
	mWeights_DDS = new double[levelsCount];
	mZeros_DDS = new uint[levelsCount];
	for (uint i = 0; i < levelsCount; i++)
	{
		if (mSignatureController->IsLevelEnabled(i))
		{
			mWeights_DDS[i] = 0;
			mZeros_DDS[i] = 0;
		}
	}

	uint nOfDims = mSignatureController->GetDimension();
	mMapTable = new sMapItem*[nOfDims];
	mMapTableCounter = new uint[nOfDims];
	for (uint i = 0; i < nOfDims; i++)
	{
		mMapTableCounter[i] = 0;
		mMapTable[i] = new sMapItem[mSignatureController->GetDomain(i)];
	}
}

template<class TKey>
void cRTreeSignatureIndex<TKey>::Delete_DDS_DDO()
{
	Delete();

	delete mWeights_DDS;
	delete mZeros_DDS;
}

template<class TKey>
void cRTreeSignatureIndex<TKey>::CreateQuerySignature_DDS(const char* ql, const char* qh, cRQBuffers<TKey> *rqBuffers)
{
	uint levelCount = mSignatureController->GetLevelsCount();

	for (uint i = 0; i < levelCount; i++)
	{
		if (IsEnabled(i))
		{
			cSignatureParams* sigParams = mSignatureController->GetSignatureParams(i);
			rqBuffers->nOfLevelBits[i] = cSignatureRecord::GenerateQuerySignature_DDS(ql, qh, sigParams, rqBuffers->QueryTrueBitOrders, rqBuffers->NarrowDimensions, &mMapTable[i], &mMapTableCounter[i], mSignatureController->GetDomains());
		}
	}
}

template<class TKey>
void cRTreeSignatureIndex<TKey>::CreateQuerySignature_DDO(const char* ql, const char* qh, cRQBuffers<TKey> *rqBuffers)
{
	uint levelCount = mSignatureController->GetLevelsCount();

	for (uint i = 0; i < levelCount; i++)
	{
		if (IsEnabled(i))
		{
			cSignatureParams* sigParams = mSignatureController->GetSignatureParams(i);
			rqBuffers->nOfLevelBits[i] = cSignatureRecord::GenerateQuerySignature_DDO(ql, qh, sigParams, rqBuffers->QueryTrueBitOrders, rqBuffers->NarrowDimensions, &mMapTable[i], &mMapTableCounter[i], mSignatureController->GetDomains());
		}
	}
}

}}}

#endif
