/**************************************************************************}
{                                                                          }
{    cCacheStatistics.cpp                             		      					 }
{                                                                          }
{                                                                          }
{                 Copyright (c) 2001, 2003	   			Michal Kratky          }
{                                                                          }
{    VERSION: 0.2														DATE 20/02/2003                }
{                                                                          }
{             following functionality:                                     }
{               statistic of cache                                         }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      xx/xx/xxxx                                                          }
{                                                                          }
{**************************************************************************/

#include "dstruct/paged/core/cCacheStatistics.h"

namespace dstruct {
  namespace paged {
	namespace core {

uint cCacheStatistics::BufferedWrite = 0;
uint cCacheStatistics::BufferedWriteCount = 0;

void cCacheStatistics::ResetCounters() 
{ 
	for (unsigned int i = 0 ; i < mDACModes ; i++)
	{
		mNodeDACRead[i].Reset(); 
		mLeafNodeDACRead[i].Reset(); 
		mNodeDACWrite[i].Reset(); 
		mLeafNodeDACWrite[i].Reset(); 
	}
}

/**
 * Reset cache timers and counters, sum timers and counters.
 */
void cCacheStatistics::Reset()
{
	ResetCounters();
	ResetCountersSum();
	// ResetTimers();
	// ResetTimersSum();
}

void cCacheStatistics::ResetCountersSum() 
{ 
	for (unsigned int i = 0 ; i < mDACModes ; i++)
	{
		mNodeDACRead[i].ResetSum(); 
		mLeafNodeDACRead[i].ResetSum(); 
		mNodeDACWrite[i].ResetSum(); 
		mLeafNodeDACWrite[i].ResetSum(); 
	}
}

/*
void cCacheStatistics::AddCounters() 
{ 
	for (unsigned int i = 0 ; i < mDACModes ; i++)
	{
		mNodeDACRead[i].AddCounter();
		mLeafNodeDACRead[i].AddCounter();
		mNodeDACWrite[i].AddCounter(); 
		mLeafNodeDACWrite[i].AddCounter(); 
	}
}

void cCacheStatistics::ResetTimers() 
{ 
	mTimer.Reset(); 
	mRealReadTimer.Reset();
	mRealWriteTimer.Reset();
	mNodeSerialTimer.Reset();
	mNodeDeserialTimer.Reset();
	mNodeCopyTimer.Reset();
}

void cCacheStatistics::ResetTimersSum() 
{ 
	mTimer.ResetSum();
	mRealReadTimer.ResetSum();
	mRealWriteTimer.ResetSum();
	mNodeSerialTimer.ResetSum();
	mNodeDeserialTimer.ResetSum();
	mNodeCopyTimer.ResetSum();
}

void cCacheStatistics::AddTimers() 
{ 
	mTimer.AddTime(); 
	mRealReadTimer.AddTime();
	mRealWriteTimer.AddTime();
	mNodeSerialTimer.AddTime();
	mNodeDeserialTimer.AddTime();
	mNodeCopyTimer.AddTime();
} */

void cCacheStatistics::Print() {
	printf("****************************** Cache statistics: ******************************\n");
	/*printf("Complete Cache Time: ");
	mTimer.Print("\n");
	printf("Real timers:  Write: ");
	mRealWriteTimer.Print(" \t ");
	printf("Read: ");
	mRealReadTimer.Print("\n");
	printf("Time: Serialization: ");
	mNodeSerialTimer.Print("\t");
	printf("Deserialization: ");
	mNodeDeserialTimer.Print("\n");
	printf("Node Copy Time: ");
	mNodeCopyTimer.Print("\n\n");*/

	const int mb = 1024*1024;

	printf("Block Size: %d\n", mBlockSize);

	int completeDAC = mNodeDACRead[DAC_Logical].GetValue() + mLeafNodeDACRead[DAC_Logical].GetValue() + mNodeDACWrite[DAC_Logical].GetValue() + mLeafNodeDACWrite[DAC_Logical].GetValue();
	printf("IO R/W:    Logical: %u/%.2fMB \t (#IN: %u, #LN: %u)\n", completeDAC, ((double)completeDAC * mBlockSize)/mb,
		(mNodeDACRead[DAC_Logical].GetValue() + mNodeDACWrite[DAC_Logical].GetValue()), (mLeafNodeDACRead[DAC_Logical].GetValue() + mLeafNodeDACWrite[DAC_Logical].GetValue()));

	int completeRealDAC = mNodeDACRead[DAC_Physical].GetValue() + mLeafNodeDACRead[DAC_Physical].GetValue() + mNodeDACWrite[DAC_Physical].GetValue() + mLeafNodeDACWrite[DAC_Physical].GetValue();
	printf("           Physical: %u/%.2fMB \t (#IN: %u, #LN: %u)\n", completeRealDAC, ((double)completeRealDAC * mBlockSize)/mb,
		(mNodeDACRead[DAC_Physical].GetValue() + mNodeDACWrite[DAC_Physical].GetValue()), (mLeafNodeDACRead[DAC_Physical].GetValue() + mLeafNodeDACWrite[DAC_Physical].GetValue()));

	int readDAC = mNodeDACRead[DAC_Logical].GetValue() + mLeafNodeDACRead[DAC_Logical].GetValue();
	printf("IO Read:   Logical: %u/%.2fMB \t (#IN: %u, #LN: %u)\n", readDAC, ((double)readDAC * mBlockSize)/mb,
		mNodeDACRead[DAC_Logical].GetValue(), mLeafNodeDACRead[DAC_Logical].GetValue());

	int readRealDAC = mNodeDACRead[DAC_Physical].GetValue() + mLeafNodeDACRead[DAC_Physical].GetValue();
	printf("           Physical: %u/%.2fMB \t (#IN: %u, #LN: %u)\n", readRealDAC, ((double)readRealDAC * mBlockSize)/mb,
		mNodeDACRead[DAC_Physical].GetValue(), mLeafNodeDACRead[DAC_Physical].GetValue());

	int writeDAC = mNodeDACWrite[DAC_Logical].GetValue() + mLeafNodeDACWrite[DAC_Logical].GetValue();
	printf("IO Write:  Logical: %u/%.2fMB \t (#IN: %u, #LN: %u)\n", writeDAC, ((double)writeDAC * mBlockSize)/mb,
		mNodeDACWrite[DAC_Logical].GetValue(), mLeafNodeDACWrite[DAC_Logical].GetValue());

	int writeRealDAC = mNodeDACWrite[DAC_Physical].GetValue() + mLeafNodeDACWrite[DAC_Physical].GetValue();
	printf("           Physical: %u/%.2fMB \t (#IN: %u, #LN: %u)\n", writeRealDAC, ((double)writeRealDAC * mBlockSize)/mb,
		mNodeDACWrite[DAC_Physical].GetValue(), mLeafNodeDACWrite[DAC_Physical].GetValue());
	printf("*******************************************************************************\n");
}

/*
void cCacheStatistics::PrintAverage() {
	printf("**************************** Average cache statistics: ****************************\n");
	printf("Chnage cCacheStatistics::PrintAverage()!\n");
	/*printf("DAC all (real): %g (%g)\n", ((mLeafNodeDACRead[DAC_Logical].GetAverage() + mLeafNodeDACWrite[DAC_Logical].GetAverage())), ((mLeafNodeDACRead[DAC_Physical].GetAverage() + mLeafNodeDACWrite[DAC_Physical].GetAverage())));
	printf("DACRead all (real): %g (%g)\n", mLeafNodeDACRead[DAC_Logical].GetAverage(), mLeafNodeDACRead[DAC_Physical].GetAverage());
	printf("DACWrite all (real): %g (%g)\n", mLeafNodeDACWrite[DAC_Logical].GetAverage(), mLeafNodeDACWrite[DAC_Physical].GetAverage());*/
/*}

void cCacheStatistics::PrintSum() {
	printf("**************************** Sum cache statistics: ****************************\n");
	printf("Complete Cache Time: ");
	mTimer.PrintSum("\n");
	printf("Real timers:  Write: ");
	mRealWriteTimer.PrintSum(" \t ");
	printf("Read: ");
	mRealReadTimer.PrintSum("\n");
	printf("Time: Serial: ");
	mNodeSerialTimer.PrintSum("\t");
	printf("Deserial: ");
	mNodeDeserialTimer.PrintSum("\n");
	printf("Node Copy Time: ");
	mNodeCopyTimer.PrintSum("\n\n");

	printf("IO R/W:    Logical: %u \t (nodes: %u + leaf nodes: %u)\n", (mNodeDACRead[DAC_Logical].GetSum() + mLeafNodeDACRead[DAC_Logical].GetSum() + mNodeDACWrite[DAC_Logical].GetSum() + mLeafNodeDACWrite[DAC_Logical].GetSum()),
		(mNodeDACRead[DAC_Logical].GetSum() + mNodeDACWrite[DAC_Logical].GetSum()), (mLeafNodeDACRead[DAC_Logical].GetSum() + mLeafNodeDACWrite[DAC_Logical].GetSum()));
	printf("           Physical: %u \t (nodes: %u + leaf nodes: %u)\n", (mNodeDACRead[DAC_Physical].GetSum() + mLeafNodeDACRead[DAC_Physical].GetSum() + mNodeDACWrite[DAC_Physical].GetSum() + mLeafNodeDACWrite[DAC_Physical].GetSum()),
		(mNodeDACRead[DAC_Physical].GetSum() + mNodeDACWrite[DAC_Physical].GetSum()), (mLeafNodeDACRead[DAC_Physical].GetSum() + mLeafNodeDACWrite[DAC_Physical].GetSum()));

	printf("IO Read:   Logical: %u \t (nodes: %u + leaf nodes: %u)\n", (mNodeDACRead[DAC_Logical].GetSum() + mLeafNodeDACRead[DAC_Logical].GetSum()),
		mNodeDACRead[DAC_Logical].GetSum(), mLeafNodeDACRead[DAC_Logical].GetSum());
	printf("           Physical: %u \t (nodes: %u + leaf nodes: %u)\n", (mNodeDACRead[DAC_Physical].GetSum() + mLeafNodeDACRead[DAC_Physical].GetSum()),
		mNodeDACRead[DAC_Physical].GetSum(), mLeafNodeDACRead[DAC_Physical].GetSum());

	printf("IO Write:  Logical: %u \t (nodes: %u + leaf nodes: %u)\n", (mNodeDACWrite[DAC_Logical].GetSum() + mLeafNodeDACWrite[DAC_Logical].GetSum()),
		mNodeDACWrite[DAC_Logical].GetSum(), mLeafNodeDACWrite[DAC_Logical].GetSum());
	printf("           Physical: %u \t (nodes: %u + leaf nodes: %u)\n", (mNodeDACWrite[DAC_Physical].GetSum() + mLeafNodeDACWrite[DAC_Physical].GetSum()),
		mNodeDACWrite[DAC_Physical].GetSum(), mLeafNodeDACWrite[DAC_Physical].GetSum());
	printf("*******************************************************************************\n");
}*/
}}}