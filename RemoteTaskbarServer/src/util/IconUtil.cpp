/*
* Copyright (c) 2014 Evan Kale
* Email: EvanKale91@gmail.com
* Website: www.ISeeDeadPixel.com
* 
* This file is part of RemoteTaskbar.
*
* RemoteTaskbar is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "IconUtil.h"

using namespace Gdiplus;


#ifndef GCL_HICON
#define GCL_HICON           (-14)
#endif

#ifndef GCL_HICONSM
#define GCL_HICONSM         (-34)
#endif

int IconUtil::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

BITMAP_AND_BYTES IconUtil::createAlphaChannelBitmapFromIcon(HICON hIcon)
{

	// Get the icon info
	ICONINFO iconInfo = {0};
	GetIconInfo(hIcon, &iconInfo);

	// Get the screen DC
	HDC dc = GetDC(NULL);

	// Get icon size info
	BITMAP bm = {0};
	GetObject( iconInfo.hbmColor, sizeof( BITMAP ), &bm );

	// Set up BITMAPINFO
	BITMAPINFO bmi = {0};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = bm.bmWidth;
	bmi.bmiHeader.biHeight = -bm.bmHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	// Extract the color bitmap
	int nBits = bm.bmWidth * bm.bmHeight;
	int* colorBits = new int[nBits];
	GetDIBits(dc, iconInfo.hbmColor, 0, bm.bmHeight, colorBits, &bmi, DIB_RGB_COLORS);

	// Check whether the color bitmap has an alpha channel.
	// (On my Windows 7, all file icons I tried have an alpha channel.)
	bool hasAlpha = FALSE;
	for (int i = 0; i < nBits; i++)
	{
		if ((colorBits[i] & 0xff000000) != 0)
		{
			hasAlpha = TRUE;
			break;
		}
	}

	// If no alpha values available, apply the mask bitmap
	if (!hasAlpha)
	{
		// Extract the mask bitmap
		int* maskBits = new int[nBits];
		GetDIBits(dc, iconInfo.hbmMask, 0, bm.bmHeight, maskBits, &bmi, DIB_RGB_COLORS);
		// Copy the mask alphas into the color bits
		for (int i = 0; i < nBits; i++)
		{
			if (maskBits[i] == 0)
			{
				colorBits[i] |= 0xff000000;
			}
		}
		delete[] maskBits;
	} 

	// Release DC and GDI bitmaps
	ReleaseDC(NULL, dc); 
	::DeleteObject(iconInfo.hbmColor);
	::DeleteObject(iconInfo.hbmMask); 

	// Create GDI+ Bitmap
	Bitmap* bmp = new Bitmap(bm.bmWidth, bm.bmHeight, bm.bmWidth*4, PixelFormat32bppARGB, (BYTE*)colorBits);
	BITMAP_AND_BYTES ret = {bmp, colorBits};

	return ret;
}


int IconUtil::HIconToPNG(unsigned char * buffer, unsigned int bufferLength, HICON hIcon, unsigned int * outputSize)
{
	BITMAP_AND_BYTES bbs = createAlphaChannelBitmapFromIcon(hIcon);

	CLSID encoder;
	GetEncoderClsid(L"image/png",&encoder);

	int size;
	ULONG ul;
	IStream *s = NULL;
	Gdiplus::Bitmap * bmp;

	ULARGE_INTEGER lisize;
	LARGE_INTEGER offset;
 
	if(CreateStreamOnHGlobal(NULL, TRUE, &s) != S_OK)
	{
		return -1;
	}

	bbs.bmp->Save(s, &encoder, NULL);

	delete bbs.bmp;
	delete [] bbs.bytes;

	/* get stream size */
	offset.QuadPart = 0;
	if(s->Seek(offset, STREAM_SEEK_END, &lisize) != S_OK)
	{
		s->Release();
		//printf("Failed to get the size of the stream!");
		return -1;
	}
	
	size = (int)lisize.QuadPart;

	*outputSize = size;
 
	/* seek back to beginning of stream */
	s->Seek(offset, STREAM_SEEK_SET, NULL);

	// provided buffer is too small
	if(bufferLength < size)
	{
		s->Release();
		return -1;
	}

	if(s->Read(buffer, size, &ul) != S_OK || size != (int)ul)
	{
		s->Release();
		return -1;
	}
 
	s->Release();
	return size;
}

HICON IconUtil::GetHICON(HWND hWnd)
{
	HICON hIcon = (HICON)GetClassLongPtr(hWnd, GCL_HICON);

	if (hIcon == NULL)
		hIcon = (HICON)SendMessage(hWnd, WM_GETICON, TRUE, 0);
			
	if (hIcon == NULL)
		hIcon = (HICON)SendMessage(hWnd, WM_GETICON, FALSE, 0);

	if (hIcon == NULL)
		hIcon = (HICON)GetClassLongPtr(hWnd, GCL_HICONSM);

	if (hIcon == NULL)
		hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));

	return hIcon;
}