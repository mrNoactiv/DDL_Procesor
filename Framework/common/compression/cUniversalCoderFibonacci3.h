/**
*	\file cUniversalCoderFibonacci3.h
*	\author Jiri Wadler
*	\version 1.0
*	\date 5.12.2009
*	\brief Conventional bit oriented Encoding/Decoding using Fibonacci of order 3.
*/
#ifndef __cUniversalCoderFibonacci3_h__
#define __cUniversalCoderFibonacci3_h__

#include "common/compression/cCoderCommon.h"
#include "common/memorystructures/cStack.h"
#include "common/compression/cFibonacci3.h"
#include "common/cBitArray.h"
#define DIM(x)	 (sizeof(x) / sizeof(x[0]))

namespace common {
	namespace compression {

// Conventional bit oriented algorithm for Fibonaci of order 3 encode/decode
class cUniversalCoderFibonacci3
{
public:

	cUniversalCoderFibonacci3();
	~cUniversalCoderFibonacci3();

	
	static unsigned int Int2Code(const unsigned int Number, cBitArray *mBits);

    static unsigned int encode(int bytePerNumber, const unsigned char* sourceBuffer, unsigned char* encodedBuffer, unsigned int count);
	static unsigned int decode(int bytePerNumber, unsigned char* encodedBuffer, unsigned char* decodedBuffer, unsigned int count);
	static cStack<int> mStack;
	static cBitArray mBits;
}; 
}}
#endif 