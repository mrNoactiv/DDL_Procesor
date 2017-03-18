/**************************************************************************}
{                                                                          }
{    cBitString.cpp                                                        }
{                                                                          }
{                                                                          }
{    Copyright (c) 2001                      Michal Kratky                 }
{                                                                          }
{    VERSION: 0.2                            DATE 7/10/2001                }
{                                                                          }
{    following functionality:                                              }
{       Bit string                                                         }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      25/3/2002                                                           }
{      26/12/2002 add mByteLength, mItemLength                             }
{      30/01/2003 Debug Read() and Write() method                          }
{                                                                          }
{**************************************************************************/

#include "cBitString.h"

cBitString::cBitString(): mNumber()
{
	mLength = mByteLength = mItemLength = mMask = 0;
}

cBitString::cBitString(unsigned int length)
{
	mLength = length;
	mByteLength = CalculateByteLength();
	mItemLength = CalculateIntLength();
	mNumber.Resize(mItemLength);
	mNumber.SetCount(mItemLength);
	SetMask();
	Clear();
}

cBitString::cBitString(unsigned int length, unsigned int number): mNumber()
{
	mLength = length;
	mByteLength = CalculateByteLength();
	mItemLength = CalculateIntLength();
	mNumber.Resize(mItemLength);
		mNumber.SetCount(mItemLength);
	SetMask();
	Clear();
	mNumber[0] = number;
}

/**
 * Copy constructor - resizing mNumber and its setting from bString.
 **/
cBitString::cBitString(const cBitString &bString)
{
	Copy(bString);
}

cBitString::~cBitString()
{
	mNumber.Resize(0);
}

/**
 * Set bit at index into bit string.
 **/
void cBitString::SetBit(unsigned int index, bool bit)
{
	unsigned int delka=GetLength();
	if (!(index < delka))
	{
		int bla = 0;
	}

	assert(index < GetLength());

	if (index < GetLength())
	{
		unsigned int order, cindex;
		unsigned int *current;

		Calc(index, &order, &current, &cindex);
		if (bit)
		{
			*current = (int)(*current | (1 << cindex));
		}
		else
		{
			*current = (int) (*current & ~(1 << cindex));
		}
	}
}

/**
 * Set <index> int of bit string.
 **/
void cBitString::SetInt(unsigned int index, unsigned int value)
{
	if (index < mItemLength)
	{
		if (index == mItemLength-1 && mMask != Const_ItemMaxValue)
		{
			value &= mMask;
		}
		mNumber[index] = value;
	}
}

/**
 * Sets zero int of bit string.
 **/
void cBitString::SetInt(unsigned int value)
{
	SetInt(0, value);
}

void cBitString::SetBitString(const cBitString &bString)
{
	Copy(bString);
}

/**
 * Set bit string at string value. If length of string < <length>
 * then add 0 from left.
 **/
void cBitString::SetString(char *string, unsigned int length)
{
	unsigned int strLen = (unsigned int)strlen(string), differ = 0;
	Resize(length * SizeOfChar);
	mCurrentBit = mLength - 1;
	Clear();

	if (strLen < length)
		differ = length - strLen;

	for (unsigned int i = 0; i < length ; i++)
	{
		if (strLen < length)
		{
			if (i < differ)
				SetByte(i, 0);
			else
				SetByte(i, *(string + i - differ));
		}
		else
			SetByte(i, *(string + i));
	}
}

/**
 * Setting max value of bit string (by his size). !!!!
 **/
void cBitString::SetMaxValue()
{
	/*for (unsigned int i = 0 ; i < GetIntLength() ; i++)
	{
		SetInt(i, (unsigned int)~0);
	}*/
	for (unsigned int i = 0 ; i < GetLength() ; i++)
	{
		SetBit(i, true);
	}
}

/**
 * Get value of number between first and last bits.
 **/
unsigned int cBitString::GetValue(unsigned int first, unsigned int last) const
{
	unsigned int tmpVal, value = 0, index = 0;

	for(unsigned int i = first ; i <= last ; i++)
	{
		tmpVal = (GetBit(i)) ? 1 : 0;
		if (tmpVal != 0)
		{
			value |= tmpVal << index;
		}
		index++;
	}
	return value;
}

/**
 * Set the highest bit as current - for GetNextBit(), SetNextBit().
 **/
void cBitString::SetHighestBitAsCurrent() 
{ 
	mCurrentBit = mLength - 1; 
}

/**
 * Set the lowest bit as current - for GetNextBit(), SetNextBit().
 **/
void cBitString::SetLowestBitAsCurrent() 
{ 
	mCurrentBit = 0;
}

/**
 * Set the current bit at the value.
 **/
bool cBitString::SetPreviousBit(bool value) 
{ 
	SetBit(mCurrentBit--, value);
	return value;
}

/**
 * Set the current bit at the value.
 **/
bool cBitString::SetNextBit(bool value) 
{ 
	SetBit(mCurrentBit++, value);
	return value;
}

/**
 * Cleaning bit string.
 */
void cBitString::Clear() 
{
	for (unsigned int i = 0 ; i < GetIntLength() ; i++)
	{
		if (GetInt(i) != 0)
		{
			SetInt(i, (unsigned int)0);
		}
	}
	mCurrentBit = 0;
}

/**
 * Generate random bit string.
 */
void cBitString::GenerateRandom()
{
	for (unsigned int i = 0 ; i < mItemLength ; i++)
	{
		SetInt(i, cNumber::Random(Const_ItemMaxValue));
	}
}

/**
 * Add <value> to bit string.
 */
void cBitString::Add(unsigned int value)
{
	ullong number;
	for (unsigned int i = 0 ; i < GetIntLength() ; i++)
	{
		number = (ullong)GetInt(i) + (ullong)value;

		if (value != 0)
			SetInt(i, (unsigned int)number);

		if (*(((char *)&number) + ItemByteLength) > 0)
			value = 1;
		else
			break;
	}
}

/**
 * Sub <value> from bit string.
 * !!!!! NOT FULLY !!!!!
 **/
void cBitString::Sub(unsigned int value)
{
	llong number;
	for (unsigned int i = 0 ; i < GetIntLength() ; i++)
	{
		number = (ullong)GetInt(i) - (ullong)value;

		if (value != 0)
			SetInt(i, (unsigned int)number);

		if (number < 0)
			value = 1;
		else
		{
			break;
		}
	}
}

/**
 * Increment value of bit string.
 **/
void cBitString::Increment()
{
	Add(1);
}

/**
 * Decrement value of bit string.
 **/
void cBitString::Decrement()
{
	Sub(1);
}

/**
 * Do this = this or bitString.
 **/
void cBitString::Or(const cBitString &bitString)
{
	for (unsigned int i = 0 ; i < mItemLength ; i++)
	{
		mNumber[i] = mNumber[i] | bitString.GetInt(i);
	}
}

/**
 * Do this = this or bitString.
 **/
void cBitString::XOR(const cBitString &bitString)
{
	for (unsigned int i = 0 ; i < mItemLength ; i++)
	{
		mNumber[i] ^= bitString.GetInt(i);
	}
}

/**
 * Do this = this and bitString.
 **/
void cBitString::And(const cBitString &bitString)
{
	for (unsigned int i = 0 ; i < mItemLength ; i++)
	{
		mNumber[i] = mNumber[i] & bitString.GetInt(i);
	}
}

/**
 * Do this = bitString1 & bitString2.
 */
void cBitString::And(const cBitString &bitString1, const cBitString &bitString2)
{
	for (unsigned int i = 0 ; i < mItemLength ; i++)
	{
		mNumber[i] = bitString1.GetInt(i) & bitString2.GetInt(i);
	}
}

/**
 * Return length of bit string in ints (not real length).
 **/
unsigned int cBitString::CalculateIntLength() const
{
	unsigned int length = mLength / ItemBitLength;
	length += (mLength % ItemBitLength == 0) ? 0: 1;
	return length;
}

/**
 * Return length of bit string in bytes (not real length).
 **/
unsigned int cBitString::CalculateByteLength() const
{
	unsigned int length = mLength / SizeOfByte;
	length += (mLength % SizeOfByte == 0) ? 0: 1;
	return length;
}

/**
 * This method return value of bit (true/false) at location index
 * from this bit string.
 **/
bool cBitString::GetBit(unsigned int index) const
{
	if (index < mLength)
	{
		unsigned int order, cindex;
		unsigned int *current;

		Calc(index, &order, &current, &cindex);
		if ((*current & (1 << cindex)) > 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

/**
 * Get string representation of bit string.
 **/
void cBitString::GetString(cString &string)
{
	unsigned char ch = 0;

	for (unsigned int i = 0; i < GetByteLength() ; i++)
	{
		if ((ch = GetByte(i)) != 0)
		{
			string += (char)ch;
		}
	}
}

bool cBitString::GetPreviousBit()
{
	return GetBit(mCurrentBit--);
}

bool cBitString::GetNextBit()
{
	return GetBit(mCurrentBit++);
}

/**
 * Calculation of values for methods GetBit() and SetBit().
 **/
void cBitString::Calc(unsigned int index, unsigned int *order, unsigned int **current, unsigned int *cindex) const
{
	// order of item in array mNumber which bit on index is
	*order = index / ItemBitLength;
	*current = (unsigned int *)mNumber.GetArray() + *order; // item which bit on index is
	*cindex = index - *order * ItemBitLength;               // index in current item
}

/**
 * Because bit length may be < item length * ItemBitLength, then set mask item (use by SetInt() for example).
 */
void cBitString::SetMask()
{
	unsigned int wholeBitLength = mItemLength * ItemBitLength;
	unsigned int emptyBits =  wholeBitLength - mLength, value;
	mMask = Const_ItemMaxValue;

	if (emptyBits != 0)
	{
		for (unsigned int i = mLength ; i < wholeBitLength ; i++)
		{
			value = ~(1 << i);
			mMask &= value;
		}
	}
}

/**
 * Return true if bit string is zero.
 */
bool cBitString::IsZero() const
{
	bool ret = true;
	for (unsigned int i = 0 ; i < GetIntLength() ; i++)
	{
		if (GetInt(i) != 0)
		{
			ret = false;
			break;
		}
	}
	return ret;
}

/**
 * Copy contents of bString into this.
 **/
void cBitString::Copy(const cBitString &bString)
{
	Resize(bString);
	for (unsigned int i = 0 ; i < GetIntLength() ; i++)
	{
		SetInt(i, bString.GetInt(i));
	}
}

/**
 * Calculation average of two bit strings. It based on two step:
 *   * adding of all int of two bit strings
 *   * right rotation of result
 *
 * !!!!!!!!!!!!!!!!!!!!!! NOT FULL FUNCTION !!!!!!!!!!!!!!!!!!!!!!!
 * Don't look at different length of bString1 and bString2.
 **/
void cBitString::Average(cBitString &bString1, cBitString &bString2)
{
	llong sum;
	unsigned int carry = 0;

	Resize(bString1);

	for (unsigned int i = 0 ; i < bString1.GetIntLength(); i++)
	{
		sum = (ullong)bString1.GetInt(i) + (ullong)bString2.GetInt(i) + (ullong)carry;

		if (i != 0)
		{
			unsigned int temp = (((unsigned int)sum & (unsigned int)1) == 1) ? Const_1_31 : 0;
			SetInt(i-1, GetInt(i-1) | temp);
		}

		if (i == bString1.GetIntLength()-1)
		{
			SetInt(i, (unsigned int)(sum >> 1));
		}
		else
		{
			SetInt(i, ((unsigned int)sum >> 1));
		}

		// set carry
		if ((sum & (ullong)Const_1_32) > 0)     // 1 << 32
		{
			carry = 1;
		}
		else
		{
			carry = 0;
		}
	}
}

/**
 * Serialization of bit string.
 *
 * !!!!!!!!!!!!!!!!!!!!!! NOT FULL FUNCTION !!!!!!!!!!!!!!!!!!!!!!!
 * While without change of size.
 **/
bool cBitString::Write(cStream *stream)
{
	return stream->Write((char *)mNumber.GetArray(), GetByteLength());
}

bool cBitString::Write(cStream *stream, int byteLength)
{
	return stream->Write((char *)mNumber.GetArray(), byteLength);
}

/**
 * Serialization of bit string (its current size).
 *
 * !!!!!!!!!!!!!!!!!!!!!! NOT FULL FUNCTION !!!!!!!!!!!!!!!!!!!!!!!
 * While without change of size.
 **/
bool cBitString::WriteCurrent(cStream *stream)
{
	return stream->Write((char *)mNumber.GetArray(), (mCurrentBit+7)/8);
}

/**
 * Deserialization of bit string.
 *
 * !!!!!!!!!!!!!!!!!!!!!! NOT FULL FUNCTION !!!!!!!!!!!!!!!!!!!!!!!
 * While without change of size.
 **/
bool cBitString::Read(cStream *stream)
{
	bool ret = stream->Read((char *)mNumber.GetArray(), GetByteLength());
	return ret;
}

bool cBitString::Read(cStream *stream, int byteSize)
{
	bool ret = stream->Read((char *)mNumber.GetArray(), byteSize);
	return ret;
}

/// Dangerous method!!!
void cBitString::ReadWithoutCopy(cCharStream *stream, int byteSize)
{
	char* mem = stream->GetCharArray();
	mNumber.SetMem(mem);
	stream->Seek(stream->GetOffset() + byteSize);
}

/**
 * Comparing two instance of cBitString.
 * Return:
 *   true ... bit strings are same
 *   false ... bit strings aren't same

 *   >0 ... this bit string is bigger then bs
 *   0  ... bit strings are same;
 *   <0 ... this bit string is lesser then bs
 * !!!!!!!!!!! WARNING !!!!!!!!!
 * Only +1,0,-1 are returning! Because long long (__int64) would be
 * must returned.
 */
int cBitString::Equal(const cBitString &bString) const
{
	const cBitString *longerBStr = this;
	const cBitString *shorterBStr = &bString;
	int longerILen = GetIntLength();  // not real int length!

	int shorterILen = bString.GetIntLength();
	bool flagT = true;

	if (mLength > bString.GetLength())
	{
		int tmp = longerILen;
		longerILen = shorterILen;
		shorterILen = tmp;
		flagT = false;

		longerBStr = this;
		shorterBStr = &bString;
	}

	for (int i = longerILen-1 ; i >= 0 ; i--)
	{
		if (i <= shorterILen)
		{
			if (longerBStr->GetInt(i) > shorterBStr->GetInt(i))
			{
				if (flagT)
					return 1;
				else
					return -1;
			}
			else if (longerBStr->GetInt(i) < shorterBStr->GetInt(i))
			{
				if (flagT)
					return -1;
				else
					return 1;
			}
		}
		else {
			if (longerBStr->GetInt(i) > 0)
			{
				if (flagT)
					return 1;
				else
					return -1;
			}
		}
	}
	return 0;
}

/**
 * Resize at <bitLength> bits.
 **/
void cBitString::Resize(unsigned int bitLength)
{
	if (mLength != bitLength)
	{
		unsigned int len = mLength;
		mLength = bitLength;
		mByteLength = CalculateByteLength();
		mItemLength = CalculateIntLength();

		if (len < bitLength)
		{
			mNumber.Resize(mItemLength);
			mNumber.SetCount(mItemLength);
		}
		SetMask();
		// Clear();
	}
}

/**
 * Resize at <bitLength> bits. Use cMemory for allocation.
 **/
void cBitString::Resize(unsigned int bitLength, cMemory *memory)
{
	if (mLength != bitLength)
	{
		unsigned int len = mLength;
		mLength = bitLength;
		mByteLength = CalculateByteLength();
		mItemLength = CalculateIntLength();

		if (len < bitLength)
		{
			mNumber.Resize(mItemLength, memory->GetMemory(mItemLength * mNumber.GetItemSize()));
			mNumber.SetCount(mItemLength);
		}
		SetMask();
		// Clear();
	}
}

/**
 * Resize this at length of bString.
 **/
void cBitString::Resize(const cBitString &bString)
{
	if (mLength != bString.GetLength())
	{
		mLength = bString.GetLength();
		mByteLength = CalculateByteLength();
		mItemLength = CalculateIntLength();

		if (GetIntCapacity() != mItemLength)
		{
			mNumber.Resize(mItemLength);
				mNumber.SetCount(mItemLength);
			Clear();
		}
		SetMask();
	}
}

/**
 * Print bit string as binary numbers.
 **/
void cBitString::Print(char *str) const
{
	Print(cCommon::MODE_BIN, str);
}

/**
 * Print bit string.
 **/
void cBitString::Print(int mode, char *str, bool currentFlag) const
{
	int length;

	if (mode == cCommon::MODE_DEC)
	{
		length = GetIntLength();
		if (currentFlag)
		{
			length = (mCurrentBit + ItemBitLength-1) / ItemBitLength;
		}
	}
	else
	{
		length = mLength;
		if (currentFlag)
		{
			length = mCurrentBit;
		}
	}

	printf("");

	for (int i = length - 1 ; i >= 0 ; i--)
	{
		if (mode == cCommon::MODE_DEC)
		{
			printf("%u", GetInt(i));
			if (i != 0)
			{
				printf(",");
			}
		}
		else
		{
			printf("%d", GetBit(i));
			if ((i % 32) == 0 && i !=0)
			{
				// printf (" | ");
			}
			else
			{
				if (i % 8 == 0)
				{
					// printf(" ");
				}
			}
		}
	}
	printf("%s", str);
}

/**
 * Return byt size to bit size.
 */
int cBitString::ByteSize(int bitSize)
{
	int len = ((bitSize % SizeOfByte) == 0) ? 0 : 1;
	len += bitSize / SizeOfByte;
	return len;
}

/**
 * Operator = then copy contents of bString into this.
 **/
int cBitString::operator = (const cBitString &bString)
{
	Copy(bString);
	return 0;
}

bool cBitString::operator == (const cBitString& bString) const
{
	return Equal(bString) == 0;
}

bool cBitString::operator != (const cBitString& bString) const
{
	return Equal(bString) != 0;
}

bool cBitString::operator >  (const cBitString& bString) const
{
	return Equal(bString) > 0;
}

bool cBitString::operator >= (const cBitString& bString) const
{
	return Equal(bString) >= 0;
}

bool cBitString::operator <  (const cBitString& bString) const
{
	return Equal(bString) < 0;
}

bool cBitString::operator <= (const cBitString& bString) const
{
	return Equal(bString) <= 0;
}

/**
 * Add this and bString into this.
 */
bool cBitString::Add(const cBitString& bString)
{
	llong value;
	unsigned int number, carry = 0;

	for (unsigned int i = 0 ; i < mItemLength ; i++)
	{
		value = (ullong)GetInt(i) + (ullong)bString.GetInt(i) + (ullong)carry;
		if (value <= Const_1_32_1)
		{
			carry = 0;
		}
		else
		{
			carry = 1;
		}
		number = (unsigned int)value;
		SetInt(i, number);
	}
	return (carry == 1);
}

/**
 * Do this = op1 - op2. If result is negative return true, else return false.
 */
bool cBitString::Sub(const cBitString &op1, const cBitString &op2)
{
	llong value;
	unsigned int carry = 0;

	for (unsigned int i = 0 ; i < mItemLength ; i++)
	{
		if ((value = (llong)op1.GetInt(i) - (llong)(op2.GetInt(i) + carry)) < 0)
		{
			value = (Const_1_32 - (op2.GetInt(i) + carry)) + op1.GetInt(i);
			carry = 1;
		}
		else
		{
			carry = 0;
		}
		SetInt(i, (unsigned int)value);
	}

	return (carry == 1);
}

/**
 * @author: tos
 * @function: Do this = abs(op1 - op2).
 */
void cBitString::SubAbs(const cBitString &op1, const cBitString &op2)
{
	if (op1 < op2)
	{
		Sub(op2,op1);
	}
	else
	{
		Sub(op1,op2);
	}
}

/**
 * Do this = op1 * op2. If result overflow "this" then return false, else return false.
 *	tmpBS - is used for strore first argument during multiplication, without using new object cBitString
 */
bool cBitString::UMul(cBitString &op1, const cBitString &op2, cBitString &tmpBS)
{
	ullong value;
	unsigned int mul2,c;
	ullong tmp;
	if (tmpBS.GetIntLength() < op1.GetIntLength()) return false;
	for(unsigned int i = 0; i < tmpBS.GetIntLength(); i++){ //copy op1 to tmpBS
		if (i < op1.GetIntLength()){
			tmpBS.SetInt(i, op1.GetInt(i));
		}else{
			tmpBS.SetInt(i, 0);
		}
	}
		
	Clear(); //clear result
	for(unsigned int i = 0; (i < op2.GetIntLength()); i++) 
	{
		i = op2.upZeroTest(i);                    //test - whether is any item nonzero
		if (i >= op2.GetIntLength()) break;       // all items above index are zero
		mul2 = op2.GetInt(i); 
		for(unsigned int j=0; j < tmpBS.GetIntLength(); j++ )
		{
			j = tmpBS.upZeroTest(j); 
			if (j >= tmpBS.GetIntLength()) break; 
			value = (ullong)tmpBS.GetInt(j) * mul2; // multiplication of items
			if (value != 0)                         // this should by always true, if method upZero is used  
			{ 
				if ( (i + j) > ( mItemLength - 1 ))   //result overflow
				{
					return false;
				}
				tmp = (ullong)mNumber[i + j] + *((unsigned int *)(&value)); //test to carry
				mNumber[i + j] += *((unsigned int *)(&value));
				c = i + j;
				if (tmp > Const_1_32_1)     // is carry
				{ 
					do
					{
						if ( ++c > ( mItemLength - 1 )) 
						{
							return false;
						}
						mNumber[c]++;
					} while( mNumber[c] == 0 ); //next carry
				}

				//if (*((unsigned int *)(&value) + 1) != 0){ //value is bigger then max unsig. int	
				if (value > Const_1_32_1)
				{
					if ( i + j + 1 > ( mItemLength - 1 )) 
					{
						return false;
					}
					tmp = (ullong)mNumber[i + j + 1] + *((unsigned int *)(&value) + 1);
					mNumber[i + j + 1] += *((unsigned int *)(&value) + 1);
					c = i + j + 1;
					if (tmp > Const_1_32_1)    // is carry
					{
						do
						{
							if ( ++c > ( mItemLength - 1 )) 
							{
								return false;
							}
							mNumber[c]++;
						} while( mNumber[c] == 0 ); // next carry
					}
				}
			}
		}

	}
	return true; //result is correct, without overflow
}

/**
 * Shift right.
 */
bool cBitString::ShiftRight()
{
	unsigned int number, value, carry = 0;

	for (int i = (int)mItemLength-1 ; i >= 0 ; i--)
	{
		number = GetInt(i);
		if (number != 0 || carry != 0)
		{
			value = number & 1;
			number >>= 1;
			number += carry;
			SetInt(i, number);

			if (value == 0)
			{
				carry = 0;
			}
			else
			{
				carry = Const_1_31;
			}
		}
	}
	return (carry == 1);
}

/**
 * Shift left.
 */
bool cBitString::ShiftLeft()
{
	unsigned int number, value, carry = 0;

	for (unsigned int i = 0 ; i < mItemLength ; i++)
	{
		number = GetInt(i);
		if (number != 0 || carry != 0)
		{
			value = number & Const_1_31;
			number <<= 1;
			number += carry;
			SetInt(i, number);

			if (value == 0)
			{
				carry = 0;
			}
			else
			{
				carry = 1;
			}
		}
	}
	return (carry == 1);
}

/**
 * Set the count most significant bits to value.
 */
void cBitString::SetMostSignificant(unsigned int count, bool value)
{
	int index = mLength - count;
	for (int i = mLength - 1 ; i >= index ; i--)
	{
		SetBit(i, value);
	}
}

/**
 * Set the count most significant bits to value.
 */
void cBitString::SetFewSignificant(unsigned int count, bool value)
{
	for (unsigned int i = 0 ; i < count ; i++)
	{
		SetBit(i, value);
	}
}

void cBitString::RescaleBits(int shifts)
{
	if (shifts > 0)		// "shrink" the bits
	{
		int divider = 1 << shifts;

		for(unsigned int i = 0; i < mLength; i++)
		{
			bool val = GetBit(i);
			SetBit(i, false);
			SetBit(i / divider, GetBit(i / divider) || val);
		}
	}
	// todo: "strech" the bits
}

bool cBitString::GetBlock(unsigned int& searchStartIndex, unsigned int& blockStartIndex, unsigned int& blockEndIndex)
{
	bool found = false;

	while(searchStartIndex < GetLength() && GetBit(searchStartIndex) == false)
	{
		searchStartIndex++;
	}
	blockStartIndex = searchStartIndex;

	while(searchStartIndex < GetLength() && GetBit(searchStartIndex) == true)
	{
		searchStartIndex++;
		found = true;
	}
	blockEndIndex = searchStartIndex - 1;

	return found;
}

unsigned int cBitString::FindFirstSetBit() const{
	unsigned int position = mLength - 1;

	unsigned int byte =  position / 8;
	if (*((unsigned char *)mNumber.GetArray() + byte--) == 0){
		position -= position % 8;
		while((*((unsigned char *)mNumber.GetArray() + byte--) == 0) && (position !=0))
			position -= 8;
	}

	unsigned int item = position / ItemBitLength;
	unsigned int bitInItem = position - item * ItemBitLength;
	cBitString_item valueOfItem = *((unsigned int *)mNumber.GetArray() + item);
		
	while( !( valueOfItem & (1 << bitInItem ) ) && (position > 0)){
		position--;
		item = position / ItemBitLength;
		valueOfItem = *((unsigned int *)mNumber.GetArray() + item);
		bitInItem = position - item * ItemBitLength;
	}

	return position;
}

int cBitString::Weight() const
{
	int numberWeight[] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,1,2,2,3,2,3,3,4,
		2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
		2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,
		4,5,5,6,5,6,6,7,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
		2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,2,3,3,4,3,4,4,5,
		3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
		4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8};
	int weight = 0;

	/*for (int i = 0 ; i < mLength ; i++)
	{
		if (GetBit(i))
		{
			weight++;
		}
	}*/

	unsigned int byteLength = GetByteLength();
	for (unsigned int i = 0 ; i < byteLength ; i++)
	{
		weight += numberWeight[GetByte(i)];
	}

	return weight;
}

/*
void main(void){
	cBitString result(64*9);
	cBitString tmp(64*9);
	cBitString operand2(64*2);
	operand2.SetInt(0, 0xFFFFFFFF);
	operand2.SetInt(1, 0xFF);
	result.SetInt(0, 0xFFFFFFFF);
	result.SetInt(1, 0xFF);
	result.Print(cCommon::MODE_DEC, "\n");
	operand2.Print(cCommon::MODE_DEC, "\n");
	result.UMul(result, operand2, tmp);
	result.Print(cCommon::MODE_DEC, "\n\n");

	cBitString str1(32);
	str1.SetInt(0,0);
	str1.Print(cCommon::MODE_DEC,"\n");
	unsigned int i = str1.FindFirstSetBit();
	cout << "poz: " << i << endl;
	str1.Print(cCommon::MODE_DEC,"\n");
}*/