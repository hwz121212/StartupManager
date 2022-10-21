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

#include "UpdateDlg.h"
#include "resource.h"
#include "types\tfstream.h"
#include "boot\clibraryinfo.h"

#include <wininet.h>

#define TRANSFER_SIZE 100

int UpdateDlg::UpdateCheck::ExtractFirstNumber(tstring &nrStr)
{
	if (nrStr.length() <= 0)
		return 0;
	
	std::string::size_type pos = nrStr.find(_T("."));
	tstring first = nrStr.substr(0, pos);
	nrStr.erase(0, pos+1);
	
	return _ttoi(first.c_str());
}

bool UpdateDlg::UpdateCheck::NewVersionAvailable(tstring currentVersion, tstring latestVersion)
{
	if ((currentVersion.length() <= 0) || (latestVersion.length() <= 0))
		return false;
	
	int cMaj = ExtractFirstNumber(currentVersion);
	int lMaj = ExtractFirstNumber(latestVersion);
	int cMin = ExtractFirstNumber(currentVersion);
	int lMin = ExtractFirstNumber(latestVersion);
	int cRev = ExtractFirstNumber(currentVersion);
	int lRev = ExtractFirstNumber(latestVersion);
	int cBuild = ExtractFirstNumber(currentVersion);
	int lBuild = ExtractFirstNumber(latestVersion);
	
	return ((cMaj < lMaj) || ((cMaj == lMaj) && ((cMin < lMin) || ((cMin == lMin) && ((cRev < lRev)
		|| (cRev == lRev) && (cBuild < lBuild))))));
}

void UpdateDlg::UpdateCheck::CheckNewVersion(DOWNLOADPARAM *pDlData)
{
	tifstream infoFile(toNarrowString(pDlData->strFileName).c_str(), std::ios::in);
	if (!infoFile)
		::PostMessage(pDlData->hWnd, WM_USER_DOWNLOADFAILED, 0, 0);
	
	getline(infoFile, pDlData->pUpdateData->latestVersion);
	getline(infoFile, pDlData->pUpdateData->updateURL);
	
	infoFile.close();
	_tremove(pDlData->strFileName.c_str());
	
	if (pDlData->pUpdateData->latestVersion.length() <= 0)
	{
		pDlData->pUpdateData->latestVersion = _T("0.0.0.0");
		::PostMessage(pDlData->hWnd, WM_USER_DOWNLOADFAILED, 0, 0);
	}
	
	if (NewVersionAvailable(pDlData->pUpdateData->currentVersion, pDlData->pUpdateData->latestVersion))
		::PostMessage(pDlData->hWnd, WM_USER_UPDATEAVAILABLE, 0, 0);
	else
		::PostMessage(pDlData->hWnd, WM_USER_NOUPDATE, 0, 0);
}

bool UpdateDlg::UpdateCheck::GetCurrentVersion(tstring &currentVersion)
{
	TCHAR szFileName[MAX_PATH];
	::GetModuleFileName(GetModuleHandle(0), szFileName, MAX_PATH);
	
	Boot::cLibraryInfo lInfo;
	lInfo.SetDllName(szFileName);
	currentVersion = _T("");
	return lInfo.GetFileVersion(currentVersion);
}

tstring UpdateDlg::UpdateCheck::GetLocation()
{
	TCHAR szFileName[MAX_PATH];
	::GetModuleFileName(GetModuleHandle(0), szFileName, MAX_PATH);
	Boot::File file;
	file.SetPath(szFileName);
	file.SetPath(file.GetDir() + _T("latest_version.txt"));
	return file.GetPath();
}

bool UpdateDlg::UpdateCheck::Download(const tstring &localFile, const tstring &remoteFile)
{
	HINTERNET hInternetRoot = ::InternetOpen(_T("Startup Manager"), INTERNET_OPEN_TYPE_PRECONFIG, 0, 0, 0);
	if (hInternetRoot)
	{
		DWORD dwType;
		if (::InternetGetConnectedState(&dwType, 0))
		{
			//Canonicalization of the URL converts unsafe characters into escape character equivalents
			TCHAR canonicalURL[1024];
			DWORD nSize = 1024;
			::InternetCanonicalizeUrl(remoteFile.c_str(), canonicalURL, &nSize, ICU_BROWSER_MODE);		
			
			DWORD options = INTERNET_FLAG_TRANSFER_BINARY|INTERNET_FLAG_RELOAD;
			HINTERNET hSession = ::InternetOpenUrl(hInternetRoot, canonicalURL, 0, 0, options, 0);
			if (hSession)
			{
				HANDLE hFile;
				BYTE pBuf[TRANSFER_SIZE];
				DWORD dwReadSizeOut, dwTotalReadSize = 0;
				
				hFile = ::CreateFile(localFile.c_str(), GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
				if (hFile != INVALID_HANDLE_VALUE)
				{
					BOOL bRead = FALSE;
					do 
					{
						DWORD dwWriteSize, dwNumWritten;
						bRead = ::InternetReadFile(hSession, pBuf, TRANSFER_SIZE, &dwReadSizeOut);
						dwWriteSize = dwReadSizeOut;
						
						if (bRead && (dwReadSizeOut > 0))
						{
							dwTotalReadSize += dwReadSizeOut;
							::WriteFile(hFile, pBuf, dwWriteSize, &dwNumWritten, 0); 
			
							if (dwWriteSize != dwNumWritten)
							{
								::CloseHandle(hFile);
								::InternetCloseHandle(hSession);
								::InternetCloseHandle(hInternetRoot);
								_tremove(localFile.c_str());
								return false;
							}
						}
						else 
						{
							if (!bRead)
							{
								::CloseHandle(hFile);
								::InternetCloseHandle(hSession);
								::InternetCloseHandle(hInternetRoot);
								_tremove(localFile.c_str());
								return false;
							}
							break;
						}
					} while (bRead);
					
					::CloseHandle(hFile);
					::InternetCloseHandle(hSession);
					::InternetCloseHandle(hInternetRoot);
					return true;
				}
			}
		}
		
		::InternetCloseHandle(hInternetRoot);
	}

	return false;
}

tstring UpdateDlg::UpdateCheck::Check(const tstring &remoteFile)
{
	//set update parameters
	m_pUpdateData.latestVersion = _T("");
	m_pUpdateData.updateURL = _T("");
	if (!GetCurrentVersion(m_pUpdateData.currentVersion))
	{
		::SendMessage(m_hWnd, WM_USER_DOWNLOADFAILED, 0, 0);
		return _T("0.0.0.0");
	}
	
	//set download parameters
	m_pDlData.hWnd = m_hWnd;
	m_pDlData.strURL = remoteFile;
	m_pDlData.strFileName = GetLocation();
	m_pDlData.pUpdateData = &m_pUpdateData;
	
	//create wait thread, this thread takes care of the download thread, we don't wait until this thread is terminated
	DWORD dwThreadId = 0;
	HANDLE dlThread = ::CreateThread(0, 0, UpdateDlg::UpdateCheck::DownloadThread, &m_pDlData, 0, &dwThreadId);
	if (!dlThread)
		::PostMessage(m_hWnd, WM_USER_DOWNLOADFAILED, 0, 0);
	
	//return current version
	return m_pUpdateData.currentVersion;
}

DWORD WINAPI UpdateDlg::UpdateCheck::DownloadThread(LPVOID pParam)
{
	//retrieve download parameters
	DOWNLOADPARAM *const pDownloadParam = static_cast<DOWNLOADPARAM *>(pParam);
	
	//download the requested file to the requested location
	if (Download(pDownloadParam->strFileName.c_str(), pDownloadParam->strURL.c_str()))
		CheckNewVersion(pDownloadParam);
	else
		::PostMessage(pDownloadParam->hWnd, WM_USER_DOWNLOADFAILED, 0, 0);	
	
	return S_OK;
}

UpdateDlg::UpdateDlg(HWND hWndParent) : CDialog(IDD_UPDATE, hWndParent), m_Check(0)
{
	m_bDownloading = false;
}

BOOL UpdateDlg::OnInitDialog()
{
	SetIconLarge(IDW_MAIN);
	SetIconSmall(IDW_MAIN);
	
	m_TT.Create(GetHwnd());
	
	::EnableWindow(::GetDlgItem(GetHwnd(), IDOK), FALSE);
	::SetDlgItemText(GetHwnd(), IDC_INFORMATION, LoadString(DLGUPDATE_STR_CHECKING));
	
	m_Check = new UpdateCheck(GetHwnd());

	m_bDownloading = true;
#ifndef PORTABLE
  #ifdef UNICODE
	tstring currentVersion = m_Check->Check(_T("http://startupmanager.org/versioninfo/st-m_latest_version.txt"));
  #else
	tstring currentVersion = m_Check->Check(_T("http://startupmanager.org/versioninfo/st-m_latest_version_ANSI.txt"));
  #endif
#else
  #ifdef UNICODE
	tstring currentVersion = m_Check->Check(_T("http://startupmanager.org/versioninfo/st-m_latest_version_portable.txt"));
  #else
	tstring currentVersion = m_Check->Check(_T("http://startupmanager.org/versioninfo/st-m_latest_version_portable_ANSI.txt"));
  #endif
#endif
	::SetDlgItemText(GetHwnd(), IDC_CURRENT, currentVersion.c_str());
	
	return TRUE;
}

HBRUSH UpdateDlg::OnCtlColorStatic(WPARAM wParam, LPARAM lParam)
{
	HBRUSH result = (HBRUSH)(LONG_PTR)CDialog::DialogProc(GetHwnd(), WM_CTLCOLORSTATIC, wParam, lParam);
	
	if (((HWND)lParam == ::GetDlgItem(GetHwnd(), IDC_CURRENT)) || ((HWND)lParam == ::GetDlgItem(GetHwnd(), IDC_LATEST)))
	{
		LOGFONT lf;
		HFONT hBoldFont = (HFONT)::SendMessage((HWND)lParam, WM_GETFONT, 0, 0);
		::GetObject(hBoldFont, sizeof(LOGFONT), &lf);
		lf.lfWeight = FW_HEAVY;
		hBoldFont = ::CreateFontIndirect(&lf);
		::SelectObject((HDC)wParam, hBoldFont);
		::DeleteObject(hBoldFont);
	}
	
	return result;
}

void UpdateDlg::OnUpdateAvailable()
{
	m_bDownloading = false;
	::SetDlgItemText(GetHwnd(), IDC_LATEST, m_Check->GetLatestVersion().c_str());
	::SetDlgItemText(GetHwnd(), IDC_INFORMATION, LoadString(DLGUPDATE_STR_YES));
	m_Update.AttachDlgItem(IDC_INFORMATION, this);
	m_Update.SetTarget(m_Check->GetUpdateUrl().c_str());
	::InvalidateRect(m_Update.GetHwnd(), 0, FALSE);
	m_TT.Add(LoadString(DLGUPDATE_TT_IDC_INFO), IDC_INFORMATION);
	::EnableWindow(::GetDlgItem(GetHwnd(), IDOK), TRUE);
}

void UpdateDlg::OnNoUpdate()
{
	m_bDownloading = false;
	::SetDlgItemText(GetHwnd(), IDC_LATEST, m_Check->GetLatestVersion().c_str());
	::SetDlgItemText(GetHwnd(), IDC_INFORMATION, LoadString(DLGUPDATE_STR_NO));
	::EnableWindow(::GetDlgItem(GetHwnd(), IDOK), TRUE);
}

void UpdateDlg::OnDownloadFailed()
{
	m_bDownloading = false;
	tstring msg_information = LoadString(MSG_INFORMATION);
	::MessageBox(GetHwnd(), LoadString(DLGUPDATE_MSG_FAILED), msg_information.c_str(), MB_OK | MB_ICONINFORMATION);
	::EnableWindow(::GetDlgItem(GetHwnd(), IDOK), TRUE);
}

BOOL UpdateDlg::DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CTLCOLORSTATIC:
		return (BOOL)(LONG_PTR)OnCtlColorStatic(wParam, lParam);

	case WM_USER_UPDATEAVAILABLE:
		OnUpdateAvailable();
		break;

	case WM_USER_NOUPDATE:
		OnNoUpdate();
		break;

	case WM_USER_DOWNLOADFAILED:
		OnDownloadFailed();
		break;
	} // switch (uMsg)

	//default dialog procedure for other messages
	return CDialog::DialogProc(hwnd, uMsg, wParam, lParam);	
} // BOOL CALLBACK DialogProc(...)
