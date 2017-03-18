/**************************************************************************}
{                                                                          }
{    cBasicTypeTree.h                                                      }
{                                                                          }
{                                                                          }
{    Copyright (c) 2003                      Pavel Moravec                 }
{                                                                          }
{    VERSION: 0.1                            DATE 9/10/2003                }
{                                                                          }
{    following functionality:                                              }
{       Implementation of Basic type (float, int, etc).                    }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{                                                                          }
{**************************************************************************/

#ifndef __cBasicType_h__
#define __cBasicType_h__

#include <string.h>
#include <float.h>

#include "common/compression/Coder.h"
#include "common/stream/cStream.h"
#include "common/datatype/cDataType.h"
#include "common/datatype/cDTDescriptor.h"
#include "common/cNumber.h"
#include "common/utils/cHistogram.h"
#include "common/memdatstruct/cMemoryBlock.h"
#include "common/datatype/cComparator.h"

#ifndef MAX_STR_SIZE
#  define MAX_STR_SIZE 32
#endif

using namespace common::datatype;
using namespace common::stream;
using namespace common::compression;
using namespace common::utils;
using namespace common::memdatstruct;

template <class DataType>
class cBasicType : public cDataType
{
protected:
	DataType mValue;

public:
	typedef cBasicType<DataType> Type;
	typedef DataType T;

	static const char SER_SIZE = sizeof(T);
	static const unsigned int LengthType = cDataType::LENGTH_FIXLEN;  // VARLEN or FIXLEN data type

	cBasicType(const T& value) { mValue = value; }
	cBasicType() {}

	inline T GetValue() const;
	inline void SetValue(const T& value);
	inline char* GetData() const { return (char*)&mValue; }

	static inline unsigned int GetLSize(uint dataLength, const cDTDescriptor* dtd);

	inline unsigned int GetSize(const cDTDescriptor* pSd = NULL) const;
	inline unsigned int GetSize_instance(const char* data, const cDTDescriptor *dTd = NULL) const;
	inline unsigned int GetSize(uint itemSize) const;
	inline unsigned int GetMaxSize(const cDTDescriptor* pSd) const;

	inline void Clear();
	inline void CopyFrom(const Type &from);
	inline unsigned int CopyTo(char *dst, const cDTDescriptor* sd) const;
	inline void CopyPointersFrom(const Type &from);

	inline void Resize(const cDTDescriptor* sd);
	inline void Resize(char *dst, const cDTDescriptor* sd);

	// inline static void Format(cSizeInfo<T> &sizeInfo, cMemoryBlock* memory, Type& item)	{ }

	inline bool Equals(const Type &b);
	inline int Compare(const Type &b);
	inline bool IsZero();
	inline int Compare(const char* item2, const cDTDescriptor* sd) const;

	static inline bool ResizeSet(T& value1, const T& value2, const cDTDescriptor* sd, cMemoryBlock* memBlock);
	static inline bool ResizeSet(T* value1, const T& value2, const cDTDescriptor* sd, cMemoryBlock* memBlock);
	static void Free(T &tuple, cMemoryBlock *memBlock = NULL);

	inline bool Write(cStream *out) const;
	inline bool Read(cStream *inp);

	inline char* ToString(char*str) const;
	inline static void Print(const T& value, const char* delim, const cDTDescriptor* sd);
	inline static void Print(const char *data, const char* delim, const cDTDescriptor* sd);
	inline void Print(const char* delim, const cDTDescriptor* sd) const;

	// Arithmetic operations. Ussualy are not needed by the data structure. It should be explicitly statet in the description of the data structure
	inline void Sum(const Type& b);
	inline void Inc();

	inline int CompareArray(const char* array1, const char* array2, uint length);
	static inline unsigned int HashValue(const char *value, uint hashTableSize, const cDTDescriptor* dtd);
	static inline unsigned int HashValue(const T &value, uint hashTableSize, const cDTDescriptor* dtd);
	inline uint HashValue(const char *array, uint length, uint hashTableSize);

	static inline unsigned int GetSize(const char *data, const cDTDescriptor* pSd);
	static inline unsigned int GetMaxSize(const char *data, const cDTDescriptor* pSd);

	static inline unsigned int Decode(T* item, T* referenceItem, char *memory, unsigned int mem_size);
	static inline unsigned int Encode(const T* item, T* referenceItem, char *memory, unsigned int mem_size);

	// Instance methods working with char*
	inline const char* GetCPValue() const;

	// Static methods working with char*
	static inline int Compare(const T& value1, const T& value2, const cDTDescriptor* pSd);
	static inline bool IsEqual(const T& value1, const T& value2, const cDTDescriptor* pSd);
	static inline int Compare(const char* item1, const char* item2, const cDTDescriptor* pSd);
	static inline void Copy(char* dst, const char* src, const cDTDescriptor *pSd) { memcpy(dst, src, GetSize(src, pSd)); }

	static inline T GetValue(char* data, unsigned int order = 0);
	static inline T GetValueC(const char* data, unsigned int order = 0);
	static inline void SetValue(T& value1, const T& value2, const cDTDescriptor* pDtd);
	static inline void SetValue(char* data, const T& value, unsigned int order = 0);
	static inline void SetValueC(const char* data, const T& value, unsigned int order = 0);

	static inline char* ToString(const T& item, char* str, unsigned int &size);

	inline operator char*() const;

	// for codding purpose
	static inline uint Encode(uint method, const char* sourceBuffer, char* encodedBuffer, const cDTDescriptor* dtd, uint bufferLength = 1);
	static inline uint Decode(uint method, char* encodedBuffer, char* decodedBuffer, const cDTDescriptor* dtd, uint bufferLength = 1);
	static inline uint GetEncodedSize(uint method, char* sourceBuffer, const cDTDescriptor* sd, uint bufferLength = 1);
	static inline uint EncodedSize(uint method, char* sourceBuffer, const cDTDescriptor* dtd);

	// for data distribution histogram
	static inline uint GetDimension(const cDTDescriptor* pSd);
	inline void AddToHistogram(cHistogram** histogram, const cDTDescriptor* dtd) const;

	// for ri purpose
	static char* MergeTuple(const char* cBitString_Mask, const char* cNTuple_t1, const char* cNTuple_t2, char* cNTuple_origin, const cDTDescriptor* pSd) { return NULL; };
	static unsigned int CutTuple(const char* cBitString_Mask, const char* cNTuple_prefix, const char* cNTuple_tuple, char* cNTuple_result, const cDTDescriptor* pSd) { return NULL; };
	static inline int Equal(const char* tuple1, const char* tuple2, const cDTDescriptor *pSd) { return NULL; };
	static inline int Equal(const char* tuple1, const char* tuple2, uint length, const cDTDescriptor *pSd) { return NULL; };
	static inline unsigned char GetLength(const char* tuple, const cDTDescriptor* pSd = NULL) { return NULL; };
	static char* SetMask(const char* cBitString_Mask, const char* cNTuple_RI, const char* cNTuple_Key, char* cBitString_Result, const cDTDescriptor* pSd) { return NULL; };
	static char* SetMask(const char* cTuple_t1, const char* cTuple_t2, char* cBitString_Mask, const cDTDescriptor* pSd) { return NULL; };
	static inline char* SetMask(const char* cTuple_t1, const char* cBitString_Mask1, const char* cTuple_t2, const char* cBitString_Mask2, char* cBitString_Result, const cDTDescriptor* pSd) { return NULL; };

	//static unsigned int SameValues(char* cBitString_Mask, const char* cNTuple_prefix, const char* cNTuple_tuple, const cDTDescriptor* pSd, unsigned int sameValues) { return NULL; };
	static char* SetMinRefItem(const char* cNTuple_item, const char* cNTuple_refItem,  /*char* cBitString_Mask,*/ char* cNTuple_result, const cDTDescriptor* pSd) { return NULL; };
	//static char* CompleteMinRefItem(char* cBitString_Mask, const char* cNTuple_minItem, const char* cNTuple_key, char* cNTuple_partMinItem, char* cNTuple_result, const cDTDescriptor* pSd) { return NULL; };
	static void Print2File(FILE *StreamInfo, const char *data, const char* delim, const cDTDescriptor* pSd) {};
	static char* MergeMasks(char* cBitString_Mask1, char* cBitString_Mask2, const char* cTuple_minItem1, const char* cTuple_minItem2, char* cBitString_result, const cDTDescriptor* pSd) { return NULL; };
	//static bool Equal(char* cBitString_Mask1, const char* cTuple_t1, char* cBitString_Mask2, const char* cTuple_t2, const cDTDescriptor* pSd) { return false; };
	static bool IsCompatible(char* cBitString_Mask, const char* cTuple_RI, const char* cTuple_Key, const cDTDescriptor* pSd) { return false; };


	//My

	static inline T GetType(string type);
};

template <class T>
inline T cBasicType<T>::GetValue() const
{
	return mValue;
}

template <class T>
inline void cBasicType<T>::SetValue(const T& value)
{
	mValue = value;
}

/**
* \return In memory size of the data type
*/
template <class T>
inline unsigned int cBasicType<T>::GetSize(const char *data, const cDTDescriptor* pSd)
{
	UNUSED(data);
	UNUSED(pSd);
	return sizeof(T);
}

/**
* \return In memory size of the data type
*/
template <class T>
inline unsigned int cBasicType<T>::GetSize(uint itemSize) const
{
	UNUSED(itemSize);
	return sizeof(T);
}


template <class T>
inline unsigned int cBasicType<T>::GetMaxSize(const char *data, const cDTDescriptor* pSd)
{
	UNUSED(data);
	UNUSED(pSd);
	return sizeof(T);
}

template <class T>
inline unsigned int cBasicType<T>::GetLSize(uint dataLength, const cDTDescriptor* dtd)
{
	UNUSED(dtd);
	return SER_SIZE;
}

template <class T>
inline unsigned int cBasicType<T>::GetSize(const cDTDescriptor* pDtd) const
{
	UNUSED(pDtd);
	return SER_SIZE;
}

template <class T>
inline unsigned int cBasicType<T>::GetSize_instance(const char* data, const cDTDescriptor *pDtd) const
{
	UNUSED(data);
	UNUSED(pDtd);
	return SER_SIZE;
}

template <class T>
inline unsigned int cBasicType<T>::GetMaxSize(const cDTDescriptor* pSd) const
{
	UNUSED(pSd);
	return SER_SIZE;
}

template <class T>
inline const char* cBasicType<T>::GetCPValue() const
{
	return (const char*)&mValue;
}

template <class T>
inline void cBasicType<T>::Clear()
{
	mValue = (Type)0;
}

/// Copy one item into the second item
template <class T>
inline void cBasicType<T>::CopyFrom(const Type &from)
{
	mValue = from;  // mk: not tested
}

/// Copy one item into the second item
template <class T>
inline unsigned int cBasicType<T>::CopyTo(char *dst, const cDTDescriptor* sd) const
{
	*((T*)dst) = mValue;
	return sizeof(T);
}

/// Should copy only pointers in the memory inside of items (if there are any). Can lead to head corruption during the delete phase
template <class T>
inline void cBasicType<T>::CopyPointersFrom(const Type &from)
{
	mValue = from;   // mk: not tested
}

///// Copy whole array of items from one array to another. The blocks can not overlap!!
//template <class T>
//inline void cBasicType<T>::CopyBlock(Type* to, const Type* from, unsigned int count, const cSizeInfo<Type> &sizeInfo)
//{
//	memcpy((void*)to, (const void*)from, count * sizeInfo.GetSize());
//}
//
///// Move whole array of items from one array to another. The blocks can overlap.
//template <class T>
//inline void cBasicType<T>::MoveBlock(Type* to, const Type* from, unsigned int count, const cSizeInfo<Type> &sizeInfo)	
//{
//	memmove((void*)to, (const void*)from, count * sizeInfo.GetSize());
//}

template <class T>
inline void cBasicType<T>::Resize(const cDTDescriptor* sd)
{
}

template <class T>
inline bool cBasicType<T>::ResizeSet(T& value1, const T& value2, const cDTDescriptor* sd, cMemoryBlock* memBlock)
{
	value1 = value2;
	return true;
}

template <class T>
inline bool cBasicType<T>::ResizeSet(T* value1, const T& value2, const cDTDescriptor* sd, cMemoryBlock* memBlock)
{
	*value1 = value2;
	return true;
}

template <class T>
void cBasicType<T>::Free(T &tuple, cMemoryBlock *memBlock)
{
}

template <class T>
inline void cBasicType<T>::Resize(char *dst, const cDTDescriptor* sd)
{
	CopyTo(dst, sd);
}

template <class T>
inline bool cBasicType<T>::Equals(const Type &b)
{
	return (mValue == b);
}

template <class T>
inline int cBasicType<T>::Compare(const Type &b)
{
	return (b>mValue) ? -1 : ((mValue == b) ? 0 : 1);
}

template <class T>
inline bool cBasicType<T>::IsZero()
{
	return true;
}

template <class T>
inline int cBasicType<T>::Compare(const char* item2, const cDTDescriptor* sd) const
{
	int ret = 0;
	// T value1 = item1::Type;
	T value2 = *((T*)item2);
	if (mValue > value2)
	{
		ret = 1;
	}
	else if (mValue < value2)
	{
		ret = -1;
	}
	return ret;
}

/**
* Compare two values, returns: -1,0,1.
*/
template <class T>
inline int cBasicType<T>::Compare(const T& value1, const T& value2, const cDTDescriptor* pDtd)
{
	int ret = 0;
	if (value1 > value2)
	{
		ret = 1;
	}
	else if (value1 < value2)
	{
		ret = -1;
	}
	return ret;
}

template <class T>
inline bool cBasicType<T>::IsEqual(const T& value1, const T& value2, const cDTDescriptor* pDtd)
{
	return Compare(value1, value2, pDtd) == 0;
}

/**
* Compare two values.
*/
template <class T>
inline int cBasicType<T>::Compare(const char* item1, const char* item2, const cDTDescriptor* pDtd)
{
	int ret = 0;
	// T value1 = item1::Type;
	T value1 = *((T*)item1);
	T value2 = *((T*)item2);
	if (value1 > value2)
	{
		ret = 1;
	}
	else if (value1 < value2)
	{
		ret = -1;
	}
	return ret;
}

/**
* Compare two arrays of values, it returns: -1, 0, 1.
*/
template <class T>
inline int cBasicType<T>::CompareArray(const char* array1, const char* array2, uint length)
{
	return cComparator<T>::Compare((const T*)array1, (const T*)array2, length);
}

template <class T>
inline bool cBasicType<T>::Write(cStream *out) const
{
	return out->Write((char*)&mValue, SER_SIZE);
}

template <class T>
inline bool cBasicType<T>::Read(cStream *inp)
{
	return inp->Read((char*)&mValue, SER_SIZE);
}

template <class T>
inline char* cBasicType<T>::ToString(char*str) const
{
	sprintf(str, "%d", mValue);
	return str;
}

// Arithmetic operations. Usualy are not needed by the data structure. It should be explicitly stated in the description of the data structure
template <class T>
inline void cBasicType<T>::Sum(const Type& b)
{
	mValue = mValue + b;
}

template <class T>
inline void cBasicType<T>::Inc()
{
	mValue++;
}

template <class T>
inline unsigned int cBasicType<T>::HashValue(const char *value, uint hashTableSize, const cDTDescriptor* dtd)
{
	return (unsigned int)*((T*)value) % hashTableSize;
}

template <class T>
inline unsigned int cBasicType<T>::HashValue(const T& value, uint hashTableSize, const cDTDescriptor* dtd)
{
	return (unsigned int)(value) % hashTableSize;
}

template <class T>
inline unsigned int cBasicType<T>::HashValue(const char *array, uint length, uint hashTableSize)
{
	// return (unsigned int)(value) % hashTableSize;

	/* 
The hash function is of the form h(x, i) = (h1(x) + (i ? 1) • h2(x)) mod m for the ith trial, where
h1(x) = x mod m, h2(x) = 1 + (x mod m?), m is a prime number, and m? = m ? 1.
    */

	T* p = (T*)array;
	unsigned int hashValue = 0;
	uint m2 = hashTableSize - 1;

	for (unsigned int i = 0 ; i < length ; i++)
	{
		uint value = (uint)*p;
		uint h1 = value % hashTableSize;
		uint h2 = 1 + (value & m2);
		hashValue += h1 + i * h2;
		p++;
	}
	return hashValue % hashTableSize;
}

/// Warning: %d
template <class T>
inline void cBasicType<T>::Print(const T& value, const char *delim, const cDTDescriptor* sd)
{
	printf("%d%s", value, delim);
}

/// Warning: %d
template <class T>
inline void cBasicType<T>::Print(const char *data, const char *delim, const cDTDescriptor* sd)
{
	printf("%d%s", *((T*)data), delim);
}

/// Warning: %d
template <class T>
inline void cBasicType<T>::Print(const char *delim, const cDTDescriptor* sd) const
{
	printf("%d%s", mValue, delim);
}

/**
* Encode item into the memory (for deserialization or decompression purpose).
*
* \param item Input item.
* \param referenceItem Reference item, which can be used during the encoding (not in this type of class).
* \param memory Source memory where the item is encoded.
* \param mem_size Size of the memory.
* \return Number of bytes writen into the memory
*
* \warning This method is only usable for primitive data types (binary operations, data copying, ...).
*/
template <class T>
unsigned int cBasicType<T>::Encode(const T* item, T* referenceItem, char *memory, unsigned int mem_size)
{
	assert(mem_size >= SER_SIZE);

	*item = *((T&)memory);

	if (referenceItem != NULL)
	{
		*item += *referenceItem;
	}

	return SER_SIZE;
}

/**
* Decode item from the memory (for serialization or compression purpose).
*
* \param item Decoded item is stored here.
* \param referenceItem Reference item, which can be used during the decoding (not in this type of class).
* \param memory Source memory from which the item is decoded.
* \param mem_size Size of the memory.
* \return Number of bytes readed from memory.
*
* \warning This method is only usable for primitive data types (instace building, binary operations, data copying, ...).
*/
template <class T>
unsigned int cBasicType<T>::Decode(T* item, T* referenceItem, char *memory, unsigned int mem_size)
{
	assert(mem_size >= SER_SIZE);

	T value;

	if (referenceItem != NULL)
	{
		value = *item - *referenceItem;
	}
	else
	{
		value = *item;
	}

	*((T&)memory) = *item;

	return SER_SIZE;
}

template <class T>
inline char* cBasicType<T>::ToString(const T& item, char* str, unsigned int &size)
{
	sprintf(str, "%d", item);
	size = strlen(str);
	return str;
}

template <class T> inline T cBasicType<T>::GetValue(char* data, unsigned int order)
{
	return *((T*)data + order);
}

template <class T> inline T cBasicType<T>::GetValueC(const char* data, unsigned int order)
{
	return *((T*)data + order);
}

template <class T> inline void cBasicType<T>::SetValue(T& value1, const T& value2, const cDTDescriptor* pDtd)
{
	value1 = value2;
}

template <class T> inline void cBasicType<T>::SetValue(char* data, const T& value, unsigned int order)
{
	*((T*)data + order) = value;
}

template <class T> inline void cBasicType<T>::SetValueC(const char* data, const T& value, unsigned int order)
{
	*((T*)data + order) = value;
}

template <class T> inline cBasicType<T>::operator char*() const
{
	return (char*)&mValue;
}

template <class T>
inline uint cBasicType<T>::Encode(uint method, const char * sourceBuffer, char * encodedBuffer, const cDTDescriptor* dtd, uint bufferLength)
{
	return Coder::encode(method, SER_SIZE, sourceBuffer, encodedBuffer, bufferLength);
}

template <class T>
inline uint cBasicType<T>::Decode(uint method, char * encodedBuffer, char * decodedBuffer, const cDTDescriptor* dtd, uint bufferLength)
{
	return Coder::decode(method, SER_SIZE, encodedBuffer, decodedBuffer, bufferLength);
}

template <class T>
inline uint cBasicType<T>::GetEncodedSize(uint method, char* sourceBuffer, const cDTDescriptor* sd, uint bufferLength)
{
	return cNumber::BitsToBytes(Coder::GetSize(method, SER_SIZE, sourceBuffer, 1));
}

template <class T>
inline uint cBasicType<T>::EncodedSize(unsigned int method, char * sourceBuffer, const cDTDescriptor* dtd)
{
	unsigned int ret = Coder::estimateSizeInBits(method, SER_SIZE, sourceBuffer, 1);


	return ret;
}

template <class T>
inline uint cBasicType<T>::GetDimension(const cDTDescriptor* pSd)
{
	return 1;
}

template <class T>
inline void cBasicType<T>::AddToHistogram(cHistogram** histogram, const cDTDescriptor* dtd) const
{
	histogram[0]->AddValue(mValue);
}

template<class T>
inline T cBasicType<T>::GetType(string type)
{
	if (type.compare("INT") == 0)
	{
		return new cInt();
	}
	else if (type.compare("CHAR") == 0)
	{
		return new cChar();
	}
	else if (type.compare("FLOAT") == 0)
	{
		return new cFloat();
	}
	else if (type.compare("VARCHAR") == 0)
	{
		return new cNTuple();
	}

	else 
		return NULL;

}


class cWChar : public cBasicType<wchar_t>
{
public:
	static const wchar_t MAX = 255;
	static const wchar_t ZERO = 0;
	static const char CODE = 'w';

	inline virtual char GetCode()								{ return CODE; }
	inline static bool IsZero(const Type &a)					{ return a.GetValue() == ZERO; }
};

class cChar : public cBasicType<unsigned char>
{
public:
	static const unsigned char MAX = 255;
	static const unsigned char ZERO = 0;
	static const char CODE = 'c';

	inline virtual char GetCode()								{ return CODE; }
	inline static bool IsZero(const Type &a)					{ return a.GetValue() == ZERO; }
};




class cInt : public cBasicType<int>
{
public:
	static const int MAX = 2147483647;
	static const int ZERO = 0;
	static const char CODE = 'i';

	inline virtual char GetCode()								{ return CODE; }
	inline static bool IsZero(const Type &a)					{ return a.GetValue() == ZERO; }
};

class cUInt : public cBasicType<unsigned int>
{
public:
	static const unsigned int MAX = 4294967295;
	static const unsigned int ZERO = 0;
	static const char CODE = 'u';

	cUInt(unsigned int& value) :cBasicType((unsigned int)value) {}
	cUInt(const unsigned int& value) : cBasicType(value) {}
	cUInt() {}

	inline virtual char GetCode()								{ return CODE; }
	inline static bool IsZero(const Type &a)					{ return a.GetValue() == ZERO; }
	inline static void WriteAsText(const unsigned int &a, cStream *stream);

	operator unsigned int(void) const
	{
		return mValue;
	}

	/*
	inline void Resize(const cUInt &value, const cDTDescriptor* sd)
	{
	mValue = value.GetValue();
	}

	bool operator == (const cUInt &value) const
	{
	return mValue == value.GetValue();
	}*/
};

inline void cUInt::WriteAsText(const unsigned int &a, cStream *stream)
{
	const unsigned int BUFFER_SIZE = 20;
	char buffer[BUFFER_SIZE];
	unsigned int size;

	cUInt::ToString(a, buffer, size);
	stream->Write(buffer, size);
}

class cShort : public cBasicType<short>
{
public:
	static const short MAX = 32767;
	static const short ZERO = 0;
	static const char CODE = 's';

	cShort(short& value) :cBasicType((short)value) {}
	cShort(const short& value) : cBasicType(value) {}
	cShort() {}

	inline virtual char GetCode()								{ return CODE; }
	inline static bool IsZero(const Type &a)					{ return a.GetValue() == ZERO; }
};

class cUShort : public cBasicType<unsigned short>
{
public:
	static const unsigned short MAX = 65535;
	static const unsigned short ZERO = 0;
	static const char CODE = 'S';

	cUShort(unsigned short& value) :cBasicType((unsigned short)value) {}
	cUShort(const unsigned short& value) : cBasicType(value) {}
	cUShort() {}

	inline virtual char GetCode()								{ return CODE; }
	inline static bool IsZero(const Type &a)					{ return a.GetValue() == ZERO; }
};

class cLong : public cBasicType<long long>
{
public:
	static const long long MAX = 9223372036854775806;
	static const long long ZERO = 0;
	static const char CODE = 'l';

	inline virtual char GetCode()								{ return CODE; }
	inline static bool IsZero(const Type &a)					{ return a.GetValue() == ZERO; }
};

class cULong : public cBasicType<unsigned long long>
{
public:
	static const long long MAX = 18446744073709551615;
	static const long long ZERO = 0;
	static const char CODE = 'L';

	inline virtual char GetCode()								{ return CODE; }
	inline static bool IsZero(const Type &a)					{ return a.GetValue() == ZERO; }
};

class cFloat : public cBasicType<float>
{
public:
	static const float MAX;
	static const float ZERO;
	static const char CODE = 'f';

	inline virtual char GetCode()								{ return CODE; }
	inline static bool IsZero(const Type &a)					{ return a.GetValue() == 0; }
};

class cDouble : public cBasicType<double>
{
public:
	static const double MAX;
	static const double ZERO;
	static const char CODE = 'd';

	inline virtual char GetCode()								{ return CODE; }
	inline static bool IsZero(const Type &a)					{ return a.GetValue() == 0; }
};
#endif
