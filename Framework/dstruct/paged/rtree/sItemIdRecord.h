#ifndef __sItemIdRecord_h__
#define __sItemIdRecord_h__

namespace dstruct {
	namespace paged {
		namespace rtree {

struct sItemIdRecord
{
	tNodeIndex NodeIndex;
	unsigned short ItemOrder;
	unsigned short Level;
	unsigned short ParentItemOrder;
};

}}}
#endif