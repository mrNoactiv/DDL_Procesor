
#ifdef CUDA_ENABLED
#include "cCudaTimer.h"
float cCudaTimer::TimeMalloc = 0.0f;
float cCudaTimer::TimeQuery = 0.0f;
float cCudaTimer::TimeDtoH = 0.0f;
float cCudaTimer::TimeHtoD = 0.0f;
float cCudaTimer::TimeResultVector = 0.0f;
float cCudaTimer::TimeSearchArray = 0.0f;
bool cCudaTimer::measureAsync = false;

cCudaTimer::cCudaTimer()
{
	//cudaEventCreate(&start);
	//cudaEventCreate(&stop);
}
cCudaTimer::~cCudaTimer()
{
}
void cCudaTimer::Start()
{
	/*if (measureAsync)
	{*/
		cudaEventCreate(&start);
		cudaEventCreate(&stop);
		cudaEventRecord(start, 0);
		time = 0.0f;
	/*}
	else
	{
		cutCreateTimer(&kernelTime);
		cutResetTimer(kernelTime);
		cutStartTimer(kernelTime);
	}*/
}
void cCudaTimer::Stop()
{
	/*if (measureAsync)
	{*/
		cudaEventRecord( stop, 0 ); 
		cudaEventSynchronize( stop );
		cudaEventElapsedTime( &time, start, stop );
		cudaEventDestroy( start );
		cudaEventDestroy( stop );
	/*}
	else
	{
		cudaThreadSynchronize();
		cutStopTimer(kernelTime);
	}*/
}

float cCudaTimer::GetTime()
{
	/*if (measureAsync)
	{*/
		return time;
	/*}
	else
	{
		return cutGetTimerValue(kernelTime);
	}*/
}
void cCudaTimer::ResetTimers()
{
	cCudaTimer::TimeMalloc = 0.0f;
	cCudaTimer::TimeQuery = 0.0f;
	cCudaTimer::TimeDtoH = 0.0f;
	cCudaTimer::TimeHtoD = 0.0f;
	cCudaTimer::TimeResultVector = 0.0f;
	cCudaTimer::TimeSearchArray = 0.0f;
}
#endif
