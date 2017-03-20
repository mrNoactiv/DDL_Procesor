#include "cTreeNodeHeader.h"

namespace dstruct {
  namespace paged {
	namespace core {

cTreeNodeHeader::cTreeNodeHeader()
{
	mOrderingEnabled = false;
}

/**
* Construktor
* \param itemInMemSize Size of an item in main memory.
* \param itemSerialSize Size of an item when serialized on the secondary storage.
*/
cTreeNodeHeader::cTreeNodeHeader(bool leafNode, unsigned int keyInMemSize, unsigned int dataInMemSize, bool varKey, bool varData, unsigned int dsMode)
{
	mOrderingEnabled = false;
	cTreeNodeHeader::Init(leafNode, keyInMemSize, dataInMemSize, varKey, varData, dsMode);
}

cTreeNodeHeader::cTreeNodeHeader(const cTreeNodeHeader &header)
{
	mNodeFanoutCapacity = header.GetNodeFanoutCapacity();
	mNodeDeltaCapacity = header.GetNodeDeltaCapacity();

	mNodeExtraItemCount = header.GetNodeExtraItemCount();
	mNodeExtraLinkCount = header.GetNodeExtraLinkCount();

	mNodeSerialSize = header.GetNodeSerialSize();
	mNodeInMemSize = header.GetNodeInMemSize();
	mItemSize = header.GetItemSize();
	mKeySize = header.GetKeySize();
	mDataSize = header.GetDataSize();
}

cTreeNodeHeader::~cTreeNodeHeader()
{
}

/**
 * Method compute node capacity for a known node size, item size, extra item count and extra link count 
 */
void cTreeNodeHeader::ComputeNodeCapacity(unsigned int blockSize, bool isLeaf)
{
	uint itemSize;

	// in the case of compression increase the capacity
	if (isLeaf && (mDStructMode == cDStructConst::DSMODE_RI || mDStructMode == cDStructConst::DSMODE_CODING || mDStructMode == cDStructConst::DSMODE_RICODING))
	{
		itemSize = mItemSize / mCompressionRatio;
	}
	else if (VariableLenKeyEnabled() || (isLeaf && VariableLenDataEnabled()))
	{
		// in the case of variable key or data increase the capacity
		// i.e. the average item size = max item size / 7 (let us suppose strings)
		// !! very stupid solution !!
		if (mItemSize > 250)
		{
			itemSize = mItemSize / 25;  
		} else if (mItemSize > 200)
		{
			itemSize = mItemSize / 20;  
		} else if (mItemSize > 100)
		{
			itemSize = mItemSize / 7;  
		} else if (mItemSize > 65)
		{
			itemSize = mItemSize / 5;  
		} else if (mItemSize > 30)
		{
			itemSize = mItemSize / 3;
		}
		else
		{
			itemSize = mItemSize / 2;
		}
	} else
	{
		itemSize = mItemSize;
	}

	unsigned int basSize = NODE_PREFIX_SERIAL +
		sizeof(bool) +   // see: bool cTreeNode<TKey>::IsLeaf(), PCH, MK: nenÌ to dob¯e, tohle souvisÌ s pamÏùovou reprezentacÌ uzlu, 
						 //   ten bool se na disk zapisuje do mItemCount
		mNodeExtraItemCount * mItemSize +         // number of extra items in the node
		mNodeExtraLinkCount * sizeof(tNodeIndex); // number of extra links in the node

	if ((isLeaf && mDStructMode == cDStructConst::DSMODE_RI) || (isLeaf && mDStructMode == cDStructConst::DSMODE_RICODING))
	{
		basSize += 3 * sizeof(ushort) + sizeof(uchar); // variable with the number of subNodes + capacity of subNodes + subNodes headers + variable with the number of node updates
	}

	unsigned int size = blockSize - basSize; // size of the items data without extra information
	mNodeCapacity = size / itemSize;

	mIsLeaf = isLeaf;
	if (isLeaf)
	{
		mLinkSize = 0;
	} 
	else
	{
		mLinkSize = sizeof(tNodeIndex);
	}

	mNodeDeltaCapacity = 0;
	mNodeItemsSpaceSize = size;

	// computer real size of a inner node
	mNodeSerialSize = blockSize;

	SetInMemOrders(mIsLeaf);
}

/**
 * Set memory offsets in the header, these offsets are used when we accees data in char* array of a node.
 */
void cTreeNodeHeader::SetInMemOrders(bool isLeaf)
{
	assert(mNodeItemsSpaceSize != cCommon::UNDEFINED_UINT);

	if (isLeaf && (mDStructMode == cDStructConst::DSMODE_RI || mDStructMode == cDStructConst::DSMODE_RICODING))
	{
		mOffsetSubNodesCount = sizeof(bool);  // see: bool cTreeNode<TKey>::IsLeaf()
		mOffsetSubNodesCapacity = mOffsetSubNodesCount + sizeof(ushort);
		mOffsetUpdatesCount = mOffsetSubNodesCapacity + sizeof(ushort);
		mOffsetSubNodesHeaders = mOffsetUpdatesCount + sizeof(uchar);
		mOffsetItems = mOffsetSubNodesHeaders + sizeof(ushort);
	}
	else
	{
		mOffsetItems = sizeof(bool);  // see: bool cTreeNode<TKey>::IsLeaf()
	}

	mOffsetItemOrder = mOffsetItems + mNodeItemsSpaceSize; 
	mOffsetExtraItems = mOffsetItemOrder + mNodeCapacity * ItemSize_ItemOrder;
	mOffsetExtraLinks = mOffsetExtraItems + mNodeExtraItemCount * mItemSize;
	mNodeInMemSize = mOffsetExtraLinks + mNodeExtraLinkCount * ItemSize_Links;
	mOffsetLinks = -1;
}
}}}