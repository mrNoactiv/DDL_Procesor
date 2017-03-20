
#include "cUniversalCoderFastFibonacci3.h"
#include "cFibonacci3EncodeTab.h"
#include "cFibonacci3DecodeTab.h"
#include "cFibonacci3.h"
#include "log256table.h"
#include "math.h"
#define IGNOREFASTARRAYE 1
#define DIRECTWRITEE 1
#define IGNOREFASTARRAYD 1
#define DIRECTWRITED 1
#define getEstimationF3_32_16(k,num) k=39023+(num+147147265)/294294531; if (num>FibEncode3_16[k].to) k++
#define getEstimationF3_16_16(k,num) k=19511+(num+8577)/17155; if (num>FibEncode3_16[k].to) k++
#define TAB256
#define X64
//#define BSRI

namespace common {
	namespace compression {

cBitArray cUniversalCoderFastFibonacci3::mBits = cBitArray();

cUniversalCoderFastFibonacci3::cUniversalCoderFastFibonacci3()
{

}

cUniversalCoderFastFibonacci3::~cUniversalCoderFastFibonacci3()
{
}
//Fibonacci <<F k-Left shift
inline unsigned int cUniversalCoderFastFibonacci3::Fibonacci3LeftShift(unsigned int number, const unsigned int N)
{

	return FibNumbers3_0[N] * number 
		 + (FibNumbers3_1[N]+FibNumbers3_2[N]) * Fibonacci3RightShift_1[number]
		 + FibNumbers3_1[N] * Fibonacci3RightShift_2[number];
}

unsigned int cUniversalCoderFastFibonacci3::decode(int bytePerNumber, unsigned char* encodedBuffer, unsigned char* decodedBuffer,unsigned int count) 
{
	mBits.SetMemoryForRead(encodedBuffer,0);
 
	register unsigned int n=0;
	register int actState=0;
	register int tabPos;
	register int readByte;
	register int lenA=0;
	unsigned int j=0,i=0;

	while (j<count) 
	{
		readByte=encodedBuffer[i++];

		tabPos=(actState)+readByte;
		actState=Fibonacci3MapTable[tabPos].newState;
		if (lenA>0) {//prvni je se shiftem pokud je predchozi
			n+=Fibonacci3LeftShift(Fibonacci3MapTable[tabPos].code0,lenA);
		} else {
			n=Fibonacci3MapTable[tabPos].code0;
		}
		lenA+=Fibonacci3MapTable[tabPos].bits0;
		if (Fibonacci3MapTable[tabPos].out0) {
			n+=FibNumbers3sumDecode[lenA];
			*((unsigned int *)(decodedBuffer+j*bytePerNumber))=n;
			j++;

			lenA=0;
			n=0;
		} else continue;
		if (Fibonacci3MapTable[tabPos].bits1) {
			n=Fibonacci3MapTable[tabPos].code1;
			lenA=Fibonacci3MapTable[tabPos].bits1;
			if (Fibonacci3MapTable[tabPos].out1) { 
				n+=FibNumbers3sumDecode[lenA];
				*((unsigned int *)(decodedBuffer+j*bytePerNumber))=n;
				j++;

				lenA=0;
				n=0;
			} else continue;
		} else continue;
		if (Fibonacci3MapTable[tabPos].bits2) {
			n=Fibonacci3MapTable[tabPos].code2;
			lenA=Fibonacci3MapTable[tabPos].bits2;
			if (Fibonacci3MapTable[tabPos].out2) { 
				n+=FibNumbers3sumDecode[lenA];
				*((unsigned int *)(decodedBuffer+j*bytePerNumber))=n;
				j++;
	
				lenA=0;
				n=0;
			} else continue;
		} else continue;
		if (Fibonacci3MapTable[tabPos].bits3) {
			n=Fibonacci3MapTable[tabPos].code3;
			lenA=Fibonacci3MapTable[tabPos].bits3;
			if (Fibonacci3MapTable[tabPos].out3) { 
				n+=FibNumbers3sumDecode[lenA];
				*((unsigned int *)(decodedBuffer+j*bytePerNumber))=n;
				j++;

				lenA=0;
				n=0;
			} else continue;
		} else continue;
	}
	return i;
}

unsigned int cUniversalCoderFastFibonacci3::encode(int bytePerNumber, const unsigned char* sourceBuffer, unsigned char* encodedBuffer, unsigned int count) 
{
    int  j = 0;
	register unsigned int  k;
	unsigned int remain = 0;
	int rempos = 0;
	unsigned short vals[8];
	 int bytes, bits;
	int valcount = 0; 
	unsigned int q;
	unsigned char len;

	unsigned int extendedchar;
	unsigned int mask=0xFFFFFFFF>>(32-bytePerNumber*8);
	unsigned short *buf=(unsigned short *)encodedBuffer;
	for (unsigned int i=0; i<count; i++) 
	{
		unsigned int num = (*((unsigned int *)(sourceBuffer+i*bytePerNumber)))&mask;
		num = cCoderCommon::Increment(bytePerNumber, num);

		if (num==1) {
			remain |= 7 << (rempos);
			rempos += 3;
	
			while (rempos>=16) {

					buf[j++] = remain & 0xffff;

				remain >>=16;
				rempos -= 16;
			}	
		}
		else if (num==2) {
			remain |= 14 << (rempos);
			rempos += 4;
		
			while (rempos>=16) {

					buf[j++] = remain & 0xffff;

				remain >>=16;
				rempos -= 16;
			}	
		}
		else if (num<FibNumbers3[16]) 
		{
			valcount = 0;
			//find fibsum,len,q
		
				len=FibEncode3_16[num].len;
				q=num-FibEncode3_16[num].fibsum;
	
			//encode q
			bits = len+4;
		
			extendedchar = FibEncode3_16[q].val;
			extendedchar|=14<<len;
			remain |= extendedchar << (rempos);
			rempos += bits;

			if (rempos>=16) {

					buf[j++] = remain & 0xffff;

				remain >>=16;
				rempos -= 16;
			}	
			
			//pokud pretece
			if (rempos>=16) {

					buf[j++] = remain & 0xffff;

				rempos -= 16;
				remain =extendedchar>>(bits-rempos);	
			}
			
		}
		else
		{	
#ifdef TAB256		
		register unsigned int t, tt; 
if (tt = num >> 16)
{
  k = (t = tt >> 8) ? 24 + LogTable256_1[t] : 16 + LogTable256_1[tt];
}
else 
{
  k = (t = num >> 8) ? 8 + LogTable256_1[t] : LogTable256_1[num];
}/**/
k=k-1;
#endif
#ifdef BSRI
#ifdef X32
			__asm {//to same jako:len=log2(num);
				 BSR eax,num;
	
				  mov k,eax;
	
			}
#endif
#endif
			int fs=FibNumbers3sum_est[k];
	
			while (FibNumbers3sum_64[fs+1]+1<=num) {
				fs++;

			}

			len=(unsigned char)fs+1;
			num=num-(unsigned int)FibNumbers3sum_64[fs]-1;			
				
			//zakoduj q, coz je ted "num"
			if (num>=FibNumbers3[32]) {//593
				if (num>=4165752206) {
					k=39038;
				}
				else {
					getEstimationF3_32_16(k,num);
				}	
				
			}
			else
				if (num>=FibNumbers3[16]) {//297
					getEstimationF3_16_16(k,num);
				}
	
			else {
				k=num;
			}

			valcount = 0;
			bytes = FibEncode3_16[k].bytes;
			bits = FibEncode3_16[k].bits;
		
			extendedchar = FibEncode3_16[k].val;

			len=(unsigned char)(len-bytes*16);

			num -= FibEncode3_16[k].from;

			if (bytes==2) 
			{
				if (num<FibNumbers3[16]) 
				{
					vals[valcount++] = 0;
				}
				else
				{

					getEstimationF3_16_16(k,num);
					vals[valcount++] = FibEncode3_16[k].val;
					num -= FibEncode3_16[k].from;
				}
				bytes--;				
			}
			if (bytes==1) 
			{
				vals[valcount++] = FibEncode3_16[num].val;
				bytes--;								
			}


			//zapis vsecny cele byty
			while (--valcount>=0)
			{

	
				remain |= vals[valcount] << rempos;
			
				
				buf[j++] = remain & 0xffff;

				remain >>= 16;

			}
			//zapis zbytek
			
		
			remain |= extendedchar << (rempos);
			rempos += len;
	

			while (rempos>=16) {
			
				buf[j++] = remain & 0xffff;

				remain >>=16;
				rempos -= 16;
			}	

			//delimiter
			remain|=14<<rempos;
			rempos += 4;
			if (rempos>=16) {

				buf[j++] = remain & 0xffff;

				remain >>=16;
				rempos -= 16;
			}	


		} 
	}
	int retbits=j*2*8+rempos;

	if (rempos>8) 
	{
			buf[j] = remain & 0xffff;
	}
	else if (rempos>0) 
	{
			encodedBuffer[j*2] = remain & 0xff;
	}
    return retbits;

}
}}
