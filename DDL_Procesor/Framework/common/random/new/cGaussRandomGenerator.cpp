#include "cGaussRandomGenerator.h"

namespace common {
  namespace random_new {

cGaussRandomGenerator::~cGaussRandomGenerator()
{
	if (mBuffer != NULL) {
		delete mBuffer;
	}
}

cGaussRandomGenerator::cGaussRandomGenerator(const bool Randomize,double sigma2):
	cAbstractRandomGenerator(), m_UnifRandom(Randomize), iset(0), mBuffer(NULL), mBufferCount(0)
{
	this->sigma2=sigma2;
	this->sigma=sqrt(sigma2);
	this->maxnum=sigma2;
}
double cGaussRandomGenerator::GetInvNormFun(double y)
{
	double pi=4.0*atan(1.0);
	double k1=1/(sqrt(2*pi*sigma2));
	double k2=-1/(2*sigma2);

	double topOfInterval=k1*exp(0.0);
	//deleno 2,protoze beru jen kladnou pulku intervalu
	double inInterval=topOfInterval/2-(topOfInterval/(100*sigma2))*(y/2);
	double inInterval0=topOfInterval/2;
	//top pro posun pulky intervalu k 0
	double topOfInInterval=sqrt(log(inInterval0/k1)/k2)*2;
	double x=sqrt(log(inInterval/k1)/k2)*2-topOfInInterval;
	return x-0.5;
}
double cGaussRandomGenerator::GetNext32()
{	
	double ret;
	do {
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
		unsigned long n= (unsigned long)(n0*256*256*256+n1*256*256+n2*256+n3);
	//	n=(n%((unsigned long)maxnum));
		double y=(double)n;
		if (maxnum!=UINT_MAX) {
			do {
				y=rand();
			} while (y> maxnum*100);
			
		}

		ret=GetInvNormFun(y);
	}while (ret<0); 
	return ret;
}

double cGaussRandomGenerator::GetNext()
{
	double fac, rsq, v1, v2;
	if (iset == 0)
	{
		do
		{
			v1 = 2.0*m_UnifRandom.GetNext() - 1.0;
			v2 = 2.0*m_UnifRandom.GetNext() - 1.0;
			rsq = v1*v1 + v2*v2;
		} while (rsq >= 1.0 || rsq == 0.0);
		fac = sqrt(-2.0*log(rsq)/rsq);
		gset = v1*fac;
		iset = 1;
		return v2*fac;
	}
	iset = 0;
	return gset;
}

unsigned long long cGaussRandomGenerator::RandRange64(unsigned long long min,unsigned long long max) {
		unsigned long long p0=rand()%256;
		unsigned long long p1=rand()%256;
		unsigned long long p2=rand()%256;
		unsigned long long p3=rand()%256;
		unsigned long long p4=rand()%256;
		unsigned long long p5=rand()%256;
		unsigned long long p6=rand()%256;
		unsigned long long p7=rand()%256;
		unsigned long long n=(p7*256*256*256*256*256*256*256)
							+(p6*256*256*256*256*256*256)
							+(p5*256*256*256*256*256)
							+(p4*256*256*256*256)
							+(p3*256*256*256)
							+(p2*256*256)
							+(p1*256)
							+p0;
		n=n%(max-min+1)+min;
	return n;
}

unsigned int cGaussRandomGenerator::RandRange(unsigned int min,unsigned int max) {
		unsigned int n0,n1,n2,n3;
		n0=(unsigned int)rand()%256;
		n1=(unsigned int)rand()%256;
		n2=(unsigned int)rand()%256;
		n3=(unsigned int)rand()%256;
		unsigned int n = (unsigned int)(n0*256*256*256+n1*256*256+n2*256+n3);
		n=n%(max-min+1)+min;
		return n;
}

double cGaussRandomGenerator::Func(double x) {
	return 2*(1/sqrt(2*M_PI*sigma2))*exp(-(x*x)/(2*sigma2));
}

void cGaussRandomGenerator::InitBuffer(int count) {
  int i=0;
  double lowerBound=0;
  double upperBound = sigma2;
  double psum=0;

	if (mBufferCount != count)
	{
		if (mBuffer != NULL)
		{
			delete mBuffer;
		}
		mBuffer= new unsigned int[count];
	}
  mBufferCount=count;	

  while (i<count) {
		//minimalni krok
		double unit=(upperBound-lowerBound)/100;
		if (unit<1) unit=1;
		//zjisti x
		double x=(upperBound-unit);
		if (x<lowerBound){
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
			for (unsigned int z = 0 ; (z<expectedCount && i<count) ; z++) {
				mBuffer[i++]=RandRange((unsigned int)x, (unsigned int)(upperBound-1));
			}
		}
		upperBound=x;
	}
}

unsigned int cGaussRandomGenerator::GetNext32buffer(unsigned int max)
{
	  
	unsigned int randN=RandRange(0,UINT_MAX-1);
	double x=((double)mBufferCount/(double)max)*(double)randN;
	unsigned int l= (unsigned int)x;
	unsigned int u= (unsigned int)(x+1);
	if (mBuffer[l]>mBuffer[u]) {
		return (unsigned int)(fabs((double)mBuffer[l]-(double)mBuffer[u])*(x-l)+mBuffer[u]);
	}
	else {
		return (unsigned int)(fabs((double)mBuffer[l]-(double)mBuffer[u])*(x-l)+mBuffer[l]);
	}
}

unsigned long long cGaussRandomGenerator::GetNext64buffer()
{
	  
	unsigned long long randN = RandRange64(0,ULLONG_MAX-1);
	double x = ((double)mBufferCount/(double)ULLONG_MAX)*(double)randN;
	unsigned long long l = (unsigned long long)x;
	unsigned long long u = (unsigned long long)(x+1);
	if (mBuffer[l] > mBuffer[u]) {
		return (unsigned long long)(fabs((double)mBuffer[l]-(double)mBuffer[u])*(x-l)+mBuffer[u]);
	}
	else {
		return (unsigned long long)(fabs((double)mBuffer[l]-(double)mBuffer[u])*(x-l)+mBuffer[l]);
	}
}
}}
  

