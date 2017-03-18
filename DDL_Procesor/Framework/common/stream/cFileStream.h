/**
 *	\file cFileStream.h
 *	\author Jiøí Dvorský (1998), Michal Krátký (2014)
 *	\version 0.2
 *	\date jan 2014
 *	\brief File streams
 */

#ifndef __cFileStream_h__
#define __cFileStream_h__

#include <stdio.h>
#include "cStream.h"
#include "common/cCommon.h"

using namespace common;


// access mode
#define ACCESS_READ  1 
#define ACCESS_WRITE 0
#define ACCESS_READWRITE 2

// create mode
#define FILE_OPEN   0   // win32: OPEN_EXISTING
#define FILE_CREATE 1   // win32: CREATE_ALWAYS

// flags
#define FLAGS_NORMAL 0
#define DIRECT_IO 1        // direct IO: win32: FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH
#define SEQUENTIAL_SCAN 2  // win32: FILE_FLAG_SEQUENTIAL_SCAN

namespace common {
	namespace stream {

class IFileStream: public cStream
{
public:
	virtual bool Open(const char* name, const ushort accessMode, const ushort createMode, const ushort flags = FLAGS_NORMAL) = 0;
	virtual bool Close() = 0;
	virtual int Status(void) const = 0;
};

class IFileInputStream: public IFileStream
{
public:
#ifdef UNICODE
	virtual bool Open(const wchar_t* name, const ushort flags = FLAGS_NORMAL) = 0;
#else
	virtual bool Open(const char* name, const ushort flags = FLAGS_NORMAL) = 0;
#endif
};

class IFileOutputStream : public IFileStream
{
public:
#ifdef UNICODE
	virtual bool Open(wchar_t* name, const ushort createMode = FILE_CREATE, const ushort flags = FLAGS_NORMAL) = 0;
#else
	virtual bool Open(const char* name, const ushort createMode = FILE_CREATE, const ushort flags = FLAGS_NORMAL) = 0;
#endif
};

}}

#ifdef LINUX
#include "cFileStream_lnx.h"
#else
#include "cFileStream_win.h"
#endif

#endif  //  __cFileStream_h__