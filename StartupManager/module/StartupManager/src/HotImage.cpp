/*
 * this file is part of Startup Manager
 * Copyright (C) 2004-2008 Glenn Van Loon, glenn@startupmanager.org
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

#include "HotImage.h"

CHotImage::CHotImage() : m_bUrlVisited(FALSE), m_bClicked(FALSE)
{
	m_Target = _T("");
	
	// Create the cursor
	m_hCursor = ::LoadCursor(0, IDC_HAND); 
	
	// IDC_HAND is not available on Win95, so load a reasonable alternative
	if (!m_hCursor)    
		m_hCursor = ::LoadCursor(0, IDC_ARROW);
}

CHotImage::~CHotImage()
{
}

void CHotImage::Create(HWND hWndParent, int ImageID, HINSTANCE hInst)
{
	//display image
	Display(ImageID, hWndParent, hInst);
}

void CHotImage::SetTarget(const tstring &target)
{
	m_Target = target;
}

void CHotImage::OnLButtonDown()
{
	::SetCapture(GetHwnd());
	m_bClicked = TRUE;
}

void CHotImage::OnLButtonUp(LPARAM lParam)
{
	::ReleaseCapture();
	if (m_bClicked)
	{	
		m_bClicked = FALSE;
		POINT pt;
		RECT rc;
		pt.x = (short)LOWORD(lParam);
		pt.y = (short)HIWORD(lParam);
		::ClientToScreen(GetHwnd(), &pt);
		::GetWindowRect(GetHwnd(), &rc);
		
		if (PtInRect(&rc, pt)) 
			OpenUrl();
	}
}

void CHotImage::OpenUrl()
{
	int result = 0;
	if (m_Target.length() > 0)
		result = (int)(LRESULT)::ShellExecute(0, _T("open"), m_Target.c_str(), 0, 0, SW_SHOWNORMAL);
	else
	{
		DebugWarnMsg(TEXT("No target set"));
		return;
	}

	if(result > 32)
		m_bUrlVisited = TRUE;
	else
		DebugWarnMsg(TEXT("ShellExecute Failed"));
}

LRESULT CHotImage::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		OnLButtonDown();
		break;

	case WM_LBUTTONUP:
		OnLButtonUp(lParam);
		break;

	case WM_SETCURSOR:
		::SetCursor(m_hCursor);
		return 1L;

	case WM_NCHITTEST:
		return HTCLIENT;  // Claim that the mouse is in a client area
	}
	
	// Pass unhandled messages on to the default window procedure
	return CImage::WndProc(hWnd, uMsg, wParam, lParam);
}
