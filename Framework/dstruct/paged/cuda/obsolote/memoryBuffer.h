#ifndef __MemoryBuffer_h__
#define __MemoryBuffer_h__

//#include "globalDefs.h"
//#include "dataDefs.h"
//#include "singletonDestroyer.h"
#include "dstruct/paged/core/cMemoryPool.h"

class MemoryBuffer
{

friend class DataManager;

public:
	MemoryBuffer(char* pointer);
	~MemoryBuffer(void);


private:
	unsigned int currentPosition;
	unsigned int maxItemsInBuffer;
	unsigned int dimension;
	unsigned int* buffer;
	unsigned int bufferSizeInBytes;
	unsigned int currentPositionOnGPU;
	unsigned int itemsCount;
	bool Allocate(const unsigned int memorySize, const unsigned int _dimension);
	//cMemoryPool* mMemoryPool;

public:
	inline bool Allocate(const unsigned int maxItemsCount, const unsigned int dimension,const int itemSizeInBytes);
	bool Append(unsigned int* tuple);
	bool Finalize();
	unsigned int* GetItemArray();
	unsigned int GetNumberOfTuples();
	unsigned int GetNumberOfItems();
	unsigned int GetNumberOfItemsOnGPU();
	void PrintBuffer();
	bool ProvideSpecificBlock(unsigned int startItemIndex, unsigned int noItems);
	unsigned int GetBufferSizeInBytes();
	bool CanInsertTuple();
	bool IsEmpty();
	bool Flush();
	unsigned int GetActualArraySize();
	unsigned int GetCurrentPositionOnGPU();
};

bool MemoryBuffer::Allocate(const unsigned int maxItemsCount, const unsigned int _dimension,const int itemSizeInBytes)
{
	currentPosition=0;
	dimension=_dimension;
	//maxItemsInBuffer = maxItemsCount;
	buffer = new unsigned int [maxItemsCount*dimension];
	bufferSizeInBytes = maxItemsInBuffer*itemSizeInBytes;
	return true;
}
#endif;
