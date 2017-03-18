#include "cSpaceDescriptor.h"
#include "common/datatype/tuple/cTuple.h"

namespace common {
	namespace datatype {
		namespace tuple {

/// Constructor
/// Values are cuted/filled at the byte size.
cSpaceDescriptor::cSpaceDescriptor(unsigned int dimension, cDataType *type, cDataType *dimensionType)
  :cDTDescriptor(), mBitIndexes(NULL), mByteIndexes(NULL), mSizes(NULL), mDimensionTypes(NULL), mDebug(false), mDimensionTypeCodes(NULL),
  mDimensionSd(NULL), mHomogenousSd(false), mType(NULL), mComputeOrderValues(false)
{
	Create(dimension, type, dimensionType);
}

/// Constructor for bulkloading
cSpaceDescriptor::cSpaceDescriptor(unsigned int dimension, cDataType *type, cDataType *dimensionType, bool computeOrderValues)
	:cDTDescriptor(), mBitIndexes(NULL), mByteIndexes(NULL), mSizes(NULL), mDimensionTypes(NULL), mDebug(false), mDimensionTypeCodes(NULL),
	mDimensionSd(NULL), mHomogenousSd(false), mType(NULL), mComputeOrderValues(computeOrderValues), mMaxValue(NULL), mMinValue(NULL), mNormalizedValues1(NULL),
	mNormalizedValues2(NULL), mPrecomputedUnitSize(NULL)
{
	Create(dimension, type, dimensionType);
}
cSpaceDescriptor::cSpaceDescriptor(unsigned int dimension, cDataType *type, cDataType **dimensionType, bool computeOrderValues)
	:cDTDescriptor(), mBitIndexes(NULL), mByteIndexes(NULL), mSizes(NULL), mDimensionTypes(NULL), mDebug(false), mDimensionTypeCodes(NULL),
	mDimensionSd(NULL), mHomogenousSd(false), mType(NULL), mComputeOrderValues(computeOrderValues), mMaxValue(NULL), mMinValue(NULL), mNormalizedValues1(NULL),
	mNormalizedValues2(NULL), mPrecomputedUnitSize(NULL)
{
	Create(dimension, type, dimensionType);
}


			// VAL644
/// Values are cuted/filled at the byte size.
cSpaceDescriptor::cSpaceDescriptor(bool computeOrderValues) :cDTDescriptor(), mBitIndexes(NULL), mByteIndexes(NULL), mSizes(NULL),
mDimensionTypes(NULL), /*mMaxTuple(NULL), mComputeOrderValues(computeOrderValues), mMaxValue(NULL), mMinValue(NULL), mNormalizedValues1(NULL),
					   mNormalizedValues2(NULL), mPrecomputedUnitSize(NULL), */ mDebug(false), mDimensionTypeCodes(NULL), mDimensionSd(NULL), mHomogenousSd(false), mType(NULL)
{
}

// VAL644
/// Constructor
cSpaceDescriptor::cSpaceDescriptor(const cSpaceDescriptor &treeSpaceDescriptor, bool computeOrderValues)
	:cDTDescriptor(), mBitIndexes(NULL), mByteIndexes(NULL), mSizes(NULL), mDimensionTypes(NULL), /*mMaxTuple(NULL), mComputeOrderValues(computeOrderValues),
																								  mMaxValue(NULL), mMinValue(NULL), mNormalizedValues1(NULL), mNormalizedValues2(NULL), mPrecomputedUnitSize(NULL),*/ mDebug(false), mDimensionTypeCodes(NULL),
	mDimensionSd(NULL), mHomogenousSd(false), mType(NULL)
{
	Create(treeSpaceDescriptor);
}

// VAL644
/// Constructor
/// Values are cuted/filled at the byte size.
cSpaceDescriptor::cSpaceDescriptor(unsigned int dimension, cDataType *type, bool computeOrderValues)
	:cDTDescriptor(), mBitIndexes(NULL), mByteIndexes(NULL), mSizes(NULL), mDimensionTypes(NULL), /*mMaxTuple(NULL), mComputeOrderValues(computeOrderValues),
																								  mMaxValue(NULL), mMinValue(NULL), mNormalizedValues1(NULL), mNormalizedValues2(NULL), mPrecomputedUnitSize(NULL),*/ mDebug(false), mDimensionTypeCodes(NULL),
																								  mDimensionSd(NULL), mHomogenousSd(false), mType(NULL)
{
	Create(dimension, type);
}

// VAL644
/// Constructor
cSpaceDescriptor::cSpaceDescriptor(unsigned int dimension)
	: mBitIndexes(NULL), mByteIndexes(NULL), mSizes(NULL), mDimensionTypes(NULL), /*mMaxTuple(NULL), mComputeOrderValues(computeOrderValues),
																				  mMaxValue(NULL), mMinValue(NULL), mNormalizedValues1(NULL), mNormalizedValues2(NULL), mPrecomputedUnitSize(NULL),*/ mDebug(false), mDimensionTypeCodes(NULL),
																				  mDimensionSd(NULL), mHomogenousSd(false), mType(NULL)
{
	Create(dimension, NULL);
}

/// Create - VAL644 
void cSpaceDescriptor::Create(unsigned int dimension, cDataType *type)
{
	if (dimension != parent::mDimension)
	{
		Delete();
		parent::mDimension = dimension;		

		mType = type; // ZWI0009
		mTypeCode = type->GetCode(); // ZWI0009
		
		mBitIndexes = new unsigned int[parent::mDimension];
		mByteIndexes = new unsigned int[parent::mDimension];
		mSizes = new unsigned int[parent::mDimension + 1];
		mDimensionTypes = new cDataType*[parent::mDimension];
		mDimensionTypeCodes = new char[parent::mDimension];
		mDimensionSd = new cSpaceDescriptor*[parent::mDimension];
	}

	for (unsigned int i = 0; i < parent::mDimension; i++)
	{
		SetDimensionType(i, type);
		if (type != NULL)
		{
			mDimensionTypeCodes[i] = type->GetCode();
		}
		mDimensionSd[i] = NULL;
	}

	if (type != NULL)
	{
		mHomogenousSd = true;
		mTypeSize = type->GetSize();
		Setup();
	}
}

/// Create
void cSpaceDescriptor::Create(unsigned int dimension, cDataType *type, cDataType *dimensionType)
{
	assert(type != NULL);

	if (dimension != parent::mDimension)
	{
		Delete();
		parent::mDimension = dimension;
		mType = type;
		mTypeCode = type->GetCode();

		mBitIndexes = new unsigned int[parent::mDimension];
		mByteIndexes = new unsigned int[parent::mDimension];
		mSizes = new unsigned int[parent::mDimension + 1];
		mDimensionTypes = new cDataType*[parent::mDimension];
		mDimensionTypeCodes = new char[parent::mDimension];
		mDimensionSd = new cSpaceDescriptor*[parent::mDimension];

		mHomogenousSd = (dimensionType == NULL) ? false : true;
	}

	if (dimensionType != NULL)
	{
		for (unsigned int i = 0; i < parent::mDimension; i++)
		{
			SetDimensionType(i, dimensionType);
			mDimensionTypeCodes[i] = dimensionType->GetCode();
			mDimensionSd[i] = NULL;
		}
		Setup();
	}
	if (mComputeOrderValues)
	{
		mMaxValue->SetMaxValues(this);
		mMinValue->Clear(this);
		ComputeNormalizedUnitSize();
	}
}
//create pro vice typu
void cSpaceDescriptor::Create(unsigned int dimension, cDataType *type, cDataType **dimensionType)
{
	assert(type != NULL);

	if (dimension != parent::mDimension)
	{
		Delete();
		parent::mDimension = dimension;
		mType = type;
		mTypeCode = type->GetCode();

		mBitIndexes = new unsigned int[parent::mDimension];
		mByteIndexes = new unsigned int[parent::mDimension];
		mSizes = new unsigned int[parent::mDimension + 1];
		mDimensionTypes = new cDataType*[parent::mDimension];
		mDimensionTypeCodes = new char[parent::mDimension];
		mDimensionSd = new cSpaceDescriptor*[parent::mDimension];

		mHomogenousSd = (dimensionType == NULL) ? false : true;
	}

	if (dimensionType != NULL)
	{
		for (unsigned int i = 0; i < parent::mDimension; i++)
		{
			SetDimensionType(i, dimensionType[i]);
			mDimensionTypeCodes[i] = dimensionType[i]->GetCode();
			mDimensionSd[i] = NULL;
		}
		Setup();
	}
	if (mComputeOrderValues)
	{
		mMaxValue->SetMaxValues(this);
		mMinValue->Clear(this);
		ComputeNormalizedUnitSize();
	}
}




void cSpaceDescriptor::Create(const cSpaceDescriptor &treeSpaceDescriptor)
{
	if (parent::mDimension != treeSpaceDescriptor.GetDimension())
	{
		Delete();
		parent::mDimension = treeSpaceDescriptor.GetDimension();
		mBitIndexes = new unsigned int[parent::mDimension];
		mByteIndexes = new unsigned int[parent::mDimension];
		mSizes = new unsigned int[parent::mDimension + 1];
		mDimensionTypes = new cDataType*[parent::mDimension];
		mDimensionTypeCodes = new char[parent::mDimension];
		mDimensionSd = new cSpaceDescriptor*[parent::mDimension];
	}

	for (unsigned int i = 0; i < parent::mDimension; i++)
	{
		SetDimensionType(i, treeSpaceDescriptor.GetDimensionType(i));
		mDimensionTypeCodes[i] = treeSpaceDescriptor.GetDimensionType(i)->GetCode();
		mDimensionSd[i] = treeSpaceDescriptor.GetDimSpaceDescriptor(i);
	}

	Setup();
}

/// Destructor
cSpaceDescriptor::~cSpaceDescriptor()
{
	Delete();
}

void cSpaceDescriptor::Null()
{
	mBitIndexes = NULL;
	mByteIndexes = NULL;
	mSizes = NULL;
	mDimensionTypeCodes = NULL;
	mDimensionTypes = NULL;
}

void cSpaceDescriptor::Delete()
{
	if (mBitIndexes != NULL)
	{
		delete mBitIndexes;
	}
	if (mByteIndexes != NULL)
	{
		delete mByteIndexes;
	}
	if (mSizes != NULL)
	{
		delete mSizes;
	}
	if (mDimensionTypeCodes != NULL)
	{
		delete mDimensionTypeCodes;
	}	
	if (mDimensionTypes != NULL)
	{
		for (unsigned int i = 0; i < parent::mDimension; i++)
		{
			if (mDimensionTypes[i] != NULL)
			{
				if (mHomogenousSd && i == 0)
				{
					delete mDimensionTypes[i];
				}
				mDimensionTypes[i] = NULL;
			}
		}

		delete mDimensionTypes;
		mDimensionTypes = NULL;
	}
	if (mDimensionSd != NULL)
	{
		delete mDimensionSd;
	}

	if (mType != NULL)
	{
		delete mType;
	}
}

// Compute bit and byte indexes of each dimension and byte size
void cSpaceDescriptor::Setup()
{
	// VAL644
	//unsigned int i;

	//mByteSize = 0;
	//if (parent::mDimension > 0)
	//{
	//	mBitIndexes[0] = 0;
	//	mByteIndexes[0] = 0;
	//	mByteSize = mDimensionTypes[0]->GetMaxSize(mDimensionSd[0]);
	//	mSizes[0] = mByteSize;
	//	if (mDimensionSd[0] != NULL)
	//	{
	//		mDimensionSd[0]->Setup();
	//	}

	//	for (i = 1; i < parent::mDimension; i++)
	//	{
	//		if (mDimensionSd[i] != NULL)
	//		{
	//			mDimensionSd[i]->Setup();
	//		}

	//		mByteIndexes[i] = mByteIndexes[i - 1] + GetDimensionType(i - 1)->GetMaxSize(GetDimSpaceDescriptor(i - 1));
	//		mBitIndexes[i] = mByteIndexes[i] * cNumber::BYTE_LENGTH;
	//		mSizes[i] = GetDimensionType(i)->GetMaxSize(GetDimSpaceDescriptor(i));
	//		mByteSize += mSizes[i];
	//	}
	//	mSizes[i] = mByteSize;
	//}

	unsigned int i;
	mSize = mTypeSize = 0;
	char lastTypeCode = NULL;
	mHomogenousSd = true;

	if (parent::mDimension > 0)
	{
		mBitIndexes[0] = 0;
		mByteIndexes[0] = 0;
		mIsAnyDimDescriptor = false;

		for (i = 0 ; i < parent::mDimension ; i++)
		{
			if (mDimensionSd[i] != NULL)
			{
				mIsAnyDimDescriptor = true;
				mDimensionSd[i]->Setup();
			}

			if (i > 0 && mHomogenousSd && GetDimensionTypeCode(i) != lastTypeCode)
			{
				mHomogenousSd = false;
			}
			lastTypeCode = GetDimensionTypeCode(i);

			if (i != 0)
			{
				mByteIndexes[i] = mByteIndexes[i - 1] + GetDimensionType(i-1)->GetMaxSize(GetDimSpaceDescriptor(i-1));
				mBitIndexes[i] = mByteIndexes[i] * cNumber::BYTE_LENGTH;
			}

			mSizes[i] = GetDimensionType(i)->GetMaxSize(GetDimSpaceDescriptor(i));//tuple
			mSize += mSizes[i];
		}
		mTypeSize = mType->GetSize(mSize);
	}
	if (mComputeOrderValues)
	{
		if (mMaxValue == NULL)
		{
			mMaxValue = new cTuple(this);
		}
		if (mMinValue == NULL)
		{
			mMinValue = new cTuple(this);
		}
		if (mNormalizedValues1 == NULL)
		{
			mNormalizedValues1 = new unsigned int[mDimension];
		}
		if (mNormalizedValues2 == NULL)
		{
			mNormalizedValues2 = new unsigned int[mDimension];
		}
		if (mPrecomputedUnitSize == NULL)
		{
			mPrecomputedUnitSize = new double[mDimension];
		}
	}
}

/// Set type of dimension
/// \param dim Dimension which should be set
/// \param type Type of the dimension
void cSpaceDescriptor::SetDimensionType(unsigned int dim, cDataType *dimensionType)
{
	assert(dimensionType != NULL);

	mDimensionTypes[dim] = dimensionType;
	mDimensionTypeCodes[dim] = dimensionType->GetCode();
}

/// Set type of dimension - if you use this method, you set this space descriptor as homogeneous - it means all dimensions share the 
/// same data type (and inner space descriptor).
/// \param type Type of the dimension
void cSpaceDescriptor::SetDimensionType(cDataType *type)
{
	mHomogenousSd = true;
	mDimensionTypes[0] = type;

	if (type != NULL)
	{
		mDimensionTypeCodes[0] = type->GetCode();
		mTypeSize = type->GetSize(GetDimSpaceDescriptor(0));
	}
}


/// Get size of serialized space descriptor
unsigned int cSpaceDescriptor::GetSerialSize()
{
	return sizeof(parent::mDimension) + parent::mDimension * sizeof(cDataType::CODE);
}

/**
 * Get the size of the item in the case the item includes only length dimensions.
 */
unsigned int cSpaceDescriptor::GetLSize(unsigned int tupleLength) const
{
	int size = 0;

	if (mHomogenousSd)
	{
		size = mSizes[0] * tupleLength;
	} else
	{
		for (uint i = 0 ; i < tupleLength ; i++)
		{
			size += mSizes[0];
		}
	}
	return size;
}

bitmask_t cSpaceDescriptor::getIntBits(unsigned nDims, unsigned nBytes, char const* c, unsigned y)
{
	unsigned const bit = y%8;
	unsigned const offs = whichByte(nBytes,y);
	unsigned d;
	bitmask_t bits = 0;
	c += offs;
	for (d = 0; d < nDims; ++d)
	{
		bits |= rdbit(*c, bit) << d;
		c += nBytes;
	}
	return bits;
}

int cSpaceDescriptor::hilbert_cmp_work(unsigned nDims, unsigned nBytes, unsigned nBits,
				 unsigned max, unsigned y, char const* c1, char const* c2,	 unsigned rotation,
				 bitmask_t bits, bitmask_t index, BitReader getBits)
{
	bitmask_t const one = 1;
	bitmask_t const nd1Ones = ones(bitmask_t,nDims) >> 1; /* used in adjust_rotation macro */
	while (y-- > max)
	{
		bitmask_t reflection = getBits(nDims, nBytes, c1, y);
		bitmask_t diff = reflection ^ getBits(nDims, nBytes, c2, y);
		bits ^= reflection;
		bits = rotateRight(bits, rotation, nDims);
		if (diff)
		{
			unsigned d;
			diff = rotateRight(diff, rotation, nDims);
			for (d = 1; d < nDims; d *= 2)
			{
				index ^= index >> d;
				bits  ^= bits  >> d;
				diff  ^= diff  >> d;
			}
			return (((index ^ y ^ nBits) & 1) == (bits < (bits^diff)))? -1: 1;
		}
		index ^= bits;
		reflection ^= one << rotation;
		adjust_rotation(rotation,nDims,bits);
		bits = reflection;
	}
	return 0;
}

 /// Serialization of space descriptor.
bool cSpaceDescriptor::Write(cStream *stream) const
{
	bool ret = stream->Write((char*)&(parent::mDimension), sizeof(parent::mDimension));
	ret &= stream->Write((char*)(&mTypeCode), sizeof(mTypeCode));

	for (unsigned int i = 0; i < parent::mDimension ; i++)
	{
		char code = mDimensionTypes[i]->GetCode();
		ret &= stream->Write(&code, sizeof(mDimensionTypes[i]->GetCode()));
	}
	return ret;
}

// Deserialization of space descriptor.
bool cSpaceDescriptor::Read(cStream *stream)
{
	unsigned int dimension;
	bool anyChange = false;

	bool ret = stream->Read((char*)&dimension, sizeof(dimension));
	ret &= stream->Read(&mTypeCode, sizeof(mTypeCode));

	if (parent::mDimension != dimension)
	{
		Delete();
		anyChange = true;
	}

	char code;
	for (unsigned int i = 0; i < parent::mDimension; i++)
	{
		ret &= stream->Read((char*)&code, sizeof(cDataType::CODE));
		if (GetDimensionTypeCode(i) != code && mDimensionTypes[i] != NULL)
		{
			delete mDimensionTypes[i];
			mDimensionTypes[i] = CreateDataType(code);
			anyChange = true;
		}
	}

	if (anyChange)
	{
		Setup();
	}
	return ret;
}

cDataType* cSpaceDescriptor::CreateDataType(char code)
{
	cDataType *type;
	switch(code)
	{
	case cChar::CODE:
		type = new cChar();
		break;
	case cWChar::CODE:
		type = new cWChar();
		break;
	case cFloat::CODE:
		type = new cFloat();
		break;
	case cInt::CODE:
		type = new cInt();
		break;
	case cUInt::CODE:
		type = new cUInt();
		break;
	case cLong::CODE:
		type = new cLong();
		break;
	case cDouble::CODE:
		type = new cDouble();
		break;
	default:
		printf("cSpaceDescriptor::CreateDataType() - data type not found!\n");
		type = new cChar();
		break;
	}
	return type;
}

// It is necessary to call ComputeIndexes() for setup the space descriptor
void cSpaceDescriptor::SetDimSpaceDescriptor(unsigned int order, cSpaceDescriptor* sd)
{
	mDimensionSd[order] = sd;
}

/// Compare two tuples according to their z-value. The first step is tuple normalization and therefore
/// tuples with different domain in each dimension can be comprared with better distribution
/// \param tuple1 The first tuple
/// \param tuple2 The second tuple
/// \return
///		- -1 if the first tuple is smaller than the second tuple
///		- 0 if the tuples are the same
///		- 1 if the second tuple is smaller than the first tuple
int cSpaceDescriptor::CompareNormalizedZValues(const char* tuple1, const char* tuple2, cSpaceDescriptor* pSd)
{
	unsigned int mask = 0x80000000;

	assert(mComputeOrderValues);
	ComputeNormalizedValues(tuple1, tuple2, pSd);
	for (unsigned int j = 0; j < 32; j++)
	{
		for (unsigned int i = 0; i < parent::mDimension; i++)
		{
			if ((mNormalizedValues1[i] & mask) < (mNormalizedValues2[i] & mask))
			{
				return -1;
			}
			else if ((mNormalizedValues1[i] & mask) > (mNormalizedValues2[i] & mask))
			{
				return 1;
			}
		}
		mask >>= 1;
	}

	return 0;
}

/// Compare two items according to their hilbert value.
/// \param tuple1 The first tuple
/// \param tuple2 The second tuple
/// \return
///		- -1 if the first tuple is smaller than the second tuple
///		- 0 if the tuples are the same
///		- 1 if the second tuple is smaller than the first tuple
int cSpaceDescriptor::CompareNormalizedHValues(const char* tuple1, const char* tuple2, cSpaceDescriptor* pSd)
{
	int nDims = parent::mDimension;
	int nBytes = sizeof(unsigned int);
	int nBits = nBytes * 8;
	bitmask_t const one = 1;
	bitmask_t bits = one << (nDims - 1);

	assert(mComputeOrderValues);
	if (mDebug)
	{
		cTuple::Print(tuple1, "\n", pSd);
		cTuple::Print(tuple2, "\n", pSd);
	}
	ComputeNormalizedValues(tuple1, tuple2, pSd);
	char* c1 = (char*) mNormalizedValues1;
	char* c2 = (char*) mNormalizedValues2;

	return hilbert_cmp_work(nDims, nBytes, nBits, 0, nBits,
		(char const*) c1, (char const*) c2,
		0, bits, bits, cSpaceDescriptor::getIntBits);
}

void cSpaceDescriptor::ComputeNormalizedValues(const char* tuple1, const char* tuple2, const cSpaceDescriptor* pSd)
{
	for (unsigned int i = 0; i < pSd->GetDimension(); i++)
	{

		switch (pSd->GetDimensionTypeCode(i))
		{
		case cInt::CODE:
			mNormalizedValues1[i] = (unsigned int) ((cTuple::GetInt(tuple1, i, pSd) - mMinValue->GetInt(i, pSd)) * mPrecomputedUnitSize[i]);
			mNormalizedValues2[i] = (unsigned int) ((cTuple::GetInt(tuple2, i, pSd) - mMinValue->GetInt(i, pSd)) * mPrecomputedUnitSize[i]);
			break;
		case cUInt::CODE:
			//assert(tuple1.GetUInt(i) <= mMaxValue->GetUInt(i));
			//assert(tuple1.GetUInt(i) >= mMinValue->GetUInt(i));
			mNormalizedValues1[i] = (unsigned int) ((cTuple::GetUInt(tuple1, i, pSd) - mMinValue->GetUInt(i, pSd)) * mPrecomputedUnitSize[i]);
			//assert(tuple2.GetUInt(i) <= mMaxValue->GetUInt(i));
			//assert(tuple2.GetUInt(i) >= mMinValue->GetUInt(i));
			mNormalizedValues2[i] = (unsigned int) ((cTuple::GetUInt(tuple2, i, pSd) - mMinValue->GetUInt(i, pSd)) * mPrecomputedUnitSize[i]);
			break;
		case cFloat::CODE:
			mNormalizedValues1[i] = (unsigned int) ((cTuple::GetFloat(tuple1, i, pSd) - mMinValue->GetFloat(i, pSd)) * mPrecomputedUnitSize[i]);
			mNormalizedValues2[i] = (unsigned int) ((cTuple::GetFloat(tuple2, i, pSd) - mMinValue->GetFloat(i, pSd)) * mPrecomputedUnitSize[i]);
			break;
		case cChar::CODE:
			mNormalizedValues1[i] = (unsigned int) (cTuple::GetByte(tuple1, i, pSd) * mPrecomputedUnitSize[i]);
			mNormalizedValues2[i] = (unsigned int) (cTuple::GetByte(tuple2, i, pSd) * mPrecomputedUnitSize[i]);
			break;
		case cShort::CODE:
			mNormalizedValues1[i] = (unsigned int) (cTuple::GetShort(tuple1, i, pSd) * mPrecomputedUnitSize[i]);
			mNormalizedValues2[i] = (unsigned int) (cTuple::GetShort(tuple2, i, pSd) * mPrecomputedUnitSize[i]);
			break;
		}

	}
}

void cSpaceDescriptor::ComputeNormalizedUnitSize()
{
	assert(mComputeOrderValues);
	for (unsigned int i = 0; i < parent::mDimension; i++)
	{
		switch (GetDimensionTypeCode(i))
		{
		case cInt::CODE:
			mPrecomputedUnitSize[i] = (double)Z_VALUE_SIZE / (double)(mMaxValue->GetInt(i, this) - mMinValue->GetInt(i, this));
			break;
		case cUInt::CODE:
			mPrecomputedUnitSize[i] = (double)Z_VALUE_SIZE / (double)(mMaxValue->GetUInt(i, this) - mMinValue->GetUInt(i, this));
			break;
		case cFloat::CODE:
			mPrecomputedUnitSize[i] = (double)Z_VALUE_SIZE / (double)(mMaxValue->GetFloat(i, this) - mMinValue->GetFloat(i, this));
			break;
		case cChar::CODE:
			mPrecomputedUnitSize[i] = (double)Z_VALUE_SIZE / (double)(mMaxValue->GetByte(i, this) - mMinValue->GetByte(i, this));
			break;
		case cShort::CODE:
			mPrecomputedUnitSize[i] = (double)Z_VALUE_SIZE / (double)(mMaxValue->GetShort(i, this) - mMinValue->GetShort(i, this));
			break;
		}
	}
}
}}}