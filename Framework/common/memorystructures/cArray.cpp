
#include "common/memorystructures/cArray.h"


int compare3(const void *a, const void *b)
{
	unsigned int *arr1 = (unsigned int*)a;
	unsigned int *arr2 = (unsigned int*)b;
	return *arr1 - *arr2;
}
