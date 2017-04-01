#pragma once

#include "cCompleteRTree.h"




#include "common/cNumber.h"
#include "common/utils/cTimer.h"
#include "common/stream/cStream.h"
#include "common/datatype/cBasicType.h"
#include "common/datatype/tuple/cTuple.h"
#include "common/datatype/tuple/cSpaceDescriptor.h"


#include "dstruct/paged/core/cNodeCache.h"
#include "dstruct/paged/core/cDStructConst.h"
#include "dstruct/paged/sequentialarray/cSequentialArray.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayContext.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayHeader.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayNodeHeader.h"

template<class TKey>
class cCompleteSeqArray
{
public:
	cSequentialArray<TKey> *mSeqArray;
	cSequentialArrayHeader<TKey> *mHeader;



	cCompleteSeqArray(const char * uniqueName, const unsigned int blockSize, const unsigned int cacheSize, cSpaceDescriptor * sd, cQuickDB *quickDB);
	bool setArray(const char * uniqueName, const unsigned int blockSize, const unsigned int cacheSize, cSpaceDescriptor * sd, cQuickDB *quickDB);

};

template<class TKey>
inline cCompleteSeqArray<TKey>::cCompleteSeqArray(const char * uniqueName, const unsigned int blockSize, const unsigned int cacheSize, cSpaceDescriptor * sd, cQuickDB * quickDB)
{
	setArray(uniqueName, blockSize, cacheSize, sd, quickDB);
}

template<class TKey>
inline bool cCompleteSeqArray<TKey>::setArray(const char * uniqueName, const unsigned int blockSize, const unsigned int cacheSize, cSpaceDescriptor *sd, cQuickDB *quickDB)
{
	mHeader = new cSequentialArrayHeader<TKey>(uniqueName, blockSize, sd, cDStructConst::DSMODE_DEFAULT);
	//mHeader->SetCodeType(ELIAS_DELTA);asi zbytečné

	mSeqArray = new cSequentialArray<TKey>();
	if (!mSeqArray->Create(mHeader, quickDB))
	{
		printf("Sequential array: creation failed\n");
		return false;
	}
	else
	{
		return true;
	}
}
