
#include "cUniversalCoderFixedLength.h"

namespace common {
	namespace compression {

unsigned int cUniversalCoderFixedLength::decode(int bytesPerNumber, unsigned char* encodedBuffer, unsigned char* decodedBuffer, unsigned int count) 
{
	uint offset = 0;
	cBitArray bitArray;
	bitArray.SetMemoryForRead(encodedBuffer, 0);

	uint maxBitLength = bitArray.GetBits(HEADER_BITLENGTH);
	uint value = 0;

	for (uint i = 0 ; i < count ; i++)
	{
		if (maxBitLength != 0)
		{
			value = bitArray.GetBits(maxBitLength);
		}
		*((uint*)(decodedBuffer + offset)) = value;
		offset += bytesPerNumber;
		// printf("%u, ", *(((uint*)decodedBuffer)+i));
	}
	// printf("\n");
	uint bitLength = HEADER_BITLENGTH + count * maxBitLength;
	return cCoderCommon::GetByteLength(bitLength);
}

/**
 * It returns the codeword byte size.
 */
unsigned int cUniversalCoderFixedLength::decode_aligned(int bytesPerNumber, unsigned char* encodedBuffer, unsigned char* decodedBuffer, unsigned int count) 
{
	uint srcIndex = 0;
	uint dstIndex = 0;

	uint codewordSize = *(encodedBuffer + srcIndex++);

	for (uint i = 0 ; i < count ; i++)
	{
		uchar *pdst = decodedBuffer + dstIndex;
		uchar *psrc = encodedBuffer + srcIndex;

		for (uint j = 0 ; j < codewordSize ; j++)
		{
			*pdst = *psrc;
			pdst++;
			psrc++;
		}

		// reset other bytes of decodedBuffer
		uint rest = bytesPerNumber - codewordSize;
		for (uint j = 0 ; j < rest ; j++)
		{
			*pdst = 0;
			pdst++;
		}

		srcIndex += codewordSize;
		dstIndex += bytesPerNumber;
	}

	return srcIndex;
}

/*
 * It returns the codeword bit length.
 */
unsigned int cUniversalCoderFixedLength::encode(int bytesPerNumber, const unsigned char* sourceBuffer, unsigned char* encodedBuffer, unsigned int count) 
{
	uint offset = 0;

	uint maxBitLength = 0;
	for (uint i = 0 ; i < count ; i++)
	{
		uint len = cCoderCommon::GetBitLength(sourceBuffer + offset, bytesPerNumber);
		if (len > maxBitLength)
		{
			maxBitLength = len;
		}
		offset += bytesPerNumber;
	}

	// the length of encoded buffer is not known therefore try to compute a value
	cBitArray bitArray;
	bitArray.SetMemoryForWrite(encodedBuffer, 0);

	// write encoded value into the bit array
	offset = 0;
	bitArray.AddBits(maxBitLength, HEADER_BITLENGTH);
	if (maxBitLength > 0)
	{
		for (uint i = 0 ; i < count ; i++)
		{
			uint value = *((uint*)(sourceBuffer + offset));
			bitArray.AddBits(value, maxBitLength);
			offset += bytesPerNumber;
		}
	}
	bitArray.StoreLastByte();

	uint bitLength = maxBitLength * count + HEADER_BITLENGTH;

	//uchar tmpBuffer[100];
	//uint codewordByteLength = decode(bytesPerNumber, encodedBuffer, (uchar*)tmpBuffer, count);
	//uint byteLength = count * bytesPerNumber;
	//for (uint i = 0 ; i < byteLength ; i++)
	//{
	//	if (*(sourceBuffer+i) != *(tmpBuffer+i))
	//	{
	//		printf("%d != %d\n", *(sourceBuffer+i), *(tmpBuffer+i));
	//	}
	//}

	return bitLength;
}

/*
 * It returns the codeword bit length.
 */
unsigned int cUniversalCoderFixedLength::encode_aligned(int bytesPerNumber, const unsigned char* sourceBuffer, unsigned char* encodedBuffer, unsigned int count) 
{
	uint offset = 0;

	uint maxBitLength = 0;
	for (uint i = 0 ; i < count ; i++)
	{
		uint len = cCoderCommon::GetBitLength(sourceBuffer + offset, bytesPerNumber);
		if (len > maxBitLength)
		{
			maxBitLength = len;
		}
		offset += bytesPerNumber;
	}

	uint codewordSize = maxBitLength >> 3; // / cNumber::BYTE_LENGTH
	if ((maxBitLength & 7) != 0)   // % cNumber::BYTE_LENGTH
	{
		codewordSize++;
	}

	uint dstIndex = 0, srcIndex = 0;

	// the first byte includes the codeword size
	*(encodedBuffer + dstIndex++) = codewordSize;

	if (codewordSize != 0)
	{
		// copy only codewordSize bytes into encodedBuffer
		for (uint i = 0 ; i < count ; i++)
		{
			uchar *pdst = (uchar*)encodedBuffer + dstIndex;
			uchar *psrc = (uchar*)sourceBuffer + srcIndex;

			for (uint j = 0 ; j < codewordSize ; j++)
			{
				*pdst = *psrc;
				pdst++;
				psrc++;
			}

			dstIndex += codewordSize;
			srcIndex += bytesPerNumber;
		}
	}

	uint bitLength = dstIndex << 3; // * 8

	//uchar tmpBuffer[100];
	//uint codewordByteLength = decode(bytesPerNumber, encodedBuffer, (uchar*)tmpBuffer, count);
	//uint byteLength = count * bytesPerNumber;
	//for (uint i = 0 ; i < byteLength ; i++)
	//{
	//	if (*(sourceBuffer+i) != *(tmpBuffer+i))
	//	{
	//		printf("%d != %d\n", *(sourceBuffer+i), *(tmpBuffer+i));
	//	}
	//}

	return bitLength;
}

/**
 * It returns the codeword bit length.
 */
unsigned int cUniversalCoderFixedLength::GetCodewordSize(int bytesPerNumber, const unsigned char* sourceBuffer, unsigned int count) 
{
	uint offset = 0;
	uint maxBitLength = 0;

	for (uint i = 0 ; i < count ; i++)
	{
		uint len = cCoderCommon::GetBitLength(sourceBuffer + offset, bytesPerNumber);
		if (len > maxBitLength)
		{
			maxBitLength = len;
		}
		offset += bytesPerNumber;
	}

	return maxBitLength * count + HEADER_BITLENGTH;
}

/**
 * It returns the codeword bit length.
 */
unsigned int cUniversalCoderFixedLength::GetCodewordSize_aligned(int bytesPerNumber, const unsigned char* sourceBuffer, unsigned int count) 
{
	uint offset = 0;

	uint maxBitLength = 0;
	for (uint i = 0 ; i < count ; i++)
	{
		uint len = cCoderCommon::GetBitLength(sourceBuffer + offset, bytesPerNumber);
		if (len > maxBitLength)
		{
			maxBitLength = len;
		}
		offset += bytesPerNumber;
	}

	uint codewordSize = maxBitLength >> 3; // / cNumber::BYTE_LENGTH
	if ((maxBitLength & 7) != 0)   // % cNumber::BYTE_LENGTH
	{
		codewordSize++;
	}

	return (1 + codewordSize * count) << 3;
}

}}