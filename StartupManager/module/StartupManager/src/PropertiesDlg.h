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
 
#ifndef PROPDLG_H
#define PROPDLG_H

#include "..\Win32++\dialog.h"
#include "boot\btcore.h"
#include "ToolTip.h"
#include "HyperLink.h"

using namespace Boot;

class PropDlg : public CDialog
{
public:
    PropDlg(Boot::WorkItem const& item, const tstring &flags, HWND hWndParent = 0);
    virtual ~PropDlg();
      
private:
	tstring GetPathFromCommand(const tstring &command);
    void GetInfo();
    void OnSelchange();
    void InitTabCtrl();
    virtual BOOL OnInitDialog();
    virtual LRESULT OnNotify(WPARAM wParam, LPARAM lParam);
	void UpdateBackgroundBrush();
	HBRUSH OnCtlColorStatic(WPARAM wParam, LPARAM lParam);
    virtual BOOL DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
    Boot::WorkItem const& m_File;
    tstring m_Flag;
    File m_Files[2];
	HWND m_hTabCtrl;
	CToolTip m_TT;
	CHyperlink m_Google;
	CHyperlink m_Dir;
	BOOL m_bThemeActive;
	HBRUSH m_hBrush;
};

#endif //PROPDLG_H
