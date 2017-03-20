#include "cSignatureController.h"

cSignatureController::cSignatureController(uint levels, uint queryTypesCount, uint dimension)
{
	mLevelsCount = levels;
	mDimension = dimension;
	mLevelEnabled = new bool[mLevelsCount];
	mSignatureParams = new cSignatureParams*[mLevelsCount];
	
	for (uint i = 0; i < mLevelsCount; i++)
	{
		mSignatureParams[i] = new cSignatureParams(mDimension);
	}

	mDomains = new uint[mDimension];

	mBuildType = SignatureBuild_Insert;

	mQueryTypesCount = queryTypesCount;
	mQueryTypes = new bool*[mQueryTypesCount];
}

cSignatureController::~cSignatureController()
{
	if (mLevelEnabled != NULL)
	{
		delete mLevelEnabled;
		mLevelEnabled = NULL;
	}

	if (mSignatureParams != NULL)
	{
		for (uint i = 0; i < mLevelsCount; i++)
		{
			delete mSignatureParams[i];
		}

		delete mSignatureParams;
		mSignatureParams = NULL;
	}

	if (mDomains != NULL)
	{
		delete mDomains;
		mDomains = NULL;
	}
	
	if (mQueryTypes != NULL)
	{
		delete mQueryTypes;
		mQueryTypes = NULL;
	}
}


void cSignatureController::Setup_DIS(uint pDimension, uint pNodeItemCapacity, uint pLeafNodeItemCapacity, uint pNodeFreeSpace)
{
	const float AvgUtilization = 0.70f;
	// since I get the bit length length 144 for the avg. leaf node item count 71 and weight 55
	// the bit length for the leaf node is decreased by mean of K
	//const double KL = 1.0;
	// for the inner node, I have detect the oposite problem
	//const double KI = 1.4; // povodne 1.4
	mDimension = pDimension;
	uint LNTUPLE_SIZE = 2;
	

	// we need the same number of 0 and 1-bits, therefore the number of bits is multiplied by 2;
	uint avgLeafNodeItemCount = AvgUtilization * pLeafNodeItemCapacity;
	uint avgNodeItemCount = AvgUtilization * pNodeItemCapacity;

	// the avg. number of items in nodes rooted by a node in the level -2
	uint subTreeAvgItemCount = avgLeafNodeItemCount * avgNodeItemCount;

	uint bLength = 2 * avgLeafNodeItemCount * mSignatureParams[0]->GetBitCount();
	uint byteSize = ByteAlignment(bLength * mBLengthConstant); // the bit length = 2x weight
	for (uint i = 0; i < mDimension; i++)
	{
		mSignatureParams[0]->SetLength(byteSize, i); 
	}
		
	// now compute the avg. number of items rooted by a node in a level
	for (uint i = 1; i < mLevelsCount; i++)
	{
		if (mLevelEnabled[i])
		{
			bLength = 2 * pow((double)avgNodeItemCount, (int)i) * avgLeafNodeItemCount * mSignatureParams[i]->GetBitCount() * mBLengthConstant;
			for (uint j = 0; j < mDimension; j++)
			{
				if (bLength < mDomains[j])
				{
					mSignatureParams[i]->SetLength(ByteAlignment(bLength), j);
				}
				else
				{
					mSignatureParams[i]->SetLength(ByteAlignment(mDomains[j]), j);
					mSignatureParams[i]->SetBitCount(1, j);
				}
			}
		}
	}


	// Ok, the bit-lengths are computed, now compute the chunk bit-length.
	// The rule: if it is necessary (the signature bit-length can be stored in one block),
	//   the chunk bit-length is equal to the signature bit-length,
	//   if it is not possible, the chunk bit-length is set as big as possible, but signatureBitLength % chunkBitLength must be 0.
	for (uint i = 0; i < mLevelsCount; i++)
	{
		cSignatureParams* sigOnLevel = mSignatureParams[i];
		sigOnLevel->SetDimension(pDimension);

		if (mLevelEnabled[i])
		{
			uint chunkBitLength = 0;
			for (uint j = 0; j < pDimension; j++)
			{
				chunkBitLength += ByteAlignment(sigOnLevel->GetLength(j));
			}
			uint sigByteSize = (chunkBitLength / cNumber::BYTE_LENGTH) + (2 * pDimension * LNTUPLE_SIZE) + LNTUPLE_SIZE;

			if (sigByteSize < pNodeFreeSpace)
			{
				for (uint j = 0; j < pDimension; j++)
				{
					uint bitLength = ByteAlignment(sigOnLevel->GetLength(j));
					sigOnLevel->SetChunkLength(bitLength, j);
					sigOnLevel->SetLength(bitLength, j);
				}
				sigOnLevel->SetKeyType(cKeyType::DIS_NODEINDEX);
			}
			else
			{
				chunkBitLength = ByteAlignment(sigOnLevel->GetMaxLength());
				sigByteSize = (chunkBitLength / cNumber::BYTE_LENGTH) + LNTUPLE_SIZE;

				if (sigByteSize < pNodeFreeSpace)
				{
					for (uint j = 0; j < pDimension; j++)
					{
						uint bitLength = ByteAlignment(sigOnLevel->GetLength(j));
						sigOnLevel->SetChunkLength(bitLength, j);
						sigOnLevel->SetLength(bitLength, j);
					}
					sigOnLevel->SetKeyType(cKeyType::DIS_NODEINDEX_DIMENSION);
				}
				else
				{
					for (uint j = 0; j < pDimension; j++)
					{
						uint sigBitLength = ByteAlignment(sigOnLevel->GetLength(j));

						// try to detect the number of bytes in the last chunk
						// and to do the alignment to pNodeFreeSpace
						uint nodeBitFreeSpace = (pNodeFreeSpace - LNTUPLE_SIZE) * cNumber::BYTE_LENGTH;
						uint nofblocks = sigBitLength / nodeBitFreeSpace;
						float rest = (sigBitLength % nodeBitFreeSpace) / (float) nodeBitFreeSpace;

						if ((rest >= 0.5) || (nofblocks == 0))
						{
							nofblocks++;
						}

						sigBitLength = nofblocks * (pNodeFreeSpace - LNTUPLE_SIZE) * cNumber::BYTE_LENGTH;
						chunkBitLength = sigBitLength / nofblocks;

						sigOnLevel->SetChunkLength(chunkBitLength, j);
						sigOnLevel->SetLength(sigBitLength, j);
					}
					sigOnLevel->SetKeyType(cKeyType::DIS_NODEINDEX_DIMENSION_CHUNKORDER);
				}
			}
		}

		

		for (uint j = 0; j < pDimension; j++)
		{
			sigOnLevel->SetChunkByteSize(static_cast<uint>(ceil(sigOnLevel->GetChunkLength(j) / 8.0)), j);
			sigOnLevel->SetChunkCount(static_cast<uint>(ceil(sigOnLevel->GetLength(j) / (double) sigOnLevel->GetChunkLength(j))), j);
		}

	}
}

void cSignatureController::Setup_DDS(uint pDimension, uint pNodeItemCapacity, uint pLeafNodeItemCapacity, uint pNodeFreeSpace)
{
	const float AvgUtilization = 0.70f;
	// since I get the bit length length 144 for the avg. leaf node item count 71 and weight 55
	// the bit length for the leaf node is decreased by mean of K
	//const double KL = 1.0;
	// for the inner node, I have detect the oposite problem
	//const double KI = 1.4; // povodne 1.4
	mDimension = pDimension;
	uint LNTUPLE_SIZE = 2;


	// we need the same number of 0 and 1-bits, therefore the number of bits is multiplied by 2;
	uint avgLeafNodeItemCount = AvgUtilization * pLeafNodeItemCapacity;
	uint avgNodeItemCount = AvgUtilization * pNodeItemCapacity;

	// the avg. number of items in nodes rooted by a node in the level -2
	uint subTreeAvgItemCount = avgLeafNodeItemCount * avgNodeItemCount;

	uint bLength = 2 * avgLeafNodeItemCount * mSignatureParams[0]->GetBitCount()  * mDimension;
	uint byteSize = ByteAlignment(bLength * mBLengthConstant); // the bit length = 2x weight
	mSignatureParams[0]->SetLength(byteSize);

	// now compute the avg. number of items rooted by a node in a level
	for (uint i = 1; i < mLevelsCount; i++)
	{
		if (mLevelEnabled[i])
		{
			bLength = 2 * pow((double)avgNodeItemCount, (int)i)  * mDimension * avgLeafNodeItemCount * mSignatureParams[i]->GetBitCount() * mBLengthConstant;
			mSignatureParams[i]->SetLength(ByteAlignment(bLength));
		}
	}


	for (uint i = 0; i < mLevelsCount; i++)
	{
		cSignatureParams* sigOnLevel = mSignatureParams[i];
		if (mLevelEnabled[i])
		{
			uint chunkBitLength = ByteAlignment(sigOnLevel->GetLength());
			uint sigByteSize = (chunkBitLength / cNumber::BYTE_LENGTH) + LNTUPLE_SIZE;

			if (sigByteSize < pNodeFreeSpace)
			{
				sigOnLevel->SetChunkLength(chunkBitLength);
				sigOnLevel->SetLength(chunkBitLength);
				sigOnLevel->SetKeyType(cKeyType::DDS_NODEINDEX);
			}
			else
			{
				uint sigBitLength = ByteAlignment(sigOnLevel->GetLength());

				// try to detect the number of bytes in the last chunk
				// and to do the alignment to pNodeFreeSpace
				uint nodeBitFreeSpace = (pNodeFreeSpace - LNTUPLE_SIZE) * cNumber::BYTE_LENGTH;
				uint nofblocks = sigBitLength / nodeBitFreeSpace;
				float rest = (sigBitLength % nodeBitFreeSpace) / (float) nodeBitFreeSpace;

				if (rest >= 0.5)
				{
					nofblocks++;
				}

				sigBitLength = nofblocks * (pNodeFreeSpace - LNTUPLE_SIZE) * cNumber::BYTE_LENGTH;
				chunkBitLength = sigBitLength / nofblocks;

				sigOnLevel->SetChunkLength(chunkBitLength);
				sigOnLevel->SetLength(sigBitLength);
				sigOnLevel->SetKeyType(cKeyType::DDS_NODEINDEX_CHUNKORDER);
			}
		}

		sigOnLevel->SetDimension(pDimension);
		sigOnLevel->SetChunkByteSize(static_cast<uint>(ceil(sigOnLevel->GetChunkLength() / 8.0)));
		sigOnLevel->SetChunkCount(static_cast<uint>(ceil(sigOnLevel->GetLength() / (double) sigOnLevel->GetChunkLength())));
	}
}

void cSignatureController::Setup_DDO(uint pDimension, uint pNodeItemCapacity, uint pLeafNodeItemCapacity, uint pNodeFreeSpace)
{
	mDimension = pDimension;
	uint LNTUPLE_SIZE = 2;
	uint itemSize = sizeof(ullong);

	// the avg. number of items in nodes rooted by a node in the level -2
	uint subTreeAvgItemCount = pLeafNodeItemCapacity * pNodeItemCapacity;

	mSignatureParams[0]->SetLength(pLeafNodeItemCapacity); // the bit length = 2x weight
	// now compute the avg. number of items rooted by a node in a level
	for (uint i = 1; i < mLevelsCount; i++)
	{
		if (mLevelEnabled[i])
		{
			mSignatureParams[i]->SetLength(subTreeAvgItemCount); // the bit length = 2x weight
		}
		subTreeAvgItemCount *= pLeafNodeItemCapacity;
	}


	for (uint i = 0; i < mLevelsCount; i++)
	{
		cSignatureParams* sigOnLevel = mSignatureParams[i];
		if (mLevelEnabled[i])
		{
			uint chunkBitLength = sigOnLevel->GetLength();
			uint sigByteSize = (chunkBitLength * itemSize) + LNTUPLE_SIZE;

			if (sigByteSize < pNodeFreeSpace)
			{
				sigOnLevel->SetChunkLength(chunkBitLength);
				sigOnLevel->SetLength(chunkBitLength);
				sigOnLevel->SetKeyType(cKeyType::DDO_NODEINDEX);
			}
			else
			{
				uint sigLength = sigOnLevel->GetLength();

				// try to detect the number of items in the last chunk
				// and to do the alignment to pNodeFreeSpace
				uint itemsPerNode = (pNodeFreeSpace - LNTUPLE_SIZE) / itemSize;
				uint nofblocks = sigLength / itemsPerNode;
				float rest = (sigLength % itemsPerNode) / (float) itemsPerNode;

				if (rest >= 0.5)
				{
					nofblocks++;
				}

				sigOnLevel->SetChunkLength(itemsPerNode);
				sigOnLevel->SetLength(sigLength);
				sigOnLevel->SetKeyType(cKeyType::DDO_NODEINDEX_CHUNKORDER);
			}

			sigOnLevel->SetDimension(pDimension);
			sigOnLevel->SetChunkByteSize(sigOnLevel->GetChunkLength() * itemSize);
			sigOnLevel->SetChunkCount(static_cast<uint>(ceil(sigOnLevel->GetLength() / (double) sigOnLevel->GetChunkLength())));
		}
	}
}


uint cSignatureController::ByteAlignment(uint bitLength)
{
	uint alignment = bitLength % cNumber::BYTE_LENGTH;
	if (alignment > 0)
	{
		alignment = cNumber::BYTE_LENGTH - alignment;
	}
	return bitLength + alignment;
}

void cSignatureController::Print()
{
	printf("------------------------ cSignatureController::Print() ------------------------\n");
	for (uint i = 0 ; i < mLevelsCount ; i++)
	{
		cSignatureParams* sigOnLevel = mSignatureParams[i];
		if (mLevelEnabled[i])
		{
			printf("InvLvl %u: L: %u,\tS: %uB, Chunk (L: %u, N: %u), BC: %d\n", i, sigOnLevel->GetLength(),
				(sigOnLevel->GetLength() * mDimension) / cNumber::BYTE_LENGTH, sigOnLevel->GetChunkLength(),
				sigOnLevel->GetLength() / sigOnLevel->GetChunkLength(), sigOnLevel->GetBitCount());
		}
	}
	printf("-------------------------------------------------------------------------------\n");
}