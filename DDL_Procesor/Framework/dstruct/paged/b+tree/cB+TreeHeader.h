/**
*	\file cBpTreeHeader.h
*	\author Radim Baca
*	\version 0.1
*	\date feb 2008
*	\brief Header of the cBpTree.
*/


#ifndef __cBpTreeHeader_h__
#define __cBpTreeHeader_h__

#include "dstruct/paged/b+tree/cB+TreeConst.h"
#include "dstruct/paged/core/cTreeHeader.h"
#include "dstruct/paged/b+tree/cB+TreeNodeHeader.h"

using namespace dstruct::paged::core;

/**
*	Header of the cBpTree.
* It is parameterized by the key type being stored in the tree. 
*
*	\author Radim Baca, David Bednář, Michal Krátký
*	\version 0.1
*	\date feb 2008
**/
namespace dstruct {
	namespace paged {
		namespace bptree {

template<class TKey>
class cBpTreeHeader : public cTreeHeader
{
private:
	uint mInMemCacheSize; // 0 means no cache is created

	cMBRectangle<cTuple>* mTreeMBR; // MBR of root node

	inline virtual void Init(const char* uniqueName, cDTDescriptor *dd);
	void Init(const char* uniqueName, 
		unsigned int blockSize, 
		cDTDescriptor *dd, 
		uint keySize, 
		uint dataSize,
		bool variableLenData = false, 
		uint dsMode = cDStructConst::DSMODE_DEFAULT, 
		uint treeCode = cDStructConst::BTREE,
		uint compressionRatio = 1);

public:
	//static const unsigned int TREECODE_BPTREE = 0x100;
	//static const unsigned int TREECODE_BPTREE_DUP = 0x101; //< allow duplicate insertion, untested -- may not work

	cBpTreeHeader(const char* uniqueName, cDTDescriptor *dd);
	cBpTreeHeader(const char* uniqueName, 
		unsigned int blockSize, 
		cDTDescriptor *dd, 
		uint keySize, 
		uint dataSize,  
		bool variableLenData = false, 
		uint dsMode = cDStructConst::DSMODE_DEFAULT,	
		uint treeCode = cDStructConst::BTREE,
		uint compressionRatio = 1);

	~cBpTreeHeader();

	inline void SetInMemCacheSize(uint value);
	inline uint GetInMemCacheSize();

	inline cMBRectangle<cTuple>* GetTreeMBR();
};

template<class TKey>
dstruct::paged::bptree::cBpTreeHeader<TKey>::~cBpTreeHeader()
{
	if (TKey::CODE == cTuple::CODE)
	{
		if (mTreeMBR != NULL)
		{
			delete mTreeMBR;
			mTreeMBR = NULL;
		}
	}
}

/**
* Constructor for an existing B-tree.
* \param uniqueName Unique name of a data structure instance. This value will be used to search the header in the cache.
*/
template<class TKey>
cBpTreeHeader<TKey>::cBpTreeHeader(const char* uniqueName, cDTDescriptor *dd)
	: mInMemCacheSize(0), mTreeMBR(NULL)
{
	Init(uniqueName, dd);
}

/**
* Constructor for a new B-tree.
* \param uniqueName Unique name of a data structure instance.
* \param blockSize Size of a secondary storage page. This value has to be equal to the cNodeCache page size!
* \param keySize Size of the key.
* \param dataSize Size of the data.
* \param variableLenData Flag is true if the data structure should use a variable len data.
* \param dsMode Mode of the data structure, see cDStructConst.h, DSMODE_*
*/
template<class TKey>
cBpTreeHeader<TKey>::cBpTreeHeader(const char* uniqueName, uint blockSize, cDTDescriptor *dd, uint keySize, uint dataSize,  
								   bool variableLenData, uint dsMode, uint treeCode, uint compressionRatio)
	: mInMemCacheSize(0), mTreeMBR(NULL)
{
	Init(uniqueName, blockSize, dd, keySize, dataSize, variableLenData, dsMode, treeCode, compressionRatio);
}

template<class TKey> inline void cBpTreeHeader<TKey>::SetInMemCacheSize(uint value)
{
	mInMemCacheSize = value;
}

template<class TKey> inline uint cBpTreeHeader<TKey>::GetInMemCacheSize()
{
	return mInMemCacheSize;
}

template<class TKey>
inline cMBRectangle<cTuple>* cBpTreeHeader<TKey>::GetTreeMBR()
{
	return mTreeMBR;
}

/**
* Initialization of a new B-tree.
* \param uniqueName Unique name of a data structure instance.
* \param blockSize Size of a secondary storage page. This value has to be equal to the cNodeCache page size!
* \param keySize Size of the key.
* \param dataSize Size of the data.
* \param variableLenData Flag is true if the data structure should use a variable len data.
* \param dsMode Mode of the data structure, see cDStructConst.h, DSMODE_*
*/
template<class TKey>
void cBpTreeHeader<TKey>::Init(const char* uniqueName, uint blockSize, cDTDescriptor *dd, uint keySize, uint dataSize,
	bool variableLenData, uint dsMode, uint dsCode, uint compressionRatio)
{
	cTreeHeader::Init();

	SetTitle("B+tree");
	SetVersion((float)0.20);
	SetBuild(0x20031201);

	SetNodeHeaderCount(2);
	SetNodeHeader(HEADER_LEAFNODE, new cBpTreeNodeHeader<TKey>(true, keySize, dataSize, TKey::LengthType == cDataType::LENGTH_VARLEN, variableLenData, dsMode));
	SetNodeHeader(HEADER_NODE, new cBpTreeNodeHeader<TKey>(false, keySize, 0, TKey::LengthType == cDataType::LENGTH_VARLEN, variableLenData, dsMode));
	GetNodeHeader(HEADER_LEAFNODE)->SetKeyDescriptor(dd);
	GetNodeHeader(HEADER_NODE)->SetKeyDescriptor(dd);

	mDStructCode = dsCode;
	SetDStructCode(dsCode);
	DuplicatesAllowed(mDStructCode == cDStructConst::BTREE_DUP);

	((cTreeNodeHeader*)mNodeHeaders[HEADER_NODE])->SetNodeFanoutCapacity(0);
	((cTreeNodeHeader*)mNodeHeaders[HEADER_NODE])->SetNodeDeltaCapacity(0);
	((cTreeNodeHeader*)mNodeHeaders[HEADER_NODE])->SetNodeExtraLinkCount(1);
	((cTreeNodeHeader*)mNodeHeaders[HEADER_NODE])->SetNodeExtraItemCount(0);

	((cTreeNodeHeader*)mNodeHeaders[HEADER_LEAFNODE])->SetNodeFanoutCapacity(0);
	((cTreeNodeHeader*)mNodeHeaders[HEADER_LEAFNODE])->SetNodeDeltaCapacity(0);
	((cTreeNodeHeader*)mNodeHeaders[HEADER_LEAFNODE])->SetNodeExtraLinkCount(3);
	((cTreeNodeHeader*)mNodeHeaders[HEADER_LEAFNODE])->SetNodeExtraItemCount(0);

	SetMeasureTime(true);
	SetMeasureCount(true);
	SetCacheMeasureTime(true);
	SetCacheMeasureCount(true);
	AddHeaderSize(sizeof(mDuplicates));

	for (uint i = 0; i < mNodeHeaderCount; i++)
	{
		((cTreeNodeHeader*)mNodeHeaders[i])->VariableLenKeyEnabled(TKey::LengthType == cDataType::LENGTH_VARLEN);
		((cTreeNodeHeader*)mNodeHeaders[i])->VariableLenDataEnabled(variableLenData);
	}

	((cTreeNodeHeader*)mNodeHeaders[HEADER_LEAFNODE])->SetMaxCompressionRatio(compressionRatio);
	((cTreeNodeHeader*)mNodeHeaders[HEADER_NODE])->SetMaxCompressionRatio(compressionRatio);
	((cTreeNodeHeader*)mNodeHeaders[HEADER_LEAFNODE])->ComputeNodeCapacity(blockSize, true);
	((cTreeNodeHeader*)mNodeHeaders[HEADER_NODE])->ComputeNodeCapacity(blockSize, false);

	ComputeTmpBufferSize();
	SetName(uniqueName);

	// TreeMBR is neccessary for histograms
	if (TKey::CODE == cTuple::CODE)
		mTreeMBR = new cMBRectangle<cTuple>((cSpaceDescriptor*) dd);
}

/**
* Initialization of an existing B-tree.
* \param uniqueName Unique name of a data structure instance. This value will be used to search the header in the cache.
*/
template<class TKey>
void cBpTreeHeader<TKey>::Init(const char* uniqueName, 
		cDTDescriptor *dd)
{
	SetNodeHeaderCount(2);
	SetNodeHeader(HEADER_LEAFNODE, new cBpTreeNodeHeader<TKey>());
	SetNodeHeader(HEADER_NODE, new cBpTreeNodeHeader<TKey>());
	GetNodeHeader(HEADER_LEAFNODE)->SetKeyDescriptor(dd);
	GetNodeHeader(HEADER_NODE)->SetKeyDescriptor(dd);

	SetName(uniqueName);

	// TreeMBR is neccessary for histograms
	if (TKey::CODE == cTuple::CODE)
		mTreeMBR = new cMBRectangle<cTuple>((cSpaceDescriptor*) dd);
}

}}}
#endif