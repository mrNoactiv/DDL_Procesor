#include "cNode.h"

namespace dstruct {
  namespace paged {
	namespace core {

cNode::cNode(unsigned int size, unsigned int order): mDebug(false)
{
	Init(size, order, NULL);
}

cNode::cNode(): mDebug(false)
{
	mData = NULL;
	mIndex = EMPTY_INDEX;
}

cNode::~cNode()
{
	if (mData != NULL)
	{
		delete []mData;
		mData = NULL;
	}
	//Delete();
}

void cNode::Delete()
{
	if (mData != NULL)
	{
		delete []mData;
		mData = NULL;
	}
}

void cNode::ClearHeader()
{
	if (mHeader != NULL)
	{
		delete mHeader;
		mHeader = NULL;
	}
}

//void cNode::SetRealSize(unsigned int size)
//{
//	mRealSize = size;
//}
//
//unsigned int cNode::GetRealSize() const
//{
//	return mRealSize;
//}

unsigned int cNode::GetSize()
{
	return mMaxSize;
}

void cNode::Write(cStream* stream)
{
	stream->Write(mData, mHeader->GetNodeSerialSize());
}

void cNode::Read(cStream* stream)
{
	stream->Read(mData, mHeader->GetNodeSerialSize());
}
}}}