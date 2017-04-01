/**
 *	\file cHNTuple.h
 *	\author Michal Kratky, Filip Krizka
 *	\version 2.2
 *	\date dec 2011
 *	\brief Heterogeneous tuple for a tree data structure. It contains an array of items of the different types.
 */

#ifndef __cHNTuple_h__
#define __cHNTuple_h__

#include <limits.h>

#include "common/datatype/tuple/cCommonNTuple.h"
#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "common/stream/cStream.h"
#include "common/cString.h"

namespace common {
	namespace datatype {
		namespace tuple {

			class cHNTuple : public cNTuple
			{
			public:
				static const unsigned int MAX_DIM = 20;
				static const unsigned int LengthType = cDataType::LENGTH_VARLEN;

				char * GetData() const;
				static inline unsigned int GetSizePart(const char *data, unsigned int order, const cSpaceDescriptor* pSd);  //delka, kde zacinaji data order-ty itemu
				static inline char* GetPValue(const char *data, unsigned int order, const cSpaceDescriptor* pSd);  //adresa, kde zacinaji data order-ty itemu
				static inline unsigned int GetItemSize(const char *data, unsigned int order, const cSpaceDescriptor* pSd);     //velikost order-ty itemu
				static inline unsigned int GetItemSizeShift(const char *data, unsigned int order, const cSpaceDescriptor* pSd, unsigned int &byteShift);
				static inline unsigned int GetDimInnerNTuple(const char *data, unsigned int order, const cSpaceDescriptor* pSd); //jakou dimenzi ma vnitrni tuple

				inline unsigned int GetSize(const cDTDescriptor* pSd) const;
				static inline unsigned int GetSize(const char* data, const cDTDescriptor* pSd);
				static inline unsigned int GetSize(const char* data, const cSpaceDescriptor* pSd);
				static inline unsigned int GetMaxSize(const char *data, const cDTDescriptor* pSd);
				static inline unsigned int GetMaxSize(const char *data, const cSpaceDescriptor* pSd);

				static inline void Copy(char* cHNTuple_dst, const char* cHNTuple_src, const cSpaceDescriptor *pSd);
				static inline void Copy(char* cHNTuple_dst, const char* cHNTuple_src, const cDTDescriptor* pSd);

				// static inline unsigned int GetInMemSize(const cSpaceDescriptor* pSd);

				inline void SetValue(unsigned int order, float value, const cSpaceDescriptor* pSd);
				inline void SetValue(unsigned int order, double value, const cSpaceDescriptor* pSd);
				inline void SetValue(unsigned int order, int value, const cSpaceDescriptor* pSd);
				inline void SetValue(unsigned int order, unsigned int value, const cSpaceDescriptor* pSd);
				inline void SetValue(unsigned int order, unsigned short value, const cSpaceDescriptor* pSd);
				inline void SetValue(unsigned int order, short value, const cSpaceDescriptor* pSd);
				inline void SetValue(unsigned int order, uchar value, const cSpaceDescriptor* pSd);
				//inline void SetValue(unsigned int order, char* value, const cSpaceDescriptor* pSd);
				inline void SetValue(unsigned int order, const cNTuple& value, const cSpaceDescriptor* pSd);
				inline void SetValue(unsigned int order, const cLNTuple& value, const cSpaceDescriptor* pSd);

				inline void Clear(const cSpaceDescriptor* pSd);

				inline int Equal(const char* tuple2, const cSpaceDescriptor* pSd) const;
				static inline int Equal(const char* cUnfTuple_t1, const char* cUnfTuple_t2, const unsigned int order, const cSpaceDescriptor* pSd);
				static inline int Equal(const cHNTuple& tuple1,const char* tuple2,const cSpaceDescriptor* psd);//GRU0047
				static inline int Equal(const cHNTuple& tuple1, const char* tuple2, const cDTDescriptor* psd);//GRU0047
				static inline int Equal(const char * tuple1, const char* tuple2, const unsigned int order, const cDTDescriptor* psd);//GRU0047
				static inline int Equal(const char * tuple1, const char* tuple2,  const cDTDescriptor* psd);//gru0047

				inline unsigned int Copy(const char *srcData, const cSpaceDescriptor* pDtD);//gru0047	
				inline unsigned int Copy(const char *srcData, const cDTDescriptor* pDtD);//gru0047

	void Resize(const cSpaceDescriptor *pSd);

	static inline void SetTuple(char* data, const cHNTuple &tuple, const cSpaceDescriptor* pSd);
	

	inline unsigned int GetUInt(unsigned int order, const cSpaceDescriptor* pSd) const;
	inline int GetInt(unsigned int order, const cSpaceDescriptor* pSd) const;
	inline char* GetNTuple(unsigned int order, const cSpaceDescriptor* pSd);
	inline unsigned short GetUShort(unsigned int order, const cSpaceDescriptor* pSd) const;
	static inline unsigned short GetUShort(const char *data, unsigned int order, const cSpaceDescriptor* pSd);
	static inline char* GetNTuple(const char* data,unsigned int order, const cSpaceDescriptor* pSd);
	
	void Print(const char *string, const cSpaceDescriptor* pSd) const;
	void Print(unsigned int order, char *string, const cSpaceDescriptor* pSd) const;

	static void PrintPom(const char *data);

	//Staticke funkce
	static inline void SetValue(char *data, unsigned int order, float value, const cSpaceDescriptor* pSd);
	static inline void SetValue(char *data, unsigned int order, double value, const cSpaceDescriptor* pSd);
	static inline void SetValue(char *data, unsigned int order, int value, const cSpaceDescriptor* pSd);
	static inline void SetValue(char *data, unsigned int order, unsigned int value, const cSpaceDescriptor* pSd);
	static inline void SetValue(char *data, unsigned int order, unsigned short value, const cSpaceDescriptor* pSd);
	static inline void SetValue(char *data, unsigned int order, short value, const cSpaceDescriptor* pSd);
	static inline void SetValue(char *data, unsigned int order, uchar value, const cSpaceDescriptor* pSd);
	static inline void SetNTuple(char *data, unsigned int order, const char* value, const cSpaceDescriptor* pSd);

	static inline int GetInt(const char *data, unsigned int order, const cSpaceDescriptor* pSd);
	static inline unsigned int GetUInt(const char *data, unsigned int order, const cSpaceDescriptor* pSd);
	//static inline const cNTuple* GetNTuple(const char *data, unsigned int order, const cSpaceDescriptor* pSd);
	static void Print(const char *data, const char* delim, const cSpaceDescriptor* pSd);
	static void Print(const char *data, const char* delim, const cDTDescriptor* pSd);
	static float UnitIntervalLength(const char* cTuple_t1, const char* cTuple_t2, const unsigned int order, const cSpaceDescriptor* pSd);
};
}}}

namespace common {
	namespace datatype {
		namespace tuple {

void cHNTuple::SetValue(unsigned int order, float value, const cSpaceDescriptor* pSd)
{
	SetValue(mData, order, value, pSd);	
}

void cHNTuple::SetValue(unsigned int order, double value, const cSpaceDescriptor* pSd)
{
	SetValue(mData, order, value, pSd);	
}

void cHNTuple::SetValue(unsigned int order, int value, const cSpaceDescriptor* pSd)
{
	SetValue(mData, order, value, pSd);	
}

void cHNTuple::SetValue(unsigned int order, unsigned int value, const cSpaceDescriptor* pSd)
{
	SetValue(mData, order, value, pSd);	
}

void cHNTuple::SetValue(unsigned int order, unsigned short value, const cSpaceDescriptor* pSd)
{
	SetValue(mData, order, value, pSd);	
}

void cHNTuple::SetValue(unsigned int order, short value, const cSpaceDescriptor* pSd)
{
	SetValue(mData, order, value, pSd);	
}

void cHNTuple::SetValue(unsigned int order, uchar value, const cSpaceDescriptor* pSd)
{
	SetValue(mData, order, value, pSd);	
}

void cHNTuple::SetValue(unsigned int order, const cNTuple& value, const cSpaceDescriptor* pSd)
{
	char* mem = GetPValue(mData, order, pSd);
	cNTuple::SetValue(mem, value, pSd->GetDimSpaceDescriptor(order));
}

void cHNTuple::SetValue(unsigned int order, const cLNTuple& value, const cSpaceDescriptor* pSd)
{
	char* mem = GetPValue(mData, order, pSd);
	cLNTuple::SetValue(mem, value, pSd->GetDimSpaceDescriptor(order));
}

/*static*/ void cHNTuple::SetValue(char *data, unsigned int order, float value, const cSpaceDescriptor* pSd)
{
	*((float*)(GetPValue(data, order, pSd))) = value;
}

/*static*/ void cHNTuple::SetValue(char *data, unsigned int order, double value, const cSpaceDescriptor* pSd)
{
	*((double*)(GetPValue(data, order, pSd))) = value;
}

/*static*/ void cHNTuple::SetValue(char *data, unsigned int order, int value, const cSpaceDescriptor* pSd)
{
	*((int*)(GetPValue(data, order, pSd))) = value;
}

/*static*/ void cHNTuple::SetValue(char *data, unsigned int order, unsigned int value, const cSpaceDescriptor* pSd)
{
	char* mem = GetPValue(data, order, pSd);
	*((unsigned int*)(mem)) = value;
}

/*static*/ void cHNTuple::SetValue(char *data, unsigned int order, unsigned short value, const cSpaceDescriptor* pSd)
{
	*((unsigned short*)(GetPValue(data, order, pSd))) = value;
}

/*static*/ void cHNTuple::SetValue(char *data, unsigned int order, short value, const cSpaceDescriptor* pSd)
{
	*((short*)(GetPValue(data, order, pSd))) = value;
}

void cHNTuple::SetValue(char *data, unsigned int order, uchar value, const cSpaceDescriptor* pSd)
{
	*((uchar*)(GetPValue(data, order, pSd))) = value;
}

inline int cHNTuple::Equal(const char* tuple2, const cSpaceDescriptor* pSd) const
{
	// you must check dimension a typeSize
	return CompareLexicographically(tuple2, pSd);
}

/* This method is only for NTuple.
*/
/*static*/ void cHNTuple::SetNTuple(char *data, unsigned int order, const char* value, const cSpaceDescriptor* pSd)
{
	unsigned int delka_co = cNTuple::GetSize(GetPValue(value, order, pSd), pSd->GetDimSpaceDescriptor(order));
	unsigned int delka_kam = cNTuple::GetSize(GetPValue(data, order, pSd), pSd->GetDimSpaceDescriptor(order));
	unsigned int zacatek_posun = cHNTuple::GetSizePart(GetPValue(data, order, pSd), order, pSd);
	unsigned int max_delka = cHNTuple::GetMaxSize(GetPValue(data, order, pSd), pSd);
	int pocet_bajtu = max_delka-zacatek_posun-delka_co;

	memmove(data + (zacatek_posun + delka_co), data + (zacatek_posun + delka_kam), pocet_bajtu);
	memcpy(data + zacatek_posun, value + zacatek_posun, delka_co);
}

inline void cHNTuple::SetTuple(char* data, const cHNTuple &tuple, const cSpaceDescriptor* pSd)
{
	memcpy(data, tuple.GetData(), tuple.GetSize(tuple.GetData(), pSd));
}

/*static*/ inline int cHNTuple::Equal(const char* cUnfTuple_t1, const char* cUnfTuple_t2, const unsigned int order, const cSpaceDescriptor* pSd)
{
	char typeCode = pSd->GetDimensionTypeCode(order);
	int ret = 1;

	switch (typeCode)
	{
	case cNTuple::CODE:
		{
		char* a1 = GetPValue(cUnfTuple_t1, order, pSd);
		char* a2 = GetPValue(cUnfTuple_t2, order, pSd);
		//cNTuple::Print(a1, " < ", pSd->GetDimSpaceDescriptor(order));
		//cNTuple::Print(a2, "\n", pSd->GetDimSpaceDescriptor(order));
		ret = cNTuple::Equal(a1, a2, pSd->GetDimSpaceDescriptor(order));
		break;
		}
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

inline int cHNTuple::Equal(const cHNTuple& tuple1, const char* tuple2, const cSpaceDescriptor* psd)//GRU0047
{
	const char * tuple = tuple1.GetData();
	
	for (int i = 0; i < psd->GetDimension(); i++)
	{
		int ret=Equal(tuple, tuple2, i, psd);
		if (ret == -1 || ret == 1)
		{
			return ret;
			break;
		}
	}
}

inline int cHNTuple::Equal(const cHNTuple & tuple1, const char * tuple2, const cDTDescriptor * psd)//gru0047
{
	const char * tuple = tuple1.GetData();


	for (int i = 0; i < psd->GetDimension(); i++)
	{
		int ret = Equal(tuple, tuple2, i, (cSpaceDescriptor*)psd);
		if (ret == -1 || ret == 1)
		{
			return ret;
			break;
		}
	}
}

inline int cHNTuple::Equal(const char * tuple1, const char * tuple2, const unsigned int order, const cDTDescriptor * psd)//gru0047
{

		return Equal(tuple1, tuple2, order, (cSpaceDescriptor*)psd);
	
}

inline int cHNTuple::Equal(const char * tuple1, const char * tuple2, const cDTDescriptor * psd)//gru0047
{
	for (int i = 0; i < psd->GetDimension(); i++)
	{
		int ret = Equal(tuple1, tuple2, i, (cSpaceDescriptor*)psd);
		if (ret == -1 || ret == 1)
		{
			return ret;
			break;
		}
	}
}

inline unsigned int cHNTuple::Copy(const char * srcData, const cSpaceDescriptor * pDtD)
{
	uint size = pDtD->GetSize();
	memcpy(mData, srcData, size);
	return size;
}

inline unsigned int cHNTuple::Copy(const char * srcData, const cDTDescriptor * pDtD)
{
	return Copy(srcData, (const cSpaceDescriptor*)pDtD);
}



inline unsigned int cHNTuple::GetUInt(unsigned int order, const cSpaceDescriptor* pSd) const
{
	return GetUInt(mData, order, pSd);
}

inline int cHNTuple::GetInt(unsigned int order, const cSpaceDescriptor* pSd) const
{
	return GetInt(mData, order, pSd);
}


/*static*/ inline unsigned int cHNTuple::GetUInt(const char *data, unsigned int order, const cSpaceDescriptor* pSd)
{
	unsigned int ret = *((unsigned int*)(GetPValue(data, order, pSd)));
    return ret;
}

/*static*/ inline int cHNTuple::GetInt(const char *data, unsigned int order, const cSpaceDescriptor* pSd)
{
    return *((int*)(GetPValue(data, order, pSd)));
}

/*static*/ inline char* cHNTuple::GetNTuple(unsigned int order, const cSpaceDescriptor* pSd)
{
	return GetPValue(mData, order, pSd); 
}

inline unsigned short cHNTuple::GetUShort(unsigned int order, const cSpaceDescriptor* pSd) const
{
    return GetInt(mData, order, pSd);
}

/*static*/ inline unsigned short cHNTuple::GetUShort(const char *data, unsigned int order, const cSpaceDescriptor* pSd)
{
    unsigned short ret = *((unsigned int*)(GetPValue(data, order, pSd)));
    return ret;
}

inline char* cHNTuple::GetNTuple(const char* data,unsigned int order, const cSpaceDescriptor* pSd)
{
    return GetPValue(data, order, pSd); 
}

/**/
inline unsigned int cHNTuple::GetSize(const cDTDescriptor* pDtd) const
{
	return cHNTuple::GetSize(mData, (const cSpaceDescriptor*)pDtd);
}

inline unsigned int cHNTuple::GetSize(const char* data, const cDTDescriptor* pDtd)
{
	return cHNTuple::GetSize(data, (const cSpaceDescriptor*)pDtd);
}

inline unsigned int cHNTuple::GetSize(const char* data, const cSpaceDescriptor* pSd)
{
	unsigned int dimension = pSd->GetDimension();
	unsigned int size = SIZEPREFIX_LEN;

	for (unsigned int i = 0 ; i < dimension ; i++)
	{
		cHNTuple::GetItemSizeShift(data, i, pSd, size);
		//unsigned int tmpSize = cHNTuple::GetItemSizeShift(data, i, pSd, size);
		//size +=  tmpSize;
	}
	return size; // + SIZEPREFIX_LEN;
}

inline char* cHNTuple::GetPValue(const char *data, unsigned int order, const cSpaceDescriptor* pSd)
{
	return (char*)data + GetSizePart(data, order, pSd);
}

inline unsigned int cHNTuple::GetSizePart(const char *data, unsigned int order, const cSpaceDescriptor* pSd)
{
	unsigned int previousItemsSize = SIZEPREFIX_LEN;

	for (unsigned int i = 0 ; i < order ; i++)
	{
		GetItemSizeShift(data, i, pSd, previousItemsSize);
	}
	return  previousItemsSize;
}

/**
 * Return MAX_UINT in the case of an error.
 */ 
inline unsigned int cHNTuple::GetItemSizeShift(const char *data, unsigned int order, const cSpaceDescriptor* pSd, unsigned int &byteShift)
{
	unsigned int len = UINT_MAX;
	char typeCode = pSd->GetDimensionTypeCode(order);

	if (typeCode == cWChar::CODE)
	{
		len = cWChar::SER_SIZE;
	}
	else if (typeCode == cUInt::CODE)
	{
		len = cUInt::SER_SIZE;
	}
	else if (typeCode == cInt::CODE)
	{
		len = cInt::SER_SIZE;
	}
	else if (typeCode == cChar::CODE)
	{
		len = cChar::SER_SIZE;
	}
	else if (typeCode == cShort::CODE)
	{
		len = cShort::SER_SIZE;
	}
	else if (typeCode == cUShort::CODE)
	{
		len = cUShort::SER_SIZE;
	}
	else if (typeCode == cNTuple::CODE || typeCode == cLNTuple::CODE)
	{
		char* mem = (char*)data + byteShift;
		if (typeCode == cNTuple::CODE)
		{
			len = cNTuple::GetSize(mem, pSd->GetDimSpaceDescriptor(order));
		}
		else 
		{
			len = cLNTuple::GetSize(mem, pSd->GetDimSpaceDescriptor(order));
		}
	}
	else
	{
		printf("Critical Error: cHNTuple::GetItemSizeShift(): the data type is not supported!");
		// mk!! len = pSd->GetType(order)->Type::SER_SIZE;
	}
	byteShift += len;
	return len;
}

/*
 * Return MAX_UINT in the case of an error.
 */ 
inline unsigned int cHNTuple::GetItemSize(const char *data, unsigned int order, const cSpaceDescriptor* pSd)
{
	uint itemSize = UINT_MAX;
	
	switch (pSd->GetDimensionTypeCode(order))
	{
	case cUInt::CODE:
		itemSize = sizeof(unsigned int);
		break;
	case cNTuple::CODE:
		char* mem = GetPValue(data, order, pSd);
		itemSize = cNTuple::GetSize(mem, pSd->GetDimSpaceDescriptor(order));
		break;
	}
	return itemSize;
}

unsigned int cHNTuple::GetDimInnerNTuple(const char *data, unsigned int order, const cSpaceDescriptor* pSd)
{
	//return   (unsigned int)(*data);  //1byte
	// *((unsigned int*)(data));       //4byte

	unsigned int previousItemsSize = 0;
	for (unsigned int i = 0 ; i < order ; i++)
	{
		previousItemsSize += GetItemSize(data, i, pSd);
	}
	return   (unsigned int)(*(data + SIZEPREFIX_LEN + previousItemsSize));
}

/**
 * It returns the maximal size of the tuple related to the space descriptor.
 */
inline unsigned int cHNTuple::GetMaxSize(const char *data, const cSpaceDescriptor* pSd)
{
	//printf("cHNTuple::GetInMemSize - should not be called\n");
	return pSd->GetSize() + SIZEPREFIX_LEN;

	/*unsigned int dimension = pSd->GetDimension();
	unsigned int delka = 0;

	for (unsigned int order=0; order<dimension; order++)
	{
		char typeCode = pSd->GetTypeCode(order);

		if (typeCode == cChar::CODE)
		{
			delka += pSd->GetByteSize(order);
		} 
		else  if (typeCode == cInt::CODE || typeCode == cUInt::CODE)
		{
			delka += pSd->GetByteSize(order);
		}
		else if (typeCode == cNTuple::CODE)
		{
			delka += cNTuple::GetMaxSize(NULL, pSd->GetDimSpaceDescriptor(order));
		}
		else if (typeCode == cWChar::CODE)
		{
			delka += pSd->GetByteSize(order);
		}
	}
	delka += SIZEPREFIX_LEN;
	unsigned int delka2 = pSd->GetByteSize() + SIZEPREFIX_LEN;

	if (delka != delka2)
	{
		printf("Critical Error: delka != delka2!");
	}

	return delka;*/
}

inline unsigned int cHNTuple::GetMaxSize(const char *data, const cDTDescriptor* pSd)
{
	return cHNTuple::GetMaxSize(data, (const cSpaceDescriptor*)pSd);
}

inline void cHNTuple::Copy(char* cHNTuple_dst, const char* cHNTuple_src, const cSpaceDescriptor *pSd)
{
	memcpy(cHNTuple_dst, cHNTuple_src, GetSize(cHNTuple_src, pSd));
}

inline void cHNTuple::Copy(char* cHNTuple_dst, const char* cHNTuple_src, const cDTDescriptor* pSd)
{
       Copy(cHNTuple_dst, cHNTuple_src, (const cSpaceDescriptor*)pSd);
}

/**
* Set all bits of the tuple values to zero (except the )
*/
void cHNTuple::Clear(const cSpaceDescriptor* pSd)
{
	memset(mData + SIZEPREFIX_LEN, 0, GetSize(mData, pSd));
}

}}}
#endif