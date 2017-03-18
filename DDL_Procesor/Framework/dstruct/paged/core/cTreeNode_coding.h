template<class TKey>
void cTreeNode<TKey>::SplitLeafNode_coding(cTreeNode<TKey>& newNode, cTreeNode<TKey> &tmpNode, cNodeBuffers<TKey>* buffers)
{
	unsigned int i, halfCount = (mItemCount + 1)/2;
	unsigned int debug = false;
	unsigned int size = 0, occupiedSpace;
	char *key, *data;
	cTreeNodeHeader *nodeHeader = GetNodeHeader();

	for (i = 0; i < halfCount; i++)
	{
		GetKeyData(i, &key, &data, &buffers->itemBuffer);
		tmpNode.SetItemPOrder(i, size);
		size += tmpNode.SetLeafItemPo(size, key, data, TKey::NOT_DEFINED, &buffers->itemBuffer2);
	}
	occupiedSpace = size;

	size = 0;
	for (unsigned int j = 0; i < mItemCount; j++, i++)
	{
		GetKeyData(i, &key, &data, &buffers->itemBuffer);
		newNode.SetItemPOrder(j, size);
		size += newNode.SetLeafItemPo(size, key, data, TKey::NOT_DEFINED, &buffers->itemBuffer2);
	}
	
	memcpy(mData + nodeHeader->GetItemOrderOffset(), tmpNode.GetPItemPOrder(0), halfCount * cTreeNodeHeader::ItemSize_ItemOrder);
	memcpy(mData + nodeHeader->GetItemsOffset(), tmpNode.GetItemPtr(0), occupiedSpace);

	newNode.SetItemCount(mItemCount - halfCount);
	SetItemCount(halfCount);
	mHeader->IncrementNodeCount();

	// compute free size
	assert(occupiedSpace <= GetNodeHeader()->GetNodeItemsSpaceSize());
	assert(size <= GetNodeHeader()->GetNodeItemsSpaceSize());
	mFreeSize = mHeader->GetNodeItemsSpaceSize() - occupiedSpace;
	newNode.SetFreeSize(newNode.GetNodeHeader()->GetNodeItemsSpaceSize() - size);
	
	//if (GetNodeHeader()->GetRuntimeMode() == cDStructConst::RTMODE_DEBUG)
	//{
	//	if (!IsLeaf())
	//	{
	//		printf("split of an inner node:\n");
	//		PrintCoding(codingKeyBuffer);
	//		newNode.PrintCoding(codingKeyBuffer);
	//	} else
	//	{
	//		printf("\nsplit of leaf nodes:\n");
	//		PrintCoding(codingKeyBuffer);
	//		newNode.PrintCoding(codingKeyBuffer);
	//	}
	//}
}



