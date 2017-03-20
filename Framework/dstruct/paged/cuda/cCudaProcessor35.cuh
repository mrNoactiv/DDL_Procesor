/*!
* \class cCudaProcessor35
*
* \brief Range query processor for CUDA capability 3.5 or above.
*
* \author Pavel Bednar
* \date 2015-01-26
*/
#ifndef __cCudaProcessor35_cuh__
#define __cCudaProcessor35_cuh__
#include "dstruct/paged/cuda/cCudaGlobalDefs.cuh"
#include "dstruct/paged/cuda/cCudaParams.h"
#include "common/cCommon.h"

using namespace common;

class cCudaProcessor35
{
private:
	//void cudaRangeQuery_DBFS_35(uint* nodeIndices, uint nodeCount, uint level, cCudaParamsRecursive params);

public:
	void RangeQuery_DBFS_35(uint* nodeIndices, uint nodeCount, uint level, cCudaParamsRecursive params);
	void RangeQuery_BFS_35(cCudaParamsBasic_List params, uint &outCount, uint** items, bool batch, uint rootIndex);
	void RangeQuery_DFS_35(uint* nodeIndices, uint nodeCount, uint level, cCudaParamsRecursive params);
};

#endif
