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
 
#ifndef FILE_H
#define FILE_H

/// @file File.h Declaration of a class representing a file.

#include "cLibraryInfo.h"

namespace Boot
{

	/**
	*	@brief This class represents a file.
	*	@details File implements some file operations.
	*/
	class File
	{
	public:
		/**
		*	@name Construction/Destruction
		*	@{
		*/
		/// The constructor.
		File();
		/// The destructor.
		virtual ~File();
		/**	@}	<!-- end of the group --> */
	 
		/**
		*	@name Setter functions
		*	@{
		*/
		/**
		*	@brief Assign a file.
		*	@see GetPath()
		*	@param file The path of the file to assign.
		*/
		virtual void SetPath(const tstring &file);
		/**	@}	<!-- end of the group --> */

		/**
		*	@name Getter functions
		*	@{
		*/
		/**
		*	@brief Get the path of the file which is represented.
		*	@see SetPath()
		*	@return The path of the represented file.
		*/
		virtual tstring GetPath() const {return m_Path;}
		/**
		*	@brief Get the extension of the represented file.
		*	@see GetName()
		*	@see GetDir()
		*	@return The extenstion of the represented file.
		*/
		virtual tstring GetExt();
		/**
		*	@brief Get the name of the represented file.
		*	@see GetExt()
		*	@see GetDir()
		*	@return The name of the represented file.
		*/
		virtual tstring GetName();
		/**
		*	@brief Get the directory in which the represented file
		*	is located.
		*	@see GetExt()
		*	@see GetName()
		*	@return The directory in which the represented file is
		*	located.
		*/
		virtual tstring GetDir();
		/**
		*	@brief Get the icon of the file which is represented.
		*	@see GetSmallIcon()
		*	@return The icon of the represented file.
		*/
		virtual HICON GetIcon();
		/**
		*	@brief Get the 16x16 icon of the represented file.
		*	@see GetIcon()
		*	@return The 16x16 icon of the represented file.
		*/
		virtual HICON GetSmallIcon();
		/**
		*	@brief Get the properties of the represented file
		*	(author, version, ...)
		*	@param info (out) A structure containing the properties
		*	of the represented file
		*/
		virtual void GetProperties(cLibraryInfo::DLLINFO &info);
		/**	@}	<!-- end of the group --> */

		/**
		*	@name Operations
		*	@{
		*/
		/**
		*	@brief Rename the represented file.
		*	@param newpath The new path for the file.
		*/
		virtual void RenameFile(const tstring &newpath);
		/// Remove the represented file.
		virtual void DeleteFile();
		/**
		*	@brief Create a shortcut to the represented file.
		*	@param Dir The directory in which the shortcut
		*	should be placed.
		*	@param Name The shortcuts name.
		*	@param Args (Optional) Arguments to pas to the
		*	file in the shortcut.
		*	@return The path of the shortcut.
		*/
		virtual tstring CreateShortcut(const tstring &Dir, const tstring &Name,
			const tstring &Args = _T(""));
		/**	@}	<!-- end of the group --> */

		/**
		*	@brief Check if the represented file exists.
		*	@return A boolean indication whether the file
		*	exists (true) or not (false).
		*/
		virtual bool IsExist();
	 
	protected:
		tstring m_Path;	///<	The path of the file which is represented.
	};

	inline File::File()
		: m_Path(_T(""))
	{
	}

	inline File::~File()
	{
	}

	inline void File::DeleteFile()
	{
		_tremove(m_Path.c_str());
	}

}	// namespace Boot

#endif	// FILE_H
