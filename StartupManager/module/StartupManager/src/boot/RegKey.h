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

#ifndef REGKEY_H
#define REGKEY_H

/// @file RegKey.h Declaration of a registry wrapper class.

#include <windows.h>

#ifndef KEY_WOW64_64KEY
	#define KEY_WOW64_64KEY   (0x0100L)    ///<	64-bit registry access
#endif
#ifndef KEY_WOW64_32KEY
	#define KEY_WOW64_32KEY   (0x0200L)    ///<	32-bit registry access
#endif

/**
 *	@brief A registry key class.
 *	@details This class forms a wrapper class for the registry
 *	api calls.
 */
class CRegKey
{
public:
	/**
	 *	@name Construction/Destruction
	 *	@{
	 */
	/// The constructor.
	CRegKey();
	/// The destructor.
	~CRegKey();
	/**	@}	<!-- end of the group --> */

	/**
	 *	@name Operations
	 *	@{
	 */
	/**
	 *	@brief Create a registry key.
	 *	@param hKeyParent A handle to the key of which the new
	 *	key will be a child.
	 *	@param lpszKeyName The name of the new key.
	 *	@param lpszClass The type of the new key.
	 *	@param dwOptions Some options.
	 *	@param samDesired The desired access method.
	 *	@param lpSecAttr The desired security attributs.
	 *	@param lpdwDisposition Disposition.
	 *	@return A LONG indicating success or failure.
	 */
	LONG Create(HKEY hKeyParent, LPCTSTR lpszKeyName, LPTSTR lpszClass = REG_NONE,
		DWORD dwOptions = REG_OPTION_NON_VOLATILE, REGSAM samDesired = KEY_ALL_ACCESS,
		LPSECURITY_ATTRIBUTES lpSecAttr = 0, LPDWORD lpdwDisposition = 0);
	/**
	 *	@brief Open a registry key.
	 *	@param hKeyParent A handle to the parent key.
	 *	@param lpszKeyName The name of the key to open.
	 *	@param samDesired The desired access method.
	 *	@return A LONG indicating success or failure.
	 */
	LONG Open(HKEY hKeyParent, LPCTSTR lpszKeyName, REGSAM samDesired = KEY_ALL_ACCESS);
	/**
	 *	@brief Close the current key.
	 *	@see Attach()
	 *	@see Detach()
	 *	@return A LONG indicating success or failure.
	 */
	LONG Close();
	/**
	 *	@brief Attach a handle to an open key.
	 *	@param hKey The handle to the key to attach.
	 *	@see Close()
	 *	@see Detach()
	 */
	void Attach(HKEY hKey);
	/**
	 *	@brief Detach an open key.
	 *	@see Close()
	 *	@see Attach()
	 *	@return A handle to the open key.
	 */
	HKEY Detach();
	/**
	 *	@brief Set/Create a registry value of the type REG_SZ
	 *	(string)
	 *	@param lpszValue The value to set.
	 *	@param lpszValueName The name of the registry value.
	 *	@return A LONG indicating success or failure.
	 */
	LONG SetValue(LPCTSTR lpszValue, LPCTSTR lpszValueName = 0);
	/**
	 *	@brief Set/Create a registry value of the type
	 *	REG_DWORD (DWORD)
	 *	@param dwValue The value to set.
	 *	@param lpszValueName The name of the registry value.
	 *	@return A LONG indicating success or failure.
	 */
	LONG SetValue(DWORD dwValue, LPCTSTR lpszValueName);
	/**
	 *	@brief Delete a registry value.
	 *	@param lpszValue The value to delete.
	 *	@return A LONG indicating success or failure.
	 */
	LONG DeleteValue(LPCTSTR lpszValue);
	/**
	 *	@brief Retrieve a DWORD value from the registry.
	 *	@param dwValue (out) The value.
	 *	@param lpszValueName The name of the value to
	 *	retrieve.
	 *	@return A LONG indicating success or failure.
	 */
	LONG QueryValue(DWORD &dwValue, LPCTSTR lpszValueName);
	/**
	 *	@brief Retrieve a string value from the registry.
	 *	@param szValue (out) The value.
	 *	@param lpszValueName The name of the value to
	 *	retrieve.
	 *	@param pdwCount A pointer to a DWORD that
	 *	contains the size of the value.
	 *	@return A LONG indicating success or failure.
	 */
	LONG QueryValue(LPTSTR szValue, LPCTSTR lpszValueName, DWORD *pdwCount);
	/**
	 *	@brief Delete a subkey of the currently open key.
	 *	@param lpszSubKey The name of the subkey to delete.
	 *	@see RecurseDeleteKey()
	 *	@return A LONG indicating success or failure.
	 */
	LONG DeleteSubKey(LPCTSTR lpszSubKey);
	/**
	 *	@brief Recursively delete a subkey of the currently
	 *	open key.
	 *	@param lpszKey The name of the subkey to delete.
	 *	@see DeleteSubKey()
	 *	@return A LONG indicating success or failure.
	 */
	LONG RecurseDeleteKey(LPCTSTR lpszKey);
	/**	@}	<!-- end of the group --> */

	/// Check whether the key is open.
	bool IsOpen() const;

	/// Convert the object to a HKEY.
	operator HKEY() const;

private:
	REGSAM m_Sam;	///< The access method used to open the key.
	HKEY m_hKey;	///< A handle to the opened key.
};

inline bool CRegKey::IsOpen() const
{
	return (m_hKey != NULL);
}

inline CRegKey::operator HKEY() const
{
	return m_hKey;
}

#endif	// REGKEY_H
