#include "cSignatureStorage.h"

namespace dstruct {
	namespace paged {
		namespace rtree {

void cSignatureStorage::Init(cQuickDB* quickDB, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSD) 
{
	char strTmp[40];
	uint nOfDims = pSignatureParams->GetDimension();
	uint keyType = pSignatureParams->GetKeyType();
	uint blockSize = quickDB->GetNodeCache()->GetBlockSize();

	mKeySD = CreateKeySD();
	mDataSD = new cSpaceDescriptor(DATA_LENGTH, new cTuple(), new cUInt());

	uint maxLength = 0, maxSignatureOrder = 0;
	for (uint i = 0; i < pSignatureParams->GetDimension(); i++)
	{
		if (pSignatureParams->GetChunkLength(i) > maxLength)
		{
			maxSignatureOrder = i;
			maxLength = pSignatureParams->GetChunkLength(i);
		}
	}

	if ((pSignatureParams->GetKeyType() == cKeyType::DDO_NODEINDEX_CHUNKORDER)
		|| (pSignatureParams->GetKeyType() == cKeyType::DDS_NODEINDEX_CHUNKORDER)
		|| (pSignatureParams->GetKeyType() == cKeyType::DIS_NODEINDEX_DIMENSION_CHUNKORDER))
	{
		uint maxChunksCount = 0;
		for (uint i = 0; i < pSignatureParams->GetDimension(); i++)
			maxChunksCount += pSignatureParams->GetChunkCount(i);
		mSplitChunks = new cLinkedList<cChunkInfo>(maxChunksCount);
	}

	sprintf(strTmp, "sigarray%d", keyType);
	mArrayHeader = new cSequentialArrayHeader<cSignatureRecord>(strTmp, blockSize, pSignatureSD[maxSignatureOrder], cDStructConst::DSMODE_DEFAULT);

	sprintf(strTmp, "btree%d", keyType);
	mConversionIndexHeader = new cBpTreeHeader<cTuple>(strTmp, blockSize, mKeySD, mKeySD->GetSize(), mDataSD->GetSize(), false, cDStructConst::DSMODE_DEFAULT);
	mConversionIndexHeader->SetInMemCacheSize(INMEMCACHE_SIZE);
}

bool cSignatureStorage::Create(cQuickDB* quickDB)
{
	mArray = new cSequentialArray<cSignatureRecord>();
	if (!mArray->Create(mArrayHeader, quickDB))
	{
		printf("Signature array: creation failed\n");
		return false;
	}

	mConversionIndex = new cBpTree<cTuple>();
	if (!mConversionIndex->Create(mConversionIndexHeader, quickDB))
	{
		printf("Conversion index: creation failed!\n");
		return false;
	}

	return true;
}

bool cSignatureStorage::Open(cQuickDB* quickDB, bool readOnly)
{
	mArray = new cSequentialArray<cSignatureRecord>();
	if (!mArray->Open(mArrayHeader, quickDB))
	{
		printf("Signature array: open failed\n");
		return false;
	}

	mConversionIndex = new cBpTree<cTuple>();
	if (!mConversionIndex->Open(mConversionIndexHeader, quickDB, readOnly))
	{
		printf("Conversion index: creation failed!\n");
		return false;
	}

	return true;
}

void cSignatureStorage::Close()
{
	mConversionIndex->Close();
	mArray->Close();
}


void cSignatureStorage::Delete()
{
	if (mKeySD != NULL)
	{
		delete mKeySD;
		mKeySD = NULL;
	}

	if (mDataSD != NULL)
	{
		delete mDataSD;
		mDataSD = NULL;
	}

	if (mArrayHeader != NULL)
	{
		delete mArrayHeader;
		mArrayHeader = NULL;
	}

	if (mArray != NULL)
	{
		delete mArray;
		mArray = NULL;
	}

	if (mConversionIndexHeader != NULL)
	{
		delete mConversionIndexHeader;
		mConversionIndexHeader = NULL;
	}

	if (mConversionIndex != NULL)
	{
		delete mConversionIndex;
		mConversionIndex = NULL;
	}

	if (mSplitChunks != NULL)
	{
		delete mSplitChunks;
		mSplitChunks = NULL;
	}
}


void cSignatureStorage::PrintStructuresInfo()
{
	mConversionIndex->PrintInfo();
	mArray->PrintInfo();
}

void cSignatureStorage::PrintSignaturesInfo(uint level, uint* pUniqueValues, uint pNodesCount, double* pWeights, uint* pZeros, cSignatureParams* pSignatureParams)
{
	uint nOfDims = pSignatureParams->GetDimension();
	//uint nOfBits = pSignatureParams->GetBitCount();

	printf("InvLvl %u:\tSigs: %d ", level, mArray->GetHeader()->GetItemCount());
	printf("Zero Sigs.: (");
	for (uint i = 0; i < nOfDims; i++)
	{
		printf("%u", pZeros[i]);
		if (i < nOfDims - 1)
			printf("; ");
	}
	printf(")\n");
	
	double sumAvg = 0, sumUniques = 0, sumAvgLength = 0;
	for (uint i = 0; i < nOfDims; i++)
	{
		sumAvg += pWeights[i];
		sumUniques += pUniqueValues[i];
		sumAvgLength += pSignatureParams->GetLength(i);
	}

	//////////////////////////////////////////////////////////////////
	float avgLength = sumAvgLength / (float)nOfDims;
	printf("Avg L.:\t\t%.2f (", avgLength);
	for (uint i = 0; i < nOfDims; i++)
	{
		printf("%d", pSignatureParams->GetLength(i));
		if (i < nOfDims - 1)
			printf("; ");
	}
	printf(")\n");

	////////////////////////////////////////////////////////////////////
	float avgWeight = sumAvg / nOfDims / (float)pNodesCount;
	printf("Avg W.:\t\t%.2f (", avgWeight);
	for (uint i = 0; i < nOfDims; i++)
	{
		printf("%.2f", pWeights[i] / (float) pNodesCount);
		if (i < nOfDims - 1)
			printf("; ");
	}
	printf(")\n");

	printf("Avg W [%%].:\t%.2f\n", (avgWeight / avgLength)*100);

	///////////////////////////////////////////////////////////////////////
	printf("Uniques:\t%.2f (", sumUniques / nOfDims / (float) pNodesCount);
	for (uint i = 0; i < nOfDims; i++)
	{
		printf("%.2f", pUniqueValues[i] / (float) pNodesCount);
		if (i < nOfDims - 1)
			printf("; ");
	}
	printf(")\n");

/*	printf("Conflicts [%%]:\t%d (", (uint) ((1.00f - (sumAvg / nOfBits / sumUniques)) * 100));
	for (uint i = 0; i < nOfDims; i++)
	{
		printf("%d", (uint) ((1.00f - (pWeights[i] / nOfBits / pUniqueValues[i])) * 100));
		if (i < nOfDims - 1)
			printf("; ");
	}*/
	printf(")\n--------------------------------------------------------------------------------\n");
}

void cSignatureStorage::PrintSignaturesInfo(uint level, uint pItemsCount, uint pNodesCount, double pWeights, uint pZeros, cSignatureParams* pSignatureParams, cSignatureController* pSignatureController)
{
	printf("InvLvl %u:\tL: %u Sigs: %d ", level, pSignatureParams->GetLength(), mArray->GetHeader()->GetItemCount());
	printf("Zero Sigs.: %d\n", pZeros);
	printf("Query Types:\t%d\n", pSignatureController->GetQueryTypesCount());
	printf("Weight.:\t%.2f\n", pWeights / (float) pNodesCount);
	printf("Avg Weight [%%].:\t%.8f\n", (((pWeights / (float)pNodesCount)) / pSignatureParams->GetLength()) * 100);
	printf("Uniques:\t%.2f\n", pItemsCount / (float) pNodesCount);
//	printf("Conflicts [%%]:\t%d \n", (uint)((1.00f - (pWeights / pSignatureController->GetQueryTypesCount() / pSignatureParams->GetBitCount() / pItemsCount)) * 100));
	printf("--------------------------------------------------------------------------------\n");

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void cSignatureStorage_1::ClearNodeSignature(uint nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cSignatureKey* pConvKey)
{
	uint nOfDims = pSignatureParams->GetDimension();

	if (SignatureExists(pConvKey, nodeIndex))
	{
		cSequentialArrayNode<cSignatureRecord> *node = mArray->ReadNodeW(pConvKey->GetSignatureIndex());
		char* sigRecord = node->GetItem(pConvKey->GetPosition());

		for (uint i = 0; i < nOfDims; i++)
		{
			cSignatureRecord::ClearTuple(sigRecord, i, pSignatureSDs[i]);
		}

		mArray->UnlockW(&node);
	}
}

void cSignatureStorage_1::ReplicateNodeSignature(uint destNodeIndex, uint srcNodeIndex, uint level, cSignatureParams* pSignatureParams, cInsertSigBuffers* buffers)
{
	cSignatureRecord* nodeSignature = buffers->SigNodes[level][0]; // only one type of nodeSignature in this signature storage
	cSignatureKey* convKey = buffers->ConvIndexKeys[level];
	uint signatureIndex = 0, position = 0;

	if (SignatureExists(convKey, srcNodeIndex))
	{
		cSequentialArrayNode<cSignatureRecord> *node = mArray->ReadNodeR(convKey->GetSignatureIndex());
		char* sigRecord = node->GetItem(convKey->GetPosition());
		nodeSignature->CopyFrom(sigRecord, mArray->GetKeyDescriptor());
		mArray->UnlockR(&node);

		mArray->AddItem(signatureIndex, position, *nodeSignature);
		convKey->SetKey(destNodeIndex, mKeySD);
		convKey->SetData(signatureIndex, position, mDataSD);
		mConversionIndex->Insert(*convKey->GetKey(), *convKey->GetData());
	}
}

void cSignatureStorage_1::CreateItemSignature(uint nodeIndex, uint level, const char* item, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cInsertSigBuffers* buffers, cSignatureController* pSignatureController, sMapItem** pMapTable, uint* pMapTableCounter)
{
	uint nOfDims = pSignatureParams->GetDimension();
	cSignatureRecord* nodeSignature = buffers->SigNodes[level][0]; // only one type of nodeSignature in this signature storage
	cSignatureKey* convKey = buffers->ConvIndexKeys[level];
	uint signatureIndex = 0, position = 0;
	cSequentialArrayNode<cSignatureRecord> *node = NULL;
	char* sigRecord = NULL;

	// Create/Read signature record
	bool sigExists = false;
	if (sigExists = SignatureExists(convKey, nodeIndex))
	{
		node = mArray->ReadNodeW(convKey->GetSignatureIndex());
		sigRecord = node->GetItem(convKey->GetPosition());
	}
	else
	{
		sigRecord = nodeSignature->GetData();
		for (uint i = 0; i < nOfDims; i++)
		{
			cSignatureRecord::ClearTuple(sigRecord, i, pSignatureSDs[i]);
		}
	}

	// Modify signature record
	for (uint i = 0; i < nOfDims; i++)
	{
		uint sigLength = pSignatureParams->GetLength(i);
		uint value = cTuple::GetUInt(item, i, NULL);

		uint nOfBits = pSignatureParams->GetBitCount(i);
		for (uint j = 0; j < nOfBits; j++)
		{
			uint trueBitOrder = cSignature::GetTrueBitOrder(value, sigLength, j);
			cSignatureRecord::SetTrueBit(sigRecord, i, pSignatureSDs[i], trueBitOrder);
		}
	}

	// Save/Unlock signature record
	if (sigExists)
	{
		mArray->UnlockW(&node);
	}
	else
	{
		mArray->AddItem(signatureIndex, position, *nodeSignature);
		convKey->SetKey(nodeIndex, mKeySD);
		convKey->SetData(signatureIndex, position, mDataSD);
		mConversionIndex->Insert(*convKey->GetKey(), *convKey->GetData());
	}
}

bool cSignatureStorage_1::IsMatched(uint nodeIndex, uint level, uint orderIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cQueryProcStat *queryProcStat, cRQBuffers<cTuple>* buffers, cSignatureController* pSignatureController)
{
	cArray<ullong> *trueBitOrders = buffers->QueryTrueBitOrders;
	cArray<uint> *narrowDims = buffers->NarrowDimensions;
	uint nOfDims = narrowDims->Count();
	cSignatureKey* convKey = buffers->ConvIndexKeys[level];

	if (SignatureExists(convKey, nodeIndex, queryProcStat, level))
	{
		cSequentialArrayNode<cSignatureRecord>* node = mArray->ReadNodeR(convKey->GetSignatureIndex());
		char* sigRecord = node->GetItem(convKey->GetPosition());
		queryProcStat->IncSigLarInQuery(level);

		for (uint i = 0; i < nOfDims; i++)
		{
			uint narrowDim = narrowDims->GetRefItem(i);

			uint nOfBits = pSignatureParams->GetBitCount(narrowDim);
			for (uint j = 0; j < nOfBits; j++)
			{
				uint trueBitOrder = trueBitOrders->GetRefItem(orderIndex++);
				bool bitValue = cSignatureRecord::IsMatched(sigRecord, narrowDim, pSignatureSDs[narrowDim], trueBitOrder);

				queryProcStat->IncComputCompareQuery();
				if (!bitValue)
				{
					mArray->UnlockR(&node);
					return false;
				}
			}
		}

		mArray->UnlockR(&node);
		return true;
	}
	else
	{
		printf("cRTreeSignatureIndex::IsMatched - Signature not exists !!!");
		return true;
	}

	return true;
}

void cSignatureStorage_1::ComputeWeight(tNodeIndex nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, double* pWeights, uint* pZeros, cSignatureKey* convKey)
{
	uint nOfDims = pSignatureParams->GetDimension();

	if (SignatureExists(convKey, nodeIndex))
	{
		cSequentialArrayNode<cSignatureRecord> *node = mArray->ReadNodeR(convKey->GetSignatureIndex());
		char* sigRecord = node->GetItem(convKey->GetPosition());

		for (uint i = 0; i < nOfDims; i++)
		{
			int weight = cSignatureRecord::ComputeSignatureWeight(sigRecord, i, pSignatureSDs[i]);

			pWeights[i] += (double) weight;
			if (weight == 0)
				pZeros[i]++;
		}

		mArray->UnlockR(&node);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void cSignatureStorage_2::ClearNodeSignature(uint nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cSignatureKey* pConvKey)
{
	uint nOfDims = pSignatureParams->GetDimension();

	for (uint i = 0; i < nOfDims; i++)
	{
		if (SignatureExists(pConvKey, nodeIndex, i))
		{
			cSequentialArrayNode<cSignatureRecord> *node = mArray->ReadNodeW(pConvKey->GetSignatureIndex());
			char* sigRecord = node->GetItem(pConvKey->GetPosition());
			cSignatureRecord::ClearTuple(sigRecord, pSignatureSDs[i]);
			mArray->UnlockW(&node);
		}
	}
}

void cSignatureStorage_2::ReplicateNodeSignature(uint destNodeIndex, uint srcNodeIndex, uint level, cSignatureParams* pSignatureParams, cInsertSigBuffers* buffers)
{
	cSignatureKey* convKey = buffers->ConvIndexKeys[level];
	uint nOfDims = pSignatureParams->GetDimension();
	uint signatureIndex = 0, position = 0;

	for (uint i = 0; i < nOfDims; i++)
	{
		if (SignatureExists(convKey, srcNodeIndex, i))
		{
			cSignatureRecord* nodeSignature = buffers->SigNodes[level][i];
			cSequentialArrayNode<cSignatureRecord> *node = mArray->ReadNodeR(convKey->GetSignatureIndex());
			char* sigRecord = node->GetItem(convKey->GetPosition());
			nodeSignature->CopyFrom(sigRecord, mArray->GetKeyDescriptor());
			mArray->UnlockR(&node);

			mArray->AddItem(signatureIndex, position, *nodeSignature);
			convKey->SetKey(destNodeIndex, i, mKeySD);
			convKey->SetData(signatureIndex, position, mDataSD);
			mConversionIndex->Insert(*convKey->GetKey(), *convKey->GetData());
		}
	}
}

void cSignatureStorage_2::CreateItemSignature(uint nodeIndex, uint level, const char* item, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cInsertSigBuffers* buffers, cSignatureController* pSignatureController, sMapItem** pMapTable, uint* pMapTableCounter)
{
	uint nOfDims = pSignatureParams->GetDimension();
	cSequentialArrayNode<cSignatureRecord> *node = NULL;
	uint signatureIndex = 0, position = 0;
	uint previousSignatureIndex = UINT_MAX;
	cSignatureKey* convKey = buffers->ConvIndexKeys[level];

	for (uint i = 0; i < nOfDims; i++)
	{
		uint nOfBits = pSignatureParams->GetBitCount(i);
		uint value = cTuple::GetUInt(item, i, NULL);
		cSpaceDescriptor *dimSD = pSignatureSDs[i];
		uint sigLength = pSignatureParams->GetLength(i);

		if (SignatureExists(convKey, nodeIndex, i))
		{
			signatureIndex = convKey->GetSignatureIndex();
			if (signatureIndex != previousSignatureIndex)
			{
				mArray->UnlockW(&node);
				node = mArray->ReadNodeW(signatureIndex);
			}
			
			char* sigRecord = node->GetItem(convKey->GetPosition());
			for (uint j = 0; j < nOfBits; j++)
			{
				uint trueBitOrder = cSignature::GetTrueBitOrder(value, sigLength, j);
				cSignatureRecord::SetTrueBit(sigRecord, dimSD, trueBitOrder);
			}

			previousSignatureIndex = signatureIndex;
		}
		else
		{
			cSignatureRecord* nodeSignature = buffers->SigNodes[level][i];
			char* sigRecord = nodeSignature->GetData();
			cSignatureRecord::ClearTuple(sigRecord, dimSD);

			for (uint j = 0; j < nOfBits; j++)
			{
				uint trueBitOrder = cSignature::GetTrueBitOrder(value, sigLength, j);
				cSignatureRecord::SetTrueBit(nodeSignature->GetData(), dimSD, trueBitOrder);
			}

			mArray->AddItem(signatureIndex, position, *nodeSignature);
			convKey->SetKey(nodeIndex, i, mKeySD);
			convKey->SetData(signatureIndex, position, mDataSD);
			mConversionIndex->Insert(*convKey->GetKey(), *convKey->GetData());
		}
	}

	mArray->UnlockW(&node);
}


bool cSignatureStorage_2::IsMatched(uint nodeIndex, uint level, uint orderIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cQueryProcStat *queryProcStat, cRQBuffers<cTuple>* buffers, cSignatureController* pSignatureController)
{
	cArray<ullong> *trueBitOrders = buffers->QueryTrueBitOrders;
	cArray<uint> *narrowDims = buffers->NarrowDimensions;
	uint nOfDims = narrowDims->Count();
	cSignatureKey* convKey = buffers->ConvIndexKeys[level];
	cSequentialArrayNode<cSignatureRecord> *node = NULL;
	uint previousSignatureIndex = UINT_MAX;

	for (uint i = 0; i < nOfDims; i++)
	{
		uint narrowDim = narrowDims->GetRefItem(i);
		cSpaceDescriptor *dimSD = pSignatureSDs[narrowDim];
		uint nOfBits = pSignatureParams->GetBitCount(narrowDim);

		if (SignatureExists(convKey, nodeIndex, narrowDim, queryProcStat, level))
		{
			uint signatureIndex = convKey->GetSignatureIndex();
			if (signatureIndex != previousSignatureIndex)
			{
				mArray->UnlockR(&node);
				node = mArray->ReadNodeR(signatureIndex);
				queryProcStat->IncSigLarInQuery(level);
			}
			char* sigRecord = node->GetItem(convKey->GetPosition());

			for (uint j = 0; j < nOfBits; j++)
			{
				uint trueBitOrder = trueBitOrders->GetRefItem(orderIndex++);
				bool bitValue = cSignatureRecord::IsMatched(sigRecord, dimSD, trueBitOrder);

				queryProcStat->IncComputCompareQuery();
				if (!bitValue)
				{
					mArray->UnlockR(&node);
					return false;
				}
			}
			previousSignatureIndex = signatureIndex;
		}
		else
		{
			printf("cRTreeSignatureIndex::IsMatched - Signature not exists !!!");
			break;
		}
	}
	mArray->UnlockR(&node);

	return true;
}

void cSignatureStorage_2::ComputeWeight(tNodeIndex nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, double* pWeights, uint* pZeros, cSignatureKey* convKey)
{
	uint nOfDims = pSignatureParams->GetDimension();

	for (uint i = 0; i < nOfDims; i++)
	{
		cSpaceDescriptor *dimSD = pSignatureSDs[i];
		if (SignatureExists(convKey, nodeIndex, i))
		{
			cSequentialArrayNode<cSignatureRecord> *node = mArray->ReadNodeR(convKey->GetSignatureIndex());
			char* sigRecord = node->GetItem(convKey->GetPosition());

			int weight = cSignatureRecord::ComputeSignatureWeight(sigRecord, dimSD);
			
			pWeights[i] += (double) weight;
			if (weight == 0)
				pZeros[i]++;

			mArray->UnlockR(&node);
		}
	}

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void cSignatureStorage_3::ClearNodeSignature(uint nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cSignatureKey* pConvKey)
{
	uint nOfDims = pSignatureParams->GetDimension();
	
	for (uint i = 0; i < nOfDims; i++)
	{
		cSpaceDescriptor *dimSD = pSignatureSDs[i];
		uint nOfChunks = pSignatureParams->GetChunkCount(i);

		for (uint j = 0; j < nOfChunks; j++)
		{
			if (SignatureExists(pConvKey, nodeIndex, i, j))
			{
				cSequentialArrayNode<cSignatureRecord> *node = mArray->ReadNodeW(pConvKey->GetSignatureIndex());
				char* sigRecord = node->GetItem(pConvKey->GetPosition());

				cSignatureRecord::ClearTuple(sigRecord, dimSD);
				mArray->UnlockW(&node);

				cChunkInfo chunkInfo(nodeIndex, i, j, pConvKey->GetSignatureIndex(), pConvKey->GetPosition());
				mSplitChunks->AddItem(chunkInfo);
			}
		}
	}
}

void cSignatureStorage_3::ReplicateNodeSignature(uint destNodeIndex, uint srcNodeIndex, uint level, cSignatureParams* pSignatureParams, cInsertSigBuffers* buffers)
{
	cSignatureKey* convKey = buffers->ConvIndexKeys[level];
	uint nOfDims = pSignatureParams->GetDimension();
	uint signatureIndex = 0, position = 0;

	for (uint i = 0; i < nOfDims; i++)
	{
		cSignatureRecord* nodeSignature = buffers->SigNodes[level][i];
		uint nOfChunks = pSignatureParams->GetChunkCount(i);
		for (uint j = 0; j < nOfChunks; j++)
		{
			if (SignatureExists(convKey, srcNodeIndex, i, j))
			{
				cSequentialArrayNode<cSignatureRecord> *node = mArray->ReadNodeR(convKey->GetSignatureIndex());
				char* sigRecord = node->GetItem(convKey->GetPosition());
				nodeSignature->CopyFrom(sigRecord, mArray->GetKeyDescriptor());
				mArray->UnlockR(&node);

				mArray->AddItem(signatureIndex, position, *nodeSignature);
				convKey->SetKey(destNodeIndex, i, j, mKeySD);
				convKey->SetData(signatureIndex, position, mDataSD);
				mConversionIndex->Insert(*convKey->GetKey(), *convKey->GetData());
			}
		}
	}
}

void cSignatureStorage_3::CreateItemSignature(uint nodeIndex, uint level, const char* item, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cInsertSigBuffers* buffers, cSignatureController* pSignatureController, sMapItem** pMapTable, uint* pMapTableCounter)
{
	uint chunkLength = pSignatureParams->GetChunkLength();
	uint nOfDims = pSignatureParams->GetDimension();
	cSequentialArrayNode<cSignatureRecord> *node = NULL;
	uint signatureIndex = 0, position = 0;
	uint previousSignatureIndex = UINT_MAX;
	cSignatureKey* convKey = buffers->ConvIndexKeys[level];

	for (uint i = 0; i < nOfDims; i++)
	{
		uint nOfBits = pSignatureParams->GetBitCount(i);
		cSpaceDescriptor *dimSD = pSignatureSDs[i];
		uint value = cTuple::GetUInt(item, i, NULL);
		uint sigLength = pSignatureParams->GetLength(i);

		for (uint j = 0; j < nOfBits; j++)
		{
			uint trueBitOrder = cSignature::GetTrueBitOrder(value, sigLength, j);
			uint sigChunkOrder = trueBitOrder / chunkLength;
			uint chunkTrueBitOrder = trueBitOrder - (sigChunkOrder * chunkLength);

			if (SignatureExists(convKey, nodeIndex, i, sigChunkOrder))
			{
				signatureIndex = convKey->GetSignatureIndex();
				if (signatureIndex != previousSignatureIndex)
				{
					mArray->UnlockW(&node);
					node = mArray->ReadNodeW(signatureIndex);
				}
				char* sigRecord = node->GetItem(convKey->GetPosition());

				cSignatureRecord::SetTrueBit(sigRecord, dimSD, chunkTrueBitOrder);

				UpdateSplitChunks(nodeIndex, i, sigChunkOrder);
				previousSignatureIndex = signatureIndex;
			}
			else
			{
				mArray->UnlockW(&node);

				if (mSplitChunks->GetItemCount() == 0)
				{
					cSignatureRecord* nodeSignature = buffers->SigNodes[level][i];
					cSignatureRecord::ClearTuple(nodeSignature->GetData(), dimSD);
					cSignatureRecord::SetTrueBit(nodeSignature->GetData(), dimSD, chunkTrueBitOrder);
					mArray->AddItem(signatureIndex, position, *nodeSignature);
					previousSignatureIndex = UINT_MAX;
				}
				else
				{
					cChunkInfo* chunk = mSplitChunks->GetItem(0);
					signatureIndex = chunk->GetSignatureIndex();
					position = chunk->GetPosition();

					node = mArray->ReadNodeW(signatureIndex);
					char* sigRecord = node->GetItem(chunk->GetPosition());
					cSignatureRecord::SetTrueBit(sigRecord, dimSD, chunkTrueBitOrder);
					previousSignatureIndex = signatureIndex;

					convKey->SetKey(chunk->GetNodeIndex(), chunk->GetDimension(), chunk->GetChunkOrder(), mKeySD);
					mConversionIndex->Delete(*convKey->GetKey());
					mSplitChunks->DeleteItem((uint) 0);
				}

				convKey->SetKey(nodeIndex, i, sigChunkOrder, mKeySD);
				convKey->SetData(signatureIndex, position, mDataSD);
				mConversionIndex->Insert(*convKey->GetKey(), *convKey->GetData());
			}
		}
	}
	mArray->UnlockW(&node);
}

bool cSignatureStorage_3::IsMatched(uint nodeIndex, uint level, uint orderIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cQueryProcStat *queryProcStat, cRQBuffers<cTuple>* buffers, cSignatureController* pSignatureController)
{
	cArray<ullong> *trueBitOrders = buffers->QueryTrueBitOrders;
	cArray<uint> *narrowDims = buffers->NarrowDimensions;
	uint chunkLength = pSignatureParams->GetChunkLength();
	uint nOfDims = narrowDims->Count();
	cSignatureKey* convKey = buffers->ConvIndexKeys[level];
	cSequentialArrayNode<cSignatureRecord> *node = NULL;
	uint previousSignatureIndex = UINT_MAX;

	uint signatureIndex = 0;
	for (uint i = 0; i < nOfDims; i++)
	{
		uint narrowDim = narrowDims->GetRefItem(i);
		cSpaceDescriptor *dimSD = pSignatureSDs[narrowDim];
		uint nOfBits = pSignatureParams->GetBitCount(narrowDim);

		for (uint j = 0; j < nOfBits; j++)
		{
			uint trueBitOrder = trueBitOrders->GetRefItem(orderIndex++);
			uint sigChunkOrder = trueBitOrder / chunkLength;
			uint chunkTrueBitOrder = trueBitOrder % chunkLength;

			if (SignatureExists(convKey, nodeIndex, narrowDim, sigChunkOrder, queryProcStat, level))
			{
				signatureIndex = convKey->GetSignatureIndex();
				if (signatureIndex != previousSignatureIndex)
				{
					mArray->UnlockR(&node);
					node = mArray->ReadNodeR(signatureIndex);
					queryProcStat->IncSigLarInQuery(level);
				}
			}
			else
			{
				mArray->UnlockR(&node);
				return false;
			}

			char* sigRecord = node->GetItem(convKey->GetPosition());
			bool bitValue = cSignatureRecord::IsMatched(sigRecord, dimSD, chunkTrueBitOrder);

			queryProcStat->IncComputCompareQuery();
			if (!bitValue)
			{
				mArray->UnlockR(node);
				return false;
			}
			previousSignatureIndex = signatureIndex;
		}
	}
	mArray->UnlockR(&node);

	return true;
}

void cSignatureStorage_3::ComputeWeight(tNodeIndex nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, double* pWeights, uint* pZeros, cSignatureKey* convKey)
{
	uint nOfDims = pSignatureParams->GetDimension();
	
	for (uint i = 0; i < nOfDims; i++)
	{
		cSpaceDescriptor *dimSD = pSignatureSDs[i];
		uint nOfChunks = pSignatureParams->GetChunkCount(i);

		double sumWeight = 0;
		for (uint j = 0; j < nOfChunks; j++)
		{
			if (SignatureExists(convKey, nodeIndex, i, j))
			{
				cSequentialArrayNode<cSignatureRecord> *node = mArray->ReadNodeR(convKey->GetSignatureIndex());
				char* sigRecord = node->GetItem(convKey->GetPosition());

				double weight = cSignatureRecord::ComputeSignatureWeight(sigRecord, dimSD);
				sumWeight += weight;
				if (weight == 0)
					pZeros[i]++;

				mArray->UnlockR(&node);
			}
		}
		pWeights[i] += (double) sumWeight;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void cSignatureStorage_4::ClearNodeSignature(uint nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cSignatureKey* pConvKey)
{
	cSpaceDescriptor *dimSD = pSignatureSDs[0];

	if (SignatureExists(pConvKey, nodeIndex))
	{
		cSequentialArrayNode<cSignatureRecord> *node = mArray->ReadNodeW(pConvKey->GetSignatureIndex());
		char* sigRecord = node->GetItem(pConvKey->GetPosition());
		cSignatureRecord::ClearTuple(sigRecord, dimSD);
		mArray->UnlockW(&node);
	}
}

void cSignatureStorage_4::ReplicateNodeSignature(uint destNodeIndex, uint srcNodeIndex, uint level, cSignatureParams* pSignatureParams, cInsertSigBuffers* buffers)
{
	cSignatureKey* convKey = buffers->ConvIndexKeys[level];
	cSignatureRecord* nodeSignature = buffers->SigNodes[level][0];
	uint signatureIndex = 0, position = 0;

	if (SignatureExists(convKey, srcNodeIndex))
	{
		cSequentialArrayNode<cSignatureRecord> *node = mArray->ReadNodeR(convKey->GetSignatureIndex());
		char* sigRecord = node->GetItem(convKey->GetPosition());
		nodeSignature->CopyFrom(sigRecord, mArray->GetKeyDescriptor());
		mArray->UnlockR(&node);

		mArray->AddItem(signatureIndex, position, *nodeSignature);
		convKey->SetKey(destNodeIndex, mKeySD);
		convKey->SetData(signatureIndex, position, mDataSD);
		mConversionIndex->Insert(*convKey->GetKey(), *convKey->GetData());
	}
}

void cSignatureStorage_4::CreateItemSignature(uint nodeIndex, uint level, const char* item, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cInsertSigBuffers* buffers, cSignatureController* pSignatureController, sMapItem** pMapTable, uint* pMapTableCounter)
{
	uint signatureIndex = 0, position = 0;
	cSignatureKey* convKey = buffers->ConvIndexKeys[level];
	cSignatureRecord* nodeSignature = buffers->SigNodes[level][0];
	cSpaceDescriptor *dimSD = pSignatureSDs[0];
	uint sigLength = pSignatureParams->GetLength();
	uint nOfDims = pSignatureParams->GetDimension();
	uint nOfBits = pSignatureParams->GetBitCount();
	uint nOfQueryTypes = pSignatureController->GetQueryTypesCount();
	cSequentialArrayNode<cSignatureRecord> *node = NULL;
	char* sigRecord = NULL;

	bool sigExists = false;
	if (sigExists = SignatureExists(convKey, nodeIndex))
	{
		node = mArray->ReadNodeW(convKey->GetSignatureIndex());
		sigRecord = node->GetItem(convKey->GetPosition());
	}
	else
	{
		sigRecord = nodeSignature->GetData();
		cSignatureRecord::ClearTuple(sigRecord, dimSD);
	}

	for (uint i = 0; i < nOfQueryTypes; i++)
	{
		ullong value = cSignatureRecord::ComputeTupleValue(item, pSignatureController->GetQueryType(i), nOfDims, pMapTable, pMapTableCounter, pSignatureController->GetDomains());

		for (uint j = 0; j < nOfBits; j++)
		{
			uint trueBitOrder = cSignature::GetTrueBitOrder(value, sigLength, j);
			cSignatureRecord::SetTrueBit(sigRecord, dimSD, trueBitOrder);
		}
	}

	if (sigExists)
	{
		mArray->UnlockW(&node);
	}
	else
	{
		mArray->AddItem(signatureIndex, position, *nodeSignature);
		convKey->SetKey(nodeIndex, mKeySD);
		convKey->SetData(signatureIndex, position, mDataSD);
		mConversionIndex->Insert(*convKey->GetKey(), *convKey->GetData());
	}

}


bool cSignatureStorage_4::IsMatched(uint nodeIndex, uint level, uint orderIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cQueryProcStat *queryProcStat, cRQBuffers<cTuple>* buffers, cSignatureController* pSignatureController)
{
	cArray<ullong> *trueBitOrders = buffers->QueryTrueBitOrders;
	cSignatureKey* convKey = buffers->ConvIndexKeys[level];
	cSpaceDescriptor *dimSD = pSignatureSDs[0];
	uint nOfBits = pSignatureParams->GetBitCount();
	uint nRQBits = pSignatureController->GetRQBits();
	cSequentialArrayNode<cSignatureRecord> *node = NULL;
	uint previousChunkOrder = UINT_MAX;
	char* sigRecord = NULL;
	bool result = true;

	if (SignatureExists(convKey, nodeIndex))
	{
		node = mArray->ReadNodeR(convKey->GetSignatureIndex());
		char* sigRecord = node->GetItem(convKey->GetPosition());
		queryProcStat->IncSigLarInQuery(level);

		for (uint i = 0; i < nRQBits; i++)
		{
			result = true;
			for (uint j = 0; j < nOfBits; j++)
			{
				uint trueBitOrder = trueBitOrders->GetRefItem(orderIndex);
				bool bitValue = cSignatureRecord::IsMatched(sigRecord, dimSD, trueBitOrder);

				queryProcStat->IncComputCompareQuery();
				if (!bitValue)
				{
					mArray->UnlockR(&node);
					result = false;
					orderIndex += (nOfBits - j);
					j = nOfBits; // no more cycles, jump to new combination of RQBits;
				}
				else
				{
					orderIndex++;
				}
			}

			if (result)
			{
				break;
			}
		}
	}
	else
	{
		printf("cRTreeSignatureIndex::IsMatched - Signature not exists !!!");
	}
	mArray->UnlockR(&node);

	return result;;
}

void cSignatureStorage_4::ComputeWeight(tNodeIndex nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, double* pWeights, uint* pZeros, cSignatureKey* convKey)
{
	cSpaceDescriptor *dimSD = pSignatureSDs[0];

	if (SignatureExists(convKey, nodeIndex))
	{
		cSequentialArrayNode<cSignatureRecord> *node = mArray->ReadNodeR(convKey->GetSignatureIndex());
		char* sigRecord = node->GetItem(convKey->GetPosition());

		int weight = cSignatureRecord::ComputeSignatureWeight(sigRecord, dimSD);
		
		*pWeights += (double) weight;
		if (weight == 0)
			(*pZeros)++;

		mArray->UnlockR(&node);
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void cSignatureStorage_5::ClearNodeSignature(uint nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cSignatureKey* pConvKey)
{
	uint nOfChunks = pSignatureParams->GetChunkCount();
	cSpaceDescriptor *dimSD = pSignatureSDs[0];

	for (uint i = 0; i < nOfChunks; i++)
	{
		if (SignatureExists(pConvKey, nodeIndex, i))
		{
			cSequentialArrayNode<cSignatureRecord> *node = mArray->ReadNodeW(pConvKey->GetSignatureIndex());
			char* sigRecord = node->GetItem(pConvKey->GetPosition());
			cSignatureRecord::ClearTuple(sigRecord, dimSD);
			mArray->UnlockW(&node);

			cChunkInfo chunkInfo(nodeIndex, i, pConvKey->GetSignatureIndex(), pConvKey->GetPosition());
			mSplitChunks->AddItem(chunkInfo);
		}
	}
}

void cSignatureStorage_5::ReplicateNodeSignature(uint destNodeIndex, uint srcNodeIndex, uint level, cSignatureParams* pSignatureParams, cInsertSigBuffers* buffers)
{
	uint nOfChunks = pSignatureParams->GetChunkCount();
	cSignatureKey* convKey = buffers->ConvIndexKeys[level];
	cSignatureRecord* nodeSignature = buffers->SigNodes[level][0];
	uint signatureIndex = 0, position = 0;

	for (uint i = 0; i < nOfChunks; i++)
	{
		if (SignatureExists(convKey, srcNodeIndex, i))
		{
			cSequentialArrayNode<cSignatureRecord> *node = mArray->ReadNodeR(convKey->GetSignatureIndex());
			char* sigRecord = node->GetItem(convKey->GetPosition());
			nodeSignature->CopyFrom(sigRecord, mArray->GetKeyDescriptor());
			mArray->UnlockR(&node);

			mArray->AddItem(signatureIndex, position, *nodeSignature);
			convKey->SetKey(destNodeIndex, i, mKeySD);
			convKey->SetData(signatureIndex, position, mDataSD);
			mConversionIndex->Insert(*convKey->GetKey(), *convKey->GetData());
		}
	}
}


void cSignatureStorage_5::CreateItemSignature(uint nodeIndex, uint level, const char* item, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cInsertSigBuffers* buffers, cSignatureController* pSignatureController, sMapItem** pMapTable, uint* pMapTableCounter)
{
	uint signatureIndex = 0, position = 0;
	cSignatureKey* convKey = buffers->ConvIndexKeys[level];
	cSignatureRecord* nodeSignature = buffers->SigNodes[level][0];
	cSpaceDescriptor *dimSD = pSignatureSDs[0];
	uint sigLength = pSignatureParams->GetLength();
	uint chunkLength = pSignatureParams->GetChunkLength();
	uint nOfDims = pSignatureParams->GetDimension();
	uint nOfBits = pSignatureParams->GetBitCount();
	uint nOfQueryTypes = pSignatureController->GetQueryTypesCount();
	cSequentialArrayNode<cSignatureRecord> *node = NULL;
	uint previousSignatureIndex = UINT_MAX;

	for (uint i = 0; i < nOfQueryTypes; i++)
	{
		ullong value = cSignatureRecord::ComputeTupleValue(item, pSignatureController->GetQueryType(i), nOfDims, pMapTable, pMapTableCounter, pSignatureController->GetDomains());

		for (uint j = 0; j < nOfBits; j++)
		{
			uint trueBitOrder = cSignature::GetTrueBitOrder(value, sigLength, j);
			uint sigChunkOrder = trueBitOrder / chunkLength;
			uint chunkTrueBitOrder = trueBitOrder - (sigChunkOrder * chunkLength);

			if (SignatureExists(convKey, nodeIndex, sigChunkOrder))
			{
				signatureIndex = convKey->GetSignatureIndex();
				if (signatureIndex != previousSignatureIndex)
				{
					mArray->UnlockW(&node);
					node = mArray->ReadNodeW(signatureIndex);
					previousSignatureIndex = signatureIndex;
				}
				char* sigRecord = node->GetItem(convKey->GetPosition());
				cSignatureRecord::SetTrueBit(sigRecord, dimSD, chunkTrueBitOrder);

				UpdateSplitChunks(nodeIndex, sigChunkOrder);
			}
			else
			{
				mArray->UnlockW(&node);

				if (mSplitChunks->GetItemCount() == 0)
				{
					char* sigRecord = nodeSignature->GetData();
					cSignatureRecord::ClearTuple(sigRecord, dimSD);
					cSignatureRecord::SetTrueBit(sigRecord, dimSD, chunkTrueBitOrder);
					mArray->AddItem(signatureIndex, position, *nodeSignature);
					previousSignatureIndex = UINT_MAX;
				}
				else
				{
					cChunkInfo* chunk = mSplitChunks->GetItem(0);
					signatureIndex = chunk->GetSignatureIndex();
					position = chunk->GetPosition();

					node = mArray->ReadNodeW(signatureIndex);
					char* sigRecord = node->GetItem(position);
					cSignatureRecord::SetTrueBit(sigRecord, dimSD, chunkTrueBitOrder);

					convKey->SetKey(chunk->GetNodeIndex(), chunk->GetChunkOrder(), mKeySD);
					mConversionIndex->Delete(*convKey->GetKey());
					mSplitChunks->DeleteItem((uint) 0);
				}

				convKey->SetKey(nodeIndex, sigChunkOrder, mKeySD);
				convKey->SetData(signatureIndex, position, mDataSD);
				mConversionIndex->Insert(*convKey->GetKey(), *convKey->GetData());
			}
		}
	}

	mArray->UnlockW(&node);
}


bool cSignatureStorage_5::IsMatched(uint nodeIndex, uint level, uint orderIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cQueryProcStat *queryProcStat, cRQBuffers<cTuple>* buffers, cSignatureController* pSignatureController)
{
	uint signatureIndex = 0, position = 0;
	cArray<ullong> *trueBitOrders = buffers->QueryTrueBitOrders;
	cSignatureKey* convKey = buffers->ConvIndexKeys[level];
	uint nOfBits = pSignatureParams->GetBitCount();
	uint nRQBits = pSignatureController->GetRQBits();
	uint chunkLength = pSignatureParams->GetChunkLength();
	cSequentialArrayNode<cSignatureRecord> *node = NULL;
	cSpaceDescriptor *dimSD = pSignatureSDs[0];
	uint previousSignatureIndex = UINT_MAX;
	char* sigRecord = NULL;
	bool result = true;

	for (uint i = 0; i < nRQBits; i++)
	{
		result = true;
		for (uint j = 0; j < nOfBits; j++)
		{
			uint trueBitOrder = trueBitOrders->GetRefItem(orderIndex);
			uint sigChunkOrder = trueBitOrder / chunkLength;
			uint chunkTrueBitOrder = trueBitOrder % chunkLength;

			if (SignatureExists(convKey, nodeIndex, sigChunkOrder))
			{
				signatureIndex = convKey->GetSignatureIndex();
				if (signatureIndex != previousSignatureIndex)
				{
					mArray->UnlockR(&node);
					node = mArray->ReadNodeR(signatureIndex);
					previousSignatureIndex = signatureIndex;
					queryProcStat->IncSigLarInQuery(level);
				}
				char* sigRecord = node->GetItem(convKey->GetPosition());
				bool bitValue = cSignatureRecord::IsMatched(sigRecord, dimSD, chunkTrueBitOrder);

				queryProcStat->IncComputCompareQuery();
				if (!bitValue)
				{
					result = false;
					orderIndex += (nOfBits - j);
					j = nOfBits; // no more cycles, jump to new combination of RQBits;
				}
				else
				{
					orderIndex++;
				}
			}
			else
			{
				result = false;
				orderIndex += (nOfBits - j);
				j = nOfBits; // no more cycles, jump to new combination of RQBits;
			}
		}

		if (result)
		{
			break;
		}
	}
	mArray->UnlockR(&node);

	return result;
}

void cSignatureStorage_5::ComputeWeight(tNodeIndex nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, double* pWeights, uint* pZeros, cSignatureKey* convKey)
{
	uint nOfChunks = pSignatureParams->GetChunkCount();
	cSpaceDescriptor *dimSD = pSignatureSDs[0];

	int sumWeight = 0;
	for (uint i = 0; i < nOfChunks; i++)
	{
		if (SignatureExists(convKey, nodeIndex, i))
		{
			cSequentialArrayNode<cSignatureRecord> *node = mArray->ReadNodeR(convKey->GetSignatureIndex());
			char* sigRecord = node->GetItem(convKey->GetPosition());
			double weight = cSignatureRecord::ComputeSignatureWeight(sigRecord, dimSD);
			sumWeight += weight;
			if (weight == 0)
				(*pZeros)++;

			mArray->UnlockR(&node);
		}
	}

	*pWeights += (double) sumWeight;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void cSignatureStorage_6::ClearNodeSignature(uint nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cSignatureKey* pConvKey)
{
	cSpaceDescriptor *dimSD = pSignatureSDs[0];

	if (SignatureExists(pConvKey, nodeIndex))
	{
		cSequentialArrayNode<cSignatureRecord> *node = mArray->ReadNodeW(pConvKey->GetSignatureIndex());
		char* sigRecord = node->GetItem(pConvKey->GetPosition());
		cSignatureRecord::ResetTuple(sigRecord, dimSD);
		mArray->UnlockW(&node);
	}
}

void cSignatureStorage_6::ReplicateNodeSignature(uint destNodeIndex, uint srcNodeIndex, uint level, cSignatureParams* pSignatureParams, cInsertSigBuffers* buffers)
{
	printf("cSignatureStorage_6:: ReplicateNodeSignature is not implemented !!!");
}

void cSignatureStorage_6::CreateItemSignature(uint nodeIndex, uint level, const char* item, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cInsertSigBuffers* buffers, cSignatureController* pSignatureController, sMapItem** pMapTable, uint* pMapTableCounter)
{
	uint signatureIndex = 0, position = 0;
	cSignatureKey* convKey = buffers->ConvIndexKeys[level];
	cSignatureRecord* nodeSignature = buffers->SigNodes[level][0];
	cSpaceDescriptor *dimSD = pSignatureSDs[0];
	uint nOfDims = pSignatureParams->GetDimension();
	uint nOfQueryTypes = pSignatureController->GetQueryTypesCount();
	cSequentialArrayNode<cSignatureRecord> *node = NULL;
	char* sigRecord = NULL;


/*	uint length = 0;

	uint c = pSignatureSDs[0]->GetDimension();
	uint d = pSignatureSDs[0]->GetTypeSize();

	if (nodeIndex == 5)
	{
		int f = 3;
	}

	if (SignatureExists(convKey, 5))
	{
		cSequentialArrayNode<cSignatureRecord> *node2 = mArray->ReadNodeW(convKey->GetSignatureIndex());
		char* sigRecord2 = node2->GetItem(convKey->GetPosition());
		length = cLNTuple::GetLength(sigRecord2, dimSD);
		if (length > 85)
		{
			int c = 3;
		}
		mArray->UnlockW(&node2);
	}*/

	bool sigExists = false;
	if (sigExists = SignatureExists(convKey, nodeIndex))
	{
		node = mArray->ReadNodeW(convKey->GetSignatureIndex());
		sigRecord = node->GetItem(convKey->GetPosition());
	}
	else
	{
		sigRecord = nodeSignature->GetData();
		cSignatureRecord::ResetTuple(sigRecord, dimSD);
	}



	for (uint i = 0; i < nOfQueryTypes; i++)
	{
		ullong value = cSignatureRecord::ComputeTupleValue(item, pSignatureController->GetQueryType(i), nOfDims, pMapTable, pMapTableCounter, pSignatureController->GetDomains());
		cSignatureRecord::AddValue(sigRecord, dimSD, value);
	}


	if (sigExists)
	{
		mArray->UnlockW(&node);
	}
	else
	{
		mArray->AddMaxItem(signatureIndex, position, *nodeSignature);
		convKey->SetKey(nodeIndex, mKeySD);
		convKey->SetData(signatureIndex, position, mDataSD);
		mConversionIndex->Insert(*convKey->GetKey(), *convKey->GetData());
	}

/*	if (SignatureExists(convKey, 5))
	{
		cSequentialArrayNode<cSignatureRecord> *node2 = mArray->ReadNodeW(convKey->GetSignatureIndex());
		char* sigRecord2 = node2->GetItem(convKey->GetPosition());
		length = cLNTuple::GetLength(sigRecord2, dimSD);
		if (length > 85)
		{
			int c = 3;
		}
		mArray->UnlockW(&node2);
	}*/
}



bool cSignatureStorage_6::IsMatched(uint nodeIndex, uint level, uint orderIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cQueryProcStat *queryProcStat, cRQBuffers<cTuple>* buffers, cSignatureController* pSignatureController)
{
	cArray<ullong> *trueBitOrders = buffers->QueryTrueBitOrders;
	cSignatureKey* convKey = buffers->ConvIndexKeys[level];
	uint nRQBits = pSignatureController->GetRQBits();
	cSequentialArrayNode<cSignatureRecord> *node = NULL;
	cSpaceDescriptor *dimSD = pSignatureSDs[0];
	char* sigRecord = NULL;
	bool result = false;

	if (SignatureExists(convKey, nodeIndex))
	{
		node = mArray->ReadNodeR(convKey->GetSignatureIndex());
		char* sigRecord = node->GetItem(convKey->GetPosition());
		queryProcStat->IncSigLarInQuery(level);

		for (uint i = 0; i < nRQBits; i++)
		{
			ullong trueBitOrder = trueBitOrders->GetRefItem(orderIndex++);
			bool found = cSignatureRecord::ValueExists(sigRecord, dimSD, trueBitOrder, queryProcStat);

			if (found)
			{
				result = true;
				break;
			}
		}
	}
	else
	{
		printf("cRTreeSignatureIndex::IsMatched - Signature not exists !!!");
	}
	mArray->UnlockR(&node);

	return result;
}

void cSignatureStorage_6::ComputeWeight(tNodeIndex nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, double* pWeights, uint* pZeros, cSignatureKey* convKey)
{
	cSpaceDescriptor *dimSD = pSignatureSDs[0];

	if (SignatureExists(convKey, nodeIndex))
	{
		cSequentialArrayNode<cSignatureRecord> *node = mArray->ReadNodeR(convKey->GetSignatureIndex());
		char* sigRecord = node->GetItem(convKey->GetPosition());

		uint weight = cSignatureRecord::GetItemsCount(sigRecord, dimSD);
		*pWeights += (double) weight;
		if (weight == 0)
			(*pZeros)++;

		mArray->UnlockR(&node);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void cSignatureStorage_7::ClearNodeSignature(uint nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cSignatureKey* pConvKey)
{
	uint nOfChunks = pSignatureParams->GetChunkCount();
	cSpaceDescriptor *dimSD = pSignatureSDs[0];

	for (uint i = 0; i < nOfChunks; i++)
	{
		if (SignatureExists(pConvKey, nodeIndex, i))
		{
			cSequentialArrayNode<cSignatureRecord> *node = mArray->ReadNodeW(pConvKey->GetSignatureIndex());
			char* sigRecord = node->GetItem(pConvKey->GetPosition());
			cSignatureRecord::ResetTuple(sigRecord, dimSD);
			mArray->UnlockW(&node);

			cChunkInfo chunkInfo(nodeIndex, i, pConvKey->GetSignatureIndex(), pConvKey->GetPosition());
			mSplitChunks->AddItem(chunkInfo);
		}
	}
}

void cSignatureStorage_7::ReplicateNodeSignature(uint destNodeIndex, uint srcNodeIndex, uint level, cSignatureParams* pSignatureParams, cInsertSigBuffers* buffers)
{
	printf("cSignatureStorage_7:: ReplicateNodeSignature is not implemented !!!");
}

void cSignatureStorage_7::CreateItemSignature(uint nodeIndex, uint level, const char* item, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cInsertSigBuffers* buffers, cSignatureController* pSignatureController, sMapItem** pMapTable, uint* pMapTableCounter)
{
	uint signatureIndex = UINT_MAX, position = UINT_MAX;
	uint chunkOrder = 0;
	cSignatureKey* convKey = buffers->ConvIndexKeys[level];
	cSignatureRecord* nodeSignature = buffers->SigNodes[level][0];
	uint nOfDims = pSignatureParams->GetDimension();
	uint nOfBits = pSignatureParams->GetBitCount();
	uint nOfChunks = pSignatureParams->GetChunkCount();
	uint nOfQueryTypes = pSignatureController->GetQueryTypesCount();
	uint chunkCapacity = pSignatureParams->GetChunkLength();
	cSequentialArrayNode<cSignatureRecord> *node = NULL;
	cSpaceDescriptor *dimSD = pSignatureSDs[0];
	char* sigRecord = NULL;
	bool sigExists = false;

	for (uint i = 0; i < nOfChunks; i++)
	{
		if (SignatureExists(convKey, nodeIndex, i))
		{
			chunkOrder = i;
			signatureIndex = convKey->GetSignatureIndex();
			position = convKey->GetPosition();

			node = mArray->ReadNodeW(signatureIndex);
			sigRecord = node->GetItem(position);

			if (cSignatureRecord::GetItemsCount(sigRecord, dimSD) < chunkCapacity)
			{
				UpdateSplitChunks(nodeIndex, chunkOrder);
				sigExists = true;
				break;
			}
		}
		else
		{
			break;
		}
	}


	if (!sigExists)
	{
		mArray->UnlockW(&node);
		if (mSplitChunks->GetItemCount() == 0)
		{
			sigRecord = nodeSignature->GetData();
			cSignatureRecord::ResetTuple(sigRecord, dimSD);
			chunkOrder++;
		}
		else
		{
			cChunkInfo* chunk = mSplitChunks->GetItem(0);
			signatureIndex = chunk->GetSignatureIndex();
			position = chunk->GetPosition();

			convKey->SetKey(chunk->GetNodeIndex(), chunk->GetChunkOrder(), mKeySD);
			mConversionIndex->Delete(*convKey->GetKey());
			mSplitChunks->DeleteItem((uint)0);

			convKey->SetKey(nodeIndex, chunkOrder, mKeySD);
			convKey->SetData(signatureIndex, position, mDataSD);
			mConversionIndex->Insert(*convKey->GetKey(), *convKey->GetData());

			node = mArray->ReadNodeW(signatureIndex);
			sigRecord = node->GetItem(position);
			sigExists = true;
		}
	}


	for (uint i = 0; i < nOfQueryTypes; i++)
	{
		ullong value = cSignatureRecord::ComputeTupleValue(item, pSignatureController->GetQueryType(i), nOfDims, pMapTable, pMapTableCounter, pSignatureController->GetDomains());
		cSignatureRecord::AddValue(sigRecord, dimSD, value);
	}

	if (sigExists)
	{
		mArray->UnlockW(&node);
	}
	else
	{
		mArray->AddMaxItem(signatureIndex, position, *nodeSignature);
		convKey->SetKey(nodeIndex, chunkOrder, mKeySD);
		convKey->SetData(signatureIndex, position, mDataSD);
		mConversionIndex->Insert(*convKey->GetKey(), *convKey->GetData());
	}
}

bool cSignatureStorage_7::IsMatched(uint nodeIndex, uint level, uint orderIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, cQueryProcStat *queryProcStat, cRQBuffers<cTuple>* buffers, cSignatureController* pSignatureController)
{
	cArray<ullong> *trueBitOrders = buffers->QueryTrueBitOrders;
	cSignatureKey* convKey = buffers->ConvIndexKeys[level];
	uint nRQBits = pSignatureController->GetRQBits();
	uint nOfChunks = pSignatureParams->GetChunkCount();
	cSpaceDescriptor *dimSD = pSignatureSDs[0];

	for (uint i = 0; i < nRQBits; i++)
	{
		ullong trueBitOrder = trueBitOrders->GetRefItem(orderIndex++);

		for (uint j = 0; j < nOfChunks; j++)
		{
			if (SignatureExists(convKey, nodeIndex, j))
			{
				cSequentialArrayNode<cSignatureRecord> *node = mArray->ReadNodeR(convKey->GetSignatureIndex());
				char* sigRecord = node->GetItem(convKey->GetPosition());
				queryProcStat->IncSigLarInQuery(level);

				bool found = cSignatureRecord::ValueExists(sigRecord, dimSD, trueBitOrder, queryProcStat);
				mArray->UnlockR(&node);
				
				if (found)
				{
					return true;
				}
			}
			else
			{
				return false;
			}
		}
	}

	return false;
}

void cSignatureStorage_7::ComputeWeight(tNodeIndex nodeIndex, cSignatureParams* pSignatureParams, cSpaceDescriptor** pSignatureSDs, double* pWeights, uint* pZeros, cSignatureKey* convKey)
{
	uint nOfChunks = pSignatureParams->GetChunkCount();
	cSpaceDescriptor *dimSD = pSignatureSDs[0];

	uint weight = 0;
	for (uint i = 0; i < nOfChunks; i++)
	{
		if (SignatureExists(convKey, nodeIndex, i))
		{
			cSequentialArrayNode<cSignatureRecord> *node = mArray->ReadNodeR(convKey->GetSignatureIndex());
			char* sigRecord = node->GetItem(convKey->GetPosition());
			weight += cSignatureRecord::GetItemsCount(sigRecord, dimSD);
			mArray->UnlockR(&node);
		}
	}

	*pWeights += (double) weight;
	if (weight == 0)
		(*pZeros)++;
}

}}}