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

#include "RenameDlg.h"
#include "resource.h"

#include <tchar.h>


RenameDlg::RenameDlg(Boot::Core &BootList, const Boot::WorkItem &item, HWND hWndParent)
	: CDialog(IDD_RENAME, hWndParent), m_BootList(BootList), m_Old(item)
{
}

BOOL RenameDlg::OnInitDialog()
{
	m_TT.Create(GetHwnd());
	m_TT.Add(LoadString(DLGRENAME_TT_IDOK), IDOK);
	m_TT.Add(LoadString(DLGRENAME_TT_IDC_NEWNAME), IDC_NEWNAME);
	 
	::SendMessage(::GetDlgItem(GetHwnd(), IDC_NEWNAME), EM_LIMITTEXT, (WPARAM)30, 0); // name max 30 chars
	
	tstring title = LoadString(DLGRENAME_STR_TITLE);
	title = title + m_Old.GetName() + _T("...");
	::SetWindowText(GetHwnd(), title.c_str());
			 
	::SetDlgItemText(GetHwnd(), IDC_NEWNAME, m_Old.GetName().c_str());
	
	return TRUE;
}

void RenameDlg::OnOK()
{
	try
	{
		//rename item...
		
		int length = ::GetWindowTextLength(::GetDlgItem(GetHwnd(), IDC_NEWNAME));
		TCHAR *buffer = new TCHAR[length + 1];
		::GetDlgItemText(GetHwnd(), IDC_NEWNAME, buffer, length + 1);
		tstring newName = buffer;
		delete [] buffer;
		
		tstring msg_information = LoadString(MSG_INFORMATION);
		if (newName.length() > 0)
		{
			m_BootList.Rename(m_Old, newName);

			CDialog::OnOK();
		}
		else
			::MessageBox(GetHwnd(), LoadString(DLGRENAME_MSG_EMPTYFIELD), msg_information.c_str(), MB_ICONINFORMATION | MB_OK);
	}
	
	catch (const Boot::Exception &e)
	{
		tstring msg_information = LoadString(MSG_INFORMATION);
		::MessageBox(GetHwnd(), (LoadString(DLGRENAME_MSG_FAILED) + (_T("\n\t") + e.GetMsg())).c_str(), msg_information.c_str(), MB_OK | MB_ICONWARNING);
	}
	
	catch (...)
	{
		DebugErrMsg(_T("RenameDlg::OnOK... Unknown error"));
	}
}
