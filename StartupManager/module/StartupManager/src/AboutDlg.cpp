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

#include "AboutDlg.h"
#include "resource.h"
#include "Boot\File.h"

AboutDlg::AboutDlg(HWND hWndParent) : CDialog(IDD_ABOUT, hWndParent)
{
}

tstring AboutDlg::GetLicenseLocation()
{
	TCHAR szFileName[MAX_PATH];
	::GetModuleFileName(::GetModuleHandle(0), szFileName, MAX_PATH);
	Boot::File tmp;
	tmp.SetPath(szFileName);
	tstring license = tmp.GetDir() + _T("license.txt");
	return license;
}

BOOL AboutDlg::OnInitDialog()
{
	m_TT.Create(GetHwnd());
	m_TT.Add(_T("http://startupmanager.org/"), IDC_HOMEPAGE);
	m_TT.Add(_T("GNU General Public License (GPL)"), IDC_LICENSE);
	m_TT.Add(_T("Win32++ Website"), IDC_WIN32XX);
	
	m_Home.AttachDlgItem(IDC_HOMEPAGE, this);
	m_Home.SetTarget(_T("http://startupmanager.org/"));
	m_License.AttachDlgItem(IDC_LICENSE, this);
	m_License.SetTarget(GetLicenseLocation().c_str());
	m_Win32xx.AttachDlgItem(IDC_WIN32XX, this);
	m_Win32xx.SetTarget(_T("http://users.bigpond.net.au/programming/index.htm"));

	m_Header.Display(IDI_HEADER, GetHwnd(), GetApp()->GetInstanceHandle());

	m_Donate.Create(::GetDlgItem(GetHwnd(), IDC_DONATE), IDI_DONATE, GetApp()->GetInstanceHandle());
	m_Donate.SetTarget(_T("https://sourceforge.net/donate/index.php?group_id=137182"));
	
	return TRUE;
}
