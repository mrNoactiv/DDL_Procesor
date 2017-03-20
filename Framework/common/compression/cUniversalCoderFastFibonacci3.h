/**
*	\file cUniversalCoderFastFibonacci3.h
*	\author Jiri Wadler
*	\version 1.0
*	\date 5.12.2009
*	\brief Fast Encoding/Decoding using Fibonacci of order 3. bits are stored in bytes most significant last.
*/
#ifndef __cUniversalCoderFastFibonacci3_h__
#define __cUniversalCoderFastFibonacci3_h__

#include "common/compression/cCoderCommon.h"
#include "common/memorystructures/cStack.h"
#include "common/cBitArray.h"
#include "common/compression/cUniversalCoderFastFibonacci2.h"
#include "common/cBit.h"
#define DIM(x)	 (sizeof(x) / sizeof(x[0]))

namespace common {
	namespace compression {

// Fast algorithm for Fibonaci of order 3 encode/decode
class cUniversalCoderFastFibonacci3
{
public:
	cUniversalCoderFastFibonacci3();
	~cUniversalCoderFastFibonacci3();

    static unsigned int Fibonacci3LeftShift(unsigned int number, const unsigned int N);
	
	static unsigned int encode(int bytePerNumber, const unsigned char* sourceBuffer, unsigned char* encodedBuffer, unsigned int count);
	static unsigned int decode(int bytePerNumber, unsigned char* encodedBuffer, unsigned char* decodedBuffer, unsigned int count);
	static cBitArray mBits;
}; 
}}
#endif 