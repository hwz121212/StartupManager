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

#ifndef BTREG_H
#define BTREG_H

/// @file BtReg.h Declarations for the registry section classes.

#include "BtSection.h"
#include "BtRegException.h"

namespace Boot
{

	/**
	 *	@namespace Boot::Registry
	 *	@brief Registry start sections stuff.
	 *	@details This group namespace definitions for registry start
	 *	sections and items.
	 */
	namespace Registry
	{

		/**
		 *	@brief A registry start section.
		 *	@details All registry start sections can be represented
		 *	using this class.
		 */
		class Section : public Boot::Section
		{
		public:
			/// Enumeration of possible registry access methods.
			enum KEYBITS
			{
				X64KEY 	  = KEY_WOW64_64KEY,	///<	64-bit registry access.
				X32KEY    = KEY_WOW64_32KEY,	///<	32-bit registry access.
				NORMALKEY = 0					///<	Normal registry access.
			};

			/**
			 *	@name Construction/Destruction
			 /	@{
			 */
			/**
			 *	@brief The constructor.
			 *	@param root A handle to the parent key.
			 *	@param regkey The name of the key to represent.
			 *	@param dwHiveFlag The access method.
			 *	@param section The name of the represented section.
			 */
			Section(HKEY root, const tstring &regkey, KEYBITS dwHiveFlag, const tstring &section);
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
			 *	@brief Get the parent key.
			 *	@return A handle to the parent key.
			 */
			HKEY GetRoot() const;
			/**
			 *	@brief Get the name of the represented registry key.
			 *	@return The name of the represented registry key.
			 */
			tstring GetRegkey() const;
			/**
			 *	@brief Get the used access method.
			 *	@return The used access method.
			 */
			KEYBITS GetHiveFlag() const;
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
			 *	@param item A pointer to the item to add (must be
			 *	of type Boot::Registry::Item!).
			 *	@see Delete()
			 *	@exception Boot::Exception An exception that is
			 *	thrown in case of an error.
			 */
			virtual void Add(Boot::Item *item) throw (Boot::Exception);
			/**
			 *	@brief Delete an item from this section.
			 *	@param item A pointer to the item to delete (must be
			 *	of type Boot::Registry::Item!).
			 *	@see Add()
			 *	@exception Boot::Exception An exception that is
			 *	thrown in case of an error.
			 */
			virtual void Delete(Boot::Item *item) throw (Boot::Exception);
			/**
			 *	@brief Rename an item from this section.
			 *	@param item A pointer to the item to rename (must be
			 *	of type Boot::Registry::Item!).
			 *	@param newName The new name for the item.
			 *	@exception Boot::Exception An exception that is
			 *	thrown in case of an error.
			 */
			virtual void Rename(Boot::Item *item, const tstring &newName) throw (Boot::Exception);
			/**
			 *	@brief Enable an item from this section.
			 *	@param item A pointer to the item to enable (must be
			 *	of type Boot::Registry::Item!).
			 *	@pre The item is disabled.
			 *	@post The item is enabled.
			 *	@see Disable()
			 *	@exception Boot::Exception An exception that is
			 *	thrown in case of an error.
			 */
			virtual void Enable(Boot::Item *& item) throw (Boot::Exception);
			/**
			 *	@brief Disabled an item from this section.
			 *	@param item A pointer to the item to disable (must be
			 *	of type Boot::Registry::Item!).
			 *	@pre The item is enabled.
			 *	@post The item is disabled.
			 *	@see Enable()
			 *	@exception Boot::Exception An exception that is
			 *	thrown in case of an error.
			 */
			virtual void Disable(Boot::Item *& item) throw (Boot::Exception);
			/**	@} <!-- end of the group --> */

			const HKEY m_Root;			///<	A handle to the parent key.
			const tstring m_Regkey;		///<	The name of the represented key.
			const KEYBITS m_dwHiveFlag; ///<	The used access method: 32 or 64 bit key.

		private:
			/**
			 *	@name Construction/Destruction
			 *	@{
			 */
			/**
			 *	@brief Copy constructor.
			 *	@details Private to prevent copy construction.
			 */
			Section(const Section &r);
			/**
			 *	@brief Copy assignment operator.
			 *	@details Private to prevent copy assignment.
			 */
			Section& operator= (const Section &r);
			/**	@} <!-- end of the group --> */

			/// Insert an item that was read.
			void InsertReadItem(LPCTSTR szProgramPath, LPCTSTR szValueName, DWORD type) throw (Boot::Exception);

			/// Get the location of disabled items.
			void GetDisabledLocation() throw (Boot::Exception);
		};

		inline Section::~Section()
		{
		}

		inline HKEY Section::GetRoot() const
		{
			return m_Root;
		}

		inline tstring Section::GetRegkey() const
		{
			return m_Regkey;
		}

		inline Section::KEYBITS Section::GetHiveFlag() const
		{
			return m_dwHiveFlag;
		}

		/**
		 *	@brief A registry start item.
		 *	@details A registry start item can only be used with a
		 *	registry start section.
		 */
		class Item : public Boot::Item
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
			/**	@} <!-- end of the group --> */

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
			/**	@} <!-- end of the group --> */
	 
		protected:
			/// Split the command into a path and parameters.
			bool SeparateFlags();
			
			tstring m_Cmd;	///<	The items command.
		};

		inline Item::Item(Section *section)
			: Boot::Item(section), m_Cmd(_T(""))
		{
		}

		inline Item::~Item()
		{
		}

	}	// namespace Registry

}	// namespace Boot

#endif	// BTREG_H
