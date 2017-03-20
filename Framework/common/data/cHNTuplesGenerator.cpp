#include "cHNTuplesGenerator.h"

namespace common {
	namespace data {

cHNTuplesGenerator::cHNTuplesGenerator():mFileName(NULL), mDimension(0), mLowBound(0), mHighBound(0), mTuplesCount(0), mTuple(NULL), mStream(NULL), mStreamQ(NULL), mTextFile(NULL)
{
}

cHNTuplesGenerator::cHNTuplesGenerator(
	char* paFileName, 
	cSpaceDescriptor* paSpaceDescriptor)
: mFileName(paFileName)
{
	// Descriptor pro HLAVNI TUPLE
	mSpaceDescriptor = paSpaceDescriptor;

	// Descriptor pro vnitrni TUPLE (2.souradnice)
	vsd = paSpaceDescriptor->GetInnerSpaceDescriptor(1);

	mTuple = new tKey();
	mTuple->Resize(mSpaceDescriptor);

	datapom = new tKeyPom();
	datapom->Resize(vsd);
}

cHNTuplesGenerator::~cHNTuplesGenerator()
{
	if (mTuple != NULL)
	{
		delete mTuple;
	}
	if (mStream != NULL)
	{
		delete mStream;
	}
	if (mStreamQ != NULL)
	{
		delete mStreamQ;
	}

}

unsigned int** cHNTuplesGenerator::CreateTuples(cSpaceDescriptor *spaceDescriptor,unsigned int queriesCount, bool doTextFile)
{
	cHNTuple *mTuple = new cHNTuple();
	
	unsigned int** queries;
    unsigned int step = 0;
    unsigned int position = 0;

	if (queriesCount>0)
	{
		queries = new unsigned int*[queriesCount];
		for (unsigned int i = 0; i < queriesCount; i++)
		{
			queries[i] = new unsigned int[mDimension];
		}

		//queries = new unsigned int[queriesCount];
	    step = mTuplesCount/queriesCount;
	}

	char* tempString = new char[sizeof(int)];

	char fileInfo[1024];
	strcpy(fileInfo, this->GetFileName());
	strcpy(fileInfo + strlen(this->GetFileName()), ".info");
    
	mStream = new cIOStream;
	FILE *mStreamInfo;
	mStatus = TREE_FILE_OPEN;
	unsigned int number = 0;

	if (!mStream->Open(this->GetFileName(), CREATE_ALWAYS))
	{
		mStatus = TREE_FILE_NOT_OPEN;
	}

	if (mStatus==TREE_FILE_OPEN)
	{
		mStream->Write((char*) &mDimension, sizeof(int));
		mStream->Write((char*) &mTuplesCount, sizeof(int));

        if (doTextFile)
		{
		  fopen_s(&mStreamInfo, fileInfo, "w");

      	  fprintf_s(mStreamInfo, "Dimension: %d \n", mDimension);
      	  fprintf_s(mStreamInfo, "Tuples Count: %d \n\n", mTuplesCount);
		}

        mTuple->Resize(spaceDescriptor);

		for (unsigned int i = 0 ; i < this->GetTuplesCount() ; i++)
		{
            if (doTextFile)
				fprintf_s(mStreamInfo, "<");

			for (unsigned int j = 0 ; j < this->GetDimension() ; j++)
			{
				
				//fk number = cNumber::RandomInInterval(this->GetLowBound(),this->GetHighBound());

				if ((queriesCount>0)&&(i==position))
				{
				    queries[position/step][j] = number;
					if (j==this->GetDimension()-1)
					{
						position += step;
					}
				}

				//fk mTuple->SetValue(j,number);
				
	            if (doTextFile)
				{
					if (j>0) 
						fprintf_s(mStreamInfo, ", %u", number);
					else
						fprintf_s(mStreamInfo, "%u", number);
				}
			}
			
            if (doTextFile)
				fprintf_s(mStreamInfo, ">\n");

			mStream->Write((char*) mTuple->GetData(), spaceDescriptor->GetByteSize());
		}
		mStream->Close();
		fclose(mStreamInfo);
	}

	if (queriesCount>0)
	  return queries;
	else
	  return NULL;	
}


void cHNTuplesGenerator::CreateTuplesAndQueries(cSpaceDescriptor *spaceDescriptor,unsigned int queriesCount, bool doTextFile)
{
	cHNTuple *mTuple = new cHNTuple();
	
    unsigned int step = 0;
    unsigned int position = 0;

	if (queriesCount>0)
	{
	    step = mTuplesCount/queriesCount;
	}

	char* tempString = new char[sizeof(int)];
    
    char basicFile[2048];
	strcpy(basicFile, this->GetFileName());

	char fileInfo[2048];
	strcpy(fileInfo, basicFile);
	strcpy(fileInfo + strlen(fileInfo), ".info");

	char fileQuery[2048];
	strcpy(fileQuery, basicFile);
	strcpy(fileQuery + strlen(fileQuery), "Q");

	char fileQueryInfo[2048];
	strcpy(fileQueryInfo, basicFile);
	strcpy(fileQueryInfo + strlen(fileQueryInfo), ".infoQ");

	mStream = new cIOStream;
	mStreamQ = new cIOStream;

	FILE *mStreamInfo;
	FILE *mStreamInfoQ;

	mStatus = TREE_FILE_OPEN;
	unsigned int number = 0;

	if (!mStream->Open(this->GetFileName(), CREATE_ALWAYS))
	{
		mStatus = TREE_FILE_NOT_OPEN;
	}

	if (!mStreamQ->Open(fileQuery, CREATE_ALWAYS))
	{
		mStatus = TREE_FILE_NOT_OPEN;
	}

	if (mStatus==TREE_FILE_OPEN)
	{
		mStream->Write((char*) &mDimension, sizeof(int));
		mStream->Write((char*) &mTuplesCount, sizeof(int));

		mStreamQ->Write((char*) &mDimension, sizeof(int));
		mStreamQ->Write((char*) &mTuplesCount, sizeof(int));

        if (doTextFile)
		{
		  fopen_s(&mStreamInfo, fileInfo, "w");
		  fopen_s(&mStreamInfoQ, fileQueryInfo, "w");

      	  fprintf_s(mStreamInfo, "Dimension: %d \n", mDimension);
      	  fprintf_s(mStreamInfo, "Tuples Count: %d \n\n", mTuplesCount);
		}

        mTuple->Resize(spaceDescriptor);

		for (unsigned int i = 0 ; i < this->GetTuplesCount() ; i++)
		{
            if (doTextFile)
			{
				fprintf_s(mStreamInfo, "<");

				if ((queriesCount>0)&&(i==position))
				{
					fprintf_s(mStreamInfoQ, "<");
				}
			}
			for (unsigned int j = 0 ; j < this->GetDimension() ; j++)
			{
				
				//fk number = cNumber::RandomInInterval(this->GetLowBound(),this->GetHighBound());

				//fk mTuple->SetValue(j,number);
				
	            if (doTextFile)
				{
					if (j>0) 
						fprintf_s(mStreamInfo, ", %u", number);
					else
						fprintf_s(mStreamInfo, "%u", number);

					if ((queriesCount>0)&&(i==position))
					{
						if (j>0) 
							fprintf_s(mStreamInfoQ, ", %u", number);
						else
							fprintf_s(mStreamInfoQ, "%u", number);
					}
				}
			}
			
            if (doTextFile)
			{
				fprintf_s(mStreamInfo, ">\n");
				
				if ((queriesCount>0)&&(i==position))
				{
					fprintf_s(mStreamInfoQ, ">\n");
				}
			}

			mStream->Write((char*) mTuple->GetData(), spaceDescriptor->GetByteSize());
			
			if ((queriesCount>0)&&(i==position))
			{
				mStreamQ->Write((char*) mTuple->GetData(), spaceDescriptor->GetByteSize());
				position += step;
			}
		}
		mStream->Close();
		mStreamQ->Close();
		fclose(mStreamInfo);
		fclose(mStreamInfoQ);
	}

}

bool cHNTuplesGenerator::FileOpen()
{
	mStream = new cIOStream;
	mStatus = TREE_FILE_OPEN;

	if (!mStream->Open(this->GetFileName(), OPEN_ALWAYS))
	{
		mStatus = TREE_FILE_NOT_OPEN;
		return false;
	}
	else
	{
		mStream->Read((char*) &mDimension, sizeof(int));
		mStream->Read((char*) &mTuplesCount, sizeof(int));

		//fk cSpaceDescriptor *spaceDescriptor = new cSpaceDescriptor(this->GetDimension(), new cUIntType());

		//fk mTuple = new cHNTuple(spaceDescriptor);
		return true;
	}
}

void cHNTuplesGenerator::FileClose()
{
	mStream->Close();
}

cHNTuple* cHNTuplesGenerator::GetNextTuple()
{
	//fk mStream->Read(mTuple->GetData(), mTuple->GetSpaceDescriptor()->GetByteSize());
    return mTuple;
}

bool cHNTuplesGenerator::TextFileOpen()
{
    fopen_s(&mTextFile, mFileName, "r");

    fscanf_s(mTextFile, "Dimension: %d \n", &mDimension);
    fscanf_s(mTextFile, "Tuples Count: %d \n\n", &mTuplesCount);

	//fk cSpaceDescriptor *spaceDescriptor = new cSpaceDescriptor(this->GetDimension(), new cUIntType());

	//fk mTuple = new cHNTuple(spaceDescriptor);

	return true;
}

void cHNTuplesGenerator::TextFileClose()
{
	fclose(mTextFile);
}

/*
cTreeTuple* cHNTuplesGenerator::GetNextTupleFromTextFile()
{
	for (unsigned int i=0; i<mDimension; i++)
	{
		if (i!=mDimension-1)
		{
			fscanf_s(mTextFile, "%d, ", mTuple->GetPUInt(i));
		}
		else
		{
			fscanf_s(mTextFile, "%d\n", mTuple->GetPUInt(i));
		}
	}

	/*for (unsigned int j=0; j<mDimension; j++)
	{
		unsigned int bu;
		bu = mTuple->GetUInt(j);
		unsigned int c = bu;
	}*/

//    return mTuple;
//}

cHNTuple* cHNTuplesGenerator::GetNextTupleFromTextFile()
{
	char *ax = new char[2048];
	char min[] = "min";
	char max[] = "max";

	fgets(ax, 2048, mTextFile);
	char* start = ax;
	char* end = ax;

	for (unsigned int i=0; i<mDimension; i++)
	{
		if (i<mDimension-1)
		{
			end = strchr(end+1,',');
			*end = '\0';
		}
		else
		{
			end = strchr(end+1,'\n');
			*end = '\0';
		}

		if (strcmp(start,min)==0)
		{
			//fk mTuple->Clear(i);
		}
		else if (strcmp(start,max)==0)
		{
			//fk mTuple->SetMaxValue(i);
		}
		else
		{
			//fk mTuple->SetValue(i,atoui(start));
		}

		start = end+1;
    }

    delete ax;
    return mTuple;
}

unsigned int cHNTuplesGenerator::atoui ( char* a )
{
 unsigned ui = 0;
 if (a==((void*)0)) return 0;                  //trap zero address pointer.
while (*a>='0' && *a<='9')
 {
  ui = (ui * 10) + (*a-'0' );
  a++;
 }
 return ( ui );
}

void cHNTuplesGenerator::SplitXMLFile(unsigned int pdimenze)
{
	FILE* mTextFile;
	FILE* mTextFile1;
	FILE* mTextFile2;

	unsigned int pocet1 = 0;
	unsigned int pocet2 = 0;
	unsigned int maxDim = 0;

	char buff[2048];
	char buff2[2048];
	char *pch;

	fopen_s(&mTextFile, ".\\Results\\BigXML.txt", "r");
	fopen_s(&mTextFile1, ".\\Results\\BigXML.txt.first", "w");
	fopen_s(&mTextFile2, ".\\Results\\BigXML.txt.second", "w");

	fseek (mTextFile1, 0, SEEK_SET );
	fprintf(mTextFile1, "Dimension:  \n");
	fprintf(mTextFile1, "Tuples Count:         \n\n");

	fseek (mTextFile2, 0, SEEK_SET );
	fprintf(mTextFile2, "Dimension:   \n");
	fprintf(mTextFile2, "Tuples Count:         \n\n");

	fgets(buff, 2048, mTextFile);
	fgets(buff, 2048, mTextFile);
	fgets(buff, 2048, mTextFile);

	while (fgets(buff, 2048, mTextFile))
	{
		strcpy_s(buff2, sizeof(buff), buff);
		//printf("%s",buff2);

		int poc;
		poc = 0;
		pch = strtok(buff, " ,.-");
		while (pch != NULL)
		{
			poc++;
			pch = strtok (NULL, " ,.-");
			
		}

		if (maxDim<poc)
			maxDim = poc;

		if (poc>pdimenze)
		{
			pocet2++;
			fputs(buff2, mTextFile2);
		}
		else
		{
			pocet1++;
			fputs(buff2, mTextFile1);
		}
	}

	fseek (mTextFile1, 0, SEEK_SET );
	fprintf(mTextFile1, "Dimension: %d\n", pdimenze);
	fprintf(mTextFile1, "Tuples Count: %d\n", pocet1);

	fseek (mTextFile2, 0, SEEK_SET );
	fprintf(mTextFile2, "Dimension: %d\n", maxDim);
	fprintf(mTextFile2, "Tuples Count: %d\n", pocet2);

	fclose(mTextFile);
	fclose(mTextFile1);
	fclose(mTextFile2);

}

// FK, upraveny generator aby umel nacitat tuples i bez nekterych chybejicich dimenzi
// a s ruznymi oddelovaci
// Tato verze presunuje druhou pozici (1) nakonec - id_term
cHNTuple* cHNTuplesGenerator::GetNextTupleFromTextFileXML()
{
	const char min[] = "min";
	const char min2[] = "min\12";
	const char max[] = "max";
	const char max2[] ="max\12";

	fgets(ax, 2048, mTextFile);

	unsigned int hodnota;
	unsigned int id_term;
	char * pch;
	pch = strtok(ax, " ,.-");

	unsigned int i = 0;
	while (pch != NULL)
	{
		if ((strncmp(pch,min,3)==0) || (strcmp(pch,min2)==0))
		{
			hodnota = 0;
		}
		else if ((strncmp(pch,max,3)==0) || (strcmp(pch,max2)==0))
		{
			hodnota = UINT_MAX;
		}
		else
		{
			hodnota = atoui(pch);
		}

		if (i==0)
		{
			mTuple->SetValue(0, hodnota, mSpaceDescriptor);
		}
		else if (i==1)
		{
			id_term = hodnota;
		}
		else
		{
			datapom->SetValue(i-2, hodnota, vsd);
		}

		i++;
		pch = strtok (NULL, " ,.-");
	}
	
	if (i<2)
		return NULL;

	datapom->SetLength(i-2);
	//datapom->Print("\n", mSpaceDescriptor->GetInnerSpaceDescriptor(1));

	mTuple->SetValue(1, *datapom, mSpaceDescriptor);
	mTuple->SetValue(2, id_term, mSpaceDescriptor);

	//mTuple->Print("", mSpaceDescriptor);

	return mTuple;
}


}}