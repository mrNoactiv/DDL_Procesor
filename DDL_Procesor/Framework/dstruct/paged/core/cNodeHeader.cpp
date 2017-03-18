#include "cNodeHeader.h"

namespace dstruct {
  namespace paged {
	namespace core {

//inline void cNodeHeader::AddHeaderSize(unsigned int userHeaderSerSize) 
//{
//	mRealHeaderSize += userHeaderSerSize;
//	if (mRealHeaderSize > mHeaderSize)
//	{
//		unsigned int mul = mRealHeaderSize / BLOCK_SIZE;
//		if ((mRealHeaderSize % BLOCK_SIZE) != 0)
//		{
//			mul++;
//		}
//		mHeaderSize += mul * BLOCK_SIZE;
//	}
//}

cNodeHeader::cNodeHeader()
{
	cNodeHeader::Init();
}

cNodeHeader::cNodeHeader(const cNodeHeader &header)
{
	mNodeCount = header.GetInnerNodeCount();
	mItemCount = header.GetInnerItemCount();
	mNodeCapacity = header.GetNodeCapacity();
	mItemSize = header.GetItemSize();
	mNodeSerialSize = header.GetNodeSerialSize();
	mNodeInMemSize = header.GetNodeInMemSize();
	mCacheMeasureCount = header.GetCacheMeasureCount();
	mCacheMeasureTime = header.GetCacheMeasureTime();

	//AddHeaderSize(26 * sizeof(unsigned int) + sizeof(bool));
}

cNodeHeader::~cNodeHeader()
{
}

}}}