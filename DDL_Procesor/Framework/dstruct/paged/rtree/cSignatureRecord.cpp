#include "cSignatureRecord.h"

namespace dstruct {
	namespace paged {
		namespace rtree {

cSignatureRecord::cSignatureRecord(char* mem, const cSpaceDescriptor* pSD)
{
	mNodeSignature = new(mem) cLNTuple(mem);
	cLNTuple::SetLength(mNodeSignature->GetData(), pSD);
}

void cSignatureRecord::Combine(uint i, ullong value, const char* ql, const char* qh, cSignatureParams* pSignatureParams, cArray<ullong> *pQueryTrueBitOrders, cArray<uint> *pNarrowDims, sMapItem** pMapTable, uint* pMapTableCounter, uint* domains, bool DDS)
{
	uint dimension = pNarrowDims->Count();

	if (i < dimension)
	{
		int narrowDim = pNarrowDims->GetRefItem(i);
		uint qlValue = cTuple::GetUInt(ql, narrowDim, NULL);
		uint qhValue = cTuple::GetUInt(qh, narrowDim, NULL);
		for (int j = qlValue; j <= qhValue; j++)
		{
			uint mapValue = j; // GetMapValue(j, pMapTable[narrowDim], &pMapTableCounter[narrowDim], false);
			ullong dimValue = mapValue * pow(domains[narrowDim], narrowDim);

			assert(value + dimValue < ULLONG_MAX);
			Combine(i + 1, value + dimValue, ql, qh, pSignatureParams, pQueryTrueBitOrders, pNarrowDims, pMapTable, pMapTableCounter, domains, DDS);
		}
	}
	else
	{
		uint sigLength = pSignatureParams->GetLength();
		uint nOfBits = pSignatureParams->GetBitCount();

		for (uint i = 0; i < nOfBits; i++)
		{
			if (DDS)
			{
				uint trueBitOrder = cSignature::GetTrueBitOrder(value, sigLength, i);
				pQueryTrueBitOrders->Add(trueBitOrder);
			}
			else
			{
				pQueryTrueBitOrders->Add(value);
			}
		}
	}
}


ullong cSignatureRecord::ComputeTupleValue(const char* item, bool* queryType, uint dimension, sMapItem** pMapTable, uint* pMapTableCounter, uint* domains)
{
	ullong value = 0;
	for (int i = 0; i < dimension; i++)
	{
		if (queryType[i])
		{
			uint realValue = cTuple::GetUInt(item, i, NULL);
			uint mapValue = realValue; // GetMapValue(realValue, pMapTable[i], &pMapTableCounter[i], true);
			ullong dimValue = mapValue * pow(domains[i], i);
			assert(value + dimValue < ULLONG_MAX);
			value += dimValue;
		}
	}

	return value;
}


uint cSignatureRecord::GenerateQuerySignature_DDS(const char* ql, const char* qh, cSignatureParams* pSignatureParams, cArray<ullong> *pQueryTrueBitOrders, cArray<uint> *pNarrowDims, sMapItem** pMapTable, uint* pMapTableCounter, uint* domains)
{
	uint previousBitCounts = pQueryTrueBitOrders->Count();
	Combine(0, 0, ql, qh, pSignatureParams, pQueryTrueBitOrders, pNarrowDims, pMapTable, pMapTableCounter, domains, true);
	return pQueryTrueBitOrders->Count() - previousBitCounts;
}


uint cSignatureRecord::GenerateQuerySignature_DDO(const char* ql, const char* qh, cSignatureParams* pSignatureParams, cArray<ullong> *pQueryTrueBitOrders, cArray<uint> *pNarrowDims, sMapItem** pMapTable, uint* pMapTableCounter, uint* domains)
{
	uint previousBitCounts = pQueryTrueBitOrders->Count();
	Combine(0, 0, ql, qh, pSignatureParams, pQueryTrueBitOrders, pNarrowDims, pMapTable, pMapTableCounter, domains, false);
	return pQueryTrueBitOrders->Count() - previousBitCounts;
}

/*
* This methods fills the array of true bit orders - it is a representation of the query signature.
*/
uint cSignatureRecord::GenerateQuerySignature_DIS(const char* item, cSignatureParams* pSignatureParams, cArray<ullong> *pQueryTrueBitOrders, cArray<uint> *pNarrowDims)
{
	uint nOfDims = pNarrowDims->Count();
	uint res = 0;

	for (uint i = 0; i < nOfDims; i++)
	{
		uint narrowDim = pNarrowDims->GetRefItem(i);
		uint sigLength = pSignatureParams->GetLength(narrowDim);
		uint value = cTuple::GetUInt(item, narrowDim, NULL);
		uint nOfBits = pSignatureParams->GetBitCount(narrowDim);

		cSignature::GenerateSignature(value, pQueryTrueBitOrders, sigLength, nOfBits);
		res += nOfBits;
	}

	return res/*nOfDims * nOfBits*/;
}

uint cSignatureRecord::GetMapValue(uint realValue, sMapItem* pMapTable, uint* pMapTableCounter, bool pCanAdd)
{
	uint count = *pMapTableCounter;
	for (uint i = 0; i < count; i++)
	{
		if (pMapTable[i].realValue == realValue)
			return pMapTable[i].mapValue;
	}

	if (pCanAdd)
	{
		sMapItem *pmi = pMapTable + count;
		pmi->realValue = realValue;
		pmi->mapValue = count;
		*pMapTableCounter = *pMapTableCounter + 1;
	}
	else
	{
		return cUInt::MAX;
		//printf("SignatureRecord - Error: Value does not exist in Mapping Table !!!");
	}

	return count;
}

}}}