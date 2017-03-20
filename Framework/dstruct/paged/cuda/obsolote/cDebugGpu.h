#ifdef CUDA_ENABLED
#ifndef __cDebugGpu_h__
#define __cDebugGpu_h__

#include <dstruct/paged/cuda/globalDefs.h>
#include "cGpuConst.h"
#include <cuda_runtime.h>
#include <cuda.h>
#include "cCudaParams.h"

extern "C" void printLeafNodesGpu(unsigned int *D_globalMem, unsigned int blocksCount,unsigned int* offsets, unsigned int* offsetSizes,unsigned int dim);
extern "C" void printLeafNodeGpu(DATATYPE* D_globalMem,unsigned int* D_offsets,short* D_itemsCount, unsigned int itemsOrder,unsigned int dim);
extern "C" void printInnerNodeGpu(DATATYPE* D_globalMem,unsigned int* D_offsets,short* D_itemsCount, unsigned int itemsOrder,unsigned int dim);
extern "C" void checkResultVectorGpu(bool* outputs, unsigned int vectorSize);
extern "C" void printArray(unsigned int *source,unsigned int size,unsigned int itemCount);
extern "C" void printMemory(DATATYPE* D_Inputs,unsigned int* D_Offsets,bool* D_NodeTypes,short* D_ItemsCount, unsigned int noItems,unsigned int dim);
extern "C" void printTupleArray(unsigned int* inputs,unsigned int noInputsInChunk, unsigned int dim);

template<class TKey>
class cDebugGpu
{
private:
protected:
public:

	cDebugGpu();
	~cDebugGpu();
	static void PrintInnerNodeGpu(cMemoryManagerCuda<TKey>* cmm, unsigned int itemOrder,unsigned int dim);
	static void PrintLeafNodeGpu(cMemoryManagerCuda<TKey>* cmm, unsigned int itemOrder,unsigned int dim);
	static void CheckResultVectorGpu(bool* outputs, unsigned int vectorSize);
	static void PrintArray(unsigned int *source,unsigned int size,unsigned int itemCount);
	static void PrintMemory(DATATYPE* D_Inputs,unsigned int* D_Offsets,bool* D_NodeTypes,short* D_ItemsCount, unsigned int noItems,unsigned int dim);
	static void PrintTupleArray(unsigned int* inputs,unsigned int noInputsInChunk, unsigned int dim);
};

template<class TKey>
cDebugGpu<TKey>::cDebugGpu()
{
}

template<class TKey>
cDebugGpu<TKey>::~cDebugGpu()
{

}


template<class TKey>
void cDebugGpu<TKey>::PrintLeafNodeGpu(cMemoryManagerCuda<TKey>* cmm, unsigned int itemOrder,unsigned int dim)
{
	printf("\nPrinting Leaf Node MBRs (GPU):\n");
	printLeafNodeGpu(cmm->GetD_Inputs(), cmm->GetD_Offsets(), cmm->GetD_ItemsCount(),itemOrder, dim);
}
template<class TKey>
void cDebugGpu<TKey>::PrintInnerNodeGpu(cMemoryManagerCuda<TKey>* cmm, unsigned int itemOrder,unsigned int dim)
{
	printf("\nPrinting Inner Node MBRs (GPU):\n");
	printInnerNodeGpu(cmm->GetD_Inputs(), cmm->GetD_Offsets(), cmm->GetD_ItemsCount(),itemOrder, sd->GetDimension());
}
template<class TKey>
void cDebugGpu<TKey>::CheckResultVectorGpu(bool* outputs, unsigned int vectorSize)
{
	checkResultVectorGpu(outputs,vectorSize);
}
template<class TKey>
void cDebugGpu<TKey>::PrintArray(unsigned int *source,unsigned int size,unsigned int itemCount)
{
	printArray(source,size,itemCount);
}
template<class TKey>
void cDebugGpu<TKey>::PrintMemory(DATATYPE* D_Inputs,unsigned int* D_Offsets,bool* D_NodeTypes,short* D_ItemsCount, unsigned int noItems,unsigned int dim)
{
	printf("\nPrinting GPU's Memory:\n");
	printMemory(D_Inputs,D_Offsets,D_NodeTypes,D_ItemsCount,noItems,dim);
}
template<class TKey>
void cDebugGpu<TKey>::PrintTupleArray(unsigned int* inputs,unsigned int noInputsInChunk, unsigned int dim)
{
	printf("\nPrinting GPU's Memory:\n");
	printTupleArray(inputs,noInputsInChunk,dim);
}

#endif
#endif