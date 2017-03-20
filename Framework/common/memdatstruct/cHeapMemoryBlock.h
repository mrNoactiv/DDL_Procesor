#pragma once

namespace common {
	namespace memdatstruct {

class cHeapMemoryBlock
{
public:
	cHeapMemoryBlock();
	~cHeapMemoryBlock(void);
	
	void Init(unsigned int size);

	int GetSize(void);
	char * GetBlock_memory(void);

private:

	char * block_memory;
	unsigned int size;

};
}}

