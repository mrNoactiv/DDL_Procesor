#ifndef __sCoverRecord_h__
#define __sCoverRecord_h__

#include "common/cCommon.h"

using namespace common;

namespace dstruct {
	namespace paged {
		namespace core {

			struct sCoverRecord
			{
				ushort startItemOrder;
				ushort endItemOrder;
				char* mask;
				char* minRefItem;
				bool optimal;
			};
}}}
#endif