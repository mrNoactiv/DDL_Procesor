/**
*	\file cTreeNode.h
*	\author Michal Krátký (from 2001), Radim Baca (from 2006)
*	\version 0.3
*	\date may 2010
*	\brief Node of fixed-length capacity
*/

#ifndef __cTreeNode_h__
#define __cTreeNode_h__

namespace dstruct {
  namespace paged {
	namespace core {
		template<class TKey> class cTreeNode;
}}}

#include "common/datatype/tuple/cMBRectangle.h"
#include "dstruct/paged/core/cTreeNode_SubNode.h"
#include "common/stream/cStream.h"
#include "common/datatype/cDataVarLen.h"
#include "dstruct/paged/core/cTreeNodeHeader.h"
#include "dstruct/paged/core/cNodeItem.h"
#include "dstruct/paged/core/cNode.h"
#include "dstruct/paged/core/cNodeBuffers.h"
#include "common/cNumber.h"
#include "common/memorystructures/cLinkedList.h"
#include "common/utils/cHistogram.h"
#include "common/datatype/tuple/cMbrSideSizeOrder.h"

using namespace common::datatype::tuple;
using namespace common::utils;
using namespace common::memorystructures;

typedef unsigned short tItemOrder; // type of the item order 

namespace dstruct {
  namespace paged {
	namespace core {

/**
 * Node of the tree structure with fixed size of items, where the link are part of the node item.
 * We expect two type of nodes: (1) nodes with item = link + key, and (2) nodes with item = key + data.
 * The first type of node set the item with SetKey and SetLink methods and the second type of node use the SetKeyData method.
 * The items in the node are not automaticaly ordered.
 * The node use an itemOrder table which is kept sorted and which point on item in the node.
 * Therefore, the ordered data structure usually set the items through the SetOrderedItem method.
 *
 * Terminology: 
 *  - lOrder - the logical order, pOrder - the physical order
 *  - *Po - this method works with the defined physical order
 *  - GetC* - it returns const char*, Get*Ptr - it returns char* without any transformation, e.g. coding ...
 *
 *	\author Michal Kratky, Radim Baca
 *	\version 0.2
 *	\date may 2010 (0.1) - basic version, aug 2012 (0.2) - compression support, some changes of terminology
 **/
template<class TKey>
class cTreeNode: public cNode
{
protected:
	typedef cTreeNode_SubNode<TKey> TSubNode; // for DSMODE_RI, it means reference items mode
	typedef cBitString TMask;               // for DSMODE_RI, it means bit mask of the reference block 
	typedef cMBRectangle<TKey> TMBR;

	static const unsigned int ATTRIBUT_LEAF = 0;

	// leaf operations
	inline unsigned int SetLeafItemPo(unsigned int pOrder, const char* key, char* data, uint keyLength = cCommon::UNDEFINED_UINT, sItemBuffers* buffers = NULL);
	inline unsigned int GetLeafItemSize(const char* key, const char* leafData) const;
	inline void GetKeyData(unsigned int lOrder, char** key, char** data, sItemBuffers* buffers = NULL, ushort lSubNodeOrder = USHRT_MAX);

	int AddLeafItem_default(const char* key, char* data, sItemBuffers* buffers = NULL);
	int AddLeafItem_ri(const char* key, char* data, bool incFlag, cNodeBuffers<TKey>* buffers = NULL);

	int InsertLeafItem_default(const char* key, char* data, bool allowDuplicateKey, sItemBuffers* buffers = NULL);
	int InsertLeafItem_ri(const char* key, char* data, bool allowDuplicateKey, cNodeBuffers<TKey>* buffers = NULL);

	void SplitLeafNode_ri(cTreeNode<TKey>& newNode, cTreeNode<TKey> &tmpNode, cNodeBuffers<TKey>* buffers = NULL);
	void SplitLeafNode_coding(cTreeNode<TKey>& newNode, cTreeNode<TKey> &tmpNode, cNodeBuffers<TKey>* buffers = NULL);

	// common operations
	void Split_default(cTreeNode<TKey>& newNode, cTreeNode<TKey> &tmpNode);

	inline char* GetItems() const;
	inline char* GetItemOrders() const;

	inline uint GetKeyLength(char* key, char* subNodeHeader = NULL) const;
	inline uint GetKeySize(char* key, uint keyLength = NULL, sItemBuffers* buffers = NULL);

	inline unsigned int GetDataSize(const char* data) const;
	inline unsigned int GetItemSize(const char* keyData) const;
	inline unsigned int GetItemSize(unsigned int lOrder) const;

	inline char* GetKeyPtrPo(unsigned int pOrder) const;
	inline char* GetKeyPtr(unsigned int lOrder) const;

	inline void SetItemPo(unsigned int pOrder, const char* item, unsigned int itemSize);
	inline void SetItem(unsigned int lOrder, const char* item);

	inline tItemOrder& GetItemPOrder(const tItemOrder& lOrder) const;
	inline char* GetPItemPOrder(const tItemOrder& lOrder) const;
	inline void SetItemPOrder(const tItemOrder &lOrder, const tItemOrder &pOrder);
	inline void IncItemPOrder(const tItemOrder &lOrder, int incValue);
	inline void InsertItemPOrder(const tItemOrder &lOrder, const tItemOrder &pOrder);

	int FindOrder_default(const TKey& key, int mode, sItemBuffers* buffers = NULL, int lo = 1) const;
	int FindOrder_ri(const TKey& key, int mode, sItemBuffers* buffers = NULL) const;

public:
	inline char* GetItemPtr(unsigned int itemOrder) const;

	static const int INSERT_YES;
	static const int INSERT_AT_THE_END;
	static const int INSERT_NOSPACE;
	static const int INSERT_EXIST;
	static const int SUBNODE_EXIST;
	static const int SUBNODE_NOTEXIST;
	static const tNodeIndex EMPTY_LINK        = 0xffffffff;	
	static const unsigned int EMPTY_LINK_BYTE = 0xff;

	/** parameter value for FindOrder() method **/
	static const int FIND_E   = 0;        // equal
	static const int FIND_SBE = 1;        // smalest bigger or equal
	static const int FIND_INSERT = 2;     // very similar to SBE, but return FIND_EQUAL if the item already exists
	// return values (used by FindOrder)
	static const int FIND_NOTEXIST = -1;  // no item find
	static const int FIND_EQUAL    = -2;  // equal item exists

	static const int REAL_SIZE_CONST = 128;	// for compute real size 
	static const int RTREE_SN_COUNT = 2; // must be multiplication of two
	static const int SPLIT_COUNT = 1;    // number of splits in Rtree rebuild

	static unsigned int Leaf_Splits_Count;
	static unsigned int Inner_Splits_Count;

	cTreeNode(const cTreeNode<TKey>* origNode, const char *mem);
    cTreeNode();
	~cTreeNode();

	void Init();

	// leaf operations
	int InsertLeafItem(const char* key, char* data, bool allowDuplicateKey, cNodeBuffers<TKey>* buffers = NULL);
	int AddLeafItem(const char* key, char* data, bool incFlag, cNodeBuffers<TKey>* buffers = NULL);
	inline bool HasLeafFreeSpace(const char* key, const char* data) const;

	void DeleteLeafItem(const char* key, sItemBuffers* buffers = NULL);

	inline char* GetData(unsigned int lOrder, sItemBuffers* buffers = NULL) const;
	inline char* GetData(const char* item);
	inline char* GetDataPo(unsigned int keySize, unsigned int pOrder) const;

	// inner operations
	bool InsertItem(const unsigned int itemOrder, const char* key, const tNodeIndex &childIndex);
	int InsertItem(const char *key, const tNodeIndex childIndex, bool allowDuplicateKey);
	bool AddItem(const char* tItem_item, const tNodeIndex &nodeIndex, bool incFlag = false);
	inline bool HasFreeSpace(const char* key) const;

	inline void SetKey(unsigned int lOrder, const char* key);
	inline unsigned int SetKeyPo(unsigned int pOrder, const char* key);
	inline void SetLinkPo(unsigned int pOrder, const tNodeIndex& link);
	inline void SetLink(unsigned int lOrder, const tNodeIndex& link);
	inline tNodeIndex GetLink(unsigned int order) const;

	inline void SetExtraItem(unsigned int order, TKey &extraItem);
	inline void SetExtraLink(unsigned int order, tNodeIndex link);
	inline char* GetExtraItem(unsigned int order) const;
	inline tNodeIndex GetExtraLink(unsigned int order) const;
	inline tNodeIndex* GetExtraLinks() const;

	// common operations
	void Split(cTreeNode<TKey>& newNode, cTreeNode<TKey> &tmpNode, cNodeBuffers<TKey>* buffers);

	void Write(cStream* mStream) const;
	void Read(cStream* mStream);

	// flags
	bool IsLeaf() const;
	void SetLeaf(bool leaf);
	bool IsFull() const;
	bool IsOrdered() const;

	static bool IsLeaf(const tNodeIndex index);
	static inline tNodeIndex GetNodeIndex(const tNodeIndex leafIndex);
	static inline tNodeIndex GetLeafNodeIndex(const tNodeIndex index);

	inline cTreeNodeHeader* GetNodeHeader() const;
	inline void SetData(char* data);
	inline char* GetData() const;

	inline void CopyItems(char* pItems, uint pItemCount);
	inline void CopyItemOrders(tItemOrder* pItemOrders, uint pItemCount);
	inline void CopyItemCount(uint pItemCount);

	inline const char* GetCKey(unsigned int lOrder, sItemBuffers* buffers = NULL, ushort lSubNodeOrder = USHRT_MAX) const;
	inline const char* GetCPartKey(unsigned int lOrder, sItemBuffers* buffers = NULL, ushort lSubNodeOrder = USHRT_MAX) const;
	inline const char* GetCItem(unsigned int lOrder, sItemBuffers* buffers = NULL) const;

	bool CheckNode() const;
	void Print(sItemBuffers* buffers = NULL) const;
	void Print2File(char* FileName) const;
	void Print2File(char* FileName, sItemBuffers* buffer, bool onlyHeaders = false, ushort subNodeLOrder = -1) const;
	void ComputeSubNodesDistribution(cHistogram* hist);
	void ComputeDimDistribution(cHistogram** hist);

	int FindItemOrder(const char* key, bool allowDuplicateKey, sItemBuffers* buffers = NULL, ushort lSubNodeOrder = USHRT_MAX);
	int FindOrder(const TKey& key, int mode, sItemBuffers* buffers = NULL) const;
	int FindOrder(const char* key, int mode, sItemBuffers* buffers = NULL) const;
	int FindOrder(const char* key, int mode, sItemBuffers* buffers, int startOrder, int maxOrder) const;
	int FindOrderInsert(const char* key, int mode, sItemBuffers* buffers = NULL, ushort lSubNodeOrder = USHRT_MAX) const;

	void Copy(cTreeNode<TKey> * source);
	inline void SwapItemOrder(int a, int b);
	inline void CopyKeyTo(char* key, unsigned int order, sItemBuffers* buffers = NULL);

protected:
	// DSMODE_RI - it means reference items mode
	inline ushort GetSubNodesCount() const;
	inline void SetSubNodesCount(ushort count);
	inline void IncSubNodesCount();

	inline ushort GetSubNodesCapacity() const;
	inline void SetSubNodesCapacity(ushort capacity);
	inline void IncSubNodesCapacity(ushort value = 1);

	inline unsigned char GetUpdatesCount() const;
	inline void SetUpdatesCount(unsigned char count);
	inline void IncUpdatesCount();

	inline void SetSubNodeHeadersOffset(ushort offset);
	inline ushort GetSubNodeHeadersOffset() const;

	inline ushort GetSubNodeLOrder(ushort itemOrder) const;
	inline char* GetSubNodeHeaderByItem(ushort itemOrder) const;

	inline char* GetSubNodeHeaders() const;

	inline char* GetSubNode(ushort subNodeOrder) const;
	inline char* GetSubNodeHeader(ushort subNodeOrder) const;

private:
	inline ushort FreeSize(ushort subNodeOrder) const;
	inline ushort CompleteSize(ushort subNodeOrder) const;

	inline uint GetSubNodeHeaderSize(const char* key) const;
	inline uint GetSubNodeHeaderSize2(char* subNodeHeader) const;

	inline uint CutKey(char* subNodeHeader, const char* key, char* cutKey);

	inline void UpdateItemOrderIntervals(ushort subNodeLOrder, short shift = 1);
	void UpdateSubNodeHeader(char* subNodeHeader, uint itemSize, uint lastItemOrder);
	inline void UpdatePOrders(char* subNodeHeader, short shift, bool allNextSubNodes, ushort startItem = -1);
	void UpdateSubNodesPOrders(ushort lSubNodeOrder, short shift, bool allNextSubNodes);
	inline void Shift(ushort subNodeLOrder, int shift, uint startByte, uint startItem = -1);

	char* CreateSubNode(const char* key, ushort lOrder, ushort lSubNodeOrder);
	inline char* CreateSubNode(ushort subNodePOrder, ushort subNodeLOrder, ushort lOrder, const char* key);
	uint InsertToSubNode(ushort lSubNodeOrder, const char* key, ushort lOrder, char* data, sItemBuffers* buffers);

	int FindCompatibleSubNode(const char* key, int* lOrder, char **subNodeHeader, ushort* lSubNodeOrder, bool allowDuplicateKey, sItemBuffers* buffers);
	int FindCompatibleSubNode_Rtree(const char* key, int* lOrder, char **subNodeHeader, ushort* lSubNodeOrder, sItemBuffers* buffers);

	bool FindCompatibleSubNode(const char* item, ushort lOrder, char **subNodeHeader, ushort* lSubNodeOrder);
	bool FindCompatibleSubNode2(const char* item, ushort lOrder, char **subNodeHeader, ushort* lSubNodeOrder);
	char* FindSuitableSubNode(ushort lOrder, ushort* lSubNodeOrder);

	char* SubNodeShift(ushort lSubNodeOrder, short shift, bool allNextSubNodes);
	char* Replacement(ushort lSubNodeOrder, short size);
	void SubNodeReplace(ushort lSubNodeOrder, short shift);

	void ConsistencyTest(sItemBuffers *buffers);

	// DSMODE_RI - Rebuild methods
	
	inline void Rebuild_pre(cNodeBuffers<TKey>* buffers);
	inline void Rebuild_pre(uint itemCount, cNodeBuffers<TKey>* buffers);
	inline void Rebuild2_pre(uint itemCount, cNodeBuffers<TKey>* buffers);
	inline void Rebuild3_pre(uint itemCount, cNodeBuffers<TKey>* buffers);
	inline void Rebuild2_post(cNodeBuffers<TKey>* buffers);
	inline void Rebuild3_post(cNodeBuffers<TKey>* buffers);

	inline char* CreateSubNode(uint pOrder, sCoverRecord* snRecord, uint previousSubNodePOrder);
	void RefItemsReconstruction(char* rNode, cLinkedList<sCoverRecord>* subNodes, cNodeBuffers<TKey>* buffers);
	uint ComputeSize(sCoverRecord* subNode1, sCoverRecord* subNode2, char* mergedMask, cNodeBuffers<TKey>* buffers);

	// for B-tree
	cLinkedList<sCoverRecord>* ComputeMaxMaskDistribution(char* rNode, uint itemCount, cNodeBuffers<TKey>* buffers);
	cLinkedList<sCoverRecord>* ComputeTransition(char* rNode, cLinkedList<sCoverRecord>* subNodes, cNodeBuffers<TKey>* buffers);
	cLinkedList<sCoverRecord>* MergeSubNodesByTransition(char* rNode, cLinkedList<sCoverRecord>* distribution, cLinkedList<sCoverRecord>* transition, cNodeBuffers<TKey>* buffers);
	cLinkedList<sCoverRecord>* SplitToSubNodes(cLinkedList<sCoverRecord>* subNodes, uint startItem, uint endItem, cNodeBuffers<TKey>* buffers);

	void Rebuild(char* subNode, const char* key, char* data, uint lOrder, cNodeBuffers<TKey>* buffers);
	void Rebuild(char* rNode, char* subNodeHeader, cLinkedList<sCoverRecord>* subNodes, cNodeBuffers<TKey>* buffers);
	char* Rebuild(ushort lSubNodeOrder, const char* key, cNodeBuffers<TKey>* nodeBuffers);
	void NodeRebuild(cNodeBuffers<TKey>* buffers);
	void NodeRebuild_Rtree(cNodeBuffers<TKey>* buffers);

	// for R-tree
	int Rebuild_FindTwoDisjMbrs(uint dimOrder, uint loOrder, uint hiOrder, uint mbrOrder, float pUtilization, cNodeBuffers<TKey>* buffers);
	bool Rebuild_SortBy(uint dimension, uint loOrder, uint hiOrder, cNodeBuffers<TKey>* buffers);
	void Rebuild_CreateMbr(uint startOrder, uint finishOrder, char* TMbr_mbr, cNodeBuffers<TKey>* buffers);
	inline void Rebuild_SwapItemOrder(char* rNode, uint lOrder1, uint lOrder2);
	void Rebuild_ComputeMasks(cNodeBuffers<TKey>* buffers);
	void Rebuild_Rtree(cNodeBuffers<TKey>* buffers);
	inline char* GetMask(char* masks, ushort lOrder);

	bool IsDistributionDifferent(char* rNode, cLinkedList<sCoverRecord>* subNodes, bool nodeRebuild);

	inline char* CreateSubNode(char* tmpNode, uint pOrder, sCoverRecord* snRecord, uint previousSubNodePOrder);
	inline void SetItemPOrder(char* lOrders, const tItemOrder &lOrder, const tItemOrder &pOrder);

	void Print(cLinkedList<sCoverRecord>* list, char* rNode);

protected: // we used them in cRTreeLeafNode
	inline char* GetKey(char* rNode, uint lOrder, uint itemCount = UINT_MAX);
	inline char* GetData(char* rNode, uint lOrder, uint itemCount = UINT_MAX);
	inline char* GetMbr(char* mbrs, ushort lOrder);

	void Rebuild_Rtree_pre(uint itemCount, cNodeBuffers<TKey>* buffers);
	inline void Rebuild_post(cNodeBuffers<TKey>* buffers);
	char* Reconstruction(char* subNode, const char* key, char* data, uint lOrder, cNodeBuffers<TKey>* buffers);
	void Rebuild_CutLongest(uint loOrder, uint hiOrder, ushort mbrOrder, ushort splitOrder, cNodeBuffers<TKey>* buffers);
	void Rebuild_ComputeMasks(ushort snCount, char* finalMbrs, ushort itemStart, ushort itemEnd, uint baseItemCount, cNodeBuffers<TKey>* buffers);
	void Rebuild_Rtree(ushort snCount, char* finalMbrs, ushort itemStart, ushort itemEnd, uint baseItemCount, cNodeBuffers<TKey>* buffers);

};

template<class TKey> const int cTreeNode<TKey>::INSERT_YES = 0;
template<class TKey> const int cTreeNode<TKey>::INSERT_AT_THE_END = 1;
template<class TKey> const int cTreeNode<TKey>::INSERT_NOSPACE = INT_MIN + 1;
template<class TKey> const int cTreeNode<TKey>::INSERT_EXIST = INT_MIN + 2;
template<class TKey> const int cTreeNode<TKey>::SUBNODE_EXIST = INT_MIN + 3;
template<class TKey> const int cTreeNode<TKey>::SUBNODE_NOTEXIST = INT_MIN + 4;
/*
	static const int INSERT_NOSPACE = -9999999;
	static const int INSERT_EXIST = -9999998;
	*/

template<class TKey>
unsigned int cTreeNode<TKey>::Leaf_Splits_Count = 0;

template<class TKey>
unsigned int cTreeNode<TKey>::Inner_Splits_Count = 0;

/**
 * This contructor is used in the case when the node is created on the pool, this pool creates a memory
 * and the node uses it. Values are coppied from the origNode.
 */
template<class TKey> cTreeNode<TKey>::cTreeNode(const cTreeNode<TKey>* origNode, const char* mem)
{
	mHeader = origNode->GetNodeHeader();
	mData = (char*)mem;

	if ((GetNodeHeader()->GetDStructMode() == cDStructConst::DSMODE_RI) || (GetNodeHeader()->GetDStructMode() == cDStructConst::DSMODE_RICODING))
	{
		SetLeaf(origNode->IsLeaf()); // necessary for inicialization of ref items !!!
		Init(); // necessary for inicialization of ref items !!!
		SetSubNodesCount(0);
		SetSubNodesCapacity(0);
		SetSubNodeHeadersOffset(0);
		SetUpdatesCount(0);
	}
}

template<class TKey> cTreeNode<TKey>::cTreeNode()
{
}

template<class TKey> cTreeNode<TKey>::~cTreeNode()
{
	// do nothing - memory object must be deleted
}

///
template<class TKey> 
void cTreeNode<TKey>::Init()
{
	if (((mHeader->GetDStructMode() == cDStructConst::DSMODE_RI) || (mHeader->GetDStructMode() == cDStructConst::DSMODE_RICODING)) && (IsLeaf()))
	{
		mItemCount = 0;
		SetFreeSize(mHeader->GetNodeItemsSpaceSize());
		SetSubNodesCount(0);
		SetSubNodesCapacity(0);
		SetSubNodeHeadersOffset(0);
		SetUpdatesCount(0);
	}
}

template<class TKey> 
inline cTreeNodeHeader* cTreeNode<TKey>::GetNodeHeader() const
{
	return (cTreeNodeHeader*) mHeader;
}

// returns the start position of items in the node
template<class TKey>
inline char* cTreeNode<TKey>::GetItems() const
{
	return mData + GetNodeHeader()->GetItemsOffset();
}

template<class TKey> 
inline char* cTreeNode<TKey>::GetItemOrders() const
{
	return mData + GetNodeHeader()->GetItemOrderOffset();
}

/**
 * This method is appropriate, for example before a temporary node must be deleted in a pool (SetData(NULL)).
 */
template<class TKey> 
void cTreeNode<TKey>::SetData(char* data)
{
	mData = data;
}

template<class TKey> 
char* cTreeNode<TKey>::GetData() const
{
	return mData;
}

/*
Methods CopyItems are used in the case of the bulkloading
!! Works only for tuples and data of the fixed length
*/
template<class TKey>
void cTreeNode<TKey>::CopyItems(char* pItems, uint pItemCount)
{
	uint keySize, dataSize;

	if (IsLeaf())
	{
		keySize = TKey::GetSize(NULL, mHeader->GetKeyDescriptor());
		dataSize = GetNodeHeader()->GetDataSize();
	}
	else
	{
		// we have the max size of inner nodes' items
		keySize = GetNodeHeader()->GetKeySize();
		dataSize = GetNodeHeader()->GetLinkSize();
	}

	memcpy(mData + GetNodeHeader()->GetItemsOffset(), pItems, pItemCount * (keySize + dataSize));
	mFreeSize -= pItemCount * (keySize + dataSize);
	mHeader->IncrementItemCount(pItemCount);
}

template<class TKey>
void cTreeNode<TKey>::CopyItemOrders(tItemOrder* pItemOrders, uint pItemCount)
{
	memcpy(mData + GetNodeHeader()->GetItemOrderOffset(), pItemOrders, pItemCount * sizeof(tItemOrder));
}

template<class TKey>
void cTreeNode<TKey>::CopyItemCount(uint pItemCount)
{
	mItemCount = pItemCount;
}

template<class TKey> 
inline uint cTreeNode<TKey>::GetKeyLength(char* key, char* subNodeHeader) const
{
	unsigned int dsMode = mHeader->GetDStructMode();

	if (dsMode == cDStructConst::DSMODE_RI || dsMode == cDStructConst::DSMODE_RICODING)
	{
		assert(subNodeHeader != NULL);

		if (TKey::CODE == cTuple::CODE)
		{
			uint riLength = TKey::GetLength(TSubNode::GetMinRefItem(this->GetItems(),subNodeHeader), mHeader->GetKeyDescriptor());
			char* mask = TSubNode::GetMask(this->GetItems(), subNodeHeader);
			return riLength - TMask::GetNumberOfBits(mask, riLength, 1);
		}
		else
		{
			return TKey::GetLength(key, mHeader->GetKeyDescriptor());
		}
	}
	else
	{
		return TKey::GetLength(key, mHeader->GetKeyDescriptor());
	}
}

template<class TKey>
inline uint cTreeNode<TKey>::GetKeySize(char* key, uint keyLength, sItemBuffers* buffers) 
{
	unsigned int dsMode = mHeader->GetDStructMode();

	if (dsMode == cDStructConst::DSMODE_RICODING)
	{
		return TKey::Encode(mHeader->GetCodeType(), key, buffers->codingBuffer, mHeader->GetKeyDescriptor(), keyLength);
	}
	else if (dsMode == cDStructConst::DSMODE_RI)
	{
		return TKey::GetLSize(keyLength, mHeader->GetKeyDescriptor());
	}
	else
	{
		return TKey::GetSize(key, mHeader->GetKeyDescriptor());
	}
}

/**
* \return Size of the variable length data
*/
template<class TKey> inline unsigned int cTreeNode<TKey>::GetDataSize(const char* data) const
{
	return GetNodeHeader()->VariableLenDataEnabled() ? cDataVarlen::GetSize(data) : GetNodeHeader()->GetDataSize();
}

/**
 * LEAF AND INNER NODE METHOD
 * Return the size of the key + data for inner as well as leaf nodes.
 */
template<class TKey> inline unsigned int cTreeNode<TKey>::GetItemSize(const char* keyData) const
{
	unsigned int keySize, dataSize;

	if (IsLeaf())
	{
		keySize = TKey::GetSize(keyData, mHeader->GetKeyDescriptor());
		dataSize = GetDataSize(keyData + keySize);
	}
	else
	{
		// we have the max size of inner nodes' items
		keySize = GetNodeHeader()->GetKeySize(); 
		dataSize = GetNodeHeader()->GetLinkSize();
	}

	return keySize + dataSize;
}


/**
* LEAF NODE METHOD
* Return the size of the key + data
*/
template<class TKey> inline unsigned int cTreeNode<TKey>::GetLeafItemSize(const char* key, const char* leafData) const
{
	assert(IsLeaf());
	unsigned int keySize = TKey::GetSize(key, mHeader->GetKeyDescriptor());
	unsigned int dataSize = GetDataSize(leafData);

	return keySize + dataSize;
}

/**
 * LEAF AND INNER NODE METHOD
 * Return the size of the item.
*/
template<class TKey> inline unsigned int cTreeNode<TKey>::GetItemSize(unsigned int lOrder) const
{
	return GetItemSize(GetCKey(lOrder));
}


/**
 * LEAF AND INNER NODE METHOD
 * Set the item in the node in the physical order. Be careful when using this method;
 * you should set the ItemOrder as well.
 * \param pOrder The physical order of the item.
*/
template<class TKey> 
inline void cTreeNode<TKey>::SetItemPo(unsigned int pOrder, const char* item, unsigned int itemSize)
{
	assert(pOrder + itemSize <= mHeader->GetNodeItemsSpaceSize());
	memcpy(GetItems() + pOrder, item, itemSize);
}

/**
* Set the item in the node without considering its logical order. Be careful when using this method;
* you should set the ItemOrder as well.
* \param pOrder The physical order of the item.
* \param key A key to be inserted.
* \return Size of memory occupied by the inserted key.
*/
template<class TKey> 
inline unsigned int cTreeNode<TKey>::SetKeyPo(unsigned int pOrder, const char* key)
{
	assert(!IsLeaf());
	unsigned int size = GetNodeHeader()->GetKeySize();  

	assert(pOrder + sizeof(tNodeIndex) + size <= mHeader->GetNodeItemsSpaceSize());
	memcpy(GetItems() + sizeof(tNodeIndex) + pOrder , key, size);
	return size;
}

/**
* Set the key in the node without considering its lOrder. Be careful when using this method;
* you should set the ItemOrder as well.
* \param pOrder The physical order of the key.
* \param data The leaf data of the key and data.
* \param encodedKeyBuffer It buffer usable only in DSMode == cDStructConst::DSMODE_CODING
*/
template<class TKey> 
inline unsigned int cTreeNode<TKey>::SetLeafItemPo(unsigned int pOrder, const char* key, char* data, uint keyLength, sItemBuffers* buffers)
{
	int ret = INSERT_YES;
	unsigned int keySize;
	unsigned int dataSize = GetDataSize(data); 
	unsigned int dsMode = mHeader->GetDStructMode();
	char* mem = GetItems() + pOrder;

	if (dsMode == cDStructConst::DSMODE_CODING || dsMode == cDStructConst::DSMODE_RICODING)
	{
		keySize = TKey::Encode(mHeader->GetCodeType(), key, buffers->codingBuffer, mHeader->GetKeyDescriptor(), keyLength);
		key = buffers->codingBuffer;

		if (pOrder + keySize + dataSize > mHeader->GetNodeItemsSpaceSize())
		{
			ret = INSERT_NOSPACE;
		}
	}
	else if (dsMode == cDStructConst::DSMODE_RI)
	{
		keySize = TKey::GetLSize(keyLength, mHeader->GetKeyDescriptor());
	}
	else
	{
		keySize = TKey::GetSize(key, mHeader->GetKeyDescriptor());
	}

	if (ret != INSERT_NOSPACE)
	{
		memcpy(mem, key, keySize);
		memcpy(mem + keySize, data, dataSize);
		ret = keySize + dataSize;
	}

	return ret;
}


/**
* LEAF NODE METHOD
* Get key and data on a specified lorder
* \param lOrder The logical order of the key.
*/
template<class TKey> 
void cTreeNode<TKey>::GetKeyData(unsigned int lOrder, char** key, char** data, sItemBuffers* buffers, ushort lSubNodeOrder)
{
	assert(IsLeaf());
	assert(mHeader->GetDStructMode() == cDStructConst::DSMODE_DEFAULT || buffers != NULL);
//	assert(mHeader->GetDStructMode() == cDStructConst::DSMODE_DEFAULT || mHeader->GetDStructMode() == cDStructConst::DSMODE_CODING || lSubNodeOrder != USHRT_MAX);

	if ((mHeader->GetDStructMode() == cDStructConst::DSMODE_RI || mHeader->GetDStructMode() == cDStructConst::DSMODE_RICODING) && (lSubNodeOrder == USHRT_MAX))
	{
		int c = 3;
		//printf("pruser");
	}
	unsigned int keyByteSize;
	char* currentKey = GetItems() + GetItemPOrder(lOrder);
	unsigned int dsMode = mHeader->GetDStructMode();

	if (dsMode == cDStructConst::DSMODE_CODING || dsMode == cDStructConst::DSMODE_RICODING)
	{
		if (dsMode == cDStructConst::DSMODE_CODING)
		{
			keyByteSize = TKey::Decode(mHeader->GetCodeType(), currentKey, buffers->codingBuffer, mHeader->GetKeyDescriptor());
			*key = buffers->codingBuffer;
		}
		else // in the case of DSMODE_RICODING
		{
			char* subNodeHeader = (lSubNodeOrder == USHRT_MAX) ? this->GetSubNodeHeaderByItem(lOrder) : this->GetSubNodeHeader(lSubNodeOrder);
			//char* subNodeHeader = this->GetSubNodeHeader(lSubNodeOrder);
			char* minRefItem = TSubNode::GetMinRefItem(this->GetItems(), subNodeHeader);

			uint currentKeyLength = GetKeyLength(currentKey, subNodeHeader);
			keyByteSize = TKey::Decode(mHeader->GetCodeType(), currentKey, buffers->codingBuffer, mHeader->GetKeyDescriptor(), currentKeyLength);
			*key = TKey::MergeTuple(TSubNode::GetMask(this->GetItems(), subNodeHeader), minRefItem, buffers->codingBuffer, buffers->riBuffer, mHeader->GetKeyDescriptor());
		}
	}
	else if (dsMode == cDStructConst::DSMODE_RI)
	{
		char* subNodeHeader = (lSubNodeOrder == USHRT_MAX) ? this->GetSubNodeHeaderByItem(lOrder) : this->GetSubNodeHeader(lSubNodeOrder);
		//char* subNodeHeader = this->GetSubNodeHeader(lSubNodeOrder);
		char* minRefItem = TSubNode::GetMinRefItem(this->GetItems(), subNodeHeader);

		uint currentKeyLength = GetKeyLength(currentKey, subNodeHeader);
		keyByteSize = TKey::GetLSize(currentKeyLength, mHeader->GetKeyDescriptor());
		*key = TKey::MergeTuple(TSubNode::GetMask(this->GetItems(), subNodeHeader), minRefItem, currentKey, buffers->riBuffer, mHeader->GetKeyDescriptor());
	}
	else
	{
		*key = currentKey; 
		keyByteSize = TKey::GetSize(currentKey, mHeader->GetKeyDescriptor());
	}

	*data = currentKey + keyByteSize; 
}


/**
 * LEAF AND INNER NODE METHOD
 * Set item on a specified lOrder.
 * \param lOrder The logical order of the key.
*/
template<class TKey> inline void cTreeNode<TKey>::SetItem(unsigned int lOrder, const char* item)
{
	assert(GetItemPOrder(lOrder) + mHeader->GetItemSize() <= mHeader->GetNodeItemsSpaceSize());
	memcpy(GetItems() + GetItemPOrder(lOrder), item, mHeader->GetItemSize());
}

/**
 * INNER NODE METHOD
 * Set key on a specified lOrder.
 * \param lOrder The logical order of the key.
*/
template<class TKey> inline void cTreeNode<TKey>::SetKey(unsigned int lOrder, const char* key)
{
	cTreeNodeHeader* nodeHeader = GetNodeHeader();

	assert(GetItemPOrder(lOrder) + nodeHeader->GetLinkSize() + nodeHeader->GetKeySize() <= mHeader->GetNodeItemsSpaceSize());
	memcpy(GetItems() + nodeHeader->GetLinkSize() + GetItemPOrder(lOrder), key, nodeHeader->GetKeySize());
}

/**
 * INNER NODE METHOD
 * Set link for the item with the physical order.
 * \param pOrder The physical order.
 * \param link The link to be set.
 */
template<class TKey> inline void cTreeNode<TKey>::SetLinkPo(unsigned int pOrder, const tNodeIndex& link)
{
	assert(!IsLeaf());
	*((tNodeIndex*) (GetItems() + pOrder)) = link;
}

/**
 * INNER NODE METHOD
 * Set link for the item with the logical order.
 * \param lOrder The logical order.
 * \param link The link to be set.
 */
template<class TKey> inline void cTreeNode<TKey>::SetLink(unsigned int lOrder, const tNodeIndex& link)
{
	assert(!IsLeaf());
	*((tNodeIndex*) (GetItems() + GetItemPOrder(lOrder))) = link;
}

template<class TKey> inline void cTreeNode<TKey>::SetExtraItem(unsigned int order, TKey &extraItem) 
{ 
	TKey::Copy(GetExtraItem(order), extraItem);
}

template<class TKey> inline void cTreeNode<TKey>::SetExtraLink(unsigned int order, tNodeIndex link)
{
	*(((tNodeIndex*)(mData + GetNodeHeader()->GetExtraLinksOffset())) + order) = link;
}


/**
 * LEAF AND INNER NODE METHOD
 * It returns the pointer on the item, however transformations are processed, e.g. coding ...
 * \param lOrder The logical order of the item.
 * \return Pointer to the node item on the specified logical order. In the case of the leaf node, it corresponds 
 *         to the pointer on the key.
 */
template<class TKey> inline const char* cTreeNode<TKey>::GetCItem(unsigned int lOrder, sItemBuffers* buffers) const
{
	uint dsMode = mHeader->GetDStructMode();
	unsigned int keySize = GetNodeHeader()->GetKeySize();
	char* mem = GetItems() + GetItemPOrder(lOrder);

	assert(dsMode == cDStructConst::DSMODE_DEFAULT || buffers != NULL);

	if (IsLeaf())
	{
		if (dsMode == cDStructConst::DSMODE_RI)
		{
			char* subNodeHeader = this->GetSubNodeHeaderByItem(lOrder);
			char* minRefItem = TSubNode::GetMinRefItem(this->GetItems(), subNodeHeader);
			uint encodedKeySize = TKey::GetLSize(GetKeyLength(mem, subNodeHeader), mHeader->GetKeyDescriptor());
			buffers->riBuffer = TKey::MergeTuple(TSubNode::GetMask(this->GetItems(), subNodeHeader), minRefItem, mem, buffers->riBuffer, mHeader->GetKeyDescriptor());
			memcpy(buffers->riBuffer + keySize, mem + encodedKeySize, GetDataSize(mem + encodedKeySize));
			mem = buffers->riBuffer;
		}
		else if (dsMode == cDStructConst::DSMODE_CODING)
		{
			uint encodedKeySize = TKey::Decode(mHeader->GetCodeType(), mem, buffers->codingBuffer, mHeader->GetKeyDescriptor());
			memcpy(buffers->codingBuffer + keySize, mem + encodedKeySize, GetDataSize(buffers->codingBuffer + encodedKeySize));
			mem = buffers->codingBuffer;
		}
		else  if (dsMode == cDStructConst::DSMODE_RICODING)
		{
			char* subNodeHeader = this->GetSubNodeHeaderByItem(lOrder);
			char* minRefItem = TSubNode::GetMinRefItem(this->GetItems(), subNodeHeader);
			uint itemLength = GetKeyLength(mem, subNodeHeader);

			uint encodedKeySize = TKey::Decode(mHeader->GetCodeType(), mem, buffers->codingBuffer, mHeader->GetKeyDescriptor(), itemLength);
			
			buffers->riBuffer = TKey::MergeTuple(TSubNode::GetMask(this->GetItems(), subNodeHeader), minRefItem, buffers->codingBuffer, buffers->riBuffer, mHeader->GetKeyDescriptor());
			
			memcpy(buffers->riBuffer + keySize, mem + encodedKeySize, GetDataSize(mem + encodedKeySize));
			mem = buffers->riBuffer;
		}

	}
	return mem; 
}

/**
* It returns the pointer on the key without concatenation with reference item 
* \param lOrder The logical order of the item.
* \return Constant pointer to the node key on the specified order.Warning: In the case of coding, it returns the buffer.
*/
template<class TKey> inline const char* cTreeNode<TKey>::GetCPartKey(unsigned int lOrder, sItemBuffers* buffers, ushort lSubNodeOrder) const
{
	char* key = GetItems() + GetItemPOrder(lOrder);

	if (mHeader->GetDStructMode() == cDStructConst::DSMODE_RICODING)
	{
		char* subNodeHeader = this->GetSubNodeHeader(lSubNodeOrder);
		uint keyLength = GetKeyLength(key, subNodeHeader);

		TKey::Decode(mHeader->GetCodeType(), key, buffers->codingBuffer, mHeader->GetKeyDescriptor(), keyLength);
		key = buffers->codingBuffer;
	}

	return key;
}

/**
 * It returns the pointer on the key and transformations are processed, e.g. coding ...
 * \param lOrder The logical order of the item.
 * \return Constant pointer to the node key on the specified order. Warning: In the case of ri and coding, it returns the buffer.
 */
template<class TKey> inline const char* cTreeNode<TKey>::GetCKey(unsigned int lOrder, sItemBuffers* buffers, ushort lSubNodeOrder) const
{
	uint dsMode = mHeader->GetDStructMode();
	char* mem = GetItems() + GetNodeHeader()->GetLinkSize() + GetItemPOrder(lOrder);

	if (IsLeaf()) 
	{
		assert(dsMode == cDStructConst::DSMODE_DEFAULT || buffers != NULL);
		if ((dsMode == cDStructConst::DSMODE_RI || dsMode == cDStructConst::DSMODE_RICODING) && (lSubNodeOrder == USHRT_MAX))
		{
			int c = 3;
			//printf("pruser");
		}

		//assert(dsMode == cDStructConst::DSMODE_DEFAULT || dsMode == cDStructConst::DSMODE_CODING || lSubNodeOrder != USHRT_MAX);

		if (dsMode == cDStructConst::DSMODE_CODING)
		{
			TKey::Decode(mHeader->GetCodeType(), mem, buffers->codingBuffer, mHeader->GetKeyDescriptor());
			mem = buffers->codingBuffer;
		}
		else if (dsMode == cDStructConst::DSMODE_RI)
		{

			char* subNodeHeader = (lSubNodeOrder == USHRT_MAX) ? this->GetSubNodeHeaderByItem(lOrder) : this->GetSubNodeHeader(lSubNodeOrder);
			char* minRefItem = TSubNode::GetMinRefItem(this->GetItems(), subNodeHeader);
			mem = TKey::MergeTuple(TSubNode::GetMask(this->GetItems(), subNodeHeader), minRefItem, mem, buffers->riBuffer, mHeader->GetKeyDescriptor());
		}
		else if (dsMode == cDStructConst::DSMODE_RICODING)
		{
			char* subNodeHeader = (lSubNodeOrder == USHRT_MAX) ? this->GetSubNodeHeaderByItem(lOrder) : this->GetSubNodeHeader(lSubNodeOrder);
			char* minRefItem = TSubNode::GetMinRefItem(this->GetItems(), subNodeHeader);
			uint keyLength = GetKeyLength(mem, subNodeHeader);

			TKey::Decode(mHeader->GetCodeType(), mem, buffers->codingBuffer, mHeader->GetKeyDescriptor(), keyLength);
			
			mem = TKey::MergeTuple(TSubNode::GetMask(this->GetItems(), subNodeHeader), minRefItem, buffers->codingBuffer, buffers->riBuffer, mHeader->GetKeyDescriptor());
			
		}
	}
	return mem; 
}

/**
 * LEAF AND INNER NODE METHOD
 * It returns the pointer on the item, no transformation is processed, e.g. coding ...
 * \param lOrder The logical order of the item.
 * \return Pointer to the node item on the specified logical order. In the case of the leaf node, it corresponds
 *         to the pointer on the key.
*/
template<class TKey> inline char* cTreeNode<TKey>::GetItemPtr(unsigned int lOrder) const
{
	return GetItems() + GetItemPOrder(lOrder);
}

/**
 * LEAF AND INNER NODE METHOD
 * It returns the pointer on the key and no transofrmation is processed, e.g. coding ...
 * \param lOrder The logical order of the item.
 * \return Pointer to the node key on the specified order.
*/
template<class TKey> inline char* cTreeNode<TKey>::GetKeyPtr(unsigned int lOrder) const
{
	return GetItems() + GetNodeHeader()->GetLinkSize() + GetItemPOrder(lOrder);
}

/**
 * LEAF AND INNER NODE METHOD
 * It returns the pointer on the key, no transformation is processed, e.g. coding ...
 * \param pOrder The physical order of the key.
 * \return Pointer to the node key on the specified order.
 */
template<class TKey> inline char* cTreeNode<TKey>::GetKeyPtrPo(unsigned int pOrder) const
{
	return GetItems() + GetNodeHeader()->GetLinkSize() + pOrder;
}

/**
* LEAF NODE METHOD
* Return data of item with specified order
*/
template<class TKey>
inline char* cTreeNode<TKey>::GetData(const char* item)
{
	return (char*)item + TKey::GetSize(item, GetNodeHeader()->GetKeyDescriptor());
}

/**
 * LEAF NODE METHOD
 * Return data of item with specified order
 */
template<class TKey> 
inline char* cTreeNode<TKey>::GetData(unsigned int lOrder, sItemBuffers* buffers) const
{
	assert(IsLeaf());
	
	unsigned int dsMode = mHeader->GetDStructMode();
	char* key = GetItems() + GetItemPOrder(lOrder) + GetNodeHeader()->GetLinkSize();
	char* data;

	assert(dsMode == cDStructConst::DSMODE_DEFAULT || buffers != NULL);

	if (dsMode == cDStructConst::DSMODE_DEFAULT) 
	{
		data = key + GetNodeHeader()->GetKeySize();
	}
	else if (dsMode == cDStructConst::DSMODE_CODING)
	{
		unsigned int keySize = TKey::Decode(mHeader->GetCodeType(), key, buffers->codingBuffer, mHeader->GetKeyDescriptor());
		data = key + keySize;
	}
	else if (dsMode == cDStructConst::DSMODE_RI)
	{
		char* subNodeHeader = this->GetSubNodeHeaderByItem(lOrder);
		data = key + TKey::GetLSize(GetKeyLength(key, subNodeHeader), mHeader->GetKeyDescriptor());
	}
	else if (dsMode == cDStructConst::DSMODE_RICODING)
	{
		char* subNodeHeader = this->GetSubNodeHeaderByItem(lOrder);
		uint keyLength = GetKeyLength(key, subNodeHeader);

		unsigned int keySize = TKey::Decode(mHeader->GetCodeType(), key, buffers->codingBuffer, mHeader->GetKeyDescriptor(), keyLength);
		data = key + keySize;
	}

	return data;
}

/**
* \param order Byte order of the data in the node.
* \return Constant pointer to the node data on the specified order.
*/
template<class TKey> inline char* cTreeNode<TKey>::GetDataPo(unsigned int keySize, unsigned int pOrder) const
{
	return GetItems() + keySize + pOrder; 
}

template<class TKey> inline tItemOrder& cTreeNode<TKey>::GetItemPOrder(const tItemOrder& lOrder) const
{
	cTreeNodeHeader* header = GetNodeHeader();

	assert(header->GetItemOrderOffset() + lOrder + sizeof(tItemOrder) <= header->GetLinksOffset());
	return *(((tItemOrder*) (mData + header->GetItemOrderOffset())) + lOrder);
}

template<class TKey> inline char* cTreeNode<TKey>::GetPItemPOrder(const tItemOrder& lOrder) const
{
	cTreeNodeHeader* header = GetNodeHeader();

	assert(header->GetItemOrderOffset() + lOrder + sizeof(tItemOrder) <= header->GetLinksOffset());

	return (char*)(((tItemOrder*)(mData + header->GetItemOrderOffset())) + lOrder);
}

template<class TKey> inline void cTreeNode<TKey>::SetItemPOrder(const tItemOrder &lOrder, const tItemOrder &pOrder)
{
	cTreeNodeHeader* header = GetNodeHeader();

	assert(header->GetItemOrderOffset() + lOrder <= header->GetLinksOffset());

	*((tItemOrder*)GetPItemPOrder(lOrder)) = pOrder;
}

template<class TKey> inline void cTreeNode<TKey>::IncItemPOrder(const tItemOrder &lOrder, int incValue)
{
	cTreeNodeHeader* header = GetNodeHeader();

	assert(header->GetItemOrderOffset() + lOrder <= header->GetLinksOffset());
	(*((tItemOrder*)GetPItemPOrder(lOrder))) += incValue;
}

// LEAF AND INNER NODE METHOD 
// Method creates place for inserting new physical order on position lOrder and insert it 
template<class TKey> inline void cTreeNode<TKey>::InsertItemPOrder(const tItemOrder &lOrder, const tItemOrder &pOrder)
{
	if (lOrder != mItemCount)
	{
		tItemOrder *pItems = (tItemOrder *)GetPItemPOrder(lOrder);
		memmove((char*) (pItems + 1), (char*) pItems, (mItemCount - lOrder) * sizeof(tItemOrder));
	}
	SetItemPOrder(lOrder, pOrder);
}

template<class TKey> inline tNodeIndex cTreeNode<TKey>::GetLink(unsigned int lOrder) const 
{ 
	unsigned int position = GetNodeHeader()->GetItemsOffset() + GetItemPOrder(lOrder);
	return *((tNodeIndex*)(mData  + position));
}

template<class TKey> inline char* cTreeNode<TKey>::GetExtraItem(unsigned int order) const
{
	// mk: does not tested
	cTreeNodeHeader* header = GetNodeHeader();
	return (mData + header->GetExtraItemsOffset() + order * header->GetItemSize());
}

template<class TKey> inline tNodeIndex cTreeNode<TKey>::GetExtraLink(unsigned int order) const
{ 
	return *(((tNodeIndex*)(mData +  GetNodeHeader()->GetExtraLinksOffset())) + order);
}

/**
 * Methods for manipulating with index of leaf node. Leaf node's the most important bit has value one.
 **/
template<class TKey> inline bool cTreeNode<TKey>::IsLeaf(const tNodeIndex index)
{
	bool ret = false;
	if ((index & 0x80000000) != 0) 
	{
		ret = true;
	}
	return ret;
}

template<class TKey> inline tNodeIndex cTreeNode<TKey>::GetNodeIndex(const tNodeIndex leafIndex)
{
	return 0x7fffffff & leafIndex;
}

template<class TKey> inline tNodeIndex cTreeNode<TKey>::GetLeafNodeIndex(const tNodeIndex index)
{
	return index | 0x80000000;
}

template<class TKey> inline bool cTreeNode<TKey>::IsFull() const 
{
	return mItemCount == mHeader->GetNodeCapacity();
}

/**
 * Return if node is leaf.
 **/
template<class TKey> inline bool cTreeNode<TKey>::IsLeaf() const
{
	return *((bool*)(mData + ATTRIBUT_LEAF));
}

/**
 * Set node as leaf or non-leaf..
 */
template<class TKey> inline void cTreeNode<TKey>::SetLeaf(bool leaf)
{
	*((bool*)(mData + ATTRIBUT_LEAF)) = leaf;
}


template<class TKey> 
void cTreeNode<TKey>::CopyKeyTo(char* item, unsigned int order, sItemBuffers* buffers)
{
	  memcpy(item, GetCKey(order, buffers), GetNodeHeader()->GetKeySize());
}

//******************************************************************************************************************
//*****************************************  INSERT AND SPLIT  *****************************************************
//******************************************************************************************************************

/**
* Check whether a leaf node contains enough free space or not.
* \return
*	- true if the node contains enough free space
*	- false if there is not enough space or number of items in the node reached the maximum.
*/
template<class TKey> 
bool cTreeNode<TKey>::HasLeafFreeSpace(const char* key, const char* data) const
{
	assert(IsLeaf());
	unsigned int size = GetLeafItemSize(key, data);
	assert(!(mItemCount == mHeader->GetNodeCapacity() && (mFreeSize > size))); // Compression check, if node is really full or only ItemOrders array is full
	return (size <= mFreeSize) && (mItemCount < mHeader->GetNodeCapacity());
}

/**
* Check whether an inner node contains enough free space or not.
* \return
*	- true if the node contains enough free space
*	- false if there is not enough space or number of items in the node reached the maximum.
*/
template<class TKey> 
bool cTreeNode<TKey>::HasFreeSpace(const char* key) const
{
	assert(!IsLeaf());
	unsigned int size = GetNodeHeader()->GetKeySize() + sizeof(tNodeIndex);
	return (size <= mFreeSize) && (mItemCount < mHeader->GetNodeCapacity());
}

/**
 * This is only a draft of a delete method! It is necessary to merge neighbour nodes etc.
 */
template<class TKey> void cTreeNode<TKey>::DeleteLeafItem(const char* key, sItemBuffers* buffers = NULL)
{
	cTreeNodeHeader *header = GetNodeHeader();

	assert(mItemCount >= 1);
	assert(GetNodeHeader()->VariableLenDataEnabled() == false && TKey::LengthType == cDataType::LENGTH_FIXLEN);

	uint dstLOrder = FindOrder(key, FIND_E, buffers);

	ushort dstPOrder = GetItemPOrder(dstLOrder);
	ushort srcPOrder = 0, srcLOrder = 0;
	for (uint i = 0; i < mItemCount; i++)
	{
		ushort pOrder = GetItemPOrder(i);
		if (pOrder > srcPOrder)
		{
			srcPOrder = pOrder;
			srcLOrder = i;
		}
	}

	uint itemSize = header->GetItemSize();

	if (dstPOrder != srcPOrder)
	{
		uint keySize = header->GetKeySize();
		// change the last and deleted items
		SetLeafItemPo(dstPOrder, GetCKey(srcLOrder), GetData(srcLOrder, buffers), keySize, buffers);
		SetItemPOrder(srcLOrder, dstPOrder);
	}

	// move all p orders after the deleted item
	tItemOrder* p = (tItemOrder*)(mData + header->GetItemOrderOffset()) + dstLOrder;
	memmove(p, p + 1, (mItemCount - dstLOrder) * sizeof(tItemOrder));

	mItemCount--;
	mFreeSize += itemSize;
	mHeader->DecrementItemCount();
}

/**
 * LEAF NODE METHOD
 * Insert key into a leaf node, where the correct position is first searched by the cut interval method.
 * Return:
 *   - INSERT_YES 
 *   - INSERT_OVERFULL - it should be never returned in a new implementation
 *   - INSERT_EXIST
 **/
template<class TKey> int cTreeNode<TKey>::InsertLeafItem(const char* key, char* data, bool allowDuplicateKey, cNodeBuffers<TKey>* buffers)
{
	assert(IsLeaf());
	int ret;

	unsigned int dsMode = mHeader->GetDStructMode();

	assert(dsMode == cDStructConst::DSMODE_DEFAULT || buffers != NULL);

	switch(dsMode)
	{
	case cDStructConst::DSMODE_DEFAULT:
		ret = InsertLeafItem_default(key, data, allowDuplicateKey);
		break;
	case cDStructConst::DSMODE_CODING:
		ret = InsertLeafItem_default(key, data, allowDuplicateKey, &buffers->itemBuffer);
		break;
	case cDStructConst::DSMODE_RI:
	case cDStructConst::DSMODE_RICODING:
		ret = InsertLeafItem_ri(key, data, allowDuplicateKey, buffers);
		break;
	}

	if (ret != INSERT_NOSPACE)
	{
		mHeader->IncrementItemCount();
	}

	return ret;
}

/**
 * LEAF NODE METHOD
 * Insert key into a leaf node, where the correct position is first searched by the cut interval method.
 * Return:
 *   - INSERT_YES 
 *	 - INSERT_AT_THE_END
 *   - INSERT_EXIST
 *   - INSERT_NOSPACE
 **/
template<class TKey> int cTreeNode<TKey>::InsertLeafItem_default(const char* key, char* data, bool allowDuplicateKey, sItemBuffers* buffers)
{
	// find logical order of item
	int lOrder = FindItemOrder(key, allowDuplicateKey, buffers);
	if (lOrder == INSERT_EXIST)
	{
		return INSERT_EXIST;
	}

	// specify physical order of item and insert it 
	uint pOrder = mHeader->GetNodeItemsSpaceSize() - mFreeSize; // return free item in mItems
	uint itemSize = SetLeafItemPo(pOrder, key, data, cCommon::UNDEFINED_UINT, buffers);

	if (itemSize == INSERT_NOSPACE)
	{
		return INSERT_NOSPACE;
	}
	InsertItemPOrder(lOrder, pOrder);

	// update headers
	assert(mItemCount < mHeader->GetNodeCapacity());
	mItemCount++;
	assert(mFreeSize >= itemSize);
	mFreeSize -= itemSize;
	assert(mFreeSize <= mHeader->GetNodeItemsSpaceSize());

	return (lOrder != mItemCount - 1) ? INSERT_YES : INSERT_AT_THE_END;
}

/**
* Insert key into a leaf node, where the correct position is first searched by the cut interval method.
* Return:
* -INSERT_YES
* -INSERT_EXIST
* -INSERT_NOSPACE
**/
template<class TKey>
int cTreeNode<TKey>::AddLeafItem(const char* key, char* data, bool incFlag, cNodeBuffers<TKey>* buffers)
{
	assert(IsLeaf());
	int ret;

	uint dsMode = mHeader->GetDStructMode();
	assert(dsMode == cDStructConst::DSMODE_DEFAULT || buffers != NULL);

	switch (dsMode)
	{
	case cDStructConst::DSMODE_DEFAULT:
		ret = AddLeafItem_default(key, data);
		break;
	case cDStructConst::DSMODE_CODING:
		ret = AddLeafItem_default(key, data, &buffers->itemBuffer);
		break;
	case cDStructConst::DSMODE_RI:
	case cDStructConst::DSMODE_RICODING:
		ret = AddLeafItem_ri(key, data, incFlag, buffers);
		break;
	}

	if ((incFlag) && (ret != INSERT_NOSPACE))
	{
		mHeader->IncrementItemCount();
	}

	return ret;
}

/**
* LEAF NODE METHOD
* Add item at the end of the leaf node.
* \return false if node is is overfull.
*/
template<class TKey>
int cTreeNode<TKey>::AddLeafItem_default(const char* key, char* data, sItemBuffers* buffers)
{
	uint pOrder = mHeader->GetNodeItemsSpaceSize() - mFreeSize; // return free item in mItems
	uint itemSize = SetLeafItemPo(pOrder, key, data, cCommon::UNDEFINED_UINT, buffers);

	if (itemSize == INSERT_NOSPACE)
	{
		return INSERT_NOSPACE;
	}
	SetItemPOrder(mItemCount, pOrder);

	mItemCount++;
	assert(mFreeSize >= itemSize);
	mFreeSize -= itemSize;
	assert(mFreeSize <= mHeader->GetNodeItemsSpaceSize());

	return INSERT_YES;
}

/**
 * INNER NODE METHOD
 * Add item at the end of the inner node.
 * \return false if node is is overfull.
*/
template<class TKey>
bool cTreeNode<TKey>::AddItem(const char* key, const tNodeIndex &nodeIndex, bool incFlag)
{
	assert(!IsLeaf());

	unsigned int freeOrder = mHeader->GetNodeItemsSpaceSize() - mFreeSize;
	SetItemPOrder(mItemCount, freeOrder);
	unsigned int keySize = GetNodeHeader()->GetKeySize();

	// since the max size of the item in each inner node, we need this validation
	// assert (keySize <= TKey::GetSize(key, GetNodeHeader()->GetKeyDescriptor()));
	assert(keySize <= TKey::GetMaxSize(key, mHeader->GetKeyDescriptor()));     

	SetKeyPo(freeOrder, key /* 2M: Add keySize parameter */);
	SetLinkPo(freeOrder, nodeIndex);

	mItemCount++;
	assert(mFreeSize > keySize + sizeof(tNodeIndex));
	mFreeSize -= keySize + sizeof(tNodeIndex);
	assert(mFreeSize <= mHeader->GetNodeItemsSpaceSize());

	if (incFlag)
	{
		mHeader->IncrementItemCount();
	}

	return (mItemCount <= mHeader->GetNodeCapacity());
}

/**
 * INNER NODE METHOD
 * Insert key into an inner node, where the correct position is first searched by the cut interval method.
 * Return:
 *   - INSERT_YES 
 *   - INSERT_OVERFULL - it should be never returned in a new implementation
 *   - INSERT_EXIST
 **/
template<class TKey> int cTreeNode<TKey>::InsertItem(const char* key, const tNodeIndex childIndex, bool allowDuplicateKey)
{
	assert(!IsLeaf());
	unsigned int itemSize = GetNodeHeader()->GetKeySize() + sizeof(tNodeIndex);

	// find logical order of item
	int lOrder = FindItemOrder((char*)key, allowDuplicateKey);
	if (lOrder == INSERT_EXIST)
	{
		return INSERT_EXIST;
	}

	unsigned int pOrder = mHeader->GetNodeItemsSpaceSize() - mFreeSize;
	InsertItemPOrder(lOrder, pOrder);
	SetKeyPo(pOrder, key);
	SetLinkPo(pOrder, childIndex);

	mHeader->IncrementItemCount();
	assert(mItemCount < mHeader->GetNodeCapacity());
	mItemCount++;
	assert(mFreeSize > itemSize);
	mFreeSize -= itemSize;
	assert(mFreeSize <= mHeader->GetNodeItemsSpaceSize());

	return (lOrder != mItemCount - 1) ? cTreeNode<TKey>::INSERT_YES : cTreeNode<TKey>::INSERT_AT_THE_END;
}

/**
 * INNER NODE METHOD
 * Insert item and link at lOrder position in node, move other items and links.
 * \return false if the node is overfull.
 */
template<class TKey>
bool cTreeNode<TKey>::InsertItem(const unsigned int lOrder, const char *item, const tNodeIndex &childIndex)
{
	assert(!IsLeaf());
	cTreeNodeHeader *nodeHeader = GetNodeHeader();

	// since we have the max size of the item in each inner node, we need this validation
	assert(!IsLeaf() && nodeHeader->GetKeySize() >= TKey::GetSize(item, mHeader->GetKeyDescriptor()));

	uint itemSize = nodeHeader->GetKeySize() + sizeof(tNodeIndex);
	uint count = mItemCount - lOrder;

	if (count != 0)
	{
		// move item pointers and links
		char* pi1 = GetPItemPOrder(lOrder);
		char* pi2 = GetPItemPOrder(lOrder + 1);
		memmove(pi2, pi1, count * sizeof(tItemOrder));
	}

	// set new item and link
	unsigned int pOrder = mHeader->GetNodeItemsSpaceSize() - mFreeSize;
	SetKeyPo(pOrder, item);
	SetItemPOrder(lOrder, pOrder);
	SetLinkPo(pOrder, childIndex);

	mItemCount++;
	assert(mFreeSize > itemSize);
	mFreeSize -= itemSize;
	mHeader->IncrementItemCount();
	assert(mFreeSize <= mHeader->GetNodeItemsSpaceSize());

	return (mItemCount <= mHeader->GetNodeCapacity());
}


/**
 * Split this node into two nodes. Written for ordered DS.
 * \param newNode Newly created node.
 * \param tmpNode Temporary node used during the split (to reorder items in this node).
 **/
template<class TKey>
void cTreeNode<TKey>::Split(cTreeNode<TKey>& newNode, cTreeNode<TKey> &tmpNode, cNodeBuffers<TKey>* buffers)
{
	unsigned int dsMode = mHeader->GetDStructMode();

	assert(dsMode == cDStructConst::DSMODE_DEFAULT || buffers != NULL);

	if (dsMode == cDStructConst::DSMODE_DEFAULT || !IsLeaf())
	{
		Split_default(newNode, tmpNode);
	}
	else if (dsMode == cDStructConst::DSMODE_RI || dsMode == cDStructConst::DSMODE_RICODING)
	{
		SplitLeafNode_ri(newNode, tmpNode, buffers);
	}
	else if (dsMode == cDStructConst::DSMODE_CODING)
	{
		SplitLeafNode_coding(newNode, tmpNode, buffers);
	}
}

/**
 * Split this node into two nodes. Written for ordered DS.
 * \param newNode Newly created node.
 * \param tmpNode Temporary node used during the split (to reorder items in this node).
 **/
template<class TKey>
void cTreeNode<TKey>::Split_default(cTreeNode<TKey>& newNode, cTreeNode<TKey> &tmpNode)
{
	unsigned int i, halfCount = (mItemCount + 1)/2;
	unsigned int debug = false;
	unsigned int size, occupiedSpace;
	cTreeNodeHeader *nodeHeader = GetNodeHeader();

	size = 0;
	for (i = 0; i < halfCount; i++)
	{
		char* item = GetItemPtr(i);
		tmpNode.SetItemPOrder(i, size);
		unsigned int itemSize = GetItemSize(i);
		tmpNode.SetItemPo(size, item, itemSize);
		size += itemSize;
	}
	occupiedSpace = size;

	size = 0;
	for (unsigned int j = 0; i < mItemCount; j++, i++)
	{
		char* item = GetItemPtr(i);
		newNode.SetItemPOrder(j, size);
		unsigned int itemSize = GetItemSize(i);
		newNode.SetItemPo(size, item, itemSize);
		size += itemSize;
	}

	memcpy(mData + nodeHeader->GetItemOrderOffset(), tmpNode.GetPItemPOrder(0), halfCount * cTreeNodeHeader::ItemSize_ItemOrder);
	memcpy(mData + nodeHeader->GetItemsOffset(), tmpNode.GetItemPtr(0), occupiedSpace);

	newNode.SetItemCount(mItemCount - halfCount);
	SetItemCount(halfCount);

	mHeader->IncrementNodeCount();

	// compute free size
	assert(occupiedSpace <= mHeader->GetNodeItemsSpaceSize());
	assert(size <= mHeader->GetNodeItemsSpaceSize());
	mFreeSize = mHeader->GetNodeItemsSpaceSize() - occupiedSpace;
	newNode.SetFreeSize(newNode.mHeader->GetNodeItemsSpaceSize() - size);
}

//******************************************************************************************************************
//*****************************************       FIND  ORDER    ***************************************************
//******************************************************************************************************************
/**
* Find logical order of inserting item.
*/
template<class TKey> int cTreeNode<TKey>::FindItemOrder(const char* key, bool allowDuplicateKey, sItemBuffers* buffers, ushort lSubNodeOrder)
{
	int equal, ret;
	uint maxItem = (lSubNodeOrder != USHRT_MAX) ? TSubNode::GetLastItemOrder(this->GetSubNodeHeader(lSubNodeOrder)) : mItemCount - 1;

	if (mItemCount != 0 && (equal = TKey::Compare(key, GetCKey(maxItem, buffers, lSubNodeOrder), mHeader->GetKeyDescriptor())) < 0)
	{
		// find the position where the item should be inserted
		if (allowDuplicateKey)
		{
			ret = FindOrderInsert(key, FIND_SBE, buffers, lSubNodeOrder);
		}
		else
		{
			ret = FindOrderInsert(key, FIND_INSERT, buffers, lSubNodeOrder);
			if (ret == FIND_EQUAL)
			{
				ret = INSERT_EXIST;
			}
		}
	}
	else
	{
		if (mItemCount != 0 && (!allowDuplicateKey && equal == 0))
		{
			ret = INSERT_EXIST;
		}
		else
		{
			ret = maxItem + 1;
		}
	}

	return ret;
}

template<class TKey> int cTreeNode<TKey>::FindOrder(const char* item, int mode, sItemBuffers* buffers) const
{
  return FindOrder(item, mode, buffers, 0, mItemCount-1);
}

template<class TKey> int cTreeNode<TKey>::FindOrder(const char* item, int mode, sItemBuffers* buffers, int firstOrder, int lastOrder) const
{
	assert(mHeader->GetDStructMode() == cDStructConst::DSMODE_DEFAULT || buffers != NULL);

	int equal, ret;
	int mid = 0;
	assert(firstOrder <= lastOrder);

	// RB - metodu jsem prepsal jen pro FIND_SBE a FIND_E, snad jsem správně pochopil její fungování ...
	assert(mode == FIND_SBE || mode == FIND_E);

	if (mItemCount != 0)
	{
		/*
		Vašek nameril zhorseni poctu porovnani.
		// solve extreme cases
		if ((equal = TKey::Compare(item, GetCKey(firstOrder, buffers), mHeader->GetKeyDescriptor())) <= 0) 
		{
			if (mode == FIND_E && equal == 0 || mode == FIND_SBE)
			{
				return firstOrder;
			}
			return FIND_NOTEXIST;
		}
		else if ((equal = TKey::Compare(item, GetCKey(lastOrder, buffers), mHeader->GetKeyDescriptor())) >= 0)
		{
			if (equal == 0)
			{
				return lastOrder;
			}
			return FIND_NOTEXIST;			
		}
		*/

		// main cut interval algorithm
		do
		{
			mid = (firstOrder + lastOrder) / 2;

			if ((equal = TKey::Compare(item, GetCKey(mid, buffers), mHeader->GetKeyDescriptor())) == 0)
			{
				return mid;
			}

			if (equal == -1)
			{
				lastOrder = mid-1;
			}
			else
			{
				firstOrder = mid+1;
				if (firstOrder > lastOrder)
				{
					mid++;
				}
			}
		}
		while(firstOrder <= lastOrder);

		ret = mid;
	} 
	else
	{
		ret = FIND_NOTEXIST;
	}

	if (mode == FIND_E)
	{
		return FIND_NOTEXIST;
	}
	return ret;
}

/**
 * Find order of item according mode.
 * \param mode The mode can be: FIND_SBE - find smalest bigger or equal item, FIND_E - find equal item).
 * \return
 *		- The position of the item in the node.
 *		- FIND_NOTEXIST if the mode if FIND_SBE and the item is bigger then the biggest item in the node or the mode is FIND_E and the item is not found.
 */
template<class TKey> int cTreeNode<TKey>::FindOrder(const TKey& item, int mode, sItemBuffers* buffers) const
{
	unsigned int dsMode = mHeader->GetDStructMode();
	int ret;

	if ((dsMode == cDStructConst::DSMODE_RI || dsMode == cDStructConst::DSMODE_RICODING) && (IsLeaf()))
	{
		ret = FindOrder_ri(item, mode, buffers);
	}
	else
	{
		ret = FindOrder_default(item, mode, buffers, 0);
	}
	return ret;
}

/**
 * Find order of item according mode.
 * \param mode The mode can be: FIND_SBE - find smalest bigger or equal item, FIND_E - find equal item).
 * \return
 *		- The position of the item in the node.
 *		- FIND_NOTEXIST if the mode if FIND_SBE and the item is bigger then the biggest item in the node or the mode is FIND_E and the item is not found.
 */
template<class TKey> int cTreeNode<TKey>::FindOrder_default(const TKey& key, int mode, sItemBuffers* buffers, int lo) const
{
	int lastItemOrder = mItemCount - 1, equal, ret;
	int mid = 0;
	bool find = false;
	int hi = (int)mItemCount - 1;
	const cDTDescriptor* dtd = mHeader->GetKeyDescriptor();

	// RB - metodu jsem prepsal jen pro FIND_SBE a FIND_E, snad jsem správně pochopil její fungování ...
	assert(mode == FIND_SBE || mode == FIND_E);

	if (mItemCount != 0)
	{
		// solve extreme cases
		if ((equal = key.Compare(GetCKey(0, buffers), dtd)) > 0)
		{
			if ((equal = key.Compare(GetCKey(lastItemOrder, buffers), dtd)) < 0)
			{
				// main cut interval algorithm
				do
				{
					mid = (lo + hi) / 2;

					if ((equal = key.Compare(GetCKey(mid, buffers), dtd)) != 0)
					{
						if (equal == -1)
						{
							hi = mid - 1;
						}
						else
						{
							lo = mid + 1;
							if (lo > hi)
							{
								mid++;
							}
						}
					}
					else

					{
						find = true;
						break;
					}

				} while (lo <= hi);

				ret = mid;
			}
			else
			{
				if (equal == 0)
				{
					ret = lastItemOrder;
					find = true;
				}
				else
				{
					return FIND_NOTEXIST;
				}


			}
		}
		else
		{
			if (mode == FIND_E && equal == 0 || mode == FIND_SBE)
			{
				return 0;
			}
			else

			{
				return FIND_NOTEXIST;
			}

		}
	}
	else
	{
		ret = FIND_NOTEXIST;
	}

	if (mode == FIND_E && !find)
	{
		ret = FIND_NOTEXIST;
	}
	else if (mode == FIND_SBE && mHeader->DuplicatesAllowed())
	{
		// in the case of duplicated keys, you must find the first key
		while (ret != 0 && (key.Compare(GetCKey(ret - 1, buffers), dtd) == 0))
		{
			ret--;
		}
	}

	return ret;
}

/**
 * Find order of the key in the node according mode.
 * \param mode The mode can be: FIND_SBE - find smalest bigger or equal key;  FIND_INSERT - find smalest bigger key.
 * \return
 *		- The position in the node where the key should be inserted.
 *		- FIND_EQUAL if the mode is FIND_INSERT and the key already exists in the node.
 *		- FIND_NOTEXIST if the mode if FIND_SBE and the key is bigger then the biggest key in the node.
 */
template<class TKey> int cTreeNode<TKey>::FindOrderInsert(const char* key, int mode, sItemBuffers* buffers, ushort lSubNodeOrder) const
{
	// if you are doing any changes in the method do the same changes in the second InsertFindorder method as well
	int equal = 0;
	int mid = 0;
	//int lo = 1;
	//int hi = (int)mItemCount - 1;
	int lo = (lSubNodeOrder != USHRT_MAX) ? TSubNode::GetFirstItemOrder(this->GetSubNodeHeader(lSubNodeOrder)) + 1 : 1;
	int hi = (lSubNodeOrder != USHRT_MAX) ? TSubNode::GetLastItemOrder(this->GetSubNodeHeader(lSubNodeOrder)) : (int)mItemCount - 1;
	int lastItemOrder = hi;

	// RB - metodu jsem prepsal jen pro FIND_SBE a FIND_INSERT, snad jsem správně pochopil její fungování ...
	assert(mode == FIND_SBE || mode == FIND_INSERT);

	if (mItemCount != 0)
	{
		// solve extreme cases
		if ((equal = TKey::Compare(key, GetCKey(/*0*/lo - 1, buffers, lSubNodeOrder), mHeader->GetKeyDescriptor())) <= 0)
		{
			if (mode == FIND_INSERT && equal == 0)
			{
				return FIND_EQUAL;
			}
			return lo - 1;// 0;
		}
		else if ((equal = TKey::Compare(key, GetCKey(lastItemOrder, buffers, lSubNodeOrder), mHeader->GetKeyDescriptor())) >= 0)
		{
			if (equal == 0)
			{
				if (mode == FIND_INSERT)
				{
					return FIND_EQUAL;
				}
				return lastItemOrder;
			}
			else
			{
				if (mode == FIND_SBE)
				{
					return FIND_NOTEXIST;
				}
				return mItemCount;
			}
		}

		// main cut interval algorithm
		do
		{
			mid = (lo + hi) / 2;

			if ((equal = TKey::Compare(key, GetCKey(mid, buffers, lSubNodeOrder), mHeader->GetKeyDescriptor())) == 0)
			{
				if (mode == FIND_INSERT)
				{
					return FIND_EQUAL;
				}
				else
				{
					return mid;
				}
			}

			if (equal == -1)
			{
				hi = mid-1;
			}
			else
			{
				lo = mid+1;
				if (lo > hi)
				{
					mid++;
				}
			}
		}
		while(lo <= hi);

		return mid;
	} 
	else
	{
		return 0;
	}
}



//******************************************************************************************************************
//*****************************************       READ WRITE     ***************************************************
//******************************************************************************************************************

/**
 * Serialize node into the stream.
 **/
template<class TKey> 
void cTreeNode<TKey>::Write(cStream* stream) const
{
	unsigned int itemCount, fanoutCount, extraItemCount, extraLinkCount;
	char decodedKeyBuffer[60]; // only for GetNodeHeader()->GetDStructMode() == cDStructConst::DSMODE_CODING
	/*sItemBuffers buff;
	buff.codingBuffer = decodedKeyBuffer;
	buff.riBuffer = decodedKeyBuffer + 30;
	if (IsLeaf())
	{
	Print2File("nodes.txt", false, &buff);
	}*/


	if (IsLeaf())
	{
		itemCount = GetLeafNodeIndex(mItemCount);	//mTreeHeader->GetNodeItemCapacity();	//GetLeafIndex(mItemCount);
		if (GetNodeHeader()->GetNodeFanoutCapacity() == 0)
		{
			fanoutCount = 0;
		}
		else
		{
			fanoutCount = mItemCount + GetNodeHeader()->GetNodeDeltaCapacity();
		}
		extraItemCount = GetNodeHeader()->GetNodeExtraItemCount();
		extraLinkCount = GetNodeHeader()->GetNodeExtraLinkCount();
	}
	else
	{
		itemCount = mItemCount;
		fanoutCount = mItemCount + GetNodeHeader()->GetNodeDeltaCapacity();
		extraItemCount = GetNodeHeader()->GetNodeExtraItemCount();
		extraLinkCount = GetNodeHeader()->GetNodeExtraLinkCount();
	}


	// mRealSize is not used, Why? But it seems that is it ok.
	stream->Write((char* )&itemCount, sizeof(itemCount));
	stream->Write((char* )&mFreeSize, sizeof(mFreeSize));

	if ((GetNodeHeader()->GetDStructMode() == cDStructConst::DSMODE_RI || GetNodeHeader()->GetDStructMode() == cDStructConst::DSMODE_RICODING) &&  (IsLeaf()))
	{
		unsigned int sizeOfRest = GetNodeHeader()->GetNodeInMemSize() - GetNodeHeader()->GetExtraItemsOffset();

		stream->Write(mData, GetNodeHeader()->GetNodeSerialSize() - cTreeNodeHeader::NODE_PREFIX_SERIAL - sizeOfRest);

		if (GetNodeHeader()->GetNodeDeltaCapacity() != -1)
		{
			stream->Write(mData + GetNodeHeader()->GetLinksOffset(), GetNodeHeader()->GetNodeDeltaCapacity() * sizeof(tNodeIndex)); // write links
		}
		stream->Write(mData + GetNodeHeader()->GetExtraItemsOffset(),  sizeOfRest);// extra items, extra links

		return;
	}

	if (IsLeaf())
	{
		if (GetNodeHeader()->VariableLenDataEnabled())
		{
			for(unsigned int i = 0; i < mItemCount ; i++)
			{
				unsigned int position = GetItemPOrder(i);
				unsigned int keySize;

				if (GetNodeHeader()->GetDStructMode() == cDStructConst::DSMODE_CODING)
				{
					keySize = TKey::Decode(GetNodeHeader()->GetCodeType(), GetKeyPtrPo(position), decodedKeyBuffer, GetNodeHeader()->GetKeyDescriptor());
				}
				else
				{
					keySize = TKey::GetSize(GetKeyPtrPo(position), GetNodeHeader()->GetKeyDescriptor());
				}
				unsigned int dataSize = GetDataSize(GetDataPo(keySize, position));
				stream->Write((char*)GetItemPtr(i), keySize + dataSize);
			}
		} else
		{
			for(unsigned int i = 0; i < mItemCount ; i++)
			{
				unsigned int keySize;

				if (GetNodeHeader()->GetDStructMode() == cDStructConst::DSMODE_CODING)
				{
					keySize = TKey::Decode(GetNodeHeader()->GetCodeType(), GetKeyPtr(i), decodedKeyBuffer, GetNodeHeader()->GetKeyDescriptor());
				}
				else
				{
					keySize = TKey::GetSize(GetKeyPtr(i), GetNodeHeader()->GetKeyDescriptor());
				}

				stream->Write((char*)GetItemPtr(i), keySize + GetNodeHeader()->GetDataSize());
			}
		}
	} else
	{
		for(unsigned int i = 0; i < mItemCount ; i++)
		{
			stream->Write((char*)GetItemPtr(i), GetNodeHeader()->GetItemSize());
		}
	}

	unsigned int sizeOfRest = GetNodeHeader()->GetNodeInMemSize() - GetNodeHeader()->GetExtraItemsOffset();
	if (sizeOfRest != 0)
	{
		stream->Write(mData + GetNodeHeader()->GetExtraItemsOffset(),  sizeOfRest); // extra items, extra links
	}
}

/**
 * Read node from the stream. Note that writing the node into stream by write method
 * and subsequent reading causes ordering of the items.
 * \param stream Stream array, must be a type cCharStream!
 **/
template<class TKey> void cTreeNode<TKey>::Read(cStream* stream)
{
	unsigned int itemCount, fanoutCount, extraItemCount, extraLinkCount;
	unsigned int itemOffset = 0;
	unsigned int bufferSize = 120;
	char decodedKeyBuffer[120]; // only for GetNodeHeader()->GetDStructMode() == cDStructConst::DSMODE_CODING or DSMODE_RICODING

	// mRealSize is not used, Why? But it seems that is it ok.
	stream->Read((char*)&itemCount, sizeof(itemCount));
	stream->Read((char* )&mFreeSize, sizeof(mFreeSize));
	SetLeaf(IsLeaf(itemCount));

	if ((IsLeaf()) == true)
	{
		mItemCount = GetNodeIndex(itemCount);
		if (GetNodeHeader()->GetNodeFanoutCapacity() == 0)
		{
			fanoutCount = 0;
		}
		else
		{
			fanoutCount = mItemCount + GetNodeHeader()->GetNodeDeltaCapacity();
		}
		extraItemCount = GetNodeHeader()->GetNodeExtraItemCount();
		extraLinkCount = GetNodeHeader()->GetNodeExtraLinkCount();
	}
	else
	{
		mItemCount = itemCount;
		fanoutCount = mItemCount + GetNodeHeader()->GetNodeDeltaCapacity();
		extraItemCount = GetNodeHeader()->GetNodeExtraItemCount();
		extraLinkCount = GetNodeHeader()->GetNodeExtraLinkCount();
	}

	if ((GetNodeHeader()->GetDStructMode() == cDStructConst::DSMODE_RI || GetNodeHeader()->GetDStructMode() == cDStructConst::DSMODE_RICODING) &&  (IsLeaf()))
	{
		unsigned int sizeOfRest = GetNodeHeader()->GetNodeInMemSize() - GetNodeHeader()->GetExtraItemsOffset();

		stream->Read(mData, GetNodeHeader()->GetNodeSerialSize() - cTreeNodeHeader::NODE_PREFIX_SERIAL - sizeOfRest);

		//printf("%d\n", mIndex);
		unsigned int firstByte = 0;
		for (ushort i = 0; i < this->GetSubNodesCount(); i++)
		{
			char* subNodeHeader = this->GetSubNodeHeader(i);
			ushort firstByte = TSubNode::GetSubNodePOrder(subNodeHeader);
			firstByte += cBitString::ByteSize(TKey::GetLength(TSubNode::GetMinRefItem(this->GetItems(), subNodeHeader), GetNodeHeader()->GetKeyDescriptor()));
			firstByte += TKey::GetSize(TSubNode::GetMinRefItem(this->GetItems(), subNodeHeader), GetNodeHeader()->GetKeyDescriptor());
			firstByte += TKey::GetSize(TSubNode::GetMaxRefItem(this->GetItems(), subNodeHeader), GetNodeHeader()->GetKeyDescriptor());

			if (GetNodeHeader()->GetDStructMode() == cDStructConst::DSMODE_RI)
			{
				for (unsigned int j = TSubNode::GetFirstItemOrder(subNodeHeader); j <= TSubNode::GetLastItemOrder(subNodeHeader); j++)
				{
					SetItemPOrder(j, firstByte);
					firstByte += TKey::GetLSize(GetKeyLength(mData + GetNodeHeader()->GetItemsOffset() + firstByte, subNodeHeader), GetNodeHeader()->GetKeyDescriptor());
					firstByte += GetDataSize(mData + GetNodeHeader()->GetItemsOffset() + firstByte);
				}

			}
			else
			{
				for (unsigned int j = TSubNode::GetFirstItemOrder(subNodeHeader); j <= TSubNode::GetLastItemOrder(subNodeHeader); j++)
				{
					SetItemPOrder(j, firstByte);
					uint keyLength = GetKeyLength(mData + GetNodeHeader()->GetItemsOffset() + firstByte, subNodeHeader);

					TKey::Decode(GetNodeHeader()->GetCodeType(), mData + GetNodeHeader()->GetItemsOffset() + firstByte, decodedKeyBuffer, GetNodeHeader()->GetKeyDescriptor(), keyLength);
					firstByte += TKey::Encode(GetNodeHeader()->GetCodeType(), decodedKeyBuffer, decodedKeyBuffer + bufferSize/2, GetNodeHeader()->GetKeyDescriptor(), keyLength);
					firstByte += GetDataSize(mData + GetNodeHeader()->GetItemsOffset() + firstByte);
				}

			}
		}
		
		if (GetNodeHeader()->GetNodeDeltaCapacity() != -1)
		{
			stream->Read(mData + GetNodeHeader()->GetLinksOffset(), GetNodeHeader()->GetNodeDeltaCapacity() * sizeof(tNodeIndex)); // write links
		}
		stream->Read(mData + GetNodeHeader()->GetExtraItemsOffset(),  sizeOfRest);// extra items, extra links
		

		return;
	}

	// TODO - nacitani dat uzlu podle mne muzeme prepsat na nacteni celeho bloku; ale jen v pripade ze neni promenliva delka zaznamu
	if (IsLeaf())
	{
		if (GetNodeHeader()->VariableLenDataEnabled())
		{
			for(unsigned int i = 0; i < mItemCount ; i++)
			{
				char *data = ((cCharStream*)stream)->GetCharArray();
				unsigned int keySize;

				if (GetNodeHeader()->GetDStructMode() == cDStructConst::DSMODE_CODING)
				{
					keySize = TKey::Decode(GetNodeHeader()->GetCodeType(), data, decodedKeyBuffer, GetNodeHeader()->GetKeyDescriptor());
				}
				else
				{
					keySize = TKey::GetSize(data, GetNodeHeader()->GetKeyDescriptor());
				}

				unsigned int dataSize = GetDataSize(data + keySize);
				SetItemPOrder(i, itemOffset);
				stream->Read(mData + GetNodeHeader()->GetItemsOffset() + itemOffset, keySize + dataSize);
				itemOffset += keySize + dataSize;
			}
		} else
		{
			for(unsigned int i = 0; i < mItemCount ; i++)
			{
				char *data = ((cCharStream*)stream)->GetCharArray();
				unsigned int keySize;

				if (GetNodeHeader()->GetDStructMode() == cDStructConst::DSMODE_CODING)
				{
					keySize = TKey::Decode(GetNodeHeader()->GetCodeType(), data, decodedKeyBuffer, GetNodeHeader()->GetKeyDescriptor());
				}
				else
				{
					keySize = TKey::GetSize(data, GetNodeHeader()->GetKeyDescriptor());
				}

				SetItemPOrder(i, itemOffset);
				stream->Read(mData + GetNodeHeader()->GetItemsOffset() + itemOffset, keySize + GetNodeHeader()->GetDataSize());
				itemOffset += keySize + GetNodeHeader()->GetDataSize();
			}
		}
	} else
	{
		for(unsigned int i = 0; i < mItemCount ; i++)
		{
			SetItemPOrder(i, i * GetNodeHeader()->GetItemSize());
			stream->Read(GetItemPtr(i), GetNodeHeader()->GetItemSize());
		}
	}

	unsigned int sizeOfRest = GetNodeHeader()->GetNodeInMemSize() - GetNodeHeader()->GetExtraItemsOffset();
	if (sizeOfRest != 0)
	{
		stream->Read(mData + GetNodeHeader()->GetExtraItemsOffset(),  sizeOfRest); // extra items, extra links
	}

}


//******************************************************************************************************************
//*****************************************         DEBUG        ***************************************************
//******************************************************************************************************************

/**
* Test method. Only for debug purposes.
* \return true if the node is ordered (a consistence test for e.g. B-tree)
*/
template<class TKey> bool cTreeNode<TKey>::IsOrdered() const
{
	bool ret = true;
	for (unsigned int i = 0 ; i < mItemCount-1; i++)
	{
		if (TKey::Compare(GetCItem(i), GetCItem(i+1), true) > 0)
		{
			ret = false;
			printf("Critical Error: cTreeNode<TKey>::IsOrdered(): Node is not ordered!\n");
			break;
		}
	}
	return ret;
}



/**
 * Checks the node consistency. Just for debug purposes.
 * \return false when the item are not in the ascendent order.
 **/
template <class TKey>
bool cTreeNode<TKey>::CheckNode() const
{
	for (unsigned int i = 1; i < mItemCount; i++)
	{
		if (TKey::Compare(GetCItem(i - 1), GetCItem(i), GetNodeHeader()->GetKeyDescriptor()) != -1)
		{
			return false;
		}
	}
	if (!IsLeaf())
	{
		// tento cyklus je jen pro vnitrni uzly, ktere ukazuji na listy
		for (unsigned int i = 0; i < mItemCount; i++)
		{
			if (!IsLeaf(GetLink(i)))
			{
				return false;
			}
		}
	}
	if (IsLeaf())
	{
		if (GetExtraLink(0) == 0 || GetExtraLink(1) == 0)
		{
			return false;
		}
	}
	return true;
}

/**
 * Method, which prints contents of the node.
 **/
template <class TKey>
void cTreeNode<TKey>::Print(sItemBuffers* buffers) const
{
	printf("\n Node || ");
	printf("%d, count: %d", mIndex, mItemCount);
	printf(" (%s) || \n", (IsLeaf()?"leaf":"inner"));

	for (unsigned int i = 0 ; i < mItemCount ; i++) 
	{
		//printf("| *");
		printf("(%d: %d)", i, GetItemPOrder(i));
		printf("(%d + %d)", GetItemSize(GetCKey(i, buffers)), GetNodeHeader()->GetLinkSize());

		TKey::Print(GetCKey(i, buffers), "\t", mHeader->GetKeyDescriptor());
		
		if (!IsLeaf())
		{
			printf("L:%X ", GetLink(i));
		}

		 printf("\n");
	}

	// B-tree specific
	if (IsLeaf()) 
	{
		printf(" | prev: ");
		printf("%d", GetExtraLink(0));
		printf("| next:");
		printf("%d", GetExtraLink(1));
		printf(" ");
	}
	/*printf("| max: ");
	mExtraItems[0].Print(mode);*/
	printf(" |\n");
}


template <class TKey>
void cTreeNode<TKey>::Print2File(char* fileName) const
{
	FILE *streamInfo = fopen(fileName, "a");

	fprintf(streamInfo,"|| ");
	fprintf(streamInfo,"%d, count: %d", mIndex, mItemCount);
	fprintf(streamInfo," (%s) || \n", (IsLeaf()?"leaf":"inner"));

	for (unsigned int i = 0 ; i < mItemCount ; i++) 
	{

		//printf("| *");
		fprintf(streamInfo,"(%d)", GetItemPOrder(i));
		TKey::Print2File(streamInfo, GetCKey(i), " ", mHeader->GetKeyDescriptor());
		if (!IsLeaf())
		{
			fprintf(streamInfo,"L:%X ", GetLink(i));
		}
		fprintf(streamInfo,"\n");
	}

	// B-tree specific
	if (IsLeaf()) 
	{
		fprintf(streamInfo," | prev: ");
		fprintf(streamInfo,"%d", GetExtraLink(0));
		fprintf(streamInfo,"| next:");
		fprintf(streamInfo,"%d", GetExtraLink(1));
		fprintf(streamInfo," ");
	}
	/*printf("| max: ");
	mExtraItems[0].Print(mode);*/
	fprintf(streamInfo," |\n");

	fclose(streamInfo);
}

template <class TKey>
void cTreeNode<TKey>::Copy(cTreeNode<TKey> * source)
{
	memcpy(mData + GetNodeHeader()->GetItemsOffset(), 
		source->GetDataMemory() + source->GetNodeHeader()->GetItemsOffset(), 
		source->GetItemCount() * mHeader->GetItemSize());
		
	memcpy(mData + GetNodeHeader()->GetItemOrderOffset(), 
		source->GetDataMemory() + source->GetNodeHeader()->GetItemOrderOffset(), 
		source->GetItemCount() * sizeof(cTreeNodeHeader::ItemSize_ItemOrder));
	
	if (!IsLeaf())
	{
		memcpy(mData + GetNodeHeader()->GetLinksOffset(), 
			source->GetDataMemory() + source->GetNodeHeader()->GetLinksOffset(), 
			source->GetItemCount() * sizeof(cTreeNodeHeader::ItemSize_Links));
	}

	memcpy(mData + GetNodeHeader()->GetExtraItemsOffset(), 
		source->GetDataMemory() + source->GetNodeHeader()->GetExtraItemsOffset(), 
		source->GetNodeHeader()->GetNodeExtraItemCount() * sizeof(tNodeIndex));
	memcpy(mData + GetNodeHeader()->GetExtraLinksOffset(), 
		source->GetDataMemory() + source->GetNodeHeader()->GetExtraLinksOffset(), 
		source->GetNodeHeader()->GetNodeExtraLinkCount() * sizeof(tNodeIndex));
	// TODO - spravne nastavit mFreeSpace
}

template<class TKey> 
void cTreeNode<TKey>::SwapItemOrder(int a, int b)
{
	if (a != b)
	{
		tItemOrder pOrder = GetItemPOrder(b);		

		SetItemPOrder(b, GetItemPOrder(a));
		SetItemPOrder(a, pOrder);
	}
}

#include "dstruct/paged/core/cTreeNode_ri.h"
#include "dstruct/paged/core/cTreeNode_rebuild.h"
#include "dstruct/paged/core/cTreeNode_coding.h"

}}}
#endif   //  __cTreeNode_h__
