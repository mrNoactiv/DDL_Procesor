// Returns the number of subnodes in the node
template<class TKey> 
inline ushort cTreeNode<TKey>::GetSubNodesCount() const
{
	return *(ushort *)(mData + GetNodeHeader()->GetSubNodesCountOffset());
}

// Sets the number of subnodes in the node
template<class TKey> 
inline void cTreeNode<TKey>::SetSubNodesCount(ushort count)
{
	*(ushort *)(mData + GetNodeHeader()->GetSubNodesCountOffset()) = count;
}

// Increments the number of subnodes in the node
template<class TKey>
inline void cTreeNode<TKey>::IncSubNodesCount()
{
	(*(ushort *) (mData + GetNodeHeader()->GetSubNodesCountOffset()))++;
}

// Returns the capacity of subnodes in the node
template<class TKey>
inline ushort cTreeNode<TKey>::GetSubNodesCapacity() const
{
	return *(ushort *)(mData + GetNodeHeader()->GetSubNodesCapacityOffset());
}

// Sets the capacity of subnodes in the node
template<class TKey>
inline void cTreeNode<TKey>::SetSubNodesCapacity(ushort capacity)
{
	*(ushort *)(mData + GetNodeHeader()->GetSubNodesCapacityOffset()) = capacity;
}

// Increments the capacity of subnodes in the node
template<class TKey>
inline void cTreeNode<TKey>::IncSubNodesCapacity(ushort value)
{
	(*(ushort *)(mData + GetNodeHeader()->GetSubNodesCapacityOffset())) += value;
}

// Returns the number of node updates
template<class TKey>
inline unsigned char cTreeNode<TKey>::GetUpdatesCount() const
{
	return *(uchar *) (mData + GetNodeHeader()->GetUpdatesCountOffset());
}

// Sets the number of node updates
template<class TKey>
inline void cTreeNode<TKey>::SetUpdatesCount(uchar count)
{
	*(uchar *) (mData + GetNodeHeader()->GetUpdatesCountOffset()) = count;
}

// Increments the number of node updates
template<class TKey>
inline void cTreeNode<TKey>::IncUpdatesCount()
{
	(*(uchar *) (mData + GetNodeHeader()->GetUpdatesCountOffset()))++;
}

// Sets the offset of subnode headers 
// It's not possible to store the information in header, since the offset is not fixed
template<class TKey>
inline void cTreeNode<TKey>::SetSubNodeHeadersOffset(ushort offset)
{
	*(ushort *)(mData + GetNodeHeader()->GetSubNodesHeadersOffset()) = offset;
}

// Returns the offset of subnode headers 
// It's not possible to store the information in header, since the offset is not fixed
template<class TKey>
inline ushort cTreeNode<TKey>::GetSubNodeHeadersOffset() const
{
	return *(ushort *)(mData + GetNodeHeader()->GetSubNodesHeadersOffset());
}

// Returns the logical order of subnode of item with specified logical order
template<class TKey>
inline ushort cTreeNode<TKey>::GetSubNodeLOrder(ushort itemOrder) const
{
	return TSubNode::GetSubNodeOrder(this->GetSubNodeHeaders(), itemOrder, this->GetSubNodesCount());
}

template<class TKey>
inline char* cTreeNode<TKey>::GetSubNodeHeaders() const
{
	return this->GetItems() + this->GetSubNodeHeadersOffset();
}

template<class TKey>
inline char* cTreeNode<TKey>::GetSubNodeHeaderByItem(ushort itemOrder) const
{
	return TSubNode::GetSubNodeHeader(this->GetSubNodeHeaders(), itemOrder, GetSubNodesCount());
}

// Returns the subnode of specified order
template<class TKey>
inline char* cTreeNode<TKey>::GetSubNode(ushort subNodeOrder) const
{
	return TSubNode::GetSubNode(this->GetItems(), this->GetSubNodeHeaders(), subNodeOrder);
}

// Returns the subnode of specified order
template<class TKey>
inline char* cTreeNode<TKey>::GetSubNodeHeader(ushort subNodeOrder) const
{
	return TSubNode::GetSubNodeHeader(this->GetSubNodeHeaders(), subNodeOrder);
}

template<class TKey>
inline ushort cTreeNode<TKey>::FreeSize(ushort subNodeOrder) const
{
	return TSubNode::FreeSize(this->GetSubNodeHeaders(), this->GetSubNodeHeadersOffset(), subNodeOrder, this->GetSubNodesCount());
}

template<class TKey>
inline ushort cTreeNode<TKey>::CompleteSize(ushort subNodeOrder) const
{
	return TSubNode::CompleteSize(this->GetSubNodeHeaders(), this->GetSubNodeHeadersOffset(), subNodeOrder, this->GetSubNodesCount());
}

// Increments item order intervals of particular subnodes
template<class TKey>
inline void cTreeNode<TKey>::UpdateItemOrderIntervals(ushort subNodeLOrder, short shift)
{
	TSubNode::UpdateItemOrderIntervals(this->GetSubNodeHeaders(), subNodeLOrder, shift, this->GetSubNodesCount());
}

// Shift specified subnode and update all shifted item orders
template<class TKey>
inline void cTreeNode<TKey>::Shift(ushort subNodeLOrder, int shift, uint startByte, uint startItem)
{
	TSubNode::Shift(this->GetItems(), shift, startByte, (subNodeLOrder < this->GetSubNodesCount() - 1) ? TSubNode::GetSubNodePOrder(this->GetSubNodeHeader(subNodeLOrder + 1)) : this->GetSubNodeHeadersOffset());
	UpdatePOrders(this->GetSubNodeHeader(subNodeLOrder), shift, false, startItem);
}

// Creates the new subnode with the specific logical order and reorganize its neighbours
template<class TKey>
char* cTreeNode<TKey>::CreateSubNode(const char* key, ushort lOrder, ushort lSubNodeOrder)
{
	char* subNodeHeader = NULL;
	if (GetSubNodesCount() == GetSubNodesCapacity())
	{
		ushort subNodesCount = 0.01 * (mHeader->GetNodeCapacity() - mItemCount) + 1;
		ushort newSnOffset = GetNodeHeader()->GetItemOrderOffset() - GetNodeHeader()->GetItemsOffset() - ((this->GetSubNodesCount() + subNodesCount) * TSubNode::HEADER_SIZE);
		if (subNodesCount * TSubNode::HEADER_SIZE > mFreeSize)
		{
			return NULL;
		}

		if (mItemCount > 0)
		{
			short overlap = TSubNode::GetLastByte(this->GetSubNodeHeader(this->GetSubNodesCount() - 1)) - newSnOffset;
			if (overlap > 0)
			{
				//Replacement(this->GetSubNodesCount() - 1, overlap);
				SubNodeReplace(this->GetSubNodesCount() - 1, overlap);
			}
			memmove(this->GetItems() + newSnOffset, this->GetItems() + this->GetSubNodeHeadersOffset(), this->GetSubNodesCount() * TSubNode::HEADER_SIZE);
		}
		this->IncSubNodesCapacity(subNodesCount);
		this->SetSubNodeHeadersOffset(newSnOffset);
		mFreeSize -= subNodesCount * TSubNode::HEADER_SIZE;
	}


	if (mItemCount == 0) // create first subnode
	{
		subNodeHeader = CreateSubNode(0, 0, 0, key);
	}
	else
	{
		ushort freeSize = this->FreeSize(GetSubNodesCount() - 1);
		ushort minSubNodeSize = GetSubNodeHeaderSize(key) + 2 * TKey::GetSize(key, mHeader->GetKeyDescriptor());
		ushort optSubNodeSize = (freeSize > 2 * minSubNodeSize) ? freeSize / 2 : minSubNodeSize;

		if (optSubNodeSize > freeSize)
		{
			if (Replacement(GetSubNodesCount() - 1, minSubNodeSize) == NULL)
			{
				return NULL;
			}
		}

		if ((lSubNodeOrder == GetSubNodesCount())) // create subnode at the end
		{
			ushort pSubNodeOrder = TSubNode::GetLastByte(this->GetSubNodeHeader(GetSubNodesCount() - 1)) + 1;
			subNodeHeader = CreateSubNode(pSubNodeOrder, lSubNodeOrder, lOrder, key);
		}
		else // create subnode at the beginning or in the middle
		{
			ushort subNodePOrder = TSubNode::GetSubNodePOrder(this->GetSubNodeHeader(lSubNodeOrder));
			SubNodeShift(lSubNodeOrder, optSubNodeSize, true);
    		memmove(this->GetSubNodeHeader(lSubNodeOrder + 1), this->GetSubNodeHeader(lSubNodeOrder), (GetSubNodesCount() - lSubNodeOrder)*TSubNode::HEADER_SIZE);
			subNodeHeader = CreateSubNode(subNodePOrder, lSubNodeOrder, lOrder, key);

		}
	}

	return subNodeHeader;
}

// Creates the new subnode on the specific position (pOrder) in the node and returns it
// subNodePOrder - physical order of subnode
// subNodeLOrder - physical order of subnode
// lOrder - logical order of first item
template<class TKey>
inline char* cTreeNode<TKey>::CreateSubNode(ushort subNodePOrder, ushort subNodeLOrder, ushort lOrder, const char* key)
{
	char* subNode = TSubNode::CreateSubNode(this->GetItems(), this->GetSubNodeHeaders(), subNodeLOrder, subNodePOrder, lOrder, key, mHeader->GetKeyDescriptor());

	mFreeSize -= GetSubNodeHeaderSize(key);
	IncSubNodesCount();
	return subNode;
}

// Returns the size of subnode according to first item
template<class TKey>
inline uint cTreeNode<TKey>::GetSubNodeHeaderSize(const char* key) const
{
	unsigned int subNodeSize = 	2 * TKey::GetSize(key, mHeader->GetKeyDescriptor()) // size of reference items
								+ TMask::ByteSize(TKey::GetLength(key, mHeader->GetKeyDescriptor()));             // size of mask

	return subNodeSize;
}


// Returns the size of specified subnode
template<class TKey>
inline uint cTreeNode<TKey>::GetSubNodeHeaderSize2(char* subNodeHeader) const
{
	return GetItemPOrder(TSubNode::GetFirstItemOrder(subNodeHeader)) - TSubNode::GetSubNodePOrder(subNodeHeader);
}

// Returns cut of key according to selected subnode
template<class TKey>
inline uint cTreeNode<TKey>::CutKey(char* subNodeHeader, const char* key, char* cutKey)
{
	char* mask = TSubNode::GetMask(this->GetItems(), subNodeHeader);
	char* minRI = TSubNode::GetMinRefItem(this->GetItems(), subNodeHeader);
	return TKey::CutTuple(mask, minRI, key, cutKey, mHeader->GetKeyDescriptor());

}

// Updates the header of selected subnode
template<class TKey>
void cTreeNode<TKey>::UpdateSubNodeHeader(char* subNodeHeader, uint itemSize, uint lastItemOrder)
{
	if (TSubNode::GetLastItemOrder(subNodeHeader) == TSubNode::NOT_DEFINED) // in the case of first item of subnode
		TSubNode::SetLastItemOrder(subNodeHeader, lastItemOrder);
	else
		TSubNode::IncLastItemOrder(subNodeHeader);

	TSubNode::IncLastByte(subNodeHeader, itemSize);
}

// Increments physical orders of specified items by value "shift"
template<class TKey>
void cTreeNode<TKey>::UpdatePOrders(char* subNodeHeader, short shift, bool allNextSubNodes, ushort startItem)
{
	ushort firstItem = ((short)startItem == -1) ? TSubNode::GetFirstItemOrder(subNodeHeader) : startItem;
	ushort lastItem = (allNextSubNodes) ? mItemCount : TSubNode::GetLastItemOrder(subNodeHeader) + 1;

	for (ushort i = firstItem; i < lastItem; i++)
	{
		IncItemPOrder(i, shift);
	}
}

// Increments physical orders of specified subnodes by value "shift"
template<class TKey>
void cTreeNode<TKey>::UpdateSubNodesPOrders(ushort lSubNodeOrder, short shift, bool allNextSubNodes)
{
	ushort endSubNodeOrder = (allNextSubNodes) ? GetSubNodesCount() - 1 : lSubNodeOrder;

	for (ushort i = lSubNodeOrder; i <= endSubNodeOrder; i++)
	{
		char* subNodeHeader = this->GetSubNodeHeader(i);
		TSubNode::IncSubNodePOrder(subNodeHeader, shift);
		TSubNode::IncLastByte(subNodeHeader, shift);
	}
}


// Shifts the subnode(s) by value "shift"
template<class TKey>
char* cTreeNode<TKey>::SubNodeShift(ushort lSubNodeOrder, short shift, bool allNextSubNodes)
{
	// physical move of subnode(s)
	char* subNodeHeader = this->GetSubNodeHeader(lSubNodeOrder);
	char* subNode = this->GetSubNode(lSubNodeOrder);
	ushort subNodePOrder = TSubNode::GetSubNodePOrder(subNodeHeader);
	if (allNextSubNodes)
	{
		memmove(subNode + shift, subNode, this->GetSubNodeHeadersOffset() - subNodePOrder - ((shift > 0) ? shift : 0));
		subNode = subNode + shift;
	}
	else
	{
		subNode = TSubNode::ShiftSubNode(subNode, shift, this->CompleteSize(lSubNodeOrder));
	}

	// updates physical orders of subnode(s)
	UpdateSubNodesPOrders(lSubNodeOrder, shift, allNextSubNodes);

	// update of item orders
	subNodeHeader = this->GetSubNodeHeader(lSubNodeOrder);
	UpdatePOrders(subNodeHeader, shift, allNextSubNodes);

	return subNode;
}

// Invokes the replacement of subnodes to get free space in specified subnode
// Returns the subnode (WARNING - during replacing the physical order of subnode can be changed)
template<class TKey>
char* cTreeNode<TKey>::Replacement(ushort lSubNodeOrder, short size)
{
	if (mFreeSize >= size)
	{
		int realSize = size - this->FreeSize(lSubNodeOrder);
		SubNodeReplace(lSubNodeOrder, realSize);
		return GetSubNode(lSubNodeOrder); // it is neccessary, because subNode could be shifted
	}
	else
	{
		return NULL;
	}
}

// recursive replacement of neighbours and looking for free space
// return true, if free space has been prepared
template<class TKey>
void cTreeNode<TKey>::SubNodeReplace(ushort lSubNodeOrder, short shift)
{
	ushort freeSize = 0, subNodeShift = 0;
	char* subNodeHeader = NULL;

	// right neightbours
	for (short i = lSubNodeOrder + 1; i < GetSubNodesCount(); i++)
	{
		if ((freeSize = this->FreeSize(i)) > 0)
		{
			subNodeShift = (freeSize > shift) ? shift : freeSize;

			// shift right subnode and its left neighbours
			char* aSubNode = SubNodeShift(i, subNodeShift, false);
			for (ushort j = 0; j < (i - (lSubNodeOrder + 1)); j++)
			{
				aSubNode = SubNodeShift(i - j - 1, subNodeShift, false);
			}

			if ((shift -= subNodeShift) == 0)
				return;
		}
	}

	// left neightbours
	for (short i = lSubNodeOrder - 1; i >= 0; i--)
	{
		if ((freeSize = this->FreeSize(i)) > 0)
		{
			subNodeShift = (freeSize > shift) ? shift : freeSize;

			// shift left subnode and its right neighbours
			char* aSubNode = SubNodeShift(i + 1, -subNodeShift, false);
			for (ushort j = i + 2; j <= lSubNodeOrder; j++)
			{
				aSubNode = SubNodeShift(j, -subNodeShift, false);
			}

			if ((shift -= subNodeShift) == 0)
				return;
		}
	}

	assert(false); // it never should happen
}

/**
* Insert key into specified subnode
* Return:
*   - INSERT_YES
*	- INSERT_AT_THE_END
*   - INSERT_NOSPACE
**/
template<class TKey>
uint cTreeNode<TKey>::InsertToSubNode(ushort lSubNodeOrder, const char* key, ushort lOrder, char* data, sItemBuffers* buffers)
{
	cTreeNodeHeader *nodeHeader = GetNodeHeader();
	// cut the key according to selected subnode
	char* cutKey = buffers->riBuffer;
	char* subNodeHeader = this->GetSubNodeHeader(lSubNodeOrder);
	ushort cutKeyLength = CutKey(subNodeHeader, key, cutKey);
	ushort itemSize = GetKeySize(cutKey, cutKeyLength, buffers) + GetDataSize(data);

	// prepare the space in the subnode for cut item
	if (this->FreeSize(lSubNodeOrder) < itemSize)
	{
		if (Replacement(lSubNodeOrder, itemSize) == NULL)
		{
			return INSERT_NOSPACE;
		}
	}

	// specify the physical order of cut item
	ushort pOrder;
	if (mHeader->GetDStructCode() == cDStructConst::BTREE)
	{
		if ((TSubNode::GetLastItemOrder(subNodeHeader) == TSubNode::NOT_DEFINED) || ((TSubNode::GetLastItemOrder(subNodeHeader) + 1) == lOrder))
		{
			pOrder = TSubNode::GetLastByte(subNodeHeader);
			TSubNode::SetMaxRefItem(this->GetItems(), subNodeHeader, key, TKey::GetSize(key, mHeader->GetKeyDescriptor()));
		}
		else // it means, that we insert at the beginning or into the middle of the RIBlock
		{
			pOrder = GetItemPOrder(lOrder);
			Shift(lSubNodeOrder, itemSize, pOrder, lOrder);
		}
	}
	else
	{
		pOrder = TSubNode::GetLastByte(subNodeHeader);
		if (TSubNode::GetLastItemOrder(subNodeHeader) == TSubNode::NOT_DEFINED)
		{
			TSubNode::SetMaxRefItem(this->GetItems(), subNodeHeader, key, TKey::GetSize(key, mHeader->GetKeyDescriptor()));
		}
	}

	// TODO In the case of RICODING, two encodings are presented in the case of inserting in the middle of reference subnode
	// insert cut item into node
	itemSize = SetLeafItemPo(pOrder, cutKey, data, cutKeyLength, buffers);
	if (itemSize == INSERT_NOSPACE)
	{
		return INSERT_NOSPACE;
	}
	InsertItemPOrder(lOrder, pOrder);

	// update headers of subnode and its right neighbours
	UpdateSubNodeHeader(subNodeHeader, itemSize, lOrder); 
	UpdateItemOrderIntervals(lSubNodeOrder);

	// update header and node informations
	assert(mItemCount < mHeader->GetNodeCapacity());
	mItemCount++;
	assert(mFreeSize >= itemSize);
	mFreeSize -= itemSize;
	assert(mFreeSize <= mHeader->GetNodeItemsSpaceSize());

	return (lOrder != mItemCount - 1) ? INSERT_YES : INSERT_AT_THE_END;
}

template<class TKey>
int cTreeNode<TKey>::FindCompatibleSubNode_Rtree(const char* key, int* lOrder, char **subNodeHeader, ushort* lSubNodeOrder, sItemBuffers* buffers)
{
	int ret = SUBNODE_NOTEXIST;
	for (ushort i = 0; i < this->GetSubNodesCount(); i++)
	{
		*subNodeHeader = this->GetSubNodeHeader(i);
		char* minRefItem = TSubNode::GetMinRefItem(this->GetItems(), *subNodeHeader);
		char* maxRefItem = TSubNode::GetMaxRefItem(this->GetItems(), *subNodeHeader);

		if (cMBRectangle<TKey>::IsInRectangle(minRefItem, maxRefItem, key, (cSpaceDescriptor*)mHeader->GetKeyDescriptor()))
		{
			*lSubNodeOrder = i;
			ret = SUBNODE_EXIST;
			break;
		}
	}

	if (ret == SUBNODE_EXIST)
	{
		*lOrder = TSubNode::GetLastItemOrder(*subNodeHeader) + 1;
	}
	else
	{
		*lOrder = mItemCount;
		*lSubNodeOrder = this->GetSubNodesCount();
		*subNodeHeader = NULL;
	}

	return ret;
}

template<class TKey>
int cTreeNode<TKey>::FindCompatibleSubNode(const char* key, int* lOrder, char **subNodeHeader, ushort* lSubNodeOrder, bool allowDuplicateKey, sItemBuffers* buffers)
{
	int loSn = 0;
	int hiSn = this->GetSubNodesCount() - 1;
	do
	{
		int midSn = (loSn + hiSn) / 2;
		*subNodeHeader = this->GetSubNodeHeader(midSn);
		char* mask = TSubNode::GetMask(this->GetItems(), *subNodeHeader);
		char* minRefItem = TSubNode::GetMinRefItem(this->GetItems(), *subNodeHeader);
		char* maxRefItem = TSubNode::GetMaxRefItem(this->GetItems(), *subNodeHeader);
		const char* firstItem = GetCKey(TSubNode::GetFirstItemOrder(*subNodeHeader), buffers, midSn);
		*lSubNodeOrder = midSn;

		bool higher, lower;
		if ((higher = (TKey::Equal(key, firstItem, mHeader->GetKeyDescriptor()) > -1))
			&& (lower = (TKey::Equal(key, maxRefItem, mHeader->GetKeyDescriptor()) < 1)))
		{
			break;
		}

		if (!higher)
		{
			hiSn = midSn - 1;
		}
		else
		{
			loSn = midSn + 1;
			if (loSn > hiSn)
			{
				midSn++;
			}
		}

	} while (loSn <= hiSn);


	/*if (mItemCount == 10)
	{
		int c = 3;
	}*/
	*lOrder = FindItemOrder(key, allowDuplicateKey, buffers, *lSubNodeOrder);
	if (*lOrder == INSERT_EXIST)
	{
		return INSERT_EXIST;
	}

	if (FindCompatibleSubNode(key, *lOrder, subNodeHeader, lSubNodeOrder)) // found compatible subnode or specify new subnode
	{
		return SUBNODE_EXIST;
	}
	else
	{
		return SUBNODE_NOTEXIST;
	}
}

// Returns the compatible subnode and its logical order, if exists
template<class TKey>
bool cTreeNode<TKey>::FindCompatibleSubNode2(const char* item, ushort lOrder, char **subNodeHeader, ushort* lSubNodeOrder)
{
	*subNodeHeader = FindSuitableSubNode(lOrder, lSubNodeOrder);

	if (TKey::IsCompatible(TSubNode::GetMask(this->GetItems(), *subNodeHeader), TSubNode::GetMinRefItem(this->GetItems(), *subNodeHeader), item, mHeader->GetKeyDescriptor())) //(IsSubNodeCompatible(*subNode, item)) // found subnode is compatible with item
	{
		return true;
	}
	else if (lOrder <= TSubNode::GetFirstItemOrder(*subNodeHeader)) // no compatible subnode found, item can be insert at the beginning -> leads to create subnode
	{
		*lSubNodeOrder = 0;
		*subNodeHeader = NULL;
		return true;
	}
	else if (lOrder > TSubNode::GetLastItemOrder(*subNodeHeader)) // no compatible subnode found, item can be insert after subnode -> leads to create subnode
	{
		*subNodeHeader = this->GetSubNodeHeader(*lSubNodeOrder + 1);
		if ((*lSubNodeOrder + 1 == GetSubNodesCount()) || (!TKey::IsCompatible(TSubNode::GetMask(this->GetItems(), *subNodeHeader), TSubNode::GetMinRefItem(this->GetItems(), *subNodeHeader), item, mHeader->GetKeyDescriptor()))) //(IsSubNodeCompatible(this->GetNextSubNode(*subNode), item)) // next subnode is compatible with item
		{
			*subNodeHeader = NULL;
		}

		*lSubNodeOrder = *lSubNodeOrder + 1;
		return true;
	}
	else
	{
		return false; // no compatible subnode found -> leads to rebuild
	}
}

// Returns the compatible subnode and its logical order, if exists
template<class TKey>
bool cTreeNode<TKey>::FindCompatibleSubNode(const char* item, ushort lOrder, char **subNodeHeader, ushort* lSubNodeOrder)
{
	//*subNodeHeader = FindSuitableSubNode(lOrder, lSubNodeOrder);

	if (TKey::IsCompatible(TSubNode::GetMask(this->GetItems(), *subNodeHeader), TSubNode::GetMinRefItem(this->GetItems(), *subNodeHeader), item, mHeader->GetKeyDescriptor())) //(IsSubNodeCompatible(*subNode, item)) // found subnode is compatible with item
	{
		return true;
	}
	else if (lOrder <= TSubNode::GetFirstItemOrder(*subNodeHeader)) // no compatible subnode found, item can be insert at the beginning -> leads to create subnode
	{
		*subNodeHeader = this->GetSubNodeHeader(*lSubNodeOrder - 1);
		if ((*lSubNodeOrder > 0) && (TKey::IsCompatible(TSubNode::GetMask(this->GetItems(), *subNodeHeader), TSubNode::GetMinRefItem(this->GetItems(), *subNodeHeader), item, mHeader->GetKeyDescriptor())))
		{
			*lSubNodeOrder = *lSubNodeOrder - 1;
		}
		else
		{
			*subNodeHeader = NULL;
		}

		//*lSubNodeOrder = (*lSubNodeOrder > 0)? *lSubNodeOrder - 1 : 0;
		return true;
	}
	else if (lOrder > TSubNode::GetLastItemOrder(*subNodeHeader)) // no compatible subnode found, item can be insert after subnode -> leads to create subnode
	{
		*subNodeHeader = this->GetSubNodeHeader(*lSubNodeOrder + 1);
		if ((*lSubNodeOrder + 1 == GetSubNodesCount()) || (!TKey::IsCompatible(TSubNode::GetMask(this->GetItems(), *subNodeHeader), TSubNode::GetMinRefItem(this->GetItems(), *subNodeHeader), item, mHeader->GetKeyDescriptor()))) //(IsSubNodeCompatible(this->GetNextSubNode(*subNode), item)) // next subnode is compatible with item
		{
			*subNodeHeader = NULL;
		}

		*lSubNodeOrder = *lSubNodeOrder + 1;
		return true;
	}
	else
	{
		return false; // no compatible subnode found -> leads to rebuild
	}
}

// Returns the first suitable subnode and its logical order according to logical order of inserting item
template<class TKey>
char* cTreeNode<TKey>::FindSuitableSubNode(ushort lOrder, ushort* lSubNodeOrder)
{
	ushort subNodesCount = GetSubNodesCount();

	for (ushort i = 0; i < subNodesCount; i++)
	{
		char* subNodeHeader = this->GetSubNodeHeader(i);
		if ((TSubNode::GetFirstItemOrder(subNodeHeader) <= lOrder) && (lOrder <= TSubNode::GetLastItemOrder(subNodeHeader) + 1)) // + 1 because of case lOrder == mItemCount
		{
			*lSubNodeOrder = i;
			return subNodeHeader;
		}
	}

	return this->GetSubNodeHeader(0);
}


/**
* Split this node into two nodes by subnodes.
* \param newNode Newly created node.
* \param tmpNode Temporary node used during the split (to reorder items in this node).
**/
template<class TKey>
void cTreeNode<TKey>::SplitLeafNode_ri(cTreeNode<TKey>& newNode, cTreeNode<TKey> &tmpNode, cNodeBuffers<TKey>* buffers)
{
	ushort halfCount = (mItemCount + 1) / 2;
	ushort leftSubNodeLo = this->GetSubNodeLOrder(halfCount - 1);
	ushort rightSubNodeLo = this->GetSubNodeLOrder(halfCount);
	char* leftSubNodeHeader = this->GetSubNodeHeader(leftSubNodeLo);
	char* rightSubNodeHeader = this->GetSubNodeHeader(rightSubNodeLo);

	// avoids the split of subnode - bad influence on utilization ?!?
	/*uint THRESHOLD = 0.15 * mItemCount;
	if (leftSubNodeLo == rightSubNodeLo)
	{
		uint leftCount = (rightSubNodeLo == 0) ? cUInt::MAX : (halfCount - TSubNode::GetLastItemOrder(this->GetPreviousSubNode(rightSubNode)));
		uint rightCount = (rightSubNodeLo == GetSubNodesCount() - 1) ? cUInt::MAX : TSubNode::GetFirstItemOrder(this->GetNextSubNode(leftSubNode)) - halfCount;

		if ((leftCount < THRESHOLD) || (rightCount < THRESHOLD))
		{
			if (leftCount < rightCount)
			{
				leftSubNodeLo--;
				leftSubNode = this->GetPreviousSubNode(rightSubNode);
				halfCount = TSubNode::GetFirstItemOrder(rightSubNode);
			}
			else
			{
				rightSubNodeLo++;
				rightSubNode = this->GetNextSubNode(leftSubNode);
				halfCount = TSubNode::GetFirstItemOrder(rightSubNode);
			}
		}
	}*/

	// defines shifts of right part of the node
	ushort nodeShift = TSubNode::GetSubNodePOrder(rightSubNodeHeader);
	ushort nodeShiftedItems = TSubNode::GetFirstItemOrder(rightSubNodeHeader);
	ushort subNodeShift = GetItemPOrder(halfCount) - GetItemPOrder(nodeShiftedItems);
	ushort subNodeShiftedItems = halfCount - nodeShiftedItems;
	ushort headerSize = GetSubNodeHeaderSize2(rightSubNodeHeader);

	// updates the physical orders of the right part of the node
	UpdateSubNodesPOrders(rightSubNodeLo + 1, -(nodeShift + subNodeShift), true);

	// copy of the right part of the node at the beginning of new node
	if (leftSubNodeLo != rightSubNodeLo)
	{
		memcpy(newNode.GetItems(), GetItems() + nodeShift, GetNodeHeader()->GetItemOrderOffset() - nodeShift);
	}
	else
	{
		memcpy(newNode.GetItems(), GetItems() + nodeShift, headerSize);
		memcpy(newNode.GetItems() + headerSize, GetItems() + GetItemPOrder(halfCount), GetNodeHeader()->GetItemOrderOffset() - GetItemPOrder(halfCount));
	}
	memcpy(newNode.GetItems() + GetSubNodeHeadersOffset(), GetItems() + GetSubNodeHeadersOffset() + (rightSubNodeLo * TSubNode::HEADER_SIZE), (GetSubNodesCount() - rightSubNodeLo) * TSubNode::HEADER_SIZE);
	memcpy(newNode.GetItemOrders(), GetItemOrders() + (halfCount * cTreeNodeHeader::ItemSize_ItemOrder), (mItemCount - halfCount) * cTreeNodeHeader::ItemSize_ItemOrder);

	// update header and node informations of the new node
	newNode.SetItemCount(mItemCount - halfCount);
	newNode.SetSubNodesCount(GetSubNodesCount() - rightSubNodeLo);
	newNode.SetSubNodesCapacity(GetSubNodesCapacity());
	newNode.SetSubNodeHeadersOffset(GetSubNodeHeadersOffset());
	TSubNode::SetSubNodePOrder(newNode.GetSubNodeHeader(0), 0);
	TSubNode::IncLastByte(newNode.GetSubNodeHeader(0), -(nodeShift + subNodeShift));
	newNode.SetFreeSize(TSubNode::TotalFreeSize(newNode.GetSubNodeHeaders(), newNode.GetSubNodeHeadersOffset(), newNode.GetSubNodesCount()));

	// update physical orders of items and subnodes information
	rightSubNodeHeader = newNode.GetSubNodeHeader(0);
	TSubNode::SetFirstItemOrder(rightSubNodeHeader, 0);
	TSubNode::IncLastItemOrder(rightSubNodeHeader, -halfCount);
	newNode.UpdateItemOrderIntervals(0, -halfCount);
	newNode.UpdatePOrders(rightSubNodeHeader, -(nodeShift + subNodeShift), true);


	// modification of left part of node
	if (leftSubNodeLo == rightSubNodeLo) // in the case of subnode split
	{
		const char* key = GetCKey(halfCount - 1, &buffers->itemBuffer, leftSubNodeLo);
		TSubNode::SetMaxRefItem(this->GetItems(), leftSubNodeHeader, key, TKey::GetSize(key, mHeader->GetKeyDescriptor()));
		TSubNode::SetLastByte(leftSubNodeHeader, GetItemPOrder(halfCount));
		TSubNode::SetLastItemOrder(leftSubNodeHeader, halfCount - 1);
	}

	this->SetItemCount(halfCount);
	this->SetSubNodesCount(leftSubNodeLo + 1);
	this->SetFreeSize(TSubNode::TotalFreeSize(this->GetSubNodeHeaders(), this->GetSubNodeHeadersOffset(), this->GetSubNodesCount()));

	mHeader->IncrementNodeCount();
}


/**
* Find order of item according mode.
* \param mode The mode can be: FIND_SBE - find smalest bigger or equal item, FIND_E - find equal item).
* \return
*		- The position of the item in the node.
*		- FIND_NOTEXIST if the mode if FIND_SBE and the item is bigger then the biggest item in the node or the mode is FIND_E and the item is not found.
*/
template<class TKey>
int cTreeNode<TKey>::FindOrder_ri(const TKey& item, int mode, sItemBuffers* buffers) const
{
	ushort subNodesCount = GetSubNodesCount();
	char* queryItem = buffers->riBuffer;
	int equal;

	assert(mode == FIND_SBE || mode == FIND_E);
	int loSn = 0;
	int hiSn = subNodesCount;
	do
	{
		int midSn = (loSn + hiSn) / 2;
		char* subNodeHeader = this->GetSubNodeHeader(midSn);
		char* mask = TSubNode::GetMask(this->GetItems(), subNodeHeader);
		char* minRefItem = TSubNode::GetMinRefItem(this->GetItems(), subNodeHeader);
		char* maxRefItem = TSubNode::GetMaxRefItem(this->GetItems(), subNodeHeader);
		const char* firstItem = GetCKey(TSubNode::GetFirstItemOrder(subNodeHeader), buffers, midSn);

		bool higher, lower;
		if ((higher = (TKey::Equal(item, firstItem, mHeader->GetKeyDescriptor()) > -1))
			&& (lower = (TKey::Equal(item, maxRefItem, mHeader->GetKeyDescriptor()) < 1)))
		{
			ushort queryItemLength = TKey::CutTuple(mask, minRefItem, item, queryItem, mHeader->GetKeyDescriptor());

			int lo = TSubNode::GetFirstItemOrder(subNodeHeader);
			int hi = TSubNode::GetLastItemOrder(subNodeHeader);
			do
			{
				int mid = (lo + hi) / 2;
				if ((equal = TKey::Equal(queryItem, GetCPartKey(mid, buffers, midSn), queryItemLength, mHeader->GetKeyDescriptor())) == 0)
				{
					return mid;
				}

				if (equal == -1)
				{
					hi = mid - 1;
				}
				else
				{
					lo = mid + 1;
				}
			} while (lo <= hi);
		}

		if (!higher)
		{
			hiSn = midSn - 1;
		}
		else
		{
			loSn = midSn + 1;
		}

	} while (loSn <= hiSn);

/*	for (ushort i = 0; i < subNodesCount; i++)
	{
		char* subNodeHeader = this->GetSubNodeHeader(i);
		char* mask = TSubNode::GetMask(this->GetItems(), subNodeHeader);
		char* minRefItem = TSubNode::GetMinRefItem(this->GetItems(), subNodeHeader);
		char* maxRefItem = TSubNode::GetMaxRefItem(this->GetItems(), subNodeHeader);

		if ((TKey::Equal(item.GetData(), minRefItem, mHeader->GetKeyDescriptor()) > -1) && (TKey::Equal(item.GetData(), maxRefItem, mHeader->GetKeyDescriptor()) < 1))
		{
			ushort queryItemLength = TKey::CutTuple(mask, minRefItem, item.GetData(), queryItem, mHeader->GetKeyDescriptor());

			int lo = TSubNode::GetFirstItemOrder(subNodeHeader);
			int hi = TSubNode::GetLastItemOrder(subNodeHeader);
			do
			{
				int mid = (lo + hi) / 2;
				if ((equal = TKey::Equal(queryItem, GetCPartKey(mid, buffers, i), queryItemLength, mHeader->GetKeyDescriptor())) == 0)
				{
					return mid;
				}

				if (equal == -1)
				{
					hi = mid - 1;
				}
				else
				{
					lo = mid + 1;
					if (lo > hi)
					{
						mid++;
					}
				}
			} while (lo <= hi);
		}
	}*/

	return FIND_NOTEXIST;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Insert key into a leaf node, where the correct position is first searched by the cut interval method.
 * Return:
 *   - INSERT_YES 
 *	 - INSERT_AT_THE_END
 *   - INSERT_EXIST
 *	 - INSERT_NOSPACE
 **/
template<class TKey> int cTreeNode<TKey>::InsertLeafItem_ri(const char* key, char* data, bool allowDuplicateKey, cNodeBuffers<TKey>* buffers)
{
	char* subNodeHeader = NULL;
	int inserted = INSERT_YES;
	int lOrder = 0;
	ushort lSubNodeOrder = 0;

	if (mItemCount == 0)
	{
		CreateSubNode(key, 0, 0);
		inserted = InsertToSubNode(0, key, 0, data, &buffers->itemBuffer);
	}
	else
	{
		if ((inserted = FindCompatibleSubNode(key, &lOrder, &subNodeHeader, &lSubNodeOrder, allowDuplicateKey, &buffers->itemBuffer)) == SUBNODE_EXIST) // found compatible subnode or specify new subnode
		{
			if (subNodeHeader == NULL) // new subnode has to be created
			{
				if (CreateSubNode(key, lOrder, lSubNodeOrder) == NULL) // if no subnode has been chosen -> node is full
				{
					return INSERT_NOSPACE;
				}
			}
			inserted = InsertToSubNode(lSubNodeOrder, key, lOrder, data, &buffers->itemBuffer);
		}
		else
		{
			if (inserted == INSERT_EXIST)
			{
				return INSERT_EXIST;
			}

			if ((subNodeHeader = Rebuild(lSubNodeOrder, key, buffers)) == NULL) // no subnode was chosen -> node is full
			{
				return INSERT_NOSPACE;
			}
			inserted = InsertToSubNode(lSubNodeOrder, key, lOrder, data, &buffers->itemBuffer);
		}

		// the issue in the case of subnode shift in InsertToSubNode -> subnode is not on the same position !!!
		this->IncUpdatesCount();
		if (this->GetUpdatesCount() % 150 == 0)
		{
			this->NodeRebuild(buffers);
		}
	}

	if (mHeader->GetRuntimeMode() == cDStructConst::RTMODE_VALIDATION)
	{
		ConsistencyTest(&buffers->itemBuffer);
	}

	return inserted;
}

template<class TKey> int cTreeNode<TKey>::AddLeafItem_ri(const char* key, char* data, bool incFlag, cNodeBuffers<TKey>* buffers)
{
	char* subNodeHeader = NULL;
	int inserted = INSERT_YES;
	int lOrder = 0;
	ushort lSubNodeOrder = 0;

	if (mItemCount == 0)
	{
		CreateSubNode(key, 0, 0);
		inserted = InsertToSubNode(0, key, 0, data, &buffers->itemBuffer);
	}
	else
	{
		if ((inserted = FindCompatibleSubNode_Rtree(key, &lOrder, &subNodeHeader, &lSubNodeOrder, &buffers->itemBuffer)) != SUBNODE_EXIST) // found compatible subnode or specify new subnode
		{
			if (CreateSubNode(key, lOrder, lSubNodeOrder) == NULL) // if no subnode has been chosen -> node is full
			{
				return INSERT_NOSPACE;
			}
		}
		else if (inserted == INSERT_EXIST) // is it possible in Rtree?
		{
			return INSERT_EXIST;
		}
		inserted = InsertToSubNode(lSubNodeOrder, key, lOrder, data, &buffers->itemBuffer);

		this->IncUpdatesCount();
		if (this->GetUpdatesCount() % 20 == 0)
		{
			//Print2File("node.txt", &buffers->itemBuffer);
			this->NodeRebuild_Rtree(buffers);
			//Print2File("node2.txt", &buffers->itemBuffer);
		}
	}


	//Print2File("node.txt", &buffers->itemBuffer);
	if (mHeader->GetRuntimeMode() == cDStructConst::RTMODE_VALIDATION)
	{
		ConsistencyTest(&buffers->itemBuffer);
	}

	return (inserted == INSERT_AT_THE_END) ? INSERT_YES : inserted; // in the case of R-tree, we do not count with case INSERT_AT_THE_END 
}

// ********************************************************************************************************************************************
// ********************************************************* DEBUG & PRINT ********************************************************************
// ********************************************************************************************************************************************


// Method, which prints contents of the node.
template <class TKey>
void cTreeNode<TKey>::Print2File(char* fileName, sItemBuffers* buffer, bool onlyHeaders, ushort subNodeLOrder) const
{
	FILE *streamInfo = fopen(fileName, "a");

	fprintf(streamInfo,"|| ");
	fprintf(streamInfo, "%d, count: %d, freesize: %d, sn count: %d, sn capacity: %d, sn offset: %d", mIndex, mItemCount, mFreeSize, GetSubNodesCount(), GetSubNodesCapacity(), GetSubNodeHeadersOffset());
	fprintf(streamInfo," (%s) ||", (IsLeaf()?"leaf":"inner"));
	
	if (!onlyHeaders)
	{
		ushort from = ((short)subNodeLOrder == -1) ? 0 : subNodeLOrder;
		ushort to = ((short)subNodeLOrder == -1) ? GetSubNodesCount() - 1 : subNodeLOrder;
		for (ushort i = from; i <= to; i++)
		{
			char* aSubNodeHeader = this->GetSubNodeHeader(i);

			fprintf(streamInfo, " \n%d. Mask:", i + 1);
			TMask::Print2File(streamInfo, TSubNode::GetMask(this->GetItems(), aSubNodeHeader), TKey::GetLength(TSubNode::GetMinRefItem(this->GetItems(), aSubNodeHeader), mHeader->GetKeyDescriptor()));
			fprintf(streamInfo, " MinRI: ");
			TKey::Print2File(streamInfo, TSubNode::GetMinRefItem(this->GetItems(), aSubNodeHeader), " ", mHeader->GetKeyDescriptor());
			fprintf(streamInfo, " MaxRI: ");
			TKey::Print2File(streamInfo, TSubNode::GetMaxRefItem(this->GetItems(), aSubNodeHeader), " ", mHeader->GetKeyDescriptor());

			fprintf(streamInfo, ": ");
			fprintf(streamInfo, " first: %d, last: %d, freesize: %d \n", TSubNode::GetFirstItemOrder(aSubNodeHeader), TSubNode::GetLastItemOrder(aSubNodeHeader), this->FreeSize(i));
			fprintf(streamInfo, " position: %d, lastbyte: %d \n", TSubNode::GetSubNodePOrder(aSubNodeHeader), TSubNode::GetLastByte(aSubNodeHeader));

			
			for (ushort j = TSubNode::GetFirstItemOrder(aSubNodeHeader); j <= TSubNode::GetLastItemOrder(aSubNodeHeader); j++)
			{
				fprintf(streamInfo, " %d - %d : ", j, GetItemPOrder(j));
				//TKey::Print2File(streamInfo, TSubNode::GetMinRefItem(GetSubNodeIo(j)), " ", mHeader->GetKeyDescriptor());
				TKey::Print2File(streamInfo, GetCKey(j, buffer, i), " ", mHeader->GetKeyDescriptor());
			}
		}
	}

	if (IsLeaf()) 
	{
		fprintf(streamInfo,"\n | prev: ");
		fprintf(streamInfo,"%d", GetExtraLink(0));
		fprintf(streamInfo,"| next:");
		fprintf(streamInfo,"%d", GetExtraLink(1));
		fprintf(streamInfo,"| parent:");
		fprintf(streamInfo,"%d", GetExtraLink(2));
		fprintf(streamInfo," ");
	}

	fprintf(streamInfo," |\n\n");
    fclose(streamInfo);
}

template<class TKey>
void cTreeNode<TKey>::ComputeDimDistribution(cHistogram** hist)
{
	for (uint i = 0; i < mItemCount; i++)
	{
		TKey::AddToHistogram(GetCKey(i), hist, mHeader->GetKeyDescriptor());
	}
}

template<class TKey>
void cTreeNode<TKey>::ComputeSubNodesDistribution(cHistogram* hist)
{
	hist->AddValue(GetSubNodesCount());
	/*
	char* subNode = this->GetFirstSubNode();
	for (uint i = 0; i < GetSubNodesCount(); i++)
	{
		hist->AddNrToHist(TSubNode::GetItemsCount(subNode));
		subNode = this->GetNextSubNode(subNode);
	}
	*/
}

// Test if the node is consistent 
// It is activated only in the mode RTMODE_VALIDATION
template<class TKey> 
void cTreeNode<TKey>::ConsistencyTest(sItemBuffers *buffers)
{
	// check subnodes integrity
	if (this->GetSubNodesCount() > this->GetSubNodesCapacity())
	{
		printf("Consistency Test: SubNodes Capacity Error !!!\n");
	}

	// check size
	if ((mItemCount > 0) && (mFreeSize != TSubNode::TotalFreeSize(this->GetSubNodeHeaders(), this->GetSubNodeHeadersOffset(), this->GetSubNodesCount())))
	{
		printf("Consistency Test: Check FreeSize Error !!!\n");
		printf("%d != %d", mFreeSize, TSubNode::TotalFreeSize(this->GetSubNodeHeaders(), this->GetSubNodeHeadersOffset(), this->GetSubNodesCount()));
	}

	// check the physical orders of subnodes
	for (ushort i = 0; i < GetSubNodesCount(); i++)
	{
		char* subNodeHeader = this->GetSubNodeHeader(i);
		ushort firstItemOrder = TSubNode::GetFirstItemOrder(subNodeHeader);
		ushort lastItemOrder = TSubNode::GetLastItemOrder(subNodeHeader);

		if ((firstItemOrder > lastItemOrder) || (firstItemOrder < 0) || (lastItemOrder > mItemCount))
		{
			printf("Consistency Test: Check Item Orders Intervals Error !!!\n");
		}

		uint correctFirstOrder = (i == 0) ? 0 : (TSubNode::GetLastItemOrder(this->GetSubNodeHeader(i-1)) + 1);
		if (firstItemOrder != correctFirstOrder)
		{
			printf("Consistency Test: Check Item Orders Intervals Error !!!\n");
		}

		if ((i == (GetSubNodesCount() - 1)) && (lastItemOrder != (mItemCount - 1)))
		{
			printf("Consistency Test: Check Item Orders Intervals Error !!!\n");
		}

		if (((i <  GetSubNodesCount() - 1) && (TSubNode::GetLastByte(subNodeHeader) > TSubNode::GetSubNodePOrder(this->GetSubNodeHeader(i + 1))))
			|| ((i == GetSubNodesCount() - 1) && (TSubNode::GetSubNodePOrder(subNodeHeader) > GetSubNodeHeadersOffset())))
		{
			printf("Consistency Test: Check SubNodes POrders Error !!!\n");
		}
	}


	// check items
	for (ushort i = 0; i < mItemCount; i++)
	{
		if (GetItemPOrder(i) <= 0)
		{
			printf("Consistency Test: Check Item Order Error !!!");
		}

		if (TKey::Equal(GetCKey(i, buffers), TSubNode::GetMinRefItem(this->GetItems(), this->GetSubNodeHeaderByItem(i)), mHeader->GetKeyDescriptor()) == -1)
		{
			printf("%d %d\n", i, mItemCount);
			printf("Consistency Test: Item > Min Reference Item !!!\n");
			Print2File("badNode.txt", false, buffers);
			exit(1);
		}

		if (i == TSubNode::GetLastItemOrder(this->GetSubNodeHeaderByItem(i)))
		{
			if (mHeader->GetDStructCode() == cDStructConst::BTREE)
			{
				if (TKey::Equal(GetCKey(i, buffers), TSubNode::GetMaxRefItem(this->GetItems(), this->GetSubNodeHeaderByItem(i)), mHeader->GetKeyDescriptor()) != 0)
				{
					printf("Consistency Test: Item != Max Reference Item !!!");
				}
			}
			else
			{
				if (TKey::Equal(GetCKey(i, buffers), TSubNode::GetMaxRefItem(this->GetItems(), this->GetSubNodeHeaderByItem(i)), mHeader->GetKeyDescriptor()) > 0)
				{
					printf("Consistency Test: Item > Max Reference Item !!!");
				}
			}
		}
	}
}