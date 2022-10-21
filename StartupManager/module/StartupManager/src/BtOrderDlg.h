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
 
#ifndef BTORDERDLG_H
#define BTORDERDLG_H

#include "..\Win32++\dialog.h"
#include "ListView.h"
#include "boot\btcore.h"
#include "types\tstring.h"
#include "ToolTip.h"
#include "BrowseEdit.h"

using namespace Boot;

class BtOrderDlg : public CDialog
{
public:
    BtOrderDlg(Boot::Core &BootList, BOOL edit, HWND hWndParent = 0);
    virtual ~BtOrderDlg() {}
      
private:
	tstring GetFolder(UINT nID);
    static tstring OnBrowse(HWND hWndParent, LPTSTR typeString, tstring folder = _T(""));
    void ReadBatchFile();
    void OnNew();
    void OnOpen();
    void OnMoveUp();
    void OnMoveDown();
    void OnAdd();
    void OnDelete();
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual LRESULT OnNotify(WPARAM wParam, LPARAM lParam);
    virtual BOOL DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
    Boot::Core &m_BootList;
    BOOL m_bBrowse;
    tstring m_Path;
    tstring m_Section;
	tstring m_BatchLocation;
    bool m_bBrowsed;
	bool m_bIsWinNT;
    CListView m_List;
	CToolTip m_TT;
    
    /**************************************************************************/
    
    //class for batch dialog, visible for BtOrderDlg class only
    class CreateBatchDlg : public CDialog
	{
	public:
    	CreateBatchDlg(Boot::Core &BootList, HWND hWndParent = 0);
    	virtual ~CreateBatchDlg() {}
    	void GetData(tstring &name, tstring &section) const;
      
	private:
    	virtual BOOL OnInitDialog();
    	virtual void OnOK();
		
		Boot::Core &m_BootList;
		tstring m_File;
		tstring m_Place;
		CToolTip m_TT;
	};
	
	/**************************************************************************/
	
	//class for new item dialog, visible for BtOrderDlg class only
	class AddToBatchDlg : public CDialog
	{
	public:
		AddToBatchDlg(HWND hWndParent);
		virtual ~AddToBatchDlg() {}
		tstring GetData() const {return m_Command;}
		
	private:
		virtual BOOL OnInitDialog();
		virtual void OnOK();
		
		tstring m_Command;
		CToolTip m_TT;
		CBrowseEdit m_Browse;
	};
};

#endif //BTORDERDLG_H
