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

#include "..\Win32++\frame.h"
#include "resource.h"
#include "FrameApp.h"
#include "Boot\File.h"

#ifdef PORTABLE
	#include "types/tstring.h"
	#include "types/tfstream.h"
	#include "boot/regkey.h"
	#include "boot/wintools.h"

	void CreateRegistryEntries()
	{
		CRegKey hKey;
		hKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Startup Manager"), KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
		if (!hKey.IsOpen())
		{
			CRegKey hKeyNew;
			hKeyNew.Create(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Startup Manager"), REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
			hKeyNew.SetValue(_T(""), _T("dFolderLocation"));
			hKeyNew.SetValue(_T(""), _T("dRegLocation"));
			hKeyNew.Create(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Startup Manager\\startupfolder"), REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
			hKeyNew.Create(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Startup Manager\\startupreg"), REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
			hKeyNew.Create(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Startup Manager\\startupwinini"), REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
			hKeyNew.Create(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Startup Manager\\startupwinlogon"), REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
		}
	}

	tstring GetFolder(UINT nID)
	{
		IMalloc *gpMalloc;
		SHGetMalloc(&gpMalloc);
		
		TCHAR tmp[MAX_PATH];
		SHGetSpecialFolderPath(0, tmp, nID, 0);
		tstring path = tmp;
	
		gpMalloc->Release();

		return path;
	}

	void CreateDirectories()
	{
		tstring strFolder = _T("");
		
		TCHAR buffer[100];
		::GetEnvironmentVariable(_T("OS"), buffer, 100);
		if (_tcscmp(_T("Windows_NT"), buffer) == 0)
			strFolder = GetFolder(CSIDL_COMMON_APPDATA);
		else
			strFolder = GetFolder(CSIDL_COMMON_DOCUMENTS);

		::CreateDirectory((strFolder + _T("\\")).c_str(), 0);
		::CreateDirectory((strFolder + _T("\\Startup Manager\\")).c_str(), 0);
		::CreateDirectory((strFolder + _T("\\Startup Manager\\batch\\")).c_str(), 0);
	}

	void CreateIniFile(HINSTANCE hInstance)
	{
		TCHAR szFileName[MAX_PATH];
		::GetModuleFileName(hInstance, szFileName, MAX_PATH);
		File file;
		file.SetPath(szFileName);
		tstring path = file.GetDir() + _T("st-m_settings.ini");

		tofstream iniFile(toNarrowString(path).c_str(), std::ios::app);
		if (iniFile)
			iniFile.close();

		if (::GetPrivateProfileInt(_T("settings"), _T("RememberSizePos"), 2, path.c_str()) == 2)
			::WritePrivateProfileString(_T("settings"), _T("RememberSizePos"), _T("1"), path.c_str());

		if (::GetPrivateProfileInt(_T("settings"), _T("RememberSortInfo"), 2, path.c_str()) == 2)
			::WritePrivateProfileString(_T("settings"), _T("RememberSortInfo"), _T("1"), path.c_str());

		if (::GetPrivateProfileInt(_T("settings"), _T("AutoUpdate"), 2, path.c_str()) == 2)
			::WritePrivateProfileString(_T("settings"), _T("AutoUpdate"), _T("1"), path.c_str());

		if (::GetPrivateProfileInt(_T("settings"), _T("UseXSL"), 2, path.c_str()) == 2)
			::WritePrivateProfileString(_T("settings"), _T("UseXSL"), _T("1"), path.c_str());
	}
#endif

HINSTANCE CStartupMgrApp::GetResourceInstance(HINSTANCE hInstance)
{
#ifdef UNICODE
	TCHAR szFileName[MAX_PATH];
	::GetModuleFileName(hInstance, szFileName, MAX_PATH);
	File file;
	file.SetPath(szFileName);
	tstring path = file.GetDir() + _T("Language.dll");
	HINSTANCE hTranslation = ::LoadLibrary(path.c_str());
	return (hTranslation ? hTranslation : hInstance);
#else
	return hInstance;
#endif
}

CStartupMgrApp::CStartupMgrApp()
{
}

BOOL CStartupMgrApp::InitInstance()
{
	//Start Tracing
	TRACE(_T("Tracing started"));

#ifdef PORTABLE
	//CreateRegistryEntries();
	CreateDirectories();
	CreateIniFile(GetInstanceHandle());
#endif

	//Load language specific resources
	SetResourceHandle(GetResourceInstance(GetInstanceHandle()));

	m_pFrame = new CMainFrame();
	m_pFrame->SetFrameMenu(IDW_MAIN);

	//Create the Frame Window
	m_pFrame->Create();

	//End the program if the frame window creation fails
	if (!m_pFrame->GetHwnd())
	{
		::MessageBox(0, _T("Failed to create Frame window"), _T("ERROR"), MB_ICONERROR);
		
		// returning FALSE ends the application
		return FALSE;
	}

	return TRUE;
}
