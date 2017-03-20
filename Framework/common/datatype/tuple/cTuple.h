/**
 *	\file cTuple.h
 *	\author Michal Kratky, Radim Baca
 *	\version 0.1
 *	\date jun 2006
 *	\brief Homogenous tuple for a tree data structure. It contains an array of items of the same type.
 */

#ifndef __cTuple_h__
#define __cTuple_h__

#include <assert.h>
#include <stdio.h>
#include <iostream>   
#include <string> 

#include "common/datatype/cDataType.h"
#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "common/datatype/cComparator.h"
#include "common/stream/cStream.h"
#include "common/datatype/cBasicType.h"
#include "common/cNumber.h"
#include "common/cString.h"
#include "common/cBitString.h"
#include "common/cBitArray.h"
#include "common/compression/Coder.h"
#include "common/utils/cHistogram.h"
#include "common/memdatstruct/cMemoryBlock.h"

using namespace common::compression;
using namespace common::datatype;
using namespace common::utils;

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
* Represents n-dimensional tuple. Homogenous tuple for a tree data structure. It contains an array of items of the same type.
* Tuple does not contain the reference to the space descriptor, therefore, almost no asserts are contained in the tuple!
* Application has to do the asserts by itself!
*
* Written just for integer data types (int, short, char) since the methods like >, Greater, Equal will not return correct answers.
*
* \author Radim Baca, Michal Kratky
* \version 2.2
* \date feb 2011
**/

namespace common {
	namespace datatype {
		namespace tuple {

			class cTuple : public cDataType
			{
			public:
				typedef cTuple T;

			protected:
				static const unsigned int SIZEPREFIX_LEN = 1; /// Number of bytes reserved for the information about the NTuple length -> we work with NTuple in some methods

				char *mData;

			private:

				unsigned int Sum();

			public:
				static const unsigned int LengthType = cDataType::LENGTH_FIXLEN;
				static const char CODE = 't';

				inline virtual char GetCode() { return CODE; }

				static unsigned int countCompare; //val644 - staticka promenna na ukladani poctu porovnavani
				static unsigned int basicCountCompare; //val644 - pocet porovnani pro zakladni RQ
				static unsigned int countCompareOrder; //val644 - ulozeni poctu porovnani pri setrizeni rozsahovych dotazu
				static double basicTotalReadNodes; //val644 - pocet nacteni uzlu, pri vyhledavani rozsahovych dotazu
				static unsigned int countCompareLevel[24]; //val644 - staticka promenna na ukladani poctu porovnavani pro ruzne urovne stromu
				static unsigned int levelTree; //val644 - pro ulozeni v jake urovni stromu se nachazi
				static unsigned int readNodesInLevel[24]; //val644 - promenna pro ukladani poctu nacteni uzlu pro Level stromu
				static unsigned int itemsCountForLevel[24]; // val644 - ulozeni poctu prvku v korenovem uzlu;
				static bool typeRQordered; //val644 - zda jsou rozsahove dotazy setrizene
				static unsigned int typeRQ; // 0 = Vzdy se nastavi na zacatek, 1 = na zacatek predesleho rozsahoveho dotazu, 2 = konec predesleho rozsahoveho dotazu (tento typ odstrani duplicity ve vysledku)
				static unsigned int callCountCompare; //val644 - pocet zavolani porovnani
				static unsigned int addItemOrder; //val644 - pocet posunuti v listech
				int flagRQ = 0; //val644 - nastaveni zda se maji pridat klice z listoveho uzlu od prvniho klice, poprve je v listovem uzlu dany RQ flagRQ = 1, ano  pridavat od prvniho klice flagRQ = 2;
				static float indexSizeMB; //val644 - velikost BTree v MB
				static unsigned int tupleLeafCountItems; //val644 - celkovy pocet listovych uzlu v btree
				static unsigned int leafItemsCount; //val644 - celkovy pocet klicu v listech

			private:
				// bool IsInInterval(const cTuple &ql, const cTuple &qh, unsigned int order) const;
				void hilbert_c2i(int nDims, int nBits, cBitString &hvalue, const cSpaceDescriptor* pSd) const;
				static inline double ComputeTaxiDistance(const char* data, const cSpaceDescriptor* pSd);

			public:
				cTuple();
				cTuple(const cSpaceDescriptor *pSd);
				cTuple(const cSpaceDescriptor *pSd1, const cSpaceDescriptor *pSd2);
				cTuple(const cDTDescriptor *pSd);
				cTuple(const cSpaceDescriptor *pSd, unsigned int len);
				~cTuple();

				cTuple(char* buffer);
				static inline int GetObjectSize(const cSpaceDescriptor *pSd);

				void Free(cMemoryBlock *memBlock = NULL);

				bool Resize(const cSpaceDescriptor *pSd, cMemoryBlock *memBlock = NULL);
				bool Resize(const cSpaceDescriptor * pSd1, const cSpaceDescriptor * pSd2, cMemoryBlock * memBlock=NULL);
				bool Resize(const cDTDescriptor *pSd, uint length);
				bool Resize(const cDTDescriptor *pSd);

				inline char* Init(char* mem);
				// static inline unsigned int Sizeof();
				// inline void Format(cSpaceDescriptor *pSd, cMemoryBlock* memory);
				void SetValue(const cTuple &tuple, const cSpaceDescriptor *pSd);

				void SetFlagRQ(uint value); //val644

				inline void SetValue(unsigned int order, float value, const cDTDescriptor* pSd);
				inline void SetValue(unsigned int order, double value, const cDTDescriptor* pSd);
				inline void SetValue(unsigned int order, int value, const cDTDescriptor* pSd);
				inline void SetValue(unsigned int order, unsigned int value, const cDTDescriptor* pSd);
				inline void SetValue(unsigned int order, char value, const cDTDescriptor* pSd);
				inline void SetValue(unsigned int order, unsigned char value, const cDTDescriptor* pSd);
				inline void SetValue(unsigned int order, wchar_t value, const cDTDescriptor* pSd);
				inline void SetValue(unsigned int order, unsigned short value, const cDTDescriptor* pSd);
				inline void SetValue(unsigned int order, short value, const cDTDescriptor* pSd);
				// inline void SetValue(unsigned int order, const char* cNTuple_value, const cSpaceDescriptor* pSd);
				inline void SetValue(unsigned int order, char* cTuple_value, const cDTDescriptor* pSd);

				void SetMaxValues(const cSpaceDescriptor* pSd);
				inline void SetMaxValue(unsigned int order, const cSpaceDescriptor* pSd);
				inline void Clear(const cSpaceDescriptor* pSd);
				inline void Clear(unsigned int order, const cSpaceDescriptor* pSd);
				inline void ClearOther(unsigned int order, const cSpaceDescriptor* pSd);

				inline float GetFloat(unsigned int order, const cDTDescriptor* pSd) const;
				inline double GetDouble(unsigned int order, const cDTDescriptor* pSd) const;
				inline int GetInt(unsigned int order, const cDTDescriptor* pSd) const;
				inline unsigned int GetUInt(unsigned int order, const cDTDescriptor* pSd) const;
				inline char GetByte(unsigned int order, const cDTDescriptor* pSd) const;
				inline unsigned char GetUChar(unsigned int order, const cDTDescriptor* pSd) const;
				inline short GetShort(unsigned int order, const cDTDescriptor* pSd) const;
				inline unsigned short GetUShort(unsigned int order, const cDTDescriptor* pSd) const;
				inline void GetString(unsigned int order, cString &string, const cDTDescriptor* pSd) const;
				inline wchar_t GetWChar(unsigned int order, const cDTDescriptor* pSd) const;
				// inline char* GetNTuple(unsigned int order, const cDTDescriptor* pSd) const;
				inline char* GetTuple(unsigned int order, const cDTDescriptor* pSd);
				static inline char* GetTuple(char* data, unsigned int order, const cDTDescriptor* pSd);

				inline float* GetPFloat(unsigned int order, const cDTDescriptor* pSd) const;
				inline int* GetPInt(unsigned int order, const cDTDescriptor* pSd) const;
				inline unsigned int* GetPUInt(unsigned int order, const cDTDescriptor* pSd) const;
				inline char* GetPByte(unsigned int order, const cDTDescriptor* pSd) const;
				inline unsigned char* GetPUChar(unsigned int order, const cDTDescriptor* pSd) const;
				inline bool IsZero(unsigned int order, const cSpaceDescriptor* pSd) const;
				inline bool IsZero(const cSpaceDescriptor* pSd) const;

				inline unsigned int GetSize(const cDTDescriptor* pSd) const;
				inline unsigned int GetSize_instance(const char *data, const cDTDescriptor *pDtd) const;
				inline unsigned int GetSize(uint tupleSize) const;
				inline unsigned int GetMaxSize(const cDTDescriptor* pSd) const;
				inline unsigned int GetSerialSize(const cDTDescriptor* pSd) const;
				inline char* GetData() const;
				inline void SetData(char* data) { mData = data; }
				inline operator char*() const;
				inline unsigned int GetRealDimension(const cSpaceDescriptor* pSd) const;
				inline unsigned int GetRealDimension(unsigned int hi, const cSpaceDescriptor* pSd) const;

				double EuclidianIntDistance(const cTuple &tuple, const cSpaceDescriptor* pSd) const;
				float UnitIntervalLength(const cTuple &tuple, unsigned int order, const cSpaceDescriptor* pSd) const;
				static inline int CompareZOrder(const char* tuple1, const char* tuple2, cSpaceDescriptor* pSd);
				static inline int CompareHOrder(const char* tuple1, const char* tuple2, cSpaceDescriptor* pSd);
				static inline int CompareTaxiOrder(const char* tuple1, const char* tuple2, const cSpaceDescriptor* pSd);
				inline int CompareArray(const char* array1, const char* array2, uint length);
				inline unsigned int HashValue(const char *array, unsigned int length, unsigned int hashTableSize);

				inline bool Read(cStream *stream, const cSpaceDescriptor* pSd);
				inline bool Write(cStream *stream, const cSpaceDescriptor* pSd) const;

				inline int Equal(const cTuple &tuple, unsigned int order, const cSpaceDescriptor* pSd) const;
				inline int Equal(const cTuple &tuple, const cSpaceDescriptor* pSd) const;
				inline int Equal(const char* tuple2, const cSpaceDescriptor* pSd) const;
				inline bool Equal(unsigned int start, unsigned int k, const cTuple &tuple, const cSpaceDescriptor* pSd) const;
				// inline int Equal(const char* tuple2, unsigned int order) const;
				//void operator = (const cTuple &tuple);
				//void operator += (const cTuple &tuple);
				// inline bool operator == (const cTuple &tuple) const;
				//inline bool operator != (const cTuple &tuple) const;
				//inline bool operator > (const cTuple &tuple) const;
				//inline bool Greater(const cTuple &tuple) const;
				inline int Compare(const cTuple &tuple, const cSpaceDescriptor* pSd) const;
				int CompareLexicographically(const char* tuple2, const cSpaceDescriptor* pSd) const;
				inline int CompareLexicographically(const cTuple &tuple, const cSpaceDescriptor* pSd) const;

				static inline int Compare_uint(const uint* t1, const uint* t2, const cSpaceDescriptor* pSd); //val644

				inline void Copy(const cTuple &tuple);
				inline unsigned int CopyTo(char *data, const cSpaceDescriptor* pSd) const;
				inline unsigned int CopyTo(char *data, const cDTDescriptor* pDtD) const;
				inline unsigned int Copy(const char *srcData, const cSpaceDescriptor* pDtD);
				inline unsigned int Copy(const char *srcData, const cDTDescriptor* pDtD);

				void Print(const char *string, const cSpaceDescriptor* pSd) const;
				void Print(const char *string, const cDTDescriptor* pSd) const;
				void Print(unsigned int order, const char *string, const cSpaceDescriptor* pSd) const;

				inline unsigned int HashValue(unsigned int hashTableSize, const cDTDescriptor* dtd) const;
				inline unsigned int OrderedHashValue(const cDTDescriptor* dtd, const unsigned int hashType = 0, const unsigned int hashArg = 0) const;	// for extentible and linear hashing

				void ComputeHAddress(cBitString &hvalue, const cSpaceDescriptor* pSd) const;

				// Instance methods, but working with char*
				inline int Compare(const char* tuple2, const cSpaceDescriptor* pSd) const;
				inline int Compare(const char* tuple2, const cDTDescriptor *dd) const;

				static inline void Clear(char* data, const cSpaceDescriptor* pSd);
				// Static methods working with char*
				static inline void SetTuple(char* data, const cTuple &tuple, const cSpaceDescriptor* pSd);
				static inline void SetTuple(char* dst_data, const char* src_data, const cSpaceDescriptor* pSd);
				static inline cTuple* GetValue(char* value, const cDTDescriptor* pSd);
				static inline cTuple* GetValueC(const char* value, const cDTDescriptor* pSd);
				static bool ResizeSet(cTuple &t1, const cTuple &t2, const cDTDescriptor* pSd, cMemoryBlock* memBlock);

				static void Free(cTuple &tuple, cMemoryBlock *memBlock = NULL);

				static inline void Copy(char* cUnfTuple_dst, const char* cUnfTuple_src, const cDTDescriptor *pSd);
				// static inline bool Equal(const cTuple &tuple1, const char* tuple2);
				static inline bool IsEqual(const char* tuple1, const char* tuple2, const cSpaceDescriptor *pSd);
				static inline bool IsEqual(const char* tuple1, const char* tuple2, const cDTDescriptor *pDtd);

				static inline int Equal(const char* tuple1, const char* tuple2, uint tupleLength, const cDTDescriptor *pSd);
				static inline int Equal(const char* tuple1, const char* tuple2, const cDTDescriptor *pSd);
				static inline int Equal(const char* tuple1, const char* tuple2, const cSpaceDescriptor *pSd);
				static inline int Equal(const char* cUnfTuple_t1, const char* cUnfTuple_t2, const unsigned int order, const cSpaceDescriptor* pSd);
				static inline int Equal(const char* cUnfTuple_t1, const char* cUnfTuple_t2, const unsigned int order1, const unsigned int order2, const cSpaceDescriptor* pSd);
				static inline int Equal(const char* cTuple_t1, const unsigned int order1, const cSpaceDescriptor* sd1, char* cLNTuple_t2, const unsigned int order2, const cSpaceDescriptor* sd2);

				static inline int Compare(const cTuple& t1, const cTuple& t2, const cDTDescriptor *pDtd);
				static inline int Compare(const char* dst, const char* src, const cDTDescriptor *pDtd);
				static inline int Compare(const char* tuple1, const char* tuple2, const cSpaceDescriptor *pSd);
				static inline int CompareLexicographically(const char* tuple1, const char* tuple2, const cSpaceDescriptor* pSd, uint dimension = NOT_DEFINED);

				static inline int ComparePartly(const char* cTuple_t1, const char* cNTuple_t2, const cDTDescriptor* pSd, unsigned int startOrder);
				static inline unsigned int HashValue(char *tuple, uint hashTableSize, const cDTDescriptor* dtd);

				static void Print(const char *data, const char* delim, const cSpaceDescriptor* pSd);
				static void Print(const char *data, const char* delim, const cDTDescriptor* pSd);
				static inline void WriteAsText(char* data, cStream *stream, const cSpaceDescriptor* pSd);
				static void Print2File(FILE *StreamInfo, const char *data, const char* delim, const cSpaceDescriptor* pSd);
				static void Print2File(FILE *StreamInfo, const char *data, const char* delim, const cDTDescriptor* pSd);

				//val644 - start
				static inline void PrintInfoCompare(unsigned int basicCountCompare, unsigned int treeHeight); //val644
				static inline void SaveToFileStatisticInfoCompare(string fileName, string typeRQ, int rq_min, int rq_max, bool new_typeRQ, double mTime, double timeOrderRQ, unsigned int RQ_resultSize, int retry_RangeQuery, int count_RQ, double basicTime);
				static inline int GetTotalReadNodes(); //val644
				static inline void ResetStatistic(); //val644
				//val644 - end

				static inline unsigned int GetLSize(uint tupleLength, const cDTDescriptor* dtd);

				static inline unsigned int GetSize(const char *data, const cDTDescriptor* pSd);
				static inline unsigned int GetMaxSize(const char *data, const cDTDescriptor* pSd);

				static inline unsigned int GetUInt(const char *data, unsigned int order, const cSpaceDescriptor* pSd);
				static inline int GetInt(const char *data, unsigned int order, const cSpaceDescriptor* pSd);
				static inline float GetFloat(const char *data, unsigned int order, const cSpaceDescriptor* pSd);
				static inline short GetShort(const char *data, unsigned int order, const cSpaceDescriptor* pSd);
				static inline unsigned short GetUShort(const char *data, unsigned int order, const cSpaceDescriptor* pSd);
				static inline unsigned char GetUByte(const char *data, unsigned int order, const cSpaceDescriptor* pSd);
				static inline char GetByte(const char *data, unsigned int order, const cSpaceDescriptor* pSd);
				static inline unsigned char GetUChar(const char *data, unsigned int order, const cDTDescriptor* pSd);
				static inline char* GetPChar(const char* data, unsigned int order, const cDTDescriptor* pSd);

				static inline void SetValue(char *data, unsigned int order, int value, const cSpaceDescriptor* pSd);
				static inline void SetValue(char *data, unsigned int order, unsigned int value, const cSpaceDescriptor* pSd);
				static inline void SetValue(char *data, unsigned int order, unsigned short value, const cSpaceDescriptor* pSd);
				static inline void SetValue(char *data, unsigned int order, unsigned char value, const cSpaceDescriptor* pSd);
				static inline void SetValue(char *data, unsigned int order, float value, const cSpaceDescriptor* pSd);
				static inline void SetValue(char *data, unsigned int order, char value, const cSpaceDescriptor* pSd);

				// static bool ModifyMbr(const char* cTuple_t, char*  cTuple_pQl, char*  cTuple_pQh, const cSpaceDescriptor* pSd);
				// static bool IsInBlock(const char* cTuple_t, const char* cTuple_ql, const char* cTuple_qh, const cSpaceDescriptor* pSd);
				// static bool IsInBlock(const char* cTuple_t, const cTuple &ql, const cTuple &qh, const cSpaceDescriptor* pSd);
				// static bool IsInInterval(const char* cTuple_t, const char* cTuple_ql, const char* cTuple_qh, unsigned int order);
				// static bool IsInInterval(const char* cTuple_t, const cTuple &ql, const cTuple &qh, unsigned int order);
				static float UnitIntervalLength(const char* cTuple_t1, const char* cTuple_t2, const unsigned int order, const cSpaceDescriptor* pSd);

				// Peter Chovanec 11.10.2011
				static double TaxiCabDistance(const char* cTuple_t1, const char* cTuple_t2, const cDTDescriptor* pSd);
				static char* Subtract(const char* cTuple_t1, const char* cTuple_t2, char* cTuple_result, const cDTDescriptor* pSd);
				static char* Add(const char* cTuple_t1, const char* cTuple_t2, char* cTuple_result, const cDTDescriptor* pSd);

				static inline void Subtract(const char* cTuple_t1, const char* cTuple_t2, char* cTuple_result, unsigned int order, const cSpaceDescriptor* pSd);
				static inline void Add(const char* cTuple_t1, const char* cTuple_t2, char* cTuple_result, unsigned int order, const cSpaceDescriptor* pSd);
				static inline double TaxiCabDistanceValue(const char* cTuple_t1, const char* cTuple_t2, unsigned int order, const cSpaceDescriptor* pSd);

				static inline uint GetDimension(const cDTDescriptor* pSd);

				// for histogram purpose
				inline void AddToHistogram(cHistogram** histogram, const cDTDescriptor* dtd) const;
				static inline void AddToHistogram(const char* cTuple_tuple, cHistogram** histogram, const cDTDescriptor* dtd);

				// for codding purpose
				static inline uint Encode(uint method, const char* sourceBuffer, char* encodedBuffer, const cDTDescriptor* sd, uint tupleLength = NOT_DEFINED);
				static inline uint Decode(uint method, char* encodedBuffer, char* decodedBuffer, const cDTDescriptor* sd, uint tupleLength = NOT_DEFINED);
				static inline uint GetEncodedSize(uint method, char* sourceBuffer, const cDTDescriptor* sd, uint tupleLength = NOT_DEFINED);
				static inline uint EncodedSize(uint method, char* sourceBuffer, const cDTDescriptor* sd);

				// for ri purpose
				inline int GetLength() const { return 0; };
				static inline int GetLength(const char* cTuple_t1, const cDTDescriptor* pSd) { return ((cSpaceDescriptor*)pSd)->GetDimension(); };

				//static unsigned int SameValues(char* cBitString_Mask, const char* cTuple_prefix, const char* cTuple_tuple, const cDTDescriptor* pSd, unsigned int sameValues);

				//static double CommonPrefixLength(const char* cNTuple_prefix, const char* cTuple_tuple, const cDTDescriptor* pSd, unsigned int prefixLength);
				//static double PrefixLength(const char* cNTuple_prefix, const char* cTuple_tuple, const cDTDescriptor* pSd, unsigned int prefixLength);

				//static bool StartsWith(char* cNTuple_prefix, const char* cTuple_tuple, const cDTDescriptor* pSd, unsigned int prefixLength);
				//static char* CompleteMinRefItem(char* cBitString_Mask, const char* cTuple_minItem, const char* cTuple_key, char* cNTuple_partMinItem, char* cTuple_result, const cDTDescriptor* pSd);

				//static bool Equal(char* cBitString_Mask1, const char* cTuple_t1, char* cBitString_Mask2, const char* cTuple_t2, const cDTDescriptor* pSd);


				static inline uint CutTuple(const char* cBitString_Mask, const char* cTuple_RI, const char* cTuple_Key, char* cTuple_Result, const cDTDescriptor* pSd);
				static inline char* MergeTuple(const char* cBitString_Mask, const char* cTuple_RI, const char* cTuple_Key, char* cTuple_Result, const cDTDescriptor* pSd);
				static inline char* SetMask(const char* cBitString_Mask, const char* cTuple_RI, const char* cTuple_Key, char* cBitString_Result, const cDTDescriptor* pSd);
				static inline char* SetMask(const char* cTuple_t1, const char* cTuple_t2, char* cBitString_Result, const cDTDescriptor* pSd);
				static inline char* SetMask(const char* cTuple_t1, const char* cBitString_Mask1, const char* cTuple_t2, const char* cBitString_Mask2, char* cBitString_Result, const cDTDescriptor* pSd);
				static inline char* SetMinRefItem(const char* cTuple_RI, const char* cTuple_Key, char* cTuple_Result, const cDTDescriptor* pSd);
				static inline char* MergeMasks(char* cBitString_Mask1, char* cBitString_Mask2, const char* cTuple_RI1, const char* cTuple_RI2, char* cBitString_Result, const cDTDescriptor* pSd);

				static inline bool IsCompatible(const char* cBitString_Mask, const char* cTuple_RI, const char* cTuple_Key, const cDTDescriptor* pSd);
			};
		}
	}
}

#include "common/datatype/tuple/cCommonNTuple.h"

namespace common {
	namespace datatype {
		namespace tuple {

			/**
			* \return real dimension of tuple. Real dimension means the all last zero coordinates are cutted.
			*/
			inline unsigned int cTuple::GetRealDimension(const cSpaceDescriptor* pSd) const
			{
				return GetRealDimension(pSd->GetDimension(), pSd);
			}

			//val644
			inline void cTuple::PrintInfoCompare(unsigned int basicCountCompare, unsigned int treeHeight)
			{
				bool fullInfo = true;
				float avg;
				float a;
				float b;

				a = cTuple::countCompare;
				b = basicCountCompare;
				avg = a / b;
				avg = (avg)* 100;
				printf("\n    CountCompare SUM: %i \t [%0.3f %%]", cTuple::countCompare, avg);

				a = cTuple::countCompare + countCompareOrder;
				avg = a / b;
				avg = (avg)* 100;

				printf("\n       CC with order: %i \t [%0.3f %%]", (cTuple::countCompare + countCompareOrder), avg);

				a = cTuple::countCompare;
				b = cTuple::callCountCompare;
				avg = a / b;
				printf("\n             Call CC: %i", cTuple::callCountCompare);
				printf("\n        AVG compare Tuple: %f", avg);

				/*avg = (float)cTuple::countCompare / myHeader->GetNodeCount();
				printf("\n        SUM/AllNodes: %i/%i = %4.3f", cTuple::countCompare, myHeader->GetNodeCount(), avg);

				avg = (float)cTuple::countCompare / myHeader->GetItemCount();
				printf("\n        SUM/AllItems: %i/%i = %4.3f", cTuple::countCompare, myHeader->GetItemCount(), avg);*/

				int sum = 0;
				for (unsigned int i = 0; i < 24; i++)
				{
					sum += cTuple::readNodesInLevel[i];
				}
				a = sum;
				b = basicTotalReadNodes;
				avg = a / b;
				avg = (avg)* 100;
				printf("\n        AllReadsNodes: %i\t[%0.3f%%]", sum, avg);

				printf("\n    CountCompareFor: ");
				if (fullInfo)
				{
					for (unsigned int i = 0; i < 24; i++)
					{
						if (cTuple::countCompareLevel[i] != 0)
						{
							printf("\n\tLevel[%i]: %i", i, cTuple::countCompareLevel[i]);

							int avgForNodes = 0;

							//if (i == 0)
							//{
							printf("\n\t   CountItems: %i", cTuple::itemsCountForLevel[i]);
							printf("\n\t   TotalReadsNodes: %i", cTuple::readNodesInLevel[i]);
							//avg = (float)cTuple::countCompareLevel[i] / QUERIES_IN_MULTIQUERY;
							//printf("\n\t   /RangeQueryCount: %i/%i = %4.3f", cTuple::countCompareLevel[i], QUERIES_IN_MULTIQUERY, avg);
						//}
						//else if (i > 0)
						//{
						//	printf("\n\t   TotalReadsNodes: %i", cTuple::readNodesInLevel[i]);

						//	if (i == treeHeight)
						//	{
								//avg = (float)cTuple::countCompareLevel[i] / myHeader->GetLeafNodeCount();
								//printf("\n\t   CCFL/LeafNodes: %i/%i = %4.3f", cTuple::countCompareLevel[i], myHeader->GetLeafNodeCount(), avg);
								//avg = (float)cTuple::countCompareLevel[i] / myHeader->GetLeafItemCount();
								//printf("\n\t   CCFL/LeafItems: %i/%i = %4.3f", cTuple::countCompareLevel[i], myHeader->GetLeafItemCount(), avg);
						//	}
						//}
						}
					}
				}
			}

			inline void cTuple::SaveToFileStatisticInfoCompare(string fileName, string typeRQ, int rq_min, int rq_max, bool new_typeRQ, double mTime, double timeOrderRQ, unsigned int RQ_resultSize, int retry_RangeQuery, int count_RQ, double basicTime)
			{
				ofstream saveFile;
				saveFile.open(fileName + ".csv", std::ios::app);

				if (new_typeRQ)
				{
					saveFile << "\n";
					saveFile << "\n";
					saveFile << "\n";
					saveFile << "Range query repeat count;" << retry_RangeQuery;
					saveFile << "\nCount of keys;" << leafItemsCount << ";Range query size;Count of ranqe queries\n";
					saveFile << "Index size: " << indexSizeMB << "MB;;;" << count_RQ << ";;;;;;;;;;;";
					for (unsigned int i = 0; i < 24; i++)
					{
						if (cTuple::countCompareLevel[i] != 0)
						{
							saveFile << ";Level [" << i << "]";
							//if (i == 0)
							//{
							saveFile << ";Level [" << i << "]";
							saveFile << ";Level [" << i << "]";
							//}
							//else
							//{
							//	saveFile << ";Level [" << i << "]";
							//}

							//if (i < 23)
							//{
							//	if (cTuple::countCompareLevel[i + 1] == 0)
							//	{
							//		saveFile << ";Level [" << i << "]";
							//	}
							//}
						}
					}
					saveFile << "\n;;;";
					saveFile << "Execution time;Time in %;Number of comparisons;Number of comparisons in %;Order;Time used for ordering;Total number of comparisons;Total number of comparisons in %; Result size;Avg. number of key comparison;Total read nodes;Total read nodes in %";
					for (unsigned int i = 0; i < 24; i++)
					{
						if (cTuple::countCompareLevel[i] != 0)
						{
							saveFile << ";Number of comparisons";

							//if (i == 0)
							//{
							saveFile << ";Number of keys";
							saveFile << ";Read nodes";
							//}
							//else
							//{
							//	saveFile << ";Read nodes";
							//}

							//if (i < 23)
							//{
							//	if (cTuple::countCompareLevel[i + 1] == 0)
							//	{
							//		saveFile << ";Number of leaf nodes";
							//	}
							//}
						}
					}
				}
				saveFile << "\n";

				if (typeRQ == "RQ" | typeRQ == "RQ_Order" | typeRQ == "RQ_LoHi" | typeRQ == "RQ_LoHiO")
				{
					saveFile << ";" << (char *)&typeRQ << ";" << rq_min << " ~ " << rq_max << ";" << mTime << ";" << (((mTime / basicTime)) * 100);
				}
				else
				{
					saveFile << ";" << (char *)&typeRQ << " - " << cTuple::typeRQ << ";" << rq_min << " ~ " << rq_max << ";" << mTime << ";" << (((mTime / basicTime)) * 100);
				}

				float avg;
				float a;
				float b;

				a = cTuple::countCompare;
				b = basicCountCompare;
				avg = a / b;
				avg = (avg)* 100;
				saveFile << ";" << cTuple::countCompare << ";" << avg << ";" << cTuple::countCompareOrder << ";" << timeOrderRQ;

				a = cTuple::countCompare + cTuple::countCompareOrder;
				avg = a / b;
				avg = (avg)* 100;
				saveFile << ";" << (cTuple::countCompareOrder + cTuple::countCompare) << ";" << avg << ";" << RQ_resultSize;

				a = cTuple::countCompare;
				b = cTuple::callCountCompare;
				avg = a / b;
				saveFile << ";" << avg;

				int sum = 0;
				for (unsigned int i = 0; i < 24; i++)
				{
					sum += cTuple::readNodesInLevel[i];
				}
				a = sum;
				b = cTuple::basicTotalReadNodes;
				avg = a / b;
				avg = (avg)* 100;
				saveFile << ";" << sum << ";" << avg;

				uint cmpCount = cComparator<void>::CountCompare;

				for (unsigned int i = 0; i < 24; i++)
				{
					if (cTuple::countCompareLevel[i] != 0)
					{
						saveFile << ";" << cTuple::countCompareLevel[i];

						//if (i == 0)
						//{
						saveFile << ";" << cTuple::itemsCountForLevel[i] << ";" << cTuple::readNodesInLevel[i];
						//}
						//else if (i > 0)
						//{
						//	saveFile << ";" << cTuple::readNodesInLevel[i];
						//}

						//if (i < 23)
						//{
						//	if (cTuple::countCompareLevel[i + 1] == 0)
						//	{
						//		saveFile << ";" << tupleLeafCountItems;
						//	}
						//}
					}
				}

				saveFile.flush();
				saveFile.close();
			}

			inline int cTuple::GetTotalReadNodes()
			{
				int a = 0;
				for (unsigned int i = 0; i < 24; i++)
				{
					a += readNodesInLevel[i];
				}
				return a;
			}

			inline void cTuple::ResetStatistic()
			{
				for (unsigned int res = 0; res < 24; res++)
				{
					countCompareLevel[res] = 0;
					readNodesInLevel[res] = 0;
					itemsCountForLevel[res] = 0;
				}
				countCompare = 0;
				callCountCompare = 0;
			}


			inline char* cTuple::GetData() const
			{
				return mData;
			}

			inline cTuple::operator char*() const
			{
				return mData;
			}

			inline unsigned int cTuple::GetSerialSize(const cDTDescriptor* pSd) const
			{
				return ((cSpaceDescriptor*)pSd)->GetSize();
			}

			//void cTuple::Format(cSpaceDescriptor *treeSpaceDescriptor, cMemoryBlock* memory)
			//{
			//	mDimension = treeSpaceDescriptor->GetDimension();
			//	mTypeSize = treeSpaceDescriptor->GetType(0)->GetSerialSize();
			//	mData = memory->GetMemory(mDimension * mTypeSize);
			//}

			inline void cTuple::Clear(char* data, const cSpaceDescriptor* pSd)
			{
				memset(data, 0, pSd->GetSize());
			}

			/**
			* Set all bits of the tuple values to zero
			*/
			void cTuple::Clear(const cSpaceDescriptor* pSd)
			{
				memset(mData, 0, pSd->GetSize());
			}

			/**
			* Set min value in the order-th coordinate.
			*/
			inline void cTuple::Clear(unsigned int order, const cSpaceDescriptor* pSd)
			{
				SetValue(order, 0, pSd);
			}

			/**
			* Clear the all coordinates out of first.
			*/
			void cTuple::ClearOther(unsigned int order, const cSpaceDescriptor* pSd)
			{
				// old: memset(mData + order * mTypeSize, 0, (mDimension - order) * mTypeSize);
			}

			/**
			* Compare k values of tuples starting by the _start coordinate.
			* \param start starting coordinate of the tuple where method start the comparison
			* \param k how many coordinates is compared
			* \invariant k > 0
			* \invariant start + k <= dimension
			* \return true if the first k values in tuples are the same
			*/
			inline bool cTuple::Equal(unsigned int _start, unsigned int k, const cTuple &tuple, const cSpaceDescriptor* pSd) const
			{
#ifndef NDEBUG
				assert((char)(_start + k) <= pSd->GetDimension());
				assert(k > 0);
#endif

				//if ((_start + k) == mDimension)
				//{
				//	start =  mTreeSpaceDescriptor->GetByteIndex(_start);
				//	size = mDimension * mTypeSize - start;
				//} else
				//{
				//	start =  mTreeSpaceDescriptor->GetByteIndex(_start);
				//	size = mTreeSpaceDescriptor->GetByteIndex(_start + k) - start;	
				//}

				printf("cTuple::Equal: Rewrite it!\n");
				return false;
				// old: return memcmp(mData + start * mTypeSize, tuple.GetData() + start, k * mTypeSize) == 0;
			}

			/**
			* Byte comparison between tuples. Using the memcmp function. Similar to Greater method.
			* \param tuple
			* \return
			*		- -1 if the this tuple is smaller than the parameter
			*		- 0 if the tupleas are the same
			*		- 1 if the parameter is bigger than this tuple
			*/
			inline int cTuple::Compare(const cTuple &tuple, const cSpaceDescriptor* pSd) const
			{
				// * !!! It seems that memcmp is not useable, due to opposite ordering of bytes. !!!
				// return memcmp((void*)mData, (void*)tuple.GetData(), mDimension * mTypeSize);
				return CompareLexicographically(tuple, pSd);
			}

			inline int cTuple::Equal(const char* tuple1, const char* tuple2, uint tupleLength, const cDTDescriptor *pSd)
			{
				return CompareLexicographically(tuple1, tuple2, (cSpaceDescriptor*)pSd, tupleLength);
			}

			inline int cTuple::Equal(const char* tuple1, const char* tuple2, const cDTDescriptor *pSd)
			{
				return CompareLexicographically(tuple1, tuple2, (cSpaceDescriptor*)pSd);
			}

			inline int cTuple::Equal(const char* tuple1, const char* tuple2, const cSpaceDescriptor *pSd)
			{
				return CompareLexicographically(tuple1, tuple2, pSd);
			}

			inline bool cTuple::IsEqual(const char* tuple1, const char* tuple2, const cSpaceDescriptor *pSd)
			{
				return (memcmp((void*)tuple1, (void*)tuple2, pSd->GetSize()) == 0);
				// return Compare(tuple1, tuple2, pSd) == 0;
			}

			inline bool cTuple::IsEqual(const char* tuple1, const char* tuple2, const cDTDescriptor *pDtd)
			{
				return IsEqual(tuple1, tuple2, (const cSpaceDescriptor*)pDtd);
			}

			inline int cTuple::Compare(const cTuple& t1, const cTuple& t2, const cDTDescriptor *pSd)
			{
				return t1.Compare(t2, pSd);
			}

			inline int cTuple::Compare(const char* tuple1, const char* tuple2, const cDTDescriptor *pDtd)
			{
				return Compare(tuple1, tuple2, (const cSpaceDescriptor*)pDtd);
			}

			inline int cTuple::Compare(const char* tuple1, const char* tuple2, const cSpaceDescriptor *pSd)
			{
				// return CompareLexicographically(tuple1, tuple2, pSd); // cmpv1
				int ret;

				if (pSd->IsSpaceHomogenous())
				{
					//ret = pSd->GetDimensionType(0)->CompareArray(tuple1, tuple2, pSd->GetDimension());
					ret = Compare_uint((const uint*)tuple1, (const uint*)tuple2, pSd);

					/*
					for test purposes:

					char code = pSd->GetTypeCode(0);
					uint len = pSd->GetDimension();
					switch(code)
					{
					case cUInt::CODE:
						ret = cComparator<uint>::Compare((const uint*)tuple1, (const uint*)tuple2, len);
						break;
					case cInt::CODE:
						ret = cComparator<int>::Compare((const int*)tuple1, (const int*)tuple2, len);
						break;
					default:
						ret = CompareLexicographically(tuple1, tuple2, pSd);
						break;
					}*/
				}
				else
				{
					ret = CompareLexicographically(tuple1, tuple2, pSd);
				}
				return ret;
			}

			/**
			* Compare two tuples in the case the include only uints.
			* \return -1 if the tuple1 < tuple2, 0 if tuples are the same, 1 if tuple1 > tuple2s.
			*/
			inline int cTuple::Compare_uint(const uint* t1, const uint* t2, const cSpaceDescriptor* pSd)
			{
				int ret = 0;
				uint dim = pSd->GetDimension();
				uint* pt1 = (uint*)t1;
				uint* pt2 = (uint*)t2;
				cTuple::callCountCompare++;

				for (uint i = 0; i < dim; i++)
				{
					uint v1 = *pt1;
					uint v2 = *pt2;

					//val644 - start - increment countCompare
					cTuple::countCompare++;
					cTuple::countCompareLevel[cTuple::levelTree]++;
					//val644 - end - increment countCompare

					if (v1 < v2)
					{
						ret = -1;
						break;
					}
					else if (v1 > v2)
					{
						ret = 1;
						break;
					}
					pt1++;
					pt2++;
				}
				return ret;
			}

			/**
			 * Semantic of this method is rather problematic for cTuple, since cDataType::CompareArray is designed for
			 * comparison of two arrays of primitive data type values.
			 */
			inline int cTuple::CompareArray(const char* array1, const char* array2, uint length)
			{
				printf("Warning: cTuple::CompareArray(): This method should not be invoked!\n");
				return -1;
			}

			inline unsigned int cTuple::HashValue(const char *array, unsigned int length, unsigned int hashTableSize)
			{
				printf("Warning: cTuple::HashValue(): This method should not be invoked!\n");
				return 0;
			}

			inline int cTuple::Compare(const char* tuple2, const cSpaceDescriptor *pSd) const
			{
				return Compare(mData, tuple2, pSd);
			}

			inline int cTuple::Compare(const char* tuple2, const cDTDescriptor *pDtd) const
			{
				// you must check dimension a typeSize
				return Compare(mData, tuple2, (const cSpaceDescriptor *)pDtd);
			}

			/**
			* Compare values in this tuple and another tuple
			* \return true if this tuple is greater then the second tuple from the first dimension
			*/
			inline int cTuple::CompareLexicographically(const cTuple &tuple, const cSpaceDescriptor* pSd) const
			{
				int ret = 0;
				unsigned int dim = pSd->GetDimension();

				for (unsigned int i = 0; i < dim; i++)
				{
					ret = Equal(tuple, i, pSd);
					if (ret != 0)
					{
						break;
					}
				}

				return ret;
			}

			/// Compare tuples
			inline int cTuple::CompareLexicographically(const char* tuple1, const char* tuple2, const cSpaceDescriptor* pSd, uint dimension)
			{
				// assert(mDimension == tuple.GetDimension());
				int ret = 0;
				unsigned int dim = (dimension == NOT_DEFINED) ? pSd->GetDimension() : dimension;

				for (unsigned int i = 0; i < dim; i++)
				{
					ret = Equal(tuple1, tuple2, i, pSd);
					if (ret != 0)
					{
						break;
					}
				}

				return ret;
			}

			/// Compare tuples (tuple and char*)
			inline int cTuple::CompareLexicographically(const char* tuple2, const cSpaceDescriptor* pSd) const
			{
				return CompareLexicographically(GetData(), tuple2, pSd);
			}


			/**
			* Compare two tuples.
			* \return -1 if the tuple1 < tuple2, 0 if tuples are the same, 1 if tuple1 > tuple2s.
			*/
			inline int cTuple::ComparePartly(const char* cTuple_t1, const char* cNTuple_t2, const cDTDescriptor* pSd, unsigned int startOrder)
			{
				// assert(mDimension == tuple.GetDimension());
				cSpaceDescriptor *spaceDescriptor = (cSpaceDescriptor *)pSd;
				int ret = 0;
				unsigned int dim = spaceDescriptor->GetDimension();

				for (unsigned int i = startOrder; i < dim; i++)
				{
					ret = Equal(cTuple_t1, cNTuple_t2 + SIZEPREFIX_LEN, i, i - startOrder, spaceDescriptor);
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

			/// Copy only the pointer address!! Rewrite the pointers in this tuple by pointers in the parameter tuple. 
			/// This method can even lead to heap error during the delete phase, because you will try to free the same memory twice.
			inline void cTuple::Copy(const cTuple &tuple)
			{
				mData = tuple.GetData();
			}

			/**
			* \param data Destination memory.
			* \return Size of the data copied into the data parameter.
			*/
			inline unsigned int cTuple::CopyTo(char* data, const cSpaceDescriptor* pSd) const
			{
				// win: CopyMemory(data, mData, pSd->GetByteSize());
				uint size = pSd->GetSize();
				memcpy(data, mData, size);
				return size;
			}

			/**
			* \param data Destination memory.
			* \return Size of the data copied into the data parameter.
			*/
			inline unsigned int cTuple::CopyTo(char* data, const cDTDescriptor* pSd) const
			{
				return CopyTo(data, (const cSpaceDescriptor*)pSd);
			}

			/**
			* \param data Source memory
			* \return Size of the data copied into the data parameter.
			*/
			inline unsigned int cTuple::Copy(const char* data, const cSpaceDescriptor* pSd)
			{
				uint size = pSd->GetSize();
				memcpy(mData, data, size);
				return size;
			}

			/**
			* \param data Source memory
			* \return pDtD data descriptor
			*/
			inline unsigned int cTuple::Copy(const char *srcData, const cDTDescriptor* pDtD)
			{
				return Copy(srcData, (const cSpaceDescriptor*)pDtD);
			}


			void cTuple::SetValue(unsigned int order, float value, const cDTDescriptor* pSd)
			{
				printf("cTuple::SetValue - float type not supported!\n");
			}

			void cTuple::SetValue(unsigned int order, double value, const cDTDescriptor* pSd)
			{
				printf("cTuple::SetValue - double type not supported!\n");
			}

			/**
			* Set the int value of the dimension specified by the order parameter
			* \param order Dimension whose value should be set
			* \param value New value of the dimension
			* \invariant order < tuple dimension
			*/
			void cTuple::SetValue(unsigned int order, int value, const cDTDescriptor* pSd)
			{
				
				int* temp = (int*)(mData + ((cSpaceDescriptor*)pSd)->GetDimensionOrder(order));

				*(int*)(mData + ((cSpaceDescriptor*)pSd)->GetDimensionOrder(order)) = value;
				


			}

			/**
			* Set the unsigned int value of the dimension specified by the order parameter
			* \param order Dimension whose value should be set
			* \param value New value of the dimension
			* \invariant order < tuple dimension
			*/
			void cTuple::SetValue(unsigned int order, unsigned int value, const cDTDescriptor* pSd)
			{
				*(unsigned int*)(mData + ((cSpaceDescriptor*)pSd)->GetDimensionOrder(order)) = value;
			}

			/**
			* Set the char value of the dimension specified by the order parameter
			* \param order Dimension whose value should be set
			* \param value New value of the dimension
			* \invariant order < tuple dimension
			*/
			void cTuple::SetValue(unsigned int order, char value, const cDTDescriptor* pSd)
			{
				*(char*)(mData + ((cSpaceDescriptor*)pSd)->GetDimensionOrder(order)) = value;
			}

			/**
			* Set the unsigned char value of the dimension specified by the order parameter
			* \param order Dimension whose value should be set
			* \param value New value of the dimension
			* \invariant order < tuple dimension
			*/
			void cTuple::SetValue(unsigned int order, unsigned char value, const cDTDescriptor* pSd)
			{
				*(unsigned char*)(mData + ((cSpaceDescriptor*)pSd)->GetDimensionOrder(order)) = value;
			}

			/**
			* Set the unsigned char value of the dimension specified by the order parameter
			* \param order Dimension whose value should be set
			* \param value New value of the dimension
			* \invariant order < tuple dimension
			*/
			void cTuple::SetValue(unsigned int order, wchar_t value, const cDTDescriptor* pSd)
			{
				*(wchar_t*)(mData + ((cSpaceDescriptor*)pSd)->GetDimensionOrder(order)) = value;
			}

			/**
			* Set the short value of the dimension specified by the order parameter
			* \param order Dimension whose value should be set
			* \param value New value of the dimension
			* \invariant order < tuple dimension
			*/
			void cTuple::SetValue(unsigned int order, short value, const cDTDescriptor* pSd)
			{
				*(short*)(mData + ((cSpaceDescriptor*)pSd)->GetDimensionOrder(order)) = value;
			}

			/**
			* Set the unsigned short value of the dimension specified by the order parameter
			* \param order Dimension whose value should be set
			* \param value New value of the dimension
			* \invariant order < tuple dimension
			*/
			void cTuple::SetValue(unsigned int order, unsigned short value, const cDTDescriptor* pSd)
			{
				*(unsigned short*)(mData + ((cSpaceDescriptor*)pSd)->GetDimensionOrder(order)) = value;
			}

			inline float cTuple::GetFloat(unsigned int order, const cDTDescriptor* pSd) const
			{
				return *(float*)(mData + ((cSpaceDescriptor*)pSd)->GetDimensionOrder(order));
			}

			inline double cTuple::GetDouble(unsigned int order, const cDTDescriptor* pSd) const
			{
				printf("cTuple::GetDouble - double type not supported!\n");
				return (double)0.0;
			}

			/**
			* Return the int value of the dimension specified by the order parameter
			* \param order Dimension whose value should be returned
			* \return int value of the dimension
			* \invariant order < tuple dimension
			* \invariant value type in the dimension has to be int
			*/
			inline int cTuple::GetInt(unsigned int order, const cDTDescriptor* pSd) const
			{
				return *(int*)(mData + ((cSpaceDescriptor*)pSd)->GetDimensionOrder(order));
			}

			/**
			* Return the unsigned int value of the dimension specified by the order parameter
			* \param order Dimension whose value should be returned
			* \return unsigned int value of the dimension
			* \invariant order < tuple dimension
			* \invariant value type in the dimension has to be unsigned int
			*/
			inline unsigned int cTuple::GetUInt(unsigned int order, const cDTDescriptor* pSd) const
			{
				return *(unsigned int*)(mData + ((cSpaceDescriptor*)pSd)->GetDimensionOrder(order));
			}

			/**
			* Return the byte value of the dimension specified by the order parameter
			* \param order Dimension whose value should be returned
			* \return byte value of the dimension
			* \invariant order < tuple dimension
			* \invariant value type in the dimension has to be char (byte)
			*/
			inline char cTuple::GetByte(unsigned int order, const cDTDescriptor* pSd) const
			{
				return *(mData + ((cSpaceDescriptor*)pSd)->GetDimensionOrder(order));
			}

			/**
			* Return the byte value of the dimension specified by the order parameter
			* \param order Dimension whose value should be returned
			* \return byte value of the dimension
			* \invariant order < tuple dimension
			* \invariant value type in the dimension has to be char (byte)
			*/
			inline unsigned char cTuple::GetUChar(unsigned int order, const cDTDescriptor* pSd) const
			{
				return *(unsigned char*)(mData + ((cSpaceDescriptor*)pSd)->GetDimensionOrder(order));
			}


			/**
			* Return the unicode char value of the dimension specified by the order parameter
			* \param order Dimension whose value should be returned
			* \return byte value of the dimension
			* \invariant order < tuple dimension
			* \invariant value type in the dimension has to be char (byte)
			*/
			inline wchar_t cTuple::GetWChar(unsigned int order, const cDTDescriptor* pSd) const
			{
				return *(mData + ((cSpaceDescriptor*)pSd)->GetDimensionOrder(order));
			}

			


			/**
			* Return the short value of the dimension specified by the order parameter
			* \param order Dimension whose value should be returned
			* \return short value of the dimension
			* \invariant order < tuple dimension
			* \invariant value type in the dimension has to be short
			*/
			inline short cTuple::GetShort(unsigned int order, const cDTDescriptor* pSd) const
			{
				return *(short*)(mData + ((cSpaceDescriptor*)pSd)->GetDimensionOrder(order));
			}

			/**
			* Return the short value of the dimension specified by the order parameter
			* \param order Dimension whose value should be returned
			* \return short value of the dimension
			* \invariant order < tuple dimension
			* \invariant value type in the dimension has to be short
			*/
			inline unsigned short cTuple::GetUShort(unsigned int order, const cDTDescriptor* pSd) const
			{
				return *(unsigned short*)(mData + ((cSpaceDescriptor*)pSd)->GetDimensionOrder(order));
			}

			/**
			* Return the cString value of the dimension
			* \param order Dimension whose value should be returned
			* \param string returned value
			*/
			inline void cTuple::GetString(unsigned int order, cString &string, const cDTDescriptor* pSd) const
			{
				const int STRING_LENGTH = 128;
				char str[STRING_LENGTH];

				switch (((cSpaceDescriptor*)pSd)->GetDimensionTypeCode(order))
				{
				case cInt::CODE:
					// mk: snprintf((char*)str, STRING_LENGTH, "%d", GetShort(order, pSd));
					// but snprintf is not supported by Visual Studio, we can use: _snprintf or _snprintf_s
					sprintf((char*)str, "%d", GetInt(order, pSd));
					break;
				case cShort::CODE:
					sprintf((char*)str, "%d", GetShort(order, pSd));
					break;
				case cChar::CODE:
					sprintf((char*)str, "%c", this->GetByte(order, pSd));
					break;
				case cFloat::CODE:
					sprintf((char*)str, "%f", this->GetByte(order, pSd));
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
			inline float* cTuple::GetPFloat(unsigned int order, const cDTDescriptor* pSd) const
			{
				printf("cTuple::GetPFloat - float type not supported!\n");
				return NULL;
			}

			/**
			* Return the int value of the dimension specified by the order parameter by reference
			* \param order Dimension whose value should be returned
			* \return int value of the dimension by reference
			* \invariant order < tuple dimension
			* \invariant value type in the dimension has to be int
			*/
			inline int* cTuple::GetPInt(unsigned int order, const cDTDescriptor* pSd) const
			{
				return (int*)(mData + ((cSpaceDescriptor*)pSd)->GetDimensionOrder(order));
			}

			/**
			* Return the unsigned int value of the dimension specified by the order parameter by reference
			* \param order Dimension whose value should be returned
			* \return unsigned int value of the dimension by reference
			* \invariant order < tuple dimension
			* \invariant value type in the dimension has to be unsigned int
			*/
			inline unsigned int* cTuple::GetPUInt(unsigned int order, const cDTDescriptor* pSd) const
			{
				return (unsigned int*)(mData + ((cSpaceDescriptor*)pSd)->GetDimensionOrder(order));
			}

			/**
			* Return the byte value of the dimension specified by the order parameter by reference
			* \param order Dimension whose value should be returned
			* \return byte value of the dimension by reference
			* \invariant order < tuple dimension
			* \invariant value type in the dimension has to be char (byte)
			*/
			inline char* cTuple::GetPByte(unsigned int order, const cDTDescriptor* pSd) const
			{
				return (mData + ((cSpaceDescriptor*)pSd)->GetDimensionOrder(order));
			}

			/**
			* Return the byte value of the dimension specified by the order parameter by reference
			* \param order Dimension whose value should be returned
			* \return byte value of the dimension by reference
			* \invariant order < tuple dimension
			* \invariant value type in the dimension has to be char (byte)
			*/
			inline unsigned char* cTuple::GetPUChar(unsigned int order, const cDTDescriptor* pSd) const
			{
				return (unsigned char*)(mData + ((cSpaceDescriptor*)pSd)->GetDimensionOrder(order));
			}

			/**
			* Read tuple from stream.
			*/
			inline bool cTuple::Read(cStream *stream, const cSpaceDescriptor* pSd)
			{
				return stream->Read(mData, pSd->GetSize());
			}

			/**
			* Write tuple into stream.
			*/
			inline bool cTuple::Write(cStream *stream, const cSpaceDescriptor* pSd) const
			{
				return stream->Write(mData, pSd->GetSize());
			}

			/**
			 * Write the tuple stored in data into stream as text.
			 * Warning: It is for cUInt only.
			 */
			inline void cTuple::WriteAsText(char* data, cStream *stream, const cSpaceDescriptor* pSd)
			{
				unsigned int dim = pSd->GetDimension();//4; /* !!!7!!! */
				const unsigned int BUFFER_SIZE = 20;
				char buffer[BUFFER_SIZE];
				unsigned int size;

				for (unsigned int i = 0; i < dim; i++)
				{
					cUInt::ToString(cTuple::GetUInt(data, i, pSd), buffer, size);
					stream->Write(buffer, size);
					if (i < dim - 1)
					{
						stream->Write((char*)",", sizeof(char));
					}
				}
				stream->Write((char*)"\r", sizeof(char));
				stream->Write((char*)"\n", sizeof(char));
			}

			/**
			* Set max value in the order-th coordinate.
			*/
			inline void cTuple::SetMaxValue(unsigned int order, const cSpaceDescriptor* pSd)
			{
				//			printf("cTuple::SetMaxValue - method not supported. Can be implemented ...\n");
				switch (pSd->GetDimensionTypeCode(order))
				{
				case cFloat::CODE: SetValue(order, cFloat::MAX, pSd);  break;
				case cUInt::CODE: SetValue(order, cUInt::MAX, pSd);  break; //PB: cUInt::MAX returns incorrect value -1
				case cInt::CODE: SetValue(order, cInt::MAX, pSd);  break;
				case cUShort::CODE: SetValue(order, cUShort::MAX, pSd);  break;
				default: printf("cTuple::SetMaxValue - method not supported. Can be implemented ...\n"); break;
				}
			}

			/**
			* Compare z-values.
			* \return
			*		- -1 if the this < tuple
			*		- 0 if the this == tuple
			*		- 1 if the this > tuple
			*/
			int cTuple::CompareZOrder(const char* tuple1, const char* tuple2, cSpaceDescriptor* pSd)
			{
				return pSd->CompareNormalizedZValues(tuple1, tuple2, pSd); // Ondrej Prda added 12.6.2015
			}

			/**
			* Compare hilbert values.
			* \return
			*		- -1 if the this < tuple
			*		- 0 if the this == tuple
			*		- 1 if the this > tuple
			*/
			int cTuple::CompareHOrder(const char* tuple1, const char* tuple2, cSpaceDescriptor* pSd)
			{
				return pSd->CompareNormalizedHValues(tuple1, tuple2, pSd); // Ondrej Prda added 12.6.2015
			}

			/**
			* Compare taxi ordering values (from the beginning of the space).
			* \return
			*		- -1 if the this < tuple
			*		- 0 if the this == tuple
			*		- 1 if the this > tuple
			*/
			inline int cTuple::CompareTaxiOrder(const char* tuple1, const char* tuple2, const cSpaceDescriptor* pSd)
			{
				double d1 = ComputeTaxiDistance(tuple1, pSd);
				double d2 = ComputeTaxiDistance(tuple2, pSd);
				int res = 0;

				if (d1 < d2)
				{
					res = -1;
				}
				else if (d1 > d2)
				{
					res = 1;
				}

				return res;
			}

			/**
			* Compute Taxi distance (from the beginning of the space).
			* \return distance
			*/
			inline double cTuple::ComputeTaxiDistance(const char* data, const cSpaceDescriptor* pSd)
			{
				double distance = 0.0;
				unsigned int dim = pSd->GetDimension();

				assert(pSd->GetDimensionTypeCode(0) == cUInt::CODE);

				for (unsigned int i = 0; i < dim; i++)
				{
					distance += (double) GetUInt(data, i, pSd) / 4294967295;
				}
				return distance;
			}

			/**
			* Compare the tuple values in every dimension starting from first until values in dimensions are different.
			* This is the same method as Compare.
			* \return the return value correponds to the first different dimension in tuples.
			*		- -1 if this tuple has lower value
			*		- 0 if the tuples are the same
			*		- 1 if this tuple is bigger
			*/
			inline int cTuple::Equal(const cTuple &tuple, const cSpaceDescriptor* pSd) const
			{
				return Compare(tuple, pSd);
			}

			inline int cTuple::Equal(const char* tuple2, const cSpaceDescriptor* pSd) const
			{
				// you must check dimension a typeSize
				return CompareLexicographically(tuple2, pSd);
			}

			/**
			* Return real dimension of tuple. Real means the all zero last coordinates are cutted.
			* \param high sets the highest coordinates where zeros are searched
			*/
			inline unsigned int cTuple::GetRealDimension(unsigned int high, const cSpaceDescriptor* pSd) const
			{
				int lo = 0, hi = high - 1;
				unsigned int realdim = 0;
				for (; ; )
				{
					int index = (lo + hi) / 2;
					if (IsZero(index, pSd))
					{
						if (index > 0 && !IsZero(index - 1, pSd))
						{
							realdim = index;
							break;
						}
						else
						{
							hi = index - 1;

							if (lo > hi)
							{
								realdim = index;
								break;
							}
						}
					}
					else
					{
						lo = index + 1;
						if (lo > hi)
						{
							realdim = index + 1;
							break;
						}
					}
				}
				return realdim;
			}

			/**
			* \return true when all bytes of mData are zero.
			*/
			inline bool cTuple::IsZero(const cSpaceDescriptor* pSd) const
			{
				unsigned int size = pSd->GetSize();

				for (unsigned int i = 0; i < size; i++)
				{
					if (mData[i] != 0)
					{
						return false;
					}
				}
				return true;
			}

			/**
			* Test for minimal value.
			* \param order Specify order of the item within this tuple.
			* \return true when the item is zero.
			*/
			inline bool cTuple::IsZero(unsigned int order, const cSpaceDescriptor* pSd) const
			{
				bool ret = false;

				switch (pSd->GetDimensionTypeCode(order))
				{
				case cInt::CODE:
					ret = (GetInt(order, pSd) == (int)0);
					break;
				case cUInt::CODE:
					ret = (GetUInt(order, pSd) == (short)0);
					break;
				case cChar::CODE:
					ret = (GetByte(order, pSd) == (char)0);
					break;
				case cFloat::CODE:
					ret = (GetFloat(order, pSd) == (float)0);
					break;
				}
				return ret;
			}

			/**
			* Equality test of order-th coordinate.
			* \return -1 if this < tuple, 0 if tuples' coordinates are the same, 1 if this > tuple.
			*/
			inline int cTuple::Equal(const cTuple &tuple, unsigned int order, const cSpaceDescriptor* pSd) const
			{
				assert(order < pSd->GetDimension());
				int ret = 1;

				//val644 - start - increment countCompare
				cTuple::countCompare++;
				cTuple::countCompareLevel[cTuple::levelTree]++;
				//val644 - end - increment countCompare

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
				case cFloat::CODE:
					if ((float)GetFloat(order, pSd) < (float)tuple.GetFloat(order, pSd))
					{
						ret = -1;
					}
					else if (GetFloat(order, pSd) == tuple.GetFloat(order, pSd))
					{
						ret = 0;
					}
					break;
				case cInt::CODE:
					if (GetInt(order, pSd) < tuple.GetInt(order, pSd))
					{
						ret = -1;
					}
					else if (GetInt(order, pSd) == tuple.GetInt(order, pSd))
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

			inline void cTuple::SetTuple(char* data, const cTuple &tuple, const cSpaceDescriptor* pSd)
			{
				memcpy(data, tuple.GetData(), pSd->GetSize());
			}

			inline void cTuple::SetTuple(char* dst_data, const char* src_data, const cSpaceDescriptor* pSd)
			{
				memcpy(dst_data, src_data, pSd->GetSize());
			}

			inline cTuple* cTuple::GetValue(char* value, const cDTDescriptor* pSd)
			{
				cTuple* ret = new cTuple((cSpaceDescriptor*)pSd);
				ret->Copy(value, pSd);
				return ret;
			}

			inline cTuple* cTuple::GetValueC(const char* value, const cDTDescriptor* pSd)
			{
				cTuple* ret = new cTuple((cSpaceDescriptor*)pSd);
				ret->Copy(value, pSd);
				return ret;
			}

			inline void cTuple::Copy(char* cUnfTuple_dst, const char* cUnfTuple_src, const cDTDescriptor *pSd)
			{
				memcpy(cUnfTuple_dst, cUnfTuple_src, GetSize(NULL, pSd));
			}

			inline unsigned int cTuple::GetUInt(const char* data, unsigned int order, const cSpaceDescriptor* pSd)
			{
				return *(((unsigned int*)data) + order);
			}

			inline int cTuple::GetInt(const char* data, unsigned int order, const cSpaceDescriptor* pSd)
			{
				return *(((int*)data) + order);
			}

			inline float cTuple::GetFloat(const char* data, unsigned int order, const cSpaceDescriptor* pSd)
			{
				return *(((float*)data) + order);
			}

			inline unsigned short cTuple::GetUShort(const char* data, unsigned int order, const cSpaceDescriptor* pSd)
			{
				return *(((unsigned short*)data) + order);
			}

			inline short cTuple::GetShort(const char* data, unsigned int order, const cSpaceDescriptor* pSd)
			{
				return *(((short*)data) + order);
			}

			inline unsigned char cTuple::GetUByte(const char* data, unsigned int order, const cSpaceDescriptor* pSd)
			{
				return *(((unsigned char*)data) + order);
			}

			inline char cTuple::GetByte(const char* data, unsigned int order, const cSpaceDescriptor* pSd)
			{
				return *(((char*)data) + order);
			}

			inline unsigned char cTuple::GetUChar(const char* data, unsigned int order, const cDTDescriptor* pSd)
			{
				return *(((char*)data) + order);
			}

			inline char* cTuple::GetPChar(const char* data, unsigned int order, const cDTDescriptor* pSd)
			{
				return ((char*)data) + order;
			}

			inline void cTuple::SetValue(char *data, unsigned int order, int value, const cSpaceDescriptor* pSd)
			{
				*(((int*)data) + order) = value;
			}

			inline void cTuple::SetValue(char *data, unsigned int order, unsigned int value, const cSpaceDescriptor* pSd)
			{
				*(((unsigned int*)data) + order) = value;
			}

			inline void cTuple::SetValue(char *data, unsigned int order, unsigned short value, const cSpaceDescriptor* pSd)
			{
				*(((unsigned short*)data) + order) = value;
			}

			inline void cTuple::SetValue(char *data, unsigned int order, unsigned char value, const cSpaceDescriptor* pSd)
			{
				*(((unsigned char*)data) + order) = value;
			}

			inline void cTuple::SetValue(char *data, unsigned int order, float value, const cSpaceDescriptor* pSd)
			{
				*(((float*)data) + order) = value;
			}

			inline void cTuple::SetValue(char *data, unsigned int order, char value, const cSpaceDescriptor* pSd)
			{
				*(data + order) = value;
			}

			inline void cTuple::SetValue(unsigned int order, char* cTuple_value, const cDTDescriptor* pSd)
			{
				// get the correct inner sd for the tuple
				cSpaceDescriptor *sd = ((cSpaceDescriptor*)pSd)->GetDimSpaceDescriptor(0);  // the correct versions: GetInnerSpaceDescriptor(order) or GetInnerSpaceDescriptor()
				unsigned int size = cTuple::GetSize(cTuple_value, sd);//dřive sd
				memcpy(mData + order * size, cTuple_value, size);
			}

			inline char* cTuple::GetTuple(unsigned int order, const cDTDescriptor* pSd)
			{
				cSpaceDescriptor *sd = ((cSpaceDescriptor*)pSd)->GetDimSpaceDescriptor(0);
				unsigned int size = cTuple::GetSize(NULL, sd);
				return mData + (order * size);
			}

			inline char* cTuple::GetTuple(char* data, unsigned int order, const cDTDescriptor* pSd)
			{
				cSpaceDescriptor *sd = ((cSpaceDescriptor*)pSd)->GetDimSpaceDescriptor(0);
				unsigned int size = cTuple::GetSize(NULL, sd);
				return data + (order * size);
			}


			inline unsigned int cTuple::GetSize(const cDTDescriptor* pSd) const
			{
				return cTuple::GetSize(mData, pSd);
			}

			inline unsigned int cTuple::GetSize_instance(const char *data, const cDTDescriptor *pDtd) const
			{
				return cTuple::GetSize(data, pDtd);
			}

			/**
			* \Return The size of the tuple in memory when serialized
			*/
			inline unsigned int cTuple::GetSize(uint tupleSize) const
			{
				return tupleSize;
			}

			inline unsigned int cTuple::GetMaxSize(const cDTDescriptor* pSd) const
			{
				return cTuple::GetSize(mData, pSd);
			}

			/* Return the size of tuple with specified length */
			unsigned int cTuple::GetLSize(uint tupleLength, const cDTDescriptor* dtd)
			{
				cSpaceDescriptor *sd = (cSpaceDescriptor *)dtd;
				return sd->GetLSize(tupleLength);
			}

			inline uint cTuple::GetDimension(const cDTDescriptor* dtd)
			{
				return ((cSpaceDescriptor *)dtd)->GetDimension();
			}

			/**
			 * Return the the size of the static representation.
			 */
			inline unsigned int cTuple::GetSize(const char *data, const cDTDescriptor* pSd)
			{
				UNUSED(data);
				return ((cSpaceDescriptor*)pSd)->GetSize();
			}

			/**
			 * In this case, this method id the same as GetSize.
			 */
			inline unsigned int cTuple::GetMaxSize(const char *data, const cDTDescriptor* pSd)
			{
				return GetSize(data, pSd);
			}

			inline unsigned int cTuple::HashValue(char *tuple, uint hashTableSize, const cDTDescriptor* dtd)
			{
				cSpaceDescriptor *sd = (cSpaceDescriptor*)dtd;
				unsigned int dim = sd->GetDimension();
				return sd->GetDimensionType(0)->HashValue(tuple, dim, hashTableSize);

				/*
				unsigned int hashValue = 0;
				cSpaceDescriptor *sd = (cSpaceDescriptor*)dtd;
				unsigned int dim = sd->GetDimension();
				uint m2 = hashTableSize - 1;*/

				/*
			The hash function is of the form h(x, i) = (h1(x) + (i ? 1) • h2(x)) mod m for the ith trial, where
			h1(x) = x mod m, h2(x) = 1 + (x mod m?), m is a prime number, and m? = m ? 1.
				*/

				/*
				for (unsigned int i = 0 ; i < dim ; i++)
				{
					uint value = GetUInt(tuple, i, sd);
					uint h1 = value % hashTableSize;
					uint h2 = 1 + (value & m2);
					uint tmp = h1 + i * h2;
					hashValue += tmp;
				}
				return hashValue % hashTableSize;
				*/
			}

			inline unsigned int cTuple::HashValue(uint hashTableSize, const cDTDescriptor* dtd) const
			{
				return HashValue(mData, hashTableSize, dtd);
			}

			/**
			* Equality test of order-th coordinate.
			* \return -1 if this < tuple, 0 if tuples' coordinates are the same, 1 if this > tuple.
			*/
			inline int cTuple::Equal(const char* cUnfTuple_t1, const char* cUnfTuple_t2, const unsigned int order, const cSpaceDescriptor* pSd)
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
				case cFloat::CODE:
					if ((float)GetFloat(cUnfTuple_t1, order, pSd) < (float)GetFloat(cUnfTuple_t2, order, pSd))
					{
						ret = -1;
					}
					else if (GetFloat(cUnfTuple_t1, order, pSd) == GetFloat(cUnfTuple_t2, order, pSd))
					{
						ret = 0;
					}
					break;
				case cInt::CODE:
					if (GetInt(cUnfTuple_t1, order, pSd) < GetInt(cUnfTuple_t2, order, pSd))
					{
						ret = -1;
					}
					else if (GetInt(cUnfTuple_t1, order, pSd) == GetInt(cUnfTuple_t2, order, pSd))
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
			inline int cTuple::Equal(const char* cUnfTuple_t1, const char* cUnfTuple_t2, const unsigned int order1, const unsigned int order2, const cSpaceDescriptor* pSd)
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
				case cFloat::CODE:
					if ((float)GetFloat(cUnfTuple_t1, order1, pSd) < (float)GetFloat(cUnfTuple_t2, order2, pSd))
					{
						ret = -1;
					}
					else if (GetFloat(cUnfTuple_t1, order1, pSd) == GetFloat(cUnfTuple_t2, order2, pSd))
					{
						ret = 0;
					}
					break;
				case cInt::CODE:
					if (GetInt(cUnfTuple_t1, order1, pSd) < GetInt(cUnfTuple_t2, order2, pSd))
					{
						ret = -1;
					}
					else if (GetInt(cUnfTuple_t1, order1, pSd) == GetInt(cUnfTuple_t2, order2, pSd))
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
			* Warning: the second tuple is cLNTuple!
			*/

			inline int cTuple::Equal(const char* cTuple_t1, const unsigned int order1, const cSpaceDescriptor* sd1,
				char* cLNTuple_t2, const unsigned int order2, const cSpaceDescriptor* sd2)
			{
				//char typeCode = sd1->GetTypeCode(order1);
				//assert(typeCode == sd2->GetTypeCode(order2));
				// nechat: assert(order < mDimension);
				int ret = 1;

				//if (typeCode == cUInt::CODE)
				//{
				if (GetUInt(cTuple_t1, order1, sd1) < cLNTuple::GetUInt(cLNTuple_t2, order2, sd2))
				{
					ret = -1;
				}
				else if (GetUInt(cTuple_t1, order1, sd1) == cLNTuple::GetUInt(cLNTuple_t2, order2, sd2))
				{
					ret = 0;
				}
				return ret;
				//}
				//else if (typeCode == cUShort::CODE)
				//{
				//	if (GetUShort(cTuple_t1, order1, sd1) < cLNTuple::GetUShort(cLNTuple_t2, order2, sd2))
				//	{
				//		ret = -1;
				//	}
				//	else if (GetUShort(cTuple_t1, order1, sd1) == cLNTuple::GetUShort(cLNTuple_t2, order2, sd2))
				//	{
				//		ret = 0;
				//	}
				//	return ret;
				//}
				//else if (typeCode == cChar::CODE)
				//{
				//	if ((unsigned char)GetByte(cTuple_t1, order1, sd1) < (unsigned char)cLNTuple::GetByte(cLNTuple_t2, order2, sd2))
				//	{
				//		ret = -1;
				//	}
				//	else if (GetByte(cTuple_t1, order1, sd1) == cLNTuple::GetByte(cLNTuple_t2, order2, sd2))
				//	{
				//		ret = 0;
				//	}
				//}
				//else if (typeCode == cFloat::CODE)
				//{
				//	if ((float)GetFloat(cTuple_t1, order1, sd1) < (float)cLNTuple::GetFloat(cLNTuple_t2, order2, sd2))
				//	{
				//		ret = -1;
				//	}
				//	else if (GetFloat(cTuple_t1, order1, sd1) == cLNTuple::GetFloat(cLNTuple_t2, order2, sd2))
				//	{
				//		ret = 0;
				//	}
				//}
				//else if (typeCode == cInt::CODE)
				//{
				//	if (GetInt(cTuple_t1, order1, sd1) < cLNTuple::GetInt(cLNTuple_t2, order2, sd2))
				//	{
				//		ret = -1;
				//	}
				//	else if (GetInt(cTuple_t1, order1, sd1) == cLNTuple::GetInt(cLNTuple_t2, order2, sd2))
				//	{
				//		ret = 0;
				//	}
				//}
				//return ret;
			}

			// Method codes values in sourceBuffer by chosen algorithm
			// Method aligns to bytes!!!
			inline uint cTuple::Encode(uint method, const char* sourceBuffer, char* encodedBuffer, const cDTDescriptor* dtd, uint tupleLength)
			{
				cSpaceDescriptor *sd = (cSpaceDescriptor *)dtd;
				uint ret = Coder::encode(method, sd->GetDimensionSize(0), sourceBuffer, encodedBuffer, (tupleLength == NOT_DEFINED) ? sd->GetDimension() : tupleLength);
				return cNumber::BitsToBytes(ret);
			}

			// Method decodes values in sourceBuffer by chosen algorithm
			inline uint cTuple::Decode(uint method, char* encodedBuffer, char* decodedBuffer, const cDTDescriptor* dtd, uint tupleLength)
			{
				cSpaceDescriptor *sd = (cSpaceDescriptor *)dtd;
				return Coder::decode(method, sd->GetDimensionSize(0), encodedBuffer, decodedBuffer, (tupleLength == NOT_DEFINED) ? sd->GetDimension() : tupleLength);
			}

			// Method estimates the estimate size of tuple
			// Method aligns to bytes!!!
			inline uint cTuple::GetEncodedSize(uint method, char* sourceBuffer, const cDTDescriptor* dtd, uint tupleLength)
			{
				cSpaceDescriptor *sd = (cSpaceDescriptor *)dtd;
				uint dim = (tupleLength == NOT_DEFINED) ? sd->GetDimension() : tupleLength;

				return cNumber::BitsToBytes(Coder::GetSize(method, sd->GetDimensionSize(0), sourceBuffer, dim));
				//uint bitSize = 0;
				//for (uint j = 0; j < dim; j++)
				//{
				//	bitSize += Coder::GetSize(GetUInt(sourceBuffer, j, sd));
				//}
				//return cNumber::BitsToBytes(bitSize)/* bitSize*/;
			}

			// DO NOT USE IT !!! Coder::estimateSizeInBits(...) != Coder::encode(...)
			inline uint cTuple::EncodedSize(uint method, char* sourceBuffer, const cDTDescriptor* dtd)
			{
				cSpaceDescriptor *sd = (cSpaceDescriptor *)dtd;
				unsigned int ret = Coder::estimateSizeInBits(method, sd->GetDimensionSize(0), sourceBuffer, sd->GetDimension());
				return ret;
			}

			/// Compute non euclidian distance between tuples $t1 and $t2 in dimension $order
			inline double cTuple::TaxiCabDistanceValue(const char* cTuple_t1, const char* cTuple_t2, unsigned int order, const cSpaceDescriptor* pSd)
			{
				double sum = 0;

				switch (pSd->GetDimensionTypeCode(order))
				{
				case cFloat::CODE:
					sum += ((GetFloat(cTuple_t1, order, pSd) > GetFloat(cTuple_t2, order, pSd)) ? (GetFloat(cTuple_t1, order, pSd) - GetFloat(cTuple_t2, order, pSd)) : (GetFloat(cTuple_t2, order, pSd) - GetFloat(cTuple_t1, order, pSd)));
					break;
				case cUInt::CODE:
					sum += ((GetUInt(cTuple_t1, order, pSd) > GetUInt(cTuple_t2, order, pSd)) ? (GetUInt(cTuple_t1, order, pSd) - GetUInt(cTuple_t2, order, pSd)) : (GetUInt(cTuple_t2, order, pSd) - GetUInt(cTuple_t1, order, pSd)));
					break;
				case cInt::CODE:
					sum += ((GetInt(cTuple_t1, order, pSd) > GetInt(cTuple_t2, order, pSd)) ? (GetInt(cTuple_t1, order, pSd) - GetInt(cTuple_t2, order, pSd)) : (GetInt(cTuple_t2, order, pSd) - GetInt(cTuple_t1, order, pSd)));
					break;
				case cUShort::CODE:
					sum += ((GetUShort(cTuple_t1, order, pSd) > GetUShort(cTuple_t2, order, pSd)) ? (GetUShort(cTuple_t1, order, pSd) - GetUShort(cTuple_t2, order, pSd)) : (GetUShort(cTuple_t2, order, pSd) - GetUShort(cTuple_t1, order, pSd)));
					break;
				case cShort::CODE:
					sum += ((GetShort(cTuple_t1, order, pSd) > GetShort(cTuple_t2, order, pSd)) ? (GetShort(cTuple_t1, order, pSd) - GetShort(cTuple_t2, order, pSd)) : (GetShort(cTuple_t2, order, pSd) - GetShort(cTuple_t1, order, pSd)));
					break;
				case cChar::CODE:
					printf("cTuple::TaxiDistance - method not supported for char.");
					break;
				}

				return sum;
			}

			inline void cTuple::Add(const char* cTuple_t1, const char* cTuple_t2, char* cTuple_result, unsigned int order, const cSpaceDescriptor* pSd)
			{
				switch (pSd->GetDimensionTypeCode(order))
				{
				case cUInt::CODE:
					SetValue(cTuple_result, order, GetUInt(cTuple_t1, order, pSd) + GetUInt(cTuple_t2, order, pSd), pSd);
					break;
				case cInt::CODE:
					SetValue(cTuple_result, order, GetInt(cTuple_t1, order, pSd) + GetInt(cTuple_t2, order, pSd), pSd);
					break;
				case cUShort::CODE:
					SetValue(cTuple_result, order, GetUShort(cTuple_t1, order, pSd) + GetUShort(cTuple_t2, order, pSd), pSd);
					break;
				case cShort::CODE:
					SetValue(cTuple_result, order, GetShort(cTuple_t1, order, pSd) + GetShort(cTuple_t2, order, pSd), pSd);
					break;
				case cChar::CODE:
					SetValue(cTuple_result, order, GetByte(cTuple_t1, order, pSd) + GetByte(cTuple_t2, order, pSd), pSd);
					break;
				case cFloat::CODE:
					SetValue(cTuple_result, order, GetFloat(cTuple_t1, order, pSd) + GetFloat(cTuple_t2, order, pSd), pSd);
					break;
				}
			}


			inline void cTuple::Subtract(const char* cTuple_t1, const char* cTuple_t2, char* cTuple_result, unsigned int order, const cSpaceDescriptor* pSd)
			{
				switch (pSd->GetDimensionTypeCode(order))
				{
				case cUInt::CODE:
					SetValue(cTuple_result, order, GetUInt(cTuple_t1, order, pSd) - GetUInt(cTuple_t2, order, pSd), pSd);
					break;
				case cInt::CODE:
					SetValue(cTuple_result, order, GetInt(cTuple_t1, order, pSd) - GetInt(cTuple_t2, order, pSd), pSd);
					break;
				case cUShort::CODE:
					SetValue(cTuple_result, order, GetUShort(cTuple_t1, order, pSd) - GetUShort(cTuple_t2, order, pSd), pSd);
					break;
				case cShort::CODE:
					SetValue(cTuple_result, order, GetShort(cTuple_t1, order, pSd) - GetShort(cTuple_t2, order, pSd), pSd);
					break;
				case cChar::CODE:
					SetValue(cTuple_result, order, GetByte(cTuple_t1, order, pSd) - GetByte(cTuple_t2, order, pSd), pSd);
					break;
				case cFloat::CODE:
					SetValue(cTuple_result, order, GetFloat(cTuple_t1, order, pSd) - GetFloat(cTuple_t2, order, pSd), pSd);
					break;
				}
			}

			/**
			* Equality test of order-th coordinate.
			* \return -1 if this < tuple, 0 if tuples' coordinates are the same, 1 if this > tuple.
			*/
			//inline int cTuple::Equal(const char* tuple2, unsigned int order) const
			//{
			//	assert(mTypeSize == GetTypeSize(tuple2));
			//	int ret = 1;
			//
			//	switch (mTypeSize)
			//	{
			//	case 4:
			//		if (GetUInt(order) < GetUInt(tuple2, order))
			//		{
			//			ret = -1;
			//		}
			//		else if (GetUInt(order) == GetUInt(tuple2, order))
			//		{
			//			ret = 0;
			//		}
			//		break;
			//	case 2:
			//		if (GetUShort(order) < GetUShort(tuple2, order))
			//		{
			//			ret = -1;
			//		}
			//		else if (GetUShort(order) == GetUShort(tuple2, order))
			//		{
			//			ret = 0;
			//		}
			//		break;
			//	case 1:
			//		if ((unsigned char)GetByte(order) < (unsigned char)GetByte(tuple2, order))
			//		{
			//			ret = -1;
			//		}
			//		else if (GetByte(order) == GetByte(tuple2, order))
			//		{
			//			ret = 0;
			//		}
			//		break;
			//	}
			//	return ret;
			//}

			/**
			 * The size of the instance.
			 */
			inline int cTuple::GetObjectSize(const cSpaceDescriptor *pSd)
			{
				return sizeof(cTuple) + GetSize(NULL, pSd);
			}

			/*
			 * When tuple is allocated in the pool, you need to set the pointer of mData.
			 * ?? What about destructor? It is necessary to set mData = NULL before the
			 * destructor is invoked.
			 */
			char* cTuple::Init(char* mem)
			{
				mData = mem + sizeof(mData);
				return mData;
			}


			/// t1 + t2 operator
			inline char* cTuple::MergeTuple(const char* cBitString_Mask, const char* cTuple_RI, const char* cTuple_Key, char* cTuple_Result, const cDTDescriptor* pSd)
			{
				const cSpaceDescriptor* spaceDescriptor = ((cSpaceDescriptor*)pSd);
				unsigned int dim = spaceDescriptor->GetDimension();
				int order = 0;

				for (unsigned int i = 0; i < dim; i++)
				{
					if (/*cBitString*/cBitArray::GetBit(cBitString_Mask, i))
						SetValue(cTuple_Result, i, GetUInt(cTuple_RI, i, spaceDescriptor), spaceDescriptor);
					else
						SetValue(cTuple_Result, i, GetUInt(cTuple_RI, i, spaceDescriptor) + GetUInt(cTuple_Key, order++, spaceDescriptor), spaceDescriptor);
				}

				return cTuple_Result;
			}

			/// t1 - t2 operator
			inline uint cTuple::CutTuple(const char* cBitString_Mask, const char* cTuple_RI, const char* cTuple_Key, char* cTuple_Result, const cDTDescriptor* pSd)
			{
				const cSpaceDescriptor* spaceDescriptor = ((cSpaceDescriptor*)pSd);
				uint dim = ((cSpaceDescriptor*)pSd)->GetDimension();
				uint resultLength = 0;

				for (unsigned int i = 0; i < dim; i++)
				{
					if (/*cBitString*/cBitArray::GetBit(cBitString_Mask, i) == 0)
						SetValue(cTuple_Result, resultLength++, GetUInt(cTuple_Key, i, spaceDescriptor) - GetUInt(cTuple_RI, i, spaceDescriptor), spaceDescriptor);
				}

				return resultLength;
			}

			// if tuple1 is same as tuple2 in dimension i, then set mask 1, otherwise 0
			inline char* cTuple::SetMask(const char* cBitString_Mask, const char* cTuple_RI, const char* cTuple_Key, char* cBitString_Result, const cDTDescriptor* pSd)
			{
				uint dim = ((cSpaceDescriptor*)pSd)->GetDimension();

				for (uint i = 0; i < dim; i++)
				{
					cBitString::SetBit(cBitString_Result, i, /*cBitString*/cBitArray::GetBit(cBitString_Mask, i) && !abs(Equal(cTuple_RI, cTuple_Key, i, i, (cSpaceDescriptor*)pSd)));
				}

				return cBitString_Result;
			}

			inline char* cTuple::SetMask(const char* cTuple_t1, const char* cTuple_t2, char* cBitString_Result, const cDTDescriptor* pSd)
			{
				const cSpaceDescriptor* spaceDescriptor = ((cSpaceDescriptor*)pSd);
				uint dim = ((cSpaceDescriptor*)pSd)->GetDimension();

				for (uint i = 0; i < dim; i++)
				{
					cBitString::SetBit(cBitString_Result, i, GetUInt(cTuple_t1, i, spaceDescriptor) == GetUInt(cTuple_t2, i, spaceDescriptor));
				}

				return cBitString_Result;
			}

			inline char* cTuple::SetMask(const char* cTuple_t1, const char* cBitString_Mask1, const char* cTuple_t2, const char* cBitString_Mask2, char* cBitString_Result, const cDTDescriptor* pSd)
			{
				const cSpaceDescriptor* spaceDescriptor = ((cSpaceDescriptor*)pSd);
				uint dim = ((cSpaceDescriptor*)pSd)->GetDimension();

				for (uint i = 0; i < dim; i++)
				{
					cBitString::SetBit(cBitString_Result, i, (/*cBitString*/cBitArray::GetBit(cBitString_Mask1, i) && /*cBitString*/cBitArray::GetBit(cBitString_Mask2, i) && (GetUInt(cTuple_t1, i, spaceDescriptor) == GetUInt(cTuple_t2, i, spaceDescriptor))));
				}

				return cBitString_Result;
			}


			// Compute new reference item from the actual reference item and inserting item
			inline char* cTuple::SetMinRefItem(const char* cTuple_RI, const char* cTuple_Key, char* cTuple_Result, const cDTDescriptor* pSd)
			{
				const cSpaceDescriptor* spaceDescriptor = ((cSpaceDescriptor*)pSd);
				uint dim = ((cSpaceDescriptor*)pSd)->GetDimension();

				for (uint i = 0; i < dim; i++)
				{
					SetValue(cTuple_Result, i, GetUInt(cTuple_Key, i, spaceDescriptor) <= GetUInt(cTuple_RI, i, spaceDescriptor) ? GetUInt(cTuple_Key, i, spaceDescriptor) : GetUInt(cTuple_RI, i, spaceDescriptor), spaceDescriptor);
				}

				return cTuple_Result;

			}

			// Returns true, if key is compatible with subnode with specified mask and reference item
			inline bool cTuple::IsCompatible(const char* cBitString_Mask, const char* cTuple_RI, const char* cTuple_Key, const cDTDescriptor* pSd)
			{
				const cSpaceDescriptor* spaceDescriptor = ((cSpaceDescriptor*)pSd);
				uint dim = ((cSpaceDescriptor*)pSd)->GetDimension();

				for (uint i = 0; i < dim; i++)
				{
					if (/*cBitString*/cBitArray::GetBit(cBitString_Mask, i))
					{
						if (GetUInt(cTuple_RI, i, spaceDescriptor) != GetUInt(cTuple_Key, i, spaceDescriptor))
							return false;
					}
					else
					{
						if (GetUInt(cTuple_RI, i, spaceDescriptor) > GetUInt(cTuple_Key, i, spaceDescriptor))
							return false;
					}
				}

				return true;
			}


			inline char* cTuple::MergeMasks(char* cBitString_Mask1, char* cBitString_Mask2, const char* cTuple_RI1, const char* cTuple_RI2, char* cBitString_Result, const cDTDescriptor* pSd)
			{
				const cSpaceDescriptor* spaceDescriptor = ((cSpaceDescriptor*)pSd);
				uint tupleLength = ((cSpaceDescriptor*)pSd)->GetDimension();

				for (uint i = 0; i < tupleLength; i++)
				{
					if ((/*cBitString*/cBitArray::GetBit(cBitString_Mask1, i) && /*cBitString*/cBitArray::GetBit(cBitString_Mask2, i)) && (GetUInt(cTuple_RI1, i, spaceDescriptor) == GetUInt(cTuple_RI2, i, spaceDescriptor)))
						cBitString::SetBit(cBitString_Result, i, true);
					else
						cBitString::SetBit(cBitString_Result, i, false);
				}

				return cBitString_Result;
			}

			inline void cTuple::AddToHistogram(cHistogram** histogram, const cDTDescriptor* dtd) const
			{
				uint tupleLength = ((cSpaceDescriptor*)dtd)->GetDimension();

				for (uint i = 0; i < tupleLength; i++)
				{
					histogram[i]->AddValue(GetUInt(i, (cSpaceDescriptor*)dtd));
				}
			}

			inline void cTuple::AddToHistogram(const char* cTuple_tuple, cHistogram** histogram, const cDTDescriptor* dtd)
			{
				uint tupleLength = ((cSpaceDescriptor*)dtd)->GetDimension();

				for (uint i = 0; i < tupleLength; i++)
				{
					histogram[i]->AddValue(GetUInt(cTuple_tuple, i, (cSpaceDescriptor*)dtd));
				}
			}


			inline unsigned int cTuple::OrderedHashValue(const cDTDescriptor* dtd, const unsigned int hashType, const unsigned int hashArg) const	// order preserving hash
			{
				cSpaceDescriptor *sd = (cSpaceDescriptor*)dtd;
				unsigned int dim = sd->GetDimension();
				unsigned int hashValue = 0;

				switch (hashType)
				{

				case 0:	// first int only
					hashValue = GetUInt(mData, 0, sd);
					if (hashArg > 0 && hashArg < 32)
					{
						if ((hashValue >> (32 - hashArg))>0)	// overflow
							hashValue = UINT_MAX;
						else hashValue << hashArg;				// bonus
					}
					break;

				case 2:
					hashValue = GetUInt(mData, 0, sd);
					if (hashArg > 0 && hashArg < 32)
					{
						if ((hashValue >> (32 - hashArg))>0)	// overflow
							hashValue = UINT_MAX;
						else hashValue << hashArg;				// bonus
					}
					hashValue = ~(UINT_MAX - hashValue);	// 
					break;

				case 1: // drop first n bits of each element
					if (hashArg == 0 || hashArg >= 32) // invalid hashArg -> do not drop anything
					{
						hashValue = GetUInt(mData, 0, sd);
						break;
					}

					unsigned int free_bits = 32;	// size of hashValue
					unsigned int drop = hashArg;
					unsigned int shift = 32 - drop;

					for (int i = 0; i < dim; i++)
					{
						unsigned int val = GetUInt(mData, i, sd);

						if (free_bits < shift)	// whole val does not fit
						{
							val >>= shift - free_bits;
							shift = free_bits;
						}

						if ((val >> shift) > 0)	// owerflow
						{
							if (i == 0)
							{
								hashValue = UINT_MAX;
								break;
							}
							uint mask = UINT_MAX >> (32 - free_bits);
							hashValue = (hashValue << free_bits) | mask;
							break;
						}
						hashValue = (hashValue << shift) | val;
						if (free_bits <= shift) break;	// all bits used
						free_bits -= shift;
					}
					break;
				}
				return hashValue;
			}

		}
	}
}
#endif
