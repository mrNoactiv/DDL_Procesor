#ifdef CUDA_ENABLED

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::InitGpu(uint blockSize, uint dim, uint bufferCapacity, uint nodeCapacity)
{
	//TransferIndexToGpu
	assert(parent::mMemoryManagerCuda != NULL);
	cRTreeHeader<TKey>* treeHeader = GetRTreeHeader();
	parent::mMemoryManagerCuda->Init(blockSize, dim, bufferCapacity, nodeCapacity);
	cudaDeviceProp gpuProp;
	cudaGetDeviceProperties(&gpuProp, 0);
	float cc = gpuProp.major + (gpuProp.minor / 10.0);

	switch (cGpuConst::ALGORITHM_TYPE)
	{
	default:
		break;
	case cGpuAlgorithm::Coprocessor_BFS:
		cGpuConst::MEMORY_LOCATION = cGpuMainMemoryLocation::Cpu;
		assert(cGpuConst::MEMORY_LOCATION == cGpuMainMemoryLocation::Cpu);
		break;
	case cGpuAlgorithm::Gpu_BFS:
		assert(cGpuConst::MEMORY_LOCATION == cGpuMainMemoryLocation::Gpu);
		assert(cGpuConst::RESULT_STRUCT == cGpuResultStructure::DistinctList);
		break;
	case cGpuAlgorithm::Gpu_BFS_35:
		assert(cGpuConst::MEMORY_LOCATION == cGpuMainMemoryLocation::Gpu);
		assert(cGpuConst::RESULT_STRUCT == cGpuResultStructure::DistinctList);
		if (cc < 3.5)
		{
			printf("\nWARNING: Cannot use recursive GPU BFS. Insufficient CUDA capability. Using general GPU BFS instead.");
			cGpuConst::ALGORITHM_TYPE = cGpuAlgorithm::Gpu_BFS;
		}
		break;
	}

	cGpuConst::PrintGpuInfo();
	if (cGpuConst::MEMORY_LOCATION == cGpuMainMemoryLocation::Gpu)
	{
		printf("\nTransfering nodes to GPU memory...");
		TransferIndexToGpu(treeHeader->GetRootIndex(), 0, treeHeader->GetHeight());
		uint levelsOnGpu = 0;/*TransferIndexToGpu(blockSize);*/
		treeHeader->SetLastGpuLevel(levelsOnGpu);
		printf("\rTransfering nodes to GPU memory...finished");
	}
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::TransferIndexToGpu(tNodeIndex nodeIndex, uint level, const uint height)
{
	TNode* currentNode = NULL;
	TLeafNode* currentLeafNode = NULL;

	if (TNode::IsLeaf(nodeIndex))
	{
		currentLeafNode = ReadLeafNodeR(nodeIndex);
		currentLeafNode->TransferLeafNodeToGpu();
		parent::mSharedCache->UnlockR(currentLeafNode);
	}
	else
	{
		currentNode = ReadInnerNodeR(nodeIndex);
		currentNode->TransferInnerNodeToGpu();
		for (unsigned int i = 0; i < currentNode->GetItemCount(); i++)
		{
			TransferIndexToGpu(currentNode->GetLink(i),level,height);
		}
		parent::mSharedCache->UnlockR(currentNode);
	}
}
template<class TMbr, class TKey, class TNode, class TLeafNode>
uint cCommonRTree<TMbr, TKey, TNode, TLeafNode>::TransferIndexToGpu(uint blockSize)
{
	TNode* currentNode = NULL;
	TLeafNode* currentLeafNode = NULL;
	mGpuCopyBufferCapacity = 512;
	cRTreeHeader<TKey>* treeHeader = GetRTreeHeader();
	uint height = treeHeader->GetHeight();
	uint size_buffer = sizeof(cArray<tNodeIndex>) + mGpuCopyBufferCapacity * sizeof(tNodeIndex) * 2;
	uint size_rawData = mGpuCopyBufferCapacity * blockSize;
	cMemoryBlock* memBlock = parent::mMemoryManager->GetMem(size_buffer + size_rawData);
	char* mem = memBlock->GetMem();
	cDbfsLevel* buff1 = (cDbfsLevel*)mem;
	mem += sizeof(cDbfsLevel);
	buff1->Init(cRangeQueryConfig::SEARCH_STRUCT_ARRAY, mem, mGpuCopyBufferCapacity);
	mem += cDbfsLevel::GetSize(cRangeQueryConfig::SEARCH_STRUCT_ARRAY, mGpuCopyBufferCapacity, 1);
	cDbfsLevel* buff2 = (cDbfsLevel*)mem;
	mem += sizeof(cDbfsLevel);
	buff2->Init(cRangeQueryConfig::SEARCH_STRUCT_ARRAY, mem, mGpuCopyBufferCapacity);
	mem += cDbfsLevel::GetSize(cRangeQueryConfig::SEARCH_STRUCT_ARRAY, mGpuCopyBufferCapacity, 1);

	char* data = mem;
	uint currentlevel = 0;
	uint transferedNodes = 0;
	uint maxNodes = parent::mMemoryManagerCuda->GetMaxNodes();

	/* level by level (supports partial tree transfer)*/
	buff1->Add(treeHeader->GetRootIndex());
	while (currentlevel < height - 1 && (transferedNodes + mGpuCopyBufferCapacity) < maxNodes)
	{
		bool isLeafLevel = (currentlevel == height);
		//uint nodeType = isLeafLevel ? cTreeHeader::HEADER_LEAFNODE : cTreeHeader::HEADER_NODE;
		uint nodeType = isLeafLevel ? 0 : 1;
		//mNodeCache->BulkRead(buff,startIndex,endIndex,nodeType); //nevim jak jednoduse zjistit start a end index v cArrray. Vyplati se bulk pri cteni po urovnich?
		for (uint i = 0; i < buff1->Count(); i++)
		{
			currentNode = parent::ReadInnerNodeR(buff1->GetRefItem(i));
			data += blockSize;
			for (uint j = 0; j < currentNode->GetItemCount(); j++)
			{
				if (buff2->Count() + 1 < mGpuCopyBufferCapacity)
				{
					buff2->Add(currentNode->GetLink(j));
				}
				else
				{
					//flush buffer buff2
					bool isNextLevelLeaf = (currentlevel + 1 == height);
					uint nodeType = isNextLevelLeaf ? 0 : 1;// ? cTreeHeader::HEADER_LEAFNODE : cTreeHeader::HEADER_NODE;
					TransferIndexToGpu_FlushBuffer(buff2, data, nodeType);
				}
			}
		}
		transferedNodes += buff1->Count();
		if (buff1->Count() > 0)
		{
			TransferIndexToGpu_FlushBuffer(buff1, data, cTreeHeader::HEADER_NODE);
		}

		//switch buffers
		cDbfsLevel *tmp = buff2;
		buff2 = buff1;
		buff1 = tmp;
		currentlevel++;
	}
	//flush the rest of the buffer
	if (buff1->Count() > 0)
	{
		TransferIndexToGpu_FlushBuffer(buff1, data, cTreeHeader::HEADER_LEAFNODE);
	}
	parent::mMemoryManager->ReleaseMem(memBlock);
}
template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::TransferIndexToGpu_FlushBuffer(cDbfsLevel* buffer, char* data, uint nodeType)
{
	buffer->Sort();
	uint startIndex = 0;
	uint endIndex = buffer->Count() - 1;
	parent::mSharedCache->BulkRead(buffer, startIndex, endIndex, nodeType);
	//copy to gpu
	//parent::mMemoryManagerCuda

	buffer->ClearCount();
	//provest unlock?

}

#endif
