#include "cHNTuple.h"
#include "math.h"
#include "common/cNumber.h"

namespace common {
	namespace datatype {
		namespace tuple {
			char * cHNTuple::GetData() const
			{
				return mData;
			}

/**
* Resize the tuple acording to space descriptor. Size is get from the space descritor
*/
void cHNTuple::Resize(const cSpaceDescriptor* pSd)
{
	if (mData != NULL)
	{
		delete mData;
	}

	unsigned int len = cHNTuple::GetMaxSize(NULL, pSd);
	//unsigned int len = pSd->GetByteSize() + SIZEPREFIX_LEN;  //!!!fk ETIOPIE
	mData = new char[len];	
	mData[0] = ((cSpaceDescriptor*)pSd)->GetDimension();  //nastaveni dimenze, musi byt!!!
	// mk! Clear((cSpaceDescriptor*)pSd);
}

/**
* Print this tuple
* \param delim This string is printed out at the end of the tuple.
*/
void cHNTuple::Print(const char *delim, const cSpaceDescriptor* pSd) const
{
	Print(mData, delim, pSd);
}

/**
* Print just one dimension of this tuple
* \param order Order of the dimension.
* \param string This string is printed out at the end of the tuple.
*/
void cHNTuple::Print(unsigned int order, char *string, const cSpaceDescriptor* pSd) const
{
	//fk tohle je blbost
	Print(mData, string, pSd);
}

/*static*/ void cHNTuple::Print(const char *data, const char* delim, const cSpaceDescriptor* pSd)
{
	unsigned int dimension = (unsigned int)(*data);

	printf("(");
	for (unsigned int order=0; order<dimension; order++)
	{
		char typeCode = pSd->GetDimensionTypeCode(order);

		if (typeCode == cChar::CODE)
		{
			printf("%X", (unsigned char) GetByte(data, order, pSd));
		} 
		else if (typeCode == cInt::CODE)
		{
			printf("%i", GetInt(data, order, pSd));
		}	
		else if (typeCode == cUInt::CODE)
		{
			printf("%u", GetUInt(data, order, pSd));
		}
		else if (typeCode == cNTuple::CODE)
		{
			//!!!!!!!!!! fk 2012-09-24
			cNTuple::Print(GetPValue(data, order, pSd), "", pSd->GetDimSpaceDescriptor(order));
		}
		if (order != dimension - 1)
		{
			printf(", ");
		}
	}
	printf(")%s", delim);
}

void cHNTuple::Print(const char * data, const char * delim, const cDTDescriptor * pSd)
{
	Print(data, delim, (cSpaceDescriptor*)pSd);
}

void cHNTuple::PrintPom(const char *data)
{
	for (unsigned int i=0; i<30; i++)
	{
		printf("\n%d: ",i);
		for (unsigned int j=0; j<8; j++)
		{
			printf("%d", (data[i] >> (7-j))&1);
		}

	}
	return;
}

/*static*/ float cHNTuple::UnitIntervalLength(const char* cTuple_t1, const char* cTuple_t2, const unsigned int order, const cSpaceDescriptor* pSd)
{
	// assert(order < pSd->GetDimension());
	float interval = 0.0;

	switch (pSd->GetDimensionTypeCode(order))
	{
	case cUInt::CODE:
		if (GetUInt(cTuple_t1, order, pSd) < GetUInt(cTuple_t2, order, pSd))
		{
			interval = (float)(GetUInt(cTuple_t2, order, pSd) - GetUInt(cTuple_t1, order, pSd));
		}
		else
		{
			interval = (float)(GetUInt(cTuple_t1, order, pSd) - GetUInt(cTuple_t2, order, pSd));
		}
		interval /= cUInt::MAX;
		break;
	case 1:
		interval = (float)cNumber::Abs(GetByte(cTuple_t1, order, pSd) - GetByte(cTuple_t2, order, pSd)) / cChar::MAX;
		break;
	}
	return interval;
}

}}}