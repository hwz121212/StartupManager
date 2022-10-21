/* 
 * this file is part of the Startup Manager splash screen
 * Copyright (C) 2007-2008 Glenn Van Loon, glenn@startupmanager.org
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

// The code to insert a button in an edit control was based on a Catch22 tutorial,
// this tutorial can be found over here: http://www.catch22.net/tuts/editbutton.asp
// I rewrote the code to be a Win32++ control and added support for tooltips, I
// also extended it to support windows xp themes and added browse functionality.

///////////////////////////////////////
// BrowseEdit.cpp

#include "BrowseEdit.h"
#include "resource.h"

#include <windowsx.h>

#ifndef WM_THEMECHANGED
#define WM_THEMECHANGED		0x031A 
#endif

BOOL CBrowseEdit::m_bEnableButtonTheme = TRUE;

CBrowseEdit::CBrowseEdit() : m_bButtonDown(FALSE), m_bMouseDown(FALSE), m_Tracking(FALSE),
							m_bHover(FALSE), m_bEnabled(TRUE)
{
	m_cxLeftEdge   = 0;
	m_cxRightEdge  = 0;
	m_cyTopEdge    = 0;
	m_cyBottomEdge = 0;

	m_themesDll    = 0;
	m_hButtonTheme = 0;
	
	pfnOpenThemeData       = 0;
	pfnDrawThemeBackground = 0;
	pfnCloseThemeData      = 0;
	pfnDrawThemeText       = 0;

	m_BrowseFilter = _T("All files (*.*)\0*.*\0");
	m_InitialDir   = _T("");
}

CBrowseEdit::~CBrowseEdit()
{
	if (m_themesDll != 0)
		::FreeLibrary(m_themesDll);
}

BOOL CBrowseEdit::InitThemesDll()
{
	m_themesDll = ::LoadLibrary(_T("UXTHEME.DLL"));
	if (m_themesDll)
	{
		pfnOpenThemeData       = (PFNOPENTHEMEDATA)::GetProcAddress(m_themesDll, "OpenThemeData");
		pfnDrawThemeBackground = (PFNDRAWTHEMEBACKGROUND)::GetProcAddress(m_themesDll, "DrawThemeBackground");
		pfnCloseThemeData      = (PFNCLOSETHEMEDATA)::GetProcAddress(m_themesDll, "CloseThemeData");
		pfnDrawThemeText       = (PFNDRAWTHEMETEXT)::GetProcAddress(m_themesDll, "DrawThemeText");

		if (pfnOpenThemeData && pfnCloseThemeData && pfnDrawThemeBackground && pfnDrawThemeText)
			return TRUE;			
		
		// Something went wrong!
		TRACE(_T("XP themes not supported"));
        return FALSE;
	}
	
	return FALSE;
}

BOOL CBrowseEdit::AttachDlgItem(UINT nID, CWnd *pParent)
{
	if (CWnd::AttachDlgItem(nID, pParent) == FALSE)
		return FALSE;
	
	m_bEnabled = ::IsWindowEnabled(GetHwnd());

    // We only need one call of InitThemes and one call of OpenThemeData.
    if (!m_hButtonTheme && m_bEnableButtonTheme)
        if (InitThemesDll() == TRUE)
            m_hButtonTheme = pfnOpenThemeData(GetHwnd(), L"Button");

	// force the edit control to update its non-client area
	::SetWindowPos(GetHwnd(), 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE |
		SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);

	// create the tooltip control
	m_ToolTip.Create(GetHwnd());

	return TRUE;
}

void CBrowseEdit::SetBrowseInfo(LPCTSTR filter, const tstring &initialDir /*= _T("")*/)
{
	m_BrowseFilter = filter;
	m_InitialDir   = initialDir;
}

void CBrowseEdit::SetToolTips(LPCTSTR edittip, LPCTSTR buttontip)
{
	// get the coordinates of the window
	// work out where the button is located
	RECT rcBtn;
	::GetWindowRect(GetHwnd(), &rcBtn);
	GetButtonRect(&rcBtn);
	POINT pt;
	pt.x = rcBtn.left;
	pt.y = rcBtn.bottom;
	::ScreenToClient(GetHwnd(), &pt);
	rcBtn.left   = pt.x;
	rcBtn.bottom = pt.y;
	pt.x = rcBtn.right;
	pt.y = rcBtn.top;
	::ScreenToClient(GetHwnd(), &pt);
	rcBtn.right = pt.x;
	rcBtn.top   = pt.y;
	m_ToolTip.Add(buttontip, &rcBtn);

	// get the coordinates of the window
	// adapt window size to exclude button
	RECT rcClient;
	::GetClientRect(GetHwnd(), &rcClient);
	m_ToolTip.Add(edittip, &rcClient);
}

LRESULT CBrowseEdit::OnNcCalcSize(WPARAM wParam, LPARAM lParam)
{	
	RECT *prect = (RECT *)lParam;
	RECT oldrect = *prect;

	// let the old wndproc allocate space for the borders,
	// or any other non-client space.
	WndProcDefault(GetHwnd(), WM_NCCALCSIZE, wParam, lParam);
   
	// calculate what the size of each window border is,
	// we need to know where the button is going to live.
	m_cxLeftEdge   = prect->left - oldrect.left; 
	m_cxRightEdge  = oldrect.right - prect->right;
	m_cyTopEdge    = prect->top - oldrect.top;
	m_cyBottomEdge = oldrect.bottom - prect->bottom;   

	// now we can allocate additional space by deflating the
	// rectangle even further. Our button will go on the right-hand side,
	// and will be the same width as a scrollbar button
	prect->right -= ::GetSystemMetrics(SM_CXVSCROLL);

	// that's it! Easy or what!
	return 0L;
}

// retrieve the coordinates of an inserted button, given the
// specified window rectangle. 
void CBrowseEdit::GetButtonRect(RECT *rect)
{
	rect->right  -= m_cxRightEdge;
	rect->top    += m_cyTopEdge;
	rect->bottom -= m_cyBottomEdge;
	rect->left    = rect->right - ::GetSystemMetrics(SM_CXVSCROLL);

	if (m_cxRightEdge > m_cxLeftEdge)
		::OffsetRect(rect, m_cxRightEdge - m_cxLeftEdge, 0);
}

void CBrowseEdit::DrawThemedButton(HDC hDC, RECT *prect)
{
	DWORD dThemeState;
	
	::InflateRect(prect, 1, 1);

	if (m_bEnabled == TRUE)
	{
		::FillRect(hDC, prect, ::GetSysColorBrush(COLOR_WINDOW));

		if (m_bMouseDown == TRUE)
		{
			if (m_bButtonDown == TRUE)
				dThemeState = PBS_PRESSED;
			else
				dThemeState = PBS_HOT;
		}
		else if (m_bHover == TRUE)
			dThemeState = PBS_HOT;
		else
			dThemeState = PBS_NORMAL;
	}
	else
		dThemeState = PBS_DISABLED;

	pfnDrawThemeBackground(m_hButtonTheme, hDC, BP_PUSHBUTTON, dThemeState, prect, 0);
	pfnDrawThemeText(m_hButtonTheme, hDC, BP_PUSHBUTTON, dThemeState, L"...", 3,
		DT_CENTER | DT_VCENTER | DT_SINGLELINE, 0, prect);
}

void CBrowseEdit::DrawOldButton(HDC hDC, RECT *prect)
{
	if (m_bEnabled == TRUE)
	{
		if (m_bButtonDown == TRUE)
			::DrawEdge(hDC, prect, EDGE_RAISED, BF_RECT | BF_FLAT | BF_ADJUST);
		else
			::DrawEdge(hDC, prect, EDGE_RAISED, BF_RECT | BF_ADJUST);
	}
	else
		::DrawEdge(hDC, prect, EDGE_ETCHED, BF_RECT | BF_ADJUST);

	// fill the inside of the button
	::FillRect(hDC, prect, ::GetSysColorBrush(COLOR_BTNFACE));

	::SetBkMode(hDC, TRANSPARENT);

	if (m_bEnabled == FALSE)
		::SetTextColor(hDC, RGB(135,135,135));
	
	::DrawText(hDC, _T("..."), 3, prect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void CBrowseEdit::DrawInsertedButton(RECT *prect)
{
	HDC hDC = ::GetWindowDC(GetHwnd());

	// now draw our inserted button:
	if (m_hButtonTheme)
		DrawThemedButton(hDC, prect);
	else
		DrawOldButton(hDC, prect);
		
	::ReleaseDC(GetHwnd(), hDC);
}

LRESULT CBrowseEdit::OnNcPaint(WPARAM wParam, LPARAM lParam)
{
	// let the old window procedure draw the borders / other non-client
	// bits-and-pieces for us.
	WndProcDefault(GetHwnd(), WM_NCPAINT, wParam, lParam);
		
	// get the screen coordinates of the window.
	// adjust the coordinates so they start from 0,0
	RECT rect;
	::GetWindowRect(GetHwnd(), &rect);
	::OffsetRect(&rect, -rect.left, -rect.top);
		
	// work out where to draw the button
	GetButtonRect(&rect);

	DrawInsertedButton(&rect);

	// that's it! This is too easy!
	return 0L;
}

void CBrowseEdit::OnNcLButtonDown(LPARAM lParam)
{
	// get the screen coordinates of the mouse
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
		
	// get the position of the inserted button
	RECT rect;
	::GetWindowRect(GetHwnd(), &rect);
	pt.x -= rect.left;
	pt.y -= rect.top;
	::OffsetRect(&rect, -rect.left, -rect.top);
	GetButtonRect(&rect);
		
	// check that the mouse is within the inserted button
	if (::PtInRect(&rect, pt))
	{
		::SetCapture(GetHwnd());
			
		m_bButtonDown = TRUE;
		m_bMouseDown  = TRUE;
			
		//redraw the non-client area to reflect the change
		DrawInsertedButton(&rect);
	}
}

void CBrowseEdit::OnNcMouseMove()
{
	// Don't to things if they aren't really required...
	if (m_hButtonTheme)
	{
		// We want to get WM_NCMOUSEHOVER and WM_NCMOUSELEAVE messages, so we call _TrackMouseEvent
		if (m_Tracking == FALSE)
		{
			TRACKMOUSEEVENT tme;
			::ZeroMemory(&tme, sizeof(TRACKMOUSEEVENT));
			tme.cbSize      = sizeof(tme);
			tme.hwndTrack   = GetHwnd();
			tme.dwFlags     = TME_NONCLIENT | TME_LEAVE | TME_HOVER;
			tme.dwHoverTime = 1;
			m_Tracking = ::_TrackMouseEvent(&tme);
		}
	}
}

void CBrowseEdit::OnNcMouseHover()
{
	// get the coordinates of the mouse
	POINT pt;
	::GetCursorPos(&pt);
		
	// get the position of the inserted button
	RECT rect;
	::GetWindowRect(GetHwnd(), &rect);
	pt.x -= rect.left;
	pt.y -= rect.top;
	::OffsetRect(&rect, -rect.left, -rect.top);
		
	GetButtonRect(&rect);
		
	// check that the mouse is within the inserted button
	if (::PtInRect(&rect, pt))	
		m_bHover = TRUE;
	
	::RedrawWindow(GetHwnd(), 0, 0, RDW_FRAME | RDW_INVALIDATE);
}

void CBrowseEdit::OnNcMouseLeave()
{
	m_Tracking = FALSE;
	m_bHover = FALSE;
	::RedrawWindow(GetHwnd(), 0, 0, RDW_FRAME | RDW_INVALIDATE);
}

void CBrowseEdit::OnButtonClicked()
{
	TCHAR szFileName[MAX_PATH + 1];
	szFileName[0] = 0;
	
	OPENFILENAME ofn;
	::ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner   = m_hWndParent;
	ofn.lpstrFilter = m_BrowseFilter;
	ofn.lpstrFile   = szFileName;
	ofn.nMaxFile    = MAX_PATH;
	ofn.Flags       = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	if (m_InitialDir.length() > 0)
	{
		const tstring &InitialDir = m_InitialDir;
		ofn.lpstrInitialDir = InitialDir.c_str();
	}
		
	if (::GetOpenFileName(&ofn))
		::SetWindowText(GetHwnd(), szFileName);
}

void CBrowseEdit::OnLButtonUp(LPARAM lParam)
{
	if (m_bMouseDown == FALSE)
		return;
		
	// get the SCREEN coordinates of the mouse
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	::ClientToScreen(GetHwnd(), &pt);
		
	// get the position of the inserted button
	RECT rect;
	::GetWindowRect(GetHwnd(), &rect);
	pt.x -= rect.left;
	pt.y -= rect.top;
	::OffsetRect(&rect, -rect.left, -rect.top);
		
	GetButtonRect(&rect);
		
	// check that the mouse is within the inserted button
	if (::PtInRect(&rect, pt))
		OnButtonClicked();
		
	::ReleaseCapture();
	m_bButtonDown = FALSE;
	m_bMouseDown  = FALSE;
		
	// redraw the non-client area to reflect the change.
	DrawInsertedButton(&rect);
}

void CBrowseEdit::OnMouseMove(LPARAM lParam)
{
	if (m_bMouseDown == FALSE)
		return;

	// get the SCREEN coordinates of the mouse
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	::ClientToScreen(GetHwnd(), &pt);
		
	// get the position of the inserted button
	RECT rect;
	::GetWindowRect(GetHwnd(), &rect);
	pt.x -= rect.left;
	pt.y -= rect.top;
	::OffsetRect(&rect, -rect.left, -rect.top);

	GetButtonRect(&rect);
		
	BOOL oldstate = m_bButtonDown;

	// check that the mouse is within the inserted button
	if (::PtInRect(&rect, pt))
		m_bButtonDown = TRUE;
	else
		m_bButtonDown = FALSE;        
		
	// redraw the non-client area to reflect the change.
	// to prevent flicker, we only redraw the button if its state
	// has changed
	if (oldstate != m_bButtonDown)
		DrawInsertedButton(&rect);
}

void CBrowseEdit::OnDestroy()
{
	if (m_hButtonTheme)
	{
		pfnCloseThemeData(m_hButtonTheme);
		
		if (m_themesDll != 0)
		{
			::FreeLibrary(m_themesDll);
			m_themesDll = 0;
		}
	}
}

LRESULT CBrowseEdit::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_THEMECHANGED:
		{
			if(m_hButtonTheme)
				pfnCloseThemeData(m_hButtonTheme);
			m_hButtonTheme = pfnOpenThemeData(GetHwnd(), L"Button");
		}
		break;
	
	case WM_NCCALCSIZE:
		return OnNcCalcSize(wParam, lParam);

	case WM_NCPAINT:
		return OnNcPaint(wParam, lParam);

	case WM_NCHITTEST:
		{
			// get the screen coordinates of the mouse
			POINT pt;
			pt.x = GET_X_LPARAM(lParam);
			pt.y = GET_Y_LPARAM(lParam);

			// get the position of the inserted button
			RECT rect;
			::GetWindowRect(GetHwnd(), &rect);
			GetButtonRect(&rect);
		
			// check that the mouse is within the inserted button
			if(::PtInRect(&rect, pt))
				return HTBORDER;
		}
		break;

	case WM_NCRBUTTONDBLCLK:
	case WM_NCLBUTTONDOWN:
		OnNcLButtonDown(lParam);
		break;

	case WM_NCMOUSEMOVE:
		OnNcMouseMove();
		break;

	case WM_NCMOUSEHOVER:
		OnNcMouseHover();
		break;

	case WM_NCMOUSELEAVE:
		OnNcMouseLeave();
		break;

	case WM_LBUTTONUP:
		OnLButtonUp(lParam);
		break;

	case WM_MOUSEMOVE:
		OnMouseMove(lParam);
		break;

	case WM_ENABLE:
		m_bEnabled = (BOOL)wParam;
		break;

	case WM_DESTROY:
		OnDestroy();
		break;

	default:
		break;
	}
	
	// Pass unhandled messages on to parent WndProc
	return WndProcDefault(hWnd, uMsg, wParam, lParam);
}
