/*!
* \file cTuplesGenerator.h
*
* \author Pavel Bednar
* \date feb 2015
*
* \brief Class for loading tuples. Supports random tuples or reading source file.
* \comment For now the random mode support only fixed number of tuples specified in ctor. The file mode does not support loading tuple at specified position yet.
*/
#ifndef __cTuplesGenerator_h__
#define __cTuplesGenerator_h__

#include "common/stream/cFileStream.h"
#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "common/cCommon.h"
#include "common/utils/cSSEUtils.h"
#include "common/stream/cFileStream.h"

namespace common {
	namespace data {

using namespace common::datatype::tuple;
#ifdef SSE_ENABLED
static const __m128i mSseEndLineChar = _mm_loadu_si128((__m128i*) "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
static const __m128i mSseSplitChar = _mm_loadu_si128((__m128i*) ",,,,,,,,,,,,,,,,");
#endif
template<class TDataType, class TTuple>
class cTuplesGenerator
{
private:
	enum Modes { FromFile, Random };
	uint mDimension;
	TTuple** mTupleArray;
	char** mRandData; //pre-filled array of random data for tuples with variable length
	uint mArrayOrder; //progress in tuple generation.
	uint mRandArrayMaxCount; //sets maximum size of mRandArray
	uint mTuplesCount; //number of tuples to generate or read from file
	uint mCount; //internal count of tuples (can be different as mTuplesCount because query files contains lo and hi tuple as one item.)
	uint mDomain;
	Modes mMode;
	cSpaceDescriptor* mSD;
	//SSE
	uint* mSplitIndices;
	uint mSplitsCount;
	//buffer
	static const uint BUFF_LEN = 262144;
	static const uint MAX_LINE_LENGTH = 512;
	cFileStream* mStream;
	int mBufferOffset;
	char* mFileName;
	char* mBuffer;
	llong mFileOffset;
	int mByteRead;
	ushort mLineLength;
	int mLineStartBufferOffset;     // offset of the line in the buffer
	char *mTmpFileLine; //for old implementation
	bool mIsRangeQueriesFile; //if source file contatins range queries, there are two tuples for each query.
	bool mIsNTuple; //true if TDataType is equal to cNTuple
	bool mIsVarLen; // variable length of tuples
private:
	TTuple* GetNextRandTuple();
	TTuple* GetNextTupleFromFile(); //should be private, just for debug
	
	//VAL644
	void GetNextRandTuple(TTuple& emptyTuple);
	void GetNextTupleFromFile(TTuple& emptyTuple); //should be private, just for debug
	//VAL644

	void GenerateRandomArray();
	bool ReadBuffer(char* buffer, uint count);
	bool ReadLine(unsigned int *separatorIndices, unsigned int &separatorCount);
	bool ReadHeader(uint &mDimension, uint &mTuplesCount);
	bool ReadHeaderBLWith(uint &mDimension); // bulkloading with txt header
	bool ReadHeaderBLWithout(uint &mDimension, uint &mTuplesCount); // bulkloading without txt header
	bool OpenFile();
	void ResetFileStream();
	void GenerateRandomData();
	inline int SearchText(const char mainChar, const char secondaryChar, const char *inputArray, const unsigned int arraySize, unsigned int* foundIndices, unsigned int &foundCount);

	void ResetFileStreamBL(); // Bulkloading
	void ResetFileStreamBL(uint Dimension); //Bulkloading
public:
	cTuplesGenerator(char* file, bool isRangeQueriesFile, bool computeOrderValues = false); //constructor for file mode
	cTuplesGenerator(char* file, bool isRangeQueriesFile, bool computeOrderValues, uint tuplesCount); // constructor for file mode with defined dimension and tuplesCount in the begg. of file
	cTuplesGenerator(uint tuplesCount, uint dim, uint domain, bool variableLength = false, bool computeOrderValues = false); // computeOrderValues is utilized for bulkload
	~cTuplesGenerator();
	inline bool HasNextTuple();
	inline TTuple* GetTuple(uint order);
	inline void GetTuple(TTuple& emptyTuple, uint order); // VAL644
	inline char* GetTupleData(uint order);
	inline TTuple* GetNextTuple();
	inline void GetNextTuple(TTuple& emptyTuple); // VAL644
	inline void GetNextTuples(TTuple* output, uint count, uint &returned);
	inline void GetNextTuples(TTuple* qls, TTuple* qhs, uint count, uint &returned);
	inline uint GetTuplesCount();
	inline cSpaceDescriptor* GetSpaceDescriptor();
	inline TTuple GetRandTuple(uint order);
	inline void ResetPosition();
	inline void ResetPosition(uint Dimension); // added 12.6.2015
};

template<class TDataType, class TTuple>
cTuplesGenerator<TDataType, TTuple>::cTuplesGenerator(char* file, bool isRangeQueriesFile, bool computeOrderValues)
{
	mMode = Modes::FromFile;
	mIsRangeQueriesFile = isRangeQueriesFile;
	mArrayOrder = 0;
	mFileName = file;
	mTmpFileLine = new char[2048];
	mBuffer = new char[BUFF_LEN];
	mStream = new cFileStream();
	mTuplesCount = 0;
	ResetFileStream();
	mSD = new cSpaceDescriptor(mDimension, new TTuple(), new TDataType(), computeOrderValues);


	if (isRangeQueriesFile)
	{
		mCount = mTuplesCount * 2;
	}
	else
	{
		mCount = mTuplesCount;
	}

	mTupleArray = new TTuple*[mCount];

	for (uint i = 0; i < mCount; i++)
	{
		mTupleArray[i] = new TTuple(mSD);
	}
}

/// bulkload from standart export with diff tuplesCount
template<class TDataType, class TTuple>
cTuplesGenerator<TDataType, TTuple>::cTuplesGenerator(char* file, bool isRangeQueriesFile, bool computeOrderValues, uint tuplesCount)
{
	mIsVarLen = false; //DOCASNE RESENI. v cBulkLoading se pravdepodobně nevola destruktor a mIsVarLen je v tomto ctoru TRUE -> způsobuje chybu v debug modu, když se v destruktoru pokusí dealokovat mRandArray, ktere nebylo inicializovano
	mMode = Modes::FromFile;
	mIsRangeQueriesFile = isRangeQueriesFile;
	mArrayOrder = 0;
	mFileName = file;
	mTmpFileLine = new char[2048];
	mBuffer = new char[BUFF_LEN];
	mStream = new cFileStream();
	mTuplesCount = tuplesCount;
	ResetFileStreamBL();
	mSD = new cSpaceDescriptor(mDimension, new TTuple(), new TDataType(), computeOrderValues);

	if (isRangeQueriesFile)
	{
		mCount = mTuplesCount * 2;
	}
	else
	{
		mCount = mTuplesCount;
	}

	mTupleArray = new TTuple*[mCount];

	for (uint i = 0; i < mCount; i++)
	{
		mTupleArray[i] = new TTuple(mSD);
	}
}

template<class TDataType, class TTuple>
cTuplesGenerator<TDataType, TTuple>::cTuplesGenerator(uint tuplesCount, uint dim, uint domain, bool variableLength, bool computeOrderValues)
{
	mMode = Modes::Random;
	mTuplesCount = tuplesCount;
	mRandArrayMaxCount = tuplesCount;
	mSD = new cSpaceDescriptor(dim, new TTuple(), new TDataType(), computeOrderValues);
	mArrayOrder = 0;
	mDimension = dim;
	mDomain = domain;
	mIsNTuple = false;
	mIsVarLen = variableLength;
	if (typeid(TTuple).name() == typeid(cNTuple).name())
	{
		mIsNTuple = true;
		mRandData = new char*[tuplesCount];
	}
	GenerateRandomArray();
}

template<class TDataType, class TTuple>
cTuplesGenerator<TDataType, TTuple>::~cTuplesGenerator()
{
	if (mMode == Modes::FromFile)
	{
		if (mStream != NULL)
		{
			//mStream->Close();
			delete mStream;
			mStream = NULL;
		}

		if (mBuffer != NULL)
		{
			delete mBuffer;
			mBuffer = NULL;
		}
		if (mSplitIndices != NULL)
		{
			delete[] mSplitIndices;
			mSplitIndices = NULL;
		}
		if (mTmpFileLine != NULL)
		{
			delete mTmpFileLine;
			mTmpFileLine = NULL;
		}
	}
	if (mTupleArray != NULL)
	{
		for (uint i = 0; i < mTuplesCount; i++)
		{
			delete mTupleArray[i];
			mTupleArray[i] = NULL;
		}
		delete[] mTupleArray;
		mTupleArray = NULL;
	}
	if (mSD != NULL)
	{
		delete mSD;
		mSD = NULL;
	}

	if (mRandData != NULL && mIsNTuple)
	{
		for (uint i = 0; i<mTuplesCount; i++)
		{
			mRandData[i] = NULL;
			delete mRandData[i];
		}
		mRandData = NULL;
		delete[] mRandData;
	}
}

template<class TDataType, class TTuple>
bool common::data::cTuplesGenerator<TDataType, TTuple>::ReadHeader(uint &mDimension, uint &mTuplesCount)
{
	sscanf(mBuffer, "Dimension: %d Tuples Count: %d", &mDimension, &mTuplesCount);
	mSplitIndices = new uint[mDimension];
	ReadLine(mSplitIndices, mSplitsCount);
	ReadLine(mSplitIndices, mSplitsCount);
	ReadLine(mSplitIndices, mSplitsCount);
	if (mDimension == 0)
		throw "cTuplesGenerator::ReadHeader(): Failed to parse dimension!";
	if (mTuplesCount == 0)
		throw "cTuplesGenerator::ReadHeader(): Failed to parse tuples count!";
	return true;
}

///BULKLOADING for files with no header (dimension and tuples count lines)
template<class TDataType, class TTuple>
bool common::data::cTuplesGenerator<TDataType, TTuple>::ReadHeaderBLWith(uint &mDimension)
{
	sscanf(mBuffer, "Dimension: %d", &mDimension);
	mSplitIndices = new uint[mDimension];
	ReadLine(mSplitIndices, mSplitsCount);
	ReadLine(mSplitIndices, mSplitsCount);
	ReadLine(mSplitIndices, mSplitsCount);
	if (mDimension == 0)
		throw "cTuplesGenerator::ReadHeader(): Failed to parse dimension!";
	//if (mTuplesCount == 0)
	//throw "cTuplesGenerator::ReadHeader(): Failed to parse tuples count!";*/
	return true;
}

///BULKLOADING for files with no header (dimension and tuples count lines)
template<class TDataType, class TTuple>
bool common::data::cTuplesGenerator<TDataType, TTuple>::ReadHeaderBLWithout(uint &mDimension, uint &mTuplesCount)
{
	mSplitIndices = new uint[mDimension];
	/*ReadLine(mSplitIndices, mSplitsCount);
	ReadLine(mSplitIndices, mSplitsCount);
	ReadLine(mSplitIndices, mSplitsCount);
	if (mDimension == 0)
	throw "cTuplesGenerator::ReadHeader(): Failed to parse dimension!";
	//if (mTuplesCount == 0)
	//throw "cTuplesGenerator::ReadHeader(): Failed to parse tuples count!";*/
	return true;
}

template<class TDataType, class TTuple>
bool cTuplesGenerator<TDataType, TTuple>::ReadLine(unsigned int *separatorIndices, unsigned int &separatorCount)
{
	separatorCount = 0;

	// End of the last line reached? Finish the work.
	if (mByteRead != BUFF_LEN && mBufferOffset == mByteRead)
	{
		return false;
	}

	if (mByteRead <= 0)
	{
		if (!ReadBuffer(mBuffer, BUFF_LEN))
		{
			return false;
		}
	}

	int breakPos = -1;
	do
	{
		breakPos = SearchText('\n', ',', mBuffer + mBufferOffset, mByteRead - mBufferOffset, separatorIndices, separatorCount);

		if (breakPos == -1) //Break-line not found. Only part of line left in buffer. Need to seek back to start of the line and fill buffer to contain whole line.
		{
			// seek back
			mFileOffset -= mByteRead - mBufferOffset;
			mStream->Seek(mFileOffset);
			if (!ReadBuffer(mBuffer, BUFF_LEN))
			{
				return false;
			}
		}
		else
		{
			mLineLength = breakPos + 1;
			if ((mBuffer + mBufferOffset)[breakPos - 1] == '\r')
				breakPos--; //windows ends line with \r\n instead of single \n
			separatorIndices[separatorCount++] = breakPos;
			mLineStartBufferOffset = mBufferOffset;
			mBufferOffset += mLineLength;
			return true;
		}
	} while (breakPos == -1);
	return true;
}
template<class TDataType, class TTuple>
bool cTuplesGenerator<TDataType, TTuple>::ReadBuffer(char* buffer, uint count)
{
	bool ret = true;
	mStream->Read(buffer, BUFF_LEN, &mByteRead);
	if (mByteRead <= 0)
		printf("\nWarning: wrong byte count!");
	mFileOffset += mByteRead;
	mBufferOffset = 0;

	if (mByteRead == 0)
	{
		ret = false;
	}
	return ret;
}
template<class TDataType, class TTuple>
inline bool cTuplesGenerator<TDataType, TTuple>::HasNextTuple()
{
	int tuplesCount = mIsRangeQueriesFile ? mTuplesCount * 2 : mTuplesCount;
	return mArrayOrder < tuplesCount;
}

template<class TDataType, class TTuple>
inline TTuple* cTuplesGenerator<TDataType, TTuple>::GetTuple(uint order)
{
	/*
	if (mMode == Modes::Random)
	{
	if (mIsNTuple)
	{
	emptyTuple.Resize(GetSpaceDescriptor(), mTupleArray[order].GetLength());
	TTuple::Copy(emptyTuple.GetData(), mTupleArray[order].GetData(), GetSpaceDescriptor());
	}
	else
	{
	// bug: emptyTuple.Resize(GetSpaceDescriptor());
	TTuple::Copy(emptyTuple.GetData(), mTupleArray[order].GetData(), GetSpaceDescriptor());
	}
	}
	else
	{
	printf("Critical Error! cTuplesGenerator::GetTuple() is not implemented when reading a file stream.");
	assert(false);
	}
	*/
	return &(*mTupleArray[order]);
}

// VAL644
template<class TDataType, class TTuple>
inline void cTuplesGenerator<TDataType, TTuple>::GetTuple(TTuple& emptyTuple, uint order)
{
	if (mMode == Modes::Random)
	{
		if (mIsNTuple)
		{
			emptyTuple.Resize(GetSpaceDescriptor(), mRandArray[order].GetLength());
			TTuple::Copy(emptyTuple.GetData(), mRandArray[order].GetData(), GetSpaceDescriptor());
		}
		else
		{
			// bug: emptyTuple.Resize(GetSpaceDescriptor());
			TTuple::Copy(emptyTuple.GetData(), mRandArray[order].GetData(), GetSpaceDescriptor());
		}
	}
	else
	{
		printf("Critical Error! cTuplesGenerator::GetTuple() is not implemented when reading a file stream.");
		assert(false);
	}
}


template<class TDataType, class TTuple>
inline TTuple* cTuplesGenerator<TDataType, TTuple>::GetNextTuple()
{
	if (mMode == Modes::Random)
		return GetNextRandTuple();
	else
		return GetNextTupleFromFile();
}

// VAL644
template<class TDataType, class TTuple>
inline void cTuplesGenerator<TDataType, TTuple>::GetNextTuple(TTuple& emptyTuple)
{
	emptyTuple.Resize(GetSpaceDescriptor());
	if (mMode == Modes::Random)
		GetNextRandTuple(emptyTuple);
	else
		GetNextTupleFromFile(emptyTuple);
}

template<class TDataType, class TTuple>
inline void cTuplesGenerator<TDataType, TTuple>::GetNextTuples(TTuple* inputs, uint count, uint &returned)
{
	returned = 0;
	for (uint i = 0; i<count; i++)
	{
		if (HasNextTuple())
		{
			inputs[i] = *GetNextTuple();
		}
		else
		{
			break;
		}
		returned = i + 1;
	}

}
template<class TDataType, class TTuple>
inline void cTuplesGenerator<TDataType, TTuple>::GetNextTuples(TTuple* qls, TTuple* qhs, uint count, uint &returned)
{
	returned = 0;
	for (uint i = 0; i < count; i++)
	{
		if (HasNextTuple())
		{
			qls[i] = *GetNextTuple();
		}
		if (HasNextTuple())
		{
			qhs[i] = *GetNextTuple();
		}
		else
		{
			break;
		}
		returned = i + 1;
	}

}
template<class TDataType, class TTuple>
TTuple* cTuplesGenerator<TDataType, TTuple>::GetNextRandTuple()
{
	return &(*mTupleArray[mArrayOrder++]);
}
template<class TDataType, class TTuple>
TTuple* cTuplesGenerator<TDataType, TTuple>::GetNextTupleFromFile()
{
	TTuple* tuple = &(*mTupleArray[mArrayOrder]);
	tuple->Resize(mSD);
	if (!ReadLine(mSplitIndices, mSplitsCount))
		;// throw "cTuplesGenerator::GetNextTuple(): Attempt to get next tuple when all available tuples are already returned";
	uint offset = mLineStartBufferOffset;
	short len;
	int low;

	for (int i = 0; i < mSplitsCount; i++)
	{
		if (i == 0)
		{
			if (mSplitIndices[0] == 0)
				continue;
			low = mLineStartBufferOffset;
			len = mSplitIndices[i];
		}
		else
		{
			low = mLineStartBufferOffset + mSplitIndices[i - 1] + 1;
			len = mSplitIndices[i] - mSplitIndices[i - 1] - 1;
		}
		char* word = mBuffer + low;
		uint nmbr;
		if (strncmp(word, "min", 3) == 0)
		{
			tuple->SetValue(i, TDataType::ZERO, mSD);
		}
		else if (strncmp(word, "max", 3) == 0)
		{
			tuple->SetValue(i, TDataType::MAX, mSD);
		}
		else
		{
			switch (mSD->GetDimensionTypeCode(i))
			{
			case cFloat::CODE: tuple->SetValue(i, atof(word), mSD);  break;
			case cUInt::CODE:  tuple->SetValue(i, cNumber::atoui_ffast(word, len), mSD); break;
			case cInt::CODE:   tuple->SetValue(i, cNumber::atoi_ffast(word, len), mSD); break;
			default:
				throw "\nError! cTuplesGenerator::GetNextTupleFromFile(). Unknown data type."; break;
			}
		}
		offset += mSplitIndices[i] + 1;
	}
	//printf("\n");
	mArrayOrder++;
	return tuple;
}
//VAL644
template<class TDataType, class TTuple>
void cTuplesGenerator<TDataType, TTuple>::GetNextRandTuple(TTuple& emptyTuple)
{
	if (mArrayOrder >= mTuplesCount)
	{
		throw "cTuplesGenerator::GetNextTuple(): Attempt to get next tuple when all available tuples are already returned";
	}
	TTuple::Copy(emptyTuple.GetData(), mTupleArray[mArrayOrder++]->GetData(), GetSpaceDescriptor());
	//emptyTuple = mRandArray[mArrayOrder++];
}


template<class TDataType, class TTuple>
void cTuplesGenerator<TDataType, TTuple>::GetNextTupleFromFile(TTuple& mTuple)
{
	if (!ReadLine(mSplitIndices, mSplitsCount))
		;// throw "cTuplesGenerator::GetNextTuple(): Attempt to get next tuple when all available tuples are already returned";
	uint offset = mLineStartBufferOffset;
	short len;
	int low;

	for (int i = 0; i < mSplitsCount; i++)
	{
		if (i == 0)
		{
			if (mSplitIndices[0] == 0)
				continue;
			low = mLineStartBufferOffset;
			len = mSplitIndices[i];
		}
		else
		{
			low = mLineStartBufferOffset + mSplitIndices[i - 1] + 1;
			len = mSplitIndices[i] - mSplitIndices[i - 1] - 1;
		}
		char* word = mBuffer + low;
		uint nmbr;
		if (strncmp(word, "min", 3) == 0)
		{
			mTuple.SetValue(i, TDataType::ZERO, mSD);
		}
		else if (strncmp(word, "max", 3) == 0)
		{
			mTuple.SetValue(i, TDataType::MAX, mSD);
		}
		else
		{
			switch (mSD->GetDimensionTypeCode(i))
			{
			case cFloat::CODE: mTuple.SetValue(i, atof(word), mSD);  break;
			case cUInt::CODE:  mTuple.SetValue(i, cNumber::atoui_ffast(word, len), mSD); break;
			case cInt::CODE:   mTuple.SetValue(i, cNumber::atoi_ffast(word, len), mSD); break;
			default:
				throw "\nError! cTuplesGenerator::GetNextTupleFromFile(). Unknown data type."; break;
			}
		}
		offset += mSplitIndices[i] + 1;
	}
	//printf("\n");
	mArrayOrder++;
}

template<class TDataType, class TTuple>
inline void cTuplesGenerator<TDataType, TTuple>::GenerateRandomArray()
{
	common::random::cGaussRandomGenerator rg(false);
	uint howMany = ((mTuplesCount - mArrayOrder) > mRandArrayMaxCount) ? mRandArrayMaxCount : mTuplesCount - mArrayOrder;
	mTupleArray = new TTuple*[howMany];

	for (unsigned int i = 0; i < howMany; i++)
	{
		if (i % 10000 == 0)
		{
			printf("Number of pre-generated tuples: %d   \r", i);
		}
		uint len;
		if (mIsNTuple)
		{
			len = (mIsVarLen) ? i % (GetSpaceDescriptor()->GetDimension() - 1) + 2 : GetSpaceDescriptor()->GetDimension();
			mTupleArray[i] = new TTuple(mSD, len);
			//mTupleArray[i]->Resize(mSD, len);
			for (unsigned int j = 0; j < len; j++)
			{
				mTupleArray[i]->SetValue(j, rg.GetNextUInt(mDomain), mSD);
			}

			mRandData[i] = new char[len * 2 + 1];
			for (unsigned int j = 1; j < len * 2; j++)
			{
				mRandData[i][j] = 'a';
			}
			mRandData[i][0] = len * 2;
		}
		else
		{
			len = mSD->GetDimension();
			mTupleArray[i] = new TTuple(mSD);
			//mTupleArray[i]->Resize(mSD);
			for (unsigned int j = 0; j < len; j++)
			{
				mTupleArray[i]->SetValue(j, rg.GetNextUInt(mDomain), mSD);
			}

		}
	}
	printf("\n");

}
template<class TDataType, class TTuple>
inline char* cTuplesGenerator<TDataType, TTuple>::GetTupleData(uint order)
{
	if (mIsNTuple && mMode == Modes::Random)
	{
		return mRandData[order];
	}
	else
	{
		printf("\ncTuplesGenerator::GetTupleData is implemented only for variable length tuples in random mode.");
		assert(false);
	}
}
template<class TDataType, class TTuple>
inline uint cTuplesGenerator<TDataType, TTuple>::GetTuplesCount()
{
	return mTuplesCount;
}
template<class TDataType, class TTuple>
inline cSpaceDescriptor* cTuplesGenerator<TDataType, TTuple>::GetSpaceDescriptor()
{
	return mSD;
}
template<class TDataType, class TTuple>
inline TTuple cTuplesGenerator<TDataType, TTuple>::GetRandTuple(uint order)
{
	return &(*mTupleArray[order]);
}
template<class TDataType, class TTuple>
inline void cTuplesGenerator<TDataType, TTuple>::ResetPosition()
{
	mArrayOrder = 0;
	if (mMode == Modes::FromFile)
	{
		ResetFileStream();
	}
}

template<class TDataType, class TTuple>
inline void cTuplesGenerator<TDataType, TTuple>::ResetPosition(uint Dimension)
{
	mArrayOrder = 0;
	if (mMode == Modes::FromFile)
	{
		ResetFileStreamBL(Dimension);
	}
}

template<class TDataType, class TTuple>
inline void cTuplesGenerator<TDataType, TTuple>::ResetFileStream()
{
	mByteRead = -1;
	mFileOffset = 0;
	mArrayOrder = 0;
	if (mStream != NULL)
	{
		mStream->Close();
		mStream->Open(mFileName, ACCESS_READ, FILE_OPEN, FLAGS_NORMAL);
		mTuplesCount = 0;
		if (!ReadBuffer(mBuffer, BUFF_LEN))
			throw "cTuplesGenerator::ctor(char*): Attempt to load a file into buffer has failed!";
		if (!ReadHeader(mDimension, mTuplesCount))
			throw "cTuplesGenerator::ctor(char*): Attempt to read input file has failed!";
	}
}

///BULKLOADING
template<class TDataType, class TTuple>
inline void cTuplesGenerator<TDataType, TTuple>::ResetFileStreamBL()
{
	mByteRead = -1;
	mFileOffset = 0;
	mArrayOrder = 0;
	if (mStream != NULL)
	{
		mStream->Close();
		mStream->Open(mFileName, ACCESS_READ, FILE_OPEN, FLAGS_NORMAL);
		//mTuplesCount = 0;
		if (!ReadBuffer(mBuffer, BUFF_LEN))
			throw "cTuplesGenerator::ctor(char*): Attempt to load a file into buffer has failed!";
		if (!ReadHeaderBLWith(mDimension))
			throw "cTuplesGenerator::ctor(char*): Attempt to read input file has failed!";
	}
}

///BULKLOADING
template<class TDataType, class TTuple>
inline void cTuplesGenerator<TDataType, TTuple>::ResetFileStreamBL(uint Dimension)
{
	mByteRead = -1;
	mFileOffset = 0;
	mArrayOrder = 0;
	if (mStream != NULL)
	{
		mStream->Close();
		mStream->Open(mFileName, ACCESS_READ, FILE_OPEN, FLAGS_NORMAL);
		//mTuplesCount = 0;
		if (!ReadBuffer(mBuffer, BUFF_LEN))
			throw "cTuplesGenerator::ctor(char*): Attempt to load a file into buffer has failed!";
		if (!ReadHeaderBLWithout(Dimension, mTuplesCount))
			throw "cTuplesGenerator::ctor(char*): Attempt to read input file has failed!";
	}
}

template<class TDataType, class TTuple>
inline int cTuplesGenerator<TDataType, TTuple>::SearchText(const char mainChar, const char secondaryChar, const char *inputArray, const unsigned int arraySize, unsigned int* foundIndices, unsigned int &foundCount)
{
#ifdef SSE_ENABLED
	return cSSEUtils::SearchText('\n', ',', mSseEndLineChar, mSseSplitChar, mBuffer + mBufferOffset, mByteRead - mBufferOffset, foundIndices, foundCount);
#else
	foundCount = 0;
	int result = -1;

	for (uint i = 0; i < arraySize; i++)
	{
		if (inputArray[i] == mainChar)
		{
			return i;
		}
		if (inputArray[i] == secondaryChar)
		{
			foundIndices[foundCount++] = i;
		}
	}
	return result;
#endif
}
}
}
#endif 

