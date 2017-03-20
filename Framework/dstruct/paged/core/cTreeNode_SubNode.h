/**
*	\file cTreeNode_SubNode.h
*	\author Peter Chovanec
*	\version 0.1
*	\date june 2012
*	\brief part of node represents reference item and its items. It contains RI Header, RI and items
*/

#include "dstruct/paged/core/cTreeNodeHeader.h"
#include "dstruct/paged/core/sCoverRecord.h"
#include "common/datatype/tuple/cMBRectangle.h"

#ifndef __cTreeNode_SubNode_h__
#define __cTreeNode_SubNode_h__

using namespace common::datatype::tuple;

namespace dstruct {
  namespace paged {
	namespace core {

class cTreeNodeHeader;

template<class TKey>
class cTreeNode_SubNode
{
public:
	// SubNodePOrder, FirstItemOrder, LastItemOrder, SubNodeLastByte, MinRefItemPosition, MaxRefItemPosition
	static const uint HEADER_SIZE =  // RI Block header size
					  4 * sizeof(ushort) + 2 * sizeof(uchar); // subnode physical order, itemOrderStart, itemOrderEnd, lastbyte, minRefItem, maxRefItem
public:
	static const unsigned short NOT_DEFINED = 65535;

public:

	static inline ushort GetSubNodePOrder(char* subNodeHeader);
	static inline void SetSubNodePOrder(char* subNodeHeader, ushort pSubNodeOrder);
	static inline void IncSubNodePOrder(char* subNodeHeader, ushort incValue);

	static inline ushort GetFirstItemOrder(char* subNodeHeader);
	static inline void SetFirstItemOrder(char* subNodeHeader, ushort firstItemOrder);
	static inline void IncFirstItemOrder(char* subNodeHeader, ushort incValue = 1);
	static inline ushort GetLastItemOrder(char* subNodeHeader);
	static inline void SetLastItemOrder(char* subNodeHeader, ushort lastItemOrder);
	static inline void IncLastItemOrder(char* subNodeHeader, ushort incValue = 1);

	static inline ushort GetLastByte(char* subNodeHeader);
	static inline void SetLastByte(char* subNodeHeader, ushort lastByte);
	static inline void IncLastByte(char* subNodeHeader, ushort incValue);

	static inline uchar GetMinRefItemPosition(char* subNodeHeader);
	static inline void SetMinRefItemPosition(char* subNodeHeader, uchar minRefItemPosition);
	static inline uchar GetMaxRefItemPosition(char* subNodeHeader);
	static inline void SetMaxRefItemPosition(char* subNodeHeader, uchar maxRefItemPosition);

	static inline uint GetItemsCount(char* subNodeHeader);

	static inline char* GetMask(char* data, char* subNodeHeader);
	static inline char* GetMinRefItem(char* data, char* subNodeHeader);
	static inline char* GetMaxRefItem(char* data, char* subNodeHeader);
	static inline void SetMaxRefItem(char* data, char* subNodeHeader, const char* maxRefItem, ushort itemSize);

	static inline char* GetSubNodeHeader(char* subNodeHeaders, ushort lOrder);
	static inline char* GetSubNode(char* subNodes, char* subNodeHeaders, ushort lOrder);
	static inline char* GetSubNodeHeader(char* subNodeHeaders, ushort itemOrder, ushort subNodesCount);
	static inline char* GetSubNode(char* data, char* subNodeHeaders, ushort itemOrder, ushort subNodesCount);
	static inline ushort GetSubNodeOrder(char* subNodeHeaders, ushort itemOrder, ushort subNodesCount);

	static inline ushort TotalFreeSize(char* subNodeHeaders, ushort subNodesOffset, ushort subNodesCount);
	// static inline bool HasFreeSpace(char* subNodeHeaders, ushort lOrder, ushort subNodesCount);
	static inline ushort FreeSize(char* subNodeHeaders, ushort subNodesOffset, ushort lOrder, ushort subNodesCount);
	static inline ushort CompleteSize(char* subNodeHeaders, ushort subNodesOffset, ushort lOrder, ushort subNodesCount);

	static inline char* CreateSubNode(char* subNodes, char* subNodesHeaders, ushort lOrder, ushort pOrder, ushort startItemOrder, const char* refItem, const cDTDescriptor* keyDescriptor);
	static inline char* CreateSubNode(char* subNodes, char* subNodesHeaders, ushort lOrder, ushort pOrder, sCoverRecord* snRecord, const cDTDescriptor* keyDescriptor);
	static inline char* CreateSubNode(char* subNodes, char* subNodesHeaders, ushort lOrder, ushort pOrder, ushort startItemOrder, char* mask, char* mbr, const cDTDescriptor* keyDescriptor);

	static inline void Shift(char* subNodes, short shift, ushort startByte, ushort nextSubNodePOrder);
	static inline char* ShiftSubNode(char* subNode, short shift, ushort completeSize);

	static inline void UpdateItemOrderIntervals(char* subNodesHeaders, ushort startLOrder, short shift, ushort subNodesCount);
	static inline ushort MaskWeight(char* subNode, ushort maskLength);

};

template<class TKey>
inline ushort cTreeNode_SubNode<TKey>::GetSubNodePOrder(char* subNodeHeader)
{
	return *(ushort *)subNodeHeader;
}

template<class TKey>
inline void cTreeNode_SubNode<TKey>::SetSubNodePOrder(char* subNodeHeader, ushort pSubNodeOrder)
{
	*(ushort *)(subNodeHeader) = pSubNodeOrder;
}

template<class TKey>
inline void cTreeNode_SubNode<TKey>::IncSubNodePOrder(char* subNodeHeader, ushort incValue)
{
	(*(ushort *)(subNodeHeader)) += incValue;
}

template<class TKey>
inline ushort cTreeNode_SubNode<TKey>::GetFirstItemOrder(char* subNodeHeader)
{
	return *(ushort *)(subNodeHeader + sizeof(ushort));
}

template<class TKey>
inline void cTreeNode_SubNode<TKey>::SetFirstItemOrder(char* subNodeHeader, ushort firstItemOrder)
{
	*(ushort *)(subNodeHeader + sizeof(ushort)) = firstItemOrder;
}

template<class TKey>
inline void cTreeNode_SubNode<TKey>::IncFirstItemOrder(char* subNodeHeader, ushort incValue)
{
	(*(ushort *)(subNodeHeader + sizeof(ushort))) += incValue;
}

template<class TKey>
inline ushort cTreeNode_SubNode<TKey>::GetLastItemOrder(char* subNodeHeader)
{
	return *(ushort *)(subNodeHeader + (2 * sizeof(ushort)));
}

template<class TKey>
inline void cTreeNode_SubNode<TKey>::SetLastItemOrder(char* subNodeHeader, ushort lastItemOrder)
{
	*(unsigned short *)(subNodeHeader + (2 * sizeof(ushort))) = lastItemOrder;
}

template<class TKey>
inline void cTreeNode_SubNode<TKey>::IncLastItemOrder(char* subNodeHeader, ushort incValue)
{
	(*(ushort *)(subNodeHeader + (2 * sizeof(ushort)))) += incValue;
}

// Returns last byte of the specified RI Block
template<class TKey>
inline ushort cTreeNode_SubNode<TKey>::GetLastByte(char* subNodeHeader)
{
	return *(ushort *)(subNodeHeader + (3 * sizeof(ushort)));
}

// Set last byte of the specified RI Block
template<class TKey>
inline void cTreeNode_SubNode<TKey>::SetLastByte(char* subNodeHeader, ushort lastByte)
{
	*(ushort *)(subNodeHeader + (3 * sizeof(ushort))) = lastByte;
}

template<class TKey>
inline void cTreeNode_SubNode<TKey>::IncLastByte(char* subNodeHeader, ushort incValue)
{
	(*(ushort *)(subNodeHeader + (3 * sizeof(ushort)))) += incValue;
}

// Returns minimal reference item of the specified RI Block
template<class TKey>
inline uchar cTreeNode_SubNode<TKey>::GetMinRefItemPosition(char* subNodeHeader)
{
	return *(uchar *)(subNodeHeader + (4 * sizeof(ushort)));
}

// Set minimal reference item of the specified subnode
template<class TKey>
inline void cTreeNode_SubNode<TKey>::SetMinRefItemPosition(char* subNodeHeader, uchar minRefItemPosition)
{
	*(uchar *)(subNodeHeader + (4 * sizeof(ushort))) = minRefItemPosition;
}

// Returns maximal reference item of the specified subnode
template<class TKey>
inline uchar cTreeNode_SubNode<TKey>::GetMaxRefItemPosition(char* subNodeHeader)
{
	return *(uchar *)(subNodeHeader + (4 * sizeof(ushort)) + (1 * sizeof(uchar)));
}

// Set maximal reference item of the specified RI Block
template<class TKey>
inline void cTreeNode_SubNode<TKey>::SetMaxRefItemPosition(char* subNodeHeader, uchar maxRefItemPosition)
{
	*(uchar *)(subNodeHeader + (4 * sizeof(ushort)) + (1 * sizeof(uchar))) = maxRefItemPosition;
}

// return number of items in specified block
template<class TKey>
inline uint cTreeNode_SubNode<TKey>::GetItemsCount(char* subNodeHeader)
{
	if (GetLastItemOrder(subNodeHeader) == NOT_DEFINED)
		return 0;
	else
		return GetLastItemOrder(subNodeHeader) - GetFirstItemOrder(subNodeHeader) + 1;
}

// return reference item of the block
template<class TKey>
inline char* cTreeNode_SubNode<TKey>::GetMask(char* data, char* subNodeHeader)
{
	return data + GetSubNodePOrder(subNodeHeader);
}

// return reference item of the block
template<class TKey>
inline char* cTreeNode_SubNode<TKey>::GetMinRefItem(char* data, char* subNodeHeader)
{
	return data + GetSubNodePOrder(subNodeHeader) + GetMinRefItemPosition(subNodeHeader);
}

// return reference item of the block
template<class TKey>
inline char* cTreeNode_SubNode<TKey>::GetMaxRefItem(char* data, char* subNodeHeader)
{
	return data + GetSubNodePOrder(subNodeHeader) + GetMaxRefItemPosition(subNodeHeader);
}

template<class TKey>
inline void cTreeNode_SubNode<TKey>::SetMaxRefItem(char* data, char* subNodeHeader, const char* maxRefItem, ushort itemSize)
{
	memcpy(GetMaxRefItem(data, subNodeHeader), maxRefItem, itemSize);
}

// return subnode with specified order
template<class TKey>
inline char* cTreeNode_SubNode<TKey>::GetSubNodeHeader(char* subNodeHeaders, ushort lOrder)
{
	return subNodeHeaders + (lOrder * HEADER_SIZE);
}

// return subnode with specified order
template<class TKey>
inline char* cTreeNode_SubNode<TKey>::GetSubNode(char* subNodes, char* subNodeHeaders, ushort lOrder)
{
	return subNodes + GetSubNodePOrder(GetSubNodeHeader(subNodeHeaders, lOrder));
}

template<class TKey>
inline char* cTreeNode_SubNode<TKey>::GetSubNodeHeader(char* subNodeHeaders, ushort itemOrder, ushort subNodesCount)
{
	int loSn = 0;
	int hiSn = subNodesCount;
	do
	{
		int midSn = (loSn + hiSn) / 2;
		char* subNodeHeader = GetSubNodeHeader(subNodeHeaders, midSn);
		
		if (GetFirstItemOrder(subNodeHeader) > itemOrder)
		{
			hiSn = midSn - 1;
		}
		else if (GetLastItemOrder(subNodeHeader) < itemOrder)
		{
			loSn = midSn + 1;
		}
		else
		{
			return subNodeHeader;
		}
	} while (loSn <= hiSn);

	return NULL; // NULL means big problem
}

// return RI Block where belongs item with specified itemOrder
template<class TKey>
inline char* cTreeNode_SubNode<TKey>::GetSubNode(char* subNodes, char* subNodeHeaders, ushort itemOrder, ushort subNodesCount)
{
	// TODO - Vyhladanie delenim intervalu
	for (ushort i = 0; i < subNodesCount; i++)
	{
		char* snHeader = GetSubNodeHeader(subNodeHeaders, i);
		if ((GetFirstItemOrder(snHeader) <= itemOrder) && (itemOrder <= GetLastItemOrder(snHeader)))
			return GetSubNode(subNodes, subNodeHeaders, i);
	}
}

// returns logical order of subnode where belongs item with specified itemOrder
template<class TKey>
inline ushort cTreeNode_SubNode<TKey>::GetSubNodeOrder(char* subNodeHeaders, ushort itemOrder, ushort subNodesCount)
{
	// TODO - Vyhladanie delenim intervalu
	for (ushort i = 0; i < subNodesCount; i++)
	{
		char* snHeader = GetSubNodeHeader(subNodeHeaders, i);
		if ((GetFirstItemOrder(snHeader) <= itemOrder) && (itemOrder <= GetLastItemOrder(snHeader)))
			return i;
	}

	return NULL; // NULL means big problem
}

template<class TKey>
inline ushort cTreeNode_SubNode<TKey>::TotalFreeSize(char* subNodeHeaders, ushort subNodesOffset, ushort subNodesCount)
{
	uint freeSize = 0;

	for (ushort i = 0; i < subNodesCount; i++)
	{
		freeSize += FreeSize(subNodeHeaders, subNodesOffset, i, subNodesCount);
	}

	return freeSize;
}

// return true, if reference item with order got by parameter has some free space
/*
template<class TKey>
inline bool cTreeNode_SubNode<TKey>::HasFreeSpace(char* subNodeHeaders, ushort lOrder, ushort subNodesCount)
{
	ushort currentEndByte = GetLastByte(GetSubNodeHeader(subNodeHeaders, lOrder));
	ushort nextFirstByte = (lOrder == subNodesCount - 1) ? snOffset : GetSubNodePOrder(GetSubNodeHeader(subNodeHeaders, lOrder + 1));

	assert(nextFirstByte >= currentEndByte);
	return nextFirstByte > currentEndByte + 1;
}*/

// return free space of reference item with order got by parameter 
template<class TKey>
inline ushort cTreeNode_SubNode<TKey>::FreeSize(char* subNodeHeaders, ushort subNodesOffset, ushort lOrder, ushort subNodesCount)
{
	ushort currentEndByte = GetLastByte(GetSubNodeHeader(subNodeHeaders, lOrder));
	ushort nextFirstByte = (lOrder == subNodesCount - 1) ? subNodesOffset : GetSubNodePOrder(GetSubNodeHeader(subNodeHeaders, lOrder + 1));

	assert(nextFirstByte >= currentEndByte);
	return nextFirstByte - currentEndByte;
}

// return complete space of specified reference item and linked items(including reference item space)
template<class TKey>
inline ushort cTreeNode_SubNode<TKey>::CompleteSize(char* subNodeHeaders, ushort subNodesOffset, ushort lOrder, ushort subNodesCount)
{
	ushort currentFirstByte = GetSubNodePOrder(GetSubNodeHeader(subNodeHeaders, lOrder));
	ushort nextFirstByte = (lOrder == subNodesCount - 1) ? subNodesOffset : GetSubNodePOrder(GetSubNodeHeader(subNodeHeaders, lOrder + 1));

	return nextFirstByte - currentFirstByte;
}

template<class TKey>
inline char* cTreeNode_SubNode<TKey>::CreateSubNode(char* subNodes, char* subNodesHeaders, ushort lOrder, ushort pOrder, ushort startItemOrder, char* mask, char* mbr, const cDTDescriptor* keyDescriptor)
{
	const cSpaceDescriptor *spaceDescriptor = (cSpaceDescriptor*)keyDescriptor;

	// set subnode
	char* newSubNode = subNodes + pOrder;
	uint RISize = TKey::GetSize(cMBRectangle<TKey>::GetLoTuple(mbr), keyDescriptor);
	uint length = spaceDescriptor->GetDimension();

	cBitString::Copy(newSubNode, mask, length); // copy mask
	memcpy(newSubNode + cBitString::ByteSize(length), cMBRectangle<TKey>::GetLoTuple(mbr), RISize);             // set minimal ref item and its position
	memcpy(newSubNode + RISize + cBitString::ByteSize(length), cMBRectangle<TKey>::GetHiTuple(mbr, spaceDescriptor), RISize);

	// set subnode header
	char* newSubNodeHeader = subNodesHeaders + (lOrder * HEADER_SIZE);
	SetSubNodePOrder(newSubNodeHeader, pOrder);
	SetMinRefItemPosition(newSubNodeHeader, cBitString::ByteSize(length));
	SetMaxRefItemPosition(newSubNodeHeader, RISize + cBitString::ByteSize(length));
	SetLastByte(newSubNodeHeader, pOrder + (2 * RISize) + cBitString::ByteSize(length));
	SetFirstItemOrder(newSubNodeHeader, startItemOrder);
	SetLastItemOrder(newSubNodeHeader, NOT_DEFINED);

	return newSubNodeHeader;
}

template<class TKey>
inline char* cTreeNode_SubNode<TKey>::CreateSubNode(char* subNodes, char* subNodesHeaders, ushort lOrder, ushort pOrder, ushort startItemOrder, const char* refItem, const cDTDescriptor* keyDescriptor)
{
	// set subnode
	char* newSubNode = subNodes + pOrder;
	uint RISize = TKey::GetSize(refItem, keyDescriptor);
	uint length = TKey::GetLength(refItem, keyDescriptor);

	cBitString::SetBits(newSubNode, length, false); // set mask to 1s
	memcpy(newSubNode + cBitString::ByteSize(length), refItem, RISize);             // set minimal ref item and its position
	memcpy(newSubNode + RISize + cBitString::ByteSize(length), refItem, RISize);

	// set subnode header
	char* newSubNodeHeader = subNodesHeaders + (lOrder * HEADER_SIZE);
	SetSubNodePOrder(newSubNodeHeader, pOrder);
	SetMinRefItemPosition(newSubNodeHeader, cBitString::ByteSize(length));
	SetMaxRefItemPosition(newSubNodeHeader, RISize + cBitString::ByteSize(length));
	SetLastByte(newSubNodeHeader, pOrder + (2 * RISize) + cBitString::ByteSize(length));
	SetFirstItemOrder(newSubNodeHeader, startItemOrder);
	SetLastItemOrder(newSubNodeHeader, NOT_DEFINED);

	return newSubNodeHeader;
}


template<class TKey>
inline char* cTreeNode_SubNode<TKey>::CreateSubNode(char* subNodes, char* subNodesHeaders, ushort lOrder, ushort pOrder, sCoverRecord* snRecord, const cDTDescriptor* keyDescriptor)
{
	// set subnode
	char* newSubNode = subNodes + pOrder;
	uint RISize = TKey::GetSize(snRecord->minRefItem, keyDescriptor);
	uint length = TKey::GetLength(snRecord->minRefItem, keyDescriptor);

	cBitString::Copy(newSubNode, snRecord->mask, RISize); // copy mask ?? shount not be length??
	memcpy(newSubNode + cBitString::ByteSize(length), snRecord->minRefItem, RISize);             // set minimal ref item and its position
	memcpy(newSubNode + RISize + cBitString::ByteSize(length), snRecord->minRefItem, RISize);

	// set subnode header
	char* newSubNodeHeader = subNodesHeaders + (lOrder * HEADER_SIZE);
	SetSubNodePOrder(newSubNodeHeader, pOrder);
	SetMinRefItemPosition(newSubNodeHeader, cBitString::ByteSize(length));
	SetMaxRefItemPosition(newSubNodeHeader, RISize + cBitString::ByteSize(length));
	SetLastByte(newSubNodeHeader, pOrder + (2 * RISize) + cBitString::ByteSize(length));
	SetFirstItemOrder(newSubNodeHeader, snRecord->startItemOrder);
	SetLastItemOrder(newSubNodeHeader, snRecord->endItemOrder);

	return newSubNodeHeader;

}

// shift inside of block (when we want to insert item at the beginning or in the middle of the block)  
template<class TKey>
inline void cTreeNode_SubNode<TKey>::Shift(char* subNodes, short shift, ushort startByte, ushort nextSubNodePOrder)
{
	memmove(subNodes + startByte + shift, subNodes + startByte, nextSubNodePOrder - startByte - ((shift > 0) ? shift : 0));
}

// complete shift of subnode (when we want to insert item into full block, neighbours have to be shifted)
template<class TKey>
inline char* cTreeNode_SubNode<TKey>::ShiftSubNode(char* subNode, short shift, ushort completeSize)
{
	memmove(subNode + shift, subNode, completeSize - ((shift > 0) ? shift : 0));
	
	return subNode + shift;
}

// Increments logical order intervals of particular subnodes
template<class TKey>
inline void cTreeNode_SubNode<TKey>::UpdateItemOrderIntervals(char* subNodesHeaders, ushort startLOrder, short shift, ushort subNodesCount)
{
	// for all next ri blocks increment intervals
	for (ushort i = startLOrder + 1; i < subNodesCount; i++)
	{
		IncFirstItemOrder(GetSubNodeHeader(subNodesHeaders, i), shift);
		IncLastItemOrder(GetSubNodeHeader(subNodesHeaders, i), shift);
	}
}

// Returns the length of the mask
template<class TKey>
inline ushort cTreeNode_SubNode<TKey>::MaskWeight(char* subNode, ushort maskLength)
{
	return cBitString::GetNumberOfBits(GetMask(subNode), maskLength, 1);
}

}}}
#endif
