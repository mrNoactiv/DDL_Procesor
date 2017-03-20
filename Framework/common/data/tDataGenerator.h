#ifndef __tDataGenerator_h__
#define __tDataGenerator_h__

#include "common/random/cUniformRandomGenerator.h"
#include "cDataSetDescriptor.h"
#include "common/stream/cFileStream.h"

using namespace common::random;
using namespace common::stream;

class tDataGenerator
{
public:

	tDataGenerator() {}

	//virtual int testRangeQuery();
	virtual int GetNext() = 0;
	virtual unsigned int GetUNext() = 0;
	virtual void Skip(unsigned int bytes) { UNREFERENCED_PARAMETER(bytes); }

};

class tIncrementalData: public tDataGenerator
{
protected:
	unsigned int index;

	//virtual int testRangeQuery();
	virtual int GetNext() {return ++index;};
	virtual unsigned int GetUNext() {return ++index;};
public:

	tIncrementalData():index(0) {printf("*** Test with incremental data ***\n");}

};

class tRandomData: public tDataGenerator
{
protected:
	cUniformRandomGenerator *gen;
	unsigned int count;

	//virtual int testRangeQuery();
	virtual int GetNext() {return (int)gen->GetNext() * count;};
	virtual unsigned int GetUNext() {return (unsigned int)(gen->GetNext() * count);};
public:

	tRandomData(unsigned int _count):count(_count) { gen = new cUniformRandomGenerator(50);printf("*** Test with random data ***\n");}

};

class tNormalizedRealData: public tDataGenerator
{
	cFileInputStream *mStream;
	char *read_buff;
	char *inverse_buff;

protected:
	virtual int GetNext();
	virtual unsigned int GetUNext();
	virtual void Skip(unsigned int bytes);
public:

	tNormalizedRealData(char *file_name);
	~tNormalizedRealData();

};

#endif