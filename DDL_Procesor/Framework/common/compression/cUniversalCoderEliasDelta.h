/**
*	\file cUniversalCoderEliasDelta.h
*	\author Jiri Wadler
*	\version 1.0
*	\date 5.12.2009
*	\brief Encoding/Decoding using Elias-delta code.
*/
#ifndef __cUniversalCoderEliasDelta_h__
#define __cUniversalCoderEliasDelta_h__

#include "common/compression/cCoderCommon.h"
#include "common/cBitArray.h"

//#include "cArray.h"

namespace common {
	namespace compression {

// Conventional bit oriented algorithm for Elias-delta encode/decode
class cUniversalCoderEliasDelta
{
public:

	cUniversalCoderEliasDelta();
	~cUniversalCoderEliasDelta();


	static unsigned int Int2Code(const unsigned int Number, cBitArray *mBits);
    static unsigned int encode(int bytePerNumber, const unsigned char* sourceBuffer, unsigned char* encodedBuffer, unsigned int count);
	static unsigned int decode(int bytePerNumber, unsigned char* encodedBuffer, unsigned char* decodedBuffer, unsigned int count);
	static cBitArray mBits;
}; 
}}
#endif 