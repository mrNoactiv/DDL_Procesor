#pragma once
//#define CUDA_CAPABILITY_13
#define DEFINE_CUDA_EXTERN_VAR
#define PRINT 0
//#define CUDA_DEBUG 0


// includes, project
#include "lib/cuda/cutil_inline.h"
#include <cuda_runtime_api.h>
#include <cublas_v2.h>

//#include "rq.h"
#include "rq2.h"
#include "utils_Kernel.h"
#include "nvToolsExtCuda.h" 
#include "cGpuConst.h"
#include "common/utils/cTimer.h"
#include "cCudaParams.h"
cublasStatus_t status;
cublasHandle_t handle;						//the handle to the CUBLAS library context

float tmpTime, tQuery, tCopyHD, tCopyDH;	//timers
float tgCopyHD,tgCopyDH,tgQuering,tgTmp;			//global timers
cudaEvent_t start, stop;


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	
///	Prepare kernel settings for CUDA global functions.
/// </summary>
/// <remarks>	Gajdi, 25.07.2011. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
void prepareKernelSettings(unsigned int maxInputsInBuffer,cudaDeviceProp dp)
{
	ks[0].blockSize = cGpuConst::THREADS_PER_BLOCK;
	ks[0].dimBlock = dim3(cGpuConst::THREADS_PER_BLOCK,1,1);
	ks[0].sharedMemSize = (unsigned int)dp.sharedMemPerBlock;

	ks[0].noChunks = cGpuConst::NO_OF_CHUNKS;
	unsigned int noBlocks = getNumberOfParts(maxInputsInBuffer, cGpuConst::THREADS_PER_BLOCK * ks[0].noChunks);
	if (noBlocks > dp.maxGridSize[0])
	{
		unsigned int multiplicator = noBlocks / dp.maxGridSize[0];
		if ((noBlocks % dp.maxGridSize[0]) != 0)
			multiplicator++;
		ks[0].noChunks *= multiplicator;
		ks[0].dimGrid = getNumberOfParts(maxInputsInBuffer, cGpuConst::THREADS_PER_BLOCK * ks[0].noChunks);
	}
	else
	{
		ks[0].dimGrid = dim3(noBlocks, 1,1);
	}
#if (PRINT==1)
	ks[0].print();
#endif
}
void prepareKernelSettingsOffsets(unsigned int blocksCount,cudaDeviceProp dp)
{
	ks[0].blockSize = dp.maxThreadsPerBlock;
	ks[0].dimBlock = dim3(dp.maxThreadsPerBlock,1,1);
	ks[0].sharedMemSize = (unsigned int)dp.sharedMemPerBlock;

	ks[0].noChunks = 1;
	
	if (blocksCount > dp.maxGridSize[0])
	{
		printf("\nError: Current GPU cannot search all blocks. Maximum thread blocks limiti reached.");
		unsigned int multiplicator = blocksCount / dp.maxGridSize[0];
		if ((blocksCount % dp.maxGridSize[0]) != 0)
			multiplicator++;
		ks[0].noChunks *= multiplicator;
		ks[0].dimGrid = getNumberOfParts(blocksCount, cGpuConst::THREADS_PER_BLOCK * ks[0].noChunks);
	}
	else
	{
		ks[0].dimGrid = dim3(blocksCount, 1,1);
	}
#if (PRINT==1)
	ks[0].print();
#endif
}
extern "C" void copyRQToConstantMemory(unsigned int dim,unsigned int*pql, unsigned int* pqh)
{
	RQElement* rqe = new RQElement[dim];
	for (unsigned int j = 0;j <dim;j++)
	{
		rqe[j].minimum = pql[j]; 
		rqe[j].maximum = pqh[j];
	}
	cutilSafeCall(cudaMemcpyToSymbol(C_RQElement,rqe, dim * sizeof(RQElement), 0, cudaMemcpyHostToDevice));
	DataInfo* di = new DataInfo();
	di->noRQ=1;
	di->dim = dim;
	cutilSafeCall(cudaMemcpyToSymbol(C_dataInfo,di, sizeof(DataInfo), 0, cudaMemcpyHostToDevice));

	//cGpuConst::IsRQInConstantMemory=true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Common memory management. </summary>
/// <remarks>	Gajdi, 25.07.2011. </remarks>
/// <param name="initialize">true to initialize. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////
void commonMemoryManagement(bool initialize)
{
	if (initialize)
	{ 
		maxInputs = dm->fm->getMaxItemsInBuffer();

		//Copy DataInfo to constant memory
		cudaMemcpyToSymbol(C_dataInfo,&dataStorage.info,sizeof(DataInfo));	
#ifdef CUDA_CONSTANT_MEM
		cutilSafeCall(cudaMemcpyToSymbol(C_RQElement, dataStorage.data.H_rqs, dataStorage.info.dim * sizeof(RQElement), 0, cudaMemcpyHostToDevice));
		//cudaMemcpyToSymbol(C_RQElement,&dataStorage.data.H_rqs[0], sizeof(RQElement), sizeof(RQElement)*dataStorage.info.dim, cudaMemcpyHostToDevice);
#endif        

#ifdef __BACKWARD_TEST_
		DataInfo tmpDataInfo;
		cudaMemcpyFromSymbol(&tmpDataInfo, "C_dataInfo", sizeof(DataInfo));		
		dataStorage.info.print();
		dataStorage.data.printRq(dataStorage.info.dim);
#endif

		//Creates a buffer for RangeQueries data on DEVICE
		cutilSafeCall (cudaMalloc((void**)&D_bufferRQ, sizeof(RQElement) * dataStorage.info.dim * dataStorage.info.noRQ ));
		cudaMemcpy( D_bufferRQ, &dataStorage.data.H_rqs[0], sizeof(RQElement) * dataStorage.info.dim * dataStorage.info.noRQ , cudaMemcpyHostToDevice );
#ifdef __BACKWARD_TEST_
		cutilSafeCall (cudaMemcpy( dataStorage.data.H_rqs, &D_bufferRQ[0], sizeof(RQElement) * dataStorage.info.dim * dataStorage.info.noRQ , cudaMemcpyDeviceToHost ));
		for (unsigned int i=0; i<dataStorage.info.dim; i++)
		{
			printf("%u:\t%f\t%f\n", i, (float)dataStorage.data.H_rqs[i].minimum, (float)dataStorage.data.H_rqs[i].maximum);
		}
#endif

#if (USE_MAPPED_MEMORY==1) //Using simple CUDA pinned memory system


		//Creates a new Page-locked buffer on HOST for input data. This buffer hides an original buffer of the FileManager. This should increase performance because of cudaHostAllocWriteCombined nad Page-locking
		cutilSafeCall( cudaHostAlloc((void**)&dataStorage.data.H_inputVectors, sizeof(DATATYPE) * dataStorage.info.dim * maxInputs, cudaHostAllocWriteCombined));
		//cutilSafeCall (cudaMallocHost((void**)&dataStorage.data.H_inputVectors,  sizeof(DATATYPE) * dataStorage.info.dim * maxInputs));
		dm->fm->appendBuffer((char*)dataStorage.data.H_inputVectors, sizeof(DATATYPE) * dataStorage.info.dim * maxInputs, true);

		//Allocates input buffer at the DEVICE
		cutilSafeCall (cudaMalloc((void**)&D_bufferInputVectors, sizeof(DATATYPE) * dataStorage.info.dim * maxInputs ));

		//Creates a buffer for output data on HOST
		cutilSafeCall (cudaMallocHost((void**)&H_bufferResults, sizeof(bool) * maxInputs));

		//Creates a buffer for output data on DEVICE
		cutilSafeCall (cudaMalloc((void**)&D_bufferResults, sizeof(bool) * maxInputs));

#elif (USE_MAPPED_MEMORY==2) //Using CUDA Zero-Copy memory system - mapped memory

		//Creates a new Page-locked, mapped buffer on HOST for input data. This buffer hides an original buffer of the FileManager.
		cutilSafeCall( cudaHostAlloc((void**)&dataStorage.data.H_inputVectors, sizeof(DATATYPE) * dataStorage.info.dim * maxInputs, cudaHostAllocMapped));
		dm->fm->appendBuffer((char*)dataStorage.data.H_inputVectors, sizeof(DATATYPE) * dataStorage.info.dim * maxInputs, true);

		//Gets the DEVICE pointer to the mapped buffer 
		cutilSafeCall( cudaHostGetDevicePointer((void**)&D_bufferInputVectors, (void*)dataStorage.data.H_inputVectors, 0) );

		//Creates a new Page-locked, mapped buffer on HOST for output data.
		cutilSafeCall( cudaHostAlloc((void**)&H_bufferResults, sizeof(bool) * maxInputs, cudaHostAllocMapped));

		//Gets the DEVICE pointer to the mapped buffer 
		cutilSafeCall( cudaHostGetDevicePointer((void**)&D_bufferResults, (void*)H_bufferResults, 0) );
#else //Cuda simple memory
		cutilSafeCall(cudaMallocHost((void**)&dataStorage.data.H_inputVectors, sizeof(DATATYPE) * dataStorage.info.dim * maxInputs));
		//cutilSafeCall (cudaMallocHost((void**)&dataStorage.data.H_inputVectors,  sizeof(DATATYPE) * dataStorage.info.dim * maxInputs));
		dm->fm->appendBuffer((char*)dataStorage.data.H_inputVectors, sizeof(DATATYPE) * dataStorage.info.dim * maxInputs, true);

		//Allocates input buffer at the DEVICE
		cutilSafeCall (cudaMalloc((void**)&D_bufferInputVectors, sizeof(DATATYPE) * dataStorage.info.dim * maxInputs ));

		//Creates a buffer for output data on HOST
		cutilSafeCall (cudaMallocHost((void**)&H_bufferResults, sizeof(bool) * maxInputs));
		//Creates a buffer for output data on DEVICE
		cutilSafeCall (cudaMalloc((void**)&D_bufferResults, sizeof(bool) * maxInputs));
#endif
		printf("GPU memory allocation ... OK\n");
	}

	else
	{
		cudaFree(D_bufferRQ);

#if (USE_MAPPED_MEMORY==1)  //Using simple CUDA pinned memory system without mapping
		cudaFree(D_bufferInputVectors);
		cudaFreeHost(dataStorage.data.H_inputVectors);
		cudaFree(D_bufferResults);
		cudaFreeHost(H_bufferResults);
#elif (USE_MAPPED_MEMORY==2)  //Using CUDA Zero-Copy memory system with mapping
		/*cudaFreeHost(H_bufferInputVectors);
		cudaFreeHost(H_bufferResults);*/
		cudaFree(D_bufferInputVectors);
		cudaFreeHost(dataStorage.data.H_inputVectors);
		cudaFree(D_bufferResults);
		cudaFreeHost(H_bufferResults);
#else //cuda simple memory
		cudaFree(D_bufferInputVectors);
		cudaFreeHost(dataStorage.data.H_inputVectors);
		cudaFree(D_bufferResults);
		cudaFreeHost(H_bufferResults);
#endif

		dm->fm->appendBuffer(0, 0, false);
	}
}
void commonMemoryManagement_B(bool initialize)
{
	if (initialize)
	{
		maxInputs = dm->mb->GetNumberOfTuples();

		//Copy DataInfo to constant memory
		cudaMemcpyToSymbol(C_dataInfo,&dataStorage.info,sizeof(DataInfo));	
#ifdef CUDA_CONSTANT_MEM
		cutilSafeCall(cudaMemcpyToSymbol(C_RQElement, dataStorage.data.H_rqs, dataStorage.info.dim * sizeof(RQElement), 0, cudaMemcpyHostToDevice));
		//cudaMemcpyToSymbol(C_RQElement,&dataStorage.data.H_rqs[0], sizeof(RQElement), sizeof(RQElement)*dataStorage.info.dim, cudaMemcpyHostToDevice);
#endif        

#ifdef __BACKWARD_TEST_
		DataInfo tmpDataInfo;
		cudaMemcpyFromSymbol(&tmpDataInfo, "C_dataInfo", sizeof(DataInfo));		
		dataStorage.info.print();
#endif

		//Creates a buffer for RangeQueries data on DEVICE
		cutilSafeCall (cudaMalloc((void**)&D_bufferRQ, sizeof(RQElement) * dataStorage.info.dim * dataStorage.info.noRQ ));
		cutilSafeCall (cudaMemcpy( D_bufferRQ, &dataStorage.data.H_rqs[0], sizeof(RQElement) * dataStorage.info.dim * dataStorage.info.noRQ , cudaMemcpyHostToDevice ));
#ifdef __BACKWARD_TEST_
		cutilSafeCall (cudaMemcpy( dataStorage.data.H_rqs, &D_bufferRQ[0], sizeof(RQElement) * dataStorage.info.dim * dataStorage.info.noRQ , cudaMemcpyDeviceToHost ));
		for (unsigned int i=0; i<dataStorage.info.dim; i++)
		{
			printf("%u:\t%f\t%f\n", i, (float)dataStorage.data.H_rqs[i].minimum, (float)dataStorage.data.H_rqs[i].maximum);
		}
#endif

#if (USE_MAPPED_MEMORY==1) //Using simple CUDA pinned memory system


		//Creates a new Page-locked buffer on HOST for input data. This buffer hides an original buffer of the FileManager. This should increase performance because of cudaHostAllocWriteCombined nad Page-locking
		//cutilSafeCall( cudaHostAlloc((void**)&dataStorage.data.H_inputVectors, sizeof(DATATYPE) * dataStorage.info.dim * maxInputs, cudaHostAllocWriteCombined));
		//cutilSafeCall (cudaMallocHost((void**)&dataStorage.data.H_inputVectors,  sizeof(DATATYPE) * dataStorage.info.dim * maxInputs));
		//dm->fm->appendBuffer((char*)dataStorage.data.H_inputVectors, sizeof(DATATYPE) * dataStorage.info.dim * maxInputs, true);

		//Allocates input buffer at the DEVICE
		cutilSafeCall (cudaMalloc((void**)&D_bufferInputVectors, sizeof(DATATYPE) * dataStorage.info.dim * maxInputs ));

		//Creates a buffer for output data on HOST
		cutilSafeCall (cudaMallocHost((void**)&H_bufferResults, sizeof(bool) * maxInputs));

		//Creates a buffer for output data on DEVICE
		cutilSafeCall (cudaMalloc((void**)&D_bufferResults, sizeof(bool) * maxInputs));

#elif (USE_MAPPED_MEMORY==2) //Using CUDA Zero-Copy memory system - mapped memory

		//Creates a new Page-locked, mapped buffer on HOST for input data. This buffer hides an original buffer of the FileManager.
		//cutilSafeCall( cudaHostAlloc((void**)&dataStorage.data.H_inputVectors, sizeof(DATATYPE) * dataStorage.info.dim * maxInputs, cudaHostAllocMapped));
		//dm->fm->appendBuffer((char*)dataStorage.data.H_inputVectors, sizeof(DATATYPE) * dataStorage.info.dim * maxInputs, true);

		//Gets the DEVICE pointer to the mapped buffer 
		cutilSafeCall( cudaHostGetDevicePointer((void**)&D_bufferInputVectors, (void*)dataStorage.data.H_inputVectors, 0) );

		//Creates a new Page-locked, mapped buffer on HOST for output data.
		cutilSafeCall( cudaHostAlloc((void**)&H_bufferResults, sizeof(bool) * maxInputs, cudaHostAllocMapped));

		//Gets the DEVICE pointer to the mapped buffer 
		cutilSafeCall( cudaHostGetDevicePointer((void**)&D_bufferResults, (void*)H_bufferResults, 0) );
#else //Cuda simple memory
		//cutilSafeCall(cudaMallocHost((void**)&dataStorage.data.H_inputVectors, sizeof(DATATYPE) * dataStorage.info.dim * maxInputs));
		//cutilSafeCall (cudaMallocHost((void**)&dataStorage.data.H_inputVectors,  sizeof(DATATYPE) * dataStorage.info.dim * maxInputs));
		//dm->fm->appendBuffer((char*)dataStorage.data.H_inputVectors, sizeof(DATATYPE) * dataStorage.info.dim * maxInputs, true);

		//Allocates input buffer at the DEVICE
		cutilSafeCall (cudaMalloc((void**)&D_bufferInputVectors, sizeof(DATATYPE) * dataStorage.info.dim * maxInputs ));

		//Creates a buffer for output data on HOST
		cutilSafeCall (cudaMallocHost((void**)&H_bufferResults, sizeof(bool) * maxInputs)); 
		//Creates a buffer for output data on DEVICE
		cutilSafeCall (cudaMalloc((void**)&D_bufferResults, sizeof(bool) * maxInputs));
#endif
#if (PRINT==1)
		printf("GPU memory allocation ... OK\n");
#endif
	}

	else
	{
		cudaFree(D_bufferRQ);

#if (USE_MAPPED_MEMORY==1)  //Using simple CUDA pinned memory system without mapping
		cudaFree(D_bufferInputVectors);
		cudaFreeHost(dataStorage.data.H_inputVectors);
		cudaFree(D_bufferResults);
		//cudaFreeHost(H_bufferResults);//cant be dealocated in case of rtree in order to get results
#elif (USE_MAPPED_MEMORY==2)  //Using CUDA Zero-Copy memory system with mapping
		/*cudaFreeHost(H_bufferInputVectors);
		cudaFreeHost(H_bufferResults);*/
		cudaFree(D_bufferInputVectors);
		cudaFreeHost(dataStorage.data.H_inputVectors);
		cudaFree(D_bufferResults);
		//cudaFreeHost(H_bufferResults);//cant be dealocated in case of rtree in order to get results
#else //cuda simple memory
		cudaFree(D_bufferInputVectors);
		cudaFreeHost(dataStorage.data.H_inputVectors);
		cudaFree(D_bufferResults);
		// cudaFreeHost(H_bufferResults); //cant be dealocated in case of rtree in order to get results
#endif

		//dm->fm->appendBuffer(0, 0, false);
	}
}
void commonMemoryManagement_B2(bool initialize,unsigned int itemsInOffset,bool* bufferResults)
{
	if (initialize)
	{
#if (USE_MAPPED_MEMORY==1) //Using simple CUDA pinned memory system
		cutilSafeCall (cudaMalloc((void**)&D_bufferResults, sizeof(bool) * itemsInOffset));
#elif (USE_MAPPED_MEMORY==2) //Using CUDA Zero-Copy memory system - mapped memory
		cutilSafeCall( cudaHostGetDevicePointer((void**)&D_bufferResults, (void*)bufferResults, 0) );
#else //Cuda simple memory
		cutilSafeCall (cudaMalloc((void**)&D_bufferResults, sizeof(bool) * itemsInOffset));
#endif
	}

	else
	{
#if (USE_MAPPED_MEMORY==1)  //Using simple CUDA pinned memory system without mapping
		cudaFree(D_bufferResults);
#elif (USE_MAPPED_MEMORY==2)  //Using CUDA Zero-Copy memory system with mapping
		cudaFree(D_bufferResults);
#else //cuda simple memory
		cudaFree(D_bufferResults);
#endif
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Fill buffer of input vectors. </summary>
/// <remarks>	Gajdi, 25.07.2011. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
void fillInputVectors()
{
	unsigned int remainVectors = dataStorage.data.noInputVectors-processedInputs;
	currentInputs = MINIMUM(remainVectors,maxInputs);
	if (!dm->fm->provideSpecificBlock(processedInputs, currentInputs)) exit(1);
#if (CUDA_DEBUG==1)
	for(int i=0;i<currentInputs;i++) //number of tuples in buffer
	{
		printf("Tuple (");
		for (int j=0;j< dataStorage.info.dim;j++)
		{
			printf("%u",dataStorage.data.H_inputVectors[i*dataStorage.info.dim+j]);
			if (j < dataStorage.info.dim-1)
				printf(",");
		}
		printf(") inserted into host buffer\n");
	}
#endif
	cudaEventRecord( start, 0 );
#if (USE_MAPPED_MEMORY==0) || (USE_MAPPED_MEMORY==1)  
	cutilSafeCall (cudaMemcpy( D_bufferInputVectors, &dataStorage.data.H_inputVectors[0], sizeof(DATATYPE) * dataStorage.info.dim * currentInputs, cudaMemcpyHostToDevice ));
#endif
	cudaEventRecord( stop, 0 ); 
	cudaEventSynchronize( stop );
	cudaEventElapsedTime( &tmpTime, start, stop );
	tCopyHD += tmpTime;

#ifdef __BACKWARD_TEST_
	checkHostMatrix<DATATYPE>(dataStorage.data.H_inputVectors, 1, currentInputs , true, "%u ", "Inputs");
	checkDeviceMatrix<DATATYPE>(D_bufferInputVectors,1, currentInputs , true, "%u ", "Inputs");
#endif


	processedInputs += currentInputs;
}
void fillInputVectors_B()
{
	unsigned int remainVectors = dataStorage.data.noInputVectors-processedInputs;
	currentInputs = MINIMUM(remainVectors,maxInputs);
	//if (!dm->mb->provideSpecificBlock(processedInputs, currentInputs)) exit(1);
#if (CUDA_DEBUG==2)
	for(int i=0;i<currentInputs;i++) //number of tuples in buffer
	{
		printf("Tuple (");
		for (int j=0;j< dataStorage.info.dim;j++)
		{
			printf("%u",dataStorage.data.H_inputVectors[i*dataStorage.info.dim+j]);
			if (j < dataStorage.info.dim-1)
				printf(",");
		}
		printf(") inserted into host buffer\n");
	}
#endif
	cudaEventRecord( start, 0 );
#if (USE_MAPPED_MEMORY==0) || (USE_MAPPED_MEMORY==1)  
	cutilSafeCall (cudaMemcpy( D_bufferInputVectors, &dataStorage.data.H_inputVectors[0], sizeof(DATATYPE) * dataStorage.info.dim * currentInputs, cudaMemcpyHostToDevice ));
#endif
	cudaEventRecord( stop, 0 ); 
	cudaEventSynchronize( stop );
	cudaEventElapsedTime( &tmpTime, start, stop );
	tCopyHD += tmpTime;

#ifdef __BACKWARD_TEST_
	checkHostMatrix<DATATYPE>(dataStorage.data.H_inputVectors, 1, currentInputs , true, "%u ", "Inputs");
	checkDeviceMatrix<DATATYPE>(D_bufferInputVectors,1, currentInputs , true, "%u ", "Inputs");
#endif

	processedInputs += currentInputs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Entry point for CUDA project. </summary>
/// <remarks>	Gajdi, 25.07.2011. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void startProcessingOnCUDA()
{
	nvtxRangeId_t id0 = nvtxRangeStart("Initialize And Memory Copy");

	if (!checkDeviceProperties(deviceProp)) return;

	if (!deviceProp.canMapHostMemory) exit(0);
#if (USE_MAPPED_MEMORY==2)													//Using simple CUDA pinned memory system with mapping
	cutilSafeCall( cudaSetDeviceFlags(cudaDeviceMapHost));				//Must be called befor any data is allocated on GPU
#endif

	//Timers
	cudaEventCreate(&start); 
	cudaEventCreate(&stop);
	tQuery = 0.0f;
	tCopyHD = 0.0f;
	tCopyDH = 0.0f;

	commonMemoryManagement(true);

	//Opens output file
	FILE * file;
	char* fileName = new char[256];
	strcpy(fileName, dm->getResults_data_file());
	file = fopen(fileName,"w");

	//-------------------------------------------------------------------------------------
	//Process INPUT VECTORS
	//-------------------------------------------------------------------------------------
	processedInputs = 0;
	bufferOffset = 0;
#if (PRINT==1)
	printf("Computation ... ");
#endif
	long long resultSize = 0;
	nvtxRangeEnd(id0);
	nvtxRangeId_t id1 = nvtxRangeStart("Processing");
	while(processedInputs<dataStorage.data.noInputVectors)
	{
		fillInputVectors();																										//Loads input vectors to the device buffer D_bufferInputVectors

		cudaEventRecord( start, 0 );
		prepareKernelSettings(currentInputs,deviceProp);
		//if (deviceProp.major < 2)
		//	rqHFT<<<ks[0].dimGrid, ks[0].dimBlock, ks[0].sharedMemSize>>>( D_bufferInputVectors, currentInputs, ks[0].noChunks, D_bufferRQ, D_bufferResults);
		//else
		nvtxRangeId_t id2 = nvtxRangeStart("Range query");

#ifdef CUDA_CAPABILITY_13
		rqHFT_v13<<<ks[0].dimGrid, ks[0].dimBlock, ks[0].sharedMemSize>>>( D_bufferInputVectors, currentInputs, ks[0].noChunks, D_bufferRQ, D_bufferResults,cGpuConst::THREADS_PER_BLOCK);
#elif DATAALIGMENT_XXX
		rqHFT_XXX<4><<<ks[0].dimGrid, ks[0].dimBlock, ks[0].sharedMemSize>>>( D_bufferInputVectors, currentInputs, ks[0].noChunks, D_bufferRQ, D_bufferResults,cGpuConst::THREADS_PER_BLOCK);
#else
		rqHFT<4><<<ks[0].dimGrid, ks[0].dimBlock, ks[0].sharedMemSize>>>( D_bufferInputVectors, currentInputs, ks[0].noChunks, D_bufferRQ, D_bufferResults,cGpuConst::THREADS_PER_BLOCK);
#endif
		nvtxRangeEnd(id2);
		cudaEventRecord( stop, 0 ); 
		cudaEventSynchronize( stop );
		cudaEventElapsedTime( &tmpTime, start, stop );
		tQuery += tmpTime;

		//Copy results: D->H

		cudaEventRecord( start, 0 );
#if (USE_MAPPED_MEMORY==0) || (USE_MAPPED_MEMORY==1) 
		cutilSafeCall(cudaMemcpy(H_bufferResults,D_bufferResults,sizeof(bool) * currentInputs, cudaMemcpyDeviceToHost));
#endif
		cudaEventRecord( stop, 0 ); 
		cudaEventSynchronize( stop );
		cudaEventElapsedTime( &tmpTime, start, stop );
		tCopyDH += tmpTime;

#ifdef __BACKWARD_TEST_
		checkHostMatrix<bool>(H_bufferResults, 1, currentInputs, true, "%u ", "Results");
#endif


		//Physical save to disc
		fwrite(H_bufferResults, sizeof(bool), currentInputs, file);
		bufferOffset += currentInputs;
		resultSize += dm->getResultSize(H_bufferResults,currentInputs);
	}
	nvtxRangeEnd(id1);
	//Closes output file
	fflush(file);   
	fclose(file);

	commonMemoryManagement(false);

	printf("Finished\n");
	printf("\nReal bufferSize:\t %u (bytes)\n", dm->fm->getBufferSizeInBytes());
	printf("\nGPU timer ... Quering:\t %f (ms)\n", tQuery);
	printf("\nGPU timer ... Copying H->D:\t %f (ms)\n", tCopyHD);
	printf("\nGPU timer ... Copying D->H:\t %f (ms)\n", tCopyDH);
	printf("\nRange query: ");
	//unsigned int dim = 8; //načítat dimenzi dynamicky
	dm->dataStorage.data.printRq(dm->dataStorage.info.dim);
	printf("\nResulSize: %d",resultSize);
	printf("\n\n");
	dm->saveTime(tQuery, tCopyHD, tCopyDH, dm->fm->getBufferSizeInBytes(),resultSize, true);
	cudaDeviceReset();
}
extern "C" bool* startProcessingOnCUDA_B()
{
	nvtxRangeId_t id0 = nvtxRangeStart("Initialize And Memory Copy");

	if (!checkDeviceProperties(deviceProp)) return 0;

	if (!deviceProp.canMapHostMemory) exit(0);
#if (USE_MAPPED_MEMORY==2)													//Using simple CUDA pinned memory system with mapping
	cutilSafeCall( cudaSetDeviceFlags(cudaDeviceMapHost));				//Must be called befor any data is allocated on GPU
#endif

	//Timers
	cudaEventCreate(&start); 
	cudaEventCreate(&stop);
	tQuery = 0.0f;
	tCopyHD = 0.0f;
	tCopyDH = 0.0f;

	commonMemoryManagement_B(true);
#if (CUDA_DEBUG==2)
	//Opens output file
	FILE * file;
	char* fileName = new char[256];
	strcpy(fileName, dm->getResults_data_file());
	file = fopen(fileName,"w");
#endif
	//-------------------------------------------------------------------------------------
	//Process INPUT VECTORS
	//-------------------------------------------------------------------------------------
	processedInputs = 0;
	bufferOffset = 0;
#if (PRINT==1)
	printf("Computation ... ");
#endif
	long long resultSize = 0;
	nvtxRangeEnd(id0);
	nvtxRangeId_t id1 = nvtxRangeStart("Processing");
	while(processedInputs<dataStorage.data.noInputVectors)
	{
		fillInputVectors_B();	//Loads input vectors to the device buffer D_bufferInputVectors

		cudaEventRecord( start, 0 );
		prepareKernelSettings(currentInputs,deviceProp);
		//if (deviceProp.major < 2)
		//	rqHFT<<<ks[0].dimGrid, ks[0].dimBlock, ks[0].sharedMemSize>>>( D_bufferInputVectors, currentInputs, ks[0].noChunks, D_bufferRQ, D_bufferResults);
		//else
		nvtxRangeId_t id2 = nvtxRangeStart("Range query");

#ifdef CUDA_CAPABILITY_13
		rqHFT_v13<<<ks[0].dimGrid, ks[0].dimBlock, ks[0].sharedMemSize>>>( D_bufferInputVectors, currentInputs, ks[0].noChunks, D_bufferRQ, D_bufferResults,cGpuConst::THREADS_PER_BLOCK);
#else
		rqHFT<4><<<ks[0].dimGrid, ks[0].dimBlock, ks[0].sharedMemSize>>>( D_bufferInputVectors, currentInputs, ks[0].noChunks, D_bufferRQ, D_bufferResults,cGpuConst::THREADS_PER_BLOCK);
#endif
		nvtxRangeEnd(id2);
		cudaEventRecord( stop, 0 ); 
		cudaEventSynchronize( stop );
		cudaEventElapsedTime( &tmpTime, start, stop );
		tQuery += tmpTime;

		//Copy results: D->H

		cudaEventRecord( start, 0 );
#if (USE_MAPPED_MEMORY==0) || (USE_MAPPED_MEMORY==1) 
		cutilSafeCall(cudaMemcpy(H_bufferResults,D_bufferResults,sizeof(bool) * currentInputs, cudaMemcpyDeviceToHost));
#endif
		cudaEventRecord( stop, 0 ); 
		cudaEventSynchronize( stop );
		cudaEventElapsedTime( &tmpTime, start, stop );
		tCopyDH += tmpTime;

#ifdef __BACKWARD_TEST_
		checkHostMatrix<bool>(H_bufferResults, 1, currentInputs, true, "%u ", "Results");
#endif

#if (CUDA_DEBUG==2)
		//Physical save to disc
		fwrite(H_bufferResults, sizeof(bool), currentInputs, file);
#endif
		bufferOffset += currentInputs;
		resultSize += dm->getResultSize(H_bufferResults,currentInputs);
	}
	nvtxRangeEnd(id1);
#if (CUDA_DEBUG==2)
	//Closes output file
	fflush(file);   
	fclose(file);
#endif
	commonMemoryManagement_B(false);
#if (CUDA_DEBUG>=1)
	printf("Finished searching block on GPU\n");
	printf("\nReal bufferSize:\t %u (bytes)\n", dm->mb->GetBufferSizeInBytes());
	printf("\nGPU timer ... Quering:\t %f (ms)\n", tQuery);
	printf("\nGPU timer ... Copying H->D:\t %f (ms)\n", tCopyHD);
	printf("\nGPU timer ... Copying D->H:\t %f (ms)\n", tCopyDH);
	printf("\nRange query: ");
	dm->dataStorage.data.printRq(dm->dataStorage.info.dim);
	printf("\nResulSize: %d",resultSize);
	printf("\n\n");
#endif
#if (CUDA_DEBUG==2)
	dm->saveTime(tQuery, tCopyHD, tCopyDH, dm->mb->GetBufferSizeInBytes(),resultSize, true);
#endif
	tgCopyDH += tCopyDH;
	tgCopyHD += tCopyHD;
	tgQuering += tQuery;
	return H_bufferResults;
}
extern "C" void startProcessingOnCUDA_Universal(cCudaParams params,unsigned int *D_globalMem, cudaDeviceProp dp,bool* H_resultVector,bool* D_resultVector,unsigned int dim, unsigned int memOffset,unsigned int memOffsetSize,unsigned int blocksCount,unsigned int totalInputs,unsigned int* offsets, unsigned int* offsetSizes)
{
	_timeb mStartTime;
	_ftime_s(&mStartTime);
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	//cudaEventRecord(start,0);
	//unsigned int itemsInMemOffset = memOffsetSize / sizeof(DATATYPE);
	//unsigned int tuplesInMemOffset = itemsInMemOffset / dataStorage.info.dim;
	//nvtxRangeId_t id0 = nvtxRangeStart("Initialize And Memory Copy");
	//if (!checkDeviceProperties(deviceProp)) return;
	//if (!deviceProp.canMapHostMemory) exit(0);
	//Timers
	////cudaEventCreate(&start); 
	////cudaEventCreate(&stop);//
	tQuery = 0.0f;
	//tCopyHD = 0.0f; 
	tCopyDH = 0.0f;

	//commonMemoryManagement_B2(true,itemsInMemOffset,H_resultVector);

	//-------------------------------------------------------------------------------------
	//Process INPUT VECTORS
	//-------------------------------------------------------------------------------------
	//nvtxRangeEnd(id0);
	//nvtxRangeId_t id1 = nvtxRangeStart("Processing");
	
	//nvtxRangeId_t id2 = nvtxRangeStart("Range query");
	cudaEventRecord( start, 0 );
	prepareKernelSettingsOffsets(params.NoOfBlocks,dp);
	if (params.AlgorithmType == GPU_ALGORITHM_TYPE::SINGLE_BLOCK)
	{
		rqHFT_Universal<4><<<ks[0].dimGrid, ks[0].dimBlock, ks[0].sharedMemSize>>>( D_globalMem,1,NULL,NULL,params, D_resultVector);
	}
	else if (params.AlgorithmType == GPU_ALGORITHM_TYPE::ONE_TB_ONE_BLOCK || params.AlgorithmType == GPU_ALGORITHM_TYPE::ONE_TB_MULTIPLE_BLOCKS)
	{
		rqHFT_Universal<4><<<ks[0].dimGrid, ks[0].dimBlock, ks[0].sharedMemSize>>>(D_globalMem,blocksCount,offsets,offsetSizes,params, D_resultVector);
	}
	//nvtxRangeEnd(id2);
	cudaEventRecord( stop, 0 ); 
	cudaEventSynchronize( stop );
	cudaEventElapsedTime( &tmpTime, start, stop );
	tQuery += tmpTime; 

	//Copy results: D->H
	cudaEventRecord( start, 0 );
#if (USE_MAPPED_MEMORY==0) || (USE_MAPPED_MEMORY==1) 
	if (params.AlgorithmType == GPU_ALGORITHM_TYPE::SINGLE_BLOCK)
	{
	    unsigned int tuplesInMemOffset = memOffsetSize / sizeof(DATATYPE) / dim;
		cutilSafeCall(cudaMemcpy(H_resultVector,(D_resultVector+ (memOffset/11) * sizeof(bool)),sizeof(bool) * tuplesInMemOffset, cudaMemcpyDeviceToHost));
	}
	else
	{
		(cudaMemcpy(H_resultVector,D_resultVector,sizeof(bool) * totalInputs, cudaMemcpyDeviceToHost));
	}
#endif
	cudaEventRecord( stop, 0 ); 
	cudaEventSynchronize( stop );
	cudaEventElapsedTime( &tmpTime, start, stop );
	tCopyDH += tmpTime;

	tgCopyDH += tCopyDH;
	//tgCopyHD += tCopyHD;
	tgQuering += tQuery;
	_timeb mEndTime;
	_ftime_s(&mEndTime);
	double endTime = mEndTime.time + mEndTime.millitm / 1000.0;
	double startTime = mStartTime.time + mStartTime.millitm / 1000.0;
	double time = endTime - startTime;
	tgTmp += time;
	return;
}

extern "C" void startProcessingOnCUDA_SingleBlock(unsigned int *D_globalMem, cudaDeviceProp dp,bool* H_resultVector,bool* D_resultVector,unsigned int dim, unsigned int memOffset,unsigned int memOffsetSize)
{
	cCudaParams params;
	params.AlgorithmType= GPU_ALGORITHM_TYPE::SINGLE_BLOCK;
	params.ThreadsPerBlock = cGpuConst::THREADS_PER_BLOCK;
	params.NoOfChunks = cGpuConst::NO_OF_CHUNKS;
	params.NoOfBlocks = memOffsetSize / sizeof(DATATYPE);
	params.DebugFlag = cGpuConst::DEBUG_FLAG;
	params.SingleBlockOffset = memOffset;
	params.SingleBlockOffsetSize = memOffsetSize;
	startProcessingOnCUDA_Universal(params,D_globalMem,dp,H_resultVector,D_resultVector,dim,memOffset,memOffsetSize,-1,-1,NULL,NULL);
}
extern "C" void startProcessingOnCUDA_SingleOffset(unsigned int *D_globalMem, cudaDeviceProp dp,bool* H_resultVector,bool* D_resultVector,unsigned int dim, unsigned int blocksCount,unsigned int totalInputs,unsigned int* offsets, unsigned int* offsetSizes)
{
	//single block
	cCudaParams params;
	params.AlgorithmType = GPU_ALGORITHM_TYPE::ONE_TB_ONE_BLOCK;
	params.ThreadsPerBlock = cGpuConst::THREADS_PER_BLOCK;
	params.NoOfBlocks = blocksCount;
	params.NoOfChunks = 1;
	params.DebugFlag = cGpuConst::DEBUG_FLAG;
	startProcessingOnCUDA_Universal(params,D_globalMem,dp,H_resultVector,D_resultVector,dim,-1,-1,blocksCount,totalInputs,offsets,offsetSizes);

}
extern "C" void startProcessingOnCUDA_MultipleOffsets(unsigned int *D_globalMem, cudaDeviceProp dp,bool* H_resultVector,bool* D_resultVector,unsigned int dim, unsigned int blocksCount,unsigned int totalInputs,unsigned int* offsets, unsigned int* offsetSizes)
{
	//multi block
	cCudaParams params;
	params.AlgorithmType = GPU_ALGORITHM_TYPE::ONE_TB_MULTIPLE_BLOCKS;
	params.ThreadsPerBlock = cGpuConst::THREADS_PER_BLOCK;
	params.NoOfBlocks = dp.multiProcessorCount;
	params.NoOfChunks = (blocksCount / params.NoOfBlocks)+1;
	params.DebugFlag = cGpuConst::DEBUG_FLAG;
	startProcessingOnCUDA_Universal(params,D_globalMem,dp,H_resultVector,D_resultVector,dim,-1,-1,blocksCount,totalInputs,offsets,offsetSizes);
	
}

extern "C" void printTimerHD()
{
	printf(";GPU H->D:;%f",tgCopyHD);
	tgCopyHD=0.0;
}
extern "C" void addToTimerHD(float value)
{
	tgCopyHD+=value;
}
extern "C" void printTimerDH()
{
	printf(";GPU D->H: ;%f",tgCopyDH);
	tgCopyDH=0.0;
}
extern "C" void printTimerQuering()
{
	printf(";GPU Quering: ;%f",tgQuering);
	tgQuering=0.0;
}
extern "C" void printTimerTmp()
{
	printf(";SiB:;%f",tgTmp);
	tgCopyHD=0.0;
}
extern "C" void clearTimers()
{
	tgCopyDH=0.0;
	tgCopyHD=0.0;
	tgQuering=0.0;
	tgTmp=0.0;
}