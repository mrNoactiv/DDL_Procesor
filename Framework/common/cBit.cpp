
#include "cBit.h"

#include "math.h"
#include <stdio.h>

#define IGNOREFASTARRAY 1
cFastBitArray cBit::mBits=cFastBitArray();
//creates the newinstance of cUniversalCoder
//\param CompressType defines a compression method
//\param ParameterM sets the base parameter for GOLOMB code. For other codes the parameter is not applied 
cBit::cBit()
{
	
}


cBit::~cBit()
{
}

//Prints the input bit bufffer. The buffer is represented by bytes.
//Each byte is printed so that the most significant bit is print last.
//\param number of bytes and char buffer
void cBit::PrintBuffer(unsigned int ncount,unsigned char* buffer)
{
	mBits.SetMemoryForRead(buffer,0);
	for (unsigned int i=0;i<ncount*8;i++) {
		printf("%d",mBits.GetNextBit());
	}
	printf("\n");
}
//Prints the input bit bufffer. The buffer is represented by bytes.
//Each byte is printed so that the most significant bit is print first.
//\param number of bytes and char buffer
void cBit::PrintBufferHiLo(unsigned int ncount,unsigned char* buffer)
{
	mBits.SetMemoryForRead(buffer,0);
	for (unsigned int i=0;i<ncount*8;i++) {
		printf("%d",mBits.GetNextBitHi2Lo());
	}
	printf("\n");
}
//prints all bytes of buffer.
void cBit::PrintBytes(unsigned int ncount,unsigned char* buffer) {
	for (unsigned int i=0;i<ncount;i++) {
//		int bit=(mBits.mCharArray[0]>>i)
		printf("%d ",buffer[i]);
	}
	printf("\n");

}
//get the i-th bit from 64 bit (unsigned long long) variable the most significan bit is at 0 position
//\param input the long long variable storing the bits
//\param i position of the bit
int cBit::getBit64Hi2Lo(unsigned long long * input,int i) {
	int bit= ((*input)>>(63-i))&1;
	//printf("%d ",bit);

	return bit;
}

//set the i-th bit in 64 bit (unsigned long long) variable the most significan bit is at 0 position
//\param input the long long variable storing the bits
//\param i position of the bit
void cBit::setBit64Hi2Lo(unsigned long long * input,int i,int bit) {
	(*input) = (((unsigned long long)bit) << (63-i))|(*input);
	//printf("%d ",bit);
}
//prints all bits of unsigned long long variable. The most significant bit is printed first
void cBit::PrintBits64Hi2Lo(unsigned long long * input)
{
	int ncount=64;
	for (int i=0;i<ncount;i++) {
		printf("%d",getBit64Hi2Lo(input,i));
	}
	printf("\n");
}
//gets i-th bit stored in unsigned char variable. The most significant bit is at 0 position
int cBit::getBit8Hi2Lo(unsigned char * input,int i) {
	int bit= ((*input)>>(7-i))&1;
	//printf("%d ",bit);

	return bit;
}
//sets i-th bit stored in unsigned char variable. The most significant bit is at 0 position
void cBit::setBit8Hi2Lo(unsigned char * input,int i,int bit) {
	(*input) = (((unsigned char)bit) << (7-i))|(*input);
	//printf("%d ",bit);
}

//prints all bits of unsigned char variable. The most significant bit is printed first
void cBit::PrintBits8Hi2Lo(unsigned char * input)
{
	int ncount=8;
	for (int i=0;i<ncount;i++) {
		printf("%d",getBit8Hi2Lo(input,i));
	}
	printf("\n");
}
//get the i-th bit from 64 bit (unsigned long long) variable the most significan bit is at 63 position
//\param input the long long variable storing the bits
//\param i position of the bit
int cBit::getBit64Lo2Hi(unsigned long long * input,int i) {
	int bit= ((*input)>>i)&1;
	//printf("%d ",bit);

	return bit;
}
//set the i-th bit in 64 bit (unsigned long long) variable the most significan bit is at 63 position
//\param input the long long variable storing the bits
//\param i position of the bit
void cBit::setBit64Lo2Hi(unsigned long long * input,int i,int bit) {
	(*input) = (((unsigned long long)bit) << i)|(*input);
	//printf("%d ",bit);
}

//prints all bits of unsigned long long variable. The most significant bit is printed last
void cBit::PrintBits64Lo2Hi(unsigned long long * input)
{
	int ncount=64;
	for (int i=0;i<ncount;i++) {
		printf("%d",getBit64Lo2Hi(input,i));
	}
	printf("\n");
}
//gets i-th bit stored in unsigned char variable. The most significant bit is at 7 position
int cBit::getBit8Lo2Hi(unsigned char * input,int i) {
	int bit= ((*input)>>i)&1;
	//printf("%d ",bit);

	return bit;
}
//sets i-th bit stored in unsigned char variable. The most significant bit is at 0 position
void cBit::setBit8Lo2Hi(unsigned char * input,int i,int bit) {
	(*input) = (((unsigned char)bit) << i)|(*input);
	//printf("%d ",bit);
}

//prints all bits of unsigned char variable. The most significant bit is printed last
void cBit::PrintBits8Lo2Hi(unsigned char * input)
{
	int ncount=8;
	for (int i=0;i<ncount;i++) {
		printf("%d",getBit8Lo2Hi(input,i));
	}
	printf("\n");
}
//reverses bits in unsigned char variable hi <-> lo
unsigned char cBit::reverseBits(unsigned char input) {
	unsigned char output=0;
	for (int i=0;i<8;i++) {
		int bit=getBit8Hi2Lo(&input,i);
		setBit8Lo2Hi(&output,i,bit);
	}
	return output;
}



