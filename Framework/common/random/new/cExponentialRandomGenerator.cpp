#include "cExponentialRandomGenerator.h"

namespace common {
  namespace random_new {

cExponentialRandomGenerator::cExponentialRandomGenerator(const bool Randomize,double mLambda2):
	cAbstractRandomGenerator(), m_UnifRandom(Randomize), mBuffer(NULL), mBufferCount(0)
{
	this->mLambda2 = mLambda2;
	this->mLambda = sqrt(mLambda2);
	this->mMaxnum = mLambda2;
}

cExponentialRandomGenerator::cExponentialRandomGenerator(const int ARandSeed):
	cAbstractRandomGenerator(), m_UnifRandom(ARandSeed)
{
}


cExponentialRandomGenerator::~cExponentialRandomGenerator()
{
}
double cExponentialRandomGenerator::GetInvExpFun(double y)
{

	double topOfInterval=(1/mLambda)*exp(0.0);
	double inInterval=topOfInterval-(topOfInterval/(mLambda2))*y;
	double x= -(1/(1/mLambda))*log(inInterval/(1/mLambda));
	return x;
}


double cExponentialRandomGenerator::GetNext32()
{	
	//double y= m_UnifRandom.GetNext()*maxnum;
	double n0,n1,n2,n3;
	do {
		n0=rand();
	}while (n0>=256);
	do {
		n1=rand();
	}while (n1>=256);
	do {
		n2=rand();
	}while (n2>=256);
	do {
		n3=rand();
	}while (n3>=256);
	unsigned long n = (unsigned long)(n0*256*256*256+n1*256*256+n2*256+n3);
//	n=(n%((unsigned long)maxnum));
	double y=(double)n;
	return GetInvExpFun(y);
}

double cExponentialRandomGenerator::GetNext()
{
	double dRet;
	do
	{
		dRet = m_UnifRandom.GetNext();
	} while (dRet == 0.0);
	return -log(dRet);
}


unsigned int cExponentialRandomGenerator::RandRange(unsigned int min,unsigned int max) {
		unsigned int n0,n1,n2,n3;
		n0=(unsigned int)rand()%256;
		n1=(unsigned int)rand()%256;
		n2=(unsigned int)rand()%256;
		n3=(unsigned int)rand()%256;
		unsigned int n = (unsigned int)(n0*256*256*256+n1*256*256+n2*256+n3);
		n=n%(max-min+1)+min;
		return n;
}

double cExponentialRandomGenerator::Func(double x) {
	return (1/this->mLambda)*exp(-x/this->mLambda);
}

void cExponentialRandomGenerator::InitBuffer(int count) {
  int i=0;
  double lowerBound=0;
  double upperBound = mLambda2;
  double psum=0;

	if (mBufferCount != count)
	{
		if (mBuffer != NULL)
		{
			delete mBuffer;
		}
		mBuffer = new unsigned int[count];
	}
  mBufferCount=count;	

  while (i<count) {
		//minimalni krok
		double unit=(upperBound-lowerBound)/100;
		if (unit<1) unit=1;
		//zjisti x
		double x=(upperBound-unit);
		if (x<lowerBound) {
			x=lowerBound;
		}
		//vyposti pravdepodobnost x
		double p;
		double p1=Func(x);
		double p2=Func(upperBound);
		p=unit*(p1+p2)/2;
		//cisel
		psum=psum+p;
		double expectedCountD=psum*count-i;
	
		//zaokrouhli nahodne nahoru nebo dolu
		unsigned int randN,check;
		if (expectedCountD>0) {
			unsigned int expectedCount = (unsigned int)expectedCountD;
			double rest=(expectedCountD-(unsigned int)expectedCountD);
			check = (unsigned int)(UINT_MAX*rest);
			randN = RandRange(0,UINT_MAX-1);
			if (check>randN) {
				expectedCount++;
			}
			//napocitej cisla
			for (unsigned int z=0;(z<expectedCount && i<count);z++) {
				mBuffer[i++] = RandRange((unsigned int)x, (unsigned int)(upperBound-1));
				
			}
		}
		upperBound=x;
  }
}

unsigned int cExponentialRandomGenerator::GetNext32buffer(unsigned int max)
{
	  
	unsigned int randN = RandRange(0,UINT_MAX-1);
	double x=((double)mBufferCount/(double)max)*(double)randN;
	unsigned int l = (unsigned int)x;
	unsigned int u = (unsigned int)(x+1);
	if (mBuffer[l]>mBuffer[u]) {
	return (unsigned int)(fabs((double)mBuffer[l]-(double)mBuffer[u])*(x-l)+mBuffer[u]);
	}
	else {
	return (unsigned int)(fabs((double)mBuffer[l]-(double)mBuffer[u])*(x-l)+mBuffer[l]);
	}
}
}}
