#include "common/compression/cCoderCommon.h"

namespace common {
	namespace compression {

unsigned int cCoderCommon::Increment(int bytePerNumber, unsigned int value)
{
	unsigned int retValue = value;

	if (bytePerNumber == 1)
	{
		if (value == UCHAR_MAX)
		{
			retValue = value;
			printf("Coder error: Unsuported value UCHAR_MAX, it is not possible to encode the value!");
		} else
		{
			retValue = value + 1;
		}
	}
	else if (bytePerNumber == 2)
	{
		if (value == USHRT_MAX)
		{
			retValue = value;
			printf("Coder error: Unsuported value USHRT_MAX, it is not possible to encode the value!");
		} else
		{
			retValue = value + 1;
		}
	}
	else if (bytePerNumber == 4)
	{
		if (value == UINT_MAX)
		{
			retValue = value;
			printf("Coder error: Unsuported value UINT_MAX, it is not possible to encode the value!");
		} else
		{
			retValue = value + 1;
		}
	}
	else 
	{
		printf("Coder error: Unsuported number of bytes per number!");
	}
	return retValue;
}


uint cCoderCommon::GetBitLength(const uchar* number, uint bytesPerNumber)
{
	bool finishf = false;
	uint value;

	switch(bytesPerNumber)
	{
	case 4:
		value = *((uint*)number);
		break;
	case 2:
		value = (uint)*((ushort*)number);
		break;
	case 1:
		value = (uint)*number;
		break;
	}

	return cNumber::Length(value);
}

uint cCoderCommon::GetByteLength(uint bitLength)
{
	uint byteLength = bitLength / cNumber::BYTE_LENGTH;
	if (bitLength % cNumber::BYTE_LENGTH > 0)
	{
		byteLength++;
	}
	return byteLength;
}

}}