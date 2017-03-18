//Proxy methods
//...

//The rest are only AVX methods
#ifdef AVX_ENABLED
/**
* Metoda předpipraví SSE registry podle dimenze dat, tak aby byl optimálně naplněn pro zpracování více tuplů současně.
* ql, qh, tuple jsou výstupní parametry.
*/
template<class TTuple>
inline void cMBRectangle<TTuple>::InicializeAVXRegistry(__m256 &ql1, __m256 &qh1, const float *pql, const float *pqh, const cSpaceDescriptor* pSd)
{
	unsigned int dim = pSd->GetDimension();
	float ql[8];
	float qh[8];
	switch (dim)
	{
	case 1:
	case 5:
	case 6:
	case 7:
	case 8:
	default:
		ql1 = _mm256_loadu_ps(ql);
		qh1 = _mm256_loadu_ps(qh);
		break;
	case 4:
		for (unsigned int i = 0; i<8; i++)
		{
			if (i < 4)
			{
				ql[i] = *(pql + i);
				qh[i] = *(pqh + i);
			}
			else
			{
				ql[i] = *(pql + (i - 4));
				qh[i] = *(pqh + (i - 4));
			}
		}
		ql1 = _mm256_loadu_ps(ql);
		qh1 = _mm256_loadu_ps(qh);
		break;
	case 2:
		for (unsigned int i = 0; i<8; i++)
		{
			if (i < 2)
			{
				ql[i] = *(pql + i);
				qh[i] = *(pqh + i);
			}
			else if (i < 4)
			{
				ql[i] = *(pql + (i - 2));
				qh[i] = *(pql + (i - 2));
			}
			else if (i < 6)
			{
				ql[i] = *(pql + (i - 4));
				qh[i] = *(pqh + (i - 4));
			}
			else if (i < 8)
			{
				ql[i] = *(pql + (i - 6));
				qh[i] = *(pqh + (i - 6));
			}
		}
		ql1 = _mm256_loadu_ps(ql);
		qh1 = _mm256_loadu_ps(qh);
		break;
	case 3:
		for (unsigned int i = 0; i<6; i++)
		{
			if (i < 3)
			{
				ql[i] = *(pql + i);
				qh[i] = *(pqh + i);
			}
			else
			{
				ql[i] = *(pql + (i - 3));
				qh[i] = *(pqh + (i - 3));
			}
		}
		ql1 = _mm256_loadu_ps(ql);
		qh1 = _mm256_loadu_ps(qh);
		break;
	}
}

static inline short getResultFromAVX(unsigned int &dim, __m256 &m4)
{
	short r = _mm256_movemask_ps(m4);
	switch (dim)
	{
	case 1:
		return (r & 1); break;
	case 2:
		return (r & 3); break;
	case 3:
		return (r & 7); break;
	case 4:
		return (r & 15); break;
	case 5:
		return (r & 31); break;
	case 6:
		return (r & 63); break;
	case 7:
		return (r & 127); break;
	case 8:
		return (r); break;
	}
	return 0;
}
/**
* \return true if the tuple is contained into n-dimensional query block.
*/

template<class TTuple>
inline bool cMBRectangle<TTuple>::IsInRectangleAVX(const  float* ql, const  float* qh, float* tuple, const cSpaceDescriptor* pSd)
{
	unsigned int registrySize = 8;
	bool ret = true;
	__m256 m1;
	__m256 m4;
	__m256 ql1, qh1;
	unsigned int j = 0;
	unsigned int dim = pSd->GetDimension();
	unsigned int remainingDim = pSd->GetDimension();
	unsigned int cycles = (dim / ((float)registrySize) + 0.5f);
	short result = 0;

	for (int i = 0; i < cycles; i++)
	{
		if (remainingDim >= registrySize)
		{
			dim = registrySize;
			remainingDim -= registrySize;
		}
		else
		{
			dim = remainingDim;
		}
		ql1 = _mm256_loadu_ps(ql + i*dim);
		qh1 = _mm256_loadu_ps(qh + i*dim);

		j += dim;
		m1 = _mm256_loadu_ps(tuple + i*dim);
		m4 = _mm256_cmp_ps(m1, ql1, _CMP_LT_OQ);
		result = getResultFromAVX(dim, m4);
		if (result != 0)
		{
			ret = false;
			break;
		}
		m4 = _mm256_cmp_ps(m1, qh1, _CMP_GT_OQ);
		result = getResultFromAVX(dim, m4);
		if (result != 0)
		{
			ret = false;
			break;
		}
	}
	return ret;
}
template<class TTuple>
inline bool cMBRectangle<TTuple>::IsInRectangleAVX(const  float* ql, const  float* qh, float* tuple, const cSpaceDescriptor* pSd, short &mask)
{
	unsigned int registrySize = 8;
	bool ret = true;
	__m256 m1;
	__m256 m4;
	__m256 ql1, qh1;
	unsigned int j = 0;
	unsigned int dim = pSd->GetDimension();
	unsigned int remainingDim = pSd->GetDimension();
	unsigned int cycles = (dim / ((float)registrySize) + 0.5f);
	short result = 0;

	for (int i = 0; i < cycles; i++)
	{
		if (remainingDim >= registrySize)
		{
			dim = registrySize;
			remainingDim -= registrySize;
		}
		else
		{
			dim = remainingDim;
		}
		ql1 = _mm256_loadu_ps(ql + i*dim);
		qh1 = _mm256_loadu_ps(qh + i*dim);

		j += dim;
		m1 = _mm256_loadu_ps(tuple + i*dim);
		m4 = _mm256_cmp_ps(m1, ql1, _CMP_LT_OQ);
		//result =getResultFromAVX(dim,m4);
		if (i == cycles - 1)
			result = _mm256_movemask_ps(m4)  & mask;
		else
			result = _mm256_movemask_ps(m4);
		//result = (remainingDim < registrySize) ? _mm256_movemask_ps( m4 ) & mask : _mm256_movemask_ps( m4 );
		if (result != 0)
		{
			ret = false;
			break;
		}
		m4 = _mm256_cmp_ps(m1, qh1, _CMP_GT_OQ);
		if (i == cycles - 1)
			result = _mm256_movemask_ps(m4)  & mask;
		else
			result = _mm256_movemask_ps(m4);
		//result = (remainingDim < registrySize) ? _mm256_movemask_ps( m4 ) & mask : _mm256_movemask_ps( m4 );
		//result =getResultFromAVX(dim,m4);
		if (result != 0)
		{
			ret = false;
			break;
		}
	}
	return ret;
}
template<class TTuple>
inline int cMBRectangle<TTuple>::IsInRectangleAVX_d4(const  float* ql, const  float* qh, float* tuple, const cSpaceDescriptor* pSd)
{
	unsigned int registrySize = 8;
	__m256 m1;
	__m256 m4;
	__m256 ql1, qh1;
	unsigned int j = 0;
	unsigned int dim = pSd->GetDimension();
	unsigned int remainingDim = pSd->GetDimension();
	unsigned int cycles = (dim / ((float)registrySize) + 0.5f);
	short result = 0;
	short result2 = 0;
	for (int i = 0; i < cycles; i++)
	{
		if (remainingDim >= registrySize)
		{
			dim = registrySize;
			remainingDim -= registrySize;
		}
		else
		{
			dim = remainingDim;
		}
		ql1 = _mm256_loadu_ps(ql + i*dim);
		qh1 = _mm256_loadu_ps(qh + i*dim);

		j += dim;
		m1 = _mm256_loadu_ps(tuple + i*dim);

		m4 = _mm256_cmp_ps(m1, qh1, _CMP_LT_OQ);
		result = _mm256_movemask_ps(m4);
		//result =getResultFromAVXRegistryMulti(dim,m4);


		m4 = _mm256_cmp_ps(m1, qh1, _CMP_GT_OQ);
		result2 = _mm256_movemask_ps(m4);
		//result2 =getResultFromAVXRegistryMulti(dim,m4);
		if (dim == 4)
		{
			int r = result | result2;
			//bool firstOk = ((result ==0 || result ==2) && (result2==0 || result2 == 2)) ;
			//bool secondOk = ((result ==0 || result ==3) && (result2==0 || result2 == 3)) ;
			return ((r & 240) == 0 ? 2 : 0) | ((r & 15) == 0 ? 1 : 0);
			/*bool firstOk = (r & 15) == 0;
			bool secondOk = (r & 240) == 0;
			if (firstOk && secondOk) return 1;
			else if (!firstOk && !secondOk) return 0;
			else if (firstOk && !secondOk) return 2;
			else if (!firstOk && secondOk) return 3;
			else return 0;*/
		}
		else
		{
			return (result | result2) != 0;
			//return (result ==0  && result2==0 ) ? 2 : 0 ;
		}
	}
}
#endif