/**************************************************************************}
{                                                                          }
{    cImage.h                                                              }
{      Image                                                               }
{                                                                          }
{                                                                          }
{    Copyright (c) 2006                      Michal Kratky                 }
{                                                                          }
{    VERSION: 0.2                            DATE 21/11/2006               }
{                                                                          }
{    following functionality:                                              }
{       image                                                              }
{                                                                          }
{                                                                          }
{    UPDATE HISTORY:                                                       }
{      //                                                                  }
{                                                                          }
{**************************************************************************/

#ifndef __cImage_h__
#define __cImage_h__

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

class cImage
{
private:
	unsigned int mWidth;
	unsigned int mHeight;
	unsigned int mSize;
	unsigned int *mImage;

public:
	static const unsigned char COLOR_MIN = 0;
	static const unsigned char COLOR_MAX = 255;
	static const unsigned int COLOR_MAX_UINT = ~0;
	static const unsigned int BYTES_PER_PIXEL = 3;

private:
	void RepaireRange(unsigned int ql1, unsigned int ql2, unsigned int qh1, unsigned int qh2, unsigned int &h1, unsigned int &h2, unsigned int &length) const;

public:
	cImage();
	cImage(unsigned int width, unsigned int height);
	~cImage();

	unsigned int Order(unsigned int x, unsigned int y) const;
	void Resize(unsigned int width, unsigned int height);
	void SetImage(unsigned int *mem, unsigned int width, unsigned int height);
	void SetImageAsNull();

	inline void SetValue(unsigned int x, unsigned int y, unsigned int value);

	inline unsigned int GetValue(unsigned int x, unsigned int y) const;

	inline unsigned int GetWidth() const;
	inline unsigned int GetHeight() const;
	inline unsigned int GetSize() const;

	void SetRange(unsigned int *mem, unsigned int size, unsigned int ql1, unsigned int ql2, unsigned int qh1, unsigned int qh2);
	void GetRange(unsigned int *mem, unsigned int size, unsigned int sizeX, unsigned int sizeY, unsigned int mPosInBlock1, unsigned int mPosInBlock2,
		unsigned int ql1, unsigned int ql2, unsigned int qh1, unsigned int qh2) const;

	void ClearRange(unsigned int ql1, unsigned int ql2, unsigned int qh1, unsigned int qh2);

	inline unsigned int* GetData();
	void SetData(unsigned int *data, unsigned int size);

	static unsigned int GetIntColor(unsigned char r, unsigned char g, unsigned char b);
	static void GetColor(unsigned int color, unsigned char &r, unsigned char &g, unsigned char &b);
};

inline void cImage::SetValue(unsigned int x, unsigned int y, unsigned int value)
{ 
	assert(x < mWidth && y < mHeight);
	mImage[Order(x,y)] = value; 
}

inline unsigned int cImage::GetValue(unsigned int x, unsigned int y) const
{	
	assert(x < mWidth && y < mHeight);
	return mImage[Order(x,y)];
}

inline unsigned int cImage::GetWidth() const
{
	return mWidth;
}

inline unsigned int cImage::GetHeight() const
{
	return mHeight;
}

inline unsigned int cImage::GetSize() const
{
	return mSize;
}

inline unsigned int cImage::Order(unsigned int x, unsigned int y) const
{
	return (y * mWidth) + x;
}

inline unsigned int* cImage::GetData()
{
	return mImage;
}
#endif