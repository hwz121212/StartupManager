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

#include "leftview.h"
#include "resource.h"
#include "FrameApp.h"
#include "MainFrm.h"

namespace
{
	void GradientFillRect(HDC hdc, LPRECT rcGradient, COLORREF firstColor, COLORREF secondColor, BOOL isVertical)
	{
		BYTE startRed   = GetRValue(firstColor);
		BYTE startGreen = GetGValue(firstColor);
		BYTE startBlue  = GetBValue(firstColor);
		BYTE endRed     = GetRValue(secondColor);
		BYTE endGreen   = GetGValue(secondColor);
		BYTE endBlue    = GetBValue(secondColor);

		HBRUSH endColor = CreateSolidBrush(secondColor);
		FillRect(hdc, rcGradient, endColor);
		DeleteObject(endColor);

		// Gradient line width/height
		int dy = 2;
		int length = (isVertical ? rcGradient->bottom - rcGradient->top : rcGradient->right - rcGradient->left) - dy;
		for (int dn = 0; dn <= length; dn += dy) 
		{ 
			BYTE currentRed = (BYTE)::MulDiv(endRed-startRed, dn, length) + startRed;
			BYTE currentGreen = (BYTE)::MulDiv(endGreen-startGreen, dn, length) + startGreen;
			BYTE currentBlue = (BYTE)::MulDiv(endBlue-startBlue, dn, length) + startBlue;

			RECT currentRect = {0};
			if (isVertical) 
			{ 
				currentRect.left = rcGradient->left;
				currentRect.top = rcGradient->top + dn;
				currentRect.right = currentRect.left + rcGradient->right - rcGradient->left;
				currentRect.bottom = currentRect.top + dy;
			} 
			else 
			{ 
				currentRect.left = rcGradient->left + dn;
				currentRect.top = rcGradient->top;
				currentRect.right = currentRect.left + dy;
				currentRect.bottom = currentRect.top + rcGradient->bottom - rcGradient->top;
			}

			HBRUSH currentColor = ::CreateSolidBrush(RGB(currentRed, currentGreen, currentBlue));
			::FillRect(hdc, &currentRect, currentColor);
			::DeleteObject(currentColor);
		}
	}
}

CLeftView::CLeftView()
{
	m_BootList = 0;

	m_ncRect.left   = 0; 
	m_ncRect.right  = 0;
	m_ncRect.top    = 0;
	m_ncRect.bottom = 0; 
}

CLeftView::~CLeftView()
{
	ImageList_Destroy(TreeView_GetImageList(GetHwnd(), 0));
}

tstring CLeftView::GetSection()
{
	tstring parent = GetParentText();
	tstring select = GetSelItem();

	tstring selsection = _T("");
	if ((parent == _T("Current User")) || (parent == _T("All Users")) || (parent == _T("Computer")))
		selsection = _T("Registry\\") + parent + _T("\\") + select;
	else if ((select == _T("Main")) || (parent == _T("Main")))
		selsection = select;
	else
		selsection = parent + _T("\\") + select;

	return selsection;
}

void CLeftView::OnInitialUpdate()
{
	try
	{
		//set treeview imagelist
		HBITMAP hbm = ::LoadBitmap(GetApp()->GetInstanceHandle(), MAKEINTRESOURCE(IDI_TREE));
		HIMAGELIST hImg = ImageList_Create(16, 16, ILC_COLORDDB | ILC_MASK, 10, 0);
		ImageList_AddMasked(hImg, hbm, RGB(255, 0, 0));
		SetImageList(hImg);
		::DeleteObject(hbm);
	
		//add items to treeview
		HTREEITEM root, st, ini, reg, rcurr, rall, rcomp;
		root  = AddRoot(_T("Main"), 4, 5);
		st    = AddItem(_T("StartUp"), 6, 6, root);
		AddItem(_T("Current User"), 7, 7, st);
		AddItem(_T("All Users"), 7, 7, st);
		reg   = AddItem(_T("Registry"), 0, 0, root);
		rcurr = AddItem(_T("Current User"), 2, 1, reg);
		rall  = AddItem(_T("All Users"), 2, 1, reg);
		rcomp = AddItem(_T("Computer"), 2, 1, reg);
		ini   = AddItem(_T("WinINI"), 8, 8, root);
		AddItem(_T("Load"), 9, 9, ini);
		AddItem(_T("Run"), 9, 9, ini);
	
		//add all registry sections
		tstring section = _T("");
		for (Boot::Core::const_reverse_iterator iter = m_BootList->rbegin(); iter != m_BootList->rend(); ++iter)
			if ((*iter)->GetLongSection().find(_T("Registry: ")) != std::string::npos)
			{
				section = (*iter)->GetSection();
				section = section.substr((*iter)->GetSection().rfind(_T("\\"))+1);
				if ((*iter)->GetSection().find(_T("Registry\\Computer")) == std::string::npos)
				{
					if (dynamic_cast<Boot::Registry::Section *>(*iter)->GetRoot() == HKEY_CURRENT_USER)
						AddItem(section.c_str(), 3, 3, rcurr);
					else if (dynamic_cast<Boot::Registry::Section *>(*iter)->GetRoot() == HKEY_LOCAL_MACHINE)
						AddItem(section.c_str(), 3, 3, rall);
				}
				else
					AddItem(section.c_str(), 3, 3, rcomp);
			}
	
		//expand main item
		TreeView_Expand(GetHwnd(), root, TVE_EXPAND);
		TreeView_Expand(GetHwnd(), st, TVE_EXPAND);
		TreeView_Expand(GetHwnd(), reg, TVE_EXPAND);
		TreeView_Expand(GetHwnd(), ini, TVE_EXPAND);
	
		//set background color
		TreeView_SetBkColor(GetHwnd(), RGB(248, 249, 252));
	}

	catch (Boot::Exception const& e)
	{
		::MessageBox(GetHwnd(), e.GetMsg().c_str(), LoadString(MSG_INFORMATION), MB_OK | MB_ICONINFORMATION);
	}

	catch (...)
	{
		DebugErrMsg(_T("CLeftView::OnInitialUpdate... Unknown exception"));
	}
}

LRESULT CLeftView::OnNotifyReflect(WPARAM /*wParam*/, LPARAM lParam)
{
//	CMainFrame* pMainFrame = (CMainFrame*)GetApp()->GetFrame();
	CMainFrame& MainFrame = GetStartupMgrApp().GetMainFrame();

	if (((LPNMHDR)lParam)->code == TVN_SELCHANGED)
		MainFrame.UpdateGUI();

	return 0L;
}

LRESULT CLeftView::OnNcCalcSize(WPARAM wParam, LPARAM lParam)
{
	RECT *prect = (RECT *)lParam;

	// let the old wndproc allocate space for the borders,
	// or any other non-client space.
	WndProcDefault(GetHwnd(), WM_NCCALCSIZE, wParam, lParam); 

	// calculate what the size of each window border is,
	// we need to know the location of the non-client area.
	m_ncRect.left   = prect->left;
	m_ncRect.right  = prect->right;
	m_ncRect.top    = prect->top;
	m_ncRect.bottom = prect->top + 30;

	// now we can allocate additional space
	prect->top += 32;

	// take scrollbar into account
	SCROLLBARINFO sbi;
	::ZeroMemory(&sbi, sizeof(SCROLLBARINFO));
	sbi.cbSize = sizeof(sbi);
	if (::GetScrollBarInfo(GetHwnd(), OBJID_VSCROLL, &sbi))
		if (!(sbi.rgstate[0] & STATE_SYSTEM_INVISIBLE))
			m_ncRect.right  += ::GetSystemMetrics(SM_CXVSCROLL);

	// that's it! Easy or what!
	return 0L;
}

void CLeftView::DrawTitleBar(CDC& cDC)
{
	// fill the non-client area with a gradient fill algorithm
	// first fill the first half
	RECT firstHalf;
	firstHalf.left   = m_ncRect.left;
	firstHalf.right  = m_ncRect.right;
	firstHalf.top    = m_ncRect.top;
	firstHalf.bottom = m_ncRect.bottom / 2;
	GradientFillRect(cDC, &firstHalf, RGB(248, 249, 252), RGB(152,150,180), TRUE);
	// and then the second half
	RECT secondHalf;
	secondHalf.left   = m_ncRect.left;
	secondHalf.right  = m_ncRect.right;
	secondHalf.top    = m_ncRect.bottom / 2;
	secondHalf.bottom = m_ncRect.bottom;
	GradientFillRect(cDC, &secondHalf, RGB(152,150,180), RGB(248, 249, 252), TRUE);

	// draw the title text
	::SetBkMode(cDC, TRANSPARENT);
	::SetTextColor(cDC, RGB(248, 249, 252));
	tstring treetitle = LoadString(STR_TREETITLE);
	::DrawText(cDC, treetitle.c_str(), static_cast<int>(treetitle.length()), &m_ncRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

LRESULT CLeftView::OnNcPaint(WPARAM wParam, LPARAM lParam)
{
	// let the old window procedure draw the borders / other non-client
	// bits-and-pieces for us.
	WndProcDefault(GetHwnd(), WM_NCPAINT, wParam, lParam);

	// get device context
	CDC cDC = ::GetWindowDC(GetHwnd());
	
	// make sure there is some distance (2 pixels) between the non-client and client area
	RECT rc;
	rc.left   = m_ncRect.left;
	rc.right  = m_ncRect.right;
	rc.top    = m_ncRect.bottom;
	rc.bottom = m_ncRect.bottom + 2;
	HBRUSH hBrush = ::CreateSolidBrush(RGB(248, 249, 252));
	::FillRect(cDC, &rc, hBrush);
	::DeleteObject(hBrush);

	// draw the title bar...
	DrawTitleBar(cDC);

	// that's it!
	return 0L;
}

LRESULT CLeftView::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{	
	case WM_NCCALCSIZE:
		return OnNcCalcSize(wParam, lParam);

	case WM_NCPAINT:
		return OnNcPaint(wParam, lParam);
	}
	
	// Pass unhandled messages on to parent WndProc
	return WndProcDefault(hWnd, uMsg, wParam, lParam);
}
