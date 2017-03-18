/**************************************************************************}
{                                                                          }
{    cSpaceDescriptor_BS.h                                            		     }
{                                                                          }
{                                                                          }
{    Copyright (c) 2001,2002					       Michal Kratky                 }
{                                                                          }
{    VERSION: 0.2														DATE 13/03/2002                }
{                                                                          }
{    following functionality:                                              }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      16/09/2002                                                          }
{                                                                          }
{**************************************************************************/

#ifndef __cSpaceDescriptor_BS_h__
#define __cSpaceDescriptor_BS_h__

#include <iostream>
#include <math.h>

#include "cBitString.h"

namespace common {
	namespace datatype {
		namespace tuple {

class cSpaceDescriptor_BS  
{
protected:
	unsigned int mDimension;
	unsigned int *mBitSizes;
	unsigned int *mByteSizes;
	unsigned int mByteSize;
	unsigned int mBitSize;
	unsigned int mAddressType;

private:
	void ComputeSize();

public:
	static const unsigned int ADDRESS_Z = 0;
	static const unsigned int ADDRESS_C = 1;
	static const unsigned int ADDRESS_H = 2;

	static const unsigned int UINT_LENGTH = 32;
	static const unsigned int UBYTE_LENGTH = 8;

	cSpaceDescriptor_BS();
	cSpaceDescriptor_BS(unsigned int dimension, unsigned int bitSize, unsigned int addressType = ADDRESS_Z);
	cSpaceDescriptor_BS(const cSpaceDescriptor_BS &sd);
	~cSpaceDescriptor_BS();

	void Create(unsigned int dimension, unsigned int bitSize, unsigned int addressType = ADDRESS_Z);
	void Create(const cSpaceDescriptor_BS& desc);

	virtual void SetBitSize(unsigned int dim, unsigned int bitSize);
	void SetDescriptor(cSpaceDescriptor_BS &spaceDesc);
	void SetInterleavingFactors();
	void SetPermMatrix();

	inline unsigned int GetDimension() const;
	inline unsigned int GetByteSize() const;
	inline unsigned int GetBitSize() const;
	inline unsigned int GetBitSize(unsigned int dim) const;
	inline unsigned int GetAddressType() const;
	unsigned int GetMinBitSize() const;
	unsigned int GetMaxBitSize() const;
	unsigned int GetMaxValue(unsigned int dimension) const;

	bool Read(cStream *stream);
	bool Write(cStream *stream);
};

inline unsigned int cSpaceDescriptor_BS::GetBitSize(unsigned int dim) const
{
	if (dim < mDimension)
	{
		return *(mBitSizes + dim);
	}
	else 
	{
		return 0;
	}
}

inline unsigned int cSpaceDescriptor_BS::GetDimension() const
{ 
	return mDimension; 
}
inline unsigned int cSpaceDescriptor_BS::GetAddressType() const 
{ 
	return mAddressType; 
}

inline unsigned int cSpaceDescriptor_BS::GetByteSize() const
{
	return mByteSize;
}

inline unsigned int cSpaceDescriptor_BS::GetBitSize() const
{
	return mBitSize;
}
}}}
#endif