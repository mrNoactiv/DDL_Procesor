
#include "cUniversalCoderEliasFibonacci.h"

namespace common {
	namespace compression {

//Fibonacci numbers from 1 position
cBitArray cUniversalCoderEliasFibonacci::mBits=cBitArray();
cStack<int> cUniversalCoderEliasFibonacci::mStack=cStack<int>();

const unsigned int cUniversalCoderEliasFibonacci::FibNumbers[] = 
{
	1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597,
	2584, 4181, 6765, 10946, 17711, 28657, 46368, 75025, 121393, 196418,
	317811, 514229, 832040, 1346269, 2178309, 3524578, 5702887, 9227465,
	14930352, 24157817, 39088169, 63245986, 102334155, 165580141, 267914296,
	433494437, 701408733, 1134903170, 1836311903
};

cUniversalCoderEliasFibonacci::cUniversalCoderEliasFibonacci()
{

}

cUniversalCoderEliasFibonacci::~cUniversalCoderEliasFibonacci()
{
}

unsigned int cUniversalCoderEliasFibonacci::decode(int bytePerNumber, unsigned char* encodedBuffer, unsigned char* decodedBuffer, unsigned int count) 
{
	mBits.SetMemoryForRead(encodedBuffer,0);
	unsigned char bit,prevBit;
	int fibEnd;
	unsigned int i=0;
	unsigned int n,delka,x;

	//dekoduj bity
	while (i < count) {
	    fibEnd=0;
		delka=0;
		n=0;
		x=0;
		//nacti delku jako Fibonacci kod
		prevBit=0;
		while (!fibEnd) { //kdyz nacte 1 tak konec
			bit=(unsigned char)mBits.GetNextBitHi2Lo();
			//printf("%d",bit);
			if ((prevBit==1) && (bit==1)) {//11 konec fib kodu
				break;
			}
			if (bit) {
				delka+=cUniversalCoderEliasFibonacci::FibNumbers[x];
			}
			x++;
			prevBit=bit;
		}
		x=delka-1;
		n=n|(1<<(x));
		while (x>0) {
			x--;	
			bit=(unsigned char)mBits.GetNextBitHi2Lo();
			//printf("%d",bit);
			n=n|(bit<<x);
		} 
		*((unsigned int *)(decodedBuffer+i*bytePerNumber))=n;
		//printf("(%d)",n);
		i++;
	}

	unsigned int totalBitsRead=mBits.GetBitRead();
	return (totalBitsRead+7)/8;//+7 pocitej i nedokoncene byte

}
unsigned int cUniversalCoderEliasFibonacci::encode(int bytePerNumber, const unsigned char* sourceBuffer, unsigned char* encodedBuffer, unsigned int count) 
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




/// Encode number into output memory
/// \param Number number which should be encoded into memory
/// \return 
///		- 0 if encoding of number into memory was ok
///		- 1 if the memory is full. 
unsigned int cUniversalCoderEliasFibonacci::Int2Code(const unsigned int Number, cBitArray *mBits)
{
	unsigned int tmp,delka,add=0;
	//zjisti pocet bitu cisla
	tmp=Number;
	delka=1;
	//zjisti delku cisla a zapis do x
	if (tmp>>delka) {
		add=1;
		tmp=tmp>>1;
	}/**/
	while (tmp>>delka) {
		delka++;
	}
	if (add) delka++;
	// zapis delku pomoci fibonacci
	// cUniversalCoderFibonacci2::Int2Fib(delka-1,mBits);
	unsigned int n=delka;
	int i = DIM(FibNumbers) - 1;	
	while (FibNumbers[i] > n)
	{
		i--;
	}
	while (i >= 0)
	{
		if (FibNumbers[i] <= n)
		{
			n -= FibNumbers[i];
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
		//mBits.SetBit(m_CurrentBit++, i);
		mBits->SetNextBitHi2LoNoCheck((unsigned char)i);
	}
	//mBits.SetBit(m_CurrentBit++, 1);
    mBits->SetNextBitHi2LoNoCheck((unsigned char)1);

	//zapis binarni cislo bez uvodni 1
	for (int i=delka-2;i>=0;i--) {
		mBits->SetNextBitHi2LoNoCheck((unsigned char)((Number>>i)&1));
	}
	
	return 0;
}
}}

