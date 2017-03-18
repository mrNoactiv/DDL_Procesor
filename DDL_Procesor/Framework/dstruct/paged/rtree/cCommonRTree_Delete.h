/**
* Return true if the tuple exists in R-Tree, data return in item.
* Because this implementation of R-tree allows overlapped regions, do the point query using range query.
**/
template<class TMbr, class TKey, class TNode, class TLeafNode>
bool cCommonRTree<TMbr, TKey, TNode, TLeafNode>::Delete(const TKey &item)
{
	return Delete(parent::mHeader->GetRootIndex(), item, 0);
}

template<class TMbr, class TKey, class TNode, class TLeafNode>
bool cCommonRTree<TMbr, TKey, TNode, TLeafNode>::Delete(const tNodeIndex& nodeIndex, const TKey& item, uint level)
{
	bool deleted = false;
	TNode* node = NULL;
	TNode* node2 = NULL;
	TLeafNode* leafNode = NULL;
	TLeafNode* leafNode2 = NULL;

	if (TNode::IsLeaf(nodeIndex))
	{
		leafNode = mTree->ReadLeafNodeW(nodeIndex);
		deleted = leafNode->Delete(item); // it does not need buffers for the default R-tree
		mQuickDB->GetNodeCache()->UnlockW(leafNode);
	}
	else
	{
		node = mTree->ReadInnerNodeW(nodeIndex);

		int itemOrder = -1;
		int itemCount = (int) node->GetItemCount();

		while (true)
		{
			node->DeleteScan(itemOrder, item);

			if (itemOrder < itemCount)
			{
				deleted = Delete(node->GetLink(itemOrder), item, level + 1);
				if (deleted)
				{
					if (level == parent::mHeader->GetHeight() - 1)
					{
						leafNode2 = mTree->ReadLeafNodeW(node->GetLink(itemOrder));
						leafNode2->ModifyMbr(node->GetCKey(itemOrder));
						mQuickDB->GetNodeCache()->UnlockW(leafNode2);
					}
					else
					{
						node2 = mTree->ReadInnerNodeW(node->GetLink(itemOrder));
						node2->ModifyMbr((char*)node->GetCKey(itemOrder));
						mQuickDB->GetNodeCache()->UnlockW(node2);
					}
				}
			}
			else
			{
				break;
			}
		}
		mQuickDB->GetNodeCache()->UnlockW(node);
	}
	
	return deleted;
}


