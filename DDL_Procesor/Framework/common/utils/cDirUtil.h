/**************************************************************************}
{                                                                          }
{    cDirUtil.h                                 		      		  	   }
{                                                                          }
{                                                                          }
{                 Copyright (c) 2010, 2011	   			Peter Chovanec     }
{                                                                          }
{    VERSION: 1.0										DATE ??/??/2011    }
{                                                                          }
{             following functionality:                                     }
{               method for a work with a directory                         }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{    xx.xx.xxxx                                                            }
{                                                                          }
{                                                                          }
{**************************************************************************/

#ifndef __cDirUtil_h__
#define __cDirUtil_h__

#include <string.h>
#include <windows.h>

namespace common {
	namespace utils {

class cDirUtil
{
private:
	static bool IsDots(char* str);

public:
	static const int STR_LENGTH = 1024;

	static void Append(char* dir, const char* file, char *output);
	static bool Exists(char* filename);	
	static bool DeleteDirectory(char* sPath); 

};
}}
#endif