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

/// @file cLibraryInfo.cpp Definition of a class to retrieve file information.

#include "cLibraryInfo.h"
#include <tchar.h>

namespace Boot
{

	cLibraryInfo::cLibraryInfo()
	{
		m_DllInfo.stDllName        = _T("");
		m_DllInfo.stCompany        = _T("");
		m_DllInfo.stCopyRight      = _T("");
		m_DllInfo.stDescription    = _T("");
		m_DllInfo.stFileVersion    = _T("");
		m_DllInfo.stMajorVersion   = _T("");
		m_DllInfo.stMinorVersion   = _T("");
		m_DllInfo.stInternalName   = _T("");
		m_DllInfo.stOriginalName   = _T("");
		m_DllInfo.stProductName    = _T("");
		m_DllInfo.stProductVersion = _T("");
		m_DllInfo.stDllVersion     = _T("");
		m_DllInfo.stFileOS         = _T("");
		m_DllInfo.stFileType       = _T("");
		m_DllInfo.stLanguageId     = _T("");
		m_DllInfo.stCharSet        = _T("");
		
		m_pVersionInfo = 0;

		m_bInfoObtained = false;
		m_bLibLoaded = false;
	}
	 
	cLibraryInfo::~cLibraryInfo()
	{
		if (m_pVersionInfo != 0)
			delete [] m_pVersionInfo;
	}
	 
	void cLibraryInfo::SetDllName(const tstring &name)
	{
		m_bInfoObtained = (name == m_DllInfo.stDllName) ? true : false;
		m_DllInfo.stDllName = name;
	}
	 
	bool cLibraryInfo::GetInfo()
	{
		DWORD length;
		DWORD nullHandle;
		TCHAR fileName[MAX_PATH];
		LPVOID versionPtr;
		UINT verLength;

		// Determine whether the operating system can obtain version information
		// about the file. If version information is available, GetFileVersionInfoSize
		// returns the size in bytes of that information.
		// As with other file installation functions, GetFileVersionInfo works only
		// with Win32 file images. It does not work with 16-bit Windows file images.
		
		// Its is neccessary to call this function before calling GetFileVersionInfo.
		if (m_DllInfo.stDllName.length() <= 0)
			return false;

		_tcscpy(fileName, m_DllInfo.stDllName.c_str());
		length = ::GetFileVersionInfoSize(fileName, &nullHandle);

		if (length <= 0)
			return false;

		// Now call the GetFileVersionInfo function to version information
		// First initialize the member variable to recieve the information.
		m_pVersionInfo = new BYTE[length];
		if (!::GetFileVersionInfo(fileName, 0, length, m_pVersionInfo))
			return false;

		// The Win32 API contains the following predefined version information
		// strings.
		// CompanyName, FileDescription, FileVersion, InternalName, LegalCopyright,
		// OriginalFilename ProductName ProductVersion
		if (!::VerQueryValue(m_pVersionInfo, _T("\\"), &versionPtr, &verLength))
			return false;

		m_FixedFileInfo = *(VS_FIXEDFILEINFO*)versionPtr;

		if (!GetDynamicInfo())
			return false;

		// Get the information which is fixed for the module.
		if (!GetFixedFileInfo())
			return false;

		// Set the infoObtained flag to true.
		m_bInfoObtained = true;

		return true;
	}
	 
	bool cLibraryInfo::GetFixedFileInfo()
	{
		TCHAR version[256];
		_sntprintf(version, sizeof(version)/sizeof(version[0]) - 1, _T("%d.%d.%d.%d"), HIWORD(m_FixedFileInfo.dwFileVersionMS), LOWORD(m_FixedFileInfo.dwFileVersionMS),
					HIWORD(m_FixedFileInfo.dwFileVersionLS), LOWORD(m_FixedFileInfo.dwFileVersionLS));
		m_DllInfo.stDllVersion = version;

		if (m_FixedFileInfo.dwFileType == VFT_DRV)
			switch (m_FixedFileInfo.dwFileSubtype)
			{
			case VFT2_DRV_DISPLAY: m_DllInfo.stFileType = _T("Display driver"); break;
			case VFT2_DRV_INSTALLABLE: m_DllInfo.stFileType = _T("Installable driver"); break;
			case VFT2_DRV_KEYBOARD: m_DllInfo.stFileType = _T("Keyboard driver"); break;
			case VFT2_DRV_LANGUAGE: m_DllInfo.stFileType = _T("Language driver"); break;
			case VFT2_DRV_MOUSE: m_DllInfo.stFileType = _T("Mouse driver"); break;
			case VFT2_DRV_NETWORK: m_DllInfo.stFileType = _T("Network driver"); break;
			case VFT2_DRV_PRINTER: m_DllInfo.stFileType = _T("Printer driver"); break;
			case VFT2_DRV_SOUND: m_DllInfo.stFileType = _T("Sound driver"); break;
			case VFT2_DRV_SYSTEM: m_DllInfo.stFileType = _T("System driver"); break;
			case VFT2_UNKNOWN: m_DllInfo.stFileType = _T("Unknown driver"); break;
			}
		else if (m_FixedFileInfo.dwFileType == VFT_FONT)
			switch (m_FixedFileInfo.dwFileSubtype)
			{
			case VFT2_FONT_RASTER: m_DllInfo.stFileType = _T("Raster font"); break;
			case VFT2_FONT_TRUETYPE: m_DllInfo.stFileType = _T("Truetype font"); break;
			case VFT2_FONT_VECTOR: m_DllInfo.stFileType = _T("Vector font"); break;
			case VFT2_UNKNOWN: m_DllInfo.stFileType = _T("Unknown font"); break;
			}
		else if (m_FixedFileInfo.dwFileType == VFT_APP)
			m_DllInfo.stFileType = _T("Application");
		else if (m_FixedFileInfo.dwFileType == VFT_DLL)
			m_DllInfo.stFileType = _T("Dynamic link library");
		else if (m_FixedFileInfo.dwFileType == VFT_STATIC_LIB)
			m_DllInfo.stFileType = _T("Static link library");
		else if (m_FixedFileInfo.dwFileType == VFT_VXD)
			m_DllInfo.stFileType = _T("Virtual device");
		else if (m_FixedFileInfo.dwFileType == VFT_UNKNOWN)
			m_DllInfo.stFileType = _T("Unknown type");

		switch (m_FixedFileInfo.dwFileOS)
		{
		case VOS_DOS: m_DllInfo.stFileOS = _T("MS-DOS"); break;
		case VOS_DOS_WINDOWS16: m_DllInfo.stFileOS = _T("16-bit windows running on MS-DOS"); break;
		case VOS_DOS_WINDOWS32: m_DllInfo.stFileOS = _T("Win32 API running on MS-DOS"); break;
		case VOS_OS216: m_DllInfo.stFileOS = _T("16-bit OS/2"); break;
		case VOS_OS216_PM16: m_DllInfo.stFileOS = _T("16-bit Presentation manager running on 16-bit OS/2"); break;
		case VOS_OS232: m_DllInfo.stFileOS = _T("32-bit OS/2"); break;
		case VOS_NT: m_DllInfo.stFileOS = _T("Windows NT"); break;
		case VOS_NT_WINDOWS32: m_DllInfo.stFileOS = _T("Win32 API on Windows NT"); break;
		case VOS_UNKNOWN: m_DllInfo.stFileOS = _T("Unknown OS"); break;
		}


		return true;
	}
	 
	bool cLibraryInfo::GetDynamicInfo()
	{
		// TODO: Some exe's / dlls have incorrect / mismatched codepage info - ie, nWiz.exe v111.75 (NVIDIA nView Wizard) -
		// - perhaps add some check to find an "actual" codepages stored in the StringTable's szKey value (as opposed to the one
		// in \VarFileInfo\Translation) ?
		LPVOID lpTranslation;
		UINT uTranslationLen;
		if (::VerQueryValue(m_pVersionInfo, _T("\\VarFileInfo\\Translation"), (void**)&lpTranslation, static_cast<UINT*>(&uTranslationLen)))
		{
			m_Translation = *(TRANSLATE*)lpTranslation;

			DoVerQueryValueFor(m_DllInfo.stCompany);
			DoVerQueryValueFor(m_DllInfo.stCopyRight);
			DoVerQueryValueFor(m_DllInfo.stProductName);
			DoVerQueryValueFor(m_DllInfo.stProductVersion);
			DoVerQueryValueFor(m_DllInfo.stOriginalName);
			DoVerQueryValueFor(m_DllInfo.stDescription);
			DoVerQueryValueFor(m_DllInfo.stFileVersion);
			DoVerQueryValueFor(m_DllInfo.stInternalName);
			return true;
		}
		else return false;
	}

	bool cLibraryInfo::DoVerQueryValueFor(tstring &dllInfoMember)
	{
		TCHAR* verStringName = _T("");
		TCHAR query[MAX_PATH];

		if(&dllInfoMember == &m_DllInfo.stCompany)
		{
			verStringName = _T("CompanyName");
		}
		else if(&dllInfoMember == &m_DllInfo.stCopyRight)
		{
			verStringName = _T("LegalCopyRight");
		}
		else if(&dllInfoMember == &m_DllInfo.stProductName)
		{
			verStringName = _T("ProductName");
		}
		else if(&dllInfoMember == &m_DllInfo.stProductVersion)
		{
			verStringName = _T("ProductVersion");
		}
		else if(&dllInfoMember == &m_DllInfo.stOriginalName)
		{
			verStringName = _T("OriginalFileName");
		}
		else if(&dllInfoMember == &m_DllInfo.stDescription)
		{
			verStringName = _T("FileDescription");
		}
		else if(&dllInfoMember == &m_DllInfo.stFileVersion)
		{
			verStringName = _T("FileVersion");
		}
		else if(&dllInfoMember == &m_DllInfo.stInternalName)
		{
			verStringName = _T("InternalName");
		}

		_sntprintf(query, sizeof(query)/sizeof(query[0]) - 1, _T("\\stringFileInfo\\%04x%04x\\%s"), m_Translation.languageId, m_Translation.characterSet, verStringName);
		LPVOID lpQueryOutput;
		UINT uQueryLength;
		if(::VerQueryValue(m_pVersionInfo, query, &lpQueryOutput, &uQueryLength))
		{
			dllInfoMember = (LPTSTR)lpQueryOutput;
			return true;
		}
		else return false;
	}
	 
	bool cLibraryInfo::GetCompanyName(tstring &companyName)
	{
		bool stat = (!m_bInfoObtained) ? GetInfo() : true;
		companyName = (stat) ? m_DllInfo.stCompany : _T("");

		return stat;
	}
	 
	bool cLibraryInfo::GetCopyRight(tstring &copyRight)
	{
		bool stat = (!m_bInfoObtained) ? GetInfo() : true;
		copyRight = (stat) ? m_DllInfo.stCopyRight : _T("");

		return stat;
	}
	 
	bool cLibraryInfo::GetOrigFileName(tstring &origName)
	{
		bool stat = (!m_bInfoObtained) ? GetInfo() : true;
		origName = (stat) ? m_DllInfo.stOriginalName : _T("");

		return stat;
	}
	 
	bool cLibraryInfo::GetInternalName(tstring &internalName)
	{
		bool stat = (!m_bInfoObtained) ? GetInfo() : true;
		internalName = (stat) ? m_DllInfo.stInternalName : _T("");

		return stat;
	}
	 
	bool cLibraryInfo::GetProductName(tstring &prodName)
	{
		bool stat = (!m_bInfoObtained) ? GetInfo() : true;
		prodName = (stat) ? m_DllInfo.stProductName : _T("");

		return stat;
	}
	 
	bool cLibraryInfo::GetProductVersion(tstring &prodVersion)
	{
		bool stat = (!m_bInfoObtained) ? GetInfo() : true;
		prodVersion = (stat) ? m_DllInfo.stProductVersion : _T("");

		return stat;
	}
	 
	bool cLibraryInfo::GetDescription(tstring &desc)
	{
		bool stat = (!m_bInfoObtained) ? GetInfo() : true;
		desc = (stat) ? m_DllInfo.stDescription : _T("");

		return stat;
	}
	 
	bool cLibraryInfo::GetFileVersion(tstring &fileVer)
	{
		bool stat = (!m_bInfoObtained) ? GetInfo() : true;
		fileVer = (stat) ? m_DllInfo.stFileVersion : _T("");

		return stat;
	}
	 
	bool cLibraryInfo::GetMajorVersion(tstring &majorVer)
	{
		bool stat = (!m_bInfoObtained) ? GetInfo() : true;
		majorVer = (stat) ? m_DllInfo.stMajorVersion : _T("");

		return stat;
	}
	 
	bool cLibraryInfo::GetMinorVer(tstring &minorVer)
	{
		bool stat = (!m_bInfoObtained) ? GetInfo() : true;
		minorVer = (stat) ? m_DllInfo.stMinorVersion : _T("");

		return stat;
	}
	 
	bool cLibraryInfo::GetLanguage(tstring &language)
	{
		bool stat = (!m_bInfoObtained) ? GetInfo() : true;
		language = (stat) ? m_DllInfo.stLanguageId : _T("");

		return stat;
	}
	 
	bool cLibraryInfo::GetCharacterSet(tstring &characterSet)
	{
		bool stat = (!m_bInfoObtained) ? GetInfo() : true;
		characterSet = (stat) ? m_DllInfo.stCharSet : _T("");

		return stat;
	}
	 
	bool cLibraryInfo::GetLanguageId(WORD &langId)
	{
		bool stat = (!m_bInfoObtained) ? GetInfo() : true;
		langId = (stat) ? m_Translation.languageId : WORD(0);

		return stat;
	}
	 
	bool cLibraryInfo::GetCharacterset(WORD &charSet)
	{
		bool stat = (!m_bInfoObtained) ? GetInfo() : true;
		charSet = (stat) ? m_Translation.characterSet : (WORD)0;

		return stat;
	}
	 
	bool cLibraryInfo::GetDllInfo(DLLINFO &info)
	{
		bool stat = (!m_bInfoObtained) ? GetInfo() : true;
		
		if (stat)
		{
			info.stDllName = m_DllInfo.stDllName;
			info.stCompany = m_DllInfo.stCompany;
			info.stCopyRight = m_DllInfo.stCopyRight;
			info.stDescription = m_DllInfo.stDescription;
			info.stFileVersion = m_DllInfo.stFileVersion;
			info.stMajorVersion = m_DllInfo.stMajorVersion;
			info.stMinorVersion = m_DllInfo.stMinorVersion;
			info.stInternalName = m_DllInfo.stInternalName;
			info.stOriginalName = m_DllInfo.stOriginalName;
			info.stProductName = m_DllInfo.stProductName;
			info.stProductVersion = m_DllInfo.stProductVersion;
			info.stDllVersion = m_DllInfo.stDllVersion;
			info.stFileOS = m_DllInfo.stFileOS;
			info.stFileType = m_DllInfo.stFileType;
			info.stLanguageId = m_DllInfo.stLanguageId;
			info.stCharSet = m_DllInfo.stCharSet;
		}
		
		return stat;
	}
	 
	bool cLibraryInfo::GetDllInfo(BYTE *versionInfo)
	{
		bool stat = (!m_bInfoObtained) ? GetInfo() : true;
		versionInfo = (stat) ? m_pVersionInfo : 0;
		
		return stat;
	}
	 
	bool cLibraryInfo::GetDllInfo(VS_FIXEDFILEINFO &fixedInfo)
	{
		bool stat = (!m_bInfoObtained) ? GetInfo() : true;
		
		if (stat)
		{
			fixedInfo.dwSignature = m_FixedFileInfo.dwSignature;
			fixedInfo.dwStrucVersion = m_FixedFileInfo.dwStrucVersion;
			fixedInfo.dwFileVersionMS = m_FixedFileInfo.dwFileVersionMS; 
			fixedInfo.dwFileVersionLS = m_FixedFileInfo.dwFileVersionLS;
			fixedInfo.dwProductVersionMS = m_FixedFileInfo.dwProductVersionMS; 
			fixedInfo.dwProductVersionLS = m_FixedFileInfo.dwProductVersionLS;
			fixedInfo.dwFileFlagsMask = m_FixedFileInfo.dwFileFlagsMask;
			fixedInfo.dwFileFlags = m_FixedFileInfo.dwFileFlags; 
			fixedInfo.dwFileOS = m_FixedFileInfo.dwFileOS;
			fixedInfo.dwFileType = m_FixedFileInfo.dwFileType;
			fixedInfo.dwFileSubtype = m_FixedFileInfo.dwFileSubtype; 
			fixedInfo.dwFileDateMS = m_FixedFileInfo.dwFileDateMS;
			fixedInfo.dwFileDateLS = m_FixedFileInfo.dwFileDateLS;
		}
		
		return stat;
	}

}	// namespace Boot
