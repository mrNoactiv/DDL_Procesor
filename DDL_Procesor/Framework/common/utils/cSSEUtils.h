#ifdef SSE_ENABLED
/*!
 * \file cSSEUtils.h
 *
 * \author Pavel Bednar
 * \date sep 2011
 *
 * 
 */
#ifndef __cSSE_Utils_h__
#define __cSSE_Utils_h__


#include "common/cCommon.h"
#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "common/datatype/tuple/cTuple.h"

//SSE
#include <emmintrin.h>
#include <smmintrin.h>

//AVX
#include <immintrin.h>
#ifndef LINUX
	#include <intrin.h>
	//#define cpuid    __cpuid //  Gets info about CPU on windows
#else
	//void cpuid(int CPUInfo[4], int InfoType) { __asm__ __volatile__(	"cpuid": "=a" (CPUInfo[0]),"=b" (CPUInfo[1]),"=c" (CPUInfo[2]),"=d" (CPUInfo[3]) :"a" (InfoType));}
#endif

using namespace common::datatype;
using namespace common::datatype::tuple;


namespace common {
	namespace utils {

// for finding the lowest 1-bit
static const char ReverseLogTable256[256] = 
{
-1, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1
, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2,
0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1
, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 7, 0, 1, 0, 2,
0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1
, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2,
0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0 
};
static const char InvertList1BitTable[256][9] =
{
	{0,0,0,0,0,0,0,0,0},	{1,0,0,0,0,0,0,0,0},	{1,1,0,0,0,0,0,0,0},	{2,0,1,0,0,0,0,0,0},	{1,2,0,0,0,0,0,0,0},	{2,0,2,0,0,0,0,0,0},	{2,1,2,0,0,0,0,0,0},
	{3,0,1,2,0,0,0,0,0},	{1,3,0,0,0,0,0,0,0},	{2,0,3,0,0,0,0,0,0},	{2,1,3,0,0,0,0,0,0},	{3,0,1,3,0,0,0,0,0},	{2,2,3,0,0,0,0,0,0},	{3,0,2,3,0,0,0,0,0},
	{3,1,2,3,0,0,0,0,0},	{4,0,1,2,3,0,0,0,0},	{1,4,0,0,0,0,0,0,0},	{2,0,4,0,0,0,0,0,0},	{2,1,4,0,0,0,0,0,0},	{3,0,1,4,0,0,0,0,0},	{2,2,4,0,0,0,0,0,0},
	{3,0,2,4,0,0,0,0,0},	{3,1,2,4,0,0,0,0,0},	{4,0,1,2,4,0,0,0,0},	{2,3,4,0,0,0,0,0,0},	{3,0,3,4,0,0,0,0,0},	{3,1,3,4,0,0,0,0,0},	{4,0,1,3,4,0,0,0,0},
	{3,2,3,4,0,0,0,0,0},	{4,0,2,3,4,0,0,0,0},	{4,1,2,3,4,0,0,0,0},	{5,0,1,2,3,4,0,0,0},	{1,5,0,0,0,0,0,0,0},	{2,0,5,0,0,0,0,0,0},	{2,1,5,0,0,0,0,0,0},
	{3,0,1,5,0,0,0,0,0},	{2,2,5,0,0,0,0,0,0},	{3,0,2,5,0,0,0,0,0},	{3,1,2,5,0,0,0,0,0},	{4,0,1,2,5,0,0,0,0},	{2,3,5,0,0,0,0,0,0},	{3,0,3,5,0,0,0,0,0},
	{3,1,3,5,0,0,0,0,0},	{4,0,1,3,5,0,0,0,0},	{3,2,3,5,0,0,0,0,0},	{4,0,2,3,5,0,0,0,0},	{4,1,2,3,5,0,0,0,0},	{5,0,1,2,3,5,0,0,0},	{2,4,5,0,0,0,0,0,0},
	{3,0,4,5,0,0,0,0,0},	{3,1,4,5,0,0,0,0,0},	{4,0,1,4,5,0,0,0,0},	{3,2,4,5,0,0,0,0,0},	{4,0,2,4,5,0,0,0,0},	{4,1,2,4,5,0,0,0,0},	{5,0,1,2,4,5,0,0,0},
	{3,3,4,5,0,0,0,0,0},	{4,0,3,4,5,0,0,0,0},	{4,1,3,4,5,0,0,0,0},	{5,0,1,3,4,5,0,0,0},	{4,2,3,4,5,0,0,0,0},	{5,0,2,3,4,5,0,0,0},	{5,1,2,3,4,5,0,0,0},
	{6,0,1,2,3,4,5,0,0},	{1,6,0,0,0,0,0,0,0},	{2,0,6,0,0,0,0,0,0},	{2,1,6,0,0,0,0,0,0},	{3,0,1,6,0,0,0,0,0},	{2,2,6,0,0,0,0,0,0},	{3,0,2,6,0,0,0,0,0},
	{3,1,2,6,0,0,0,0,0},	{4,0,1,2,6,0,0,0,0},	{2,3,6,0,0,0,0,0,0},	{3,0,3,6,0,0,0,0,0},	{3,1,3,6,0,0,0,0,0},	{4,0,1,3,6,0,0,0,0},	{3,2,3,6,0,0,0,0,0},
	{4,0,2,3,6,0,0,0,0},	{4,1,2,3,6,0,0,0,0},	{5,0,1,2,3,6,0,0,0},	{2,4,6,0,0,0,0,0,0},	{3,0,4,6,0,0,0,0,0},	{3,1,4,6,0,0,0,0,0},	{4,0,1,4,6,0,0,0,0},
	{3,2,4,6,0,0,0,0,0},	{4,0,2,4,6,0,0,0,0},	{4,1,2,4,6,0,0,0,0},	{5,0,1,2,4,6,0,0,0},	{3,3,4,6,0,0,0,0,0},	{4,0,3,4,6,0,0,0,0},	{4,1,3,4,6,0,0,0,0},
	{5,0,1,3,4,6,0,0,0},	{4,2,3,4,6,0,0,0,0},	{5,0,2,3,4,6,0,0,0},	{5,1,2,3,4,6,0,0,0},	{6,0,1,2,3,4,6,0,0},	{2,5,6,0,0,0,0,0,0},	{3,0,5,6,0,0,0,0,0},
	{3,1,5,6,0,0,0,0,0},	{4,0,1,5,6,0,0,0,0},	{3,2,5,6,0,0,0,0,0},	{4,0,2,5,6,0,0,0,0},	{4,1,2,5,6,0,0,0,0},	{5,0,1,2,5,6,0,0,0},	{3,3,5,6,0,0,0,0,0},
	{4,0,3,5,6,0,0,0,0},	{4,1,3,5,6,0,0,0,0},	{5,0,1,3,5,6,0,0,0},	{4,2,3,5,6,0,0,0,0},	{5,0,2,3,5,6,0,0,0},	{5,1,2,3,5,6,0,0,0},	{6,0,1,2,3,5,6,0,0},
	{3,4,5,6,0,0,0,0,0},	{4,0,4,5,6,0,0,0,0},	{4,1,4,5,6,0,0,0,0},	{5,0,1,4,5,6,0,0,0},	{4,2,4,5,6,0,0,0,0},	{5,0,2,4,5,6,0,0,0},	{5,1,2,4,5,6,0,0,0},
	{6,0,1,2,4,5,6,0,0},	{4,3,4,5,6,0,0,0,0},	{5,0,3,4,5,6,0,0,0},	{5,1,3,4,5,6,0,0,0},	{6,0,1,3,4,5,6,0,0},	{5,2,3,4,5,6,0,0,0},	{6,0,2,3,4,5,6,0,0},
	{6,1,2,3,4,5,6,0,0},	{7,0,1,2,3,4,5,6,0},	{1,7,0,0,0,0,0,0,0},	{2,0,7,0,0,0,0,0,0},	{2,1,7,0,0,0,0,0,0},	{3,0,1,7,0,0,0,0,0},	{2,2,7,0,0,0,0,0,0},
	{3,0,2,7,0,0,0,0,0},	{3,1,2,7,0,0,0,0,0},	{4,0,1,2,7,0,0,0,0},	{2,3,7,0,0,0,0,0,0},	{3,0,3,7,0,0,0,0,0},	{3,1,3,7,0,0,0,0,0},	{4,0,1,3,7,0,0,0,0},
	{3,2,3,7,0,0,0,0,0},	{4,0,2,3,7,0,0,0,0},	{4,1,2,3,7,0,0,0,0},	{5,0,1,2,3,7,0,0,0},	{2,4,7,0,0,0,0,0,0},	{3,0,4,7,0,0,0,0,0},	{3,1,4,7,0,0,0,0,0},
	{4,0,1,4,7,0,0,0,0},	{3,2,4,7,0,0,0,0,0},	{4,0,2,4,7,0,0,0,0},	{4,1,2,4,7,0,0,0,0},	{5,0,1,2,4,7,0,0,0},	{3,3,4,7,0,0,0,0,0},	{4,0,3,4,7,0,0,0,0},
	{4,1,3,4,7,0,0,0,0},	{5,0,1,3,4,7,0,0,0},	{4,2,3,4,7,0,0,0,0},	{5,0,2,3,4,7,0,0,0},	{5,1,2,3,4,7,0,0,0},	{6,0,1,2,3,4,7,0,0},	{2,5,7,0,0,0,0,0,0},
	{3,0,5,7,0,0,0,0,0},	{3,1,5,7,0,0,0,0,0},	{4,0,1,5,7,0,0,0,0},	{3,2,5,7,0,0,0,0,0},	{4,0,2,5,7,0,0,0,0},	{4,1,2,5,7,0,0,0,0},	{5,0,1,2,5,7,0,0,0},
	{3,3,5,7,0,0,0,0,0},	{4,0,3,5,7,0,0,0,0},	{4,1,3,5,7,0,0,0,0},	{5,0,1,3,5,7,0,0,0},	{4,2,3,5,7,0,0,0,0},	{5,0,2,3,5,7,0,0,0},	{5,1,2,3,5,7,0,0,0},
	{6,0,1,2,3,5,7,0,0},	{3,4,5,7,0,0,0,0,0},	{4,0,4,5,7,0,0,0,0},	{4,1,4,5,7,0,0,0,0},	{5,0,1,4,5,7,0,0,0},	{4,2,4,5,7,0,0,0,0},	{5,0,2,4,5,7,0,0,0},
	{5,1,2,4,5,7,0,0,0},	{6,0,1,2,4,5,7,0,0},	{4,3,4,5,7,0,0,0,0},	{5,0,3,4,5,7,0,0,0},	{5,1,3,4,5,7,0,0,0},	{6,0,1,3,4,5,7,0,0},	{5,2,3,4,5,7,0,0,0},
	{6,0,2,3,4,5,7,0,0},	{6,1,2,3,4,5,7,0,0},	{7,0,1,2,3,4,5,7,0},	{2,6,7,0,0,0,0,0,0},	{3,0,6,7,0,0,0,0,0},	{3,1,6,7,0,0,0,0,0},	{4,0,1,6,7,0,0,0,0},
	{3,2,6,7,0,0,0,0,0},	{4,0,2,6,7,0,0,0,0},	{4,1,2,6,7,0,0,0,0},	{5,0,1,2,6,7,0,0,0},	{3,3,6,7,0,0,0,0,0},	{4,0,3,6,7,0,0,0,0},	{4,1,3,6,7,0,0,0,0},
	{5,0,1,3,6,7,0,0,0},	{4,2,3,6,7,0,0,0,0},	{5,0,2,3,6,7,0,0,0},	{5,1,2,3,6,7,0,0,0},	{6,0,1,2,3,6,7,0,0},	{3,4,6,7,0,0,0,0,0},	{4,0,4,6,7,0,0,0,0},
	{4,1,4,6,7,0,0,0,0},	{5,0,1,4,6,7,0,0,0},	{4,2,4,6,7,0,0,0,0},	{5,0,2,4,6,7,0,0,0},	{5,1,2,4,6,7,0,0,0},	{6,0,1,2,4,6,7,0,0},	{4,3,4,6,7,0,0,0,0},
	{5,0,3,4,6,7,0,0,0},	{5,1,3,4,6,7,0,0,0},	{6,0,1,3,4,6,7,0,0},	{5,2,3,4,6,7,0,0,0},	{6,0,2,3,4,6,7,0,0},	{6,1,2,3,4,6,7,0,0},	{7,0,1,2,3,4,6,7,0},
	{3,5,6,7,0,0,0,0,0},	{4,0,5,6,7,0,0,0,0},	{4,1,5,6,7,0,0,0,0},	{5,0,1,5,6,7,0,0,0},	{4,2,5,6,7,0,0,0,0},	{5,0,2,5,6,7,0,0,0},	{5,1,2,5,6,7,0,0,0},
	{6,0,1,2,5,6,7,0,0},	{4,3,5,6,7,0,0,0,0},	{5,0,3,5,6,7,0,0,0},	{5,1,3,5,6,7,0,0,0},	{6,0,1,3,5,6,7,0,0},	{5,2,3,5,6,7,0,0,0},	{6,0,2,3,5,6,7,0,0},
	{6,1,2,3,5,6,7,0,0},	{7,0,1,2,3,5,6,7,0},	{4,4,5,6,7,0,0,0,0},	{5,0,4,5,6,7,0,0,0},	{5,1,4,5,6,7,0,0,0},	{6,0,1,4,5,6,7,0,0},	{5,2,4,5,6,7,0,0,0},
	{6,0,2,4,5,6,7,0,0},	{6,1,2,4,5,6,7,0,0},	{7,0,1,2,4,5,6,7,0},	{5,3,4,5,6,7,0,0,0},	{6,0,3,4,5,6,7,0,0},	{6,1,3,4,5,6,7,0,0},	{7,0,1,3,4,5,6,7,0},
	{6,2,3,4,5,6,7,0,0},	{7,0,2,3,4,5,6,7,0},	{7,1,2,3,4,5,6,7,0},	{8,0,1,2,3,4,5,6,7}
};

class cSSEUtils
{
private: 
	static inline char Lowest1Bit(ushort value);

public:
	cSSEUtils();
	~cSSEUtils();
	template <class T> static inline void InitializeQueryRegisters(__m128i &sse_ql,  __m128i &sse_qh,const T *p_ql, const T* p_qh,const unsigned int dim);
	template <class T> static inline void InitializeQueryRegister(__m128i &sse_register,  const T *p_ql, const unsigned int dim);
	template <class T> static int FindItem(const T &searchItem, __m128i &sse_searchItem, const T *inputArray,const unsigned int arraySize);
	// template <class T> static int FindItem(const T &searchItem, __m128i &sse_register, const T *inputArray, const unsigned int arraySize);
	template <class T> static int FindItem(const T searchItem, const T *inputArray,const unsigned int arraySize,const T secondarySearchItem, unsigned int* foundIndices,unsigned int &foundCount );
	static inline int SearchText(const char mainChar, const char secondaryChar, const __m128i &sse_mainChar, const __m128i &sse_secondaryChar, const char *inputArray, const unsigned int arraySize, unsigned int* foundIndices, unsigned int &foundCount);
	static inline int SearchText_sigmod(const __m128i &sse_mainChar, const __m128i &sse_secondaryChar, const char *inputArray, const unsigned int arraySize, unsigned int* foundIndices, unsigned int &foundCount);
	static inline short GetPackCount(size_t size);
	static inline short GetTruePosition(const int result, const short packCount);
	static inline short GetTruePosition_pc16(const int result); //PackCount=16
	static inline short GetTruePosition_pc8(const int result); //PackCount=8
	static inline short GetTruePosition_pc4(const int result); //PackCount=4
	static inline short GetTruePosition_pc2(const int result); //PackCount=2

	static inline void GetBinaryArray(const short result, const short packCount, bool* outBinArray);
	static inline void GetBinaryArray_pc16(const short result, bool* outBinArray); //PackCount=16
	static inline int  GetBinaryArray_sigmod(const short result, short* outBinArray);
	static inline void GetBinaryArray_pc8(const short result, bool* outBinArray); //PackCount=8
	static inline void GetBinaryArray_pc4(const short result, bool* outBinArray); //PackCount=4
	static inline void GetBinaryArray_pc2(const short result, bool* outBinArray); //PackCount=2
	static inline int atoi_sse(const char *str);
	//static inline void PrintCpuInfo();
	};

template <class T>
void cSSEUtils::InitializeQueryRegisters(__m128i &sse_ql, __m128i &sse_qh,const T *p_ql, const T* p_qh,const unsigned int dim)
{
	InitializeQueryRegister(sse_ql,p_ql,dim);
	InitializeQueryRegister(sse_qh,p_qh,dim);
}

template <class T>
void cSSEUtils::InitializeQueryRegister(__m128i &sse_register, const T *queryElement, const unsigned int dim)
{
#ifndef LINUX
	__declspec(align(16)) char *pqe = (T*)(queryElement);
#else
	char *pqe  __attribute__((aligned(16)))  = (T*)(queryElement);
#endif
	const short maxNumbersInRegister = 128 / (sizeof(T)*8);

	if (dim > maxNumbersInRegister)
	{
		printf("\nCritical Error. cSSEUtils::InicializeSSERegistry(): Dimension is to high to store in SSE register.");
		exit(0);
	}

	T tmpElement[maxNumbersInRegister];
	switch (dim)
	{
	case maxNumbersInRegister:
		sse_register = _mm_loadu_si128((__m128i*)pqe);
		break;
	default:
		short position = 0;
		for (int i=0;i<maxNumbersInRegister;i+=dim)
		{
			tmpElement[position] = *(pqe);
			position++;
		}
		sse_register =_mm_loadu_si128( (__m128i*)tmpElement);
		break;
	}
}

template <class T>
int cSSEUtils::FindItem(const T &searchItem, __m128i &sse_searchItem, const T *inputArray, const unsigned int arraySize)
{
	int result = -1 ;
	__m128i sse_bucket, sse_result;
	const short packCount = GetPackCount(sizeof(T));
	int maxPackCount = (arraySize / packCount) * packCount;

	int i;
	for (i = 0 ; i < maxPackCount ; i += packCount)
	{
		sse_bucket = _mm_loadu_si128((__m128i*)(inputArray + i));
		switch (packCount)
		{
			case 2:
				sse_result = _mm_cmpeq_epi64(sse_bucket, sse_searchItem); break;
			case 4:
				sse_result = _mm_cmpeq_epi32(sse_bucket, sse_searchItem); break;
			case 8:
				sse_result = _mm_cmpeq_epi16(sse_bucket, sse_searchItem); break;
			case 16:
				sse_result = _mm_cmpeq_epi8(sse_bucket, sse_searchItem); break;
			default:
				printf("\nCritical Error: cSSEUtils::FindItem(). Invalid pack count.");
				break;
		}

		if ((result = _mm_movemask_epi8(sse_result)) != 0)
		{
			return i + GetTruePosition(result, packCount);				
		} else
		{
			result = -1;
		}
	}

	// process the rest
	for ( ; i < arraySize; i++)
	{
		if (inputArray[i] == searchItem)
		{
			return i;
		}
	}
	return result;
}

template <class T>
int cSSEUtils::FindItem(const T searchItem, const T *inputArray,const unsigned int arraySize,const T secondarySearchItem, unsigned int* foundIndices,unsigned int &foundCount )
{
	foundCount=0;
	int result = -1 ;
	short result2 = -1 ;
	const short packCount = GetPackCount(sizeof(T));
	bool outBinArray[16]; // temporary binary array, the max length is 16 (for 16x byte)
	__m128i sse_searchItem, sse_bucket,sse_result;
	__m128i sse_searchItem2,sse_result2;
	
	InitializeQueryRegister<T>(sse_searchItem,&searchItem,1);
	InitializeQueryRegister<T>(sse_searchItem2,&secondarySearchItem,1);

	int i;
	for (i=0;i<arraySize / packCount;i++)
	{
		sse_bucket = _mm_loadu_si128((__m128i*)(inputArray + i*packCount));
		switch (packCount)
		{
		case 2:
			sse_result = _mm_cmpeq_epi64(sse_bucket,sse_searchItem);
			sse_result2 = _mm_cmpeq_epi64(sse_bucket,sse_searchItem2);
			break;
		case 4:
			sse_result = _mm_cmpeq_epi32(sse_bucket,sse_searchItem);
			sse_result2 = _mm_cmpeq_epi32(sse_bucket,sse_searchItem2);
			break;
		case 8:
			sse_result = _mm_cmpeq_epi16(sse_bucket,sse_searchItem);
			sse_result2 = _mm_cmpeq_epi16(sse_bucket,sse_searchItem2);
			break;
		case 16:
			sse_result = _mm_cmpeq_epi8(sse_bucket,sse_searchItem);
			sse_result2 = _mm_cmpeq_epi8(sse_bucket,sse_searchItem2);
			break;
		default:
			printf("\nCritical Error: cSSEUtils::FindItem(). Invalid pack count.");
			break;
		}
		result = _mm_movemask_epi8(sse_result);
		if (result != 0)
		{
			result = i * packCount + GetTruePosition(result,packCount);				
		}
		else
		{
			result = -1;
		}

		result2 = _mm_movemask_epi8(sse_result2);
		if (result2 != 0) //found at least one secondary search item
		{
			GetBinaryArray(result2, packCount, outBinArray); //array where secondary search item is true
			for (int j=0;j<packCount;j++)
			{
				if (outBinArray[j])
				{
					int foundIndex = i * packCount + j;
					if (result != -1 && foundIndex > result)
					{
						// the secondary symbol is after the primary symbol found => finish searching
						return result;
					} else
					{
						foundIndices[foundCount] = foundIndex;
						foundCount++;
					}
				}
			}
		}

		if (result != -1)
		{
			return result;
		}
	}
	//the rest
	i++;
	for (int j = i * packCount; j < arraySize; j++)
	{
		if (inputArray[j] == searchItem)
		{
			return j;
		}
		if (inputArray[j == secondarySearchItem])
		{
			foundIndices[foundCount] = j ;
			foundCount++;
		}
	}
	return result;
}

int cSSEUtils::SearchText(const char mainChar, const char secondaryChar, const __m128i &sse_mainChar, const __m128i &sse_secondaryChar, const char *inputArray, const unsigned int arraySize, unsigned int* foundIndices, unsigned int &foundCount)
{
	foundCount = 0;
	int result = -1;
	__m128i sse_bucket, sse_result;
	const short packCount = GetPackCount(sizeof(char));
	int maxPackCount = (arraySize / packCount) * packCount;

	int i;
	for (i = 0; i < maxPackCount; i += packCount)
	{
		sse_bucket = _mm_loadu_si128((__m128i*)(i + inputArray));
		sse_result = _mm_cmpeq_epi8(sse_bucket, sse_mainChar);

		result = _mm_movemask_epi8(sse_result);
		if (result != 0)
		{
			result = i + GetTruePosition(result, packCount);
		}
		else
		{
			result = -1;
		}

		sse_result = _mm_cmpeq_epi8(sse_bucket, sse_secondaryChar);
		ushort result2 = (ushort)_mm_movemask_epi8(sse_result);

		if (result2 != 0)
		{
			// process both bytes of the 16bit result
			for (uint j = 0; j < 2; j++)
			{
				char* array;
				int indexOffset;

				if (j == 0)
				{
					array = (char*)InvertList1BitTable[(uchar)result2];
					indexOffset = i;
				}
				else
				{
					array = (char*)InvertList1BitTable[*(((uchar*)&result2) + 1)];
					indexOffset = i + 8;
				}

				int len = array[0];
				if (len > 0)
				{
					for (int k = 1; k <= len; k++)
					{
						int foundIndex = indexOffset + array[k];

						if (result != -1 && foundIndex > result)
						{
							// the secondary symbol is after the primary symbol found => finish searching
							return result;
						}
						else
						{
							foundIndices[foundCount++] = foundIndex;
						}
					}
				}
			}
		}

		if (result != -1)
		{
			return result;
		}
	}

	// do the rest
	for (; i < arraySize; i++)
	{
		if (inputArray[i] == mainChar)
		{
			return i;
		}
		if (inputArray[i] == secondaryChar)
		{
			foundIndices[foundCount++] = i;
		}
	}
	return result;
}
int cSSEUtils::SearchText_sigmod(const __m128i &sse_mainChar, const __m128i &sse_secondaryChar, const char *inputArray, const unsigned int arraySize, unsigned int* foundIndices, unsigned int &foundCount)
{
	return SearchText('\n', '|', sse_mainChar, sse_secondaryChar, inputArray, arraySize, foundIndices, foundCount);
}

short cSSEUtils::GetPackCount(size_t size)
{
	return 128 / (size * 8);
}

short cSSEUtils::GetTruePosition(const int result, const short packCount)
{
	switch (packCount)
	{
	case 16:
		return GetTruePosition_pc16(result);
	case 8:
		return GetTruePosition_pc8(result);
	case 4:
		return GetTruePosition_pc4(result);
	case 2:
		return GetTruePosition_pc2(result);
		break;
	default:
		return 0;
	}
}

/**
 * Fest decoding algorithm, it is necessary to rewrite it for others :GetTruePosition_pc* algorithms.
 */
short cSSEUtils::GetTruePosition_pc16(const int value)
{
	return (short)cSSEUtils::Lowest1Bit((ushort)value);

	/*
	short len2;

	if ((value & 1) != 0)
		len2 = 0;
	else if ((value & 2) != 0)
		len2 = 1;
	else if ((value & 4) != 0)
		len2 = 2;
	else if ((value & 8) != 0)
		len2 = 3;
	else if ((value & 16) != 0)
		len2 = 4;
	else if ((value & 32) != 0)
		len2 = 5;
	else if ((value & 64) != 0)
		len2 = 6;
	else if ((value & 128) != 0)
		len2 = 7;
	else if ((value & 256) != 0)
		len2 = 8;
	else if ((value & 512) != 0)
		len2 = 9;
	else if ((value & 1024) != 0)
		len2 = 10;
	else if ((value & 2048) != 0)
		len2 = 11;
	else if ((value & 4096) != 0)
		return 12;
	else if ((value & 8192) != 0)
		len2 = 13;
	else if ((value & 16384) != 0)
		len2 = 14;
	else if ((value & 32768) != 0)
		len2 = 15;
	else 
		len2 = -1;

	if (len1 != len2)
	{
		int bla = 0;
	}

	return len2;*/
}

inline char cSSEUtils::Lowest1Bit(ushort value)
{
	uchar loByte = (uchar)value;
	char len = ReverseLogTable256[loByte];

	if (len == -1)
	{
		uchar hiByte = (uchar)(value >> 8);
		len = ReverseLogTable256[hiByte];

		if (len != -1)
		{
			len += 8;
		}
	}

	return len;
}

short cSSEUtils::GetTruePosition_pc8(const int result)
{
	if ((result & 3) != 0)
		return 0;
	else if ((result & 12) != 0)
		return 1;
	else if ((result & 48) != 0)
		return 2;
	else if ((result & 192) != 0)
		return 3;
	else if ((result & 768) != 0)
		return 4;
	else if ((result & 3072) != 0)
		return 5;
	else if ((result & 12288) != 0)
		return 6;
	else if ((result & 49152) != 0)
		return 7;
	else 
		return -1;
}
short cSSEUtils::GetTruePosition_pc4(const int result)
{
	if ((result & 15) != 0)
		return 0;
	else if ((result & 240) != 0)
		return 1;
	else if ((result & 3840) != 0)
		return 2;
	else if ((result & 61440) != 0)
		return 3;
}
short cSSEUtils::GetTruePosition_pc2(const int result)
{
	if ((result & 255) != 0)
		return 0;
	else if ((result & 65535) != 0)
		return 1;
}

void cSSEUtils::GetBinaryArray(const short result, const short packCount, bool* outBinArray)
{
	switch (packCount)
	{
	case 16:
		GetBinaryArray_pc16(result, outBinArray);
		break;
	case 8:
		GetBinaryArray_pc8(result, outBinArray);
		break;
	case 4:
		GetBinaryArray_pc4(result, outBinArray);
		break;
	case 2:
		GetBinaryArray_pc2(result, outBinArray);
		break;
	}
}
int cSSEUtils::GetBinaryArray_sigmod(const short result, short* outBinArray)
{
	int cnt = 0;
	if ((result & 1 )!= 0) outBinArray[cnt++] = 0;
	if ((result & 2 )!= 0) outBinArray[cnt++] = 1;
	if ((result & 4 )!= 0) outBinArray[cnt++] = 2;
	if ((result & 8 )!= 0) outBinArray[cnt++] = 3;
	if ((result & 16 )!= 0) outBinArray[cnt++] = 4;
	if ((result & 32 )!= 0)	outBinArray[cnt++] = 5;
	if ((result & 64 )!= 0)	outBinArray[cnt++] = 6;
	if ((result & 128 )!= 0) outBinArray[cnt++] = 7;
	if ((result & 256 )!= 0) outBinArray[cnt++] = 8;
	if ((result & 512 )!= 0) outBinArray[cnt++] = 9;
	if ((result & 1024 )!= 0) outBinArray[cnt++] = 10;
	if ((result & 2048 )!= 0) outBinArray[cnt++] = 11;
	if ((result & 4096 )!= 0) outBinArray[cnt++] = 12;
	if ((result & 8195 )!= 0) outBinArray[cnt++] = 13;
	if ((result & 16384 )!= 0) outBinArray[cnt++] = 14;
	if ((result & 32768 )!= 0) outBinArray[cnt++] = 15;
	return cnt;
}
void cSSEUtils::GetBinaryArray_pc16(const short result, bool* outBinArray)
{
	outBinArray[0] = (result & 1) != 0;
	outBinArray[1] = (result & 2) != 0;
	outBinArray[2] = (result & 4) != 0;
	outBinArray[3] = (result & 8) != 0;
	outBinArray[4] = (result & 16) != 0;
	outBinArray[5] = (result & 32) != 0;
	outBinArray[6] = (result & 64) != 0;
	outBinArray[7] = (result & 128) != 0;
	outBinArray[8] = (result & 256) != 0;
	outBinArray[9] = (result & 512) != 0;
	outBinArray[10] = (result & 1024) != 0;
	outBinArray[11] = (result & 2048) != 0;
	outBinArray[12] = (result & 4096) != 0;
	outBinArray[13] = (result & 8192) != 0;
	outBinArray[14] = (result & 16384) != 0;
	outBinArray[15] = (result & 32768) != 0;

	/*
	bool out[16] = {
		(result & 1) != 0,
		(result & 2) != 0,
		(result & 4) != 0,
		(result & 8) != 0,
		(result & 16) != 0,
		(result & 32) != 0,
		(result & 64) != 0,
		(result & 128) != 0,
		(result & 256) != 0,
		(result & 512) != 0,
		(result & 1024) != 0,
		(result & 2048) != 0,
		(result & 4096) != 0,
		(result & 8192) != 0,
		(result & 16384) != 0,
		(result & 32768) != 0
	};
	return out;*/
}
void cSSEUtils::GetBinaryArray_pc8(const short result, bool* outBinArray)
{
	outBinArray[0] = (result & 3) != 0;
	outBinArray[1] = (result & 12) != 0;
	outBinArray[2] = (result & 48) != 0;
	outBinArray[3] = (result & 192) != 0;
	outBinArray[4] = (result & 768) != 0;
	outBinArray[5] = (result & 3072) != 0;
	outBinArray[6] = (result & 12288) != 0;
	outBinArray[7] = (result & 49152) != 0;

	/*
	bool out[8] = {
		(result & 3) != 0,
		(result & 12) != 0,
		(result & 48) != 0,
		(result & 192) != 0,
		(result & 768) != 0,
		(result & 3072) != 0,
		(result & 12288) != 0,
		(result & 49152) != 0
	};
	return out;*/
}
void cSSEUtils::GetBinaryArray_pc4(const short result, bool* outBinArray)
{
	outBinArray[0] = (result & 15) != 0;
	outBinArray[1] = (result & 240) != 0;
	outBinArray[2] = (result & 3840) != 0;
	outBinArray[3] = (result & 61440) != 0;

	/*
	bool out[4] = {
		(result & 15) != 0,
		(result & 240) != 0,
		(result & 3840) != 0,
		(result & 61440) != 0
	};
	return out;*/
}
void cSSEUtils::GetBinaryArray_pc2(const short result, bool* outBinArray)
{
	outBinArray[0] = (result & 255) != 0;
	outBinArray[1] = (result & 65535) != 0;
	/*
	bool out[2] = {
		(result & 255) != 0,
		(result & 65535) != 0
	};
	return out;*/
}
int cSSEUtils::atoi_sse(const char *str)
{
	size_t len = strlen(str);
	int value = 0;

	int sign = 1;
	if (str[0] == '-') // handle negative
	{ 
		sign = -1;
		++str;
		--len;
	}
	const short packCount = 4;
	char tmpArray[packCount];
	const short cycles =  (len-1) / packCount + 1; //to avoid expensive modulo
	__m128i multiplier = _mm_set_epi32(1,10,100,1000);
	__m128i multiplicand;
	__m128i partialResult;
	short remainingDim = len;
	short charPosition = len-1;

	short numbersInRegister;
	for (int c = 0; c<cycles;c++)
	{
		if (c>0)
		{
			multiplier = _mm_mullo_epi32(multiplier,_mm_set1_epi32(10000)); //multiplies previous values for next cycle.
		}
		if (remainingDim >= packCount) //determine how many significant numbers is in the register
		{
			numbersInRegister = packCount;
			remainingDim -= packCount;
		}
		else
		{
			numbersInRegister = remainingDim;
		}
		for (int p=packCount-1;p>=0;p--)
		{
			if (p >= packCount-numbersInRegister)
			{
				tmpArray[p] = str[charPosition];
				charPosition--;
			}
			else
				tmpArray[p] = '0';
		}
		multiplicand = _mm_sub_epi32(_mm_set_epi32(tmpArray[3],tmpArray[2],tmpArray[1],tmpArray[0]),_mm_set1_epi32('0')); //loads SSE register and for each item substracts char '0'
		partialResult = _mm_mullo_epi32(multiplicand,multiplier); //multiplication
		partialResult = _mm_hadd_epi32(partialResult, partialResult); //sum two neighboring numbers two times produces sum of 4 numbers in register.
		partialResult = _mm_hadd_epi32(partialResult, partialResult); 
		value += _mm_extract_epi32(partialResult,0); //extract item in 0th position
	}
	return value;
}
/*
casy pro 100 000 000 volani - ctyrmistné cislo (1cyklus pro SSE)
3.279 (3.29162 (3.29162+0))s - atoi
1.344 (1.34161 (1.34161+0))s - cSSEUtils::atoi_sse
1.143 (1.13881 (1.13881+0))s - cSSEUtils::atoi_sse2
0.821 (0.826805 (0.826805+0))s - cNumber::atoi_ffast

static const __m128i mSseAtoiBase1 = _mm_set_epi32(1, 10, 100, 1000);
static const __m128i mSseAtoiBase2 = _mm_set_epi32(10000, 100000, 1000000, 10000000);
static const __m128i mSseAtoiBase3 = _mm_set_epi32(100000000, 1000000000, 0, 0);
static const __m128i mSseAtoiChars = _mm_set1_epi32('0');
*/
//int cSSEUtils::atoi_sse2(const char *str)
//{
//	size_t len = strlen(str);
//	int value = 0;
//
//	int sign = 1;
//	if (str[0] == '-') // handle negative
//	{ 
//		sign = -1;
//		++str;
//		--len;
//	}
//	const short packCount = 4;
//	const short cycles =  (len-1) / packCount + 1; //to avoid expensive modulo
//	__m128i multiplier;
//	__m128i multiplicand;
//	__m128i partialResult;
//	short remainingDim = len;
//	short charPosition = len-1;
//
//	short numbersInRegister;
//	for (int c = 0; c<cycles;c++)
//	{
//		if (c==0) multiplier = mSseAtoiBase1;
//		if (c==1) multiplier = mSseAtoiBase2;
//		if (c==2) multiplier = mSseAtoiBase3;
//		
//		if (remainingDim >= packCount) //determine how many significant numbers is in the register
//		{
//			numbersInRegister = packCount;
//			remainingDim -= packCount;
//		}
//		else
//		{
//			numbersInRegister = remainingDim;
//		}
//		
//		const char* charReverseIndices = InvertedCharArray[numbersInRegister];
//		//multiplicand = _mm_sub_epi32(_mm_set_epi32(str[charReverseIndices[0]],str[charReverseIndices[1]],str[charReverseIndices[2]],str[charReverseIndices[3]]),mSseAtoiChars); //loads SSE register and for each item substracts char '0'
//		if (numbersInRegister == 4)
//			multiplicand = _mm_sub_epi32(_mm_set_epi32(str[3],str[2],str[1],str[0]),mSseAtoiChars); //loads SSE register and for each item substracts char '0'
//		else if (numbersInRegister == 3)
//			multiplicand = _mm_sub_epi32(_mm_set_epi32(str[2],str[1],str[0],'0'),mSseAtoiChars); //loads SSE register and for each item substracts char '0'
//		else if (numbersInRegister == 2)
//			multiplicand = _mm_sub_epi32(_mm_set_epi32(str[1],str[0],'0','0'),mSseAtoiChars); //loads SSE register and for each item substracts char '0'
//		else if (numbersInRegister == 1)
//			multiplicand = _mm_sub_epi32(_mm_set_epi32(str[0],'0','0','0'),mSseAtoiChars); //loads SSE register and for each item substracts char '0'
//		partialResult = _mm_mullo_epi32(multiplicand,multiplier); //multiplication
//		partialResult = _mm_hadd_epi32(partialResult, partialResult); //sum two neighboring numbers two times produces sum of 4 numbers in register.
//		partialResult = _mm_hadd_epi32(partialResult, partialResult); //sum two neighboring numbers two times produces sum of 4 numbers in register.
//		value += _mm_extract_epi32(partialResult,0); //extract item in 0th position
//	}
//	return value;
//}
/*
void cSSEUtils::PrintCpuInfo()
{
	int info[4];
	cpuid(info, 0);
	int nIds = info[0];

	cpuid(info, 0x80000000);
	int nExIds = info[0];

	//  Detect Instruction Set
	if (nIds >= 1){
		cpuid(info, 0x00000001);
		if ((info[3] & ((int)1 << 23)) != 0) printf("\nMMX supported");
		if ((info[3] & ((int)1 << 25)) != 0) printf("\nSSE supported");
		if ((info[3] & ((int)1 << 26)) != 0) printf("\nSSE 2 supported");
		if ((info[2] & ((int)1 << 0)) != 0) printf("\nSSE 3 supported");

		if ((info[2] & ((int)1 << 9)) != 0) printf("\nSSSE 3 supported");
		if ((info[2] & ((int)1 << 19)) != 0) printf("\nSSE 4.1 supported");
		if ((info[2] & ((int)1 << 20)) != 0) printf("\nSSE 4.2 supported");

		if ((info[2] & ((int)1 << 28)) != 0) printf("\nAVX supported");
		if ((info[2] & ((int)1 << 12)) != 0) printf("\nFMA3 supported");
	}
	if (nIds >= 0x00000007){
		cpuid(info, 0x00000007);
		if ((info[1] & ((int)1 << 3)) != 0) printf("\nBMI 1 supported");
		if ((info[1] & ((int)1 << 8)) != 0) printf("\nBMI 2 supported");
		if ((info[1] & ((int)1 << 5)) != 0) printf("\nAVX 2 supported");
	}

	if (nExIds >= 0x80000001){
		cpuid(info, 0x80000001);
		if ((info[3] & ((int)1 << 29)) != 0) printf("\nx64 supported");
		if ((info[2] & ((int)1 << 6)) != 0) printf("\nSSE 4a supported");
		if ((info[2] & ((int)1 << 16)) != 0) printf("\nFMA 4 supported");
		if ((info[2] & ((int)1 << 11)) != 0) printf("\nXOP supported");
	}
}*/
}}
#endif
#endif