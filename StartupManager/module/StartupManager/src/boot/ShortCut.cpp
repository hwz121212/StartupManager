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

/// @file ShortCut.cpp Definition of a class representing a shortcut.

#include "ShortCut.h"
#include <shlobj.h>
#include <cassert>
#include <tchar.h>

namespace Boot
{

	bool ShortCut::Create(File file, const tstring &Dir, const tstring &Name, const tstring &Args)
	{
		SetPath(file.GetPath());
		m_File.SetPath(file.CreateShortcut(Dir, Name, Args));
		return (m_File.GetPath().length() > 0);
	}

	bool ShortCut::Create(const tstring &Path, const tstring &Dir, const tstring &Name, const tstring &Args)
	{
		File file;
		file.SetPath(Path);
		return Create(file, Dir, Name, Args);
	}
	 
	tstring ShortCut::ResolveLinkTarget(bool &bSuccess)
	{
		bSuccess = true;

		if (m_Path.length() > 0)
			return m_Path;
		
		assert(m_File.GetPath().length() > 0);			
	 
		HRESULT hres;
		IShellLink *psl;
		TCHAR szGotPath[MAX_PATH + 1], szArguments[MAX_PATH + 1];
		WIN32_FIND_DATA wfd;
		LPTSTR pszPath = _T(""); // assume failure
		tstring ret;

		CoInitialize(0);
	 
		// Get a pointer to the IShellLink interface.
		hres = CoCreateInstance(CLSID_ShellLink, 0, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&psl);
		if (SUCCEEDED(hres))
		{
			IPersistFile *ppf;

			// Get a pointer to the IPersistFile interface.
			if (SUCCEEDED(psl->QueryInterface(IID_IPersistFile, (void**)&ppf)))
			{
				hres = ppf->Load(toWideString(m_File.GetPath()).c_str(), STGM_READ);
				if (SUCCEEDED(hres))
				{
					// Resolve the link.
					hres = psl->Resolve(0, MAKELPARAM(SLR_ANY_MATCH | SLR_NO_UI, 500));
					if (SUCCEEDED(hres))
					{
						if (hres != S_OK)
							bSuccess = false;

						_tcscpy(szGotPath, m_File.GetPath().c_str());
						// Get the path to the link target.
						hres = psl->GetPath(szGotPath, MAX_PATH, (WIN32_FIND_DATA*)&wfd, SLGP_UNCPRIORITY);
						pszPath = szGotPath;
						if (!SUCCEEDED(hres))
						{
							bSuccess = false;
							ppf->Release();
							psl->Release();
							CoUninitialize();
							return _T("");
						}

						// Get the arguments
						psl->GetArguments(szArguments, MAX_PATH);
					}
					else
					{
						bSuccess = false;
						ppf->Release();
						psl->Release();
						CoUninitialize();
						return _T("");
					}
				}
				else
				{
					bSuccess = false;
					ppf->Release();
					psl->Release();
					CoUninitialize();
					return _T("");
				}
				
				// Release pointer to IPersistFile interface.
				ppf->Release();
			}
			else
			{
				bSuccess = false;
				psl->Release();
				CoUninitialize();
				return _T("");
			}

			// Release pointer to IShellLink interface.
			psl->Release();
		}
		else
		{
			bSuccess = false;
			CoUninitialize();
			return _T("");
		}

		CoUninitialize();
		ret = pszPath;
		if (_tcslen(szArguments) > 0)
		{
			ret = _T("\"") + ret + _T("\" ");
			ret = ret + szArguments;
		}
		return (m_Path = ret);
	}

	void ShortCut::GetProperties(cLibraryInfo::DLLINFO &info)
	{
		if (m_Path.length() <= 0)
		{
			bool bSuccess = true;
			ResolveLinkTarget(bSuccess);
		}

		File::GetProperties(info);
	}

}	// namespace Boot
