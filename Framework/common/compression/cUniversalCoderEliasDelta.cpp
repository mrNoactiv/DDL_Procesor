
#include "cUniversalCoderEliasDelta.h"
#include "math.h"

namespace common {
	namespace compression {

cBitArray cUniversalCoderEliasDelta::mBits=cBitArray();

cUniversalCoderEliasDelta::cUniversalCoderEliasDelta()
{
}

cUniversalCoderEliasDelta::~cUniversalCoderEliasDelta()
{
}
unsigned int cUniversalCoderEliasDelta::decode(int bytePerNumber, unsigned char* encodedBuffer, unsigned char* decodedBuffer, unsigned int count) 
{
	mBits.SetMemoryForRead(encodedBuffer,0);
	unsigned char bit;
	unsigned int i=0;
	unsigned int n,delka,x;
	//dekoduj bity
	while (i < count) {
		delka=1;
		n=0;
		x=0;
		//nacti delku x, binrani kod
		bit=(unsigned char)mBits.GetNextBitHi2Lo();
		//printf("%d",bit);
		while (!bit) { //kdyz nacte 1 tak konec
			  delka++;
			  bit=(unsigned char)mBits.GetNextBitHi2Lo();
			  //printf("%d",bit);
		}
		//nacti x, binarni kod
		delka--;
		x=x|(1<<delka);
		while (delka>0) {
			delka--;	
			bit=(unsigned char)mBits.GetNextBitHi2Lo();	
			//printf("%d",bit);
			x=x|(bit<<delka);
		} 

		//nacti binarni cislo delky x, zapis 1 dopredu
		x--;
		n=n|(1<<(x));
		while (x>0) {
			x--;	
			bit=(unsigned char)mBits.GetNextBitHi2Lo();
			//printf("%d",bit);
			n=n|(bit<<x);
		} 
		*((unsigned int *)(decodedBuffer+i*bytePerNumber))=n;
		i++;
		//printf(",%d",n);
	}
	unsigned int totalBitsRead=mBits.GetBitRead();
	return (totalBitsRead+7)/8;//+7 pocitej i nedokoncene byte
}

unsigned int cUniversalCoderEliasDelta::encode(int bytePerNumber, const unsigned char*sourceBuffer, unsigned char* encodedBuffer, unsigned int count) 
{
	mBits.SetMemoryForWrite(encodedBuffer,0);
	/**ENCODE START**/
	unsigned int mask=0xFFFFFFFF>>(32-bytePerNumber*8);
	unsigned int n;
	for (unsigned int i=0;i<count;i++) {
		n=(*((unsigned int *)(sourceBuffer+i*bytePerNumber)))&mask;
		n = cCoderCommon::Increment(bytePerNumber, n);
		Int2Code(n,&cUniversalCoderEliasDelta::mBits);
	}
	/**ENCODE END**/
	mBits.StoreRemainingBits();
	return mBits.GetBitSizeCorrect();
}


/// Encode number into output memory
/// \param Number number which should be encoded into memory
/// \param mBits cFastBitArray storing the bits
/// \return 
///		- 0 if encoding of number into memory was ok
///		- 1 if the memory is full. 
unsigned int cUniversalCoderEliasDelta::Int2Code(const unsigned int Number, cBitArray *mBits)
{
	unsigned int tmp,delka,x,add=0;
		//zjisti pocet bitu cisla
		tmp=Number;
		//zjisti delku cisla a zapis do x
		x=1;
		if (tmp>>x) {
			add=1;
			tmp=tmp>>1;
		}/**/
		while (tmp>>x) {
			x++;
		}
		if (add) x++;
		//zjisti delku x
		delka=1;
		while ((x>>delka)) {
			delka++;
		}	
		//zapis delka-1 nul
		for (unsigned int i=0;i<delka-1;i++) {
			mBits->SetNextBitHi2LoNoCheck(0);
		}
		//zapis x
		for (int i=delka-1;i>=0;i--) {
			mBits->SetNextBitHi2LoNoCheck((x>>i)&1);
		}
		//zapis binarni cislo bez uvodni 1
		for (int i=x-2;i>=0;i--) {
			mBits->SetNextBitHi2LoNoCheck((Number>>i)&1);
		}
	return 0;
}
}}

