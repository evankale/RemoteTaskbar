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

#ifndef ICON_UTIL_H
#define ICON_UTIL_H

#include <Windows.h>
#include <GdiPlus.h>

struct BITMAP_AND_BYTES
{
	Gdiplus::Bitmap* bmp;
	int* bytes;
};

class IconUtil
{
public:
	static HICON GetHICON(HWND hWnd);
	static int HIconToPNG(unsigned char * buffer, unsigned int bufferLength, HICON hIcon, unsigned int * outputSize);

private:
	static BITMAP_AND_BYTES createAlphaChannelBitmapFromIcon(HICON hIcon);
	static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
};


#endif