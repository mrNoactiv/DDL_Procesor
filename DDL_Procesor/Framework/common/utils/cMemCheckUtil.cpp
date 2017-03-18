#include "cMemCheckUtil.h"

namespace common {
	namespace utils {

void cMemCheckUtil::CrtCheckMemory()
{
	#ifdef __CRT_DEBUG__
		_CrtCheckMemory();
	#endif
}

}}
