#pragma once
#include "dstruct/paged/rtree/cRTree.h";

template<class TKey>
class cCompleteRTree
{
public:
	
	cRTree<TKey> *mIndex;//prázdné tělo stromu strom
	cRTreeHeader<TKey> *mIndexHeader;
	int indexColumnPosition;

	cCompleteRTree(const char* uniqueName,int position, uint blockSize, cSpaceDescriptor *dd, uint keySize, uint dataSize,bool variableLenData, uint dsMode, uint treeCode, uint compressionRatio, unsigned int CODETYPE, unsigned int RUNTIME_MODE, bool HISTOGRAMS, cQuickDB *quickDB);
	bool SetRTree(const char* uniqueName, uint blockSize, cSpaceDescriptor *dd, uint keySize, uint dataSize, bool variableLenData, uint dsMode, uint treeCode, uint compressionRatio, unsigned int CODETYPE, unsigned int RUNTIME_MODE, bool HISTOGRAMS, cQuickDB *quickDB);

};
template<class TKey>
cCompleteRTree<TKey>::cCompleteRTree(const char* uniqueName,int position, uint blockSize, cSpaceDescriptor *dd, uint keySize, uint dataSize, bool variableLenData, uint dsMode, uint treeCode, uint compressionRatio, unsigned int codeType, unsigned int runtimeMode, bool histograms,  cQuickDB *quickDB)
{
	SetRTree(uniqueName, blockSize, dd, keySize, dataSize, variableLenData, dsMode, treeCode, compressionRatio, codeType, runtimeMode, histograms, quickDB);
	indexColumnPosition = position;
}
template<class TKey>
inline bool cCompleteRTree<TKey>::SetRTree(const char* uniqueName, uint blockSize, cSpaceDescriptor *dd, uint keySize, uint dataSize, bool variableLenData, uint dsMode, uint treeCode, uint compressionRatio, unsigned int CODETYPE, unsigned int RUNTIME_MODE, bool HISTOGRAMS, cQuickDB *quickDB)
{
	mIndexHeader = new cRTreeHeader<TKey>(dd, dataSize, variableLenData, dsMode, treeCode, compressionRatio);
	mIndexHeader->SetName(uniqueName);
	mIndexHeader->SetRuntimeMode(RUNTIME_MODE);
	mIndexHeader->SetCodeType(CODETYPE);
	mIndexHeader->SetSignatureEnabled(false);
	mIndexHeader->SetOnlyMemoryProcessing(false);
	//mIndexHeader->SetSignatureController(sigController);
	mIndexHeader->HeaderSetup(blockSize);
	mIndexHeader->SetOrderingEnabled(false);
	mIndexHeader->SetHistogramEnabled(HISTOGRAMS);
	

	
	mIndex = new cRTree<TKey>();
	if (!mIndex->Create(mIndexHeader, quickDB))
	{
		printf("Key index: creation failed!\n");
		return false;
	}
	else
		return true;
}
