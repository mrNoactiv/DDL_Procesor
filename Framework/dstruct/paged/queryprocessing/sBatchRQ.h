#ifndef __sBatchRQ__
#define __sBatchRQ__

#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "common/datatype/tuple/cTuple.h"
#include "common/datatype/tuple/cHNTuple.h"

using namespace common::datatype::tuple;

namespace dstruct {
	namespace paged {

// A structure summarizing batch and cartesian range query
struct sBatchRQ
{
	unsigned short mode;
	const cTuple *qls;     // Batch RQ
	const cTuple *qhs;     // Batch RQ
	const unsigned int queriesCount;  // the number of queries of Batch RQ
	cHNTuple *ql;    // Cartesian RQ
	cHNTuple *qh;    // Cartesian RQ
	cSpaceDescriptor *sd;  // SD for cHNTuple of Cartesian RQ
};
}}

#endif