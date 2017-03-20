#include "dstruct/mmemory/dom/extendedDom/type/cXDGKey.h"

/// Constructor
cXDGKey::cXDGKey()
{
}

/// Constructor
cXDGKey::cXDGKey(cTreeSpaceDescriptor *mTreeSpaceDescr):cTreeTuple(mTreeSpaceDescr)
{
}

/// Print DeweyId in hexadecimal format
void cXDGKey::Print(const char *string) const
{

	printf("%d-%d", GetKey(), GetOrder());
	if (HasNext())
	{
		printf("*%s", string);
	} else
	{
		printf("%s", string);
	}
}

