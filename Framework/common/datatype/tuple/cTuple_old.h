/**
 *	\file cTuple.h
 *	\author Michal Kratky, Radim Baca
 *	\version 0.1
 *	\date jun 2006
 *	\brief Tuple for a tree data structure
 */

#ifndef __cTuple_h__
#define __cTuple_h__

namespace common {
	namespace datatype {
		namespace tuple {
  class cTuple;
}}}

#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "common/stream/cStream.h"
#include "common/datatype/cBasicType.h"
#include "common/cNumber.h"
#include "common/cString.h"
#include "common/cMemory.h"
#include "common/cBitString.h"
#include "dstruct/mmemory/cMemoryBlock.h"

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

/**
* Represents n-dimensional tuple. Every tuple is bounded with its space descriptor
* which holds meta information about the tuple (dimension, type of each item etc.).
*
* \author Michal Kratky, Radim Baca
* \version 0.1
* \date jun 2006
**/

namespace common {
	namespace datatype {
		namespace tuple {

class cTuple
{
protected:
	char *mData;
	cSpaceDescriptor *mSpaceDescriptor;

private:
	bool IsInInterval(const cTuple &ql, const cTuple &qh, unsigned int order) const;
	void hilbert_c2i(int nDims, int nBits, cBitString &hvalue) const;
	inline double ComputeTaxiDistance() const;

public:
	cTuple();
	cTuple(cSpaceDescriptor *spaceDescriptor);
	~cTuple();

	void Free();

	void Resize(cSpaceDescriptor *spaceDescriptor);
	void Resize(cSpaceDescriptor *spaceDescriptor, cMemory *memory);
	inline void Format(cSpaceDescriptor *spaceDescriptor, cMemoryBlock* memory);
	inline cSpaceDescriptor* GetSpaceDescriptor() const;
	void SetTuple(const cTuple &tuple);

	inline void SetValue(unsigned int order, float value);
	inline void SetValue(unsigned int order, double value);
	inline void SetValue(unsigned int order, int value);
	inline void SetValue(unsigned int order, unsigned int value);
	inline void SetValue(unsigned int order, char value);
	inline void SetValue(unsigned int order, unsigned char value);
	inline void SetValue(unsigned int order, wchar_t value);
	inline void SetValue(unsigned int order, unsigned short value);
	inline void SetValue(unsigned int order, short value);

	void SetMaxValues();
	inline void SetMaxValue(unsigned int order);
	inline void Clear();
	inline void Clear(unsigned int order);
	inline void ClearOther(unsigned int order);

	inline float GetFloat(unsigned int order) const;
	inline double GetDouble(unsigned int order) const;
	inline int GetInt(unsigned int order) const;
	inline unsigned int GetUInt(unsigned int order) const;
	inline char GetByte(unsigned int order) const;
	inline unsigned char GetUChar(unsigned int order) const;
	inline short GetShort(unsigned int order) const;
	inline unsigned short GetUShort(unsigned int order) const;
	inline void GetString(unsigned int order, cString &string) const;
	inline wchar_t GetWChar(unsigned int order) const;

	inline float* GetPFloat(unsigned int order) const;
	inline int* GetPInt(unsigned int order) const;
	inline unsigned int* GetPUInt(unsigned int order) const;
	inline char* GetPByte(unsigned int order) const;
	inline unsigned char* GetPUChar(unsigned int order) const;
	inline bool IsZero(unsigned int order) const;
	inline bool IsZero() const;

	inline unsigned int GetSerialSize() const;
	inline char* GetData() const;
	unsigned int GetRealDimension() const;  // !! EW: It is not inline !!
	unsigned int GetRealDimension(unsigned int hi) const;  // !! EW: It is not inline !!

	double EuclidianIntDistance(const cTuple &tuple) const;
	float UnitIntervalLength(const cTuple &tuple, unsigned int order) const;
	inline int CompareZOrder(const cTuple& tuple) const;
	inline int CompareHOrder(const cTuple& tuple) const;
	inline int CompareTaxiOrder(const cTuple& tuple) const;
	
	inline bool Read(cStream *stream);
	inline bool Write(cStream *stream) const;

	inline int Equal(const cTuple &tuple, unsigned int order) const;
	inline int Equal(const cTuple &tuple) const;
	inline bool Equal(unsigned int start, unsigned int k, const cTuple &tuple) const;
	void operator = (const cTuple &tuple);
	void operator += (const cTuple &tuple);
	inline bool operator == (const cTuple &tuple) const;
	inline bool operator != (const cTuple &tuple) const;
	inline bool operator > (const cTuple &tuple) const;
	inline bool Greater(const cTuple &tuple) const;
	inline int Compare(const cTuple &tuple) const;
	inline void Copy(const cTuple &tuple);
	//inline bool Less(const cTuple &tuple);

	bool IsInBlock(const cTuple &ql, const cTuple &qh) const;
	bool ModifyMbr(cTuple &mbrl, cTuple &mbrh) const;

	void Print(const char *string) const;
	void Print(unsigned int order, char *string) const;
	
	void ComputeHAddress(cBitString &hvalue) const;	

	unsigned int Sum1();
	unsigned int Sum2();
};
}}}
#endif