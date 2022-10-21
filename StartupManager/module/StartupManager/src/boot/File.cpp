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

/// @file File.cpp Definition of a class representing a file.

#include "File.h"

#include <shellapi.h>
#include <shlobj.h>
#include <tchar.h>
#include <cassert>
#include <io.h>

namespace Boot
{

	void File::SetPath(const tstring &file)
	{
		m_Path = file;
		trim(m_Path);
	}

	tstring File::GetExt()
	{
		if (m_Path.length() <= 0)
			return _T("");
		
		std::string::size_type pos = m_Path.find_last_of(_T("."));

		if (pos != std::string::npos)
			return m_Path.substr(pos + 1);
		
		return _T("");
	}

	tstring File::GetName()
	{
		if (m_Path.length() <= 0)
			return _T("");
		
		std::string::size_type pos = m_Path.find_last_of(_T("\\/:"));

		if (pos != std::string::npos)
			return m_Path.substr(pos + 1);
		 
		return m_Path;
	}
	 
	tstring File::GetDir()
	{
		if (m_Path.length() <= 0)
			return _T("");
		
		tstring name = GetName();
		tstring dir = m_Path;
		dir.erase(m_Path.length() - name.length(), name.length());
		
		// if no directory specified, it should be one of the system directories
		if ((dir.length() <= 0) || (dir.at(0) == _T(' ')))
		{
			TCHAR *buffer = new TCHAR[MAX_PATH + 1];
			::GetWindowsDirectory(buffer, MAX_PATH+1);
			TCHAR *test = new TCHAR[MAX_PATH + 1];
			_sntprintf(test, sizeof(TCHAR[MAX_PATH+1])/sizeof(TCHAR) - 1, _T("%s\\%s"), buffer, name.c_str());
			File tFile;
			tFile.SetPath(test);
			delete [] test;
			if (tFile.IsExist())
			{
				dir = buffer;
				dir += _T("\\");
				delete [] buffer;
			}
			else
			{
				delete [] buffer;
				
				buffer = new TCHAR[MAX_PATH + 1];
				::GetSystemDirectory(buffer, MAX_PATH+1);
				test = new TCHAR[MAX_PATH + 1];
				_sntprintf(test, sizeof(TCHAR[MAX_PATH+1])/sizeof(TCHAR) - 1, _T("%s\\%s"), buffer, name.c_str());
				tFile.SetPath(test);
				delete [] test;
				if (tFile.IsExist())
				{
					dir = buffer;
					dir += _T("\\");
				}
				else
					dir = _T("");
				delete [] buffer;
			}
		}

		return dir;
	}
	 
	HICON File::GetIcon()
	{
		tstring fullpath = GetDir() + GetName();
		
		HICON icon = 0;
		if (::ExtractIconEx(fullpath.c_str(), 0, &icon, 0, 1) == 1)
			return icon;
		
		SHFILEINFO fi;
		::ZeroMemory(&fi, sizeof(SHFILEINFO));
		SHGetFileInfo(fullpath.c_str(), FILE_ATTRIBUTE_NORMAL, &fi, sizeof(fi), SHGFI_ICON | SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES);
		return fi.hIcon;
	}

	HICON File::GetSmallIcon()
	{
		tstring fullpath = GetDir() + GetName();
		
		HICON icon = 0;
		if (::ExtractIconEx(fullpath.c_str(), 0, 0, &icon, 1) == 1)
			return icon;
		else
			return 0;
	}

	void File::RenameFile(const tstring &newpath)
	{
		assert(m_Path.length() > 0);
		assert(newpath.length() > 0);
		
		tstring fullpath = GetDir() + GetName();
		_trename(fullpath.c_str(), newpath.c_str());
		m_Path = newpath;
	}
	 
	bool File::IsExist()
	{
		if (m_Path.length() <= 0)
			return false;
		
		return ((_taccess((GetDir() + GetName()).c_str(), 0)) != -1);
	}
	 
	tstring File::CreateShortcut(const tstring &Dir, const tstring &Name, const tstring &Args)
	{
		assert(m_Path.length() > 0);
		assert(Dir.length() > 0);
		assert(Name.length() > 0);
		
		if (SUCCEEDED(CoInitialize(0)))
		{
			tstring location = Dir + _T("\\") + Name;
			if (Name.find(_T(".lnk")) != (Name.length()-4))
				location += _T(".lnk");
					
			IShellLink *pLink;
			IPersistFile *pPersistFile;
			if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, 0, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pLink)))
			{
				tstring fullpath = GetDir() + GetName();
				pLink->SetPath(fullpath.c_str());
				pLink->SetArguments(Args.c_str());
				pLink->SetDescription(GetName().c_str());
				pLink->SetShowCmd(SW_SHOW);

				if (SUCCEEDED(pLink->QueryInterface(IID_IPersistFile, (void**)&pPersistFile)))
				{
					pPersistFile->Save(toWideString(location).c_str(), true);
					pPersistFile->Release();
				}
				else
				{
					pLink->Release();
					CoUninitialize();
					return _T("");
				}
			 
				pLink->Release();
			}
			else
			{
				CoUninitialize();
				return _T("");
			}

			CoUninitialize();
			return location;
		}
		 
		return _T("");
	}
	 
	void File::GetProperties(cLibraryInfo::DLLINFO &info)
	{
		if (m_Path.length() <= 0)
		{
			info.stDllName		  = _T("");
			info.stCompany		  = _T("");
			info.stCopyRight	  = _T("");
			info.stFileVersion	  = _T("");
			info.stInternalName	  = _T("");
			info.stOriginalName	  = _T("");
			info.stProductName	  = _T("");
			info.stProductVersion = _T("");
			info.stDllVersion	  = _T("");
			info.stFileOS		  = _T("");
			info.stFileType		  = _T("");
			info.stDescription	  = _T("");
			return;
		}
		
		tstring fullpath = GetDir() + GetName();
		cLibraryInfo lInfo;
		lInfo.SetDllName(fullpath.c_str());
		if (!lInfo.GetDllInfo(info))
		{
			info.stDllName		  = GetName().c_str();
			info.stCompany		  = _T("");
			info.stCopyRight	  = _T("");
			info.stFileVersion	  = _T("");
			info.stInternalName	  = _T("");
			info.stOriginalName	  = _T("");
			info.stProductName	  = _T("");
			info.stProductVersion = _T("");
			info.stDllVersion	  = _T("");
			info.stFileOS		  = _T("");
			info.stFileType		  = _T("");
			info.stDescription	  = fullpath.c_str();
		}
	}

}	// namespace Boot
