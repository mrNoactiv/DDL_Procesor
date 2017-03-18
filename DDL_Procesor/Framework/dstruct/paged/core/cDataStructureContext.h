/**
*	\file cDataStructureContext.h
*	\author Radim Baca
*	\version 0.1
*	\date apr 2009
*	\brief Abstract class representing context information during the work with some data structure.
*/


#ifndef __cDataStructureContext_h__
#define __cDataStructureContext_h__


namespace dstruct {
  namespace paged {
	namespace core {

/**
* Abstract class representing context information during the work with some data structure.
* This class is an ancestor for different specialized data structures (tree data structures, persistent array etc.).
* Context information can be also specific for a range query operation and for a simple update operations.
*
*	\author Radim Baca
*	\version 0.1
*	\date apr 2009
**/
class cDataStructureContext
{
	bool mOpen;		// true value inicate that the context is currently used.
public:
	cDataStructureContext()		{ mOpen = false; }
	~cDataStructureContext()	{}

	inline void Open();
	inline void Close();
	inline bool IsOpen() const		{ return mOpen; }
};

/**
* Open context. Context has to be closed in order to use this method.
*/
void cDataStructureContext::Open()
{
	assert(!mOpen);
	mOpen = true;
}

/**
* Close context. Context has to be opened in order to use this method.
*/
void cDataStructureContext::Close()
{
	assert(mOpen);
	mOpen = false;
}

	}}}
#endif