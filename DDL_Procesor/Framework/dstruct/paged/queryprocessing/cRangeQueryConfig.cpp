#include "cRangeQueryConfig.h"

namespace dstruct {
  namespace paged {

cRangeQueryConfig::cRangeQueryConfig(): mFinalResultSize(FINAL_RESULTSIZE_UNDEFINED), mSignatureEnabled(false), mBulkReadEnabled(false), 
mLeafIndicesCapacity(1), mMaxReadNodes(1), mSearchMethod(SEARCH_DFS), mNodeIndexCapacity_BulkRead(0)
{
}

}}