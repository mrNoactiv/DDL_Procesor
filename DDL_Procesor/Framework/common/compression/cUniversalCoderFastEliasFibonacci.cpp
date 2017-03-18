#include "cUniversalCoderFastEliasFibonacci.h"
#include "cUniversalCoderFibonacci2.h"
#include "log256table.h"
#include "math.h"
#define SEGMENT 32
#define SEGMENT_FILTER 0xff
#define SEGMENT_TYPE unsigned int

#define IGNOREFASTARRAY 1
#define DIRECTWRITE 1
#define X64
// #define X32
#define TAB256
//#define BSRI
//#define LOOP
namespace common {
	namespace compression {

//Fiboacci 2 numbers pocitane od 1
cStack<int> cUniversalCoderFastEliasFibonacci::mStack = cStack<int>();

cBitArray cUniversalCoderFastEliasFibonacci::mBits=cBitArray();

cUniversalCoderFastEliasFibonacci::cUniversalCoderFastEliasFibonacci()
{
}

cUniversalCoderFastEliasFibonacci::~cUniversalCoderFastEliasFibonacci()
{
}

unsigned int cUniversalCoderFastEliasFibonacci::decode(int bytePerNumber, unsigned char* encodedBuffer, unsigned char* decodedBuffer, unsigned int count) 
{
	register unsigned int n=0;
	register int actState=0;
	register int tabPos;
	int readByte;
	register unsigned int nextPlus=0;
	register int lenA=0;
	unsigned int j=0,i=0;

	while (j<count) {

		readByte=encodedBuffer[i++];

		tabPos=(actState)+readByte;
		actState=EliasFibonacciMapTable[tabPos].newState;
		
		if (lenA>=8) {
			lenA-=8;
			nextPlus+=readByte<<lenA;
			if (lenA==0) {
				*((unsigned int *)(decodedBuffer+j*bytePerNumber))=nextPlus;
				j++;
				/*
				if (nextPlus==1023) {
				printf("(%d)",nextPlus);
			}/**/
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
		
		lenA=EliasFibonacciMapTable[tabPos].bitsToRead;
		if (EliasFibonacciMapTable[tabPos].writeCount) {
			//vypise aspon 1 cislo
			nextPlus=EliasFibonacciMapTable[tabPos].write0+nextPlus;
			
			/*
			if (nextPlus==1023) {
				printf("(%d)",nextPlus);
			}/**/
			*((unsigned int *)(decodedBuffer+j*bytePerNumber))=nextPlus;
			j++;
			nextPlus=EliasFibonacciMapTable[tabPos].codePlusNext<<lenA;
			//dalsi jen pokud jsou

			if (EliasFibonacciMapTable[tabPos].write1) {
				n=EliasFibonacciMapTable[tabPos].write1;
				*((unsigned int *)(decodedBuffer+j*bytePerNumber))=n;
				j++;
			} else continue;
			if (EliasFibonacciMapTable[tabPos].write2) {
				n=EliasFibonacciMapTable[tabPos].write2;
				*((unsigned int *)(decodedBuffer+j*bytePerNumber))=n;
				j++;
			} else continue;
			if (EliasFibonacciMapTable[tabPos].write3) {
				n=EliasFibonacciMapTable[tabPos].write3;
				*((unsigned int *)(decodedBuffer+j*bytePerNumber))=n;
				j++;
			} else continue;


		}
		else {
			//pokracuju v nacitani cisla
			nextPlus+=EliasFibonacciMapTable[tabPos].codePlusNext<<lenA;
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


//unsigned int * test=new unsigned int [32];
unsigned int cUniversalCoderFastEliasFibonacci::encode(int bytePerNumber, const unsigned char* sourceBuffer, unsigned char* encodedBuffer, unsigned int count) 
{
  int  j = 0; 
	unsigned int buffer = 0;
	/*
	for (int i=0;i<32;i++) {
		test[i]=0;
	}*/
	
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
		buffer=(unsigned int)prefixEF[len].prefix;
		plen=prefixEF[len].prefixLen;

		//castecne
		shift=(SEGMENT-rempos)-plen;
		
		//rempos=1, len=4 musim posunout o 3 doleva. 8-1-4=3;
		if (shift>0) {
			remain |= buffer << shift;
//			test[-shift]++;
			rempos+=plen;
		}
		else //pretece
		{//rempos=1, len=9 musim posunout o 2 doprava.
			rempos = -shift;
			remain|=buffer>>rempos;
//			test[shift]++;
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
				if (rempos>0) {
					remain=buffer<<(32+shift);
//					test[(32-(shift))]++;
				}
				else {
					remain=0;
				}
				
			
			/**/
		}
		//CODE
		buffer=num;

		//castecne
		shift=(SEGMENT-rempos)-len;
		
		//rempos=1, len=4 musim posunout o 3 doleva. 8-1-4=3;
		if (shift>0) {
			remain |= buffer << shift;
//			test[-shift]++;
			rempos+=len;
		}
		else //pretece
		{//rempos=1, len=9 musim posunout o 2 doprava.
			rempos = -shift;
			remain|=buffer>>rempos;
//			test[shift]++;
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
				if (shift<0) {
					remain=buffer<<(32+shift);
//         			test[(32-(shift))]++;

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
	/*
	for (int i=0;i<32;i++) {
		printf(" %u",test[i]);
	}
	printf("\n");*/
    return retbits;
}

unsigned int cUniversalCoderFastEliasFibonacci::encode16(int bytePerNumber, const unsigned char * sourceBuffer, unsigned char* encodedBuffer, unsigned int count) 
{
    int  j = 0;
	register unsigned int buffer = 0;
	SEGMENT_TYPE remain = 0;
	int rempos = 0;
	unsigned int len;
	unsigned int plen;
    int shift;
	unsigned int mask=0xFFFFFFFF>>(32-bytePerNumber*8);
	SEGMENT_TYPE *buf=(SEGMENT_TYPE *)encodedBuffer;
	for (unsigned int i=0; i<count; i++) 
	{
		unsigned int num = (*((unsigned int *)(sourceBuffer + i*bytePerNumber)))&mask;
		num = cCoderCommon::Increment(bytePerNumber, num);

#ifdef LOOP
		//len=1;
		//while (num>>len) len++;
#endif
		/*
		if ((len & 32768)>0) {len=16;}
		else {while (num>>len) len++;}/**/
#ifdef TAB256
		/*LOG TABLE*/
		
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
		//PREFIX

		buffer=(prefixEF16[len].prefix)|num;
		plen=prefixEF16[len].prefixLen;
		//castecne
		shift=(SEGMENT-rempos)-plen;
		//rempos=1, len=4 musim posunout o 3 doleva. 8-1-4=3;
		if (shift>=0) {
			remain |= buffer << (shift);
//			test[-shift]++;
			rempos+=plen;
		}
		else //pretece
		{//rempos=1, len=9 musim posunout o 2 doprava.
			rempos = -shift;
			remain|=buffer>>rempos;
//			test[shift]++;
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
//					test[(32-(shift))]++;
				}
				else {
					remain=0;
				}
				
			
			/**/
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
	/*
	for (int i=0;i<32;i++) {
		printf(" %u",test[i]);
	}
	printf("\n");*/
    return retbits;
}
}}
