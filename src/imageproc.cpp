#include "stdafx.h"
#include "imageproc.h"

void Invert(Bitmap *pBitmap)
{
	if (!pBitmap) return;

	int width = pBitmap->GetWidth();
	int height = pBitmap->GetHeight();

	BitmapData bitmapData;
	pBitmap->LockBits(&Rect(0, 0, width, height), ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData);

	//bitmapData now contains all the locked data. We must now copy the data before we start the convolution.
	unsigned int *pRawBitmapOrig = (unsigned int*)bitmapData.Scan0;	// for easy access and indexing

																	//unsigned int *pRawBitmapCopy = new unsigned int[bitmapData.Stride*bitmapData.Height];

																	//memcpy(pRawBitmapCopy, bitmapData.Scan0, bitmapData.Stride*bitmapData.Height);

	int nPixels = height*bitmapData.Stride / 4;
	for (int i = 0; i < nPixels; i++)
	{
		unsigned int curColor = pRawBitmapOrig[i];

		// NOTE: RGB Alpha

		int a = (curColor & 0xff000000) >> 24;
		int r = (curColor & 0xff0000) >> 16;
		int g = (curColor & 0xff00) >> 8;
		int b = curColor & 0xff;
		r = (a - r);
		g = (a - g);
		b = (a - b);

		unsigned int finalColor = r + (g << 8) + (b << 16) + (a << 24);
		pRawBitmapOrig[i] = finalColor;
	}
	pBitmap->UnlockBits(&bitmapData);
}

void GrayScale(Bitmap *pBitmap)
{
	//Gray scale with luminosity algorithm: 0.21 R + 0.72 G + 0.07 B.
	if (!pBitmap) return;

	int width = pBitmap->GetWidth();
	int height = pBitmap->GetHeight();

	BitmapData bitmapData;
	pBitmap->LockBits(&Rect(0, 0, width, height), ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData);

	//bitmapData now contains all the locked data. We must now copy the data before we start the convolution.
	unsigned int *pRawBitmapOrig = (unsigned int*)bitmapData.Scan0;	// for easy access and indexing

																	//unsigned int *pRawBitmapCopy = new unsigned int[bitmapData.Stride*bitmapData.Height];

																	//memcpy(pRawBitmapCopy, bitmapData.Scan0, bitmapData.Stride*bitmapData.Height);

	int nPixels = height*bitmapData.Stride / 4;
	for (int i = 0; i < nPixels; i++)
	{
		unsigned int curColor = pRawBitmapOrig[i];

		// NOTE: RGB Alpha

		int a = (curColor & 0xff000000) >> 24;
		int r = (curColor & 0xff0000) >> 16;
		int g = (curColor & 0xff00) >> 8;
		int b = curColor & 0xff;
		int gray = 0.2125*r + 0.7154*g + 0.0721*b;

		unsigned int finalColor = gray + (gray << 8) + (gray << 16) + (a << 24);
		pRawBitmapOrig[i] = finalColor;
	}
	pBitmap->UnlockBits(&bitmapData);
}

void CChannel(Bitmap *pBitmap, int c)
{
	if (!pBitmap) return;

	int width = pBitmap->GetWidth();
	int height = pBitmap->GetHeight();

	BitmapData bitmapData;
	pBitmap->LockBits(&Rect(0, 0, width, height), ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData);

	//bitmapData now contains all the locked data. We must now copy the data before we start the convolution.
	unsigned int *pRawBitmapOrig = (unsigned int*)bitmapData.Scan0;	// for easy access and indexing
	//unsigned int *pRawBitmapCopy = new unsigned int[bitmapData.Stride*bitmapData.Height];
	//memcpy(pRawBitmapCopy, bitmapData.Scan0, bitmapData.Stride*bitmapData.Height);

	int nPixels = height*bitmapData.Stride / 4;
	for (int i = 0; i < nPixels; i++)
	{
		if (c == 1)
			pRawBitmapOrig[i] &= 0xffff0000;
		else if (c == 2)
			pRawBitmapOrig[i] &= 0xff00ff00;
		else if (c == 3)
			pRawBitmapOrig[i] &= 0xff0000ff;
	}
	pBitmap->UnlockBits(&bitmapData);
}

Bitmap* Rotate(Bitmap *pBitmap, int c)
{
	if (!pBitmap) return pBitmap;

	int width = pBitmap->GetWidth();
	int height = pBitmap->GetHeight();

	BitmapData bitmapData;
	pBitmap->LockBits(&Rect(0, 0, width, height), ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData);

	//bitmapData now contains all the locked data. We must now copy the data before we start the convolution.
	unsigned int *pRawBitmapOrig = (unsigned int*)bitmapData.Scan0;	// for easy access and indexing

	//unsigned int *pRawBitmapCopy = new unsigned int[bitmapData.Stride*bitmapData.Height];

	//memcpy(pRawBitmapCopy, bitmapData.Scan0, bitmapData.Stride*bitmapData.Height);

	int MX = max(height, width);
	Bitmap *pBitmapNew = new Bitmap(height, width, PixelFormat32bppARGB);
	BitmapData bitmapDataNew;
	pBitmapNew->LockBits(&Rect(0, 0, height, width), ImageLockModeWrite, PixelFormat32bppARGB, &bitmapDataNew);
	unsigned int *pRawBitmapNew = (unsigned int*)bitmapDataNew.Scan0;

	Bitmap *pBitmapNew2 = new Bitmap(height, width, PixelFormat32bppARGB);
	BitmapData bitmapDataNew2;
	pBitmapNew2->LockBits(&Rect(0, 0, height, width), ImageLockModeWrite, PixelFormat32bppARGB, &bitmapDataNew2);
	unsigned int *pRawBitmapNew2 = (unsigned int*)bitmapDataNew2.Scan0;

	int nPixels = height*bitmapData.Stride / 4;
	//int j = 0;
	/*for (int i = 0; i < nPixels; i++)
	{
		//pRawBitmapNew[nPixels - i - 1] = pRawBitmapOrig[i];
	}*/

	unsigned int tmp;

	for (int i = 0; i < MX; i++)
	{
		for (int j = 0; j < MX; j++)
		{
			int indexOrig = i * width + j;
			int indexNew = j * height + i;
			if (indexOrig < nPixels && indexNew < nPixels)
			{
				if (tmp = pRawBitmapOrig[indexOrig])
					pRawBitmapNew[indexNew] = tmp;
			}
		}
	}

	if (c == 1)
	{
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				int indexNew = i * height + j;
				int indexNew2 = i * height + height - j - 1;
				if (indexNew < nPixels && indexNew2 < nPixels)
				{
					if (tmp = pRawBitmapNew[indexNew])
						pRawBitmapNew2[indexNew2] = tmp;
				}
			}
		}
	}
	else if (c == 2)
	{
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				int indexNew = i * height + j;
				int indexNew2 = (width - i - 1) * height + j;
				if (indexNew < nPixels && indexNew2 < nPixels)
				{
					if (tmp = pRawBitmapNew[indexNew])
						pRawBitmapNew2[indexNew2] = tmp;
				}
			}
		}
	}
	//bitmapData = bitmapDataNew;
	//memcpy(pRawBitmapOrig, bitmapDataNew.Scan0, bitmapData.Stride*bitmapData.Height);
	//memcpy(pRawBitmapOrig, pRawBitmapNew, bitmapData.Stride*bitmapData.Height);
	//bitmapData = bitmapDataNew;
	pBitmap->UnlockBits(&bitmapData);
	pBitmapNew->UnlockBits(&bitmapDataNew);
	pBitmapNew2->UnlockBits(&bitmapDataNew);
	//delete pBitmap;
	//pRawBitmapOrig = NULL;
	return pBitmapNew2;
	//pBitmap->SetWidth(height);
	//pBitmap->SetHeight(width);
	//pBitmapNew->UnlockBits(&bitmapDataNew);
}

void Mirror(Bitmap *pBitmap, int c)
{
	if (!pBitmap) return;

	int width = pBitmap->GetWidth();
	int height = pBitmap->GetHeight();

	BitmapData bitmapData;
	pBitmap->LockBits(&Rect(0, 0, width, height), ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData);

	//bitmapData now contains all the locked data. We must now copy the data before we start the convolution.
	unsigned int *pRawBitmapOrig = (unsigned int*)bitmapData.Scan0;	// for easy access and indexing
	unsigned int *pRawBitmapCopy = new unsigned int[bitmapData.Stride*bitmapData.Height];
	memcpy(pRawBitmapCopy, bitmapData.Scan0, bitmapData.Stride*bitmapData.Height);

	/*Bitmap *pBitmapNew = new Bitmap(width, height, PixelFormat32bppARGB);
	BitmapData bitmapDataNew;
	pBitmapNew->LockBits(&Rect(0, 0, width, height), ImageLockModeWrite, PixelFormat32bppARGB, &bitmapDataNew);
	unsigned int *pRawBitmapNew = (unsigned int*)bitmapDataNew.Scan0;*/

	int nPixels = height*bitmapData.Stride / 4;
	unsigned int tmp;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int index = i * width + j;
			int indexNew = i * width + width - j - 1;
			if (index < nPixels && indexNew < nPixels)
			{
				if (tmp = pRawBitmapCopy[index])
					pRawBitmapOrig[indexNew] = tmp;
			}
		}
	}

	pBitmap->UnlockBits(&bitmapData);
	//pBitmapNew->UnlockBits(&bitmapDataNew);
	//return pBitmapNew;
}

Bitmap* Mirror2(Bitmap *pBitmap, int c)
{
	if (!pBitmap) return pBitmap;

	int width = pBitmap->GetWidth();
	int height = pBitmap->GetHeight();

	Bitmap *tmp = pBitmap->Clone(0, 0, width, height, PixelFormat32bppARGB);
	Mirror(tmp, 1);

	BitmapData bitmapData;
	pBitmap->LockBits(&Rect(0, 0, width, height), ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData);
	unsigned int *pRawBitmapOrig = (unsigned int*)bitmapData.Scan0;	// for easy access and indexing

	BitmapData bitmapData2;
	tmp->LockBits(&Rect(0, 0, width, height), ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData2);
	unsigned int *pRawBitmapCopy = (unsigned int*)bitmapData2.Scan0;	// for easy access and indexing

	Bitmap *pBitmapNew = new Bitmap(width * 2, height, PixelFormat32bppARGB);
	BitmapData bitmapDataNew;
	pBitmapNew->LockBits(&Rect(0, 0, width * 2, height), ImageLockModeWrite, PixelFormat32bppARGB, &bitmapDataNew);
	unsigned int *pRawBitmapNew = (unsigned int*)bitmapDataNew.Scan0;

	int nPixels = height*bitmapData.Stride / 4;
	int nPixels2 = height*bitmapDataNew.Stride / 4;
	int offset = 0;
	unsigned int pixCol;
	for (int i = 0; i < nPixels; i++)
	{
		if (i != 0 && i%width == 0)
			offset+=2;
		if(c == 1)
			pixCol = pRawBitmapOrig[i];
		else if (c == 2)
			pixCol = pRawBitmapCopy[i];
		int nX = i%width + width * offset;
		if (nX < nPixels2)
		{
			pRawBitmapNew[nX] = pixCol;
		}	
	}
	offset = 1;
	for (int i = 0; i < nPixels; i++)
	{
		if (i != 0 && i%width == 0)
			offset += 2;
		if (c == 1)
			pixCol = pRawBitmapCopy[i];
		else if (c == 2)
			pixCol = pRawBitmapOrig[i];

		int nX = i%width + width * offset;
		if (nX < nPixels2)
		{
			pRawBitmapNew[nX] = pixCol;
		}
	}

	pBitmap->UnlockBits(&bitmapData);
	tmp->UnlockBits(&bitmapData);
	pBitmapNew->UnlockBits(&bitmapData);
	return pBitmapNew;
}
void rightMirror(Bitmap *, int)
{

}

void Hist(Bitmap *pBitmap, unsigned int hist[][256], unsigned int *mx)
{
	if (!pBitmap) return;

	for (int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 256; j++)
			hist[i][j] = 0;
	}

	int width = pBitmap->GetWidth();
	int height = pBitmap->GetHeight();

	BitmapData bitmapData;
	pBitmap->LockBits(&Rect(0, 0, width, height), ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData);

	unsigned int *pRawBitmapOrig = (unsigned int*)bitmapData.Scan0;	// for easy access and indexing

	int nPixels = height*bitmapData.Stride / 4;
	for (int i = 0; i < nPixels; i++)
	{
		unsigned int curColor = pRawBitmapOrig[i];

		// NOTE: a is not necessarily for alpha, r isn't red, etc.
		// But this is okay because order is preserved and all bits are treated equally.

		// NOTE: RGB Alpha

		int a = (curColor & 0xff000000) >> 24;
		int r = (curColor & 0xff0000) >> 16;
		int g = (curColor & 0xff00) >> 8;
		int b = curColor & 0xff;
		int gray = 0.2125*r + 0.7154*g + 0.0721*b;

		hist[0][r]+=1;
		hist[1][g] += 1;
		hist[2][b] += 1;
		hist[3][gray] += 1;
	}
	*mx = 0;
	for(int i = 0; i < 4; i++)
	{
		for(int j=0; j<256; j++)
			hist[i][j] > *mx ? *mx = hist[i][j] : *mx += 0;
	}
	pBitmap->UnlockBits(&bitmapData);
}

Bitmap* Scale(Bitmap *pBitmap, int newW, int newH)
{
	// scales images using an optimized form of bresenham's formula
	if (!pBitmap) return pBitmap;

	int width = pBitmap->GetWidth();
	int height = pBitmap->GetHeight();

	BitmapData bitmapData;
	pBitmap->LockBits(&Rect(0, 0, width, height), ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData);
	unsigned int *pRawBitmapOrig = (unsigned int*)bitmapData.Scan0;	// for easy access and indexing

	Bitmap *pBitmapNew = new Bitmap(newW, newH, PixelFormat32bppARGB);
	BitmapData bitmapDataNew;
	pBitmapNew->LockBits(&Rect(0, 0, newW, newH), ImageLockModeWrite, PixelFormat32bppARGB, &bitmapDataNew);
	unsigned int *pRawBitmapNew = (unsigned int*)bitmapDataNew.Scan0;


		// YD compensates for the x loop by subtracting the width back out
		int YD = (height / newH) * width - width;
		int YR = height % newH;
		int XD = width / newW;
		int XR = width % newW;
		int outOffset = 0;
		int inOffset = 0;

		for(int y = newH, YE = 0; y > 0; y--) {
			for (int x = newW, XE = 0; x > 0; x--) {
				pRawBitmapNew[outOffset++] = pRawBitmapOrig[inOffset];
				inOffset += XD;
				XE += XR;
				if (XE >= newW) {
					XE -= newW;
					inOffset++;
				}
			}
			inOffset += YD;
			YE += YR;
			if (YE >= newH) {
				YE -= newH;
				inOffset += width;
			}
		}
		//return Image.createRGBImage(rawOutput, newWidth, newHeight, false);
		pBitmap->UnlockBits(&bitmapData);
		//tmp->UnlockBits(&bitmapData);
		pBitmapNew->UnlockBits(&bitmapData);
		return pBitmapNew;
}