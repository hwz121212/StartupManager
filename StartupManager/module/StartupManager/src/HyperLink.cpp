/*
 * this file is part of Startup Manager
 * Copyright (C) 2004-2008 Glenn Van Loon, glenn@startupmanager.org
 * HyperLink.cpp: Copyright (c) 2005-2007  David Nash
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

#include "HyperLink.h"

CHyperlink::CHyperlink() : m_bUrlVisited(FALSE), m_bClicked(FALSE), m_crVisited(RGB(128, 0, 128)), 
                            m_crNotVisited(RGB(0,0,255)), m_hUrlFont(0)
{
	m_Target = _T("");
	
	// Create the cursor
	m_hCursor = ::LoadCursor(0, IDC_HAND); 
	
	// IDC_HAND is not available on Win95, so load a reasonable alternative
	if (!m_hCursor)    
		m_hCursor = ::LoadCursor(0, IDC_ARROW);
}

CHyperlink::~CHyperlink()
{
	if (m_hUrlFont) ::DeleteObject(m_hUrlFont);
}

BOOL CHyperlink::AttachDlgItem(UINT nID, CWnd* pParent)
{
	BOOL bSuccess = CWnd::AttachDlgItem(nID, pParent);

	LOGFONT lf;
	m_hUrlFont = (HFONT)::SendMessage( GetHwnd(), WM_GETFONT, 0, 0);
	::GetObject(m_hUrlFont, sizeof(LOGFONT), &lf);
	lf.lfUnderline = TRUE;	
	m_hUrlFont = ::CreateFontIndirect(&lf); 

	return bSuccess;
}

void CHyperlink::SetTarget(const tstring &target)
{
	m_Target = target;
}

void CHyperlink::OnLButtonDown()
{
	::SetCapture(GetHwnd());
	m_bClicked = TRUE;
}

void CHyperlink::OnLButtonUp(LPARAM lParam)
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

void CHyperlink::OpenUrl()
{
	int result = 0;
	if (m_Target.length() > 0)
		result = (int)(LRESULT)::ShellExecute(0, _T("open"), m_Target.c_str(), 0, 0, SW_SHOWNORMAL);
	else
	{
		TCHAR szUrl[ MAX_PATH + 1 ];
		// Get the url link text
		::GetWindowText(GetHwnd(), szUrl, MAX_PATH);
		result = (int)(LRESULT)::ShellExecute(0, _T("open"), szUrl, 0, 0, SW_SHOWNORMAL);
	}

	if (result > 32)
	{
		m_bUrlVisited = TRUE;
		
		// redraw the window to update the color
		::InvalidateRect(GetHwnd(), 0, FALSE);
	}
	else
		DebugWarnMsg(TEXT("ShellExecute Failed"));
}

LRESULT CHyperlink::OnMessageReflect(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/)
{
	// Messages such as WM_CTLCOLORSTATIC are reflected back to the CWnd object that created them.
	if (uMsg ==  WM_CTLCOLORSTATIC)
	{ 
		HDC hDC = (HDC)wParam; 
	
		::SetTextColor(hDC, m_bUrlVisited? m_crVisited : m_crNotVisited);
		::SetBkMode(hDC, TRANSPARENT);
		::SelectObject(hDC, m_hUrlFont);
		return (LRESULT)::GetSysColorBrush(COLOR_BTNFACE);
	}
	return 0L;
}

LRESULT CHyperlink::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
	return WndProcDefault(hWnd, uMsg, wParam, lParam);
}
