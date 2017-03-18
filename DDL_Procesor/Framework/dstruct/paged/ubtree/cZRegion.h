/***


***/

#ifndef __cZRegion_h__
#define __cZRegion_h__

#include <assert.h>
#include <stdio.h>

#include "common/datatype/cDataType.h"
#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "common/datatype/tuple/cTuple.h"
#include "dstruct/paged/ubtree/cBitAddress.h"

using namespace common::compression;
using namespace common::datatype;
using namespace common::utils;

namespace common {
	namespace datatype {
		namespace tuple {

class cZRegion
{
	char* low;
	char* high;
public:
	typedef cZRegion T;

protected:
	char *mData;

public:
	static const unsigned int LengthType = cDataType::LENGTH_FIXLEN;
	static const char CODE = 'a';

public:
	cZRegion();
	cZRegion::cZRegion(char* low, char* high);
	~cZRegion();
	
	char* cZRegion::getLowAdress(){return low;};
	char* cZRegion::getHighAdress(){return high;};
	void Free(cMemoryBlock *memBlock = NULL);
	
	
};
}}}
#endif