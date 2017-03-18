/**
*	\file cUniversalCoderFastEliasFibonacci.h
*	\author Jiri Wadler
*	\version 1.0
*	\date 5.12.2009
*	\brief Fast Encoding/Decoding using Elias-Fibonacci code.
*/
#ifndef __cUniversalCoderFastEliasFibonacci_h__
#define __cUniversalCoderFastEliasFibonacci_h__

#include "common/compression/cCoderCommon.h"
#include "common/memorystructures/cStack.h"
#include "common/cBitArray.h"
#include "common/compression/cUniversalCoderFastFibonacci2.h"
#include "common/cBit.h"
#include "common/compression/cEliasFibonacciEncodeTab.h"
#include "common/compression/cEliasFibonacciDecodeTab.h"
#define DIM(x)	 (sizeof(x) / sizeof(x[0]))

namespace common {
	namespace compression {

// Fast algorithm for Elias-Fibonaci encode/decode
class cUniversalCoderFastEliasFibonacci
{
public:
	cUniversalCoderFastEliasFibonacci();
	~cUniversalCoderFastEliasFibonacci();

	static cStack<int> mStack;
    
	static unsigned int encode(int bytePerNumber, const unsigned char* sourceBuffer, unsigned char* encodedBuffer, unsigned int count);
	static unsigned int encode16(int bytePerNumber, const unsigned char* sourceBuffer, unsigned char* encodedBuffer, unsigned int count);
	static unsigned int decode(int bytePerNumber, unsigned char* encodedBuffer, unsigned char* decodedBuffer, unsigned int count);
	static cBitArray mBits;
}; 
}}
#endif 