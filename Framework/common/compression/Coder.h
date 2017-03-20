// MathFuncsLib.h
#ifndef __Coder_h__
#define __Coder_h__

#include "limits.h"
#include "common/compression/cCoderCommon.h"

#define MEMCPY32 -1
#define FIXED32 0
#define ELIAS_DELTA 1
#define ELIAS_DELTA_FAST 5
#define FIBONACCI2 2
#define FIBONACCI2_FAST 6
#define FIBONACCI3 3
#define FIBONACCI3_FAST 7
#define ELIAS_FIBONACCI 4
#define ELIAS_FIBONACCI_FAST 8
#define FIXED_LENGTH_CODING 9
#define FIXED_LENGTH_CODING_ALIGNED 10

namespace common {
	namespace compression {

	class Coder
	{
	public:
		static unsigned int encode(int method,int bytePerNumber, const char* sourceBuffer, char* encodedBuffer, unsigned int count);
		static unsigned int decode(int method,int bytePerNumber, char* encodedBuffer, char* decodedBuffer, unsigned int count);
		static char * methodName(int method);
		static unsigned int estimateSizeInBits(int method, int bytePerNumber, char* sourceBuffer, unsigned int count);
		static void print(int method, char* buffer,unsigned int bytes);
		static uint GetSize(int method, int bytePerNumber, char* sourceBuffer, uint count);
		static uint GetSize(unsigned int value);
	};
}}
#endif