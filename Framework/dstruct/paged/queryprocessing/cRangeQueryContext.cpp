#include "cRangeQueryContext.h"

namespace dstruct {
	namespace paged {

/// Constructor - query box is set, you must delete these objects!
cRangeQueryContext::cRangeQueryContext(cTuple *ql, cTuple *qh)
{
	mQl = ql;
	mQh = qh;
}

}}
