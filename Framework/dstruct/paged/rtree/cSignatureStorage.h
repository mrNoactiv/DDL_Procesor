/**************************************************************************}
{                                                                          }
{    cSignatureStorage.cpp                                                 }
{                                                                          }
{                                                                          }
{    Copyright (c) 2001                      Michal Kratky                 }
{                                                                          }
{    VERSION: 0.2                            DATE 01/08/2003               }
{                                                                          }
{    following functionality:                                              }
{       multidimensional signature                                         }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      xx/xx/xxxx                                                          }
{                                                                          }
{**************************************************************************/

#ifndef __cSignatureStorage_h__
#define __cSignatureStorage_h__

#include "common/cSignature.h"
#include "common/cBitString.h"
#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "common/datatype/tuple/cTuple.h"
#include "dstruct/paged/rtree/cSignatureRecord.h"
#include "dstruct/paged/rtree/cSignatureController.h"
#include "dstruct/paged/sequentialarray/cSequentialArray.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayHeader.h"
#include "dstruct/paged/b+tree/cB+Tree.h"
#include "dstruct/paged/b+tree/cB+TreeHeader.h"
#include "dstruct/paged/rtree/cInsertBuffers.h"

using namespace dstruct::paged::sqarray;
using namespace common::datatype::tuple;

namespace dstruct {
	namespace paged {
		namespace rtree {

class cChunkInfo
{
	uint mNodeIndex;
	uint mDimension;
	uint mChunkOrder;
	uint mSignatureIndex;
	uint mPosition;

public:
	cChunkInfo()
	{
	}

	cChunkInfo(uint pNodeIndex, uint pDimension, uint pChunkOrder, uint pSignatureIndex, uint pPosition)
	{
		mNodeIndex = pNodeIndex;
		mDimension = pDimension;
		mChunkOrder = pChunkOrder;
		mSignatureIndex = pSignatureIndex;
		mPosition = pPosition;
	}

	cChunkInfo(uint pNodeIndex, uint pChunkOrder, uint pSignatureIndex, uint pPosition)
	{
		mNodeIndex = pNodeIndex;
		mDimension = 0;
		mChunkOrder = pChunkOrder;
		mSignatureIndex = pSignatureIndex;
		mPosition = pPosition;
	}

	inline uint GetNodeIndex() { return mNodeIndex; }
	inline uint GetDimension() { return mDimension; }
	inline uint GetChunkOrder() { return mChunkOrder; }
	inline uint GetSignatureIndex() { return mSignatureIndex; }
	inline uint GetPosition() { return mPosition; }
};



class cSignatureStorage
{
protected:
	// Constants
	static const uint INMEMCACHE_SIZE = 1048576; //32388608;/* 4194304;*/// 2097152;
	static const uint DATA_LENGTH = 2; // first value for node index, second for signature chunk order

	// Space descriptors for signature item, and conversion table key and data
	cSpaceDescriptor* mKeySD;
	cSpaceDescriptor* mDataSD;

	cTuple* mKey;
	cTuple* mData;

	cSequentialArrayHeader<cSignatureRecord>* mArrayHeader;
	cSequentialArray<cSignatureRecord>* mArray;

	cBpTreeHeader<cTuple>* mConversionIndexHeader;
	cBpTree<cTuple>* mConversionIndex;

	cLinkedList<cChunkInfo>* mSplitChunks;

	inline bool SignatureExists(cSignatureKey* convIndexKey, tNodeIndex nodeIndex, cQueryProcStat *queryProcStat = NULL, int invLevel = -1);
	inline bool SignatureExists(cSignatureKey* convIndexKey, tNodeIndex nodeIndex, uint dimension, cQueryProcStat *queryProcStat = NULL, int invLevel = -1);
	inline bool SignatureExists(cSignatureKey* convIndexKey, tNodeIndex nodeIndex, uint dimension, uint signatureChunkOrder, cQueryProcStat *queryProcStat = NULL, int invLevel = -1);

	inline void UpdateSplitChunks(tNodeIndex nodeIndex, uint signatureChunkOrder);
	inline void UpdateSplitChunks(tNodeIndex nodeIndex, uint dimension, uint signatureChunkOrder);

public:
	void Init(cQuickDB* quickDB, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs);
	void Init(cQuickDB* quickDB, cSignatureParams* pSignatureParams, cSignatureController* pSignatureController, cSpaceDescriptor** pSignatureSDs);
	bool Create(cQuickDB* quickDB);
	bool Open(cQuickDB* quickDB, bool readOnly);
	void Close();
	void Delete();

	virtual inline cSpaceDescriptor* CreateSignatureSD(cSignatureParams* pSignatureParams, uint pDimOrder) = 0;
	virtual inline cSpaceDescriptor* CreateKeySD() = 0;

	inline cSignatureKey* CreateConversionItem(char* buffer);
	inline uint GetSignatureObjectSize(cSpaceDescriptor* pSignatureSD) const;
	inline uint GetConversionItemObjectSize() const;

	inline float GetConversionTableSizeMB(uint blockSize) const;
	inline float GetSignatureArraySizeMB(uint blockSize) const;
	void PrintStructuresInfo();
	void PrintSignaturesInfo(uint level, uint* pUniqueValues, uint pNodesCount, double* pWeights, uint* pZeros, cSignatureParams* pSignatureParams);
	void PrintSignaturesInfo(uint level, uint pItemsCount, uint pNodesCount, double pWeights, uint pZeros, cSignatureParams* pSignatureParams, cSignatureController* pSignatureController);

	virtual void ClearNodeSignature(uint nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cSignatureKey* pConvKey) = 0;
	virtual void ReplicateNodeSignature(uint destNodeIndex, uint srcNodeIndex, uint level, cSignatureParams* pSignatureParams, cInsertSigBuffers* buffers) = 0;

	virtual void CreateItemSignature(uint nodeIndex, uint level, const char* item, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cInsertSigBuffers* buffers, cSignatureController* pSignatureController = NULL, sMapItem** pMapTable = NULL, uint* pMapTableCounter = NULL) = 0;
	virtual bool IsMatched(uint nodeIndex, uint level, uint orderIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cQueryProcStat *queryProcStat, cRQBuffers<cTuple>* buffers, cSignatureController* pSignatureController = NULL) = 0;
	virtual void ComputeWeight(tNodeIndex nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, double* pWeights, uint* pZeros, cSignatureKey* convKey) = 0;

};


inline float cSignatureStorage::GetConversionTableSizeMB(uint blockSize) const
{
	return mConversionIndex->GetIndexSizeMB(blockSize);
};

inline float cSignatureStorage::GetSignatureArraySizeMB(uint blockSize) const
{
	return mArray->GetIndexSizeMB(blockSize);
};

inline cSignatureKey* cSignatureStorage::CreateConversionItem(char* buffer)
{
	return new(buffer) cSignatureKey(buffer, mKeySD);
}

inline uint cSignatureStorage::GetConversionItemObjectSize() const
{
	return cSignatureKey::GetObjectSize(mKeySD, mDataSD);
};

inline uint cSignatureStorage::GetSignatureObjectSize(cSpaceDescriptor* pSignatureSD) const
{
	return cSignatureRecord::GetObjectSize(pSignatureSD);
};

inline bool cSignatureStorage::SignatureExists(cSignatureKey* convIndexKey, tNodeIndex nodeIndex, cQueryProcStat *queryProcStat, int invLevel)
{
	convIndexKey->SetKey(nodeIndex, mKeySD);
	return mConversionIndex->PointQuery(*convIndexKey->GetKey(), convIndexKey->GetData()->GetData(), queryProcStat, invLevel);
}

inline bool cSignatureStorage::SignatureExists(cSignatureKey* convIndexKey, tNodeIndex nodeIndex, uint dimension, cQueryProcStat *queryProcStat, int invLevel)
{
	convIndexKey->SetKey(nodeIndex, dimension, mKeySD);
	return mConversionIndex->PointQuery(*convIndexKey->GetKey(), convIndexKey->GetData()->GetData(), queryProcStat, invLevel);
}

inline bool cSignatureStorage::SignatureExists(cSignatureKey* convIndexKey, tNodeIndex nodeIndex, uint dimension, uint signatureChunkOrder, cQueryProcStat *queryProcStat, int invLevel)
{
	convIndexKey->SetKey(nodeIndex, dimension, signatureChunkOrder, mKeySD);
	return mConversionIndex->PointQuery(*convIndexKey->GetKey(), convIndexKey->GetData()->GetData(), queryProcStat, invLevel);
}

inline void cSignatureStorage::UpdateSplitChunks(tNodeIndex nodeIndex, uint signatureChunkOrder)
{
	uint nOfChunks = mSplitChunks->GetItemCount();
	for (uint i = 0; i < nOfChunks; i++)
	{
		cChunkInfo* info = mSplitChunks->GetItem(i);
		if ((info->GetNodeIndex() == nodeIndex) && (info->GetChunkOrder() == signatureChunkOrder))
		{
			mSplitChunks->DeleteItem(i);
			nOfChunks--;
		}
	}
}

inline void cSignatureStorage::UpdateSplitChunks(tNodeIndex nodeIndex, uint dimension, uint signatureChunkOrder)
{
	uint nOfChunks = mSplitChunks->GetItemCount();
	for (uint i = 0; i < nOfChunks; i++)
	{
		cChunkInfo* info = mSplitChunks->GetItem(i);
		if ((info->GetNodeIndex() == nodeIndex) && (info->GetDimension() == dimension) && (info->GetChunkOrder() == signatureChunkOrder))
		{
			mSplitChunks->DeleteItem(i);
			nOfChunks--;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class cSignatureStorage_1 : public cSignatureStorage
{
	static const uint KEY_LENGTH = 1; // NODEINDEX

	virtual inline cSpaceDescriptor* CreateSignatureSD(cSignatureParams* pSignatureParams, uint pDimOrder);
	virtual inline cSpaceDescriptor* CreateKeySD();

	virtual void ClearNodeSignature(uint nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cSignatureKey* pConvKey);
	virtual void ReplicateNodeSignature(uint destNodeIndex, uint srcNodeIndex, uint level, cSignatureParams* pSignatureParams, cInsertSigBuffers* buffers);

	virtual void CreateItemSignature(uint nodeIndex, uint level, const char* item, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cInsertSigBuffers* buffers, cSignatureController* pSignatureController = NULL, sMapItem** pMapTable = NULL, uint* pMapTableCounter = NULL);
	virtual bool IsMatched(uint nodeIndex, uint level, uint orderIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cQueryProcStat *queryProcStat, cRQBuffers<cTuple>* buffers, cSignatureController* pSignatureController = NULL);
	virtual void ComputeWeight(tNodeIndex nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, double* pWeights, uint* pZeros, cSignatureKey* convKey);
};

inline cSpaceDescriptor* cSignatureStorage_1::CreateSignatureSD(cSignatureParams* pSignatureParams, uint pDimOrder)
{
	cSpaceDescriptor *signatureSD = NULL, *tupleSignatureSD = NULL;
	uint nOfDims = pSignatureParams->GetDimension();

	// tuple of signatures (dimension of tuple = dimension of r-tree)
	cSpaceDescriptor *resultSD = new cSpaceDescriptor(nOfDims, new cLNTuple(), new cLNTuple());

	for (uint i = 0; i < nOfDims; i++)
	{
		// signature of value in one dimension (dimension of tuple = length of signature)
		tupleSignatureSD = new cSpaceDescriptor(pSignatureParams->GetChunkByteSize(i), new cLNTuple(), new cChar());
		resultSD->SetDimSpaceDescriptor(i, tupleSignatureSD);
	}
	resultSD->Setup();

	return resultSD;
}

inline cSpaceDescriptor* cSignatureStorage_1::CreateKeySD()
{
	cSpaceDescriptor* resultSD = new cSpaceDescriptor(KEY_LENGTH, new cTuple(), new cUInt());
	return resultSD;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class cSignatureStorage_2 : public cSignatureStorage
{
	static const uint KEY_LENGTH = 2; // NODEINDEX, DIMENSION

	virtual inline cSpaceDescriptor* CreateSignatureSD(cSignatureParams* pSignatureParams, uint pDimOrder);
	virtual inline cSpaceDescriptor* CreateKeySD();

	virtual void ClearNodeSignature(uint nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cSignatureKey* pConvKey);
	virtual void ReplicateNodeSignature(uint destNodeIndex, uint srcNodeIndex, uint level, cSignatureParams* pSignatureParams, cInsertSigBuffers* buffers);

	virtual void CreateItemSignature(uint nodeIndex, uint level, const char* item, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cInsertSigBuffers* buffers, cSignatureController* pSignatureController = NULL, sMapItem** pMapTable = NULL, uint* pMapTableCounter = NULL);
	virtual bool IsMatched(uint nodeIndex, uint level, uint orderIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cQueryProcStat *queryProcStat, cRQBuffers<cTuple>* buffers, cSignatureController* pSignatureController = NULL);
	virtual void ComputeWeight(tNodeIndex nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, double* pWeights, uint* pZeros, cSignatureKey* convKey);
};

inline cSpaceDescriptor* cSignatureStorage_2::CreateSignatureSD(cSignatureParams* pSignatureParams, uint pDimOrder)
{
	cSpaceDescriptor *resultSD = new cSpaceDescriptor(pSignatureParams->GetChunkByteSize(pDimOrder), new cLNTuple(), new cChar());   // signature of value in one dimension (dimension of tuple = length of signature)
	return resultSD;
}

inline cSpaceDescriptor* cSignatureStorage_2::CreateKeySD()
{
	cSpaceDescriptor* resultSD = new cSpaceDescriptor(KEY_LENGTH, new cTuple(), new cUInt());
	return resultSD;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class cSignatureStorage_3 : public cSignatureStorage
{
	static const uint KEY_LENGTH = 3; // NODEINDEX, DIMENSION, CHUNKORDER

	virtual inline cSpaceDescriptor* CreateSignatureSD(cSignatureParams* pSignatureParams, uint pDimOrder);
	virtual inline cSpaceDescriptor* CreateKeySD();

	virtual void ClearNodeSignature(uint nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cSignatureKey* pConvKey);
	virtual void ReplicateNodeSignature(uint destNodeIndex, uint srcNodeIndex, uint level, cSignatureParams* pSignatureParams, cInsertSigBuffers* buffers);

	virtual void CreateItemSignature(uint nodeIndex, uint level, const char* item, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cInsertSigBuffers* buffers, cSignatureController* pSignatureController = NULL, sMapItem** pMapTable = NULL, uint* pMapTableCounter = NULL);
	virtual bool IsMatched(uint nodeIndex, uint level, uint orderIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cQueryProcStat *queryProcStat, cRQBuffers<cTuple>* buffers, cSignatureController* pSignatureController = NULL);
	virtual void ComputeWeight(tNodeIndex nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, double* pWeights, uint* pZeros, cSignatureKey* convKey);
};

inline cSpaceDescriptor* cSignatureStorage_3::CreateSignatureSD(cSignatureParams* pSignatureParams, uint pDimOrder)
{
	cSpaceDescriptor* resultSD = new cSpaceDescriptor(pSignatureParams->GetChunkByteSize(pDimOrder), new cLNTuple(), new cChar());
	return resultSD;
}

inline cSpaceDescriptor* cSignatureStorage_3::CreateKeySD()
{
	cSpaceDescriptor* resultSD = new cSpaceDescriptor(KEY_LENGTH, new cTuple(), new cUInt());
	return resultSD;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class cSignatureStorage_4 : public cSignatureStorage
{
	static const uint KEY_LENGTH = 1; // NODEINDEX

	virtual inline cSpaceDescriptor* CreateSignatureSD(cSignatureParams* pSignatureParams, uint pDimOrder);
	virtual inline cSpaceDescriptor* CreateKeySD();

	virtual void ClearNodeSignature(uint nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cSignatureKey* pConvKey);
	virtual void ReplicateNodeSignature(uint destNodeIndex, uint srcNodeIndex, uint level, cSignatureParams* pSignatureParams, cInsertSigBuffers* buffers);

	virtual void CreateItemSignature(uint nodeIndex, uint level, const char* item, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cInsertSigBuffers* buffers, cSignatureController* pSignatureController = NULL, sMapItem** pMapTable = NULL, uint* pMapTableCounter = NULL);
	virtual bool IsMatched(uint nodeIndex, uint level, uint orderIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cQueryProcStat *queryProcStat, cRQBuffers<cTuple>* buffers, cSignatureController* pSignatureController = NULL);
	virtual void ComputeWeight(tNodeIndex nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, double* pWeights, uint* pZeros, cSignatureKey* convKey);
};

inline cSpaceDescriptor* cSignatureStorage_4::CreateSignatureSD(cSignatureParams* pSignatureParams, uint pDimOrder)
{
	cSpaceDescriptor *resultSD = new cSpaceDescriptor(pSignatureParams->GetChunkByteSize(), new cLNTuple(), new cChar());
	return resultSD;
}

inline cSpaceDescriptor* cSignatureStorage_4::CreateKeySD()
{
	cSpaceDescriptor* resultSD = new cSpaceDescriptor(KEY_LENGTH, new cTuple(), new cUInt());
	return resultSD;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class cSignatureStorage_5 : public cSignatureStorage
{
	static const uint KEY_LENGTH = 2; // NODEINDEX, CHUNK_ORDER

	virtual inline cSpaceDescriptor* CreateSignatureSD(cSignatureParams* pSignatureParams, uint pDimOrder);
	virtual inline cSpaceDescriptor* CreateKeySD();

	virtual void ClearNodeSignature(uint nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cSignatureKey* pConvKey);
	virtual void ReplicateNodeSignature(uint destNodeIndex, uint srcNodeIndex, uint level, cSignatureParams* pSignatureParams, cInsertSigBuffers* buffers);

	virtual void CreateItemSignature(uint nodeIndex, uint level, const char* item, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cInsertSigBuffers* buffers, cSignatureController* pSignatureController = NULL, sMapItem** pMapTable = NULL, uint* pMapTableCounter = NULL);
	virtual bool IsMatched(uint nodeIndex, uint level, uint orderIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cQueryProcStat *queryProcStat, cRQBuffers<cTuple>* buffers, cSignatureController* pSignatureController = NULL);
	virtual void ComputeWeight(tNodeIndex nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, double* pWeights, uint* pZeros, cSignatureKey* convKey);
};

inline cSpaceDescriptor* cSignatureStorage_5::CreateSignatureSD(cSignatureParams* pSignatureParams, uint pDimOrder)
{
	cSpaceDescriptor* resultSD = new cSpaceDescriptor(pSignatureParams->GetChunkByteSize(), new cLNTuple(), new cChar());
	return resultSD;
}

inline cSpaceDescriptor* cSignatureStorage_5::CreateKeySD()
{
	cSpaceDescriptor* resultSD = new cSpaceDescriptor(KEY_LENGTH, new cTuple(), new cUInt());
	return resultSD;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class cSignatureStorage_6 : public cSignatureStorage
{
	static const uint KEY_LENGTH = 1; // NODEINDEX

	virtual inline cSpaceDescriptor* CreateSignatureSD(cSignatureParams* pSignatureParams, uint pDimOrder);
	virtual inline cSpaceDescriptor* CreateKeySD();

	virtual void ClearNodeSignature(uint nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cSignatureKey* pConvKey);
	virtual void ReplicateNodeSignature(uint destNodeIndex, uint srcNodeIndex, uint level, cSignatureParams* pSignatureParams, cInsertSigBuffers* buffers);

	virtual void CreateItemSignature(uint nodeIndex, uint level, const char* item, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cInsertSigBuffers* buffers, cSignatureController* pSignatureController = NULL, sMapItem** pMapTable = NULL, uint* pMapTableCounter = NULL);
	virtual bool IsMatched(uint nodeIndex, uint level, uint orderIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cQueryProcStat *queryProcStat, cRQBuffers<cTuple>* buffers, cSignatureController* pSignatureController = NULL);
	virtual void ComputeWeight(tNodeIndex nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, double* pWeights, uint* pZeros, cSignatureKey* convKey);
};

inline cSpaceDescriptor* cSignatureStorage_6::CreateSignatureSD(cSignatureParams* pSignatureParams, uint pDimOrder)
{
	cSpaceDescriptor* resultSD = new cSpaceDescriptor(pSignatureParams->GetChunkLength(), new cLNTuple(), new cULong());
	return resultSD;
}

inline cSpaceDescriptor* cSignatureStorage_6::CreateKeySD()
{
	cSpaceDescriptor* resultSD = new cSpaceDescriptor(KEY_LENGTH, new cTuple(), new cUInt());
	return resultSD;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class cSignatureStorage_7 : public cSignatureStorage
{
	static const uint KEY_LENGTH = 2; // NODEINDEX, CHUNK_ORDER

	virtual inline cSpaceDescriptor* CreateSignatureSD(cSignatureParams* pSignatureParams, uint pDimOrder);
	virtual inline cSpaceDescriptor* CreateKeySD();

	virtual void ClearNodeSignature(uint nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cSignatureKey* pConvKey);
	virtual void ReplicateNodeSignature(uint destNodeIndex, uint srcNodeIndex, uint level, cSignatureParams* pSignatureParams, cInsertSigBuffers* buffers);

	virtual void CreateItemSignature(uint nodeIndex, uint level, const char* item, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cInsertSigBuffers* buffers, cSignatureController* pSignatureController = NULL, sMapItem** pMapTable = NULL, uint* pMapTableCounter = NULL);
	virtual bool IsMatched(uint nodeIndex, uint level, uint orderIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cQueryProcStat *queryProcStat, cRQBuffers<cTuple>* buffers, cSignatureController* pSignatureController = NULL);
	virtual void ComputeWeight(tNodeIndex nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, double* pWeights, uint* pZeros, cSignatureKey* convKey);
};

inline cSpaceDescriptor* cSignatureStorage_7::CreateSignatureSD(cSignatureParams* pSignatureParams, uint pDimOrder)
{
	cSpaceDescriptor* resultSD = new cSpaceDescriptor(pSignatureParams->GetChunkLength(), new cLNTuple(), new cULong());
	return resultSD;
}

inline cSpaceDescriptor* cSignatureStorage_7::CreateKeySD()
{
	cSpaceDescriptor* resultSD = new cSpaceDescriptor(KEY_LENGTH, new cTuple(), new cUInt());
	return resultSD;
}

}}}
#endif
