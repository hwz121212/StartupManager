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

/// @file BtReg.cpp Definitions for the registry section classes.

#include "BtReg.h"
#include "wintools.h"

//#include <shlwapi.h>
 
namespace Boot
{

	namespace Registry
	{

		void Item::SetPathFlags(const tstring &file, const tstring &flag)
		{
			if (file.length() <= 0)
				if (!(GetAttributes() & attrErrCorrupt))
					SetAttributes(GetAttributes() | attrErrCorrupt);

			SetPath(file);
			m_Flags = flag;
			m_Cmd = GetPath() + ((m_Flags.length() > 0) ? (_T(" ") + m_Flags) : _T(""));
		}
	 
		tstring Item::GetFlags()
		{
			if (m_Flags.length() <= 0)
				SeparateFlags();
			return m_Flags;
		}
	 
		/**************************************************************************/
		/* I don't know how to improve this function, so if you have better code  */
		/* to take care of this, please help me!			                      */
		/**************************************************************************/ 
		bool Item::SeparateFlags()
		{
			if (m_Cmd.length() <= 0)
			{
				//empty command...
				return false;
			}
			trim(m_Cmd);
			
			/*//separate path - flags
			m_Flags = PathGetArgs(m_Cmd.c_str());
			if (GetFlags().length() > 0)
				SetPath(m_Cmd.erase(m_Cmd.find(m_Flags), m_Flags.length()));
			else
				SetPath(m_Cmd);
			
			//eliminate spaces
			if (GetPath().at(path.length()-1) == _T(' '))
				SetPath(GetPath().erase(GetPath().length()-1, 1));
			
			//eliminate quotes
			for (unsigned i = 0; i < GetPath().length(); i++)
				if (GetPath().at(i) == _T('"'))
					SetPath(GetPath().erase(i, 1));
			
			return true;*/
			
			tstring buffer = _T("");

			for (unsigned i = 0; i < m_Cmd.length(); i++)
				buffer += static_cast<TCHAR>(tolower(m_Cmd.at(i)));

			if (buffer.find(_T("rundll32")) != std::string::npos)
			{
				SetPath(_T("Rundll32.exe"));
				m_Flags = m_Cmd.substr(m_Cmd.find(_T(" ")), m_Cmd.length());
				trim(m_Flags);
				return true;
			}
		
			if (buffer.find(_T("regsvr32")) != std::string::npos)
			{
				SetPath(_T("Regsvr32.exe"));
				m_Flags = m_Cmd.substr(m_Cmd.find(_T(" ")), m_Cmd.length());
				trim(m_Flags);
				return true;
			}
		
			buffer = m_Cmd;
		
			// the filepath might be between quotes
			if (buffer.at(0) == _T('"'))
			{
				unsigned Quote = 0;
				int iQuotes = 1;
				for (unsigned i = 0; (i < buffer.length()) && (iQuotes <= 2); i++)
					if (buffer.at(i) == _T('"'))
					{
						buffer.erase(i, 1);
						Quote = i;
						iQuotes++;
					}
		 
				SetPath(buffer.substr(0, Quote));
				m_Flags = buffer.substr(Quote, buffer.length()-GetPath().length());
				trim(m_Flags);
				return true;
			}	 
		
			// if none of the above, try to separate the path and the flags manually			
			std::string::size_type pos = buffer.find_last_of(_T("*.*"));
			if (pos != std::string::npos)
				buffer = buffer.substr(pos);
			else
			{
				// no extension found, thus we are unable to separate the command
				SetPath(buffer);
				m_Flags = _T("");
				return false;
			}
		
			pos = buffer.find_last_of(_T("*-\\/,*"));
			if (pos != std::string::npos)
			{
				if (buffer.at(pos) == _T(' '))
					buffer = buffer.substr(pos-1);
				else
					buffer = buffer.substr(pos);
			}
			else
			{
				pos = buffer.find(_T(" "));
				if (pos != std::string::npos)
					buffer = buffer.substr(pos);
				else
					buffer = _T("");
			}
		 
			m_Flags = buffer;
			trim(m_Flags);
			buffer = m_Cmd;
			SetPath(buffer.erase(buffer.find(m_Flags), m_Flags.length()));
			
			return true;
		}
		/**************************************************************************/
	 
		void Item::SetCommand(const tstring &command)
		{
			if (command.length() <= 0)
				if (!(GetAttributes() & attrErrCorrupt))
					SetAttributes(GetAttributes() | attrErrCorrupt);
			
			m_Cmd = command;
			if (!SeparateFlags())
			{
				// cannot separate path-flags
				SetPath(command);
				m_Flags = _T("");
			}
		}
		
		tstring Item::GetCommand()
		{
			if (m_Cmd.length() > 0)
				return m_Cmd;
			else
			{
				m_Cmd = GetPath();
				if (m_Cmd.length() > 0)
					return m_Cmd;
				else
				{
					if (GetAttributes() & attrErrCorrupt)
						return _T("");
					else
						throw Exception(_T("Item has empty command"));
				}
			}
		}
		
		Section::Section(HKEY root, const tstring &regkey, KEYBITS dwHiveFlag, const tstring &section)
			: Boot::Section(section), m_Root(root), m_Regkey(regkey), m_dwHiveFlag(dwHiveFlag)
		{
			assert((root == HKEY_LOCAL_MACHINE) || (root == HKEY_CURRENT_USER));
			assert(regkey.length() > 0);
			assert(section.length() > 0);

			GetDisabledLocation();
		}
		
		void Section::GetDisabledLocation() throw (Boot::Exception)
		{
			try
			{
				CRegKey hKey;
				hKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Startup Manager"), KEY_ALL_ACCESS | (IsWindows64() ? X64KEY : NORMALKEY));
				if (hKey.IsOpen())
				{
					TCHAR szValue[256] = {0};
					DWORD dwValueLen = 256*sizeof(TCHAR);
					hKey.QueryValue(szValue, _T("dRegLocation"), &dwValueLen);
					if (_tcslen(szValue) > 0)
						m_Diskey = szValue;
					else
					{
						if (GetWindowsVersion().find(_T("Windows 2000")) != std::string::npos)
							m_Diskey = _T("SOFTWARE\\Startup Manager\\startupreg");
						else
							m_Diskey = _T("SOFTWARE\\Microsoft\\Shared Tools\\MSConfig\\startupreg");
						
						if (hKey.SetValue(m_Diskey.c_str(), _T("dRegLocation")) != ERROR_SUCCESS)
							throw Exception(_T("Write access to registry denied"));
					}
				}	
				else
				{
					throw Exception(_T("Access to registry denied, make sure Startup Manager is installed properly."));
				}
			}

			catch (Boot::Exception const&)
			{
				throw;	// re-throw
			}

			catch (...)
			{
				throw Exception(_T("Boot::Registry::Section::GetDisabledLocation... Unknown exception"));
			}
		}
		
		bool Section::ProperlyInitialized()
		{
			return ((GetSection().length() > 0) && (m_Regkey.length() > 0)
				&& ((m_Root == HKEY_LOCAL_MACHINE) || (m_Root == HKEY_CURRENT_USER))
				&& ((m_dwHiveFlag == X64KEY) || (m_dwHiveFlag == X32KEY) || (m_dwHiveFlag == NORMALKEY)));
		}

		void Section::InsertReadItem(LPCTSTR szProgramPath, LPCTSTR szValueName, DWORD type) throw (Boot::Exception)
		{
			try
			{
				//add item to list
				Item *item = new Item(this);
				item->SetiName(szValueName);
					
				if (type == REG_EXPAND_SZ)
				{
					TCHAR szExpanded[MAX_PATH];
					::ExpandEnvironmentStrings(szProgramPath, szExpanded, MAX_PATH);
					item->SetCommand(szExpanded);
				}
				else
					item->SetCommand(szProgramPath);
					
				if ((_tcslen(szValueName) <= 0) || (_tcslen(szProgramPath) <= 0))
					item->SetAttributes(Item::attrErrCorrupt);
				else
					item->SetAttributes(Item::attrRegular);
					
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
				throw Exception(_T("Boot::Registry::Section::InsertReadItem... Unknown error"));
			}
		}

		void Section::Read()
		{
			try
			{
				assert(ProperlyInitialized());
			
				//Open registry key
				CRegKey hKey;
				hKey.Open(m_Root, m_Regkey.c_str(), KEY_READ | m_dwHiveFlag);
				if (hKey.IsOpen())
				{
					TCHAR *szValueName, *szProgramPath;
					DWORD dwValueNameLen = 256*sizeof(TCHAR), dwProgramPathLen = MAX_PATH*sizeof(TCHAR);
					szValueName = new TCHAR[dwValueNameLen/sizeof(TCHAR)];
					szProgramPath = new TCHAR[dwProgramPathLen/sizeof(TCHAR)];
					UINT type;

					//read items from registry key
					LRESULT lResult = ERROR_SUCCESS;
					int i = 0;
					while ((lResult = ::RegEnumValue(hKey, i, szValueName, &dwValueNameLen, 0, (DWORD*)&type, (LPBYTE)szProgramPath, &dwProgramPathLen)) != ERROR_NO_MORE_ITEMS)
					{
						switch (lResult)
						{
						case ERROR_SUCCESS:
							try
							{
								InsertReadItem(szProgramPath, szValueName, type);
							}

							catch (Boot::Exception const&)
							{
								//We don't want an exception for one item to break others,
								//so we just continue...
							}
					
							//reset size to default
							dwValueNameLen	 = 256*sizeof(TCHAR);
							dwProgramPathLen = MAX_PATH*sizeof(TCHAR);
							delete [] szValueName;
							delete [] szProgramPath;
							szValueName = new TCHAR[dwValueNameLen/sizeof(TCHAR)];
							szProgramPath = new TCHAR[dwProgramPathLen/sizeof(TCHAR)];

							i++;
							break;

						case ERROR_MORE_DATA:
							//allocate more space and try again!
							dwValueNameLen	 = 2*dwValueNameLen;
							dwProgramPathLen = 2*dwValueNameLen;
							delete [] szValueName;
							delete [] szProgramPath;
							szValueName = new TCHAR[dwValueNameLen/sizeof(TCHAR)];
							szProgramPath = new TCHAR[dwProgramPathLen/sizeof(TCHAR)];
							break;

						case ERROR_NO_MORE_ITEMS:
							//we're done
							break;

						default:
							delete [] szValueName;
							delete [] szProgramPath;
							throw Exception(_T("RegEnumValue failed"));
							break;
						}
					}
					
					if (szValueName)
						delete [] szValueName;
					if (szProgramPath)
						delete [] szProgramPath;
				}
				else
					throw Exception(_T("Couldn't open registry key"));
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
			
				item->SetAttributes(Item::attrRegular);

				//open registry key
				CRegKey hKey;
				hKey.Open(m_Root, m_Regkey.c_str(), KEY_ALL_ACCESS | m_dwHiveFlag);
				if (hKey.IsOpen())
				{
					//add item to registry key
					if (hKey.SetValue(dynamic_cast<Item *>(item)->GetCommand().c_str(), item->GetiName().c_str()) != ERROR_SUCCESS)
						throw Exception(_T("Write access to registry denied"));

					//add item to list
					InsertAfter(item);
					if (GetNode() != item)
						throw Exception(_T("Failed to add item to list"));
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
				throw Exception(_T("Boot::Registry::Section::Add... Unknown error"));
			}
		}
	 
		void Section::Delete(Boot::Item *item) throw (Boot::Exception)
		{
			try
			{
				assert(ProperlyInitialized());
				assert(dynamic_cast<Item *>(item) != 0);
			
				if (item->IsEnabled())
				{
					//open registry key
					CRegKey hKey;
					hKey.Open(m_Root, m_Regkey.c_str(), KEY_ALL_ACCESS | m_dwHiveFlag);
					if (hKey.IsOpen())
					{
						//remove item from registry
						if (hKey.DeleteValue(item->GetiName().c_str()) != ERROR_SUCCESS)
							throw Exception(_T("Unable to delete item from registry (access denied)"));
					}
					else
						throw Exception(_T("Couldn't open registry key"));
				}
				else
				{
					CRegKey hKey;
					hKey.Open(HKEY_LOCAL_MACHINE, m_Diskey.c_str(), KEY_ALL_ACCESS | (IsWindows64() ? X64KEY : NORMALKEY));
					if (hKey.IsOpen())
					{
						if (hKey.DeleteSubKey(item->GetiName().c_str()) != ERROR_SUCCESS)
						{
							::RegCloseKey(hKey);
							throw Exception(_T("Couldn't remove disabled key (access denied)"));
						}
					}
					else
						throw Exception(_T("Couldn't open registry key"));
				}
			
				//remove item from list
				List::Delete();
			}

			catch (Boot::Exception const&)
			{
				throw;	// re-throw
			}

			catch (...)
			{
				throw Exception(_T("Boot::Registry::Section::Delete... Unknown error"));
			}
		}
	 
		void Section::Rename(Boot::Item *item, const tstring &newName) throw (Boot::Exception)
		{
			try
			{
				assert(ProperlyInitialized());
				assert(dynamic_cast<Item *>(item) != 0);
			
				//open registry key
				CRegKey hKey;
				hKey.Open(m_Root, m_Regkey.c_str(), KEY_ALL_ACCESS | m_dwHiveFlag);						
				if (hKey.IsOpen())
				{
					//create item with the new name
					if (hKey.SetValue(dynamic_cast<Item *>(item)->GetCommand().c_str(), newName.c_str()) != ERROR_SUCCESS)
						throw Exception(_T("Write access to registry denied"));
					//remove old item
					if (hKey.DeleteValue(item->GetiName().c_str()) != ERROR_SUCCESS)
						throw Exception(_T("Unable to remove old item from registry (access denied)"));

					item->SetiName(newName);
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
				throw Exception(_T("Boot::Registry::Section::Rename... Unknown error"));
			}
		}

		void Section::Enable(Boot::Item *& item) throw (Boot::Exception)
		{
			try
			{
				assert(ProperlyInitialized());
				assert(dynamic_cast<Item *>(item) != 0);
				assert(item->IsDisabled());
			
				if (item->GetAttributes() & Item::attrErrCorrupt)
					throw Exception(_T("Item is corrupted"));
			
				//stuff to enable a disabled registry item...
				CRegKey hKey;
				hKey.Open(HKEY_LOCAL_MACHINE, m_Diskey.c_str(), KEY_ALL_ACCESS | (IsWindows64() ? X64KEY : NORMALKEY));
				if (hKey.IsOpen())
				{
					Item *tmp = dynamic_cast<Item *>(GetNode());
					if (tmp->IsEnabled())
						throw Exception(_T("Item already enabled"));
				
					if (hKey.DeleteSubKey(item->GetiName().c_str()) != ERROR_SUCCESS)
						throw Exception(_T("Couldn't remove disabled key (access denied)"));

					Item *enabled = new Item(this);
					enabled->SetiName(tmp->GetiName());
					enabled->SetCommand(tmp->GetCommand());
					enabled->SetAttributes(tmp->GetAttributes() & ~Item::attrDisabled);
					List::Delete();
					
					try
					{
						Add(enabled);
					}

					catch (...)
					{
						delete enabled;
						throw;	// re-throw
					}

					item = enabled;
				}
				else
					throw Exception(_T("Access to registry denied"));

				assert(item->IsEnabled());
			}

			catch (Boot::Exception const&)
			{
				throw;	// re-throw
			}

			catch (...)
			{
				throw Exception(_T("Boot::Registry::Section::Enable... Unknown error"));
			}
		}

		void Section::Disable(Boot::Item *& item) throw (Boot::Exception)
		{
			try
			{
				assert(ProperlyInitialized());
				assert(dynamic_cast<Item *>(item) != 0);
				assert(item->IsEnabled());
			
				if (item->GetAttributes() & Item::attrErrCorrupt)
					throw Exception(_T("Item is corrupted"));
			
				//stuff to disable registy item...
			
				Item *tmp = dynamic_cast<Item *>(item);
			
				CRegKey hKey;
				hKey.Open(HKEY_LOCAL_MACHINE, m_Diskey.c_str(), KEY_ALL_ACCESS | (IsWindows64() ? X64KEY : NORMALKEY));
				if (hKey.IsOpen())
				{
					tstring name = tmp->GetiName();
					DWORD dwDisposition;
					CRegKey hKeyNew;
					hKeyNew.Create(hKey, name.c_str(), REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0), 0, &dwDisposition);
					if (hKeyNew.IsOpen())
					{
						if (dwDisposition == REG_OPENED_EXISTING_KEY)
							throw Exception(_T("An item with this name already exists"));

						try
						{
							tstring hkey = (m_Root == HKEY_LOCAL_MACHINE) ? _T("HKLM") : _T("HKCU");
							tstring command = tmp->GetCommand();
							tstring inimapping = _T("0");
							tstring key = m_Regkey;
							if ((m_Root == HKEY_LOCAL_MACHINE) && (m_dwHiveFlag == X32KEY))
								key.insert(9, _T("Wow6432Node\\"));

							if (hKeyNew.SetValue(command.c_str(), _T("command")) != ERROR_SUCCESS)
								throw Exception(_T("Unable to set command value"));
							if (hKeyNew.SetValue(hkey.c_str(), _T("hkey")) != ERROR_SUCCESS)
								throw Exception(_T("Unable to set hkey value"));
							if (hKeyNew.SetValue(inimapping.c_str(), _T("inimapping")) != ERROR_SUCCESS)
								throw Exception(_T("Unable to set inimapping value"));
							if (hKeyNew.SetValue(name.c_str(), _T("item")) != ERROR_SUCCESS)
								throw Exception(_T("Unable to set item value"));
							if (hKeyNew.SetValue(key.c_str(), _T("key")) != ERROR_SUCCESS)
								throw Exception(_T("Unable to set key value"));
						}

						catch (...)
						{
							hKeyNew.Close();
							hKey.DeleteSubKey(name.c_str());
							throw;	// re-throw
						}

						Item *disabled = new Item(this);
						disabled->SetiName(tmp->GetiName());
						disabled->SetCommand(tmp->GetCommand());
						disabled->SetAttributes(tmp->GetAttributes() | Item::attrDisabled);
						Delete(tmp);
				
						InsertAfter(disabled);
						if (GetNode() != disabled)
						{
							delete disabled;
							throw Exception(_T("Failed to add item to list"));
						}

						item = disabled;
					}
					else
						throw Exception(_T("Couldn't create key"));
				}
				else
					throw Exception(_T("Access to registry denied"));

				assert(item->IsDisabled());
			}

			catch (Boot::Exception const&)
			{
				throw;	// re-throw
			}

			catch (...)
			{
				throw Exception(_T("Boot::Registry::Section::Disable... Unknown error"));
			}
		}
		
		tstring Section::GetLongSection() const
		{
			tstring section = _T("Registry: ");
			section = section + ((m_Root == HKEY_LOCAL_MACHINE) ? _T("HKLM\\") : _T("HKCU\\"));
			section = section + m_Regkey + ((m_dwHiveFlag == X64KEY) ? _T(" (64-bit)") : _T(""));
			return section;
		}
 
	} //namespace Registry

} //namespace Boot
