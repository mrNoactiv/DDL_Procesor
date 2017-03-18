/**
*	\file cUniversalCoderEliasDelta.h
*	\author Jiri Wadler
*	\version 1.0
*	\date 5.12.2009
*	\brief Encoding/Decoding using Elias-delta code.
*/
#ifndef __cUniversalCoderFixedLength_h__
#define __cUniversalCoderFixedLength_h__

#include "common/compression/cCoderCommon.h"
#include "common/cBitArray.h"

namespace common {
	namespace compression {
class cUniversalCoderFixedLength
{
private:
	static const uint HEADER_BITLENGTH = 6;

public:
	cUniversalCoderFixedLength();
	~cUniversalCoderFixedLength();

    static unsigned int encode(int bytePerNumber, const unsigned char* sourceBuffer, unsigned char* encodedBuffer, unsigned int count);
    static unsigned int encode_aligned(int bytePerNumber, const unsigned char* sourceBuffer, unsigned char* encodedBuffer, unsigned int count);
	static unsigned int decode(int bytePerNumber, unsigned char* encodedBuffer, unsigned char* decodedBuffer, unsigned int count);
	static unsigned int decode_aligned(int bytePerNumber, unsigned char* encodedBuffer, unsigned char* decodedBuffer, unsigned int count);
	static uint GetCodewordSize(int bytesPerNumber, const unsigned char* sourceBuffer, unsigned int count);
	static uint GetCodewordSize_aligned(int bytesPerNumber, const unsigned char* sourceBuffer, unsigned int count);
}; 
}}
#endif