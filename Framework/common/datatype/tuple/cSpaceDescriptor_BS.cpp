#include "cSpaceDescriptor_BS.h"

namespace common {
	namespace datatype {
		namespace tuple {

cSpaceDescriptor_BS::cSpaceDescriptor_BS()
{
	mBitSizes = NULL;
}

cSpaceDescriptor_BS::cSpaceDescriptor_BS(unsigned int dimension, unsigned int bitSize, unsigned int addressType)
{
	Create(dimension, bitSize, addressType);
	ComputeSize();
}

cSpaceDescriptor_BS::cSpaceDescriptor_BS(const cSpaceDescriptor_BS &spaceDesc)
{
	mDimension = spaceDesc.mDimension;
	mBitSizes = new unsigned int[mDimension];
	mAddressType = spaceDesc.mAddressType;
	for (unsigned int i = 0; i < mDimension ; i++)
	{
		*(mBitSizes + i) = *((spaceDesc.mBitSizes) + i);
	}
	ComputeSize();
}

cSpaceDescriptor_BS::~cSpaceDescriptor_BS()
{
	if (mBitSizes != NULL)
	{
		delete []mBitSizes;
	}
}

/**
 * Create and set mem. data.
 */
void cSpaceDescriptor_BS::Create(unsigned int dimension, unsigned int bitSize, unsigned int addressType)
{
	mDimension = dimension;
	mBitSizes = new unsigned int[dimension];
	mAddressType = addressType;

	for (unsigned int i = 0 ; i < dimension ; i++)
	{
		*(mBitSizes + i) = bitSize;
	}
	ComputeSize();
}

/**
 * Create and set mem. data.
 */
void cSpaceDescriptor_BS::Create(const cSpaceDescriptor_BS& desc)
{
	mDimension = desc.mDimension;
	mBitSizes = new unsigned int[mDimension];
	mAddressType = desc.mAddressType;

	for (unsigned int i = 0 ; i < mDimension ; i++)
	{
		*(mBitSizes + i) = *((desc.mBitSizes)+i);
	}
	ComputeSize();
}

void cSpaceDescriptor_BS::SetBitSize(unsigned int dim, unsigned int bitSize)
{
	if (dim < mDimension)
	{
		*(mBitSizes + dim) = bitSize;
	}
	ComputeSize();
}

void cSpaceDescriptor_BS::SetDescriptor(cSpaceDescriptor_BS &spaceDesc)
{
	unsigned int newDim = spaceDesc.GetDimension(); 

	if (mDimension != newDim)
	{
		delete []mBitSizes;
		mBitSizes = new unsigned int[newDim];
		mDimension = newDim;
	}

	mAddressType = spaceDesc.GetAddressType();
	for (unsigned int i = 0; i < newDim ; i++)
	{
		SetBitSize(i, spaceDesc.GetBitSize(i));
	}
	ComputeSize();
}

void cSpaceDescriptor_BS::ComputeSize()
{
	mByteSize = 0;
	mBitSize = 0;
	for (unsigned int i = 0; i < mDimension ; i++)
	{
		mByteSize += mBitSizes[i] / UBYTE_LENGTH;		// rb - not dependent on cBitString
		if (mBitSizes[i] % UBYTE_LENGTH > 0) mByteSize++;

		//mByteSize += cBitString::ByteSize(*(mBitSizes + i));
		mBitSize += *(mBitSizes + i);
	}
}

unsigned int cSpaceDescriptor_BS::GetMinBitSize() const
{
	unsigned bitSize = *mBitSizes;
	for (unsigned int i = 1 ; i < mDimension ; i++)
	{
		if (bitSize > *(mBitSizes + i))
		{
			bitSize = *(mBitSizes + i);
		}
	}
	return bitSize;
}

unsigned int cSpaceDescriptor_BS::GetMaxBitSize() const
{
	unsigned bitSize = 0;
	for (unsigned int i = 0; i < mDimension ; i++)
	{
		if (bitSize < *(mBitSizes + i))
		{
			bitSize = *(mBitSizes + i);
		}
	}
	return bitSize;
}

unsigned int cSpaceDescriptor_BS::GetMaxValue(unsigned int dimension) const
{
	return (unsigned int)(pow((float)2, (int)(GetBitSize(dimension))-1));
}

/**
 * Serialization of space descriptor.
 **/
bool cSpaceDescriptor_BS::Write(cStream *stream)
{
	/*bool ret1 = stream->Write((char*)&mDimension, sizeof(mDimension));
	bool ret2 = stream->Write((char*)mBitSizes, sizeof(mBitSizes[0]));
	return ret1 & ret2;*/

	bool ret = stream->Write((char*)&mDimension, sizeof(mDimension));
	for (unsigned int i = 0; i < mDimension ; i++)
	{
		ret &= stream->Write((char*)(mBitSizes + i), sizeof(mBitSizes[i]));
	}
	return ret;
}

/**
 * Deserialization of space descriptor.
 *
 * !!!!!!!!!!!!!!!!!!!!!! NOT FULL FUNCTION !!!!!!!!!!!!!!!!!!!!!!!
 * While without change of size.
 **/
bool cSpaceDescriptor_BS::Read(cStream *stream)
{
	/*bool ret1 = stream->Read((char*)&mDimension, sizeof(mDimension));
	bool ret2 = stream->Read((char*)mBitSizes, sizeof(mBitSizes[0]));

	for (unsigned int i = 1 ; i < mDimension ; i++)
	{
		mBitSizes[i] = mBitSizes[0];
	}

	return ret1 & ret2;*/

	unsigned int dimension = mDimension;
	bool ret = stream->Read((char*)&mDimension, sizeof(mDimension));

	if (mDimension != dimension)
	{
		if (mBitSizes != NULL)
		{
			delete mBitSizes;
		}
		mBitSizes = new unsigned int[mDimension];
	}

	for (unsigned int i = 0; i < mDimension; i++)
	{
		ret &= stream->Read((char*)(mBitSizes + i), sizeof(mBitSizes[0]));
	}
	return ret;
}
}}}