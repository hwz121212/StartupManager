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
 
#ifndef RIGHTVIEW_H
#define RIGHTVIEW_H


#include "ListView.h"
#include "TitleTip.h"
#include "boot\btcore.h"

#include <shlwapi.h>

using namespace Boot;

class CRightView : public CListView
{
public:
    CRightView();
    virtual ~CRightView();
	Boot::WorkItem GetItem(int pos = -1);
	void GetSortInfo(int &columnIndex, bool &isAscending);
	void Sort(int columnIndex = -1, bool isAscending = true);
    void AddItem(LPCTSTR pszText, int iItem, int iSubItem, HICON iIconLarge = 0, HICON iIconSmall = 0);
    void UpdateColumns();
    void DataToList(const tstring &section);
    void SetCoreObj(Boot::Core *List) {m_BootList = List;}
	virtual LRESULT OnNotifyReflect(WPARAM wParam, LPARAM lParam);
	void OnRightClick();
    
protected:
    virtual void OnInitialUpdate();
	void SetHeaderSortImage(int columnIndex, bool isAscending = true);
	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
    void OnColumnClick(LPNMLISTVIEW nmlv);
	void OnClick(LPARAM lParam);
	void OnCheckbox(int iItem, bool SpaceBar = false);
	void OnDblClick();
	UINT OnCustomDraw(LPNMLVCUSTOMDRAW lplvCustomDraw);
	void OnTooltipNeedTextA(LPNMTTDISPINFOA pDispInfo);
	void OnTooltipNeedTextW(LPNMTTDISPINFOW pDispInfo);
	void OnTooltipShow();
	void OnTooltipPop();
	void OnMouseMove(LPARAM lParam);
	void OnMouseLeave();
	void OnMouseWheel();
	virtual LRESULT OnToolTipNotify(LPNMHDR pnmh);
    virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    
private:
	int GetMaxTipWidth();
    void SectionToList(Section *section);
    
	struct SORTDATA
	{
		HWND hWndList;
		int iSubItem;
		BOOL bAscending;
	};

    Boot::Core *m_BootList;
	bool m_Space;
	bool m_Return;
	bool m_Delete;
	bool m_bAscending;
	int m_iLastColumn;
	SORTDATA m_SortData;

	CTitleTip m_LabelTip;
	bool m_bLabelTipOpen;
	int m_iActiveItem;
	int m_iActiveSubItem;
	tstring m_strTip;
	std::string m_strANSITip;
	std::wstring m_strUNICODETip;
};

#endif // RIGHTVIEW_H
