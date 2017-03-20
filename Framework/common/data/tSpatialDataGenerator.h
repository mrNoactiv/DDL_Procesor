#ifndef __tSpatialDataGenerator_h__
#define __tSpatialDataGenerator_h__

#include "cStream.h"
#include "cUniformRandomGenerator.h"
#include "cDataSetDescriptor.h"
#include "tDataGenerator.h"

class tRealData: public tDataGenerator
{
	cStream *mStream;
	char *read_buff;
	cDataSetDescriptor *mDataSetDescriptor;

protected:
	virtual int GetNext();
	virtual unsigned int GetUNext();	
public:
	
	inline cDataSetDescriptor *GetDataSetDescriptor();
	void Close();

	tRealData(const char *file_name, cDataType *type);
	~tRealData();
};

inline cDataSetDescriptor *tRealData::GetDataSetDescriptor()
{
	return mDataSetDescriptor;
}

#endif