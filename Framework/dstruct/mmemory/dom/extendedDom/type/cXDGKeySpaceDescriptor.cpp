#include "dstruct/mmemory/dom/extendedDom/type/cXDGKeySpaceDescriptor.h"

/// Constructor
cXDGKeySpaceDescriptor::cXDGKeySpaceDescriptor()
{
	Init();
}

cXDGKeySpaceDescriptor::cXDGKeySpaceDescriptor(const cXDGKeySpaceDescriptor &sd)
{
	Init(sd);
}

/// Destructor
cXDGKeySpaceDescriptor::~cXDGKeySpaceDescriptor()
{
}

/// Initialize the object.
void cXDGKeySpaceDescriptor::Init()
{
	Create(DIMENSION, new cUIntType());
	ComputeIndexes();
}

/// Initialize the object.
void cXDGKeySpaceDescriptor::Init(const cXDGKeySpaceDescriptor &sd)
{
	Create(sd.GetDimension(), new cUIntType());
	ComputeIndexes();
}

/// Print
void cXDGKeySpaceDescriptor::Print(char *string) const
{
}

