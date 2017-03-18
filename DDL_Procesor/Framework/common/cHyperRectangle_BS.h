/**
*	\file cHyperRectangle_BS.h
*	\author Michal Kratky
*	\version 0.1
*	\date jun 2006
*	\brief Tree Hyper Rectangle
*/

#ifndef __cHyperRectangle_BS_h__
#define __cHyperRectangle_BS_h__

namespace dstruct {
	namespace paged {
		namespace rtree {
  class cTuple_BS;
}}}

#include "common/datatype/tuple/cTuple_BS.h"
#include "common/datatype/tuple/cSpaceDescriptor_BS.h"
#include "cBasicType.h"

using namespace common::datatype::tuple;

/**
*	Represents the n-dimensional Hyper Rectangle
*
*	\author Michal Kratky
*	\version 0.1
*	\date jun 2006
**/
class cHyperRectangle_BS
{
private:
	cTuple_BS mStartTuple;
	cTuple_BS mEndTuple;

public:
	cHyperRectangle_BS(cSpaceDescriptor_BS* spaceDescr);
	~cHyperRectangle_BS();
	
	void Resize(cSpaceDescriptor_BS* spaceDescr);

	inline cTuple_BS* GetStartTuple();
	inline cTuple_BS* GetEndTuple();
	inline cTuple_BS& GetRefStartTuple() const;
	inline cTuple_BS& GetRefEndTuple() const;

	static double IntersectionVolume(const cTuple_BS &ql1, const cTuple_BS &qh1, const cTuple_BS &ql2, const cTuple_BS &qh2);
	static double myIntersectionVolume(const cTuple_BS &ql1, const cTuple_BS &qh1, const cTuple_BS &ql2, const cTuple_BS &qh2, const cTuple_BS &tmp_interval);
	static unsigned int approximateIntersectionVolume(const cTuple_BS &ql1, const cTuple_BS &qh1, const cTuple_BS &ql2, const cTuple_BS &qh2, const cTuple_BS &tmp_interval);
	static bool exactIntersectionVolume(const cTuple_BS &ql1, const cTuple_BS &qh1, const cTuple_BS &ql2, const cTuple_BS &qh2, const cTuple_BS &tmp_interval, cBitString &intersectionVolume, cBitString &tmpMul);
	static bool IsIntersected(const cTuple_BS &ql1, const cTuple_BS &qh1, const cTuple_BS &ql2, const cTuple_BS &qh2);
	static bool IsContained(const cTuple_BS &ql1, const cTuple_BS &qh1, const cTuple_BS &ql2, const cTuple_BS &qh2);
	void Subquadrant(cBitString& hqCode, cBitString& tmpString);
	static void PrepareHyperblock(const cTuple_BS &t1, const cTuple_BS &t2, cTuple_BS &ql, cTuple_BS &qh);

	static double Volume(const cTuple_BS &hrl, const cTuple_BS &hrq);
	static bool myVolume(const cTuple_BS &hrl, const cTuple_BS &hrq, cTuple_BS &tmpTuple, cBitString &volume, cBitString &tmp);
	static unsigned int approximateVolume(const cTuple_BS &hrl, const cTuple_BS &hrq, const cTuple_BS &tmpTuple);

	inline void operator = (const cHyperRectangle_BS &tuple);

	void Print(int mode, char *str);
};

inline cTuple_BS* cHyperRectangle_BS::GetStartTuple()
{ 
	return &mStartTuple; 
}
inline cTuple_BS* cHyperRectangle_BS::GetEndTuple()
{ 
	return &mEndTuple; 
}

inline cTuple_BS& cHyperRectangle_BS::GetRefStartTuple() const
{ 
	return (cTuple_BS&)mStartTuple; 
}
inline cTuple_BS& cHyperRectangle_BS::GetRefEndTuple() const
{ 
	return (cTuple_BS&)mEndTuple; 
}

inline void cHyperRectangle_BS::operator = (const cHyperRectangle_BS &rectangle)
{
	mStartTuple = rectangle.GetRefStartTuple();
	mEndTuple = rectangle.GetRefEndTuple();
}

#endif