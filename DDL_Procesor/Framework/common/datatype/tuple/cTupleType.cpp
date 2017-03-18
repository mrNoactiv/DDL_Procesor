#include "cTupleType.h"

namespace common {
	namespace datatype {
		namespace tuple {

cTupleType::Type cTupleType::MAX;

void cTupleType::SetMax(cTreeSpaceDescriptor *sd)
{
	MAX.Resize(sd);
	MAX.SetMaxValues();
}

void cTupleType::FreeMax()
{
	MAX.Free();
}
}}}