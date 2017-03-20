#pragma once
#include "globalDefs.h"
#include "dataDefs.h"
#include "singletonDestroyer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	FileManager.</summary>
///
/// <remarks>	</remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
class FileManager
{

friend class DataManager;

public:
	FileManager();
	FileManager(const FileManager&);
	~FileManager(void);

	FileManager& operator= (const FileManager&);

private:
	FILE * dataFile;
	bool readBinary;
	char *buffer;
	unsigned int bufferSizeInBytes;

	unsigned int maxItemsInBuffer;	//maximum items that can be stored in the buffer
	unsigned int noItemsInBuffer;	//current number of items in the buffer
	unsigned int itemDimension;		//dimension of an item
	unsigned int itemSizeInBytes;	//total size of an item

	long long currentPosition;		//current position in the dataFile
	long long lastPosition;			//last position in the dataFile
	unsigned int firstItemIndex;	//The index of the fist item currently stored in the buffer
	unsigned int lastPossibleItemIndex;	 //The last possible item index;

public:
	bool openFile(char* fileName, const unsigned int _itemDimension, const bool _readBinary=true);
	void closeFile(void);

	void appendBuffer(char* , const unsigned int, const bool);
	bool allocateBuffer(const unsigned int bufferLimitSizeInBytes, const unsigned int itemSizeInBytes);
	void freeBuffer();

	bool loadNextBlock();
	bool provideSpecificBlock(unsigned int startItemIndex, unsigned int noItems);
	void resetPositions();
	void print() const;

	unsigned int getNumberOfItems() const;
	inline unsigned int getMaxItemsInBuffer() const { return maxItemsInBuffer;	}
	unsigned int getBufferSizeInBytes() const { return bufferSizeInBytes;}
};