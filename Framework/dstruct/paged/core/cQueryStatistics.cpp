/**************************************************************************}
{                                                                          }
{    cQueryStatistics.cpp                             		      					 }
{                                                                          }
{                                                                          }
{                 Copyright (c) 2001, 2003	   			Michal Kratky          }
{                                                                          }
{    VERSION: 0.2														DATE 20/02/2003                }
{                                                                          }
{             following functionality:                                     }
{               statistics of tree operations                              }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      xx/xx/xxxx                                                          }
{                                                                          }
{**************************************************************************/

#include "cQueryStatistics.h"

namespace dstruct {
  namespace paged {
	namespace core {

cQueryStatistics::cQueryStatistics(): mTimerCount(0), mCounters(NULL), mTimers(NULL)
{
	mTimer1 = new cTimer();
	mTimer2 = new cTimer();
	mTimer3 = new cTimer();
}

cQueryStatistics::~cQueryStatistics(void)
{
	if (mTimers != NULL)
	{
		delete []mTimers;
	}
	if (mCounters != NULL)
	{
		delete []mCounters;
	}
	delete mTimer1;
	delete mTimer2;
	delete mTimer3;
}

/**
 * Resize according to tree header.
 */
void cQueryStatistics::Resize()
{
	mGroupCounterCount = 10;
	mSeparatelyCounterCount = 10;
	mCounterCount = mGroupCounterCount + mSeparatelyCounterCount;
	mTimerCount = 10;

	mCounters = new cCounter[mCounterCount];
	mTimers = new cTimer[mTimerCount];
}

/**
 * Reset counters and timers and sum counters and timers.
 */
void cQueryStatistics::Reset()
{
	ResetCounters();
	ResetCountersSum();
	ResetTimers();
	ResetTimersSum();
}

void cQueryStatistics::ResetCounters() 
{ 
	for (unsigned int i = 0 ; i < mGroupCounterCount ; i++)
	{
		mCounters[i].Reset();
	}
}

void cQueryStatistics::ResetCountersSum() 
{ 
	for (unsigned int i = 0 ; i < mGroupCounterCount ; i++)
	{
		mCounters[i].ResetSum(); 
	}
}

void cQueryStatistics::AddCounters()
{ 
	for (unsigned int i = 0 ; i < mGroupCounterCount ; i++)
	{
		mCounters[i].AddCounter(); 
	}
}

void cQueryStatistics::ResetTimers() 
{ 
	for (unsigned int i = 0 ; i < mTimerCount ; i++)
	{
		mTimers[i].Reset();
	}
}

void cQueryStatistics::ResetTimersSum() 
{ 
	for (unsigned int i = 0 ; i < mTimerCount ; i++)
	{
		mTimers[i].ResetSum();
	}
}

void cQueryStatistics::AddTimers()
{ 
	for (unsigned int i = 0 ; i < mTimerCount ; i++)
	{
		mTimers[i].AddTime();
	}
}
}}}