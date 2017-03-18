/**
*	\file cDStructConst.h
*	\author Michal Krátký
*	\version 0.1
*	\date sep 2011
*	\brief Constants of data structures
*/

#ifndef __cDStructConst_h__
#define __cDStructConst_h__

#include "common/cCommon.h"

using namespace common;

namespace dstruct {
  namespace paged {
	namespace core {

/**
*	Constants of data structures
*
*	\author Michal Krátký
*	\version 0.1
*	\date sep 2011
**/
class cDStructConst
{
public:
	static const int INSERT_NO = -1;
	static const int INSERT_YES = 1;
	static const int INSERT_DUPLICATE = 0;

	static const uint DSMODE_DEFAULT = 0;  // 000
	static const uint DSMODE_RI = 1;       // 001
	static const uint DSMODE_CODING = 2;   // 010
	static const uint DSMODE_RICODING = 5; // 101 bit RI + CODING

	static const uint DSMODE_SIGNATURES = 6;
	static const uint DSMODE_ORDERED = 7;

	static const uint RTMODE_DEFAULT = 0;
	static const uint RTMODE_DEBUG = 1;
	static const uint RTMODE_VALIDATION = 2;

	static const uint TUPLE = 0;
	static const uint NTUPLE = 1;

	static const uint FIXED_LENGTH = 0;
	static const uint VARIABLE_LENGTH = 1;

	static const uint CODE_NOCODING = 0; // see cCoder.h

	static const uint DS_UNKNOWN = 0;
	static const uint BTREE = 1;
	static const uint BTREE_DUP = 2;
	static const uint RTREE = 3;


};
}}}
#endif