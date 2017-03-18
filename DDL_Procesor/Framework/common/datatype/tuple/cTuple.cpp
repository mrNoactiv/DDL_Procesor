#include "cTuple.h"

namespace common {
	namespace datatype {
		namespace tuple {

			unsigned int cTuple::countCompare = 0; //val644 - staticka promenna na ukladani poctu porovnavani
			unsigned int cTuple::basicCountCompare; //val644 - pocet porovnani pro zakladni RQ
			unsigned int cTuple::countCompareOrder; //val644 - ulozeni poctu porovnani pri setrizeni rozsahovych dotazu
			unsigned int cTuple::countCompareLevel[24]; //val644 - staticka promenna na ukladani poctu porovnavani pro ruzne urovne stromu
			double cTuple::basicTotalReadNodes; //val644 - pocet nacteni uzlu, pri vyhledavani rozsahovych dotazu
			unsigned int cTuple::levelTree = 0; //val644 - pro ulozeni v jake urovni stromu se nachazi
			unsigned int cTuple::readNodesInLevel[24]; //val644 - promenna pro ukladani poctu nacteni uzlu pro uroven stromu
			unsigned int cTuple::itemsCountForLevel[24]; // val644 - ulozeni poctu prvku v korenovem uzlu;
			bool cTuple::typeRQordered = false; //val644 - zda jsou rozsahove dotazy setrizene
			unsigned int cTuple::typeRQ = 0; // 0 = Vzdy se nastavi na zacatek, 1 = na zacatek predesleho rozsahoveho dotazu, 2 = konec predesleho rozsahoveho dotazu (tento typ odstrani duplicity ve vysledku)
			unsigned int cTuple::callCountCompare = 0; //val644 - pocet zavolani porovnani
			unsigned int cTuple::addItemOrder; //val644 - pocet posunuti v listech
			float cTuple::indexSizeMB; //val644 - velikost BTree v MB
			unsigned int cTuple::tupleLeafCountItems; //val644 - celkovy pocet listovych uzlu v btree
			unsigned int cTuple::leafItemsCount; //val644 - celkovy pocet klicu v listech
			
/**
* Constructor
*/
cTuple::cTuple(): mData(NULL)
{
}

/**
* Constructor
*/
cTuple::cTuple(const cSpaceDescriptor *spaceDescriptor): mData(NULL)
{
	Resize(spaceDescriptor);
}

cTuple::cTuple(const cSpaceDescriptor* pSd1, const cSpaceDescriptor* pSd2) : mData(NULL)
{
	Resize(pSd1, pSd2);

}
			

cTuple::cTuple(const cDTDescriptor *dtDescriptor): mData(NULL)
{
	Resize((const cSpaceDescriptor *)dtDescriptor);
}

cTuple::cTuple(const cSpaceDescriptor *spaceDescriptor, unsigned int len): mData(NULL)
{
	Resize(spaceDescriptor);
}

cTuple::cTuple(char* buffer)
{
	mData = buffer + sizeof(cTuple);
}

/**
* Destructor
*/
cTuple::~cTuple()
{
	Free();
}

void cTuple::Free(cMemoryBlock *memBlock)
{
	if (memBlock != NULL)
	{
		mData = NULL;
	}
	else if (mData != NULL)
	{
		delete mData;
		mData = NULL;
	}
}

void cTuple::Free(cTuple &tuple, cMemoryBlock *memBlock)
{
	tuple.Free(memBlock);
}

/**
* Resize the tuple acording to space descriptor
*/
bool cTuple::Resize(const cSpaceDescriptor* pSd, cMemoryBlock *memBlock)
{
	if (mData != NULL && memBlock == NULL)
	{
		// if the MemBlock is used, probably the previous allocation has been done 
		// using the MemBlock as well, therefore it is not possible to use delete
		delete mData; 
	}

	unsigned int size = GetMaxSize(NULL, pSd);

	if (memBlock == NULL)
	{
		mData = new char[size];
	}
	else
	{
		mData = memBlock->GetMemory(size);
	}

	if (mData != NULL)
	{
		Clear(pSd);
	}
	return mData != NULL;
}

			//moje
bool cTuple::Resize(const cSpaceDescriptor* pSd1, const cSpaceDescriptor* pSd2, cMemoryBlock *memBlock)
{
	if (mData != NULL && memBlock == NULL)
	{
		// if the MemBlock is used, probably the previous allocation has been done 
		// using the MemBlock as well, therefore it is not possible to use delete
		delete mData;
	}

	unsigned int size1 = GetMaxSize(NULL, pSd1);
	unsigned int size2 = GetMaxSize(NULL, pSd2);
	unsigned int size = size1 + size2;
	if (memBlock == NULL)
	{
		mData = new char[size];
	}
	else
	{
		mData = memBlock->GetMemory(size);
	}

	if (mData != NULL)
	{
		Clear(pSd1);
		Clear(pSd2);
	}
	return mData != NULL;
}



bool cTuple::Resize(const cDTDescriptor *pSd, uint length)
{
	return Resize((cSpaceDescriptor*)pSd);
}

/**
* Resize the tuple acording to space descriptor
*/
bool cTuple::Resize(const cDTDescriptor* pSd)
{
	return Resize((cSpaceDescriptor*)pSd);
}

/**
* Resize the tuple acording to the space descriptor and set the tuple.
*/
bool cTuple::ResizeSet(cTuple &t1, const cTuple& t2, const cDTDescriptor* pDtd, cMemoryBlock* memBlock)
{
	cSpaceDescriptor *sd = (cSpaceDescriptor*)pDtd;
	bool ret;
	if ((ret = t1.Resize(sd, memBlock)))
	{
		t1.SetValue(t2, sd);
	}
	return ret;
}

/**
* Copy values from the tuple in the argument into this tuple
*/
//void cTuple::operator = (const cTuple &tuple)
//{
//	if ((unsigned int)pSd->GetDimension() < (unsigned int)tuple.GetDimension())
//	{
//		CopyMemory(mData, tuple.GetData(), pSd->GetDimension() * mTypeSize);
//	} else
//	{
//		CopyMemory(mData, tuple.GetData(), tuple.GetDimension() * mTypeSize);
//	}
//}

/**
* Inrease values in this tuple using the values in the parameter
*/
//void cTuple::operator += (const cTuple &tuple)
//{
//	assert(pSd->GetDimension() != tuple.GetDimension());
//	
//	for (unsigned int i = 0; i < (unsigned int)pSd->GetDimension(); i++)
//	{
//		switch (mTypeSize)
//		{
//		case 4:
//			SetValue(i, GetUInt(i, pSd) + tuple.GetUInt(i, pSd));
//			break;
//		case 2:
//			SetValue(i, GetUShort(i) + tuple.GetUShort(i, pSd));
//			break;
//		case 1:
//			SetValue(i, GetByte(i, pSd) + tuple.GetByte(i, pSd));
//			break;
//		}
//	}
//}

/**
* Compute euclidian distance between the tuples
*/
double cTuple::EuclidianIntDistance(const cTuple &tuple, const cSpaceDescriptor *pSd) const
{
	int tmp;
	double sum = 0;
	unsigned int dim = pSd->GetDimension();
	
	for(unsigned int i = 0; i < dim; i++)
	{
		switch (pSd->GetDimensionTypeCode(i))
		{
		case cUInt::CODE:
			tmp = GetUInt(i, pSd) - tuple.GetUInt(i, pSd);
			sum += tmp * tmp;
			break;
		case cUShort::CODE:
			tmp = GetUShort(i, pSd) - tuple.GetUShort(i, pSd);
			sum += tmp * tmp;
			break;
		case cChar::CODE:
			tmp = GetByte(i, pSd) - tuple.GetByte(i, pSd);
			sum += tmp * tmp;
			break;
		}
	}
	return sum;
}

/**
* Modify MBR according to the tuple.
* \param mbrl Lower tuple of the MBR.
* \param mbrh Higher tuple of the MBR.
* \return
*		- true if the MBR was modified,
*		- false otherwise.
*/
//bool cTuple::ModifyMbr(cTuple &mbrl, cTuple &mbrh) const
//{
//	bool modified = false;
//
//	for (unsigned int i = 0 ; i < (unsigned int)pSd->GetDimension() ; i++)
//	{
//		switch (mTypeSize)
//		{
//		case 4:
//			if (mbrl.GetUInt(i, pSd) <= mbrh.GetUInt(i, pSd))
//			{
//				if (mbrl.GetUInt(i, pSd) > GetUInt(i, pSd))
//				{
//					mbrl.SetValue(i, GetUInt(i, pSd));
//					modified = true;
//				}
//				else if (mbrh.GetUInt(i, pSd) < GetUInt(i, pSd))
//				{
//					mbrh.SetValue(i, GetUInt(i, pSd));
//					modified = true;
//				}
//			}
//			else
//			{
//				if (mbrh.GetUInt(i, pSd) > GetUInt(i, pSd))
//				{
//					mbrh.SetValue(i, GetUInt(i, pSd));
//					modified = true;
//				}
//				else if (mbrl.GetUInt(i, pSd) < GetUInt(i, pSd))
//				{
//					mbrl.SetValue(i, GetUInt(i, pSd));
//					modified = true;
//				}
//			}
//			break;
//		case 2:
//			if (mbrl.GetUShort(i) <= mbrh.GetUShort(i))
//			{
//				if (mbrl.GetUShort(i) > GetUShort(i))
//				{
//					mbrl.SetValue(i, GetUShort(i));
//					modified = true;
//				}
//				else if (mbrh.GetUShort(i) < GetUShort(i))
//				{
//					mbrh.SetValue(i, GetUShort(i));
//					modified = true;
//				}
//			}
//			else
//			{
//				if (mbrh.GetUShort(i) > GetUShort(i))
//				{
//					mbrh.SetValue(i, GetUShort(i));
//					modified = true;
//				}
//				else if (mbrl.GetUShort(i) < GetUShort(i))
//				{
//					mbrl.SetValue(i, GetUShort(i));
//					modified = true;
//				}
//			}
//			break;
//		case 1:
//			if (mbrl.GetByte(i, pSd) <= mbrh.GetByte(i, pSd))
//			{
//				if (mbrl.GetByte(i, pSd) > GetByte(i, pSd))
//				{
//					mbrl.SetValue(i, GetByte(i, pSd));
//					modified = true;
//				}
//				else if (mbrh.GetByte(i, pSd) < GetByte(i, pSd))
//				{
//					mbrh.SetValue(i, GetByte(i, pSd));
//					modified = true;
//				}
//			}
//			else
//			{
//				if (mbrh.GetByte(i, pSd) > GetByte(i, pSd))
//				{
//					mbrh.SetValue(i, GetByte(i, pSd));
//					modified = true;
//				}
//				else if (mbrl.GetByte(i, pSd) < GetByte(i, pSd))
//				{
//					mbrl.SetValue(i, GetByte(i, pSd));
//					modified = true;
//				}
//			}
//			break;
//		}
//	}
//
//	return modified;
//}

/**
* Modify MBR according to the tuple. The parameters include tuples in char* arrays.
*
* \param cTuple_t Input tuple.
* \param cTuple_ql Lower tuple of the MBR.
* \param cTuple_qh Higher tuple of the MBR.
* \param pSd SpaceDescriptor of the tuples.
* \return
*		- true if the MBR was modified,
*		- false otherwise.
*/
//bool cTuple::ModifyMbr(const char* cTuple_t, char*  cTuple_ql, char*  cTuple_qh, const cSpaceDescriptor* pSd)
//{
//	bool modified = false;
//	unsigned int dim = pSd->GetDimension();
//	char type = pSd->GetType(0)->GetCode();
//
//	for (unsigned int i = 0 ; i < dim ; i++)
//	{
//		switch (type)
//		{
//		case cUInt::CODE:
//			if (cTuple::GetUInt(cTuple_ql, i) <= cTuple::GetUInt(cTuple_qh, i))
//			{
//				if (cTuple::GetUInt(cTuple_ql, i) > cTuple::GetUInt(cTuple_t, i))
//				{
//					cTuple::SetValue(cTuple_ql, i, cTuple::GetUInt(cTuple_t, i));
//					modified = true;
//				}
//				else if (cTuple::GetUInt(cTuple_qh, i) < cTuple::GetUInt(cTuple_t, i))
//				{
//					cTuple::SetValue(cTuple_qh, i, cTuple::GetUInt(cTuple_t, i));
//					modified = true;
//				}
//			}
//			else
//			{
//				if (cTuple::GetUInt(cTuple_qh, i) > cTuple::GetUInt(cTuple_t, i))
//				{
//					cTuple::SetValue(cTuple_qh, i, cTuple::GetUInt(cTuple_t, i));
//					modified = true;
//				}
//				else if (cTuple::GetUInt(cTuple_ql, i) < cTuple::GetUInt(cTuple_t, i))
//				{
//					cTuple::SetValue(cTuple_ql, i, cTuple::GetUInt(cTuple_t, i));
//					modified = true;
//				}
//			}
//			break;
//		case cUShort::CODE:
//			if (cTuple::GetUShort(cTuple_ql, i) <= cTuple::GetUShort(cTuple_qh, i))
//			{
//				if (cTuple::GetUShort(cTuple_ql, i) > cTuple::GetUShort(cTuple_t, i))
//				{
//					cTuple::SetValue(cTuple_ql, i, cTuple::GetUShort(cTuple_t, i));
//					modified = true;
//				}
//				else if (cTuple::GetUShort(cTuple_qh, i) < cTuple::GetUShort(cTuple_t, i))
//				{
//					cTuple::SetValue(cTuple_qh, i, GetUShort(cTuple_t, i));
//					modified = true;
//				}
//			}
//			else
//			{
//				if (cTuple::GetUShort(cTuple_qh, i) > cTuple::GetUShort(cTuple_t, i))
//				{
//					cTuple::SetValue(cTuple_qh, i, cTuple::GetUShort(cTuple_t, i));
//					modified = true;
//				}
//				else if (cTuple::GetUShort(cTuple_ql, i) < cTuple::GetUShort(cTuple_t, i))
//				{
//					cTuple::SetValue(cTuple_ql, i, cTuple::GetUShort(cTuple_t, i));
//					modified = true;
//				}
//			}
//			break;
//		case cChar::CODE:
//			if (cTuple::GetByte(cTuple_ql, i) <= cTuple::GetByte(cTuple_qh, i))
//			{
//				if (cTuple::GetByte(cTuple_ql, i) > cTuple::GetByte(cTuple_t, i))
//				{
//					cTuple::SetValue(cTuple_ql, i, cTuple::GetByte(cTuple_t, i));
//					modified = true;
//				}
//				else if (cTuple::GetByte(cTuple_qh, i) < cTuple::GetByte(cTuple_t, i))
//				{
//					cTuple::SetValue(cTuple_qh, i, cTuple::GetByte(cTuple_t, i));
//					modified = true;
//				}
//			}
//			else
//			{
//				if (cTuple::GetByte(cTuple_qh, i) > cTuple::GetByte(cTuple_t, i))
//				{
//					cTuple::SetValue(cTuple_qh, i, cTuple::GetByte(cTuple_t, i));
//					modified = true;
//				}
//				else if (cTuple::GetByte(cTuple_ql, i) < cTuple::GetByte(cTuple_t, i))
//				{
//					cTuple::SetValue(cTuple_ql, i, cTuple::GetByte(cTuple_t, i));
//					modified = true;
//				}
//			}
//			break;
//		}
//	}
//
//	return modified;
//}

/**
* Equality test of order-th coordinate. 
* \return -1 if this < tuple (all coordinates of this < all values of tuple), 0 if tuples' coordinates are the same, 1 if this > tuple.
*/
//inline int cTuple::EqualMultidim(const cTuple &tuple) const
//{
//	__m128i m1, m2, m3;
//	for (int i=0 ; i < pSd->GetDimension() ; i+=4) {
//		m1 = _mm_load_ps((unsigned int*)mData + i);
//		m2 = _mm_load_ps(B+i);
//		m3 = _mm_mul_ps (m1,m2);
//		_mm_store_ps (C+i,m3);
//	}
//}

//unsigned int cTuple::Sum(const cSpaceDescriptor* pSd)
//{
//	unsigned int sum;
//	for (char i = 0 ; i < pSd->GetDimension() ; i++)
//	{
//		sum = pSd->GetDimension() + i;
//	}
//	return sum;
//}

/**
* Length of the interval of order-th coordinates this and tuple.
*/
float cTuple::UnitIntervalLength(const cTuple &tuple, unsigned int order, const cSpaceDescriptor* pSd) const
{
	assert((char)order < pSd->GetDimension());
	float interval = 0.0;

	switch (pSd->GetDimensionTypeCode(order))
	{
	case cUInt::CODE:
		if (GetUInt(order, pSd) < tuple.GetUInt(order, pSd))
		{
			interval = (float)(tuple.GetUInt(order, pSd) - GetUInt(order, pSd));
		}
		else
		{
			interval = (float)(GetUInt(order, pSd) - tuple.GetUInt(order, pSd));
		}
		interval /= 4294967295;
		break;
	case cChar::CODE:
		interval = (float)cNumber::Abs(GetByte(order, pSd) - tuple.GetByte(order, pSd))/256;
		break;
	}
	return interval;
}

/**
* Length of the interval of the order-th coordinate.
*/
float cTuple::UnitIntervalLength(const char* cTuple_t1, const char* cTuple_t2, const unsigned int order, const cSpaceDescriptor* pSd)
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

/**
* \return true if the tuple is contained into n-dimensional query block.
*/
//static bool IsInBlock(const char* cTuple_t, const cTuple &ql, const cTuple &qh, const cSpaceDescriptor* pSd)
//{
//	bool ret = true;
//	unsigned int dim = pSd->GetDimension();
//
//	for (unsigned int i = 0 ; i < dim ; i++)
//	{
//		if (!cTuple::IsInInterval(cTuple_t, ql.GetData(), qh.GetData(), i))
//		{
//			ret = false;
//			break;
//		}
//	}
//	return ret;
//}

/**
* \return true if the tuple is contained into n-dimensional query block.
*/
//bool cTuple::IsInBlock(const char* cTuple_t, const char* cTuple_ql, const char* cTuple_qh, const cSpaceDescriptor* pSd)
//{
//	bool ret = true;
//	unsigned int dim = pSd->GetDimension();
//
//	for (unsigned int i = 0 ; i < dim ; i++)
//	{
//		if (!cTuple::IsInInterval(cTuple_t, cTuple_ql, cTuple_qh, i))
//		{
//			ret = false;
//			break;
//		}
//	}
//	return ret;
//}

/**
* \return true if the tuple is contained into n-dimensional query block.
*/
//bool cTuple::IsInBlock(const cTuple &ql, const cTuple &qh) const
//{
//	bool ret = true;
//	for (unsigned int i = 0 ; i < (unsigned int)pSd->GetDimension() ; i++)
//	{
//		if (!IsInInterval(ql, qh, i))
//		{
//			ret = false;
//			break;
//		}
//	}
//	return ret;
//}

/**
* Set max values.
*/
void cTuple::SetMaxValues(const cSpaceDescriptor* pSd)
{
	unsigned int dim = pSd->GetDimension();
	for (unsigned int i = 0 ; i < dim ; i++)
	{
		SetMaxValue(i, pSd);
	}
}

/**
* \return true if the tuple coordinate is contained in the interval.
*/
//bool cTuple::IsInInterval(const char* cUnfTuple_t, const char* cUnfTuple_ql, const char* cUnfTuple_qh, unsigned int order)
//{
//	// assert(order < pSd->GetDimension());
//
//	bool ret = true;
//	int eq;
//
//	if ((eq = cTuple::Equal(cUnfTupleTuple_t, cUnfTupleTuple_ql, order)) > 0)
//	{
//		if (cTuple::Equal(cUnfTupleTuple_t, cUnfTupleTuple_qh, order) > 0)
//		{
//			ret = false;
//		}
//	}
//	else if (eq < 0)
//	{
//		if (cTuple::Equal(cUnfTupleTuple_t, cUnfTupleTuple_qh, order) < 0)
//		{
//			ret = false;
//		}
//	}
//	return ret;
//}

/**
* \return true if the tuple coordinate is contained in the interval.
*/
//bool cTuple::IsInInterval(const cTuple &ql, const cTuple &qh, unsigned int order) const
//{
//	assert((char)order < pSd->GetDimension());
//
//	bool ret = true;
//	int eq;
//
//	if ((eq = Equal(ql, order)) > 0)
//	{
//		if (Equal(qh, order) > 0)
//		{
//			ret = false;
//		}
//	}
//	else if (eq < 0)
//	{
//		if (Equal(qh, order) < 0)
//		{
//			ret = false;
//		}
//	}
//	return ret;
//}

/**
* Print this tuple
* \param string This string is printed out at the end of the tuple.
*/
void cTuple::Print(const char *delim, const cSpaceDescriptor* pSd) const
{
	printf("(");
	unsigned int dim = pSd->GetDimension();

	for (unsigned int i = 0 ; i < dim  ; i++)
	{
		Print(i, "", pSd);
		if (i != dim - 1)
		{
			printf(", ");
		}
	}
	printf(")%s", delim);
}

void cTuple::Print(const char *string, const cDTDescriptor* pSd) const
{
	Print(string, (cSpaceDescriptor*)pSd);
}

void cTuple::Print2File(FILE *streamInfo, const char *data, const char* delim, const cSpaceDescriptor* pSd)
{
	unsigned int dim = pSd->GetDimension();

	fprintf(streamInfo, "(");
	for (unsigned int i = 0 ; i < dim ; i++)
	{
		fprintf(streamInfo, "%u", cTuple::GetUInt(data, i, pSd));
		if (i != dim-1)
		{
			fprintf(streamInfo, ",");
		}
	}
	fprintf(streamInfo, ")%s", delim);
}

void cTuple::Print2File(FILE *StreamInfo, const char *data, const char* delim, const cDTDescriptor* dd)
{
	Print2File(StreamInfo, data, delim, (cSpaceDescriptor*)dd);
}

/**
* Set the tuple. The dimension and data types must be the same!
*/
void cTuple::SetValue(const cTuple &tuple, const cSpaceDescriptor* pSd)
{
	Clear(pSd);
	memcpy(mData, tuple.GetData(), pSd->GetSize());
}

/*
	val644
	Set flagRQ value.
*/

void cTuple::SetFlagRQ(uint value)
{
	flagRQ = value;
}

/**
* Print just one dimension of this tuple
* \param order Order of the dimension.
* \param string This string is printed out at the end of the tuple.
*/
void cTuple::Print(unsigned int order, const char *string, const cSpaceDescriptor* pSd) const
{
	char typeCode = pSd->GetDimensionTypeCode(order);

	if (typeCode == cChar::CODE)
	{
		printf("%X", (unsigned char)GetByte(order, pSd));
	} 
	else  if (typeCode == cInt::CODE)
	{
		printf("%d", GetInt(order, pSd));
	}
	else if (typeCode == cUInt::CODE)
	{
		printf("%u", GetUInt(order, pSd));
	}
	printf("%s", string);
}

void cTuple::ComputeHAddress(cBitString &hvalue, const cSpaceDescriptor* pSd) const
{
	hilbert_c2i(pSd->GetDimension(),32, hvalue, pSd);
}

// ------------------------------------------------------------------------------------------
// Static methods
// ------------------------------------------------------------------------------------------
/// Error: Only uint!
void cTuple::Print(const char *data, const char* delim, const cSpaceDescriptor* pSd)
{
	unsigned int dim = pSd->GetDimension();

	printf("(");
	for (unsigned int i = 0 ; i < dim ; i++)
	{
		switch(pSd->GetDimensionTypeCode(i))
		{
			case cUInt::CODE:
				printf("%u", cTuple::GetUInt(data, i, pSd));
				break;
			case cChar::CODE:
				cBitString::Print((char*)(data + sizeof(char) * i), 8);
				break;
		}

		if (i != dim-1)
		{
			printf(", ");
		}
	}
	printf(")%s", delim);
}

void cTuple::Print(const char *data, const char* delim, const cDTDescriptor* dd)
{
	Print(data, delim, (cSpaceDescriptor*)dd);
}

void cTuple::hilbert_c2i(int nDims, int nBits, cBitString &hvalue, const cSpaceDescriptor* pSd) const
{
	bitmask_t const one = 1;
	bitmask_t const ndOnes = (one << nDims) - 1;
	bitmask_t const nthbits = (((one << nDims*nBits) - one) / ndOnes) >> 1;
	int b, d;
	int rotation = 0; /* or (nBits * (nDims-1)) % nDims; */
	bitmask_t reflection = 0;
	bitmask_t index = 0;

	for (b = nBits; b--;)
	{
		bitmask_t bits = reflection;
		reflection = 0;

		for ( d = 0; d < nDims; d++)
		{
			int value = GetUInt(d, pSd);
			reflection |= ((value >> b) & 1 ) << d;
		}

		bits ^= reflection;
		bits = rotateRight(bits, rotation, nDims);
		index |= bits << nDims*b;
		reflection ^= one << rotation;

		adjust_rotation(rotation, nDims, bits);
	}

	index ^= nthbits;

	for (d = 1; ; d *= 2) 
	{
		bitmask_t t;
		if (d <= 32) 
		{
			t = index >> d;
			if (!t)
			{
				break;
			}
		}
		else 
		{
			t = index >> 32;
			t = t >> (d - 32);
			if (!t)
			{
				break;
			}
		}
		index ^= t;
	}

	int kk = sizeof(index);

	hvalue.SetInt(0, (unsigned int)index);
	hvalue.SetInt(1, (unsigned int)(index >> 32));
}

/// this - tuple operator
/*void cTuple::operator -= (const cTuple &tuple, const cSpaceDescriptor* pSd)
{
	unsigned int dim = pSd->GetDimension();

	for (unsigned int j = 0; j < dim; j++)
	{
		this->Subtract(tuple, j, pSd);
	}
}*/

/// t1 + t2 operator
char* cTuple::Add(const char* cTuple_t1, const char* cTuple_t2, char* cTuple_result, const cDTDescriptor* pSd)
{
	unsigned int dim = ((cSpaceDescriptor*)pSd)->GetDimension();

	for (unsigned int j = 0; j < dim; j++)
	{
		Add(cTuple_t1, cTuple_t2, cTuple_result, j, ((cSpaceDescriptor*)pSd));
	}

	return cTuple_result;
}

/// t1 - t2 operator
char* cTuple::Subtract(const char* cTuple_t1, const char* cTuple_t2, char* cTuple_result, const cDTDescriptor* pSd)
{
	unsigned int dim = ((cSpaceDescriptor*)pSd)->GetDimension();

	for (unsigned int j = 0; j < dim; j++)
	{
		Subtract(cTuple_t1, cTuple_t2, cTuple_result, j, ((cSpaceDescriptor*)pSd));
	}

	return cTuple_result;
}

/// Compute non euclidian distance from tuple to this tuple
double cTuple::TaxiCabDistance(const char* cTuple_t1, const char* cTuple_t2, const cDTDescriptor* pSd)
{
	double sum = 0;
	unsigned int dim = ((cSpaceDescriptor*)pSd)->GetDimension();

	for (unsigned int i = 0; i < dim; i++)
	{
		sum += TaxiCabDistanceValue(cTuple_t1, cTuple_t2, i, ((cSpaceDescriptor*)pSd));
	}

	return sum;
}

/// Compare query with prefix
/// \return true it is same in all dimensions
//bool cTuple::StartsWith(char* cNTuple_prefix, const char* cTuple_tuple, const cDTDescriptor* pSd, unsigned int prefixLength)
//{
//	const cSpaceDescriptor* spaceDescriptor = ((cSpaceDescriptor*)pSd);
//
//	for (unsigned int i = 0; i < prefixLength; i++)
//	{
//		if (Equal(cNTuple_prefix/* + SIZEPREFIX_LEN*/, cTuple_tuple, i, spaceDescriptor) != 0)
//		{
//			return false;
//		}
//	}
//
//	return true;
//
//}



///// Returns the number of 1's in the mask, if the cTuple_tuple will be added
//unsigned int cTuple::SameValues(char* cBitString_Mask, const char* cTuple_prefix, const char* cTuple_tuple, const cDTDescriptor* pSd, unsigned int sameValues)
//{
//	unsigned int dim = ((cSpaceDescriptor*)pSd)->GetDimension();
//	unsigned int length = 0;
//
//	for (unsigned int i = 0; i < dim; i++)
//	{
//		if ((cBitString::GetBit(cBitString_Mask, i) == 1) && (Equal(cTuple_prefix, cTuple_tuple, i, ((cSpaceDescriptor*)pSd)) == 0))
//			length++;
//	}
//
//	return (length == sameValues) ? sameValues : length;
//}


/// If the new prefix length is same as the old prefix length, returns length, otherwise 0
/*double cTuple::CommonPrefixLength(const char* cNTuple_prefix, const char* cTuple_tuple, const cDTDescriptor* pSd, unsigned int prefixLength)
{
	double length = 0;

	for (unsigned int i = 0; i < prefixLength; i++)
	{
		if (Equal(cNTuple_prefix + SIZEPREFIX_LEN, cTuple_tuple, i, ((cSpaceDescriptor*)pSd)) == 0)
			length++;
		else
			break;
	}

	return (length == prefixLength) ? length : 0;
}*/


/// Returns the length of new prefix after insert of specified tuple
/*double cTuple::PrefixLength(const char* cNTuple_prefix, const char* cTuple_tuple, const cDTDescriptor* pSd, unsigned int prefixLength)
{
	double length = 0;

	for (unsigned int i = 0; i < prefixLength; i++)
	{
		if (Equal(cNTuple_prefix + SIZEPREFIX_LEN, cTuple_tuple, i, ((cSpaceDescriptor*)pSd)) == 0)
			length++;
		else
			break;
	}

	return length;

}


// it creates complete minimal ri from the calculated part and first item of block
char* cTuple::CompleteMinRefItem(char* cBitString_Mask, const char* cTuple_minItem, const char* cTuple_key, char* cNTuple_partMinItem, char* cTuple_result, const cDTDescriptor* pSd)
{
	const cSpaceDescriptor* spaceDescriptor = ((cSpaceDescriptor*)pSd);
	unsigned int tupleLength = ((cSpaceDescriptor*)pSd)->GetDimension();
	unsigned int j = 0;

	for (unsigned int i = 0; i < tupleLength; i++)
	{
		if (cBitString::GetBit(cBitString_Mask, i) == 1)
		{
			SetValue(cTuple_result, i, GetUInt(cTuple_key, i, spaceDescriptor), spaceDescriptor);
		}
		else
		{
			SetValue(cTuple_result, i, GetUInt(cTuple_minItem, i, spaceDescriptor) + GetUInt(cNTuple_partMinItem + SIZEPREFIX_LEN, j++, spaceDescriptor), spaceDescriptor);
		}
	}


	return cTuple_result;
}*/

/*bool cTuple::Equal(char* cBitString_Mask1, const char* cTuple_t1, char* cBitString_Mask2, const char* cTuple_t2, const cDTDescriptor* pSd)
{
	const cSpaceDescriptor* spaceDescriptor = ((cSpaceDescriptor*) pSd);
	unsigned int tupleLength = ((cSpaceDescriptor*) pSd)->GetDimension();

	for (unsigned int i = 0; i < tupleLength; i++)
	{
		if (cBitString::GetBit(cBitString_Mask1, i) != cBitString::GetBit(cBitString_Mask2, i))
			return false;

		if ((cBitString::GetBit(cBitString_Mask1, i) == 1) && (cBitString::GetBit(cBitString_Mask2, i) == 1) && (GetUInt(cTuple_t1, i, spaceDescriptor) != GetUInt(cTuple_t2, i, spaceDescriptor)))
			return false;
	}

	return true;
}*/


}}}