#include "cSignatureKey.h"

namespace dstruct {
	namespace paged {
		namespace rtree {

cSignatureKey::cSignatureKey(char* mem, cSpaceDescriptor* pKeySD)
{
	mem += sizeof(cSignatureKey);
	mKey = new(mem) cTuple(mem);
	mem += cTuple::GetObjectSize(pKeySD);
	mData = new(mem) cTuple(mem);
}

}}}