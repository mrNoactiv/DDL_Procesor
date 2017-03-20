
#include "cUniversalCoderFibonacci3.h"
#include "math.h"

namespace common {
	namespace compression {

cStack<int> cUniversalCoderFibonacci3::mStack=cStack<int>();
cBitArray cUniversalCoderFibonacci3::mBits=cBitArray();

cUniversalCoderFibonacci3::cUniversalCoderFibonacci3()
{

}

cUniversalCoderFibonacci3::~cUniversalCoderFibonacci3()
{
}

unsigned int cUniversalCoderFibonacci3::decode(int bytePerNumber, unsigned char* encodedBuffer, unsigned char* decodedBuffer, unsigned int count) 
{
	mBits.SetMemoryForRead(encodedBuffer,0);
	unsigned int i=0;
	unsigned int n,fn;
	int inx;
	int bit0=0,bit1=0,bit2=0,bit3=0;
	//dekoduj bity
	while (i < count) {
		//nacti kod
		bit3=mBits.GetNextBitLo2Hi();
		bit2=mBits.GetNextBitLo2Hi();
		bit1=mBits.GetNextBitLo2Hi();
		if ((bit1==1) && (bit2==1) && (bit3==1)) {
			n=1;
			*((unsigned int *)(decodedBuffer+i*bytePerNumber))=n;i++;
			//printf(",%d",n);
			continue;
		}
		bit0=mBits.GetNextBitLo2Hi();
		if ((bit0==1) && (bit1==1) && (bit2==1)) {
			n=2;
			*((unsigned int *)(decodedBuffer+i*bytePerNumber))=n;i++;
			//printf(",%d",n);
			continue;
		}
		fn=0;
		inx=0;
		while (!((bit0==1) && (bit1==1) && (bit2==1))) { //dokud nejsou 3x1
			if (bit3) {
				fn+=FibNumbers3[inx];
			}
			inx++;
			bit3=bit2;
			bit2=bit1;
			bit1=bit0;
			bit0=mBits.GetNextBitLo2Hi();
		}
		n=FibNumbers3sum[inx-1]+fn+1;
		*((unsigned int *)(decodedBuffer+i*bytePerNumber))=n;i++;
		//printf(",%d",n);
	}
	unsigned int totalBitsRead=mBits.GetBitRead();
	return (totalBitsRead+7)/8;//+7 pocitej i nedokoncene byte



}
unsigned int cUniversalCoderFibonacci3::encode(int bytePerNumber, const unsigned char * sourceBuffer, unsigned char* encodedBuffer, unsigned int count) 
{
	mBits.SetMemoryForWrite(encodedBuffer,0);
	/**ENCODE START**/
	unsigned int mask=0xFFFFFFFF>>(32-bytePerNumber*8);
	unsigned int n;
	for (unsigned int i=0;i<count;i++) {
		n=(*((unsigned int *)(sourceBuffer+i*bytePerNumber)))&mask;
		n = cCoderCommon::Increment(bytePerNumber, n);
		Int2Code(n,&mBits);
	}
	/**ENCODE END**/
	mBits.StoreRemainingBits();
	return mBits.GetBitSizeCorrect();
}

//Encode integer into Fibonacci code
// \param Number number to encode
// \param mBits cFastBitArray storing the bits
unsigned int cUniversalCoderFibonacci3::Int2Code(const unsigned int Number, cBitArray *mBits)
{
	int countFib = DIM(FibNumbers3) - 1;
	int n = DIM(FibNumbers3sum) - 1;
	unsigned int num = Number;	
	int bit;
	//Sn-2<Number<=Sn-1
	if (num==1) {
		mBits->SetNextBitLo2HiNoCheck(1);
		mBits->SetNextBitLo2HiNoCheck(1);
		mBits->SetNextBitLo2HiNoCheck(1);
		return 0;
	}
	if (num==2) {
		mBits->SetNextBitLo2HiNoCheck(0);
		mBits->SetNextBitLo2HiNoCheck(1);
		mBits->SetNextBitLo2HiNoCheck(1);
		mBits->SetNextBitLo2HiNoCheck(1);
		return 0;
	}
	while (FibNumbers3sum[n] >= num)
	{
		n--;
	}
	
	//Q=N-(Sn-2)-1
	unsigned int q=num-FibNumbers3sum[n]-1;
	if (q==0) {
		countFib=0;
	}
	else {
		while (FibNumbers3[countFib] > q)
		{
			countFib--;
		}
	}
	//pridej pocatecni nuly
	while (n>countFib) {
		mStack.Push(0);
		n--;
	}
    
	while (countFib >= 0)
	{
		if (FibNumbers3[countFib] <= q)
		{
			q -= FibNumbers3[countFib];
			mStack.Push(1);
		}
		else
		{
			mStack.Push(0);
		}
		countFib--;
	}
	
	while (!mStack.Empty()) {
		bit=mStack.Pop();
		mBits->SetNextBitLo2HiNoCheck((unsigned char)bit);
	}

	
	mBits->SetNextBitLo2HiNoCheck(0);
	mBits->SetNextBitLo2HiNoCheck(1);
	mBits->SetNextBitLo2HiNoCheck(1);
	mBits->SetNextBitLo2HiNoCheck(1);
	return 0;
}
}}

