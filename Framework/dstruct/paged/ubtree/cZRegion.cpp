#include "cZRegion.h"

namespace common {
	namespace datatype {
		namespace tuple {

			
/**
* Constructor
*/
cZRegion::cZRegion(): mData(NULL)
{
}

/**
* Constructor
*/
cZRegion::cZRegion(char* low, char* high): mData(NULL)
{
}


/**
* Destructor
*/
cZRegion::~cZRegion()
{
	Free();
}

void cZRegion::Free(cMemoryBlock *memBlock)
{
	if (memBlock != NULL)
	{
		mData = NULL;
	}
	else if (mData != NULL)
	{
		delete mData;
		mData = NULL;
	}
}



}}}