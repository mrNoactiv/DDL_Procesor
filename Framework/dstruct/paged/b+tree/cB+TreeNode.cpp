
#include "dstruct/paged/b+tree/cB+TreeNode.h"

namespace dstruct {
	namespace paged {
		namespace bptree {

cLeafIndices::cLeafIndices(char* memory)
{
	__int64 tmp;
	char* mem = memory;
	mIndexItems = (cLeafIndexItem**)mem;
	mem += sizeof(cLeafIndexItem*) * LeafNode_Count;
	for (unsigned int i = 0 ; i < LeafNode_Count ; i++)
	{
		mIndexItems[i] = (cLeafIndexItem*)mem;
		mIndexItems[i]->states = mem + sizeof(cLeafIndexItem);
		mem += sizeof(cLeafIndexItem) + Query_Count * sizeof(char);

		//tmp = (__int64)(mem - memory);
		//if (tmp >= 62008)
		//{
		//	int bla = 0;
		//}
	}
}

unsigned int cLeafIndices::GetMemSize()
{
	return sizeof(cLeafIndexItem**) + (LeafNode_Count * (sizeof(cLeafIndexItem) + Query_Count * sizeof(char)));
}

}}}
