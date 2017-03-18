/**************************************************************************}
{                                                                          }
{    cUBTreeConst.h                                 		           }
{                                                                          }
{																		    }
{                                                                          }
{    VERSION: 0.1			    DATE 
}
{                                                                          }
{             following functionality:                                     }
{               constants of the UBtree                                }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{                                                                          }
{**************************************************************************/

#ifndef __cUBTreeConst_h__
#define __cUBTreeConst_h__

namespace dstruct {
	namespace paged {
		namespace ubtree {

class cUBTreeConst
{
public:
	static const int NO_COMPRESSION = 0;
	static const int FIB_DIF_COMPRESSION = 1;
	static const int DIF_SIMPLE_COMPRESSION = 2;

	static const unsigned int INSERT_MODE_INSERT_ONLY = 0;
	static const unsigned int INSERT_MODE_INSERT_OR_INC = 1;
};
}}}
#endif
