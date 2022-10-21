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

/// @file RegKey.cpp Definition of a registry wrapper class.

#include "RegKey.h"

#include <cassert>

namespace
{
	/******************************************************************************/
	/*   Replacement for the RegDeleteKeyEx function which is only available      */
	/*   for windows xp x64, windows server 2003 and windows vista (& longhorn)   */
	/******************************************************************************/
	LONG RegDeleteKeyEx(HKEY hKey, LPCTSTR lpSubKey, REGSAM samDesired)
	{
		// Load the Common Controls DLL
		HMODULE hAdvapi = ::LoadLibraryA("ADVAPI32.DLL");
		if (!hAdvapi)
			return ERROR_FILE_NOT_FOUND;

		// Declare a pointer to function
		LONG (STDAPICALLTYPE* pfnInit) (HKEY, LPCTSTR, REGSAM, DWORD) = 0;

		// Store the address of the RegDeleteKeyEx function
	#ifdef _UNICODE
		(FARPROC&)pfnInit = ::GetProcAddress(hAdvapi, "RegDeleteKeyExW");
	#else
		(FARPROC&)pfnInit = ::GetProcAddress(hAdvapi, "RegDeleteKeyExA");
	#endif

		LONG hResult = ERROR_SUCCESS;
		if (!pfnInit)
		{
			// Can't call RegDeleteKeyEx, so call RegDeleteKey instead
			hResult = ::RegDeleteKey(hKey, lpSubKey);
		}
		else
		{
			// Call RegDeleteKeyEx
			hResult = ((*pfnInit)(hKey, lpSubKey, samDesired, 0));
		}

		::FreeLibrary(hAdvapi);

		return hResult;
	}
	/******************************************************************************/
}

CRegKey::CRegKey()
{
	m_Sam = 0;
	m_hKey = 0;
}

CRegKey::~CRegKey()
{
	if (m_hKey != 0)
		::RegCloseKey(m_hKey);
}

LONG CRegKey::Create(HKEY hKeyParent, LPCTSTR lpszKeyName, LPTSTR lpszClass, DWORD dwOptions, 
					REGSAM samDesired, LPSECURITY_ATTRIBUTES lpSecAttr, LPDWORD lpdwDisposition)
{
	if (m_hKey != 0)
		Close();
	
	m_Sam = samDesired;
	return ::RegCreateKeyEx(hKeyParent, lpszKeyName, 0, lpszClass, dwOptions, m_Sam, lpSecAttr, &m_hKey, lpdwDisposition);
}

LONG CRegKey::Open(HKEY hKeyParent, LPCTSTR lpszKeyName, REGSAM samDesired)
{
	if (m_hKey != 0)
		Close();
	
	m_Sam = samDesired;
	return ::RegOpenKeyEx(hKeyParent, lpszKeyName, 0, m_Sam, &m_hKey);
}

LONG CRegKey::Close()
{
	LONG hResult = ERROR_SUCCESS;
	if (m_hKey != 0)
		hResult = ::RegCloseKey(m_hKey);
	m_hKey = 0;
	return hResult;
}

void CRegKey::Attach(HKEY hKey)
{
	assert(m_hKey == 0);
	m_hKey = hKey;
}

HKEY CRegKey::Detach()
{
	HKEY retVal = m_hKey;
	if (m_hKey != 0)
		::RegCloseKey(m_hKey);
	m_hKey = 0;
	return retVal;
}

LONG CRegKey::SetValue(LPCTSTR lpszValue, LPCTSTR lpszValueName)
{
	assert(m_hKey != 0);
	assert(lpszValue != 0);
	return ::RegSetValueEx(m_hKey, lpszValueName, 0, REG_SZ, (LPBYTE)lpszValue, ((lstrlen(lpszValue)+1)*sizeof(TCHAR)));
}

LONG CRegKey::SetValue(DWORD dwValue, LPCTSTR lpszValueName)
{
	assert(m_hKey != 0);
	return ::RegSetValueEx(m_hKey, lpszValueName, 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(dwValue));
}

LONG CRegKey::DeleteValue(LPCTSTR lpszValue)
{
	assert(m_hKey != 0);
	return ::RegDeleteValue(m_hKey, lpszValue);
}

LONG CRegKey::QueryValue(DWORD &dwValue, LPCTSTR lpszValueName)
{
	assert(m_hKey != 0);
	DWORD dwValueLen = 256;
	return ::RegQueryValueEx(m_hKey, lpszValueName, 0, 0, (LPBYTE)&dwValue, &dwValueLen);
}

LONG CRegKey::QueryValue(LPTSTR szValue, LPCTSTR lpszValueName, DWORD *pdwCount)
{
	assert(m_hKey != 0);
	assert(*pdwCount > 0);
	
	DWORD length = *pdwCount;
	
	UINT type;
	LONG hResult = ::RegQueryValueEx(m_hKey, lpszValueName, 0, (DWORD*)&type, (LPBYTE)szValue, pdwCount);
	if (hResult != ERROR_SUCCESS)
		return hResult;
	
	if (type == REG_EXPAND_SZ)
	{
		TCHAR *tmpStr = new TCHAR[length/sizeof(TCHAR)];
		::ExpandEnvironmentStrings(szValue, tmpStr, length);
		lstrcpy(szValue, tmpStr);
		delete [] tmpStr;
	}
	
	return hResult;
}

LONG CRegKey::DeleteSubKey(LPCTSTR lpszSubKey)
{
	assert(m_hKey != 0);
	return RegDeleteKeyEx(m_hKey, lpszSubKey, m_Sam);
}

LONG CRegKey::RecurseDeleteKey(LPCTSTR lpszKey)
{
	assert(m_hKey != 0);
	
	CRegKey subkey;
	LONG hResult = ERROR_SUCCESS;
	if ((hResult = subkey.Open(m_hKey, lpszKey, m_Sam)) != ERROR_SUCCESS)
		return hResult;
	
	TCHAR szValue[256];
	DWORD szValueLen = 256*sizeof(TCHAR);
	while (::RegEnumKeyEx(subkey, 0, szValue, &szValueLen, 0, 0, 0, 0) != ERROR_NO_MORE_ITEMS)
	{
		if ((hResult = subkey.RecurseDeleteKey(szValue)) != ERROR_SUCCESS)
		{
			subkey.Close();
			return hResult;
		}
		
		szValueLen = 256*sizeof(TCHAR);
	}
	subkey.Close();
	
	return DeleteSubKey(lpszKey);
}
