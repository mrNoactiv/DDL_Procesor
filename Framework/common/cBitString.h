/**************************************************************************}
{                                                                          }
{    cBitString.h                                                          }
{                                                                          }
{                                                                          }
{    Copyright (c) 2001                      Michal Kratky                 }
{                                                                          }
{    VERSION: 0.2                            DATE 7/10/2001                }
{                                                                          }
{    following functionality:                                              }
{       bit string                                                         }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      11/2/2002                                                           }
{                                                                          }
{**************************************************************************/

#ifndef __cBitString_h__
#define __cBitString_h__

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "common/memorystructures/cArray.h"
#include "common/stream/cStream.h"
#include "common/cString.h"
#include "common/cNumber.h"
#include "common/cMemory.h"
#include "common/stream/cCharStream.h"

typedef unsigned int cBitString_item;

using namespace common::stream;

class cBitString
{
protected:
	cArray<cBitString_item> mNumber;  // bit string as field of chars
	unsigned int mCurrentBit;         // for GetNextBit()
	unsigned int mLength;             // length in bits
	unsigned int mByteLength;
	unsigned int mItemLength;
	unsigned int mMask;               // because bit length may be < item length * ItemBitLength

	static const bool mDebug = false;

	static const int DefaultValue = 0;
	static const int ItemBitLength = 32;
	static const int ItemByteLength = 4;

	// precalculation values
	static const unsigned int Const_1_31 = 2147483648;    // 1 << 31, 2^31
	static const ullong Const_1_32 = 4294967296;          // 1 << 32, 2^32
	static const unsigned int Const_1_32_1 = 4294967295;  // 2^32-1 - maximum 32bit number
	static const cBitString_item Const_ItemMaxValue = 4294967295;

	void Calc(unsigned int index, unsigned int *order, unsigned int **current, unsigned int *cindex) const;
	void SetMask();
	inline unsigned int GetUInt(unsigned int itemOrder, unsigned int loIndex, unsigned int hiIndex) const;
	
protected:
	void Copy(const cBitString &bString);

public:
	static const unsigned int SizeOfByte = 8;
	static const unsigned int SizeOfChar = 8;

public:
	cBitString();
	cBitString(unsigned int bitLength);
	cBitString(unsigned int bitLength, unsigned int number);
	cBitString(const cBitString &bString);
	~cBitString();

	void SetBit(unsigned int index, bool value);
	inline void SetByte(unsigned int index, unsigned char value);
	inline void SetByte(unsigned char value);
	void SetInt(unsigned int index, unsigned int value);
	void SetInt(unsigned int value);
	void SetBitString(const cBitString &bString);
	void SetString(char *string, unsigned int length);
	void SetMaxValue();
	void SetHighestBitAsCurrent();
	void SetLowestBitAsCurrent();
	bool SetPreviousBit(bool value);
	bool SetNextBit(bool value);
	void Add(unsigned int value);
	void Sub(unsigned int value);
	void Increment();
	void Decrement();
	void Clear();
	void GenerateRandom();

	inline unsigned int GetLength() const;
	inline unsigned int GetByteLength() const;
	inline unsigned int GetIntLength() const;
	inline unsigned int GetSerialSize() const;

	inline unsigned int GetCapacity() const;
	inline unsigned int GetByteCapacity() const;
	inline unsigned int GetIntCapacity() const;
	unsigned int CalculateByteLength() const;
	unsigned int CalculateIntLength() const;

	bool GetBit(unsigned int index) const;
	inline unsigned char GetByte(unsigned int index) const;
	inline unsigned char GetByte() const;
	inline unsigned int GetInt(unsigned int index) const;
	inline unsigned int GetUInt(unsigned int loIndex, unsigned int hiIndex) const;
	inline unsigned int GetInt() const;
	void GetString(cString &string);
	unsigned int GetValue(unsigned int first, unsigned int last) const;
	bool GetPreviousBit();
	bool GetNextBit();

	void Resize(unsigned int bitLength);
	void Resize(unsigned int bitLength, cMemory *memory);
	void Resize(const cBitString &bString);

	bool Read(cStream *stream);
	bool Read(cStream *stream, int byteLength);
	void ReadWithoutCopy(cCharStream *stream,int byteSize);
	bool Write(cStream *stream);
	bool Write(cStream *stream, int byteLength);

	bool WriteCurrent(cStream *stream);

    int Equal(const cBitString& bString) const;
	bool IsZero() const;
	void Average(cBitString &bString1, cBitString &bString2);

	int operator = (const cBitString &bString);
    bool operator == (const cBitString& bString) const;
	bool operator != (const cBitString& bString) const;
	bool operator >  (const cBitString& bString) const;
	bool operator >= (const cBitString& bString) const;
	bool operator <  (const cBitString& bString) const;
	bool operator <= (const cBitString& bString) const;

	bool Add(const cBitString& bString);
	bool Sub(const cBitString &op1, const cBitString &op2);
	void SubAbs(const cBitString &op1, const cBitString &op2);
	bool UMul(cBitString &op1, const cBitString &op2, cBitString &tmpBS);
	bool ShiftRight();
	bool ShiftLeft();
	void SetMostSignificant(unsigned int count, bool value);
	void SetFewSignificant(unsigned int count, bool value);

	void Or(const cBitString &bitString);
	void XOR(const cBitString &bitString);
	void And(const cBitString &bitString);
	void And(const cBitString &bitString1, const cBitString &bitString2);

	int Weight() const;

	void Print(int mode, char *str, bool currentFlag=false) const;
	void Print(char *str) const;

	static int ByteSize(int bitSize);

	// tos
	void RescaleBits(int shifts);
	bool GetBlock(unsigned int& tmpIndex, unsigned int& blockStartIndex, unsigned int& blockEndIndex);
	unsigned int FindFirstSetBit() const;
	inline unsigned int upZeroTest(unsigned int index) const;

	inline static void SetBit(char* bitArray, unsigned int index, bool value);
	inline static bool GetBit(const char* bitArray, unsigned int index);
	inline static unsigned int GetNumberOfBits(const char* bitArray, unsigned int length, bool value);
	inline static void SetBits(char* bitArray, unsigned int length, bool value);
	inline static bool Equal(char* bitArray1, unsigned int length1, char* bitArray2, unsigned int length2);
	inline static bool Equal(char* bitArray1, char* bitArray2, unsigned int length);
	inline static char* And(char* bitArray1, char* bitArray2, char* bitArrayResult, unsigned int byteSize);
	inline static char* Or(char* bitArray1, char* bitArray2, char* bitArrayResult, unsigned int byteSize);
	inline static char* Copy(char* bitArrayDest, const char* bitArraySrc, unsigned int byteSize);
	inline static char* CompleteMask(char* baseBits, char* partBits, char* resultBits, unsigned int length);
	inline static int Weight(char* bitArray, unsigned int length);
	inline static bool AndCompare(char* bitArray1, char* bitArray2, unsigned int byteSize);
	
	inline static void Print(char* bitArray, unsigned int bitLength);
	inline static void Print2File(FILE *StreamInfo, char* bitArray, unsigned int bitLength);
};

/**
 * Set <index> byte into bit string.
 **/
inline void cBitString::SetByte(unsigned int index, unsigned char value)
{
	if (index < GetByteLength())
	{
		*((char *)mNumber.GetArray() + index) = value;
	}
}

inline void cBitString::SetByte(unsigned char value)
{
	SetByte(0, value);
}

inline unsigned int cBitString::GetLength() const
{
	return mLength;
}
inline unsigned int cBitString::GetByteLength() const
{
	return mByteLength;
}
inline unsigned int cBitString::GetIntLength() const
{
	return mItemLength;
}

inline unsigned int cBitString::GetSerialSize() const
{
	return mByteLength;
}

inline unsigned int cBitString::GetCapacity() const
{
	return mNumber.Size() * ItemBitLength;
}
inline unsigned int cBitString::GetByteCapacity() const
{
	return mNumber.Size() * ItemByteLength;
}
inline unsigned int cBitString::GetIntCapacity() const
{
	return mNumber.Size();
}

/**
 * Get <index> byte from bit string.
 **/
inline unsigned char cBitString::GetByte(unsigned int index) const
{
	if (index < GetByteLength())
	{
		return *((char *)mNumber.GetArray() + index);
	}
	else
	{
		return 0;
	}
}

inline unsigned char cBitString::GetByte() const
{
	return GetByte(0);
}

inline unsigned int cBitString::GetInt(unsigned int index) const
{
	if (index < GetIntLength())
	{
		return mNumber[index];
	}
	else
	{
		return 0;
	}
}

inline unsigned int cBitString::GetInt() const 
{
	return mNumber[0]; 
}

inline unsigned int cBitString::GetUInt(unsigned int loIndex, unsigned int hiIndex) const
{
#ifndef NDEBUG
	assert(hiIndex - loIndex < ItemBitLength);
	assert(loIndex <= hiIndex);
	assert(loIndex < mLength && hiIndex < mLength);
#endif

	unsigned int loIndexOrder = loIndex / ItemBitLength;
	unsigned int hiIndexOrder = hiIndex / ItemBitLength;
	unsigned int number = 0;

	unsigned int loStartBitInItem = loIndexOrder * ItemBitLength;
	unsigned int loIndexInItem = loIndex - loStartBitInItem;

	if (loIndexOrder == hiIndexOrder)
	{
		unsigned int hiIndexInItem = hiIndex - loStartBitInItem;

		number = GetUInt(loIndexOrder, loIndexInItem, hiIndexInItem);
		number >>= loIndex;
	}
	else
	{
		unsigned int hiStartBitInItem = hiIndexOrder * ItemBitLength;
		unsigned int hiIndexInItem = hiIndex - hiStartBitInItem;

		unsigned int number1 = GetUInt(loIndexOrder, loIndexInItem, ItemBitLength-1);
		unsigned int number2 = GetUInt(hiIndexOrder, 0, hiIndexInItem);

		number1 >>= loIndexInItem;
		number2 <<= (ItemBitLength - loIndexInItem);
		number = number1 | number2;
	}
	return number;
}

/// Get uint from to position. The result is not shifted.
inline unsigned int cBitString::GetUInt(unsigned int itemOrder, unsigned int loIndex, unsigned int hiIndex) const
{
	/*unsigned int number = 0;
	unsigned int mask = 1 << loIndex;
	unsigned int defaultMask = mask;

	for (unsigned int i = loIndex+1 ; i <= hiIndex ; i++)
	{
		defaultMask <<= 1;
		mask |= defaultMask;
	}*/

	unsigned int myDefMask1 = (0xffffffff << loIndex);
	unsigned int myDefMask2 = 0xffffffff >> (ItemBitLength - 1 - hiIndex);
	unsigned int mask = myDefMask1 & myDefMask2;

	unsigned int number = mNumber[itemOrder] & mask;
	return number;
}

/**
 * return index of first non zero item in bit string (if all items with gereater index are zero return GetIntLength())
 **/
inline unsigned int cBitString::upZeroTest(unsigned int index) const{
	unsigned int i;
	if ( index > (GetIntLength() - 1)) return GetIntLength();
	for( i = index; (i < GetIntLength()) && (mNumber[i] == 0) ; i++ );
	return i;
}

/**
 * This method return value of bit (true/false) at location index
 * from this bit string.
 **/
inline bool cBitString::GetBit(const char* bitArray, unsigned int index)
{
	unsigned int order, cindex;
	unsigned int *current;

	order = index / ItemBitLength;
	current = (unsigned int *)bitArray + order; // item which bit on index is
	cindex = index - order * ItemBitLength;               // index in current item

	if ((*current & (1 << cindex)) > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * Set bit at index into bit string.
 **/
inline void cBitString::SetBit(char* bitArray, unsigned int index, bool value)
{
	unsigned int order, cindex;
	unsigned int *current;

	order = index / ItemBitLength;
	current = (unsigned int *)bitArray + order; // item which bit on index is
	cindex = index - order * ItemBitLength;               // index in current item

	if (value)
	{
		*current = (int)(*current | (1 << cindex));
	}
	else
	{
		*current = (int) (*current & ~(1 << cindex));
	}
}

inline unsigned int cBitString::GetNumberOfBits(const char* bitArray, unsigned int length, bool value) 
{
	unsigned int count = 0;

	for (unsigned int i = 0 ; i < length ; i++)
	{
		unsigned char currentByte = *(bitArray +  (i / SizeOfByte)) & (1 << (i % SizeOfByte));
		if (value)
		{
			if (currentByte != 0)
			{
				count++;
			}
		}
		else
		{
			if (currentByte == 0)
			{
				count++;
			}
		}
	}
	return count;
}

inline void cBitString::SetBits(char* bitArray, unsigned int length, bool value)
{
	unsigned int rest = length % SizeOfByte;
	unsigned int byteLength = length / SizeOfByte;

	if (rest != 0)
	{
		byteLength++;
		//printf("Critical Error: cBitString::SetBits(): length % SizeOfByte != 0");
	}

	unsigned char byteValue;
	if (value)
	{
		byteValue = 255;
	}
	else
	{
		byteValue = 0;
	}

	memset(bitArray, byteValue, byteLength);
}


inline bool cBitString::Equal(char* bitArray1, char* bitArray2, unsigned int length)
{
	for (unsigned int i = 0 ; i < length ; i++)
	{
		if (GetBit(bitArray1, i) != GetBit(bitArray2, i))
			return false;
	}

	return true;
}

// it not works for me
inline bool cBitString::Equal(char* bitArray1, unsigned int length1, char* bitArray2, unsigned int length2)
{
	if (length1 != length2)
	{
		return false;
	}

	unsigned int rest = length1 % SizeOfByte;
	unsigned int byteLength = length1 / SizeOfByte;

	if (rest != 0)
	{
		byteLength++;
		//printf("Critical Error: cBitString::Equal(): length1 % SizeOfByte != 0 && length2 % SizeOfByte != 0");
	}

	return memcmp(bitArray1, bitArray2, byteLength) == 0;
}

inline int cBitString::Weight(char* bitArray, unsigned int length)
{
	/*int numberWeight[] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,1,2,2,3,2,3,3,4,
		2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
		2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,
		4,5,5,6,5,6,6,7,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
		2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,2,3,3,4,3,4,4,5,
		3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
		4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8};*/
	int weight = 0;

	for (int i = 0 ; i < length ; i++)
	{
		if (GetBit(bitArray, i))
		{
			weight++;
		}
	}
	/*
	unsigned int byteLength = GetByteLength();
	for (unsigned int i = 0 ; i < byteLength ; i++)
	{
		weight += numberWeight[GetByte(i)];
	}*/

	return weight;
}

inline void cBitString::Print(char* bitArray, unsigned int bitLength)
{
	for (unsigned int i = 0 ; i < bitLength ; i++)
	{
		if (GetBit(bitArray, i))
		{
			printf("1");
		}
		else
		{
			printf("0");
		}
	}
}

inline void cBitString::Print2File(FILE *streamInfo, char* bitArray, unsigned int bitLength)
{
	for (unsigned int i = 0 ; i < bitLength ; i++)
	{
		if (GetBit(bitArray, i))
		{
			fprintf(streamInfo, "1");
		}
		else
		{
			fprintf(streamInfo, "0");
		}
	}
}

/*
 * Signature operation - do and together with a compare .
 */
inline bool cBitString::AndCompare(char* bitArray1, char* bitArray2, unsigned int byteSize)
{
	bool ret = true;
	const unsigned int bytesInInt = 4;
	unsigned int intSize = byteSize / bytesInInt;
	unsigned int restByteSize = byteSize % bytesInInt;

	unsigned int *pIBA1 = (unsigned int*)bitArray1;
	unsigned int *pIBA2 = (unsigned int*)bitArray2;

	for (unsigned int i = 0 ; i < intSize ; i++, pIBA1++, pIBA2++)
	{
		if (*pIBA1 == 0)  // a query signature includes almost the zero values
		{
			continue;
		}
		unsigned int result = *pIBA1 & *pIBA2;
		if (result != *pIBA1)
		{
			ret = false;
			break;
		}
	}

	if (ret && restByteSize != 0)
	{
		unsigned char *pCBA1 = (unsigned char*)pIBA1;
		unsigned char *pCBA2 = (unsigned char*)pIBA2;

		for (unsigned int i = 0 ; i < restByteSize ; i++, pCBA1++, pCBA2++)
		{
			if (*pCBA1 == 0)  // a query signature includes almost the zero values
			{
				continue;
			}

			unsigned char result = *pCBA1 & *pCBA2;
			if (result != *pCBA1)
			{
				ret = false;
				break;
			}
		}
	}

	return ret;
}

inline char* cBitString::And(char* bitArray1, char* bitArray2, char* bitArrayResult, unsigned int byteSize)
{
	for (unsigned int i = 0 ; i < byteSize ; i++)
	{
		bitArrayResult[i] = bitArray1[i] & bitArray2[i];
	}

	return bitArrayResult;
}

inline char* cBitString::Or(char* bitArray1, char* bitArray2, char* bitArrayResult, unsigned int byteSize)
{
	for (unsigned int i = 0 ; i < byteSize ; i++)
	{
		bitArrayResult[i] = bitArray1[i] | bitArray2[i];
	}

	return bitArrayResult;
}

inline char* cBitString::Copy(char* bitArrayDest, const char* bitArraySrc, unsigned int byteSize)
{
	for (unsigned int i = 0 ; i < byteSize ; i++)
	{
		bitArrayDest[i] = bitArraySrc[i];
	}

	return bitArrayDest;
}

inline char* cBitString::CompleteMask(char* baseBits, char* partBits, char* resultBits, unsigned int length)
{
	unsigned int j = 0;
	for (unsigned int i = 0 ; i < length ; i++)
	{
		if (GetBit(baseBits, i))
		{
			SetBit(resultBits, i, true);
		}
		else
		{
			SetBit(resultBits, i, GetBit(partBits, j++));
		}
	}

	return resultBits;
}

#endif
