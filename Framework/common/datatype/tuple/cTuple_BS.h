/**************************************************************************}
{                                                                          }
{    cTuple_BS.h                                                              }
{                                                                          }
{                                                                          }
{    Copyright (c) 2001                      Michal Kratky                 }
{                                                                          }
{    VERSION: 0.2                            DATE 5/11/2001                }
{                                                                          }
{    following functionality:                                              }
{      Tuple - point in n-dimensional space.                               }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      01/03/2002 - Tomas Skopal                                           }
{      22/02/2002                                                          }
{                                                                          }
{**************************************************************************/

#ifndef __cTuple_BS_h__
#define __cTuple_BS_h__

#include "cObject.h"
#include "cBitString.h"
#include "cString.h"
#include "cSpaceDescriptor_BS.h"
// #include "cHilbert.h"
#include "cNumber.h"

namespace common {
	namespace datatype {
		namespace tuple {

class cTuple_BS  
{
private:
	cArray<cBitString> *mValues;    // tuple = array of bit strings
	cSpaceDescriptor_BS *mSpaceDescriptor;

	void SetValuesZ(const cBitString &address);
	void SetValuesC(const cBitString &address);
	// void SetValuesH(const cBitString &address);
	void AddressZ(cBitString &address) const;
	void AddressC(cBitString &address) const;
	// void AddressH(cBitString &address);
	static bool mDebug;

public:
	cTuple_BS();
	cTuple_BS(cSpaceDescriptor_BS *spaceDescriptor);
	void Init();
	~cTuple_BS();

	void Resize(cSpaceDescriptor_BS *spaceDescriptor);
	void Resize(cSpaceDescriptor_BS *spaceDescriptor, cMemory *memory);
	void SetTuple(const cTuple_BS &tuple);
	void SetValue(unsigned int dimension, const cBitString &value);
	void SetValues(const cBitString &address);
	void Clear();
	void ClearOther(unsigned int number);
	void SetMaxValues();

	inline cBitString& GetRefValue(unsigned int dimension) const;
	inline cBitString* GetValue(unsigned int dimension);
	unsigned int GetRealDimension() const;
	inline cSpaceDescriptor_BS* GetSpaceDescriptor() const;
	unsigned int GetSerialSize() const;

	void Address(cBitString &address) const;
	bool IsInBlock(const cTuple_BS &tupleY, const cTuple_BS &tupleZ) const;
	static float EuclidianDistance(const cTuple_BS &tuple1, const cTuple_BS &tuple2);
	void ModifyMbr(cTuple_BS &mbrl, cTuple_BS &mbrh) const;

	bool Read(cStream *stream);
	bool Write(cStream *stream) const;
	int Equal(const cTuple_BS &tuple) const;
	void operator = (const cTuple_BS &tuple);
	bool operator == (const cTuple_BS &tuple) const;
	bool operator != (const cTuple_BS &tuple);

	void Print(int mode, char *str) const;
};

/**
 * Return <dimension> cordinate.
 **/
inline cBitString& cTuple_BS::GetRefValue(unsigned int dimension) const
{
	if (dimension >= mSpaceDescriptor->GetDimension())
	{
		dimension = 0;
	}
	return *(mValues->GetArray() + dimension);
}

/**
 * Return <dimension> cordinate.
 **/
inline cBitString* cTuple_BS::GetValue(unsigned int dimension)
{
	if (dimension >= mSpaceDescriptor->GetDimension()) 
	{
		dimension = 0;
	}
	return mValues->GetArray(dimension);
}

inline cSpaceDescriptor_BS* cTuple_BS::GetSpaceDescriptor() const
{	
	return mSpaceDescriptor; 
}
}}}
#endif
