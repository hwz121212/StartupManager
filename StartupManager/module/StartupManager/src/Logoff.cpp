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

#include "Logoff.h"

#include <tchar.h>

CLogoff::CLogoff()
{
	TCHAR buffer[100];
	::GetEnvironmentVariable(_T("OS"), buffer, 100);
	if (_tcscmp(_T("Windows_NT"), buffer) == 0)
		m_bIsWinNT = TRUE;
	else
		m_bIsWinNT = FALSE;
}
 
void CLogoff::EnableShutdownPrivileges()
{
	HANDLE token;
	TOKEN_PRIVILEGES privileges;

	if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
		::MessageBox(0, _T("ERROR: Unable to open process token."), _T("Error"), MB_OK | MB_ICONERROR);

	::LookupPrivilegeValue(0, SE_SHUTDOWN_NAME, &privileges.Privileges[0].Luid);

	privileges.PrivilegeCount = 1;
	privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	::AdjustTokenPrivileges(token, FALSE, &privileges, 0, (PTOKEN_PRIVILEGES)0, 0);
	if (::GetLastError() != ERROR_SUCCESS)
		::MessageBox(0, _T("ERROR: Unanble to adjust token privileges."), _T("Error"), MB_OK | MB_ICONERROR);
}

void CLogoff::DisableShutdownPrivileges()
{
	HANDLE token;
	TOKEN_PRIVILEGES privileges;

	if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
		::MessageBox(0, _T("Unable to open process token."), _T("Error"), MB_OK | MB_ICONERROR);
 
	::LookupPrivilegeValue(0, SE_SHUTDOWN_NAME, &privileges.Privileges[0].Luid);

	privileges.PrivilegeCount = 1;
	privileges.Privileges[0].Attributes = 0;

	::AdjustTokenPrivileges(token, FALSE, &privileges, 0, (PTOKEN_PRIVILEGES)0, 0);
	if (::GetLastError() != ERROR_SUCCESS)
		::MessageBox(0, _T("ERROR: Unanble to adjust token privileges."), _T("Error"), MB_OK | MB_ICONERROR);
}
 
void CLogoff::Logoff()
{
	::ExitWindowsEx(EWX_LOGOFF, 0);
}

void CLogoff::Poweroff()
{
	if (m_bIsWinNT) 
		EnableShutdownPrivileges();
	 
	::ExitWindowsEx(EWX_POWEROFF, 0);
	
	if (m_bIsWinNT)
		DisableShutdownPrivileges();
}

void CLogoff::Restart()
{
	if (m_bIsWinNT)
		EnableShutdownPrivileges();
	 
	::ExitWindowsEx(EWX_REBOOT, 0);
	 
	if (m_bIsWinNT)
		DisableShutdownPrivileges();
}

void CLogoff::Shutdown()
{
	if (m_bIsWinNT)
		EnableShutdownPrivileges();
	 
	::ExitWindowsEx(EWX_SHUTDOWN, 0);
	 
	if (m_bIsWinNT)
		DisableShutdownPrivileges();
}
