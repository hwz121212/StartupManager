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

/// @file BtWinLogon.cpp Definitions of the winlogon section classes.

#include "BtWinLogon.h"
#include "WinTools.h"


namespace Boot
{

	namespace WinLogon
	{

		void Item::SetPath(const tstring &file)
		{
			if (file.length() <= 0)
				if (!(GetAttributes() & attrErrCorrupt))
					SetAttributes(GetAttributes() | attrErrCorrupt);

			File::SetPath(file);
			SetiName(GetName());
		}

		Section::Section(const tstring &part, const tstring &section) : Boot::Section(section), m_Part(part)
		{
			assert((part == _T("Userinit")) || (part == _T("Shell")));
			assert(section.length() > 0);

			m_Diskey = _T("SOFTWARE\\Startup Manager\\startupwinlogon");
		}

		bool Section::ProperlyInitialized()
		{
			return ((GetSection().length() > 0) && ((m_Part == _T("Userinit")) || (m_Part == _T("Shell"))));
		}

		void Section::InsertReadItem(const tstring &path) throw (Exception)
		{
			try
			{		
				TCHAR userinit[MAX_PATH + 1];
				::GetSystemDirectory(userinit, MAX_PATH);
				tstring userinitPath = userinit;
				userinitPath = userinitPath + _T("\\userinit.exe");

				TCHAR explorer[MAX_PATH + 1];
				::GetWindowsDirectory(explorer, MAX_PATH);
				tstring explorerPath = explorer;
				explorerPath = explorerPath + _T("\\explorer.exe");
				tstring explorerPath2 = explorerPath + + _T("\\Explorer.exe");
				tstring explorerPath3 = _T("Explorer.exe");

				Item *item = new Item(this);
				item->SetPath(path);

				if ((path == userinitPath) || (path == explorerPath) || (path == explorerPath2)
					|| (path == explorerPath3))
					item->SetAttributes(Item::attrSystem);
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
				throw Exception(_T("Boot::WinLogon::Section::InsertReadItem... Unknown error"));
			}
		}
		
		void Section::Read()
		{
			try
			{
				assert(ProperlyInitialized());

				tstring FileToAdd, ThePath;

				//read winlogon
				CRegKey hKey;
				hKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon"), KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
				if (hKey.IsOpen())
				{
					TCHAR szValue[256] = {0};
					DWORD dwValueLen = 256*sizeof(TCHAR);
					hKey.QueryValue(szValue, m_Part.c_str(), &dwValueLen);

					//if there's an item in the winlogon section we're reading
					if (_tcslen(szValue) > 0)
					{			
						FileToAdd = szValue;

						//search for spaces, this is what separates items in winlogon
						while (FileToAdd.find(_T(",")) != std::string::npos)
						{
							//place the file path into the ThePath variable
							ThePath = FileToAdd.substr(0, FileToAdd.find(_T(",")));
							//erase this file path (we have it in ThePath now)
							FileToAdd.erase(0, FileToAdd.find(_T(","))+1);
						
							if (ThePath.length() > 0)
							{
								try
								{
									InsertReadItem(ThePath);
								}

								catch (Boot::Exception const&)
								{
									//We don't want an exception for one item to break others,
									//so we just continue...
								}
							}
						}
				 
						//if the loop is handled, no (more) "," found, there might still be an item
						if (FileToAdd.length() > 0)
						{
							try
							{
								InsertReadItem(FileToAdd);
							}

							catch (Boot::Exception const&)
							{
								//We don't want an exception for one item to break others,
								//so we just continue...
							}
						}
					}
				}
				else
					throw Exception(_T("Access to winlogon denied"));
			}

			catch (Boot::Exception const&)
			{
				// Ignore...
			}
			
			catch (...)
			{
				throw;
			}
		}
	 
		void Section::Add(Boot::Item *item) throw (Boot::Exception)
		{
			try
			{
				assert(ProperlyInitialized());
				assert(dynamic_cast<Item *>(item) != 0);
			
				item->SetAttributes(Item::attrRegular);

				CRegKey hKey;
				hKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon"), KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
				if (hKey.IsOpen())
				{
					TCHAR szValue[256] = {0};
					DWORD dwValueLen = 256*sizeof(TCHAR);
					hKey.QueryValue(szValue, m_Part.c_str(), &dwValueLen);

					if ((_tcslen(szValue) > 0) && (szValue[0] != ' '))
					{
						_sntprintf(szValue, sizeof(szValue)/sizeof(szValue[0]) - 1, _T("%s,%s"), szValue, item->GetPath().c_str());
						if (hKey.SetValue(szValue, m_Part.c_str()) != ERROR_SUCCESS)
							throw Exception(_T("Write access to registry denied"));
					}
					else
					{
						tstring value = item->GetPath();
						if (m_Part == _T("Userinit")) value = value + _T(",");
						if (hKey.SetValue(value.c_str(), m_Part.c_str()) != ERROR_SUCCESS)
							throw Exception(_T("Write access to registry denied"));
					}
				
					//add item to list
					SelLast();
					InsertAfter(item);
					if (GetNode() != item)
						throw Exception(_T("Failed to add item to list"));
				}
				else
					throw Exception(_T("Access to winlogon denied"));
			}

			catch (Boot::Exception const&)
			{
				throw;	// re-throw
			}

			catch (...)
			{
				throw Exception(_T("Boot::WinLogon::Section::Add... Unknown error"));
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
					CRegKey hKey;
					hKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon"), KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
					if (hKey.IsOpen())
					{
						TCHAR szValue[256] = {0};
						DWORD dwValueLen = 256*sizeof(TCHAR);
						hKey.QueryValue(szValue, m_Part.c_str(), &dwValueLen);
								 
						if (_tcslen(szValue) > 0)
						{
							tstring stbuffer = szValue;
							size_t BeginPos = 0;
							//try to find the item we want to remove
							if ((BeginPos = stbuffer.find(item->GetPath().c_str())) != std::string::npos)
							{
								//remove the item from the win.ini command
								if (BeginPos > 0)
									stbuffer.erase(BeginPos-1, item->GetPath().length()+1);
								else
									stbuffer.erase(BeginPos, item->GetPath().length());
								
								if (hKey.SetValue(stbuffer.c_str(), m_Part.c_str()) != ERROR_SUCCESS)
									throw Exception(_T("Write access to registry denied"));
							}
						}
					}
					else
						throw Exception(_T("Access to winlogon denied"));
				}
				else
				{
					CRegKey hKey;
					hKey.Open(HKEY_LOCAL_MACHINE, m_Diskey.c_str(), KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
					if (hKey.IsOpen())
					{
						if (hKey.DeleteSubKey(item->GetiName().c_str()) != ERROR_SUCCESS)
							throw Exception(_T("Couldn't remove disabled key (access denied)"));
					}
					else
						throw Exception(_T("Access to registry denied"));
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
				throw Exception(_T("Boot::WinLogon::Section::Delete... Unknown error"));
			}
		}
		
		void Section::Rename(Boot::Item *, tstring const&) throw (Boot::Exception)
		{
			throw Exception(_T("Cannot rename winlogon items"));
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

				//stuff to enable a disabled win.ini item...
				CRegKey hKey;
				hKey.Open(HKEY_LOCAL_MACHINE, m_Diskey.c_str(), KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
				if (hKey.IsOpen())
				{
					if (hKey.DeleteSubKey(item->GetiName().c_str()) != ERROR_SUCCESS)
						throw Exception(_T("Couldn't remove disabled key (access denied)"));

					Item *enabled = new Item(this);
					enabled->SetPath(item->GetPath());
					enabled->SetAttributes(item->GetAttributes() & ~Item::attrDisabled);
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
				throw Exception(_T("Boot::WinLogon::Section::Enable... Unknown error"));
			}
		}

		void Section::Disable(Boot::Item *& item) throw (Boot::Exception)
		{
			try
			{
				assert(ProperlyInitialized());
				assert(dynamic_cast<Item *>(item) != 0);
				assert(item->IsEnabled());
			
				//stuff to disable winlogon item...
			
				Item *tmp = dynamic_cast<Item *>(item);

				CRegKey hKey;
				hKey.Open(HKEY_LOCAL_MACHINE, m_Diskey.c_str(), KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
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
							tstring command = tmp->GetPath();
							if (hKeyNew.SetValue(command.c_str(), _T("command")) != ERROR_SUCCESS)
								throw Exception(_T("Unable to set command value"));
							if (hKeyNew.SetValue(m_Part.c_str(), _T("part")) != ERROR_SUCCESS)
								throw Exception(_T("Unable to set hkey value"));
						}

						catch (...)
						{
							hKeyNew.Close();
							hKey.DeleteSubKey(name.c_str());
							throw;	// re-throw
						}

						Item *disabled = new Item(this);
						disabled->SetPath(tmp->GetPath());
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
				throw Exception(_T("Boot::WinLogon::Section::Enable... Unknown error"));
			}
		}
		
		tstring Section::GetLongSection() const
		{
			tstring ret = _T("Registry: HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon (") + m_Part + _T(")");
			return ret;
		}
 
	} //WinLogon

} //namespace Boot
