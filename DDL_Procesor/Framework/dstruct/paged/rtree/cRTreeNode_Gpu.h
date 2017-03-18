template<class TMbr>
inline void cRTreeNode<TMbr>::TransferInnerNodeToGpu()
{
	cMemoryManagerCuda* mmc = parent::mHeader->GetMemoryManagerCuda();
	//printf("\nCopying node %u to GPU.", parent::GetIndex());
	if (!mmc->FindNode(parent::GetIndex())) //node is not on GPU
	{
#ifdef CUDA_MEASURE
		cTimer tmrCopy;
		tmrCopy.Start();
#endif
		unsigned int gpuId, gpuItemOrder;
		uint blockDataSize = GetSpaceDescriptor()->GetDimension() * sizeof(uint); //Single block for low or high tuple.
		uint sizeMbr = parent::GetItemCount() * 2 * blockDataSize; //*2 because of two tuples are stored in inner node (low and high MBR)
		uint sizeChildren = parent::GetItemCount()*sizeof(int); //array of children indices
		cMemoryBlock* memBlock = parent::mHeader->GetMemoryManager()->GetMem(sizeMbr + sizeChildren);
		uint* mbr = (unsigned int*)memBlock->GetMem();
		uint* children = mbr + sizeMbr;
		SerializeKeys(mbr, children);
		//mmc->CopyNodeToGpuNew(GetSpaceDescriptor()->GetDimension(),serialized,size,false,parent::GetItemCount(),parent::GetIndex(),gpuId,gpuItemOrder);
		mmc->TransferNodeToGpu(cTreeHeader::HEADER_NODE, parent::GetIndex(), mbr, children, sizeMbr, sizeChildren, parent::GetItemCount());

		parent::mHeader->GetMemoryManager()->ReleaseMem(memBlock);
		//	cDebugCpu<TKey>::PrintInnerNodeCpu(this,GetSpaceDescriptor()); //debug
		//	cDebugGpu<TKey>::PrintInnerNodeGpu(mmc,GetGpuItemOrder(),GetSpaceDescriptor()->GetDimension());
#ifdef CUDA_MEASURE
		tmrCopy.Stop();
		cCudaTimer::TimeHtoD += tmrCopy.GetRealTime();
#endif
	}
}

template<class TMbr>
inline void cRTreeNode<TMbr>::SerializeKeys(uint* mbr, uint* children)
{
	uint dim = GetSpaceDescriptor()->GetDimension();
	uint blockDataSize = GetSpaceDescriptor()->GetDimension() * sizeof(uint);
	uint loOffset = 0;
	uint hiOffset = parent::GetItemCount() * dim;
	bool isParrentLeaf = parent::IsLeaf();
	for (unsigned int i = 0; i < parent::GetItemCount(); i++)
	{
		memcpy(mbr + loOffset, TMbr::GetLoTuple(parent::GetCKey(i)), blockDataSize);
		loOffset += dim;
		memcpy(mbr + hiOffset, TMbr::GetHiTuple(parent::GetCKey(i), GetSpaceDescriptor()), blockDataSize);
		hiOffset += dim;
		if (!isParrentLeaf)
			children[i] = parent::GetNodeIndex(parent::GetLink(i));
		else
			children[i] = parent::GetItemPOrder(i);
	}
	
}
template<class TMbr>
inline void cRTreeNode<TMbr>::SerializeKeys(uint* data)
{
	uint offset = parent::GetItemCount() * 2 * GetSpaceDescriptor()->GetDimension();
	SerializeKeys(data,data+offset);

}
