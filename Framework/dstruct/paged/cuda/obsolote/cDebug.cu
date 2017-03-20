#ifdef CUDA_ENABLED
#include "cDebug.h"

__device__  void Debug_PrintTuple(DATATYPE* inputs, unsigned int dim,char* delim)
{
	if (threadIdx.x ==0 && blockIdx.x==0)
	{
		printf("(");
		for (unsigned int d= 0; d < dim;d++)
		{
			printf("%d",inputs[d]);
			if (d< dim-1)
				printf(",");
		}
		printf(")");
		printf("%s",delim);
	}
}

__global__ void Debug_CheckResultVector(bool* outputs, unsigned int vectorSize)
{
	unsigned int resultSize = 0;
	if (threadIdx.x ==0)
	{
		for (unsigned int j= 0; j < vectorSize;j++)
		{
			if (outputs[j])
				resultSize++;
		}
		printf("\nResultsize on GPU: %d;", resultSize);
	}
}
__global__ void Debug_PrintMemory(const DATATYPE* inputs,const unsigned int* memOffset, const unsigned int noInputs, const unsigned int noChunks, const RQElement* query,  bool* outputs,unsigned int dim)
{

	//thread 0 prints global memory
	if (threadIdx.x ==0)
	{
		printf("\n");
		for (int i=0;i < noInputs;i++)
		{
			if (i% 11 == 0)
				printf("\n");
			printf("%d,",inputs[i]);
		}
	}
	printf("\n\nhotovo\n\n");
}
__global__ void Debug_PrintLeafNodes(DATATYPE* g_inputs,unsigned int blocksCount, unsigned int* offsets,unsigned int* offsetSizes,unsigned int dim)
{
	unsigned int offsetSize;
	unsigned int offset; 
	unsigned int noInputsInChunk;
	DATATYPE* inputs;

	if (threadIdx.x == 0 && blockIdx.x==0)
	{
		for (unsigned int b = 0; b < blocksCount;b++)
		{
			if (true)
			{
				offsetSize = offsetSizes[b];
				offset = offsets[b];
				inputs = g_inputs+offset;
				noInputsInChunk = offsetSize / sizeof(DATATYPE)/dim;
				for (unsigned int t = 0; t < noInputsInChunk;t++)
				{
					for (unsigned int d = 0; d < dim;d++)
					{
						printf("%d,", inputs[d+t*dim]);
					}
					printf("\n");
				}
				printf("\n");
			}
		}
	}
}
__global__ void Debug_PrintLeafNode(DATATYPE* D_globalMem,unsigned int* D_offsets,short* D_itemsCount, unsigned int itemOrder,unsigned int dim)
{
	unsigned int noInputsInChunk;
	DATATYPE* inputs;

	if (threadIdx.x == 0 && blockIdx.x==0)
	{
		if (true)
		{
			inputs = D_globalMem+D_offsets[itemOrder];
			noInputsInChunk = D_itemsCount[itemOrder];
			for (unsigned int t = 0; t < noInputsInChunk;t++)
			{
				for (unsigned int d = 0; d <dim;d++)
				{
					printf("%d,", inputs[d+t*dim]);
				}
				printf("\n");
			}
			printf("\n");
		}
	}
}
__global__ void Debug_PrintTupleArray(unsigned int* inputs,unsigned int noInputsInChunk, unsigned int dim)
{

	if (threadIdx.x == 0 && blockIdx.x==0)
	{
		for (unsigned int t = 0; t < noInputsInChunk;t++)
		{
			for (unsigned int d = 0; d <dim;d++)
			{
				printf("%d,", inputs[d+t*dim]);
			}
			printf("\n");
		}
		printf("\n");
	}
}
__global__ void Debug_PrintInnerNode(DATATYPE* g_inputs,unsigned int* offsets, short* itemsCount ,unsigned int itemOrder,unsigned int dim)
{
	unsigned int noInputsInChunk;
	DATATYPE* loInputs;
	DATATYPE* hiInputs;


	if (threadIdx.x == 0 && blockIdx.x==0)
	{
		if (true)
		{
			loInputs = g_inputs+offsets[itemOrder];
			hiInputs = g_inputs+offsets[itemOrder]+(itemsCount[itemOrder]*dim);
			noInputsInChunk = itemsCount[itemOrder];
			for (unsigned int t = 0; t < noInputsInChunk;t++)
			{
				Debug_PrintTuple(&loInputs[t*dim],dim," : ");
				Debug_PrintTuple(&hiInputs[t*dim],dim,"\n");
			}
		}
	}
}
extern __device__ void Device_PrintInnerNode(DATATYPE* g_inputs,unsigned int loOffset,unsigned int hiOffset,unsigned int offsetSize,unsigned int dim)
{
	unsigned int noInputsInChunk;
	DATATYPE* loInputs;
	DATATYPE* hiInputs;

	if (threadIdx.x == 0 && blockIdx.x==0)
	{
		printf("\n\nPrinting GPU Innner Node memory\n");
		if (true)
		{
			loInputs = g_inputs+loOffset;
			hiInputs = g_inputs+hiOffset;
			noInputsInChunk = offsetSize / sizeof(DATATYPE)/ dim;
			for (unsigned int t = 0; t < noInputsInChunk;t++)
			{
				Debug_PrintTuple(&loInputs[t*dim],dim," : ");
				Debug_PrintTuple(&hiInputs[t*dim],dim,"\n");
			}
		}
	}
}
__global__ void Debug_PrintArray(unsigned int *source,unsigned int size,unsigned int itemCount)
{
	if (threadIdx.x == 0 && blockIdx.x==0)
	{
		unsigned int position = 0;
		for (unsigned int t = 0; t < itemCount;t++)
		{
			printf("\n%d", source + position);
			position += size;
		}
	}
}

__global__ void Debug_PrintMemory(DATATYPE* D_Inputs,unsigned int* D_Offsets,bool* D_NodeTypes,short* D_ItemsCount, unsigned int itemsCount,unsigned int dim)
{
	DATATYPE* inputs;
	//DATATYPE* loInputs;
	DATATYPE* hiInputs;
	bool nodeType;
	if (threadIdx.x == 0 && blockIdx.x==0)
	{
		for (unsigned int i = 0; i < itemsCount ;i++)
		{
			inputs = D_Inputs+D_Offsets[i];
			nodeType = D_NodeTypes[i];
			if (nodeType == 0)
			{
				printf("\n---GPU BLOCK: %d (LEAF)---\n",i);
				for (unsigned int t = 0; t < D_ItemsCount[i];t++)
				{
					for (unsigned int d = 0; d < dim;d++)
					{
						printf("%d,", inputs[d+t * dim]);
					}
					printf("\n");
				}
			}
			else
			{
				printf("\n---GPU BLOCK: %d (INNER)---\n",i);
				hiInputs = inputs + D_ItemsCount[i] * sizeof(DATATYPE) ;
				for (unsigned int t = 0; t <  D_ItemsCount[i];t++)
				{
					Debug_PrintTuple(&inputs[t*dim],dim," : ");
					Debug_PrintTuple(&hiInputs[t*dim],dim,"\n");
				}
			}
			printf("\n");
		}
	}
}
__device__ void Device_PrintArray(unsigned int *source,unsigned int size,unsigned int itemCount)
{
	if (threadIdx.x == 0 && blockIdx.x==0)
	{
		unsigned int position = 0;
		for (unsigned int t = 0; t < itemCount;t++)
		{
			printf("\n%d", source + position);
			position += size;
		}
	}
}
__device__ unsigned int get_smid(void) {

	unsigned int ret;

	asm("mov.u32 %0, %smid;" : "=r"(ret) );

	return ret;
}

extern "C" void printLeafNodesGpu(unsigned int *D_globalMem, unsigned int blocksCount,unsigned int* offsets, unsigned int* offsetSizes,unsigned int dim)
{
	Debug_PrintLeafNodes<<<1,1>>>(D_globalMem,blocksCount,offsets,offsetSizes,dim);
	cudaThreadSynchronize();
}
extern "C" void printLeafNodeGpu(DATATYPE* D_globalMem,unsigned int* D_offsets,short* D_itemsCount, unsigned int itemsOrder,unsigned int dim)
{
	Debug_PrintLeafNode<<<1,1>>>(D_globalMem,D_offsets,D_itemsCount,itemsOrder,dim);
	cudaThreadSynchronize();
}
extern "C" void printInnerNodeGpu(DATATYPE* D_globalMem,unsigned int* D_offsets,short* D_itemsCount, unsigned int itemsOrder,unsigned int dim)
{
	Debug_PrintInnerNode<<<1,1>>>(D_globalMem,D_offsets,D_itemsCount,itemsOrder,dim);
	cudaThreadSynchronize();
}
extern "C" void checkResultVectorGpu(bool* outputs, unsigned int vectorSize)
{
	Debug_CheckResultVector<<<1,1>>>(outputs,vectorSize);
	cudaThreadSynchronize();
}
extern "C" void printArray(unsigned int *source,unsigned int size,unsigned int itemCount)
{
	Debug_PrintArray<<<1,1>>>(source,size,itemCount);
	cudaThreadSynchronize();
}
extern "C" void printMemory(DATATYPE* D_Inputs,unsigned int* D_Offsets,bool* D_NodeTypes,short* D_ItemsCount, unsigned int noItems,unsigned int dim)
{
	//cudaDeviceSetLimit(cudaLimitPrintfFifoSize, 10000000);
	Debug_PrintMemory<<<1,1>>>(D_Inputs,D_Offsets,D_NodeTypes,D_ItemsCount,noItems,dim);
	cudaThreadSynchronize();
}
extern "C" void printTupleArray(unsigned int* inputs,unsigned int noInputsInChunk, unsigned int dim)
{
	//cudaDeviceSetLimit(cudaLimitPrintfFifoSize, 10000000);
	Debug_PrintTupleArray<<<1,1>>>(inputs,noInputsInChunk,dim);
	cudaThreadSynchronize();
}


#endif