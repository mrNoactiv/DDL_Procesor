/**
*	\file cUniversalCoderFastEliasDelta.h
*	\author Jiri Wadler
*	\version 1.0
*	\date 5.12.2009
*	\brief Fast Encoding/Decoding using Elias-delta code.
*/
#ifndef __cUniversalCoderFastEliasDelta_h__
#define __cUniversalCoderFastEliasDelta_h__

//#include "cArray.h"
//#include "cCharStream.h"
#include "common/compression/cCoderCommon.h"
#include "common/cBitArray.h"
#include "common/cBit.h"
#include "common/compression/cEliasDeltaEncodeTab.h"
#include "common/compression/cEliasDeltaDecodeTab.h"
// Fast algorithm for Elias-delta encode/decode

namespace common {
	namespace compression {

class cUniversalCoderFastEliasDelta
{
public:
	cUniversalCoderFastEliasDelta();
	~cUniversalCoderFastEliasDelta();


	static unsigned int encode(int bytePerNumber, const unsigned char* sourceBuffer, unsigned char* encodedBuffer, unsigned int count);
	static unsigned int encode16(int bytePerNumber, const unsigned char* sourceBuffer, unsigned char* encodedBuffer, unsigned int count);
	static unsigned int decode(int bytePerNumber, unsigned char* encodedBuffer, unsigned char* decodedBuffer, unsigned int count);
	static cBitArray mBits;
	static unsigned int estimateSizeInBits(int bytePerNumber, unsigned char* sourceBuffer,unsigned int count);
}; 
}}
#endif 