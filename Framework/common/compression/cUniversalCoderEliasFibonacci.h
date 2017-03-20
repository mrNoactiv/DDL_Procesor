#ifndef __cUniversalCoderEliasFibonacci_h__
#define __cUniversalCoderEliasFibonacci_h__

#include "math.h"

#include "common/compression/cCoderCommon.h"
#include "common/memorystructures/cStack.h"
#include "common/cBitArray.h"
#include "common/cBit.h"

#define DIM(x)	 (sizeof(x) / sizeof(x[0]))

namespace common {
	namespace compression {

class cUniversalCoderEliasFibonacci
{
public:
    static const unsigned int FibNumbers[];
	cUniversalCoderEliasFibonacci();
	~cUniversalCoderEliasFibonacci();

	static unsigned int Int2Code(const unsigned int Number, cBitArray *mBits);
    static unsigned int encode(int bytePerNumber, const unsigned char * sourceBuffer, unsigned char* encodedBuffer, unsigned int count);
	static unsigned int decode(int bytePerNumber, unsigned char* encodedBuffer, unsigned char* decodedBuffer, unsigned int count);
	static cBitArray mBits;
	static cStack<int> mStack;
}; 
}}
#endif 
