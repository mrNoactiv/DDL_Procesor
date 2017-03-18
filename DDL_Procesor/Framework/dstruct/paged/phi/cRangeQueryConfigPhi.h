
#ifdef PHI_ENABLED
#ifndef __cRangeQueryConfigPhi2_h__
#define __cRangeQueryConfigPhi2_h__
#include "common/PhiOffloadPush.h"
#include "stdio.h"
#include <omp.h>
#include "dstruct/paged/phi/cResultSetPhi.h"
#include "common/cCommon.h"

//namespace dstruct { //for now cannot use namespace, phi would not see struct
 // namespace paged {

typedef struct sRangeQueryConfigPhi
{
	uint NoInputs;
	uint NoThreads;
	uint InputsPerThread;
	uint ResultSize;
	uint Dim;
	uint RegisterSize;


};
class cRangeQueryConfigPhi
{
private:
	__declspec(target(mic)) cResultSetPhi<int,int> *mHashTable;
	__declspec(target(mic)) char* mMemory;

public:
	inline void Init();
	inline void Test();

public:
	void __attribute__((target(mic))) Test_ThreadWorker();
	void __attribute__((target(mic))) InitMic();
};

void cRangeQueryConfigPhi::Init()
{

	mMemory = (char*)malloc(1000000);
	char* mem = mMemory;

	mem+= sizeof(cResultSetPhi<int,int>);
	cResultSetPhi<int,int>* h2 = (cResultSetPhi<int,int>*)mMemory;

	#pragma offload target(mic) in(mMemory:length(1000000) alloc_if(1) free_if(0))
	{

	}
	#pragma offload target(mic) out(mMemory:length(1000000) alloc_if(0) free_if(0))
	{
		omp_set_num_threads(10);
		InitMic();
		printf("\nSize: %d", mHashTable->Count());
		#pragma omp parallel
		{
			Test_ThreadWorker();
		}
	}

	fprintf(stderr,"\nFINAL READ %d",h2->Count());

	/*mMemory = (char*)malloc(1000000);
	char* mem = mMemory;
	mHashTable = (cResultSetPhi<int,int>*)mem;
	mem+= sizeof(cResultSetPhi<int,int>);
	mHashTable->Init(mem, 100);
	#pragma offload target(mic) inout(mMemory:length(1000000) alloc_if(1) free_if(0))
	{
		omp_set_num_threads(10);
		InitMic();
		printf("\nSize: %d", mHashTable->Count());
		#pragma omp parallel
		{
			Test_ThreadWorker();
		}
	}
*/
	//printf("\nSize 2: %d", mHashTable->Count());
}
void cRangeQueryConfigPhi::InitMic()
{
#ifdef __MIC__
	//mMemory = new char[1000000];
	char* mem = mMemory;
	mHashTable = (cResultSetPhi<int,int>*)mem;
	mem+= sizeof(cResultSetPhi<int,int>);
	mHashTable->Init(mem, 100);
	printf("\ntest1 done.");
#endif
}
void cRangeQueryConfigPhi::Test_ThreadWorker()
{
#ifdef __MIC__
	int tid = omp_get_thread_num();
	fprintf(stderr,"\nthread %d reads %d",tid,mHashTable->Count());
	mHashTable->Add(tid);
	fprintf(stderr,"\nthread %d reads %d",tid,mHashTable->Count());
#else
	printf("\nFailed to run on PHI.");
#endif
}
//}}
#endif
#include "common/PhiOffloadPop.h"
#endif

