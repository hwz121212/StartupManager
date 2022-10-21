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
 
#ifndef LOGDLG_H
#define LOGDLG_H

#include "..\Win32++\dialog.h"
#include "ListView.h"
#include "Boot\File.h"
#include "ToolTip.h"
#include "types\utf16fstream.h"


class LogDlg : public CDialog
{
public:
    LogDlg(HWND hWndParent = 0);
    virtual ~LogDlg() {}
      
private:
	void GetLogPath();
	void ReadLogAsASCII(const tstring &file);
#ifdef UNICODE
	void ReadLogAsUTF16LE(utf16ifstream &file);
#endif
	void ReadLog();
    virtual BOOL OnInitDialog();
	void OnClear();
    virtual BOOL DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	int m_nCount;
	Boot::File m_Log;
	CListView m_List;
	CToolTip m_TT;
};

#endif //LOGDLG_H
