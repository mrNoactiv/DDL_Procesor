#ifndef __cNodeBuffers_h__
#define __cNodeBuffers_h__

#include "common/memorystructures/cLinkedList.h"
#include "dstruct/paged/core/sCoverRecord.h"
#include "common/datatype/tuple/cMbrSideSizeOrder.h"

namespace dstruct {
	namespace paged {
		namespace core {

struct sItemBuffers
{
	char* riBuffer;                    // buffer used for reference items
	char* codingBuffer;                // buffer used for coding
};

using namespace common::memorystructures;
using namespace common::datatype::tuple;

//template <class TRecord> class cLinkedList;

template<class TKey>
class cNodeBuffers
{
public:
	cMemoryBlock* bufferMemBlock;    // memblock for all buffers

	sItemBuffers itemBuffer;	// buffer used for compression and ri
	sItemBuffers itemBuffer2;	// buffer 2 used for compression and ri

	cMemoryBlock* riMemBlock;	// memblock for all additional buffers during ri processing
	cMemoryBlock* riMemBlock2;	// memblock for all additional buffers during ri processing
	cMemoryBlock* riMemBlock3;	// memblock for all additional buffers during ri processing

	char* tmpNode;							// buffer used for reconstruction of the node during ri rebuild
	char* tmpNode2;							// buffer used for reconstruction of the node during ri rebuild
	ushort* tmpNodeItemOrders;				// buffer used for reconstruction of the node during ri rebuild
	cLinkedList<sCoverRecord>* subNodes;		// buffer used for distribution of the item during ri rebuild
	cLinkedList<sCoverRecord>* transition;	// buffer used for transition of the subnodes during ri rebuild
	char* subNodesMasks;					// buffer used for distribution masks
	char* transMasks;						// buffer used for transition masks
	ushort* subNodesOrders;					// buffer used for logical orders of subnodes
	char* mergedMasks;						// buffer used for masks of merged subnodes
	char* mergedMasks2;						// buffer used for masks of merged subnodes
	char* refItems;							// buffer used for reference items of new subNodes

	// buffers for node processing in RTree
	char *cRTLeafNode_mem;
	char *TKey_ql1, *TKey_qh1;
	char *mbr1Lo, *mbr1Hi, *mbr2Lo, *mbr2Hi;

	char *mbrs;
	char *masks;
	tMbrSideSizeOrder *mbrSide;
};

}}}
#endif