#ifdef CUDA_ENABLED

#ifndef __cCudaParams_h__
#define __cCudaParams_h__
	
typedef enum GPU_ALGORITHM_TYPE { SINGLE_BLOCK = 1, ONE_TB_ONE_BLOCK = 2, ONE_TB_MULTIPLE_BLOCKS = 3};
typedef enum SEARCH_BLOCK_TYPE { LEAF = 0, INNER = 1};

struct cCudaParams
{
public:
	unsigned int ThreadsPerBlock;
	unsigned int NoBlocks;
	unsigned int NoChunks;
	bool DebugFlag;
	unsigned int NodeCapacity; //Maximum number of tuples in node.
	unsigned int NodeTypes; 
	unsigned short Dimension;
	unsigned int TBCount;
	unsigned int QueriesInBatch;
	unsigned int ResultRowSize;
	unsigned int Mode;
	unsigned int BlockSize;
	cudaDeviceProp DeviceProperties;
	unsigned int* D_Inputs;			//Array of input data.
	unsigned int* D_SearchOffsets;
	bool* D_RelevantQueries;
	unsigned int* D_ChildIndices;
	unsigned int* D_ResultList;
	int NodeType;
	bool* D_Results;			//Returning bool array representing each compared tuple.
	
	//Sequential search
	bool SequencialSearch; //Searches all items in D_Inputs, D_ItemOrders does not matter.
	unsigned int SequencialNoChunks;
	unsigned int SequencialNoInputs;
};

struct cCudaParamsBasic
{
public:
	unsigned int NodeCapacity; //Maximum number of tuples in node.
	unsigned short Dimension;
	unsigned int BlockSize;
	unsigned int* D_Inputs;			//Array of input data.
	unsigned int* D_SearchOffsets;
	unsigned int* D_ChildIndices;
};

struct cCudaParamsBasic_List :cCudaParamsBasic
{
	unsigned int* D_ResultList;
	unsigned int* D_Qls;
	unsigned int* D_Qhs;
	unsigned int NoLevels;
	unsigned int QueriesInBatch;
};

struct cCudaParamsRecursive
{
	unsigned int NodeCapacity; //Maximum number of tuples in node.
	unsigned short Dimension;
	unsigned int BlockSize;
	unsigned int* D_Inputs;			//Array of input data.
	unsigned int* D_ResultList;
	unsigned int NoLevels;
	unsigned int QueriesInBatch;
	unsigned int* D_ChildIndices;
};

#endif
#endif
