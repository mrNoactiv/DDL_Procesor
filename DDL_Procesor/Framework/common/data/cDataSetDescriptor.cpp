/**
*	\file cDataSetDescriptor.h
*	\author Radim Baca
*	\version 0.1
*	\date mar 2007
*	\brief Data descriptor for data files
*/

#include "cDataSetDescriptor.h"

cDataSetDescriptor::cDataSetDescriptor()
{
	mMin = NULL;
	mMax = NULL;
}

cDataSetDescriptor::cDataSetDescriptor(unsigned int dimension, cDataType *type)
{
	CreateSpaceDescriptor(dimension, type);
}

cDataSetDescriptor::cDataSetDescriptor(cTreeSpaceDescriptor &treeSpaceDescriptor)
{
	CreateSpaceDescriptor(treeSpaceDescriptor);
}

cDataSetDescriptor::~cDataSetDescriptor()
{
}

void cDataSetDescriptor::CreateSpaceDescriptor(unsigned int dimension, cDataType *type)
{
	mTreeSpaceDescriptor = new cTreeSpaceDescriptor(dimension, type, true);
	mMin = new int[dimension];
	mMax = new int[dimension];}

void cDataSetDescriptor::CreateSpaceDescriptor(cTreeSpaceDescriptor &treeSpaceDescriptor)
{
	unsigned int dimension = treeSpaceDescriptor.GetDimension();
	mTreeSpaceDescriptor = &treeSpaceDescriptor;
	mMin = new int[dimension];
	mMax = new int[dimension];

}

void cDataSetDescriptor::SetMinMaxToSpaceDescriptor(cTreeSpaceDescriptor &treeSpaceDescriptor)
{
	if (!treeSpaceDescriptor.ComputeZValues())
	{
		printf("cDataSetDescriptor::SetMinMaxToSpaceDescriptor - min max can not be set!\n");
	}
	for (unsigned int i = 0; i < treeSpaceDescriptor.GetDimension(); i++)
	{
		treeSpaceDescriptor.GetMaxValue()->SetValue(i, mMax[i]);
		treeSpaceDescriptor.GetMinValue()->SetValue(i, mMin[i]);
	}
}

bool cDataSetDescriptor::Read(cStream *stream, cDataType *type)
{
	unsigned int dimension;

	//stream->Read((char *)&mNumberOfRecords, sizeof(unsigned int));
	//stream->Read((char *)&dimension, sizeof(unsigned int));
	dimension = 2;
	mNumberOfRecords = 5678381;
	
	CreateSpaceDescriptor(dimension, type);
	
	/*for (unsigned int i = 0; i < dimension; i++)
	{
		stream->Read((char *)&mMin[i], sizeof(int));
		stream->Read((char *)&mMax[i], sizeof(int));
	}*/

	mMin[0] = 104052286;
	mMin[1] = 40994746;
	mMax[0] = 111056889;
	mMax[1] = 45005904;

	return true;
}

bool cDataSetDescriptor::Write(cStream *stream) const
{
	return true;
}
