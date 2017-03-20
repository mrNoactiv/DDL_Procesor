#ifdef CUDA_ENABLED
#pragma once
#ifndef __cCudaTimer_h__
#define __cCudaTimer_h__


#include "cGpuConst.h"
#include <cuda_runtime.h>
#include <cuda.h>
#include "../../../lib/cuda/cutil.h"

class cCudaTimer
{
private:
	cudaEvent_t start, stop;
	float time;
	unsigned int kernelTime;
protected:
public:
	static bool measureAsync;
	static float TimeMalloc;
	static float TimeQuery;
	static float TimeHtoD;
	static float TimeDtoH;
	static float TimeResultVector;
	static float TimeSearchArray;
	cCudaTimer();
	~cCudaTimer();
	void Start();
	void Stop();
	float GetTime();
	static void ResetTimers();
};

#endif
#endif