#include "stdlib.h"
#include "stdio.h"


#ifdef CUDA_ENABLED
#define CUDA_CONSTANT_MEM 1
//#define CUDA_MEASURE 1
//#define RQ_SHARED_MEMORY 2

#ifndef __cGpuConst_h__
#define __cGpuConst_h__

#include "common/cCommon.h"

/*new classes*/

class cGpuMainMemoryLocation
{
public:
	enum Collect {Gpu = 1, Cpu = 2};
};
class cGpuResultStructure
{
public:
	enum Collect {BoolArray = 0, DistinctList = 1};
};

class cGpuMemoryType
{
public:
	enum Collect {Pinned = 0, ZeroCopy = 1};
};
class cGpuAlgorithm
{
public:
	enum Collect {Disabled = -1, Automatic = 0, Coprocessor_BFS = 1 /*,Coprocessor_DFS = 2*/, Gpu_BFS = 3, Gpu_BFS_35 = 4,Gpu_DFS_35 = 5, Gpu_DBFS_35 = 6  };
};
/*--------------*/

class cGPU_ALGORITHM
{
public: 
	enum Collect {OneBlockPerGpu = 1, OneBlockPerSM = 2, MaxScheduledThreadBlocksPerSM = 3, NChunksPerSM = 4};
};

class cGPU_MEMTYPE
{
public: 
	enum Collect {PINNED=0, ZEROCOPY = 1};
};


class cGpuQueryType
{
public: 
	typedef enum QueryTypes { SINGLEQUERY = 0, BATCHQUERY = 1};//same as in cRtreeConst, but NVCC does not recognize include path from base amphora folder
};
class cGpuConst
{
private:
	//static size_t mMemoryAllocationSize;
	//static unsigned int mNodeCapacity;
	//static unsigned int mCacheSize;
	inline static char* GetAlgorithmName(short type);
public:

	/*new properties*/

	static short ALGORITHM_TYPE;// = cGpuAlgorithm::Gpu_BFS_35;
	static short MEMORY_LOCATION;// = cGpuMainMemoryLocation::Gpu;
	static const short MEMORY_TYPE = cGpuMemoryType::Pinned;
	static const short RESULT_STRUCT = cGpuResultStructure::DistinctList;
	static const bool BUFFERED_COPY = false;
	static const short QUERY_TYPE = cGpuQueryType::SINGLEQUERY;


	/*old ones*/
	static const int NOT_ASSIGNED = -1;
	//static const int MAX_ITEMS = 1000000; //set size of bool* for GPU's results.
	//static unsigned int CHUNKS_PER_BLOCK;
	static const int FIXED_MEMORY_SIZE = NOT_ASSIGNED;// NOT_ASSIGNED; //if set the specified amount of GPU will be used for GPU initialization instead of determining free memory
	//static const int BATCH_VERSION = 0;// NOT_ASSIGNED; //if set the specified amount of GPU will be used for GPU initialization instead of determining free memory
	//static const bool USE_COPY_BUFFER = false;
	//static const bool TRANSFER_ALL = false;
	//static const bool TRANSFER_ALL_CPU_RESULTSET = true;
	//static const bool USE_RESULT_LIST = false;
	static const bool USE_2DKERNEL = false;
	static float SPLIT_RATIO;
	//static const int MEMORY_RESERVE = 400000000; //If all available memory is allocated, its needed to have some amount of memory in reserve for future GPU's memory allocation
	static unsigned int THREADS_PER_BLOCK;
	static unsigned int MAX_SCHEDULED_BLOCKS;
	//static float FREE_MEMORY_ALLOCATION_COEFICIENT; //CUDA memory manager allocates percentage of free memory
	static bool DEBUG_FLAG;
	//static bool SEARCH_INNER_NODES_ON_GPU;
	//static const int ALGORITHM = cGPU_ALGORITHM::OneBlockPerSM;
	//static bool UNLOCK_NODES;
	static const bool BATCH_ONLY_RELEVANT = false;
	static const bool STORE_CHILD_REFERENCES = true;
	//static const int MEMORY_TYPE = cGPU_MEMTYPE::PINNED;
	static const short NODE_HEADER = 1;
	static const short NODE_LEAF = 0;


	inline static void PrintGpuInfo();
/*static const int
	static inline void SetMemoryAllocationSize(size_t size);
	static inline size_t GetMemoryAllocationSize();
	static inline void SetNodeCapacity(unsigned int capacity);
	static inline unsigned int GetNodeCapacity();
	static inline void SetCacheSize(unsigned int size);
	static inline unsigned int GetCacheSize();
*/
};
//
//
//void cGpuConst::SetMemoryAllocationSize(size_t size)
//{
//	mMemoryAllocationSize = size;
//}
//size_t cGpuConst::GetMemoryAllocationSize()
//{
//	return mMemoryAllocationSize;
//}
//void cGpuConst::SetNodeCapacity(unsigned int capacity)
//{
//	mNodeCapacity=capacity;
//}
//unsigned int cGpuConst::GetNodeCapacity()
//{
//	return mNodeCapacity;
//}
//void cGpuConst::SetCacheSize(unsigned int size)
//{
//	mCacheSize=size;
//}
//unsigned int cGpuConst::GetCacheSize()
//{
//	return mCacheSize;
//}
void cGpuConst::PrintGpuInfo()
{
	printf("\n\n-------GPU Settings--------");
	printf("\nMain memory location:\t %s", cGpuConst::MEMORY_LOCATION == cGpuMainMemoryLocation::Cpu ? "CPU" : "GPU");
	printf("\nMemory type:\t\t %s", cGpuConst::MEMORY_TYPE == cGpuMemoryType::Pinned ? "Pinned" : "Zero copy");
	printf("\nBuffered copy:\t\t %s", cGpuConst::BUFFERED_COPY ? "Enabled" : "Disabled");
	printf("\nAlgorithm type:\t\t %s", GetAlgorithmName(cGpuConst::ALGORITHM_TYPE));
	printf("\nResultset structure:\t %s", cGpuConst::RESULT_STRUCT == cGpuResultStructure::BoolArray ? "bool array" : "distinct list of int");
	if (cGpuConst::QUERY_TYPE == cGpuQueryType::BATCHQUERY)
	{
		printf("\nBatch compare mode:\t %s", cGpuConst::BATCH_ONLY_RELEVANT ? "Only relevant" : "Robin");

	}
	printf("\n---------------------------");
}
char* cGpuConst::GetAlgorithmName(short type)
{
	switch (type)
	{
	case cGpuAlgorithm::Disabled: return "GPU Disabled";
	case cGpuAlgorithm::Automatic: return "Automatic";
	case cGpuAlgorithm::Coprocessor_BFS: return "BFS coprocessor";
	case cGpuAlgorithm::Gpu_BFS: return "BFS native";
	case cGpuAlgorithm::Gpu_BFS_35: return "BFS native (recursive)";
	case cGpuAlgorithm::Gpu_DFS_35: return "DFS native (recursive)";
	case cGpuAlgorithm::Gpu_DBFS_35: return "DBFS native (recursive)";
	default:
		return "unknown";
	}
}
#endif
#endif
