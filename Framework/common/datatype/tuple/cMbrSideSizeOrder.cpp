#include "common/datatype/tuple/cMbrSideSizeOrder.h"

namespace common {
	namespace datatype {
		namespace tuple {

int compare4(const void *a, const void *b)
{
	unsigned int *arr1 = (unsigned int*)a;
	unsigned int *arr2 = (unsigned int*)b;
	return *arr2 - *arr1;
}

}}}