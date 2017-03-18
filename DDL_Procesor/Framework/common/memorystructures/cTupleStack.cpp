#include "cTupleStack.h"

/// Constructor
cTupleStack::cTupleStack(const int size):cStack(size), mSpaceDescriptor(NULL)
{
}

/// Constructor
cTupleStack::cTupleStack(const cTreeSpaceDescriptor &sd, const int size): cStack(size)
{
	mSpaceDescriptor = new cTreeSpaceDescriptor(sd);
	Resize();
}

/// Destructor
cTupleStack::~cTupleStack()
{
	if (mSpaceDescriptor != NULL)
	{
		delete mSpaceDescriptor;
	}
}

/// Create space descriptor for tuples in stack. Delete old if there is any.
/// \param dimension Dimension of tuples
/// \param bitLenghtCard Size of one dimension in bites
void cTupleStack::CreateSpaceDescriptor(unsigned int dimension, cDataType *type)
{ 
	if (mSpaceDescriptor != NULL)
	{
		delete mSpaceDescriptor;
	}
	mSpaceDescriptor = new cTreeSpaceDescriptor(dimension, type);
	Resize();
}

/// Create space descriptor for the tuples in stack. Delete old if there is any.
/// \param sd Space descriptor from which the copy is made
void cTupleStack::CreateSpaceDescriptor(const cTreeSpaceDescriptor &sd)
{
	if (mSpaceDescriptor != NULL)
	{
		delete mSpaceDescriptor;
	}
	mSpaceDescriptor = new cTreeSpaceDescriptor(sd);
	Resize();
}
