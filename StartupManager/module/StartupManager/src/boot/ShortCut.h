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
 
#ifndef SHORTCUT_H
#define SHORTCUT_H

/// @file ShortCut.h Declaration of a class representing a shortcut.

#include "File.h"

namespace Boot
{

	/// This class represents a shortcut.
	class ShortCut : public File
	{
	public:
		/**
		*	@name Construction/Destruction
		*	@{
		*/
		/// The constructor.
		ShortCut();
		/// The destructor.
		virtual ~ShortCut();
		/**	@}	<!-- end of the group --> */

		/**
		*	@name Operations
		*	@{
		*/
		/**
		*	@brief Create a shortcut to a file.
		*	@param file The file to which the shortcut should be created.
		*	@param Dir The directory in which the shortcut should be
		*	placed.
		*	@param Name The name of the shortcut.
		*	@param Args (optional) Arguments to pas to the file in the
		*	shortcut.
		*	@return A boolean indicating success (true) or failure
		*	(false).
		*/
		bool Create(File file, const tstring &Dir, const tstring &Name, const tstring &Args = _T(""));
		/**
		*	@brief Create a shortcut to a file.
		*	@param Path The path of the file to which the shortcut should
		*	be created.
		*	@param Dir The directory in which the shortcut should be
		*	placed.
		*	@param Name The name of the shortcut.
		*	@param Args (optional) Arguments to pas to the file in the
		*	shortcut.
		*	@return A boolean indicating success (true) or failure
		*	(false).
		*/
		bool Create(const tstring &Path, const tstring &Dir, const tstring &Name, const tstring &Args = _T(""));
		/**
		*	@brief Get the path of the file to which the represented
		*	shortcut refers.
		*	@param bSuccess Boolean indicating success (true) or failure
		*	(false).
		*	@return The path of the file to which the represented shortcut
		*	refers.
		*/
		tstring ResolveLinkTarget(bool &bSuccess);
		/**	@}	<!-- end of the group --> */

		/**
		*	@name Getter functions
		*	@{
		*/
		/**
		*	@brief Get the properties of the represented file (author,
		*	version, ...)
		*	@param info (out) A structure containing the properties of the
		*	represented file
		*/
		virtual void GetProperties(cLibraryInfo::DLLINFO &info);
		/**
		*	@brief Get the file that is represented.
		*	@return A reference to the represented shortcut.
		*/
		File& Link();
		/**	@}	<!-- end of the group --> */

	protected:
		File m_File;	///<	The shortcut we represent.
	};

	inline ShortCut::ShortCut()
	{
	}

	inline ShortCut::~ShortCut()
	{
	}

	inline File& ShortCut::Link()
	{
		return m_File;
	}

}	// namespace Boot

#endif	// SHORTCUT_H
