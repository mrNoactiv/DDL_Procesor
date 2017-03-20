#include "common/memdatstruct/cMemoryBlock.h"
#include "iostream"

namespace common {
	namespace memdatstruct {


#ifdef TEST_CORRECTNESS
void cMemoryBlock::Set_ownerThreadID(int threadID) { this->ownerThreadID = threadID; }
int cMemoryBlock::Get_ownerThreadID() { return this->ownerThreadID; }
#endif
}}
