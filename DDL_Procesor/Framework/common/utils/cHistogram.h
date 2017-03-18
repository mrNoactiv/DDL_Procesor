/**************************************************************************}
{                                                                          }
{    cHistogram.h                                 		      	 	   }
{                                                                          }
{                                                                          }
{                 Copyright (c) 2010	   				Peter Chovanec     }
{                                                                          }
{    VERSION: 1.0										DATE 09/05/2011    }
{                                                                          }
{             following functionality:                                     }
{               histogram of values in interval                            }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{    xx.xx.xxxx                                                            }
{                                                                          }
{                                                                          }
{**************************************************************************/

#ifndef __cHistogram_h__
#define __cHistogram_h__

#include <assert.h>
#include <stdio.h>
#include <common/cCommon.h>
#include <common/cBitArray.h>


namespace common {
	namespace utils {


class cHistogram
{
public:
	static const uint HISTOGRAM = 0;			// count number of occurences for individual values
	static const uint BIT_HISTOGRAM_SPARSE = 1;	// count number of values
	static const uint BIT_HISTOGRAM_DENSE = 2; 	// count number of values

private: 
	static const uint NOT_SPECIFIED = 4294967295;

	uint mHistogramType;

	// for histogram purpose
	uint* mHistogram;
	
	// for bit-histogram purpose
	char* mBitHistogram;
	uint* mValues;
	uint mUniqueValues;

	// interval
	uint mMinValue;
	uint mMaxValue;
public:
	cHistogram(uint paMaxValue, uint paCapacity = cHistogram::NOT_SPECIFIED, uint paHistogramType = cHistogram::HISTOGRAM);
	cHistogram(uint paMinValue, uint paMaxValue, uint paCapacity = cHistogram::NOT_SPECIFIED, uint paHistogramType = cHistogram::HISTOGRAM);
	~cHistogram();
	
	
	inline void ClearHistogram();
	void Print2File(char* paFileName, char* paHeader = "");
	
	inline uint GetCount(uint value) const;
	inline uint GetUniqueValuesCount();

	inline void AddValue(uint value, uint count = 1);
};


inline void cHistogram::ClearHistogram()
{
	uint range = mMaxValue - mMinValue;
	switch (mHistogramType)
	{
		case cHistogram::HISTOGRAM:
			memset(mHistogram, 0, range * sizeof(uint));
			break;
		case cHistogram::BIT_HISTOGRAM_DENSE:
			memset(mBitHistogram, 0, (range / cNumber::BYTE_LENGTH) + 1);
			mUniqueValues = 0;
			break;
		case cHistogram::BIT_HISTOGRAM_SPARSE:
			for (uint i = 0; i < mUniqueValues; i++)
			{
				cBitString::SetBit((char*) mBitHistogram, mValues[i], false);
			}
			mUniqueValues = 0;
			break;
	}
}

// returns number of unique values
inline uint cHistogram::GetUniqueValuesCount()
{
	uint count = 0;
	uint range = mMaxValue - mMinValue + 1;
	switch (mHistogramType)
	{
		case cHistogram::HISTOGRAM:
			for (uint i = 0; i < range; i++)
			{
				count += (mHistogram[i] > 0) ? 1 : 0;
			}
			break;
		case cHistogram::BIT_HISTOGRAM_DENSE:
			count = mUniqueValues;
			break;
		case cHistogram::BIT_HISTOGRAM_SPARSE:
			count = mUniqueValues;
			break;
	}

	return count;
}

// returns number of occurrences of specified value
inline uint cHistogram::GetCount(uint value) const
{
	assert(value < mMaxValue);

	uint count = 0;
	uint realOrder = value - mMinValue;
	switch (mHistogramType)
	{
	case cHistogram::HISTOGRAM:
		count = mHistogram[realOrder];
		break;
	case cHistogram::BIT_HISTOGRAM_DENSE:
		count = (cBitString::GetBit(mBitHistogram, realOrder)) ? 1 : 0;
		break;
	case cHistogram::BIT_HISTOGRAM_SPARSE:
		count = (cBitString::GetBit(mBitHistogram, realOrder)) ? 1 : 0;
		break;
	}

	return count;
}
	
// adds value into histogram
inline void cHistogram::AddValue(uint value, uint count)
{
	assert(value < mMaxValue);

	uint realOrder = value - mMinValue;
	switch (mHistogramType)
	{
		case cHistogram::HISTOGRAM:
			if (realOrder < mMaxValue - 1)
			{
				mHistogram[realOrder] += count;
			}
			else
			{
				mHistogram[mMaxValue - 1] += count;
			}
			break;
		case cHistogram::BIT_HISTOGRAM_DENSE:
			if (!cBitString::GetBit(mBitHistogram, realOrder))
			{
				cBitString::SetBit(mBitHistogram, realOrder, true);
				mUniqueValues++;
			}
			break;
		case cHistogram::BIT_HISTOGRAM_SPARSE:
			if (!cBitString::GetBit(mBitHistogram, realOrder))
			{
				cBitString::SetBit(mBitHistogram, realOrder, true);
				mValues[mUniqueValues++] = realOrder;
			}
			break;
	}
}

}}
#endif