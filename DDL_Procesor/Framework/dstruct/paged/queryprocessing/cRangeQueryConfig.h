/**
*	\file cRangeQueryConfig.h
*	\author Peter Chovanec
*	\version 0.1
*	\date nov 2012
*	\brief Configurator of Range Query Processing
*/

#ifndef __cRangeQueryConfig_h__
#define __cRangeQueryConfig_h__

#ifdef CUDA_ENABLED
#include "dstruct/paged/cuda/cGpuConst.h"
#endif

namespace dstruct {
  namespace paged {

class QueryType
{
  public: 
	typedef enum QueryTypes { SINGLEQUERY = 0, BATCHQUERY = 1, CARTESIANQUERY = 2};
};

class cRangeQueryConfig
{
private:
	static const unsigned int UINT_SIZE = 8;

	unsigned int mFinalResultSize;
	bool mSignatureEnabled;

	unsigned int mQueryProcessingType;
	bool mBulkReadEnabled;
	unsigned int mLeafIndicesCapacity; // how many leaf indices can be stored in the indices array
	unsigned int mMaxReadNodes;        // how many leafs can be read by bufferedRead at once

	unsigned int mMaxIndexDiff_BulkRead;       // it defines the maximum difference between node indices during buffered read
	unsigned int mNodeIndexCapacity_BulkRead;  // how many leaf indices can be stored in the indices array
	unsigned int mSearchMethod;  // it defines which search method is used to search nodes.
	unsigned int mSearchStruct;
	unsigned int mDevice;
public:
	static const unsigned int FINAL_RESULTSIZE_UNDEFINED = 0;
	//search methods
	static const int DEVICE_CPU = 0;
	static const int DEVICE_GPU = 1;
	static const int DEVICE_PHI = 2;
	static const int SEARCH_DFS = 0;
	static const int SEARCH_DBFS = 1;
	static const int SEARCH_BFS = 2;
	static const int SEARCH_STRUCT_ARRAY = 0;
	static const int SEARCH_STRUCT_HASHTABLE = 1;


public:
	cRangeQueryConfig();

	inline unsigned int GetFinalResultSize() const;
	inline bool IsSignatureEnabled() const;
	inline bool IsBulkReadEnabled() const;
	inline unsigned int GetQueryProcessingType() const;

	inline unsigned int GetLeafIndicesCapacity() const;
	inline unsigned int GetMaxReadNodes() const;

	inline unsigned int GetMaxIndexDiff_BulkRead() const;
	inline unsigned int GetNodeIndexCapacity_BulkRead() const;
	inline unsigned int GetSearchMethod() const;
	inline unsigned int GetSearchStruct() const;
	inline unsigned int GetDevice() const;

	inline void SetFinalResultSize(unsigned int finalResultSize);
	inline void SetSignatureEnabled(bool signatureEnabled);
	inline void SetBulkReadEnabled(bool bufferedReadEnabled);
	inline void SetQueryProcessingType(unsigned int queryProcessingType);

	inline void SetLeafIndicesCapacity(unsigned int leafIndicesCapacity);
	inline void SetMaxReadNodes(unsigned int maxReadNodes);

	inline void SetMaxIndexDiff_BulkRead(unsigned int maxIndexDiff);
	inline void SetNodeIndexCapacity_BulkRead(unsigned int nodeIndexCapacity);
	inline void SetSearchMethod(unsigned int method);
	inline void SetSearchStruct(unsigned int method);
	inline void SetDevice(unsigned int device);

#ifdef CUDA_ENABLED
private:
	unsigned int mGpuAlgorithm;
	float mCudaCapability;
	unsigned int mThreadsPerBlock;
public:
	inline unsigned int GetGpuAlgorithm() const;
	inline float GetGpuCapability() const;
	inline unsigned int GetGpuThreads() const;
	inline bool IsBucketOrderNodeIndex() const;
	inline void SetGpuAlgorithm(unsigned int value);
	inline void SetGpuCapability(float value);
	inline void SetGpuThreads(unsigned int value);
#endif
};

inline unsigned int cRangeQueryConfig::GetFinalResultSize() const
{
	return mFinalResultSize;
}

inline bool cRangeQueryConfig::IsSignatureEnabled() const
{
	return mSignatureEnabled;
}

inline bool cRangeQueryConfig::IsBulkReadEnabled() const
{
	return mBulkReadEnabled;
}

inline unsigned int cRangeQueryConfig::GetQueryProcessingType() const
{
	return mQueryProcessingType;
}

inline unsigned int cRangeQueryConfig::GetLeafIndicesCapacity() const
{
	return mLeafIndicesCapacity;
}

inline unsigned int cRangeQueryConfig::GetMaxReadNodes() const
{
	return mMaxReadNodes;
}

inline unsigned int cRangeQueryConfig::GetMaxIndexDiff_BulkRead() const
{
	return mMaxIndexDiff_BulkRead;
}

inline unsigned int cRangeQueryConfig::GetNodeIndexCapacity_BulkRead() const
{
	return mNodeIndexCapacity_BulkRead;
}
inline unsigned int cRangeQueryConfig::GetSearchMethod() const
{
	return mSearchMethod;
}
inline unsigned int cRangeQueryConfig::GetSearchStruct() const
{
	return mSearchStruct;
}
inline unsigned int cRangeQueryConfig::GetDevice() const
{
	return mDevice;
}
inline void cRangeQueryConfig::SetFinalResultSize(unsigned int finalResultSize)
{
	mFinalResultSize = finalResultSize;
}

inline void cRangeQueryConfig::SetSignatureEnabled(bool signatureEnabled)
{
	mSignatureEnabled = signatureEnabled;
}

inline void cRangeQueryConfig::SetBulkReadEnabled(bool bulkReadEnabled)
{
	mBulkReadEnabled = bulkReadEnabled;
}

inline void cRangeQueryConfig::SetQueryProcessingType(unsigned int queryProcessingType)
{
	mQueryProcessingType = queryProcessingType;
}

inline void cRangeQueryConfig::SetLeafIndicesCapacity(unsigned int leafIndicesCapacity)
{
	mLeafIndicesCapacity = leafIndicesCapacity;
}

inline void cRangeQueryConfig::SetMaxReadNodes(unsigned int maxReadNodes)
{
	mMaxReadNodes = maxReadNodes;
}

inline void cRangeQueryConfig::SetMaxIndexDiff_BulkRead(unsigned int maxIndexDiff)
{
	mMaxIndexDiff_BulkRead = maxIndexDiff;
}

inline void cRangeQueryConfig::SetNodeIndexCapacity_BulkRead(unsigned int nodeIndexCapacity)
{
	mNodeIndexCapacity_BulkRead = nodeIndexCapacity;
}

inline void cRangeQueryConfig::SetSearchMethod(unsigned int method)
{
	if (method != cRangeQueryConfig::SEARCH_DFS && !mBulkReadEnabled)
	{
		SetBulkReadEnabled(true);
		//printf("\nInfo: Bulk read was enabled.");
	}
	mSearchMethod = method;
}
inline void cRangeQueryConfig::SetSearchStruct(unsigned int type)
{
	mSearchStruct = type;
}
inline void cRangeQueryConfig::SetDevice(unsigned int device)
{
	mDevice = device;
}
#include "dstruct/paged/queryprocessing/cRangeQueryConfig_Gpu.h"
#include "dstruct/paged/queryprocessing/cRangeQueryConfig_Phi.h"
}}
#endif