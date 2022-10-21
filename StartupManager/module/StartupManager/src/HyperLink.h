/*
 * this file is part of Startup Manager
 * Copyright (C) 2004-2008 Glenn Van Loon, glenn@startupmanager.org
 * HyperLink.h: Copyright (c) 2005-2007  David Nash
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

#ifndef HYPERLINK_H
#define HYPERLINK_H

#include "..\Win32++\WinCore.h"
#include "types\tstring.h"

#ifndef IDC_HAND
#define IDC_HAND  MAKEINTRESOURCE(32649)
#endif

class CHyperlink : public CWnd
{
public:
	CHyperlink();
	virtual ~CHyperlink();
	virtual BOOL AttachDlgItem(UINT nID, CWnd* pParent);
	void SetTarget(const tstring &target);
	void OnLButtonDown();
	void OnLButtonUp(LPARAM lParam);

protected:
	virtual void OpenUrl();
	virtual LRESULT OnMessageReflect(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	BOOL	m_bUrlVisited;
	BOOL	m_bClicked;		
	COLORREF m_crVisited;
	COLORREF m_crNotVisited;
	HCURSOR m_hCursor;	
	HFONT	m_hUrlFont;
	tstring m_Target;
};
 
#endif //HYPERLINK_H
