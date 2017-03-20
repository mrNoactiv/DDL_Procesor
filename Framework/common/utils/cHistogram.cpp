#include "common/utils/cHistogram.h"

namespace common {
	namespace utils {

cHistogram::cHistogram(uint paMaxValue, uint paCapacity, uint paHistogramType)
{
	mHistogramType = paHistogramType;
	
	mMinValue = 0;
	mMaxValue = paMaxValue + 1;

	uint range = mMaxValue - mMinValue;
	switch (mHistogramType)
	{
		case cHistogram::HISTOGRAM:
			mHistogram = new uint[range];
			memset(mHistogram, 0, range * sizeof(uint));
			break;
		case cHistogram::BIT_HISTOGRAM_DENSE:
			mBitHistogram = new char[(range / cNumber::BYTE_LENGTH) + 1];
			memset(mBitHistogram, 0, (range / cNumber::BYTE_LENGTH) + 1);
			mUniqueValues = 0;
			break;
		case cHistogram::BIT_HISTOGRAM_SPARSE:
			mBitHistogram = new char[(range / cNumber::BYTE_LENGTH) + 1];
			memset(mBitHistogram, 0, (range / cNumber::BYTE_LENGTH) + 1);

			if (paCapacity == cHistogram::NOT_SPECIFIED)
			{
				printf("cHistogram:: ERROR - Capacity of bit-histogram is not specified !!!");
			}

			mValues = new uint[paCapacity];
			mUniqueValues = 0;
			break;
	}

    ClearHistogram();
}


cHistogram::cHistogram(uint paMinValue, uint paMaxValue, uint paCapacity, uint paHistogramType)
{
	mHistogramType = paHistogramType;

	mMinValue = paMinValue;
	mMaxValue = paMaxValue + 1;

	uint range = mMaxValue - mMinValue;
	switch (mHistogramType)
	{
		case cHistogram::HISTOGRAM:
			mHistogram = new uint[range];
			break;
		case cHistogram::BIT_HISTOGRAM_DENSE:
			mBitHistogram = new char[(range / cNumber::BYTE_LENGTH) + 1];
			memset(mBitHistogram, 0, (range / cNumber::BYTE_LENGTH) + 1);
			mUniqueValues = 0;
			break;
		case cHistogram::BIT_HISTOGRAM_SPARSE:
			mBitHistogram = new char[(range / cNumber::BYTE_LENGTH) + 1];
			memset(mBitHistogram, 0, (range / cNumber::BYTE_LENGTH) + 1);

			if (paCapacity == cHistogram::NOT_SPECIFIED)
			{
				printf("cHistogram:: ERROR - Capacity of bit-histogram is not specified !!!");
			}

			mValues = new uint[paCapacity];
			mUniqueValues = 0;
			break;
	}

	ClearHistogram();
}

cHistogram::~cHistogram()
{
	switch (mHistogramType)
	{
		case cHistogram::HISTOGRAM:
			delete mHistogram;
			mHistogram = NULL;
			break;
		case cHistogram::BIT_HISTOGRAM_DENSE:
			delete mBitHistogram;
			mBitHistogram = NULL;
			break;
		case cHistogram::BIT_HISTOGRAM_SPARSE:
			delete mBitHistogram;
			mBitHistogram = NULL;
			delete mValues;
			mValues = NULL;
			break;
	}
}



void cHistogram::Print2File(char* paFileName, char* paHeader)
{
	FILE *StreamInfo = fopen(paFileName, "a");

	fprintf(StreamInfo, paHeader);
	fprintf(StreamInfo, "\n\n");
	
	uint range = mMaxValue - mMinValue + 1;
	switch (mHistogramType)
	{
		case cHistogram::HISTOGRAM:
			fprintf(StreamInfo, "Number -> Count\n");
			for (uint i = 0; i < range - 1; i++)
			{
				if (mHistogram[i] > 0)
					fprintf(StreamInfo, "%d -> %d\n", i, mHistogram[i]);
			}
			fprintf(StreamInfo, "> %d -> %d\n", mMaxValue, mHistogram[range - 1]);
			break;

		case cHistogram::BIT_HISTOGRAM_DENSE:
			fprintf(StreamInfo, "Number -> Exists\n");
			for (uint i = 0; i < range; i++)
			{
				if (cBitString::GetBit(mBitHistogram, i))
				{
					fprintf(StreamInfo, "%d -> true\n", i);
				}
			}
			break;
		case cHistogram::BIT_HISTOGRAM_SPARSE:
			fprintf(StreamInfo, "Number -> Exists\n");
			for (uint i = 0; i < range; i++)
			{
				if (cBitString::GetBit(mBitHistogram, i))
				{
					fprintf(StreamInfo, "%d -> true\n", i);
				}
			}
			break;
	}

	fclose(StreamInfo);
}

}}