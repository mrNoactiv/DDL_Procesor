/**
 *	\file cBitStringNew.h
 *	\author Michal Kratky
 *	\version 0.1
 *	\date jan 2012
 *	\brief dopsat
 */

#ifndef __cBitStringNew_h__
#define __cBitStringNew_h__


/**
* Represents ...
*
* \author Michal Kratky
* \version 0.1
* \date jan 2012
**/

namespace common {

class cBitStringNew
{
public:

	static const unsigned int UINT_LENGTH = 32;

	static inline unsigned int GetUInt(char* data, register unsigned int lo, register unsigned int hi);
	static inline void SetBit(char *data, unsigned int order, bool value);
};

/**
 * This method return the uint value of the bit string between lo and hi indices. 
 * These indices must be in the same int!
 **/
unsigned int cBitStringNew::GetUInt(char* data, unsigned int lo, unsigned int hi)
{
	const unsigned int segmentLength = 32;
	const unsigned int segmentLengthMinusOne = 31;
	const unsigned int shiftConst = 5;

	register unsigned int currentInt = *((unsigned int*)data + (lo >> shiftConst)); // /  segmentLength

	// remove all bits in the segment in higher bits then hi
	currentInt <<= segmentLengthMinusOne - (hi % segmentLength); 
	// shift the number to the first bit
	currentInt >>= segmentLengthMinusOne - ((hi - lo) % segmentLength);

	return currentInt;
}

void cBitStringNew::SetBit(char *data, unsigned int order, bool value)
{
	const unsigned int SegmentLength = 32;
	unsigned int intOrder = order / SegmentLength;
	unsigned int *currentInt = ((unsigned int*)data) + intOrder;
	unsigned int currentOrder = order - (intOrder * UINT_LENGTH);

	if (value)
	{
		*currentInt = (int)(*currentInt | (1 << currentOrder));
	}
	else
	{
		*currentInt = (int)(*currentInt & ~(1 << currentOrder));
	}
}

}
#endif
