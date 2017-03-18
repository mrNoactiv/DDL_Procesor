/**************************************************************************}
{                                                                          }
{    cCacheStatistics.h                               		      					 }
{                                                                          }
{                                                                          }
{    Copyright (c) 2001, 2003	   			       Michal Kratky                 }
{                                                                          }
{    VERSION: 0.2														 DATE 20/02/2003               }
{                                                                          }
{    following functionality:                                              }
{       statistics of cache                                                }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      xx/xx/xxxx                                                          }
{                                                                          }
{**************************************************************************/

#ifndef __cCacheStatistics_h__
#define __cCacheStatistics_h__

#include "common/utils/cCounter.h"
#include "common/utils/cTimer.h"
#include "dstruct/paged/core/cTreeHeader.h"

// mkxx using namespace common::utils;

namespace dstruct {
  namespace paged {
	namespace core {

class cCacheStatistics
{
private:
	// Disk Access Count - DAC
	static const unsigned int mDACModes = 2;
	int mBlockSize;              // tree page size 
	cCounter mNodeDACRead[mDACModes], mNodeDACWrite[mDACModes];
	cCounter mLeafNodeDACRead[mDACModes], mLeafNodeDACWrite[mDACModes];

	/*
	cTimer mTimer;              // whole time of caching
	cTimer mRealReadTimer;      // time of reading the node in the secondary storage
	cTimer mRealWriteTimer;     // time of writing the node on the secondary storage
	cTimer mNodeSerialTimer;    // time of node serialization
	cTimer mNodeDeserialTimer;  // time of node deserialization
	cTimer mNodeCopyTimer;      // time of node copying
	*/

	void ResetCounters();
	void ResetCountersSum();

public:
	// DAC mode
	static const unsigned int DAC_Physical = 0;   // real DAC (only access at disk)
	static const unsigned int DAC_Logical = 1;    // all access into cache (even access into memory)

	static uint BufferedWrite;
	static uint BufferedWriteCount;

	void Reset();
	// void ResetCounters();
	// void ResetCountersSum();
	// void ResetTimers();
	// void ResetTimersSum();

	// void AddCounters();
	// void AddTimers();

	inline unsigned int GetDACWrite(unsigned int mode) const;
	inline unsigned int GetDACRead(unsigned int mode) const;

	inline cCounter& GetNodeDACWrite(unsigned int mode);
	inline cCounter& GetNodeDACRead(unsigned int mode);
	inline cCounter& GetLeafNodeDACWrite(unsigned int mode);
	inline cCounter& GetLeafNodeDACRead(unsigned int mode);

	inline void IncrementDAC(bool readFlag, unsigned int nodeType, unsigned int accessType);

	// since it is rather complicated in multi-thread environment, I had to remove it.
	/*
	inline cTimer* GetTimer();
	inline cTimer* GetRealReadTimer();
	inline cTimer* GetRealWriteTimer();
	inline cTimer* GetNodeSerialTimer();
	inline cTimer* GetNodeDeserialTimer();
	inline cTimer* GetNodeCopyTimer();
	*/

	void Print();
	// void PrintAverage();
	// void PrintSum();

	inline void SetBlockSize(int pageSize);
};

inline unsigned int cCacheStatistics::GetDACRead(const unsigned int mode) const
{ 
	return mNodeDACRead[mode].GetValue() + mLeafNodeDACRead[mode].GetValue(); 
}
inline unsigned int cCacheStatistics::GetDACWrite(const unsigned int mode) const
{ 
	return mNodeDACWrite[mode].GetValue() + mLeafNodeDACWrite[mode].GetValue(); 
}

inline cCounter& cCacheStatistics::GetNodeDACRead(const unsigned int mode) 
{ 
	return mNodeDACRead[mode]; 
}

inline cCounter& cCacheStatistics::GetNodeDACWrite(const unsigned int mode) 
{ 
	return mNodeDACWrite[mode]; 
}

inline cCounter& cCacheStatistics::GetLeafNodeDACRead(const unsigned int mode) 
{ 
	return mLeafNodeDACRead[mode];
}

inline cCounter& cCacheStatistics::GetLeafNodeDACWrite(const unsigned int mode) 
{ 
	return mLeafNodeDACWrite[mode]; 
}

inline void cCacheStatistics::IncrementDAC(bool readFlag, unsigned int nodeHeaderId, unsigned int accessType)
{
	if (readFlag)
	{
		if (nodeHeaderId == cTreeHeader::HEADER_LEAFNODE)  // I don't know if nodeType corresponds to these constants
		{
			GetLeafNodeDACRead(accessType).Increment();
		}
		else // if (nodeType == cTreeHeader::HEADER_NODE)
		{
			GetNodeDACRead(accessType).Increment();
		}
	}
	else
	{
		if (nodeHeaderId == cTreeHeader::HEADER_LEAFNODE)
		{
			GetLeafNodeDACWrite(accessType).Increment();
		}
		else // if (nodeType == cTreeHeader::HEADER_NODE)
		{
			GetNodeDACWrite(accessType).Increment();
		}
	}

	/*
	if (accessType == DAC_Physical)
	{
		IncrementDAC(readFlag, nodeType, DAC_Logical); // the physical access means the logical as well physical access
	}*/
}

/*
inline cTimer* cCacheStatistics::GetTimer() 
{ 
	return &mTimer; 
}
inline cTimer* cCacheStatistics::GetRealReadTimer() 
{ 
	return &mRealReadTimer; 
}
inline cTimer* cCacheStatistics::GetRealWriteTimer() 
{ 
	return &mRealWriteTimer; 
}
inline cTimer* cCacheStatistics::GetNodeSerialTimer() 
{ 
	return &mNodeSerialTimer; 
}
inline cTimer* cCacheStatistics::GetNodeDeserialTimer() 
{ 
	return &mNodeDeserialTimer; 
}
inline cTimer* cCacheStatistics::GetNodeCopyTimer() 
{ 
	return &mNodeCopyTimer; 
}
*/

inline void cCacheStatistics::SetBlockSize(int blockSize) 
{ 
	mBlockSize = blockSize; 
}
}}}
#endif