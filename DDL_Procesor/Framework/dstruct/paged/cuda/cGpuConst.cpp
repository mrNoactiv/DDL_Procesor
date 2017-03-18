#ifdef CUDA_ENABLED
#include "cGpuConst.h"

//private members
unsigned int cGpuConst::THREADS_PER_BLOCK = 1024;
float cGpuConst::SPLIT_RATIO = 0.8;

//public members
unsigned int cGpuConst::MAX_SCHEDULED_BLOCKS=8;
bool cGpuConst::DEBUG_FLAG=false;
short cGpuConst::ALGORITHM_TYPE = cGpuAlgorithm::Gpu_BFS;
short cGpuConst::MEMORY_LOCATION = cGpuMainMemoryLocation::Gpu;
//old:
//bool cGpuConst::SEARCH_INNER_NODES_ON_GPU=true;
//unsigned int cGpuConst::CHUNKS_PER_BLOCK = 16;

#endif
