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

#ifndef CLIBRARYINFO_H
#define CLIBRARYINFO_H

/// @file cLibraryInfo.h Declaration of a class to retrieve file information.

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <shlobj.h>

#include "..\types\tstring.h"

namespace Boot
{

	/// A class to retrieve file information.
	class cLibraryInfo
	{
	public:
		/// A structure containing all file information.
		typedef struct _DLLINFO
		{
			tstring stDllName;			///<	The name of the file.
			tstring stCompany;			///<	The name of the company.
			tstring stCopyRight;		///<	Copyright notice.
			tstring stDescription;		///<	File description.
			tstring stFileVersion;		///<	File version.
			tstring stMajorVersion;		///<	Major version.
			tstring stMinorVersion;		///<	Minor version.
			tstring stInternalName;		///<	Internal name.
			tstring stOriginalName;		///<	Original name.
			tstring stProductName;		///<	Product name.
			tstring stProductVersion;	///<	Product version.
			tstring stDllVersion;		///<	Dll version.
			tstring stFileOS;			///<	Operating system for which the file was written.
			tstring stFileType;			///<	File type.
			tstring stLanguageId;		///<	Language in which the file is written.
			tstring stCharSet;			///<	Characterset the file uses.
		} DLLINFO, *LPDLLINFO;			///<	Struct variable and pointer.

		/**
		*	@name Construction/Destruction
		*	@{
		*/
		/// The constructor.
		cLibraryInfo();
		/// The destructor.
		~cLibraryInfo();
		/**	@}	<!-- end of the group --> */

		/**
		*	@name Operations
		*	@{
		*/
		/**
		*	@brief Set the name of the file.
		*	@param name The files name.
		*/
		void SetDllName(const tstring &name);
		/**
		*	@brief Get the name of the file.
		*	@return A const reference to the files name.
		*/
		const tstring& GetDllName() const;
		/**
		*	@brief Get the character set.
		*	@param characterSet (out) The characterset.
		*	@return A boolean indicating success or failure.
		*/
		bool GetCharacterSet(tstring &characterSet);
		/**
		*	@brief Get the language.
		*	@param language (out) The language.
		*	@return A boolean indicating success or failure.
		*/
		bool GetLanguage(tstring &language);
		/**
		*	@brief Get the minor version.
		*	@param minorVer (out) The minor version.
		*	@return A boolean indicating success or failure.
		*/
		bool GetMinorVer(tstring &minorVer);
		/**
		*	@brief Get the major version.
		*	@param majorVer (out) The major version.
		*	@return A boolean indicating success or failure.
		*/
		bool GetMajorVersion(tstring &majorVer);
		/**
		*	@brief Get the file version.
		*	@param fileVer (out) The file version.
		*	@return A boolean indicating success or failure.
		*/
		bool GetFileVersion(tstring &fileVer);
		/**
		*	@brief Get the files description.
		*	@param desc (out) The description.
		*	@return A boolean indicating success or failure.
		*/
		bool GetDescription(tstring &desc);
		/**
		*	@brief Get the product version.
		*	@param prodVersion (out) The product version.
		*	@return A boolean indicating success or failure.
		*/
		bool GetProductVersion(tstring &prodVersion);
		/**
		*	@brief Get the product name.
		*	@param prodName (out) The product name.
		*	@return A boolean indicating success or failure.
		*/
		bool GetProductName(tstring &prodName);
		/**
		*	@brief Get the internal name.
		*	@param internalName (out) The internal name.
		*	@return A boolean indicating success or failure.
		*/
		bool GetInternalName(tstring &internalName);
		/**
		*	@brief Get the original name.
		*	@param origName (out) The original name.
		*	@return A boolean indicating success or failure.
		*/
		bool GetOrigFileName(tstring &origName);
		/**
		*	@brief Get the copyright notice.
		*	@param copyRight (out) The copyright notice.
		*	@return A boolean indicating success or failure.
		*/
		bool GetCopyRight(tstring &copyRight);
		/**
		*	@brief Get the company name.
		*	@param companyName (out) The company name.
		*	@return A boolean indicating success or failure.
		*/
		bool GetCompanyName(tstring &companyName);
		/**
		*	@brief Get the information as DLLINFO structure.
		*	@param info (out) A structure containing the file
		*	info.
		*	@return A boolean indicating success or failure.
		*/
		bool GetDllInfo(DLLINFO &info);
		/**
		*	@brief Get the information as a BYTE pointer.
		*	@param versionInfo (out) A pointer to an internal
		*	file info structure.
		*	@return A boolean indicating success or failure.
		*/
		bool GetDllInfo(BYTE *versionInfo);
		/**
		*	@brief Get the information as a VS_FIXEDFILEINFO
		*	structure.
		*	@param fixedInfo (out) The VS_FIXEDFILEINFO
		*	structure.
		*	@return A boolean indicating success or failure.
		*/
		bool GetDllInfo(VS_FIXEDFILEINFO &fixedInfo);
		/**
		*	@brief Get the files language identifier.
		*	@param langId (out) The language identifier.
		*	@return A boolean indicating success or failure.
		*/
		bool GetLanguageId(WORD &langId);
		/**
		*	@brief Get the files character set.
		*	@param charSet (out) The files character set.
		*	@return A boolean indicating success or failure.
		*/
		bool GetCharacterset(WORD &charSet);
		/**	@}	<!-- end of the group --> */

	private:
		/**
		*	@name Operations
		*	@{
		*/
		/// Get dynamic information
		bool GetDynamicInfo();
		/// Call VerQueryValue to fill the given m_DllInfo member
		bool DoVerQueryValueFor(tstring &dllInfoMember);
		/// Get fixed information
		bool GetFixedFileInfo();
		/// Get file information
		bool GetInfo();
		/**	@}	<!-- end of the group --> */
		
		/// Structure containing language information
		struct TRANSLATE
		{
			WORD languageId;	///<	The language id
			WORD characterSet;	///<	The used character set
		} m_Translation;		///<	The TRANSLATE member variable

		BYTE *m_pVersionInfo;				///<	Pointer to a BYTE containing the file information.
		VS_FIXEDFILEINFO m_FixedFileInfo;	///<	Structure containing the fixed file information.
		DLLINFO m_DllInfo;					///<	Structure containing the file information.
		bool m_bInfoObtained;				///<	Indicates whether information was retrieved or not.
		bool m_bLibLoaded;					///<	Indicates whether library is loaded or not.
	};
 
}	// namespace Boot

#endif	// CLIBRARYINFO_H
