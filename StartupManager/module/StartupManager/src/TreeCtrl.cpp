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

#include "TreeCtrl.h"

#include <tchar.h>

void CTreeCtrl::PreCreate(CREATESTRUCT &cs)
{
	cs.style = WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | TVS_HASBUTTONS | TVS_HASLINES
		                  | TVS_LINESATROOT | TVS_TRACKSELECT | TVS_FULLROWSELECT;
	cs.dwExStyle = WS_EX_CLIENTEDGE;
	cs.lpszClass = _T("SysTreeView32");
}

HTREEITEM CTreeCtrl::AddRoot(LPCTSTR pszText, int iImage, int iSelImage)
{
	try
	{
		TVINSERTSTRUCT tvInsert;

		ZeroMemory(&tvInsert, sizeof(TVINSERTSTRUCT));
		tvInsert.hParent             = 0;
		tvInsert.hInsertAfter        = TVI_ROOT;
		tvInsert.item.mask           = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		tvInsert.item.pszText        = const_cast<LPTSTR>(pszText);
		tvInsert.item.cchTextMax     = static_cast<int>(_tcslen(pszText));
		tvInsert.item.iImage         = iImage;
		tvInsert.item.iSelectedImage = iSelImage;

		HTREEITEM current = static_cast<HTREEITEM>(TreeView_InsertItem(GetHwnd(), &tvInsert));
		if (!current)
		{
			throw (CWinException(_T("CTreeCtrl::AddRoot ... TVM_INSERTITEM failed ")));
		}

		return current;
	}

	catch (CWinException &e)
	{
		e.MessageBox();
		return 0;
	}

	catch (...)
	{
	#ifdef _DEBUG
		::MessageBox(m_hWndParent, _T("Unknown error while adding item to treecontrol"), _T("Exception"), MB_OK);
	#endif

		return 0;
	}
}

HTREEITEM CTreeCtrl::AddItem(LPCTSTR pszText, int iImage, int iSelImage, HTREEITEM hParent)
{
	try
	{
		TVINSERTSTRUCT tvInsert;

		ZeroMemory(&tvInsert, sizeof(TVINSERTSTRUCT));
		tvInsert.hParent             = hParent;
		tvInsert.hInsertAfter        = TVI_LAST;
		tvInsert.item.mask           = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		tvInsert.item.pszText        = const_cast<LPTSTR>(pszText);
		tvInsert.item.cchTextMax     = static_cast<int>(_tcslen(pszText));
		tvInsert.item.iImage         = iImage;
		tvInsert.item.iSelectedImage = iSelImage;

		HTREEITEM current = static_cast<HTREEITEM>(TreeView_InsertItem(GetHwnd(), &tvInsert));
		if (!current)
		{
			throw (CWinException(_T("CTreeCtrl::AddRoot ... TVM_INSERTITEM failed ")));
		}

		return current;
	}

	catch (CWinException &e)
	{
		e.MessageBox();
		return 0;
	}

	catch (...)
	{
	#ifdef _DEBUG
		::MessageBox(m_hWndParent, _T("Unknown error while adding item to treecontrol"), _T("Exception"), MB_OK);
	#endif

		return 0;
	}
}

LPCTSTR CTreeCtrl::GetSelItem()
{
	HTREEITEM Selected = TreeView_GetSelection(GetHwnd());
	if (Selected != 0)
	{
		LPCTSTR pszText;

		TVITEM tvi;
		ZeroMemory(&tvi, sizeof(TVITEM));
		tvi.mask       = TVIF_TEXT;
		tvi.pszText    = m_Buffer;
		tvi.cchTextMax = 256;
		tvi.hItem      = Selected;
		TreeView_GetItem(GetHwnd(), &tvi);

		pszText = m_Buffer;
		return pszText;
	}

	return _T("");
}

LPCTSTR CTreeCtrl::GetParentText()
{
	HTREEITEM Selected = TreeView_GetSelection(GetHwnd());
	if (Selected != 0)
	{
		HTREEITEM Parent = TreeView_GetParent(GetHwnd(), Selected);
		if (Parent != 0)
		{
			LPCTSTR pszText;

			TVITEM tvi;
			ZeroMemory(&tvi, sizeof(TVITEM));
			tvi.mask       = TVIF_TEXT;
			tvi.pszText    = m_Buffer;
			tvi.cchTextMax = 256;
			tvi.hItem      = Parent;
			TreeView_GetItem(GetHwnd(), &tvi);

			pszText = m_Buffer;
			return pszText;
		}
	}

	return _T("");
}
