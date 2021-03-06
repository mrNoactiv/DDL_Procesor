#ifdef CUDA_ENABLED
#pragma once

#ifdef __DEVICE_EMULATION__
	#define EMUSYNC __syncthreads()
#else
#define EMUSYNC
#endif

#include "globalDefs.h"
#include "lib\cuda\cutil_inline.h"


//--------------------------------------------------------------------------
// NON-TEMPLATE FUNCTIONS
//--------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Check device properties. </summary>
///
/// <remarks>	Gajdi, 5.9.2010. </remarks>
///
/// <param name="deviceProp">	[in,out] the device property. </param>
///
/// <returns>	true if it succeeds, false if it fails. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
__host__ bool checkDeviceProperties(cudaDeviceProp &deviceProp);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Check CUDA error. </summary>
///
/// <remarks>	Gajdi, 5.9.2010. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
__host__ void checkError();

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Convert to float texture. </summary>
///
/// <remarks>	Gajdi, 5.9.2010. </remarks>
///
/// <param name="src">		[in,out] If non-null, source for the texture.</param>
/// <param name="width">	The width. </param>
/// <param name="height">	The height. </param>
/// <param name="dst">		[in,out] If non-null, destination for the texture. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////
__global__ void convertToFloatTexture(const unsigned char *src, const unsigned int width, const unsigned int height, float* dst );

//--------------------------------------------------------------------------
// TEMPLATE FUNCTIONS
//--------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Convert to float pitch texture. </summary>
///
/// <remarks>	Gajdi, 5.9.2010. </remarks>
///
/// <param name="src">				[in,out] If non-null, source for the texture. </param>
/// <param name="minValueIndex">	Zero-based index of the minimum value. </param>
/// <param name="maxValueIndex">	Zero-based index of the maximum value. </param>
/// <param name="width">			The width. </param>
/// <param name="height">			The height. </param>
/// <param name="stride">			The stride. </param>
/// <param name="dst">				[in,out] If non-null, destination for the texture. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////
template<bool normalize>__global__ void convertToFloatPitchTexture(const float *src, const int minValueIndex, const int maxValueIndex, const unsigned int width, const unsigned int height, const unsigned int stride, float* dst)
{
	
	unsigned int col = blockIdx.x * blockDim.x + threadIdx.x;
	unsigned int row = blockIdx.y;

	if (normalize)
	{
		__shared__ float diff;
		__shared__ float maxValue;
		__shared__ float minValue;
		if (threadIdx.x == 0)
		{
			minValue = src[minValueIndex];
			maxValue = src[maxValueIndex];
			diff = (float)(maxValue - minValue);
		}
		__syncthreads();

		if ((col < width) && (row < height))
		{
			dst[row * stride + col] = (diff == 0) ? 0 : (float)(src[row*width+col] - minValue) / diff;
		}
	}
	else
	{
		if ((col < width) && (row < height))
		{
			dst[row * stride + col] = src[row*width+col];
		}
	}
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Gets a vector sum. </summary>
///
/// <remarks>	Gajdi, 5.9.2010. </remarks>
///
/// <param name="v">		[in,out] If non-null, the vector. </param>
/// <param name="vSize">	Size of the vector. </param>
/// <param name="tid">		The thread ID. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////
template< class T> __device__ void getVectorSUM(T *v, unsigned int vSize, const unsigned int tid)
{
	while(vSize > 1)
	{
		unsigned int oddIndex = ((vSize & 1)>0) ? vSize-1 : 0;
		vSize = (vSize >> 1);
		if (tid < vSize)
			v[tid] += v[tid + vSize];
		if (vSize >= 32)
			__syncthreads(); 
		if ((tid == 0)&&(oddIndex>0))
		{
			v[tid] += v[oddIndex];
			__syncthreads(); 
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Check device matrix. </summary>
///
/// <remarks>	Copies the matrix from device to host and prints its elements. </remarks>
///
/// <param name="m">		[in,out] If non-null, the matrix. </param>
/// <param name="mSize">	The size of matrix. </param>
/// <param name="message">	[in,out] If non-null, the message. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////
template< class T> __host__ void checkDeviceMatrix(const T *m, const unsigned int rows, const unsigned int cols, const bool isRowMatrix = true, const char* format = "%f ", const char* message = "")
{
	printf("\n------------------------- DEVICE MEMORY: %s [%u %u] %s\n", message, rows, cols, (isRowMatrix) ? "row matrix" : "column matrix " );
	T *tmp;
	CUDA_SAFE_CALL(cudaMallocHost((void**)&tmp, rows * cols * sizeof(T)));
	CUDA_SAFE_CALL(cudaMemcpy(tmp, m, rows * cols * sizeof(T), cudaMemcpyDeviceToHost));
	for (unsigned int i=0; i<rows * cols; i++)
	{
		if ((isRowMatrix)&&((i%cols)==0))
			printf("\nRow: ");
		if ((!isRowMatrix)&&((i%rows)==0))
			printf("\nCol: ");
		printf(format, tmp[i]);
	}
	cudaFreeHost(tmp);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Check device matrix. </summary>
///
/// <remarks>	Copies the matrix from device to host and prints its elements. </remarks>
///
/// <param name="m">		[in,out] If non-null, the matrix. </param>
/// <param name="mSize">	The size of matrix. </param>
/// <param name="message">	[in,out] If non-null, the message. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////
template< class T> __host__ void checkDeviceMatrixCUBLAS(const T *m, const unsigned int rows, const unsigned int cols, const bool isRowMatrix = true, const char* format = "%f ", const char* message = "")
{
	printf("\n------------------------- DEVICE MEMORY: %s [%u %u] %s\n", message, rows, cols, (isRowMatrix) ? "row matrix" : "column matrix " );
	unsigned int tmpSize = rows * cols * sizeof(T);
	T *tmp = (T*)malloc(tmpSize);
	cublasGetVector (rows *cols, sizeof(T), m, 1, tmp, 1);
	for (unsigned int i=0; i<rows * cols; i++)
	{
		if ((isRowMatrix)&&((i%cols)==0))
			printf("\nRow %u: ", i/cols);
		if ((!isRowMatrix)&&((i%rows)==0))
			printf("\nCol %u: ", i/rows);
		printf(format, tmp[i]);
	}
	free(tmp);
}



////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Check host matrix. </summary>
///
/// <remarks>	Prints all elements of a given matrix storen in host memory.</remarks>
///
/// <param name="m">		[in,out] If non-null, the matrix data. </param>
/// <param name="mSize">	The size of m. </param>
/// <param name="message">	[in,out] If non-null, the message. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////
template< class T> __host__ void checkHostMatrix(const T *m, const unsigned int rows, const unsigned int cols, const bool isRowMatrix = true, const char* format = "%f ", const char* message = "")
{
	printf("\n------------------------- DEVICE MEMORY: %s [%u %u] %s\n", message, rows, cols, (isRowMatrix) ? "row matrix" : "column matrix " );
	for (unsigned int i=0; i<rows * cols; i++)
	{
		if ((isRowMatrix)&&((i%cols)==0))
			printf("\nRow: ");
		if ((!isRowMatrix)&&((i%rows)==0))
			printf("\nCol: ");
		printf(format, m[i]);
	}
}

#endif