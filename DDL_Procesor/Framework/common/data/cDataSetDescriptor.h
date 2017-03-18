/**
*	\file cDataSetDescriptor.h
*	\author Radim Baca
*	\version 0.1
*	\date mar 2007
*	\brief Data descriptor for data files
*/

#ifndef __cDataSetDescriptor_h__
#define __cDataSetDescriptor_h__

#include "common/datatype/cDataType.h"
#include "common/datatype/cBasicType.h"
#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "common/datatype/tuple/cTuple.h"

using namespace common::datatype::tuple;

/**
*	\class cDataSetDescriptor store the basic information about the data set. We use it as a header for data file
*			where the tuple information are stored
*
*	\author Radim Baca
*	\version 0.1
*	\date mar 2007
**/
class cDataSetDescriptor
{
	cSpaceDescriptor *mTreeSpaceDescriptor;
	unsigned int mNumberOfRecords;
	int *mMin;
	int *mMax;

public:
	cDataSetDescriptor();
	cDataSetDescriptor(unsigned int dimension, cDataType *type);
	cDataSetDescriptor(cSpaceDescriptor &treeSpaceDescriptor);
	~cDataSetDescriptor();

	void CreateSpaceDescriptor(unsigned int dimension, cDataType *type);
	void CreateSpaceDescriptor(cSpaceDescriptor &treeSpaceDescriptor);

	inline void SetRecordsCount(unsigned int count);
	inline unsigned int GetRecordsCount() const;
	inline void SetMin(int index, int min);
	inline int GetMin(int index) const;
	inline void SetMax(int index, int max);
	inline int GetMax(int index) const;
	inline cSpaceDescriptor *GetTreeSpaceDescriptor();
	void SetMinMaxToSpaceDescriptor(cSpaceDescriptor &treeSpaceDescriptor);

	bool Read(cStream *stream, cDataType *type);
	bool Write(cStream *stream) const;
};

inline cSpaceDescriptor *cDataSetDescriptor::GetTreeSpaceDescriptor()
{
	return mTreeSpaceDescriptor;
}

inline void cDataSetDescriptor::SetRecordsCount(unsigned int count)
{
	mNumberOfRecords = count;
}

inline unsigned int cDataSetDescriptor::GetRecordsCount() const
{
	return mNumberOfRecords;
}

inline void cDataSetDescriptor::SetMin(int index, int min)
{
	mMin[index] = min;
}

inline int cDataSetDescriptor::GetMin(int index) const
{
	return mMin[index];
}

inline void cDataSetDescriptor::SetMax(int index, int max)
{
	mMax[index] = max;
}

inline int cDataSetDescriptor::GetMax(int index) const
{
	return mMax[index];
}

#endif