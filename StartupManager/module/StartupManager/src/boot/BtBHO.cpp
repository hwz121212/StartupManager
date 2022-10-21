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

/// @file BtBHO.cpp Declarations for the Browser Helper Objects section classes.

#include "BtBHO.h"

namespace Boot
{

	namespace BHO
	{

		BOOL Section::IsReadItemDisabled(LPCTSTR szKeyName) throw (Boot::Exception)
		{
			try
			{
				//Open BHO subkey (CLSID)
				tstring subkey = m_Regkey + _T("\\");
				subkey = subkey + szKeyName;
				CRegKey hKey;
				hKey.Open(m_Root, subkey.c_str(), KEY_READ | m_dwHiveFlag);
				if (hKey.IsOpen())
				{
					BOOL result = FALSE;
					DWORD dwDisabled = 0;

					if (SUCCEEDED(hKey.QueryValue(dwDisabled, _T("NoExplorer"))) && (dwDisabled & 1))
						result = TRUE;
					else
						result = FALSE;

					return result;
				}
				else
					throw Exception(_T("Couldn't open BHO subkey"));
			}
			
			catch (Boot::Exception const&)
			{
				throw;	// re-throw
			}
			
			catch (...)
			{
				throw Exception(_T("Boot::BHO::Section::ReadSubkey... Unknown error"));
			}
		}

		void Section::InsertReadItem(LPCTSTR szCLSID, LPCTSTR szProgramPath, LPCTSTR szValueName) throw (Boot::Exception)
		{
			try
			{
				if (_tcslen(szValueName) <= 0)
					_tcscpy(const_cast<LPTSTR>(szValueName), szCLSID);

				//add item to list
				Item *item = new Item(this);
				item->SetCLSID(szCLSID);
				item->SetiName(szValueName);
				item->SetPath(szProgramPath);
				
				DWORD type = Item::attrErrNotInit;

				try
				{
					type = (IsReadItemDisabled(szCLSID) == TRUE) ? Item::attrDisabled : Item::attrRegular;
				}

				catch (...)
				{
					item->SetAttributes(Item::attrErrCorrupt);
					InsertAfter(item);

					throw;	// re-throw
				}

				if ((_tcslen(szValueName) <= 0) || (_tcslen(szProgramPath) <= 0))
					item->SetAttributes(type | Item::attrErrCorrupt);
				else
					item->SetAttributes(type);

				InsertAfter(item);
				if (GetNode() != item)
				{
					delete item;
					throw Exception(_T("Failed to add item to list"));
				}
			}

			catch (Boot::Exception const&)
			{
				throw;	// re-throw
			}

			catch (...)
			{
				throw Exception(_T("Unknown error"));
			}
		}

		void Section::LookUpCLSID(LPCTSTR szCLSID) throw (Boot::Exception)
		{
			try
			{
				if (_tcslen(szCLSID) <= 0)
					throw Exception(_T("Invalid CLSID"));
					
				DWORD dwValueNameLen = 256;
				TCHAR *szValueName = new TCHAR[dwValueNameLen];
				_tcscpy(szValueName, _T(""));
				DWORD dwProgramPathLen = MAX_PATH;
				TCHAR *szProgramPath = new TCHAR[dwProgramPathLen];
				_tcscpy(szProgramPath, _T(""));

				//Open CLSID key
				CRegKey hKey;
				tstring CLSIDkey = _T("SOFTWARE\\Classes\\CLSID\\");
				CLSIDkey = CLSIDkey + szCLSID;
				hKey.Open(m_Root, CLSIDkey.c_str(), KEY_READ | m_dwHiveFlag);
				if (hKey.IsOpen())
				{
					if (hKey.QueryValue(szValueName, _T(""), &dwValueNameLen) != ERROR_SUCCESS)
						_tcscpy(szValueName, _T(""));

					//Open CLSID InprocServer32 subkey
					CRegKey hSubKey;
					hSubKey.Open(hKey, _T("InprocServer32"), KEY_READ | m_dwHiveFlag);
					if (hSubKey.IsOpen())
						hSubKey.QueryValue(szProgramPath, _T(""), &dwProgramPathLen);

					try
					{
						InsertReadItem(szCLSID, szProgramPath, szValueName);
					}

					catch (...)
					{
						delete [] szValueName;
						delete [] szProgramPath;

						throw;	// re-throw
					}

					delete [] szValueName;
					delete [] szProgramPath;
				}
				else
				{
					try
					{
						InsertReadItem(szCLSID, szProgramPath, szValueName);
					}

					catch (...)
					{
						delete [] szValueName;
						delete [] szProgramPath;

						throw;	// re-throw
					}

					delete [] szValueName;
					delete [] szProgramPath;
				}
			}

			catch (Boot::Exception const&)
			{
				throw;	// re-throw
			}

			catch (...)
			{
				throw Exception(_T("Boot::BHO::Section::LookUpCLSID... Unknown error"));
			}
		}

		void Section::Read()
		{
			try
			{
				assert(ProperlyInitialized());

				//Open BHO key
				CRegKey hKey;
				hKey.Open(m_Root, m_Regkey.c_str(), KEY_READ | m_dwHiveFlag);
				if (hKey.IsOpen())
				{
					DWORD dwKeyNameLen = 256*sizeof(TCHAR);
					TCHAR *szKeyName = new TCHAR[dwKeyNameLen/sizeof(TCHAR)];

					//read items from registry key
					LRESULT lResult = ERROR_SUCCESS;
					int i = 0;
					while ((lResult = ::RegEnumKeyEx(hKey, i, szKeyName, &dwKeyNameLen, 0, 0, 0, 0)) != ERROR_NO_MORE_ITEMS)
					{
						switch (lResult)
						{
						case ERROR_SUCCESS:
							try
							{
								LookUpCLSID(szKeyName);
							}

							catch (Boot::Exception const&)
							{
								//We don't want an exception for one item to break others,
								//so we just continue...
							}

							//reset size to default
							dwKeyNameLen = 256*sizeof(TCHAR);
							delete [] szKeyName;
							szKeyName = new TCHAR[dwKeyNameLen/sizeof(TCHAR)];

							i++;
							break;

						case ERROR_MORE_DATA:
							//allocate more space and try again!
							dwKeyNameLen = 2*dwKeyNameLen;
							delete [] szKeyName;
							szKeyName = new TCHAR[dwKeyNameLen/sizeof(TCHAR)];
							break;

						case ERROR_NO_MORE_ITEMS:
							//we're done
							break;

						default:
							delete [] szKeyName;
							throw Boot::Exception(_T("RegEnumKeyEx failed"));
							break;
						}
					}
					
					if (szKeyName)
						delete [] szKeyName;
				}
				else
					throw Exception(_T("Couldn't open BHO key"));
			}

			catch (Boot::Exception const&)
			{
				// Ignore...
			}
			
			catch (...)
			{
				throw;	// re-throw
			}
		}
		
		void Section::Add(Boot::Item *item) throw (Boot::Exception)
		{
			try
			{
				assert(ProperlyInitialized());
				assert(dynamic_cast<Item *>(item) != 0);
			
				tstring CLSID = dynamic_cast<Item*>(item)->GetCLSID().c_str();

				CRegKey hKey;
				tstring CLSIDkey = _T("SOFTWARE\\Classes\\CLSID\\") + CLSID;
				hKey.Open(m_Root, CLSIDkey.c_str(), KEY_READ | m_dwHiveFlag);
				if (!hKey.IsOpen())
					throw Exception(_T("Invalid CLSID"));

				//open registry key
				hKey.Open(m_Root, m_Regkey.c_str(), KEY_CREATE_SUB_KEY | m_dwHiveFlag);
				if (hKey.IsOpen())
				{
					//create new BHO subkey with item CLSID
					CRegKey hSubKey;
					if (hSubKey.Create(hKey, CLSID.c_str(), REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | m_dwHiveFlag) != ERROR_SUCCESS)
						throw Exception(_T("Couldn't create BHO subkey"));
				
					//add item to list
					LookUpCLSID(CLSID.c_str());

					delete item;
					item = GetNode();
				}
				else
					throw Exception(_T("Couldn't open registry key"));
			}

			catch (Boot::Exception const&)
			{
				throw;	// re-throw
			}
			
			catch (...)
			{
				throw Exception(_T("Boot::BHO::Section::Add... Unkown error"));
			}
		}

		void Section::Delete(Boot::Item *item) throw (Boot::Exception)
		{
			try
			{
				assert(ProperlyInitialized());
				assert(dynamic_cast<Item *>(item) != 0);
			
				//Open BHO key
				CRegKey hKey;
				hKey.Open(m_Root, m_Regkey.c_str(), KEY_ALL_ACCESS | m_dwHiveFlag);
				if (hKey.IsOpen())
				{
					if (hKey.DeleteSubKey(dynamic_cast<Item*>(item)->GetCLSID().c_str()) != ERROR_SUCCESS)
						throw Exception(_T("Unable to delete item from registry (access denied)"));
				}
				else
					throw Exception(_T("Couldn't open BHO key"));

				//remove item from list
				List::Delete();
			}

			catch (Boot::Exception const&)
			{
				throw;
			}
			
			catch (...)
			{
				throw Exception(_T("Boot::BHO::Section::Delete... Unkown error"));
			}
		}

		void Section::Rename(Boot::Item *item, const tstring &newName) throw (Boot::Exception)
		{
			try
			{
				assert(ProperlyInitialized());
				assert(dynamic_cast<Item *>(item) != 0);

				//Open BHO subkey
				CRegKey hKey;
				tstring CLSIDkey = _T("SOFTWARE\\Classes\\CLSID\\");
				CLSIDkey = CLSIDkey + dynamic_cast<Item*>(item)->GetCLSID().c_str();
				hKey.Open(m_Root, CLSIDkey.c_str(), KEY_ALL_ACCESS | m_dwHiveFlag);
				if (hKey.IsOpen())
				{
					if (hKey.SetValue(newName.c_str()) != ERROR_SUCCESS)
						throw Exception(_T("Unable to rename item (access to registry denied)"));

					item->SetiName(newName);
				}
				else
					throw Exception(_T("Couldn't open CLSID subkey"));
			}

			catch (Boot::Exception const&)
			{
				throw;	// re-throw
			}
			
			catch (...)
			{
				throw Exception(_T("Unkown error"));
			}
		}

		void Section::Enable(Boot::Item *& item) throw (Boot::Exception)
		{
			try
			{
				assert(ProperlyInitialized());
				assert(dynamic_cast<Item *>(item) != 0);
				assert(item->IsDisabled());

				//Open BHO subkey
				CRegKey hKey;
				hKey.Open(m_Root, (m_Regkey + _T("\\") + dynamic_cast<Item*>(item)->GetCLSID()).c_str(), KEY_ALL_ACCESS | m_dwHiveFlag);
				if (hKey.IsOpen())
				{
					if (hKey.DeleteValue(_T("NoExplorer")) != ERROR_SUCCESS)
						throw Exception(_T("Unable to disable item (access to registry denied)"));
				}
				else
					throw Exception(_T("Couldn't open BHO subkey"));
			
				item->SetAttributes(item->GetAttributes() & ~Item::attrDisabled);

				assert(item->IsEnabled());
			}

			catch (Boot::Exception const&)
			{
				throw;	// re-throw
			}
			
			catch (...)
			{
				throw Exception(_T("Unkown error"));
			}
		}

		void Section::Disable(Boot::Item *& item) throw (Boot::Exception)
		{
			try
			{
				assert(ProperlyInitialized());
				assert(dynamic_cast<Item *>(item) != 0);
				assert(item->IsEnabled());

				//Open BHO subkey
				CRegKey hKey;
				hKey.Open(m_Root, (m_Regkey + _T("\\") + dynamic_cast<Item*>(item)->GetCLSID()).c_str(), KEY_ALL_ACCESS | m_dwHiveFlag);
				if (hKey.IsOpen())
				{
					DWORD dwValue = 1;
					if (hKey.SetValue(dwValue, _T("NoExplorer")) != ERROR_SUCCESS)
						throw Exception(_T("Unable to disable item (access to registry denied)"));
				}
				else
					throw Exception(_T("Couldn't open BHO subkey"));

				item->SetAttributes(item->GetAttributes() | Item::attrDisabled);

				assert(item->IsDisabled());
			}

			catch (Boot::Exception const&)
			{
				throw;	// re-throw
			}
			
			catch (...)
			{
				throw Exception(_T("Boot::BHO::Section::Disable... Unkown error"));
			}
		}

	} //namespace BHO

} //namespace Boot
