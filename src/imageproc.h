#pragma once
#include "stdafx.h"

/*void TestConvolution(Bitmap *);
void LockBitsConvolution(Bitmap *, int, int);*/

void RemoveBlue(Bitmap *);
void Invert(Bitmap *);
void GrayScale(Bitmap *);
void CChannel(Bitmap *, int);
Bitmap* Rotate(Bitmap *, int);
void Mirror(Bitmap *, int);
Bitmap* Mirror2(Bitmap *, int);
void rightMirror(Bitmap *, int);
void Hist(Bitmap *, unsigned int [][256], unsigned int *);
Bitmap* Scale(Bitmap *, int , int );
