#include "dstruct/mmemory/dom/extendedDom/type/cXDGKeyType.h"

cXDGKeyType::Type cXDGKeyType::MAX;

void cXDGKeyType::SetMax(cTreeSpaceDescriptor *sd)
{
	MAX.Resize(sd);
	MAX.SetMaxValues();
}

void cXDGKeyType::FreeMax()
{
	MAX.Free();
}
