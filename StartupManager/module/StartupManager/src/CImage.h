/*
 * this file is part of Startup Manager
 * Copyright (C) 2004-2008 Glenn Van Loon, glenn@startupmanager.org
 * CImage.h: Copyright (c) 2007  David Nash
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef CIMAGE_H
#define CIMAGE_H

#include "..\Win32++\WinCore.h"
#include <olectl.h>

#define HIMETRIC_PER_INCH   2540    // number HIMETRIC units per inch

class CImage : public CWnd
{
public:
	CImage();
	virtual ~CImage();
	void Display(int ResourceHandle, HWND hParent, HINSTANCE hInst);

protected:
	int m_nHeight;
	int m_nWidth;

	void PreCreate(CREATESTRUCT &cs);
	virtual void OnPaint(HDC hDC);
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
     
private:
	IPicture* m_pIPicture;
};

#endif //CIMAGE_H
