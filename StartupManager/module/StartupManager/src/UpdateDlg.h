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

#ifndef UPDATEDLG_H
#define UPDATEDLG_H

#include "..\Win32++\dialog.h"
#include "ToolTip.h"
#include "HyperLink.h"
#include "types\tstring.h"
#include "boot\file.h"

#define WM_USER_UPDATEAVAILABLE		WM_USER+1	//update is available
#define WM_USER_NOUPDATE			WM_USER+2	//user already has latest version
#define WM_USER_DOWNLOADFAILED		WM_USER+4	//download failed

class UpdateDlg : public CDialog
{
public:
	class UpdateCheck
	{
	public:
		UpdateCheck(HWND hWnd) : m_hWnd(hWnd) {}
		~UpdateCheck() {}
		tstring Check(const tstring &remoteFile);
		tstring GetCurrentVersion() const {return m_pUpdateData.currentVersion;}
		tstring GetLatestVersion() const {return m_pUpdateData.latestVersion;}
		tstring GetUpdateUrl() const {return m_pUpdateData.updateURL;}

	private:
		struct UPDATEPARAM
		{
			tstring currentVersion;
			tstring latestVersion;
			tstring updateURL;
		};
	
		struct DOWNLOADPARAM
		{
			HWND hWnd;
			tstring strURL;
			tstring strFileName;
			UPDATEPARAM *pUpdateData;
		};

		static void CheckNewVersion(DOWNLOADPARAM *pDlData);
		bool GetCurrentVersion(tstring &currentVersion);
		tstring GetLocation();
		static DWORD WINAPI DownloadThread(LPVOID pParam);
		static int ExtractFirstNumber(tstring &nrStr);
		static bool NewVersionAvailable(tstring currentVersion, tstring latestVersion);
		static bool Download(const tstring &localFile, const tstring &remoteFile);

		DOWNLOADPARAM m_pDlData;
		UPDATEPARAM m_pUpdateData;
		HWND m_hWnd;
	};

    UpdateDlg(HWND hWndParent = 0);
	virtual ~UpdateDlg() {delete m_Check;}
      
private:
    virtual BOOL OnInitDialog();
	HBRUSH OnCtlColorStatic(WPARAM wParam, LPARAM lParam);
	void OnUpdateAvailable();
	void OnNoUpdate();
	void OnDownloadFailed();
	virtual BOOL DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	CToolTip m_TT;
	CHyperlink m_Update;
	bool m_bDownloading;
	UpdateCheck *m_Check;
};

#endif //UPDATEDLG_H
