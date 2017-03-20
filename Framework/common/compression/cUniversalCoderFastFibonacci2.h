/**
*	\file cUniversalCoderFastFibonacci2.h
*	\author Jiri Wadler
*	\version 1.0
*	\date 5.12.2009
*	\brief Fast Encoding/Decoding using Fibonacci of order 2, bits are stored in bytes most significant last.
*/
#ifndef __cUniversalCoderFastFibonacci2_h__
#define __cUniversalCoderFastFibonacci2_h__

#include "common/compression/cCoderCommon.h"
#include "common/memorystructures/cStack.h"
#include "common/cBitArray.h"
#include "common/cBit.h"
//Mapping table for Fast Fibonacci of order 2

namespace common {
	namespace compression {

// Fast algorithm for Fibonaci of order 2 encode/decode, bits are stored in bytes most significant last.
class cUniversalCoderFastFibonacci2
{
public:
	cUniversalCoderFastFibonacci2();
	~cUniversalCoderFastFibonacci2();
    static int pass;
	
    static inline unsigned int Fibonacci2LeftShift(unsigned int number, const unsigned int N);
	
	static unsigned int encode(int bytePerNumber, const unsigned char* sourceBuffer, unsigned char* encodedBuffer, unsigned int count);
	static unsigned int decode(int bytePerNumber, unsigned char* encodedBuffer, unsigned char* decodedBuffer, unsigned int count);
	static cBitArray mBits;
}; 
}}
#endif 