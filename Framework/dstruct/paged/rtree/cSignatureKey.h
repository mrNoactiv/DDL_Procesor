/**************************************************************************}
{                                                                          }
{    cSignatureKey.cpp                                                     }
{                                                                          }
{                                                                          }
{    Copyright (c) 2001                      Peter Chovanec                }
{                                                                          }
{    VERSION: 0.1                            DATE 23/01/2015               }
{                                                                          }
{    following functionality:                                              }
{       item of conversion table between R-tree and Signature Array        }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      xx/xx/xxxx                                                          }
{                                                                          }
{**************************************************************************/

#ifndef __cSignatureKey_h__
#define __cSignatureKey_h__

#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "common/datatype/tuple/cTuple.h"
#include "dstruct/paged/rtree/cSignatureController.h"

using namespace common::datatype::tuple;

namespace dstruct {
	namespace paged {
		namespace rtree {

class cSignatureKey
{
private:
	cTuple* mKey;
	cTuple* mData;

public:
	cSignatureKey(char* mem, cSpaceDescriptor* pKeySD);

	static inline uint GetObjectSize(const cSpaceDescriptor* pKeySD, const cSpaceDescriptor* pDataSD);
	inline cTuple* GetKey() const;
	inline cTuple* GetData() const;

	inline void SetKey(uint nodeIndex, const cSpaceDescriptor* pKeySD);
	inline void SetKey(uint nodeIndex, uint dimension, const cSpaceDescriptor* pKeySD);
	inline void SetKey(uint nodeIndex, uint dimension, uint chunkOrder, const cSpaceDescriptor* pKeySD);

	inline void SetData(uint signatureIndex, uint position, const cSpaceDescriptor* pDataSD);
	inline uint GetSignatureIndex();
	inline uint GetPosition();

};


inline cTuple* cSignatureKey::GetKey() const
{
	return mKey;
}


inline cTuple* cSignatureKey::GetData() const
{
	return mData;
}


inline uint cSignatureKey::GetObjectSize(const cSpaceDescriptor* pKeySD, const cSpaceDescriptor* pDataSD)
{
	return sizeof(cSignatureKey) + cTuple::GetObjectSize(pKeySD) + cTuple::GetObjectSize(pDataSD);
}

inline void cSignatureKey::SetKey(uint nodeIndex, const cSpaceDescriptor* pKeySD)
{
	mKey->SetValue(0, nodeIndex, pKeySD);
}

inline void cSignatureKey::SetKey(uint nodeIndex, uint dimension, const cSpaceDescriptor* pKeySD)
{
	mKey->SetValue(0, nodeIndex, pKeySD);
	mKey->SetValue(1, dimension, pKeySD);
}


inline void cSignatureKey::SetKey(uint nodeIndex, uint dimension, uint chunkOrder, const cSpaceDescriptor* pKeySD)
{
	mKey->SetValue(0, nodeIndex, pKeySD);
	mKey->SetValue(1, dimension, pKeySD);
	mKey->SetValue(2, chunkOrder, pKeySD);
}

inline void cSignatureKey::SetData(uint signatureIndex, uint position, const cSpaceDescriptor* pDataSD)
{
	mData->SetValue(0, signatureIndex, pDataSD);
	mData->SetValue(1, position, pDataSD);
}

inline uint cSignatureKey::GetSignatureIndex()
{
	//return ((uint*) mData->GetData())[0];
	uint* tmpData = (uint*) mData->GetData();
	return tmpData[0];
}

inline uint cSignatureKey::GetPosition()
{
	//return ((uint*) mData->GetData())[1];
	uint* tmpData = (uint*) mData->GetData();
	return tmpData[1];
}

}}}
#endif
