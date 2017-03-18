/**
*	\file cQueryStatistics.h
*	\author Michal Krátký
*	\version 0.1
*	\date feb 2003
*	\version 0.2
*	\date jul 2011
*	\brief Statistics of tree operations
*/

#ifndef __cQueryStatistics_h__
#define __cQueryStatistics_h__

#include "common/utils/cTimer.h"
#include "common/utils/cCounter.h"

using namespace common::utils;

namespace dstruct {
  namespace paged {
	namespace core {

/**
*	Statistics of tree operations
*
*	\author Michal Krátký
*	\version 0.2
*	\date jul 2011
**/
class cQueryStatistics
{
private:
	cCounter *mCounters;
	unsigned int mCounterCount;
	unsigned int mGroupCounterCount;
	unsigned int mSeparatelyCounterCount;
	common::utils::cTimer *mTimers;
	unsigned int mTimerCount;

public:
	cQueryStatistics();
	~cQueryStatistics(void);

	common::utils::cTimer *mTimer1;
	common::utils::cTimer *mTimer2;
	common::utils::cTimer *mTimer3;

	//void Resize(const cTreeHeader *header);
	void Resize();
	inline cCounter* GetCounter(unsigned int index);
	inline common::utils::cTimer* GetTimer(unsigned int index);
	inline unsigned int GetCounterCount() const;
	inline unsigned int GetTimerCount() const;

	void Reset();
	void ResetCounters();
	void ResetCountersSum();
	void ResetTimers();
	void ResetTimersSum();

	void AddCounters();
	void AddTimers();
};

inline cCounter* cQueryStatistics::GetCounter(unsigned int index)
{ 
	if (index >= mCounterCount)
	{
		index = mCounterCount-1;
	}
	return (mCounters + index); 
}

inline common::utils::cTimer* cQueryStatistics::GetTimer(unsigned int index)
{
	if (index >= mTimerCount)
	{
		index = mTimerCount-1;
	}
	return (mTimers + index);
}

inline unsigned int cQueryStatistics::GetCounterCount() const
{
	return mGroupCounterCount;
}

inline unsigned int cQueryStatistics::GetTimerCount() const
{
	return mTimerCount;
}
}}}
#endif