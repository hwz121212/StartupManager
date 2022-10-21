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
 
#ifndef TREE_H
#define TREE_H

#include "..\Win32++\WinCore.h" 

class CTreeCtrl : public CWnd
{
public:
	CTreeCtrl() {}
	virtual ~CTreeCtrl() {}
	virtual void PreCreate(CREATESTRUCT &cs);
	inline void SetImageList(HIMAGELIST hImageList);
	HTREEITEM AddRoot(LPCTSTR pszText, int iImage, int iSelImage);
	HTREEITEM AddItem(LPCTSTR pszText, int iImage, int iSelImage, HTREEITEM hParent);
	LPCTSTR GetSelItem();
	LPCTSTR GetParentText();

private:
	TCHAR m_Buffer[257];
};

inline void CTreeCtrl::SetImageList(HIMAGELIST hImageList)
{
	TreeView_SetImageList(GetHwnd(), hImageList, 0);
}

#endif
