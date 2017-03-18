#include "dstruct/paged/queryprocessing/cQueryProcStat.h"

namespace dstruct {
	namespace paged {

cQueryProcStat::cQueryProcStat()
{
	Reset();
}

void cQueryProcStat::Reset()
{
	mLarIn = 0;
	mLarLn = 0;
	mRelevantLn = 0;
	mRelevantIn = 0;
	mNofQuery = 0;
	mComputCompare = 0;
	mSiIn = 0;
	mSiLn = 0;
	for (unsigned int i = 0 ; i < MAX_HEIGHT ; i++)
	{
		mLarInLevel[i] = 0;
		mSigLarInLevel[i] = 0;
		mSigCTLarInLevel[i] = 0;
	}
}

void cQueryProcStat::ResetQuery()
{
	mLarInQuery = 0;
	mLarLnQuery = 0;
	mRelevantLnQuery = 0;
	mRelevantInQuery = 0;
	mComputCompareQuery = 0;
	mSiInQuery = 0;
	mSiLnQuery = 0;
	for (unsigned int i = 0 ; i < MAX_HEIGHT ; i++)
	{
		mLarInQueryLevel[i] = 0;
		mSigLarInQueryLevel[i] = 0;
		mSigCTLarInQueryLevel[i] = 0;
	}
}

void cQueryProcStat::Print() const
{
	/*printf("\n----------------------- cQueryProcStat::Print() -----------------------\n");
	printf("Summary Result:\n#Query %llu\n", mNofQuery);
	printf("#Logical Access Read: %llu (#IN: %llu, , #Rel.IN: %llu, #LN: %llu, #Rel.LN: %llu)\n", GetLarN(), mLarIn, GetRelevantIn(), mLarLn, GetRelevantLn());
	printf("#Scan Invocations: %llu (#IN: %llu, #LN: %llu)\n", GetSiN(), mSiIn, mSiLn);
	printf("#Compare: %llu\n", mComputCompare);
	printf("\nAverage Result:\n");
	printf("#Logical Access Read: %.2f (#IN: %.2f, #Rel.IN: %.2f, #LN: %.2f, #Rel.LN: %.2f)\n", GetLarNAvg(), GetLarInAvg(), GetRelevantInAvg(), GetLarLnAvg(), GetRelevantLnAvg());
	printf("#Scan Invocations: %.2f (#IN: %.2f, #LN: %.2f)\n", GetSiNAvg(), GetSiInAvg(), GetSiLnAvg());
	printf("LAR for Levels: ");
	for (unsigned int i = 0 ; i < MAX_HEIGHT ; i++)
	{
		if (mLarInLevel[i] != 0)
		{
			printf("#L %d: %.2f; ", i, (double)mLarInLevel[i] / mNofQuery);
		}
	}*/
	printf("#Compare: %.2f\n", GetComputCompareAvg());
	//printf("--------------------------------------------------------------------\n");
}

void cQueryProcStat::PrintLAR() const
{
	printf("#Logical Access Read: %.2f (#IN: %.2f, #Rel.IN: %.2f, #LN: %.2f, #Rel.LN: %.2f)\n", GetLarNAvg(), GetLarInAvg(), GetRelevantInAvg(), GetLarLnAvg(), GetRelevantLnAvg());
	printf("#Relevancy of Scanned Subtrees [%%]: %.2f (Rel. IN: %.2f, Rel. LN: %.2f)\n", ((GetRelevantInAvg() + GetRelevantLnAvg()) / GetLarNAvg()) * 100, (GetRelevantInAvg() / GetLarInAvg()) * 100, (GetRelevantLnAvg() / GetLarLnAvg()) * 100);
}

void cQueryProcStat::PrintSigLAR(unsigned int levelCount, bool* levelsEnabled) const
{
	
	float sum = 0.0f;
	for (unsigned int i = 0; i < levelCount; i++)
	{
		if (levelsEnabled[i])
		{
			sum += GetSigLarAvg(i);
		}
	}

	printf("#Logical Access Read in Signature Array: %.2f (for InvLvl ", sum);
	for (unsigned int i = 0; i < levelCount; i++)
	{
		if (levelsEnabled[i])
		{
			printf("%i: %.2f", i, GetSigLarAvg(i));
		}
		if (i < levelCount - 1)
			printf("; ");
	}
	printf(")\n");

	///////////////////////////
	sum = 0.0f;
	for (unsigned int i = 0; i < levelCount; i++)
	{
		if (levelsEnabled[i])
		{
			sum += GetSigCTLarAvg(i);
		}
	}

	printf("#Logical Access Read in Signature Conversion Table: %.2f (for InvLvl ", sum);
	for (unsigned int i = 0; i < levelCount; i++)
	{
		if (levelsEnabled[i])
		{
			printf("%i: %.2f", i, GetSigCTLarAvg(i));
		}
		if (i < levelCount - 1)
			printf("; ");
	}
	printf(")\n");

}


void cQueryProcStat::Print2File(char* statFile) const
{
	FILE * file;
	file =  fopen(statFile,"at");

	fprintf(file, "\n----------------------- cQueryProcState::Print() -----------------------\n");
	fprintf(file, "Summary Result:\n#Query %llu\n", mNofQuery);
	fprintf(file, "#Logical Access Read: %llu (#IN: %llu, #LN: %llu, #Rel. LN: %llu)\n", GetLarN(), mLarIn, mLarLn, GetRelevantLn());
	fprintf(file, "#Compare: %llu\n", mComputCompare);
	fprintf(file, "\nAverage Result:\n");
	fprintf(file, "#Logical Access Read: %.2f (#IN: %.2f, #LN: %.2f, #Rel. LN: %d)\n", GetLarNAvg(), GetLarInAvg(), GetLarLnAvg(), GetRelevantLnAvg());
	fprintf(file, "#Compare: %.2f\n", GetComputCompareAvg());
	fprintf(file, "--------------------------------------------------------------------\n");
	fflush(file);
	fclose(file);
}

}}