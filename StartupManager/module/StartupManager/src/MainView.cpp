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

#include "FrameApp.h"
#include "MainFrm.h"
#include "MainView.h"
#include "resource.h"

CMainView::CMainView()
{
	//::SetProcessDefaultLayout(LAYOUT_RTL);
	SetPanes(GetTreeView(), GetListView());
	SetBarPos(200);
}

LRESULT CMainView::OnNotify(WPARAM wParam, LPARAM lParam)
{
//	CMainFrame* pMainFrame = (CMainFrame*)GetApp()->GetFrame();
	CMainFrame& MainFrame = GetStartupMgrApp().GetMainFrame();
	
	// Pass notification back up to CMainFrame
	return MainFrame.OnNotify(wParam, lParam);
}

LRESULT CMainView::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//switch(uMsg)
	//{
	//}

	// Pass unhandled messages on to the default window procedure
	return WndProcDefault(hWnd, uMsg, wParam, lParam);
}
