#include "cBitAddress.h"

namespace common {
	namespace datatype {
		namespace tuple {
			
/**
* Constructor
*/
cBitAddress::cBitAddress(): mData(NULL)
{
}

/**
* Constructor
*/
cBitAddress::cBitAddress(const cSpaceDescriptor *spaceDescriptor): mData(NULL)
{
	// Resize(spaceDescriptor);
}

cBitAddress::cBitAddress(char* buffer)
{
	mData = buffer + sizeof(cBitAddress);
}

/**
* Destructor
*/
cBitAddress::~cBitAddress()
{
	Free();
}

void cBitAddress::Free(cMemoryBlock *memBlock)
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


/******************************************************************************/
void cBitAddress::TupleToBitAddress(cSpaceDescriptor *pSd, const cTuple &t, char* adress)
{
	// char* bVal = new char;
	char bVal; // new
	char* pBVal = &bVal;
	int maxSize = pSd->GetSize();
	int dim = pSd->GetDimension();
	bool bl = 0; 
	int pos = 0;
	int count = (maxSize/dim)*4; // new

	for(int a = 0; a < count; a++) 
	{
		for(int b=0; b < dim; b++)
		{
				
			// bVal = t.GetPByte(b, pSd);
			bVal = t.GetByte(b, pSd); // new

			bl = cBitString::GetBit(pBVal, a);
			//printf("%d  Dimenze: %d , bit: %d  pos: %d \n",bl,a,b, pos);
			cBitString::SetBit(adress, pos, bl);
			pos++;
		}
	}	
}

void cBitAddress::BitAddressToTuple(cSpaceDescriptor *pSd, char* addr, cTuple &tuple)
{
	int dim = pSd->GetDimension();
	int dimSize = pSd->GetDimensionSize(1);
	int pos = 0;
	bool bl = 0; 
	int vyp;
	int count = dimSize*4; // new

	for(int b=0; b < dim; b++)
	{
		pos = 0;
		uint value = 0;
		char* pValue = (char*)(&value);

		for(int a = 0; a < count; a++)
		{
			vyp = b + dim*a;
			bl = cBitString::GetBit(addr, vyp);
			//printf("%d dimenze:  vypocet: %d  pozice %d\n",bl,vyp,pos);
			cBitString::SetBit(pValue, pos, bl);
			pos++;
		}	
		//printf("!!!!!! dimenze: %d  hodnota: %d  \n",b,value);
		tuple.SetValue(b, value, pSd);
	}
}

/********************************************************************************/

//void cBitAddress::Free(cBitAddress &tuple, cMemoryBlock *memBlock)
//{
//	tuple.Free(memBlock);
//}

/**
* Resize the tuple acording to space descriptor
*/
//bool cBitAddress::Resize(const cSpaceDescriptor* pSd, cMemoryBlock *memBlock)
//{
//	if (mData != NULL && memBlock != NULL)
//	{
//		delete mData;
//	}
//	unsigned int size = GetMaxSize(NULL, pSd);
//
//	if (memBlock == NULL)
//	{
//		mData = new char[size];
//	}
//	else
//	{
//		mData = memBlock->GetMemory(size);
//	}
//
//	if (mData != NULL)
//	{
//		Clear(pSd);
//	}
//	return mData != NULL;
//}
//
//bool cBitAddress::Resize(const cDTDescriptor *pSd, uint length)
//{
//	return Resize((cSpaceDescriptor*)pSd);
//}
//
///**
//* Resize the tuple acording to space descriptor
//*/
//bool cBitAddress::Resize(const cDTDescriptor* pSd)
//{
//	return Resize((cSpaceDescriptor*)pSd);
//}
//
///**
//* Resize the tuple acording to the space descriptor and set the tuple.
//*/
//bool cBitAddress::ResizeSet(cBitAddress &t1, const cBitAddress& t2, const cDTDescriptor* pDtd, cMemoryBlock* memBlock)
//{
//	cSpaceDescriptor *sd = (cSpaceDescriptor*)pDtd;
//	bool ret;
//	if ((ret = t1.Resize(sd, memBlock)))
//	{
//		t1.SetValue(t2, sd);
//	}
//	return ret;
//}

}}}