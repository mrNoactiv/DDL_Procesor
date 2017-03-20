#ifndef __cInsertBuffers_h__
#define __cInsertBuffers_h__

#include "common/memorystructures/cStack.h"
#include "common/memdatstruct/cMemoryBlock.h"
#include "dstruct/paged/rtree/cSignatureRecord.h"
#include "dstruct/paged/rtree/cSignatureKey.h"

using namespace common::memdatstruct;

namespace dstruct {
	namespace paged {
		namespace rtree {

class cInsertSigBuffers
{
  public:
	cSignatureRecord*** SigNodes;	// one signature record for each allowed level of the tree
	cSignatureKey** ConvIndexKeys;	// keys of conversion index in the case of signatures
};

template<class TKey>
class cInsertBuffers
{
public:
	cMemoryBlock* bufferMemBlock;     // memblock for all buffers

	char* tMbr_insert;
	char* tMbr_update;
	char* tMbr_mbr;

	cNodeBuffers<TKey> nodeBuffer;	          // buffers used in node processing
	cInsertSigBuffers signatureBuffers; // buffers used for signatures

	cArray<uint>* CurrentPath;					// path in tree used during bulk signature index creation

	// Since cTuple inherits cDataType, if we need an instance of cTuple, we must use the following
	// lines in the pool:
	// size += header->GetKeySize(); ...
	// insertBuffers->tmp_item.SetData(buffer);
	// buffer += header->GetKeySize();
	TKey tmpKeyORTree;							// temporary item for ordering rtree, fk

};

}}}
#endif