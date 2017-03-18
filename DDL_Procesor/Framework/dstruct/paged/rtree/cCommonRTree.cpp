#include "dstruct/paged/rtree/cCommonRTree.h"

namespace dstruct {
	namespace paged {
		namespace rtree {

int compare(const void * a, const void * b)
{
	uint** arr1 = (uint**)a;
	uint** arr2 = (uint**)b;
	return (*arr1)[0] - (*arr2)[0];
}

int compare2(const void * a, const void * b)
{
	cArray<uint>** arr1 = (cArray<uint>**)a;
	cArray<uint>** arr2 = (cArray<uint>**)b;
	return (*arr1)->GetRefItem(0) - (*arr2)->GetRefItem(0);
}

}}}