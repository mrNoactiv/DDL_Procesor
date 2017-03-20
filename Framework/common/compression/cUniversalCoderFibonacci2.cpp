
#include "cUniversalCoderFibonacci2.h"
#include "math.h"

namespace common {
	namespace compression {

cStack<int> cUniversalCoderFibonacci2::mStack=cStack<int>();
//Fibonacci numbers counted from 1 position
cBitArray cUniversalCoderFibonacci2::mBits=cBitArray();

cUniversalCoderFibonacci2::cUniversalCoderFibonacci2()
{

}

cUniversalCoderFibonacci2::~cUniversalCoderFibonacci2()
{
}

unsigned int cUniversalCoderFibonacci2::decode(int bytePerNumber, unsigned char* encodedBuffer, unsigned char* decodedBuffer, unsigned int count) 
{
	mBits.SetMemoryForRead(encodedBuffer,0);
	unsigned char bit,prevBit;
	int fibEnd;
	unsigned int i=0;
	unsigned int n,delka,x;
	
	//dekoduj bity
	while (i < count)  {
	    fibEnd=0;
		delka=0;
		n=0;
		x=0;
		//nacti delku jako Fibonacci kod
		prevBit=0;
		while (!fibEnd) { //kdyz nacte 1 tak konec
			bit=(unsigned char)mBits.GetNextBitLo2Hi();
			//printf("%d",bit);
			if ((prevBit==1) && (bit==1)) {//11 konec fib kodu
				break;
			}
			if (bit) {
				delka+=FibNumbers2[x];
			}
			x++;
			prevBit=bit;
		}
		n=delka;
		*((unsigned int *)(decodedBuffer+i*bytePerNumber))=n;
		//printf("(%d)",n);
		i++;
	}
	unsigned int totalBitsRead=mBits.GetBitRead();
	return (totalBitsRead+7)/8;//+7 pocitej i nedokoncene byte
}

unsigned int cUniversalCoderFibonacci2::encode(int bytePerNumber, const unsigned char* sourceBuffer, unsigned char * encodedBuffer ,unsigned int count) 
{
	mBits.SetMemoryForWrite(encodedBuffer,0);
	/**ENCODE START**/
	unsigned int mask=0xFFFFFFFF>>(32-bytePerNumber*8);
	unsigned int n;
	for (unsigned int i=0;i<count;i++) {
		n=(*((unsigned int *)(sourceBuffer+i*bytePerNumber)))&mask;
		n = cCoderCommon::Increment(bytePerNumber, n);
		cUniversalCoderFibonacci2::Int2Code(n,&cUniversalCoderFibonacci2::mBits);
	}
	/**ENCODE END**/
	mBits.StoreRemainingBits();
	return mBits.GetBitSizeCorrect();
}

//Fibonacci <<F k-Left shift
unsigned int cUniversalCoderFibonacci2::leftShift(unsigned int number,int shift) 
{
	unsigned int shifted=0;
	unsigned int n=number;
	int bit;
	int inCode=0;
	//Preved na kod
	int i = DIM(FibNumbers2) - 1;
	while (FibNumbers2[i] > n)
	{
		i--;
	}
	while (i >= 0)
	{
		inCode=(inCode<<1);
		if (FibNumbers2[i] <= n)
		{
			n -= FibNumbers2[i];
			inCode+=1;
		}
		i--;
	}
	//posun
	for (int i=0;i<32;i++) {
		bit=(inCode>>i)&1;
		if (bit) {
			shifted+=FibNumbers2[i+shift];
		}
	}
	return shifted;
}

/// Encode number into output memory
/// \param Number number which should be encoded into memory
/// \return 
///		- 0 if encoding of number into memory was ok
///		- 1 if the memory is full. 
unsigned int cUniversalCoderFibonacci2::Int2Code(const unsigned int Number, cBitArray *mBits)
{
    int i = DIM(FibNumbers2) - 1;
	unsigned int n = Number ;	// Fibonacci codes cannot encode 0
	/*ver 2*/
	unsigned long long buffer=0;
	while (FibNumbers2[i] > n)
	{
		i--;
	}
	unsigned int len=i+1;
	while (i >= 0)
	{
		buffer=(buffer<<1);
		if (FibNumbers2[i] <= n)
		{
			n -= FibNumbers2[i];
			buffer|=1;//mStack.Push(1);
		}
		i--;
	}
	for (int i=0;i<len;i++) {
	   mBits->SetNextBitLo2HiNoCheck((buffer>>i)&1);
	}
	mBits->SetNextBitLo2HiNoCheck(1);
/**

	while (FibNumbers2[i] > n)
	{
		i--;
	}
	while (i >= 0)
	{
		if (FibNumbers2[i] <= n)
		{
			n -= FibNumbers2[i];
			mStack.Push(1);
		}
		else
		{
			mStack.Push(0);
		}
		i--;
	}
	while (!mStack.Empty())
	{
		i = mStack.Pop();
		mBits->SetNextBitLo2HiNoCheck((unsigned char)i);
	}
	mBits->SetNextBitLo2HiNoCheck(1);
/**/	

	return 0;
}
}}

