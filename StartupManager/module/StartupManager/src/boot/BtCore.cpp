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

/// @file BtCore.cpp Definition of the start section wrapper classes.

#include "BtCore.h"
#include "WinTools.h"
#include "..\types\tfstream.h"
#include "..\types\utf16fstream.h"

#include <exception>
#include <algorithm>
#include <functional>
#include <time.h>

#ifdef UNICODE
  #define NEWLINE _T("\r\n")
#else
  #define NEWLINE std::endl
#endif

namespace Boot
{

	WorkItem& WorkItem::operator=(WorkItem const& item)
	{
		if (this != &item)
		{
			m_command = item.GetCommand();
			m_name    = item.GetName();
			m_section = item.GetSection();
		}

		return *this;
	}

	Core::SectionContainer::~SectionContainer()
	{
		for (std::vector<Section *>::iterator tmp = begin(); tmp != end(); ++tmp)
			delete (*tmp);
		clear();
	}

	void Core::SectionContainer::AddWininiSections()
	{
		Winini::Section *winini;
		winini = new Winini::Section(_T("Run"), _T("WinINI\\Run"));
		push_back(winini);
		winini = new Winini::Section(_T("Load"),_T("WinINI\\Load"));
		push_back(winini);
	}
 
	BOOL Core::SectionContainer::ShouldAdd(Registry::Section *registry)
	{
		if (m_bDisplayUnused)
			return TRUE;

		BOOL exist = FALSE;
		CRegKey hKey;
		if (hKey.Open(registry->GetRoot(), registry->GetRegkey().c_str(), KEY_ALL_ACCESS | registry->GetHiveFlag()) == ERROR_SUCCESS)
	    	exist = TRUE;	

		return exist;
	}
	
	void Core::SectionContainer::AddRegSection(int i, HKEY root, const tstring &regkey, const tstring &section)
	{
		//set access type for normal registry access
		Registry::Section::KEYBITS nAccess = IsWindows64() ? Registry::Section::X32KEY : Registry::Section::NORMALKEY;
		tstring longSection = section;
		if (i == 1)
			longSection += _T(" (64-bit)");
		
		//initialize and add registry section
		Registry::Section *registry = new Registry::Section(root, regkey, (i == 1) ? Registry::Section::X64KEY : nAccess, longSection);
		if (ShouldAdd(registry))
			push_back(registry);
		else
			delete registry;
	}
	
	void Core::SectionContainer::AddRegistrySections()
	{
		//add Browser Helper Objects section
		BHO::Section *bho;
		if (IsWindows64())
		{
			bho = new BHO::Section(Registry::Section::X64KEY, _T("Registry\\Computer\\BHO (64-bit)"));
			if (ShouldAdd(bho))
				push_back(bho);
			else
				delete bho;
		}
		bho = new BHO::Section(IsWindows64() ? Registry::Section::X32KEY : Registry::Section::NORMALKEY, _T("Registry\\Computer\\BHO"));
		if (ShouldAdd(bho))
			push_back(bho);
		else
			delete bho;

		//add logon sections
		WinLogon::Section *logon;
		logon = new WinLogon::Section(_T("Shell"), _T("Registry\\Computer\\Shell"));
		push_back(logon);
		logon = new WinLogon::Section(_T("Userinit"), _T("Registry\\Computer\\Userinit"));
		push_back(logon);
		
		//check for 64-bit windows
		int begin = IsWindows64() ? 1 : 2;
			
		//all user keys:
		//if 64-bit windows, also check the 64-bit part of the registry keys
		for (int i=begin; i<=2; i++)
		{
			AddRegSection(i, HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\\Run"), _T("Registry\\All Users\\Policies"));
			AddRegSection(i, HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunServicesOnce"), _T("Registry\\All Users\\Run Services Once"));
			AddRegSection(i, HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunServices"), _T("Registry\\All Users\\Run Services"));
			AddRegSection(i, HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnceEx"), _T("Registry\\All Users\\Run Once Ex"));
			AddRegSection(i, HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce\\Setup"), _T("Registry\\All Users\\Run Once (Setup)"));
			AddRegSection(i, HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"), _T("Registry\\All Users\\Run Once"));
			AddRegSection(i, HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunEx"), _T("Registry\\All Users\\Run Ex"));
			AddRegSection(i, HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), _T("Registry\\All Users\\Run"));
		}
		
		//current user keys:
		begin = 2; //64-bit and 32-bit access gives same result with current user???
		for (int i=begin; i<=2; i++)
		{
			AddRegSection(i, HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\\Run"), _T("Registry\\Current User\\Policies"));
			AddRegSection(i, HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunServicesOnce"), _T("Registry\\Current User\\Run Services Once"));
			AddRegSection(i, HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunServices"), _T("Registry\\Current User\\Run Services"));
			AddRegSection(i, HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnceEx"), _T("Registry\\Current User\\Run Once Ex"));
			AddRegSection(i, HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce\\Setup"), _T("Registry\\Current User\\Run Once (Setup)"));
			AddRegSection(i, HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"), _T("Registry\\Current User\\Run Once"));
			AddRegSection(i, HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunEx"), _T("Registry\\Current User\\Run Ex"));
			AddRegSection(i, HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), _T("Registry\\Current User\\Run"));
		}
	}

	void Core::SectionContainer::AddFolderSections()
	{
		Folder::Section *folder;
		folder = new Folder::Section(Folder::Section::ALL, _T("StartUp\\All Users"));
		push_back(folder);
		folder = new Folder::Section(Folder::Section::CURRENT, _T("StartUp\\Current User"));
		push_back(folder);
	}
	
	tstring Core::SectionContainer::GetDisabledLocation(LPCTSTR section) throw(Boot::Exception)
	{
		try
		{
			Registry::Section::KEYBITS flag = IsWindows64() ? Registry::Section::X64KEY : Registry::Section::NORMALKEY;
			CRegKey hKey;
			hKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Startup Manager"), KEY_ALL_ACCESS | flag);
			if (hKey.IsOpen())
			{
				TCHAR szValue[256] = {0};
				DWORD dwValueLen = 256*sizeof(TCHAR);
				hKey.QueryValue(szValue, section, &dwValueLen);
				if (_tcslen(szValue) > 0)
				{
					tstring retVal = szValue;
					return retVal;
				}
				else
					throw Boot::Exception(_T("Invalid disabled registry location"));
			}
			throw Boot::Exception(_T("Couldn't open Startup Manager key"));
		}

		catch (Boot::Exception const&)
		{
			throw;	// re-throw
		}

		catch (...)
		{
			throw Boot::Exception(_T("Core::SectionContainer::GetDisabledLocation... Unknown error"));
		}
	}
	
	void Core::SectionContainer::ReadDisabledRegistry()
	{
		try
		{
			Registry::Section::KEYBITS flag = IsWindows64() ? Registry::Section::X64KEY : Registry::Section::NORMALKEY;
			tstring disKey = GetDisabledLocation(_T("dRegLocation"));
		
			//open registry key
			CRegKey hKey;
			hKey.Open(HKEY_LOCAL_MACHINE, disKey.c_str(), KEY_ALL_ACCESS | flag);
			if (hKey.IsOpen())
			{
				TCHAR szValue[256];
				DWORD dwValueLen = 256*sizeof(TCHAR);
				int i = 0;
		
				//read items from registry key
				while (::RegEnumKeyEx(hKey, i, szValue, &dwValueLen, 0, 0, 0, 0) != ERROR_NO_MORE_ITEMS)
				{
					try
					{
						//read values in item
						tstring key = disKey + _T("\\");
						key += szValue;
						//open registry key
						CRegKey hKeySub;
						hKeySub.Open(HKEY_LOCAL_MACHINE, key.c_str(), KEY_ALL_ACCESS | flag);
						if (hKeySub.IsOpen())
						{			
							TCHAR szVal[256] = {0};
					
							DWORD dwValLen = 256*sizeof(TCHAR);
							hKeySub.QueryValue(szVal, _T("key"), &dwValLen);
							tstring key = szVal;
									 
							dwValLen = 256*sizeof(TCHAR);
							hKeySub.QueryValue(szVal, _T("item"), &dwValLen);
							tstring item = szVal;
					
							dwValLen = 256*sizeof(TCHAR);
							hKeySub.QueryValue(szVal, _T("hkey"), &dwValLen);
							HKEY root = 0;
							if (_tcslen(szVal) > 0)
							{
								if (_tcscmp(szVal, _T("HKCU")) && _tcscmp(szVal, _T("HKLM")))
									throw (Boot::Exception(_T("Core::ReadDisabledRegistry... invalid root")));

								root = _tcscmp(szVal, _T("HKCU")) ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;
							}
							else
								throw (Boot::Exception(_T("Core::ReadDisabledRegistry... Invalid information in disabled item")));
					
							dwValLen = 256*sizeof(TCHAR);
							hKeySub.QueryValue(szVal, _T("command"), &dwValLen);
							tstring command = szVal;
				
							if (key.length() <= 0)
								throw Boot::Exception(_T("Invalid information in disabled item"));
					
							Registry::Section::KEYBITS dwHiveFlag = Registry::Section::NORMALKEY;
							if (IsWindows64())
							{
								if (root == HKEY_LOCAL_MACHINE)
								{
									if (key.find(_T("Wow6432Node")) != std::string::npos)
									{
										dwHiveFlag = Registry::Section::X32KEY;
										key.erase(key.find(_T("Wow6432Node")), 12);
									}
									else
										dwHiveFlag = Registry::Section::X64KEY;
								}
								else
								{
									//no 64-bit keys for current user
									dwHiveFlag = Registry::Section::X32KEY;
								}
							}
					
							for (iterator iter = begin(); iter != end(); ++iter)
								if (typeid(**iter) == typeid(Registry::Section))
								{
									Registry::Section *tmp = dynamic_cast<Registry::Section *>(*iter);
									if ((tmp->GetRoot() == root) && (tmp->GetRegkey() == key)
										&& (tmp->GetHiveFlag() == dwHiveFlag))
									{
										Registry::Item *disitem = new Registry::Item(tmp);
										disitem->SetiName(szValue);
										disitem->SetCommand(command);
								
										if (item.length() <= 0)
											disitem->SetAttributes(Item::attrErrCorrupt);
								
										disitem->SetAttributes(disitem->GetAttributes() | Item::attrDisabled);
										(*iter)->SelFirst();
										(*iter)->InsertBefore(disitem);
										break;
									}
								}
						}
						else
							throw Boot::Exception(_T("Unable to read disabled item information"));
			
						//reset size to default
						dwValueLen = 256*sizeof(TCHAR);
						i++;
					}

					catch (...)
					{
						//reset size to default
						dwValueLen = 256*sizeof(TCHAR);
						i++;
					}
				}
			}
		}

		catch (...)
		{
			throw;	// re-throw
		}
	}
	
	void Core::SectionContainer::ReadDisabledFolder()
	{
		try
		{
			Registry::Section::KEYBITS flag = IsWindows64() ? Registry::Section::X64KEY : Registry::Section::NORMALKEY;
			tstring disKey = GetDisabledLocation(_T("dFolderLocation"));
		
			//open registry key
			CRegKey hKey;
			hKey.Open(HKEY_LOCAL_MACHINE, disKey.c_str(), KEY_ALL_ACCESS | flag);
			if (hKey.IsOpen())
			{
				TCHAR szValue[256];
				DWORD dwValueLen = 256*sizeof(TCHAR);
				int i = 0;
		
				//read items from registry key
				while (::RegEnumKeyEx(hKey, i, szValue, &dwValueLen, 0, 0, 0, 0) != ERROR_NO_MORE_ITEMS)
				{
					try
					{
						//read values in item
						//open registry key
						CRegKey hKeySub;
						hKeySub.Open(hKey, szValue, KEY_ALL_ACCESS | flag);
						if (hKeySub.IsOpen())
						{			
							TCHAR szVal[256] = {0};
						
							DWORD dwValLen = 256*sizeof(TCHAR);
							hKeySub.QueryValue(szVal, _T("item"), &dwValLen);
							tstring item = szVal;
					
							dwValLen = 256*sizeof(TCHAR);
							hKeySub.QueryValue(szVal, _T("path"), &dwValLen);
							tstring path = szVal;
					
							dwValLen = 256*sizeof(TCHAR);
							hKeySub.QueryValue(szVal, _T("command"), &dwValLen);
							tstring command = szVal;
									 
							dwValLen = 256*sizeof(TCHAR);
							hKeySub.QueryValue(szVal, _T("location"), &dwValLen);
							Folder::Section::USER location;
							if (_tcslen(szVal) > 0)
							{
								if (_tcscmp(szVal, _T("Common Startup")) & _tcscmp(szVal, _T("Startup")))
									throw (Boot::Exception(_T("Core::ReadDisabledFolder... Invalid root")));

								location = _tcscmp(szVal, _T("Common Startup")) ? Folder::Section::CURRENT : Folder::Section::ALL;
							}
							else
								throw Boot::Exception(_T("Invalid information in disabled item"));

					
							for (iterator iter = begin(); iter != end(); ++iter)
								if (typeid(**iter) == typeid(Folder::Section))
								{
									Folder::Section *tmp = dynamic_cast<Folder::Section *>(*iter);
									if (tmp->GetUser() == location)
									{
										Folder::Item *disitem = new Folder::Item(tmp);
										disitem->SetiName(item);
										disitem->SetCommand(command);
										disitem->Link().SetPath(path);
								
										if ((item.length() <= 0) || (path.length() <= 0) || (command.length() <= 0))
											disitem->SetAttributes(Item::attrErrCorrupt);
								
										disitem->SetAttributes(disitem->GetAttributes() | Item::attrDisabled);
										(*iter)->SelFirst();
										(*iter)->InsertBefore(disitem);
										break;
									}
								}
						}
						else
							throw Boot::Exception(_T("Unable to read disabled item information"));
			
						//reset size to default
						dwValueLen = 256*sizeof(TCHAR);
						i++;
					}

					catch (...)
					{
						//reset size to default
						dwValueLen = 256*sizeof(TCHAR);
						i++;
					}
				}
			}
		}

		catch (...)
		{
			throw;	// re-throw
		}
	}
	
	void Core::SectionContainer::ReadDisabledWinini()
	{
		try
		{
			Registry::Section::KEYBITS flag = IsWindows64() ? Registry::Section::X64KEY : Registry::Section::NORMALKEY;
			tstring disKey = _T("SOFTWARE\\Startup Manager\\startupwinini");
		
			//open registry key
			CRegKey hKey;
			hKey.Open(HKEY_LOCAL_MACHINE, disKey.c_str(), KEY_ALL_ACCESS | flag);
			if (hKey.IsOpen())
			{
				TCHAR szValue[256];
				DWORD dwValueLen = 256*sizeof(TCHAR);
				int i = 0;
			
				//read items from registry key
				while (::RegEnumKeyEx(hKey, i, szValue, &dwValueLen, 0, 0, 0, 0) != ERROR_NO_MORE_ITEMS)
				{
					try
					{
						//read values in item
						//open registry key
						CRegKey hKeySub;
						hKeySub.Open(hKey, szValue, KEY_ALL_ACCESS | flag);
						if (hKeySub.IsOpen())
						{			
							TCHAR szVal[256] = {0};
						
							DWORD dwValLen = 256*sizeof(TCHAR);
							hKeySub.QueryValue(szVal, _T("command"), &dwValLen);
							tstring command = szVal;
					
							dwValLen = 256*sizeof(TCHAR);
							hKeySub.QueryValue(szVal, _T("part"), &dwValLen);
							if (_tcscmp(szVal, _T("Load")) & _tcscmp(szVal, _T("Run")))
								throw Boot::Exception(_T("Invalid root"));

							tstring part = szVal;
					
							for (iterator iter = begin(); iter != end(); ++iter)
								if (typeid(**iter) == typeid(Winini::Section))
								{
									Winini::Section *tmp = dynamic_cast<Winini::Section *>(*iter);
									if (tmp->GetPart() == part)
									{
										Winini::Item *disitem = new Winini::Item(tmp);
										disitem->SetPath(command);
								
										if (command.length() <= 0)
											disitem->SetAttributes(Item::attrErrCorrupt);
								
										disitem->SetAttributes(disitem->GetAttributes() | Item::attrDisabled);
										(*iter)->SelFirst();
										(*iter)->InsertBefore(disitem);
										break;
									}
								}
						}
						else
							throw Boot::Exception(_T("Unable to read disabled item information"));
				
						//reset size to default
						dwValueLen = 256*sizeof(TCHAR);
						i++;
					}

					catch (...)
					{
						//reset size to default
						dwValueLen = 256*sizeof(TCHAR);
						i++;
					}
				}
			}
		}

		catch (...)
		{
			throw;	// re-throw
		}
	}
	
	void Core::SectionContainer::ReadDisabledWinLogon()
	{
		try
		{
			Registry::Section::KEYBITS flag = IsWindows64() ? Registry::Section::X64KEY : Registry::Section::NORMALKEY;
			tstring disKey = _T("SOFTWARE\\Startup Manager\\startupwinlogon");
		
			//open registry key
			CRegKey hKey;
			hKey.Open(HKEY_LOCAL_MACHINE, disKey.c_str(), KEY_ALL_ACCESS | flag);
			if (hKey.IsOpen())
			{
				TCHAR szValue[256];
				DWORD dwValueLen = 256*sizeof(TCHAR);
				int i = 0;
			
				//read items from registry key
				while (::RegEnumKeyEx(hKey, i, szValue, &dwValueLen, 0, 0, 0, 0) != ERROR_NO_MORE_ITEMS)
				{
					try
					{
						//read values in item
						//open registry key
						CRegKey hKeySub;
						hKeySub.Open(hKey, szValue, KEY_ALL_ACCESS | flag);
						if (hKeySub.IsOpen())
						{			
							TCHAR szVal[256] = {0};
					
							DWORD dwValLen = 256*sizeof(TCHAR);
							hKeySub.QueryValue(szVal, _T("command"), &dwValLen);
							tstring command = szVal;
					
							dwValLen = 256*sizeof(TCHAR);
							hKeySub.QueryValue(szVal, _T("part"), &dwValLen);
							if (_tcscmp(szVal, _T("Userinit")) & _tcscmp(szVal, _T("Shell")))
								throw Boot::Exception(_T("Invalid root"));

							tstring part = szVal;

							for (iterator iter = begin(); iter != end(); ++iter)
								if (typeid(**iter) == typeid(WinLogon::Section))
								{
									WinLogon::Section *tmp = dynamic_cast<WinLogon::Section *>(*iter);
									if (tmp->GetPart() == part)
									{
										WinLogon::Item *disitem = new WinLogon::Item(tmp);
										disitem->SetPath(command);
								
										if (command.length() <= 0)
											disitem->SetAttributes(Item::attrErrCorrupt);
								
										disitem->SetAttributes(disitem->GetAttributes() | Item::attrDisabled);
										(*iter)->SelFirst();
										(*iter)->InsertBefore(disitem);
										break;
									}
								}
						}
						else
							throw Boot::Exception(_T("Unable to read disabled item information"));
			
						//reset size to default
						dwValueLen = 256*sizeof(TCHAR);
						i++;
					}

					catch (...)
					{
						//reset size to default
						dwValueLen = 256*sizeof(TCHAR);
						i++;
					}
				}
			}
		}

		catch (...)
		{
			throw;	// re-throw
		}
	}
	
	void Core::SectionContainer::Init() throw (...)
	{
		try
		{
			if (size() > 0)
			{
				for (iterator tmp = begin(); tmp != end(); ++tmp)
					delete (*tmp);
				clear();
			}
			
			AddWininiSections();
			//AddRegistrySections();
			//AddFolderSections();
	
			//read all the existing sections
			std::for_each(begin(), end(), std::mem_fun(&Section::Read));

			//read disabled items and add to appropriate section
// 			ReadDisabledRegistry();
// 			ReadDisabledFolder();
// 			ReadDisabledWinini();
// 			ReadDisabledWinLogon();
		}
		
		catch (Boot::Exception const&)
		{
			throw;	// re-throw
		}
		
		catch (std::exception const& e)
		{
			throw Boot::Exception(tstring(toTstring(e.what())));
		}	
		
		catch (...)
		{
			throw;	// re-throw
		}
	}

	Core::Core(bool bDisplayUnused) throw (Boot::Exception)
		: m_StartSections(bDisplayUnused)
	{
		try
		{
			m_StartSections.Init();
		}

		catch (Boot::Exception const&)
		{
			throw;	// re-throw
		}	
		
		catch (...)
		{
			throw Boot::Exception(_T("Core::Core... Unknown exception"));
		}
	}

	void Core::Init() throw (Boot::Exception)
	{
		try
		{
			m_StartSections.Init();
		}

		catch (Boot::Exception const&)
		{
			throw;	// re-throw
		}	
		
		catch (...)
		{
			throw Boot::Exception(_T("Core::Init... Unknown exception"));
		}
	}

	int Core::GetItemCount() const throw (Boot::Exception)
	{
		try
		{
			int count = 0;

			for (const_iterator iter = m_StartSections.begin(); iter != m_StartSections.end(); ++iter)
				count += (*iter)->Count();

			return count;
		}
		
		catch (Boot::Exception const&)
		{
			throw;	// re-throw
		}
		
		catch (std::exception const& e)
		{
			throw Boot::Exception(tstring(toTstring(e.what())));
		}	
		
		catch (...)
		{
			throw Boot::Exception(_T("Core::GetItemCount... Unknown exception"));
		}
	}
	
	int Core::GetItemCount(const tstring &section) const throw (Boot::Exception)
	{
		try
		{
			if (section == _T("Main"))
			{
				//all sections
				return GetItemCount();
			}
			else if ((section == _T("StartUp")) || (section == _T("WinINI")) || (section == _T("Registry"))
		           || (section == _T("Registry\\Current User")) || (section == _T("Registry\\All Users"))
				   || (section == _T("Registry\\Computer")))
			{
				//search the right section in the list
				int count = 0;
				for (const_iterator iter = m_StartSections.begin(); iter != m_StartSections.end(); ++iter)
					if ((*iter)->GetSection().find(section) != std::string::npos)
						count += (*iter)->Count();
				return count;
			}
			else
			{
				//search the right section in the list
				for (const_iterator iter = m_StartSections.begin(); iter != m_StartSections.end(); ++iter)
					if ((*iter)->GetSection() == section)
						return (*iter)->Count();

				throw Boot::Exception(_T("Section does not exist"));
			}
		}
		
		catch (Boot::Exception const&)
		{
			throw;	// re-throw
		}
		
		catch (std::exception const& e)
		{
			throw Boot::Exception(tstring(toTstring(e.what())));
		}
		
		catch (...)
		{
			throw Boot::Exception(_T("Core::GetItemCount... Unknown exception"));
		}
	}
 
	tstring Core::GetLongSection(const tstring &section) const throw (Boot::Exception)
	{
		try
		{
			//search the right section in the list
			for (const_iterator iter = m_StartSections.begin(); iter != m_StartSections.end(); ++iter)
				if ((*iter)->GetSection() == section)
					return (*iter)->GetLongSection();

			return section;
		}

		catch (Boot::Exception const&)
		{
			throw;	// re-throw
		}
		
		catch (std::exception const& e)
		{
			throw Boot::Exception(tstring(toTstring(e.what())));
		}	
		
		catch (...)
		{
			throw Boot::Exception(_T("Core::GetLongSection... Unknown exception"));
		}
	}

	bool Core::Find(const tstring &path) const throw (Boot::Exception)
	{
		try
		{
			for (const_iterator iter = m_StartSections.begin(); iter != m_StartSections.end(); ++iter)
			{
				(*iter)->SelFirst();
				for (Item *pbtItem = (*iter)->GetNode(); (*iter)->Status() == List::OK; (*iter)->SelNext())
				{
					pbtItem = (*iter)->GetNode();
					if (pbtItem->GetPath() == path)
						return true;
				}
			}

			return false;
		}

		catch (Boot::Exception const&)
		{
			throw;	// re-throw
		}
		
		catch (std::exception const& e)
		{
			throw Boot::Exception(tstring(toTstring(e.what())));
		}

		catch (...)
		{
			throw Boot::Exception(_T("Core::Find... Unknown exception"));
		}
	}
	
	Item* Core::Get(const WorkItem &item) const throw(Boot::Exception)
	{
		try
		{
			for (const_iterator iter = m_StartSections.begin(); iter != m_StartSections.end(); ++iter)
			{
				if ((*iter)->GetSection() == item.GetSection())
				{
					(*iter)->SelFirst();
					for (Item *btItem = (*iter)->GetNode(); (*iter)->Status() == List::OK; (*iter)->SelNext())
					{
						btItem = (*iter)->GetNode();
						if ((item.GetCommand().find(btItem->GetPath()) != std::string::npos) && (item.GetName() == btItem->GetiName()))
							return btItem;
					}
				}
			}

			throw Boot::Exception(_T("Item not found"));
		}

		catch (Boot::Exception const&)
		{
			throw;	// re-throw
		}
		
		catch (std::exception const& e)
		{
			throw Boot::Exception(tstring(toTstring(e.what())));
		}

		catch (...)
		{
			throw Boot::Exception(_T("Core::Get... Unknown exception"));
		}
	}

	Item* Core::Convert(const WorkItem &item, Section *section) throw(Boot::Exception)
	{
		try
		{
			tstring strSection = item.GetSection();

			if (strSection.find(_T("StartUp")) != std::string::npos)
			{
				Folder::Item *ret = new Folder::Item(dynamic_cast<Folder::Section *>(section));
				ret->SetiName(item.GetName());
				ret->SetCommand(item.GetCommand());
				return ret;
			}
			else if (strSection.find(_T("Registry")) != std::string::npos)
			{
				if ((strSection.find(_T("Userinit")) != std::string::npos) || (strSection.find(_T("Shell")) != std::string::npos))
				{
					WinLogon::Item *ret = new WinLogon::Item(dynamic_cast<WinLogon::Section *>(section));
					ret->SetPath(item.GetCommand());
					return ret;
				}
				else if (strSection.find(_T("BHO")) != std::string::npos)
				{
					BHO::Item *ret = new BHO::Item(dynamic_cast<BHO::Section *>(section));
					ret->SetiName(item.GetName());
					ret->SetPath(item.GetCommand());
					ret->SetCLSID(item.GetName());
					return ret;
				}
				else
				{
					Registry::Item *ret = new Registry::Item(dynamic_cast<Registry::Section *>(section));
					ret->SetiName(item.GetName());
					ret->SetCommand(item.GetCommand());
					return ret;
				}
			}
			else if (strSection.find(_T("WinINI")) != std::string::npos)
			{
				Winini::Item *ret = new Winini::Item(dynamic_cast<Winini::Section *>(section));
				ret->SetPath(item.GetCommand());
				return ret;
			}
			else
				throw Boot::Exception(_T("Invalid section"));
		}

		catch (Boot::Exception const&)
		{
			throw;	// re-throw
		}

		catch (...)
		{
			throw Boot::Exception(_T("Core::Convert... Unknown exception"));
		}
	}

	void Core::Add(const WorkItem &item) throw (Boot::Exception)
	{
		try
		{
			for (iterator iter = m_StartSections.begin(); iter != m_StartSections.end(); ++iter)
				if ((*iter)->GetSection() == item.GetSection())
				{
					Item *pbtItem = Convert(item, *iter);
					
					try
					{
						(*iter)->Add(pbtItem);
					}

					catch (...)
					{
						delete pbtItem;
						throw;
					}
				}

			//log changes
			Log::LogInfo info;
			info.item    = item.GetName();
			info.section = item.GetSection();
			info.action  = Log::ADDED;
			Boot::LOG(info);
		}

		catch (Boot::Exception const&)
		{
			throw;	// re-throw
		}
		
		catch (std::exception const& e)
		{
			throw Boot::Exception(tstring(toTstring(e.what())));
		}

		catch (...)
		{
			throw Boot::Exception(_T("Core::Add... Unkown exception"));
		}
	}

	void Core::Delete(const WorkItem &item) throw (Boot::Exception)
	{
		try
		{
			Item *pbtItem = Get(item);
			if (!pbtItem)
				throw Boot::Exception(_T("Invalid item"));

			pbtItem->GetSection()->Delete(pbtItem);

			//log changes
			Log::LogInfo info;
			info.item    = item.GetName();
			info.section = item.GetSection();
			info.action  = Log::DELETED;
			Boot::LOG(info);
		}

		catch (Boot::Exception const&)
		{
			throw;	// re-throw
		}

		catch (...)
		{
			throw Boot::Exception(_T("Core::Delete... Unknown exception"));
		}
	}

	void Core::Rename(const WorkItem &item, const tstring &newName) throw (Boot::Exception)
	{
		try
		{
			Item *pbtItem = Get(item);
			if (!pbtItem)
				throw Boot::Exception(_T("Invalid item"));

			pbtItem->GetSection()->Rename(pbtItem, newName);

			//log changes
			Log::LogInfo info;
			info.item    = item.GetName()+  _T(" \xBB ") + newName;
			info.section = item.GetSection();
			info.action  = Log::RENAMED;
			Boot::LOG(info);
		}

		catch (Boot::Exception const&)
		{
			throw;	// re-throw
		}

		catch (...)
		{
			throw Boot::Exception(_T("Core::Rename... Unknown exception"));
		}
	}

	void Core::Enable(const WorkItem &item) throw (Boot::Exception)
	{
		try
		{
			Item *pbtItem = Get(item);
			if (!pbtItem)
				throw Boot::Exception(_T("Invalid item"));

			if (pbtItem->IsEnabled())
				throw Boot::Exception(_T("Item already is enabled"));

			pbtItem->GetSection()->Enable(pbtItem);

			//log changes
			Log::LogInfo info;
			info.item    = item.GetName();
			info.section = item.GetSection();
			info.action  = Log::BTENABLED;
			Boot::LOG(info);
		}

		catch (Boot::Exception const&)
		{
			throw;	// re-throw
		}

		catch (...)
		{
			throw Boot::Exception(_T("Core::Enable... Unknown exception"));
		}
	}

	void Core::Disable(const WorkItem &item) throw (Boot::Exception)
	{
		try
		{
			Item *pbtItem = Get(item);
			if (!pbtItem)
				throw Boot::Exception(_T("Invalid item"));

			if (!pbtItem->IsEnabled())
				throw Boot::Exception(_T("Item already is disabled"));

			pbtItem->GetSection()->Disable(pbtItem);

			//log changes
			Log::LogInfo info;
			info.item    = item.GetName();
			info.section = item.GetSection();
			info.action  = Log::BTDISABLED;
			Boot::LOG(info);
		}

		catch (Boot::Exception const&)
		{
			throw;	// re-throw
		}

		catch (...)
		{
			throw Boot::Exception(_T("Core::Disable... Unknown exception"));
		}
	}

	/**************************************************************************/
	
	//Initialise the static variable
	Log* Log::st_pLog = 0;
	
	Log::Log()
	{	
		TCHAR szFileName[MAX_PATH];
		::GetModuleFileName(GetModuleHandle(0), szFileName, MAX_PATH);
		File log;
		log.SetPath(szFileName);
		path = log.GetDir() + _T("log.log");
	
		st_pLog = this;
	}
	
	Log::~Log()
	{
		st_pLog = 0;
	}
	
	Log* Log::GetLog()
	{
		static Log t;
		return st_pLog;
	}
	
	tstring Log::GetUserName()
	{
		TCHAR cUserName[100];
		DWORD nUserName = sizeof(cUserName);
		::GetUserName(cUserName, static_cast<DWORD*>(&nUserName));
		tstring user = cUserName;
		return user;
	}
	
	void Log::DoLog(LogInfo info) throw (Boot::Exception)
	{
		try
		{
			assert((info.action >= ADDED) && (info.action <= BTDISABLED));

			tstring action[] = { _T("added"), _T("removed"), _T("renamed"), _T("modified"), _T("enabled"), _T("disabled") };

			TCHAR timebuf[128], datebuf[128];
			time_t ltime;
			time(&ltime);
			struct tm *today = localtime(&ltime);
			_tsetlocale(LC_TIME, _T(""));
			_tcsftime(timebuf, 128, _T("%X"), today);
			_tcsftime(datebuf, 128, _T("%x"), today);

		#ifdef UNICODE
			bool bBomPresent = false;
			bool bLogExists  = false;

			utf16ifstream iLogFile(toNarrowString(path).c_str(), std::ios::in | std::ios::binary);
			if (iLogFile.is_open())
			{
				bLogExists = true;

				//Read off byte order marker
				wchar_t bom;
				if (iLogFile.read(&bom, 1))
				{
					//Check the BOM - only handling little endian 16-bit
					if (bom == 0xFEFF)
						bBomPresent = true;
				}
				iLogFile.close();
			}

			//If an older log exists in an encoding not equal to UTF-16LE,
			//silently remove this log first and create a new one.
			if (bLogExists && !bBomPresent)
				_tremove(path.c_str());

			utf16ofstream oLogFile(toNarrowString(path).c_str(), std::ios::app | std::ios::binary);
			if (oLogFile.is_open())
			{
				if (!bBomPresent)
				{
					//Write ucs-2 little endian (utf-16) byte order marker to file.
					//Notice: little endian => to get 0xFF 0xFE, we write 0xFE 0xFF
					wchar_t bom = static_cast<wchar_t>(0xFEFF);
					oLogFile.seekp(std::ios::beg);
					oLogFile.write(&bom, 1);
				}
		#else
			tofstream oLogFile(path.c_str(), std::ios::app);
			if (oLogFile.is_open())
			{
		#endif

				oLogFile.seekp(std::ios::end);
				oLogFile << info.item.c_str() << NEWLINE;
				oLogFile << action[info.action].c_str() << NEWLINE;
				tstring user = (info.section.find(_T("Current User")) != std::string::npos) ? GetUserName().c_str() : _T("All Users");
				oLogFile << user.c_str() << NEWLINE;
				oLogFile << info.section.c_str() << NEWLINE;
				oLogFile << datebuf << NEWLINE;
				oLogFile << timebuf << NEWLINE;
				oLogFile.close();
			}
			else
				throw Boot::Exception(_T("Couldn't write to log"));
		}
		
		catch (Boot::Exception const&)
		{
			throw;	// re-throw
		}

		catch (...)
		{
			throw Boot::Exception(_T("Unknown error while generating log information"));
		}
	}
	
} //namespace Boot
