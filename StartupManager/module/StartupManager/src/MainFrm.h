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
 
#ifndef MAINFRM_H
#define MAINFRM_H

#include "..\Win32++\Frame.h"
#include "MainView.h"
#include "Logoff.h"
#include "boot\btcore.h"
#include "types\tfstream.h"
#include "updatedlg.h"

using namespace Boot;

class CMainFrame : public CFrame
{
public:
    CMainFrame();
    virtual ~CMainFrame();
	virtual CMainView&  GetMainView() {return m_MainView;}
	virtual CLeftView&  GetTreeView() {return m_MainView.GetTreeView();}
	virtual CRightView& GetListView() {return m_MainView.GetListView();}
//	virtual HIMAGELIST CreateDisabledImageList(HIMAGELIST hImageList);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnNotify(WPARAM wParam, LPARAM lParam);
	void SetTheme();
	void UpdateGUI();

private:
	void RestoreSortSettings();
	void CheckForUpdate();
	void ForceVisibleDisplay();
    virtual void OnInitialUpdate();
    void OnLog();
	void OnExportHHtml();
	void OnExportVHtml();
	void OnExportXml();
    void OnAdd();
    void OnBtNew();
    void OnBtOpen();
	void DeleteItem(int i);
    void OnDelete();
    void OnRename();
	void OnRun();
    void OnProperties();
	void OnFileProperties();
	void OnListStyle(UINT nMenuID, LVStyles style);
    void OnRefresh();
    void OnSettings();
    void OnHelpContents();
	void OnCheckUpdate();
    void OnAbout();
	void OnChangeFocus();
	void OnContextMenu();
    virtual void OnCreate();
	void SetMenuIcons();
	virtual void SetButtons(const std::vector<UINT> ToolbarData);
	virtual void SetStatusText();
    virtual void PreCreate(CREATESTRUCT &cs);
    BOOL OnDropFiles(WPARAM wParam);
	LRESULT OnDestroy();
	void OnUpdateAvailable();
	void OnUpdateCheckFailed();
    virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    CMainView m_MainView;
    CLogoff m_System;
    HBITMAP m_hBitmap;
    Boot::Core m_BootList;
	tstring m_StatusSection;
	tstring m_StatusItems;
	HWND m_hPrevFocus;
	HANDLE m_hMutex;
	UpdateDlg::UpdateCheck *m_UpdateCheck;
};

#endif //MAINFRM_H
