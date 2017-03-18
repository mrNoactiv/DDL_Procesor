/**************************************************************************}
{                                                                          }
{    cMemCheckUtil.h                                 		      	 	   }
{                                                                          }
{                                                                          }
{                 Copyright (c) 2010	   				Peter Chovanec     }
{                                                                          }
{    VERSION: 1.0										DATE 15/10/2010    }
{                                                                          }
{             following functionality:                                     }
{               method for memory validation                               }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{    xx.xx.xxxx                                                            }
{                                                                          }
{                                                                          }
{**************************************************************************/

#ifndef __cMemCheckUtil_h__
#define __cMemCheckUtil_h__

#ifdef __CRT_DEBUG__
  #include <crtdbg.h>
#endif

namespace common {
	namespace utils {

class cMemCheckUtil
{
public:
	static void CrtCheckMemory();
	
};
}}
#endif