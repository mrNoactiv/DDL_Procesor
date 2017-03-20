template<class TKey> 
inline void cRTreeLeafNode<TKey>::TransferLeafNodeToGpu()
{
	cMemoryManagerCuda* mmc = parent::mHeader->GetMemoryManagerCuda();
	uint tmp = parent::GetIndex();
	if (tmp == 37256)
	{
		int bla = 0;
	}
	if (!mmc->FindNode(parent::GetIndex())) //node is not on GPU
	{
		uint blockDataSize = GetSpaceDescriptor()->GetDimension() * sizeof(uint);
		uint sizeMbr = parent::GetItemCount() * blockDataSize; //*2 because of two tuples are stored in inner node (low and high MBR)
		//uint sizeData = parent::GetItemCount()*sizeof(char); //array of children indices
		cMemoryBlock* memBlock = parent::mHeader->GetMemoryManager()->GetMem(sizeMbr /*+ sizeData*/);
		uint* mbr = (uint*)memBlock->GetMem();
		//char* data =(char*)(mbr + sizeData);
		SerializeKeys(mbr);
		mmc->TransferNodeToGpu(cTreeHeader::HEADER_LEAFNODE, parent::GetIndex(), mbr, NULL, sizeMbr, 0, parent::GetItemCount());
		parent::mHeader->GetMemoryManager()->ReleaseMem(memBlock);
		if (cGpuConst::DEBUG_FLAG == true)
		{
			//cDebugCpu<TKey>::PrintLeafNodeCpu(this); //debug
			//cDebugGpu<TKey>::PrintLeafNodeGpu(mmc,gpuItemOrder,GetSpaceDescriptor()); //debug
		}
	}
}

template<class TKey> 
inline void cRTreeLeafNode<TKey>::SerializeKeys(uint* mbr/*,char* data*/)
{
	uint dim = GetSpaceDescriptor()->GetDimension();
	uint blockDataSize = GetSpaceDescriptor()->GetDimension() * sizeof(uint);
	unsigned int offset = 0;
	for (unsigned int i = 0 ; i < parent::GetItemCount() ; i++)
	{
		//const void* ptr = GetCItem(i);
		memcpy(mbr + offset, parent::GetCKey(i), blockDataSize);
		//cTuple* tpl = (cTuple*)ptr2;
		//tpl->Print("\n", GetSpaceDescriptor());
		offset += dim; 
		//data[i] = parent::GetCItem(i);
	}
}

//template<class TKey> 
//inline int cRTreeLeafNode<TKey>::SearchInBlock_CUDA(const TKey &ql, const TKey &qh, cItemStream* resultSet)
//{
//	int ret = NO_ITEM_FIND;
//	TransferBlockToGpu();
//	cMemoryPool* pool = GetNodeHeader()->GetMemoryPool();
//	cCudaWorker<TKey,cRTreeLeafNode<TKey>>::SearchBlockOnGPU(pool,Tree->CudaMemoryManagement,this,resultSet,(unsigned int*)ql.GetData(),(unsigned int*)qh.GetData());
//	return ret;
//}//}
