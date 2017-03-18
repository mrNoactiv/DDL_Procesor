/**************************************************************************}
{                                                                          }
{    cB+TreeConst.h                                 		           }
{                                                                          }
{   Copyright (c) 2001, 2003	   			Pavel Moravec      }
{                                                                          }
{    VERSION: 0.1			    DATE 20/08/2003                }
{                                                                          }
{             following functionality:                                     }
{               constants of the B+-tree                                 }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{                                                                          }
{**************************************************************************/

#ifndef __cBpTreeConst_h__
#define __cBpTreeConst_h__

namespace dstruct {
	namespace paged {
		namespace bptree {

class cBpTreeConst
{
public:
	static const int NO_COMPRESSION = 0;
	static const int FIB_DIF_COMPRESSION = 1;
	static const int DIF_SIMPLE_COMPRESSION = 2;

	/* a dalsi ...
	static const int DIFF_COMPRESSION = 1;
	static const int DIFF_COMPRESSION2 = 2;
	static const int GOLOMB4_COMPRESSION = 3;
	static const int GOLOMB8_COMPRESSION = 4;
	static const int GOLOMB16_COMPRESSION = 5;
	static const int ELIASGAMA_COMPRESSION = 6;
	static const int ELIASDELTA_COMPRESSION = 7;
	*/

	static const unsigned int INSERT_MODE_INSERT_ONLY = 0;
	static const unsigned int INSERT_MODE_INSERT_OR_INC = 1;
};
}}}
#endif
