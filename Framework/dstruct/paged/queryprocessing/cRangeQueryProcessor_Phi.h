/**
*	\file cRangeQueryProcessor_Phi.h
*	\author Pavel Bednar
*	\version 0.1
*	\date 2015-02-05
*	\brief Range Query processor for Intel Xeon Phi.
*/

/*!
* Common range query method for Intel Xeon Phi.
*/
template <class TKey, class TNode, class TLeafNode>
void cRangeQueryProcessor<TKey, TNode, TLeafNode>::RangeQuery_Phi(sBatchRQ *batchRQ, cRangeQueryConfig *rqConfig, cRangeQueryContext *rqContext, cTreeItemStream<TKey>* resultSet, cRQBuffers<TKey>* rqBuffers, cQueryProcStat* QueryProcStat)
{
#ifdef PHI_ENABLED

#else
	printf("\nCritical Error! Cannot process range query. Intel Xeon Phi support is not enabled.");
#endif
}

/**
* Scans single tree level on Intel Xeon Phi.
*/
template <class TKey, class TNode, class TLeafNode>
void cRangeQueryProcessor<TKey, TNode, TLeafNode>::DBFS_Phi_ScanLevel(uint level, uint nodeType, sBatchRQ *batchRQ, cTreeItemStream<TKey> *resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey> *rqBuffers, cQueryProcStat *QueryProcStat)
{
#ifdef PHI_ENABLED

#else
	printf("\nCritical Error! Cannot process range query. Intel Xeon Phi support is not enabled.");
#endif
}

//the rest are only Phi methods.
#ifdef PHI_ENABLED

#endif