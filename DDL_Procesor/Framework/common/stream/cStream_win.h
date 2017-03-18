/**************************************************************************}
{                                                                          }
{    cStream.h                                            		      	   }
{                                                                          }
{                                                                          }
{    Copyright (c) 1998, 2002					       Jiri Dvorsky		   }
{                                                                          }
{    VERSION: 2.0						               DATE 5/12/1998      }
{                                                                          }
{    following functionality:											   }
{				Tridy zabalujici WIN32 API funkce pro praci se soubory     }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      12/08/2006 - Michal Kratky, rewrite of the 32b Seek() at the 64b    }
{                                    version                               }
{                                                                          }
{**************************************************************************/

// #define _WIN32_WINNT 0x501
#ifndef LINUX
	#include <windows.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <io.h>
	#include <fcntl.h>
	// #include <initguid.h>
	#include <guiddef.h>
	#include <winioctl.h>
#else
#endif

#include "common/cCommon.h"
using namespace common;

typedef unsigned int tNodeIndex;


class cStream
{
public:
	cStream();
	~cStream();
	virtual bool Open(const char* AFName, const DWORD mode, const DWORD flag = FILE_ATTRIBUTE_NORMAL) = 0;
	virtual bool Close();
	virtual bool Seek(const llong pos, const DWORD origin = FILE_BEGIN);
	virtual DWORD GetPos();
	virtual DWORD GetSize();
	int Status(void) const
	{
		return m_Status;
	}

	virtual bool Write(char* buf, const int size, int* buflen = NULL);
	virtual bool Read(char* buf, const int size, int* buflen = NULL);
	virtual void Print();
	virtual void Print(unsigned int offset, unsigned int n);
	virtual void PrintChar(unsigned int n);

	bool CopyFrom(cStream* Stream);
	//virtual void TurnHWCacheOff();
	static BOOL GetDriveGeometry(DISK_GEOMETRY *pdg);
protected:
	HANDLE m_handle;
	int m_Status;
	ullong mSeek;
};


class cInputStream: public cStream
{
public:
	cInputStream();
	cInputStream(const char* AFName, const DWORD mode = FILE_FLAG_SEQUENTIAL_SCAN);
#ifdef UNICODE //bas064
	virtual bool Open(WCHAR* AFName, const DWORD mode = FILE_FLAG_SEQUENTIAL_SCAN); //bas064
#else
	virtual bool Open(const char* AFName, const DWORD mode = FILE_FLAG_SEQUENTIAL_SCAN);
#endif
	virtual bool Read(char* buf, const int size, int* buflen = NULL);
};




class cOutputStream : public cStream
{
public:
	cOutputStream();
	cOutputStream(const char* AFName, const DWORD mode = CREATE_ALWAYS);
#ifdef UNICODE //bas064
	virtual bool Open(WCHAR* AFName, const DWORD mode = CREATE_ALWAYS, const DWORD flag = FILE_ATTRIBUTE_NORMAL);
#else
	virtual bool Open(const char* AFName, const DWORD mode = CREATE_ALWAYS, const DWORD flag = FILE_ATTRIBUTE_NORMAL);
#endif
	virtual bool Write(char* buf, const int size, int* buflen = NULL);
};



class cIOStream : public cStream
{
public:
	cIOStream();
	cIOStream(const char* AFName, const DWORD mode = OPEN_EXISTING);
#ifdef UNICODE //bas064
	virtual bool Open(WCHAR* AFName, const DWORD mode = OPEN_EXISTING);
	virtual bool Open(WCHAR* AFName, const DWORD mode, const DWORD flags);
#else
	virtual bool Open(const char* AFName, const DWORD mode = OPEN_EXISTING);
	virtual bool Open(const char* AFName, const DWORD mode, const DWORD flags);
#endif
	virtual bool Write(char* buf, const int size, int* buflen = NULL);
	virtual bool Read(char* buf, const int size, int* buflen = NULL);
};
