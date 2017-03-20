/**
*	\file cSequentialArrayHeader.h
*	\author Radim Baca
*	\version 0.1
*	\date jul 2011
*	\brief Header of the cSequentialArray.
*/


#ifndef __cSequentialArrayHeader_h__
#define __cSequentialArrayHeader_h__

namespace dstruct {
	namespace paged {
		namespace sqarray {
template<class TItem> class cSequentialArrayHeader;
}}}

#include "dstruct/paged/core/cDStructHeader.h"
#include "dstruct/paged/sequentialarray/cSequentialArrayNodeHeader.h"

using namespace dstruct::paged::core;

namespace dstruct {
	namespace paged {
		namespace sqarray {

/**
*	Header of the cSequentialArray.
*
*	\author Radim Baca
*	\version 0.1
*	\date jul 2011
**/
template<class TItem>
class cSequentialArrayHeader :	public cDStructHeader
{
	unsigned int	mFirstNodeIndex;	/// Index of the first node in the persistent array.
	unsigned int	mLastNodeIndex;		/// Index of the last node in the persistent array.
	bool			mRewriting;			/// If true then we are only rewriting array existing in the main memory.
	unsigned int	mNextNodeIndex;		/// Variable used during the rewriting to keep the next node available.

public:
	cSequentialArrayHeader(const char* dsName, unsigned int blockSize, cDTDescriptor *dd, unsigned int dsMode = cDStructConst::DSMODE_DEFAULT);
	cSequentialArrayHeader(const cSequentialArrayHeader<TItem> &header);
	inline virtual void Init(const char* dsName, unsigned int blockSize, cDTDescriptor *dd, unsigned int dsMode = cDStructConst::DSMODE_DEFAULT);
	inline virtual bool Write(cStream *stream);
	inline virtual bool Read(cStream *stream);

	inline void SetLastNodeIndex(unsigned int lastIndex)	{ mLastNodeIndex = lastIndex; }
	inline void SetNextNodeIndex(unsigned int nextIndex)	{ mNextNodeIndex = nextIndex; }
	inline void SetFirstNodeIndex(unsigned int firstIndex)	{ mFirstNodeIndex = firstIndex; }
	inline void SetRewriting(bool rewriting)				{ mRewriting = rewriting; }
	inline void SetNodeCount(unsigned int count)			{ mNodeHeaders[0]->SetNodeCount(count); }
	inline void IncNodeCount()								{ mNodeHeaders[0]->IncrementNodeCount(); }
	inline void IncItemCount()								{ mNodeHeaders[0]->IncrementItemCount(); }

	inline unsigned int GetLastNodeIndex() const			{ return mLastNodeIndex; }
	inline unsigned int GetNextNodeIndex() const			{ return mNextNodeIndex; }
	inline unsigned int GetFirstNodeIndex() const			{ return mFirstNodeIndex; }
	inline bool GetRewriting() const						{ return mRewriting; }
	inline unsigned int GetNodeCount() const				{ return mNodeHeaders[0]->GetNodeCount(); }
	inline unsigned int GetItemCount() const				{ return mNodeHeaders[0]->GetItemCount(); }
};


/**
 * \param nodeSize Size of the node in the persistent data structure.
 */
template<class TItem>
cSequentialArrayHeader<TItem>::cSequentialArrayHeader(
	const char* dsName, unsigned int blockSize,	cDTDescriptor *dd, unsigned int dsMode)
{
	Init(dsName, blockSize, dd, dsMode);
}

template<class TItem>
cSequentialArrayHeader<TItem>::cSequentialArrayHeader(const cSequentialArrayHeader<TItem> &header)
	: cDStructHeader(header)
{
}

template<class TItem>
void cSequentialArrayHeader<TItem>::Init(
	const char* dsName, unsigned int blockSize,	cDTDescriptor *dd, unsigned int dsMode)
{
	unsigned int arity = 17; // 19;

	cDStructHeader::Init();

	SetNodeHeaderCount(1);
	SetNodeHeader(0, new cSequentialArrayNodeHeader<TItem>());

	SetTitle("Sequential array");
	SetVersion((float)0.20);
	SetBuild(0x20031201);

	SetMeasureTime(false);
	SetMeasureCount(true);
	SetCacheMeasureTime(false);
	SetCacheMeasureCount(true);

	SetName(dsName);
	GetNodeHeader(0)->SetKeyDescriptor(dd);
	GetNodeHeader(0)->SetDStructMode(dsMode);
	((cSequentialArrayNodeHeader<TItem>*)GetNodeHeader(0))->ComputeNodeCapacity(blockSize);
}

/// Write a some additional information into the stream
/// \param stream Stream where the data are serialized
template<class TItem>
bool cSequentialArrayHeader<TItem>::Write(cStream *stream) 
{ 
	bool ok = cDStructHeader::Write(stream);
		
	ok &= stream->Write((char*)&mLastNodeIndex, sizeof(mLastNodeIndex));
	ok &= stream->Write((char*)&mFirstNodeIndex, sizeof(mFirstNodeIndex));
	ok &= stream->Write((char*)&mNextNodeIndex, sizeof(mNextNodeIndex));
	ok &= stream->Write((char*)&mRewriting, sizeof(mRewriting));

	return ok;
}

/// Read a some additional header information into the stream
/// \param stream Stream from wich the data are readed
template<class TItem>
bool cSequentialArrayHeader<TItem>::Read(cStream *stream) 
{ 
	bool ok = cDStructHeader::Read(stream);

	ok &= stream->Read((char*)&mLastNodeIndex, sizeof(mLastNodeIndex));
	ok &= stream->Read((char*)&mFirstNodeIndex, sizeof(mFirstNodeIndex));
	ok &= stream->Read((char*)&mNextNodeIndex, sizeof(mNextNodeIndex));
	ok &= stream->Read((char*)&mRewriting, sizeof(mRewriting));
	return ok;
}

		}}}
#endif
