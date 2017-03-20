/**
 *	\file cCommonNTuple<TLength>.h
 *	\author Michal Kratky, Radim Baca
 *	\version 0.1
 *	\date jun 2006
 *	\brief Homogenous tuple for a tree data structure. It contains an array of items of the same type.
 */

#ifndef __cCommonNTuple_h__
#define __cCommonNTuple_h__

namespace common {
	namespace datatype {
		namespace tuple {
class cTuple;
}}}

#include "common/datatype/cDTDescriptor.h"
#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "common/stream/cStream.h"
#include "common/cString.h"
#include "common/compression/Coder.h"
#include "common/cBitString.h"
#include "common/utils/cHistogram.h"
#include "common/memdatstruct/cMemoryBlock.h"

using namespace common::compression;
using namespace common::datatype;
using namespace common::utils;
//using namespace common::memdatstruct;

/**
* Represents tuple with homogenous structure which store its length. 
* Homogenous tuple for a tree data structure. It contains an array of items of the same type.
* Tuple does not contain the reference to the space descriptor, therefore, almost no asserts are contained in the tuple!
* Application has to do the asserts by itself!
*
*
* \author Radim Baca
* \version 2.2
* \date oct 2011
**/

namespace common {
	namespace datatype {
		namespace tuple {

template <class TLength>
class cCommonNTuple : public cDataType
{
public:
	typedef cCommonNTuple<TLength> T;

protected:
	static const unsigned int MAX_LENGTH = 1000;
	static const unsigned int SIZEPREFIX_LEN = sizeof(TLength);
	char *mData;

	unsigned int Sum();
	static const unsigned int INC_VALUE = 1; // incrementation value for variable-length codes

	inline unsigned int GetDataSize(const cSpaceDescriptor *pSd);

public:
	static const char CODE = 'n';
	static const unsigned int LengthType = cDataType::LENGTH_VARLEN;

	inline virtual char GetCode();

public:
	cCommonNTuple<TLength>();
	cCommonNTuple<TLength>(const cSpaceDescriptor *pSd);
	cCommonNTuple<TLength>(const cSpaceDescriptor *pSd, unsigned int currentLength);
	cCommonNTuple<TLength>(char* buffer);
	~cCommonNTuple<TLength>();

	void Free(cMemoryBlock* memBlock = NULL);

	void Resize(const cSpaceDescriptor *pSd, unsigned int currentLength);
	void Resize(const cDTDescriptor *pSd, unsigned int currentLength);
	bool Resize(const cDTDescriptor *pSd, cMemoryBlock *memBlock = NULL);
	void Resize(const cCommonNTuple<TLength> &tuple, const cDTDescriptor *pSd);

	inline void SetValue(const cCommonNTuple<TLength> &tuple, const cSpaceDescriptor* pSd);
	inline void SetValue(unsigned int order, float value, const cSpaceDescriptor* pSd);
	inline void SetValue(unsigned int order, double value, const cSpaceDescriptor* pSd);
	inline void SetValue(unsigned int order, int value, const cSpaceDescriptor* pSd);
	inline void SetValue(unsigned int order, unsigned int value, const cSpaceDescriptor* pSd);
	inline void SetValue(unsigned int order, char value, const cSpaceDescriptor* pSd);
	inline void SetValue(unsigned int order, unsigned char value, const cSpaceDescriptor* pSd);
	inline void SetValue(unsigned int order, wchar_t value, const cSpaceDescriptor* pSd);
	inline void SetValue(unsigned int order, ullong value, const cSpaceDescriptor* pSd);
	inline void SetValue(unsigned int order, unsigned short value, const cSpaceDescriptor* pSd);
	inline void SetValue(unsigned int order, short value, const cSpaceDescriptor* pSd);
	void SetValue(unsigned int order, char* cTuple_value, const cSpaceDescriptor* pSd);

	inline void SetMaxValue(unsigned int order, const cSpaceDescriptor* pSd);	
	inline void Clear(const cSpaceDescriptor* pSd);
	inline void Clear(unsigned int order, const cSpaceDescriptor* pSd);
		
	inline void SetLength(unsigned int len);
	static inline void SetLength(char* data, const cSpaceDescriptor* pSd);

	inline float GetFloat(unsigned int order, const cSpaceDescriptor* pSd) const;
	inline double GetDouble(unsigned int order, const cSpaceDescriptor* pSd) const;
	inline int GetInt(unsigned int order, const cSpaceDescriptor* pSd) const;
	inline unsigned int GetUInt(unsigned int order, const cSpaceDescriptor* pSd) const;
	inline char GetByte(unsigned int order, const cSpaceDescriptor* pSd) const;
	inline unsigned char GetUChar(unsigned int order, const cSpaceDescriptor* pSd) const;
	inline short GetShort(unsigned int order, const cSpaceDescriptor* pSd) const;
	inline unsigned short GetUShort(unsigned int order, const cSpaceDescriptor* pSd) const;
	inline void GetString(unsigned int order, cString &string, const cSpaceDescriptor* pSd) const;
	inline wchar_t GetWChar(unsigned int order, const cSpaceDescriptor* pSd) const;
	inline char GetCChar(unsigned int order, const cSpaceDescriptor* pSd) const;

	inline float* GetPFloat(unsigned int order, const cSpaceDescriptor* pSd) const;
	inline int* GetPInt(unsigned int order, const cSpaceDescriptor* pSd) const;
	inline unsigned int* GetPUInt(unsigned int order, const cSpaceDescriptor* pSd) const;
	inline char* GetPByte(unsigned int order, const cSpaceDescriptor* pSd) const;
	inline unsigned char* GetPUChar(unsigned int order, const cSpaceDescriptor* pSd) const;
	inline unsigned char* GetPTuple(unsigned int order, const cSpaceDescriptor* pSd) const;
	static inline char* GetTuple(const char *data,unsigned int order, const cSpaceDescriptor* pSd);

	inline unsigned int GetSize(const cDTDescriptor* pSd) const;
	inline unsigned int GetSize_instance(const char *data, const cDTDescriptor *pDtd) const;
	inline unsigned int GetSize(uint tupleSize) const;
	inline unsigned int GetMaxSize(const cDTDescriptor* pSd) const;

	inline char* GetData() const;
	inline void SetData(char* pData);
	inline operator char*() const;

	inline TLength GetLength() const;

	inline bool Read(cStream *stream, const cSpaceDescriptor* pSd);
	inline bool Write(cStream *stream, const cSpaceDescriptor* pSd) const;

	inline int Equal(const cCommonNTuple<TLength> &tuple, unsigned int order, const cSpaceDescriptor* pSd) const;
	inline int Equal(const cCommonNTuple<TLength> &tuple, const cSpaceDescriptor* pSd) const;
	inline int Equal(const char* tuple2, const cSpaceDescriptor* pSd) const;
	inline int Compare(const cCommonNTuple<TLength> &tuple, const cSpaceDescriptor* pSd) const;
	inline int CompareArray(const char* array1, const char* array2, uint length);
	inline unsigned int HashValue(const char *array, unsigned int length, unsigned int hashTableSize);
	inline int CompareLexicographically(const char* tuple2, const cSpaceDescriptor* pSd) const;
	inline int CompareLexicographically(const cCommonNTuple<TLength> &tuple, const cSpaceDescriptor* pSd) const;

	inline void Copy(const cCommonNTuple<TLength> &tuple);
	inline unsigned int CopyTo(char *data, const cSpaceDescriptor* pSd) const;
	inline unsigned int CopyTo(char *data, const cDTDescriptor* pDtD) const;
	inline unsigned int Copy(const char *srcData, const cSpaceDescriptor* pDtD);
	inline unsigned int Copy(const char *srcData, const cDTDescriptor* pDtD);

	void Print(const char *string, const cSpaceDescriptor* pSd) const;
	void Print(const char *string, const cDTDescriptor* pSd) const;
	void Print(unsigned int order, const char *string, const cSpaceDescriptor* pSd) const;

	static inline unsigned int HashValue(char *tuple, uint hashTableSize, const cDTDescriptor* dtd);
	
	// Instance methods, but working with char*
	inline int Compare(const char* tuple2, const cSpaceDescriptor* pSd) const;
	inline int Compare(const char* tuple2, const cDTDescriptor *dd) const;

	// Static methods working with char*
	static inline void SetValue(char* data, const cCommonNTuple<TLength> &tuple, const cSpaceDescriptor* pSd);
	static inline char* Copy(char* cNTuple_dst, const char* cNTuple_src, const cDTDescriptor *pSd);
	static inline void Copy(char* cUnfTuple_dst, const char* cUnfTuple_src, const cSpaceDescriptor *pSd);
	static inline void CopyFromTuple(char* cNTuple_dst, const char* cTuple_src, const int size, const int length);
	static bool ResizeSet(cCommonNTuple<TLength> &t1, const cCommonNTuple<TLength> &t2, const cDTDescriptor* pSd, cMemoryBlock* memblock);
	static void Free(cCommonNTuple<TLength> &tuple, cMemoryBlock *memBlock = NULL);

	static inline void Clear(char* data, const cSpaceDescriptor* pSd);

	static inline int Equal(const char* tuple1, const char* tuple2, uint tupleLength, const cDTDescriptor *pSd);
	static inline int Equal(const char* tuple1, const char* tuple2, const cDTDescriptor *pSd);
	static inline int Equal(const char* tuple1, const char* tuple2, const cSpaceDescriptor *pSd);
	static inline int Equal(const char* cUnfTuple_t1, const char* cUnfTuple_t2, const unsigned int order, const cSpaceDescriptor* pSd);
	static inline int Equal(const char* cUnfTuple_t1, const char* cUnfTuple_t2, const unsigned int order1, const unsigned int order2, const cSpaceDescriptor* pSd);
	static inline bool IsEqual(const char* dst, const char* src, const cDTDescriptor *pSd);
	static inline int Compare(const char* dst, const char* src, const cDTDescriptor *pSd);
	static inline int CompareLexicographically(const char* tuple1, const char* tuple2, const cSpaceDescriptor* pSd);
	static inline int ComparePartly(const char* tuple1, const char* tuple2, const cDTDescriptor* pSd, unsigned int startOrder);
	static inline TLength GetLength(const char* tuple, const cDTDescriptor* pSd = NULL);
	static inline void SetLength(char* tuple, unsigned int len);

	static void Print(const char *data, const char* delim, const cSpaceDescriptor* pSd);
	static void Print(const char *data, const char* delim, const cDTDescriptor* pSd);
	static void Print(const char* data, unsigned int order, const char *string, const cSpaceDescriptor* pSd);
	static void Print2File(FILE *StreamInfo, const char *data, const char* delim, const cSpaceDescriptor* pSd);
	static void Print2File(FILE *StreamInfo, const char *data, const char* delim, const cDTDescriptor* pSd);

	static inline unsigned int GetLSize(uint tupleLength, const cDTDescriptor* dtd);
	static inline unsigned int GetSizePart(const char *data, unsigned int order, const cSpaceDescriptor* pSd);

	static inline uint GetDimension(const cDTDescriptor* dtd);

	static inline unsigned int GetSize(const char *data, const cDTDescriptor* pSd);
	static inline unsigned int GetMaxSize(const char *data, const cDTDescriptor* pSd);
	static inline int GetObjectSize(const cSpaceDescriptor *pSd);

	static inline unsigned int GetUInt(const char *data, unsigned int order, const cSpaceDescriptor* pSd);
	static inline int GetInt(const char *data, unsigned int order, const cSpaceDescriptor* pSd);
	static inline ullong GetULong(const char *data, unsigned int order, const cSpaceDescriptor* pSd);
	static inline float GetFloat(const char *data, unsigned int order, const cSpaceDescriptor* pSd);
	static inline unsigned short GetUShort(const char *data, unsigned int order, const cSpaceDescriptor* pSd);
	static inline unsigned char GetUByte(const char *data, unsigned int order, const cSpaceDescriptor* pSd);
	static inline char GetByte(const char *data, unsigned int order, const cSpaceDescriptor* pSd);
	static inline wchar_t GetWChar(const char *data, unsigned int order, const cSpaceDescriptor* pSd);
	static inline char GetCChar(const char *data, unsigned int order, const cSpaceDescriptor* pSd);

	static inline char* GetPChar(const char* data, unsigned int order, const cDTDescriptor* pSd);

	static inline void SetValue(char *data, unsigned int order, unsigned int value, const cSpaceDescriptor* pSd);
	static inline void SetValue(char *data, unsigned int order, unsigned short value, const cSpaceDescriptor* pSd);
	static inline void SetValue(char *data, unsigned int order, unsigned char value, const cSpaceDescriptor* pSd);
	static inline void SetValue(char *data, unsigned int order, ullong value, const cSpaceDescriptor* pSd);
	static inline void SetValue(char *data, unsigned int order, char value, const cSpaceDescriptor* pSd);

	// Peter Chovanec 11.10.2011
	static double TaxiCabDistance(const char* cNTuple_t1, const char* cNTuple_t2, const cDTDescriptor* pSd);
	static char* Subtract(const char* cNTuple_t1, const char* cNTuple_t2, char* cNTuple_result, const cDTDescriptor* pSd);
	static char* Add(const char* cNTuple_t1, const char* cNTuple_t2, char* cNTuple_result, const cDTDescriptor* pSd);
	
	static inline double TaxiCabDistanceValue(const char* cNTuple_t1, const char* cNTuple_t2, unsigned int order, const cSpaceDescriptor* pSd);
	static inline void Subtract(const char* cNTuple_t1, const char* cNTuple_t2, char* cNTuple_result, unsigned int order, const cSpaceDescriptor* pSd);
	static inline void Add(const char* cNTuple_t1, const char* cNTuple_t2, char* cNTuple_result, unsigned int order, const cSpaceDescriptor* pSd);

	// for codding purpose
	static inline uint Encode(uint method, const char* sourceBuffer, char* encodedBuffer, const cDTDescriptor* sd, uint tupleLength = NOT_DEFINED);
	static inline uint Decode(uint method, char* encodedBuffer, char* decodedBuffer, const cDTDescriptor* dtd, uint tupleLength = NOT_DEFINED);
	static inline uint GetEncodedSize(uint method, char* sourceBuffer, const cDTDescriptor* sd, uint tupleLength = NOT_DEFINED);
	static inline uint EncodedSize(uint method, char* sourceBuffer, const cDTDescriptor* sd);
	bool IncrementUType(const cSpaceDescriptor* pSd);
	static inline bool IncrementUType(const char* tuple, const cSpaceDescriptor* pSd, int incValue);

	// for multi range query purpose
	inline unsigned int GetLastUInt(const cSpaceDescriptor* pSd);
	inline void AddValue(unsigned int value, const cSpaceDescriptor* pSd);

	// for histogram purpose
	inline void AddToHistogram(cHistogram** histogram, const cDTDescriptor* dtd) const;

	// for ri purpose
	//static unsigned int SameValues(char* cBitString_Mask, const char* cNTuple_prefix, const char* cNTuple_tuple, const cDTDescriptor* pSd, unsigned int sameValues);

	//static double CommonPrefixLength(const char* cNTuple_prefix, const char* cNTuple_tuple, const cDTDescriptor* pSd, unsigned int prefixLength);
	//static double PrefixLength(const char* cNTuple_prefix, const char* cNTuple_tuple, const cDTDescriptor* pSd, unsigned int prefixLength);

	//static bool StartsWith(char* cNTuple_prefix, const char* cNTuple_tuple, const cDTDescriptor* pSd, unsigned int prefixLength);
    //static char* CompleteMinRefItem(char* cBitString_Mask, const char* cNTuple_minItem, const char* cNTuple_key, char* cNTuple_partMinItem, char* cNTuple_result, const cDTDescriptor* pSd);


	//static bool Equal(char* cBitString_Mask1, const char* cNTuple_t1, char* cBitString_Mask2, const char* cNTuple_t2, const cDTDescriptor* pSd);
	//static char* GetMinRefItem(const char* cNTuple_t1, const char* cNTuple_t2, char* cNTuple_result, const cDTDescriptor* pSd);

	static inline uint CutTuple(const char* cBitString_Mask, const char* cNTuple_RI, const char* cNTuple_Key, char* cNTuple_Result, const cDTDescriptor* pSd);
	static inline char* MergeTuple(const char* cBitString_Mask, const char* cNTuple_RI, const char* cNTuple_Key, char* cNTuple_Result, const cDTDescriptor* pSd);
	static inline char* SetMask(const char* cBitString_Mask, const char* cNTuple_RI, const char* cNTuple_Key, char* cBitString_Result, const cDTDescriptor* pSd);
	static inline char* SetMask(const char* cNTuple_t1, const char* cNTuple_t2, char* cBitString_Result, const cDTDescriptor* pSd);
	static inline char* SetMask(const char* cNTuple_t1, const char* cBitString_Mask1, const char* cNTuple_t2, const char* cBitString_Mask2, char* cBitString_Result, const cDTDescriptor* pSd);
	static inline char* SetMinRefItem(const char* cNTuple_RI, const char* cNTuple_Key, char* cNTuple_Result, const cDTDescriptor* pSd);
	static char* MergeMasks(char* cBitString_Mask1, char* cBitString_Mask2, const char* cNTuple_RI1, const char* cNTuple_RI2, char* cBitString_Result, const cDTDescriptor* pSd);

	static inline bool IsCompatible(const char* cBitString_Mask, const char* cNTuple_RI, const char* cNTuple_Key, const cDTDescriptor* pSd);
};
}}}

namespace common {
	namespace datatype {
		namespace tuple {
typedef cCommonNTuple<unsigned char> cNTuple;
typedef cCommonNTuple<unsigned short> cLNTuple;
}}}

#include "common/datatype/tuple/cTuple.h"

namespace common {
	namespace datatype {
		namespace tuple {

/**
* Constructor
*/
template <class TLength>
cCommonNTuple<TLength>::cCommonNTuple(): mData(NULL)
{
}

/**
* Constructor
*/
template <class TLength>
cCommonNTuple<TLength>::cCommonNTuple(const cSpaceDescriptor *spaceDescriptor): mData(NULL)
{
	Resize(spaceDescriptor);
}

/**
* Constructor
*/
template <class TLength>
cCommonNTuple<TLength>::cCommonNTuple(const cSpaceDescriptor *spaceDescriptor, unsigned int currentLength): mData(NULL)
{
	Resize(spaceDescriptor, currentLength);
}

/**
* Constructor
*/
template <class TLength>
cCommonNTuple<TLength>::cCommonNTuple(char* buffer)
{
	mData = buffer + sizeof(cCommonNTuple<TLength>);
}

/**
* Destructor
*/
template <class TLength>
cCommonNTuple<TLength>::~cCommonNTuple()
{
	Free();
}

template <class TLength>
void cCommonNTuple<TLength>::Free(cMemoryBlock *memBlock)
{
	if (memBlock != NULL)
	{
		mData = NULL;
	}
	else if (mData != NULL)
	{
		delete mData;
		mData = NULL;
	}
}

template <class TLength>
inline unsigned int cCommonNTuple<TLength>::GetUInt(const char* data, unsigned int order, const cSpaceDescriptor* pSd)
{
	return *(((unsigned int*)(data + sizeof(TLength))) + order);
}

template <class TLength>
inline ullong cCommonNTuple<TLength>::GetULong(const char* data, unsigned int order, const cSpaceDescriptor* pSd)
{
	return *(((ullong*) (data + sizeof(TLength))) + order);
}

/**
* Read tuple from stream.
*/
template <class TLength>
inline bool cCommonNTuple<TLength>::Read(cStream *stream, const cSpaceDescriptor* pSd)
{
	stream->Read(mData, sizeof(TLength));
	return stream->Read(mData + sizeof(TLength), GetDataSize(pSd));
}

/**
 * Increment values of unsigned data types, return false if a value is the max value.
 */
template <class TLength>
bool cCommonNTuple<TLength>::IncrementUType(const cSpaceDescriptor* pSd)
{
	bool ret = true;
	unsigned int length = GetLength();

	for (unsigned int i = 0 ; i < length ; i++)
	{
		unsigned int value;

		switch(pSd->GetDimensionTypeCode(i))
		{
		case cUInt::CODE:
			value = GetUInt(i, pSd);
			if (value == cUInt::MAX)
			{
				ret = false;
			}
			else
			{
				SetValue(i, value+1, pSd);
			}
			break;
		default:
			ret = false;
			break;
		}
	}
	return ret;
}

/**
* Resize the tuple acording to space descriptor
*/
template <class TLength>
void cCommonNTuple<TLength>::Resize(const cSpaceDescriptor* pSd, unsigned int currentLength)
{
	if (mData != NULL)
	{
		delete mData;
	}
	unsigned int size = cCommonNTuple<TLength>::GetMaxSize(NULL, pSd);
	mData = new char[size];
	((TLength*)mData)[0] = currentLength;
}

/**
* Resize the tuple acording to space descriptor
* \param len Size of the tuple.
*/
template <class TLength>
void cCommonNTuple<TLength>::Resize(const cDTDescriptor* pSd, unsigned int currentLength)
{
	Resize((cSpaceDescriptor*)pSd, currentLength);
}

/**
* Resize the tuple acording to space descriptor. Size is get from the space descritor
*/
template <class TLength>
bool cCommonNTuple<TLength>::Resize(const cDTDescriptor* pDtd, cMemoryBlock* memBlock)
{
	cSpaceDescriptor *pSd = (cSpaceDescriptor*)pDtd;

	if (mData != NULL && memBlock == NULL)
	{
		delete mData;
	}

	uint size = pSd->GetDimension() * pSd->GetDimensionType(0)->GetSize() + sizeof(TLength);

	if (memBlock == NULL)
	{
		mData = new char[size];
	}
	else
	{
		mData = memBlock->GetMemory(size);
	}

	if (mData != NULL)
	{
		mData[0] = pSd->GetDimension();
		Clear(pSd);
	}

	return mData != NULL;
}

/**
* Resize the tuple acording to space descriptor. Size is get from the space descritor
*/
template <class TLength>
void cCommonNTuple<TLength>::Resize(const cCommonNTuple<TLength> &tuple, const cDTDescriptor *pSd)
{
	Resize(pSd);
	CopyTo(tuple, pSd);
}

/**
* \return Length of the tuple. In other words, the number of items in the tuple.
*/
template <class TLength>
inline TLength cCommonNTuple<TLength>::GetLength() const
{
	return *((TLength *)mData);
}

template <class TLength>
inline char* cCommonNTuple<TLength>::GetData() const
{
	return mData;
}

template <class TLength>
inline void cCommonNTuple<TLength>::SetData(char* pData)
{
	mData = pData;
}

template <class TLength>
inline cCommonNTuple<TLength>::operator char*() const
{
	return mData;
}

template <class TLength>
inline char cCommonNTuple<TLength>::GetCode()
{
	return CODE; 
}

/**
* \Return The size of the tuple in memory when serialized
*/
template <class TLength>
inline unsigned int cCommonNTuple<TLength>::GetSize(const cDTDescriptor* pDtd) const
{
	return cCommonNTuple<TLength>::GetSize(mData, pDtd);
}

template <class TLength>
inline unsigned int cCommonNTuple<TLength>::GetSize_instance(const char *data, const cDTDescriptor *pDtd) const
{
	return cCommonNTuple<TLength>::GetSize(data, pDtd);
}

/**
* \Return The size of the tuple in memory when serialized
*/
template <class TLength>
inline unsigned int cCommonNTuple<TLength>::GetSize(uint tupleSize) const
{
	return sizeof(TLength) + tupleSize;
}

/**
* \Return The size of the tuple in memory when serialized
*/
template <class TLength>
inline unsigned int cCommonNTuple<TLength>::GetMaxSize(const cDTDescriptor* dtd) const
{
	return cCommonNTuple<TLength>::GetMaxSize(mData, dtd);
}

/**
* \Return The size of the data stored in the tuple
*/
template <class TLength>
inline unsigned int cCommonNTuple<TLength>::GetDataSize(const cSpaceDescriptor *pSd)
{
	uint length = *((TLength*)mData);
	return pSd->GetLSize(length);
}

/**
* Set all bits of the tuple values to zero (except the )
*/
template <class TLength>
void cCommonNTuple<TLength>::Clear(const cSpaceDescriptor* pSd)
{
	uint size = GetDataSize(pSd);
	memset(mData + sizeof(TLength), 0, size);
}

template <class TLength>
inline void cCommonNTuple<TLength>::Clear(char* data, const cSpaceDescriptor* pSd)
{
	memset(data + sizeof(TLength), 0, pSd->GetSize());
}

/**
* Set min value in the order-th coordinate.
*/
template <class TLength>
inline void cCommonNTuple<TLength>::Clear(unsigned int order, const cSpaceDescriptor* pSd)
{
	SetValue(order, 0, pSd);
}

/**
* Set the length of this cCommonNTuple<TLength>.
* \param len Length of this cCommonNTuple<TLength>.
*/
template <class TLength>
void cCommonNTuple<TLength>::SetLength(unsigned int len)
{
	// assert(len < sizeof(TLength));
	*((TLength*)mData) = (TLength)len;
}

/*
*  Recursively set the lengths of each NTuple in this NTuple
*/
template <class TLength>
void cCommonNTuple<TLength>::SetLength(char* data, const cSpaceDescriptor* pSd)
{
	uint length = pSd->GetDimension();
	*((TLength*)data) = (TLength)length;

	if (pSd->IsAnyDimDescriptor())
	{
		for (uint i = 0; i < length; i++)
		{
			if (pSd->GetDimensionTypeCode(i) == cCommonNTuple<TLength>::CODE)
			{
				SetLength(GetTuple(data, i, pSd), pSd->GetDimSpaceDescriptor(i));
			}
		}
	}
}

template <class TLength>
void cCommonNTuple<TLength>::SetLength(char* tuple, unsigned int len)
{
	// assert(len < sizeof(TLength));
	*((TLength*)tuple) = (TLength)len;
}

/**
* Compare all values in tuples
* \return true if tuples are the same
*/
//inline bool cCommonNTuple<TLength>::operator == (const cCommonNTuple<TLength> &tuple, const cSpaceDescriptor* pSd) const
//{
//	assert(mDimension == tuple.GetDimension());
//	return memcmp(mData, tuple.GetData(), mDimension * mTypeSize) == 0;
//}

/**
* Compare all values in tuples
* \return true if tuples are diferent
*/
//inline bool cCommonNTuple<TLength>::operator != (const cCommonNTuple<TLength> &tuple) const
//{
//	assert(mDimension == tuple.GetDimension());
//	return memcmp(mData, tuple.GetData(), mDimension * mTypeSize) != 0;
//}

/** 
* Compare values in this tuple and another tuple
* \return true if this tuple is greater then the second tuple in all dimension
*/
//inline bool cCommonNTuple<TLength>::operator > (const cCommonNTuple<TLength> &tuple) const
//{
//	assert(mDimension == tuple.GetDimension());
//
//	for (unsigned int i = 0; i < (unsigned int)mDimension; i++)
//	{
//		if (Equal(tuple,i) == -1)
//			return false;
//	}
//
//	return true;
//}

/**
* Compare the tuples from the first dimension until values in dimension are different.
* \return true if first coordinate in this tuple that does not match in both tuples has a greater value.
*/
//inline bool cCommonNTuple<TLength>::Greater(const cCommonNTuple<TLength> &tuple) const
//{
//	assert(mDimension == tuple.GetDimension());
//	printf("cCommonNTuple<TLength>::Greater: Check it! The first byte of the int is the first byte in the char array!");
//	return memcmp(mData, tuple.GetData(), mDimension * mTypeSize) > 0;
//}

/**
 * Semantic of this method is rather problematic for cTuple, since cDataType::CompareArray is designed for
 * comparison of two arrays of primitive data type values.
 */
template <class TLength>
inline int cCommonNTuple<TLength>::CompareArray(const char* array1, const char* array2, uint length)
{
	printf("Warning: cCommonNTuple<TLength>::CompareArray(): This method should not be invoked!\n");
	return -1;
}

template <class TLength>
inline unsigned int cCommonNTuple<TLength>::HashValue(const char *array, unsigned int length, unsigned int hashTableSize)
{
	printf("Warning: cCommonNTuple<TLength>::HashValue(): This method should not be invoked!\n");
	return 0;
}

/**
* Byte comparison between tuples. Using the memcmp function. Similar to Greater method.
* \param tuple 
* \return
*		- -1 if the this tuple is smaller than the parameter
*		- 0 if the tupleas are the same
*		- 1 if the parameter is bigger than this tuple
*/
template <class TLength>
inline int cCommonNTuple<TLength>::Compare(const cCommonNTuple<TLength> &tuple, const cSpaceDescriptor* pSd) const
{
	// * !!! It seems that memcmp is not useable, due to opposite ordering of bytes. !!!
	// return memcmp((void*)mData, (void*)tuple.GetData(), mDimension * mTypeSize);
	return CompareLexicographically(tuple, pSd);
}

template <class TLength>
inline int cCommonNTuple<TLength>::Equal(const char* tuple1, const char* tuple2, uint tupleLength, const cDTDescriptor *pSd)
{
	return CompareLexicographically(tuple1, tuple2, (cSpaceDescriptor*) pSd);
}

template <class TLength>
inline int cCommonNTuple<TLength>::Equal(const char* tuple1, const char* tuple2, const cDTDescriptor *pSd)
{
	return CompareLexicographically(tuple1, tuple2, (cSpaceDescriptor*)pSd);
}

template <class TLength>
inline int cCommonNTuple<TLength>::Equal(const char* tuple1, const char* tuple2, const cSpaceDescriptor *pSd)
{
	return CompareLexicographically(tuple1, tuple2, pSd);
}

/*
 * Return true if both tuples are the same.
 */
template <class TLength>
inline bool cCommonNTuple<TLength>::IsEqual(const char* tuple1, const char* tuple2, const cDTDescriptor *pDtd)
{
	uint l1 = GetLength(tuple1, pDtd);
	uint l2 = GetLength(tuple2, pDtd);

	return true;
	// return CompareLexicographically(tuple1, tuple2, (cSpaceDescriptor*)pSd);
}

template <class TLength>
inline int cCommonNTuple<TLength>::Compare(const char* tuple1, const char* tuple2, const cDTDescriptor *pSd)
{
	return CompareLexicographically(tuple1, tuple2, (cSpaceDescriptor*)pSd);
}

template <class TLength>
inline int cCommonNTuple<TLength>::Compare(const char* tuple2, const cSpaceDescriptor *pSd) const
{
	// you must check dimension a typeSize
	return CompareLexicographically(tuple2, pSd);
}

template <class TLength>
inline int cCommonNTuple<TLength>::Compare(const char* tuple2, const cDTDescriptor *dd) const
{
	// you must check dimension a typeSize
	return CompareLexicographically(tuple2, (cSpaceDescriptor *)dd);
}

/** 
* Compare values in this tuple and another tuple
* \return -1 if this tuple is smaller then tuple in parameter, 0 if tuples are the same, 1 if this > tuple.
*/
template <class TLength>
inline int cCommonNTuple<TLength>::CompareLexicographically(const cCommonNTuple<TLength> &tuple, const cSpaceDescriptor* pSd) const
{
	int ret = 0;
	unsigned int dim;
	if (tuple.GetLength() < GetLength()) 
	{
		dim = tuple.GetLength();

		for (unsigned int i = 0; i < dim; i++)
		{
			ret = Equal(tuple, i, pSd);
			if (ret != 0)
			{
				return ret;
			}
		}
		return -1;
	} else
	{
		dim = GetLength();

		for (unsigned int i = 0; i < dim; i++)
		{
			ret = Equal(tuple, i, pSd);
			if (ret != 0)
			{
				return ret;
			}
		}

		if (tuple.GetLength() == GetLength())
		{
			return 0;
		} else
		{
			return 1;
		}
	}

	//unsigned int dim;
	//
	//if (tuple.GetLength() < GetLength())
	//{
	//	dim = tuple.GetLength();
	//} else
	//{
	//	dim = GetLength();
	//}

	//for (unsigned int i = 0; i < dim ; i++)
	//{
	//	ret = Equal(tuple, i, pSd);
	//	if (ret != 0)
	//	{
	//		return ret;
	//	}
	//}

	//if (tuple.GetLength() == GetLength())
	//{
	//	return 0;
	//} else
	//{
	//	if (tuple.GetLength() > GetLength())
	//	{
	//		return -1;
	//	} else
	//	{
	//		return 1;
	//	}
	//}
}

/** 
* Compare values in this tuple and another tuple
* \return -1 if this tuple is smaller then tuple in parameter, 0 if tuples are the same, 1 if this > tuple.
*/
template <class TLength>
inline int cCommonNTuple<TLength>::CompareLexicographically(const char* tuple2, const cSpaceDescriptor* pSd) const
{
	return cCommonNTuple<TLength>::CompareLexicographically(GetData(), tuple2, pSd);
}

/** 
* Compare two tuples.
* \return -1 if the tuple1 < tuple2, 0 if tuples are the same, 1 if tuple1 > tuple2s.
*/
template <class TLength>
inline int cCommonNTuple<TLength>::CompareLexicographically(const char* tuple1, const char* tuple2, const cSpaceDescriptor* pSd)
{
	// assert(mDimension == tuple.GetDimension());
	int ret = 0;
	//unsigned char dim = GetLength(tuple1) < GetLength(tuple2) ? GetLength(tuple1) : GetLength(tuple2);
	unsigned char dim;
	if (GetLength(tuple1) < GetLength(tuple2)) 
	{
		dim = GetLength(tuple1);

		for (unsigned int i = 0; i < dim; i++)
		{
			ret = Equal(tuple1, tuple2, i, pSd);
			if (ret != 0)
			{
				return ret;
			}
		}
		return -1;
	} else
	{
		dim = GetLength(tuple2);

		for (unsigned int i = 0; i < dim; i++)
		{
			ret = Equal(tuple1, tuple2, i, pSd);
			if (ret != 0)
			{
				return ret;
			}
		}

		//return 0; // TOTO VYRIESIT !!!
		if (GetLength(tuple1) == GetLength(tuple2))
		{
			return 0;
		} else
		{
			return 1;
		}
	}

	//if (GetLength(tuple1) == GetLength(tuple2))
	//{
	//	return 0;
	//} else
	//{
	//	if (GetLength(tuple1) < GetLength(tuple2))
	//	{
	//		return -1;
	//	} else
	//	{
	//		return 1;
	//	}
	//}
}


/** 
* Compare two tuples.
* \return -1 if the tuple1 < tuple2, 0 if tuples are the same, 1 if tuple1 > tuple2s.
*/
template <class TLength>
inline int cCommonNTuple<TLength>::ComparePartly(const char* tuple1, const char* tuple2, const cDTDescriptor* pSd, unsigned int startOrder)
{
	// assert(mDimension == tuple.GetDimension());
	cSpaceDescriptor *spaceDescriptor = (cSpaceDescriptor *)pSd;
	int ret = 0;
	unsigned int dim = GetLength(tuple1);

	for (unsigned int i = startOrder; i < dim; i++)
	{
		ret = Equal(tuple1, tuple2, i, i - startOrder, spaceDescriptor);
		if (ret != 0)
		{
			return ret;
		}
	}

	return 0; // TOTO VYRIESIT !!!
	/*if (GetLength(tuple1) == GetLength(tuple2))
	{
		return 0;
	} else
	{
		return 1;
	}*/
}

template <class TLength>
inline unsigned int cCommonNTuple<TLength>::HashValue(char *tuple, uint hashTableSize, const cDTDescriptor* dtd)
{
	unsigned int hashValue = 0;
	cSpaceDescriptor *sd = (cSpaceDescriptor*)dtd;
	unsigned int dim = GetLength(tuple);
	const unsigned int ValuePerByte = 256;

	for (unsigned int i = 0 ; i < dim ; i++)
	{
		unsigned int tmp = (GetUInt(tuple, i, sd)  % ValuePerByte)  << (i*8);
		// unsigned int tmp = GetUInt(i, sd);
		hashValue += tmp;
	}
	return hashValue % hashTableSize;
}

/// Copy only the pointer address!! Rewrite the pointers in this tuple by pointers in the parameter tuple. 
/// This method can even lead to heap error during the delete phase, because you will try to free the same memory twice.
template <class TLength>
inline void cCommonNTuple<TLength>::Copy(const cCommonNTuple<TLength> &tuple)
{
	mData = tuple.GetData();
}

/**
* Copy data from this tuple into the parameter.
* \param data Destination memory.
* \return Size of the data copied into the data parameter.
*/
template <class TLength>
inline unsigned int cCommonNTuple<TLength>::CopyTo(char* data, const cSpaceDescriptor* pSd) const
{
	unsigned int size = GetSize(pSd);
	memcpy(data, mData, size);
	return size;
}

/**
* Copy data from the parameter into this tuple.
* \param data Source memory
* \return Size of the data copied from the data parameter.
*/
template <class TLength>
inline unsigned int cCommonNTuple<TLength>::Copy(const char* data, const cSpaceDescriptor* pSd)
{
	unsigned int size = GetSize(pSd);
	memcpy(mData, data, size);
	return size;
}

/**
* \param data Destination memory.
* \return Size of the data copied into the data parameter.
*/
template <class TLength>
inline unsigned int cCommonNTuple<TLength>::CopyTo(char *data, const cDTDescriptor* pDtD) const
{
	return CopyTo(data, (const cSpaceDescriptor*)pDtD);
}

/**
* \param data Source memory
* \return pDtD data descriptor
*/
template <class TLength>
inline unsigned int cCommonNTuple<TLength>::Copy(const char *srcData, const cDTDescriptor* pDtD)
{
	return Copy(srcData, (const cSpaceDescriptor*)pDtD);
}

template <class TLength>
inline void cCommonNTuple<TLength>::SetMaxValue(unsigned int order, const cSpaceDescriptor* pSd)
{
	switch (pSd->GetDimensionTypeCode(order))
	{
	case cInt::CODE:
		SetValue(order, cInt::MAX, pSd);
		break;
	case cUInt::CODE:
		SetValue(order, cUInt::MAX, pSd);
		break;
	case cShort::CODE:
		SetValue(order, cShort::MAX, pSd);
		break;
	case cChar::CODE:
		SetValue(order, cChar::MAX, pSd);
		break;
	}
}

/**
* Set the tuple
*/
template <class TLength>
void cCommonNTuple<TLength>::SetValue(const cCommonNTuple<TLength> &tuple, const cSpaceDescriptor* pSd)
{
	SetValue(mData, tuple, pSd);
}

/**
* Set the float value of the dimension specified by the order parameter
* \param order Dimension whose value should be set
* \param value New value of the dimension
* \invariant order < tuple dimension
*/
template <class TLength>
void cCommonNTuple<TLength>::SetValue(unsigned int order, float value, const cSpaceDescriptor* pSd)
{
	*(((float*)(mData + sizeof(TLength))) + order) = value;
}

/**
* Set the double value of the dimension specified by the order parameter
* \param order Dimension whose value should be set
* \param value New value of the dimension
* \invariant order < tuple dimension
*/
template <class TLength>
void cCommonNTuple<TLength>::SetValue(unsigned int order, double value, const cSpaceDescriptor* pSd)
{
	*(((double*)(mData + sizeof(TLength))) + order) = value;
}

/**
* Set the int value of the dimension specified by the order parameter
* \param order Dimension whose value should be set
* \param value New value of the dimension
* \invariant order < tuple dimension
*/
template <class TLength>
void cCommonNTuple<TLength>::SetValue(unsigned int order, int value, const cSpaceDescriptor* pSd)
{
	*(((int*)(mData + sizeof(TLength))) + order) = value;
	//*(int*)(mData + sizeof(TLength) + pSd->GetByteIndex(order)) = value;
}

/**
* Set the unsigned int value of the dimension specified by the order parameter
* \param order Dimension whose value should be set
* \param value New value of the dimension
* \invariant order < tuple dimension
*/
template <class TLength>
void cCommonNTuple<TLength>::SetValue(unsigned int order, unsigned int value, const cSpaceDescriptor* pSd)
{
	*(((unsigned int*)(mData + sizeof(TLength))) + order) = value;
	//*(unsigned int*)(mData + sizeof(TLength) + pSd->GetByteIndex(order)) = value;
}

template <class TLength>
void cCommonNTuple<TLength>::SetValue(unsigned int order, ullong value, const cSpaceDescriptor* pSd)
{
	*(((ullong*) (mData + sizeof(TLength))) + order) = value;
	//*(unsigned int*)(mData + sizeof(TLength) + pSd->GetByteIndex(order)) = value;
}

/**
* Set the char value of the dimension specified by the order parameter
* \param order Dimension whose value should be set
* \param value New value of the dimension
* \invariant order < tuple dimension
*/
template <class TLength>
void cCommonNTuple<TLength>::SetValue(unsigned int order, char value, const cSpaceDescriptor* pSd)
{
	*(((char*)(mData + sizeof(TLength))) + order) = value;
	//*(char*)(mData + sizeof(TLength) + pSd->GetByteIndex(order)) = value;
}

/**
* Set the unsigned char value of the dimension specified by the order parameter
* \param order Dimension whose value should be set
* \param value New value of the dimension
* \invariant order < tuple dimension
*/
template <class TLength>
void cCommonNTuple<TLength>::SetValue(unsigned int order, unsigned char value, const cSpaceDescriptor* pSd)
{
	*(((unsigned char*)(mData + sizeof(TLength))) + order) = value;
	//*(unsigned char*)(mData + sizeof(TLength) + pSd->GetByteIndex(order)) = value;
}

/**
* Set the unsigned char value of the dimension specified by the order parameter
* \param order Dimension whose value should be set
* \param value New value of the dimension
* \invariant order < tuple dimension
*/
template <class TLength>
void cCommonNTuple<TLength>::SetValue(unsigned int order, wchar_t value, const cSpaceDescriptor* pSd)
{
	*(((wchar_t*)(mData + sizeof(TLength))) + order) = value;
	//*(wchar_t*)(mData + sizeof(TLength) + pSd->GetByteIndex(order)) = value;
}

/**
* Set the short value of the dimension specified by the order parameter
* \param order Dimension whose value should be set
* \param value New value of the dimension
* \invariant order < tuple dimension
*/
template <class TLength>
void cCommonNTuple<TLength>::SetValue(unsigned int order, short value, const cSpaceDescriptor* pSd)
{
	*(((short*)(mData + sizeof(TLength))) + order) = value;
	//*(short*)(mData + sizeof(TLength) + pSd->GetByteIndex(order)) = value;
}

/**
* Set the unsigned short value of the dimension specified by the order parameter
* \param order Dimension whose value should be set
* \param value New value of the dimension
* \invariant order < tuple dimension
*/
template <class TLength>
void cCommonNTuple<TLength>::SetValue(unsigned int order, unsigned short value, const cSpaceDescriptor* pSd)
{
	*(((unsigned short*)(mData + sizeof(TLength))) + order) = value;
	//*(unsigned short*)(mData + sizeof(TLength) + pSd->GetByteIndex(order)) = value;
}

template <class TLength>
inline float cCommonNTuple<TLength>::GetFloat(unsigned int order, const cSpaceDescriptor* pSd) const
{	
	return *(((float*)(mData + sizeof(TLength))) + order);
}

template <class TLength>
inline double cCommonNTuple<TLength>::GetDouble(unsigned int order, const cSpaceDescriptor* pSd) const
{
	return *(((double*)(mData + sizeof(TLength))) + order);
}

/**
* Return the int value of the dimension specified by the order parameter
* \param order Dimension whose value should be returned
* \return int value of the dimension
* \invariant order < tuple dimension
* \invariant value type in the dimension has to be int
*/
template <class TLength>
inline int cCommonNTuple<TLength>::GetInt(unsigned int order, const cSpaceDescriptor* pSd) const
{
	double temp = sizeof(TLength);
	 temp = sizeof(cCommonNTuple);
	return *(((int*)(mData + sizeof(TLength))) + order);
	//return *(int*)(mData + sizeof(TLength) + pSd->GetByteIndex(order));
}

/**
* Return the unsigned int value of the dimension specified by the order parameter
* \param order Dimension whose value should be returned
* \return unsigned int value of the dimension
* \invariant order < tuple dimension
* \invariant value type in the dimension has to be unsigned int
*/
template <class TLength>
inline unsigned int cCommonNTuple<TLength>::GetUInt(unsigned int order, const cSpaceDescriptor* pSd) const
{
	return *(((unsigned int*)(mData + sizeof(TLength))) + order);
	//return *(unsigned int*)(mData + sizeof(TLength) + pSd->GetByteIndex(order));
}

/**
* Return the byte value of the dimension specified by the order parameter
* \param order Dimension whose value should be returned
* \return byte value of the dimension
* \invariant order < tuple dimension
* \invariant value type in the dimension has to be char (byte)
*/
template <class TLength>
inline char cCommonNTuple<TLength>::GetByte(unsigned int order, const cSpaceDescriptor* pSd) const
{
	return *(mData + sizeof(TLength) + order);
	//return *(mData + sizeof(TLength) + pSd->GetByteIndex(order));
}

/**
* Return the byte value of the dimension specified by the order parameter
* \param order Dimension whose value should be returned
* \return byte value of the dimension
* \invariant order < tuple dimension
* \invariant value type in the dimension has to be char (byte)
*/
template <class TLength>
inline unsigned char cCommonNTuple<TLength>::GetUChar(unsigned int order, const cSpaceDescriptor* pSd) const
{
	return *(((unsigned char*)(mData + sizeof(TLength))) + order);
	//return *(unsigned char*)(mData + sizeof(TLength) + pSd->GetByteIndex(order));
}

/**
* Return the unicode char value of the dimension specified by the order parameter
* \param order Dimension whose value should be returned
* \return byte value of the dimension
* \invariant order < tuple dimension
* \invariant value type in the dimension has to be char (byte)
*/
template <class TLength>
inline wchar_t cCommonNTuple<TLength>::GetWChar(unsigned int order, const cSpaceDescriptor* pSd) const
{
	return *(((wchar_t*)(mData + sizeof(TLength))) + order);
	//return *(mData + sizeof(TLength) + pSd->GetByteIndex(order));
}


template <class TLength>
inline char cCommonNTuple<TLength>::GetCChar(unsigned int order, const cSpaceDescriptor* pSd) const
{
	return *(((mData + sizeof(TLength))) + order);
	//return *(mData + sizeof(TLength) + pSd->GetByteIndex(order));
}



template <class TLength>
inline char* cCommonNTuple<TLength>::GetPChar(const char* data, unsigned int order, const cDTDescriptor* pSd)
{
	return ((char*) (data + sizeof(TLength))) + order;
}

/**
* Return the short value of the dimension specified by the order parameter
* \param order Dimension whose value should be returned
* \return short value of the dimension
* \invariant order < tuple dimension
* \invariant value type in the dimension has to be short
*/
template <class TLength>
inline short cCommonNTuple<TLength>::GetShort(unsigned int order, const cSpaceDescriptor* pSd) const
{
	return *(((short*)(mData + sizeof(TLength))) + order);
	//return *(short*)((mData + sizeof(TLength)) + pSd->GetByteIndex(order));
}

/**
* Return the short value of the dimension specified by the order parameter
* \param order Dimension whose value should be returned
* \return short value of the dimension
* \invariant order < tuple dimension
* \invariant value type in the dimension has to be short
*/
template <class TLength>
inline unsigned short cCommonNTuple<TLength>::GetUShort(unsigned int order, const cSpaceDescriptor* pSd) const
{
	return *(((unsigned short*)(mData + sizeof(TLength))) + order);
	//return *(unsigned short*)(mData + sizeof(TLength) + pSd->GetByteIndex(order));
}

/**
* Return the cString value of the dimension
* \param order Dimension whose value should be returned
* \param string returned value
*/
template <class TLength>
inline void cCommonNTuple<TLength>::GetString(unsigned int order, cString &string, const cSpaceDescriptor* pSd) const
{
	const int STRING_LENGTH = 128;
	char str[STRING_LENGTH];

	switch (pSd->GetDimensionTypeCode(order))
	{
	case cInt::CODE:
		sprintf_s((char*)str, STRING_LENGTH, "%d", GetInt(order, pSd));
		break;
	case cShort::CODE:
		sprintf_s((char*)str, STRING_LENGTH, "%d", GetShort(order, pSd));
		break;
	case cChar::CODE:
		sprintf_s((char*)str, STRING_LENGTH, "%c", this->GetByte(order, pSd));
		break;
	}
	string += (char*)str;
}

/**
* Return the float value of the dimension specified by the order parameter by reference
* \param order Dimension whose value should be returned
* \return float value of the dimension by reference
* \invariant order < tuple dimension
* \invariant value type in the dimension has to be float
*/
template <class TLength>
inline float* cCommonNTuple<TLength>::GetPFloat(unsigned int order, const cSpaceDescriptor* pSd) const
{
	printf("cCommonNTuple<TLength>::GetPFloat - float type not supported!\n");

}

/**
* Return the int value of the dimension specified by the order parameter by reference
* \param order Dimension whose value should be returned
* \return int value of the dimension by reference
* \invariant order < tuple dimension
* \invariant value type in the dimension has to be int
*/
template <class TLength>
inline int* cCommonNTuple<TLength>::GetPInt(unsigned int order, const cSpaceDescriptor* pSd) const
{
	return (int*)(mData + sizeof(TLength) + pSd->GetDimensionOrder(order));
}

/**
* Return the unsigned int value of the dimension specified by the order parameter by reference
* \param order Dimension whose value should be returned
* \return unsigned int value of the dimension by reference
* \invariant order < tuple dimension
* \invariant value type in the dimension has to be unsigned int
*/
template <class TLength>
inline unsigned int* cCommonNTuple<TLength>::GetPUInt(unsigned int order, const cSpaceDescriptor* pSd) const
{
	return (unsigned int*)(mData + sizeof(TLength) + pSd->GetDimensionOrder(order));
}

/**
* Return the byte value of the dimension specified by the order parameter by reference
* \param order Dimension whose value should be returned
* \return byte value of the dimension by reference
* \invariant order < tuple dimension
* \invariant value type in the dimension has to be char (byte)
*/
template <class TLength>
inline char* cCommonNTuple<TLength>::GetPByte(unsigned int order, const cSpaceDescriptor* pSd) const
{
	return (mData + sizeof(TLength) + pSd->GetDimensionOrder(order));
}

/**
* Return the byte value of the dimension specified by the order parameter by reference
* \param order Dimension whose value should be returned
* \return byte value of the dimension by reference
* \invariant order < tuple dimension
* \invariant value type in the dimension has to be char (byte)
*/
template <class TLength>
inline unsigned char* cCommonNTuple<TLength>::GetPUChar(unsigned int order, const cSpaceDescriptor* pSd) const
{
	return (unsigned char*)(mData + sizeof(TLength) + pSd->GetDimensionOrder(order));
}

template <class TLength>
inline unsigned char* cCommonNTuple<TLength>::GetPTuple(unsigned int order, const cSpaceDescriptor* pSd) const
{
	return (unsigned char*)mData + order * pSd->GetSize();
}

// get inner Tuples
template <class TLength>
inline char* cCommonNTuple<TLength>::GetTuple(const char *data,unsigned int order, const cSpaceDescriptor* pSd)
{
	return (char*)(data + GetSizePart(data, order, pSd));
}

template <class TLength>
inline unsigned int cCommonNTuple<TLength>::GetSizePart(const char *data, unsigned int order, const cSpaceDescriptor* pSd)
{
	// unsigned int previousItemsSize = SIZEPREFIX_LEN;  // obsolote lines, deleted: 23.4.2015
	uint byteOrder = SIZEPREFIX_LEN;

	for (unsigned int i = 0 ; i < order ; i++)
	{
		// GetItemSizeShift(data, i, pSd, previousItemsSize);
		byteOrder += pSd->GetDimensionType(i)->GetSize_instance(data + byteOrder, pSd->GetDimSpaceDescriptor(i));
	}
	// return previousItemsSize;
	return byteOrder;
}

/**
* Write tuple into stream.
*/
template <class TLength>
inline bool cCommonNTuple<TLength>::Write(cStream *stream, const cSpaceDescriptor* pSd) const
{
	return stream->Write(mData, GetSize(pSd));
}


/**
* Compare the tuple values in every dimension starting from first until values in dimensions are different. 
* This is the same method as Compare.
* \return the return value correponds to the first different dimension in tuples. 
*		- -1 if this tuple has lower value
*		- 0 if the tuples are the same
*		- 1 if this tuple is bigger
*/
template <class TLength>
inline int cCommonNTuple<TLength>::Equal(const cCommonNTuple<TLength> &tuple, const cSpaceDescriptor* pSd) const
{
	return Compare(tuple, pSd);
}

template <class TLength>
inline int cCommonNTuple<TLength>::Equal(const char* tuple2, const cSpaceDescriptor* pSd) const
{
	// you must check dimension a typeSize
	return CompareLexicographically(tuple2, pSd);
}

/**
* Equality test of order-th coordinate. 
* \return -1 if this < tuple, 0 if tuples' coordinates are the same, 1 if this > tuple.
*/
template <class TLength>
inline int cCommonNTuple<TLength>::Equal(const cCommonNTuple<TLength> &tuple, unsigned int order, const cSpaceDescriptor* pSd) const
{
	assert(order < pSd->GetDimension());
	int ret = 1;

	switch (pSd->GetDimensionTypeCode(order))
	{
	case cUInt::CODE:
		if (GetUInt(order, pSd) < tuple.GetUInt(order, pSd))
		{
			ret = -1;
		}
		else if (GetUInt(order, pSd) == tuple.GetUInt(order, pSd))
		{
			ret = 0;
		}
		break;
	case cUShort::CODE:
		if (GetUShort(order, pSd) < tuple.GetUShort(order, pSd))
		{
			ret = -1;
		}
		else if (GetUShort(order, pSd) == tuple.GetUShort(order, pSd))
		{
			ret = 0;
		}
		break;
	case cChar::CODE:
		if ((unsigned char)GetByte(order, pSd) < (unsigned char)tuple.GetByte(order, pSd))
		{
			ret = -1;
		}
		else if (GetByte(order, pSd) == tuple.GetByte(order, pSd))
		{
			ret = 0;
		}
		break;
	}
	return ret;
}

// ------------------------------------------------------------------------------------------
// Static methods
// ------------------------------------------------------------------------------------------

/**
* \return Length of the tuple. In other words, the number of items in the tuple.
*/
template <class TLength>
inline TLength cCommonNTuple<TLength>::GetLength(const char* data, const cDTDescriptor* pSd)
{
	return *((TLength*)data);
}

template <class TLength>
inline void cCommonNTuple<TLength>::SetValue(char* data, const cCommonNTuple<TLength> &tuple, const cSpaceDescriptor* pSd)
{
	memcpy(data, tuple.GetData(), tuple.GetSize(pSd));
}

template <class TLength>
inline char* cCommonNTuple<TLength>::Copy(char* cNTuple_dst, const char* cNTuple_src, const cDTDescriptor *pSd)
{
	memcpy(cNTuple_dst, cNTuple_src, GetSize(cNTuple_src, ((cSpaceDescriptor*)pSd)));
	return cNTuple_dst;
}

template <class TLength>
inline void cCommonNTuple<TLength>::Copy(char* cNTuple_dst, const char* cNTuple_src, const cSpaceDescriptor *pSd)
{
	memcpy(cNTuple_dst, cNTuple_src, GetSize(cNTuple_src, pSd));
}

template <class TLength>
inline void cCommonNTuple<TLength>::CopyFromTuple(char* cNTuple_dst, const char* cTuple_src, const int size, const int length)
{
	memcpy(cNTuple_dst, &length, sizeof(TLength));
	memcpy(cNTuple_dst + sizeof(TLength), cTuple_src, size);
}

/**
* Resize the tuple acording to the space descriptor and set the tuple.
*/
template <class TLength>
bool cCommonNTuple<TLength>::ResizeSet(cCommonNTuple<TLength> &t1, const cCommonNTuple<TLength>& t2, const cDTDescriptor* pDtd, cMemoryBlock* memBlock)
{
	cSpaceDescriptor *sd = (cSpaceDescriptor*)pDtd;
	bool ret;
	if ((ret = t1.Resize(sd, memBlock)))
	{
		t1.SetValue(t2, sd);
	}
	return ret;
}

template <class TLength>
void cCommonNTuple<TLength>::Free(cCommonNTuple<TLength> &tuple, cMemoryBlock *memBlock)
{
	tuple.Free(memBlock);
}

template <class TLength>
inline int cCommonNTuple<TLength>::GetInt(const char* data, unsigned int order, const cSpaceDescriptor* pSd)
{
	/*upraveny kod*/
	return *(int*)(data + ((cSpaceDescriptor*)pSd)->GetDimensionOrder(order));
	//return *(((int*)(data + sizeof(TLength))) + order);
}

template <class TLength>
inline float cCommonNTuple<TLength>::GetFloat(const char* data, unsigned int order, const cSpaceDescriptor* pSd)
{
	/*upraveny kod*/
	return *(float*)(data + ((cSpaceDescriptor*)pSd)->GetDimensionOrder(order));
	//return *(((float*)(data + sizeof(TLength))) + order);
}

template <class TLength>
inline unsigned short cCommonNTuple<TLength>::GetUShort(const char* data, unsigned int order, const cSpaceDescriptor* pSd)
{
	return *(((unsigned short*)(data + sizeof(TLength))) + order);
}

template <class TLength>
inline unsigned char cCommonNTuple<TLength>::GetUByte(const char* data, unsigned int order, const cSpaceDescriptor* pSd)
{
	return *(((unsigned char*)(data + sizeof(TLength))) + order);
}

template <class TLength>
inline char cCommonNTuple<TLength>::GetByte(const char* data, unsigned int order, const cSpaceDescriptor* pSd)
{
	return *(((char*)(data + sizeof(TLength))) + order);
}

template <class TLength>
inline wchar_t cCommonNTuple<TLength>::GetWChar(const char* data, unsigned int order, const cSpaceDescriptor* pSd)
{
	return *(((wchar_t*)(data + sizeof(TLength))) + order);
}
template <class TLength>
inline char cCommonNTuple<TLength>::GetCChar(const char* data, unsigned int order, const cSpaceDescriptor* pSd)
{
	return *(((data + sizeof(TLength))) + order);
}


template <class TLength>
inline void cCommonNTuple<TLength>::SetValue(char *data, unsigned int order, unsigned int value, const cSpaceDescriptor* pSd)
{
	*(((unsigned int*)(data + sizeof(TLength))) + order) = value;
}

template <class TLength>
inline void cCommonNTuple<TLength>::SetValue(char *data, unsigned int order, ullong value, const cSpaceDescriptor* pSd)
{
	*(((ullong*) (data + sizeof(TLength))) + order) = value;
}

template <class TLength>
inline void cCommonNTuple<TLength>::SetValue(char *data, unsigned int order, unsigned short value, const cSpaceDescriptor* pSd)
{
	*(((unsigned short*)(data + sizeof(TLength))) + order) = value;
}

template <class TLength>
inline void cCommonNTuple<TLength>::SetValue(char *data, unsigned int order, unsigned char value, const cSpaceDescriptor* pSd)
{
	*(((unsigned char*)(data + sizeof(TLength))) + order) = value;
}

template <class TLength>
inline void cCommonNTuple<TLength>::SetValue(char *data, unsigned int order, char value, const cSpaceDescriptor* pSd)
{
	*(data + sizeof(TLength) + order) = value;
}

/**
 * \return The size of the static representation.
 */
template <class TLength>
inline unsigned int cCommonNTuple<TLength>::GetSize(const char *data, const cDTDescriptor* pDtd)
{
	cSpaceDescriptor* sd = (cSpaceDescriptor*)pDtd;
	uint byteSize = 0;

	if (data == NULL)
	{
		byteSize = sd->GetTypeSize();
	}
	else
	{
		uint length = *((TLength*)data);
		byteSize += sizeof(TLength);

		if (sd->IsAnyDimDescriptor())
		{
			for (uint i = 0 ; i < length ; i++)
			{
				byteSize += sd->GetDimensionType(i)->GetSize_instance(data + byteSize, sd->GetDimSpaceDescriptor(i));
			}
		}
		else
		{
			byteSize += sd->GetLSize(length);
		}
	}
	return byteSize;
}

/**
 * The size of the instance.
 */
template <class TLength>
inline int cCommonNTuple<TLength>::GetObjectSize(const cSpaceDescriptor *pSd)
{
	return sizeof(cCommonNTuple<TLength>) + GetSize(NULL, pSd);
}

/**
 * Return the size of the tuple. You must know SpaceDescriptor.
 * Warning: It return the maximal size for the space descriptor.
 */
template <class TLength>
inline unsigned int cCommonNTuple<TLength>::GetMaxSize(const char *data, const cDTDescriptor* pSd)
{
	uint size = sizeof(TLength);

	if (pSd  != NULL)
	{
		size += ((cSpaceDescriptor*)pSd)->GetSize();
	}
	return size;
}

/* Return the size of tuple with specified length */
template <class TLength>
unsigned int cCommonNTuple<TLength>::GetLSize(uint tupleLength, const cDTDescriptor* dtd)
{
	cSpaceDescriptor* sd = (cSpaceDescriptor*) dtd;
	return sizeof(TLength) + sd->GetLSize(tupleLength);
}

template <class TLength>
uint cCommonNTuple<TLength>::GetDimension(const cDTDescriptor* dtd)
{
	return ((cSpaceDescriptor *)dtd)->GetDimension();
}

/**
* Equality test of order-th coordinate. 
* \return -1 if this < tuple, 0 if tuples' coordinates are the same, 1 if this > tuple.
*/
template <class TLength>
inline int cCommonNTuple<TLength>::Equal(const char* cUnfTuple_t1, const char* cUnfTuple_t2, const unsigned int order, const cSpaceDescriptor* pSd)
{
	char typeCode = pSd->GetDimensionTypeCode(order);
	// assert(order < mDimension);
	int ret = 1;

	switch (typeCode)
	{
	case cUInt::CODE:
		if (GetUInt(cUnfTuple_t1, order, pSd) < GetUInt(cUnfTuple_t2, order, pSd))
		{
			ret = -1;
		}
		else if (GetUInt(cUnfTuple_t1, order, pSd) == GetUInt(cUnfTuple_t2, order, pSd))
		{
			ret = 0;
		}
		break;
	case cUShort::CODE:
		if (GetUShort(cUnfTuple_t1, order, pSd) < GetUShort(cUnfTuple_t2, order, pSd))
		{
			ret = -1;
		}
		else if (GetUShort(cUnfTuple_t1, order, pSd) == GetUShort(cUnfTuple_t2, order, pSd))
		{
			ret = 0;
		}
		break;
	case cChar::CODE:
		if ((unsigned char)GetByte(cUnfTuple_t1, order, pSd) < (unsigned char)GetByte(cUnfTuple_t2, order, pSd))
		{
			ret = -1;
		}
		else if (GetByte(cUnfTuple_t1, order, pSd) == GetByte(cUnfTuple_t2, order, pSd))
		{
			ret = 0;
		}
		break;
	case cWChar::CODE:
		if (GetWChar(cUnfTuple_t1, order, pSd) < GetWChar(cUnfTuple_t2, order, pSd))
		{
			ret = -1;
		}
		else if (GetWChar(cUnfTuple_t1, order, pSd) == GetWChar(cUnfTuple_t2, order, pSd))
		{
			ret = 0;
		}
		break;
	}
	return ret;
}

/**
* Equality test of order-th coordinate. 
* \return -1 if this < tuple, 0 if tuples' coordinates are the same, 1 if this > tuple.
*/
template <class TLength>
inline int cCommonNTuple<TLength>::Equal(const char* cUnfTuple_t1, const char* cUnfTuple_t2, const unsigned int order1, const unsigned int order2, const cSpaceDescriptor* pSd)
{
	char typeCode = pSd->GetDimensionTypeCode(order1);
	// assert(order < mDimension);
	int ret = 1;

	switch (typeCode)
	{
	case cUInt::CODE:
		if (GetUInt(cUnfTuple_t1, order1, pSd) < GetUInt(cUnfTuple_t2, order2, pSd))
		{
			ret = -1;
		}
		else if (GetUInt(cUnfTuple_t1, order1, pSd) == GetUInt(cUnfTuple_t2, order2, pSd))
		{
			ret = 0;
		}
		break;
	case cUShort::CODE:
		if (GetUShort(cUnfTuple_t1, order1, pSd) < GetUShort(cUnfTuple_t2, order2, pSd))
		{
			ret = -1;
		}
		else if (GetUShort(cUnfTuple_t1, order1, pSd) == GetUShort(cUnfTuple_t2, order2, pSd))
		{
			ret = 0;
		}
		break;
	case cChar::CODE:
		if ((unsigned char)GetByte(cUnfTuple_t1, order1, pSd) < (unsigned char)GetByte(cUnfTuple_t2, order2, pSd))
		{
			ret = -1;
		}
		else if (GetByte(cUnfTuple_t1, order1, pSd) == GetByte(cUnfTuple_t2, order2, pSd))
		{
			ret = 0;
		}
		break;
	case cWChar::CODE:
		if (GetWChar(cUnfTuple_t1, order1, pSd) < GetWChar(cUnfTuple_t2, order2, pSd))
		{
			ret = -1;
		}
		else if (GetWChar(cUnfTuple_t1, order1, pSd) == GetWChar(cUnfTuple_t2, order2, pSd))
		{
			ret = 0;
		}
		break;
	}
	return ret;
}

template <class TLength>
bool cCommonNTuple<TLength>::IncrementUType(const char* tuple, const cSpaceDescriptor* pSd, int incValue)
{
	unsigned char nTupleLength = cCommonNTuple<TLength>::GetLength(tuple);
	bool ret = true;

	for (unsigned int i = 0; i < nTupleLength; i++)
	{
		unsigned int value;

		switch (pSd->GetDimensionTypeCode(i))
		{
		case cUInt::CODE:
			value = GetUInt(tuple, i, pSd);
			/*if (value == cUInt::MAX) 
			{
				printf("Critical Error: cTuple::IncrementUType(): value == cUInt::MAX");
				ret = false;
			}
			else*/
			{
				SetValue((char *) tuple, i, value + incValue, pSd);
			}
			break;
		case cShort::CODE:
/*			value = GetShort(tuple, i, pSd);
			if (value == cShort::MAX)
			{
				printf("Critical Error: cTuple::IncrementUType(): value == cShort::MAX");
				ret = false;
			}
			else
			{
				SetValue((char *) tuple, i, value + incValue, pSd);
			}*/
			break;
		default:
			ret = false;
			break;
		}
	}
	return ret;
}

// Method codes values in sourceBuffer by chosen algorithm
// Method aligns to bytes!!!
template <class TLength>
inline uint cCommonNTuple<TLength>::Encode(uint method, const char* sourceBuffer, char* encodedBuffer, const cDTDescriptor* dtd, uint tupleLength)
{
	cSpaceDescriptor *sd = (cSpaceDescriptor *) dtd;
	TLength nTupleLength = cCommonNTuple<TLength>::GetLength(sourceBuffer);
	*((TLength*)encodedBuffer) = nTupleLength;
        
	uint ret = Coder::encode(method, ((cSpaceDescriptor *)dtd)->GetDimensionSize(0) , sourceBuffer + cChar::SER_SIZE, encodedBuffer + cChar::SER_SIZE, nTupleLength);
	return cNumber::BitsToBytes(ret) + SIZEPREFIX_LEN;
}

// Method decodes values in sourceBuffer by chosen algorithm
template <class TLength>
inline uint cCommonNTuple<TLength>::Decode(uint method, char* encodedBuffer, char* decodedBuffer, const cDTDescriptor* dtd, uint tupleLength)
{
	cSpaceDescriptor *sd = (cSpaceDescriptor *) dtd;
    TLength nTupleLength = cCommonNTuple<TLength>::GetLength(encodedBuffer);
    *((TLength*)decodedBuffer) = nTupleLength;

	uint ret = Coder::decode(method, ((cSpaceDescriptor*) dtd)->GetDimensionSize(0), encodedBuffer + cChar::SER_SIZE, decodedBuffer + cChar::SER_SIZE, nTupleLength);
	return ret + SIZEPREFIX_LEN;
}

// Method estimates the estimate size of tuple
// Method aligns to bytes!!!
template <class TLength>
inline uint cCommonNTuple<TLength>::GetEncodedSize(uint method, char* sourceBuffer, const cDTDescriptor* dtd, uint tupleLength)
{
	cSpaceDescriptor *sd = (cSpaceDescriptor *) dtd;
	uint dim = cCommonNTuple<TLength>::GetLength(sourceBuffer);

	return cNumber::BitsToBytes(Coder::GetSize(method, sd->GetDimensionSize(0), sourceBuffer + cChar::SER_SIZE, dim));
}


// DO NOT USE IT !!! Coder::estimateSizeInBits(...) != Coder::encode(...)
template <class TLength>
uint cCommonNTuple<TLength>::EncodedSize(uint method, char* sourceBuffer, const cDTDescriptor* dtd)
{
	cSpaceDescriptor *sd = (cSpaceDescriptor *)dtd;
	uint ret = Coder::estimateSizeInBits(method, sd->GetDimensionSize(0), sourceBuffer + cChar::SER_SIZE, sd->GetDimension());
	return ret + (SIZEPREFIX_LEN * cNumber::BYTE_LENGTH);
}

/********************************* PETER CHOVANEC 10.3.2012 ***************************************/

/// Compute non euclidian distance between tuples $t1 and $t2 in dimension $order
template <class TLength>
inline double cCommonNTuple<TLength>::TaxiCabDistanceValue(const char* cNTuple_t1, const char* cNTuple_t2, unsigned int order, const cSpaceDescriptor* pSd)
{
	double sum = 0;

	switch (pSd->GetDimensionTypeCode(order))
	{
		case cFloat::CODE:
			sum += ((GetFloat(cNTuple_t1, order, pSd) > GetFloat(cNTuple_t2, order, pSd)) ? (GetFloat(cNTuple_t1, order, pSd) - GetFloat(cNTuple_t2, order, pSd)) : (GetFloat(cNTuple_t2, order, pSd) - GetFloat(cNTuple_t1, order, pSd)));
			break;
		case cUInt::CODE:
			sum += ((GetUInt(cNTuple_t1, order, pSd) > GetUInt(cNTuple_t2, order, pSd)) ? (GetUInt(cNTuple_t1, order, pSd) - GetUInt(cNTuple_t2, order, pSd)) : (GetUInt(cNTuple_t2, order, pSd) - GetUInt(cNTuple_t1, order, pSd)));
			break;
		case cInt::CODE:
			sum += ((GetInt(cNTuple_t1, order, pSd) > GetInt(cNTuple_t2, order, pSd)) ? (GetInt(cNTuple_t1, order, pSd) - GetInt(cNTuple_t2, order, pSd)) : (GetInt(cNTuple_t2, order, pSd) - GetInt(cNTuple_t1, order, pSd)));
			break;
		case cUShort::CODE:
			sum += ((GetUShort(cNTuple_t1, order, pSd) > GetUShort(cNTuple_t2, order, pSd)) ? (GetUShort(cNTuple_t1, order, pSd) - GetUShort(cNTuple_t2, order, pSd)) : (GetUShort(cNTuple_t2, order, pSd) - GetUShort(cNTuple_t1, order, pSd)));
			break;
//		case cShort::CODE:
//			sum += ((GetShort(cTuple_t1, order, pSd) > GetShort(cTuple_t2, order, pSd)) ? (GetShort(cTuple_t1, order, pSd) - GetShort(cTuple_t2, order, pSd)) : (GetShort(cTuple_t2, order, pSd) - GetShort(cTuple_t1, order, pSd)));
//			break;
		case cChar::CODE:
			printf("cTuple::TaxiDistance - method not supported for char.");
			break;
	}

	return sum;
}

template <class TLength>
inline void cCommonNTuple<TLength>::Add(const char* cNTuple_t1, const char* cNTuple_t2, char* cNTuple_result, unsigned int order, const cSpaceDescriptor* pSd)
{
	switch (pSd->GetDimensionTypeCode(order))
	{
		case cUInt::CODE:
			SetValue(cNTuple_result, order, GetUInt(cNTuple_t1, order, pSd) + GetUInt(cNTuple_t2, order, pSd), pSd);
			break;
		//case cInt::CODE:
		//	SetValue(cNTuple_result, order, GetInt(cNTuple_t1, order, pSd) + GetInt(cNTuple_t2, order, pSd), pSd);
		//	break;
		//case cUShort::CODE:
		//	SetValue(cNTuple_result, order, GetUShort(cNTuple_t1, order, pSd) + GetUShort(cNTuple_t2, order, pSd), pSd);
		//	break;
		//case cShort::CODE:
		//	SetValue(cNTuple_result, order, GetShort(cNTuple_t1, order, pSd) + GetShort(cNTuple_t2, order, pSd), pSd);
		//	break;
		//case cChar::CODE:
		//	SetValue(cNTuple_result, order, GetByte(cNTuple_t1, order, pSd) + GetByte(cNTuple_t2, order, pSd), pSd);
		//	break;
		//case cFloat::CODE:
		//	SetValue(cNTuple_result, order, GetFloat(cNTuple_t1, order, pSd) + GetFloat(cNTuple_t2, order, pSd), pSd);
		//	break;
	}
}

template <class TLength>
inline void cCommonNTuple<TLength>::Subtract(const char* cNTuple_t1, const char* cNTuple_t2, char* cNTuple_result, unsigned int order, const cSpaceDescriptor* pSd)
{
	switch (pSd->GetDimensionTypeCode(order) /* GetTypeCode(order) */)
	{
		case cUInt::CODE:
			SetValue(cNTuple_result, order, GetUInt(cNTuple_t1, order, pSd) - GetUInt(cNTuple_t2, order, pSd), pSd);
			break;
		//case cInt::CODE:
		//	SetValue(cNTuple_result, order, GetInt(cNTuple_t1, order, pSd) - GetInt(cNTuple_t2, order, pSd), pSd);
		//	break;
		//case cUShort::CODE:
		//	SetValue(cNTuple_result, order, GetUShort(cNTuple_t1, order, pSd) - GetUShort(cNTuple_t2, order, pSd), pSd);
		//	break;
		//case cShort::CODE:
		//	SetValue(cNTuple_result, order, GetShort(cNTuple_t1, order, pSd) - GetShort(cNTuple_t2, order, pSd), pSd);
		//	break;
		//case cChar::CODE:
		//	SetValue(cNTuple_result, order, GetByte(cNTuple_t1, order, pSd) - GetByte(cNTuple_t2, order, pSd), pSd);
		//	break;
		//case cFloat::CODE:
		//	SetValue(cNTuple_result, order, GetFloat(cNTuple_t1, order, pSd) - GetFloat(cNTuple_t2, order, pSd), pSd);
		//	break;
	}
}

/**
* Print this tuple
* \param delim This string is printed out at the end of the tuple.
*/
template <class TLength>
void cCommonNTuple<TLength>::Print(const char *delim, const cSpaceDescriptor* pSd) const
{
	printf("(");
	unsigned int dim = GetLength();

	for (unsigned int i = 0 ; i < dim  ; i++)
	{
		Print(i, "", pSd);
		if (i != dim - 1)
		{
			printf(", ");
		}
	}
	printf(")%s", delim);
}

template <class TLength>
void cCommonNTuple<TLength>::Print(const char *string, const cDTDescriptor* pSd) const
{
	Print(string, (cSpaceDescriptor*)pSd);
}

///**
//* Set the tuple. The dimension and data types must be the same!
//*/
//void cCommonNTuple<TLength>::SetTuple(const cCommonNTuple<TLength> &tuple, const cSpaceDescriptor* pSd)
//{
//	Clear(pSd);
//
//	memcpy(mData, tuple.GetData(), pSd->GetDimension());
//}

/**
* Print just one dimension of this tuple
* \param order Order of the dimension.
* \param string This string is printed out at the end of the tuple.
*/
template <class TLength>
void cCommonNTuple<TLength>::Print(unsigned int order, const char *string, const cSpaceDescriptor* pSd) const
{
	Print(mData, order, string, pSd);
}

template <class TLength>
void cCommonNTuple<TLength>::Print(const char* data, unsigned int order, const char *string, const cSpaceDescriptor* pSd)
{
	char typeCode = pSd->GetDimensionTypeCode(0);

	if (typeCode == cChar::CODE)
	{
		printf("%X", (unsigned char)GetByte(data, order, pSd));
	} 
	else  if (typeCode == cInt::CODE)
	{
		printf("%d", GetInt(data, order, pSd));
		
	}
	else if (typeCode ==  cUInt::CODE)
	{
		printf("%u", GetUInt(data, order, pSd));
	}
	else if (typeCode == cWChar::CODE)
	{
		printf("%C", GetWChar(data, order, pSd));
	}
	printf("%s", string);
}

// ------------------------------------------------------------------------------------------
// Static methods
// ------------------------------------------------------------------------------------------
/// Error: Only uint!
template <class TLength>
void cCommonNTuple<TLength>::Print(const char *data, const char* delim, const cSpaceDescriptor* pSd)
{
	unsigned int dim = GetLength(data);

	printf("%u(", dim);
	for (unsigned int i = 0 ; i < dim ; i++)
	{
		if (i < dim-1)
		{
			Print(data, i, ",", pSd);
		}
		else
		{
			Print(data, i, "", pSd);
		}
	}
	printf(")%s", delim);
}

template <class TLength>
void cCommonNTuple<TLength>::Print(const char *data, const char* delim, const cDTDescriptor* dd)
{
	Print(data, delim, (cSpaceDescriptor*)dd);
}

template <class TLength>
void cCommonNTuple<TLength>::Print2File(FILE *StreamInfo, const char *data, const char* delim, const cSpaceDescriptor* pSd)
{
	unsigned int dim = GetLength(data);

	fprintf(StreamInfo, "(");
	for (unsigned int i = 0 ; i < dim ; i++)
	{
		fprintf(StreamInfo, "%u", cCommonNTuple<TLength>::GetUInt(data, i, pSd));
		if (i != dim-1)
		{
			fprintf(StreamInfo, ",");
		}
	}
	fprintf(StreamInfo, ")%s", delim);
}

template <class TLength>
void cCommonNTuple<TLength>::Print2File(FILE *StreamInfo, const char *data, const char* delim, const cDTDescriptor* dd)
{
	Print2File(StreamInfo, data, delim, (cSpaceDescriptor*)dd);
}

/// t1 + t2 operator
template <class TLength>
char* cCommonNTuple<TLength>::Add(const char* cNTuple_t1, const char* cNTuple_t2, char* cNTuple_result, const cDTDescriptor* pSd)
{
	unsigned int dim = ((cSpaceDescriptor*)pSd)->GetDimension();

	for (unsigned int j = 0; j < dim; j++)
	{
		Add(cNTuple_t1, cNTuple_t2, cNTuple_result, j, ((cSpaceDescriptor*)pSd));
	}

	return cNTuple_result;
}

/// t1 - t2 operator
template <class TLength>
char* cCommonNTuple<TLength>::Subtract(const char* cNTuple_t1, const char* cNTuple_t2, char* cNTuple_result, const cDTDescriptor* pSd)
{
	unsigned int dim = ((cSpaceDescriptor*)pSd)->GetDimension();

	for (unsigned int j = 0; j < dim; j++)
	{
		Subtract(cNTuple_t1, cNTuple_t2, cNTuple_result, j, ((cSpaceDescriptor*)pSd));
	}

	return cNTuple_result;
}

/// Compute non euclidian distance from tuple to this tuple
template <class TLength>
double cCommonNTuple<TLength>::TaxiCabDistance(const char* cTuple_t1, const char* cTuple_t2, const cDTDescriptor* pSd)
{
	double sum = 0;
	unsigned int dim = ((cSpaceDescriptor*)pSd)->GetDimension();

	for (unsigned int i = 0; i < dim; i++)
	{
		sum += TaxiCabDistanceValue(cTuple_t1, cTuple_t2, i, ((cSpaceDescriptor*)pSd));
	}

	return sum;
}

/// Compare query with prefix
/// \return true it is same in all dimensions
/*template <class TLength>
bool cCommonNTuple<TLength>::StartsWith(char* cNTuple_prefix, const char* cNTuple_tuple, const cDTDescriptor* pSd, unsigned int prefixLength)
{
	const cSpaceDescriptor* spaceDescriptor = ((cSpaceDescriptor*)pSd);

	for (unsigned int i = 0; i < prefixLength; i++)
	{
		if (Equal(cNTuple_prefix, cNTuple_tuple, i, spaceDescriptor) != 0)
		{
			return false;
		}
	}

	return true;
}*/


// if tuple1 is same as tuple2 in dimension i, then set mask 1, otherwise 0
template <class TLength>
inline char* cCommonNTuple<TLength>::SetMask(const char* cBitString_Mask, const char* cNTuple_RI, const char* cNTuple_Key, char* cBitString_Result, const cDTDescriptor* pSd)
{
	unsigned int dim = GetLength(cNTuple_RI);

	for (unsigned int i = 0; i < dim; i++)
	{
		cBitString::SetBit(cBitString_Result, i, cBitString::GetBit(cBitString_Mask, i) && !abs(Equal(cNTuple_RI, cNTuple_Key, i, i, (cSpaceDescriptor*) pSd)));
	}

	return cBitString_Result;
}

/// Returns the number of 1's in the mask, if the cTuple_tuple will be added
/*template <class TLength>
unsigned int cCommonNTuple<TLength>::SameValues(char* cBitString_Mask, const char* cNTuple_prefix, const char* cNTuple_tuple, const cDTDescriptor* pSd, unsigned int sameValues)
{
	unsigned int dim = GetLength(cNTuple_prefix);
	unsigned int length = 0;

	for (unsigned int i = 0; i < dim; i++)
	{
		if ((cBitString::GetBit(cBitString_Mask, i) == 1) && (Equal(cNTuple_prefix, cNTuple_tuple, i, ((cSpaceDescriptor*)pSd)) == 0))
			length++;
	}

	return (length == sameValues) ? length : 0;
}*/

// Compute new reference item from the actual reference item and inserting item
/*char* cCommonNTuple<TLength>::SetMinRefItem(const char* cNTuple_item, const char* cNTuple_refItem,  char* cBitString_Mask, char* cNTuple_result, const cDTDescriptor* pSd)
{
	const cSpaceDescriptor* spaceDescriptor = ((cSpaceDescriptor*)pSd);
	unsigned int dim = GetLength(cNTuple_refItem);
	unsigned int riValue = 0;
	unsigned int itemValue = 0;

	for (unsigned int i = 0; i < dim; i++)
	{
		riValue = GetUInt(cNTuple_refItem, i, spaceDescriptor);
		itemValue = GetUInt(cNTuple_item, i, spaceDescriptor);

		if (cBitString::GetBit(cBitString_Mask, i) == 1)
			SetValue(cNTuple_result, i, riValue, spaceDescriptor);
		else
			SetValue(cNTuple_result, i, riValue < itemValue ? riValue : itemValue, spaceDescriptor);
	}

	SetLength(cNTuple_result, dim);

	return cNTuple_result;
}*/

template <class TLength>
inline char* cCommonNTuple<TLength>::SetMinRefItem(const char* cNTuple_RI, const char* cNTuple_Key, char* cNTuple_Result, const cDTDescriptor* pSd)
{
	const cSpaceDescriptor* spaceDescriptor = ((cSpaceDescriptor*)pSd);
	uint tupleLength = GetLength(cNTuple_Key);

	for (uint i = 0; i < tupleLength; i++)
	{
		SetValue(cNTuple_Result, i, GetUInt(cNTuple_Key, i, spaceDescriptor) <= GetUInt(cNTuple_RI, i, spaceDescriptor) ? GetUInt(cNTuple_Key, i, spaceDescriptor) : GetUInt(cNTuple_RI, i, spaceDescriptor), spaceDescriptor);
	}

	SetLength(cNTuple_Result, tupleLength);

	return cNTuple_Result;
}


/// If the new prefix length is same as the old prefix length, returns length, otherwise 0
/*template <class TLength>
double cCommonNTuple<TLength>::CommonPrefixLength(const char* cNTuple_prefix, const char* cNTuple_tuple, const cDTDescriptor* pSd, unsigned int prefixLength)
{
	double length = 0;

	for (unsigned int i = 0; i < prefixLength; i++)
	{
		if (Equal(cNTuple_prefix, cNTuple_tuple, i, ((cSpaceDescriptor*)pSd)) == 0)
			length++;
		else
			break;
	}

	return (length == prefixLength) ? length : 0;
}

/// Returns the length of new prefix after insert of specified tuple
template <class TLength>
double cCommonNTuple<TLength>::PrefixLength(const char* cNTuple_prefix, const char* cNTuple_tuple, const cDTDescriptor* pSd, unsigned int prefixLength)
{
	double length = 0;

	for (unsigned int i = 0; i < prefixLength; i++)
	{
		if (Equal(cNTuple_prefix, cNTuple_tuple, i, ((cSpaceDescriptor*)pSd)) == 0)
			length++;
		else
			break;
	}

	return length;
}*/


/// t1 + t2 operator
template <class TLength>
inline char* cCommonNTuple<TLength>::MergeTuple(const char* cBitString_Mask, const char* cNTuple_RI, const char* cNTuple_Key, char* cNTuple_Result, const cDTDescriptor* pSd)
{
	const cSpaceDescriptor* spaceDescriptor = ((cSpaceDescriptor*)pSd);
	unsigned int tupleLength = GetLength(cNTuple_RI);
	// in the case of ntuple with different lengths use this tupleLength computation
	//unsigned int tupleLength = GetLength(cNTuple_t1) + GetLength(cNTuple_t2) - cBitString::GetNumberOfBits(cBitString_Mask, GetLength(cNTuple_t1), true);
	int order = 0;

	for (unsigned int i = 0; i < tupleLength; i++)
	{
		if (cBitString::GetBit(cBitString_Mask, i))
			SetValue(cNTuple_Result, i, GetUInt(cNTuple_RI, i, spaceDescriptor), spaceDescriptor);
		else
			SetValue(cNTuple_Result, i, GetUInt(cNTuple_RI, i, spaceDescriptor) + GetUInt(cNTuple_Key, order++, spaceDescriptor), spaceDescriptor);
	}

	SetLength(cNTuple_Result, tupleLength);

	return cNTuple_Result;
}

/// t1 - t2 operator
template <class TLength>
inline uint cCommonNTuple<TLength>::CutTuple(const char* cBitString_Mask, const char* cNTuple_RI, const char* cNTuple_Key, char* cNTuple_Result, const cDTDescriptor* pSd)
{
	const cSpaceDescriptor* spaceDescriptor = ((cSpaceDescriptor*)pSd);
	unsigned int tupleLength = GetLength(cNTuple_RI);
	unsigned int resultLength = 0;

	for (unsigned int i = 0; i < tupleLength; i++)
	{
		if (cBitString::GetBit(cBitString_Mask, i) == 0)
			SetValue(cNTuple_Result, resultLength++, GetUInt(cNTuple_Key, i, spaceDescriptor) - GetUInt(cNTuple_RI, i, spaceDescriptor), spaceDescriptor);
	}

	SetLength(cNTuple_Result, resultLength);

	return resultLength;
}

// it creates complete minimal ri from the calculated part and first item of block
/*template <class TLength>
char* cCommonNTuple<TLength>::CompleteMinRefItem(char* cBitString_Mask, const char* cNTuple_minItem, const char* cNTuple_key, char* cNTuple_partMinItem, char* cNTuple_result, const cDTDescriptor* pSd)
{
	const cSpaceDescriptor* spaceDescriptor = ((cSpaceDescriptor*)pSd);
	unsigned int tupleLength = GetLength(cNTuple_key);
	unsigned int j = 0;

	for (unsigned int i = 0; i < tupleLength; i++)
	{
		if (cBitString::GetBit(cBitString_Mask, i) == 1)
		{
			SetValue(cNTuple_result, i, GetUInt(cNTuple_key, i, spaceDescriptor), spaceDescriptor);
		}
		else
		{
			SetValue(cNTuple_result, i, GetUInt(cNTuple_minItem, i, spaceDescriptor) + GetUInt(cNTuple_partMinItem, j++, spaceDescriptor), spaceDescriptor);
		}
	}

	SetLength(cNTuple_result, tupleLength);

	return cNTuple_result;
}*/

template <class TLength>
inline char* cCommonNTuple<TLength>::SetMask(const char* cNTuple_t1, const char* cNTuple_t2, char* cBitString_Result, const cDTDescriptor* pSd)
{
	const cSpaceDescriptor* spaceDescriptor = ((cSpaceDescriptor*)pSd);
	uint tupleLength = GetLength(cNTuple_t1);

	for (unsigned int i = 0; i < tupleLength; i++)
	{
		cBitString::SetBit(cBitString_Result, i, GetUInt(cNTuple_t1, i, spaceDescriptor) == GetUInt(cNTuple_t2, i, spaceDescriptor));
	}	
	
	return cBitString_Result;
}

template <class TLength>
inline char* cCommonNTuple<TLength>::SetMask(const char* cNTuple_t1, const char* cBitString_Mask1, const char* cNTuple_t2, const char* cBitString_Mask2, char* cBitString_Result, const cDTDescriptor* pSd)
{
	const cSpaceDescriptor* spaceDescriptor = ((cSpaceDescriptor*)pSd);
	uint tupleLength = GetLength(cNTuple_t1);

	for (uint i = 0; i < tupleLength; i++)
	{
		cBitString::SetBit(cBitString_Result, i, (cBitString::GetBit(cBitString_Mask1, i) && cBitString::GetBit(cBitString_Mask2, i) && (GetUInt(cNTuple_t1, i, spaceDescriptor) == GetUInt(cNTuple_t2, i, spaceDescriptor))));
	}

	return cBitString_Result;
}


template <class TLength>
inline char* cCommonNTuple<TLength>::MergeMasks(char* cBitString_Mask1, char* cBitString_Mask2, const char* cNTuple_RI1, const char* cNTuple_RI2, char* cBitString_Result, const cDTDescriptor* pSd)
{
	const cSpaceDescriptor* spaceDescriptor = ((cSpaceDescriptor*)pSd);
	uint tupleLength = GetLength(cNTuple_RI1);

	for (unsigned int i = 0; i < tupleLength; i++)
	{
		if ((cBitString::GetBit(cBitString_Mask1, i) && cBitString::GetBit(cBitString_Mask2, i)) && (GetUInt(cNTuple_RI1, i, spaceDescriptor) == GetUInt(cNTuple_RI2, i, spaceDescriptor)))
			cBitString::SetBit(cBitString_Result, i, true);
		else
			cBitString::SetBit(cBitString_Result, i, false);
	}

	return cBitString_Result;
}


template <class TLength>
inline bool cCommonNTuple<TLength>::IsCompatible(const char* cBitString_Mask, const char* cNTuple_RI, const char* cNTuple_Key, const cDTDescriptor* pSd)
{
	const cSpaceDescriptor* spaceDescriptor = ((cSpaceDescriptor*) pSd);
	uint tupleLength = GetLength(cNTuple_RI);

	for (unsigned int i = 0; i < tupleLength; i++)
	{
		if (cBitString::GetBit(cBitString_Mask, i))
		{
			if (GetUInt(cNTuple_RI, i, spaceDescriptor) != GetUInt(cNTuple_Key, i, spaceDescriptor))
				return false;
		}
		else
		{
			if (GetUInt(cNTuple_RI, i, spaceDescriptor) > GetUInt(cNTuple_Key, i, spaceDescriptor))
				return false;
		}
	}

	return true;
}
/*
template <class TLength>
bool cCommonNTuple<TLength>::Equal(char* cBitString_Mask1, const char* cNTuple_t1, char* cBitString_Mask2, const char* cNTuple_t2, const cDTDescriptor* pSd)
{
	assert(GetLength(cNTuple_t1) == GetLength(cNTuple_t2));
	
	const cSpaceDescriptor* spaceDescriptor = ((cSpaceDescriptor*)pSd);
	unsigned int tupleLength = GetLength(cNTuple_t1);
	
	for (unsigned int i = 0; i < tupleLength; i++)
	{
		if (cBitString::GetBit(cBitString_Mask1, i) != cBitString::GetBit(cBitString_Mask2, i)) 
			return false;

		if ((cBitString::GetBit(cBitString_Mask1, i) == 1) && (cBitString::GetBit(cBitString_Mask2, i) == 1) && (GetUInt(cNTuple_t1, i, spaceDescriptor) != GetUInt(cNTuple_t2, i, spaceDescriptor)))
			return false;
	}

	return true;
}*/

template <class TLength>
inline unsigned int cCommonNTuple<TLength>::GetLastUInt(const cSpaceDescriptor* pSd)
{
	return GetUInt(GetLength() - 1, pSd);
}

template <class TLength>
inline void cCommonNTuple<TLength>::AddValue(unsigned int value, const cSpaceDescriptor* pSd)
{
	//*(((unsigned int*)(mData + sizeof(TLength))) + GetLength(mData)) = value;
	SetValue(GetLength(), value, pSd);
	SetLength(mData, GetLength(mData) + 1);
}

template <class TLength>
void cCommonNTuple<TLength>::SetValue(unsigned int order, char* cTuple_value, const cSpaceDescriptor* pSd)
{
	// get the correct inner sd for the tuple
	cSpaceDescriptor *sd = pSd->GetDimSpaceDescriptor(order);  // the correct versions: GetInnerSpaceDescriptor(order) or GetInnerSpaceDescriptor()
	unsigned int size = cTuple::GetSize(cTuple_value, sd);//dřív tady byl sd, nevím proč
	memcpy(mData + sizeof(TLength) + order * size, cTuple_value, size);
}

template <class TLength>
void cCommonNTuple<TLength>::AddToHistogram(cHistogram** histogram, const cDTDescriptor* dtd) const
{
	uint tupleLength = GetLength();

	for (uint i = 0; i < tupleLength; i++)
	{
		histogram[i]->AddValue(GetUInt(i, ((cSpaceDescriptor*)dtd)));
	}
}

// ***** Obsolote methods *****

/*
Deleted: 23.4.2015

template <class TLength>
unsigned int cCommonNTuple<TLength>::GetItemSizeShift(const char *data, unsigned int order, const cSpaceDescriptor* pSd, unsigned int &byteShift)
{
	unsigned int len;
	char typeCode = pSd->GetDimensionTypeCode(order);

	if (typeCode == cWChar::CODE)
	{
		len = sizeof(wchar_t);
		byteShift += len;
		return len;
	}
	else if (typeCode == cUInt::CODE)
	{
		len = sizeof(unsigned int);
		byteShift += len;
		return len;
	}
	else if (typeCode == cNTuple::CODE || typeCode == cTuple::CODE)
	{
		char* mem = (char*)data + byteShift;
		if (typeCode == cNTuple::CODE)
		{
			len = cNTuple::GetSize_static(mem, pSd->GetDimSpaceDescriptor(order));
		}
		else 
		{
			len = cTuple::GetSize_static(mem, pSd->GetDimSpaceDescriptor(order));
		}
		byteShift += len;
		return len;
	}
	return NULL;
}*/

}}}
#endif
