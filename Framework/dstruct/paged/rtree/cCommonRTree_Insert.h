/**
 * Insert item into inner node
 **/
template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr,TKey,TNode,TLeafNode>::
	InsertIntoInnerNode(TNode *currentInnerNode, const TKey &item, uint currentLevel, const tNodeIndex& insertNodeIndex, cInsertBuffers<TKey>* insertBuffers)
{
	cRTreeHeader<TKey>* header = GetRTreeHeader();

	currentInnerNode->AddItem(insertBuffers->tMbr_insert, insertNodeIndex, true);

	if (header->IsSignatureEnabled() && ((header->GetSignatureController()->GetBuildType() == cSignatureController::SignatureBuild_Insert)))
	{
		uint invLevel = header->GetHeight() - currentLevel;
		mSignatureIndex->ModifyNodeSignature(currentInnerNode->GetIndex(), item, invLevel, insertBuffers->nodeSignatures[invLevel], insertBuffers->ConvIndexKey, insertBuffers->ConvIndexData);
	}
}

/**
 * Insert item into leaf node
 **/
template<class TMbr, class TKey, class TNode, class TLeafNode>
int cCommonRTree<TMbr, TKey, TNode, TLeafNode>::InsertIntoLeafNode(TLeafNode *currentLeafNode, const TKey &item, char* leafData, cInsertBuffers<TKey>* insertBuffers)
{
	cRTreeHeader<TKey>* header = GetRTreeHeader();
	int insertFlag;

	if (parent::mDebug)
	{
		item.Print("\n", GetSpaceDescriptor());
		currentLeafNode->Print(GetSpaceDescriptor());
	}

	if (header->GetOrderingEnabled())
	{
		insertFlag = currentLeafNode->InsertLeafItem(item, leafData, header->DuplicatesAllowed(), &insertBuffers->nodeBuffer);
	}
	else
	{
		insertFlag = currentLeafNode->AddLeafItem(item, leafData, true, &insertBuffers->nodeBuffer);
	}

	if (parent::mDebug)
	{
		item.Print("\n", GetSpaceDescriptor());
		currentLeafNode->Print(GetSpaceDescriptor());
	}

	if (header->IsSignatureEnabled() && ((header->GetSignatureController()->GetBuildType() == cSignatureController::SignatureBuild_Insert)))
	{
		mSignatureIndex->ModifyNodeSignature(currentLeafNode->GetIndex(), item, 0, &insertBuffers->signatureBuffers);
	}

	ExtraInsertIntoLeafNode2(currentLeafNode, item, insertBuffers->tmpKeyORTree);
	
	bool debug = false;
	if (debug)
	{
		currentLeafNode->Print(GetSpaceDescriptor());
		mOrderIndex->PrintFT((currentLeafNode->GetIndex() & 0x7fffffff), GetSpaceDescriptor());
	}

	return insertFlag;
}

/**
 * There is no free space for the inserted item - split the leaf node.
 **/
template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr,TKey,TNode,TLeafNode>::
	SplitLeafNode(TLeafNode *currentLeafNode, const TKey &item, tNodeIndex &insertNodeIndex, char* leafData, cInsertBuffers<TKey>* insertBuffers)
{
	cRTreeHeader<TKey>* header = GetRTreeHeader();
	const cSpaceDescriptor *sd = GetSpaceDescriptor();
	tNodeIndex nodeWithInsert;

	TLeafNode *newLeafNode = parent::ReadNewLeafNode();

	currentLeafNode->Split(*newLeafNode, TMbr::GetLoTuple(insertBuffers->tMbr_mbr), TMbr::GetHiTuple(insertBuffers->tMbr_mbr, sd), &insertBuffers->nodeBuffer);
	
	if (header->GetOrderingEnabled())
	{
		if (parent::mDebug)
		{
			item.Print("\n", GetSpaceDescriptor());
			currentLeafNode->Print(GetSpaceDescriptor());
			newLeafNode->Print(GetSpaceDescriptor());
		}

		if (item.Equal(newLeafNode->GetCKey(0), GetSpaceDescriptor()) > 0)
		{
			newLeafNode->InsertLeafItem(item, leafData, header->DuplicatesAllowed(), &insertBuffers->nodeBuffer);
		}
		else
		{
			currentLeafNode->InsertLeafItem(item, leafData, header->DuplicatesAllowed(), &insertBuffers->nodeBuffer);
		}
	}
	else
	{
		nodeWithInsert = TLeafNode::InsertTuple(currentLeafNode, newLeafNode, item, GetRTreeHeader()->GetLeafNodeHeader(), leafData, &insertBuffers->nodeBuffer);
	}

	currentLeafNode->CreateMbr(TMbr::GetLoTuple(insertBuffers->tMbr_update), TMbr::GetHiTuple(insertBuffers->tMbr_update, sd), &insertBuffers->nodeBuffer);
	newLeafNode->CreateMbr(TMbr::GetLoTuple(insertBuffers->tMbr_insert), TMbr::GetHiTuple(insertBuffers->tMbr_insert, sd), &insertBuffers->nodeBuffer);

	insertNodeIndex = TNode::GetLeafNodeIndex(newLeafNode->GetIndex());

	// DELETE AFTER
	/*if (cTreeNode<tKey>::Leaf_Splits_Count >= 1)
	{
		mTree->GetRTreeHeader()->GetSignatureController()->SetSignatureQuality(cSignatureController::PerfectSignature);
	}*/

	if (header->IsSignatureEnabled() && ((header->GetSignatureController()->GetBuildType() == cSignatureController::SignatureBuild_Insert)) && mSignatureIndex->IsEnabled(0))
	{
		if (header->GetSignatureController()->GetSignatureQuality() == cSignatureController::PerfectSignature)
		{
			mSignatureIndex->CreateLeafNodeSignature(currentLeafNode, true, &insertBuffers->signatureBuffers);
			mSignatureIndex->CreateLeafNodeSignature(newLeafNode, false, &insertBuffers->signatureBuffers);

			// DELETE AFTER
			/*cTreeNode<TKey>::Leaf_Splits_Count = 0;
			mTree->GetRTreeHeader()->GetSignatureController()->SetSignatureQuality(cSignatureController::ImperfectSignature);*/
		}
		else
		{
			mSignatureIndex->ReplicateNodeSignature(newLeafNode->GetIndex(), currentLeafNode->GetIndex(), 0, &insertBuffers->signatureBuffers);
			mSignatureIndex->ModifyNodeSignature(nodeWithInsert, item, 0, &insertBuffers->signatureBuffers);
			
			printf("pruser");
			// DELETE AFTER
			//cTreeNode<TKey>::Leaf_Splits_Count++;
		}
		
	}

	if (header->GetOrderingEnabled())
	{
		insertBuffers->tmpKeyORTree.Copy(currentLeafNode->GetCKey(0), currentLeafNode->GetNodeHeader()->GetKeyDescriptor());
		FTInsertorUpdate((currentLeafNode->GetIndex() & 0x7fffffff), insertBuffers->tmpKeyORTree );
    
		insertBuffers->tmpKeyORTree.Copy(newLeafNode->GetCKey(0), newLeafNode->GetNodeHeader()->GetKeyDescriptor());
		FTInsertorUpdate((newLeafNode->GetIndex() & 0x7fffffff), insertBuffers->tmpKeyORTree );
	}

	if (parent::mDebug && header->GetOrderingEnabled())
	{
		currentLeafNode->Print(sd);
		mOrderIndex->PrintFT((currentLeafNode->GetIndex() & 0x7fffffff), GetSpaceDescriptor());
		newLeafNode->Print(sd);
		mOrderIndex->PrintFT((newLeafNode->GetIndex() & 0x7fffffff), GetSpaceDescriptor());
	}

	parent::mSharedCache->UnlockW(newLeafNode);
	parent::mSharedCache->UnlockW(currentLeafNode);
}

/**
*	Insert first item into R-tree
*/
template<class TMbr, class TKey, class TNode, class TLeafNode>
tNodeIndex cCommonRTree<TMbr, TKey, TNode, TLeafNode>::InsertFirstItemIntoRtree(const TKey &item, TNode *currentInnerNode, cInsertBuffers<TKey>* insertBuffers)
{
	cRTreeHeader<TKey>* header = GetRTreeHeader();
	tNodeIndex newNodeIndex;

	// Phase 1: the first leaf node is created
	TLeafNode *leafNode = parent::ReadNewLeafNode();
	leafNode->SetItemCount(0);
	leafNode->SetExtraLink(0, TNode::EMPTY_LINK);
	newNodeIndex = leafNode->GetIndex();

	parent::mHeader->SetHeight(1);
	parent::mHeader->SetLeafNodeCount(1);
	parent::mSharedCache->UnlockW(leafNode);

	// -----------------------------------------------------
	// Phase 2: update the root node
	char* tMbr = parent::mMemoryPool->GetMem(parent::mHeader->GetNodeItemSize());  // we store two tuples as one MBR

	// set the MBR
	TMbr::SetLoTuple(tMbr, item, GetSpaceDescriptor());
	TMbr::SetHiTuple(tMbr, item, GetSpaceDescriptor());

	currentInnerNode->AddItem(tMbr, TNode::GetLeafNodeIndex(newNodeIndex), true);

	newNodeIndex = currentInnerNode->GetLink(0);

	if (parent::mDebug)
	{
		currentInnerNode->Print(GetSpaceDescriptor());
	}

	parent::mMemoryPool->FreeMem(tMbr);

	if (header->IsSignatureEnabled() && ((header->GetSignatureController()->GetBuildType() == cSignatureController::SignatureBuild_Insert)))
	{
		mSignatureIndex->ModifyNodeSignature(currentInnerNode->GetIndex(), item, 1, &insertBuffers->signatureBuffers);
	}

	return newNodeIndex;
}

/**
* Find closest Mbr in the inner node
* \param itemOrder - the item order of the mbr found is returned
* \return
*		- true if the MBR (mItems) in this node were modified,
*		- false otherwise.
*/
template<class TMbr, class TKey, class TNode, class TLeafNode>
bool cCommonRTree<TMbr,TKey,TNode,TLeafNode>::
FindMbr(const TKey &item, tNodeIndex &newNodeIndex, TNode *currentInnerNode, unsigned int currentLevel, unsigned int* itemOrderCurrPath,
unsigned int& itemOrder, cInsertBuffers<TKey>* insertBuffers)
{
	cRTreeHeader<TKey>* header = GetRTreeHeader();
	bool ret;
	bool debug = false;
	parent::mDebug = false;

	if (((cRTreeHeader<TKey>*)parent::mHeader)->GetOrderingEnabled())
	{
		ret = currentInnerNode->FindMbr_Ordered(item, itemOrder, mOrderIndex);
	} 
	else 
	{
		ret = currentInnerNode->FindMbr(item, itemOrder);
	}

	newNodeIndex = currentInnerNode->GetLink(itemOrder);
	itemOrderCurrPath[currentLevel] = itemOrder;

	if (header->IsSignatureEnabled() && ((header->GetSignatureController()->GetBuildType() == cSignatureController::SignatureBuild_Insert)))
	{
		uint invLevel = header->GetHeight() - currentLevel;
		mSignatureIndex->ModifyNodeSignature(currentInnerNode->GetIndex(), item, invLevel, &insertBuffers->signatureBuffers);
	}

	return ret;
}

/**
* Find closest Mbr in the inner node
* \return
*		- true if the MBR (mItems) in this node were modified,
*		- false otherwise.
*/
template<class TMbr, class TKey, class TNode, class TLeafNode>
bool cCommonRTree<TMbr,TKey,TNode,TLeafNode>::FindMbr_MP(const TKey &item, TNode *currentInnerNode,
	cStack<sItemIdRecord>& curPathStack, cStack<sItemIdRecord>& mbrHitStack)
{
	/*
	bool ret = true;
	ItemIdRecord itemIdRecord;

	if (((cRTreeHeader<TKey>*)parent::mHeader)->GetOrderingEnabled())
	{
		// mk: zpoznamkoval jsem, 20110727: ret = currentInnerNode->FindMbr_Ordered(item, itemOrder);

		//if (!currentInnerNode->IsOrdered())
		//{
		//	currentInnerNode->Print();
		//	item.Print();
		//}
	} 
	else if (currentInnerNode->FindMbr_MP(item, curPathStack, mbrHitStack) == currentInnerNode->FINDMBR_NONE)
	{
		// there is no relevant MBR in another path - you must modify the MBR
		if (mbrHitStack.Empty())
		{
			// change the item order of the current node
			curPathStack.TopRef()->ItemOrder = currentInnerNode->FindModifyMbr_MP(item);
			// add the next node
			itemIdRecord.NodeIndex = currentInnerNode->GetLink(curPathStack.TopRef()->ItemOrder);
			itemIdRecord.ItemOrder = -1;
			curPathStack.Push(itemIdRecord);
		}
		else
		{
			ret = false;
		}
	}

	return ret;*/
}

/**
 * This method synchronizes pathStack and mbrHitStack: pathStack is pop until it includes
 * only parents of the top of mbrHitStack.
 */
template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr,TKey,TNode,TLeafNode>::FixPathStack(cStack<sItemIdRecord>& curPathStack, cStack<sItemIdRecord>& mbrHitStack)
{
	while (curPathStack.Count() != mbrHitStack.TopRef()->Level)
	{
		curPathStack.Pop();        // remove the node
	}
	curPathStack.TopRef()->ItemOrder = mbrHitStack.TopRef()->ParentItemOrder;
	curPathStack.Push(mbrHitStack.Top()); // add another hit
	mbrHitStack.Pop();           // remove this hit
}

/*
 * Split the inner noder.
 * \param tMbr_insert, insertNodeIndex - inserted item of inner node
 * \return tMbr_insert, tMbr_update - MBRs of both node.
 */
template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr,TKey,TNode,TLeafNode>::SplitInnerNode(TNode *currentInnerNode, uint currentLevel, tNodeIndex& insertNodeIndex, cInsertBuffers<TKey>* insertBuffers)
{
	TNode *newNode = parent::ReadNewInnerNode();

	if (parent::mDebug)
	{
		printf("\n\n before split:");
		currentInnerNode->Print(GetSpaceDescriptor());
	}

	currentInnerNode->Split(*newNode);

	if (parent::mDebug)
	{
		printf("\n\n splited inner nodes:");
		currentInnerNode->Print(GetSpaceDescriptor());
		newNode->Print(GetSpaceDescriptor());
	}

	//FK po splitu je potreba provest insert do spravneho nodu. nasledujici standard metoda InsertTuple se neridi usporadanim
    if (GetRTreeHeader()->GetOrderingEnabled())
	{
		TNode::InsertTupleOrder(currentInnerNode, newNode, insertBuffers->tMbr_insert, insertNodeIndex, (cRTreeNodeHeader<TMbr>*)(GetRTreeHeader()->GetNodeHeader(cTreeHeader::HEADER_NODE)));
	}
	else
	{
		TNode::InsertTuple(currentInnerNode, newNode, insertBuffers->tMbr_insert, insertNodeIndex, (cRTreeNodeHeader<TMbr>*)(GetRTreeHeader()->GetNodeHeader(cTreeHeader::HEADER_NODE)));
	}

	if (parent::mDebug)
	{
		printf("\n\n after tuple inserted:");
		currentInnerNode->Print(GetSpaceDescriptor());
		newNode->Print(GetSpaceDescriptor());
	}

	currentInnerNode->CreateMbr(insertBuffers->tMbr_update);
	newNode->CreateMbr(insertBuffers->tMbr_insert);
	insertNodeIndex = newNode->GetIndex();

	if (parent::mDebug)
	{
		printf("\n\n");
		TMbr::Print(insertBuffers->tMbr_update, "\n", GetSpaceDescriptor());
		TMbr::Print(insertBuffers->tMbr_insert, "\n", GetSpaceDescriptor());
	}

	ExtraSplitInnerNode(currentLevel, currentInnerNode, newNode);

	parent::mSharedCache->UnlockW(currentInnerNode);
	parent::mSharedCache->UnlockW(newNode);
}

/**
*	Insert new root node into the tree
*/
template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr,TKey,TNode,TLeafNode>::InsertNewRootNode(const tNodeIndex& nIndex1, const tNodeIndex& nIndex2, cInsertBuffers<TKey>* insertBuffers)
{
	//ExtraInsertNewRootNode(oldRootNode);
	TNode *newRootNode = parent::ReadNewInnerNode();

	if (parent::mDebug)
	{
		TMbr::Print(insertBuffers->tMbr_update, "\n", GetSpaceDescriptor());
		TMbr::Print(insertBuffers->tMbr_insert, "\n", GetSpaceDescriptor());
	}

	newRootNode->CreateNewRootNode(parent::mHeader, insertBuffers->tMbr_update, nIndex1, insertBuffers->tMbr_insert, nIndex2);

	ExtraInsertNewRootNode(newRootNode);
	ExtraInsertNewRootNode2(newRootNode);

	parent::mSharedCache->UnlockW(newRootNode);

	if (parent::mDebug)
	{
		TMbr::Print(insertBuffers->tMbr_update, "\n", GetSpaceDescriptor());
		TMbr::Print(insertBuffers->tMbr_insert, "\n", GetSpaceDescriptor());

		printf("\n\n");
		newRootNode->Print(GetSpaceDescriptor());
		mOrderIndex->PrintFT((newRootNode->GetIndex() & 0x7fffffff), GetSpaceDescriptor());
	}
}

/**
 * Insert item into R-tree.
 **/
template<class TMbr, class TKey, class TNode, class TLeafNode>
int cCommonRTree<TMbr,TKey,TNode,TLeafNode>::Insert(const TKey &item, char* leafData)
{
	mReadWriteMutex.lock();

	if (parent::mReadOnly)
	{
		printf("Critical Error: cCommonRTree::Insert(), The tree is read only!\n");
		exit(1);
	}

	parent::Insert(item);

	cRTreeHeader<TKey>* header = GetRTreeHeader();
	uint currentLevel = 0;  // counter of acrossing pages
	tNodeIndex nodeIndex = parent::mHeader->GetRootIndex(), insertNodeIndex = 0;
	int ret = cRTreeConst::INSERT_NO;
	int state;
	SetINSERT_TRAVERSE_DOWN(state);                        // 0 ... go down, 1 ... go up
	TLeafNode* currentLeafNode = NULL;
	TNode *currentInnerNode = NULL;
	unsigned int *currPath, *itemOrderCurrPath;
	cInsertBuffers<TKey> insertBuffers;

	Insert_pre(&currPath, &itemOrderCurrPath, &insertBuffers);

	if (parent::mDebug)
	{
		item.Print("\n", GetSpaceDescriptor());
	}

	if (header->GetItemCount() == 0)
	{
		TKey::Copy(header->GetTreeMBR()->GetLoTuple()->GetData(), item.GetData(), GetSpaceDescriptor());
		TKey::Copy(header->GetTreeMBR()->GetHiTuple()->GetData(), item.GetData(), GetSpaceDescriptor());
	}
	else
	{
		header->GetTreeMBR()->ModifyMbr(item, GetSpaceDescriptor());
	}

	for (;;)
	{
		if (GetINSERT_TRAVERSE_DOWN(state))          // go down
		{
			currPath[currentLevel++] = nodeIndex;

			if (TNode::IsLeaf(nodeIndex))
			{
				currentLeafNode = parent::ReadLeafNodeW(nodeIndex);

				/*  in this algorithm, it is not possible to detect duplicated tuples correctly
				It is necessary to check all matched leaf node.
				if (currentLeafNode->CheckDuplicity(item, buffer) == cRTreeConst::INSERT_DUPLICATE)
				{
					ret = cRTreeConst::INSERT_DUPLICATE; // it means, break the main loop
					break;
				}
				*/

				ret = TNode::INSERT_NOSPACE;
				if (currentLeafNode->HasLeafFreeSpace(item, NULL))
				{
					ret = InsertIntoLeafNode(currentLeafNode, item, leafData, &insertBuffers);
				}

				// TODO Zjednotit konstanty cRTreeConst a cTreeNode !!!
				if (ret != TNode::INSERT_NOSPACE)
				{
					ret = cRTreeConst::INSERT_YES;
					state = ExtraInsertSetFlags(currentLeafNode, currentLevel, item, state, ret);
					parent::mSharedCache->UnlockW(currentLeafNode);
					break;
				}
				else
				{
					SplitLeafNode(currentLeafNode, item, insertNodeIndex, leafData, &insertBuffers);
					SetINSERT_TRAVERSE_UP(state);
					ret = cRTreeConst::INSERT_YES;
					currentLevel--;
				}
			}
			else
			{
				// for inner node
				currentInnerNode = parent::ReadInnerNodeW(nodeIndex);

				if (parent::mDebug)
				{
					currentInnerNode->Print(GetSpaceDescriptor());
					if (currentInnerNode->GetItemCount()>0)
					{
						mOrderIndex->PrintFT(currentInnerNode->GetIndex(), GetSpaceDescriptor());
					}
				}

				if (currentInnerNode->GetItemCount() == 0)
				{
					// insert simple region - "point region"
					nodeIndex = InsertFirstItemIntoRtree(item, currentInnerNode, &insertBuffers);

					if (header->GetOrderingEnabled())
					{
						FTInsertorUpdate(currentInnerNode->GetIndex(), item);
						FTInsertorUpdate((nodeIndex & 0x7fffffff) , item);
					}
				}
				else
				{
					unsigned int itemOrder;
					if (!FindMbr(item, nodeIndex, currentInnerNode, currentLevel - 1, itemOrderCurrPath, itemOrder, &insertBuffers))
					{
						printf("Critical Error: FindClosestMbrInInnerNode = false!");
					}

					TMbr::Copy(insertBuffers.tMbr_mbr, currentInnerNode->GetCKey(itemOrder), GetSpaceDescriptor());

					if (header->GetOrderingEnabled() && itemOrder == 0)
					{
						if (item.Equal(*(mOrderIndex->GetTuple(currentInnerNode->GetIndex(), GetSpaceDescriptor())), GetSpaceDescriptor()) < 0)
						{
							FTInsertorUpdate(currentInnerNode->GetIndex(), item);
							//fk zbytecne ? FTInsertorUpdate((nodeIndex & 0x7fffffff) , item);
						}
					}
				}
				parent::mSharedCache->UnlockW(currentInnerNode);
			}
		}
		else if (GetINSERT_TRAVERSE_UP(state))				// go up - work with region nodes
		{
			nodeIndex = currPath[--currentLevel];
			currentInnerNode = parent::ReadInnerNodeW(nodeIndex);

			currentInnerNode->UpdateMbr(itemOrderCurrPath[currentLevel], insertBuffers.tMbr_update);

			if (currentInnerNode->HasFreeSpace(insertBuffers.tMbr_insert))
			{
				currentInnerNode->InsertItem(itemOrderCurrPath[currentLevel]+1, insertBuffers.tMbr_insert, insertNodeIndex);
				parent::mSharedCache->UnlockW(currentInnerNode);
				break;  // fk jen nastavit state
			}
			else
			{
				SplitInnerNode(currentInnerNode, currentLevel, insertNodeIndex, &insertBuffers);
				if (nodeIndex == parent::mHeader->GetRootIndex())   // it is necessary create new root node?
				{
					InsertNewRootNode(nodeIndex, insertNodeIndex, &insertBuffers);
					ret = cRTreeConst::INSERT_YES;
					RebuildNodeSignatures(nodeIndex, insertNodeIndex, header->GetHeight() - (currentLevel + 1), true, &insertBuffers); // it must be + 1, because the height of tree increased
					break;
				}
				//fk update FT?
				RebuildNodeSignatures(nodeIndex, insertNodeIndex, header->GetHeight() - currentLevel, false, &insertBuffers);
				SetINSERT_TRAVERSE_UP(state);
			}
		}
	}

	// free temporary memory
	Insert_post(&insertBuffers);

	mReadWriteMutex.unlock();

	return ret;
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr,TKey,TNode,TLeafNode>::Insert_pre(unsigned int** currPath, unsigned int** itemOrderCurrPath, cInsertBuffers<TKey>* insertBuffers)
{
	cRTreeHeader<TKey>* header = GetRTreeHeader();

	unsigned int size = 0;
	unsigned int size_tMbr_mbr = parent::mHeader->GetNodeItemSize();
	size += size_tMbr_mbr;
	unsigned int size_tMbr_insert = size_tMbr_mbr;
	size += size_tMbr_insert;
	unsigned int size_tMbr_update = size_tMbr_mbr;
	size += size_tMbr_update;
	unsigned int size_currPath = (parent::mHeader->GetHeight() + 2) * sizeof(unsigned int); // the number of nodes + 1 - the height may be increased
	size += size_currPath;
	unsigned int size_itemOrderCP = size_currPath;
	size += size_itemOrderCP;
	unsigned int size_itemBuffer = 0;

	if (parent::mHeader->GetDStructMode() == cDStructConst::DSMODE_CODING || 
		parent::mHeader->GetDStructMode() == cDStructConst::DSMODE_RI ||
		parent::mHeader->GetDStructMode() == cDStructConst::DSMODE_RICODING) 
	{
		size += 2 * parent::mHeader->GetTmpBufferSize();

		/*unsigned int dataSize = parent::mHeader->GetLeafDataSize(); 
		size_itemBuffer = ((parent::mHeader->GetKeySize()*2) + dataSize) * 2;
		size += 2*size_itemBuffer;*/
	}

	if (header->IsSignatureEnabled() && ((header->GetSignatureController()->GetBuildType() == cSignatureController::SignatureBuild_Insert)))
	{
		size += mSignatureIndex->Insert_presize();
	}

	if (header->GetOrderingEnabled())
	{
		size += header->GetKeySize();
	}

	// get the memory from the mem pool
	cMemoryBlock* bufferMemBlock = parent::mQuickDB->GetMemoryManager()->GetMem(size);
	insertBuffers->bufferMemBlock = bufferMemBlock;
	char* mem = bufferMemBlock->GetMem();

	insertBuffers->tMbr_mbr = mem;
	mem += size_tMbr_mbr;
	insertBuffers->tMbr_insert = mem;
	mem += size_tMbr_insert;
	insertBuffers->tMbr_update = mem;
	mem += size_tMbr_update;
	*currPath = (unsigned int*)mem;
	mem += size_currPath;
	*itemOrderCurrPath = (unsigned int*)mem;
	mem += size_itemOrderCP;
	
	/*
	if (parent::mHeader->GetDStructMode() == cDStructConst::DSMODE_CODING) 
	{
		insertBuffers->nodeBuffer.itemBuffer.codingBuffer = buffer;
		buffer += size_itemBuffer;
		insertBuffers->nodeBuffer.itemBuffer2.codingBuffer = buffer;
		buffer += size_itemBuffer;
	}*/

	if (parent::mHeader->GetDStructMode() == cDStructConst::DSMODE_RI)
	{
		insertBuffers->nodeBuffer.itemBuffer.riBuffer = mem;
	}
	else if (parent::mHeader->GetDStructMode() == cDStructConst::DSMODE_CODING)
	{
		insertBuffers->nodeBuffer.itemBuffer.codingBuffer = mem;
	}
	else  if (parent::mHeader->GetDStructMode() == cDStructConst::DSMODE_RICODING)
	{
		insertBuffers->nodeBuffer.itemBuffer.riBuffer = mem;
		insertBuffers->nodeBuffer.itemBuffer.codingBuffer = mem + (parent::mHeader->GetTmpBufferSize() / 2);
	}

	if (parent::mHeader->GetDStructMode() != cDStructConst::DSMODE_DEFAULT)
	{
		mem += parent::mHeader->GetTmpBufferSize();
	}

	if (parent::mHeader->GetDStructMode() == cDStructConst::DSMODE_RI)
	{
		insertBuffers->nodeBuffer.itemBuffer2.riBuffer = mem;
	}
	else if (parent::mHeader->GetDStructMode() == cDStructConst::DSMODE_CODING)
	{
		insertBuffers->nodeBuffer.itemBuffer2.codingBuffer = mem;
	}
	else  if (parent::mHeader->GetDStructMode() == cDStructConst::DSMODE_RICODING)
	{
		insertBuffers->nodeBuffer.itemBuffer2.riBuffer = mem;
		insertBuffers->nodeBuffer.itemBuffer2.codingBuffer = mem + (parent::mHeader->GetTmpBufferSize() / 2);
	}

	if (parent::mHeader->GetDStructMode() != cDStructConst::DSMODE_DEFAULT)
	{
		mem += parent::mHeader->GetTmpBufferSize();
	}

	if (header->GetOrderingEnabled())
	{
		insertBuffers->tmpKeyORTree.SetData(mem);
		mem += header->GetKeySize();
	}

	if (header->IsSignatureEnabled() && ((header->GetSignatureController()->GetBuildType() == cSignatureController::SignatureBuild_Insert)))
	{
		mSignatureIndex->Insert_pre(mem, &insertBuffers->signatureBuffers);
	}


}

template<class TMbr, class TKey, class TNode, class TLeafNode>
void cCommonRTree<TMbr, TKey, TNode, TLeafNode>::Insert_post(cInsertBuffers<TKey>* insertBuffers)
{
	if (GetRTreeHeader()->GetOrderingEnabled())
	{
		insertBuffers->tmpKeyORTree.SetData(NULL);
	}
	parent::mQuickDB->GetMemoryManager()->ReleaseMem(insertBuffers->bufferMemBlock);
}

/**
 * Insert item into R-tree.
 **/
template<class TMbr, class TKey, class TNode, class TLeafNode>
int cCommonRTree<TMbr,TKey,TNode,TLeafNode>::Insert_MP(const TKey &item, char* leafData)
{
	/*
	CheckReadOnly();

	tNodeIndex nodeIndex = parent::mHeader->GetRootIndex(), insertNodeIndex = 0;
	const unsigned int PathStackByteSize = 500, HitStackByteSize = 3000;
	int ret = cRTreeConst::INSERT_YES;
	int state;
	SetINSERT_TRAVERSE_DOWN(state);                        // 0 ... go down, 1 ... go up
	TLeafNode* currentLeafNode = NULL;
	TNode *currentInnerNode = NULL;
	bool leafInserted = false;
	parent::mDebug = false;

	// create variables in the pool
	char* tMbr_insert = parent::mMemoryPool->GetMem(parent::mHeader->GetNodeItemSize());
	char* tMbr_update = parent::mMemoryPool->GetMem(parent::mHeader->GetNodeItemSize());
	char* mhs = parent::mMemoryPool->GetMem(HitStackByteSize);  // memory for stacks
	char* sps = parent::mMemoryPool->GetMem(PathStackByteSize);

	ItemIdRecord itemIdRec = {parent::mHeader->GetRootIndex(), -1};
	cStack<ItemIdRecord> mbrHitStack(mhs, HitStackByteSize), curPathStack(sps, PathStackByteSize);
	curPathStack.Push(itemIdRec);

	unsigned int height = GetHeader()->GetHeight();

	while(!leafInserted)
	{		
		SetINSERT_TRAVERSE_DOWN(state);

		for ( ; ; )
		{
			// ------------------- INSERT_TRAVERSE_DOWN -------------------
			if (GetINSERT_TRAVERSE_DOWN(state))          // go down
			{
				if (TNode::IsLeaf(curPathStack.TopRef()->NodeIndex))
				{
					currentLeafNode = parent::ReadLeafNodeW(curPathStack.TopRef()->NodeIndex);

					if (GetHeader()->GetHeight() != curPathStack.Count() - 1)
					{
						printf("Critical Error: Insert_MP: GetHeader()->GetHeight() != curPathStack.Count() - 1\n");
					}

					//if ((leafInserted = currentLeafNode->HasFreeSpace(item, NULL)))
					//{
					//	if ((leafInserted = InsertIntoLeafNode(currentLeafNode, item)))
					//	{
					//		break;
					//	}
					//}

					if (currentLeafNode->HasFreeSpace(item, NULL))
					{
						InsertIntoLeafNode(currentLeafNode, item, leafData);
						leafInserted = true;
						break;
					}
					else
					{
						SplitLeafNode(currentLeafNode, item, tMbr_update, tMbr_insert, insertNodeIndex, leafData);

						if (TMbr::IsIntersected(tMbr_update, tMbr_insert, GetSpaceDescriptor()))
						{
							item.Print("\n", GetSpaceDescriptor());
							TMbr::Print(tMbr_update, "\n", GetSpaceDescriptor());
							TMbr::Print(tMbr_insert, "\n", GetSpaceDescriptor());
						}

						SetINSERT_TRAVERSE_UP(state);
						leafInserted = true;
						curPathStack.Pop();
					}
				}
				else  // --------- for inner node ---------
				{
					currentInnerNode = parent::ReadInnerNodeW(curPathStack.TopRef()->NodeIndex);

					if (currentInnerNode->GetItemCount() == 0)
					{
						// insert simple region - "point region"
						itemIdRec.NodeIndex = InsertFirstItemIntoRtree(item, currentInnerNode);
						curPathStack.Push(itemIdRec);
					}
					else if (!FindClosestMbrInInnerNode_MP(item, currentInnerNode, curPathStack, mbrHitStack))
					{
						FixPathStack(curPathStack, mbrHitStack);
					}
					parent::mSharedCache->UnlockW(currentInnerNode);
				}
			} 
			// ------------------- END INSERT_TRAVERSE_DOWN -------------------
			// ------------------- INSERT_TRAVERSE_UP -------------------
			else if (GetINSERT_TRAVERSE_UP(state))				// go up - work with region nodes
			{
				currentInnerNode = parent::ReadInnerNodeW(curPathStack.TopRef()->NodeIndex);
				currentInnerNode->UpdateMbr(curPathStack.TopRef()->ItemOrder, tMbr_update);

				if (currentInnerNode->HasFreeSpace(tMbr_insert))
				{
					InsertIntoInnerNode(currentInnerNode, tMbr_insert, insertNodeIndex);
					break;
				}
				else
				{
					SplitInnerNode(currentInnerNode, tMbr_insert, insertNodeIndex, tMbr_update);

					if (curPathStack.TopRef()->NodeIndex == parent::mHeader->GetRootIndex())   // it is necessary create new root node?
					{
						InsertNewRootNode(tMbr_update, curPathStack.TopRef()->NodeIndex, tMbr_insert, insertNodeIndex);
						ret = cRTreeConst::INSERT_YES;
						break;
					}
					SetINSERT_TRAVERSE_UP(state);
				}
				curPathStack.Pop();
			}
			// ------------------- END INSERT_TRAVERSE_UP -------------------
		} 
	}

	// free temporary memory
	parent::mMemoryPool->FreeMem(tMbr_insert);
	parent::mMemoryPool->FreeMem(tMbr_update);
	parent::mMemoryPool->FreeMem(mhs);
	parent::mMemoryPool->FreeMem(sps);

	return ret;*/
}

/**
 * Insert item into R-tree.
 **/
/*
template<class TMbr, class TKey, class TNode, class TLeafNode>
int cCommonRTree<TMbr,TKey,TNode,TLeafNode>::Insert_MP(const TKey &item)
{
	FindCandidatePaths();
	// ChoosePath();
	// Insert()
}*/