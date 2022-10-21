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

#include "TitleTip.h"

void CTitleTip::PreCreate(CREATESTRUCT &cs)
{
	cs.style     = TTS_NOPREFIX | TTS_NOANIMATE | TTS_NOFADE;
	cs.lpszClass = TOOLTIPS_CLASS;
}

void CTitleTip::TrackActivate(BOOL bActivate /*= TRUE*/)
{
	::SendMessage(GetHwnd(), TTM_TRACKACTIVATE, bActivate, reinterpret_cast<LPARAM>(&m_ToolTipStruct));
}
void CTitleTip::TrackPosition(POINT pPos)
{
	::SendMessage(GetHwnd(), TTM_TRACKPOSITION, 0, MAKELPARAM(pPos.x, pPos.y));
}

void CTitleTip::UpdateRect(LPRECT rcNew)
{
	::CopyRect(&m_ToolTipStruct.rect, rcNew);
	::SendMessage(GetHwnd(), TTM_NEWTOOLRECT, 0, reinterpret_cast<LPARAM>(&m_ToolTipStruct));
}

void CTitleTip::SetMaxTipWidth(int iWidth /*= -1*/)
{
	::SendMessage(GetHwnd(), TTM_SETMAXTIPWIDTH, 0, iWidth);
}

void CTitleTip::Hide()
{
	::ShowWindow(GetHwnd(), SW_HIDE);
}

void CTitleTip::OnInitialUpdate()
{
	// Update tooltip window to always be on top
	if (!::SetWindowPos(GetHwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE))
		DebugWarnMsg(TEXT("Unable to set tooltip to foreground."));

	// Set delay time to 0, this will make the tooltip immediately popup as you the mouse
	// moves over an "active" area
	if (::SendMessage(GetHwnd(), TTM_SETDELAYTIME, TTDT_INITIAL, 0) != ERROR_SUCCESS)
		DebugWarnMsg(_T("Unable to set tooltip initial delay time."));

	// Set reshow time to 0, this will make sure that if you move from one "active" area
	// to another, the new tooltip will immediately be shown.
	if (::SendMessage(GetHwnd(), TTM_SETDELAYTIME, TTDT_RESHOW, 0) != ERROR_SUCCESS)
		DebugWarnMsg(_T("Unable to set tooltip reshow delay time."));

	::ZeroMemory(&m_ToolTipStruct, sizeof(TOOLINFO));
	m_ToolTipStruct.cbSize      = sizeof(m_ToolTipStruct);
	m_ToolTipStruct.uFlags      = TTF_SUBCLASS | TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE | TTF_TRANSPARENT;
	m_ToolTipStruct.hwnd        = m_hWndParent;
	m_ToolTipStruct.uId         = reinterpret_cast<WPARAM>(m_ToolTipStruct.hwnd);
	m_ToolTipStruct.lpszText    = LPSTR_TEXTCALLBACK;
	m_ToolTipStruct.hinst       = 0;
	m_ToolTipStruct.rect.left   = 0;
	m_ToolTipStruct.rect.top    = 0;
	m_ToolTipStruct.rect.right  = 0;
	m_ToolTipStruct.rect.bottom = 0;

	if (!::SendMessage(GetHwnd(), TTM_ADDTOOL, 0, reinterpret_cast<LPARAM>(&m_ToolTipStruct)))
		DebugWarnMsg(_T("Unable to create label tip"));
}

LRESULT CTitleTip::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	// Tooltip styles are poorly implemented in comctl 6, TTN_TRANSPARENT even doesn't
	// work at all, so we have to manually pass the mouse messages to the correct control.
	case WM_NCHITTEST:
		return HTTRANSPARENT;
	} //switch (uMsg)

	// Pass unhandled messages on to the default window procedure
	return WndProcDefault(hWnd, uMsg, wParam, lParam);
}
