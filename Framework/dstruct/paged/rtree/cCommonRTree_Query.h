/**
 * Return true if the tuple exists in R-Tree, data return in item.
 * Because this implementation of R-tree allows overlapped regions, do the point query using range query.
 **/
template<class TMbr, class TKey, class TNode, class TLeafNode>
bool cCommonRTree<TMbr,TKey,TNode,TLeafNode>::Find(const TKey &item, char* data, cRangeQueryConfig *rqConfig, cQueryProcStat *QueryProcStat)
{
	bool ret = false;
	cTreeItemStream<TKey>* resultSet = RangeQuery(item, item, rqConfig, NULL, QueryProcStat);

	if ((!parent::mHeader->DuplicatesAllowed() && resultSet->GetItemCount() == 1) || 
		(parent::mHeader->DuplicatesAllowed() && resultSet->GetItemCount() >= 1))
	{
		const char* resultItem = resultSet->GetItem();
		// item.SetData(TKey::GetData(parent::mQueryResult->GetItem(0), parent::mHeader), parent::mHeader->GetLeafDataInMemSize());
		cNodeItem::Copy(data, resultItem + parent::mHeader->GetKeySize(), parent::mHeader->GetLeafDataSize());
		ret = true;
	}
	resultSet->CloseResultSet();

	return ret;
}

/**
 * Return true if the tuples exist in R-Tree, data return in item.
 * Because this implementation of R-tree allows overlapped regions, do the point query using range query.
 **/
template<class TMbr, class TKey, class TNode, class TLeafNode>
bool cCommonRTree<TMbr,TKey,TNode,TLeafNode>::FindBatchQuery(TKey *items, char* data, cRangeQueryConfig *rqConfig, unsigned int queriesCount, cQueryProcStat* QueryProcStat)
{
	bool ret = false;
	cTreeItemStream<TKey>* resultSet = BatchRangeQuery(items, items, rqConfig, queriesCount, NULL, QueryProcStat);

	if ((!parent::mHeader->DuplicatesAllowed() && resultSet->GetItemCount() == queriesCount) || 
		(parent::mHeader->DuplicatesAllowed() && resultSet->GetItemCount() >= queriesCount))
	{
		//const char* resultItem = resultSet->GetItem(); // PCH - TODO
		//cNodeItem::Copy(data, resultItem + parent::mHeader->GetKeySize(), parent::mHeader->GetLeafDataSize()); // PCH - TODO
		ret = true;
	}

	resultSet->CloseResultSet();
	return ret;
}

/**
 * Return true if the tuples exist in R-Tree, data return in item.
 * Because this implementation of R-tree allows overlapped regions, do the point query using range query.
 **/
template<class TMbr, class TKey, class TNode, class TLeafNode>
bool cCommonRTree<TMbr,TKey,TNode,TLeafNode>::FindCartesianQuery(cHNTuple *queryTuple, cSpaceDescriptor* queryDescriptor, char* data, cRangeQueryConfig *rqConfig, unsigned int queriesCount, cQueryProcStat* QueryProcStat)
{
	bool ret = false;
	cTreeItemStream<TKey>* resultSet = CartesianRangeQuery(queryTuple, queryTuple, queryDescriptor, rqConfig, queriesCount, NULL, QueryProcStat);

	if (resultSet->GetItemCount() >= queriesCount)
	{
		//const char* resultItem = resultSet->GetItem(); // PCH - TODO
		//cNodeItem::Copy(data, resultItem + parent::mHeader->GetKeySize(), parent::mHeader->GetLeafDataSize()); // PCH - TODO
		ret = true;
	}
	else
	{
		ret = true; // TODO
	}

	resultSet->CloseResultSet();
	return ret;
}

/**
 * Return if tuple is contained into UBTree (point query).
 **/
template<class TMbr, class TKey, class TNode, class TLeafNode>
bool cCommonRTree<TMbr,TKey,TNode,TLeafNode>::PointQuery(TKey &item)
{
	return Find(item);
}

/// Range query defined by a context object. This object mainly includes the current tree path. This is the starting
/// point of the next range query. Moreover, this method is finished if the one tuple is retrieved.
/// \param rqContext The context object
/// \return False if 'the end' of the tree is reached, otherwise return true
template<class TMbr, class TKey, class TNode, class TLeafNode>
inline cTreeItemStream<TKey>* cCommonRTree<TMbr,TKey,TNode,TLeafNode>::RangeQuery(cRangeQueryConfig *rqConfig, cRangeQueryContext *rqContext)
{
	return RangeQuery(rqContext->GetRefQlTuple(), rqContext->GetRefQhTuple(), rqConfig, rqContext);
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
cTreeItemStream<TKey>* cCommonRTree<TMbr,TKey,TNode,TLeafNode>::CartesianRangeQuery(cHNTuple* qls, cHNTuple* qhs, cSpaceDescriptor* queryDescriptor, cRangeQueryConfig *rqConfig, unsigned int queriesCount, cRangeQueryContext *rqContext, cQueryProcStat *QueryProcStat)
{
	return mRQProcessor->RangeQuery(qls, qhs, queryDescriptor, rqConfig, rqContext, QueryProcStat);
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
cTreeItemStream<TKey>* cCommonRTree<TMbr,TKey,TNode,TLeafNode>::BatchRangeQuery(TKey* qls, TKey* qhs, cRangeQueryConfig *rqConfig, unsigned int queriesCount, cRangeQueryContext *rqContext, cQueryProcStat *QueryProcStat)
{
	return mRQProcessor->RangeQuery(qls, qhs, queriesCount, rqConfig, rqContext, QueryProcStat);
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
cTreeItemStream<TKey>* cCommonRTree<TMbr,TKey,TNode,TLeafNode>::RangeQuery(const TKey &ql, const TKey &qh, cRangeQueryConfig *rqConfig, cRangeQueryContext *rqContext, cQueryProcStat* QueryProcStat)
{
	return mRQProcessor->RangeQuery(&ql, &qh, rqConfig, rqContext, QueryProcStat);
}

/// Range query is finished, store the context
/// It seems this is an obsolete method
template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr,TKey,TNode,TLeafNode>::RQStoreContext(cRangeQueryContext *rqContext, unsigned int currentLevel, const TLeafNode &currentLeafNode, unsigned int orderInLeafNode, unsigned int* currPath, unsigned int* itemOrderCurrPath)
{
	// store the current path in the context
	if (rqContext != NULL)
	{
		for (unsigned int i = 0 ; i < currentLevel ; i++)
		{
			rqContext->SetContext(i, currPath[i] /* old: mTreePool->GetNodeIndex(i, 1),*/,
				itemOrderCurrPath[i] /* old: mTreePool->GetNodeIndex(i)*/);
		}

		rqContext->SetPathLength(currentLevel);
		rqContext->SetContext(currentLevel, TNode::GetLeafNodeIndex(currentLeafNode.GetIndex()), orderInLeafNode+1);
		rqContext->IncrementOrder();
	}
}