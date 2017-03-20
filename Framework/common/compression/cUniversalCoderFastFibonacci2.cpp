
#include "cUniversalCoderFastFibonacci2.h"
#include "cFibonacci2EncodeTab.h"
#include "cFibonacci2DecodeTab.h"
#include "cFibonacci2.h"
#define X64

#define IGNOREFASTARRAY 1
#define DIRECTWRITE 1
#define DIM(x)	 (sizeof(x) / sizeof(x[0]))
#define getEstimation16_16(k,num) k=2582+(num+1103)/2207; if (num>FibEncode16[k].to) k++
#define getEstimation16_32(k,num) k=5165+(num/100+24354)/48708; if (num>FibEncode16[k].to) k++


namespace common {
	namespace compression {

cBitArray cUniversalCoderFastFibonacci2::mBits=cBitArray();

cUniversalCoderFastFibonacci2::cUniversalCoderFastFibonacci2()
{

}


cUniversalCoderFastFibonacci2::~cUniversalCoderFastFibonacci2()
{
}
//Fibonacci 2 <<F k-shift
unsigned int cUniversalCoderFastFibonacci2::Fibonacci2LeftShift(unsigned int number, const unsigned int N)
{
	/*if ((number==1) && (N==2)) {
		printf("stop");
	}/**/
	return FibNumbers2_0[N] * number + FibNumbers2_1[N] * Fibonacci2RightShift_1[number];
}

unsigned int cUniversalCoderFastFibonacci2::decode(int bytePerNumber, unsigned char* encodedBuffer, unsigned char* decodedBuffer, unsigned int count) 
{
	mBits.SetMemoryForRead(encodedBuffer,0);
	//(count+7)/8;//zaokrouhli nahoru na byte
	/*
	printf("\n--------\n");
	for (int i=0;i<ncount;i++) {
		printf(",%d",mBits.GetByte(i));
	}
	/**/
	register unsigned int n=0;
	register int actState=0;
	register int tabPos;
	register int readByte;
	register int lenA=0;
	//int stopon=16363;
	unsigned int j=0,i=0;

	while (j<count) {

		readByte=encodedBuffer[i++];

		/*
		for (int z=7;z>=0;z--) {
			printf("%d",(readByte>>z)&1);
		}
		printf(",");/**/
		tabPos=(actState)+readByte;
		actState=Fibonacci2MapTable[tabPos].newState;
		if (lenA>0) {//prvni je se shiftem pokud je predchozi
			n+=Fibonacci2LeftShift(Fibonacci2MapTable[tabPos].code0,lenA);
		} else {
			n=Fibonacci2MapTable[tabPos].code0;
		}
		lenA+=Fibonacci2MapTable[tabPos].bits0;
		if (Fibonacci2MapTable[tabPos].out0) {
			/*
			printf(",%d",n);
			if (n==stopon) {
				printf("stop");
			}/**/
			*((unsigned int *)(decodedBuffer+j*bytePerNumber))=n;
			j++;
			
			
			lenA=0;
			n=0;
		} else continue;
		if (Fibonacci2MapTable[tabPos].bits1) {
			n=Fibonacci2MapTable[tabPos].code1;
			lenA=Fibonacci2MapTable[tabPos].bits1;
			if (Fibonacci2MapTable[tabPos].out1) { 
			/*
			printf(",%d",n);
			if (n==stopon) {
				printf("stop");
			}/**/
				*((unsigned int *)(decodedBuffer+j*bytePerNumber))=n;
				j++;
				lenA=0;
				n=0;
			} else continue;
		} else continue;
		if (Fibonacci2MapTable[tabPos].bits2) {
			n=Fibonacci2MapTable[tabPos].code2;
			lenA=Fibonacci2MapTable[tabPos].bits2;
			if (Fibonacci2MapTable[tabPos].out2) { 
			/*
			printf(",%d",n);
			if (n==stopon) {
				printf("stop");
			}/**/
				*((unsigned int *)(decodedBuffer+j*bytePerNumber))=n;
				j++;
				lenA=0;
				n=0;
			} else continue;
		} else continue;
		if (Fibonacci2MapTable[tabPos].bits3) {
			n=Fibonacci2MapTable[tabPos].code3;
			lenA=Fibonacci2MapTable[tabPos].bits3;
			if (Fibonacci2MapTable[tabPos].out3) { 
			/*
			printf(",%d",n);
			if (n==stopon) {
				printf("stop");
			}/**/
				*((unsigned int *)(decodedBuffer+j*bytePerNumber))=n;
				j++;	
				lenA=0;
				n=0;
			} else continue;
		} else continue;
		if (Fibonacci2MapTable[tabPos].bits4) {
			n=Fibonacci2MapTable[tabPos].code4;
			lenA=Fibonacci2MapTable[tabPos].bits4;
			if (Fibonacci2MapTable[tabPos].out4) { 
			/*
			printf(",%d",n);
			if (n==stopon) {
				printf("stop");
			}/**/
				*((unsigned int *)(decodedBuffer+j*bytePerNumber))=n;
				j++;
				lenA=0;
				n=0;
			} else continue;
		} else continue;
	}
	return i;
}

unsigned int cUniversalCoderFastFibonacci2::encode(int bytePerNumber, const unsigned char * sourceBuffer, unsigned char * encodedBuffer, unsigned int count) 
{
	/**ENCODE START**
	unsigned int mask=0xFFFFFFFF>>(32-bytePerNumber*8);
	unsigned int n;
	for (unsigned int i=0;i<count;i++) {
		n=(*((unsigned int *)(sourceBuffer+i*bytePerNumber)))&mask;
		cUniversalCoderEliasDelta::Int2Code(n,&mBits);
	}
	/**/
    int j = 0;
	register unsigned int  k=0;
	unsigned int remain = 0;
	int rempos = 0;
	unsigned short vals[4];
	 int bytes, bits;
	int valcount = 0; 
	unsigned int mask=0xFFFFFFFF>>(32-bytePerNumber*8);
	unsigned short *buf=(unsigned short *)encodedBuffer;
	for (unsigned int i=0; i<count; i++) 
	{
		unsigned int num = (*((unsigned int *)(sourceBuffer+i*bytePerNumber)))&mask;
		num = cCoderCommon::Increment(bytePerNumber, num);

		if (num<FibNumbers2[16])
		{
			valcount = 0;
			bytes = FibEncode16[num].bytes;
			bits = FibEncode16[num].bits;
			vals[valcount++] = FibEncode16[num].val;
		}
		else
		{	
			if (num>=FibNumbers2[32]) {//109
				
				getEstimation16_32(k,num);
			}	
			else
				if (num>=FibNumbers2[16]) {//55
				
					getEstimation16_16(k,num);
			}
			else
				printf("CHYBA CISLO MOC VELKE -> ROZSIR TABULKU\n");

			valcount = 0;
			bytes = FibEncode16[k].bytes;
			bits = FibEncode16[k].bits;
		
			vals[valcount++] = FibEncode16[k].val;
			num -= FibEncode16[k].from;


			if (bytes==2) 
			{
				if (num<FibNumbers2[16]) 
				{
					vals[valcount++] = 0;
				}
				else
				{
					
					getEstimation16_16(k,num);
					
					vals[valcount++] = FibEncode16[k].val;
					num -= FibEncode16[k].from;
				}
				bytes--;				
			}

			if (bytes==1) 
			{
				vals[valcount++] = FibEncode16[num].val;
				bytes--;								
			}
		} 	

		while (--valcount>0)
		{
			remain |= vals[valcount] << rempos;

				buf[j++] = remain & 0xffff;

			remain >>= 16;
		}
		
		remain |= vals[valcount] << rempos;
		rempos += bits;
		

		while (rempos>=16) {

				buf[j++] = remain & 0xffff;

			remain >>=16;
			rempos -= 16;
		}		
		// one for end
		remain |= 1 << rempos;
		rempos++;
		
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