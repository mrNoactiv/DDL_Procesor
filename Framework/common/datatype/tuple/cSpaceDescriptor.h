/**
 *	\file cSpaceDescriptor.h
 *	\author Michal Kratky
 *	\version 0.1
 *	\date jun 2006
 *	\brief Space descriptor for the cTuple
 */

#ifndef __cSpaceDescriptor_h__
#define __cSpaceDescriptor_h__

namespace common {
	namespace datatype {
		namespace tuple {
			class cTuple;
		}
	}
}

#include "common/cCommon.h"
#include "common/datatype/cDataType.h"
#include "common/datatype/cBasicType.h"
#include "common/datatype/cDTDescriptor.h"

using namespace common;
using namespace common::datatype;

#ifndef bitmask_t_
#define bitmask_t_
#define bitmask_t 	unsigned long long
#endif

#define adjust_rotation(rotation,nDims,bits)				\
do {									\
      /* rotation = (rotation + 1 + ffs(bits)) % nDims; */		\
      bits &= -bits & ((1 << (nDims-1)) - 1);				\
      while (bits)							\
	bits >>= 1, ++rotation;						\
      if ( ++rotation >= nDims )					\
	rotation -= nDims;						\
} while (0)

#define rotateRight(arg, nRots, nDims)					\
((((arg) >> (nRots)) | ((arg) << ((nDims)-(nRots)))) & ((1 << (nDims)) - 1))

#define ones(T,k) ((((T)2) << (k-1)) - 1)
#define rdbit(w,k) (((w) >> (k)) & 1)
#define whichByte(nBytes,y) (y/8)
#define setBytes(dst,pos,nBytes,val) \
     memset(&dst[0],val,pos)

/**
*	Represents the space descriptor for the cTuple. Store all metadata information about concrete cTuple
*
*	\author Michal Kratky
*	\version 0.1
*	\date jun 2006
**/
namespace common {
	namespace datatype {
		namespace tuple {

class cSpaceDescriptor: public cDTDescriptor
{
	typedef cDTDescriptor parent;

	typedef bitmask_t (*BitReader) (unsigned nDims, unsigned nBytes,
					char const* c, unsigned y);
	typedef void (*BitWriter) (unsigned d, unsigned nBytes,
		   char* c, unsigned y, int fold);

	cDataType* mType;
	char mTypeCode;

	unsigned int *mBitIndexes;
	unsigned int *mByteIndexes;
	unsigned int *mSizes;
	bool mHomogenousSd;   // true means all dimensions share the same data types (and the same inner space descriptor)
	cDataType **mDimensionTypes;
	char* mDimensionTypeCodes;
	bool mIsAnyDimDescriptor;
	cSpaceDescriptor **mDimensionSd;

	// VAL644	
	unsigned int mByteSize;
	// VAL644

	uint mSize;			// the size of the space descriptor, e.g. dimension x 8 (in the case of 2d int cTuple)
	uint mTypeSize;     // int the case of cTuple mByteSize = mMaxTypeByteSize, in the case of cNTuple sizeof(TLength) is added

	bool mDebug;

	cTuple *mMaxValue;
	cTuple *mMinValue;
	unsigned int *mNormalizedValues1;
	unsigned int *mNormalizedValues2;
	bool mComputeOrderValues;
	double *mPrecomputedUnitSize;
	static const unsigned int Z_VALUE_SIZE = 4294967295;

protected:
	void Null();
	void Delete();

	static bitmask_t getIntBits(unsigned nDims, unsigned nBytes, char const* c, unsigned y);
	int hilbert_cmp_work(unsigned nDims, unsigned nBytes, unsigned nBits, unsigned max, unsigned y,	 char const* c1, char const* c2,
		 unsigned rotation,	 bitmask_t bits, bitmask_t index, BitReader getBits);

public:
	//static const unsigned int Z_VALUE_SIZE = 4294967295;

	cSpaceDescriptor(bool computeOrderValues = false);
	cSpaceDescriptor(unsigned int dimension, cDataType *type, bool computeOrderValues = false);
	cSpaceDescriptor(const cSpaceDescriptor &treeSpaceDescriptor, bool computeOrderValues = false);
	cSpaceDescriptor(unsigned int dimension);
	cSpaceDescriptor(unsigned int dimension, cDataType *type, cDataType *dimensionType);
	cSpaceDescriptor(unsigned int dimension, cDataType *type, cDataType *dimensionType, bool computeOrderValues);
	cSpaceDescriptor(unsigned int dimension, cDataType * type, cDataType ** dimensionType, bool computeOrderValues);
	//cSpaceDescriptor(unsigned int dimension, cDataType *type, cDataType *dimensionType1, cDataType *dimensionType2, bool computeOrderValues);
	~cSpaceDescriptor();
	
	void Create(unsigned int dimension, cDataType *type); // VAL644
	void Create(unsigned int dimension, cDataType *type, cDataType *dimensionType);
	void Create(unsigned int dimension, cDataType * type, cDataType ** dimensionType);
	//void Create(unsigned int dimension, cDataType * type, cDataType * dimensionType1, cDataType * dimensionType2);
	void Create(const cSpaceDescriptor &treeSpaceDescriptor);

	void SetDimensionType(unsigned int dim, cDataType *type);
	void SetDimensionType(cDataType *type); // VAL644

	inline cDataType* GetDimensionType(unsigned int dim) const;
	inline char GetDimensionTypeCode(unsigned int dim) const;

	inline unsigned int GetBitIndex(unsigned int order) const;
	inline unsigned int GetDimensionOrder(unsigned int order) const;
	inline unsigned int GetDimensionSize(unsigned int order) const;

	inline unsigned int GetSize() const;
	inline unsigned int GetTypeSize() const;
	unsigned int GetLSize(unsigned int tupleLength) const;

	unsigned int GetSerialSize();
	inline bool IsSpaceHomogenous() const;

	void Setup();
	cDataType* CreateDataType(char code);

	inline cSpaceDescriptor* GetDimSpaceDescriptor(unsigned int order) const;
	void SetDimSpaceDescriptor(unsigned int order, cSpaceDescriptor* sd);
	inline bool IsAnyDimDescriptor() const;

	bool Read(cStream *stream);
	bool Write(cStream *stream) const;

	int CompareNormalizedZValues(const char* tuple1, const char* tuple2, cSpaceDescriptor* pSd);
	int CompareNormalizedHValues(const char* tuple1, const char* tuple2, cSpaceDescriptor* pSd);
	void ComputeNormalizedValues(const char* tuple1, const char* tuple2, const cSpaceDescriptor* pSd);
	void ComputeNormalizedUnitSize();
};

inline unsigned int cSpaceDescriptor::GetSize() const
{
	return mSize;
}

inline uint cSpaceDescriptor::GetTypeSize() const
{
	return mTypeSize;
}

inline unsigned int cSpaceDescriptor::GetDimensionSize(unsigned int order) const
{
	assert(order <= parent::mDimension);
	return mSizes[order];
}

inline unsigned int cSpaceDescriptor::GetBitIndex(unsigned int order) const
{
	assert(order < parent::mDimension);
	return mBitIndexes[order];
}

inline unsigned int cSpaceDescriptor::GetDimensionOrder(unsigned int order) const
{
	assert(order < parent::mDimension);
	return mByteIndexes[order];
}

/// \return Type of the dimension dim
inline cDataType *cSpaceDescriptor::GetDimensionType(unsigned int dim) const
{
	return mDimensionTypes[dim];
}

/// \return Type of the dimension dim
inline char cSpaceDescriptor::GetDimensionTypeCode(unsigned int dim) const
{
	return mDimensionTypeCodes[dim];
}

inline cSpaceDescriptor* cSpaceDescriptor::GetDimSpaceDescriptor(unsigned int order) const
{
	return mDimensionSd[order];
}

inline bool cSpaceDescriptor::IsAnyDimDescriptor() const
{
	return mIsAnyDimDescriptor;
}

inline bool cSpaceDescriptor::IsSpaceHomogenous() const
{
	return mHomogenousSd;
}
}}}
#endif