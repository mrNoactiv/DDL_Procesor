/**************************************************************************}
{                                                                          }
{    cMBRectangle.h                                                     }
{                                                                          }
{                                                                          }
{    Copyright (c) 2001, 2003                Tomas Skopal                  }
{                                                                          }
{    VERSION: 0.2                            DATE /08/2002                 }
{                                                                          }
{    following functionality:                                              }
{       hyper rectangle                                                    }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      04/11/2002 - Michal Kratky                                          }
{      05/07/2003 - Tomas Skopal (total reimplementation -				   }
{					            not arrays of uints yet, but two TTuples)  }
{**************************************************************************/

#ifndef __cMBRectangle_h__
#define __cMBRectangle_h__

#include "common/cCommon.h"
#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "common/datatype/cDTDescriptor.h"
#include "common/datatype/tuple/cTuple.h"
#include "common/datatype/tuple/cHNTuple.h"

#ifdef SSE_ENABLED
#include <pmmintrin.h> //SSE3
#include <smmintrin.h> //SSE4.1
#endif
#ifdef AVX_ENABLED
#include <immintrin.h> //AVX
#endif

//#ifndef LINUX // is needed?
//#include <intrin.h>
//#endif

using namespace common::datatype;

namespace common {
	namespace datatype {
		namespace tuple {


class Processing
{
  public: 
	typedef enum Compare { NoSSE = 0, SSE = 1, SSE2 = 2, SSEValid = 3};
	// SSEValid means that both functions IsInRectangle and IsIntersected are invoked 2x.
};

template<class TTuple>
class cMBRectangle : public cDataType
{
	TTuple mLoTuple;
	TTuple mHiTuple;

	//SIMD	constants
	static const unsigned int SSE_PackCount_Int = 4;
	static const unsigned int AVX_PackCount_Float = 8;

	static const int TupleCompare = Processing::NoSSE /*Processing::SSE*/ /*Processing::SSEValid*/;

public:
	typedef TTuple Tuple;
	static const unsigned int LengthType = TTuple::LengthType;

	static const char CODE = 'm';

	inline virtual char GetCode()							{ return CODE; }

	static unsigned int Computation_Compare;
	static unsigned int II_Compares;
	static unsigned int IR_Compares;

public:
	cMBRectangle();
	cMBRectangle(const cSpaceDescriptor* spaceDescr);
	~cMBRectangle();

	void Clear();
	void SetLoTuple(const TTuple &tuple);
	void SetHiTuple(const TTuple &tuple);

	inline TTuple* GetLoTuple();
	inline TTuple* GetHiTuple();
	inline const TTuple& GetRefLoTuple() const;
	inline const TTuple& GetRefHiTuple() const;

	void Resize(const cSpaceDescriptor* spaceDescr);
	inline int CompareArray(const char* array1, const char* array2, uint length);
	inline unsigned int HashValue(const char *array, unsigned int length, unsigned int hashTableSize);

	inline unsigned int GetSize_instance(const char *data, const cDTDescriptor *pDtd) const;

	// Instance methods but working with char*
	void CopyTo(char* cMbr_item, const cSpaceDescriptor* pSd) const;
	inline void CopyTo(char* cMbr_item, const cDTDescriptor* pDtD) const;
	
	// Static Methods
	static inline int Compare(const char* dst, const char* src, const cDTDescriptor *pSd);
	static inline unsigned int GetSize(const cSpaceDescriptor* pSd);
	static inline unsigned int GetSize(const char* data, const cDTDescriptor* pSd);
	static inline unsigned int GetMaxSize(const char* data, const cDTDescriptor* pSd);
	static inline unsigned int GetLSize(uint dataLength, const cDTDescriptor* dtd);

	inline unsigned int GetSize(const cDTDescriptor *dTd = NULL) const;
	inline unsigned int GetSize(uint tupleSize) const;
	inline unsigned int GetMaxSize(const cDTDescriptor *dTd) const;

	inline bool ModifyMbr(const char* TTuple_t, const cSpaceDescriptor* pSd);

	static inline void Copy(char* cMbr_dest, const char* cMbr_src, const cSpaceDescriptor* pSd);

	// static bool IsContained(const TTuple &hrl1, const TTuple &hrh1, const TTuple &hrl2, const TTuple &hrh2);
	// static double IntersectionVolume(const TTuple &ql1, const TTuple &qh1, const TTuple &ql2, const TTuple &qh2);
	static inline bool ModifyMbr(char* TTuple_ql, char* TTuple_qh, const char* TTuple_t, const cSpaceDescriptor* pSd);
	static inline bool ModifyMbr(char* cMbr_mbr, const char* TTuple_t, const cSpaceDescriptor* pSd);
	static inline bool ModifyMbrByMbr(char* TMbr_1, const char* TMbr_2, const cSpaceDescriptor* pSd);

	// static inline void ModifyMbr(TTuple &hrl1, TTuple &hrh1, const TTuple &hrl2, const TTuple &hrh2);

	// static double Volume(const TTuple &hrl, const TTuple &hrq);

	// static bool IsIntersected(const TTuple &ql1, const TTuple &qh1, const TTuple &ql2, const TTuple &qh2);
	static inline bool IsIntersected(const char* TTuple_ql1, const char* TTuple_qh1, const char* TTuple_ql2, const char* TTuple_qh2, const cSpaceDescriptor* spaceDescriptor);
	// static bool IsIntersected(const char* cMbr_qr1, const char* TTuple_ql2, const char* TTuple_qh2, const cSpaceDescriptor* spaceDescriptor);
	static bool IsIntersected(const char* cMbr_qr1, const TTuple& ql2, const TTuple& qh2, const cSpaceDescriptor* spaceDescriptor);
	static inline bool IsIntersected(const char* cMbr_1, const char* cMbr_2, const cSpaceDescriptor* spaceDescriptor);
	static inline bool IsIntersectedGeneral(const char* TTuple_ql1, const char* TTuple_qh1, const char* TTuple_ql2, const char* TTuple_qh2, const cSpaceDescriptor* pSd);
	static inline bool IsInRectangleGeneral(const char* TTuple_ql, const char* TTuple_qh, const char* TTuple_t, const cSpaceDescriptor* pSd);
	inline bool IsInRectangle(const char* TTuple_t, const cSpaceDescriptor* pSd);
	static inline bool IsIntersected(const char* cMBR_mbr, unsigned int order1, const cSpaceDescriptor *sd1, char* cLNTuple_ql2, char* cLNTuple_qh2, unsigned int order2, const cSpaceDescriptor* sd2);

	//static  bool IsIntersectedSSE(const char* cMbr_qr1, const TTuple& ql2, const TTuple& qh2, const cSpaceDescriptor* spaceDescriptor);
	static inline unsigned int IsInRectangle(const char* TTuple_ql, const char* TTuple_qh, const char* TTuple_t, const cSpaceDescriptor* pSd,unsigned int& counter);
	static inline bool IsInRectangle(const char* mbr, const char* tuple, const cSpaceDescriptor* pSd);
	static inline bool IsInRectangle(const char* TTuple_ql, const char* TTuple_qh, const char* TTuple_t, const cSpaceDescriptor* pSd);
	static inline bool IsInInterval(const char* TTuple_ql, const char* TTuple_qh, const char* tuple, unsigned int order, const cSpaceDescriptor* pSd);
	static inline bool IsInInterval(const char* TTuple_t, unsigned int order1, const cSpaceDescriptor *sd1, char* cLNTuple_ql2, char* cLNTuple_qh2, unsigned int order2, const cSpaceDescriptor* sd2);

	static inline double IntersectionVolume(const char* cMbr_1, const char* cMbr_2, const cSpaceDescriptor* pSd);
	static inline double IntersectionVolume(const char* TTuple_ql1, const char* TTuple_qh1, const char* TTuple_ql2, const char* TTuple_qh2, const cSpaceDescriptor* pSd);

	static bool IsContained(const char* cUnfTuple_ql1, const char* cUnfTuple_qh1, const char* cUnfTuple_ql2, const char* cUnfTuple_qh2, const cSpaceDescriptor* pSd);
	static bool IsContained(const char* cMbr_1, const char* cMbr_2, const cSpaceDescriptor* pSd);
	// static double Volume(const char* TTuple_hrl, const char* TTuple_hrh, const cSpaceDescriptor* pSd);
	static inline double Volume(const char* cMbr_mbr, const cSpaceDescriptor* pSd);
	static double Volume(const char* TTuple_ql, const char* TTuple_qh, const cSpaceDescriptor* pSd);
	static ullong UInt64Volume(const char* TTuple_ql, const char* TTuple_qh, const cSpaceDescriptor* pSd); //fk celocislny objem
	static uint UInt32Volume(const char* TTuple_ql, const char* TTuple_qh, const cSpaceDescriptor* pSd); //fk celocislny objem
	static double VolumeNTuple(const char* TTuple_ql, const char* TTuple_qh, const cSpaceDescriptor* pSd);
	static unsigned int DistanceToSide(const char* mbr, const char* tuple, const cSpaceDescriptor* sd, bool &isInMbr, bool findIsInMbrOnly, unsigned int minDistanceToSide);
	static unsigned int DistanceToCentre(const char* mbr, const char* tuple, const cSpaceDescriptor* sd, unsigned int minDistanceToCentre);

	static inline char* GetLoTuple(const char* cMbr_mem);
	static inline char* GetHiTuple(const char* cMbr_mem, const cSpaceDescriptor* pSd);

	static inline void SetLoTuple(char* cMbr_mem, const TTuple& tuple, const cSpaceDescriptor* pSd);
	static inline void SetHiTuple(char* cMbr_mem, const TTuple& tuple, const cSpaceDescriptor* pSd);

	static inline unsigned char GetLength(const char* tuple, const cDTDescriptor* pSd = NULL);

	// for codding purpose
	static inline unsigned int Encode(unsigned int method, const char* sourceBuffer, char* encodedBuffer, const cDTDescriptor* sd, uint bufferLength = NOT_DEFINED);
	static inline unsigned int Decode(unsigned int method, char* encodedBuffer, char* decodedBuffer, const cDTDescriptor* sd, uint bufferLength = NOT_DEFINED);
	static inline unsigned int EncodedSize(unsigned int method, char* sourceBuffer, const cDTDescriptor* sd);

	// for reference items purpose
	static char* MergeTuple(const char* cBitString_Mask, const char* cNTuple_t1, const char* cNTuple_t2, char* cNTuple_origin, const cDTDescriptor* pSd);

	static void Print(const char* cMbr_mbr, const char* delim, const cSpaceDescriptor* pSd);

	static inline bool IsInRectangleGeneral_SSE(const char* TTuple_ql, const char* TTuple_qh, const char* TTuple_t, const cSpaceDescriptor* pSd);
	static inline bool IsIntersectedGeneral_SSE(const char* TTuple_ql1, const char* TTuple_qh1, const char* TTuple_ql2, const char* TTuple_qh2, const cSpaceDescriptor* pSd);
#ifdef SSE_ENABLED
	static inline bool IsIntersectedSSE(const __m128i* ql1, const __m128i* qh1, const TTuple& ql2, const TTuple& qh2);
	static inline bool IsIntersectedSSE(const __m128i* ql1, const __m128i* qh1, const unsigned int* pql2, unsigned int* pqh2, const cSpaceDescriptor* pSd);
	static inline bool IsIntersectedSSE(const  unsigned int* ql, const  unsigned int* qh, const  unsigned int* mbr_ql, const  unsigned int* mbr_qh, const cSpaceDescriptor* pSd);

	static inline bool IsInRectangleSSE(const unsigned int* ql, const unsigned int* qh, unsigned int* tuple, const cSpaceDescriptor* pSd);
	static inline int  IsInRectangleSSE(const  __m128i &ql, const __m128i &qh, unsigned int* tuple, const cSpaceDescriptor* pSd);
	static inline bool IsInRectangleSSE(const  unsigned int* ql, const  unsigned int* qh, unsigned int* tuple, const cSpaceDescriptor* pSd, const  __m128i &mask);
	
	static inline void InicializeSSERegistry(__m128i &ql1, __m128i &qh1, const unsigned int *p_ql, const unsigned int *p_qh, const cSpaceDescriptor* pSd);
	static inline void InicializeSSERegistry(__m128i &ql1,  __m128i &qh1,__m128i &mbr_ql,__m128i &mbr_qh,const unsigned int *p_ql,const unsigned int *p_qh,const unsigned int *p_mbr_ql,const unsigned int *p_mbr_qh,const cSpaceDescriptor* pSd);
	
	static inline short GetResultFromSSE(unsigned int &dim, __m128i &m4);
	static inline short GetResultFromSSE(unsigned int &dim, short &result1, short &result2);
	static inline short GetMask_SSE(uint &dim);
#endif
#ifdef AVX_ENABLED
	static inline void InicializeAVXRegistry(__m256 &ql1, __m256 &qh1, const float *pql, const float *pqh, const cSpaceDescriptor* pSd);
	static inline bool IsInRectangleAVX(const float* ql, const float* qh, float* tuple, const cSpaceDescriptor* pSd);
	static inline bool IsInRectangleAVX(const  float* ql, const  float* qh, float* tuple, const cSpaceDescriptor* pSd, short &mask);
	static inline int IsInRectangleAVX_d4( const  float* ql, const  float* qh,float* tuple,const cSpaceDescriptor* pSd);
#endif
};

template<class TTuple>
unsigned int cMBRectangle<TTuple>::II_Compares = 0;

template<class TTuple>
unsigned int cMBRectangle<TTuple>::IR_Compares = 0;

template<class TTuple>
unsigned int cMBRectangle<TTuple>::Computation_Compare = 0;

template<class TTuple>
cMBRectangle<TTuple>::cMBRectangle() { }

template<class TTuple>
cMBRectangle<TTuple>::~cMBRectangle() { }

template<class TTuple>
cMBRectangle<TTuple>::cMBRectangle(const cSpaceDescriptor* spaceDescr)
{
	Resize(spaceDescr);
}

template<class TTuple>
inline unsigned int cMBRectangle<TTuple>::GetLSize(uint dataLength, const cDTDescriptor* dtd)
{
	printf("cMBRectangle<TTuple>::GetLSize not implemented !!!");
	return NULL;
}

template<class TTuple>
inline int cMBRectangle<TTuple>::Compare(const char* dst, const char* src, const cDTDescriptor *pSd)
{
	printf("cMBRectangle<TTuple>::Compare not implemented !!!");
	return 0;
}

template<class TTuple>
inline unsigned int cMBRectangle<TTuple>::GetSize(const cDTDescriptor *dTd) const
{
	return 2 * TTuple::GetSize(NULL, (cSpaceDescriptor*) dTd);
}

template<class TTuple>
inline unsigned int cMBRectangle<TTuple>::GetSize(uint tupleSize) const
{
	return 2 * tupleSize;
}

template<class TTuple>
inline unsigned int cMBRectangle<TTuple>::GetMaxSize(const cDTDescriptor *dTd) const
{
	//is this ok? bas064
	return 2 * TTuple::GetMaxSize(NULL, (cSpaceDescriptor*)dTd);
}

template<class TTuple>
inline unsigned int cMBRectangle<TTuple>::GetSize(const cSpaceDescriptor* pSd)
{
	return 2 * TTuple::GetSize(NULL, pSd);
}

template<class TTuple>
inline unsigned int cMBRectangle<TTuple>::GetSize(const char* data, const cDTDescriptor* pSd)
{
	unsigned int size = TTuple::GetSize(data, pSd);
	return size + TTuple::GetSize(data + size, pSd);
}

template<class TTuple>
inline unsigned int cMBRectangle<TTuple>::GetMaxSize(const char* data, const cDTDescriptor* pSd)
{
	unsigned int size = TTuple::GetMaxSize(data, (cSpaceDescriptor*)pSd);
	return size + TTuple::GetMaxSize(data + size, (cSpaceDescriptor*)pSd);
}

template<class TTuple>
inline unsigned char cMBRectangle<TTuple>::GetLength(const char* tuple, const cDTDescriptor* pSd)
{
	printf("cMBRectangle::GetLength is not implemented yet !!!");
	return NULL;
}

//template<class TTuple>
//cMBRectangle<TTuple>::cMBRectangle(common::datatype::tuple::cMBRectangle& rec)
//{
//	mSpaceDescriptor = rec.mSpaceDescriptor;
//
//	mLowTuple.Resize(mSpaceDescriptor);
//	mHiTuple.Resize(mSpaceDescriptor);
//
//	mLowTuple = rec.mLowTuple;
//	mHiTuple = rec.mHiTuple;
//}

template<class TTuple>
inline TTuple* cMBRectangle<TTuple>::GetLoTuple()
{ 
	return &mLoTuple; 
}

template<class TTuple>
inline TTuple* cMBRectangle<TTuple>::GetHiTuple()
{ 
	return &mHiTuple; 
}

template<class TTuple>
inline const TTuple& cMBRectangle<TTuple>::GetRefLoTuple() const
{ 
	return mLoTuple; 
}

template<class TTuple>
inline const TTuple& cMBRectangle<TTuple>::GetRefHiTuple() const
{ 
	return mHiTuple; 
}

//template<class TTuple>
//void cMBRectangle<TTuple>::SetHyperRectangle(const cMBRectangle &hypRect)
//{
//	mLowTuple = hypRect.GetRefLowTuple();
//	mHiTuple = hypRect.GetRefHiTuple();
//}

template<class TTuple>
void cMBRectangle<TTuple>::Clear()
{
	mLoTuple.Clear();
	mHiTuple.Clear();
}

template<class TTuple>
void cMBRectangle<TTuple>::SetLoTuple(const TTuple &tuple)
{
	mLoTuple = tuple;
}

template<class TTuple>
void cMBRectangle<TTuple>::SetHiTuple(const TTuple &tuple)
{
	mHiTuple = tuple;
}

template<class TTuple>
void cMBRectangle<TTuple>::Resize(const cSpaceDescriptor* sd)
{
	mLoTuple.Resize(sd);
	mHiTuple.Resize(sd);
}

/**
 * Semantic of this method is rather problematic for cTuple, since cDataType::CompareArray is designed for
 * comparison of two arrays of primitive data type values.
 */
template<class TTuple>
inline int cMBRectangle<TTuple>::CompareArray(const char* array1, const char* array2, uint length)
{
	printf("Warning: cMBRectangle<TTuple>::CompareArray(): This method should not be invoked!\n");
	return -1;
}

template<class TTuple>
inline unsigned int cMBRectangle<TTuple>::HashValue(const char *array, unsigned int length, unsigned int hashTableSize)
{
	printf("Warning: cMBRectangle<TTuple>::HashValue(): This method should not be invoked!\n");
	return 0;
}

template<class TTuple>
inline unsigned int cMBRectangle<TTuple>::GetSize_instance(const char *data, const cDTDescriptor *pDtd) const
{
	return cMBRectangle<TTuple>::GetSize(data, pDtd);
}

/**
 * Return the pointer to the low tuple.
 */
template<class TTuple>
inline char* cMBRectangle<TTuple>::GetLoTuple(const char* cMbr_mem)
{
	return (char*)cMbr_mem;
}

/**
 * Return the pointer to the high tuple.
 */
template<class TTuple>
inline char* cMBRectangle<TTuple>::GetHiTuple(const char* cMbr_mem, const cSpaceDescriptor* pSd)
{
	unsigned int size = TTuple::GetMaxSize(cMbr_mem, pSd);
	return (char*)(cMbr_mem + size);
}

/**
 * Copy the lo tuple to the char* Mbr.
 */
template<class TTuple>
inline void cMBRectangle<TTuple>::SetLoTuple(char* cMbr_mem, const TTuple& tuple, const cSpaceDescriptor* pSd)
{
	TTuple::SetTuple(cMbr_mem, tuple, pSd);
}

/**
 * Copy the hi tuple to the char* Mbr.
 */
template<class TTuple>
inline void cMBRectangle<TTuple>::SetHiTuple(char* cMbr_mem, const TTuple& tuple, const cSpaceDescriptor* pSd)
{
	TTuple::SetTuple(cMbr_mem + TTuple::GetMaxSize(cMbr_mem, pSd), tuple, pSd);
}

/**
 * Return true if QR1 and QR2 are intersected.
 */
template<class TTuple>
inline bool cMBRectangle<TTuple>::IsIntersected(const char* cMbr_qr1, const TTuple& ql2, const TTuple& qh2, const cSpaceDescriptor* pSd)
{
	return IsIntersected(GetLoTuple(cMbr_qr1), GetHiTuple(cMbr_qr1, pSd), ql2.GetData(), qh2.GetData(), pSd);
}

template<class TTuple>
inline bool cMBRectangle<TTuple>::IsIntersected(const char* cMbr_1, const char* cMbr_2, const cSpaceDescriptor* pSd)
{
	return IsIntersected(GetLoTuple(cMbr_1), GetHiTuple(cMbr_1, pSd), GetLoTuple(cMbr_2), GetHiTuple(cMbr_2, pSd), pSd);
}

/**
 * Return true if QR1 and QR2 are intersected.
 */
template<class TTuple>
inline bool cMBRectangle<TTuple>::IsIntersected(const char* TTuple_ql1, const char* TTuple_qh1, const char* TTuple_ql2, const char* TTuple_qh2, const cSpaceDescriptor* pSd)
{
	//return IsIntersectedGeneral(TTuple_ql1, TTuple_qh1, TTuple_ql2,TTuple_qh2, pSd);
	
	bool ret;
	if (cMBRectangle<TTuple>::TupleCompare == Processing::NoSSE)
	{
		ret = IsIntersectedGeneral(TTuple_ql1, TTuple_qh1, TTuple_ql2,TTuple_qh2, pSd);
	}
	else
	{
		ret = IsIntersectedGeneral_SSE(TTuple_ql1, TTuple_qh1, TTuple_ql2, TTuple_qh2, pSd);
	}
	return ret;
}

template<class TTuple>
inline bool cMBRectangle<TTuple>::IsIntersectedGeneral(const char* TTuple_ql1, const char* TTuple_qh1, const char* TTuple_ql2, const char* TTuple_qh2, const cSpaceDescriptor* pSd)
{
	 bool ret = false;

	// hyperectangle are intersected, if intervals in all dimensions are intersected
	for (unsigned int i = 0 ; i < pSd->GetDimension() ; i++)
	{
		ret = false;
		Computation_Compare++;
		if (TTuple::Equal(TTuple_ql1, TTuple_qh2, i, pSd) <= 0)
		{
			Computation_Compare++;
			if (TTuple::Equal(TTuple_qh1, TTuple_ql2, i, pSd) >= 0)
			{
				ret = true;
			}
		}

		if (!ret)
		{
			break;
		}
	}
	return ret;
}

/// Return volume of intersection.
//template<class TTuple>
//double cMBRectangle<TTuple>::IntersectionVolume(const TTuple &ql1, const TTuple &qh1, const TTuple &ql2, const TTuple &qh2)
//{
//	double intersectionVolume = 1.0;
//	float interval;
//
//	// hyperectangle are intersected, if intervals in all dimensions are intersected
//	for (unsigned int i = 0 ; i < ql1.GetSpaceDescriptor()->GetDimension() ; i++)
//	{
//		if (ql1.Equal(qh1, i) <= 0)
//		{
//			if (ql2.Equal(qh2, i) <= 0)
//			{
//				if (ql2.Equal(qh1, i) <= 0 && ql2.Equal(ql1, i) >= 0)
//				{
//					interval = qh2.UnitIntervalLength(ql2, i);
//				}
//				else if (ql1.Equal(ql2, i) >= 0 && ql1.Equal(qh2, i) <= 0)
//				{
//					interval = qh2.UnitIntervalLength(ql1, i);
//				}
//				else  // MBRs aren't intersect
//				{
//					intersectionVolume = 0.0;
//					break;
//				}
//
//				intersectionVolume *= interval;
//			}
//			else
//			{
//				printf("Critical Error: cMBRectangle_BS::IsIntersected(): ql2 > qh2!");
//				exit(1);
//			}
//		}
//		else
//		{
//			printf("Critical Error: cMBRectangle_BS::IsIntersected(): ql1 > qh1!");
//			exit(1);
//		}
//	}
//	return intersectionVolume;
//}

/// Return volume of intersection.
template<class TTuple>
double cMBRectangle<TTuple>::IntersectionVolume(const char* cMbr_1, const char* cMbr_2, const cSpaceDescriptor* pSd)
{
	return IntersectionVolume(GetLoTuple(cMbr_1), GetHiTuple(cMbr_1, pSd),
		GetLoTuple(cMbr_2), GetHiTuple(cMbr_2, pSd), pSd);
}

/// Return volume of intersection.
template<class TTuple>
double cMBRectangle<TTuple>::IntersectionVolume(const char* TTuple_ql1, const char* TTuple_qh1, const char* TTuple_ql2, const char* TTuple_qh2, const cSpaceDescriptor* pSd)
{
	double intersectionVolume = 1.0;
	float interval;
	unsigned int dim = pSd->GetDimension();
	char typeCode = pSd->GetDimensionTypeCode(0);

	// hyperectangle are intersected, if intervals in all dimensions are intersected
	for (unsigned int i = 0 ; i < dim ; i++)
	{
		if (TTuple::Equal(TTuple_ql1, TTuple_qh1, i, pSd) <= 0)
		{
			if (TTuple::Equal(TTuple_ql2, TTuple_qh2, i, pSd) <= 0)
			{
				if (TTuple::Equal(TTuple_ql2, TTuple_qh1, i, pSd) <= 0 && TTuple::Equal(TTuple_ql2, TTuple_ql1, i, pSd) >= 0)
				{
					interval = TTuple::UnitIntervalLength(TTuple_qh2, TTuple_ql2, i, pSd);
				}
				else if (TTuple::Equal(TTuple_ql1, TTuple_ql2, i, pSd) >= 0 && TTuple::Equal(TTuple_ql1, TTuple_qh2, i, pSd) <= 0)
				{
					interval = TTuple::UnitIntervalLength(TTuple_qh2, TTuple_ql1, i, pSd);
				}
				else  // MBRs aren't intersect
				{
					intersectionVolume = 0.0;
					break;
				}

				intersectionVolume *= interval;
			}
			else
			{
				printf("Critical Error: cMBRectangle::IntersectionVolume(): ql2 > qh2!");
				exit(1);
			}
		}
		else
		{
			printf("\nCritical Error: cMBRectangle::IntersectionVolume(): ql1 > qh1!\n");
			TTuple::Print(TTuple_ql1, "\n", pSd);
			TTuple::Print(TTuple_qh1, "\n", pSd);
			exit(1);
		}
	}
	return intersectionVolume;
}

/**
 * Return true if hyperblock2 is contained in hyperblock1.
 */
//template<class TTuple>
//bool cMBRectangle<TTuple>::IsContained(const TTuple &ql1, const TTuple &qh1, const TTuple &ql2, const TTuple &qh2)
//{
//	char *error = "Critical Error: cMBRectangle_BS::IsIntersected(): ql2 > qh2!";
//	bool ret = true;
//
//	// hyperectangle are intersected, if intervals in all dimensions are intersected
//	for (unsigned int i = 0 ; i < ql1.GetSpaceDescriptor()->GetDimension() ; i++)
//	{
//		if (ql1.Equal(qh1, i) <= 0)
//		{
//			if (ql2.Equal(qh2, i) <= 0)
//			{
//				if ((ql2.Equal(ql1, i) >= 0 && ql2.Equal(qh1, i) <= 0) &&
//					(qh2.Equal(ql1, i) >= 0 && qh2.Equal(qh1, i) <= 0))
//				{
//					continue;
//				}
//				else
//				{
//					ret = false;
//					break;
//				}
//			}
//			else
//			{
//				printf(error);
//				exit(1);
//			}
//		}
//		else
//		{
//			printf(error);
//			exit(1);
//		}
//	}
//	return ret;
//}

template<class TTuple>
bool cMBRectangle<TTuple>::IsContained(const char* cMbr_1, const char* cMbr_2, const cSpaceDescriptor* pSd)
{
	return IsContained(GetLoTuple(cMbr_1), GetHiTuple(cMbr_1, pSd), GetLoTuple(cMbr_2), GetHiTuple(cMbr_2, pSd), pSd);
}

/**
 * Return true if hyperblock2 is contained in hyperblock1.
 */
template<class TTuple>
bool cMBRectangle<TTuple>::IsContained(const char* TTuple_ql1, const char* TTuple_qh1, const char* TTuple_ql2, const char* TTuple_qh2, const cSpaceDescriptor* pSd)
{
	char *error = "\nCritical Error: cMBRectangle<TTuple>::IsContained(): ql2 > qh2!";
	bool ret = true;
	unsigned int dim = pSd->GetDimension();

	// hyperectangle are intersected, if intervals in all dimensions are intersected
	for (unsigned int i = 0 ; i < dim ; i++)
	{
		if (TTuple::Equal(TTuple_ql1, TTuple_qh1, i, pSd) <= 0)
		{
			if (TTuple::Equal(TTuple_ql2, TTuple_qh2, i, pSd) <= 0)
			{
				if ((TTuple::Equal(TTuple_ql2, TTuple_ql1, i, pSd) >= 0 && 
					TTuple::Equal(TTuple_ql2, TTuple_qh1, i, pSd) <= 0) &&
					(TTuple::Equal(TTuple_qh2, TTuple_ql1, i, pSd) >= 0 && 
					TTuple::Equal(TTuple_qh2, TTuple_qh1, i, pSd) <= 0))
				{
					continue;
				}
				else
				{
					ret = false;
					break;
				}
			}
			else
			{
				printf(error);
				exit(1);
			}
		}
		else
		{
			printf(error);
			exit(1);
		}
	}
	return ret;
}

/**
 * Compute volume of hyper-rectangle
 */
template<class TTuple>
double cMBRectangle<TTuple>::Volume(const char* TTuple_ql, const char* TTuple_qh, const cSpaceDescriptor* pSd)
{
	double volume = 1.0;
	float fa;
	unsigned int uia;
	unsigned int dim = pSd->GetDimension();

	for (unsigned int i = 0 ; i < dim ; i++)
	{
		switch (pSd->GetDimensionTypeCode(i))
		{
			case cFloat::CODE:
				fa = cNumber::Abs(TTuple::GetFloat(TTuple_qh, i, pSd) - TTuple::GetFloat(TTuple_ql, i, pSd)) / cFloat::MAX;
				volume *= fa;
				break;
			case cInt::CODE:
				volume *= (double)cNumber::Abs(TTuple::GetInt(TTuple_qh, i, pSd) - TTuple::GetInt(TTuple_ql, i, pSd)) / cInt::MAX;
				break;
			case cUInt::CODE:
				if (TTuple::GetUInt(TTuple_qh, i, pSd) > TTuple::GetUInt(TTuple_ql, i, pSd))
				{
					uia = TTuple::GetUInt(TTuple_qh, i, pSd) - TTuple::GetUInt(TTuple_ql, i, pSd);
				}
				else
				{
					uia = TTuple::GetUInt(TTuple_ql, i, pSd) - TTuple::GetUInt(TTuple_qh, i, pSd);
				}
				volume *= (double)(uia+1) / cUInt::MAX;
				break;
			case cChar::CODE:
				volume *= (double)cNumber::Abs(TTuple::GetByte(TTuple_qh, i, pSd) - TTuple::GetByte(TTuple_ql, i, pSd)) / cChar::MAX;
				break;
			case cNTuple::CODE:
				//TTuple::Print(TTuple_ql,"\n",pSd);
				//TTuple::Print(TTuple_qh,"\n",pSd);
				volume = VolumeNTuple(cHNTuple::GetPValue(TTuple_ql, i, pSd), cHNTuple::GetPValue(TTuple_qh, i, pSd), pSd->GetDimSpaceDescriptor(i));
				break;
		}
	}
	return volume;
}

/**
 * Compute volume of hyper-rectangle for NTuple only
 */
template<class TTuple>
double cMBRectangle<TTuple>::VolumeNTuple(const char* TTuple_ql, const char* TTuple_qh, const cSpaceDescriptor* pSd)
{
	double volume = 0.0;
	unsigned int uia;
	//dim -> mensi dimenze
	unsigned int dim  = ((unsigned int)(*TTuple_ql)<(unsigned int)(*TTuple_qh) ? (unsigned int)(*TTuple_ql) : (unsigned int)(*TTuple_qh));

	int diff;

	for (unsigned int i = 0 ; i < dim ; i++)
	{
		switch (pSd->GetDimensionTypeCode(i))
		{
			//case cInt::CODE:
			//	break;
			case cUInt::CODE:
				unsigned int p1 = cNTuple::GetInt(TTuple_ql, i, pSd);
				unsigned int p2 = cNTuple::GetInt(TTuple_qh, i, pSd);
				//if (p1!=p2)
				//{
				//	int bb = 1;
				//}

				diff = abs((int) (cNTuple::GetInt(TTuple_ql, i, pSd) - cNTuple::GetInt(TTuple_qh, i, pSd)));
				uia = ceil((double)((double) diff/cUInt::MAX*9));
				volume += double (uia) * (pow((double) 10.0, ((double) i + 1.0) * -1.0));
				break;
		}
	}
	return volume;
}

/**
 * Compute Uint64 volume of hyper-rectangle
 */
template<class TTuple>
ullong cMBRectangle<TTuple>::UInt64Volume(const char* TTuple_ql, const char* TTuple_qh, const cSpaceDescriptor* pSd)
{
	ullong volume = 1;
	ullong fa;
	unsigned int uia;
	unsigned int dim = pSd->GetDimension();

	for (unsigned int i = 0 ; i < dim ; i++)
	{
		switch (pSd->GetDimensionTypeCode(i)) /* efficiency warning - TTuple */
		{
			//case cFloat::CODE:
			//	fa = cNumber::Abs(TTuple::GetFloat(TTuple_qh, i, pSd) - TTuple::GetFloat(TTuple_ql, i, pSd)) / cFloat::MAX;
			//	volume *= fa;
			//	break;
			case cInt::CODE:
				volume *= (unsigned int)cNumber::Abs(TTuple::GetInt(TTuple_qh, i, pSd) - TTuple::GetInt(TTuple_ql, i, pSd));
				break;
			case cUInt::CODE:
				if (TTuple::GetUInt(TTuple_qh, i, pSd) > TTuple::GetUInt(TTuple_ql, i, pSd))
				{
					uia = TTuple::GetUInt(TTuple_qh, i, pSd) - TTuple::GetUInt(TTuple_ql, i, pSd);
				}
				else
				{
					uia = TTuple::GetUInt(TTuple_ql, i, pSd) - TTuple::GetUInt(TTuple_qh, i, pSd);
				}
				volume *= (unsigned int)(uia+1);
				break;
			//case cChar::CODE:
			//	volume *= (double)cNumber::Abs(TTuple::GetByte(TTuple_qh, i, pSd) - TTuple::GetByte(TTuple_ql, i, pSd)) / cChar::MAX;
			//	break;
		}
	}
	return volume;
}

/**
 * Compute Uint32 volume of hyper-rectangle
 */
template<class TTuple>
uint cMBRectangle<TTuple>::UInt32Volume(const char* TTuple_ql, const char* TTuple_qh, const cSpaceDescriptor* pSd)
{
	ullong volume = 1;
	ullong fa;
	unsigned int uia;
	unsigned int dim = pSd->GetDimension();

	for (unsigned int i = 0 ; i < dim ; i++)
	{
		switch (pSd->GetDimensionTypeCode(i)) /* efficiency warning - TTuple */
		{
			//case cFloat::CODE:
			//	fa = cNumber::Abs(TTuple::GetFloat(TTuple_qh, i, pSd) - TTuple::GetFloat(TTuple_ql, i, pSd)) / cFloat::MAX;
			//	volume *= fa;
			//	break;
			case cInt::CODE:
				volume *= (unsigned int)cNumber::Abs(TTuple::GetInt(TTuple_qh, i, pSd) - TTuple::GetInt(TTuple_ql, i, pSd));
				break;
			case cUInt::CODE:
				if (TTuple::GetUInt(TTuple_qh, i, pSd) > TTuple::GetUInt(TTuple_ql, i, pSd))
				{
					uia = TTuple::GetUInt(TTuple_qh, i, pSd) - TTuple::GetUInt(TTuple_ql, i, pSd);
				}
				else
				{
					uia = TTuple::GetUInt(TTuple_ql, i, pSd) - TTuple::GetUInt(TTuple_qh, i, pSd);
				}
				volume *= (unsigned int)(uia+1);
				break;
			//case cChar::CODE:
			//	volume *= (double)cNumber::Abs(TTuple::GetByte(TTuple_qh, i, pSd) - TTuple::GetByte(TTuple_ql, i, pSd)) / cChar::MAX;
			//	break;
		}
	}
	return volume;
}

/**
 * Compute volume of hyper-rectangle
 */
template<class TTuple>
inline double cMBRectangle<TTuple>::Volume(const char* cMbr_mbr, const cSpaceDescriptor* pSd)
{
	return Volume(GetLoTuple(cMbr_mbr), GetHiTuple(cMbr_mbr, pSd), pSd);
}

/**
 * DistanceToSide
 * I don't why but the TaxiDistance is much more better than the max distance to a side
 * for dim 8 but a litle bit worse for dim 2-5.
 * \param findIsInMbrOnly - a previous invocation returns isInMbr = true, it means compute distance only if the mbr is in mbr. Why?
 *  The number of operations is lower.
 *  \param minDistanceToSide The minimal distance is shorter? Finish the computation
 */
template<class TTuple>
unsigned int cMBRectangle<TTuple>::DistanceToSide(const char* mbr, const char* tuple, const cSpaceDescriptor* sd, bool &isInMbr, bool findIsInMbrOnly, unsigned int minDistanceToSide)
{
	unsigned int dim = sd->GetDimension();
	char* ql = GetLoTuple(mbr);
	char* qh = GetHiTuple(mbr, sd);
	llong dl, dh;
	unsigned int daux, d = 0;
	isInMbr = true;
	const unsigned int borderDimension = 6;

	for (unsigned int i = 0 ; i < dim ; i++)
	{
		daux = 0;
		switch (sd->GetDimensionTypeCode(i))
		{
			int tval;
			case cInt::CODE:
				tval = TTuple::GetInt(tuple, i, sd);
				dl = tval - TTuple::GetInt(ql, i, sd);
				dh = tval - TTuple::GetInt(qh, i, sd);
				break;
			case cUInt::CODE:
				llong tv = TTuple::GetUInt(tuple, i, sd);
				llong qlval = TTuple::GetUInt(ql, i, sd);
				llong qhval = TTuple::GetUInt(qh, i, sd);
				dl = tv - qlval;
				dh = tv - qhval;
				break;
		}

		if (dl < 0 && dh < 0)
		{
			daux = (unsigned int)-dl;
			isInMbr = false;
		}
		else if (dl > 0 && dh > 0)
		{
			daux = (unsigned int)dh;
			isInMbr = false;
		}

		// if a previous invocation returns isInMbr = true, you finish the computation, since you
		// find only mbrs where the tuple is contained.
		if (findIsInMbrOnly && !isInMbr)
		{
			break;
		}

		if (dim <= borderDimension)
		{
			if (i == 0 || daux > d)
			{
				d = daux;
			}
		} else
		{
		  // TaxiDistance
		  d += daux;
		}

		if (d > minDistanceToSide)
		{
			break;  // the minimal distance is shorter? Finish the computation
		}
	}
	return d;
}

/**
 * DistanceToCentre
 * I don't why but the TaxiDistance is much more better than the max distance to a side.
 *  \param minDistanceToSide The minimal distance is shorter? Finish the computation

 */
template<class TTuple>
unsigned int cMBRectangle<TTuple>::DistanceToCentre(const char* mbr, const char* tuple, const cSpaceDescriptor* sd, unsigned int minDistanceToCentre)
{
	unsigned int dim = sd->GetDimension();
	char* ql = GetLoTuple(mbr);
	char* qh = GetHiTuple(mbr, sd);
	unsigned int d = 0;

	for (unsigned int i = 0 ; i < dim ; i++)
	{
		unsigned int daux;
		switch (sd->GetDimensionTypeCode(i))
		{
			int c;
			unsigned int uc;
			case cInt::CODE:
				c = (TTuple::GetInt(qh, i, sd) + TTuple::GetInt(ql, i, sd)) / 2;
				// TaxiDistance
				d += abs(c - TTuple::GetInt(tuple, i, sd));

				// minMaxTaxiDistance
				// daux = abs(c - TTuple::GetInt(tuple, i, sd));
				break;
			case cUInt::CODE:
				uc = (TTuple::GetUInt(qh, i, sd) + TTuple::GetUInt(ql, i, sd)) / 2;
				unsigned int tval = TTuple::GetUInt(tuple, i, sd);

				if (uc >= tval)
				{
					daux = uc - tval;
				} else
				{
					daux = tval - uc;
				}

				// TaxiDistance only
				d += daux;

				break;
		}

		if (d > minDistanceToCentre)
		{
			break;  // the minimal distance is shorter? Finish the computation
		}

		// minMaxTaxiDistance
		//if (daux > d)
		//{
		//	d = daux;
		//}
	}
	return d;
}

/// Compute volume of hyper-rectangle
//template<class TTuple>
//double cMBRectangle<TTuple>::Volume(const TTuple &hrl, const TTuple &hrh)
//{
//	double volume = 1.0;
//	float fa;
//	unsigned int uia;
//
//	for (unsigned int i = 0 ; i < hrl.GetSpaceDescriptor()->GetDimension() ; i++)
//	{
//		switch (hrl.GetSpaceDescriptor()->GetType(i)->GetCode())
//		{
//			case cFloat::CODE:
//				fa = cNumber::Abs(hrh.GetFloat(i) - hrl.GetFloat(i)) / cFloat::MAX;
//				volume *= fa;
//				break;
//			case cInt::CODE:
//				volume *= (double)cNumber::Abs(hrh.GetInt(i) - hrl.GetInt(i)) / cInt::MAX;
//				break;
//			case cUInt::CODE:
//				if (hrh.GetUInt(i) > hrl.GetUInt(i))
//				{
//					uia = hrh.GetUInt(i) - hrl.GetUInt(i);
//				}
//				else
//				{
//					uia = hrl.GetUInt(i) - hrh.GetUInt(i);
//				}
//				volume *= (double)(uia+1) / cUInt::MAX;
//				break;
//			case cChar::CODE:
//				volume *= (double)cNumber::Abs(hrh.GetByte(i) - hrl.GetByte(i)) / cChar::MAX;
//				break;
//		}
//	}
//	return volume;
//}

///// Compute volume of hyper-rectangle
//template<class TTuple>
//double cMBRectangle<TTuple>::Volume(const char* TTuple_hrl, const char* TTuple_hrh, const cSpaceDescriptor* pSd)
//{
//	double volume = 1.0;
//	float fa;
//	unsigned int uia;
//	unsigned int dim = pSd->GetDimension();
//
//	for (unsigned int i = 0 ; i < dim ; i++)
//	{
//		switch (pSd->GetType(i)->GetCode()) /* efficiency warning - TTuple */
//		{
//			case cFloat::CODE:
//				fa = cNumber::Abs(TTuple::GetFloat(TTuple_hrh, i) - TTuple::GetFloat(TTuple_hrl, i)) / cFloat::MAX;
//				volume *= fa;
//				break;
//			case cInt::CODE:
//				volume *= (double)cNumber::Abs(TTuple::GetInt(TTuple_hrh, i) - TTuple::GetInt(TTuple_hrl, i)) / cInt::MAX;
//				break;
//			case cUInt::CODE:
//				if (TTuple::GetUInt(TTuple_hrh, i) > TTuple::GetUInt(TTuple_hrl, i))
//				{
//					uia = TTuple::GetUInt(TTuple_hrh, i) - TTuple::GetUInt(TTuple_hrl, i);
//				}
//				else
//				{
//					uia = TTuple::GetUInt(TTuple_hrl, i) - TTuple::GetUInt(TTuple_hrh, i);
//				}
//				volume *= (double)(uia+1) / cUInt::MAX;
//				break;
//			case cChar::CODE:
//				volume *= (double)cNumber::Abs(TTuple::GetByte(TTuple_hrh, i) - TTuple::GetByte(TTuple_hrl, i)) / cChar::MAX;
//				break;
//		}
//	}
//	return volume;
//}


///**
// * Create mbr of two mbrs.
// */
//template<class TTuple>
//void cMBRectangle<TTuple>::ModifyMbr(TTuple &hrl1, TTuple &hrh1, const TTuple &hrl2, const TTuple &hrh2)
//{
//	hrl2.ModifyMbr(hrl1, hrh1);
//	hrh2.ModifyMbr(hrl1, hrh1);
//}

/**
 * Modify the first MBR Create mbr of two mbrs.
 */
template<class TTuple>
inline bool cMBRectangle<TTuple>::ModifyMbrByMbr(char* TMbr_1, const char* TMbr_2, const cSpaceDescriptor* pSd)
{
	bool r1 = ModifyMbr(TMbr_1, GetLoTuple(TMbr_2), pSd);
	r1 |= ModifyMbr(TMbr_1, GetHiTuple(TMbr_2, pSd), pSd);
	return r1;
}

/**
* \return true if the tuple is contained into n-dimensional query block.
*/
template<class TTuple>
inline bool cMBRectangle<TTuple>::IsInRectangle(const char* TTuple_ql, const char* TTuple_qh, const char* TTuple_t, const cSpaceDescriptor* pSd)
{
	bool ret;
	if (cMBRectangle<TTuple>::TupleCompare == Processing::NoSSE)
	{
		ret = IsInRectangleGeneral(TTuple_ql, TTuple_qh, TTuple_t, pSd);
	}
	else
	{
		ret = IsInRectangleGeneral_SSE(TTuple_ql, TTuple_qh, TTuple_t, pSd);
	}
	return ret;
}

/**
* \return true if the tuple is contained into n-dimensional query block.
*/
template<class TTuple>
inline bool cMBRectangle<TTuple>::IsInRectangleGeneral(const char* TTuple_ql, const char* TTuple_qh, const char* TTuple_t, const cSpaceDescriptor* pSd)
{
	bool ret = true;
	unsigned int dim = pSd->GetDimension();

	for (unsigned int i = 0 ; i < dim ; i++)
	{
		IR_Compares++;
		if (!IsInInterval(TTuple_ql, TTuple_qh, TTuple_t, i, pSd))
		{
			ret = false;
			break;
		}
	}
	return ret;
}


/**
* \return true if the tuple is contained into n-dimensional query block. Counter returns number of comparison.
*/
template<class TTuple>
inline unsigned int cMBRectangle<TTuple>::IsInRectangle(const char* TTuple_ql, const char* TTuple_qh, const char* TTuple_t, const cSpaceDescriptor* pSd,unsigned int& counter )
{
	/*if (cRTreeConst.TupleCompare == Processing::SSE)
	{
		return IsInRectangleSSE((unsigned int)* TTuple_ql ,(unsigned int)* TTuple_ql,(unsigned int)* TTuple_t,pSd);
	}
	else*/
	{
		unsigned int dim = pSd->GetDimension();
		unsigned int ret = dim;
		counter = dim;
		for (unsigned int i = 0 ; i < dim ; i++)
		{
			IR_Compares++;
			if (!IsInInterval(TTuple_ql, TTuple_qh, TTuple_t, i, pSd))
			{
				counter=i+1;
				ret = i;
				break;
			}
		}
		return ret;
	}
}

/**
* \return true if the tuple is contained into n-dimensional query block.
*/
template<class TTuple>
inline bool cMBRectangle<TTuple>::IsInRectangle(const char* TTuple_t, const cSpaceDescriptor* pSd)
{
    unsigned int dim = pSd->GetDimension();
    for (unsigned int i = 0 ; i < dim ; i++)
    {
        if (!IsInInterval(mLoTuple, mHiTuple, TTuple_t, i, pSd))
            return false;
    }
    return true;
}

template<class TTuple>
inline unsigned int cMBRectangle<TTuple>::Encode(unsigned int method, const char* sourceBuffer, char* encodedBuffer, const cDTDescriptor* sd, uint bufferLength)
{
	printf("Critical Error: It is not defined!\n");
	return 0;
}

template<class TTuple>
inline unsigned int cMBRectangle<TTuple>::Decode(unsigned int method, char* encodedBuffer, char* decodedBuffer, const cDTDescriptor* sd, uint bufferLength)
{
	printf("Critical Error: It is not defined!\n");
	return 0;
}

template<class TTuple>
inline unsigned int cMBRectangle<TTuple>::EncodedSize(unsigned int method, char* sourceBuffer, const cDTDescriptor* sd)
{
	printf("Critical Error: It is not defined!\n");
	return 0;
}

	// for reference items purpose
template<class TTuple>
char* cMBRectangle<TTuple>::MergeTuple(const char* cBitString_Mask, const char* cNTuple_t1, const char* cNTuple_t2, char* cNTuple_origin, const cDTDescriptor* pSd)
{
	printf("Critical Error: It is not defined!\n");
	return NULL;
}
/**
* \return true if the tuple is contained into n-dimensional query block.
*/
template<class TTuple>
inline bool cMBRectangle<TTuple>::IsInRectangle(const char* mbr, const char* tuple, const cSpaceDescriptor* pSd)
{
	return IsInRectangle(GetLoTuple(mbr), GetHiTuple(mbr, pSd), tuple, pSd);
}

/**
* \return true if the tuple coordinate is contained in the interval.
*/
template<class TTuple>
bool cMBRectangle<TTuple>::IsInInterval(const char* TTuple_ql, const char* TTuple_qh, const char* tuple, unsigned int order, const cSpaceDescriptor* pSd)
{
	// assert(order < mDimension);
	//fk zbytecne char typeCode = pSd->GetType(order)->GetCode();
	bool ret = false;

	Computation_Compare++;
	if (TTuple::Equal(TTuple_ql, tuple, order, pSd) <= 0)
	{
		Computation_Compare++;
		if (TTuple::Equal(TTuple_qh, tuple, order, pSd) >= 0)
		{
			ret = true;
		}
	}

	/*bool ret = true;
	int eq;

	if ((eq = TTuple::Equal(TTuple_ql, tuple, order, pSd)) > 0)
	{
		if (TTuple::Equal(TTuple_qh, tuple, order, pSd) > 0)
		{
			ret = false;
		}
	}
	else if (eq < 0)
	{
		if (TTuple::Equal(TTuple_qh, tuple, order, pSd) < 0)
		{
			ret = false;
		}
	}*/
	return ret;
}

/*
 * It computes if two intervals are instersected (for Cartesian Range Query of the R-tree), 
 * the first interval is defined by a coordinate of MBR, 
 * the second interval is defined with a coordinate of two n-tuples.
 */
template<class TTuple>
inline bool cMBRectangle<TTuple>::IsIntersected(const char* cMBR_mbr, unsigned int order1, const cSpaceDescriptor *sd1,
	char* cLNTuple_ql2, char* cLNTuple_qh2, unsigned int order2, const cSpaceDescriptor* sd2)
{
	char* TTuple_ql1 = cMBRectangle<TTuple>::GetLoTuple(cMBR_mbr);
	bool ret = false;

	II_Compares++;

	/*
	II_Compares++;

	if (TTuple::Equal(TTuple_ql1, order1, sd1, cLNTuple_qh2, order2, sd2) > 0 ||
		TTuple::Equal(TTuple_qh1, order1, sd1, cLNTuple_ql2, order2, sd2) < 0)
	{
		ret = false;
	}*/

	Computation_Compare++;
	if (TTuple::Equal(TTuple_ql1, order1, sd1, cLNTuple_qh2, order2, sd2) <= 0)
	{
		char* TTuple_qh1 = cMBRectangle<TTuple>::GetHiTuple(cMBR_mbr, sd1);
		Computation_Compare++;
		if (TTuple::Equal(TTuple_qh1, order1, sd1, cLNTuple_ql2, order2, sd2) >= 0)
		{
			ret = true;
		}
	}
	return ret;
}

/*
 * It computes if a tuple is in the interval (for Cartesian Range Query of the R-tree), we compare only one coordinate.
 * The interval is defined with one coordinate of two n-tuples.
 */
template<class TTuple>
inline bool cMBRectangle<TTuple>::IsInInterval(const char* TTuple_t, unsigned int order1, const cSpaceDescriptor *sd1,
	char* cLNTuple_ql2, char* cLNTuple_qh2, unsigned int order2, const cSpaceDescriptor* sd2)
{
	bool ret = false;
	
	/*II_Compares++;
	if (TTuple::Equal(TTuple_t, order1, sd1, cLNTuple_ql2, order2, sd2) < 0 ||
		TTuple::Equal(TTuple_t, order1, sd1, cLNTuple_qh2, order2, sd2) > 0)
	{
		ret = false;
	}*/

	Computation_Compare++;
	if (TTuple::Equal(TTuple_t, order1, sd1, cLNTuple_ql2, order2, sd2) >= 0)
	{
		Computation_Compare++;
		if (TTuple::Equal(TTuple_t, order1, sd1, cLNTuple_qh2, order2, sd2) <= 0)
		{
			ret = true;
		}
	}
	return ret;
}

/**
* Modify MBR according to the tuple. The parameters include tuples in char* arrays.
*
* \param TTuple_ql Lower tuple of the MBR.
* \param TTuple_qh Higher tuple of the MBR.
* \param pSd SpaceDescriptor of the tuples.
* \return
*		- true if the MBR was modified,
*		- false otherwise.
*/
template<class TTuple>
inline bool cMBRectangle<TTuple>::ModifyMbr(char* TTuple_ql, char* TTuple_qh, const char* TTuple_t, const cSpaceDescriptor* pSd)
{
	bool modified = false;
	unsigned int dim = pSd->GetDimension();
	char typeCode = pSd->GetDimensionTypeCode(0);

	for (unsigned int i = 0 ; i < dim ; i++)
	{
		switch (pSd->GetDimensionTypeCode(i)) // !! Efficiency Warning !! */
		{
		case cInt::CODE: 
			//printf("ModifyMbr - Not Implemented"); //upraveno 
			break;
		case cNTuple::CODE:
			{
				char* ql = cHNTuple::GetPValue(TTuple_ql, i, pSd);
				char* qh = cHNTuple::GetPValue(TTuple_qh, i, pSd);
				char* tuple = cHNTuple::GetPValue(TTuple_t, i, pSd);
				if (cNTuple::Equal(ql, qh, pSd->GetDimSpaceDescriptor(i)) <= 0)
				{
					if (cNTuple::Equal(ql, tuple, pSd->GetDimSpaceDescriptor(i)) > 0)
					{
						cHNTuple::SetNTuple(TTuple_ql, i, TTuple_t, pSd);
						modified = true;
					}
					else if (cNTuple::Equal(qh, tuple, pSd->GetDimSpaceDescriptor(i)) < 0)
					{
						cHNTuple::SetNTuple(TTuple_qh, i, TTuple_t, pSd);
						modified = true;
					}
				}
				else
				{
					if (cNTuple::Equal(qh, tuple, pSd->GetDimSpaceDescriptor(i)) > 0)
					{
						cHNTuple::SetNTuple(TTuple_qh, i, TTuple_t, pSd);
						modified = true;
					}
					else if (cNTuple::Equal(ql, tuple, pSd->GetDimSpaceDescriptor(i)) < 0)
					{
						cHNTuple::SetNTuple(TTuple_ql, i, TTuple_t, pSd);
						modified = true;
					}
				}
			}
			break;
		case cUInt::CODE:
			if (TTuple::GetUInt(TTuple_ql, i, pSd) <= TTuple::GetUInt(TTuple_qh, i, pSd))
			{
				unsigned int uiValue = TTuple::GetUInt(TTuple_t, i, pSd);

				if (TTuple::GetUInt(TTuple_ql, i, pSd) > uiValue)
				{
					TTuple::SetValue(TTuple_ql, i, uiValue, pSd);
					modified = true;
				}
				else if (TTuple::GetUInt(TTuple_qh, i, pSd) < uiValue)
				{
					TTuple::SetValue(TTuple_qh, i, uiValue, pSd);
					modified = true;
				}
			}
			else
			{
				unsigned int uiValue = TTuple::GetUInt(TTuple_t, i, pSd);

				if (TTuple::GetUInt(TTuple_qh, i, pSd) > uiValue)
				{
					TTuple::SetValue(TTuple_qh, i, uiValue, pSd);
					modified = true;
				}
				else if (TTuple::GetUInt(TTuple_ql, i, pSd) < uiValue)
				{
					TTuple::SetValue(TTuple_ql, i, uiValue, pSd);
					modified = true;
				}
			}
			break;
		case cUShort::CODE:
			if (TTuple::GetUShort(TTuple_ql, i, pSd) <= TTuple::GetUShort(TTuple_qh, i, pSd))
			{
				unsigned short usValue = TTuple::GetUShort(TTuple_t, i, pSd);

				if (TTuple::GetUShort(TTuple_ql, i, pSd) > usValue)
				{
					TTuple::SetValue(TTuple_ql, i, usValue, pSd);
					modified = true;
				}
				else if (TTuple::GetUShort(TTuple_qh, i, pSd) < usValue)
				{
					TTuple::SetValue(TTuple_qh, i, usValue, pSd);
					modified = true;
				}
			}
			else
			{
				unsigned short usValue = TTuple::GetUShort(TTuple_t, i, pSd);

				if (TTuple::GetUShort(TTuple_qh, i, pSd) > usValue)
				{
					TTuple::SetValue(TTuple_qh, i, usValue, pSd);
					modified = true;
				}
				else if (TTuple::GetUShort(TTuple_ql, i, pSd) < usValue)
				{
					TTuple::SetValue(TTuple_ql, i, usValue, pSd);
					modified = true;
				}
			}
			break;
		case cChar::CODE:
			char bValue = TTuple::GetByte(TTuple_t, i, pSd);

			if (TTuple::GetByte(TTuple_ql, i, pSd) <= TTuple::GetByte(TTuple_qh, i, pSd))
			{
				if (TTuple::GetByte(TTuple_ql, i, pSd) > bValue)
				{
					TTuple::SetValue(TTuple_ql, i, bValue, pSd);
					modified = true;
				}
				else if (TTuple::GetByte(TTuple_qh, i, pSd) < bValue)
				{
					TTuple::SetValue(TTuple_qh, i, bValue, pSd);
					modified = true;
				}
			}
			else
			{
				if (TTuple::GetByte(TTuple_qh, i, pSd) > bValue)
				{
					TTuple::SetValue(TTuple_qh, i, bValue, pSd);
					modified = true;
				}
				else if (TTuple::GetByte(TTuple_ql, i, pSd) < bValue)
				{
					TTuple::SetValue(TTuple_ql, i, bValue, pSd);
					modified = true;
				}
			}
			break;
		}
	}

	return modified;
}

/**
* Modify MBR according to the tuple. The parameters include tuples in char* arrays.
*
* \param TTuple_ql Lower tuple of the MBR.
* \param TTuple_qh Higher tuple of the MBR.
* \param pSd SpaceDescriptor of the tuples.
* \return
*		- true if the MBR was modified,
*		- false otherwise.
*/
template<class TTuple>
inline bool cMBRectangle<TTuple>::ModifyMbr(char* cMbr_mbr, const char* TTuple_t, const cSpaceDescriptor* pSd)
{
	char* mbr_ql = GetLoTuple(cMbr_mbr);
	char* mbr_qh = GetHiTuple(cMbr_mbr, pSd);
	return ModifyMbr(mbr_ql, mbr_qh, TTuple_t, pSd);
}

template<class TTuple>
inline bool cMBRectangle<TTuple>::ModifyMbr(const char* TTuple_t, const cSpaceDescriptor* pSd)
{
	char* mbr_ql = GetLoTuple()->GetData();
	char* mbr_qh = GetHiTuple()->GetData();
	return ModifyMbr(mbr_ql, mbr_qh, TTuple_t, pSd);
}

/**
 * Copy the item into the char*.
 */
template<class TTuple>
void cMBRectangle<TTuple>::CopyTo(char* cMbr_item, const cSpaceDescriptor* pSd) const
{
	// Instance methods but working with char*
	mLoTuple.CopyTo(cMbr_item, pSd);
	mHiTuple.CopyTo(cMbr_item + mLoTuple.GetSize(pSd), pSd);
}

/**
 * Copy the item into the char*.
 */
template<class TTuple>
inline void cMBRectangle<TTuple>::CopyTo(char* cMbr_item, const cDTDescriptor* pDtD) const
{
	CopyTo(cMbr_item, (cSpaceDescriptor*)pDtD);
}

/**
 * Copy the src MBR to the dest MBR.
 */
template<class TTuple>
inline void cMBRectangle<TTuple>::Copy(char* cMbr_dest, const char* cMbr_src, const cSpaceDescriptor* pSd) 
{ 
	unsigned int len = GetMaxSize(cMbr_src , pSd);  //fk GetSize=>GetSize
	memcpy(cMbr_dest, cMbr_src, len); 
}

/**
 * Print Mbr
 */
//template<class TTuple>
//void cMBRectangle<TTuple>::Print(char* delim) const
//{
//	Print()mLowTuple.Print("x");
//	mHiTuple.Print("");
//	printf("%s", delim);
//}

/**
 * Print Mbr
 */
template<class TTuple>
void cMBRectangle<TTuple>::Print(const char* cMbr_mbr, const char* delim, const cSpaceDescriptor* pSd)
{
	TTuple::Print(GetLoTuple(cMbr_mbr), "x", pSd);
	TTuple::Print(GetHiTuple(cMbr_mbr, pSd), "", pSd);
	printf("%s", delim);
}
#include "cMBRectangle_SSE.h"
#include "cMBRectangle_AVX.h"
}}}
#endif