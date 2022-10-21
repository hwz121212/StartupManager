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

#include "SettingsDlg.h"
#include "resource.h"
#include "boot\regkey.h"
#include "boot\wintools.h"

#ifdef PORTABLE
  #include "boot\file.h"
  using Boot::File;
#endif

class CGeneralPage : public CPropertyPage
{
public:
	CGeneralPage(UINT nIDTemplate, LPCTSTR szTitle);
	virtual ~CGeneralPage() {}
	virtual BOOL DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void OnApply();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL OnQueryCancel();
	virtual void OnSetActive();
	virtual int Validate();

private:
	bool m_bRememberSizePos;
	bool m_bRememberSortInfo;
	bool m_bAutoUpdate;
};

CGeneralPage::CGeneralPage(UINT nIDTemplate, LPCTSTR szTitle)
//  modified by David Nash
//	: CPropertyPage(IDD_GENERALSETTINGS, tstring(LoadString(SETTINGSDLG_STR_TITLEP1)).c_str()),
	: CPropertyPage(nIDTemplate, szTitle),
	m_bRememberSizePos(false), m_bRememberSortInfo(false), m_bAutoUpdate(false)
{
}

BOOL CGeneralPage::DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	// on any command notification, tell the property sheet to enable the Apply button
	case WM_COMMAND:
		PropSheet_Changed(GetParent(hWnd), hWnd);
		break;

	default:
		break;
	}

	return DialogProcDefault(hWnd, uMsg, wParam, lParam);
}

void CGeneralPage::OnApply()
{
	//save changes to startup manager registry key
	CRegKey hKey;
	hKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Startup Manager"), KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
	if (HKEY(hKey) != NULL)
	{
		DWORD dwValue = (::IsDlgButtonChecked(GetHwnd(), IDC_SIZEPOS) == BST_CHECKED) ? 1 : 0;
		if (((dwValue == 1) && (m_bRememberSizePos == false)) || ((dwValue == 0) && (m_bRememberSizePos == true)))
		{
		#ifndef PORTABLE
			hKey.SetValue(dwValue, _T("RememberSizePos"));
		#else
			TCHAR szFileName[MAX_PATH];
			::GetModuleFileName(GetApp()->GetInstanceHandle(), szFileName, MAX_PATH);
			File file;
			file.SetPath(szFileName);
			tstring path = file.GetDir() + _T("st-m_settings.ini");

			tstring value = (dwValue == 1) ? _T("1") : _T("0");
			::WritePrivateProfileString(_T("settings"), _T("RememberSizePos"), value.c_str(), path.c_str());
		#endif

			m_bRememberSizePos = !m_bRememberSizePos;
		}

		dwValue = (::IsDlgButtonChecked(GetHwnd(), IDC_SORTINFO) == BST_CHECKED) ? 1 : 0;
		if (((dwValue == 1) && (m_bRememberSortInfo == false)) || ((dwValue == 0) && (m_bRememberSortInfo == true)))
		{
		#ifndef PORTABLE
			hKey.SetValue(dwValue, _T("RememberSortInfo"));
		#else
			TCHAR szFileName[MAX_PATH];
			::GetModuleFileName(GetApp()->GetInstanceHandle(), szFileName, MAX_PATH);
			File file;
			file.SetPath(szFileName);
			tstring path = file.GetDir() + _T("st-m_settings.ini");

			tstring value = (dwValue == 1) ? _T("1") : _T("0");
			::WritePrivateProfileString(_T("settings"), _T("RememberSortInfo"), value.c_str(), path.c_str());
		#endif

			m_bRememberSortInfo = !m_bRememberSortInfo;
		}

		dwValue = (::IsDlgButtonChecked(GetHwnd(), IDC_AUPDATE) == BST_CHECKED) ? 1 : 0;
		if (((dwValue == 1) && (m_bAutoUpdate == false)) || ((dwValue == 0) && (m_bAutoUpdate == true)))
		{
		#ifndef PORTABLE
			hKey.SetValue(dwValue, _T("AutoUpdate"));
		#else
			TCHAR szFileName[MAX_PATH];
			::GetModuleFileName(GetApp()->GetInstanceHandle(), szFileName, MAX_PATH);
			File file;
			file.SetPath(szFileName);
			tstring path = file.GetDir() + _T("st-m_settings.ini");

			tstring value = (dwValue == 1) ? _T("1") : _T("0");
			::WritePrivateProfileString(_T("settings"), _T("AutoUpdate"), value.c_str(), path.c_str());
		#endif

			m_bAutoUpdate = !m_bAutoUpdate;
		}

		hKey.Close();
	}	
	else
		throw (CWinException(_T("Access to registry denied")));	

	// Call the base class's OnApply
	CPropertyPage::OnApply(); 
}

void CGeneralPage::OnCancel()
{
	CPropertyPage::OnCancel();
}

BOOL CGeneralPage::OnInitDialog()
{
	CRegKey hKey;
	hKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Startup Manager"), KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
	if (HKEY(hKey) != NULL)
	{		
	#ifndef PORTABLE
		DWORD dwValue = 0;
		if ((SUCCEEDED(hKey.QueryValue(dwValue, _T("RememberSizePos")))) && (dwValue & 1))
		{
			m_bRememberSizePos = true;
			::SendDlgItemMessage(GetHwnd(), IDC_SIZEPOS, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
		}
		else
		{
			m_bRememberSizePos = false;
			::SendDlgItemMessage(GetHwnd(), IDC_SIZEPOS, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
		}

		dwValue = 0;
		if ((SUCCEEDED(hKey.QueryValue(dwValue, _T("RememberSortInfo")))) && (dwValue & 1))
		{
			m_bRememberSortInfo = true;
			::SendDlgItemMessage(GetHwnd(), IDC_SORTINFO, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
		}
		else
		{
			m_bRememberSortInfo = false;
			::SendDlgItemMessage(GetHwnd(), IDC_SORTINFO, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
		}

		dwValue = 0;
		if ((SUCCEEDED(hKey.QueryValue(dwValue, _T("AutoUpdate")))) && (dwValue & 1))
		{
			m_bAutoUpdate = true;
			::SendDlgItemMessage(GetHwnd(), IDC_AUPDATE, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
		}
		else
		{
			m_bAutoUpdate = false;
			::SendDlgItemMessage(GetHwnd(), IDC_AUPDATE, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
		}
	#else
		TCHAR szFileName[MAX_PATH];
		::GetModuleFileName(GetApp()->GetInstanceHandle(), szFileName, MAX_PATH);
		File file;
		file.SetPath(szFileName);
		tstring path = file.GetDir() + _T("st-m_settings.ini");

		if (::GetPrivateProfileInt(_T("settings"), _T("RememberSizePos"), 2, path.c_str()) == 1)
		{
			m_bRememberSizePos = true;
			::SendDlgItemMessage(GetHwnd(), IDC_SIZEPOS, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
		}
		else
		{
			m_bRememberSizePos = false;
			::SendDlgItemMessage(GetHwnd(), IDC_SIZEPOS, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
		}

		if (::GetPrivateProfileInt(_T("settings"), _T("RememberSortInfo"), 2, path.c_str()) == 1)
		{
			m_bRememberSortInfo = true;
			::SendDlgItemMessage(GetHwnd(), IDC_SORTINFO, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
		}
		else
		{
			m_bRememberSortInfo = false;
			::SendDlgItemMessage(GetHwnd(), IDC_SORTINFO, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
		}

		if (::GetPrivateProfileInt(_T("settings"), _T("AutoUpdate"), 2, path.c_str()) == 1)
		{
			m_bAutoUpdate = true;
			::SendDlgItemMessage(GetHwnd(), IDC_AUPDATE, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
		}
		else
		{
			m_bAutoUpdate = false;
			::SendDlgItemMessage(GetHwnd(), IDC_AUPDATE, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
		}
	#endif
		
		hKey.Close();
	}	
	else
		throw (CWinException(_T("Access to registry denied")));

	return TRUE; 
}

void CGeneralPage::OnOK()
{
	OnApply();

	// Call the base class's OnOK
	CPropertyPage::OnOK(); 
}

BOOL CGeneralPage::OnQueryCancel()
{
	// return TRUE means OK to cancel
	return TRUE; 
}

void CGeneralPage::OnSetActive()
{
	// Set the wizard buttons
	PropSheet_SetWizButtons(m_hWndParent, PSWIZB_NEXT);
}

int CGeneralPage::Validate()
{
	// This is where we validate (and save) the contents of this page before it is closed
	
	// return one of these values:
	// PSNRET_NOERROR. The changes made to this page are valid and have been applied
	// PSNRET_INVALID. The property sheet will not be destroyed, and focus will be returned to this page.
	// PSNRET_INVALID_NOCHANGEPAGE. The property sheet will not be destroyed, and focus will be returned 
	//                               to the page that had focus when the button was pressed.


	int nStatus = PSNRET_NOERROR;
//	int nStatus = PSNRET_INVALID;
//	int nStatus = PSNRET_INVALID_NOCHANGEPAGE;

// Tell the user what went wrong
	if (nStatus != PSNRET_NOERROR)
		::MessageBox(m_hWnd, _T("General Page Validation Failed"), _T("PageSheet Check"), MB_OK);
	
	return nStatus;
}

class CAdvancedPage : public CPropertyPage
{
public:
	CAdvancedPage(UINT nIDTemplate, LPCTSTR szTitle);
	virtual ~CAdvancedPage() {}
	virtual BOOL DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void OnApply();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL OnQueryCancel();
	virtual void OnSetActive();
	virtual int Validate();

private:
	bool m_bMsconfig;
	bool m_bUseXSL;
};

CAdvancedPage::CAdvancedPage(UINT nIDTemplate, LPCTSTR szTitle)
//    modified by David Nash
//	: CPropertyPage(IDD_ADVANCEDSETTINGS, tstring(LoadString(SETTINGSDLG_STR_TITLEP2)).c_str()),
	: CPropertyPage(nIDTemplate, szTitle), m_bMsconfig(false), m_bUseXSL(false)
{
}

BOOL CAdvancedPage::DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	// on any command notification, tell the property sheet to enable the Apply button
	case WM_COMMAND:
		PropSheet_Changed(GetParent(hWnd), hWnd);
		break;

	default:
		break;
	}

	return DialogProcDefault(hWnd, uMsg, wParam, lParam);
}

void CAdvancedPage::OnApply()
{
	tstring disRKey = _T("");
	tstring disFKey = _T("");
	
	//some settings changed, apply the changes
	if (::IsDlgButtonChecked(GetHwnd(), IDC_DMSCONFIG) == BST_CHECKED)
	{
		//set disabled item location to msconfig key
		disRKey = _T("SOFTWARE\\Microsoft\\Shared Tools\\MSConfig\\startupreg");
		disFKey = _T("SOFTWARE\\Microsoft\\Shared Tools\\MSConfig\\startupfolder");
	}
	else if (::IsDlgButtonChecked(GetHwnd(), IDC_DPRIVATE) == BST_CHECKED)
	{
		DWORD dwValue = (::IsDlgButtonChecked(GetHwnd(), IDC_DMSCONFIG) == BST_CHECKED) ? 1 : 0;
		if (((dwValue == 1) && (m_bMsconfig == false)) || ((dwValue == 0) && (m_bMsconfig == true)))
		{
			//warn that disabling items will no longer be compatible with disabled
			//items in the msconfig private key
			tstring str_information = LoadString(MSG_INFORMATION);
			::MessageBox(GetHwnd(), LoadString(DLGSETTINGS_MSG_PRIVATE), str_information.c_str(), MB_OK | MB_ICONINFORMATION);
		}
		
		//set disabled item location to startup manager private key
		disRKey = _T("SOFTWARE\\Startup Manager\\startupreg");
		disFKey = _T("SOFTWARE\\Startup Manager\\startupfolder");
	}
	else
		throw (CWinException(_T("No checkbutton checked...")));
	
	//save changes to startup manager registry key
	CRegKey hKey;
	hKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Startup Manager"), KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
	if (HKEY(hKey) != NULL)
	{
		DWORD dwValue = (::IsDlgButtonChecked(GetHwnd(), IDC_DMSCONFIG) == BST_CHECKED) ? 1 : 0;
		if (((dwValue == 1) && (m_bMsconfig == false)) || ((dwValue == 0) && (m_bMsconfig == true)))
		{
			if (hKey.SetValue(disRKey.c_str(), _T("dRegLocation")) != ERROR_SUCCESS)
			{
				hKey.Close();
				throw (CWinException(_T("Write access to registry denied")));
			}
			if (hKey.SetValue(disFKey.c_str(), _T("dFolderLocation")) != ERROR_SUCCESS)
			{
				hKey.Close();
				throw (CWinException(_T("Write access to registry denied")));
			}

			m_bMsconfig = !m_bMsconfig;
		}

		dwValue = (::IsDlgButtonChecked(GetHwnd(), IDC_USEXSL) == BST_CHECKED) ? 1 : 0;
		if (((dwValue == 1) && (m_bUseXSL == false)) || ((dwValue == 0) && (m_bUseXSL == true)))
		{
		#ifndef PORTABLE
			hKey.SetValue(dwValue, _T("UseXSL"));
		#else
			TCHAR szFileName[MAX_PATH];
			::GetModuleFileName(GetApp()->GetInstanceHandle(), szFileName, MAX_PATH);
			File file;
			file.SetPath(szFileName);
			tstring path = file.GetDir() + _T("st-m_settings.ini");

			tstring value = (::IsDlgButtonChecked(GetHwnd(), IDC_USEXSL) == BST_CHECKED) ? _T("1") : _T("0");
			::WritePrivateProfileString(_T("settings"), _T("UseXSL"), value.c_str(), path.c_str());

			m_bUseXSL = !m_bUseXSL;
		#endif
		}

		hKey.Close();
	}	
	else
		throw (CWinException(_T("Access to registry denied")));

	// Call the base class's OnApply
	CPropertyPage::OnApply(); 
}

void CAdvancedPage::OnCancel()
{ 
	CPropertyPage::OnCancel();
}

BOOL CAdvancedPage::OnInitDialog()
{
	CRegKey hKey;
	hKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Startup Manager"), KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
	if (HKEY(hKey) != NULL)
	{
		tstring disKey;
		TCHAR szValue[256] = {0};
		DWORD dwValueLen = 256*sizeof(TCHAR);
		hKey.QueryValue(szValue, _T("dRegLocation"), &dwValueLen);
		if (_tcslen(szValue) > 0)
		{
			disKey = szValue;
			if (disKey == _T("SOFTWARE\\Microsoft\\Shared Tools\\MSConfig\\startupreg"))
			{
				::SendDlgItemMessage(GetHwnd(), IDC_DMSCONFIG, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
				m_bMsconfig = true;
			}
			else if (disKey == _T("SOFTWARE\\Startup Manager\\startupreg"))
			{
				::SendDlgItemMessage(GetHwnd(), IDC_DPRIVATE, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
				
				//in win2k there's no msconfig
				if (GetWindowsVersion().find(_T("Windows 2000")) != std::string::npos)
					::EnableWindow(::GetDlgItem(GetHwnd(), IDC_DMSCONFIG), FALSE);
			}
			else
				throw (CWinException(_T("Unknown disabled items location!")));
		}
		else
		{
			hKey.Close();
			throw (CWinException(_T("Unable to retrieve disabled item location!")));	
		}
		
	#ifndef PORTABLE
		DWORD dwValue = 0;
		if ((SUCCEEDED(hKey.QueryValue(dwValue, _T("UseXSL")))) && (dwValue & 1))
		{
			m_bUseXSL = true;
			::SendDlgItemMessage(GetHwnd(), IDC_USEXSL, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
		}
		else
		{
			m_bUseXSL = false;
			::SendDlgItemMessage(GetHwnd(), IDC_USEXSL, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
		}
	#else
		TCHAR szFileName[MAX_PATH];
		::GetModuleFileName(GetApp()->GetInstanceHandle(), szFileName, MAX_PATH);
		File file;
		file.SetPath(szFileName);
		tstring path = file.GetDir() + _T("st-m_settings.ini");

		if (::GetPrivateProfileInt(_T("settings"), _T("UseXSL"), 2, path.c_str()) == 1)
		{
			m_bUseXSL = true;
			::SendDlgItemMessage(GetHwnd(), IDC_USEXSL, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
		}
		else
		{
			m_bUseXSL = false;
			::SendDlgItemMessage(GetHwnd(), IDC_USEXSL, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
		}
	#endif
		
		hKey.Close();
	}	
	else
		throw (CWinException(_T("Access to registry denied")));

	return TRUE; 
}

void CAdvancedPage::OnOK()
{
	OnApply();

	// Call the base class's OnOK
	CPropertyPage::OnOK(); 
}

BOOL CAdvancedPage::OnQueryCancel()
{
	// return TRUE means OK to cancel
	return TRUE; 
}

void CAdvancedPage::OnSetActive()
{
	// Set the wizard buttons
	PropSheet_SetWizButtons(m_hWndParent, PSWIZB_NEXT);
}

int CAdvancedPage::Validate()
{
	// This is where we validate (and save) the contents of this page before it is closed
	
	// return one of these values:
	// PSNRET_NOERROR. The changes made to this page are valid and have been applied
	// PSNRET_INVALID. The property sheet will not be destroyed, and focus will be returned to this page.
	// PSNRET_INVALID_NOCHANGEPAGE. The property sheet will not be destroyed, and focus will be returned 
	//                               to the page that had focus when the button was pressed.


	int nStatus = PSNRET_NOERROR;
//	int nStatus = PSNRET_INVALID;
//	int nStatus = PSNRET_INVALID_NOCHANGEPAGE;

// Tell the user what went wrong
	if (nStatus != PSNRET_NOERROR)
		::MessageBox(m_hWnd, _T("Advanced Page Validation Failed"), _T("PageSheet Check"), MB_OK);
	
	return nStatus;
}

SettingsDlg::SettingsDlg(HWND hWndParent) : CPropertySheet(SETTINGSDLG_STR_TITLE, hWndParent)
{
	AddPage(new CGeneralPage(IDD_GENERALSETTINGS, LoadString(SETTINGSDLG_STR_TITLEP1)));
	AddPage(new CAdvancedPage(IDD_ADVANCEDSETTINGS, LoadString(SETTINGSDLG_STR_TITLEP2)));
}

LRESULT SettingsDlg::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
//	switch (uMsg)
//	{
//
//	}

	// pass unhandled messages on for default processing
	return WndProcDefault(hWnd, uMsg, wParam, lParam);	
}
