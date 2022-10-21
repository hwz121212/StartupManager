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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "rightview.h"
#include "resource.h"
#include "..\Win32++\Frame.h"
#include "FrameApp.h"
#include "MainFrm.h"

#include <tchar.h>
#include <windowsx.h>

#ifndef HDF_SORTUP
#define HDF_SORTUP				0x0400
#endif
#ifndef HDF_SORTDOWN
#define HDF_SORTDOWN			0x0200
#endif

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL			0x020A
#endif

CRightView::CRightView()
{
	m_BootList        = 0;
	m_Space           = false;
	m_Return          = false;
	m_Delete          = false;
	m_bAscending      = true;
	m_iLastColumn     = -1;
	
	m_bLabelTipOpen   = false;
	m_iActiveItem     = -1;
	m_iActiveSubItem  = -1;
	m_strTip          = _T("");
}

CRightView::~CRightView()
{
	ImageList_Destroy(ListView_GetImageList(GetHwnd(), LVSIL_SMALL));
	ImageList_Destroy(ListView_GetImageList(GetHwnd(), LVSIL_NORMAL));
}

Boot::WorkItem CRightView::GetItem(int pos /*= -1*/)
{
	tstring name    = _T("");
	tstring command = _T("");
	tstring section = _T("");

	if (pos <= -1)
		pos = GetSelPos();

	if (pos > -1)
	{
		name = GetItemStr(pos);
		tstring flags = GetItemStr(pos, 2);
		command = GetItemStr(pos, 3);
		if (flags.length() > 0)
			command = _T("\"") + command + _T("\" ") + flags;
		section = GetItemStr(pos, 1);
	}

	Boot::WorkItem wItem(command, name, section);
	return wItem;
}

void CRightView::GetSortInfo(int &columnIndex, bool &isAscending)
{
	columnIndex = m_iLastColumn;
	isAscending = m_bAscending;
}

void CRightView::Sort(int columnIndex /*= -1*/, bool isAscending /*= true*/)
{
	if (columnIndex < 0)
	{
		SetHeaderSortImage(-1);
		return;
	}

	m_iLastColumn = columnIndex;
	m_bAscending  = isAscending;
	SetHeaderSortImage(m_iLastColumn, m_bAscending);

	m_SortData.hWndList   = GetHwnd();
	m_SortData.iSubItem   = m_iLastColumn;
	m_SortData.bAscending = m_bAscending;
	ListView_SortItemsEx(GetHwnd(), CompareFunc, &m_SortData);
}

//add an item with an icon as image to the listview
void CRightView::AddItem(LPCTSTR pszText, int iItem, int iSubItem, HICON iIconLarge, HICON iIconSmall)
{
	if (!iIconLarge && !iIconSmall)
		CListView::AddItem(pszText, iItem, iSubItem);
	else
	{
		HIMAGELIST hImgLarge = ListView_GetImageList(GetHwnd(), LVSIL_NORMAL);
		HIMAGELIST hImgSmall = ListView_GetImageList(GetHwnd(), LVSIL_SMALL);

		ImageList_AddIcon(hImgLarge, iIconLarge ? iIconLarge : iIconSmall);
		int iIndex = ImageList_AddIcon(hImgSmall, iIconSmall ? iIconSmall : iIconLarge);

		CListView::AddItem(pszText, iItem, iSubItem, iIndex);
	}
}

void CRightView::UpdateColumns()
{
	DWORD view = ::GetWindowLongPtr(GetHwnd(), GWL_STYLE);

	if ((int)(view & LVS_TYPEMASK) != DETAIL)
		return;

	Sort(m_iLastColumn, m_bAscending);

	if (ListView_GetItemCount(GetHwnd()) > 0)
	{
		ListView_SetColumnWidth(GetHwnd(), 0, LVSCW_AUTOSIZE);
		ListView_SetColumnWidth(GetHwnd(), 1, LVSCW_AUTOSIZE);
		ListView_SetColumnWidth(GetHwnd(), 2, LVSCW_AUTOSIZE_USEHEADER);
		ListView_SetColumnWidth(GetHwnd(), 3, LVSCW_AUTOSIZE);

		if (ListView_GetColumnWidth(GetHwnd(), 0) > 180)
			ListView_SetColumnWidth(GetHwnd(), 0, 180);

		if (ListView_GetColumnWidth(GetHwnd(), 2) > 100)
			ListView_SetColumnWidth(GetHwnd(), 2, 100);
	}
	else
	{
		ListView_SetColumnWidth(GetHwnd(), 0, LVSCW_AUTOSIZE_USEHEADER);
		ListView_SetColumnWidth(GetHwnd(), 1, LVSCW_AUTOSIZE_USEHEADER);
		ListView_SetColumnWidth(GetHwnd(), 2, LVSCW_AUTOSIZE_USEHEADER);
		ListView_SetColumnWidth(GetHwnd(), 3, LVSCW_AUTOSIZE_USEHEADER);
	}
}

void CRightView::SectionToList(Section *section)
{
	Item *item;
	section->SelFirst();
	for (int j=0; j<section->Count(); j++)
	{
		item = section->GetNode();
		HICON largeicon = 0, smallicon = 0;
		if (item->GetAttributes() & Item::attrErrCorrupt)
			largeicon = ::LoadIcon(GetApp()->GetInstanceHandle(), MAKEINTRESOURCE(IDI_INVALID));
		else
		{
			largeicon = item->GetIcon();
			smallicon = item->GetSmallIcon();
		}
		AddItem(item->GetiName().c_str(), 0, 0, largeicon, smallicon);
		if (largeicon)
			::DestroyIcon(largeicon);
		if (smallicon)
			::DestroyIcon(smallicon);
		AddItem(section->GetSection().c_str(), 0, 1);
		AddItem(item->GetFlags().c_str(), 0, 2);
		AddItem(item->GetPath().c_str(), 0, 3);
		ListView_SetCheckState(GetHwnd(), 0, item->IsEnabled() ? 1 : 0);
		section->SelNext();
	}
}

//write data from specified section to m_RightView
void CRightView::DataToList(const tstring &section)
{
	Clear();
	
	//turn redawing off in the ListView. This will speed things up as we add items
	::SendMessage(GetHwnd(), WM_SETREDRAW, FALSE, 0);

	Boot::Core::const_iterator iter = m_BootList->begin();
	if (section == _T("Main"))
	{
		//all sections
		for (; iter != m_BootList->end(); ++iter)
			SectionToList(*iter);
	}
	else if ((section == _T("StartUp")) || (section == _T("WinINI")) || (section == _T("Registry"))
			 || (section == _T("Registry\\Current User")) || (section == _T("Registry\\All Users"))
			 || (section == _T("Registry\\Computer")))
	{
		//search the right section in the list
		for (; iter != m_BootList->end(); ++iter)
			if ((*iter)->GetSection().find(section) != std::string::npos)
				SectionToList(*iter);
	}
	else
	{
		//search the right section in the list
		for (; iter != m_BootList->end(); ++iter)
			if ((*iter)->GetSection() == section)
				SectionToList(*iter);
	}

	UpdateColumns();
	
	//turn redrawing back on
	::SendMessage(GetHwnd(), WM_SETREDRAW, TRUE, 0);
}

void CRightView::OnInitialUpdate()
{
	//set listview to have gridlines and checkboxes
	SetExStyle(LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);

	//init the listview to have images
	HIMAGELIST hImg;
	hImg = ImageList_Create(16, 16, ILC_COLOR16, 1, 0);
	::SendMessage(GetHwnd(), LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM)hImg);
	hImg = ImageList_Create(32, 32, ILC_COLOR32, 1, 0);
	::SendMessage(GetHwnd(), LVM_SETIMAGELIST, LVSIL_NORMAL, (LPARAM)hImg);

	//set the listview columns
	InsertColumn(LoadString(STR_COL1), 0, 0);
	InsertColumn(LoadString(STR_COL2), 0, 1);
	InsertColumn(LoadString(STR_COL3), 0, 2);
	InsertColumn(LoadString(STR_COL4), 0, 3);

	//populate listview
	DataToList(_T("Main"));

	//create labeltip control
	m_LabelTip.Create(GetHwnd());
}

// This function was based on the winapizone tutorial called "How to display the sort 
// order in the listview columns", the tutorial itself can be found on the following
// location: http://www.winapizone.net/tutorials/winapi/listview/columnsortimage.php
void CRightView::SetHeaderSortImage(int columnIndex, bool isAscending /*= true*/)
{
	HWND header = ListView_GetHeader(GetHwnd());
	BOOL isCommonControlVersion6 = (GetComCtlVersion() >= 600);

	int columnCount = Header_GetItemCount(header);
	for (int i = 0; i<columnCount; i++)
	{
		HDITEM hi = {0};
        
		//I only need to retrieve the format if i'm on
		//windows xp. If not, then i need to retrieve
		//the bitmap also.
		hi.mask = HDI_FORMAT | (isCommonControlVersion6 ? 0 : HDI_BITMAP);
        
		Header_GetItem(header, i, &hi);
        
		//Set sort image to this column
		if (i == columnIndex)
		{
			//Windows xp has a easier way to show the sort order
			//in the header: i just have to set a flag and windows
			//will do the drawing. No windows xp, no easy way.
			if (isCommonControlVersion6)
			{
				hi.fmt &= ~(HDF_SORTDOWN | HDF_SORTUP);
				hi.fmt |= isAscending ? HDF_SORTUP : HDF_SORTDOWN;
			}
			else
			{
				UINT bitmapID = isAscending ? IDI_UPARROW : IDI_DOWNARROW;
        
				//If there's a bitmap, let's delete it.
				if (hi.hbm)
					::DeleteObject(hi.hbm);
                
				hi.fmt |= HDF_BITMAP | HDF_BITMAP_ON_RIGHT;
				hi.hbm = (HBITMAP)::LoadImage(GetApp()->GetInstanceHandle(), MAKEINTRESOURCE(bitmapID), IMAGE_BITMAP, 0,0, LR_LOADMAP3DCOLORS);
			}
		}
		//Remove sort image (if exists) from other columns.
		else
		{
			if (isCommonControlVersion6)
				hi.fmt &= ~(HDF_SORTDOWN | HDF_SORTUP);
			else
			{
				//If there's a bitmap, let's delete it.
				if (hi.hbm)
					::DeleteObject(hi.hbm);
                
				//Remove flags that tell windows to look for a bitmap.
				hi.mask &= ~HDI_BITMAP;
				hi.fmt &= ~(HDF_BITMAP | HDF_BITMAP_ON_RIGHT);
			}
		}
        
		Header_SetItem(header, i, &hi);
	}
}

int CALLBACK CRightView::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	SORTDATA* const SortData = reinterpret_cast<SORTDATA *>(lParamSort);
	
	TCHAR szBuf1[MAX_PATH], szBuf2[MAX_PATH];
 
	//Get the text of the two items. Note that buffer size is in characters, not bytes.
	ListView_GetItemText(SortData->hWndList, lParam1, SortData->iSubItem, szBuf1, sizeof(szBuf1) / sizeof(szBuf1[0]));
	ListView_GetItemText(SortData->hWndList, lParam2, SortData->iSubItem, szBuf2, sizeof(szBuf2) / sizeof(szBuf2[0]));

	return (_tcsicmp(szBuf1, szBuf2) * (SortData->bAscending ? 1 : -1));
}

//a header was clicked
void CRightView::OnColumnClick(LPNMLISTVIEW nmlv)
{
	if (m_iLastColumn == nmlv->iSubItem)
		m_bAscending = !m_bAscending;
	else
		m_bAscending = true;

	m_iLastColumn = nmlv->iSubItem;
	Sort(m_iLastColumn, m_bAscending);
}

//listview checkbox notification
void CRightView::OnCheckbox(int iItem, bool SpaceBar)
{
	try
	{
		Boot::WorkItem wItem(GetItem(iItem));
		Item *pbtItem = m_BootList->Get(wItem);

		if (!pbtItem)
		{
			ListView_SetCheckState(GetHwnd(), iItem, 1-ListView_GetCheckState(GetHwnd(), iItem));
			throw (Boot::Exception(_T("CRightView::OnCheckbox... Invalid item pointer")));
		}

		if (pbtItem->GetAttributes() & Item::attrSystem)
		{
			tstring msg_question = LoadString(MSG_QUESTION);
			if (::MessageBox(GetHwnd(), LoadString(MSG_DISABLESYSFILE), msg_question.c_str(), MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) == IDNO)
			{
				ListView_SetCheckState(GetHwnd(), iItem, 1-ListView_GetCheckState(GetHwnd(), iItem));
				return;
			}
		}

		try
		{
			if (ListView_GetCheckState(GetHwnd(), iItem) == 0)
			{
				if (!SpaceBar)
					m_BootList->Enable(wItem);
				else
					m_BootList->Disable(wItem);
			}
			else
			{
				if (!SpaceBar)
					m_BootList->Disable(wItem);
				else
					m_BootList->Enable(wItem);
			}
		}

		catch (Boot::Exception const& e)
		{
			ListView_SetCheckState(GetHwnd(), iItem, 1-ListView_GetCheckState(GetHwnd(), iItem));
			
			tstring msg_information = LoadString(MSG_INFORMATION);
			if (pbtItem->GetAttributes() & Item::attrErrCorrupt)
				::MessageBox(GetHwnd(), (LoadString(MSG_DISABLECORRUPTED) + (_T("\n\t") + e.GetMsg())).c_str(), msg_information.c_str(), MB_OK | MB_ICONINFORMATION);
			else
				::MessageBox(GetHwnd(), (LoadString(MSG_DISABLEFAILED) + (_T("\n\t") + e.GetMsg())).c_str(), msg_information.c_str(), MB_OK | MB_ICONINFORMATION);
		}

		catch (...)
		{
			ListView_SetCheckState(GetHwnd(), iItem, 1-ListView_GetCheckState(GetHwnd(), iItem));
			
			tstring msg_information = LoadString(MSG_INFORMATION);
			if (pbtItem->GetAttributes() & Item::attrErrCorrupt)
				::MessageBox(GetHwnd(), LoadString(MSG_DISABLECORRUPTED), msg_information.c_str(), MB_OK | MB_ICONINFORMATION);
			else
				::MessageBox(GetHwnd(), LoadString(MSG_DISABLEFAILED), msg_information.c_str(), MB_OK | MB_ICONINFORMATION);
		}
	}
	
	catch (Boot::Exception const& e)
	{
		tstring msg_information = LoadString(MSG_INFORMATION);
		::MessageBox(GetHwnd(), (LoadString(MSG_DISABLEFAILED) + (_T("\n\t") + e.GetMsg())).c_str(), msg_information.c_str(), MB_OK | MB_ICONWARNING);
		ListView_SetCheckState(GetHwnd(), iItem, 1-ListView_GetCheckState(GetHwnd(), iItem));
	}
	
	catch (...)
	{
		DebugErrMsg(_T("CRightView::OnCheckbox... Unknown error"));
		ListView_SetCheckState(GetHwnd(), iItem, 1-ListView_GetCheckState(GetHwnd(), iItem));
	}
}

//handle listview click message
void CRightView::OnClick(LPARAM lParam)
{
	LPNMITEMACTIVATE nmItem = (LPNMITEMACTIVATE)lParam;
	if (nmItem->iSubItem == 0)
	{
		LVHITTESTINFO info;
		ZeroMemory(&info, sizeof(LVHITTESTINFO));
		info.iItem    = nmItem->iItem;
		info.iSubItem = 0;
		info.pt       = nmItem->ptAction;
		ListView_HitTest(GetHwnd(), &info);
		if (info.flags == LVHT_ONITEMSTATEICON)
			OnCheckbox(nmItem->iItem);
	}
}

//handle listview double click message
void CRightView::OnDblClick()
{
//	CMainFrame* pMainFrame = (CMainFrame*)GetApp()->GetFrame();
//	HWND hwndFrame = pMainFrame->GetHwnd();
	CMainFrame& MainFrame = GetStartupMgrApp().GetMainFrame();
	HWND hwndFrame = MainFrame.GetHwnd();
	
	if (GetSelPos() != -1)
		FORWARD_WM_COMMAND(hwndFrame, IDM_EDIT_PROPERTIES, 0, 0, SendMessage);
	else
		FORWARD_WM_COMMAND(hwndFrame, IDM_FILE_ADD, 0, 0, SendMessage);
}

//handle listview rightclick message (show context menu)
void CRightView::OnRightClick()
{
//	CMainFrame* pMainFrame = (CMainFrame*)GetApp()->GetFrame();
//	HWND hwndFrame = pMainFrame->GetHwnd();
	CMainFrame& MainFrame = GetStartupMgrApp().GetMainFrame();
	HWND hwndFrame = MainFrame.GetHwnd();

	POINT CurPos;
	::GetCursorPos(&CurPos);

	//create popup menu
	HMENU hPopupMenu = ::CreatePopupMenu();

	//different menu if item selected
	if (GetSelPos() != -1)
	{
		::InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDM_EDIT_PROPERTIES, LoadString(STR_CONTEXT1));
		::InsertMenu(hPopupMenu, 1, MF_BYPOSITION | MF_STRING, IDM_EDIT_FILEPROPERTIES, LoadString(STR_CONTEXT8));
		::InsertMenu(hPopupMenu, 2, MF_BYPOSITION | MF_STRING, IDM_EDIT_RUN, LoadString(STR_CONTEXT7));
		::InsertMenu(hPopupMenu, 3, MF_SEPARATOR, 0, 0);
		::InsertMenu(hPopupMenu, 4, MF_BYPOSITION | MF_STRING, IDM_EDIT_RENAME, LoadString(STR_CONTEXT2));
		::InsertMenu(hPopupMenu, 5, MF_BYPOSITION | MF_STRING, IDM_EDIT_DELETE, LoadString(STR_CONTEXT3));
		::SetMenuDefaultItem(hPopupMenu, 0, IDM_EDIT_PROPERTIES);

		tstring section = GetItemStr(GetSelPos(), 1);
		//win.ini and winlogon don't support item names
		if ((section ==  _T("WinINI\\Load")) || (section == _T("WinINI\\Run")) ||
			(section == _T("Registry\\Computer\\Shell")) || (section == _T("Registry\\Computer\\Userinit")))
			::EnableMenuItem(hPopupMenu, IDM_EDIT_RENAME, MF_BYCOMMAND | MF_GRAYED);

		if ((ListView_GetCheckState(GetHwnd(), GetSelPos()) == 0) && (section.find(_T("BHO")) == std::string::npos))
			::EnableMenuItem(hPopupMenu, IDM_EDIT_RENAME, MF_BYCOMMAND | MF_GRAYED);

		if(_tcscmp(GetItemStr(GetSelPos(), 3), _T("")) == 0)
			::EnableMenuItem(hPopupMenu, IDM_EDIT_FILEPROPERTIES, MF_BYCOMMAND | MF_GRAYED);
	}
	else
	{
		::InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDM_FILE_ADD, LoadString(STR_CONTEXT4));
		::InsertMenu(hPopupMenu, 1, MF_BYPOSITION | MF_STRING, IDM_FILE_BTNEW, LoadString(STR_CONTEXT5));
		::InsertMenu(hPopupMenu, 2, MF_SEPARATOR, 0, 0);
		::InsertMenu(hPopupMenu, 3, MF_BYPOSITION | MF_STRING, IDM_VIEW_REFRESH, LoadString(STR_CONTEXT6));
		::SetMenuDefaultItem(hPopupMenu, 0, IDM_FILE_ADD);
	}

	::SetForegroundWindow(GetHwnd());
	::TrackPopupMenu(hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN, CurPos.x, CurPos.y, 0, hwndFrame, 0);
	::DestroyMenu(hPopupMenu);
}

UINT CRightView::OnCustomDraw(LPNMLVCUSTOMDRAW lplvCustomDraw)
{
	DWORD view = ::GetWindowLongPtr(GetHwnd(), GWL_STYLE);
	if ((int)(view & LVS_TYPEMASK) != DETAIL)
		return CDRF_DODEFAULT;

	switch (lplvCustomDraw->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		return (CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW);

	case CDDS_ITEMPREPAINT:
		if ((((int)lplvCustomDraw->nmcd.dwItemSpec) % 2) == 0)
		{
			lplvCustomDraw->clrText   = CLR_DEFAULT;
			lplvCustomDraw->clrTextBk = RGB(250,251,254);
			return CDRF_NEWFONT;
		}
		break;
	}

	return CDRF_DODEFAULT;
}

LRESULT CRightView::OnNotifyReflect(WPARAM /*wParam*/, LPARAM lParam)
{
//	CMainFrame* pMainFrame = (CMainFrame*)GetApp()->GetFrame();
	CMainFrame& MainFrame = GetStartupMgrApp().GetMainFrame();
	HMENU hMenu = MainFrame.GetFrameMenu();
	CToolbar& Toolbar = MainFrame.GetToolbar();

	if (GetSelPos() != -1)
	{
		//item selected -> enable toolbar and menu items
		Toolbar.EnableButton(IDM_EDIT_DELETE);
		Toolbar.EnableButton(IDM_EDIT_RUN);
		Toolbar.EnableButton(IDM_EDIT_PROPERTIES);
		::EnableMenuItem(::GetSubMenu(hMenu, 1), IDM_EDIT_DELETE, MF_BYCOMMAND | MF_ENABLED);
		::EnableMenuItem(::GetSubMenu(hMenu, 1), IDM_EDIT_RUN, MF_BYCOMMAND | MF_ENABLED);
		::EnableMenuItem(::GetSubMenu(hMenu, 1), IDM_EDIT_PROPERTIES, MF_BYCOMMAND | MF_ENABLED);

		tstring section = GetItemStr(GetSelPos(), 1);
		//win.ini and winlogon don't support item names
		if (!((section ==  _T("WinINI\\Load")) || (section == _T("WinINI\\Run")) ||
			(section == _T("Registry\\Computer\\Shell")) || (section == _T("Registry\\Computer\\Userinit"))))
		{
			Toolbar.EnableButton(IDM_EDIT_RENAME);
			::EnableMenuItem(::GetSubMenu(hMenu, 1), IDM_EDIT_RENAME, MF_BYCOMMAND | MF_ENABLED);
		}
		else
		{
			Toolbar.DisableButton(IDM_EDIT_RENAME);
			::EnableMenuItem(::GetSubMenu(hMenu, 1), IDM_EDIT_RENAME, MF_BYCOMMAND | MF_GRAYED);
		}

		if ((ListView_GetCheckState(GetHwnd(), GetSelPos()) == 0) && (section.find(_T("BHO")) == std::string::npos))
		{
			Toolbar.DisableButton(IDM_EDIT_RENAME);
			::EnableMenuItem(::GetSubMenu(hMenu, 1), IDM_EDIT_RENAME, MF_BYCOMMAND | MF_GRAYED);
		}
	}
	else if (ListView_GetSelectedCount(GetHwnd()) > 1)
	{
		//multiple items selected -> disable toolbar and menu items except delete
		Toolbar.EnableButton(IDM_EDIT_DELETE);
		Toolbar.DisableButton(IDM_EDIT_RENAME);
		Toolbar.DisableButton(IDM_EDIT_RUN);
		Toolbar.DisableButton(IDM_EDIT_PROPERTIES);
		::EnableMenuItem(::GetSubMenu(hMenu, 1), IDM_EDIT_DELETE, MF_BYCOMMAND | MF_ENABLED);
		::EnableMenuItem(::GetSubMenu(hMenu, 1), IDM_EDIT_RENAME, MF_BYCOMMAND | MF_GRAYED);
		::EnableMenuItem(::GetSubMenu(hMenu, 1), IDM_EDIT_RUN, MF_BYCOMMAND | MF_GRAYED);
		::EnableMenuItem(::GetSubMenu(hMenu, 1), IDM_EDIT_PROPERTIES, MF_BYCOMMAND | MF_GRAYED);
	}
	else
	{
		//no item selected -> disable toolbar and menu items
		Toolbar.DisableButton(IDM_EDIT_DELETE);
		Toolbar.DisableButton(IDM_EDIT_RENAME);
		Toolbar.DisableButton(IDM_EDIT_RUN);
		Toolbar.DisableButton(IDM_EDIT_PROPERTIES);
		::EnableMenuItem(::GetSubMenu(hMenu, 1), IDM_EDIT_DELETE, MF_BYCOMMAND | MF_GRAYED);
		::EnableMenuItem(::GetSubMenu(hMenu, 1), IDM_EDIT_RENAME, MF_BYCOMMAND | MF_GRAYED);
		::EnableMenuItem(::GetSubMenu(hMenu, 1), IDM_EDIT_RUN, MF_BYCOMMAND | MF_GRAYED);
		::EnableMenuItem(::GetSubMenu(hMenu, 1), IDM_EDIT_PROPERTIES, MF_BYCOMMAND | MF_GRAYED);
	}

	switch (((LPNMHDR)lParam)->code)
	{
	case LVN_COLUMNCLICK: OnColumnClick(reinterpret_cast<LPNMLISTVIEW>(lParam)); break;
	case NM_CLICK: OnClick(lParam); break;
	case NM_DBLCLK: OnDblClick(); break;
	case NM_RCLICK: OnRightClick(); break;
	case NM_CUSTOMDRAW: return OnCustomDraw(reinterpret_cast<LPNMLVCUSTOMDRAW>(lParam));
	} //switch(((LPNMHDR)lParam)->code)

	return 0L;
}

int CRightView::GetMaxTipWidth()
{
	RECT rcItem;
	ListView_GetSubItemRect(GetHwnd(), m_iActiveItem, m_iActiveSubItem, LVIR_LABEL, &rcItem);
	POINT ptItem;
	ptItem.x = rcItem.left;
	ptItem.y = rcItem.top;
	::ClientToScreen(GetHwnd(), &ptItem);

	RECT rcDesktop;
	::GetWindowRect(::GetDesktopWindow(), &rcDesktop);
	
	return (rcDesktop.right - ptItem.x);
}

void CRightView::OnTooltipNeedTextA(LPNMTTDISPINFOA pDispInfo)
{
	m_LabelTip.SetMaxTipWidth(GetMaxTipWidth());

	m_strANSITip = toNarrowString(m_strTip);
	pDispInfo->lpszText = const_cast<char*>(m_strANSITip.c_str());
}

void CRightView::OnTooltipNeedTextW(LPNMTTDISPINFOW pDispInfo)
{
	m_LabelTip.SetMaxTipWidth(GetMaxTipWidth());

	m_strUNICODETip = toWideString(m_strTip);
	pDispInfo->lpszText = const_cast<wchar_t*>(m_strUNICODETip.c_str());
}

void CRightView::OnTooltipShow()
{
	m_bLabelTipOpen = true;
}

void CRightView::OnTooltipPop()
{
	// Don't fade or animate tooltip window, just hide it
	m_LabelTip.Hide();
	m_bLabelTipOpen = false;
}

void CRightView::OnMouseMove(LPARAM lParam)
{
	LVHITTESTINFO info;
	::ZeroMemory(&info, sizeof(LVHITTESTINFO));
	info.pt.x = LOWORD(lParam);
	info.pt.y = HIWORD(lParam);
	if (ListView_SubItemHitTest(GetHwnd(), &info) == -1)
	{
		m_LabelTip.TrackActivate(FALSE);
		return;
	}

	if (m_bLabelTipOpen && (m_iActiveItem == info.iItem) && (m_iActiveSubItem == info.iSubItem))
		return;
	
	m_LabelTip.TrackActivate(FALSE);

	m_iActiveItem    = info.iItem;
	m_iActiveSubItem = info.iSubItem;

	TRACKMOUSEEVENT tme;
	::ZeroMemory(&tme, sizeof(TRACKMOUSEEVENT));
	tme.cbSize      = sizeof(tme);
	tme.dwFlags     = TME_LEAVE;
	tme.dwHoverTime = 0;
	tme.hwndTrack   = GetHwnd();
	::_TrackMouseEvent(&tme);

	RECT rcItem;
	ListView_GetSubItemRect(GetHwnd(), m_iActiveItem, m_iActiveSubItem, LVIR_LABEL, &rcItem);

	RECT view;
	::GetClientRect(GetHwnd(), &view);
	tstring buffer = GetItemStr(m_iActiveItem, m_iActiveSubItem);
	int strWidth = ListView_GetStringWidth(GetHwnd(), buffer.c_str());
	if ((strWidth < (rcItem.right - rcItem.left)) && ((rcItem.left + strWidth) < (view.right - 2)))
		return;
	m_strTip = buffer;

	POINT ttPos;
	ttPos.x = rcItem.left;
	ttPos.y = rcItem.top;
	::ClientToScreen(GetHwnd(), &ttPos);

	if (m_iActiveSubItem == 0)
		ttPos.x -= 1;
	else
		ttPos.x += 3;

	m_LabelTip.TrackPosition(ttPos);
	m_LabelTip.TrackActivate();
}

void CRightView::OnMouseLeave()
{
	m_LabelTip.TrackActivate(FALSE);
}

void CRightView::OnMouseWheel()
{
	m_LabelTip.TrackActivate(FALSE);
	m_iActiveItem    = -1;
	m_iActiveSubItem = -1;
}

LRESULT CRightView::OnToolTipNotify(LPNMHDR pnmh)
{
	// Only show label tips if listview has LVS_REPORT style
	DWORD view = ::GetWindowLongPtr(GetHwnd(), GWL_STYLE);
	if ((int)(view & LVS_TYPEMASK) != DETAIL)
		return 0L;

	switch (pnmh->code)
	{
	case TTN_NEEDTEXTA:
		OnTooltipNeedTextA(reinterpret_cast<LPNMTTDISPINFOA>(pnmh));
		break;

	case TTN_NEEDTEXTW:
		OnTooltipNeedTextW(reinterpret_cast<LPNMTTDISPINFOW>(pnmh));
		break;

	case TTN_SHOW:
		OnTooltipShow();
		break;

	case TTN_POP:
		OnTooltipPop();
		break;
	}

	return 0L;
}

LRESULT CRightView::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
//	CMainFrame* pMainFrame = (CMainFrame*)GetApp()->GetFrame();
//	HWND hwndFrame = pMainFrame->GetHwnd();
	CMainFrame& MainFrame = GetStartupMgrApp().GetMainFrame();
	HWND hwndFrame = MainFrame.GetHwnd();

	switch (uMsg)
	{
	case WM_KEYDOWN:
		//WM_KEYDOWN messages
		switch (wParam)
		{
		case VK_SPACE: m_Space = true; break;
		case VK_RETURN: m_Return = true; break;
		case VK_DELETE: m_Delete = true; break;
		}
		break;
		
	case WM_KEYUP:
		//WM_KEYUP messages
		switch (wParam)
		{
		case VK_SPACE:
			if (m_Space)
			{
				if (GetSelPos() != -1)
					OnCheckbox(GetSelPos(), true);
				m_Space = false;
			}
			break;

		case VK_RETURN:
			if (m_Return)
			{
				OnDblClick();
				m_Return = false;
			}
			break;

		case VK_DELETE:
			if (m_Delete)
			{
				if (ListView_GetSelectedCount(GetHwnd()) > 0)
					FORWARD_WM_COMMAND(hwndFrame, IDM_EDIT_DELETE, 0, 0, SendMessage);
				m_Delete = false;
			}
			break;
		} //switch (wParam)
		break;

	case WM_MOUSEMOVE:
		OnMouseMove(lParam);
		break;

	case WM_MOUSELEAVE:
		OnMouseLeave();
		break;

	case WM_MOUSEWHEEL:
		OnMouseWheel();
		break;

	case WM_SIZE:
		{
			RECT rcThis;
			::GetClientRect(GetHwnd(), &rcThis);
			m_LabelTip.UpdateRect(&rcThis);
		}
		break;

	case WM_NOTIFY:
		{
			LPNMHDR pnmh = reinterpret_cast<LPNMHDR>(lParam);
			if (pnmh->hwndFrom == m_LabelTip.GetHwnd())
				return OnToolTipNotify(pnmh);
		}
		break;
	} //switch (uMsg)

	// Pass unhandled messages on to the default window procedure
	return WndProcDefault(hWnd, uMsg, wParam, lParam);
} //LRESULT CRightView::WndProc(...)
