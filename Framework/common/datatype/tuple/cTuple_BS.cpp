/**************************************************************************}
{                                                                          }
{    cTuple_BS.cpp                                                            }
{                                                                          }
{                                                                          }
{    Copyright (c) 2001                      Michal Kratky                 }
{                                                                          }
{    VERSION: 0.2                            DATE 5/11/2001                }
{                                                                          }
{    following functionality:                                              }
{      Tuple - point in n-dimensional space.                               }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      03/02/2002                                                          }
{      01/02/2003 - remove foreign hilbert code                            }
{                                                                          }
{**************************************************************************/

#include "cTuple_BS.h"

namespace common {
	namespace datatype {
		namespace tuple {

bool cTuple_BS::mDebug = false;

cTuple_BS::cTuple_BS(): mValues(NULL),mSpaceDescriptor(NULL)
{
}

cTuple_BS::cTuple_BS(cSpaceDescriptor_BS *spaceDescriptor)
{
	mValues = new cArray<cBitString>;
	Resize(spaceDescriptor);
	Clear();
}

cTuple_BS::~cTuple_BS()
{
	mValues->Resize(0);
	delete mValues;
}

/**
 * Warnning: Don't use the metod by normal situation. Define for cMemory allocation purpose. Substitute empty constructor.
 */
void cTuple_BS::Init()
{
	mValues = NULL;
	mSpaceDescriptor = NULL;
}

void cTuple_BS::SetValue(unsigned int dimension, const cBitString &value)
{
	if (dimension < mSpaceDescriptor->GetDimension()) 
	{
		mValues->GetItem(dimension)->SetBitString(value);
	}
}

/**
 * Set tuple. Dimension may be different.
 */
void cTuple_BS::SetTuple(const cTuple_BS &tuple)
{
	for (unsigned int i = 0 ; i < mSpaceDescriptor->GetDimension() ; i++)
	{
		if (i < tuple.GetSpaceDescriptor()->GetDimension())
		{
			SetValue(i, tuple.GetRefValue(i));
		}
	}
}

void cTuple_BS::Resize(cSpaceDescriptor_BS *spaceDescriptor)
{
	mSpaceDescriptor = spaceDescriptor;
	if (mValues == NULL)
	{
		mValues = new cArray<cBitString>;
	}
	mValues->Resize(mSpaceDescriptor->GetDimension());
	mValues->SetCount(mSpaceDescriptor->GetDimension());

	for (unsigned int i = 0; i < mSpaceDescriptor->GetDimension() ; i++)
	{
		mValues->GetItem(i)->Resize(spaceDescriptor->GetBitSize(i));
	}
}

/**
 * Use cMemory for allocation.
 */
void cTuple_BS::Resize(cSpaceDescriptor_BS *spaceDescriptor, cMemory *memory)
{
	mSpaceDescriptor = spaceDescriptor;
	if (mValues == NULL)
	{
		mValues = (cArray<cBitString>*)memory->GetMemory(sizeof(cArray<cBitString>));
	}

	mValues->Resize(mSpaceDescriptor->GetDimension(), memory->GetMemory(mSpaceDescriptor->GetDimension() * mValues->GetItemSize()));
	mValues->SetCount(mSpaceDescriptor->GetDimension());

	for (unsigned int i = 0; i < mSpaceDescriptor->GetDimension() ; i++)
	{
		mValues->GetItem(i)->Resize(spaceDescriptor->GetBitSize(i), memory);
	}
}

/**
 * Calculation tuple's address.
 **/
void cTuple_BS::Address(cBitString &address) const
{
	address.Clear();        // !!! Bug, get, set length x realLength !!!
	switch (mSpaceDescriptor->GetAddressType()) {
	case cSpaceDescriptor_BS::ADDRESS_C:
			AddressC(address);
			break;
	/*case cSpaceDescriptor_BS::ADDRESS_H:
			AddressH(address);
			break; */
	case cSpaceDescriptor_BS::ADDRESS_Z:
	default:
      AddressZ(address);
			break;
	}
}

void cTuple_BS::AddressZ(cBitString &address) const
{
	unsigned int maxBitSize = mSpaceDescriptor->GetMaxBitSize();
	unsigned int count = 0, currlen = 0;

	// reset current bit
	address.SetHighestBitAsCurrent();
	for (unsigned int k = 0 ; k < mSpaceDescriptor->GetDimension() ; k++)
	{
		GetRefValue(k).SetHighestBitAsCurrent();
	}

	for (int i = maxBitSize-1 ; i >= 0 ; i--)
	{
		for (int j = mSpaceDescriptor->GetDimension()-1 ; j >= 0 ; j--)
		{
			currlen = GetRefValue(j).GetLength();
			if (count < currlen)
			{
				address.SetPreviousBit(GetRefValue(j).GetPreviousBit());
			}
		}
		count++;
	}
}

void cTuple_BS::AddressC(cBitString &address) const
{
	int width = mSpaceDescriptor->GetMaxValue(0)+1;
	address.SetInt(width * GetRefValue(1).GetInt() + GetRefValue(0).GetInt());
}

/*void cTuple_BS::AddressH(cBitString &address)
{
	address.SetInt(hilbert_c2i());
}*/

/**
 * Setting the cordinates of tuple according to address.
 **/
void cTuple_BS::SetValues(const cBitString &address)
{
	switch (mSpaceDescriptor->GetAddressType())
	{
	case cSpaceDescriptor_BS::ADDRESS_C:
			SetValuesC(address);
			break;
	/*case cSpaceDescriptor_BS::ADDRESS_H:
			SetValuesH(address);
			break;*/
	case cSpaceDescriptor_BS::ADDRESS_Z:
	default:
			SetValuesZ(address);
			break;
	}
}

/**
 * Clear whole tuple.
 */
void cTuple_BS::Clear()
{
	for (unsigned int i = 0 ; i < mSpaceDescriptor->GetDimension() ; i++)
	{
		mValues->GetItem(i)->Clear();
	}
}

/**
 * Set max values of tuple attribute values.
 */
void cTuple_BS::SetMaxValues()
{
	for (unsigned int i = 0 ; i < mSpaceDescriptor->GetDimension() ; i++)
	{
		mValues->GetItem(i)->SetMaxValue();
	}
}

/**
 * Clear the all coordinates out of first.
 */
void cTuple_BS::ClearOther(unsigned int number)
{
	for (unsigned int i = number ; i < mSpaceDescriptor->GetDimension() ; i++)
	{
		mValues->GetItem(i)->Clear();
	}
}

void cTuple_BS::SetValuesZ(const cBitString &address)
{
	unsigned int maxBitSize = mSpaceDescriptor->GetMaxBitSize();
	unsigned int currlen = 0, count = 0, index = address.GetLength();

	// reset current bit
	for (unsigned int k = 0 ; k < mSpaceDescriptor->GetDimension() ; k++)
	{
		GetValue(k)->SetHighestBitAsCurrent();
	}

	for (int i = maxBitSize-1 ; i >= 0 ; i--)
	{
		for (int j = mSpaceDescriptor->GetDimension()-1 ; j >= 0 ; j--)
		{
			currlen = GetValue(j)->GetLength();
			if (count < currlen)
			{
				GetValue(j)->SetPreviousBit(address.GetBit(--index));
			}
		}
		count++;
	}
}

/**
 * Only for 32 bit.
 */
void cTuple_BS::SetValuesC(const cBitString &address)
{
	int width = mSpaceDescriptor->GetMaxValue(0)+1;
	GetValue(1)->SetInt(address.GetInt()/width);
	GetValue(0)->SetInt(address.GetInt()%width);
}

/*void cTuple_BS::SetValuesH(const cBitString &address)
{
	// bitmask_t index
	hilbert_i2c(address.GetInt(), address);
}*/

/**
 * Return real dimension of tuple. Real means the all zero last coordinates are cutted.
 */
unsigned int cTuple_BS::GetRealDimension() const
{
	int lo = 0, hi = mSpaceDescriptor->GetDimension()-1;
	unsigned int realdim2 = 0;
	for ( ; ; )
	{
		int index = (lo + hi)/2;
		if (GetRefValue(index).IsZero())
		{
			if (!GetRefValue(index-1).IsZero())
			{
				realdim2 = index;
				break;
			}
			else
			{
				hi = index - 1;

				if (lo > hi)
				{
					realdim2 = index;
					break;
				}
			}
		}
		else
		{
			lo = index + 1;
			if (lo > hi)
			{
				realdim2 = index + 1;
				break;
			}
		}
	}

	/*unsigned int realdim = 0;
	for (int i = mSpaceDescriptor->GetDimension()-1 ; i >= 0 ; i--)
	{
		if (!GetRefValue(i).IsZero())
		{
			realdim = i+1;
			break;
		}
	}

	if (realdim2 != realdim)
	{
		printf("Critical Error: %d, %d!\n", realdim2, realdim);
	}*/

	return realdim2;
}

unsigned int cTuple_BS::GetSerialSize() const
{
	unsigned int serSize = 0;

	if (mSpaceDescriptor != NULL)
	{
		for (unsigned int i = 0 ; i < mSpaceDescriptor->GetDimension() ; i++) //pm - bylo: < mSpaceDescriptor->GetDimension() - 1
		{
			serSize += GetRefValue(i).GetSerialSize();
		}
	}
	return serSize;
}

/**
 * Return true if the tuple is into n-dimensional query block.
 **/
bool cTuple_BS::IsInBlock(const cTuple_BS &ql, const cTuple_BS &qh) const
{
	bool ret = true;
	for (unsigned int i = 0 ; i < mSpaceDescriptor->GetDimension() ; i++)
	{
		if (GetRefValue(i) >= ql.GetRefValue(i))
		{
			if (GetRefValue(i) > qh.GetRefValue(i))
			{
				ret = false;
				break;
			}
		}
		else
		{
			if (GetRefValue(i) < qh.GetRefValue(i))
			{
				ret = false;
				break;
			}
		}
	}
	return ret;
}

/**
 * Compute euclidian metric of two tuples.
 */
float cTuple_BS::EuclidianDistance(const cTuple_BS &tuple1, const cTuple_BS &tuple2)
{
	float distance = 0;

	for (unsigned int i = 0 ; i < tuple1.GetSpaceDescriptor()->GetDimension() ; i++)
	{
		int tmp = (int)tuple1.GetRefValue(i).GetInt(0) - (int)tuple2.GetRefValue(i).GetInt(0); // rb - was unsigned int
		distance += tmp * tmp;
	}
	return sqrt(distance);
}

/**
 * Modify mbr according to tuple.
 **/
void cTuple_BS::ModifyMbr(cTuple_BS &mbrl, cTuple_BS &mbrh) const
{
	for (unsigned int i = 0 ; i < mSpaceDescriptor->GetDimension() ; i++)
	{
		if (mbrl.GetRefValue(i) <=  mbrh.GetRefValue(i))
		{
			if (mbrl.GetRefValue(i) > GetRefValue(i))
			{
				mbrl.SetValue(i, GetRefValue(i));
			}
			else if (mbrh.GetRefValue(i) < GetRefValue(i))
			{
				mbrh.SetValue(i, GetRefValue(i));
			}
		}
		else
		{
			if (mbrh.GetRefValue(i) > GetRefValue(i))
			{
				mbrh.SetValue(i, GetRefValue(i));
			}
			else if (mbrl.GetRefValue(i) < GetRefValue(i))
			{
				mbrl.SetValue(i, GetRefValue(i));
			}
		}
	}
}

/**
 * Read tuple from stream.
 */
bool cTuple_BS::Read(cStream *stream)
{
	for (unsigned int i = 0 ; i < mSpaceDescriptor->GetDimension() ; i++)
	{
		if (!GetValue(i)->Read(stream))
		{
			return false;
		}
	}
	return true;
}

/**
 * Write tuple into stream.
 */
bool cTuple_BS::Write(cStream *stream) const
{
	for (unsigned int i = 0 ; i < mSpaceDescriptor->GetDimension() ; i++)
	{
		if (!GetRefValue(i).Write(stream))
		{
			return false;
		}
	}
	return true;
}

/**
 * Operator = then copy contents of bString into this.
 **/
void cTuple_BS::operator = (const cTuple_BS &tuple)
{
	if (!tuple.GetSpaceDescriptor())  //pm
	{
		assert(!mSpaceDescriptor);
		return;
	} 
	else if (!mSpaceDescriptor || tuple.GetSpaceDescriptor()->GetDimension() != mSpaceDescriptor->GetDimension())
	{
		Resize(tuple.GetSpaceDescriptor());
	}

	for (unsigned int i = 0 ; i < mSpaceDescriptor->GetDimension() ; i++)
	{
		GetValue(i)->SetBitString(tuple.GetRefValue(i));
	}
}

/**
 * Equal, return 0 if the instance are the same.
 **/
int cTuple_BS::Equal(const cTuple_BS &tuple) const
{
	int ret = 1;
	if (*this == tuple)
	{
		ret = 0;
	}
	return ret;
}

/**
 * Operator == compare bitstrings of two tuples.
 **/
bool cTuple_BS::operator == (const cTuple_BS &tuple) const
{
	if (!tuple.GetSpaceDescriptor())  //pm
	{
		assert(!mSpaceDescriptor);
		return false;
	} 
	else if (!mSpaceDescriptor || tuple.GetSpaceDescriptor()->GetDimension() != mSpaceDescriptor->GetDimension())
	{
		return false;
	}

	for (unsigned int i = 0 ; i < mSpaceDescriptor->GetDimension() ; i++)
	{
		if (GetRefValue(i) != tuple.GetRefValue(i)) return false;
	}
	return true;
}

/**
 * Operator != compare bitstrings of two tuples.
 **/
bool cTuple_BS::operator != (const cTuple_BS &tuple)
{
	if (!tuple.GetSpaceDescriptor())  //pm
	{
		assert(!mSpaceDescriptor);
		return false;
	} 
	else if (!mSpaceDescriptor || tuple.GetSpaceDescriptor()->GetDimension() != mSpaceDescriptor->GetDimension())
	{
		return true;
	}

	for (unsigned int i = 0 ; i < mSpaceDescriptor->GetDimension() ; i++)
	{
		if (GetRefValue(i) != tuple.GetRefValue(i)) return true;
	}
	return false;
}

void cTuple_BS::Print(int mode, char *str) const
{
	printf("(");
	for (unsigned int i = 0 ; i < mSpaceDescriptor->GetDimension() ; i++)
	{
		if (mode != cObject::MODE_CHAR)
		{
			GetRefValue(i).Print(mode, "");
		}
		else
		{
			printf("%c", GetRefValue(i).GetByte(0));
		}

		if (i != mSpaceDescriptor->GetDimension()-1)
		{
			if (mode != cObject::MODE_CHAR)
			{
				printf(",");
			}
		}
	}
	printf(")%s", str);
}
}}}