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

#include "DropDlg.h"
#include "resource.h"

#include <tchar.h>


DropDlg::DropDlg(Boot::Core &BootList, TCHAR *newfile, HWND hWndParent)
	: CDialog(IDD_DRAGDROP, hWndParent), m_BootList(BootList), m_bExp(false)
{
	m_nWidth   = 0;
	m_nHeight  = 0;
	m_nTopl    = 0;
	m_nTopc    = 0;
	
	m_File = new ShortCut;
	m_File->SetPath(newfile);
	if (m_File->GetExt() == _T("lnk"))
	{
		bool bSuccess = true;
		m_File->SetPath(m_File->ResolveLinkTarget(bSuccess));
	}
}

DropDlg::~DropDlg()
{
	if (m_File)
		delete m_File;
}

void DropDlg::Resize()
{
	//get window position
	RECT rc, wrc, wcrc;
	::GetWindowRect(GetHwnd(), &wrc);
	int xpos = wrc.left;
	int ypos = wrc.top;
	
	//resize window and adapt button text
	m_bExp = !m_bExp;
	::SetDlgItemText(GetHwnd(), IDEXP, m_bExp ? _T("\xBB") : _T("\xAB"));
	::MoveWindow(GetHwnd(), xpos, ypos, m_nWidth, (m_bExp ? m_nHeight*23/17 : m_nHeight), SWP_NOZORDER | SWP_NOMOVE);
	
	//reposition stuff
	::GetClientRect(GetHwnd(), &rc);
	::GetWindowRect(::GetDlgItem(GetHwnd(), IDCANCEL), &wcrc);
	::MoveWindow(::GetDlgItem(GetHwnd(), IDCANCEL), rc.right-(wcrc.right-wcrc.left)-(wrc.right-wcrc.right)+(((wrc.right-rc.right)-wrc.left)/2), rc.bottom-m_nTopc, wcrc.right-wcrc.left, wcrc.bottom-wcrc.top, SWP_NOZORDER);
	::GetWindowRect(::GetDlgItem(GetHwnd(), IDOK), &wcrc);
	::MoveWindow(::GetDlgItem(GetHwnd(), IDOK), rc.right-(wcrc.right-wcrc.left)-(wrc.right-wcrc.right)+(((wrc.right-rc.right)-wrc.left)/2), rc.bottom-m_nTopc, wcrc.right-wcrc.left, wcrc.bottom-wcrc.top, SWP_NOZORDER);
	::GetWindowRect(::GetDlgItem(GetHwnd(), IDEXP), &wcrc);
	::MoveWindow(::GetDlgItem(GetHwnd(), IDEXP), rc.right-(wcrc.right-wcrc.left)-(wrc.right-wcrc.right)+(((wrc.right-rc.right)-wrc.left)/2), rc.bottom-m_nTopc, wcrc.right-wcrc.left, wcrc.bottom-wcrc.top, SWP_NOZORDER);
	::GetWindowRect(::GetDlgItem(GetHwnd(), IDC_LINE), &wcrc);
	::MoveWindow(::GetDlgItem(GetHwnd(), IDC_LINE), 0, rc.bottom-m_nTopl, wrc.right-wrc.left, wcrc.bottom-wcrc.top, SWP_NOZORDER);
	
	//show or hide appropriate controls			 
	::ShowWindow(::GetDlgItem(GetHwnd(), IDC_EXP1), m_bExp ? SW_SHOWNOACTIVATE : SW_HIDE); 
	::ShowWindow(::GetDlgItem(GetHwnd(), IDC_EXP2), m_bExp ? SW_SHOWNOACTIVATE : SW_HIDE);
	::ShowWindow(::GetDlgItem(GetHwnd(), IDC_NAME), m_bExp ? SW_SHOWNOACTIVATE : SW_HIDE);
	::ShowWindow(::GetDlgItem(GetHwnd(), IDC_FLAGS), m_bExp ? SW_SHOWNOACTIVATE : SW_HIDE); 
	::EnableWindow(::GetDlgItem(GetHwnd(), IDC_NAME), m_bExp);
	::EnableWindow(::GetDlgItem(GetHwnd(), IDC_FLAGS), m_bExp);
}

BOOL DropDlg::OnInitDialog()
{
	m_TT.Create(GetHwnd());
	m_TT.Add(LoadString(DLGDROP_TT_IDOK), IDOK);
	m_TT.Add(LoadString(DLGDROP_TT_IDC_SECTION), IDC_SECTION);
	m_TT.Add(LoadString(DLGDROP_TT_IDEXP), IDEXP);
	m_TT.Add(LoadString(DLGDROP_TT_IDC_NAME), IDC_NAME);
	m_TT.Add(LoadString(DLGDROP_TT_IDC_FLAGS), IDC_FLAGS);
	
	//populate combobox with boot sections
	int i = 0;
	for (Boot::Core::const_reverse_iterator iter = m_BootList.rbegin(); iter != m_BootList.rend(); ++iter, i++)
		if (!((*iter)->GetSection().find(_T("BHO")) != std::string::npos))
			if (!((m_File->GetName().find(_T(" ")) != std::string::npos) && (((*iter)->GetSection().find(_T("WinINI")) != std::string::npos)
				|| ((*iter)->GetSection().find(_T("Userinit")) != std::string::npos)
				|| ((*iter)->GetSection().find(_T("Shell")) != std::string::npos))))
				::SendDlgItemMessage(GetHwnd(), IDC_SECTION, CB_ADDSTRING, i, (LPARAM)(*iter)->GetSection().c_str());
	::SendDlgItemMessage(GetHwnd(), IDC_SECTION, CB_SETCURSEL, 2, 0);
	
	//set info text
	tstring info = LoadString(DLGDROP_STR_INFO1);
	info = info + m_File->GetName();
	info = info + LoadString(DLGDROP_STR_INFO2);
	::SetDlgItemText(GetHwnd(), IDC_TEXT, info.c_str());
	::SetDlgItemText(GetHwnd(), IDC_NAME, m_File->GetName().erase(m_File->GetName().length() - m_File->GetExt().length() - 1, m_File->GetExt().length() + 1).c_str());
	//set file icon
	::SendDlgItemMessage(GetHwnd(), IDC_SHOWICON, STM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)m_File->GetIcon());
	
	//limit item name length to 30 characters
	::SendMessage(::GetDlgItem(GetHwnd(), IDC_NAME), EM_LIMITTEXT, (WPARAM)30, 0);
		
	//set dialog state to "contracted"
	::ShowWindow(::GetDlgItem(GetHwnd(), IDC_EXP1), SW_HIDE);
	::ShowWindow(::GetDlgItem(GetHwnd(), IDC_EXP2), SW_HIDE); 
	::ShowWindow(::GetDlgItem(GetHwnd(), IDC_NAME), SW_HIDE);
	::ShowWindow(::GetDlgItem(GetHwnd(), IDC_FLAGS), SW_HIDE); 
	::EnableWindow(::GetDlgItem(GetHwnd(), IDC_NAME), m_bExp);
	::EnableWindow(::GetDlgItem(GetHwnd(), IDC_FLAGS), m_bExp);
	
	//calculate dialog size and position
	RECT rc, wrc, wcrc;
	::GetClientRect(GetHwnd(), &rc);
	::GetWindowRect(GetHwnd(), &wrc);
	m_nHeight = wrc.bottom - wrc.top;
	m_nWidth = wrc.right - wrc.left;
	::GetWindowRect(::GetDlgItem(GetHwnd(), IDEXP), &wcrc);
	m_nTopc = ((wrc.bottom-wcrc.top)-((wrc.right-wrc.left-rc.right)/2));
	::GetWindowRect(::GetDlgItem(GetHwnd(), IDC_LINE), &wcrc);
	m_nTopl = ((wrc.bottom-wcrc.top)-((wrc.right-wrc.left-rc.right)/2));
	
	return TRUE;
}

void DropDlg::OnOK()
{
	try
	{
		//add item...
		
		tstring msg_information = LoadString(MSG_INFORMATION);
		
		int length = ::GetWindowTextLength(::GetDlgItem(GetHwnd(), IDC_SECTION));
		TCHAR *buffer = new TCHAR[length + 1];
		::GetDlgItemText(GetHwnd(), IDC_SECTION, buffer, length + 1);
		tstring section = buffer;
		delete [] buffer;
		
		tstring name = _T("");
		tstring path = m_File->GetPath();
		
		if ((section.find(_T("WinINI")) != std::string::npos)
			|| (section.find(_T("Userinit")) != std::string::npos)
			|| (section.find(_T("Shell")) != std::string::npos))
		{
			if (path.find(_T(" ")) != std::string::npos)
			{
				::MessageBox(GetHwnd(), LoadString(DLGDROP_MSG_SPACES), msg_information.c_str(), MB_OK | MB_ICONINFORMATION);
				return;
			}
		}
		
		if (m_bExp)
		{
			if (::IsWindowEnabled(::GetDlgItem(GetHwnd(), IDC_NAME)))
			{
				length = ::GetWindowTextLength(::GetDlgItem(GetHwnd(), IDC_NAME));
				buffer = new TCHAR[length + 1];
				::GetDlgItemText(GetHwnd(), IDC_NAME, buffer, length + 1);
				name = buffer;
				delete [] buffer;
			}
			
			if (::IsWindowEnabled(::GetDlgItem(GetHwnd(), IDC_FLAGS)))
			{	
				length = ::GetWindowTextLength(::GetDlgItem(GetHwnd(), IDC_FLAGS));
				buffer = new TCHAR[length + 1];
				::GetDlgItemText(GetHwnd(), IDC_FLAGS, buffer, length + 1);
				tstring flags = buffer;
				delete [] buffer;
				if (flags.length() > 0)
				{
					if (path.find(_T(" ")) != std::string::npos)
						path = _T("\"") + path + _T("\"");
					path = path + _T(" ") + flags;
				}
			}	
		}
		
		if (name.length() <= 0)
			name = m_File->GetName().erase(m_File->GetName().length() - m_File->GetExt().length() - 1, m_File->GetExt().length() + 1);
		
		Boot::WorkItem wItem(path, name, section);
		m_BootList.Add(wItem);
			 
		CDialog::OnOK();
	}
	
	catch (Boot::Exception const& e)
	{
		tstring msg_information = LoadString(MSG_INFORMATION);
		::MessageBox(GetHwnd(), (LoadString(DLGDROP_MSG_FAILED) + (_T("\n\t") + e.GetMsg())).c_str(), msg_information.c_str(), MB_OK | MB_ICONINFORMATION);
	}

	catch (...)
	{
		DebugErrMsg(_T("DropDlg::OnOK... Unknown error"));
	}
}

BOOL DropDlg::DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{			
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{							 
		case IDEXP:
			Resize();
			//fall through...
		case IDC_SECTION:
			if (m_bExp)
			{
				//enable or disable flags and name fields on combobox activity
									 
				int sectionl;
				sectionl = ::GetWindowTextLength(::GetDlgItem(GetHwnd(), IDC_SECTION));
				TCHAR *sectionb;
				sectionb = new TCHAR[sectionl + 1];
				::GetDlgItemText(GetHwnd(), IDC_SECTION, sectionb, sectionl + 1);
				tstring section;
				section = sectionb;
				delete [] sectionb;
							 
				if ((section.find(_T("WinINI")) != std::string::npos)  || (section.find(_T("Userinit")) != std::string::npos) || (section.find(_T("Shell")) != std::string::npos))
				{
					::EnableWindow(::GetDlgItem(GetHwnd(), IDC_NAME), false);
					::EnableWindow(::GetDlgItem(GetHwnd(), IDC_FLAGS), false);
				}
				else
				{
					::EnableWindow(::GetDlgItem(GetHwnd(), IDC_NAME), true);
					::EnableWindow(::GetDlgItem(GetHwnd(), IDC_FLAGS), true);
				}
			}
			break;
		}
		break;
	} // switch (uMsg)

	//default dialog procedure for other messages
	return CDialog::DialogProc(hwnd, uMsg, wParam, lParam);	
} // BOOL CALLBACK DialogProc(...)
