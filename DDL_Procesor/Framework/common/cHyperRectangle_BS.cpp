#include "cHyperRectangle_BS.h"

cHyperRectangle_BS::cHyperRectangle_BS(cSpaceDescriptor_BS* spaceDescr)
{
	Resize(spaceDescr);
}

void cHyperRectangle_BS::Resize(cSpaceDescriptor_BS* spaceDescr)
{
	mStartTuple.Resize(spaceDescr);
	mEndTuple.Resize(spaceDescr);
}

cHyperRectangle_BS::~cHyperRectangle_BS()
{
}

/**
 * Return true if the hyperrectangle q1 is intersected by q2.
 */
bool cHyperRectangle_BS::IsIntersected(const cTuple_BS &ql1, const cTuple_BS &qh1, const cTuple_BS &ql2, const cTuple_BS &qh2)
{
	bool ret = true;
	// hyperectangle are intersected, if intervals in all dimensions are intersected
	for (unsigned int i = 0 ; i < ql1.GetSpaceDescriptor()->GetDimension() ; i++)
	{
		if (ql1.GetRefValue(i) <= qh1.GetRefValue(i))
		{
			if (ql2.GetRefValue(i) <= qh2.GetRefValue(i))
			{
				if (ql2.GetRefValue(i) > qh1.GetRefValue(i) || qh2.GetRefValue(i) <  ql1.GetRefValue(i))
				{
					ret = false;
					break;
				}
			}
			else
			{
				printf("Critical Error: cHyperRectangle::IsIntersected(): ql2 > qh2!");
				exit(1);
			}
		}
		else
		{
			printf("Critical Error: cHyperRectangle::IsIntersected(): ql1 > qh1!");
			exit(1);
		}
	}
	return ret;
}

/**
 * Return sum of intersection intervals in dimension.
 */
double cHyperRectangle_BS::IntersectionVolume(const cTuple_BS &ql1, const cTuple_BS &qh1, const cTuple_BS &ql2, const cTuple_BS &qh2)
{
	double intersectionVolume = 1.0;
	unsigned int interval;

	// hyperectangle are intersected, if intervals in all dimensions are intersected
	for (unsigned int i = 0 ; i < ql1.GetSpaceDescriptor()->GetDimension() ; i++)
	{
		if (ql1.GetRefValue(i) <= qh1.GetRefValue(i))
		{
			if (ql2.GetRefValue(i) <= qh2.GetRefValue(i))
			{
				if (ql2.GetRefValue(i) <= qh1.GetRefValue(i) && ql2.GetRefValue(i) >= ql1.GetRefValue(i))
				{
					if (qh2.GetRefValue(i) <= qh1.GetRefValue(i))
					{
						interval = qh2.GetRefValue(i).GetInt() - ql2.GetRefValue(i).GetInt();
					}
					else
					{
						interval = qh1.GetRefValue(i).GetInt() - ql2.GetRefValue(i).GetInt();
					}
				}
				else if (ql1.GetRefValue(i) >= ql2.GetRefValue(i) && ql1.GetRefValue(i) <= qh2.GetRefValue(i))
				{
					if (qh1.GetRefValue(i) <= qh2.GetRefValue(i))
					{
						interval = qh2.GetRefValue(i).GetInt() - ql1.GetRefValue(i).GetInt();
					}
					else
					{
						interval = qh2.GetRefValue(i).GetInt() - ql1.GetRefValue(i).GetInt();
					}
				}
				else  // MBRs aren't intersect
				{
					intersectionVolume = 0.0;
					break;
				}

				intersectionVolume *= ((double)(interval + 1) / ql1.GetSpaceDescriptor()->GetMaxValue(i));
			}
			else
			{
				printf("Critical Error: cHyperRectangle::IsIntersected(): ql2 > qh2!");
				exit(1);
			}
		}
		else
		{
			printf("Critical Error: cHyperRectangle::IsIntersected(): ql1 > qh1!");
			exit(1);
		}
	}
	return intersectionVolume;
}

double cHyperRectangle_BS::myIntersectionVolume(const cTuple_BS &ql1, const cTuple_BS &qh1, const cTuple_BS &ql2, const cTuple_BS &qh2, const cTuple_BS &tmp_interval)
{
	double intersectionVolume = 1.0;
	//unsigned int interval;

	// hyperectangle are intersected, if intervals in all dimensions are intersected
	for (unsigned int i = 0 ; i < ql1.GetSpaceDescriptor()->GetDimension() ; i++)
	{
		if (ql1.GetRefValue(i) <= qh1.GetRefValue(i))
		{
			if (ql2.GetRefValue(i) <= qh2.GetRefValue(i))
			{
				if (ql2.GetRefValue(i) <= qh1.GetRefValue(i) && ql2.GetRefValue(i) >= ql1.GetRefValue(i))
				{

						tmp_interval.GetRefValue(i).SubAbs(qh1.GetRefValue(i), ql2.GetRefValue(i));

				}
				else if (ql1.GetRefValue(i) >= ql2.GetRefValue(i) && ql1.GetRefValue(i) <= qh2.GetRefValue(i))
				{
					tmp_interval.GetRefValue(i).SubAbs(qh2.GetRefValue(i), ql1.GetRefValue(i));
				}
				else  // MBRs aren't intersect
				{
					intersectionVolume = 0;
					break;
				}

				//intersectionVolume *= ((double)(interval + 1) / ql1.GetSpaceDescriptor()->GetMaxValue(i));
				unsigned int  v1 = tmp_interval.GetRefValue(i).FindFirstSetBit();
				unsigned int  v2 = ql1.GetSpaceDescriptor()->GetBitSize(i) - 1;
				intersectionVolume *= ((double)v1 / v2);
				//POZN: pozice bitu je pocet cifer-1
			}
			else
			{
				printf("Critical Error: cHyperRectangle::IsIntersected(): ql2 > qh2!");
				exit(1);
			}
		}
		else
		{
			printf("Critical Error: cHyperRectangle::IsIntersected(): ql1 > qh1!");
			exit(1);
		}
	}
	return intersectionVolume;
}

unsigned int cHyperRectangle_BS::approximateIntersectionVolume(const cTuple_BS &ql1, const cTuple_BS &qh1, const cTuple_BS &ql2, const cTuple_BS &qh2, const cTuple_BS &tmp_interval)
{
	unsigned int intersectionVolume = 0;
	//unsigned int interval;

	// hyperectangle are intersected, if intervals in all dimensions are intersected
	for (unsigned int i = 0 ; i < ql1.GetSpaceDescriptor()->GetDimension() ; i++)
	{
		if (ql1.GetRefValue(i) <= qh1.GetRefValue(i))
		{
			if (ql2.GetRefValue(i) <= qh2.GetRefValue(i))
			{
				if (ql2.GetRefValue(i) <= qh1.GetRefValue(i) && ql2.GetRefValue(i) >= ql1.GetRefValue(i))
				{

						tmp_interval.GetRefValue(i).SubAbs(qh1.GetRefValue(i), ql2.GetRefValue(i));

				}
				else if (ql1.GetRefValue(i) >= ql2.GetRefValue(i) && ql1.GetRefValue(i) <= qh2.GetRefValue(i))
				{
					tmp_interval.GetRefValue(i).SubAbs(qh2.GetRefValue(i), ql1.GetRefValue(i));
				}
				else  // MBRs aren't intersect
				{
					intersectionVolume = 0;
					break;
				}

				//intersectionVolume *= ((double)(interval + 1) / ql1.GetSpaceDescriptor()->GetMaxValue(i));
			    intersectionVolume += tmp_interval.GetRefValue(i).FindFirstSetBit();
				//POZN: pozice bitu je pocet cifer-1
			}
			else
			{
				printf("Critical Error: cHyperRectangle::IsIntersected(): ql2 > qh2!");
				exit(1);
			}
		}
		else
		{
			printf("Critical Error: cHyperRectangle::IsIntersected(): ql1 > qh1!");
			exit(1);
		}
	}
	return intersectionVolume;
}

bool cHyperRectangle_BS::exactIntersectionVolume(const cTuple_BS &ql1, const cTuple_BS &qh1, const cTuple_BS &ql2, const cTuple_BS &qh2, const cTuple_BS &tmp_interval, cBitString &intersectionVolume, cBitString &tmpMul)
{
	bool over = true; //?? co vraci cBitSring::UMul
	intersectionVolume.Clear();
	intersectionVolume.SetInt(1); //init. for gradual mul. 

	// hyperectangle are intersected, if intervals in all dimensions are intersected
	for (unsigned int i = 0 ; i < ql1.GetSpaceDescriptor()->GetDimension() ; i++)
	{
		if (ql1.GetRefValue(i) <= qh1.GetRefValue(i))
		{
			if (ql2.GetRefValue(i) <= qh2.GetRefValue(i))
			{
				if (ql2.GetRefValue(i) <= qh1.GetRefValue(i) && ql2.GetRefValue(i) >= ql1.GetRefValue(i))
				{
						tmp_interval.GetRefValue(i).SubAbs(qh1.GetRefValue(i), ql2.GetRefValue(i));
				}
				else if (ql1.GetRefValue(i) >= ql2.GetRefValue(i) && ql1.GetRefValue(i) <= qh2.GetRefValue(i))
				{
					tmp_interval.GetRefValue(i).SubAbs(qh2.GetRefValue(i), ql1.GetRefValue(i));
				}
				else  // MBRs aren't intersect
				{
					intersectionVolume = 0;
					break;
				}

				//intersectionVolume *= ((double)(interval + 1) / ql1.GetSpaceDescriptor()->GetMaxValue(i));
				/*unsigned int v1 = tmp_interval.GetRefValue(i).FindFirstSetBit();
				unsigned int v2 = ql1.GetSpaceDescriptor()->GetBitSize(i) - 1;
				intersectionVolume *= ((double)v1 / v2);*/
                over &= intersectionVolume.UMul(intersectionVolume, tmp_interval.GetRefValue(i), tmpMul);
				//POZN: pozice bitu je pocet cifer-1
			}
			else
			{
				printf("Critical Error: cHyperRectangle::IsIntersected(): ql2 > qh2!");
				exit(1);
			}
		}
		else
		{
			printf("Critical Error: cHyperRectangle::IsIntersected(): ql1 > qh1!");
			exit(1);
		}
	}
	return over;
}

/**
 * Return if hyperblock2 is contained in hyperblock1.
 */
bool cHyperRectangle_BS::IsContained(const cTuple_BS &hrl1, const cTuple_BS &hrh1, const cTuple_BS &hrl2, const cTuple_BS &hrh2)
{
	char *error = "Critical Error: cHyperRectangle::IsIntersected(): ql2 > qh2!";
	bool ret = true;

	// hyperectangle are intersected, if intervals in all dimensions are intersected
	for (unsigned int i = 0 ; i < hrl1.GetSpaceDescriptor()->GetDimension() ; i++)
	{
		if (hrl1.GetRefValue(i) <= hrh1.GetRefValue(i))
		{
			if (hrl2.GetRefValue(i) <= hrh2.GetRefValue(i))
			{
				if (hrl2.GetRefValue(i) >= hrl1.GetRefValue(i) & hrl2.GetRefValue(i) <= hrh1.GetRefValue(i) &
					hrh2.GetRefValue(i) >= hrl1.GetRefValue(i) & hrh2.GetRefValue(i) <= hrh1.GetRefValue(i))
				{
					continue;
				}
				else
				{
					ret = false;
					break;
				}
			}
			else
			{
				printf(error);
				exit(1);
			}
		}
		else
		{
			printf(error);
			exit(1);
		}
	}
	return ret;
}

/**
 * From the two points, prepare hyperblock, i.e. ql_i \leq qh_i.
 */
void cHyperRectangle_BS::PrepareHyperblock(const cTuple_BS &t1, const cTuple_BS &t2, cTuple_BS &ql, cTuple_BS &qh)
{
	for (unsigned int i = 0 ; i < t1.GetSpaceDescriptor()->GetDimension() ; i++)
	{
		if (t1.GetRefValue(i) <= t2.GetRefValue(i))
		{
			ql.SetValue(i, t1.GetRefValue(i));
			qh.SetValue(i, t2.GetRefValue(i));
		}
		else
		{
			ql.SetValue(i, t2.GetRefValue(i));
			qh.SetValue(i, t1.GetRefValue(i));
		}
	}
}

void cHyperRectangle_BS::Subquadrant(cBitString& hqCode, cBitString& tmpString)
{
	// tmpString.Resize(mSpaceDescriptor->GetMaxBitSize()+1); // 1 extra bit for doubling + additional incrementing

	for(unsigned int i=0; i < mStartTuple.GetSpaceDescriptor()->GetDimension() ; i++)
	{
		tmpString.Clear();	// *tos bugfix* 14.7.03

		if (hqCode.GetBit(i))			// reducing interval to the second half -> (a+b+1)/2	where $a$ is start-coord and b is end-coord
		{
			tmpString.Add(*mStartTuple.GetValue(i));
			tmpString.Add(*mEndTuple.GetValue(i));
			tmpString.Increment();
			tmpString.ShiftRight();
			mStartTuple.GetValue(i)->Clear();
			mStartTuple.GetValue(i)->Add(tmpString);
		}
		else												// reducing interval to the first half -> (a+b-1)/2	where $a$ is start-coord and b is end-coord
		{
			tmpString.Add(*mEndTuple.GetValue(i));
			tmpString.Add(*mStartTuple.GetValue(i));
			tmpString.Decrement();
			tmpString.ShiftRight();
			mEndTuple.GetValue(i)->Clear();
			mEndTuple.GetValue(i)->Add(tmpString);
		}
	}
}

/**
 * Compute volume of hyperrectangle
 */
double cHyperRectangle_BS::Volume(const cTuple_BS &hrl, const cTuple_BS &hrh)
{
	double volume = 1.0;
	for (unsigned int i = 0 ; i < hrl.GetSpaceDescriptor()->GetDimension() ; i++)
	{
		volume *= ((double)(hrh.GetRefValue(i).GetInt() - hrl.GetRefValue(i).GetInt())) / hrl.GetSpaceDescriptor()->GetMaxValue(i);
	}
	return volume;
}

/**
 * Compute volume of hyperrectangle
 */
bool cHyperRectangle_BS::myVolume(const cTuple_BS &hrl, const cTuple_BS &hrh, cTuple_BS &tmpTuple, cBitString &volume, cBitString &tmp)
{
	volume.Clear();
	volume.SetInt(1);
	bool test = true;
	for (unsigned int i = 0 ; i < hrl.GetSpaceDescriptor()->GetDimension() ; i++)
	{
		tmpTuple.GetRefValue(i).Sub(hrh.GetRefValue(i), hrl.GetRefValue(i)); // tmpTuple = hrh - hrl
		test &= volume.UMul(volume ,tmpTuple.GetRefValue(i), tmp); // counting number of ciphers tmpTuple (pozice je pocet cifer-1)
		if (!test) return false;
		//volume.Print(0,"\n");
	}
	return true;
}

unsigned int cHyperRectangle_BS::approximateVolume(const cTuple_BS &hrl, const cTuple_BS &hrh, const cTuple_BS &tmpTuple)
{
	UNREFERENCED_PARAMETER(hrl);
	UNREFERENCED_PARAMETER(hrh);
	unsigned int volume = 0; //only number of ciphers

	for (unsigned int i = 0 ; i < hrl.GetSpaceDescriptor()->GetDimension() ; i++)
	{
		// bool test = tmpTuple.GetRefValue(i).Sub(hrh.GetRefValue(i), hrl.GetRefValue(i)); // tmpTuple = hrh - hrl
		//hrh.Print(0,"\n");
		//hrl.Print(0,"\n");
		//tmpTuple.Print(0,"\n");
		volume += tmpTuple.GetRefValue(i).FindFirstSetBit(); // counting number of ciphers tmpTuple (pozice je pocet cifer-1)
	}
	return volume;
}

void cHyperRectangle_BS::Print(int mode, char *str)
{
	mStartTuple.Print(mode, "");
	mEndTuple.Print(mode, str);
}