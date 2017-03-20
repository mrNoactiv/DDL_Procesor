#include "cDStructHeader.h"

namespace dstruct {
  namespace paged {
	namespace core {

cDStructHeader::cDStructHeader()
{
}

cDStructHeader::cDStructHeader(const cDStructHeader &header)
{
	Null();

	strncpy(mTitle, header.GetTitle(), TITLE_SIZE);
	mVersion = header.GetVersion();
	mBuild = header.GetBuild();

	// mNodeSize = header.GetNodeSize();

	mMeasureTime = header.GetMeasureTime();
	mMeasureCount = header.GetMeasureCount();
	mCacheMeasureTime = header.GetCacheMeasureTime();
	mCacheMeasureCount = header.GetCacheMeasureCount();

	// mNodeSize = header.GetSize();
	mRealHeaderSize = TITLE_SIZE * sizeof(char) + sizeof(float) + 2 * sizeof(unsigned int);
	mHeaderSize = BLOCK_SIZE;

	mNodeHeaders = header.GetNodeHeader();
	mNodeHeaderCount = header.GetNodeHeaderCount();
	mNodeIds = header.GetNodeIds();
}

cDStructHeader::~cDStructHeader()
{ 
	Delete();
}
}}}