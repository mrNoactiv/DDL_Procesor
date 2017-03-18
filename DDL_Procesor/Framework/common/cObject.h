/**************************************************************************}
{                                                                          }
{    cObject.h                                                             }
{                                                                          }
{                                                                          }
{    Copyright (c) 2001                      Michal Kratky                 }
{                                                                          }
{    VERSION: 0.2                            DATE 5/11/2002                }
{                                                                          }
{    following functionality:                                              }
{       root of class hierarchy                                            }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      xx/xx/xxxx                                                          }
{                                                                          }
{**************************************************************************/

#ifndef __cObject_h__
#define __cObject_h__

class cObject
{
public:
	cObject(void);
	~cObject(void);

	static const unsigned int UNONDEFINED = (unsigned int)~0;
	static const int NONDEFINED = -1;
	static const int MODE_DEC = 0;
	static const int MODE_BIN = 1;
	static const int MODE_CHAR = 2;

	static const int STRING_LENGTH = 1024;
};
#endif
