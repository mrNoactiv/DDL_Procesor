/**
*	\file cBit.h
*	\author Jiri Wadler
*	\version 1.0
*	\date 5.12.2009
*	\brief Bit manipulation
*/

#ifndef __cBit_h__
#define __cBit_h__
#include "cFastBitArray.h"


/**
*	Bit manipulation
*
**/
class cBit
{
public:
	int CompressType;

	cBit();
	~cBit();

	
	void PrintBuffer(unsigned int ncount,unsigned char* buffer);
	void PrintBufferHiLo(unsigned int ncount,unsigned char* buffer);
	void PrintBytes(unsigned int ncount,unsigned char* buffer);
	static void PrintBits64Lo2Hi(unsigned long long * input);
	static int getBit64Lo2Hi(unsigned long long * input,int i);
    static void setBit64Lo2Hi(unsigned long long * input,int i,int bit);
	static void PrintBits8Lo2Hi(unsigned char * input);
	static int getBit8Lo2Hi(unsigned char * input,int i);
    static void setBit8Lo2Hi(unsigned char * input,int i,int bit);
	static void PrintBits64Hi2Lo(unsigned long long * input);
	static int getBit64Hi2Lo(unsigned long long * input,int i);
    static void setBit64Hi2Lo(unsigned long long * input,int i,int bit);
	static void PrintBits8Hi2Lo(unsigned char * input);
	static int getBit8Hi2Lo(unsigned char * input,int i);
    static void setBit8Hi2Lo(unsigned char * input,int i,int bit);
	static unsigned char reverseBits(unsigned char input);
	
	static cFastBitArray mBits;
}; 

#endif 