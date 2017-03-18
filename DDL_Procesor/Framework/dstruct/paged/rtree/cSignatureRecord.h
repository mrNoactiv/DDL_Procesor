/**************************************************************************}
{                                                                          }
{    cSignatureRecord.cpp                                                  }
{                                                                          }
{                                                                          }
{    Copyright (c) 2001                      Michal Kratky                 }
{                                                                          }
{    VERSION: 0.2                            DATE 01/08/2003               }
{                                                                          }
{    following functionality:                                              }
{       multidimensional signature                                         }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      xx/xx/xxxx                                                          }
{                                                                          }
{**************************************************************************/

#ifndef __cSignatureRecord_h__
#define __cSignatureRecord_h__

#include "common/cSignature.h"
#include "common/cBitString.h"
#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "common/datatype/tuple/cCommonNTuple.h"
#include "dstruct/paged/rtree/cSignatureController.h"
#include "dstruct/paged/queryprocessing/cQueryProcStat.h"

using namespace common::datatype::tuple;

namespace dstruct {
	namespace paged {
		namespace rtree {

struct sMapItem {
	uint realValue;
	uint mapValue;
};


class cSignatureRecord
{
private:

	cLNTuple *mNodeSignature;

	static uint GetMapValue(uint realValue, sMapItem* pMapTable, uint* pMapTableCounter, bool pCanAdd);
	static void Combine(uint i, ullong value, const char* ql, const char* qh, cSignatureParams* pSignatureParams, cArray<ullong> *pQueryTrueBitOrders, cArray<uint> *pNarrowDims, sMapItem** pMapTable, uint* pMapTableCounter, uint* domains, bool DDS);

public:
	cSignatureRecord(char* mem, const cSpaceDescriptor* pSD);

	inline char* GetData() const;
	inline void SetData(char* data);
	inline cLNTuple* GetNodeSignature() const;
	inline uint CopyTo(char* data, const cDTDescriptor* pDTD) const;
	inline uint CopyFrom(char* data, const cDTDescriptor* pDTD);
	inline uint GetSize(const cDTDescriptor* pDTD) const;
	static inline uint GetSize(const char* item, const cDTDescriptor* pDTD);
	inline uint GetMaxSize(const cDTDescriptor* pDTD) const;

	static inline uint GetObjectSize(const cSpaceDescriptor* pSD);

	static inline char* GetSignature(char* pNodeSignature, uint pDimension, cSpaceDescriptor* pSD);
	static inline char* GetSignature(char* pNodeSignature, cSpaceDescriptor* pSD);
	static inline void ClearTuple(char* pNodeSignature, uint pDimension, cSpaceDescriptor* pSD);
	static inline void ClearTuple(char* pNodeSignature, cSpaceDescriptor* pSD);

	static inline void ResetTuple(char* pNodeSignature, cSpaceDescriptor* pSD);

	static inline int ComputeSignatureWeight(char* pNodeSignature, uint pDimension, cSpaceDescriptor* pSD);
	static inline int ComputeSignatureWeight(char* pNodeSignature, cSpaceDescriptor* pSD);

	static uint GenerateQuerySignature_DIS(const char* item, cSignatureParams* pSignatureParams, cArray<ullong> *pQueryTrueBitOrders, cArray<uint> *pNarrowDims);
	static uint GenerateQuerySignature_DDS(const char* ql, const char* qh, cSignatureParams* pSignatureParams, cArray<ullong> *pQueryTrueBitOrders, cArray<uint> *pNarrowDims, sMapItem** pMapTable, uint* pMapTableCounter, uint* domains);
	static uint GenerateQuerySignature_DDO(const char* ql, const char* qh, cSignatureParams* pSignatureParams, cArray<ullong> *pQueryTrueBitOrders, cArray<uint> *pNarrowDims, sMapItem** pMapTable, uint* pMapTableCounter, uint* domains);

	static ullong ComputeTupleValue(const char* item, bool* queryType, uint dimension, sMapItem** pMapTable, uint* pMapTableCounter, uint* domains);

	static inline void SetTrueBit(char* pNodeSignature, uint pDimension, cSpaceDescriptor* pSD, uint pTrueBitOrder);
	static inline void SetTrueBit(char* pNodeSignature, cSpaceDescriptor* pSD, uint pTrueBitOrder);
	static inline bool IsMatched(char* pNodeSignature, uint pDimension, cSpaceDescriptor* pSD, uint pTrueBitOrder);
	static inline bool IsMatched(char* pNodeSignature, cSpaceDescriptor* pSD, uint pTrueBitOrder);

	static inline void AddValue(char* pNodeSignature, cSpaceDescriptor* pSD, ullong pTrueBitOrder);
	static inline bool ValueExists(char* pNodeSignature, cSpaceDescriptor* pSD, ullong pTrueBitOrder, cQueryProcStat *queryProcStat);
	static inline uint GetItemsCount(char* pNodeSignature, cSpaceDescriptor* pSD);
};


inline uint cSignatureRecord::GetMaxSize(const cDTDescriptor* pDTD) const
{
	return cLNTuple::GetMaxSize(mNodeSignature->GetData(), pDTD);
}

inline uint cSignatureRecord::GetSize(const cDTDescriptor* pDTD) const
{
	return cLNTuple::GetSize(mNodeSignature->GetData(), pDTD);
}

inline uint cSignatureRecord::GetSize(const char* item, const cDTDescriptor* pDTD)
{
	return cLNTuple::GetSize(item, pDTD);
}

inline cLNTuple* cSignatureRecord::GetNodeSignature() const
{
	return mNodeSignature;
}

inline char* cSignatureRecord::GetData() const
{
	return mNodeSignature->GetData();
}

inline void cSignatureRecord::SetData(char* data)
{
	mNodeSignature->SetData(data);
}

inline uint cSignatureRecord::GetObjectSize(const cSpaceDescriptor* pSD)
{
	return sizeof(cSignatureRecord) + cLNTuple::GetObjectSize(pSD);
}

inline uint cSignatureRecord::CopyTo(char* data, const cDTDescriptor* pDTD) const
{
	uint byteSize = cLNTuple::GetSize(mNodeSignature->GetData(), pDTD);
	memcpy(data, mNodeSignature->GetData(), byteSize);
	return byteSize;
}

inline uint cSignatureRecord::CopyFrom(char* data, const cDTDescriptor* pDTD) 
{
	uint byteSize = cLNTuple::GetSize(data, pDTD);
	memcpy(mNodeSignature->GetData(), data, byteSize);
	return byteSize;
}


inline void cSignatureRecord::ClearTuple(char* pNodeSignature, uint pDimension, cSpaceDescriptor* pSD)
{
	char* sigTuple = cLNTuple::GetTuple(pNodeSignature, pDimension, pSD);
	cLNTuple::Clear(sigTuple, pSD->GetDimSpaceDescriptor(pDimension));
}

inline void cSignatureRecord::ClearTuple(char* pNodeSignature, cSpaceDescriptor* pSD)
{
	cLNTuple::Clear(pNodeSignature, pSD);
}

inline void cSignatureRecord::ResetTuple(char* pNodeSignature, cSpaceDescriptor* pSD)
{
	cLNTuple::SetLength(pNodeSignature, (uint)0);
}


inline char* cSignatureRecord::GetSignature(char* pNodeSignature, uint pDimension, cSpaceDescriptor* pSD)
{
	char* sigTuple = cLNTuple::GetTuple(pNodeSignature, pDimension, pSD);
	return cLNTuple::GetPChar(sigTuple, 0, pSD->GetDimSpaceDescriptor(pDimension));
}

inline char* cSignatureRecord::GetSignature(char* pNodeSignature, cSpaceDescriptor* pSD)
{
	return cLNTuple::GetPChar(pNodeSignature, 0, pSD);
}

inline void cSignatureRecord::SetTrueBit(char* pNodeSignature, uint pDimension, cSpaceDescriptor* pSD, uint pTrueBitOrder)
{
	char* sigChunk = GetSignature(pNodeSignature, pDimension, pSD);
	cBitArray::SetBit(sigChunk, pTrueBitOrder, true);
}

inline void cSignatureRecord::SetTrueBit(char* pNodeSignature, cSpaceDescriptor* pSD, uint pTrueBitOrder)
{
	char* sigChunk = GetSignature(pNodeSignature, pSD);
	cBitArray::SetBit(sigChunk, pTrueBitOrder, true);
}

inline bool cSignatureRecord::IsMatched(char* pNodeSignature, uint pDimension, cSpaceDescriptor* pSD, uint pTrueBitOrder)
{
	char* sigChunk = GetSignature(pNodeSignature, pDimension, pSD);
	return cBitArray::GetBit(sigChunk, pTrueBitOrder);
}

inline bool cSignatureRecord::IsMatched(char* pNodeSignature, cSpaceDescriptor* pSD, uint pTrueBitOrder)
{
	char* sigChunk = GetSignature(pNodeSignature, pSD);
	return cBitArray::GetBit(sigChunk, pTrueBitOrder);
}

inline void cSignatureRecord::AddValue(char* pNodeSignature, cSpaceDescriptor* pSD, ullong pTrueBitOrder)
{
	uint length = cLNTuple::GetLength(pNodeSignature, pSD);
	cLNTuple::SetValue(pNodeSignature, length, pTrueBitOrder, pSD);
	cLNTuple::SetLength(pNodeSignature, length + 1);
}

inline bool cSignatureRecord::ValueExists(char* pNodeSignature, cSpaceDescriptor* pSD, ullong pTrueBitOrder, cQueryProcStat *queryProcStat)
{
	uint length = cLNTuple::GetLength(pNodeSignature, pSD);

	for (uint k = 0; k < length; k++)
	{
		queryProcStat->IncComputCompareQuery();
		if (pTrueBitOrder == cLNTuple::GetULong(pNodeSignature, k, pSD))
		{
			return true;
		}
	}
	return false;
}

inline uint cSignatureRecord::GetItemsCount(char* pNodeSignature, cSpaceDescriptor* pSD)
{
	return cLNTuple::GetLength(pNodeSignature, pSD);
}

inline int cSignatureRecord::ComputeSignatureWeight(char* pNodeSignature, uint pDimension, cSpaceDescriptor* pSD)
{
	uint byteSize = pSD->GetDimSpaceDescriptor(pDimension)->GetSize();
	return cBitString::Weight(GetSignature(pNodeSignature, pDimension, pSD), byteSize * cNumber::BYTE_LENGTH);
}

inline int cSignatureRecord::ComputeSignatureWeight(char* pNodeSignature, cSpaceDescriptor* pSD)
{
	uint byteSize = pSD->GetSize();
	return cBitString::Weight(GetSignature(pNodeSignature, pSD), byteSize * cNumber::BYTE_LENGTH);
}

}}}
#endif
