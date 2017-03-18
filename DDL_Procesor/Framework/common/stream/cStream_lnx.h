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

#ifndef __CSTREAMS_H__
#define __CSTREAMS_H__

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
using namespace std;

typedef unsigned int tNodeIndex;
typedef unsigned int HANDE;

class cStream
{
public:
	cStream();
	~cStream();
	virtual bool Open(const char* AFName, const ushort mode, const ushort flag = /* FILE_ATTRIBUTE_NORMAL */) = 0;
	virtual bool Close();
	virtual bool Seek(const llong pos, const ushort origin = 0 /* FILE_BEGIN */);
	virtual ushort GetPos();
	virtual ushort GetSize();
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
	// static BOOL GetDriveGeometry(DISK_GEOMETRY *pdg);
protected:
	HANDLE m_handle;
	int m_Status;
	ullong mSeek;
};


class cInputStream: public cStream
{
public:
	cInputStream();
	cInputStream(const char* AFName, const ushort mode = 0 /* FILE_FLAG_SEQUENTIAL_SCAN */);
#ifdef UNICODE //bas064
	virtual bool Open(WCHAR* AFName, const ushort mode = /* FILE_FLAG_SEQUENTIAL_SCAN */); //bas064
#else
	virtual bool Open(const char* AFName, const ushort mode = 0 /* FILE_FLAG_SEQUENTIAL_SCAN */);
#endif
	virtual bool Read(char* buf, const int size, int* buflen = NULL);
};




class cOutputStream : public cStream
{
public:
	cOutputStream();
	cOutputStream(const char* AFName, const ushort mode = 0 /*CREATE_ALWAYS*/);
#ifdef UNICODE //bas064
	virtual bool Open(WCHAR* AFName, const ushort mode = 0 /*CREATE_ALWAYS*/, const ushort flag = 0 /*FILE_ATTRIBUTE_NORMAL*/);
#else
	virtual bool Open(const char* AFName, const ushort mode = 0 /* CREATE_ALWAYS */, const ushort flag = 0 /* FILE_ATTRIBUTE_NORMAL */);
#endif
	virtual bool Write(char* buf, const int size, int* buflen = NULL);
};



class cIOStream : public cStream
{
public:
	cIOStream();
	cIOStream(const char* AFName, const ushort mode = 0 /*OPEN_EXISTING*/);
#ifdef UNICODE //bas064
	virtual bool Open(WCHAR* AFName, const ushort mode = 0 /* OPEN_EXISTING */);
	virtual bool Open(WCHAR* AFName, const ushort mode, const ushort flags);
#else
	virtual bool Open(const char* AFName, const ushort mode = 0 /* OPEN_EXISTING*/);
	virtual bool Open(const char* AFName, const ushort mode, const ushort flags);
#endif
	virtual bool Write(char* buf, const int size, int* buflen = NULL);
	virtual bool Read(char* buf, const int size, int* buflen = NULL);
};
#endif  //  __CSTREAMS_H__
