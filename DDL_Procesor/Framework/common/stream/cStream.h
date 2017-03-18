/**
 *	\file cStream.h
 *	\author Jiøí Dvorský (1998), Michal Krátký (2014)
 *	\version 0.2
 *	\date jan 2014
 *	\brief Root of stream classes
 */

#ifndef __cStream_h__
#define __cStream_h__

#include <stdio.h>
#include "common/cCommon.h"

using namespace common;

#define SEEK_BEGIN 0

namespace common {
	namespace stream {

class cStream
{
public:
	virtual bool Seek(const llong pos, const ushort origin = SEEK_BEGIN) = 0;
	virtual llong GetOffset() = 0;
	virtual llong GetSize() = 0;

	virtual bool Write(char* buf, const int size, int* buflen = NULL) = 0;
	virtual bool Read(char* buf, const int size, int* buflen = NULL) = 0;

	virtual void Print() = 0;
	virtual void Print(unsigned int offset, unsigned int n) = 0;
	virtual void PrintChar(unsigned int n) = 0;
};
}}

#endif  //  __cStream_h__