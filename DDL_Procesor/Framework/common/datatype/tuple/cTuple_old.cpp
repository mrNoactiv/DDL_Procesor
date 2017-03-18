#include "cTuple.h"

namespace common {
	namespace datatype {
		namespace tuple {

unsigned int cTuple::Sum1()
{
	unsigned int sum;
	unsigned int dim = mSpaceDescriptor->GetDimension();

	for (unsigned int i = 0 ; i < dim ; i++)
	{
		sum = dim + i;
	}
	return sum;
}

unsigned int cTuple::Sum2()
{
	unsigned int sum;

	for (unsigned int i = 0 ; i < mSpaceDescriptor->GetDimension() ; i++)
	{
		sum = mSpaceDescriptor->GetDimension() + i;
	}
	return sum;
}

/**
* Constructor
*/
cTuple::cTuple(): mData(NULL), mSpaceDescriptor(NULL)
{
}

/**
* Constructor
*/
cTuple::cTuple(cSpaceDescriptor *spaceDescriptor): mSpaceDescriptor(NULL), mData(NULL)
{
	Resize(spaceDescriptor);
}

/**
* Destructor
*/
cTuple::~cTuple()
{
	Free();
}

void cTuple::Free()
{
	if (mData != NULL)
	{
		delete mData;
		mData = NULL;
	}
}

/**
* Resize the tuple acording to space descriptor
*/
void cTuple::Resize(cSpaceDescriptor *spaceDescriptor)
{
	if (mSpaceDescriptor != NULL)
	{
		if (mSpaceDescriptor->GetByteSize() != spaceDescriptor->GetByteSize())
		{
			if (mData != NULL)
			{
				delete mData;
			}
			mData = new char[spaceDescriptor->GetByteSize()];
		}
	} else
	{
		mData = new char[spaceDescriptor->GetByteSize()];
	}
	mSpaceDescriptor = spaceDescriptor;

	// TODO je nutný clear vektoru? Nezpomaluje to zbytečně kód?
	Clear();
}

/**
* Resize tuple with a new tree space descriptor
* \param spaceDescriptor New tree space descriptor
* \param memory Memory
*/
void cTuple::Resize(cSpaceDescriptor *spaceDescriptor, cMemory *memory)
{
	mSpaceDescriptor = spaceDescriptor;
	mData = memory->GetMemory(spaceDescriptor->GetByteSize());
	Clear();
}

/**
* Copy values from the tuple in the argument into this tuple
*/
void cTuple::operator = (const cTuple &tuple)
{
#ifndef NDEBUG
	if (!tuple.GetSpaceDescriptor())
	{
		printf("error: cTuple::operator =  Space descriptor of the right tuple is not specified\n");
		assert(!tuple.GetSpaceDescriptor());
		exit(1);
	}
	else if (!mSpaceDescriptor)
	{
		printf("error: cTuple::operator =  Space descriptor of the left tuple is not specified\n");
		assert(!mSpaceDescriptor);
		exit(1);
	}	
#endif

	if (mSpaceDescriptor->GetByteSize() < tuple.GetSpaceDescriptor()->GetByteSize())
	{
		CopyMemory(mData, tuple.GetData(), mSpaceDescriptor->GetByteSize());
	} else
	{
		CopyMemory(mData, tuple.GetData(), tuple.GetSpaceDescriptor()->GetByteSize());
	}
}

/// Copy values from tuple into this tuple
void cTuple::operator += (const cTuple &tuple)
{
#ifndef NDEBUG
	if (!tuple.GetSpaceDescriptor())
	{
		printf("error: cTuple::operator +=  Space descriptor of the right tuple is not specified\n");
		assert(!tuple.GetSpaceDescriptor());
		exit(1);
	}
	else if (!mSpaceDescriptor)
	{
		printf("error: cTuple::operator +=  Space descriptor of the left tuple is not specified\n");
		assert(!mSpaceDescriptor);
		exit(1);
	}	

	if (mSpaceDescriptor->GetDimension() > tuple.GetSpaceDescriptor()->GetDimension())
	{
		printf("error: cTuple::operator +=  Space descriptor of the left tuple is not specified\n");
		exit(1);
	}
#endif
	
	for (unsigned int i = 0; i < mSpaceDescriptor->GetDimension(); i++)
	{
		switch (mSpaceDescriptor->GetType(i)->GetCode())
		{
		case cUInt::CODE:
			SetValue(i, GetUInt(i) + tuple.GetUInt(i));
			break;
		case cInt::CODE:
			SetValue(i, GetInt(i) + tuple.GetInt(i));
			break;
		case cUShort::CODE:
			SetValue(i, GetUShort(i) + tuple.GetUShort(i));
			break;
		case cShort::CODE:
			SetValue(i, GetShort(i) + tuple.GetShort(i));
			break;
		case cChar::CODE:
			SetValue(i, GetByte(i) + tuple.GetByte(i));
			break;
		case cWChar::CODE:
			SetValue(i, GetWChar(i) + tuple.GetWChar(i));
			break;
		case cFloat::CODE:
			SetValue(i, GetFloat(i) + tuple.GetFloat(i));
			break;
		}
	}
}

/// Compute euclidian distance from tuple to this tuple
double cTuple::EuclidianIntDistance(const cTuple &tuple) const
{
	int tmp;
	double sum = 0;
	
	for(unsigned int i = 0; i < mSpaceDescriptor->GetDimension(); i++)
	{
		switch (mSpaceDescriptor->GetType(i)->GetCode())
		{
		case cUInt::CODE:
			tmp = GetUInt(i) - tuple.GetUInt(i);
			sum += tmp * tmp;
			break;
		case cInt::CODE:
			tmp = GetInt(i) - tuple.GetInt(i);
			sum += tmp * tmp;
			break;
		}
	}
	return sum;
}

/**
* Modify MBR according to the tuple.
* \param mbr1 Lower tuple of the MBR.
* \param mbr2 Higher tuple of the MBR.
* \return
*		- true if the MBR was modified,
*		- false otherwise.
*/
bool cTuple::ModifyMbr(cTuple &mbrl, cTuple &mbrh) const
{
	bool modified = false;

	for (unsigned int i = 0 ; i < mSpaceDescriptor->GetDimension() ; i++)
	{
		switch (mSpaceDescriptor->GetType(i)->GetCode())
		{
		case cFloat::CODE:
			if (mbrl.GetFloat(i) <= mbrh.GetFloat(i))
			{
				if (mbrl.GetFloat(i) > GetFloat(i))
				{
					mbrl.SetValue(i, GetFloat(i));
					modified = true;
				}
				else if (mbrh.GetFloat(i) < GetFloat(i))
				{
					mbrh.SetValue(i, GetFloat(i));
					modified = true;
				}
			}
			else
			{
				if (mbrh.GetFloat(i) > GetFloat(i))
				{
					mbrh.SetValue(i, GetFloat(i));
					modified = true;
				}
				else if (mbrl.GetFloat(i) < GetFloat(i))
				{
					mbrl.SetValue(i, GetFloat(i));
					modified = true;
				}
			}
			break;
		case cInt::CODE:
			if (mbrl.GetInt(i) <= mbrh.GetInt(i))
			{
				if (mbrl.GetInt(i) > GetInt(i))
				{
					mbrl.SetValue(i, GetInt(i));
					modified = true;
				}
				else if (mbrh.GetInt(i) < GetInt(i))
				{
					mbrh.SetValue(i, GetInt(i));
					modified = true;
				}
			}
			else
			{
				if (mbrh.GetInt(i) > GetInt(i))
				{
					mbrh.SetValue(i, GetInt(i));
					modified = true;
				}
				else if (mbrl.GetInt(i) < GetInt(i))
				{
					mbrl.SetValue(i, GetInt(i));
					modified = true;
				}
			}
			break;
		case cUInt::CODE:
			if (mbrl.GetUInt(i) <= mbrh.GetUInt(i))
			{
				if (mbrl.GetUInt(i) > GetUInt(i))
				{
					mbrl.SetValue(i, GetUInt(i));
					modified = true;
				}
				else if (mbrh.GetUInt(i) < GetUInt(i))
				{
					mbrh.SetValue(i, GetUInt(i));
					modified = true;
				}
			}
			else
			{
				if (mbrh.GetUInt(i) > GetUInt(i))
				{
					mbrh.SetValue(i, GetUInt(i));
					modified = true;
				}
				else if (mbrl.GetUInt(i) < GetUInt(i))
				{
					mbrl.SetValue(i, GetUInt(i));
					modified = true;
				}
			}
			break;
		case cChar::CODE:
			if (mbrl.GetByte(i) <= mbrh.GetByte(i))
			{
				if (mbrl.GetByte(i) > GetByte(i))
				{
					mbrl.SetValue(i, GetByte(i));
					modified = true;
				}
				else if (mbrh.GetByte(i) < GetByte(i))
				{
					mbrh.SetValue(i, GetByte(i));
					modified = true;
				}
			}
			else
			{
				if (mbrh.GetByte(i) > GetByte(i))
				{
					mbrh.SetValue(i, GetByte(i));
					modified = true;
				}
				else if (mbrl.GetByte(i) < GetByte(i))
				{
					mbrl.SetValue(i, GetByte(i));
					modified = true;
				}
			}
			break;
		}
	}

	return modified;
}

/**
* Equality test of order-th coordinate. 
* \return -1 if this < tuple, 0 if tuples' coordinates are the same, 1 if this > tuple.
*/
inline int cTuple::Equal(const cTuple &tuple, unsigned int order) const
{
	assert(order < mSpaceDescriptor->GetDimension());
	int ret = 1;

	switch (mSpaceDescriptor->GetType(order)->GetCode())
	{
	case cFloat::CODE:
		if (GetFloat(order) < tuple.GetFloat(order))
		{
			ret = -1;
		}
		else if (GetFloat(order) == tuple.GetFloat(order))
		{
			ret = 0;
		}
		break;
	case cInt::CODE:
		if (GetInt(order) < tuple.GetInt(order))
		{
			ret = -1;
		}
		else if (GetInt(order) == tuple.GetInt(order))
		{
			ret = 0;
		}
		break;
	case cUInt::CODE:
		if (GetUInt(order) < tuple.GetUInt(order))
		{
			ret = -1;
		}
		else if (GetUInt(order) == tuple.GetUInt(order))
		{
			ret = 0;
		}
		break;
	case cUShort::CODE:
		if (GetUShort(order) < tuple.GetUShort(order))
		{
			ret = -1;
		}
		else if (GetUShort(order) == tuple.GetUShort(order))
		{
			ret = 0;
		}
		break;
	case cShort::CODE:
		if (GetShort(order) < tuple.GetShort(order))
		{
			ret = -1;
		}
		else if (GetShort(order) == tuple.GetShort(order))
		{
			ret = 0;
		}
		break;
	case cChar::CODE:
		if ((unsigned char)GetByte(order) < (unsigned char)tuple.GetByte(order))
		{
			ret = -1;
		}
		else if (GetByte(order) == tuple.GetByte(order))
		{
			ret = 0;
		}
		break;
	case cWChar::CODE:
		if ((unsigned char)GetWChar(order) < (unsigned char)tuple.GetWChar(order))
		{
			ret = -1;
		}
		else if (GetWChar(order) == tuple.GetWChar(order))
		{
			ret = 0;
		}
		break;
	}
	return ret;
}

/**
* Length of the interval of order-th coordinates this and tuple.
*/
float cTuple::UnitIntervalLength(const cTuple &tuple, unsigned int order) const
{
	assert(order < mSpaceDescriptor->GetDimension());
	float interval = 0.0;

	switch (mSpaceDescriptor->GetType(order)->GetCode())
	{
	case cFloat::CODE:
		interval = cNumber::Abs(GetFloat(order) - tuple.GetFloat(order))/cFloat::MAX;
		break;
	case cInt::CODE:
		interval = (float)cNumber::Abs(GetInt(order) - tuple.GetInt(order))/cInt::MAX;
		break;
	case cUInt::CODE:
		if (GetUInt(order) < tuple.GetUInt(order))
		{
			interval = (float)(tuple.GetUInt(order) - GetUInt(order));
		}
		else
		{
			interval = (float)(GetUInt(order) - tuple.GetUInt(order));
		}
		interval /= cUInt::MAX;
		break;
	case cChar::CODE:
		interval = (float)cNumber::Abs(GetByte(order) - tuple.GetByte(order))/cChar::MAX;
		break;
	}
	return interval;
}

/**
* \return true if the tuple is contained into n-dimensional query block.
*/
bool cTuple::IsInBlock(const cTuple &ql, const cTuple &qh) const
{
	bool ret = true;
	for (unsigned int i = 0 ; i < mSpaceDescriptor->GetDimension() ; i++)
	{
		if (!IsInInterval(ql, qh, i))
		{
			ret = false;
			break;
		}
	}
	return ret;
}

/**
* Set max values.
*/
void cTuple::SetMaxValues()
{
	for (unsigned int i = 0 ; i < mSpaceDescriptor->GetDimension() ; i++)
	{
		SetMaxValue(i);
	}
}

/**
* \return true if the tuple coordinate is contained in the interval.
*/
bool cTuple::IsInInterval(const cTuple &ql, const cTuple &qh, unsigned int order) const
{
	assert(order < mSpaceDescriptor->GetDimension());

	bool ret = true;
	int eq;

	if ((eq = Equal(ql, order)) > 0)
	{
		if (Equal(qh, order) > 0)
		{
			ret = false;
		}
	}
	else if (eq < 0)
	{
		if (Equal(qh, order) < 0)
		{
			ret = false;
		}
	}
	return ret;
}


/**
* Print this tuple
* \param string This string is printed out at the end of the tuple.
*/
void cTuple::Print(const char *string) const
{
	printf("(");
	for (unsigned int i = 0 ; i < mSpaceDescriptor->GetDimension() ; i++)
	{
		Print(i, "");
		if (i != mSpaceDescriptor->GetDimension()-1)
		{
			printf(", ");
		}
	}
	printf(")%s", string);
}

/// Set the tuple. The dimension may be different.
void cTuple::SetTuple(const cTuple &tuple)
{
	unsigned int minDimension = mSpaceDescriptor->GetDimension();
	if (minDimension > tuple.GetSpaceDescriptor()->GetDimension())
	{
		minDimension = tuple.GetSpaceDescriptor()->GetDimension();
	}
	// check data types
	for (unsigned int i = 0 ; i < minDimension ; i++)
	{
		assert(mSpaceDescriptor->GetType(i)->GetCode() == tuple.GetSpaceDescriptor()->GetType(i)->GetCode());
	}

	Clear();
	unsigned int minByteSize = mSpaceDescriptor->GetByteSize();
	if (minByteSize > tuple.GetSpaceDescriptor()->GetByteSize())
	{
		minByteSize = tuple.GetSpaceDescriptor()->GetByteSize();
	}
	memcpy(mData, tuple.GetData(), minByteSize);
}

/**
* Print just one dimension of this tuple
* \param order Order of the dimension.
* \param string This string is printed out at the end of the tuple.
*/
void cTuple::Print(unsigned int order, char *string) const
{
	if (mSpaceDescriptor->GetType(order)->GetCode() == cChar::CODE)
	{
		printf("%X", (unsigned char)GetByte(order));
	} 
	else if (mSpaceDescriptor->GetType(order)->GetCode() == cWChar::CODE)
	{
		printf("%X", (unsigned char)GetWChar(order));
	} 
	else  if (mSpaceDescriptor->GetType(order)->GetCode() == cInt::CODE)
	{
		printf("%d", GetInt(order));
	} 
	else if (mSpaceDescriptor->GetType(order)->GetCode() == cUInt::CODE)
	{
		printf("%d", GetUInt(order));
	}
	else if (mSpaceDescriptor->GetType(order)->GetCode() == cShort::CODE)
	{
		printf("%d", GetShort(order));
	}
	else if (mSpaceDescriptor->GetType(order)->GetCode() == cUShort::CODE)
	{
		printf("%d", GetUShort(order));
	}
	else if (mSpaceDescriptor->GetType(order)->GetCode() == cFloat::CODE)
	{
		printf("%f", GetFloat(order));
	}
	printf("%s", string);
}

void cTuple::ComputeHAddress(cBitString &hvalue) const
{
	hilbert_c2i(mSpaceDescriptor->GetDimension(),32, hvalue);
}

void cTuple::hilbert_c2i(int nDims, int nBits, cBitString &hvalue) const
{
	bitmask_t const one = 1;
	bitmask_t const ndOnes = (one << nDims) - 1;
	bitmask_t const nthbits = (((one << nDims*nBits) - one) / ndOnes) >> 1;
	int b, d;
	int rotation = 0; /* or (nBits * (nDims-1)) % nDims; */
	bitmask_t reflection = 0;
	bitmask_t index = 0;

	for (b = nBits; b--;)
	{
		bitmask_t bits = reflection;
		reflection = 0;

		for ( d = 0; d < nDims; d++)
		{
			int value = GetUInt(d);
			reflection |= ((value >> b) & 1 ) << d;
		}

		bits ^= reflection;
		bits = rotateRight(bits, rotation, nDims);
		index |= bits << nDims*b;
		reflection ^= one << rotation;

		adjust_rotation(rotation, nDims, bits);
	}

	index ^= nthbits;

	for (d = 1; ; d *= 2) 
	{
		bitmask_t t;
		if (d <= 32) 
		{
			t = index >> d;
			if (!t)
			{
				break;
			}
		}
		else 
		{
			t = index >> 32;
			t = t >> (d - 32);
			if (!t)
			{
				break;
			}
		}
		index ^= t;
	}

	int kk = sizeof(index);

	hvalue.SetInt(0, (unsigned int)index);
	hvalue.SetInt(1, (unsigned int)(index >> 32));
}

/**
* \return real dimension of tuple. Real dimension means the all last zero coordinates are cutted.
*/
unsigned int cTuple::GetRealDimension() const
{
	return GetRealDimension(mSpaceDescriptor->GetDimension());
}

inline char* cTuple::GetData() const
{
	return mData;
}

inline unsigned int cTuple::GetSerialSize() const
{
	return mSpaceDescriptor->GetByteSize();
}

inline cSpaceDescriptor* cTuple::GetSpaceDescriptor() const
{
	return mSpaceDescriptor;
}


void cTuple::Format(cSpaceDescriptor *spaceDescriptor, cMemoryBlock* memory)
{
	mSpaceDescriptor = spaceDescriptor;
	mData = memory->GetMemory(spaceDescriptor->GetByteSize());
}

/**
* Set all bits of the tuple values to zero
*/
void cTuple::Clear()
{
	if (mSpaceDescriptor)
	{
		memset(mData, 0, mSpaceDescriptor->GetByteSize());
	}
}

/// Set min value in the order-th coordinate.
inline void cTuple::Clear(unsigned int order)
{
	switch (mSpaceDescriptor->GetType(order)->GetCode())
	{
	case cFloat::CODE:
		SetValue(order, (float)0.0);
		break;
	case cInt::CODE:
		SetValue(order, (int)0);
		break;
	case cUInt::CODE:
		SetValue(order, (unsigned int)0);
		break;
	case cUShort::CODE:
		SetValue(order, (unsigned short)0);
		break;
	case cShort::CODE:
		SetValue(order, (short)0);
		break;
	case cChar::CODE:
		SetValue(order, (char)0);
		break;
	}
}

/// Clear the all coordinates out of first.
void cTuple::ClearOther(unsigned int order)
{
	assert(order < mSpaceDescriptor->GetDimension());
	unsigned int index = mSpaceDescriptor->GetByteIndex(order);
	memset(mData + index, 0, mSpaceDescriptor->GetByteSize() - index);
}

/// Compare first k values from tuple with this tuple
/// \param start starting index of vector
///	\param k size of equal
/// \invariant k > 0
/// \invariant start + k <= dimension
/// \return true if the first k values in tuples are the same
inline bool cTuple::Equal(unsigned int _start, unsigned int k, const cTuple &tuple) const
{
	unsigned int size, start;
#ifndef NDEBUG
	assert(_start + k <= mSpaceDescriptor->GetDimension());
	assert(k > 0);
	if (!tuple.GetSpaceDescriptor())  // pm
	{
		assert(!mSpaceDescriptor);
		return false;
	}
	if (!mSpaceDescriptor || tuple.GetSpaceDescriptor()->GetDimension() != mSpaceDescriptor->GetDimension())
	{
		return false;
	}
#endif

	if ((_start + k) == mSpaceDescriptor->GetDimension())
	{
		start =  mSpaceDescriptor->GetByteIndex(_start);
		size = mSpaceDescriptor->GetByteSize() - start;
	} else
	{
		start =  mSpaceDescriptor->GetByteIndex(_start);
		size = mSpaceDescriptor->GetByteIndex(_start + k) - start;	
	}
	return memcmp(mData + start, tuple.GetData() + start, size) == 0;
}

/// Compare values from tuple with this tuple
/// \return true if tuples are the same
inline bool cTuple::operator == (const cTuple &tuple) const
{
#ifndef NDEBUG
	if (!tuple.GetSpaceDescriptor())  // pm
	{
		assert(!mSpaceDescriptor);
		return false;
	}
	if (!mSpaceDescriptor || tuple.GetSpaceDescriptor()->GetDimension() != mSpaceDescriptor->GetDimension())
	{
		return false;
	}
#endif

	return memcmp(mData, tuple.GetData(), mSpaceDescriptor->GetByteSize()) == 0;
}

/// Compare values from tuple with this tuple
/// \return true if tuples are diferent
inline bool cTuple::operator != (const cTuple &tuple) const
{
#ifndef NDEBUG
	if (!tuple.GetSpaceDescriptor())  // pm
	{
		assert(!mSpaceDescriptor);
		return true;
	}
	if (!mSpaceDescriptor || tuple.GetSpaceDescriptor()->GetDimension() != mSpaceDescriptor->GetDimension())
	{
		return true;
	}
#endif

	return memcmp(mData, tuple.GetData(), mSpaceDescriptor->GetByteSize()) != 0;
}

/// Compare values in this tuple and another tuple
/// \return true if this tuple is greater then the second tuple in all dimension
inline bool cTuple::operator > (const cTuple &tuple) const
{
	if (!tuple.GetSpaceDescriptor())  // pm
	{
		assert(!mSpaceDescriptor);
		return true;
	}
	if (!mSpaceDescriptor || tuple.GetSpaceDescriptor()->GetDimension() != mSpaceDescriptor->GetDimension())
	{
		return false;
	}

	for (unsigned int i = 0; i < tuple.GetSpaceDescriptor()->GetDimension(); i++)
	{
		if (Equal(tuple,i) == -1)
			return false;
	}

	return true;
}

/// Compare the tuples from the first dimension until values in dimension are different.
/// \return true if the value in the first different dimension is bigger
inline bool cTuple::Greater(const cTuple &tuple) const
{
	if (!tuple.GetSpaceDescriptor())  // pm
	{
		assert(!mSpaceDescriptor);
		return true;
	}
	if (!mSpaceDescriptor || tuple.GetSpaceDescriptor()->GetDimension() != mSpaceDescriptor->GetDimension())
	{
		return false;
	}

	for (unsigned int i = 0; i < tuple.GetSpaceDescriptor()->GetDimension(); i++)
	{
		if (Equal(tuple,i) != 0)
		{
			if (Equal(tuple,i) == -1)
			{
				return false;
			} else
			{
				return true;
			}
		}
	}

	return false;
}

/// Byte comparison between tuples. Use memcmp function
/// \param tuple 
/// \return
///		- -1 if the this tuple is smaller than the parameter
///		- 0 if the tupleas are the same
///		- 1 if the parameter is bigger than this tuple
inline int cTuple::Compare(const cTuple &tuple) const
{
	return memcmp((void*)mData, (void*)tuple.GetData(), mSpaceDescriptor->GetByteSize());
}

/// Copy only the pointer address!! Rewrite the pointers in this tuple by pointers in the parameter tuple. 
/// This method can even lead to heap error during the delete phase, because you will try to free the same memory twice.
inline void cTuple::Copy(const cTuple &tuple)
{
	mData = tuple.GetData();
	mSpaceDescriptor = tuple.GetSpaceDescriptor();
}

/// Set the float value to the dimension specified by the order parameter
/// \param order Dimension whose value should be set
/// \param value New value of the dimension
/// \invariant order < tuple dimension
/// \invariant value in dimension has to be float
void cTuple::SetValue(unsigned int order, float value)
{
	assert(order < mSpaceDescriptor->GetDimension());
	assert(mSpaceDescriptor->GetType(order)->GetCode() == 'f');
	*(float*)(mData + mSpaceDescriptor->GetByteIndex(order)) = value;
}

/// Set the double value to the dimension specified by the order parameter
/// \param order Dimension whose value should be set
/// \param value New value of the dimension
/// \invariant order < tuple dimension
/// \invariant value in dimension has to be float
void cTuple::SetValue(unsigned int order, double value)
{
	assert(order < mSpaceDescriptor->GetDimension());
	assert(mSpaceDescriptor->GetType(order)->GetCode() == 'f');
	*(double*)(mData + mSpaceDescriptor->GetByteIndex(order)) = value;
}

/// Set the int value of the dimension specified by the order parameter
/// \param order Dimension whose value should be set
/// \param value New value of the dimension
/// \invariant order < tuple dimension
/// \invariant value in dimension has to be int
void cTuple::SetValue(unsigned int order, int value)
{
	assert(order < mSpaceDescriptor->GetDimension());
	assert(mSpaceDescriptor->GetType(order)->GetCode() == 'i');
	*(int*)(mData + mSpaceDescriptor->GetByteIndex(order)) = value;
}

/// Set the unsigned int value of the dimension specified by the order parameter
/// \param order Dimension whose value should be set
/// \param value New value of the dimension
/// \invariant order < tuple dimension
/// \invariant value in dimension has to be unsigned int
void cTuple::SetValue(unsigned int order, unsigned int value)
{
	unsigned int dimension = mSpaceDescriptor->GetDimension();
	assert(order < mSpaceDescriptor->GetDimension());
	assert(mSpaceDescriptor->GetType(order)->GetCode() == 'u');
	*(unsigned int*)(mData + mSpaceDescriptor->GetByteIndex(order)) = value;
}

/// Set the byte value of the dimension specified by the order parameter
/// \param order Dimension whose value should be set
/// \param value New value of the dimension
/// \invariant order < tuple dimension
void cTuple::SetValue(unsigned int order, char value)
{
	assert(order < mSpaceDescriptor->GetDimension());
	assert(mSpaceDescriptor->GetType(order)->GetCode() == 'c');
	*(mData + mSpaceDescriptor->GetByteIndex(order)) = value;
}

/// Set the byte value of the dimension specified by the order parameter
/// \param order Dimension whose value should be set
/// \param value New value of the dimension
/// \invariant order < tuple dimension
void cTuple::SetValue(unsigned int order, unsigned char value)
{
	assert(order < mSpaceDescriptor->GetDimension());
	assert(mSpaceDescriptor->GetType(order)->GetCode() == 'c');
	*(mData + mSpaceDescriptor->GetByteIndex(order)) = value;
}

/// Set the unicode char value of the dimension specified by the order parameter
/// \param order Dimension whose value should be set
/// \param value New value of the dimension
/// \invariant order < tuple dimension
void cTuple::SetValue(unsigned int order, wchar_t value)
{
	assert(order < mSpaceDescriptor->GetDimension());
	assert(mSpaceDescriptor->GetType(order)->GetCode() == 'w');
	*((wchar_t*)(mData + mSpaceDescriptor->GetByteIndex(order))) = value;
}

/// Set the short value of the dimension specified by the order parameter
/// \param order Dimension whose value should be set
/// \param value New value of the dimension
/// \invariant order < tuple dimension
void cTuple::SetValue(unsigned int order, short value)
{
	assert(order < mSpaceDescriptor->GetDimension());
	assert(mSpaceDescriptor->GetType(order)->GetCode() == cShort::CODE);
	*(short*)(mData + mSpaceDescriptor->GetByteIndex(order)) = value;
}

/// Set the unsigned short value of the dimension specified by the order parameter
/// \param order Dimension whose value should be set
/// \param value New value of the dimension
/// \invariant order < tuple dimension
void cTuple::SetValue(unsigned int order, unsigned short value)
{
	assert(order < mSpaceDescriptor->GetDimension());
	assert(mSpaceDescriptor->GetType(order)->GetCode() == cUShort::CODE);
	*(unsigned short*)(mData + mSpaceDescriptor->GetByteIndex(order)) = value;
}

/// Return the float value of the dimension specified by the order parameter
/// \param order Dimension whose value should be returned
/// \return float value of the dimension
/// \invariant order < tuple dimension
/// \invariant value type in the dimension has to be float
inline float cTuple::GetFloat(unsigned int order) const
{
	assert(order < mSpaceDescriptor->GetDimension());
	assert(mSpaceDescriptor->GetType(order)->GetCode() == 'f');
	return *(float*)(mData + mSpaceDescriptor->GetByteIndex(order));
}

/// Return the double value of the dimension specified by the order parameter
/// \param order Dimension whose value should be returned
/// \return double value of the dimension
/// \invariant order < tuple dimension
/// \invariant value type in the dimension has to be float
inline double cTuple::GetDouble(unsigned int order) const
{
	assert(order < mSpaceDescriptor->GetDimension());
	assert(mSpaceDescriptor->GetType(order)->GetCode() == 'f');
	return *(double*)(mData + mSpaceDescriptor->GetByteIndex(order));
}

/// Return the int value of the dimension specified by the order parameter
/// \param order Dimension whose value should be returned
/// \return int value of the dimension
/// \invariant order < tuple dimension
/// \invariant value type in the dimension has to be int
inline int cTuple::GetInt(unsigned int order) const
{
	assert(order < mSpaceDescriptor->GetDimension());
	assert(mSpaceDescriptor->GetType(order)->GetCode() == 'i');
	return *(int*)(mData + mSpaceDescriptor->GetByteIndex(order));
}

/// Return the unsigned int value of the dimension specified by the order parameter
/// \param order Dimension whose value should be returned
/// \return unsigned int value of the dimension
/// \invariant order < tuple dimension
/// \invariant value type in the dimension has to be unsigned int
inline unsigned int cTuple::GetUInt(unsigned int order) const
{
	assert(order < mSpaceDescriptor->GetDimension());
	assert(mSpaceDescriptor->GetType(order)->GetCode() == 'u');
	return *(unsigned int*)(mData + mSpaceDescriptor->GetByteIndex(order));
}

/// Return the byte value of the dimension specified by the order parameter
/// \param order Dimension whose value should be returned
/// \return byte value of the dimension
/// \invariant order < tuple dimension
/// \invariant value type in the dimension has to be char (byte)
inline char cTuple::GetByte(unsigned int order) const
{
	assert(order < mSpaceDescriptor->GetDimension());
	assert(mSpaceDescriptor->GetType(order)->GetCode() == 'c');
	return *(mData + mSpaceDescriptor->GetByteIndex(order));
}

/// Return the byte value of the dimension specified by the order parameter
/// \param order Dimension whose value should be returned
/// \return byte value of the dimension
/// \invariant order < tuple dimension
/// \invariant value type in the dimension has to be char (byte)
inline unsigned char cTuple::GetUChar(unsigned int order) const
{
	assert(order < mSpaceDescriptor->GetDimension());
	assert(mSpaceDescriptor->GetType(order)->GetCode() == 'c');
	return *(unsigned char*)(mData + mSpaceDescriptor->GetByteIndex(order));
}

/// Return the unicode char value of the dimension specified by the order parameter
/// \param order Dimension whose value should be returned
/// \return byte value of the dimension
/// \invariant order < tuple dimension
/// \invariant value type in the dimension has to be char (byte)
inline wchar_t cTuple::GetWChar(unsigned int order) const
{
	assert(order < mSpaceDescriptor->GetDimension());
	assert(mSpaceDescriptor->GetType(order)->GetCode() == 'w');
	return *(mData + mSpaceDescriptor->GetByteIndex(order));
}

/// Return the short value of the dimension specified by the order parameter
/// \param order Dimension whose value should be returned
/// \return short value of the dimension
/// \invariant order < tuple dimension
/// \invariant value type in the dimension has to be short
inline short cTuple::GetShort(unsigned int order) const
{
	assert(order < mSpaceDescriptor->GetDimension());
	assert(mSpaceDescriptor->GetType(order)->GetCode() == cShort::CODE);
	return *(short*)(mData + mSpaceDescriptor->GetByteIndex(order));
}

/// Return the short value of the dimension specified by the order parameter
/// \param order Dimension whose value should be returned
/// \return short value of the dimension
/// \invariant order < tuple dimension
/// \invariant value type in the dimension has to be short
inline unsigned short cTuple::GetUShort(unsigned int order) const
{
	assert(order < mSpaceDescriptor->GetDimension());
	assert(mSpaceDescriptor->GetType(order)->GetCode() == cUShort::CODE);
	return *(short*)(mData + mSpaceDescriptor->GetByteIndex(order));
}

/// Return the cString value of the dimension
/// \param order Dimension whose value should be returned
/// \param string returned value
inline void cTuple::GetString(unsigned int order, cString &string) const
{
	const int STRING_LENGTH = 128;
	char str[STRING_LENGTH];
	unsigned int j;

	assert(order < mSpaceDescriptor->GetDimension());

	switch (mSpaceDescriptor->GetType(order)->GetCode())
	{
	case cFloat::CODE:
		sprintf_s((char*)str, STRING_LENGTH, "%f", GetFloat(order));
		break;
	case cInt::CODE:
		sprintf_s((char*)str, STRING_LENGTH, "%d", GetInt(order));
		break;
	case cShort::CODE:
		sprintf_s((char*)str, STRING_LENGTH, "%d", GetShort(order));
		break;
	case cUInt::CODE:
		j = GetUInt(order);
		sprintf_s((char*)str, STRING_LENGTH, "%u", GetUInt(order));
		break;
	case cUShort::CODE:
		j = GetUShort(order);
		sprintf_s((char*)str, STRING_LENGTH, "%u", GetUShort(order));
		break;
	case cChar::CODE:
		sprintf_s((char*)str, STRING_LENGTH, "%c", this->GetByte(order));
		break;
	}
	string += (char*)str;
}

/// Return the float value of the dimension specified by the order parameter by reference
/// \param order Dimension whose value should be returned
/// \return float value of the dimension by reference
/// \invariant order < tuple dimension
/// \invariant value type in the dimension has to be float
inline float* cTuple::GetPFloat(unsigned int order) const
{
	assert(order < mSpaceDescriptor->GetDimension());
	assert(mSpaceDescriptor->GetType(order)->GetCode() == 'f');
	return (float*)(mData + mSpaceDescriptor->GetByteIndex(order));
}

/// Return the int value of the dimension specified by the order parameter by reference
/// \param order Dimension whose value should be returned
/// \return int value of the dimension by reference
/// \invariant order < tuple dimension
/// \invariant value type in the dimension has to be int
inline int* cTuple::GetPInt(unsigned int order) const
{
	assert(order < mSpaceDescriptor->GetDimension());
	assert(mSpaceDescriptor->GetType(order)->GetCode() == 'i');
	return (int*)(mData + mSpaceDescriptor->GetByteIndex(order));
}

/// Return the unsigned int value of the dimension specified by the order parameter by reference
/// \param order Dimension whose value should be returned
/// \return unsigned int value of the dimension by reference
/// \invariant order < tuple dimension
/// \invariant value type in the dimension has to be unsigned int
inline unsigned int* cTuple::GetPUInt(unsigned int order) const
{
	assert(order < mSpaceDescriptor->GetDimension());
	assert(mSpaceDescriptor->GetType(order)->GetCode() == 'u');
	return (unsigned int*)(mData + mSpaceDescriptor->GetByteIndex(order));
}

/// Return the byte value of the dimension specified by the order parameter by reference
/// \param order Dimension whose value should be returned
/// \return byte value of the dimension by reference
/// \invariant order < tuple dimension
/// \invariant value type in the dimension has to be char (byte)
inline char* cTuple::GetPByte(unsigned int order) const
{
	assert(order < mSpaceDescriptor->GetDimension());
	assert(mSpaceDescriptor->GetType(order)->GetCode() == 'c');
	return (mData + mSpaceDescriptor->GetByteIndex(order));
}

/// Return the byte value of the dimension specified by the order parameter by reference
/// \param order Dimension whose value should be returned
/// \return byte value of the dimension by reference
/// \invariant order < tuple dimension
/// \invariant value type in the dimension has to be char (byte)
inline unsigned char* cTuple::GetPUChar(unsigned int order) const
{
	assert(order < mSpaceDescriptor->GetDimension());
	assert(mSpaceDescriptor->GetType(order)->GetCode() == 'c');
	return (unsigned char*)(mData + mSpaceDescriptor->GetByteIndex(order));
}

/// Read tuple from stream.
inline bool cTuple::Read(cStream *stream)
{
	return stream->Read(mData, mSpaceDescriptor->GetByteSize());
}

/// Write tuple into stream.
inline bool cTuple::Write(cStream *stream) const
{
	return stream->Write(mData, mSpaceDescriptor->GetByteSize());
}

/// Set max value in the order-th coordinate.
inline void cTuple::SetMaxValue(unsigned int order)
{
	switch (mSpaceDescriptor->GetType(order)->GetCode())
	{
	case cFloat::CODE:
		SetValue(order, cFloat::MAX);
		break;
	case cInt::CODE:
		SetValue(order, cInt::MAX);
		break;
	case cUInt::CODE:
		SetValue(order, cUInt::MAX);
		break;
	case cUShort::CODE:
		SetValue(order, cUShort::MAX);
		break;
	case cShort::CODE:
		SetValue(order, cShort::MAX);
		break;
	case cChar::CODE:
		SetValue(order, (char)cChar::MAX);
		break;
	case cWChar::CODE:
		SetValue(order, (wchar_t)cWChar::MAX);
		break;
	}
}

/// Compare z-values.
/// \return
///		- -1 if the this < tuple
///		- 0 if the this == tuple
///		- 1 if the this > tuple
int cTuple::CompareZOrder(const cTuple& tuple) const
{
	return mSpaceDescriptor->CompareNormalizedZValues(*this, tuple);
}

/// Compare hilbert values.
/// \return
///		- -1 if the this < tuple
///		- 0 if the this == tuple
///		- 1 if the this > tuple
int cTuple::CompareHOrder(const cTuple& tuple) const
{
	return mSpaceDescriptor->CompareNormalizedHValues(*this, tuple);
}

/// Compare taxi ordering values (from the beginning of the space).
/// \return
///		- -1 if the this < tuple
///		- 0 if the this == tuple
///		- 1 if the this > tuple
inline int cTuple::CompareTaxiOrder(const cTuple& tuple) const
{
	double d1 = ComputeTaxiDistance();
	double d2 = tuple.ComputeTaxiDistance();
	int res = 0;

	if (d1 < d2)
	{
		res = -1;
	}
	else if (d1 > d2)
	{
		res = 1;
	}

	return res;
}

/// Compute Taxi distance (from the beginning of the space).
/// \return distance
inline double cTuple::ComputeTaxiDistance() const
{
	double distance = 0.0;
	for (unsigned int i = 0 ; i < mSpaceDescriptor->GetDimension() ; i++)
	{
		assert(mSpaceDescriptor->GetType(i)->GetCode() == cUInt::CODE);
		distance += (double)GetUInt(i) / mSpaceDescriptor->GetMaxValue()->GetUInt(i);
	}
	return distance;
}

/// Compare the tuple values in every dimension starting from first until values in dimensions are different.
/// \return the return value correponds to the first different dimension in tuples. 
///		- -1 if this tuple has lower value
///		- 0 if the tuples are the same
///		- 1 if this tuple is bigger
inline int cTuple::Equal(const cTuple &tuple) const
{
	int eq; 

	assert(GetSpaceDescriptor());
	assert(tuple.GetSpaceDescriptor());
	assert(mSpaceDescriptor->GetDimension() == tuple.GetSpaceDescriptor()->GetDimension());

	for (unsigned int i = 0; i < mSpaceDescriptor->GetDimension(); i++)
	{
		if ((eq = cTuple::Equal(tuple,i)) != 0)
		{
			return eq;
		}
	}

	return 0;
}


/**
* Return real dimension of tuple. Real means the all zero last coordinates are cutted.
* \param high sets the highest coordinates where zeros are searched
*/
unsigned int cTuple::GetRealDimension(unsigned int high) const
{
	int lo = 0, hi = high - 1;
	unsigned int realdim = 0;
	for ( ; ; )
	{
		int index = (lo + hi)/2;
		if (IsZero(index))
		{
			if (index > 0 && !IsZero(index-1))
			{
				realdim = index;
				break;
			}
			else
			{
				hi = index - 1;

				if (lo > hi)
				{
					realdim = index;
					break;
				}
			}
		}
		else
		{
			lo = index + 1;
			if (lo > hi)
			{
				realdim = index + 1;
				break;
			}
		}
	}
	return realdim;
}

/** 
* \return true when all bytes of mData are zero.
*/
inline bool cTuple::IsZero() const
{
	for (unsigned int i = 0; i < mSpaceDescriptor->GetByteSize(); i++)
	{
		if (mData[i] != 0)
		{
			return false;
		}
	}
	return true;
}

/**
* Test for minimal value.
* \param order Specify order of the item within this tuple.
* \return true when the item is zero.
*/
inline bool cTuple::IsZero(unsigned int order) const
{
	bool ret = false;
	switch (mSpaceDescriptor->GetType(order)->GetCode())
	{
	case cFloat::CODE:
		ret = (GetFloat(order) == (float)0.0);
		break;
	case cInt::CODE:
		ret = (GetInt(order) == (int)0);
		break;
	case cUInt::CODE:
		ret = (GetUInt(order) == (unsigned int)0);
		break;
	case cUShort::CODE:
		ret = (GetUInt(order) == (unsigned short)0);
		break;
	case cShort::CODE:
		ret = (GetUInt(order) == (short)0);
		break;
	case cChar::CODE:
		ret = (GetByte(order) == (char)0);
		break;
	}
	return ret;
}
}}}