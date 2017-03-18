//Proxy methods
template<class TTuple>
inline bool cMBRectangle<TTuple>::IsInRectangleGeneral_SSE(const char* TTuple_ql, const char* TTuple_qh, const char* TTuple_t, const cSpaceDescriptor* pSd)
{
#ifdef SSE_ENABLED
	bool ret;
	if (pSd->GetDimension() < cMBRectangle<TTuple>::SSE_PackCount_Int)
	{
		printf("Warning: Cant invoke IsInRectangleSSE for two tuples comparison.\nMethod must be implemented\nInstead it calls IsInRectangleSSE for single tuple comparison instead.");

		/*__m128i ql1 ;
		__m128i qh1;
		InicializeSSERegistry(ql1,qh1,(unsigned int*)TTuple_ql,(unsigned int*)TTuple_qh,pSd);*/
		ret = IsInRectangleSSE((unsigned int*)TTuple_ql, (unsigned int*)TTuple_qh, (unsigned int*)TTuple_t, pSd);
	}
	else
	{
		ret = IsInRectangleSSE((unsigned int*)TTuple_ql, (unsigned int*)TTuple_qh, (unsigned int*)TTuple_t, pSd);
	}
	if (cMBRectangle<TTuple>::TupleCompare == Processing::SSEValid)  // dut to branch missprediction
	{
		bool retNoSse = IsInRectangleGeneral(TTuple_ql, TTuple_qh, TTuple_t, pSd);
		if (ret != retNoSse)
		{
			printf("Critical Error: IsInRectangleGeneral() != IsInRectangleSSE!\n");
		}
	}
	return ret;
#else
	printf("\nCritical Error! cMBRectangle::IsInRectangleGeneral_SSE() The SSE support is not enabled!");
	return false;
#endif
}

template<class TTuple>
inline bool cMBRectangle<TTuple>::IsIntersectedGeneral_SSE(const char* TTuple_ql1, const char* TTuple_qh1, const char* TTuple_ql2, const char* TTuple_qh2, const cSpaceDescriptor* pSd)
{
#ifdef SSE_ENABLED
	bool ret;
	if (pSd->GetDimension() < cMBRectangle<TTuple>::SSE_PackCount_Int)
	{
		printf("Warning: Cant invoke IsIntersectedSSE for two tuples comparison.\nMethod must be implemented\nInstead it calls IsInterstectedSSE for single tuple comparison.");

		ret = IsIntersectedSSE((unsigned int*)TTuple_ql1, (unsigned int*)TTuple_qh1, (unsigned int*)TTuple_ql2, (unsigned int*)TTuple_qh2, pSd);
		/*
		//for two tuples at once comparison need to implement IsIntersectedSSE(__m128i ql1, __m128i ql1,__m128i mbr_ql,__m128i mbr_qh, pSd);
		__m128i ql1 ;
		__m128i ql1;
		__m128i mbr_ql;
		__m128i mbr_qh;
		InicializeSSERegistry(ql1,qh1,mbr_ql,mbr_qh,pql,pqh,psd);
		IsIntersectedSSE(ql1, ql1, mbr_ql, mbr_qh,pSd);
		*/
	}
	else
	{
		ret = IsIntersectedSSE((unsigned int*)TTuple_ql1, (unsigned int*)TTuple_qh1, (unsigned int*)TTuple_ql2, (unsigned int*)TTuple_qh2, pSd);
	}

	if (cMBRectangle<TTuple>::TupleCompare == Processing::SSEValid)  // dut to branch missprediction
	{
		bool retNoSse = IsIntersectedGeneral(TTuple_ql1, TTuple_qh1, TTuple_ql2, TTuple_qh2, pSd);
		if (ret != retNoSse)
		{
			printf("Critical Error: IsIntersectedGeneral() != IsIntersectedSSE!\n");
		}
	}
	return ret;
#else
	printf("\nCritical Error! cMBRectangle::IsIntersectedGeneral_SSE() The SSE support is not enabled!");
	return false;
#endif
}

//The rest are only SSE inmplentations
#ifdef SSE_ENABLED

/**
* \return true if the tuple is contained into n-dimensional query block.
*/
template<class TTuple>
inline bool cMBRectangle<TTuple>::IsInRectangleSSE( const  unsigned int* ql, const  unsigned int* qh,unsigned int* tuple,const cSpaceDescriptor* pSd) 
{
	bool ret = true;
	__m128i m1, m4;
	__m128i ql1, qh1;
	unsigned int j = 0;
	unsigned int dim = pSd->GetDimension();
	unsigned int remainingDim = pSd->GetDimension();
	unsigned int cycles =  dim % 4 > 0? dim / 4 + 1: dim / 4;
  short result = 0;
	// dnes: short mask = 65535; //binary mask all bits (1111)
	int a = 0;unsigned int b=0;
	for (int i=0 ; i < cycles ; i++) 
	{
		if (remainingDim >= cMBRectangle<TTuple>::SSE_PackCount_Int)
		{
			dim = cMBRectangle<TTuple>::SSE_PackCount_Int;
			remainingDim -= cMBRectangle<TTuple>::SSE_PackCount_Int;
		}
		else
		{
			dim = remainingDim;
			// dnes: mask = getMask(remainingDim);
		}
		InicializeSSERegistry(ql1,qh1,(ql+i*cMBRectangle<TTuple>::SSE_PackCount_Int),(qh+i*cMBRectangle<TTuple>::SSE_PackCount_Int),pSd);
		m1 = _mm_loadu_si128((__m128i*)(tuple + j));
		j += cMBRectangle<TTuple>::SSE_PackCount_Int;

		m4 = _mm_cmplt_epi32(m1, ql1);           // p < pql
		// dnes: result = _mm_movemask_epi8(m4) & mask;
		result = GetResultFromSSE(dim,m4);
		if (result != 0)
		{
			ret = false;
			break;
		}

		m4 = _mm_cmpgt_epi32(m1, qh1);
		// dnes: result = _mm_movemask_epi8(m4) & mask;
		result = GetResultFromSSE(dim,m4);
		if (result != 0)
		{
			ret = false;
			break;
		}
	}
	return ret;
}

/**
* \return 0 - both false; 1 - first true; 2 - second true; 3 - both true.
* For dimension = 2.
*/
template<class TTuple>
inline int cMBRectangle<TTuple>::IsInRectangleSSE( const  __m128i &ql, const __m128i &qh,unsigned int* tuple,const cSpaceDescriptor* pSd)	
{
	__m128i m1, m4;
	unsigned int dim = pSd->GetDimension();
	short result = 0;
	short result2 = 0;
	m1 = _mm_loadu_si128((__m128i*)(tuple ));
	m4 = _mm_cmplt_epi32(m1, ql);           // p < pql
	result = _mm_movemask_epi8(m4);
	m4 = _mm_cmpgt_epi32(m1, qh);
	result2 = _mm_movemask_epi8(m4);		// p > pqh
	return GetResultFromSSE(dim,result,result2);
}

template<class TTuple>
inline bool cMBRectangle<TTuple>::IsIntersectedSSE( const  unsigned int* TTuple_ql1, const  unsigned int* TTuple_qh1, 
	const  unsigned int* TTuple_ql2, const  unsigned int* TTuple_qh2, const cSpaceDescriptor* pSd) 
{ 
	bool ret = true;
	__m128i m4;
	__m128i sse_ql1, sse_qh1, sse_ql2, sse_qh2;
	unsigned int j = 0;
	unsigned int dim = pSd->GetDimension();
	unsigned int remainingDim = pSd->GetDimension();
	unsigned int cycles =  dim % 4 > 0? dim / 4 + 1: dim / 4;
  short result = 0;
	short mask = 65535; //binary mask all bits (1111)
	int a = 0;unsigned int b=0;
	for (int i=0 ; i < cycles ; i++) 
	{
		if (remainingDim >= cMBRectangle<TTuple>::SSE_PackCount_Int)
		{
			dim = cMBRectangle<TTuple>::SSE_PackCount_Int;
			remainingDim -= cMBRectangle<TTuple>::SSE_PackCount_Int;
		}
		else
		{
			dim = remainingDim;
			mask = GetMask_SSE(remainingDim);
		}
		InicializeSSERegistry(sse_ql1, sse_qh1, sse_ql2, sse_qh2, (TTuple_ql1 + i*cMBRectangle<TTuple>::SSE_PackCount_Int),
			(TTuple_qh1 + i*cMBRectangle<TTuple>::SSE_PackCount_Int), (TTuple_ql2 + i*cMBRectangle<TTuple>::SSE_PackCount_Int), 
			(TTuple_qh2 + i*cMBRectangle<TTuple>::SSE_PackCount_Int), pSd);
		j += cMBRectangle<TTuple>::SSE_PackCount_Int;

	/*for (unsigned int i = 0 ; i < pSd->GetDimension() ; i++)
	{
		if (TTuple::Equal(TTuple_ql2, TTuple_qh1, i, pSd) > 0 || TTuple::Equal(TTuple_qh2, TTuple_ql1, i, pSd) < 0)
		{
			ret = false;
			break;
		}
	}*/

		m4 = _mm_cmplt_epi32(sse_qh1, sse_ql2);           // p < pql
		result = _mm_movemask_epi8(m4) & mask;
		//result = getResultFromSSE(dim,m4);
		if (result != 0)
		{
			ret = false;
			break;
		}

		m4 = _mm_cmpgt_epi32(sse_ql1, sse_qh2);
		result = _mm_movemask_epi8(m4) & mask;
		//result = getResultFromSSE(dim,m4);
		if (result != 0)
		{
			ret = false;
			break;
		}
	}
	return ret;
}
template<class TTuple>
inline short cMBRectangle<TTuple>::GetResultFromSSE(unsigned int &dim, short &result1, short &result2)
{
	int r = result1 | result2;
	if (dim == 1)
		return (r & 0x0001) == 0 ? 2 : 0;
	else if (dim == 2)
		return ((r & 0x1100) == 0 ? 2 : 0) | ((r & 0x0011) == 0 ? 1 : 0);
	else if (dim == 3)
		return (r & 0x0111) == 0 ? 1 : 0; 
	else if (dim == 4)
		return (r & 0x1111) == 0 ? 1 : 0;
	else
		return 0;
}
template<class TTuple>
inline short cMBRectangle<TTuple>::GetResultFromSSE(unsigned int &dim, __m128i &m4)
{
	short r = _mm_movemask_epi8( m4 );
	switch (dim)
	{
	case 1:
		return (r & 15); break;
	case 2:
		return (r & 255); break;
	case 3:
		return (r & 4095); break;
	case 4:
		return (r & 65535); break;
	}
	return 0;
}
/**	
* Method loads SSE registry sse_QL a sse_QH. 
*/
template<class TTuple>
inline void cMBRectangle<TTuple>::InicializeSSERegistry(__m128i &ql1,  __m128i &qh1,const unsigned int *p_ql,const unsigned int *p_qh,const cSpaceDescriptor* pSd)	
{
	#ifndef LINUX
		__declspec(align(16)) unsigned int *pql = (unsigned int*)(p_ql);
		__declspec(align(16)) unsigned int *pqh = (unsigned int*)(p_qh);
	#else
		unsigned int *pql = (unsigned int*)(p_ql);
		unsigned int *pqh = (unsigned int*)(p_qh);
	#endif
	unsigned int dim = pSd->GetDimension();
	switch (dim)
	{
	case 1:
	case 3:
	case 4:
	default:
		ql1 = _mm_loadu_si128((__m128i*)pql);
		qh1 = _mm_loadu_si128((__m128i*)pqh);
		break;
	case 2:
		unsigned int ql[4];
		unsigned int qh[4];
		for	(unsigned int i =0;i<4;i++)
		{
			if (i < 2)
			{
				ql[i] = *(pql+i);
				qh[i] = *(pqh+i);
			}
			else
			{
				ql[i] = *(pql+(i-2));
				qh[i] = *(pqh+(i-2));
			}
		}
		ql1 =_mm_loadu_si128( (__m128i*)ql);
		qh1 = _mm_loadu_si128((__m128i*)qh);
		break;
	}
}
/**	
* Method loads four SSE registers for metothod IsIntersected.
*/
template<class TTuple>
inline void cMBRectangle<TTuple>::InicializeSSERegistry(__m128i &ql1,  __m128i &qh1,__m128i &mbr_ql,__m128i &mbr_qh,const unsigned int *p_ql,const unsigned int *p_qh,const unsigned int *p_mbr_ql,const unsigned int *p_mbr_qh,const cSpaceDescriptor* pSd)	
{
	#ifndef LINUX
		__declspec(align(16)) unsigned int *pql = (unsigned int*)(p_ql);
		__declspec(align(16)) unsigned int *pqh = (unsigned int*)(p_qh);
		__declspec(align(16)) unsigned int *pmbrql = (unsigned int*)(p_mbr_ql);
		__declspec(align(16)) unsigned int *pmbrqh = (unsigned int*)(p_mbr_qh);
	#else
		unsigned int *pql = (unsigned int*)(p_ql);
		unsigned int *pqh = (unsigned int*)(p_qh);
		unsigned int *pmbrql = (unsigned int*)(p_mbr_ql);
		unsigned int *pmbrqh = (unsigned int*)(p_mbr_qh);
	#endif

	
	unsigned int dim = pSd->GetDimension();
	switch (dim)
	{
	case 1:
	case 3:
	case 4:
	default:
		ql1 = _mm_loadu_si128((__m128i*)pql);
		qh1 = _mm_loadu_si128((__m128i*)pqh);
		mbr_ql = _mm_loadu_si128((__m128i*)p_mbr_ql);
		mbr_qh = _mm_loadu_si128((__m128i*)p_mbr_qh);
		break;
	case 2:
		unsigned int ql[4];
		unsigned int qh[4];
		unsigned int ml[4];
		unsigned int mh[4];
		for	(unsigned int i =0;i<4;i++)
		{
			if (i < 2)
			{
				ql[i] = *(pql+i);
				qh[i] = *(pqh+i);
				ml[i] = *(p_mbr_ql+i);
				mh[i] = *(p_mbr_qh+i);
			}
			else
			{
				ql[i] = *(pql+(i-2));
				qh[i] = *(pqh+(i-2));
				ml[i] = *(p_mbr_ql+(i-2));
				mh[i] = *(p_mbr_qh+(i-2));
			}
		}
		ql1 =_mm_loadu_si128( (__m128i*)ql);
		qh1 = _mm_loadu_si128((__m128i*)qh);
		mbr_ql = _mm_loadu_si128((__m128i*)ml);
		mbr_qh = _mm_loadu_si128((__m128i*)mh);
		break;
	}
}


//inline short getResultFromSSE(unsigned int &dim, __m128i &m4)
//{
//	short r = _mm_movemask_epi8( m4 );
//	if (dim == 1)
//		return (r & 0x0001); 
//	else if (dim == 2)
//		return (r & 0x0011); 
//	else if (dim == 3)
//		return (r & 0x0111); 
//	else if (dim == 4)
//		return (r & 0x1111); 
//	else
//		return 0;
//}
template<class TTuple>
inline bool cMBRectangle<TTuple>::IsIntersectedSSE(const __m128i* ql1, const __m128i* qh1,const unsigned int* pql2, unsigned int* pqh2,const cSpaceDescriptor* pSd)
{
	#ifndef LINUX
	bool ret = true;
	__m128i m1, m2, m4, m5, m6;
	//const unsigned int regCount = 4;
	unsigned int cycles = pSd->GetDimension() / 4;
	unsigned int j = 0;

	for (int i=0 ; i < cycles ; i++) 
	{
		m1 = _mm_loadu_si128((__m128i*)(pql2 + j));
		m2 = _mm_loadu_si128((__m128i*)(pqh2 + j));
		j += 4;

		m4 = _mm_cmplt_epi32(qh1[i], m1);           // p < pql

		if ((m4.m128i_u64[0] || m4.m128i_u64[1]) != 0x0)
		{
			ret = false;
			break;
		}

		m4 = _mm_cmpgt_epi32(ql1[i], m2);

		if ((m4.m128i_u64[0] || m4.m128i_u64[1]) != 0x0)
		{
			ret = false;
			break;
		}
	}

	return ret;
	#else
		printf("\nNot implemented on Linux");
		return false;
	#endif
}

template<class TTuple>
inline bool cMBRectangle<TTuple>::IsIntersectedSSE(const __m128i* ql1, const __m128i* qh1, const TTuple& ql2, const TTuple& qh2)
{
	#ifndef LINUX
	bool ret = true;
	__m128i m1, m2, m4, m5, m6;
	//const unsigned int regCount = 4;
	unsigned int *pql2 = (unsigned int *)ql2.GetData();
	unsigned int *pqh2 = (unsigned int *)qh2.GetData();
	unsigned int cycles = ql2.GetDimension() / 4;
	unsigned int j = 0;

	for (int i=0 ; i < cycles ; i++) 
	{
		m1 = _mm_loadu_si128((__m128i*)(pql2 + j));
		m2 = _mm_loadu_si128((__m128i*)(pqh2 + j));
		j += 4;

		m4 = _mm_cmplt_epi32(qh1[i], m1);           // p < pql

		if ((m4.m128i_u64[0] || m4.m128i_u64[1]) != 0x0)
		{
			ret = false;
			break;
		}

		m4 = _mm_cmpgt_epi32(ql1[i], m2);

		if ((m4.m128i_u64[0] || m4.m128i_u64[1]) != 0x0)
		{
			ret = false;
			break;
		}
	}

	return ret;
	#else
		printf("\nNot implemented on Linux");
		return false;
	#endif
}
template<class TTuple>
inline short cMBRectangle<TTuple>::GetMask_SSE(uint &dim)
{
	switch (dim)
	{
		/*case 1: return  1; break;
		case 2: return  3; break;
		case 3: return  7; break;
		case 4: return  15; break;
		case 5: return  31; break;
		case 6:	return  63; break;
		case 7:	return  127; break;
		case 8:	return 255; break;*/
	case 1: return  15; break;
	case 2: return  255; break;
	case 3: return  4095; break;
	case 4: return  65535; break;
	}
	return 0;
}
#endif