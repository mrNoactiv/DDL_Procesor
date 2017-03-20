#include "fileManager.h"
#include "dataManager.h"

FileManager::FileManager()
{
	bufferSizeInBytes = 0;
	maxItemsInBuffer = 0;
	noItemsInBuffer = 0;

	currentPosition = 0;
	lastPosition = 0;
	firstItemIndex = 0;
	lastPossibleItemIndex = 0;

	itemDimension = 1;
	itemSizeInBytes = 0;

	buffer = 0;
	dataFile = 0;
}

FileManager::~FileManager(void)
{
	closeFile();
	freeBuffer();
}


void FileManager::appendBuffer(char* _buffer, const unsigned int _bufferSizeInBytes, const bool deleteOld)
{	
	if ((buffer != 0)&&(deleteOld))
	{
		delete [] buffer;
		buffer = 0;
	}
	buffer = _buffer;
	bufferSizeInBytes = bufferSizeInBytes;
}

bool FileManager::allocateBuffer(const unsigned int bufferLimitSizeInBytes, const unsigned int _itemSizeInBytes)
{
	if (_itemSizeInBytes > bufferLimitSizeInBytes)
		return false;

	itemSizeInBytes = _itemSizeInBytes;

	if (dataFile != 0)   // ... just to read binary data from input file
	{
		if (readBinary)
		{
			_fseeki64(dataFile, 0L, SEEK_END);
			lastPosition = _ftelli64(dataFile);
			lastPossibleItemIndex = lastPosition / itemSizeInBytes;
		}
		else  //TODO
		{
		}
		resetPositions();
	}
	else
		return false;

	maxItemsInBuffer = MINIMUM(bufferLimitSizeInBytes / itemSizeInBytes, lastPossibleItemIndex);
	bufferSizeInBytes = maxItemsInBuffer*itemSizeInBytes;
	buffer = new char[bufferSizeInBytes];
	return true;
}

void FileManager::freeBuffer()
{
	if (buffer != 0)
	{
		delete [] buffer;
		buffer = 0;
	}
}

bool FileManager::openFile(char *fileName, const unsigned int _itemDimension, const bool _readBinary)
{
	readBinary = _readBinary;
	itemDimension = _itemDimension;
	if (readBinary)
		dataFile = fopen(fileName, "rb");
	else
		dataFile = fopen(fileName, "rt");
	return (dataFile != 0);
}

void FileManager::closeFile(void)
{
	if (dataFile != 0)
	{
		fclose(dataFile);
		dataFile = 0;
	}
}

bool FileManager::loadNextBlock()
{
	long long delta = lastPosition - currentPosition;
	firstItemIndex = currentPosition/itemSizeInBytes;
	noItemsInBuffer = (unsigned int)MINIMUM(maxItemsInBuffer, delta / itemSizeInBytes);
	unsigned int readItems = (unsigned int)fread(buffer, itemSizeInBytes, noItemsInBuffer, dataFile);
	currentPosition = _ftelli64(dataFile);
	//print();
	return ((readItems == noItemsInBuffer)&&(readItems>0));
}

//Dodelat - toto neni zrovna moc inteligentni.
//Nektera data uz mohou but v bufferu.
//Neni zapotrebi delat cely load bloku, kdyz se chce nahrat jen noItems, atd.
bool FileManager::provideSpecificBlock(unsigned int startItemIndex, unsigned int noItems)
{
	if (noItems == 0) return false;
	if (noItems > maxItemsInBuffer) return false;
	if ((startItemIndex + noItems) > lastPossibleItemIndex) return false;
	
	currentPosition = startItemIndex * itemSizeInBytes;
	_fseeki64(dataFile, currentPosition, SEEK_SET);	
	loadNextBlock();
}


void FileManager::resetPositions(void)
{
	noItemsInBuffer = 0;
	currentPosition = 0;
	firstItemIndex = 0;
	_fseeki64(dataFile, 0L, SEEK_SET);
}

void FileManager::print(void) const
{
	printf("\nREADING INPUT DATA ------------------------------------------------\n");
	printf("DataFile: %s\n", (dataFile == 0) ? "not set" : "set");
	printf("BufferSizeInBytes: %u\n", bufferSizeInBytes);
	printf("MaxItemsInBuffer: %u\n", maxItemsInBuffer);
	printf("NoItemsInBuffer: %u\n", noItemsInBuffer);
	printf("ItemDimension: %u\n", itemDimension);
	printf("ItemSizeInBytes: %u\n", itemSizeInBytes);
	printf("LastPosition: %llu\n", lastPosition);
	printf("CurrentPosition: %llu\n", currentPosition);
	printf("-------------------------------------------------------------------\n");
}

unsigned int FileManager::getNumberOfItems() const
{
	long long noRecords = lastPosition / itemSizeInBytes;
	return (unsigned int)noRecords;
}