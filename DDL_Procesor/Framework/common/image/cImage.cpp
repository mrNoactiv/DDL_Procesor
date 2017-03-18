#include "cImage.h"

cImage::cImage(): mImage(NULL), mWidth(0), mHeight (0), mSize(0)
{
}

cImage::cImage(unsigned int width, unsigned int height) 
{
	mImage = new unsigned int[width * height];

	mWidth = width;
	mHeight = height;
	mSize = mWidth * mHeight;
}

cImage::~cImage() 
{
	if (mImage != NULL)
	{
		delete []mImage;
	}
}

/// Resize the image if this is smaller then requirement.
void cImage::SetImage(unsigned int *mem, unsigned int width, unsigned int height)
{
	mImage = mem;
	mWidth = width;
	mHeight = height;
}

void cImage::SetImageAsNull()
{
	mImage = NULL;
	mWidth = 0;
	mHeight = 0;
}


/// Resize the image if this is smaller then requirement.
void cImage::Resize(unsigned int width, unsigned int height)
{
	if (mSize < (width * height))
	{
		if (mImage != NULL)
		{
			delete []mImage;
		}

		mImage = new unsigned int[width * height];
	}
	mWidth = width;
	mHeight = height;
	mSize = mWidth * mHeight;
}

/// Convert RGB into the single unsigned int.
unsigned int cImage::GetIntColor(unsigned char r, unsigned char g, unsigned char b)
{
	unsigned int color = (unsigned int)r | (unsigned int)g << 8 | (unsigned int)b << 16;
	return color;
}

/// Convert unsigned int into RGB.
void cImage::GetColor(unsigned int color, unsigned char &r, unsigned char &g, unsigned char &b)
{
	r = color & 0x000000FF;
	g = (color & 0x0000FF00) >> 8;
	b = (color & 0x00FF0000) >> 16;
}

void cImage::GetRange(unsigned int *mem, unsigned int size, unsigned int sizeX, unsigned int sizeY, 
					  unsigned int mPosInBlock1, unsigned int mPosInBlock2,
					  unsigned int ql1, unsigned int ql2, unsigned int qh1, unsigned int qh2) const
{
	bool mDebug = false;
	unsigned int h2, h1, length;
	RepaireRange(ql1, ql2, qh1, qh2, h1, h2, length);

	unsigned int diff = mPosInBlock1; // sizeX - length;   // drift of the image
	for (unsigned int i = ql2 ; i <= h2 ; i++)
	{
		unsigned int order = Order(ql1,i);
		if (((diff + length) > size) || ((order + length) > mSize))
		{
			int bla = 0;
		}
		memcpy(mem + diff, mImage + order, length * sizeof(unsigned int));  // copy raster row by row
		diff += sizeX;
	}

	if (mDebug)
	{
		for (unsigned int i = 0 ; i < size ; i++)
		{
			printf("%x, ", mem[i]);
		}
		printf("\n");
	}
}

/// Set range
void cImage::SetRange(unsigned int *mem, unsigned int size, unsigned int ql1, unsigned int ql2, unsigned int qh1, unsigned int qh2)
{
	unsigned int h2, h1, length;
	RepaireRange(ql1, ql2, qh1, qh2, h1, h2, length);

	unsigned int diff = 0;
	for (unsigned int i = ql2 ; i <= h2 ; i++)
	{
		memcpy(mImage + Order(ql1,i), mem + diff, length * sizeof(unsigned int));  // copy raster row by row
		diff += length;
	}
}

/// Clear range.
void cImage::ClearRange(unsigned int ql1, unsigned int ql2, unsigned int qh1, unsigned int qh2)
{
	unsigned int h2, h1, length;
	RepaireRange(ql1, ql2, qh1, qh2, h1, h2, length);

	unsigned int diff = 0;
	for (unsigned int i = ql2 ; i <= h2 ; i++)
	{
		memset(mImage + Order(ql1,i), 0, length * sizeof(unsigned int));  // clear the row by row
		diff += length;
	}
}

/// Repaire
void cImage::RepaireRange(unsigned int ql1, unsigned int ql2, unsigned int qh1, unsigned int qh2, unsigned int &h1, unsigned int &h2, unsigned int &length) const
{
	h2 = (qh2 < mHeight) ? qh2 : mHeight-1;
	h1 = (qh1 < mWidth) ? qh1 : mWidth-1;
	length = h1 - ql1 + 1;
}

/// Set data.
void cImage::SetData(unsigned int *data, unsigned int size)
{
	assert(size == mSize);
	memcpy((char*)mImage, (char*)data, mSize * sizeof(unsigned int));
}