
#include "cUniversalCoderFastEliasDelta.h"
#include "cUniversalCoderEliasDelta.h"
#include "log256table.h"
#include <stdio.h>

#include "math.h"

#define SEGMENT 32
#define SEGMENT_FILTER 0xff
#define SEGMENT_TYPE unsigned int

#define IGNOREFASTARRAY 1
#define DIRECTWRITE 1
//zousob vypoctu L(n)
#define X64
// #define X32
#define TAB256
//#define BSRI
//#define LOOP

namespace common {
	namespace compression {

cBitArray cUniversalCoderFastEliasDelta::mBits=cBitArray();

cUniversalCoderFastEliasDelta::cUniversalCoderFastEliasDelta()
{

}


cUniversalCoderFastEliasDelta::~cUniversalCoderFastEliasDelta()
{
}

unsigned int cUniversalCoderFastEliasDelta::decode(int bytePerNumber, unsigned char* encodedBuffer, unsigned char* decodedBuffer, unsigned int count) 
{
	
		//(count+7)/8;//zaokrouhli nahoru na byte
	/*printf("\n--------\n");
	for (int i=0;i<ncount;i++) {
		printf(",%d",mBits.GetByte(i));
	}*/
	register unsigned int n=0;
	register int actState=0;
	register int tabPos;
	int readByte;
	register unsigned int nextPlus=0;
	register int lenA=0;
	unsigned int j=0,i=0;

	while (j<count) 
	{
		readByte = encodedBuffer[i++];
		/*
		for (int z=7;z>=0;z--) {
			printf("%d",(readByte>>z)&1);
		}
		printf(",");/**/
		tabPos=(actState)+readByte;
		actState=EliasDeltaMapTable[tabPos].newState;
		if (lenA>=8) {
			lenA-=8;
			nextPlus+=readByte<<lenA;
			if (lenA==0) {
				*((unsigned int *)(decodedBuffer+j*bytePerNumber))=nextPlus;
				j++;
				nextPlus=0;
			}
			if (lenA<8) {
				actState=lenA<<8;
				//lenA=0;
			}
			else {
			  actState=8<<8;
			}
			continue;
		}
		lenA=EliasDeltaMapTable[tabPos].lengthAct;
		if (EliasDeltaMapTable[tabPos].writeCount) {
			//vypise aspon 1 cislo
			nextPlus=EliasDeltaMapTable[tabPos].write0+nextPlus;
			*((unsigned int *)(decodedBuffer+j*bytePerNumber))=nextPlus;
			j++;
			nextPlus=EliasDeltaMapTable[tabPos].codePlusNext<<lenA;
			//dalsi jen pokud jsou
			
				if (EliasDeltaMapTable[tabPos].write1) {
					n=EliasDeltaMapTable[tabPos].write1;
					*((unsigned int *)(decodedBuffer+j*bytePerNumber))=n;
					j++;
				} else continue;
				if (EliasDeltaMapTable[tabPos].write2) {
					n=EliasDeltaMapTable[tabPos].write2;
					*((unsigned int *)(decodedBuffer+j*bytePerNumber))=n;
					j++;
				} else continue;
				if (EliasDeltaMapTable[tabPos].write3) {
					n=EliasDeltaMapTable[tabPos].write3;
					*((unsigned int *)(decodedBuffer+j*bytePerNumber))=n;
					j++;
				} else continue;
				if (EliasDeltaMapTable[tabPos].write4) {
					n=EliasDeltaMapTable[tabPos].write4;
					*((unsigned int *)(decodedBuffer+j*bytePerNumber))=n;
					j++;
				} else continue;
				if (EliasDeltaMapTable[tabPos].write5) {
					n=EliasDeltaMapTable[tabPos].write5;
					*((unsigned int *)(decodedBuffer+j*bytePerNumber))=n;
					j++;
				} else continue;
				if (EliasDeltaMapTable[tabPos].write6) {
					n=EliasDeltaMapTable[tabPos].write6;
					*((unsigned int *)(decodedBuffer+j*bytePerNumber))=n;
					j++;
				} else continue;
				if (EliasDeltaMapTable[tabPos].write7) {
					n=EliasDeltaMapTable[tabPos].write7;
					*((unsigned int *)(decodedBuffer+j*bytePerNumber))=n;
					j++;
				} else continue;

			
		}
		else {
			//pokracuju v nacitani cisla
			nextPlus+=EliasDeltaMapTable[tabPos].codePlusNext<<lenA;
		}
	}
	
	return i;

}
//! Byte swap unsigned int
inline unsigned int swap_uint32( unsigned int val )
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF ); 
    return (val << 16) | (val >> 16);
}


unsigned int cUniversalCoderFastEliasDelta::encode(int bytePerNumber, const unsigned char* sourceBuffer, unsigned char* encodedBuffer, unsigned int count) 
{
	int  j = 0;
	register unsigned int buffer = 0;
	 SEGMENT_TYPE remain = 0;
	 int rempos = 0;
	unsigned int len,plen;
    int shift;
	unsigned int mask=0xFFFFFFFF>>(32-bytePerNumber*8);
	SEGMENT_TYPE *buf=(SEGMENT_TYPE *)encodedBuffer;
	for (unsigned int i=0; i<count; i++) 
	{
		unsigned int num = (*((unsigned int *)(sourceBuffer+i*bytePerNumber)))&mask;
		num = cCoderCommon::Increment(bytePerNumber, num);
		
		//while (num>>len) len++;
#ifdef LOOP
		len=1;
		if ((num & 2147483648)>0) {len=32;}
		else {while (num>>len) len++;}/**/
#endif
		/*LOG TABLE*/
#ifdef TAB256		
register unsigned int t, tt; 
if (tt = num >> 16)
{
  len = (t = tt >> 8) ? 24 + LogTable256_1[t] : 16 + LogTable256_1[tt];
}
else 
{
  len = (t = num >> 8) ? 8 + LogTable256_1[t] : LogTable256_1[num];
}/**/
#endif
#ifdef BSRI

#ifdef X32
		__asm {//to same jako:len=log2(num);
		 BSR eax,num;
		 inc eax;
		 mov len,eax;
		}
#endif
#endif
		/**/
		//PREFIX
		buffer=len;
		plen=prefixED[len].codeLen;


		//castecne
		shift=(SEGMENT-rempos)-plen;
		//rempos=1, len=4 musim posunout o 3 doleva. 8-1-4=3;
		if (shift>=0) {
			remain |= (SEGMENT_TYPE)(buffer << shift);
			rempos+=plen;

		}
		else //pretece
		{//rempos=1, len=9 musim posunout o 2 doprava.
			rempos = -shift;
			remain|=buffer>>rempos;
#ifdef X64			
			buf[j]=swap_uint32(remain);
#endif			
#ifdef X32
				__asm{
					mov ebx,dword ptr [buf]
					mov ecx,dword ptr [j] 
					mov eax,remain
					BSWAP eax
					mov dword ptr [ebx+ecx*4],eax 
				}
#endif		
				/**/
				j++;
				if (shift<=0) {
					remain=buffer<<(32+shift);
				}
				else {
					remain=0;
				}
				
			
			
		}
		//CODE
		buffer=(num&prefixED[len].numberAndMask);
		len--;

		//castecne
		shift=(SEGMENT-rempos)-len;
		//rempos=1, len=4 musim posunout o 3 doleva. 8-1-4=3;
		if (shift>=0) {
			remain |= (SEGMENT_TYPE)(buffer << shift);
			rempos+=len;
		}
		else //pretece
		{//rempos=1, len=9 musim posunout o 2 doprava.
			rempos = -shift;
			remain|=buffer>>rempos;
#ifdef X64			
			buf[j]=swap_uint32(remain);
#endif	
#ifdef X32
				__asm{					
					mov ebx,dword ptr [buf]
					mov ecx,dword ptr [j] 
					mov eax,remain
					BSWAP eax
					mov dword ptr [ebx+ecx*4],eax 
				}
#endif		
				j++;
				if (shift<=0) {
					remain=buffer<<(32+shift);
				}
				else {
					remain=0;
				}
		}

	}
	int retbits=j*SEGMENT+rempos;
	if (rempos>24) {//zapis 4 byte
#ifdef X64			
			buf[j]=swap_uint32(remain);
#endif	
#ifdef X32
				__asm{
					mov eax,remain;
					BSWAP eax;
					mov ebx,dword ptr [buf] 
					mov ecx,dword ptr [j] 
				    mov dword ptr [ebx+ecx*4],eax 

				}
		#endif		
	}
	else if (rempos>16) {//zapis 3 byte
#ifdef X64			
			*(unsigned char * )(buf+j)=*(((unsigned char *)&remain)+3);
			*((unsigned char * )(buf+j)+1)=*(((unsigned char *)&remain)+2);
			*((unsigned char * )(buf+j)+2)=*(((unsigned char *)&remain)+1);
#endif	
#ifdef X32
				__asm{
					mov eax,remain;
					BSWAP eax;
					mov ebx,dword ptr [buf] 
					mov ecx,dword ptr [j] 
				    mov word ptr [ebx+ecx*4],ax
					
					shr eax,16
					mov byte ptr [ebx+ecx*4+2],al
				}
		#endif		
	}
	else if (rempos>8) {//zapis 2 byte
#ifdef X64			
			*(unsigned char * )(buf+j)=*(((unsigned char *)&remain)+3);
			*((unsigned char * )(buf+j)+1)=*(((unsigned char *)&remain)+2);
#endif	
#ifdef X32
				__asm{
					mov eax,remain;
					BSWAP eax;
					mov ebx,dword ptr [buf] 
					mov ecx,dword ptr [j] 
				    mov word ptr [ebx+ecx*4],ax
				}
		#endif		
	}
	else if (rempos>0) {//zapis 1 byte
#ifdef X64			
			*(unsigned char * )(buf+j)=*(((unsigned char *)&remain)+3);
#endif	
		#ifdef X32
				__asm{
					mov eax,remain;//Vykona to same jako:(unsigned int *)buf[j]=reverse buffer
					BSWAP eax;
					mov ebx,dword ptr [buf] 
					mov ecx,dword ptr [j] 
				    mov byte ptr [ebx+ecx*4],al
				}
		#endif		
	}
    return retbits;
}

unsigned int cUniversalCoderFastEliasDelta::encode16(int bytePerNumber, const unsigned char* sourceBuffer, unsigned char* encodedBuffer, unsigned int count) 
{
	int  j = 0;
	register unsigned int buffer = 0;
	 SEGMENT_TYPE remain = 0;
	 int rempos = 0;
	unsigned int len,plen;
    int shift;
	unsigned int mask=0xFFFFFFFF>>(32-bytePerNumber*8);
	SEGMENT_TYPE *buf=(SEGMENT_TYPE *)encodedBuffer;
	for (unsigned int i=0; i<count; i++) 
	{
		unsigned int num = (*((unsigned int *)(sourceBuffer+i*bytePerNumber)))&mask;
		num = cCoderCommon::Increment(bytePerNumber, num);
#ifdef LOOP
		len=1;
		//while (num>>len) len++;
		
		if ((num & 32768)>0) {len=16;}
		else {while (num>>len) len++;}/**/
#endif
		/*LOG TABLE*/
#ifdef TAB256
		register unsigned int t, tt; 
		if (tt = num >> 16)
		{
		  len = (t = tt >> 8) ? 24 + LogTable256_1[t] : 16 + LogTable256_1[tt];
		}
		else 
		{
		  len = (t = num >> 8) ? 8 + LogTable256_1[t] : LogTable256_1[num];
		}/**/
#endif
#ifdef BSRI
#ifdef X32
		__asm {//to same jako:len=log2(num);
			mov ebx,num;
			xor eax,eax;
		 BSR ax,bx
		 inc eax;
		 mov len,eax;
		}
#endif
#endif
		/**/
			//buffer=len;
		//buffer=(buffer<<(len-1))|(num&prefixED[len].numberAndMask);
		//PREFIX
		buffer=prefixED[len].prefix|(num&prefixED[len].numberAndMask);
		plen=prefixED[len].codeLen+len-1;
		

		//castecne
		shift=(SEGMENT-rempos)-plen;
		//rempos=1, len=4 musim posunout o 3 doleva. 8-1-4=3;
		if (shift>=0) {
			remain |= (buffer << (shift));
			rempos+=plen;

		}
		else //pretece
		{//rempos=1, len=9 musim posunout o 2 doprava.
			rempos = -shift;
			remain|=buffer>>rempos;
#ifdef X64			
			buf[j]=swap_uint32(remain);
#endif			

#ifdef X32
				__asm{
					mov ebx,dword ptr [buf]
					mov ecx,dword ptr [j] 
					mov eax,remain
					BSWAP eax
					mov dword ptr [ebx+ecx*4],eax 
				}
#endif		
				j++;
				if (shift<=0) {
					remain=buffer<<(32+shift);
				}
				else {
					remain=0;
				}
		}
	}
	int retbits=j*SEGMENT+rempos;
	if (rempos>24) {//zapis 4 byte
#ifdef X64			
			buf[j]=swap_uint32(remain);
#endif			
		#ifdef X32
				__asm{
					mov eax,remain;
					BSWAP eax;
					mov ebx,dword ptr [buf] 
					mov ecx,dword ptr [j] 
				    mov dword ptr [ebx+ecx*4],eax 

				}
		#endif		
	}
	else if (rempos>16) {//zapis 3 byte
#ifdef X64			
			*(unsigned char * )(buf+j)=*(((unsigned char *)&remain)+3);
			*((unsigned char * )(buf+j)+1)=*(((unsigned char *)&remain)+2);
			*((unsigned char * )(buf+j)+2)=*(((unsigned char *)&remain)+1);
#endif	
		#ifdef X32
				__asm{
					mov eax,remain;
					BSWAP eax;
					mov ebx,dword ptr [buf] 
					mov ecx,dword ptr [j] 
				    mov word ptr [ebx+ecx*4],ax
					
					shr eax,16
					mov byte ptr [ebx+ecx*4+2],al
				}
		#endif		
	}
	else if (rempos>8) {//zapis 2 byte
#ifdef X64			
			*(unsigned char * )(buf+j)=*(((unsigned char *)&remain)+3);
			*((unsigned char * )(buf+j)+1)=*(((unsigned char *)&remain)+2);
#endif	
		#ifdef X32
				__asm{
					mov eax,remain;
					BSWAP eax;
					mov ebx,dword ptr [buf] 
					mov ecx,dword ptr [j] 
				    mov word ptr [ebx+ecx*4],ax
				}
		#endif		
	}
	else if (rempos>0) {//zapis 1 byte
#ifdef X64			
			*(unsigned char * )(buf+j)=*(((unsigned char *)&remain)+3);
#endif	
		#ifdef X32
				__asm{
					mov eax,remain;//Vykona to same jako:(unsigned int *)buf[j]=reverse buffer
					BSWAP eax;
					mov ebx,dword ptr [buf] 
					mov ecx,dword ptr [j] 
				    mov byte ptr [ebx+ecx*4],al
				}
		#endif		
	}
    return retbits;
}

unsigned int cUniversalCoderFastEliasDelta::estimateSizeInBits(int bytePerNumber, unsigned char* sourceBuffer,unsigned int count) 
{
	unsigned int len,size=0;
	unsigned int mask=0xFFFFFFFF>>(32-bytePerNumber*8);
	for (unsigned int i=0; i<count; i++) {
			unsigned int num = (*((unsigned int *)(sourceBuffer+i*bytePerNumber)))&mask;
		/*LOG TABLE*/
#ifdef TAB256		
register unsigned int t, tt; 
if (tt = num >> 16)
{
  len = (t = tt >> 8) ? 24 + LogTable256_1[t] : 16 + LogTable256_1[tt];
}
else 
{
  len = (t = num >> 8) ? 8 + LogTable256_1[t] : LogTable256_1[num];
}/**/
#endif
#ifdef BSRI

#ifdef X32
		__asm {//to same jako:len=log2(num);
		 BSR eax,num;
		 inc eax;
		 mov len,eax;
		}
#endif
#endif			//PREFIX
			size+=prefixED[len].codeLen+len-1;
	}
	return size;
}

}}
