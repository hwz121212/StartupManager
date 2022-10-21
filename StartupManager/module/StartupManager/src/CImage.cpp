/*
 * this file is part of Startup Manager
 * Copyright (C) 2004-2008 Glenn Van Loon, glenn@startupmanager.org
 * CImage.cpp: Copyright (c) 2007  David Nash
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

#include "CImage.h"
#include <tchar.h>

CImage::CImage() : m_pIPicture(0), m_nHeight(0), m_nWidth(0)
{
	::CoInitialize(0);
}

CImage::~CImage()
{
	if (m_pIPicture)
		m_pIPicture->Release();
	
	::CoUninitialize();
}

void CImage::Display(int ResID, HWND hParent, HINSTANCE hInst)
{
	// Destroy any current window
	if (GetHwnd())
	{
		if (::IsWindow(GetHwnd()))
			::DestroyWindow(GetHwnd());

		m_hWnd = 0;
	}

	// Release any current picture
	if (m_pIPicture)
	{
		m_pIPicture->Release();
		m_pIPicture = 0;
	}

	HRSRC res = ::FindResource(hInst, MAKEINTRESOURCE(ResID), _T("BINARY"));
	if (res) 
	{
		HGLOBAL mem = ::LoadResource(hInst, res);
		LPVOID data = ::LockResource(mem);
		size_t sz = ::SizeofResource(hInst, res);
	
		HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, sz);
		LPVOID pvData = GlobalLock(hGlobal);
		memcpy(pvData, data, sz); 
	
		LPSTREAM pStream = 0;
		CreateStreamOnHGlobal(hGlobal, TRUE, &pStream);
		OleLoadPicture(pStream, 0, FALSE, IID_IPicture, (LPVOID*)&m_pIPicture);
		pStream->Release();

		if (m_pIPicture != 0)
		{
			HDC hDC = ::GetDC(hParent);
			OLE_XSIZE_HIMETRIC hmWidth;
			OLE_YSIZE_HIMETRIC hmHeight;

			m_pIPicture->get_Width(&hmWidth);
			m_pIPicture->get_Height(&hmHeight);

			m_nWidth  = MulDiv(hmWidth, GetDeviceCaps(hDC, LOGPIXELSX), HIMETRIC_PER_INCH);
			m_nHeight = MulDiv(hmHeight, GetDeviceCaps(hDC, LOGPIXELSX), HIMETRIC_PER_INCH);
			::ReleaseDC(hParent, hDC);
		}

		Create(hParent);
		
		UnlockResource(mem);	// Not strictly required according to API documentation
		GlobalUnlock(hGlobal);
		GlobalFree(hGlobal);
	}
}

void CImage::OnPaint(HDC hDC)
{
	if (m_pIPicture != 0)
	{
		OLE_XSIZE_HIMETRIC hmWidth;
		OLE_YSIZE_HIMETRIC hmHeight;

		m_pIPicture->get_Width(&hmWidth);
		m_pIPicture->get_Height(&hmHeight);
		
		RECT r;
		SetRect(&r, 0, 0, m_nWidth, m_nHeight);

		m_pIPicture->Render(hDC, r.left, r.top, m_nWidth, m_nHeight, 0, hmHeight, hmWidth, -hmHeight, &r);
	}
}
void CImage::PreCreate(CREATESTRUCT &cs)
{
	// Set window size
	cs.cx = m_nWidth;
	cs.cy = m_nHeight;
}

LRESULT CImage::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	// Prevent background from being erased
	case WM_ERASEBKGND:
		return TRUE;
	}
	
	// Pass unhandled messages on to the default window procedure
	return WndProcDefault(hWnd, uMsg, wParam, lParam);
}