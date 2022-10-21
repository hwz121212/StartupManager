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

#ifndef BTFOLDER_H
#define BTFOLDER_H

/// @file BtFolder.h Declarations for the start folder section classes.

#include "BtSection.h"
#include "ShortCut.h"
#include "BtFolderException.h"

#include <shlobj.h>
#include <iostream>

namespace Boot
{

	/**
	 *	@namespace Boot::Folder
	 *	@brief Start folder sections stuff.
	 *	@details This namespace contains definitions for start folder
	 *	sections and items.
	 */
	namespace Folder
	{

		/**
		 *	@brief A start folder section.
		 *	@details All start folder sections can be represented
		 *	using this class.
		 */
		class Section : public Boot::Section
		{
		public:
			/// Enumeration of possible start folder access methods.
			enum USER
			{
				ALL		= CSIDL_COMMON_STARTUP,	///<	Access al users start folder.
				CURRENT = CSIDL_STARTUP			///<	Access curent user start folder.
			};

			/**
			 *	@name Construction/Destruction
			 *	@{
			 */
			/**
			 *	@brief The constructor.
			 *	@param user The desired access method.
			 *	@param section The name of the represented section.
			 */
			Section(USER user, const tstring &section);
			/// The destructor.
			virtual ~Section();
			/**	@} <!-- end of the group --> */
			
			/// Check whether this section was initialized properly.
			bool ProperlyInitialized();

			/**
			 *	@name Getter functions
			 *	@{
			 */
			/**
			 *	@brief Get the long section name.
			 *	@return The long section name.
			 */
			virtual tstring GetLongSection() const;
			/**
			 *	@brief Get the start folder access method.
			 *	@return The access method.
			 */
			USER GetUser() const;
			/**	@} <!-- end of the group --> */

		protected:
			/**
			 *	@name Operations
			 *	@{
			 */
			/// Read and list this sections items.
			virtual void Read();
			/**
			 *	@brief Add an item to this section.
			 *	@param item A pointer to the item to add (must be of
			 *	type Boot::Folder::Item!).
			 *	@see Delete()
			 *	@exception Boot::Exception An exception that is
			 *	thrown in case of an error.
			 */
			virtual void Add(Boot::Item *item) throw (Boot::Exception);
			/**
			 *	@brief Delete an item from this section.
			 *	@param item A pointer to the item to delete (must be
			 *	of type Boot::Folder::Item!).
			 *	@see Add()
			 *	@exception Boot::Exception An exception that is thrown
			 *	in case of an error.
			 */
			virtual void Delete(Boot::Item *item) throw (Boot::Exception);
			/**
			 *	@brief Rename an item from this section.
			 *	@param item A pointer to the item to rename (must be
			 *	of type Boot::Folder::Item!).
			 *	@param newName The new name for the item.
			 *	@exception Boot::Exception An exception that is
			 *	thrown in case of an error.
			 */
			virtual void Rename(Boot::Item *item, const tstring &newName) throw (Boot::Exception);
			/**
			 *	@brief Enable an item from this section.
			 *	@param item A pointer to the item to enable (must be
			 *	of type Boot::Folder::Item!).
			 *	@pre The item is disabled.
			 *	@post The item is enabled.
			 *	@see Disable()
			 *	@exception Boot::Exception An exception that is thrown
			 *	in case of an error.
			 */
			virtual void Enable(Boot::Item *& item) throw (Boot::Exception);
			/**
			 *	@brief Disabled an item from this section.
			 *	@param item A pointer to the item to disable (must
			 *	be of type Boot::Folder::Item!).
			 *	@pre The item is enabled.
			 *	@post The item is disabled.
			 *	@see Enable()
			 *	@exception Boot::Exception An exception that is thrown
			 *	in case of an error.
			 */
			virtual void Disable(Boot::Item *& item) throw (Boot::Exception);
			/**	@} <!-- end of the group --> */
			
		private:
			/**
			 *	@name Construction/Destruction
			 *	@{
			 */
			/**
			 *	@brief Copy constructor.
			 *	@details Private to prevent copy construction.
			 */
			Section(const Section &f);
			/**
			 *	@brief Copy assignment operator.
			 *	@details Private to prevent copy assignment.
			 */
			Section& operator= (const Section &f);
			/**	@} <!-- end of the group --> */

			/// Insert an item that was read.
			void InsertReadItem(LPCTSTR szProgramPath, LPCTSTR szValueName) throw (Boot::Exception);

			/// Get the location of disabled items.
			void GetDisabledLocation() throw(Boot::Exception);
			
			const USER m_User; ///<	The access method: current user (CURRENT) or all users (ALL).
		};

		inline Section::~Section()
		{
		}

		inline Section::USER Section::GetUser() const
		{
			return m_User;
		}

		/**
		 *	@brief A start folder item.
		 *	@details A start folder item can only be used with a start
		 *	folder section.
		 */
		class Item : public Boot::Item, public ShortCut
		{
		public:
			/**
			 *	@name Construction/Destruction
			 *	@{
			 */
			/**
			 *	@brief The constructor.
			 *	@param section A pointer to the section in which the
			 *	item is located.
			 */
			Item(Section *section);
			/// The destructor.
			virtual ~Item();
			/**	@} <!-- end of the group --> */
			
			/**
			 *	@name File function overrides.
			 *	@{
			 *	@brief Solve ambiguous access by forcing the use of
			 *	ShortCut functions.
			 *	@details See ShortCut for more information about these
			 *	functions.
			 */
			virtual void SetPath(const tstring &file);
			virtual tstring GetPath() const;
			virtual tstring GetExt();
			virtual tstring GetName();
			virtual tstring GetDir();
			virtual HICON GetIcon();
			virtual HICON GetSmallIcon();
			virtual void RenameFile(const tstring &newpath);
			virtual void DeleteFile();
			virtual bool IsExist();
			virtual tstring CreateShortcut(const tstring &Dir, const tstring &Name);
			/**	@}	<!-- end of the group --> */

			/**
			 *	@name Setter functions
			 *	@{
			 */
			/**
			 *	@brief Set the items path and parameters.
			 *	@param file The items path.
			 *	@param flag The parameters.
			 *	@see GetFlags()
			 *	@see SetCommand()
			 *	@see GetCommand()
			 */
			void SetPathFlags(const tstring &file, const tstring &flag);
			/**
			 *	@brief Set the items command.
			 *	@param command The items command.
			 *	@see SetPathFlags()
			 *	@see GetFlags()
			 *	@see GetCommand()
			 */
			void SetCommand(const tstring &command);
			/**	@}	<!-- end of the group --> */

			/**
			 *	@name Getter functions
			 *	@{
			 */
			/**
			 *	@brief Get the items parameters.
			 *	@see SetPathFlags()
			 *	@see SetCommand()
			 *	@see GetCommand()
			 *	@return A string containing the items parameters.
			 */
			virtual tstring GetFlags();
			/**
			 *	@brief Get the items command.
			 *	@see SetPathFlags()
			 *	@see GetFlags()
			 *	@see SetCommand()
			 *	@return A string containing the command.
			 */
			tstring GetCommand();
			/**	@}	<!-- end of the group --> */
	 
		protected:
			/// Split the command into a path and parameters.
			bool SeparateFlags();
			
			tstring m_Cmd; ///<	The items command.
		};

		inline Item::Item(Section *section)
			: Boot::Item(section), m_Cmd(_T(""))
		{
		}

		inline Item::~Item()
		{
		}

		inline void Item::SetPath(const tstring &file)
		{
			ShortCut::SetPath(file);
		}

		inline tstring Item::GetPath() const
		{
			return ShortCut::GetPath();
		}

		inline tstring Item::GetExt()
		{
			return ShortCut::GetExt();
		}

		inline tstring Item::GetName()
		{
			return ShortCut::GetName();
		}

		inline tstring Item::GetDir()
		{
			return ShortCut::GetDir();
		}

		inline HICON Item::GetIcon()
		{
			return ShortCut::GetIcon();
		}

		inline HICON Item::GetSmallIcon()
		{
			return ShortCut::GetSmallIcon();
		}

		inline void Item::RenameFile(const tstring &newpath)
		{
			ShortCut::RenameFile(newpath);
		}

		inline void Item::DeleteFile()
		{
			ShortCut::DeleteFile();
		}

		inline bool Item::IsExist()
		{
			return ShortCut::IsExist();
		}

		inline tstring Item::CreateShortcut(const tstring &Dir, const tstring &Name)
		{
			return ShortCut::CreateShortcut(Dir, Name);
		}

	}	// namespace Folder

}	// namespace Boot
 
#endif	// BTFOLDER_H
