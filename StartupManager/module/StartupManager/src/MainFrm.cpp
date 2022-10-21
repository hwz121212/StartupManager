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

#include "resource.h"
#include "mainfrm.h"
#include "Boot\WinTools.h"
#include "Boot\RegKey.h"
#include "logdlg.h"
#include "adddlg.h"
#include "renamedlg.h"
#include "btorderdlg.h"
#include "propertiesdlg.h"
#include "settingsdlg.h"
#include "aboutdlg.h"
#include "dropdlg.h"
#include "BtCoreExporter.h"

#include <winuser.h>
#include "lib/htmlhelp.h"

CMainFrame::CMainFrame() : m_hBitmap(0), m_hPrevFocus(0), m_BootList(false)
{
	//Create program mutex
	m_hMutex = ::CreateMutex(0, FALSE, _T("st-m"));

	GetTreeView().SetCoreObj(&m_BootList);
	GetListView().SetCoreObj(&m_BootList);
	
	//Set m_MainView as the view window of the frame
	SetView(m_MainView);
	
	// Set the Resource IDs for the toolbar buttons
	m_ToolbarData.clear();
	m_ToolbarData.push_back(IDM_FILE_ADD);
	m_ToolbarData.push_back(IDM_EDIT_DELETE);
	m_ToolbarData.push_back(IDM_EDIT_RENAME);
	m_ToolbarData.push_back(IDM_EDIT_RUN);
	m_ToolbarData.push_back(IDM_EDIT_PROPERTIES);
	m_ToolbarData.push_back(0);	// Separator
	m_ToolbarData.push_back(IDM_VIEW_REFRESH);
	m_ToolbarData.push_back(0);	// Separator
	m_ToolbarData.push_back(IDM_HELP_CONTENTS);
	m_ToolbarData.push_back(IDM_HELP_ABOUT);
}

CMainFrame::~CMainFrame()
{
	if (m_hBitmap != 0)
		::DeleteObject(m_hBitmap);

	if (m_hMutex != 0)
		::CloseHandle(m_hMutex);
}

void CMainFrame::PreCreate(CREATESTRUCT &cs)
{
#ifndef PORTABLE
    CRegKey hKey;
	hKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Startup Manager"),
		KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
	if (!hKey.IsOpen())
		return;
	
	//Check whether previous window size and position should be restored
	DWORD dwValue = 0;
	if ((SUCCEEDED(hKey.QueryValue(dwValue, _T("RememberSizePos")))) && (dwValue & 1))
	{
		TCHAR *szValue;
		DWORD dwValueLen = 256*sizeof(TCHAR);

		//Read x-coordinate from registry
		dwValueLen = 256*sizeof(TCHAR);
		szValue = new TCHAR[dwValueLen/sizeof(TCHAR)];
		if ((hKey.QueryValue(szValue, _T("PosX"), &dwValueLen) == ERROR_SUCCESS) && (_tcslen(szValue) > 0))
			cs.x = _ttoi(szValue);
		else
			cs.x = 100;
		delete [] szValue;

		//Read y-coordinate from registry
		dwValueLen = 256*sizeof(TCHAR);
		szValue = new TCHAR[dwValueLen/sizeof(TCHAR)];
		if ((hKey.QueryValue(szValue, _T("PosY"), &dwValueLen) == ERROR_SUCCESS) && (_tcslen(szValue) > 0))
			cs.y = _ttoi(szValue);
		else
			cs.y = 100;
		delete [] szValue;

		//Read width from registry
		dwValueLen = 256*sizeof(TCHAR);
		szValue = new TCHAR[dwValueLen/sizeof(TCHAR)];
		if ((hKey.QueryValue(szValue, _T("Width"), &dwValueLen) == ERROR_SUCCESS) && (_tcslen(szValue) > 0))
			cs.cx = _ttoi(szValue);
		else
			cs.cx = 700;
		delete [] szValue;

		//Read height from registry
		dwValueLen = 256*sizeof(TCHAR);
		szValue = new TCHAR[dwValueLen/sizeof(TCHAR)];
		if ((hKey.QueryValue(szValue, _T("Height"), &dwValueLen) == ERROR_SUCCESS) && (_tcslen(szValue) > 0))
			cs.cy = _ttoi(szValue);
		else
			cs.cy = 430;
		delete [] szValue;
	}
	else
	{
		cs.x = 100;
		cs.y = 100;
		cs.cx = 700;	//Width
		cs.cy = 430;	//Height
	}
#else
	TCHAR szFileName[MAX_PATH];
	::GetModuleFileName(GetApp()->GetInstanceHandle(), szFileName, MAX_PATH);
	File file;
	file.SetPath(szFileName);
	tstring path = file.GetDir() + _T("st-m_settings.ini");

	//Check whether previous window size and position should be restored
	if (::GetPrivateProfileInt(_T("settings"), _T("RememberSizePos"), 2, path.c_str()) == 1)
	{
		int value = -1;

		//Read x-coordinate from ini file
		if ((value = ::GetPrivateProfileInt(_T("position"), _T("X"), -1, path.c_str())) != -1)
			cs.x = value;
		else
			cs.x = 100;

		//Read y-coordinate from ini file
		if ((value = ::GetPrivateProfileInt(_T("position"), _T("Y"), -1, path.c_str())) != -1)
			cs.y = value;
		else
			cs.y = 100;

		//Read width from ini file
		if ((value = ::GetPrivateProfileInt(_T("size"), _T("Width"), -1, path.c_str())) != -1)
			cs.cx = value;
		else
			cs.cx = 700;

		//Read height from ini file
		if ((value = ::GetPrivateProfileInt(_T("size"), _T("Height"), -1, path.c_str())) != -1)
			cs.cy = value;
		else
			cs.cy = 430;
	}
	else
	{
		cs.x = 100;
		cs.y = 100;
		cs.cx = 700;	//Width
		cs.cy = 430;	//Height
	}
#endif
}

void CMainFrame::RestoreSortSettings()
{
#ifndef PORTABLE
    CRegKey hKey;
	hKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Startup Manager"),
		KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
	if (!hKey.IsOpen())
		return;

	//Check whether previous sort info should be restored
	DWORD dwValue = 0;
	if ((SUCCEEDED(hKey.QueryValue(dwValue, _T("RememberSortInfo")))) && (dwValue & 1))
	{
		int column = -1;
		DWORD dwValueLen = 256*sizeof(TCHAR);
		TCHAR *szValue = new TCHAR[dwValueLen/sizeof(TCHAR)];
		if (hKey.QueryValue(szValue, _T("Column"), &dwValueLen) == ERROR_SUCCESS)
			column = _ttoi(szValue);
		delete [] szValue;
	
		DWORD dwValue = 0;
		if (hKey.QueryValue(dwValue, _T("Ascending")) == ERROR_SUCCESS)
			GetListView().Sort(column, (dwValue & 1) ? true : false);
	}
#else
	TCHAR szFileName[MAX_PATH];
	::GetModuleFileName(GetApp()->GetInstanceHandle(), szFileName, MAX_PATH);
	File file;
	file.SetPath(szFileName);
	tstring path = file.GetDir() + _T("st-m_settings.ini");

	//Check whether previous sort info should be restored
	if (::GetPrivateProfileInt(_T("settings"), _T("RememberSortInfo"), 2, path.c_str()) == 1)
	{
		//Read column from ini file
		int column = ::GetPrivateProfileInt(_T("sortinfo"), _T("Column"), -1, path.c_str());

		//Read direction from ini file
		int value = ::GetPrivateProfileInt(_T("sortinfo"), _T("Ascending"), -1, path.c_str());
		if (value != -1)
			GetListView().Sort(column, (value == 1) ? true : false);
	}
#endif
}

void CMainFrame::CheckForUpdate()
{
#ifndef PORTABLE
    CRegKey hKey;
	hKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Startup Manager"),
		KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
	if (HKEY(hKey) != NULL)
	{
		//Check whether we should check for an update or not
		DWORD dwValue = 0;
		if ((SUCCEEDED(hKey.QueryValue(dwValue, _T("AutoUpdate")))) && (dwValue & 1))
		{
			m_UpdateCheck = new UpdateDlg::UpdateCheck(GetHwnd());
		#ifdef UNICODE
			m_UpdateCheck->Check(_T("http://startupmanager.org/versioninfo/st-m_latest_version.txt"));
		#else
			m_UpdateCheck->Check(_T("http://startupmanager.org/versioninfo/st-m_latest_version_ANSI.txt"));
		#endif
		}
		
		hKey.Close();
	}
#else
	TCHAR szFileName[MAX_PATH];
	::GetModuleFileName(GetApp()->GetInstanceHandle(), szFileName, MAX_PATH);
	File file;
	file.SetPath(szFileName);
	tstring path = file.GetDir() + _T("st-m_settings.ini");

	//Check whether we should check for an update or not
	if (::GetPrivateProfileInt(_T("settings"), _T("AutoUpdate"), 2, path.c_str()) == 1)
	{
		m_UpdateCheck = new UpdateDlg::UpdateCheck(GetHwnd());

	#ifdef UNICODE
		m_UpdateCheck->Check(_T("http://startupmanager.org/versioninfo/st-m_latest_version_portable.txt"));
	#else
		m_UpdateCheck->Check(_T("http://startupmanager.org/versioninfo/st-m_latest_version_portable_ANSI.txt"));
	#endif
	}
#endif
}

void CMainFrame::ForceVisibleDisplay()
{
    RECT rect;
	::GetWindowRect(m_hWnd, &rect);

    //check if the specified window-rectangle is visible on any display
	if(::MonitorFromRect(&rect, MONITOR_DEFAULTTONULL) == 0)
    {
        HMONITOR hMonitor;
        MONITORINFO mi = { sizeof(mi) };
			
        // find the nearest display to the rectangle
		hMonitor = ::MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);

		::GetMonitorInfo(hMonitor, &mi);

        // center window rectangle
        rect.left = mi.rcWork.left + ((mi.rcWork.right - mi.rcWork.left) - (rect.right-rect.left)) / 2;
        rect.top  = mi.rcWork.top  + ((mi.rcWork.bottom - mi.rcWork.top) - (rect.bottom-rect.top)) / 2;

		::SetWindowPos(m_hWnd, 0, rect.left, rect.top, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
    }
}

void CMainFrame::OnInitialUpdate()
{
	//Set application title
	tstring title = LoadString(IDW_MAIN);
#ifdef _DEBUG
	title = title + _T(" (DEBUG)");
#endif
#ifndef UNICODE
	title = title + _T(" (ANSI)");
#endif
#ifdef PORTABLE
	title = title + _T(" (PORTABLE)");
#endif
	title = title + _T(" :: ");
	title = title + LoadString(STR_TITLE);
	title = title + GetWindowsVersion();
	::SetWindowText(GetHwnd(), title.c_str());
	
	//Check Details in the View menu
	::CheckMenuRadioItem(::GetSubMenu(GetFrameMenu(), 2), IDM_VIEW_ICON, IDM_VIEW_REPORT, IDM_VIEW_REPORT, 0);

	//Window should accept dropped files
	::DragAcceptFiles(GetHwnd(), true);

	//Set correct number of items in status bar
	TCHAR buffer[51];
	_sntprintf(buffer, sizeof(buffer)/sizeof(buffer[0]) - 1, _T("%i %s"), m_BootList.GetItemCount(), LoadString(DLGLOG_STR_COUNT));
	m_StatusSection = _T("Main");
	m_StatusItems = buffer;
	SetStatusText();

	RestoreSortSettings();
	ForceVisibleDisplay();
	CheckForUpdate();
	
	//Set focus to treeview
	::SetFocus(GetTreeView().GetHwnd());
}

void CMainFrame::SetMenuIcons()
{	
	// Clear previous popup menu data
	m_MenuData.clear();
	ImageList_Destroy(m_himlMenu);
	m_himlMenu = NULL;

	// Set the icons for popup menu items
	AddMenuIcon(IDM_FILE_ADD, ::LoadIcon(GetApp()->GetInstanceHandle(), MAKEINTRESOURCE(IDI_NEW)));
	AddMenuIcon(IDM_EDIT_DELETE, ::LoadIcon(GetApp()->GetInstanceHandle(), MAKEINTRESOURCE(IDI_DELETE)));
	AddMenuIcon(IDM_EDIT_RENAME, ::LoadIcon(GetApp()->GetInstanceHandle(), MAKEINTRESOURCE(IDI_RENAME)));
	AddMenuIcon(IDM_EDIT_RUN, ::LoadIcon(GetApp()->GetInstanceHandle(), MAKEINTRESOURCE(IDI_RUN)));
	AddMenuIcon(IDM_EDIT_PROPERTIES, ::LoadIcon(GetApp()->GetInstanceHandle(), MAKEINTRESOURCE(IDI_PROPERTIES)));
	AddMenuIcon(IDM_VIEW_REFRESH, ::LoadIcon(GetApp()->GetInstanceHandle(), MAKEINTRESOURCE(IDI_REFRESH)));
	AddMenuIcon(IDM_SYSTEM_SHUTDOWN, ::LoadIcon(GetApp()->GetInstanceHandle(), MAKEINTRESOURCE(IDI_SHUTDOWN)));
	AddMenuIcon(IDM_SYSTEM_POWEROFF, ::LoadIcon(GetApp()->GetInstanceHandle(), MAKEINTRESOURCE(IDI_HYBERNATE)));
	AddMenuIcon(IDM_SYSTEM_REBOOT, ::LoadIcon(GetApp()->GetInstanceHandle(), MAKEINTRESOURCE(IDI_RESTART)));
	AddMenuIcon(IDM_SYSTEM_LOGOFF, ::LoadIcon(GetApp()->GetInstanceHandle(), MAKEINTRESOURCE(IDI_LOGOFF)));
	AddMenuIcon(IDM_HELP_CONTENTS, ::LoadIcon(GetApp()->GetInstanceHandle(), MAKEINTRESOURCE(IDI_HELP)));
	AddMenuIcon(IDM_HELP_UPDATE, ::LoadIcon(GetApp()->GetInstanceHandle(), MAKEINTRESOURCE(IDI_UPDATE)));
	AddMenuIcon(IDM_HELP_ABOUT, ::LoadIcon(GetApp()->GetInstanceHandle(), MAKEINTRESOURCE(IDI_ABOUT)));
}

void CMainFrame::SetButtons(const std::vector<UINT> ToolbarData)
{
	// A reference to the CToolbar object
	CToolbar& TB = GetToolbar();

	// Set the image lists for normal, hot and disabled buttons
	SetToolbarImages(TB, 8, RGB(255, 0, 255), IDI_TOOLBAR, IDI_TOOLBAR_HOT, IDI_TOOLBAR_DIS);

	// Set the resource IDs for the toolbar buttons
	TB.SetButtons(ToolbarData);

	// Add the TBSTYLE_AUTOSIZE style
	for (int i = 0; i < TB.GetButtonCount(); i++)
	{
		int iButtonID = TB.GetCommandID(i);
		if (iButtonID > 0)
		{
			BYTE style = TB.GetButtonStyle(iButtonID);
			TB.SetButtonStyle(iButtonID, style | TBSTYLE_AUTOSIZE);
		}
	}

	TB.SetButtonStyle(IDM_FILE_ADD, TBSTYLE_DROPDOWN | TBSTYLE_AUTOSIZE);

	// Add some text to the buttons
	TB.SetButtonText(IDM_FILE_ADD, LoadString(STR_FILE_ADD));
	TB.SetButtonText(IDM_EDIT_DELETE, LoadString(STR_EDIT_DELETE));
	TB.SetButtonText(IDM_EDIT_RENAME, LoadString(STR_EDIT_RENAME));
	TB.SetButtonText(IDM_EDIT_RUN, LoadString(STR_EDIT_RUN));
	TB.SetButtonText(IDM_EDIT_PROPERTIES, LoadString(STR_EDIT_PROPERTIES));
	TB.SetButtonText(IDM_VIEW_REFRESH, LoadString(STR_VIEW_REFRESH));
	TB.SetButtonText(IDM_HELP_CONTENTS, LoadString(STR_HELP_CONTENTS));
	TB.SetButtonText(IDM_HELP_ABOUT, LoadString(STR_HELP_ABOUT));

	CRebar& RB = GetRebar();
	if (IsRebarUsed())
        RB.ResizeBand(RB.GetBand(TB.GetHwnd()), TB.GetMaxSize());

	// Disable Rename, Delete and Properties toolbar buttons (only enabled when item selected)
	TB.DisableButton(IDM_EDIT_DELETE);
	TB.DisableButton(IDM_EDIT_RENAME);
	TB.DisableButton(IDM_EDIT_RUN);
	TB.DisableButton(IDM_EDIT_PROPERTIES);

	// Only set the icons with alpha-sections for popup menu items if the program is themed
	if (IsXPThemed()) SetMenuIcons();
}

/*
HIMAGELIST CMainFrame::CreateDisabledImageList(HIMAGELIST hImageList)
{
	HIMAGELIST hImgResult = 0;

	if (IsXPThemed())
	{
		HICON hIcon = 0;
		HIMAGELIST hImgTmp = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 9, 0);
		
		hIcon = ImageList_GetIcon(hImageList, 1, ILD_NORMAL);
		ImageList_AddIcon(hImgTmp, hIcon);
		::DestroyIcon(hIcon);

		HBITMAP hBMP = ::LoadBitmap(GetApp()->GetInstanceHandle(), MAKEINTRESOURCE(IDW_MAIN));
		ImageList_AddMasked(hImgTmp, hBMP, RGB(192, 192, 192));
		::DeleteObject(hBMP);

		for (int i = 1; i <= 4; i++)
		{
			hIcon = ImageList_GetIcon(hImageList, i, ILD_NORMAL);
			ImageList_AddIcon(hImgTmp, hIcon);
			::DestroyIcon(hIcon);
		}

		hImgResult = CFrame::CreateDisabledImageList(hImgTmp);
		ImageList_Destroy(hImgTmp);
	}
	else
		hImgResult = CFrame::CreateDisabledImageList(hImageList);

	return hImgResult;
}
*/

void CMainFrame::SetStatusText()
{
	// Get the coordinates of the parent window's client area.
	RECT rcClient;
	::GetClientRect(GetHwnd(), &rcClient);

	// Allocate an array for holding the right edge coordinates.
	int iPaneWidths[] = {rcClient.right-rcClient.left-100, -1};
	
	GetStatusbar().CreateParts(2, iPaneWidths);
	GetStatusbar().SetPartText(0, m_StatusSection.c_str());
	GetStatusbar().SetPartText(1, m_StatusItems.c_str());
}

//update items shown in listview and statusbar text
void CMainFrame::UpdateGUI()
{
	try
	{
		if (::GetFocus() != GetTreeView().GetHwnd())
			::SetFocus(GetTreeView().GetHwnd());

		tstring selsection = GetTreeView().GetSection();

		//change status bar text
		TCHAR stbuf[51];
		_sntprintf(stbuf, sizeof(stbuf)/sizeof(stbuf[0]) - 1, _T("%i %s"), m_BootList.GetItemCount(selsection), LoadString(DLGLOG_STR_COUNT));

		m_StatusItems = stbuf;
		m_StatusSection = m_BootList.GetLongSection(selsection);
		SetStatusText();

		//update listview content
		GetListView().DataToList(selsection);
	}

	catch (Boot::Exception const& e)
	{
		::MessageBox(GetHwnd(), e.GetMsg().c_str(), LoadString(MSG_INFORMATION), MB_OK | MB_ICONINFORMATION);
	}

	catch (...)
	{
		DebugErrMsg(_T("CMainFrame::UpdateGUI... Unknown exception"));
	}
}

//on file > log menu item
void CMainFrame::OnLog()
{
	HWND hFocus = ::GetFocus();
	LogDlg LogDlg(GetHwnd());
	LogDlg.DoModal();
	::SetFocus(hFocus);
}

void CMainFrame::OnExportHHtml()
{
	TCHAR szFilters[] = _T("Html Files (*.html)\0*.html\0All Files (*.*)\0*.*\0\0");
	TCHAR szFilePathName[_MAX_PATH] = _T("");
	OPENFILENAME ofn;
	::ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetHwnd();
	ofn.lpstrFilter = szFilters;
	ofn.lpstrFile = szFilePathName;
	ofn.lpstrDefExt = _T("html");
	ofn.nMaxFile = _MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	if (!::GetSaveFileName(&ofn))
		return;

	tstring location = szFilePathName;
	//ExportAsHtml(location, false);

	BtCoreExporter exp(*this, m_BootList);
	exp.DumpHtml(location, false);
}

void CMainFrame::OnExportVHtml()
{
	TCHAR szFilters[] = _T("Html Files (*.html)\0*.html\0All Files (*.*)\0*.*\0\0");
	TCHAR szFilePathName[_MAX_PATH] = _T("");
	OPENFILENAME ofn;
	::ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetHwnd();
	ofn.lpstrFilter = szFilters;
	ofn.lpstrFile = szFilePathName;
	ofn.lpstrDefExt = _T("html");
	ofn.nMaxFile = _MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	if (!::GetSaveFileName(&ofn))
		return;

	tstring location = szFilePathName;
	//ExportAsHtml(location, true);

	BtCoreExporter exp(*this, m_BootList);
	exp.DumpHtml(location, true);
}

void CMainFrame::OnExportXml()
{
	TCHAR szFilters[] = _T("Xml Files (*.xml)\0*.xml\0All Files (*.*)\0*.*\0\0");
	TCHAR szFilePathName[_MAX_PATH] = _T("");
	OPENFILENAME ofn;
	::ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetHwnd();
	ofn.lpstrFilter = szFilters;
	ofn.lpstrFile = szFilePathName;
	ofn.lpstrDefExt = _T("xml");
	ofn.nMaxFile = _MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	if (!::GetSaveFileName(&ofn))
		return;

	tstring location = szFilePathName;
	//ExportAsXml(location);

	BtCoreExporter exp(*this, m_BootList);

	tstring xslpath = _T("");

#ifndef PORTABLE
	CRegKey hKey;
	hKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Startup Manager"),
		KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
	if (hKey.IsOpen())
	{	
		//Check whether xsl stylesheet should be added
		DWORD dwValue = 0;
		if ((SUCCEEDED(hKey.QueryValue(dwValue, _T("UseXSL")))) && (dwValue & 1))
		{
			TCHAR szFileName[MAX_PATH];
			::GetModuleFileName(GetApp()->GetInstanceHandle(), szFileName, MAX_PATH);
			File file;
			file.SetPath(szFileName);

		#ifdef UNICODE
			xslpath = file.GetDir() + _T("st-m_items_utf16.xsl");
		#else
			xslpath = file.GetDir() + _T("st-m_items.xsl");
		#endif
		}
	}
#else
	TCHAR szFileName[MAX_PATH];
	::GetModuleFileName(GetApp()->GetInstanceHandle(), szFileName, MAX_PATH);
	File file;
	file.SetPath(szFileName);
	tstring inipath = file.GetDir() + _T("st-m_settings.ini");

	//Check whether xsl stylesheet should be added
	if (::GetPrivateProfileInt(_T("settings"), _T("UseXSL"), 2, inipath.c_str()) == 1)
	{
		#ifdef UNICODE
			xslpath = file.GetDir() + _T("st-m_items_utf16.xsl");
		#else
			xslpath = file.GetDir() + _T("st-m_items.xsl");
		#endif
	}
#endif

	exp.DumpXml(location, xslpath);
}

//add an item to the startup sections
void CMainFrame::OnAdd()
{
	AddDlg AddDlg(m_BootList, GetHwnd());
	AddDlg.DoModal();
	UpdateGUI();
}

void CMainFrame::DeleteItem(int i)
{
	assert(i > -1);

	try
	{
		Boot::WorkItem wItem(GetListView().GetItem(i));

		tstring question = _T("");
		UINT type = MB_YESNO;

		Item *pbtItem = m_BootList.Get(wItem);
		if (!pbtItem)
			throw (Boot::Exception(_T("CMainFrame::DeleteItem... Invalid item")));

		if (pbtItem->GetAttributes() & Item::attrSystem)
		{
			question = wItem.GetName() + LoadString(MSG_DELETESYSFILE);
			type |= MB_ICONWARNING | MB_DEFBUTTON2;
		}
		else
		{
			question = LoadString(MSG_DELETEFILE);
			type |= MB_ICONQUESTION;
		}

		tstring title = LoadString(STR_DELETE);
		title += wItem.GetName();
		title += _T("...");

		if (::MessageBox(GetHwnd(), question.c_str(), title.c_str(), type) == IDYES)
		{
			m_BootList.Delete(wItem);
			UpdateGUI();
		}
		else
			::SetFocus(GetListView().GetHwnd());
	}

	catch (Boot::Exception const& e)
	{
		tstring msg_information = LoadString(MSG_INFORMATION);
		::MessageBox(GetHwnd(), (LoadString(MSG_DELETEFAILED) + (_T("\n\t") + e.GetMsg())).c_str(), msg_information.c_str(), MB_OK | MB_ICONINFORMATION);
	}

	catch (...)
	{
		DebugErrMsg(_T("CMainFrame::DeleteItem... Unknown exception"));
	}
}

//delete the selected item from the startup sections
void CMainFrame::OnDelete()
{
	try
	{
		std::vector<int> items;

		int iNextItem = ListView_GetNextItem(GetListView().GetHwnd(), -1, LVNI_ALL | LVNI_SELECTED);
		while (iNextItem != -1)
		{
			items.push_back(iNextItem);		
			iNextItem = ListView_GetNextItem(GetListView().GetHwnd(), iNextItem, LVNI_ALL | LVNI_SELECTED);
		}

		for (int i=static_cast<int>(items.size())-1; i>=0; i--)
			DeleteItem(items.at(i));
	}
	
	catch (Boot::Exception const& e)
	{
		::MessageBox(GetHwnd(), e.GetMsg().c_str(), LoadString(MSG_INFORMATION), MB_OK | MB_ICONINFORMATION);	
	}
	
	catch (...)
	{
		DebugErrMsg(_T("CMainFrame::OnDelete... Unknown error"));
	}
}

//rename selected item
void CMainFrame::OnRename()
{
	int pos = GetListView().GetSelPos();
	if (pos > -1)
	{
		Boot::WorkItem wItem(GetListView().GetItem(pos));

		RenameDlg RenameDlg(m_BootList, wItem, GetHwnd());
		RenameDlg.DoModal();

		UpdateGUI();
	}
}

//create new boot order file
void CMainFrame::OnBtNew()
{
	BtOrderDlg BtOrderDlg(m_BootList, FALSE, GetHwnd());
	BtOrderDlg.DoModal();
	UpdateGUI();
}

//edit existing boot order file
void CMainFrame::OnBtOpen()
{
	BtOrderDlg BtOrderDlg(m_BootList, TRUE, GetHwnd());
	BtOrderDlg.DoModal();
	UpdateGUI();
}

//run selected item
void CMainFrame::OnRun()
{
	int pos = GetListView().GetSelPos();
	if (pos > -1)
	{
		tstring path = GetListView().GetItemStr(pos, 3);
		tstring flags = GetListView().GetItemStr(pos, 2);
		::ShellExecute(GetHwnd(), _T("open"), path.c_str(), flags.c_str(), 0, SW_SHOWNORMAL);
	}
}

//show item properties
void CMainFrame::OnProperties()
{
	int pos = GetListView().GetSelPos();
	if (pos > -1)
	{
		Boot::WorkItem wItem(GetListView().GetItemStr(pos, 3), GetListView().GetItemStr(pos), GetListView().GetItemStr(pos, 1));

		PropDlg PropDlg(wItem, GetListView().GetItemStr(pos, 2), GetHwnd());
		PropDlg.DoModal();
		
		::SetFocus(GetListView().GetHwnd());
	}
}

//display file properties (shell properties dialog)
void CMainFrame::OnFileProperties()
{
	int pos = GetListView().GetSelPos();
	if(pos > -1)
	{
		SHELLEXECUTEINFO ShExecInfo;
		::ZeroMemory(&ShExecInfo, sizeof(SHELLEXECUTEINFO));
		ShExecInfo.cbSize       = sizeof(ShExecInfo);
		ShExecInfo.lpVerb       = _T("properties");
		ShExecInfo.lpFile       = GetListView().GetItemStr(pos, 3);
		ShExecInfo.fMask        = SEE_MASK_INVOKEIDLIST;
		ShExecInfo.hwnd         = GetHwnd();
		ShExecInfo.lpParameters = 0;
		ShExecInfo.lpDirectory  = 0;
		ShExecInfo.nShow        = SW_SHOW;
		ShExecInfo.hInstApp     = 0;
		::ShellExecuteEx(&ShExecInfo);
	}
}

//on change listview icon style
void CMainFrame::OnListStyle(UINT nMenuID, LVStyles style)
{
	GetListView().SetIconStyle(style);
	::CheckMenuRadioItem(::GetSubMenu(GetFrameMenu(), 2), IDM_VIEW_ICON, IDM_VIEW_REPORT, nMenuID, 0);
}

//on view > refresh menu item
void CMainFrame::OnRefresh()
{
	//re-initialise bootlist
    m_BootList.Init();
    GetTreeView().SetCoreObj(&m_BootList);
    GetListView().SetCoreObj(&m_BootList);
    UpdateGUI();
}

//on system > settings menu item
void CMainFrame::OnSettings()
{
	HWND hFocus = ::GetFocus();
	SettingsDlg SettingsDlg(GetHwnd());
	SettingsDlg.DoModal();
	::SetFocus(hFocus);
}

//on help > contents menu item
void CMainFrame::OnHelpContents()
{
	TCHAR szFileName[MAX_PATH];
	::GetModuleFileName(GetModuleHandle(0), szFileName, MAX_PATH);
	File tmp;
	tmp.SetPath(szFileName);
	tstring help = tmp.GetDir() + _T("st-m.chm");
	if (HtmlHelp(GetHwnd(), help.c_str(), HH_DISPLAY_TOC, 0) == 0)
	{
		tstring msg_information = LoadString(MSG_INFORMATION);
		::MessageBox(GetHwnd(), LoadString(MSG_HELPMISSING), msg_information.c_str(), MB_OK | MB_ICONINFORMATION);
	}
}

//on help > check for update menu item
void CMainFrame::OnCheckUpdate()
{
    TRACE(_T("OnCheckupdate"));
	HWND hFocus = ::GetFocus();
	UpdateDlg UpdateDlg(GetHwnd());
	UpdateDlg.DoModal();
	::SetFocus(hFocus);
}

//on help > about menu item
void CMainFrame::OnAbout()
{
    TRACE(_T("OnAbout"));
	HWND hFocus = ::GetFocus();
	AboutDlg AboutDlg(GetHwnd());
	AboutDlg.DoModal();
	::SetFocus(hFocus);
}

void CMainFrame::OnChangeFocus()
{
	if (GetFocus() == GetTreeView())
	{
		::SetFocus(GetListView().GetHwnd());
		if (GetListView().GetSelPos() < 0)
			ListView_SetItemState(GetListView().GetHwnd(), 0, LVIS_SELECTED, LVIS_SELECTED);
	}
	else
		::SetFocus(GetTreeView().GetHwnd());
}

void CMainFrame::OnContextMenu()
{
	if (GetFocus() == GetListView())
		GetListView().OnRightClick();
}

BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
    TRACE(_T("OnCommand"));

	switch (LOWORD(wParam))
	{
	case IDM_FILE_ADD: OnAdd(); break;
	case IDM_FILE_BTNEW: OnBtNew(); break;
	case IDM_FILE_BTOPEN: OnBtOpen(); break;
	case IDM_FILE_LOG: OnLog(); break;
	case IDM_FILE_XPTHHTML: OnExportHHtml(); break;
	case IDM_FILE_XPTVHTML: OnExportVHtml(); break;
	case IDM_FILE_XPTXML: OnExportXml(); break;
	case IDM_FILE_MSCONFIG: ::ShellExecute(0, _T("open"), _T("msconfig"), 0, 0, SW_SHOWNORMAL); break;
	case IDM_FILE_REGEDIT: ::ShellExecute(0, _T("open"), _T("regedit"), 0, 0, SW_SHOWNORMAL); break;
	case IDM_FILE_EXIT: ::PostQuitMessage(0); break;
		
	case IDM_EDIT_DELETE: OnDelete(); break;
	case IDM_EDIT_RENAME: OnRename(); break;
	case IDM_EDIT_RUN: OnRun(); break;
	case IDM_EDIT_PROPERTIES: OnProperties(); break;
	case IDM_EDIT_FILEPROPERTIES: OnFileProperties(); break;
		
	case IDM_VIEW_ICON: OnListStyle(IDM_VIEW_ICON, LARGE); break;
	case IDM_VIEW_SMALL: OnListStyle(IDM_VIEW_SMALL, SMALL); break;
	case IDM_VIEW_LIST: OnListStyle(IDM_VIEW_LIST, LIST); break;
	case IDM_VIEW_REPORT: OnListStyle(IDM_VIEW_REPORT, DETAIL); break;
	case IDM_VIEW_REFRESH: OnRefresh(); break;
		
	case IDM_SYSTEM_SETTINGS: OnSettings(); break;
	case IDM_SYSTEM_SHUTDOWN: m_System.Shutdown(); break;
	case IDM_SYSTEM_POWEROFF: m_System.Poweroff(); break;
	case IDM_SYSTEM_REBOOT: m_System.Restart(); break;
	case IDM_SYSTEM_LOGOFF: m_System.Logoff(); break;
		
	case IDM_HELP_CONTENTS: OnHelpContents(); break;
	case IDM_HELP_WEBSITE: ::ShellExecute(0, _T("open"), _T("http://startupmanager.org/"), 0, 0, SW_SHOWNORMAL); break;
	case IDM_HELP_SUPPORT: ::ShellExecute(0, _T("open"), _T("https://sourceforge.net/donate/index.php?group_id=137182"), 0, 0, SW_SHOWNORMAL); break;
	case IDM_HELP_UPDATE: OnCheckUpdate(); break;
	case IDM_HELP_ABOUT: OnAbout(); break;

	case IDM_EDIT_TAB: OnChangeFocus(); break;
	case IDM_EDIT_APP: OnContextMenu(); break;
	}

	//Use the frame default message handling for remaining messages
	return CFrame::OnCommand(wParam, lParam);
}

void CMainFrame::OnCreate()
{
	m_bShowIndicatorStatus = FALSE;
	m_bShowMenuStatus = FALSE;
	CFrame::OnCreate();

	SetButtons(m_ToolbarData);
	SetTheme();
}

LRESULT CMainFrame::OnNotify(WPARAM /*wParam*/, LPARAM lParam)
{
    switch (((LPNMHDR)lParam)->code)
	{	
 	//Menu for dropdown toolbar button
	case TBN_DROPDOWN:
		{
			RECT rc;
			::SendMessage(((NMTOOLBAR*)lParam)->hdr.hwndFrom, TB_GETRECT, ((NMTOOLBAR*)lParam)->iItem, (LPARAM)&rc);
			::MapWindowPoints(((NMTOOLBAR*)lParam)->hdr.hwndFrom, HWND_DESKTOP, (LPPOINT)&rc, 2);

			TPMPARAMS tpm;
			::ZeroMemory(&tpm, sizeof(TPMPARAMS));
			tpm.cbSize = sizeof(TPMPARAMS);
			tpm.rcExclude = rc;
			
			HMENU hMenuLoaded = ::LoadMenu(GetApp()->GetResourceHandle(), MAKEINTRESOURCE(((NMTOOLBAR*)lParam)->iItem));
			HMENU hPopupMenu = ::GetSubMenu(hMenuLoaded, 0);
			::SetMenuDefaultItem(hPopupMenu, 0, ((NMTOOLBAR*)lParam)->iItem);
			::TrackPopupMenuEx(hPopupMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, rc.left, rc.bottom, GetHwnd(), &tpm);
			::DestroyMenu(hMenuLoaded);
			::DestroyMenu(hPopupMenu);
		}
		break;

	case NM_SETFOCUS:
		{
			HWND hFocus = ((LPNMHDR)lParam)->hwndFrom;
			if ((hFocus == GetTreeView().GetHwnd()) || (hFocus == GetListView().GetHwnd()))
				m_hPrevFocus = hFocus;
		}
		break;

	} //switch LPNMHDR  

	// Return 0L for any unhandled notifications
	return 0L;
}

BOOL CMainFrame::OnDropFiles(WPARAM wParam)
{
	// init hdrop
	HDROP hdrop = (HDROP)wParam;
	if (!hdrop)
		return FALSE;

	// Get the number of pathnames that have been dropped
	int wNumFilesDropped = ::DragQueryFile(hdrop, 0xFFFFFFFF, 0, 0);
	// there may be many, but we'll only use the first
	if (wNumFilesDropped > 0)
	{
		for (int i=0; i<wNumFilesDropped; i++)
		{
			// Get the number of bytes required by the file's full pathname
			UINT wPathnameSize = ::DragQueryFile(hdrop, i, 0, 0);
			// Allocate memory to contain full pathname & zero byte
			wPathnameSize += 1;
			TCHAR *pFile = new TCHAR[wPathnameSize];
			if (!pFile)
			{
				::DragFinish(hdrop);
				return FALSE;
			}
			// Copy the pathname into the buffer
			::DragQueryFile(hdrop, i, pFile, wPathnameSize);
			
			// Pop up dropped file dialog
			DropDlg DropDlg(m_BootList, pFile, GetHwnd());
			DropDlg.DoModal();
			
			delete [] pFile;
		}
	}
	// Free the memory block containing the dropped-file information
	::DragFinish(hdrop);

	return TRUE;
}

LRESULT CMainFrame::OnDestroy()
{
#ifndef PORTABLE
    CRegKey hKey;
	hKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Startup Manager"),
		KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
	if (hKey.IsOpen())
	{
		DWORD dwValue = 0;
		if ((SUCCEEDED(hKey.QueryValue(dwValue, _T("RememberSizePos")))) && (dwValue & 1))
		{
			RECT rc;
			::GetWindowRect(GetHwnd(), &rc);

			TCHAR *szValue;
			DWORD dwValueLen;

			dwValueLen = 256*sizeof(TCHAR);
			szValue = new TCHAR[dwValueLen/sizeof(TCHAR)];
			_itot(rc.left, szValue, 10);
			hKey.SetValue(szValue, _T("PosX"));
			delete [] szValue;

			dwValueLen = 256*sizeof(TCHAR);
			szValue = new TCHAR[dwValueLen/sizeof(TCHAR)];
			_itot(rc.top, szValue, 10);		
			hKey.SetValue(szValue, _T("PosY"));
			delete [] szValue;

			dwValueLen = 256*sizeof(TCHAR);
			szValue = new TCHAR[dwValueLen/sizeof(TCHAR)];
			_itot(rc.right - rc.left, szValue, 10);		
			hKey.SetValue(szValue, _T("Width"));
			delete [] szValue;

			dwValueLen = 256*sizeof(TCHAR);
			szValue = new TCHAR[dwValueLen/sizeof(TCHAR)];
			_itot(rc.bottom - rc.top, szValue, 10);		
			hKey.SetValue(szValue, _T("Height"));
			delete [] szValue;
		}

		dwValue = 0;
		if ((SUCCEEDED(hKey.QueryValue(dwValue, _T("RememberSortInfo")))) && (dwValue & 1))
		{
			int iColumn;
			bool bAscending;
			GetListView().GetSortInfo(iColumn, bAscending);

			DWORD dwValueLen = 256*sizeof(TCHAR);
			TCHAR *szValue = new TCHAR[dwValueLen/sizeof(TCHAR)];
			_itot(iColumn, szValue, 10);
			hKey.SetValue(szValue, _T("Column"));
			delete [] szValue;

			dwValue = bAscending ? 1 : 0;
			hKey.SetValue(dwValue, _T("Ascending"));
		}
	}
#else
	TCHAR szFileName[MAX_PATH];
	::GetModuleFileName(GetApp()->GetInstanceHandle(), szFileName, MAX_PATH);
	File file;
	file.SetPath(szFileName);
	tstring path = file.GetDir() + _T("st-m_settings.ini");

	if (::GetPrivateProfileInt(_T("settings"), _T("RememberSizePos"), 2, path.c_str()) == 1)
	{
		RECT rc;
		::GetWindowRect(GetHwnd(), &rc);

		TCHAR *szValue;
		DWORD dwValueLen;

		dwValueLen = 256*sizeof(TCHAR);
		szValue = new TCHAR[dwValueLen/sizeof(TCHAR)];
		_itot(rc.left, szValue, 10);
		::WritePrivateProfileString(_T("position"), _T("X"), szValue, path.c_str());
		delete [] szValue;

		dwValueLen = 256*sizeof(TCHAR);
		szValue = new TCHAR[dwValueLen/sizeof(TCHAR)];
		_itot(rc.top, szValue, 10);		
		::WritePrivateProfileString(_T("position"), _T("Y"), szValue, path.c_str());
		delete [] szValue;

		dwValueLen = 256*sizeof(TCHAR);
		szValue = new TCHAR[dwValueLen/sizeof(TCHAR)];
		_itot(rc.right - rc.left, szValue, 10);		
		::WritePrivateProfileString(_T("size"), _T("Width"), szValue, path.c_str());
		delete [] szValue;

		dwValueLen = 256*sizeof(TCHAR);
		szValue = new TCHAR[dwValueLen/sizeof(TCHAR)];
		_itot(rc.bottom - rc.top, szValue, 10);		
		::WritePrivateProfileString(_T("size"), _T("Height"), szValue, path.c_str());
		delete [] szValue;
	}

	if (::GetPrivateProfileInt(_T("settings"), _T("RememberSortInfo"), 2, path.c_str()) == 1)
	{
		int iColumn;
		bool bAscending;
		GetListView().GetSortInfo(iColumn, bAscending);

		DWORD dwValueLen = 256*sizeof(TCHAR);
		TCHAR *szValue = new TCHAR[dwValueLen/sizeof(TCHAR)];
		_itot(iColumn, szValue, 10);
		::WritePrivateProfileString(_T("sortinfo"), _T("Column"), szValue, path.c_str());
		delete [] szValue;

		::WritePrivateProfileString(_T("sortinfo"), _T("Ascending"), bAscending ? _T("1") : _T("0"), path.c_str());
	}
#endif

	::PostQuitMessage(0);
	return 0L;
}

void CMainFrame::SetTheme()
{
	// Set the rebar theme
	CRebar& RB = GetRebar();
	ThemeRebar tr    = {0};
	tr.UseThemes     = TRUE;
	tr.clrBkgnd1     = /*RGB(186, 186, 206)*/	RGB(215, 215, 229);
	tr.clrBkgnd2     = /*RGB(236, 236, 244)*/	RGB(243, 243, 247);
	tr.clrBand1      = /*RGB(244, 244, 251)*/	RGB(236, 236, 244);
	tr.clrBand2      = RGB(150, 150, 180);
	tr.FlatStyle     = FALSE;
	tr.KeepBandsLeft = TRUE;
	tr.LockMenuBand  = TRUE;
	tr.ShortBands    = /*TRUE*/					FALSE;
	tr.RoundBorders  = /*TRUE*/					FALSE;
	tr.UseLines      = FALSE;
	RB.SetRebarTheme(tr);

	// Set the toolbar theme
	CToolbar& TB = GetToolbar();
	ThemeToolbar tt = {0};
	tt.UseThemes    = TRUE;
	tt.clrHot1      = /*RGB(172, 190, 228)*/	RGB(255, 244, 204);
	tt.clrHot2      = /*RGB(192, 210, 238)*/	RGB(255, 214, 154);
	tt.clrPressed1  = /*RGB(152, 181, 226)*/	RGB(254, 154, 78);
	tt.clrPressed2  = /*RGB(152, 181, 226)*/	RGB(255, 211, 142);
	tt.clrOutline   = /*RGB( 49, 106, 197)*/	RGB(124, 124, 148);
	TB.SetToolbarTheme(tt);

	CMenubar& MB = GetMenubar();
	ThemeMenu tm = {0};
	tm.UseThemes    = TRUE;
	tm.clrHot1      = /*RGB(196, 215, 250)*/	RGB(255, 244, 204);
	tm.clrHot2      = /*RGB(120, 180, 220)*/	RGB(255, 214, 154);
	tm.clrPressed1  = /*RGB(210, 210, 200)*/	RGB(232, 233, 241);
	tm.clrPressed2  = /*RGB(248, 247, 243)*/	RGB(186, 185, 205);
	tm.clrOutline   = /*RGB(128, 128, 200)*/	RGB(124, 124, 148);
	MB.SetMenubarTheme(tm);
	SetMenuTheme(tm);

	m_MainView.SetBarColor(RGB(226, 226, 236));

	RecalcLayout();
}

void CMainFrame::OnUpdateAvailable()
{
	tstring msg = LoadString(STR_UPDATE1) + m_UpdateCheck->GetLatestVersion() + LoadString(STR_UPDATE2);
	if (::MessageBox(GetHwnd(), msg.c_str(), LoadString(STR_UPDATETITLE), MB_YESNO | MB_ICONQUESTION) == IDYES)
		::ShellExecute(NULL, _T("open"), m_UpdateCheck->GetUpdateUrl().c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void CMainFrame::OnUpdateCheckFailed()
{
	tstring msg_information = LoadString(MSG_INFORMATION);
	::MessageBox(GetHwnd(), LoadString(DLGUPDATE_MSG_FAILED), msg_information.c_str(), MB_OK | MB_ICONINFORMATION);
}

LRESULT CMainFrame::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_USER_UPDATEAVAILABLE:
		OnUpdateAvailable();
		break;

	case WM_USER_DOWNLOADFAILED:
		OnUpdateCheckFailed();
		break;

	case WM_DROPFILES:
		//handle dropped file
		OnDropFiles(wParam);
		UpdateGUI();
		break;

	case WM_SETFOCUS:
		::SetFocus(m_hPrevFocus);
		break;

	case WM_DESTROY:
		return OnDestroy();
	}

	// Pass unhandled messages on to the default window procedure
	return WndProcDefault(hWnd, uMsg, wParam, lParam);
}
