/**
*	\file cUniversalCoderFibonacci2.h
*	\author Jiri Wadler
*	\version 1.0
*	\date 5.12.2009
*	\brief Conventional bit oriented Encoding/Decoding using Fibonacci of order 2.
*	, bits are stored in bytes most significant last.
*/
#ifndef __cUniversalCoderFibonacci2_h__
#define __cUniversalCoderFibonacci2_h__

#include "common/compression/cCoderCommon.h"
#include "common/memorystructures/cStack.h"
#include "common/cBitArray.h"
#include "common/cBit.h"
#include "common/compression/cFibonacci2.h"
#define DIM(x)	 (sizeof(x) / sizeof(x[0]))

namespace common {
	namespace compression {

// Conventional bit oriented algorithm for Elias-delta encode/decode, bits are stored in bytes most significant last.
class cUniversalCoderFibonacci2
{
public:
	cUniversalCoderFibonacci2();
	~cUniversalCoderFibonacci2();
    static unsigned int Int2Code(const unsigned int Number, cBitArray *mBits);
  
	static cStack<int> mStack;
	static unsigned int leftShift(unsigned int number,int shift);
	//static unsigned int Int2Fib(const unsigned int Number,unsigned long long *mBits);
	static unsigned int encode(int bytePerNumber, const unsigned char* sourceBuffer, unsigned char* encodedBuffer, unsigned int count);
	static unsigned int decode(int bytePerNumber, unsigned char* encodedBuffer, unsigned char* decodedBuffer, unsigned int count);
	static cBitArray mBits;
}; 
}}
#endif 