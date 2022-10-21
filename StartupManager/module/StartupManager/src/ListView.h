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

#ifndef LIST_H
#define LIST_H

#include "..\Win32++\WinCore.h"
  
enum LVStyles
{
	LARGE  = LVS_ICON,        //large icons
	SMALL  = LVS_SMALLICON,   //small icons
	DETAIL = LVS_REPORT,      //detailed view
	LIST   = LVS_LIST         //list with the items
};

class CListView : public CWnd
{
public:
	CListView() {}
	virtual ~CListView() {}
	virtual void PreCreate(CREATESTRUCT &cs);
	void InsertColumn(LPCTSTR Title, int width, int iSubItem);
	void AddItem(LPCTSTR pszText, int iItem, int iSubItem, int iImage = -1);
	void Delete(int iItem);
	void Select(int iItem);
	void SetExStyle(DWORD exStyle);
	void SetIconStyle(LVStyles style);
	LPCTSTR GetItemStr(int iItem, int iSubItem = 0);
	int GetSelPos();
	void Clear();

private:
	TCHAR m_Buffer[257];
};

#endif
   
