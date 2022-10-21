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

#include "ListView.h"

#include <tchar.h>

void CListView::PreCreate(CREATESTRUCT &cs)
{
	cs.style = WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT;
	cs.dwExStyle = WS_EX_CLIENTEDGE;
	cs.lpszClass = _T("SysListView32");
}

void CListView::InsertColumn(LPCTSTR Title, int width, int iSubItem)
{
	LVCOLUMN column;

	::ZeroMemory(&column, sizeof(LVCOLUMN));
	column.mask       = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	column.fmt        = LVCFMT_LEFT;
	column.pszText    = const_cast<LPTSTR>(Title);
	column.cchTextMax = static_cast<int>(_tcslen(Title));
	column.cx         = width;
	column.iSubItem   = iSubItem;

	ListView_InsertColumn(GetHwnd(), iSubItem, &column);
}

void CListView::AddItem(LPCTSTR pszText, int iItem, int iSubItem, int iImage /*= -1*/)
{
	try
	{
		LVITEM lvItem;

		::ZeroMemory(&lvItem, sizeof(LVITEM));
		lvItem.mask	      = LVIF_TEXT;
		lvItem.iItem      = iItem;
		lvItem.iSubItem   = iSubItem;
		lvItem.pszText    = const_cast<LPTSTR>(pszText);
		lvItem.cchTextMax = static_cast<int>(_tcslen(pszText));

		if (iImage != -1)
		{
			lvItem.mask	 |= LVIF_IMAGE;
			lvItem.iImage = iImage;
		}

		if (iSubItem == 0)
		{
			if (ListView_InsertItem(GetHwnd(), &lvItem) == -1)
				throw (CWinException(_T("CListView::AddItem ... LVM_INSERTITEM failed ")));
		}
		else
		{
			if (ListView_SetItem(GetHwnd(), &lvItem) == -1)
				throw (CWinException(_T("CListView::AddItem ... LVM_SETITEM failed ")));
		}
	}

	catch (CWinException &e)
	{
		e.MessageBox();
	}

	catch (...)
	{
	#ifdef _DEBUG
		::MessageBox(m_hWndParent, _T("Unknown error while adding item to listview"), _T("Exception"), MB_OK);
	#endif
	}
}

void CListView::Delete(int iItem)
{
	ListView_DeleteItem(GetHwnd(), iItem);
}

void CListView::Select(int iItem)
{
	::SetFocus(GetHwnd());
	ListView_SetItemState(GetHwnd(), iItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
}

void CListView::SetExStyle(DWORD exStyle)
{
	DWORD currentStyle = (DWORD)::SendMessage(GetHwnd(), LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
	if (!(currentStyle & exStyle))
		::SendMessage(GetHwnd(), LVM_SETEXTENDEDLISTVIEWSTYLE, 0, exStyle);
}

void CListView::SetIconStyle(LVStyles style)
{
	DWORD view = ::GetWindowLongPtr(GetHwnd(), GWL_STYLE);

	if (static_cast<int>(view & LVS_TYPEMASK) != style)
	{
		::SetWindowLongPtr(GetHwnd(), GWL_STYLE, (view & ~LVS_TYPEMASK) | style);
	}
}

LPCTSTR CListView::GetItemStr(int iItem, int iSubItem /*=0*/)
{
	try
	{
		LVITEM lvItem;
		LPCTSTR pszText;

		::ZeroMemory(&lvItem, sizeof(LVITEM));
		lvItem.mask       = LVIF_TEXT;
		lvItem.iItem      = iItem;
		lvItem.iSubItem   = iSubItem;
		lvItem.pszText    = m_Buffer;
		lvItem.cchTextMax = 256;

		if (::SendMessage(GetHwnd(), LVM_GETITEMTEXT, iItem, reinterpret_cast<LPARAM>(&lvItem)) == -1)
			throw (CWinException(_T("CListView::GetItemStr ... LVM_GETITEM failed ")));

		pszText = m_Buffer;
		return pszText;
	}

	catch (CWinException &e)
	{
		e.MessageBox();
	}

	catch (...)
	{
	#ifdef _DEBUG
		::MessageBox(m_hWndParent, _T("Unknown error while trying to get an item from the listview"), _T("Exception"), MB_OK);
	#endif
	}

	return _T("");
}

int CListView::GetSelPos()
{
	//we check for just one selected item
	if (ListView_GetSelectedCount(GetHwnd()) != 1)
		return -1;

	//check for item selection
	for (int i = 0; i < ListView_GetItemCount(GetHwnd()); i++)
		if (ListView_GetItemState(GetHwnd(), i, LVIS_FOCUSED) != -1)
			return static_cast<DWORD>(::SendMessage(GetHwnd(), LVM_GETNEXTITEM, static_cast<UINT>(-1), LVNI_FOCUSED));

	//no item selected
	return -1;
}

void CListView::Clear()
{
	ListView_DeleteAllItems(GetHwnd());
}
