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

#include "LogDlg.h"
#include "resource.h"
#include "types\tfstream.h"

#include <cassert>

LogDlg::LogDlg(HWND hWndParent) : CDialog(IDD_LOG, hWndParent)
{
	m_nCount = 0;
}

void LogDlg::GetLogPath()
{
	TCHAR szFileName[MAX_PATH];
	::GetModuleFileName(GetModuleHandle(0), szFileName, MAX_PATH);
	m_Log.SetPath(szFileName);
	tstring path = m_Log.GetDir() + _T("log.log");
	m_Log.SetPath(path);
}

void LogDlg::ReadLogAsASCII(const tstring &file)
{
	tifstream logFile(toNarrowString(file).c_str(), std::ios::in);
	if (logFile.is_open())
	{
		while (!logFile.eof())
		{
			tstring buffer = _T("");
			
			getline(logFile, buffer);
			m_List.AddItem(buffer.c_str(), 0, 0);
			getline(logFile, buffer);
			m_List.AddItem(buffer.c_str(), 0, 1);
			getline(logFile, buffer);
			m_List.AddItem(buffer.c_str(), 0, 2);
			getline(logFile, buffer);
			m_List.AddItem(buffer.c_str(), 0, 3);
			getline(logFile, buffer);
			m_List.AddItem(buffer.c_str(), 0, 4);
			getline(logFile, buffer);
			m_List.AddItem(buffer.c_str(), 0, 5);           

			m_nCount++;
		}
		m_nCount--;

		logFile.close();
	}
	else
		m_List.AddItem(LoadString(DLGLOG_STR_EMPTY), 0, 0);
}

#ifdef UNICODE
void LogDlg::ReadLogAsUTF16LE(utf16ifstream &file)
{
	assert(file.is_open());

	while (!file.eof())
	{
		tstring buffer = _T("");
			
		getline(file, buffer);
		m_List.AddItem(buffer.c_str(), 0, 0);
		getline(file, buffer);
		m_List.AddItem(buffer.c_str(), 0, 1);
		getline(file, buffer);
		m_List.AddItem(buffer.c_str(), 0, 2);
		getline(file, buffer);
		m_List.AddItem(buffer.c_str(), 0, 3);
		getline(file, buffer);
		m_List.AddItem(buffer.c_str(), 0, 4);
		getline(file, buffer);
		m_List.AddItem(buffer.c_str(), 0, 5);           

		m_nCount++;
	}
	m_nCount--;
}
#endif

void LogDlg::ReadLog()
{
#ifdef UNICODE
	utf16ifstream logFile(toNarrowString(m_Log.GetPath()).c_str(), std::ios::in | std::ios::binary);
	if (logFile.is_open())
	{
		//Read off byte order marker
		wchar_t bom;
		if (logFile.read(&bom, 1))
		{
			//Check the BOM - only handling little endian 16-bit
			if (bom != 0xFEFF)
				ReadLogAsASCII(m_Log.GetPath());
			else
				ReadLogAsUTF16LE(logFile);
		}

		logFile.close();

		//remove new line at end of file
		::SendMessage(m_List.GetHwnd(), LVM_DELETEITEM, 0, 0);
	}
#else
	ReadLogAsASCII(m_Log.GetPath());
#endif
          
	if (ListView_GetItemCount(m_List.GetHwnd()) == 0)
		m_List.AddItem(LoadString(DLGLOG_STR_EMPTY), 0, 0);
}

BOOL LogDlg::OnInitDialog()
{
	m_TT.Create(GetHwnd());
	m_TT.Add(LoadString(DLGLOG_TT_IDCLEAR), IDCLEAR);
		
	m_List.Create(GetHwnd());
	m_List.SetExStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	::MoveWindow(m_List.GetHwnd(), 10, 10, 630, 195, TRUE);
	m_List.InsertColumn(LoadString(DLGLOG_STR_COL1), 155, 0);
	m_List.InsertColumn(LoadString(DLGLOG_STR_COL2), 75, 1);
	m_List.InsertColumn(LoadString(DLGLOG_STR_COL3), 75, 2);
	m_List.InsertColumn(LoadString(DLGLOG_STR_COL4), 493, 3);
	m_List.InsertColumn(LoadString(DLGLOG_STR_COL5), 64, 4);
	m_List.InsertColumn(LoadString(DLGLOG_STR_COL6), 64, 5);
	
	GetLogPath();
	ReadLog();
	
	TCHAR items[50];
	_sntprintf(items, sizeof(items)/sizeof(items[0]) - 1, _T("%i %s"), m_nCount, LoadString(DLGLOG_STR_COUNT));
	::SetDlgItemText(GetHwnd(), IDC_NUMBER, items);
		
	::SendMessage(m_List.GetHwnd(), LVM_SETCOLUMNWIDTH, 2, LVSCW_AUTOSIZE_USEHEADER);
	::SendMessage(m_List.GetHwnd(), LVM_SETCOLUMNWIDTH, 4, LVSCW_AUTOSIZE_USEHEADER);
	if (m_nCount > 0)
	{
		::SendMessage(m_List.GetHwnd(), LVM_SETCOLUMNWIDTH, 3, LVSCW_AUTOSIZE);
		::SendMessage(m_List.GetHwnd(), LVM_SETCOLUMNWIDTH, 5, LVSCW_AUTOSIZE);
		int width = static_cast<int>(::SendMessage(m_List.GetHwnd(), LVM_GETCOLUMNWIDTH, 2, 0));
		width = width + static_cast<int>(::SendMessage(m_List.GetHwnd(), LVM_GETCOLUMNWIDTH, 3, 0));  
		::SendMessage(m_List.GetHwnd(), LVM_SETCOLUMNWIDTH, 0, MAKELPARAM(410 - width, 0));
		::EnableWindow(::GetDlgItem(GetHwnd(), IDCLEAR), true);
	}
	else
	{
		::SendMessage(m_List.GetHwnd(), LVM_SETCOLUMNWIDTH, 0, LVSCW_AUTOSIZE);
		::SendMessage(m_List.GetHwnd(), LVM_SETCOLUMNWIDTH, 1, LVSCW_AUTOSIZE_USEHEADER);
		::SendMessage(m_List.GetHwnd(), LVM_SETCOLUMNWIDTH, 3, LVSCW_AUTOSIZE_USEHEADER);
		::SendMessage(m_List.GetHwnd(), LVM_SETCOLUMNWIDTH, 5, LVSCW_AUTOSIZE_USEHEADER);
		::EnableWindow(::GetDlgItem(GetHwnd(), IDCLEAR), false);
	}
		
	return TRUE;
}

void LogDlg::OnClear()
{
	m_List.Clear();
	m_List.AddItem(LoadString(DLGLOG_STR_EMPTY), 0, 0);
	
	TCHAR items[50];
	_sntprintf(items, sizeof(items)/sizeof(items[0]) - 1, _T("0 %s"), LoadString(DLGLOG_STR_COUNT));
	::SetDlgItemText(GetHwnd(), IDC_NUMBER, items);
	::SendMessage(m_List.GetHwnd(), LVM_SETCOLUMNWIDTH, 0, LVSCW_AUTOSIZE);
	::SendMessage(m_List.GetHwnd(), LVM_SETCOLUMNWIDTH, 1, LVSCW_AUTOSIZE_USEHEADER);
	::SendMessage(m_List.GetHwnd(), LVM_SETCOLUMNWIDTH, 2, LVSCW_AUTOSIZE_USEHEADER);
	::SendMessage(m_List.GetHwnd(), LVM_SETCOLUMNWIDTH, 3, LVSCW_AUTOSIZE_USEHEADER);
	::SendMessage(m_List.GetHwnd(), LVM_SETCOLUMNWIDTH, 4, LVSCW_AUTOSIZE_USEHEADER);
	::SendMessage(m_List.GetHwnd(), LVM_SETCOLUMNWIDTH, 5, LVSCW_AUTOSIZE_USEHEADER);
	::EnableWindow(::GetDlgItem(GetHwnd(), IDCLEAR), false);

	_tremove(m_Log.GetPath().c_str());
}

BOOL LogDlg::DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCLEAR:
			OnClear();
			break;
		}
	break;
	} // switch (uMsg)

	//default dialog procedure for other messages
	return CDialog::DialogProc(hwnd, uMsg, wParam, lParam);	
} // BOOL CALLBACK DialogProc(...)
