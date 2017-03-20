#ifndef __cSignatureController_h__
#define __cSignatureController_h__

#include "common/cCommon.h"
#include "common/cNumber.h"
#include <stdio.h>
#include <math.h>
#include <assert.h>

using namespace common;

class cKeyType
{
public:
	static const uint DIS_NODEINDEX = 1;
	static const uint DIS_NODEINDEX_DIMENSION = 2;
	static const uint DIS_NODEINDEX_DIMENSION_CHUNKORDER = 3;
	static const uint DDS_NODEINDEX = 4;
	static const uint DDS_NODEINDEX_CHUNKORDER = 5;
	static const uint DDO_NODEINDEX = 6;
	static const uint DDO_NODEINDEX_CHUNKORDER = 7;
};

class cSignatureParams
{
	uint* mLengths;
	uint* mChunkLengths;
	uint* mChunkByteSizes;
	uint* mChunkCounts;
	uint* mBitCounts;
	uint mKeyType;
	uint mDimension;
	uint mSignatureType;

public:
	cSignatureParams(uint pDimension)
	{
		mLengths = new uint[pDimension];
		mChunkLengths = new uint[pDimension];
		mChunkByteSizes = new uint[pDimension];
		mChunkCounts = new uint[pDimension];
		mBitCounts = new uint[pDimension];
	}

	~cSignatureParams()
	{
		delete mLengths;
		delete mChunkLengths;
		delete mChunkByteSizes;
		delete mChunkCounts;
		delete mBitCounts;
	}

	inline void SetLength(uint pLength, uint pOrder = 0) { mLengths[pOrder] = pLength; }
	inline void SetChunkLength(uint pChunkLength, uint pOrder = 0) { mChunkLengths[pOrder] = pChunkLength; }
	inline void SetChunkByteSize(uint pChunkByteSize, uint pOrder = 0) { mChunkByteSizes[pOrder] = pChunkByteSize; }
	inline void SetChunkCount(uint pChunkCount, uint pOrder = 0) { mChunkCounts[pOrder] = pChunkCount; }
	inline void SetBitCount(uint pBitCount, uint pOrder = 0) { mBitCounts[pOrder] = pBitCount; }
	inline void SetKeyType(uint pKeyType) { mKeyType = pKeyType; }
	inline void SetDimension(uint pDimension) { mDimension = pDimension; }
	inline void SetSignatureType(uint pSignatureType) { mSignatureType = pSignatureType; }

	inline uint GetLength(uint pOrder = 0) { return mLengths[pOrder]; }
	inline uint GetChunkLength(uint pOrder = 0) { return mChunkLengths[pOrder]; }
	inline uint GetChunkByteSize(uint pOrder = 0) { return mChunkByteSizes[pOrder]; }
	inline uint GetChunkCount(uint pOrder = 0) { return mChunkCounts[pOrder]; }
	inline uint GetBitCount(uint pOrder = 0) { return mBitCounts[pOrder]; }
	inline uint GetKeyType() { return mKeyType; }
	inline uint GetDimension() { return mDimension; }
	inline uint GetSignatureType() { return mSignatureType; }

	inline uint GetMaxLength()
	{
		uint maxLength = 0;
		for (uint i = 0; i < mDimension; i++)
		{
			maxLength = (maxLength > mLengths[i]) ? maxLength : mLengths[i];
		}
		return maxLength;
	}
};


class cSignatureController
{
protected:
	uint mLevelsCount;
	uint mBuildType;
	uint mSignatureType;
	uint mSignatureQuality;

	bool* mLevelEnabled;
	cSignatureParams** mSignatureParams;

	float mBLengthConstant;
	uint* mChunkCapacity_DDO;
	uint* mMaxChunksCount_DDO;
	uint mDimension;

	uint mRQBits; // number of bits range query in the case of DD Signature; ql = qh => mRQBits = 1
	uint* mDomains;
	uint mQueryTypesCount;
	uint mCurrentQueryType;
	bool** mQueryTypes;
private:
	uint ByteAlignment(uint bitLength);

public:
	static const uint SignatureBuild_Insert = 0;
	static const uint SignatureBuild_Bulkload = 1;

	static const uint PerfectSignature = 0;
	static const uint ImperfectSignature = 1;

	static const uint DimensionIndependent = 0;
	static const uint DimensionDependent = 1;
	static const uint DimensionDependent_Orders = 2;

	
public:
	cSignatureController(uint levelsCount, uint queryTypesCount, uint dimension);
	~cSignatureController();

	void Setup_DIS(uint dimension, uint nodeItemCapacity, uint leafNodeItemCapacity, uint blockSize);
	void Setup_DDS(uint pDimension, uint pNodeItemCapacity, uint pLeafNodeItemCapacity, uint pNodeFreeSpace);
	void Setup_DDO(uint pDimension, uint pNodeItemCapacity, uint pLeafNodeItemCapacity, uint pNodeFreeSpace);

	inline uint GetLevelsCount() const;
	inline bool IsLevelEnabled(uint pInvLevel) const;
	inline void SetLevelEnabled(bool* pLevelEnabled);

	inline cSignatureParams* GetSignatureParams(uint pInvLevel);

	inline uint GetChunkCapacity_DDO(uint pInvLevel) const;
	inline uint GetMaxChunksCount_DDO(uint pInvLevel) const;

	inline void SetBitCount(uint pBitCount, uint pInvLevel, uint pDimension);
	inline void SetBitLengthConstant(float pBLengthConstant);

	inline void SetDomains(uint* pDomains);
	inline uint* GetDomains() const;
	inline uint GetDomain(uint pDim) const;

	inline void SetQueryTypesCount(uint pQueryTypesCount);
	inline void SetQueryTypes(bool* pQueryTypes, uint pOrder);
	inline uint GetQueryTypesCount() const;
	inline bool* GetQueryType(uint pOrder) const;
	inline void SetCurrentQueryType(uint pQueryType);
	inline uint GetCurrentQueryType() const;
	
	inline uint GetBuildType() const;
	inline void SetBuildType(uint pBuildType);

	inline uint GetSignatureType() const;
	inline void SetSignatureType(uint pSignatureType);

	inline uint GetSignatureQuality() const;
	inline void SetSignatureQuality(uint pSignatureQuality);

	inline void SetRQBits(uint pRQBits);
	inline uint GetRQBits() const;

	inline uint GetDimension() const;

	void Print();
};

inline uint cSignatureController::GetLevelsCount() const
{
	return mLevelsCount;
}


inline bool cSignatureController::IsLevelEnabled(uint pInvLevel) const
{
	assert(pInvLevel < mLevelsCount);
	return mLevelEnabled[pInvLevel];
}


inline void cSignatureController::SetLevelEnabled(bool* pSigEnabledLevel)
{
	for (uint i = 0 ; i < mLevelsCount ; i++)
	{
		mLevelEnabled[i] = pSigEnabledLevel[i];
	}
}

inline void cSignatureController::SetBitCount(uint pBitCount, uint pInvLevel, uint pDimension)
{
	mSignatureParams[pInvLevel]->SetBitCount(pBitCount, pDimension);
}

inline void cSignatureController::SetBitLengthConstant(float pBLengthConstant)
{
	mBLengthConstant = pBLengthConstant;
}

inline cSignatureParams* cSignatureController::GetSignatureParams(uint pInvLevel)
{
	return mSignatureParams[pInvLevel];
}


inline uint cSignatureController::GetDimension() const
{
	return mDimension;
}



inline uint cSignatureController::GetChunkCapacity_DDO(uint pInvLevel) const
{
	assert(pInvLevel < mLevelsCount);
	return mChunkCapacity_DDO[pInvLevel];
}


inline uint cSignatureController::GetMaxChunksCount_DDO(uint pInvLevel) const
{
	assert(pInvLevel < mLevelsCount);
	return mMaxChunksCount_DDO[pInvLevel];
}

inline void cSignatureController::SetDomains(uint* pDomains)
{
	for (uint i = 0; i < mDimension; i++)
	{
		mDomains[i] = pDomains[i];
	}
}

inline uint* cSignatureController::GetDomains() const
{
	return mDomains;
}

inline uint cSignatureController::GetDomain(uint pDim) const
{
	assert(pDim < mDimension);
	return mDomains[pDim];
}

inline void cSignatureController::SetQueryTypesCount(uint pQueryTypesCount)
{
	mQueryTypesCount = pQueryTypesCount;
}

inline void cSignatureController::SetQueryTypes(bool* pQueryTypes, uint pOrder)
{
	mQueryTypes[pOrder] = pQueryTypes;
}

inline uint cSignatureController::GetQueryTypesCount() const
{
	return mQueryTypesCount;
}

inline void cSignatureController::SetCurrentQueryType(uint pQueryType)
{
	mCurrentQueryType = pQueryType;
}

inline uint cSignatureController::GetCurrentQueryType() const
{
	return mCurrentQueryType;
}

inline bool* cSignatureController::GetQueryType(uint pOrder) const
{
	assert(pOrder < mQueryTypesCount);
	return mQueryTypes[pOrder];
}

inline uint cSignatureController::GetBuildType() const
{
	return mBuildType;
}

inline void cSignatureController::SetBuildType(uint pBuildType)
{
	mBuildType = pBuildType;
}

inline uint cSignatureController::GetSignatureType() const
{
	return mSignatureType;
}

inline void cSignatureController::SetSignatureType(uint pSignatureType)
{
	mSignatureType = pSignatureType;
}


inline uint cSignatureController::GetSignatureQuality() const
{
	return mSignatureQuality;
}

inline void cSignatureController::SetSignatureQuality(uint pSignatureQuality)
{
	mSignatureQuality = pSignatureQuality;
}


inline void cSignatureController::SetRQBits(uint pRQBits)
{
	mRQBits = pRQBits;
}

inline uint cSignatureController::GetRQBits() const
{
	return mRQBits;
}

#endif