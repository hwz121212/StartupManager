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

/// @file BtFolder.cpp Definitions for the start folder section classes.

#include "BtFolder.h"
#include "wintools.h"

namespace Boot
{

	namespace Folder
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

		bool Item::SeparateFlags()
		{
			if (m_Cmd.length() <= 0)
			{
				//empty command...
				return false;
			}

			tstring buffer = m_Cmd;
		
			// the filepath might be between quotes -> following form: "the path" flags
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

			// if the filepath is not between quotes, then there are 2 possibilities:
			//	1) there are no flags -> the last characters must be *.* (too bad this is also possible for flags)
			//	2) there are flags (then the filepath does not contain spaces) -> the first space separates path and flags
			// we use the following algorithm to try to separate flags:
			//	1) if spaces -> split on the first space: path = first part, flags = second part
			//	2) else -> path = command
			//	3) if path is valid -> separation succeeded
			//	4) else -> path contains spaces -> no flags -> path = command
			if (buffer.find(_T(" ")) != std::string::npos)
			{
				SetPath(buffer.substr(0, buffer.find(_T(" "))-1));
				m_Flags = buffer.substr(buffer.find(_T(" "))+1, buffer.length());

				if (!IsExist()) // too bad this one isn't fail-safe :(
				{
					SetPath(m_Cmd);
					m_Flags = _T("");
				}
			}
			else
			{
				SetPath(m_Cmd);
				m_Flags = _T("");
			}

			return true;
		}

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
				bool bSuccess = true;
				m_Cmd = ResolveLinkTarget(bSuccess);
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

		Section::Section(USER user, const tstring &section) : Boot::Section(section), m_User(user)
		{
			assert((user == ALL) || (user == CURRENT));
			assert(section.length() > 0);

			GetDisabledLocation();
		}

		void Section::GetDisabledLocation() throw (Boot::Exception)
		{	
			try
			{
				CRegKey hKey;
				hKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Startup Manager"), KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
				if (hKey.IsOpen())
				{
					TCHAR szValue[256] = {0};
					DWORD dwValueLen = 256*sizeof(TCHAR);
					hKey.QueryValue(szValue, _T("dFolderLocation"), &dwValueLen);
					if (_tcslen(szValue) > 0)
						m_Diskey = szValue;
					else
					{
						if (GetWindowsVersion().find(_T("Windows 2000")) != std::string::npos)
							m_Diskey = _T("SOFTWARE\\Startup Manager\\startupfolder");
						else
							m_Diskey = _T("SOFTWARE\\Microsoft\\Shared Tools\\MSConfig\\startupfolder");
						
						if (hKey.SetValue(m_Diskey.c_str(), _T("dFolderLocation")) != ERROR_SUCCESS)
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
				throw Exception(_T("Boot::Folder::Section::GetDisabledLocation... Unknown exception"));
			}
		}

		bool Section::ProperlyInitialized()
		{
			return ((GetSection().length() > 0) && ((m_User == ALL) || (m_User == CURRENT)));
		}

		void Section::InsertReadItem(LPCTSTR szProgramPath, LPCTSTR szValueName) throw (Boot::Exception)
		{
			try
			{
				TCHAR buffer[MAX_PATH];
				_sntprintf(buffer, sizeof(buffer)/sizeof(buffer[0]) - 1, _T("%s\\%s"), szProgramPath, szValueName);

				Item *item = new Item(this);
				item->SetiName(szValueName);
				item->Link().SetPath(buffer);
				bool bSuccess = true;
				item->SetCommand(item->ResolveLinkTarget(bSuccess));

				if ((_tcslen(buffer) <= 1) || !bSuccess)
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
				throw Exception(_T("Section::InsertReadItem... Unknown error"));
			}
		}

		void Section::Read()
		{
			try
			{
				assert(ProperlyInitialized());
			
				//allocate memory
				IMalloc *gpMalloc;
				SHGetMalloc(&gpMalloc);

				LPITEMIDLIST pIDL = 0;
				if (SHGetSpecialFolderLocation(0, m_User, &pIDL) == NOERROR)
				{
					TCHAR path[MAX_PATH] = {0};
					SHGetPathFromIDList(pIDL, path);
				
					LPSHELLFOLDER pSFI = 0;
					if (SHGetDesktopFolder(&pSFI) == NOERROR)
					{
						LPSHELLFOLDER pSSFI = 0;
						if (pSFI->BindToObject(pIDL, 0, IID_IShellFolder, (LPVOID *)&pSSFI) == NOERROR)
						{
							LPENUMIDLIST pEnumIDList = 0;
							if (pSSFI->EnumObjects(0, SHCONTF_NONFOLDERS, &pEnumIDList) == NOERROR)
							{
								WIN32_FIND_DATA wfd;
								LPITEMIDLIST pIIL = 0;
								while (pEnumIDList->Next(1, &pIIL, 0) == NOERROR)
								{
									if (SHGetDataFromIDList(pSSFI, pIIL, SHGDFIL_FINDDATA, (PVOID)&wfd, sizeof(WIN32_FIND_DATA)) == NOERROR)
										if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && _tcscmp(wfd.cFileName, _T("desktop.ini")))
										{
											try
											{
												InsertReadItem(path, wfd.cFileName);
											}

											catch (Boot::Exception const&)
											{
												//We don't want an exception for one item to break others,
												//so we just continue...
											}
										}
								
									gpMalloc->Free(pIIL);
								}
							
								pEnumIDList->Release();
							}
						
							pSSFI->Release();
						}
					
						pSFI->Release();	
					}
				
					gpMalloc->Free(pIDL);
				}
				else
				{
					gpMalloc->Release();		 
					throw Exception(_T("Couldn't open boot folder"));
				}
		 
				//free allocated memory
				gpMalloc->Release();
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

				IMalloc *gpMalloc;
				SHGetMalloc(&gpMalloc);
				LPITEMIDLIST ItemIDList = 0;
				//get the needed boot directory
				if (SHGetSpecialFolderLocation(0, m_User, &ItemIDList) == NOERROR)
				{
					TCHAR buffer[MAX_PATH] = {0};
					SHGetPathFromIDList(ItemIDList, buffer);
				
					gpMalloc->Free(ItemIDList);
					gpMalloc->Release();

					Item *tmp = dynamic_cast<Item *>(item);

					if (tmp->Create(tmp->GetPath(), buffer, item->GetiName(), tmp->GetFlags()))
					{		 
						//add item to list
						if (item->GetiName().find(_T(".lnk")) != (item->GetiName().length()-4))
							tmp->SetiName(item->GetiName() + _T(".lnk"));
						
						InsertAfter(item);
						if (GetNode() != item)
							throw Exception(_T("Failed to add item to list"));
					}
					else
						throw Exception(_T("Unable to create shortcut"));
				}
				else
				{
					gpMalloc->Release();
					throw Exception(_T("Couldn't open boot folder"));
				}
			}

			catch (Boot::Exception const&)
			{
				throw;	// re-throw
			}
			
			catch (...)
			{
				throw Exception(_T("Boot::Folder::Section::Add... Unknown error"));
			}
		}
	 
		void Section::Delete(Boot::Item *item) throw (Boot::Exception)
		{
			try
			{
				assert(ProperlyInitialized());
				assert(dynamic_cast<Item *>(item) != 0);
			
				Item *tmp = dynamic_cast<Item *>(item);

				if (item->IsEnabled())
				{
					//remove file
					tmp->Link().DeleteFile();
				}
				else
				{
					tstring subkey = m_Diskey + _T("\\");
					tstring keyname = tmp->Link().GetPath();
					while (keyname.find(_T("\\")) != std::string::npos)
						keyname.replace(keyname.find(_T("\\")), 1, _T("^"));
					subkey += keyname;
			
					CRegKey hKey;
					hKey.Open(HKEY_LOCAL_MACHINE, m_Diskey.c_str(), KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
					if (hKey.IsOpen())
					{	
						CRegKey hKeySub;
						hKeySub.Open(HKEY_LOCAL_MACHINE, subkey.c_str(), KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
						if (!hKeySub.IsOpen())
							throw Exception(_T("Couldn't open subkey"));
						
						TCHAR szVal[256] = {0};
						DWORD dwValLen = 256*sizeof(TCHAR);
						hKeySub.QueryValue(szVal, _T("backup"), &dwValLen);
						if (_tcslen(szVal) > 0)
							_tremove(szVal);
						else
							throw Exception(_T("RegQueryValueEx returned invalid value"));	
								
						if (hKey.DeleteSubKey(keyname.c_str()) != ERROR_SUCCESS)
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
				throw Exception(_T("Boot::Folder::Section::Delete... Unknown error"));
			}
		}
	 
		void Section::Rename(Boot::Item *item, const tstring &newName) throw (Boot::Exception)
		{
			try
			{
				assert(ProperlyInitialized());
				assert(dynamic_cast<Item *>(item) != 0);
			
				//rename item
				Item *tmp = dynamic_cast<Item *>(item);
				tstring newPath = tmp->Link().GetDir() + newName;
				if (newName.find(_T(".lnk")) != (newName.length()-4))
					newPath += _T(".lnk");
				tmp->Link().RenameFile(newPath);
				tmp->SetiName(newName);
			}

			catch (Boot::Exception const&)
			{
				throw;	// re-throw
			}
			
			catch (...)
			{
				throw Exception(_T("Boot::Folder::Section::Rename... Unknown error"));
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

				//stuff to enable a disabled boot folder item...
				tstring keyname = dynamic_cast<Item *>(item)->Link().GetPath();
				while (keyname.find(_T("\\")) != std::string::npos)
					keyname.replace(keyname.find(_T("\\")), 1, _T("^"));
			
				CRegKey hKey;
				hKey.Open(HKEY_LOCAL_MACHINE, m_Diskey.c_str(), KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
				if (hKey.IsOpen())
				{					
					CRegKey hKeySub;
					hKeySub.Open(hKey, keyname.c_str(), KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
					if (!hKeySub.IsOpen())
						throw Exception(_T("Couldn't open subkey"));
				
					TCHAR szVal[256] = {0};
			
					DWORD dwValLen = 256*sizeof(TCHAR);
					hKeySub.QueryValue(szVal, _T("command"), &dwValLen);
					tstring command = szVal;
								
					dwValLen = 256*sizeof(TCHAR);
					hKeySub.QueryValue(szVal, _T("backup"), &dwValLen);
					if (_tcslen(szVal) > 0)
						_tremove(szVal);
					else
						throw Exception(_T("RegQueryValueEx returned invalid value"));
				
					dwValLen = 256*sizeof(TCHAR);
					hKeySub.QueryValue(szVal, _T("path"), &dwValLen);
					tstring path = szVal;
				
					if (hKey.DeleteSubKey(keyname.c_str()) != ERROR_SUCCESS)
						throw Exception(_T("Couldn't remove disabled key (access denied)"));
				
					Item *enabled = new Item(this);
					enabled->SetiName(item->GetiName());
					enabled->SetCommand(command);
					enabled->Link().SetPath(path);
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
				throw Exception(_T("Section::Enable... Unknown error"));
			}
		}
		
		void Section::Disable(Boot::Item *& item) throw (Boot::Exception)
		{
			try
			{
				assert(ProperlyInitialized());
				assert(dynamic_cast<Item *>(item) != 0);
				assert(item->IsEnabled());
			
				//stuff to disable boot folder item...
			
				Item *tmp = dynamic_cast<Item *>(item);
			
				CRegKey hKey;
				hKey.Open(HKEY_LOCAL_MACHINE, m_Diskey.c_str(), KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0));
				if (hKey.IsOpen())
				{
					tstring keyname = tmp->Link().GetPath();
					while (keyname.find(_T("\\")) != std::string::npos)
						keyname.replace(keyname.find(_T("\\")), 1, _T("^"));
				
					DWORD dwDisposition;
					CRegKey hKeyNew;
					hKeyNew.Create(hKey, keyname.c_str(), REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | (IsWindows64() ? KEY_WOW64_64KEY : 0), 0, &dwDisposition);
					if (hKeyNew.IsOpen())
					{
						if (dwDisposition == REG_OPENED_EXISTING_KEY)
							throw Exception(_T("An item with this name already exists"));
					
						try
						{
							tstring location = (m_User == ALL) ? _T("Common Startup") : _T("Startup");
							tstring command = tmp->GetCommand();
							tstring path = tmp->Link().GetPath();
							tstring name = tmp->Link().GetName();
							name.erase(name.find(tmp->Link().GetExt())-1, tmp->Link().GetExt().length()+1);
							TCHAR szWindowsLocation[MAX_PATH];
							::GetWindowsDirectory(szWindowsLocation, MAX_PATH);
							tstring backup = szWindowsLocation;
							backup = backup + _T("\\pss\\") + name + _T(".lnk") + location;
					
							_trename(path.c_str(), backup.c_str());
					
							if (hKeyNew.SetValue(backup.c_str(), _T("backup")) != ERROR_SUCCESS)
								throw Exception(_T("Unable to set backup value"));
							if (hKeyNew.SetValue(command.c_str(), _T("command")) != ERROR_SUCCESS)
								throw Exception(_T("Unable to set command value"));
							if (hKeyNew.SetValue(name.c_str(), _T("item")) != ERROR_SUCCESS)
								throw Exception(_T("Unable to set item value"));
							if (hKeyNew.SetValue(location.c_str(), _T("location")) != ERROR_SUCCESS)
								throw Exception(_T("Unable to set location value"));
							if (hKeyNew.SetValue(path.c_str(), _T("path")) != ERROR_SUCCESS)
								throw Exception(_T("Unable to set path value"));
						}

						catch (...)
						{
							hKeyNew.Close();
							hKey.DeleteSubKey(keyname.c_str());
							throw;	// re-throw
						}
						
						Item *disabled = new Item(this);
						disabled->SetiName(tmp->GetiName());
						disabled->SetCommand(tmp->GetCommand());
						disabled->Link().SetPath(tmp->Link().GetPath());
						disabled->SetAttributes(tmp->GetAttributes() | Item::attrDisabled);
						List::Delete();

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
				throw Exception(_T("Boot::Folder::Section::Disable... Unknown error"));
			}
		}

		tstring Section::GetLongSection() const
		{
			IMalloc *gpMalloc;
			SHGetMalloc(&gpMalloc);
			LPITEMIDLIST ItemIDList = 0;
			if (SHGetSpecialFolderLocation(0, m_User, &ItemIDList) == NOERROR)
			{
				TCHAR buffer[MAX_PATH] = {0};
				SHGetPathFromIDList(ItemIDList, buffer);
				
				gpMalloc->Free(ItemIDList);
				gpMalloc->Release();

				tstring section = _T("StartUp: ");
				section = section + buffer;
				return section;
			}
			else
			{
				gpMalloc->Release();
				return m_Section;
			}
		}

	} //namespace Folder

} //namespace Boot
