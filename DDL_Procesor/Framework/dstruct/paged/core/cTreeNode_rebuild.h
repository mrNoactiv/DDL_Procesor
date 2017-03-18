// Allocates memory for temporary variables
template<class TKey>
inline void cTreeNode<TKey>::Rebuild_pre(uint itemCount, cNodeBuffers<TKey>* buffers)
{
	// TODO - Exact allocation for initial subnodes

	// for decompressed items
	uint size = itemCount * (mHeader->GetItemSize() + sizeof(tItemOrder));

	// for subnodes arrays
	size += sizeof(cLinkedList<sCoverRecord>) + (itemCount * (sizeof(sCoverRecord)));

	// for subnodes masks
	size += itemCount * TMask::ByteSize(((cSpaceDescriptor*)mHeader->GetKeyDescriptor())->GetDimension());

	// for temporary reference items
	size += TKey::GetMaxSize(NULL, mHeader->GetKeyDescriptor());

	// get the memory from the mem pool
	buffers->riMemBlock = mHeader->GetMemoryManager()->GetMem(size);
	char* buffer = buffers->riMemBlock->GetMem();

	buffers->tmpNode = buffer;
	buffer += itemCount * (mHeader->GetItemSize() + sizeof(tItemOrder));

	buffers->subNodes = (cLinkedList<sCoverRecord>*)buffer;
	buffer += sizeof(cLinkedList<sCoverRecord>);
	buffers->subNodes->Init(buffer);
	buffer += itemCount * (sizeof(sCoverRecord));

	buffers->subNodesMasks = buffer;
	buffer += itemCount * TMask::ByteSize(((cSpaceDescriptor*)mHeader->GetKeyDescriptor())->GetDimension());

	buffers->refItems = buffer;
	buffer += TKey::GetMaxSize(NULL, mHeader->GetKeyDescriptor());
}


// Deallocates temporary memory
template<class TKey>
inline void cTreeNode<TKey>::Rebuild_post(cNodeBuffers<TKey>* buffers)
{
	mHeader->GetMemoryManager()->ReleaseMem(buffers->riMemBlock);
}

// Allocates memory for temporary variables
template<class TKey>
inline void cTreeNode<TKey>::Rebuild2_pre(uint itemCount, cNodeBuffers<TKey>* buffers)
{
	// TODO - Exact allocation for merged masks

	// transition arrays
	uint size = sizeof(cLinkedList<sCoverRecord>) + (itemCount * sizeof(cLinkedListNode<sCoverRecord>)); 

	// transition masks
	size += itemCount * TMask::ByteSize(((cSpaceDescriptor*)mHeader->GetKeyDescriptor())->GetDimension());

	// for subnodes orders
	size += (itemCount + 1) * (sizeof(ushort));

	// for masks of merged subnodes during whole rebuild
	size += 3 * mItemCount * TMask::ByteSize(((cSpaceDescriptor*)mHeader->GetKeyDescriptor())->GetDimension());

	// get the memory from the mem pool
	buffers->riMemBlock2 = mHeader->GetMemoryManager()->GetMem(size);
	char* buffer = buffers->riMemBlock2->GetMem();

	buffers->transition = (cLinkedList<sCoverRecord>*)buffer;
	buffer += sizeof(cLinkedList<sCoverRecord>);
	buffers->transition->Init(buffer);
	buffer += itemCount * sizeof(cLinkedListNode<sCoverRecord>);

	buffers->transMasks = buffer;
	buffer += itemCount * TMask::ByteSize(((cSpaceDescriptor*)mHeader->GetKeyDescriptor())->GetDimension());

	buffers->subNodesOrders = (ushort*)buffer;
	buffer += (itemCount + 1) * (sizeof(ushort));

	buffers->mergedMasks = buffer;
	buffer += 2 * mItemCount * TMask::ByteSize(((cSpaceDescriptor*)mHeader->GetKeyDescriptor())->GetDimension());

	buffers->mergedMasks2 = buffer;
	buffer += mItemCount * TMask::ByteSize(((cSpaceDescriptor*)mHeader->GetKeyDescriptor())->GetDimension());
}

template<class TKey>
inline void cTreeNode<TKey>::Rebuild2_post(cNodeBuffers<TKey>* buffers)
{
	mHeader->GetMemoryManager()->ReleaseMem(buffers->riMemBlock2);
}


// Allocates memory for temporary variables
template<class TKey>
inline void cTreeNode<TKey>::Rebuild3_pre(uint itemCount, cNodeBuffers<TKey>* buffers)
{
	// for reconstructed subnode/node
	uint size = mHeader->GetNodeInMemSize(); 

	// for reference items of new subnodes
	size += itemCount * TKey::GetMaxSize(NULL, mHeader->GetKeyDescriptor());

	// get the memory from the mem pool
	buffers->riMemBlock3 = mHeader->GetMemoryManager()->GetMem(size);
	char* buffer = buffers->riMemBlock3->GetMem();

	buffers->tmpNode2 = buffer;
	buffer += mHeader->GetNodeInMemSize();

	buffers->refItems = buffer;
	buffer += itemCount * TKey::GetMaxSize(NULL, mHeader->GetKeyDescriptor());
}

template<class TKey>
inline void cTreeNode<TKey>::Rebuild3_post(cNodeBuffers<TKey>* buffers)
{
	mHeader->GetMemoryManager()->ReleaseMem(buffers->riMemBlock3);
}

template<class TKey>
inline void cTreeNode<TKey>::Rebuild_pre(cNodeBuffers<TKey>* buffers)
{
	uint size = 2 * mHeader->GetNodeInMemSize(); // for reconstructed subnode/node

	// get the memory from the mem pool
	buffers->riMemBlock = mHeader->GetMemoryManager()->GetMem(size);
	char* buffer = buffers->riMemBlock->GetMem();

	buffers->tmpNode = buffer;
	buffer += uint(1.5 * mHeader->GetNodeInMemSize());

	buffers->tmpNodeItemOrders = (ushort*)buffer;
}

// Creates temporary node with all reconstructed items of subnode or whole node
template<class TKey>
char* cTreeNode<TKey>::Reconstruction(char* subNode, const char* key, char* data, uint lOrder, cNodeBuffers<TKey>* buffers)
{
	uint startLOrder = (subNode == NULL) ? 0 : TSubNode::GetFirstItemOrder(subNode);
	uint endLOrder = (subNode == NULL) ? mItemCount : (TSubNode::GetLastItemOrder(subNode) + 2);
	uint totalItemSize = mItemCount * mHeader->GetItemSize();
	uint freeSize = totalItemSize;
	uint itemCount = 0;
	tItemOrder* itemOrders = (tItemOrder*)(buffers->tmpNode + totalItemSize);
	char *aKey, *aData;
	bool inserted = false;

	for (ushort i = 0; i < this->GetSubNodesCount(); i++)
	{
		char* subNodeHeader = this->GetSubNodeHeader(i);
		for (ushort j = TSubNode::GetFirstItemOrder(subNodeHeader); j <= TSubNode::GetLastItemOrder(subNodeHeader); j++)
		{
			if ((inserted) || (j != lOrder))
			{
				GetKeyData(j, &aKey, &aData, &buffers->itemBuffer, i);
			}
			else
			{
				aKey = (char*)key;
				aData = data;
				inserted = true;
			}

			uint pOrder = totalItemSize - freeSize;
			uint keySize = TKey::GetSize(aKey, mHeader->GetKeyDescriptor());
			uint dataSize = GetDataSize(aData);
			memcpy(buffers->tmpNode + pOrder, aKey, keySize);
			memcpy(buffers->tmpNode + pOrder + keySize, aData, dataSize);
			itemOrders[itemCount++] = pOrder;
			freeSize -= (keySize + dataSize);
		}
	}
	return buffers->tmpNode;
}

// Returns reconstructed key
template<class TKey>
inline char* cTreeNode<TKey>::GetKey(char* rNode, uint lOrder, uint itemCount)
{
	itemCount = (itemCount != UINT_MAX) ? itemCount : mItemCount;
	uint totalItemSize = itemCount * mHeader->GetItemSize();
	tItemOrder* itemOrders = (tItemOrder*)(rNode + totalItemSize);

	return rNode + itemOrders[lOrder];
}

// Returns reconstructed key
template<class TKey>
inline char* cTreeNode<TKey>::GetData(char* rNode, uint lOrder, uint itemCount)
{
	itemCount = (itemCount != UINT_MAX) ? itemCount : mItemCount;
	uint totalItemSize = itemCount * mHeader->GetItemSize(); 
	tItemOrder* itemOrders = (tItemOrder*)(rNode + totalItemSize);

	return rNode + itemOrders[lOrder] + TKey::GetSize(rNode + itemOrders[lOrder], mHeader->GetKeyDescriptor());
}


template<class TKey> inline void cTreeNode<TKey>::SetItemPOrder(char* lOrders, const tItemOrder &lOrder, const tItemOrder &pOrder)
{
	cTreeNodeHeader* header = GetNodeHeader();

	assert(header->GetItemOrderOffset() + lOrder <= header->GetLinksOffset());
	*(((tItemOrder*) (lOrders + header->GetItemOrderOffset())) + lOrder) = pOrder;
}

template<class TKey>
inline char* cTreeNode<TKey>::CreateSubNode(char* tmpNode, uint pOrder, sCoverRecord* snRecord, uint previousSubNodePOrder)
{
	return TSubNode::CreateSubNode(tmpNode, pOrder, snRecord, previousSubNodePOrder, mHeader->GetKeyDescriptor());
}

// Creates the new subnode on the specific position (pOrder) in the node and returns it
// pOrder - physical order of subnode
// snRecord - record with subnode information like mask, minimal reference item, item order intervals
template<class TKey>
inline char* cTreeNode<TKey>::CreateSubNode(uint pOrder, sCoverRecord* snRecord, uint previousSubNodePOrder)
{
	char* subNode = TSubNode::CreateSubNode(this->GetItems(), pOrder, snRecord, previousSubNodePOrder, mHeader->GetKeyDescriptor());

	IncSubNodesCount();
	return subNode;
}

// Computes Minimal Reference Items of particular subnodes
template<class TKey>
void cTreeNode<TKey>::RefItemsReconstruction(char* rNode, cLinkedList<sCoverRecord>* subNodes, cNodeBuffers<TKey>* buffers)
{
	const cDTDescriptor *keyDescriptor = mHeader->GetKeyDescriptor();
	uint keySize = TKey::GetMaxSize(NULL, keyDescriptor);
	bool debug = false;

	sCoverRecord *current = NULL;
	cLinkedListNode<sCoverRecord>* currentNode = NULL;
	unsigned int currentNodeOrder = 0;
	for (uint i = 0; i < subNodes->GetItemCount(); i++)
	{
		current = &subNodes->GetRefItem(&currentNode, currentNodeOrder, i)->Item;

		TKey::Copy(buffers->refItems, GetKey(rNode, current->startItemOrder), keyDescriptor);
		for (uint j = current->startItemOrder + 1; j <= current->endItemOrder; j++)
		{
			buffers->refItems = TKey::SetMinRefItem(buffers->refItems, GetKey(rNode, j), buffers->refItems, keyDescriptor);
		}

		current->minRefItem = buffers->refItems;
		buffers->refItems += keySize;
	}
}

template<class TKey>
uint cTreeNode<TKey>::ComputeSize(sCoverRecord* subNode1, sCoverRecord* subNode2, char* mergedMask, cNodeBuffers<TKey>* buffers)
{
	const cDTDescriptor *keyDescriptor = mHeader->GetKeyDescriptor();
	char* rNode = buffers->tmpNode;
	uint dsMode = mHeader->GetDStructMode();
	uint maskLength = TKey::GetLength(GetKey(rNode, 0), keyDescriptor); // WARNING !!! Not works for variable length masks
	uint snHeaderSize = GetSubNodeHeaderSize(GetKey(rNode, 0));
	uint size = snHeaderSize;
	char* mask = (subNode2 != NULL) ? mergedMask : subNode1->mask;
	uint start = subNode1->startItemOrder;
	uint end = (subNode2 != NULL) ? subNode2->endItemOrder : subNode1->endItemOrder;

	if (dsMode == cDStructConst::DSMODE_RI)
	{
		if (TKey::CODE == cTuple::CODE)
		{
			size += (end - start + 1) * (TMask::GetNumberOfBits(mask, maskLength, 0) * sizeof(uint)); // uint = 4b, cUInt = 8b
		}
		else
		{
			uint trueBits = TMask::GetNumberOfBits(mask, maskLength, 1);
			for (uint i = start; i <= end; i++)
			{
				char* key = GetKey(rNode, i);
				uint keyLength = TKey::GetLength(key, keyDescriptor);

				size += (keyLength - trueBits) * sizeof(uint);
			}
		}
	}
	else
	{
		char *minRI = buffers->refItems, *cutKey = buffers->itemBuffer.riBuffer;
		TKey::Copy(minRI, GetKey(rNode, start), keyDescriptor);
		for (uint i = start + 1; i <= end; i++)
		{
			minRI = TKey::SetMinRefItem(minRI, GetKey(rNode, i), minRI, keyDescriptor);
		}

		for (uint i = start; i <= end; i++)
		{
			char* key = GetKey(rNode, i);
			uint keyLength = TKey::CutTuple(mask, minRI, key, cutKey, keyDescriptor);
			size += TKey::GetEncodedSize(mHeader->GetCodeType(), cutKey, keyDescriptor, keyLength);
		}
	}

	return size;
}

// Computes and returns the basic distribution of subnodes with maximal weights
template<class TKey>
cLinkedList<sCoverRecord>* cTreeNode<TKey>::ComputeMaxMaskDistribution(char* rNode, uint itemCount, cNodeBuffers<TKey>* buffers)
{
	const cDTDescriptor *keyDescriptor = mHeader->GetKeyDescriptor();
	uint maskLength = TKey::GetLength(GetKey(rNode, 0), keyDescriptor); // WARNING !!! Not works for variable length masks
	uint maskSize = TMask::ByteSize(maskLength);

	sCoverRecord item = { 0, 1, TKey::SetMask(GetKey(rNode, 0), GetKey(rNode, 1), buffers->subNodesMasks, keyDescriptor),  NULL, false};
	buffers->subNodes->AddItem(item);
	buffers->subNodesMasks += maskSize;
	for (ushort i = 2; i < itemCount; i++)
	{
		char* mask = TKey::SetMask(GetKey(rNode, i - 1), GetKey(rNode, i), buffers->subNodesMasks, keyDescriptor);

		sCoverRecord *lastItem = buffers->subNodes->GetLastItem();
		if (TMask::Equal(mask, lastItem->mask, maskLength)) // masks are same
		{
			lastItem->endItemOrder = i;
		}
		else
		{
			//sCoverRecord nextItem;
			if ((TMask::GetNumberOfBits(mask, maskLength, true) > TMask::GetNumberOfBits(lastItem->mask, maskLength, true)))
			{
				if (lastItem->endItemOrder - lastItem->startItemOrder == 1)
					TMask::SetBits(lastItem->mask, maskLength, true);

				lastItem->endItemOrder--;
				sCoverRecord nextItem = { (ushort)(i - 1), i, mask, NULL, false };
			    buffers->subNodes->AddItem(nextItem);
			}
			else
			{
				if (i >= itemCount - 2)
				{
					TMask::SetBits(mask, maskLength, true);
					sCoverRecord nextItem = { i, i, mask, NULL, false };
					buffers->subNodes->AddItem(nextItem);
				}
				else
				{
					mask = TKey::SetMask(GetKey(rNode, i), GetKey(rNode, i + 1), buffers->subNodesMasks, keyDescriptor);
					sCoverRecord nextItem = { i, (ushort)(i + 1), mask, NULL, false };
					buffers->subNodes->AddItem(nextItem);
				}
			}

		}

		buffers->subNodesMasks += maskSize;
	}

	return buffers->subNodes;
}

template<class TKey>
cLinkedList<sCoverRecord>* cTreeNode<TKey>::ComputeTransition(char* rNode, cLinkedList<sCoverRecord>* subNodes, cNodeBuffers<TKey>* buffers)
{
	const cDTDescriptor *keyDescriptor = mHeader->GetKeyDescriptor();
	uint maskLength = TKey::GetLength(GetKey(rNode, 0), keyDescriptor); // WARNING !!! Not works for variable length masks
	uint maskSize = TMask::ByteSize(maskLength);

	cLinkedListNode<sCoverRecord> *currentNode = NULL;
	uint currentNodeOrder = 0;
	sCoverRecord *current = &subNodes->GetRefItem(&currentNode, currentNodeOrder, 0)->Item;
	sCoverRecord *next = NULL;
	ushort snCount = subNodes->GetItemCount() - 1;
	for (ushort i = 0; i < snCount; i++)
	{
		next = &subNodes->GetRefItem(&currentNode, currentNodeOrder, i + 1)->Item;

		char* mask = TKey::SetMask(GetKey(rNode, current->endItemOrder), current->mask, GetKey(rNode, current->endItemOrder + 1), next->mask, buffers->transMasks, keyDescriptor);
		sCoverRecord item = { current->endItemOrder, (ushort)(current->endItemOrder + 1), mask };
		buffers->transition->AddItem(item);
		buffers->transMasks += maskSize;
		current = next;
	}

	return buffers->transition;
}


// Merges subnodes according to transitions
// If transitions between more neigbour subnodes have same weights, try to merge them
// It ends when all transitions are processed
template<class TKey>
cLinkedList<sCoverRecord>* cTreeNode<TKey>::MergeSubNodesByTransition(char* rNode, cLinkedList<sCoverRecord>* subNodes, cLinkedList<sCoverRecord>* transition, cNodeBuffers<TKey>* buffers)
{
	const cDTDescriptor *keyDescriptor = mHeader->GetKeyDescriptor();
	uint maskLength = TKey::GetLength(GetKey(rNode, 0), keyDescriptor); // WARNING !!! Not works for variable length masks
	uint maskSize = TMask::ByteSize(maskLength);
	ushort* snOrders = buffers->subNodesOrders;
	bool debug = false;

	while (true)
	{
		uint weight = 0, aWeight = 0, count = 0;
		cLinkedListNode<sCoverRecord> *currentNode = NULL, *currentNode2 = NULL;
		uint currentNodeOrder = 0, currentNodeOrder2 = 0;
		sCoverRecord *item = NULL, *item2 = NULL, *previous = NULL;
		for (uint i = 0; i < transition->GetItemCount(); i++)
		{
			item = &transition->GetRefItem(&currentNode, currentNodeOrder, i)->Item;
			if (!item->optimal)
			{
				item2 = &subNodes->GetRefItem(&currentNode2, currentNodeOrder2, i)->Item;

				char* tmpMask = TMask::And(item->mask, item2->mask, buffers->mergedMasks, maskSize); // we do that because of two subnodes with masks (1,0,0) and transition mask f.e. (0,1,0)
				aWeight = TMask::GetNumberOfBits(tmpMask, maskLength, true);
				if (aWeight > weight)
				{
					weight = aWeight;
					count = 0;
					snOrders[count++] = i;
				}
				else if ((aWeight == weight) && (count > 0) && (i == snOrders[count - 1] + 1) && TMask::Equal(item->mask, previous->mask, maskLength))
				{
					snOrders[count++] = i;
				}
			}
			previous = item;
		}
		if (weight == 0) // if maximal weight is 0, no more subnodes is possible to merge
		{
			break;
		}
		else
		{
			snOrders[count++] = snOrders[count - 1] + 1; // transition is relationship between two subnodes - in cycle is added left subnode(s), this adds right subnode
		}


		currentNode = NULL;
		currentNodeOrder = 0;
		sCoverRecord* previousSubNode = &subNodes->GetRefItem(&currentNode, currentNodeOrder, snOrders[0])->Item;

		uint nonMergedSize = ComputeSize(previousSubNode, NULL, NULL, buffers);
		char* mergedMask = TMask::Copy(buffers->mergedMasks, previousSubNode->mask, maskSize);
		for (uint i = 1; i < count; i++)
		{
			sCoverRecord* subNode = &subNodes->GetRefItem(&currentNode, currentNodeOrder, snOrders[i])->Item;
			mergedMask = TKey::MergeMasks(mergedMask, subNode->mask, GetKey(rNode, previousSubNode->startItemOrder), GetKey(rNode, subNode->startItemOrder), mergedMask, keyDescriptor);
			nonMergedSize += ComputeSize(subNode, NULL, NULL, buffers);  
			previousSubNode = subNode;
		}

		currentNode = NULL;
		currentNodeOrder = 0;
		sCoverRecord* startItem = &subNodes->GetRefItem(&currentNode, currentNodeOrder, snOrders[0])->Item;
		sCoverRecord* endItem = &subNodes->GetRefItem(&currentNode, currentNodeOrder, snOrders[count - 1])->Item;
		uint mergedSize = ComputeSize(startItem, endItem, mergedMask, buffers);

		if (mergedSize < nonMergedSize) // it means, two merged blocks saves more space
		{
			startItem->mask = mergedMask;
			startItem->endItemOrder = endItem->endItemOrder;

			subNodes->DeleteItem(snOrders[count - 1]);
			for (uint i = count - 2; i > 0; i--)
			{
				transition->DeleteItem(snOrders[i]);
				subNodes->DeleteItem(snOrders[i]);
			}
			transition->DeleteItem(snOrders[0]);

			buffers->mergedMasks += maskSize;
		}
		else
		{
			currentNode = NULL;
			currentNodeOrder = 0;
			for (uint i = 0; i < count - 1; i++)
			{
				sCoverRecord* current = &transition->GetRefItem(&currentNode, currentNodeOrder, snOrders[i])->Item;
				current->optimal = true;
			}
		}

		if (debug)
			Print(transition, rNode);
	}

	return subNodes;
}
/*
template<class TKey>
uint cTreeNode<TKey>::ComputeSize(cLinkedList<sCoverRecord>* subNodes, ushort startItem, ushort endItem, cNodeBuffers<TKey>* buffers)
{
	const cDTDescriptor *keyDescriptor = mHeader->GetKeyDescriptor();
	char* rNode = buffers->tmpNode;
	uint maskLength = TKey::GetLength(GetKey(rNode, 0), keyDescriptor); // WARNING !!! Not works for variable length masks
	uint maskSize = TMask::ByteSize(maskLength);

	cLinkedListNode<sCoverRecord> *currentNode = NULL;
	uint currentNodeOrder = 0;
	sCoverRecord *start = &subNodes->GetRefItem(&currentNode, currentNodeOrder, startItem)->Item;
	sCoverRecord *current = start, *previous = start;

	char* mergedMask = TMask::Copy(buffers->mergedMasks, start->mask, maskSize);
	for (uint i = startItem + 1; i <= endItem; i++)
	{
		current = &subNodes->GetRefItem(&currentNode, currentNodeOrder, i)->Item;
		mergedMask = TKey::MergeMasks(mergedMask, current->mask, GetKey(rNode, previous->startItemOrder), GetKey(rNode, current->startItemOrder), mergedMask, keyDescriptor);
		previous = current;
	}
	buffers->mergedMasks += maskSize;
	return ComputeSize(start, current, mergedMask, buffers);
}*/

template<class TKey>
cLinkedList<sCoverRecord>* cTreeNode<TKey>::SplitToSubNodes(cLinkedList<sCoverRecord>* subNodes, uint startItem, uint endItem, cNodeBuffers<TKey>* buffers)
{
	const cDTDescriptor *keyDescriptor = mHeader->GetKeyDescriptor();
	char* rNode = buffers->tmpNode;
	uint maskLength = TKey::GetLength(GetKey(rNode, 0), keyDescriptor); // WARNING !!! Not works for variable length masks
	uint maskSize = TMask::ByteSize(maskLength);
	int maxSave = 0, maxSaveOrder = 0;
	char *mergedMaskLeft, *mergedMaskRight;
	bool debug = false;

	cLinkedListNode<sCoverRecord> *currentNode = NULL;
	uint currentNodeOrder = 0;
	sCoverRecord *start = &subNodes->GetRefItem(&currentNode, currentNodeOrder, startItem)->Item;
	sCoverRecord *current = start, *previous = start;

	char* mergedMask = TMask::Copy(buffers->mergedMasks, start->mask, maskSize);
	for (uint i = startItem + 1; i <= endItem; i++)
	{
		current = &subNodes->GetRefItem(&currentNode, currentNodeOrder, i)->Item;
		mergedMask = TKey::MergeMasks(mergedMask, current->mask, GetKey(rNode, previous->startItemOrder), GetKey(rNode, current->startItemOrder), mergedMask, keyDescriptor);
		previous = current;
	}
	buffers->mergedMasks += maskSize;
	uint totalSize = ComputeSize(start, current, mergedMask, buffers);


	currentNode = NULL;	currentNodeOrder = 0;
	for (uint i = startItem; i <= endItem; i++)
	{
		current = &subNodes->GetRefItem(&currentNode, currentNodeOrder, i)->Item;
		uint snSize = ComputeSize(current, NULL, NULL, buffers);
		uint leftSize = 0, rightSize = 0;

		if (i > startItem)
		{
			cLinkedListNode<sCoverRecord> *currentNode2 = NULL;
			uint currentNodeOrder2 = 0;
			mergedMaskLeft = TMask::Copy(buffers->mergedMasks, start->mask, maskSize);
			sCoverRecord *current2 = start, *previous2 = start;
			for (uint j = startItem + 1; j < i; j++)
			{
				current2 = &subNodes->GetRefItem(&currentNode2, currentNodeOrder2, j)->Item;
				mergedMaskLeft = TKey::MergeMasks(mergedMaskLeft, current2->mask, GetKey(rNode, previous2->startItemOrder), GetKey(rNode, current2->startItemOrder), mergedMaskLeft, keyDescriptor);
				previous2 = current2;
			}
			leftSize = ComputeSize(start, current2, mergedMaskLeft, buffers);
		}

		if (i < endItem)
		{
			cLinkedListNode<sCoverRecord> *currentNode2 = NULL;
			uint currentNodeOrder2 = 0;
			sCoverRecord *start2 = &subNodes->GetRefItem(&currentNode2, currentNodeOrder2, i + 1)->Item;
			mergedMaskRight = TMask::Copy(buffers->mergedMasks2, start2->mask, maskSize);
			sCoverRecord *current2 = start2, *previous2 = start2;

			for (uint j = i + 2; j <= endItem; j++)
			{
				current2 = &subNodes->GetRefItem(&currentNode2, currentNodeOrder2, j)->Item;
				mergedMaskRight = TKey::MergeMasks(mergedMaskRight, current2->mask, GetKey(rNode, previous2->startItemOrder), GetKey(rNode, current->startItemOrder), mergedMaskRight, keyDescriptor);
				previous2 = current2;
			}
			rightSize = ComputeSize(start2, current2, mergedMaskRight, buffers);
		}

		int save = totalSize - (leftSize + snSize + rightSize);
		if (save > maxSave)
		{
			maxSave = save;
			maxSaveOrder = i;
		}
	}

	if (maxSave > 0)
	{
		subNodes->GetItem(maxSaveOrder)->optimal = true;
		if (startItem < maxSaveOrder)
			SplitToSubNodes(subNodes, startItem, maxSaveOrder - 1, buffers);
		if (endItem > maxSaveOrder)
			SplitToSubNodes(subNodes, maxSaveOrder + 1, endItem, buffers);
	}
	else
	{
		start->optimal = true;
		start->mask = mergedMask;
		start->endItemOrder = current/*subNodes->GetItem(endItem)*/->endItemOrder;
	}

	return subNodes;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class TKey>
void cTreeNode<TKey>::Print(cLinkedList<sCoverRecord>* list, char* rNode)
{
	printf("\n");
	for (uint i = 0; i < list->GetItemCount(); i++)
	{
		//if (list->GetItem(i)->startItemOrder != TBlock::NOT_DEFINED)
		{
			printf("%i. Start: %d End: %d , O:%d  ", i, list->GetItem(i)->startItemOrder, list->GetItem(i)->endItemOrder, list->GetItem(i)->optimal ? 1 : 0);
			cBitString::Print(list->GetItem(i)->mask, TKey::GetLength(GetKey(rNode, 0), mHeader->GetKeyDescriptor()));
			printf("\n");
		}
	}
}

template<class TKey>
void cTreeNode<TKey>::NodeRebuild(cNodeBuffers<TKey>* buffers)
{
	Rebuild(NULL, NULL, NULL, TSubNode::NOT_DEFINED, buffers);
	this->SetUpdatesCount(0);
}

template<class TKey>
bool cTreeNode<TKey>::IsDistributionDifferent(char* rNode, cLinkedList<sCoverRecord>* subNodes, bool nodeRebuild)
{
	uint realSnCount = nodeRebuild ? 1 : GetSubNodesCount();

	if (subNodes->GetItemCount() != realSnCount)
	{
		return true;
	}
	else
	{
		const cDTDescriptor *keyDescriptor = mHeader->GetKeyDescriptor();
		uint maskLength = TKey::GetLength(GetKey(rNode, 0), keyDescriptor); // WARNING !!! Not works for variable length masks

		cLinkedListNode<sCoverRecord> *currentNode = NULL;
		uint currentNodeOrder = 0;

		for (ushort i = 0; i < realSnCount; i++)
		{
			char* subNodeHeader = this->GetSubNodeHeader(i);
			subNodes->GetRefItem(&currentNode, currentNodeOrder, i);
			sCoverRecord* newSubNode = &currentNode->Item;

			//sCoverRecord* newSubNode = subNodes->GetItem(i);
			if ((TSubNode::GetFirstItemOrder(subNodeHeader) != newSubNode->startItemOrder)
				|| (TSubNode::GetLastItemOrder(subNodeHeader) != newSubNode->endItemOrder)
				|| (!TMask::Equal(TSubNode::GetMask(this->GetItems(), subNodeHeader), newSubNode->mask, maskLength))
				|| (!TKey::Equal(TSubNode::GetMinRefItem(this->GetItems(), subNodeHeader), newSubNode->minRefItem, keyDescriptor))
				)
			{
				return true;
			}
		}
	}
	return false;
}

template<class TKey>
void cTreeNode<TKey>::Rebuild(char* subNode, const char* key, char* data, uint lOrder, cNodeBuffers<TKey>* buffers)
{
	const cDTDescriptor *keyDescriptor = mHeader->GetKeyDescriptor();
	uint formerFreeSize = mFreeSize;
	uint itemCount = (subNode == NULL) ? mItemCount : (TSubNode::GetItemsCount(subNode) + 1);
	Rebuild_pre(itemCount, buffers); // allocation of temporary variables 

	bool debug = false;
	if (debug)
	{
		Print2File("origin", false, &buffers->itemBuffer);
	}

	// create uncompressed version of subnode or whole node
	char* rNode = Reconstruction(subNode, key, data, lOrder, buffers);

	//bool debug = false;
	if (debug)
	{
		FILE *streamInfo = fopen("recon", "a");
		for (uint i = 0; i < itemCount; i++)
		{
			fprintf(streamInfo, "%d.", i);
			TKey::Print2File(streamInfo, GetKey(rNode, i), "\n", keyDescriptor);
		}

		fclose(streamInfo);
	}

	// create initial distribution of subnodes with maximal mask weights
	cLinkedList<sCoverRecord>* subNodes = ComputeMaxMaskDistribution(rNode, itemCount, buffers);

	if (debug)
	{
		printf("\nInitial Subnodes");
		Print(subNodes, rNode);
	}

	// create transition masks of neigbour subnodes
	Rebuild2_pre(subNodes->GetItemCount() - 1, buffers);
	cLinkedList<sCoverRecord>* transition = ComputeTransition(rNode, subNodes, buffers);

	if (debug)
	{
		printf("\nInitial Transitions");
		Print(transition, rNode);
	}

	// merge neighbour subnodes with maximal transition
	subNodes = MergeSubNodesByTransition(rNode, subNodes, transition, buffers);

	if (debug)
	{
		printf("\nSubnodes after MergeSubNodesByTransition");
		Print(subNodes, rNode);
	}

	if (subNodes->GetItemCount() > 1)
	{
		for (uint i = 0; i < subNodes->GetItemCount(); i++)
		{
			subNodes->GetItem(i)->optimal = false;
		}

		subNodes = SplitToSubNodes(subNodes, 0, subNodes->GetItemCount() - 1, buffers);
		for (uint i = 0; i < subNodes->GetItemCount();)
		{
			if (subNodes->GetItem(i)->optimal)
			{
				i++;
			}
			else
			{
				subNodes->DeleteItem(subNodes->GetItem(i));
			}
		}
	}

	if (debug)
	{
		printf("\nSubnodes after SplitToSubNodes");
		Print(subNodes, rNode);
	}

	Rebuild3_pre(subNodes->GetItemCount(), buffers);
	RefItemsReconstruction(rNode, subNodes, buffers);
	if (IsDistributionDifferent(rNode, subNodes, (subNode != NULL)))
	{
		Rebuild(rNode, subNode, subNodes, buffers);
	}

	if (debug)
	{
		Print2File("new", false, &buffers->itemBuffer);
	}

	Rebuild_post(buffers);
	Rebuild2_post(buffers);
	Rebuild3_post(buffers);

	if (mHeader->GetRuntimeMode() == cDStructConst::RTMODE_VALIDATION)
	{
		if (mFreeSize < formerFreeSize)
		{
			//printf("Validation Test: Bad Rebuild");
		}
	}
}

// this method is called in the case of rebuild (in the array distribution is description of new subnodes)
template<class TKey>
void cTreeNode<TKey>::Rebuild(char* rNode, char* subNodeHeader, cLinkedList<sCoverRecord>* subNodes, cNodeBuffers<TKey>* buffers)
{
	bool nodeRebuild = (subNodeHeader == NULL);
	const cDTDescriptor *keyDescriptor = mHeader->GetKeyDescriptor();
	uint startLOrder = (nodeRebuild) ? 0 : TSubNode::GetFirstItemOrder(subNodeHeader);
	uint endLOrder = (nodeRebuild) ? mItemCount : (TSubNode::GetLastItemOrder(subNodeHeader) + 2);
	subNodeHeader = (nodeRebuild) ? this->GetSubNodeHeader(0) : subNodeHeader;
	uint pOrder = TSubNode::GetSubNodePOrder(subNodeHeader);
	sItemBuffers* itemBuffer = &buffers->itemBuffer;

	char *key, *cutKey, *data;
	uint keyLength, keySize, dataSize;

	ushort tmpCapacity = subNodes->GetItemCount();
	ushort tmpOffset = GetNodeHeader()->GetItemOrderOffset() - (tmpCapacity * TSubNode::HEADER_SIZE);
	char* tmpSubNodes = buffers->tmpNode2 + GetNodeHeader()->GetItemsOffset();
	char* tmpSubNodesHeaders = buffers->tmpNode2 + tmpOffset;

	// Toto tu musi byt ked nebude dochadzat ku kopirovaniu z tmp uzla
	//if (nodeRebuild)
	//	SetSubNodesCount(0);

	cLinkedListNode<sCoverRecord> *currentNode = NULL;
	uint currentNodeOrder = 0;
	for (ushort i = 0; i < subNodes->GetItemCount(); i++)
	{
		subNodes->GetRefItem(&currentNode, currentNodeOrder, i);
		sCoverRecord* currentSN = &currentNode->Item;

		//sCoverRecord* currentSN = subNodes->GetItem(i);

		uint riSize = TKey::GetSize(currentSN->minRefItem, keyDescriptor);
		uint riLength = TKey::GetLength(currentSN->minRefItem, keyDescriptor);
		uint snSize = (2 * riSize) + cBitString::ByteSize(riLength);

		//subNode = this->CreateSubNode(pOrder, currentSN, previousSubNodePOrder);
		subNodeHeader = TSubNode::CreateSubNode(tmpSubNodes, tmpSubNodesHeaders, i, pOrder, currentSN, keyDescriptor);
		for (uint j = currentSN->startItemOrder; j <= currentSN->endItemOrder; j++)
		{
			key = GetKey(rNode, j);
			cutKey = tmpSubNodes + pOrder + snSize;
			//cutKey = this->GetItems() + pOrder + snSize;
			keyLength = TKey::CutTuple(currentSN->mask, currentSN->minRefItem, key, cutKey, keyDescriptor);  //CutKey(subNodeHeader, key, cutKey);
			keySize = TKey::GetLSize(keyLength, keyDescriptor);
			data = GetData(rNode, j);
			dataSize = GetDataSize(data);

			// WARNING - Since we use node for cutKey, even non compressed item has to fit into node
			if (pOrder + snSize + keySize + dataSize > mHeader->GetNodeItemsSpaceSize() - (tmpCapacity * TSubNode::HEADER_SIZE))
			{
				return; //int c = 3;
			}

			if (mHeader->GetDStructMode() == cDStructConst::DSMODE_RICODING)
			{
				keySize = TKey::Encode(mHeader->GetCodeType(), cutKey, itemBuffer->codingBuffer, keyDescriptor, keyLength);
				memcpy(tmpSubNodes + pOrder + snSize, itemBuffer->codingBuffer, keySize);
				//memcpy(this->GetItems() + pOrder + snSize, itemBuffer->codingBuffer, keySize);
			}

			memcpy(tmpSubNodes + pOrder + snSize + keySize, data, GetDataSize(data));
			//memcpy(this->GetItems() + pOrder + snSize + keySize, data, GetDataSize(data));
			SetItemPOrder(buffers->tmpNode2, j, pOrder + snSize);
			//SetItemPOrder(j, pOrder + snSize);
			snSize += keySize + GetDataSize(data);
			if (pOrder + snSize > mHeader->GetNodeItemsSpaceSize())
			{
				return; //int c = 3;
			}

		}
		keySize = TKey::GetLSize(TKey::GetLength(key, keyDescriptor), keyDescriptor);
		TSubNode::SetMaxRefItem(tmpSubNodes, subNodeHeader, key, keySize);
		TSubNode::SetLastByte(subNodeHeader, pOrder + snSize);
		pOrder += snSize;

	}

	memcpy(this->GetItems(), tmpSubNodes, mHeader->GetNodeItemsSpaceSize() + mItemCount * sizeof(ushort));
	//memcpy(mData + GetNodeHeader()->GetItemOrderOffset(), buffers->tmpNode2 + GetNodeHeader()->GetItemOrderOffset(), mItemCount * sizeof(ushort));

/*	uint c = GetNodeHeader()->GetExtraLinksOffset();
	uint d = mHeader->GetNodeItemsSpaceSize() + mItemCount * sizeof(ushort);
	if (c < d)
	{
		uint e = 4;
	}

	assert(GetNodeHeader()->GetExtraLinksOffset() > mHeader->GetNodeItemsSpaceSize() + mItemCount * sizeof(ushort));*/

	this->SetSubNodesCount(subNodes->GetItemCount());
	this->SetSubNodesCapacity(subNodes->GetItemCount());
	this->SetSubNodeHeadersOffset(GetNodeHeader()->GetItemOrderOffset() - GetNodeHeader()->GetItemsOffset() - this->GetSubNodesCount() * TSubNode::HEADER_SIZE);

	if (!nodeRebuild)
	{
		this->SetSubNodesCount(this->GetSubNodesCount() - 1); // it must be -1, because of origin subnode
	}
	mFreeSize = TSubNode::TotalFreeSize(this->GetSubNodeHeaders(), this->GetSubNodeHeadersOffset(), this->GetSubNodesCount());
}


template<class TKey>
char* cTreeNode<TKey>::Rebuild(ushort lSubNodeOrder, const char* key, cNodeBuffers<TKey>* buffers)
{
	cTreeNodeHeader *nodeHeader = GetNodeHeader();
	const cDTDescriptor *keyDescriptor = mHeader->GetKeyDescriptor();
	sItemBuffers *tmpItem1 = &buffers->itemBuffer, *tmpItem2 = &buffers->itemBuffer2;

	// get subnode header
	char* subNodeHeader = this->GetSubNodeHeader(lSubNodeOrder);

	// create new mask and reference item
	char* newMask = TKey::SetMask(TSubNode::GetMask(this->GetItems(), subNodeHeader), TSubNode::GetMinRefItem(this->GetItems(), subNodeHeader), key, tmpItem1->riBuffer, keyDescriptor);
	char* newMinRefItem = TKey::SetMinRefItem(TSubNode::GetMinRefItem(this->GetItems(), subNodeHeader), key, tmpItem2->riBuffer, keyDescriptor);

	// mask and reference item of subnode is not neccessary to modify
	if (TMask::Equal(newMask, TSubNode::GetMask(this->GetItems(), subNodeHeader), TKey::GetLength(key, keyDescriptor)) && (TKey::Equal(TSubNode::GetMinRefItem(this->GetItems(), subNodeHeader), newMinRefItem, keyDescriptor) == 0))
	{
		return subNodeHeader;
	}

	bool debug = false;
	if (debug)
	{
		Print2File("subnode.txt", false, &buffers->itemBuffer);
	}
	// prepare temporary buffers
	Rebuild_pre(buffers);
	char* tmpNode = buffers->tmpNode; ushort* tmpItemOrders = buffers->tmpNodeItemOrders;

	uint oldMaskLength = TKey::GetLength(TSubNode::GetMinRefItem(this->GetItems(), subNodeHeader), keyDescriptor);
	uint newMaskLength = TKey::GetLength(newMinRefItem, keyDescriptor);
	uint oldMaskSize = TMask::ByteSize(oldMaskLength);
	uint newMaskSize = TMask::ByteSize(newMaskLength);
	uint oldMinRefItemSize = TKey::GetSize(TSubNode::GetMinRefItem(this->GetItems(), subNodeHeader), keyDescriptor);
	uint newMinRefItemSize = TKey::GetSize(newMinRefItem, keyDescriptor);
	uint maxRefItemSize = TKey::GetSize(TSubNode::GetMaxRefItem(this->GetItems(), subNodeHeader), keyDescriptor);

	// create header of modified subnode
	memcpy(tmpNode, newMask, newMaskSize);
	memcpy(tmpNode + newMaskSize, newMinRefItem, newMinRefItemSize);
	memcpy(tmpNode + newMaskSize + newMinRefItemSize, TSubNode::GetMaxRefItem(this->GetItems(), subNodeHeader), maxRefItemSize);

	newMask = tmpNode;
	newMinRefItem = tmpNode + newMaskSize;
	uint snSize = newMaskSize + newMinRefItemSize + maxRefItemSize;
	uint diffSize = (newMaskSize - oldMaskSize) + (newMinRefItemSize - oldMinRefItemSize);

	// rebuild all items of modified subnode
	char *rKey, *rData, *cutKey;
	for (uint i = TSubNode::GetFirstItemOrder(subNodeHeader); i <= TSubNode::GetLastItemOrder(subNodeHeader); i++)
	{
		GetKeyData(i, &rKey, &rData, tmpItem1, lSubNodeOrder);
		cutKey = tmpNode + snSize;
		uint keyLength = TKey::CutTuple(newMask, newMinRefItem, rKey, cutKey, keyDescriptor); 
		uint keySize = TKey::GetLSize(keyLength, keyDescriptor);

		if (mHeader->GetDStructMode() == cDStructConst::DSMODE_RICODING)
		{
			keySize = TKey::Encode(mHeader->GetCodeType(), cutKey, tmpItem2->codingBuffer, keyDescriptor, keyLength);
			memcpy(tmpNode + snSize, tmpItem2->codingBuffer, keySize);
		}

		memcpy(tmpNode + snSize + keySize, rData, GetDataSize(rData));

		tmpItemOrders[i - TSubNode::GetFirstItemOrder(subNodeHeader)] = diffSize;
		diffSize += keySize - ((i == TSubNode::GetLastItemOrder(subNodeHeader)) ? (TSubNode::GetLastByte(subNodeHeader) - GetItemPOrder(i)) : (GetItemPOrder(i + 1) - GetItemPOrder(i)));
		diffSize += GetDataSize(rData);
		snSize += keySize + GetDataSize(rData);
	}

	if (debug)
	{
		Print2File("subnode.txt", false, &buffers->itemBuffer);
	}

	// find place for rebuilded subnode
	uint oldSnSize = this->CompleteSize(lSubNodeOrder);
	int neededBytes = (snSize > oldSnSize) ? (snSize - oldSnSize) : 0;

	if (snSize >= oldSnSize)
	{
		if ((mFreeSize - this->FreeSize(lSubNodeOrder)) >= neededBytes)
		{
			SubNodeReplace(lSubNodeOrder, neededBytes);
			//subNode = this->GetSubNodeHeader(lSubNodeOrder); // it is neccessary, because subNode could be shifted
		}
		else
		{
			Rebuild_post(buffers);
			return NULL;
		}
	}

	// physical copy
	memcpy(TSubNode::GetMask(this->GetItems(), subNodeHeader), tmpNode, snSize);

	// update itemOrders
	int j = 0;
	for (uint i = TSubNode::GetFirstItemOrder(subNodeHeader); i <= TSubNode::GetLastItemOrder(subNodeHeader); i++)
	{
		SetItemPOrder(i, GetItemPOrder(i) + tmpItemOrders[j++]);
	}
	TSubNode::SetLastByte(subNodeHeader, TSubNode::GetSubNodePOrder(subNodeHeader) + snSize);

	Rebuild_post(buffers);

	mFreeSize -= diffSize;
	return subNodeHeader;
}

// Allocates memory for temporary variables
template<class TKey>
inline void cTreeNode<TKey>::Rebuild_Rtree_pre(uint itemCount, cNodeBuffers<TKey>* buffers)
{
	// for decompressed items
	uint size = itemCount * (mHeader->GetItemSize() + sizeof(tItemOrder));

	// for node mbr and for splitted mbr1, mbr2
	uint mbrSize = 2 * GetNodeHeader()->GetItemSize();
	size += 2 * RTREE_SN_COUNT * mbrSize;

	uint maskSize = TMask::ByteSize(((cSpaceDescriptor*)GetNodeHeader()->GetKeyDescriptor())->GetDimension());
	size += RTREE_SN_COUNT * maskSize;

	uint size_mbrSide = ((cSpaceDescriptor*)GetNodeHeader()->GetKeyDescriptor())->GetDimension() * sizeof(tMbrSideSizeOrder);
	size += size_mbrSide;

	size += mHeader->GetNodeInMemSize();

	// get the memory from the mem pool
	buffers->riMemBlock = mHeader->GetMemoryManager()->GetMem(size);
	char* buffer = buffers->riMemBlock->GetMem();

	buffers->tmpNode = buffer;
	buffer += itemCount * (mHeader->GetItemSize() + sizeof(tItemOrder));

	buffers->mbrs = buffer;
	buffer += 2 * RTREE_SN_COUNT * mbrSize;

	buffers->mbrSide = (tMbrSideSizeOrder*)buffer;
	buffer += size_mbrSide;

	buffers->masks = buffer;
	buffer += RTREE_SN_COUNT * maskSize;

	buffers->tmpNode2 = buffer;
	buffer += mHeader->GetNodeInMemSize();
}


template<class TKey>
void cTreeNode<TKey>::NodeRebuild_Rtree(cNodeBuffers<TKey>* buffers)
{
	const cDTDescriptor *keyDescriptor = mHeader->GetKeyDescriptor();
	const cSpaceDescriptor *spaceDescriptor = (cSpaceDescriptor*)keyDescriptor;
	Rebuild_Rtree_pre(mItemCount, buffers); // allocation of temporary variables 

	char* rNode = Reconstruction(NULL, NULL, NULL, TSubNode::NOT_DEFINED, buffers);


	bool debug = false;
	if (debug)
	{
		FILE *streamInfo = fopen("recon", "a");
		for (uint i = 0; i < mItemCount; i++)
		{
			fprintf(streamInfo, "%d.", i);
			TKey::Print2File(streamInfo, GetKey(rNode, i), "\n", keyDescriptor);
		}

		fclose(streamInfo);
	}

	char* nodeMbr = GetMbr(buffers->mbrs, 0);
	TMBR::Copy(nodeMbr, TSubNode::GetMinRefItem(this->GetItems(), this->GetSubNodeHeader(0)), spaceDescriptor);
	for (ushort i = 1; i < this->GetSubNodesCount(); i++)
	{
		TMBR::ModifyMbrByMbr(nodeMbr, TSubNode::GetMinRefItem(this->GetItems(), this->GetSubNodeHeader(i)), spaceDescriptor);
	}

	//	TKey::Print(TMBR::GetLoTuple(buffers->nodeMbr), "\n", keyDescriptor);
	//	TKey::Print(TMBR::GetHiTuple(buffers->nodeMbr, spaceDescriptor), "\n", keyDescriptor);

	Rebuild_CutLongest(0, mItemCount - 1, 0, 0, buffers);

	//	TKey::Print(TMBR::GetLoTuple(buffers->mbr1), "\n", keyDescriptor);
	//	TKey::Print(TMBR::GetHiTuple(buffers->mbr1, spaceDescriptor), "\n", keyDescriptor);
	//	TKey::Print(TMBR::GetLoTuple(buffers->mbr2), "\n", keyDescriptor);
	//	TKey::Print(TMBR::GetHiTuple(buffers->mbr2, spaceDescriptor), "\n", keyDescriptor);

	//if (TMBR::IsIntersected(buffers->mbr1, buffers->mbr2, spaceDescriptor))
	//{
	//	printf("Rebuild error: Subnodes are intersected !!!");
	//}

	Rebuild_ComputeMasks(buffers);

	//	TMask::Print(buffers->mask1, spaceDescriptor->GetDimension());
	//	TMask::Print(buffers->mask2, spaceDescriptor->GetDimension());

	Rebuild_Rtree(buffers);


	Rebuild_post(buffers);
	this->SetUpdatesCount(0);

}

template<class TKey>
inline char* cTreeNode<TKey>::GetMbr(char* mbrs, ushort lOrder)
{
	uint mbrSize = 2 * GetNodeHeader()->GetItemSize();
	return mbrs + (lOrder * mbrSize);
}

template<class TKey>
inline char* cTreeNode<TKey>::GetMask(char* masks, ushort lOrder)
{
	uint maskSize = TMask::ByteSize(((cSpaceDescriptor*)GetNodeHeader()->GetKeyDescriptor())->GetDimension());
	return masks + (lOrder * maskSize);
}

template<class TKey>
void cTreeNode<TKey>::Rebuild_Rtree(cNodeBuffers<TKey>* buffers)
{
	const cDTDescriptor *keyDescriptor = mHeader->GetKeyDescriptor();
	const cSpaceDescriptor *spaceDescriptor = (cSpaceDescriptor*)keyDescriptor;
	uint dim = spaceDescriptor->GetDimension();
	uint pOrder = 0;
	sItemBuffers* itemBuffer = &buffers->itemBuffer;
	bool first = true;
	char* rNode = buffers->tmpNode;
	uint itemOrder = 0;

	char *key, *cutKey, *data;
	uint keyLength, keySize, dataSize;

	ushort tmpCapacity = RTREE_SN_COUNT;// subNodes->GetItemCount();
	ushort tmpOffset = GetNodeHeader()->GetItemOrderOffset() - (tmpCapacity * TSubNode::HEADER_SIZE);
	char* tmpSubNodes = buffers->tmpNode2 + GetNodeHeader()->GetItemsOffset();
	char* tmpSubNodesHeaders = buffers->tmpNode2 + tmpOffset;
	
	uint snSize = 0;
	char* subNodeHeader = NULL;
	char* finalMbrs = GetMbr(buffers->mbrs, pow(2, SPLIT_COUNT) - 1);
	for (uint i = 0; i < RTREE_SN_COUNT; i++)
	{
		char* mask = GetMask(buffers->masks, i);
		char* mbr = GetMbr(finalMbrs, i);
		subNodeHeader = TSubNode::CreateSubNode(tmpSubNodes, tmpSubNodesHeaders, i, pOrder, itemOrder, GetMask(buffers->masks, i), GetMbr(finalMbrs, i), keyDescriptor);
		snSize = (2 * TKey::GetSize(NULL, keyDescriptor)) + cBitString::ByteSize(dim);

		for (uint j = itemOrder; j < mItemCount;)
		{
			key = GetKey(rNode, j);
			if (TMBR::IsInRectangle(TMBR::GetLoTuple(mbr), TMBR::GetHiTuple(mbr, spaceDescriptor), key, spaceDescriptor))
			{
				cutKey = tmpSubNodes + pOrder + snSize;
				keyLength = TKey::CutTuple(mask, TMBR::GetLoTuple(mbr), key, cutKey, keyDescriptor);  //CutKey(subNodeHeader, key, cutKey);
				keySize = TKey::GetLSize(keyLength, keyDescriptor);
				data = GetData(rNode, j);
				dataSize = GetDataSize(data);

				if (pOrder + snSize + keySize + dataSize > mHeader->GetNodeItemsSpaceSize() - (tmpCapacity * TSubNode::HEADER_SIZE))
				{
					return; //int c = 3;
				}

				if (mHeader->GetDStructMode() == cDStructConst::DSMODE_RICODING)
				{
					keySize = TKey::Encode(mHeader->GetCodeType(), cutKey, itemBuffer->codingBuffer, keyDescriptor, keyLength);
					memcpy(tmpSubNodes + pOrder + snSize, itemBuffer->codingBuffer, keySize);
				}

				memcpy(tmpSubNodes + pOrder + snSize + keySize, data, GetDataSize(data));
				SetItemPOrder(buffers->tmpNode2, j, pOrder + snSize);
				snSize += keySize + dataSize;
				j++;
			}
			else
			{
				TSubNode::SetLastByte(subNodeHeader, pOrder + snSize);
				TSubNode::SetLastItemOrder(subNodeHeader, j - 1);
				pOrder += snSize;
				itemOrder = j;
				break;
			}
		}
	}
	TSubNode::SetLastByte(subNodeHeader, pOrder + snSize);
	TSubNode::SetLastItemOrder(subNodeHeader, mItemCount - 1);

	memcpy(this->GetItems(), tmpSubNodes, mHeader->GetNodeItemsSpaceSize() + mItemCount * sizeof(ushort));
	//memcpy(mData + GetNodeHeader()->GetItemOrderOffset(), buffers->tmpNode2 + GetNodeHeader()->GetItemOrderOffset(), mItemCount * sizeof(ushort));

	this->SetSubNodesCount(RTREE_SN_COUNT);
	this->SetSubNodesCapacity(RTREE_SN_COUNT);
	this->SetSubNodeHeadersOffset(GetNodeHeader()->GetItemOrderOffset() - GetNodeHeader()->GetItemsOffset() - this->GetSubNodesCount() * TSubNode::HEADER_SIZE);

	mFreeSize = TSubNode::TotalFreeSize(this->GetSubNodeHeaders(), this->GetSubNodeHeadersOffset(), this->GetSubNodesCount());
}

template<class TKey>
void cTreeNode<TKey>::Rebuild_Rtree(ushort snCount, char* finalMbrs, ushort itemStart, ushort itemEnd, uint baseItemCount, cNodeBuffers<TKey>* buffers)
{
	const cDTDescriptor *keyDescriptor = mHeader->GetKeyDescriptor();
	const cSpaceDescriptor *spaceDescriptor = (cSpaceDescriptor*) keyDescriptor;
	uint dim = spaceDescriptor->GetDimension();
	uint pOrder = 0;
	sItemBuffers* itemBuffer = &buffers->itemBuffer;
	bool first = true;
	char* rNode = buffers->tmpNode;
	uint itemOrder = itemStart;

	char *key, *cutKey, *data;
	uint keyLength, keySize, dataSize;

	ushort tmpCapacity = snCount;// subNodes->GetItemCount();
	ushort tmpOffset = GetNodeHeader()->GetItemOrderOffset() - (tmpCapacity * TSubNode::HEADER_SIZE);
	char* tmpSubNodes = buffers->tmpNode2 + GetNodeHeader()->GetItemsOffset();
	char* tmpSubNodesHeaders = buffers->tmpNode2 + tmpOffset;

	uint snSize = 0;
	char* subNodeHeader = NULL;
	for (uint i = 0; i < snCount; i++)
	{
		char* mask = GetMask(buffers->masks, i);
		char* mbr = GetMbr(finalMbrs, i);
		subNodeHeader = TSubNode::CreateSubNode(tmpSubNodes, tmpSubNodesHeaders, i, pOrder, itemOrder - itemStart, mask, mbr, keyDescriptor);
		snSize = (2 * TKey::GetSize(NULL, keyDescriptor)) + cBitString::ByteSize(dim);

		for (uint j = itemOrder; j < itemEnd;)
		{
			key = GetKey(rNode, j, baseItemCount);
			//TKey::Print(key, "\n", keyDescriptor);
			//TKey::Print(TMBR::GetLoTuple(mbr), "\n", keyDescriptor);
			//TKey::Print(TMBR::GetHiTuple(mbr, spaceDescriptor), "\n", keyDescriptor);
			if (TMBR::IsInRectangle(TMBR::GetLoTuple(mbr), TMBR::GetHiTuple(mbr, spaceDescriptor), key, spaceDescriptor))
			{
				cutKey = tmpSubNodes + pOrder + snSize;
				keyLength = TKey::CutTuple(mask, TMBR::GetLoTuple(mbr), key, cutKey, keyDescriptor);  //CutKey(subNodeHeader, key, cutKey);
				keySize = TKey::GetLSize(keyLength, keyDescriptor);
				data = GetData(rNode, j, baseItemCount);
				dataSize = GetDataSize(data);

				if (pOrder + snSize + keySize + dataSize > mHeader->GetNodeItemsSpaceSize() - (tmpCapacity * TSubNode::HEADER_SIZE))
				{
					return; //int c = 3;
				}

				if (mHeader->GetDStructMode() == cDStructConst::DSMODE_RICODING)
				{
					keySize = TKey::Encode(mHeader->GetCodeType(), cutKey, itemBuffer->codingBuffer, keyDescriptor, keyLength);
					memcpy(tmpSubNodes + pOrder + snSize, itemBuffer->codingBuffer, keySize);
				}

				memcpy(tmpSubNodes + pOrder + snSize + keySize, data, GetDataSize(data));
				SetItemPOrder(buffers->tmpNode2, j - itemStart, pOrder + snSize);
				snSize += keySize + dataSize;
				j++;
			}
			else
			{
				TSubNode::SetLastByte(subNodeHeader, pOrder + snSize);
				TSubNode::SetLastItemOrder(subNodeHeader, j - itemStart - 1);
				pOrder += snSize;
				itemOrder = j;
				break;
			}
		}
	}
	TSubNode::SetLastByte(subNodeHeader, pOrder + snSize);
	TSubNode::SetLastItemOrder(subNodeHeader, itemEnd - itemStart - 1);

	memcpy(this->GetItems(), tmpSubNodes, mHeader->GetNodeItemsSpaceSize() + ((itemEnd - itemStart)) * sizeof(ushort));
	//memcpy(mData + GetNodeHeader()->GetItemOrderOffset(), buffers->tmpNode2 + GetNodeHeader()->GetItemOrderOffset(), mItemCount * sizeof(ushort));

	this->SetItemCount(itemEnd - itemStart);
	this->SetSubNodesCount(snCount);
	this->SetSubNodesCapacity(snCount);
	this->SetSubNodeHeadersOffset(GetNodeHeader()->GetItemOrderOffset() - GetNodeHeader()->GetItemsOffset() - this->GetSubNodesCount() * TSubNode::HEADER_SIZE);

	mFreeSize = TSubNode::TotalFreeSize(this->GetSubNodeHeaders(), this->GetSubNodeHeadersOffset(), this->GetSubNodesCount());
}

template<class TKey>
void cTreeNode<TKey>::Rebuild_ComputeMasks(cNodeBuffers<TKey>* buffers)
{
	const cDTDescriptor *keyDescriptor = mHeader->GetKeyDescriptor();
	const cSpaceDescriptor *spaceDescriptor = (cSpaceDescriptor*) keyDescriptor;
	uint dim = spaceDescriptor->GetDimension();
	uint itemOrder = 0;

	char* finalMbrs = GetMbr(buffers->mbrs, pow(2, SPLIT_COUNT) - 1);
	for (uint i = 0; i < RTREE_SN_COUNT; i++)
	{
		char* mask = GetMask(buffers->masks, i);
		cBitString::SetBits(mask, dim, true);
		char* mbr = GetMbr(finalMbrs, i);
		for (uint j = itemOrder; j < mItemCount;)
		{
			if (TMBR::IsInRectangle(TMBR::GetLoTuple(mbr), TMBR::GetHiTuple(mbr, spaceDescriptor), GetKey(buffers->tmpNode, j), spaceDescriptor))
			{
				mask = TKey::SetMask(mask, TMBR::GetLoTuple(mbr), GetKey(buffers->tmpNode, j), mask, keyDescriptor);
				j++;
			}
			else
			{
				itemOrder = j;
				break;
			}
		}
	}
}

template<class TKey>
void cTreeNode<TKey>::Rebuild_ComputeMasks(ushort snCount, char* finalMbrs, ushort itemStart, ushort itemEnd, uint baseItemCount, cNodeBuffers<TKey>* buffers)
{
	const cDTDescriptor *keyDescriptor = mHeader->GetKeyDescriptor();
	const cSpaceDescriptor *spaceDescriptor = (cSpaceDescriptor*) keyDescriptor;
	uint dim = spaceDescriptor->GetDimension();
	uint itemOrder = itemStart;

	for (uint i = 0; i < snCount; i++)
	{
		char* mask = GetMask(buffers->masks, i);
		cBitString::SetBits(mask, dim, true);
		char* mbr = GetMbr(finalMbrs, i);
		for (uint j = itemOrder; j < itemEnd;)
		{
			if (TMBR::IsInRectangle(TMBR::GetLoTuple(mbr), TMBR::GetHiTuple(mbr, spaceDescriptor), GetKey(buffers->tmpNode, j, baseItemCount), spaceDescriptor))
			{
				mask = TKey::SetMask(mask, TMBR::GetLoTuple(mbr), GetKey(buffers->tmpNode, j, baseItemCount), mask, keyDescriptor);
				j++;
			}
			else
			{
				itemOrder = j;
				break;
			}
		}
	}
}

template<class TKey>
void cTreeNode<TKey>::Rebuild_CutLongest(uint loOrder, uint hiOrder, ushort mbrOrder, ushort splitOrder, cNodeBuffers<TKey>* buffers)
{
	const cDTDescriptor *keyDescriptor = mHeader->GetKeyDescriptor();
	const cSpaceDescriptor *spaceDescriptor = (cSpaceDescriptor*)keyDescriptor;

	unsigned int dim = spaceDescriptor->GetDimension();

	char* mbr = GetMbr(buffers->mbrs, mbrOrder);
	cMbrSideSizeOrder<TKey>::ComputeSidesSize(TMBR::GetLoTuple(mbr), TMBR::GetHiTuple(mbr, spaceDescriptor), buffers->mbrSide, spaceDescriptor);
	cMbrSideSizeOrder<TKey>::QSortUInt(buffers->mbrSide, dim);

	float utilization = 0.49f;
	const float minUtilization = 0.38f;
	bool disjMbrsFound = false;
	int midOrder = -1;

	while (!disjMbrsFound)
	{
		// 50:50 utilization is not preserved
		for (unsigned int i = 0; i < dim; i++)
		{
			unsigned int dimOrder = buffers->mbrSide[i].Order;
			if ((midOrder = Rebuild_FindTwoDisjMbrs(dimOrder, loOrder, hiOrder, mbrOrder, utilization, buffers)) > (int)loOrder)
			{
				disjMbrsFound = true;
				break;
			}
		}

		if (!disjMbrsFound)
		{
			utilization -= 0.05f;
			if (utilization < 0.0)
			{
				printf("Critical Error: cRTreeLeafNode<TKey>::Split: utilization < 0.0!\n");
				break;
			}
		}
	}

	if (utilization < minUtilization)
	{
		printf("Warning: cRTreeLeafNode<TKey>::Split(): Two disjunctive mbrs are found but utilization < %.2f (%.2f)!\n", minUtilization, utilization);
	}

	if (splitOrder < SPLIT_COUNT - 1)
	{
		splitOrder++;
		Rebuild_CutLongest(loOrder, midOrder, (2*mbrOrder) + 1, splitOrder, buffers);
		Rebuild_CutLongest(midOrder + 1, hiOrder, (2 * mbrOrder) + 2, splitOrder, buffers);
	}
}

template<class TKey>
int cTreeNode<TKey>::Rebuild_FindTwoDisjMbrs(uint dimOrder, uint loOrder, uint hiOrder, uint mbrOrder, float pUtilization, cNodeBuffers<TKey>* buffers)
{
	const cDTDescriptor *keyDescriptor = mHeader->GetKeyDescriptor();
	const cSpaceDescriptor *spaceDescriptor = (cSpaceDescriptor*)keyDescriptor;

	unsigned int state, index;
	uint itemCount = hiOrder - loOrder + 1;
	unsigned minimal_count = itemCount * pUtilization;
	float diff;//, itemCount = (float)((float)parent::mItemCount * 0.5);    // mk: node utilization
	unsigned int halfItemCount = itemCount / 2;
	bool disjmbr = false;
	unsigned int dim = spaceDescriptor->GetDimension();

	int order = -1;

	// for solving of the same values problem
	if (itemCount % 2 == 0)
	{
		diff = 0.5;
		halfItemCount -= diff;
	}
	else
	{
		diff = 0.0;
	}

	bool sameValues = Rebuild_SortBy(dimOrder, loOrder, hiOrder, buffers);

	if (sameValues)
	{
		return false;
	}

	// solve problem with the same values in dimension
	if (itemCount % 2 == 0)
	{
		state = 1;
	}
	else
	{
		state = 0;
	}

	index = 0;
	bool probSameValues = false;

	for (;;)
	{
		if (state == 0)
		{
			order = (unsigned int)halfItemCount;
			state = 1;
			index++;
		}
		else if (state == 1)
		{
			order = (unsigned int)(halfItemCount - diff - index);
			state = 2;
		}
		else
		{
			order = (unsigned int)(halfItemCount + diff + index);
			index++;
			state = 1;
		}

		if (index != 0 && (order < minimal_count || order >= itemCount - 1))    // if all values in dimension are the same, then continue with next dimension
		{
			probSameValues = true;
			break;
		}

		if (TKey::Equal(GetKey(buffers->tmpNode, loOrder + order), GetKey(buffers->tmpNode, loOrder+order + 1), dimOrder, spaceDescriptor) != 0)
		{
			break;
		}
	}

	if (!probSameValues)
	{
		Rebuild_CreateMbr(loOrder, loOrder + order, GetMbr(buffers->mbrs, (2 * mbrOrder) + 1), buffers);
		Rebuild_CreateMbr(loOrder + order + 1, hiOrder, GetMbr(buffers->mbrs, (2 * mbrOrder) + 2), buffers);
	}
	else
	{
		order = -1;
	}

	return loOrder+order;
}

/// Sort nodes's tuples according values in dimension. Select-Sort is applied.
template<class TKey>
bool cTreeNode<TKey>::Rebuild_SortBy(uint dimension, uint loOrder, uint hiOrder, cNodeBuffers<TKey>* buffers)
{
	const cDTDescriptor *keyDescriptor = mHeader->GetKeyDescriptor();
	const cSpaceDescriptor *spaceDescriptor = (cSpaceDescriptor*)keyDescriptor;

	bool sortedFlag = true;
	bool sameValues = true;
	uint itemCount = hiOrder - loOrder + 1;

	// check if the sequence is sorted
	for (unsigned int i = loOrder; i < hiOrder; i++)
	{
		int cmp = TKey::Equal(GetKey(buffers->tmpNode, i), GetKey(buffers->tmpNode, i + 1), dimension, spaceDescriptor);
		if (cmp > 0)
		{
			sortedFlag = false;
			sameValues = false;
			break;
		}
		else if (cmp < 0)
		{
			sameValues = false;
		}
	}

	if (!sortedFlag)
	{
		// select-sort
		unsigned int min;

		for (unsigned int i = loOrder; i <= hiOrder; i++)
		{
			min = i;
			for (unsigned int j = i; j <= hiOrder; j++)
			{
				if (TKey::Equal(GetKey(buffers->tmpNode, j), GetKey(buffers->tmpNode, min), dimension, spaceDescriptor) < 0)
				{
					min = j;
				}
			}

			if (i != min)
			{
				Rebuild_SwapItemOrder(buffers->tmpNode, i, min);
			}
		}
	}

	return sameValues;
}

template<class TKey>
void cTreeNode<TKey>::Rebuild_CreateMbr(uint startOrder, uint finishOrder, char* TMbr_mbr, cNodeBuffers<TKey>* buffers) //const
{
	const cDTDescriptor *keyDescriptor = mHeader->GetKeyDescriptor();
	const cSpaceDescriptor *spaceDescriptor = (cSpaceDescriptor*)keyDescriptor;

	sItemBuffers* itemBuffer = &buffers->itemBuffer;
	char* TKey_ql = TMBR::GetLoTuple(TMbr_mbr);
	char* TKey_qh = TMBR::GetHiTuple(TMbr_mbr, spaceDescriptor);

	TKey::Copy(TKey_ql, GetKey(buffers->tmpNode, startOrder), spaceDescriptor);
	TKey::Copy(TKey_qh, GetKey(buffers->tmpNode, startOrder), spaceDescriptor);

	for (unsigned int i = startOrder + 1; i <= finishOrder; i++)
	{
		TMBR::ModifyMbr(TKey_ql, TKey_qh, GetKey(buffers->tmpNode, i), spaceDescriptor);
	}
}


template<class TKey>
inline void cTreeNode<TKey>::Rebuild_SwapItemOrder(char* rNode, uint lOrder1, uint lOrder2)
{
	uint totalItemSize = mItemCount * mHeader->GetItemSize();
	tItemOrder* itemOrders = (tItemOrder*)(rNode + totalItemSize);

	if (lOrder1 != lOrder2)
	{
		tItemOrder pOrder = itemOrders[lOrder2];
		itemOrders[lOrder2] = itemOrders[lOrder1];
		itemOrders[lOrder1] = pOrder;
	}
}