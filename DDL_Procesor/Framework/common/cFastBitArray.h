/**
*	\file cFastBitArray.h
*	\author Radim Baca
*	\version 0.1
*	\date mar 2007
*	\brief Fast bit retrieval from char stream. Only for sequential read and write
*	\changes 
*/

#ifndef __cFastBitArray_h__
#define __cFastBitArray_h__

#include <assert.h>
#include <string.h>

#include "common/stream/cCharStream.h"
#include "common/cBitString.h"

using namespace common::stream;

/**
*	Bit retrieval from char stream. This class can only read and write bits sequentialy. 
* Doesn't have it's own memory.
*
*	\author Radim Baca
*	\version 0.1
*	\date mar 2007
**/
class cFastBitArray
{
	cCharStream *mStream;
	unsigned int mBitSize;
	unsigned int mBitOffset;
	
	unsigned int mCapacity;
	unsigned int mSeek;
	unsigned char mActualChar;
	unsigned char mActualCharCounter;

	unsigned int mStatus;
	unsigned int mType;

public:
	static const unsigned int STATUS_END = 0;
	static const unsigned int STATUS_READ = 1;
	static const unsigned int STATUS_WRITE = 2;

	static const unsigned int READ_BYTES_FROM_LOWEST_BIT = 0;
	static const unsigned int READ_BYTES_FROM_HIGHEST_BIT = 1;

	unsigned char* mCharArray;
	unsigned int mByteSize;

	cFastBitArray(unsigned int type = READ_BYTES_FROM_LOWEST_BIT);
	~cFastBitArray();

	inline unsigned char &GetByte(const int Index) const;
	inline unsigned long long &Get64(const int Index) const;
	inline unsigned int &Get32(const int Index) const;
	inline unsigned short &Get16(const int Index) const;
	inline void SetByte(const int Index, const unsigned char &byte);
	inline void AddBits(uint value, uint bitLength);
	inline uint GetBits(uint bitLength);
    inline void SetNextByte( const unsigned char &byte);
	inline unsigned char SetNext32reverseWithLen( const unsigned int value,unsigned int bytes);
	inline void SetNext64( const unsigned long long &value); //new
    inline void SetNext32( const unsigned int &value);
    inline void SetNext16( const unsigned short &value);
	inline int GetNextBit();
	inline unsigned int GetBitRead();
	inline int GetNextBitHi2Lo();
    inline int GetNextBitLo2Hi();
	inline bool SetNextBit(int bit);
	inline bool SetNextBitHi2Lo(unsigned char bit);
	inline void SetNextBitHi2LoNoCheck(unsigned char bit); //new
	inline bool SetNextBitLo2Hi(unsigned char bit);
	inline void SetNextBitLo2HiNoCheck(unsigned char bit); //new

	inline int GetByteSize();
	inline int GetBitSize();
	inline int Bits2Bytes(const int BitCount) const;

	inline void SetMemoryForRead(unsigned char *stream, unsigned int byteSize);
	inline void SetMemoryForWrite(unsigned char *stream, unsigned int capacity);
	inline void StoreLastByte();
	inline void SetCharStreamForRead(cCharStream *stream);
	inline void SetCharStreamForWrite(cCharStream *stream);
	inline void Store();
	inline void StoreBitCorrect();
    inline int GetBitSizeCorrect();
	inline void StoreRemainingBits();
	inline unsigned char* GetCharArray();
};

inline unsigned int cFastBitArray::GetBitRead() {
	return mSeek*8+mActualCharCounter;
}

inline void cFastBitArray::SetMemoryForRead(unsigned char *stream, unsigned int byteSize)
{
	mBitOffset = 0;
	mByteSize = byteSize;
	mCharArray = stream;
	mCapacity = mByteSize;
	mSeek = 0;
	mActualChar = mCharArray[mSeek];
	mActualCharCounter = 0;
	#ifndef NDEBUG
	mStatus = STATUS_READ;
	#endif
}


/**
*	Set memory pointer. During the bit operation it will write directly to this memory.
*	Method also lock the char stream, because Store() method has to be called after all bit operations are done.
*/
inline void cFastBitArray::SetMemoryForWrite(unsigned char *stream, unsigned int capacity)
{
	mCharArray = stream;
	mCapacity = capacity;
	mByteSize = 0;
	mBitOffset = 0;
	mBitSize = 0;
	mActualChar = 0;
	mActualCharCounter = 0;
	mSeek = 0;
	#ifndef NDEBUG
	//mStream->LockStream();
	mStatus = STATUS_WRITE;
	#endif
}

/**
* Store the rest of the information into the char stream. Unlock the char stream. 
* This method should be used only with SetCharStreamForWrite() method, because it writes 
* the size of the encoded stream at the begining. If you used SetMemoryForWrite() method
* use the StoreLastByte() instead.
*/
void cFastBitArray::StoreLastByte()
{
	if (mActualCharCounter < 8)
	{
		mActualChar >>= 8 - mActualCharCounter;
	}
	mCharArray[mSeek] = mActualChar;
	mSeek++;
	mByteSize++;

	#ifndef NDEBUG
	mStatus = STATUS_END;
	#endif
}

/**
*	Read char pointer from char stream without copying the whole array from stream. 
*	First read size of the char array and then pointer. Move char stream mSeek pointer.
*	Also read first char for sequential bit reading.
*/
inline void cFastBitArray::SetCharStreamForRead(cCharStream *_stream)
{
	mBitSize = 0;

	mStream = _stream;
	mStream->Read((char*)&mBitSize, sizeof(short int));
	mByteSize = Bits2Bytes(mBitSize);
	
	mCharArray = (unsigned char*)mStream->GetCharArray();
	mStream->Seek(mStream->GetOffset() + mByteSize);
	mCapacity = mByteSize;
	mSeek = 0;
	mActualChar = mCharArray[mSeek];
	mActualCharCounter = 0;
	#ifndef NDEBUG
	mStatus = STATUS_READ;
	#endif
}

inline unsigned char* cFastBitArray::GetCharArray() 
{
  return mCharArray + mSeek;
}
/**
*	Read char pointer from the stream. During the bit operation it will write directly to this memory.
*	Method left 2 byte size at the begining to store size of the array at the end of work with memory.
*	Method also lock the char stream, because Store() method has to be called after all bit operations are done.
*/

inline void cFastBitArray::SetCharStreamForWrite(cCharStream *_stream)
{
	mStream = _stream;
		
	mCharArray = (unsigned char*)mStream->GetCharArray();
	mCapacity = mStream->GetCapacity();
	mByteSize = 0;
	mActualChar = 0;
	mActualCharCounter = 0;
mSeek = sizeof(short int);				// Space for 2 byte size information has to remain
	#ifndef NDEBUG
	mStream->LockStream();
	mStatus = STATUS_WRITE;
	#endif
}

/**
* Store the rest of the information into the char stream. Unlock the char stream. 
* This method should be used only with SetCharStreamForWrite() method, because it writes 
* the size of the encoded stream at the begining. If you used SetMemoryForWrite() method
* use the StoreLastByte() instead.
* 
*/
inline void cFastBitArray::Store()
{
	

	if (mActualCharCounter < 8)
	{
		mActualChar >>= 8 - mActualCharCounter;
	}
	mCharArray[mSeek] = mActualChar;
	mSeek++;
	mByteSize++;
	mBitSize = mByteSize << 3;
	memcpy(mCharArray, &mBitSize, sizeof(short int));

	#ifndef NDEBUG
	mStream->UnlockStream();
	mStatus = STATUS_END;
	#endif

	mStream->Seek(mStream->GetOffset() + mSeek);
	mStream->IncreaseSize(mSeek);
}

/**
* Sets the number of bis written to output by SetNextBitHi2Lo() or SetNextBitLo2Hi().
* Bits are stored into mBitSize variable
*/
inline void cFastBitArray::StoreRemainingBits() {
	if (mActualCharCounter>0) 
	{
		mCharArray[mSeek] = mActualChar;
		mSeek++;
	}
	mBitSize = (mByteSize << 3) +mActualCharCounter;
}
inline void cFastBitArray::StoreBitCorrect()
{
	unsigned int mBitSize;
	if (mActualCharCounter>0) 
	{
		mCharArray[mSeek] = mActualChar;
		mSeek++;
	}
	
	mBitSize = (mByteSize << 3) +mActualCharCounter;
	mSeek=mByteSize+2;
	if (mActualCharCounter>0) 
	{
		mSeek++;
	}

	memcpy(mCharArray, &mBitSize, sizeof(short int));

	#ifndef NDEBUG
	mStream->UnlockStream();
	mStatus = STATUS_END;
	#endif

	mStream->Seek(mStream->GetOffset() + mSeek);
	mStream->IncreaseSize(mSeek);
}

/**
* Get next bit. The bits in memory are read so that most significant bit
* is read first
* 
*/
inline int cFastBitArray::GetNextBitHi2Lo()
{
	if (mActualCharCounter==8) 
	{
		mSeek++;
		mActualChar = mCharArray[mSeek];
		mActualCharCounter = 0;	
	}
	mActualCharCounter++;
	int bit= (mActualChar>>(8-mActualCharCounter))&1;

	return bit;
}

/**
* Get next bit. The bits in memory are read so that most significant bit
* is read last
* 
*/
inline int cFastBitArray::GetNextBitLo2Hi()
{
	if (mActualCharCounter==8) 
	{
		mSeek++;
		mActualChar = mCharArray[mSeek];
		mActualCharCounter = 0;	
	}
	int bit= (mActualChar>>mActualCharCounter)&1;
	mActualCharCounter++;

	return bit;
}

inline int cFastBitArray::GetNextBit()
{
	int bit;

	assert(mStatus == STATUS_READ);
	mActualCharCounter++;

	if (mActualCharCounter > 8)
	{
		mSeek++;
		mActualChar = mCharArray[mSeek];
		mActualCharCounter = 1;
	}

	if (mType)
	{
		bit = mActualChar & 0x80;
		mActualChar <<= 1;
	} else
	{
		bit = mActualChar & 0x1;
		mActualChar >>= 1;
	}

	return bit;
}






//revers bytes in dword, set word to stream, add bytes, return last byte. 
inline unsigned char cFastBitArray::SetNext32reverseWithLen( const unsigned int value,unsigned int bytes) {
	//mCharArray[mSeek]=value;

	/* !x64!
	__asm{


  mov         eax,dword ptr [this] 
  mov         ecx,dword ptr [eax+mCharArray] 
  mov         edx,dword ptr [this] 
  mov         eax,dword ptr [edx+mSeek] 

  mov edx,value;//Vykona to same jako:(unsigned int *)buf[j]=reverse buffer
  BSWAP edx;
  mov         dword  ptr [ecx+eax],edx 

	}
	*/
	mSeek+=bytes;
	mByteSize+=bytes;
	return mCharArray[mSeek];
}
//Sets next byte in the stream 
inline void cFastBitArray::SetNextByte( const unsigned char &byte) {
		mCharArray[mSeek] = byte;
		mSeek++;
		mByteSize++;
}
//Sets next unsigned long long in the stream 
inline void cFastBitArray::SetNext64( const unsigned long long &value) {
		(*((unsigned long long *)(mCharArray+mSeek))) = value;
		mSeek+=8;
		mByteSize+=8;
}
//Sets next unsigned int in the stream 
inline void cFastBitArray::SetNext32( const unsigned int &value) {
		(*((unsigned int *)(mCharArray+mSeek))) = value;
		mSeek+=4;
		mByteSize+=4;
}
inline void cFastBitArray::SetNext16( const unsigned short &value) {
		(*((unsigned short *)(mCharArray+mSeek))) = value;
		mSeek+=2;
		mByteSize+=2;
}
/**
* Sets next bit. The bits in memory are written into byte that the most sighnificant is written first.
*/

inline bool cFastBitArray::SetNextBitHi2Lo(unsigned char bit)
{
    //return SetNextBit(bit);
	
	mActualCharCounter++;
	mActualChar=mActualChar|bit<<(8-mActualCharCounter);
	if (mActualCharCounter==8) {
		SetNextByte(mActualChar);
		mActualChar=0;
		mActualCharCounter=0;		
	}
	return mByteSize < mCapacity;
	/**/
}

inline void cFastBitArray::SetNextBitHi2LoNoCheck(unsigned char bit)
{
    //return SetNextBit(bit);
	
	mActualCharCounter++;
	mActualChar=mActualChar|bit<<(8-mActualCharCounter);
	if (mActualCharCounter==8) {
		SetNextByte(mActualChar);
		mActualChar=0;
		mActualCharCounter=0;		
	}
	
	/**/
}

/**
* Sets next bit. The bits in memory are written into byte taht the most sighnificant is written last.
*/
inline bool cFastBitArray::SetNextBitLo2Hi(unsigned char bit)
{
    //return SetNextBit(bit);
	
	mActualChar|=(bit<<mActualCharCounter);
	mActualCharCounter++;
	if (mActualCharCounter==8) {
		SetNextByte(mActualChar);
		mActualChar=0;
		mActualCharCounter=0;		
	}
	return mByteSize < mCapacity;
	/**/
}

inline void cFastBitArray::SetNextBitLo2HiNoCheck(unsigned char bit)
{
    //return SetNextBit(bit);
	
	mActualChar|=(bit<<mActualCharCounter);
	mActualCharCounter++;
	if (mActualCharCounter==8) {
		SetNextByte(mActualChar);
		mActualChar=0;
		mActualCharCounter=0;		
	}
	/**/
}


/// \return false if the memory is full. Otherwise return true
inline bool cFastBitArray::SetNextBit(int bit)
{
	assert(mStatus == STATUS_WRITE);
	mActualCharCounter++;

	if (mActualCharCounter > 8)
	{
		mCharArray[mSeek] = mActualChar;
		mSeek++;
		mActualCharCounter = 1;
		mByteSize++;
	}

	mActualChar >>= 1;
	if (bit)
	{
		mActualChar |= 0x80;
	}

	mBitSize++;

	return mByteSize < mCapacity;
}

inline void cFastBitArray::AddBits(uint value, uint bitLength)
{
	int bitLengthM1 = (int)bitLength - 1;
	for (int i = 0 ; i <= bitLengthM1 ; i++)
	{
		bool bit = cBitString::GetBit((char*)(&value), i);
		SetNextBit(bit);
	}
}

/**
 * Get uint value from the mBitOffset bit order of the bit length.
 */
inline uint cFastBitArray::GetBits(uint bitLength)
{
	uint value = 0;
	uint chunkLength = cNumber::UINT_LENGTH;

	uint bitOffsetLast = mBitOffset + bitLength - 1;
	uint shift = mBitOffset % chunkLength;
	uint invShift = (chunkLength - shift) % chunkLength;

	uint chunkOffsetHi = bitOffsetLast / chunkLength;
	uint chunkOffsetLo = mBitOffset / chunkLength;

	uint* data = ((uint*)mCharArray) + chunkOffsetLo;
	value = *data;
		
	//if (shift != 0)
	//{
	value >>= shift;  // shift the value of the first chunk
	if (chunkOffsetLo < chunkOffsetHi)
	{
		// shift the value of the second chunk
		uint b2 = *(data + 1) << invShift;
		value |= b2;   // or them
	}
	//}

	// clear bits of the next value
	uint valShift = (((chunkOffsetHi+1) * chunkLength) - 1 - bitOffsetLast + shift) % chunkLength;
	value <<= valShift;
	value >>= valShift;

 	mBitOffset = bitOffsetLast + 1;
	return value;
}

/*
pùvodní algoritmus založený na bytech
	uint value = 0;
	int bitLengthM1 = (int)bitLength - 1;
	for (int i = bitLengthM1 ; i >= 0 ; i--)
	{
		bool bit = GetNextBit();
		cBitString::SetBit((char*)(&value), i, bit);
	}
	return value;

	uint value = 0;

	uint chunkLength = cNumber::UINT_LENGTH;

	uint bitOffsetLast = mBitOffset + bitLength - 1;
	uint shift = mBitOffset % chunkLength;
	uint invShift = (chunkLength - shift) % chunkLength;

	uint chunkOffsetHi = bitOffsetLast / chunkLength;
	uint chunkOffsetLo = mBitOffset / chunkLength;
	// uint codewordChunkSize = chunkOffsetHi - chunkOffsetLo + 1;
	uint resultChunkSize = cNumber::BitsToChunks(bitLength, chunkLength);

	uint* data = ((uint*)mCharArray) + chunkOffsetLo;

	// uint i = 0; pro byte
	// do cykluc není nutný u chunk=uint
	// {
		// uchar b = mCharArray[chunkOffsetLo + i];
	value = *data;
		
	if (shift != 0)
	{
		value >>= shift;  // shift the value of the first byte
		// if (i != codewordChunkSize) // fungovalo to u byte
		if (chunkOffsetLo < chunkOffsetHi)
		{
			// shift the value of the second byte
			// uchar b2 = mCharArray[chunkOffsetLo + i + 1] << invShift;
			uint b2 = *(data + 1) << invShift;
			value |= b2;   // or them
		}
	}

	// if (i == resultChunkSize-1) pro byte
	// {
	// probably it is necessary to remove bits of the next value
	uint valShift = (((chunkOffsetHi+1) * chunkLength) - 1 - bitOffsetLast + shift) % chunkLength;
	value <<= valShift;
	value >>= valShift;
	// }

	// *(((uchar*)&value) + i) = b; pro byte
	// i++; pro byte
	// } while (i < resultChunkSize); pro byte

 	mBitOffset = bitOffsetLast + 1;
	return value;
*/

inline void cFastBitArray::SetByte(const int Index, const unsigned char &byte)
{
	assert(Index < (int)mCapacity);
	mCharArray[Index] = byte;
}


inline int cFastBitArray::GetByteSize()
{
	return mByteSize;
}

inline int cFastBitArray::GetBitSize()
{
	return (mByteSize << 3) + mActualCharCounter;
}
/**
* Gets the number of bis written to output by SetNextBitHi2Lo() or SetNextBitLo2Hi().
* 
*/
inline int cFastBitArray::GetBitSizeCorrect()
{
	return mBitSize;
}
/**
* Gets the byte written to output by SetNextBitHi2Lo() or SetNextBitLo2Hi().
* 
*/
inline unsigned char& cFastBitArray::GetByte(const int Index) const
{
	assert(Index < (int)mByteSize);
	return ((unsigned char*)mCharArray)[Index];
}
/**
* Gets the unsigned int 
* 
*/
inline unsigned int &cFastBitArray::Get32(const int Index) const
{
	assert(Index < (int)mByteSize>>2);
	return ((unsigned int *)(mCharArray))[Index];
}
inline unsigned short &cFastBitArray::Get16(const int Index) const
{
	assert(Index < (int)mByteSize>>1);
	return ((unsigned short int *)(mCharArray))[Index];
}
/**
* Gets the 8 bytes (unsigned long long) written to output by SetNextBitHi2Lo() or SetNextBitLo2Hi().
* 
*/

inline unsigned long long &cFastBitArray::Get64(const int Index) const
{
	assert(Index < (int)(mByteSize>>3));
	return ((unsigned long long *)(mCharArray))[Index];
}
inline int cFastBitArray::Bits2Bytes(const int BitCount) const
{
	/*
	int iRet = BitCount >> 3;
	if ((BitCount & 7) != 0)
	{
		iRet++;
	}
	return iRet;
	*/
	return (BitCount+7)>>3;
}


#endif