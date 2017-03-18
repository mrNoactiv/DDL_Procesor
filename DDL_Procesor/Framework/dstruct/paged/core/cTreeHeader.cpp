#include "cTreeHeader.h"

namespace dstruct {
  namespace paged {
	namespace core {

cTreeHeader::cTreeHeader() : mHistogramEnabled(false)
{
}

/// !! copy headers !!
cTreeHeader::cTreeHeader(const cTreeHeader &header): cDStructHeader(header)
{

	mHeight = header.GetHeight();
	mDuplicates = header.DuplicatesAllowed();
//	mCompressionRate = header.GetCompressionRate();

	/*mNodeRealSize = header.GetNodeRealSize();
	mLeafNodeRealSize = header.GetLeafNodeRealSize();
	mLeafNodeCount = header.GetLeafNodeCount();
	mInnerNodeCount = header.GetInnerNodeCount();
	mLeafItemCount = header.GetLeafItemCount();
	mInnerItemCount = header.GetInnerItemCount();*/
	mRootIndex = header.GetRootIndex();

	/*mNodeItemCapacity = header.GetNodeItemCapacity();
	mLeafNodeItemCapacity = header.GetLeafNodeItemCapacity();
	mNodeFanoutCapacity = header.GetNodeFanoutCapacity();
	mLeafNodeFanoutCapacity = header.GetLeafNodeFanoutCapacity();
	mLeafNodeDeltaCapacity = header.GetLeafNodeDeltaCapacity();
	mNodeDeltaCapacity = header.GetNodeDeltaCapacity();

	mNodeExtraItemCount = header.GetNodeExtraItemCount();
	mLeafNodeExtraItemCount = header.GetLeafNodeExtraItemCount();
	mNodeExtraLinkCount = header.GetNodeExtraLinkCount();
	mLeafNodeExtraLinkCount = header.GetLeafNodeExtraLinkCount();
	mNodeItemSize = header.GetNodeItemSize();
	mLeafNodeItemSize = header.GetLeafNodeItemSize();*/

	// !! what about copying attributes
	AddHeaderSize(26 * sizeof(unsigned int) + sizeof(bool)); // mk!! 26 - 8 (tmpItems)
}

cTreeHeader::~cTreeHeader()
{ 
}


/**
 * For known item, fanout, extra item and extra links capacity compute node size a capacity of leaf node.
 * Params: bool multiply - flush node size at multiply of PAGE_SIZE
 */
//void cTreeHeader::ComputeNodeSize(bool multiply)
//{
	//mNodeDeltaCapacity = mNodeFanoutCapacity - mNodeItemCapacity;
	//mNodeSize = mNodeRealSize = sizeof(unsigned int) + mNodeItemCapacity * mNodeItemSize + mNodeFanoutCapacity*sizeof(tNodeIndex) +
	//	mNodeExtraItemCount*mNodeItemSize + mNodeExtraLinkCount*sizeof(tNodeIndex);

	//if (multiply) // if multiply is set then change node size and recompute arity
	//{
	//	if (mNodeSize % BLOCK_SIZE != 0)
	//	{
	//		mNodeSize = ((mNodeSize / BLOCK_SIZE) + 1) * BLOCK_SIZE;
	//	}
	//	ComputeOptimCapacity(true);
	//}

	//ComputeLeafNodeCapacity();
	//mNodeHeaders[1]->ComputeNodeSize(multiply);
//}

///**
// * For known node size, delta capacity, extra item and extra link capacity 
// * compute item and fanout capacity.
// */
//void cTreeHeader::ComputeNodeCapacity(unsigned int maxNodeSerialSize)
//{
//	for (unsigned int i = 0; i < mNodeHeaderCount; i++)
//	{
//		((cTreeNodeHeader*)mNodeHeaders[i])->ComputeNodeCapacity(maxNodeSerialSize);
//	}
//}

/**
 * Compute node capacity by defined compression rate. Usable by compression of data structure (node need reimplement methods
 *   Read() and Write().
 * For known node size, delta capacity, extra item and extra link capacity compute item and fanout capacity.
 */
//void cTreeHeader::ComputeNodeCapacity(unsigned int compressionRate)
//{
//	for (unsigned int i = 0; i < mNodeHeaderCount; i++)
//	{
//		((cTreeNodeHeader*)mNodeHeaders[i])->ComputeNodeCapacity(compressionRate);
//	}
//}

}}}