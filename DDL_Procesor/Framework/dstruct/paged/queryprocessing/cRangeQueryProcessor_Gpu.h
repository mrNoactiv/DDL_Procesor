/**
*	\file cRangeQueryProcessor_Gpu.h
*	\author Pavel Bednar
*	\version 0.1
*	\date 2015-02-05
*	\brief Range Query processor for Gpu.
*/

/**
* Common range query method for GPU.
*/
template <class TKey, class TNode, class TLeafNode>
void cRangeQueryProcessor<TKey, TNode, TLeafNode>::RangeQuery_Gpu(sBatchRQ *batchRQ, cRangeQueryConfig *rqConfig, cRangeQueryContext *rqContext, cTreeItemStream<TKey>* resultSet, cRQBuffers<TKey>* rqBuffers, cQueryProcStat* QueryProcStat)
{
#ifdef CUDA_ENABLED
	cMemoryManagerCuda* mmc = mQuickDB->GetMemoryManagerCuda();
	InitializeGpuQuery(batchRQ);
	rqConfig->SetGpuAlgorithm(cGpuConst::ALGORITHM_TYPE);
	rqConfig->SetGpuCapability(mmc->GetDeviceProperties().major + mmc->GetDeviceProperties().minor / 1.0);
	if (rqConfig->GetGpuAlgorithm() == cGpuAlgorithm::Automatic)
	{
		//determine best algorithm
		//not implemented yet
	}
	
	switch (rqConfig->GetGpuAlgorithm())
	{
	case cGpuAlgorithm::Gpu_DFS_35:
		assert(cGpuConst::MEMORY_LOCATION == cGpuMainMemoryLocation::Gpu);
		assert(rqConfig->GetGpuCapability() >= 3.5);
		break;
	case cGpuAlgorithm::Gpu_BFS_35:
		assert(cGpuConst::MEMORY_LOCATION == cGpuMainMemoryLocation::Gpu);
		assert(rqConfig->GetGpuCapability() >= 3.5);
		break;
	case cGpuAlgorithm::Gpu_DBFS_35:
		assert(cGpuConst::MEMORY_LOCATION == cGpuMainMemoryLocation::Gpu);
		assert(rqConfig->GetGpuCapability() >= 3.5);
		break;
	case cGpuAlgorithm::Gpu_BFS:
		assert(cGpuConst::MEMORY_LOCATION == cGpuMainMemoryLocation::Gpu);
		RangeQuery_DBFS(0, batchRQ, resultSet, rqConfig, rqBuffers, QueryProcStat);
		break;;
	case cGpuAlgorithm::Coprocessor_BFS:
		assert(cGpuConst::MEMORY_LOCATION == cGpuMainMemoryLocation::Cpu);
		RangeQuery_DBFS(0, batchRQ, resultSet, rqConfig, rqBuffers, QueryProcStat);
		break;
	}
#else
	printf("\nCritical Error! Cannot process range query. GPU support is not enabled.");
#endif
}

/**
* Scans single tree level on Gpu.
*/
template <class TKey, class TNode, class TLeafNode>
void cRangeQueryProcessor<TKey, TNode, TLeafNode>::DBFS_Gpu_ScanLevel(uint level, uint nodeType, sBatchRQ *batchRQ, cTreeItemStream<TKey> *resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey> *rqBuffers, cQueryProcStat *QueryProcStat)
{
#ifdef CUDA_ENABLED
	
	TLeafNode *currentLeafNode = NULL;
	TNode* currentNode = NULL;
	cBucketHeader* bucket;
	cDbfsLevel* currentLevel = rqBuffers->GetBreadthSearchArray(rqConfig,level);
	cArray<uint>* searchArray;
	if (rqConfig->IsBucketOrderNodeIndex())
	{
		searchArray=currentLevel->ToArray();
	}
	else
	{
		searchArray=rqBuffers->GetGpuSearchArray(rqConfig, level);
		searchArray->ClearCount();
	}
	//move data to GPU if needed
	for (uint i = 0; i < currentLevel->Count(); i++)
	{
		tNodeIndex nodeIndex = currentLevel->GetRefItem(i);
		if (nodeType == 1 /*cRTreeHeader<TKey>::HEADER_NODE*/)
		{
			currentNode = mTree->ReadInnerNodeR(nodeIndex);
			currentNode->TransferInnerNodeToGpu();
		}
		else
		{
			currentLeafNode = mTree->ReadLeafNodeR(nodeIndex);
			currentLeafNode->TransferLeafNodeToGpu();
		}
		if (!rqConfig->IsBucketOrderNodeIndex())
		{
			bool nodeFound = mQuickDB->GetMemoryManagerCuda()->GetBucket(nodeIndex, &bucket);
			assert(nodeFound);
			searchArray->Add(bucket->GetBucketOrder());
		}
	}
	//search offsets on GPU
	mQuickDB->GetMemoryManagerCuda()->CopySearchArrayToGpu(searchArray);
	cCudaParams params = CreateSearchParams(mQuickDB->GetMemoryManagerCuda(), batchRQ, rqConfig, currentLevel, nodeType);
	cCudaProcessor::RangeQuery_Level(params);
	//get the result
	DBFS_Gpu_ProcessOutput(params,nodeType,level,batchRQ,resultSet,rqConfig,rqBuffers,QueryProcStat);
#else
	printf("\nCritical Error! Cannot process range query. GPU support is not enabled.");
#endif
}
//the rest are only GPU methods.
#ifdef CUDA_ENABLED
/**
* Processes the output from Gpu level search. 
* Fills the search buffer for next level or fills the result set in the case of leaf level.
*/
template <class TKey, class TNode, class TLeafNode>
void cRangeQueryProcessor<TKey, TNode, TLeafNode>::DBFS_Gpu_ProcessOutput(cCudaParams &params, uint nodeType, uint level, sBatchRQ *batchRQ, cTreeItemStream<TKey>* resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey> *rqBuffers, cQueryProcStat *queryStat)
{
	cMemoryBlock* bufferMemBlock;
	uint itemsCount;
	if (cGpuConst::RESULT_STRUCT == cGpuResultStructure::DistinctList)
	{
		uint* list = NULL;
		itemsCount = mQuickDB->GetMemoryManagerCuda()->GetHostResultListCount(params.D_ResultList);
		if (itemsCount > 0)
		{
			bufferMemBlock = mQuickDB->GetMemoryManager()->GetMem(itemsCount*sizeof(uint));
			list = (uint*)bufferMemBlock->GetMem();
			mQuickDB->GetMemoryManagerCuda()->GetHostResultList(list, itemsCount, params.D_ResultList);

			if (nodeType == cTreeHeader::HEADER_NODE)
			{
				DBFS_Gpu_FillNextLevel(level, batchRQ, resultSet, rqConfig, rqBuffers, queryStat, list, itemsCount);
			}
			else
			{
				DBFS_Gpu_FillResultSet(level, batchRQ, resultSet, rqConfig, rqBuffers, list, itemsCount);
			}
			mQuickDB->GetMemoryManager()->ReleaseMem(bufferMemBlock);
		}
	}
	else
	{
		unsigned int nodeCapacity = mTree->GetHeader()->GetLeafNodeItemCapacity();
		unsigned int nodesCount = rqBuffers->GetBreadthSearchArray(rqConfig,level)->Count();
		itemsCount = nodesCount*nodeCapacity;
		bufferMemBlock = mQuickDB->GetMemoryManager()->GetMem(itemsCount*sizeof(bool));
		bool* resultVector = (bool*)bufferMemBlock->GetMem();
		if (nodeType == cTreeHeader::HEADER_NODE)
		{
			mQuickDB->GetMemoryManagerCuda()->CopyResultVectorFromGpu(nodesCount * nodeCapacity, resultVector);
			DBFS_Gpu_FillNextLevel(level, batchRQ, resultSet, rqConfig, rqBuffers, queryStat, resultVector);
		}
		else
		{
			mQuickDB->GetMemoryManagerCuda()->CopyResultVectorFromGpu(nodesCount*nodeCapacity, resultVector);
			DBFS_Gpu_FillResultSet(level, batchRQ, resultSet, rqConfig, rqBuffers, resultVector);
		}
		mQuickDB->GetMemoryManager()->ReleaseMem(bufferMemBlock);
	}
}

/**
* Initializes the range query search on the Gpu.
*/
template <class TKey, class TNode, class TLeafNode>
void cRangeQueryProcessor<TKey, TNode, TLeafNode>::InitializeGpuQuery(sBatchRQ *batchRQ) 
{
	const cDTDescriptor* sd = mTreeHeader->GetNodeHeader(cTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor();
	uint size = batchRQ->queriesCount * sd->GetDimension() * sizeof(uint);
	cMemoryBlock* bufferMemBlock = mQuickDB->GetMemoryManager()->GetMem(2*size);
	uint* pql = (uint*)bufferMemBlock->GetMem();
	uint* pqh = (uint*)(bufferMemBlock->GetMem()+size);
	//const uint s = batchRQ->queriesCount * sd->GetDimension();
	//uint pql[s];
	//uint pqh[s];

	for (uint i=0;i<batchRQ->queriesCount;i++)
	{
		uint* qlData = (uint*) batchRQ->qls[i].GetData();
		uint* qhData = (uint*)batchRQ->qhs[i].GetData();
		for (uint d=0;d<sd->GetDimension();d++)
		{
			pql[i*sd->GetDimension() + d] = qlData[d];
			pqh[i*sd->GetDimension() + d] = qhData[d];
		}
	}
	mQuickDB->GetMemoryManagerCuda()->InicializeRangeQuery(mTreeHeader->GetNodeHeader(cTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor()->GetDimension(),pql,pqh,batchRQ->queriesCount);
	mQuickDB->GetMemoryManager()->ReleaseMem(bufferMemBlock);
}

/**
* Creates the parameters for Gpu range query processing.
*/
template <class TKey, class TNode, class TLeafNode>
cCudaParams cRangeQueryProcessor<TKey, TNode, TLeafNode>::CreateSearchParams(cMemoryManagerCuda* mmc, sBatchRQ *batchRQ, cRangeQueryConfig *rqConfig, cDbfsLevel* currentLevel, unsigned int nodeType)
{
	cCudaParams params;
	params.BlockSize = 2048; //bed157: linux debug ;
	params.ThreadsPerBlock = cGpuConst::THREADS_PER_BLOCK;
	params.NoBlocks = currentLevel->Count();
	params.DebugFlag = cGpuConst::DEBUG_FLAG;
	params.NodeCapacity = mTreeHeader->GetLeafNodeItemCapacity();
	params.NodeTypes = nodeType;
	params.Dimension = mTreeHeader->GetNodeHeader(cTreeHeader::HEADER_LEAFNODE)->GetKeyDescriptor()->GetDimension();
	params.QueriesInBatch = batchRQ->queriesCount;
	params.Mode = rqConfig->GetQueryProcessingType();
	params.DeviceProperties = mmc->GetDeviceProperties();
	params.D_Inputs = mmc->GetD_Inputs();
	params.D_Results = mmc->GetD_Results();
	params.D_SearchOffsets = mmc->GetD_SearchOffsets();
	params.D_RelevantQueries = mmc->GetD_RelevantQueries();
	params.D_ChildIndices = mmc->GetD_ChildIndices();
	params.D_ResultList = mmc->GetD_ResultList();
	params.NodeType = nodeType;
	params.TBCount = params.NoBlocks;
	params.NoChunks = 1;
	params.ResultRowSize = params.NodeCapacity;
	params.SequencialSearch = false;
	return params;
}

/**
* Fills the result set based on output from Gpu.
*/
template <class TKey, class TNode, class TLeafNode>
void cRangeQueryProcessor<TKey, TNode, TLeafNode>::DBFS_Gpu_FillResultSet(unsigned int level, sBatchRQ *batchRQ, cTreeItemStream<TKey>* resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey> *rqBuffers, bool* resultVector)
{
	cDbfsLevel *dbfsArray = rqBuffers->GetBreadthSearchArray(rqConfig,level);
	unsigned int nodesCount = dbfsArray->Count();
	TLeafNode* currentLeafNode = NULL;
	unsigned int rs = 0;
	for (unsigned int i = 0; i < nodesCount; i++)
	{
		unsigned int prevRs = rs;
		currentLeafNode = mTree->ReadLeafNodeR(dbfsArray->GetRefItem(i));
		unsigned int tuplesBefore = i *  mTree->GetHeader()->GetLeafNodeItemCapacity();
		for (unsigned int k = 0; k < currentLeafNode->GetItemCount(); k++)
		{
			if (resultVector[tuplesBefore])
			{
				rs++;
				const char* item = currentLeafNode->GetCItem(k);
				resultSet->Add(item);
			}
			tuplesBefore++;
		}
		mQuickDB->GetNodeCache()->UnlockR(currentLeafNode);
	}
}

/**
* Fills the result set based on output from Gpu.
*/
template <class TKey, class TNode, class TLeafNode>
void cRangeQueryProcessor<TKey, TNode, TLeafNode>::DBFS_Gpu_FillResultSet(unsigned int level, sBatchRQ *batchRQ, cTreeItemStream<TKey>* resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey> *rqBuffers, uint* resultList, uint resultListCount)
{
	cDbfsLevel *dbfsArray = rqBuffers->GetBreadthSearchArray(rqConfig, level);
	TLeafNode* currentLeafNode = NULL;
	for (uint i = 0; i < resultListCount; i++)
	{
		uint val = resultList[i];
		//first 24bits is position in dbfsArray, remaining 8bits is childOrder in node.
		uint order = val >> 8;//	uint order = val >> 8;
		uint childOrder = val & 255;
		//printf("\nRS #%d, nodeIndex: %u,\t order: %u", i, order, childOrder);
		resultSet->Add(mTree->ReadLeafNodeR(dbfsArray->GetRefItem(order))->GetCItem(childOrder));
	}
}

/**
* Fills the search buffer for next level base on output from Gpu.
*/
template <class TKey, class TNode, class TLeafNode>
void cRangeQueryProcessor<TKey, TNode, TLeafNode>::DBFS_Gpu_FillNextLevel(unsigned int level, sBatchRQ *batchRQ, cTreeItemStream<TKey>* resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey> *rqBuffers, cQueryProcStat *queryStat, bool* resultVector)
{
	cDbfsLevel *currentLevel = rqBuffers->GetBreadthSearchArray(rqConfig, level);
	cDbfsLevel *nextLevel = rqBuffers->GetBreadthSearchArray(rqConfig, level+1);
	unsigned int nodesCount = currentLevel->Count();
	TNode* currentNode = NULL;
	TNode* child = NULL;
	unsigned int rs = 0;
	for (unsigned int i = 0; i < nodesCount; i++)
	{
		unsigned int prevRs = rs;
		currentNode = mTree->ReadInnerNodeR(currentLevel->GetRefItem(i));
		unsigned int tuplesBefore = i *  mTree->GetHeader()->GetLeafNodeItemCapacity();
		for (unsigned int k = 0; k < currentNode->GetItemCount(); k++)
		{
			if (resultVector[tuplesBefore])
			{
				rs++;
				unsigned int nodeIndex = TNode::GetNodeIndex(currentNode->GetLink(k));
				DBFS_EnqueueNode(nodeIndex, level + 1, batchRQ, resultSet, rqConfig, rqBuffers, queryStat);
			}
			tuplesBefore++;
		}
		//printf("\nResult size: %d", rs);
		//mQuickDB->GetNodeCache()->UnlockR(currentNode);
	}
}

/**
* Fills the search buffer for next level base on output from Gpu.
*/
template <class TKey, class TNode, class TLeafNode>
void cRangeQueryProcessor<TKey, TNode, TLeafNode>::DBFS_Gpu_FillNextLevel(unsigned int level, sBatchRQ *batchRQ, cTreeItemStream<TKey>* resultSet, cRangeQueryConfig *rqConfig, cRQBuffers<TKey> *rqBuffers, cQueryProcStat *queryStat, uint* resultList, uint resultListCount)
{
	for (uint i = 0; i < resultListCount; i++)
	{
			DBFS_EnqueueNode(resultList[i], level + 1, batchRQ, resultSet, rqConfig, rqBuffers, queryStat);
	}
}
#endif