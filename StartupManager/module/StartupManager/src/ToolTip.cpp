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

#include "ToolTip.h"

void CToolTip::PreCreate(CREATESTRUCT &cs)
{
	cs.style = WS_POPUP;
	cs.lpszClass = TOOLTIPS_CLASS;
}

void CToolTip::OnInitialUpdate()
{
	if (!::SetWindowPos(GetHwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE))
		DebugWarnMsg(TEXT("Unable to set tooltip to foreground."));
	if (::SendMessage(GetHwnd(), TTM_ACTIVATE, static_cast<WPARAM>(TRUE), 0L) != ERROR_SUCCESS)
		DebugWarnMsg(TEXT("Unable to activate tooltip."));
}

BOOL CToolTip::Add(LPCTSTR Text, UINT Control)
{
	HWND hCtrl = ::GetDlgItem(m_hWndParent, Control);
	if (!hCtrl)
		return FALSE;
	
	TOOLINFO ToolTipStruct;
	::ZeroMemory(&ToolTipStruct, sizeof(TOOLINFO));
	ToolTipStruct.cbSize   = sizeof(ToolTipStruct);
	ToolTipStruct.uFlags   = TTF_SUBCLASS | TTF_IDISHWND;
	ToolTipStruct.hwnd     = hCtrl;
	ToolTipStruct.uId      = reinterpret_cast<WPARAM>(ToolTipStruct.hwnd);
	ToolTipStruct.lpszText = const_cast<LPTSTR>(Text);
	ToolTipStruct.hinst    = GetApp()->GetInstanceHandle();
	
	return (BOOL)::SendMessage(GetHwnd(), TTM_ADDTOOL, 0, reinterpret_cast<LPARAM>(&ToolTipStruct));
}

BOOL CToolTip::Add(LPCTSTR Text, RECT *Control)
{
	TOOLINFO ToolTipStruct;
	::ZeroMemory(&ToolTipStruct, sizeof(TOOLINFO));
	ToolTipStruct.cbSize   = sizeof(ToolTipStruct);
	ToolTipStruct.uFlags   = TTF_SUBCLASS;
	ToolTipStruct.hwnd     = m_hWndParent;
	ToolTipStruct.lpszText = const_cast<LPTSTR>(Text);
	ToolTipStruct.hinst    = GetApp()->GetInstanceHandle();

	::CopyRect(&ToolTipStruct.rect, Control);

	return (BOOL)::SendMessage(GetHwnd(), TTM_ADDTOOL, 0, reinterpret_cast<LPARAM>(&ToolTipStruct));
}
