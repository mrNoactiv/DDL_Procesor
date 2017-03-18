#ifdef CUDA_ENABLED
#ifndef __cDebugCpu_h__
#define __cDebugCpu_h__

//#include <dstruct\paged\cuda\globalDefs.h>
//#include <dstruct\paged\cuda\dataDefs.h>
//#include <dstruct\paged\cuda\utils_Kernel.h>

#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "common/datatype/tuple/cTuple.h"
#include "common/datatype/cBasicType.h"
#include "common/datatype/cDataType.h"
#include "common/stream/cStream.h"
#include "dstruct/paged/core/cNodeCache.h"
#include "dstruct/paged/rtree/cRTreeLeafNode.h"
#include "dstruct/paged/rtree/cRTreeLeafNodeHeader.h"
#include "common/utils/cTimer.h"

#include "common/random/cGaussRandomGenerator.h"
#include "common/data/cTuplesGenerator.h"
#include "dstruct/paged/rtree/cRTreeConst.h"
//#include "cGpuConst.h"
//#include <cuda_runtime.h>
//#include <cuda.h>
//#include "cCudaParams.h"

using namespace common::data;
using namespace common::datatype::tuple;
using namespace dstruct::paged::rtree;

template<class TKey>
class cDebugCpu
{
private:
protected:
public:

	cDebugCpu();
	~cDebugCpu();
	static void PrintLeafNodeCpu(cRTreeLeafNode<TKey>* leafNode);
	template<typename TMbr>
	static void PrintInnerNodeCpu(cRTreeNode<TMbr>* innerNode,const cSpaceDescriptor* sd);
};

//template<class TKey, class TLeafNode>
template<class TKey>
cDebugCpu<TKey>::cDebugCpu()
{
}

//template<class TKey, class TLeafNode>
template<class TKey>
cDebugCpu<TKey>::~cDebugCpu()
{

}

template<class TKey>
void cDebugCpu<TKey>::PrintLeafNodeCpu(cRTreeLeafNode<TKey>* leafNode)
{
	printf("\nPrinting Leaf Node Tuples (CPU):\n");
	for (unsigned int i = 0;i<leafNode->GetItemCount() ;i++)
	{
		TKey::Print(leafNode->GetCItem(i), "\n", leafNode->GetSpaceDescriptor());
	}
}
template<class TKey> template <typename TMbr>
void cDebugCpu<TKey>::PrintInnerNodeCpu(cRTreeNode<TMbr>* innerNode,const cSpaceDescriptor* sd)
{
	printf("\nPrinting Inner Node MBRs (CPU):\n");
	for (unsigned int i = 0 ; i < innerNode->GetItemCount() ; i++)
	{
		TKey::Print(TMbr::GetLoTuple(innerNode->GetCKey(i)), " : ", sd);
		TKey::Print(TMbr::GetHiTuple(innerNode->GetCKey(i),sd), "\n",sd);
	}
}
#endif
#endif