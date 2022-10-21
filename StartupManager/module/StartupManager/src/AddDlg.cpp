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

#include "AddDlg.h"
#include "resource.h"
#include "ToolTip.h"

#include <tchar.h>


AddDlg::AddDlg(Boot::Core &BootList, HWND hWndParent)
	: CDialog(IDD_ADD, hWndParent), m_BootList(BootList), m_strName(_T(""))
{
}

BOOL AddDlg::OnInitDialog()
{
	m_TT.Create(GetHwnd());
	m_TT.Add(LoadString(DLGADD_TT_IDOK), IDOK);
	m_TT.Add(LoadString(DLGADD_TT_IDC_SECTION), IDC_SECTION);
	m_TT.Add(LoadString(DLGADD_TT_IDC_NAME), IDC_NAME);
	m_TT.Add(LoadString(DLGADD_TT_IDC_FLAGS), IDC_FLAGS);

	//create browse control
	m_Browse.AttachDlgItem(IDC_PATH, this);
	tstring edittip = LoadString(DLGADD_TT_IDC_PATH);
	tstring buttontip = LoadString(DLGADD_TT_IDBROWSE);
	m_Browse.SetToolTips(edittip.c_str(), buttontip.c_str());
	m_Browse.SetBrowseInfo(_T("Executable (*.exe; *.com; *.bat; *.scr; *.jar; *.cgi)\0*.exe;*.com;*.bat;*.scr;*.jar;*.cgi\0Shortcut (*.lnk; *.pif)\0*.lnk;*.pif\0Library (*.dll; *.a; *.lib)\0*.dll;*.a;*.lib\0All Files (*.*)\0*.*\0"));
	
	//populate combobox with boot sections
	int i = 0;
	for (Boot::Core::const_reverse_iterator iter = m_BootList.rbegin(); iter != m_BootList.rend(); ++iter, i++)
		::SendDlgItemMessage(GetHwnd(), IDC_SECTION, CB_ADDSTRING, i, (LPARAM)(*iter)->GetSection().c_str());
	::SendDlgItemMessage(GetHwnd(), IDC_SECTION, CB_SETCURSEL, 2, 0);
	
	//limit item name length to 40 characters
	::SendMessage(::GetDlgItem(GetHwnd(), IDC_NAME), EM_LIMITTEXT, (WPARAM)40, 0);
	
	::EnableWindow(::GetDlgItem(GetHwnd(), IDC_NAME), true);
	::EnableWindow(::GetDlgItem(GetHwnd(), IDC_FLAGS), true);
	
	return TRUE;
}

void AddDlg::OnAddBHO(const tstring &section)
{
	try
	{
		int length = ::GetWindowTextLength(::GetDlgItem(GetHwnd(), IDC_NAME));
		TCHAR *buffer = new TCHAR[length + 1];
		::GetDlgItemText(GetHwnd(), IDC_NAME, buffer, length + 1);
		tstring CLSID = buffer;
		delete [] buffer;

		tstring msg_information = LoadString(MSG_INFORMATION);

		if (CLSID.length() <= 0)
		{
			::MessageBox(GetHwnd(), LoadString(DLGADD_MSG_EMPTYFIELDBHO), msg_information.c_str(), MB_OK | MB_ICONINFORMATION);
			return;
		}

		Boot::WorkItem wItem(_T(""), CLSID, section);
		m_BootList.Add(wItem);

		CDialog::OnOK();
	}

	catch (Boot::Exception const& e)
	{
		tstring msg_information = LoadString(MSG_INFORMATION);
		::MessageBox(GetHwnd(), (LoadString(DLGADD_MSG_FAILEDBHO) + (_T("\n\t") + e.GetMsg())).c_str(), msg_information.c_str(), MB_OK | MB_ICONINFORMATION);
	}

	catch (...)
	{
		throw;	// re-throw
	}
}

void AddDlg::OnOK()
{
	//add item...
	
	try
	{
		tstring msg_information = LoadString(MSG_INFORMATION);
	
		int length = ::GetWindowTextLength(::GetDlgItem(GetHwnd(), IDC_SECTION));
		TCHAR *buffer = new TCHAR[length + 1];
		::GetDlgItemText(GetHwnd(), IDC_SECTION, buffer, length + 1);
		tstring section = buffer;
		delete [] buffer;
		length = ::GetWindowTextLength(::GetDlgItem(GetHwnd(), IDC_PATH));
		buffer = new TCHAR[length + 1];
		::GetDlgItemText(GetHwnd(), IDC_PATH, buffer, length + 1);
		tstring path = buffer;
		delete [] buffer;

		if (section.find(_T("BHO")) != std::string::npos)
			OnAddBHO(section);
		else if ((path.length() > 0) && (section.length() > 0))
		{
			if ((section.find(_T("WinINI")) != std::string::npos)
				|| (section.find(_T("Userinit")) != std::string::npos)
				|| (section.find(_T("Shell")) != std::string::npos))
			{
				if (path.find(_T(" ")) != std::string::npos)
				{
					::MessageBox(GetHwnd(), LoadString(DLGADD_MSG_SPACES), msg_information.c_str(), MB_OK | MB_ICONINFORMATION);
					return;
				}
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

			tstring name = _T("");
			if (::IsWindowEnabled(::GetDlgItem(GetHwnd(), IDC_NAME)))
			{
				length = ::GetWindowTextLength(::GetDlgItem(GetHwnd(), IDC_NAME));
				buffer = new TCHAR[length + 1];
				::GetDlgItemText(GetHwnd(), IDC_NAME, buffer, length + 1);
				tstring iname = buffer;
				delete [] buffer;
				if(iname.length() > 0)
					name = iname;
			}
			if (name.length() <= 0)
			{
				File file;
				file.SetPath(path);
				name = file.GetName().erase(file.GetName().length() - file.GetExt().length() - 1, file.GetExt().length() + 1);
			}

			Boot::WorkItem wItem(path, name, section);
			m_BootList.Add(wItem);

			CDialog::OnOK();
		}
		else
			::MessageBox(GetHwnd(), LoadString(DLGADD_MSG_EMPTYFIELD), msg_information.c_str(), MB_OK | MB_ICONINFORMATION);
	}

	catch (Boot::Exception const& e)
	{
		tstring msg_information = LoadString(MSG_INFORMATION);
		::MessageBox(GetHwnd(), (LoadString(DLGADD_MSG_FAILED) + (_T("\n\t") + e.GetMsg())).c_str(), msg_information.c_str(), MB_OK | MB_ICONINFORMATION);
	}

	catch (...)
	{
		DebugErrMsg(_T("AddDlg::OnOK... Unknown error"));
	}
}

BOOL AddDlg::DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{			
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{															
		case IDC_SECTION:
			{
				//enable or disable flags and name fields on combobox activity
									 
				int sectionl = ::GetWindowTextLength(::GetDlgItem(GetHwnd(), IDC_SECTION));
				TCHAR *sectionb = new TCHAR[sectionl + 1];
				::GetDlgItemText(GetHwnd(), IDC_SECTION, sectionb, sectionl + 1);
				tstring section = sectionb;
				delete [] sectionb;
					
				if (section.find(_T("BHO")) != std::string::npos)
				{
					::SetDlgItemText(GetHwnd(), IDC_STATICNAME, _T("CLSID:"));
					::SetWindowText(m_Browse.GetHwnd(), _T(""));

					m_Browse.Disable();
					::EnableWindow(::GetDlgItem(GetHwnd(), IDC_NAME), true);
					::EnableWindow(::GetDlgItem(GetHwnd(), IDC_FLAGS), false);
				}
				else
				{
					if (m_strName.length() <= 0)
					{
						int length = ::GetWindowTextLength(::GetDlgItem(GetHwnd(), IDC_STATICNAME));
						TCHAR *buffer = new TCHAR[length + 1];
						::GetDlgItemText(GetHwnd(), IDC_STATICNAME, buffer, length + 1);
						m_strName = buffer;
						delete [] buffer;
					}

					::SetDlgItemText(GetHwnd(), IDC_STATICNAME, m_strName.c_str());

					m_Browse.Enable();

					if ((section.find(_T("WinINI")) != std::string::npos)
						|| (section.find(_T("Userinit")) != std::string::npos)
						|| (section.find(_T("Shell")) != std::string::npos))
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
			}
			break;
		}
		break;
	} // switch (uMsg)

	//default dialog procedure for other messages
	return CDialog::DialogProc(hwnd, uMsg, wParam, lParam);	
} // BOOL CALLBACK DialogProc(...)
