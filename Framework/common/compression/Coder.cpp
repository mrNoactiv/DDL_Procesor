#include "Coder.h"
#include "cUniversalCoderEliasDelta.h"
#include "cUniversalCoderFastEliasDelta.h"
#include "cUniversalCoderFibonacci2.h"
#include "cUniversalCoderEliasFibonacci.h"
#include "cUniversalCoderFibonacci3.h"
#include "cUniversalCoderFastFibonacci2.h"
#include "cUniversalCoderFastFibonacci3.h"
#include "cUniversalCoderFastEliasFibonacci.h"
#include "cUniversalCoderFixedLength.h"
#include "log256table.h"

namespace common {
	namespace compression {

uint Coder::GetSize(int method, int bytePerNumber, char* sourceBuffer, uint count)
{
	if (method == FIXED_LENGTH_CODING)
	{
		return cUniversalCoderFixedLength::GetCodewordSize(bytePerNumber, (uchar*)sourceBuffer, count);
	}
	else if (method == FIXED_LENGTH_CODING_ALIGNED)
	{
		return cUniversalCoderFixedLength::GetCodewordSize_aligned(bytePerNumber, (uchar*)sourceBuffer, count);
	}
	else
	{
		uint retSize = 0;
		for (uint i = 0; i < count; i++)
		{
			uint num = *((uint *) (sourceBuffer + (i*bytePerNumber)));
			retSize += GetSize(num);
		}
		return retSize;
	}
}

// returns length of the integer value
uint Coder::GetSize(uint value)
{
	register unsigned int t, tt;
	if (tt = value >> 16)
	{
		return (t = tt >> 8) ? 24 + LogTable256_1[t] : 16 + LogTable256_1[tt];
	}
	else
	{
		return (t = value >> 8) ? 8 + LogTable256_1[t] : LogTable256_1[value];
	}
}

unsigned int Coder::estimateSizeInBits(int method, int bytePerNumber, char* sourceBuffer, unsigned int count) {
	switch (method) {
	case FIXED32:
		{
			return count*bytePerNumber*8;
		}
	case ELIAS_DELTA_FAST:
		return cUniversalCoderFastEliasDelta::estimateSizeInBits(bytePerNumber, (unsigned char*)sourceBuffer, count);
	}	
	return 0;
}

unsigned int Coder::encode(int method,int bytePerNumber, const char* sourceBuffer, char* encodedBuffer, unsigned int count)
{
	const unsigned char* ucSourceBuffer = (unsigned char*)sourceBuffer;
	unsigned char* ucEncodedBuffer = (unsigned char*)encodedBuffer;

	switch (method) {
	/*case MEMCPY32:
		{
			unsigned int mask=0xFFFFFFFF>>(32-bytePerNumber*8);
			unsigned int n;
			switch (bytePerNumber) {
			case 1:{
				unsigned char * src=(unsigned char *)sourceBuffer;
				unsigned char * trg=(unsigned char *)encodedBuffer;
				memcpy(trg,src,count);

				   }
				break;
			case 2:{
				unsigned short * src=(unsigned short *)sourceBuffer;
				unsigned short * trg=(unsigned short *)encodedBuffer;
				memcpy(trg,src,count*2);
			}
				break;
			case 3:
				{
				unsigned int * src=(unsigned int *)sourceBuffer;
				unsigned int * trg=(unsigned int *)encodedBuffer;
				memcpy(trg,src,count*3);
	
				   }
				break;
			case 4:{
				unsigned int * src=(unsigned int *)sourceBuffer;
				unsigned int * trg=(unsigned int *)encodedBuffer;
				memcpy(trg,src,count*4);
	
				   }
				break;
			}
			return count*4*8;
		}/**/
	case FIXED32:
		{
			unsigned int mask=0xFFFFFFFF>>(32-bytePerNumber*8);
			unsigned int n;
			switch (bytePerNumber) {
			case 1: {
				unsigned char * src=(unsigned char*)ucSourceBuffer;
				unsigned char * trg=ucEncodedBuffer;
			
				for (unsigned int i=0;i<count;i++) 
				{
					 *(trg+i)=*(src+i);
				}
				}
				break;
			case 2: {
				unsigned short * src=(unsigned short *)sourceBuffer;
				unsigned short * trg=(unsigned short *)encodedBuffer;
				
				for (unsigned int i=0;i<count;i++) 
				{
					 *(trg+i)=*(src+i);
				}
				}
				break;
			case 3:
			
			case 4: {
				unsigned int * src=(unsigned int *)sourceBuffer;
				unsigned int * trg=(unsigned int *)encodedBuffer;
				
				for (unsigned int i=0;i<count;i++)
				{
					 *(trg+i)=*(src+i);
				}
				}
				break;
			}
			return count*4*8;
		}
	case ELIAS_DELTA:
		return cUniversalCoderEliasDelta::encode(bytePerNumber, ucSourceBuffer, ucEncodedBuffer,count);
	case ELIAS_DELTA_FAST:
		if (bytePerNumber<=2) {
		  return cUniversalCoderFastEliasDelta::encode16(bytePerNumber, ucSourceBuffer, ucEncodedBuffer,count);
		}
		else {
          return cUniversalCoderFastEliasDelta::encode(bytePerNumber, ucSourceBuffer, ucEncodedBuffer,count);
		}
	case ELIAS_FIBONACCI:
		return cUniversalCoderEliasFibonacci::encode(bytePerNumber, ucSourceBuffer, ucEncodedBuffer,count);
	case ELIAS_FIBONACCI_FAST:
		if (bytePerNumber<=2) {
          return cUniversalCoderFastEliasFibonacci::encode16(bytePerNumber, ucSourceBuffer, ucEncodedBuffer,count);  
		}
		else {
		  return cUniversalCoderFastEliasFibonacci::encode(bytePerNumber, ucSourceBuffer, ucEncodedBuffer,count);
		}
	case FIBONACCI2:
		return cUniversalCoderFibonacci2::encode(bytePerNumber, ucSourceBuffer, ucEncodedBuffer,count);
	case FIBONACCI2_FAST:
		return cUniversalCoderFastFibonacci2::encode(bytePerNumber, ucSourceBuffer, ucEncodedBuffer,count);
	case FIBONACCI3:
		return cUniversalCoderFibonacci3::encode(bytePerNumber, ucSourceBuffer, ucEncodedBuffer,count);
	case FIBONACCI3_FAST:
		return cUniversalCoderFastFibonacci3::encode(bytePerNumber, ucSourceBuffer, ucEncodedBuffer,count);
	case FIXED_LENGTH_CODING:
		return cUniversalCoderFixedLength::encode(bytePerNumber, ucSourceBuffer, ucEncodedBuffer,count);
	case FIXED_LENGTH_CODING_ALIGNED:
		return cUniversalCoderFixedLength::encode_aligned(bytePerNumber, ucSourceBuffer, ucEncodedBuffer,count);
	default:
		printf("Coder error: Unsuported method!");
		break;
	}	
	return 0;
}

unsigned int Coder::decode(int method,int bytePerNumber, char* encodedBuffer, char* decodedBuffer, unsigned int count) 
{
	unsigned int ret;
	unsigned char* ucEncodedBuffer = (unsigned char*)encodedBuffer;
	unsigned char* ucDecodedBuffer = (unsigned char*)decodedBuffer;

	switch (method) {
	case FIXED32:{
			unsigned int mask=0xFFFFFFFF>>(32-bytePerNumber*8);
			unsigned int n;
			for (unsigned int i=0;i<count;i++) {
				n=(*((unsigned int *)(encodedBuffer+i*bytePerNumber)))&mask;
				*((unsigned int *)(decodedBuffer+i*bytePerNumber))=n;
			}
			}
		ret = count*bytePerNumber;
		break;
	case ELIAS_DELTA:
		ret = cUniversalCoderEliasDelta::decode(bytePerNumber, ucEncodedBuffer, ucDecodedBuffer,count);
		break;
	case ELIAS_DELTA_FAST:
		ret = cUniversalCoderFastEliasDelta::decode(bytePerNumber, ucEncodedBuffer, ucDecodedBuffer,count);
		break;
	case ELIAS_FIBONACCI:
		ret = cUniversalCoderEliasFibonacci::decode(bytePerNumber, ucEncodedBuffer, ucDecodedBuffer,count);
		break;
	case ELIAS_FIBONACCI_FAST:
		ret = cUniversalCoderFastEliasFibonacci::decode(bytePerNumber, ucEncodedBuffer, ucDecodedBuffer,count);
		break;
	case FIBONACCI2:
		ret = cUniversalCoderFibonacci2::decode(bytePerNumber, ucEncodedBuffer, ucDecodedBuffer,count);
		break;
	case FIBONACCI2_FAST:
		ret = cUniversalCoderFastFibonacci2::decode(bytePerNumber, ucEncodedBuffer, ucDecodedBuffer,count);
		break;
	case FIBONACCI3:
		ret = cUniversalCoderFibonacci3::decode(bytePerNumber, ucEncodedBuffer, ucDecodedBuffer,count);
		break;
	case FIBONACCI3_FAST:
		ret = cUniversalCoderFastFibonacci3::decode(bytePerNumber, ucEncodedBuffer, ucDecodedBuffer,count);
		break;
	case FIXED_LENGTH_CODING:
		ret = cUniversalCoderFixedLength::decode(bytePerNumber, ucEncodedBuffer, ucDecodedBuffer,count);
		break;
	case FIXED_LENGTH_CODING_ALIGNED:
		ret = cUniversalCoderFixedLength::decode_aligned(bytePerNumber, ucEncodedBuffer, ucDecodedBuffer,count);
		break;
	default:
		printf("Coder error: Unsuported method!");
		break;
	}

	if (method != FIXED32 && method != FIXED_LENGTH_CODING && method != FIXED_LENGTH_CODING_ALIGNED)
	{
		if (bytePerNumber == 1)
		{
			unsigned char* data = (unsigned char*)decodedBuffer;
			for (unsigned int i = 0 ; i < count ; i++)
			{
				(*data)--;
				data++;
			}
		}
		else if (bytePerNumber == 2)
		{
			unsigned short* data = (unsigned short*)decodedBuffer;
			for (unsigned int i = 0 ; i < count ; i++)
			{
				(*data)--;
				data++;
			}
		}
		else if (bytePerNumber == 4)
		{
			unsigned int* data = (unsigned int*)decodedBuffer;
			for (unsigned int i = 0 ; i < count ; i++)
			{
				(*data)--;
				data++;
			}
		}
		else 
		{
			printf("Coder error: Unsuported number of bytes per number!");
		}
	}

	return ret;
}

char * Coder::methodName(int method) {
  switch (method) {
	case FIXED32:
		return "Fixed 32 bit";
	case ELIAS_DELTA:
		return "Elias-delta";
	case ELIAS_DELTA_FAST:
		return "Elias-delta fast";
	case ELIAS_FIBONACCI:
		return "Elias-Fibonacci";
	case ELIAS_FIBONACCI_FAST:
		return "Elias-Fibonacci fast";
	case FIBONACCI2:
		return "Fibonacci 2";
	case FIBONACCI2_FAST:
		return "Fibonacci 2 fast";
	case FIBONACCI3:
		return "Fibonacci 3";
	case FIBONACCI3_FAST:
		return "Fibonacci 3 fast";
	case FIXED_LENGTH_CODING:
		return "Fixed-length";
	case FIXED_LENGTH_CODING_ALIGNED:
		return "Fixed-length Aligned";
	}	
  return "not implemented";
}
void Coder::print(int method, char* buffer,unsigned int bytes){
	switch (method) {//bits are in Hi2Lo
	case FIXED32:
	case ELIAS_DELTA:
	case ELIAS_DELTA_FAST:
	case ELIAS_FIBONACCI:
	case ELIAS_FIBONACCI_FAST:
		for (unsigned int i=0;i<bytes;i++) {
			for (unsigned int b=0;b<8;b++) {
				 printf("%d",(buffer[i]>>(7-b))&1);
			}
		};
		break;
	case FIBONACCI2:	////bits are in Lo2Hi
	case FIBONACCI2_FAST:	
	case FIBONACCI3:	
	case FIBONACCI3_FAST:
		for (unsigned int i=0;i<bytes;i++) {
			for (unsigned int b=0;b<8;b++) {
				 printf("%d",(buffer[i]>>(b))&1);
			}
		};
	
	}	
}
}}
