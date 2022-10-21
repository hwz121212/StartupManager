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

#include "PropertiesDlg.h"
#include "resource.h"
#include "Boot\ShortCut.h"

#include <tchar.h>

PropDlg::PropDlg(Boot::WorkItem const& item, const tstring &flags, HWND hWndParent)
	: CDialog(IDD_PROPERTIES, hWndParent), m_File(item), m_Flag(flags), m_hTabCtrl(0),
	m_hBrush(0), m_bThemeActive(IsXPThemed())
{
}

PropDlg::~PropDlg()
{
	if (m_hBrush)
		::DeleteObject(m_hBrush);
}

tstring PropDlg::GetPathFromCommand(const tstring &command)
{
	tstring buffer = command;
	trim(buffer);
	
	// the filepath might be between quotes
	if (buffer.at(0) == _T('"'))
	{
		int Quote = 0;
		int iQuotes = 1;
		for (unsigned i = 0; (i < buffer.length()) && (iQuotes <= 2); i++)
			if (buffer.at(i) == _T('"'))
			{
				buffer.erase(i, 1);
				Quote = i;
				iQuotes++;
			}
	 
		return buffer.substr(0, Quote);
	}	 
	
	// if none of the above, try to separate the path and the flags manually			
	std::string::size_type pos = buffer.find_last_of(_T("*.*"));
	if (pos != std::string::npos)
		buffer = buffer.substr(pos);
	else
	{
		// no extension found, thus we are unable to separate the command
		return buffer;
	}
	
	pos = buffer.find_last_of(_T("*-\\/,*"));
	if (pos != std::string::npos)
	{
		if (buffer.at(pos) == _T(' '))
			buffer = buffer.substr(pos-1);
		else
			buffer = buffer.substr(pos);
	}
	else
	{
		pos = buffer.find(_T(" "));
		if (pos != std::string::npos)
			buffer = buffer.substr(pos);
		else
			buffer = _T("");
	}
 
	tstring flags = buffer;
	buffer = command;
	return buffer.erase(buffer.find(flags), flags.length());
}

void PropDlg::GetInfo()
{
	//if we have a shortcut, select the target
	if (m_File.GetSection().find(_T("StartUp")) != std::string::npos)
	{
		ShortCut *tmp = new ShortCut;
		tmp->SetPath(m_File.GetCommand());
		bool bSuccess = true;
		m_Files[0].SetPath(tmp->ResolveLinkTarget(bSuccess));
		delete tmp;
	}
	else
		m_Files[0].SetPath(m_File.GetCommand());

	//check whether the flags also contain a file
	if (m_Flag.length() > 0)
	{
		tstring flagPath = m_Flag;
		if (flagPath.find_first_of(_T("*-\\/,*")) == 0)
		{
			if (flagPath.find(_T(" ")) != std::string::npos)
				flagPath.erase(0, flagPath.find(_T(" ")) + 1);
			else
				flagPath = _T("");
		}
		if (flagPath.length() > 0)
			m_Files[1].SetPath(GetPathFromCommand(flagPath));
	}
}

void PropDlg::OnSelchange()
{
	UpdateBackgroundBrush();

	DWORD i = static_cast<DWORD>(::SendMessage(m_hTabCtrl, TCM_GETCURSEL, 0, 0));
	
	cLibraryInfo::DLLINFO dllInfo;
	m_Files[i].GetProperties(dllInfo);

	//set new text for hyperlinks
	::SetDlgItemText(GetHwnd(), IDC_FILENAME, m_Files[i].GetName().c_str());
	m_Dir.SetTarget(m_Files[i].GetDir().c_str());
	tstring search = _T("http://www.google.com/search?hl=en&q=") + m_Files[i].GetName() + _T("&btnG=Google+Search");
	m_Google.SetTarget(search.c_str());

	//set new text for other static controls
	::SetDlgItemText(GetHwnd(), IDC_COMPANY, dllInfo.stCompany.c_str());
	::SetDlgItemText(GetHwnd(), IDC_COPYRIGHT, dllInfo.stCopyRight.c_str());
	::SetDlgItemText(GetHwnd(), IDC_FVERSION, dllInfo.stFileVersion.c_str());
	::SetDlgItemText(GetHwnd(), IDC_INAME, dllInfo.stInternalName.c_str());
	::SetDlgItemText(GetHwnd(), IDC_ONAME, dllInfo.stOriginalName.c_str());
	::SetDlgItemText(GetHwnd(), IDC_PNAME, dllInfo.stProductName.c_str());
	::SetDlgItemText(GetHwnd(), IDC_PVERSION, dllInfo.stProductVersion.c_str());
	::SetDlgItemText(GetHwnd(), IDC_DVERSION, dllInfo.stDllVersion.c_str());
	::SetDlgItemText(GetHwnd(), IDC_FILEOS, dllInfo.stFileOS.c_str());
	::SetDlgItemText(GetHwnd(), IDC_FTYPE, dllInfo.stFileType.c_str());
	::SetDlgItemText(GetHwnd(), IDC_DESCRIPTION, dllInfo.stDescription.c_str());
}

void PropDlg::InitTabCtrl()
{
	//create tab control imagelist
	HIMAGELIST ImageList = ImageList_Create(20, 20, ILC_COLOR32 | ILC_MASK, 1, 1);
	ImageList_AddIcon(ImageList, m_Files[0].GetIcon());
	if (m_Files[1].GetPath().length() > 0)
		ImageList_AddIcon(ImageList, m_Files[1].GetIcon());
	TabCtrl_SetImageList(m_hTabCtrl, ImageList);
	
	//create tab pages
	TCITEM TabPage;
	ZeroMemory(&TabPage, sizeof(TCITEM));
	TabPage.mask	    = TCIF_TEXT | TCIF_IMAGE;
	tstring tmp         = m_Files[0].GetName();
	TabPage.pszText     = const_cast<TCHAR*>(tmp.c_str());
	TabPage.cchTextMax  = (int)_tcslen(TabPage.pszText);
	TabPage.iImage      = 0;
	::SendMessage(m_hTabCtrl, TCM_INSERTITEM, 1, (LPARAM)&TabPage);
	if (m_Files[1].GetPath().length() > 0)
	{
		tmp = m_Files[1].GetName();
		TabPage.pszText     = const_cast<TCHAR*>(tmp.c_str());
		TabPage.cchTextMax  = (int)_tcslen(TabPage.pszText);
		TabPage.iImage      = 1;
		::SendMessage(m_hTabCtrl, TCM_INSERTITEM, 2, (LPARAM)&TabPage);
	}
	
	OnSelchange();
}

BOOL PropDlg::OnInitDialog()
{
	m_hTabCtrl = ::GetDlgItem(GetHwnd(), IDC_FILETAB);
	
	//set dialog title
	tstring title = LoadString(DLGPROP_STR_TITLE) + m_File.GetName();
	::SetWindowText(GetHwnd(), title.c_str());
	
	//set tooltips
	m_TT.Create(GetHwnd());
	m_TT.Add(LoadString(DLGPROP_TT_IDOK), IDOK);
	m_TT.Add(LoadString(DLGPROP_TT_IDC_FILENAME), IDC_FILENAME);
	m_TT.Add(LoadString(DLGPROP_TT_IDC_GOOGLE), IDC_GOOGLE);

	//create hyperlinks
	m_Dir.AttachDlgItem(IDC_FILENAME, this);
	m_Google.AttachDlgItem(IDC_GOOGLE, this);
	
	GetInfo();	
	InitTabCtrl();
	
	return TRUE;
}

LRESULT PropDlg::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{								 
	case IDC_FILETAB:
		switch (((LPNMHDR)lParam)->code)
		{
		case TCN_SELCHANGE:
			OnSelchange();
			break;
		}
	break;
	}
	 
	return TRUE;
}

//This approach was suggested by an article on codeproject about correctly drawing
//themed dialogs in windows xp: http://www.codeproject.com/wtl/ThemedDialog.asp
void PropDlg::UpdateBackgroundBrush()
{
	//Destroy old brush
	if (m_hBrush)
		::DeleteObject(m_hBrush);

	m_hBrush = 0;

	//Only do this if the theme is active
	if (m_bThemeActive)
	{
		RECT rc;

		//Get tab control dimensions
		::GetWindowRect(m_hTabCtrl, &rc);

		//Get the tab control DC
		HDC hDC = ::GetDC(m_hTabCtrl);

		//Create a compatible DC
		HDC hDCMem = ::CreateCompatibleDC(hDC);
		HBITMAP hBmp = ::CreateCompatibleBitmap(hDC, rc.right - rc.left, rc.bottom - rc.top);
		HBITMAP hBmpOld = (HBITMAP)(::SelectObject(hDCMem, hBmp));

		//Tell the tab control to paint in our DC
		::SendMessage(m_hTabCtrl, WM_PRINTCLIENT, (WPARAM)(hDCMem), (LPARAM)(PRF_ERASEBKGND | PRF_CLIENT | PRF_NONCLIENT));

		//Create a pattern brush from the bitmap selected in our DC
		m_hBrush = ::CreatePatternBrush(hBmp);

		//Restore the bitmap
		::SelectObject(hDCMem, hBmpOld);

		//Cleanup
		::DeleteObject(hBmp);
		::DeleteDC(hDCMem);
		::ReleaseDC(m_hTabCtrl, hDC);
	}
}

//Read-only or disabled edit controls do not send the WM_CTLCOLOREDIT message;
//instead, they send the WM_CTLCOLORSTATIC message.
HBRUSH PropDlg::OnCtlColorStatic(WPARAM wParam, LPARAM lParam)
{
	HBRUSH result = (HBRUSH)(LONG_PTR)CDialog::DialogProc(GetHwnd(), WM_CTLCOLORSTATIC, wParam, lParam);

	if (m_bThemeActive)
	{
		RECT rc;

		//Get the controls window dimensions
		::GetWindowRect((HWND)(lParam), &rc);

		//Map the coordinates to coordinates with the upper
		//left corner of dialog control as base
		::MapWindowPoints(0, m_hTabCtrl, (LPPOINT)(&rc), 2);

		//Adjust the position of the brush for this control
		//(else we see the top left of the brush as background)
		::SetBrushOrgEx((HDC)(wParam), -rc.left, -rc.top, 0);

		//Return the brush
		return m_hBrush;
	}
			
	return result;
}

BOOL PropDlg::DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CTLCOLORSTATIC:
		return (BOOL)(LONG_PTR)OnCtlColorStatic(wParam, lParam);
		
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		break;
	} // switch (uMsg)

	//default dialog procedure for other messages
	return CDialog::DialogProc(hwnd, uMsg, wParam, lParam);	
} // BOOL CALLBACK DialogProc(...)
