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

///////////////////////////////////////
// BrowseEdit.h

#ifndef BROWSEEDIT_H
#define BROWSEEDIT_H

#include "../Win32++/WinCore.h"
#include "ToolTip.h"
#include "types/tstring.h"

#ifndef _WIN32_WINNT
#define _WIN32_WINNT		0x0501
#endif

#include <uxtheme.h>
#include <vsstyle.h>

class CBrowseEdit : public CWnd
{
public:
	CBrowseEdit();
	virtual ~CBrowseEdit();
	virtual BOOL AttachDlgItem(UINT nID, CWnd* pParent);
	void SetBrowseInfo(LPCTSTR filter, const tString &initialDir = _T(""));
	void SetToolTips(LPCTSTR edittip, LPCTSTR buttontip);
	void Enable() {::EnableWindow(GetHwnd(), TRUE);}
	void Disable() {::EnableWindow(GetHwnd(), FALSE);}

	static void DisableButtonTheme() {m_bEnableButtonTheme = FALSE;}
	static void EnableButtonTheme() {m_bEnableButtonTheme = TRUE;}

protected:
	virtual LRESULT OnNcCalcSize(WPARAM wParam, LPARAM lParam);
	virtual void GetButtonRect(RECT *rect);
	virtual void DrawThemedButton(HDC hDC, RECT *prect);
	virtual void DrawOldButton(HDC hDC, RECT *prect);
	virtual void DrawInsertedButton(RECT *prect);
	virtual LRESULT OnNcPaint(WPARAM wParam, LPARAM lParam);
	virtual void OnNcLButtonDown(LPARAM lParam);
	virtual void OnNcMouseMove();
	virtual void OnNcMouseHover();
	virtual void OnNcMouseLeave();
	virtual void OnButtonClicked();
	virtual void OnLButtonUp(LPARAM lParam);
	virtual void OnMouseMove(LPARAM lParam);
	virtual void OnDestroy();
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	BOOL InitThemesDll();

	BOOL m_bButtonDown;	// is the button up/down?
	BOOL m_bMouseDown;	// is the mouse activating the button?
	BOOL m_Tracking;	// are we tracking the mouse?
	BOOL m_bHover;		// is the mouse making the button hot?
	BOOL m_bEnabled;	// is the control enabled?
   
	// size of the current window borders.
	// given these, we know where to insert our button
	int m_cxLeftEdge, m_cxRightEdge; 
	int m_cyTopEdge, m_cyBottomEdge;

	// browse information
	LPCTSTR m_BrowseFilter;
	tstring m_InitialDir;
	
	// tooltip control
	CToolTip m_ToolTip;

	// stuff to support xp themes for the browse button
	
	HMODULE m_themesDll;
	HTHEME m_hButtonTheme;
	static BOOL m_bEnableButtonTheme;

	typedef HRESULT (WINAPI *PFNCLOSETHEMEDATA)(HTHEME);
	typedef HRESULT (WINAPI *PFNDRAWTHEMEBACKGROUND)(HTHEME, HDC, int, int, const LPRECT, const LPRECT);
	typedef HTHEME  (WINAPI *PFNOPENTHEMEDATA)(HWND, LPCWSTR);
	typedef HRESULT (WINAPI *PFNDRAWTHEMETEXT)(HTHEME, HDC, int, int, LPCWSTR, int, DWORD, DWORD, const LPRECT);

	PFNOPENTHEMEDATA pfnOpenThemeData;
	PFNDRAWTHEMEBACKGROUND pfnDrawThemeBackground;
	PFNCLOSETHEMEDATA pfnCloseThemeData;
	PFNDRAWTHEMETEXT pfnDrawThemeText;
};

#endif //BROWSEEDIT_H
