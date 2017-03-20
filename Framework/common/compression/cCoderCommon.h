// MathFuncsLib.h
#ifndef __CoderCommon_h__
#define __CoderCommon_h__

#include <stdio.h>
#include "limits.h"
#include "common/cCommon.h"
#include "common/cNumber.h"

using namespace std;
using namespace common;

namespace common {
	namespace compression {

	class cCoderCommon
	{
	public:
		static unsigned int Increment(int bytePerNumber, unsigned int value);
		static uint GetBitLength(const uchar* value, uint bytesPerNumber);
		static uint GetByteLength(uint bitLength);
	};
}}
#endif