/**
*	\file cMbrSideSizeOrder.h
*	\author Michal Kratky
*	\version 0.1
*	\date 2013
*	\brief An array of MBR's side sizes with order of each dimension.
*/

#ifndef __cMbrSideSizeOrder_h__
#define __cMbrSideSizeOrder_h__

#include "common/datatype/tuple/cSpaceDescriptor.h"

namespace common {
	namespace datatype {
		namespace tuple {

int compare4(const void *a, const void *b);

struct tMbrSideSizeOrder
{
	unsigned int SideSize;
	unsigned int Order;
};

template<class TTuple>
class cMbrSideSizeOrder  
{

public:
	static void ComputeSidesSize(const char* TTuple_mbrLo, const char* TTuple_mbrHi, tMbrSideSizeOrder *mbrSide, const cSpaceDescriptor *sd);
	static void ComputeSideSize(const char* TTuple_mbrLo, const char* TTuple_mbrHi, tMbrSideSizeOrder *mbrSide, unsigned int order, const cSpaceDescriptor *sd);

	static void QSortUInt(tMbrSideSizeOrder* mbrSide, unsigned int count);
	static void Print(tMbrSideSizeOrder* mbrSide, unsigned int count);
};

template<class TTuple>
void cMbrSideSizeOrder<TTuple>::ComputeSidesSize(const char* TTuple_mbrLo, const char* TTuple_mbrHi, tMbrSideSizeOrder *mbrSide, const cSpaceDescriptor *sd)
{
	unsigned int dim = sd->GetDimension();

	for (unsigned int i = 0 ; i < dim ; i++)
	{
		ComputeSideSize(TTuple_mbrLo, TTuple_mbrHi, mbrSide, i, sd);
		mbrSide[i].Order = i;
	}
}

template<class TTuple>
void cMbrSideSizeOrder<TTuple>::ComputeSideSize(const char* TTuple_mbrLo, const char* TTuple_mbrHi, tMbrSideSizeOrder *mbrSide, unsigned int order, const cSpaceDescriptor *sd)
{
	switch(sd->GetDimensionTypeCode(order))
	{
	case cUInt::CODE:
		unsigned int size = TTuple::GetUInt(TTuple_mbrHi, order, sd) - TTuple::GetUInt(TTuple_mbrLo, order, sd);
		mbrSide[order].SideSize = size;
		break;
	}
}

template<class TTuple>
void cMbrSideSizeOrder<TTuple>::QSortUInt(tMbrSideSizeOrder *mbrSide, unsigned int count)
{
	qsort(mbrSide, count, sizeof(tMbrSideSizeOrder), compare4);
}

template<class TTuple>
void cMbrSideSizeOrder<TTuple>::Print(tMbrSideSizeOrder *mbrSide, unsigned int count)
{
	printf("cMbrSideSizeOrder<TTuple>::Print()\n");
	for (unsigned int i = 0 ; i < count ; i++)
	{
		printf("%d: size: %u\n", mbrSide[i].Order, mbrSide[i].SideSize);
	}
	printf("----------------------------------\n");
}

}}}
#endif