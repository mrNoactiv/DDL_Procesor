#include "memoryBuffer.h"
#include "dataManager.h"
MemoryBuffer::MemoryBuffer(char* pointer)
{
	currentPosition=0;
	currentPositionOnGPU=0;
	buffer = (unsigned int*) pointer;
}

MemoryBuffer::~MemoryBuffer()
{
}

bool MemoryBuffer::Allocate(const unsigned int memorySize, const unsigned int _dimension)
{
	currentPosition=0;
	dimension=_dimension;
	bufferSizeInBytes=memorySize;
	buffer = new unsigned int [bufferSizeInBytes];
	maxItemsInBuffer = memorySize/ (sizeof(unsigned int) * _dimension);
	return true;
}
bool MemoryBuffer::Append(unsigned int* tuple)
{
	for	(int i= 0;i<  dimension;i++)
	{
		buffer[currentPosition+i] = tuple[i];
	}
	currentPosition+=dimension;
	return true;
}
bool MemoryBuffer::IsEmpty()
{
	return currentPosition == 0;
}
bool MemoryBuffer::CanInsertTuple()
{
	return currentPosition  < maxItemsInBuffer;
}
bool MemoryBuffer::Flush()
{
	currentPositionOnGPU+=currentPosition;
	currentPosition=0;
	return true;
}
unsigned int MemoryBuffer::GetActualArraySize()
{
	return currentPosition;
}
unsigned int MemoryBuffer::GetCurrentPositionOnGPU()
{
	return currentPositionOnGPU;
}

bool MemoryBuffer::Finalize()
{
	itemsCount = (int)currentPosition/dimension;
	currentPosition=0;
	return true;
}
unsigned int* MemoryBuffer::GetItemArray()
{
	return buffer;
}
unsigned int MemoryBuffer::GetNumberOfTuples()
{
	return (int)itemsCount;
}
unsigned int MemoryBuffer::GetNumberOfItems()
{
	return (int)itemsCount * dimension;
}
unsigned int MemoryBuffer::GetNumberOfItemsOnGPU()
{
	return (int)currentPositionOnGPU/dimension;
}
unsigned int MemoryBuffer::GetBufferSizeInBytes()
{
	return bufferSizeInBytes;
}

bool MemoryBuffer::ProvideSpecificBlock(unsigned int startItemIndex, unsigned int noItems)
{
	if (noItems == 0) return false;
	if (noItems > maxItemsInBuffer) return false;
	if ((startItemIndex + noItems) > maxItemsInBuffer) return false;
	
	currentPosition = startItemIndex ;
	//loadNextBlock();
}
//bool MemoryBuffer::loadNextBlock()
//{
//	long long delta = lastPosition - currentPosition;
//	firstItemIndex = currentPosition/itemSizeInBytes;
//	noItemsInBuffer = (unsigned int)MINIMUM(maxItemsInBuffer, delta / itemSizeInBytes);
//	unsigned int readItems = (unsigned int)fread(buffer, itemSizeInBytes, noItemsInBuffer, dataFile);
//	currentPosition = _ftelli64(dataFile);
//	//print();
//	return ((readItems == noItemsInBuffer)&&(readItems>0));
//}

void MemoryBuffer::PrintBuffer()
{
	for (unsigned int i=0;i<currentPosition;i++)
	{
		printf("%d,",buffer[i]);
		if (i!=0 && (i+1)%(dimension) == 0)
			printf("\n");
	}
}