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

#ifndef BTBHO_H
#define BTBHO_H

/// @file BtBHO.h Declarations for the Browser Helper Objects section classes.

#include "BtReg.h"
#include "BtBHOException.h"

namespace Boot
{

	/**
	 *	@namespace Boot::BHO
	 *	@brief Browser Helper Objects sections stuff.
	 *	@details This namespace contains definitions for Browser Helper
	 *	Objects sections and items.
	 */
	namespace BHO
	{

		/**
		 *	@brief A Browser Helper Object section.
		 *	@details All Browser Helper Objects sections can be
		 *	represented using this class.
		 */
		class Section : public Registry::Section
		{
		public:
			/**
			 *	@name Construction/Destruction
			 *	@{
			 */
			/**
			 *	@brief The constructor.
			 *	@param dwHiveFlag The desired access method.
			 *	@param section The name of the represented section.
			 */
			Section(KEYBITS dwHiveFlag, const tstring &section);
			/// The destructor.
			virtual ~Section();
			/**	@}	<!-- end of the group --> */

		protected:
			/**
			 *	@brief Check whether a read item is disabled.
			 *	@param szKeyName The item to check.
			 *	@return A boolean indicating whether the item is
			 *	disabled (true) or not (false).
			 */
			virtual BOOL IsReadItemDisabled(LPCTSTR szKeyName);

			/**
			 *	@brief Look up a CLSID and add it.
			 *	@param szCLSID The CLSID to look up.
			 */
			virtual void LookUpCLSID(LPCTSTR szCLSID);

			/**
			 *	@name Operations
			 *	@{
			 */
			/// Read and list this sections items.
			virtual void Read();
			/**
			 *	@brief Add an item to this section.
			 *	@param item A pointer to the item to add (must be of
			 *	type Boot::BHO::Item!).
			 *	@see Delete()
			 *	@exception Boot::Exception An exception that is thrown
			 *	in case of an error.
			 */
			virtual void Add(Boot::Item *item) throw (Boot::Exception);
			/**
			 *	@brief Delete an item from this section.
			 *	@param item A pointer to the item to delete (must be
			 *	of type Boot::BHO::Item!).
			 *	@see Add()
			 *	@exception Boot::Exception An exception that is thrown
			 *	in case of an error.
			 */
			virtual void Delete(Boot::Item *item) throw (Boot::Exception);
			/**
			 *	@brief Rename an item from this section.
			 *	@param item A pointer to the item to rename (must be
			 *	of type Boot::BHO::Item!).
			 *	@param newName The new name for the item.
			 *	@exception Boot::Exception An exception that is
			 *	thrown in case of an error.
			 */
			virtual void Rename(Boot::Item *item, const tstring &newName) throw (Boot::Exception);
			/**
			 *	@brief Enable an item from this section.
			 *	@param item A pointer to the item to enable (must be
			 *	of type Boot::BHO::Item!).
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
			 *	of type Boot::BHO::Item!).
			 *	@pre The item is enabled.
			 *	@post The item is disabled.
			 *	@see Enable()
			 *	@exception Boot::Exception An exception that is
			 *	thrown in case of an error.
			 */
			virtual void Disable(Boot::Item *& item) throw (Boot::Exception);
			/**	@}	<!-- end of the group --> */

		private:
			/**
			 *	@name Construction/Destruction
			 *	@{
			 */
			/**
			 *	@brief Copy constructor.
			 *	@details Private to prevent copy construction.
			 */
			Section(const Section &bho);
			/**
			 *	@brief Copy assignment operator.
			 *	@details Private to prevent copy assignment.
			 */
			Section& operator= (const Section &bho);
			/**	@}	<!-- end of the group --> */

			/// Insert an item that was read.
			void InsertReadItem(LPCTSTR szCLSID, LPCTSTR szProgramPath, LPCTSTR szValueName) throw(Boot::Exception);
		};

		inline Section::Section(KEYBITS dwHiveFlag, const tstring &section)
			: Registry::Section(HKEY_LOCAL_MACHINE,
			_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Browser Helper Objects"),
			dwHiveFlag, section)
		{
		}

		inline Section::~Section()
		{
		}

		/**
		 *	@brief A Browser Helper Object.
		 *	@details A Browser Helper Object can only be used with a
		 *	BHO section.
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
			/**	@}	<!-- end of the group --> */

			/**
			 *	@name Setter functions
			 *	@{
			 */
			/**
			 *	@brief Set the items CLSID.
			 *	@param CLSID The CLSID.
			 *	@see GetCLSID()
			 */
			void SetCLSID(const tstring &CLSID);
			/**	@}	<!-- end of the group --> */

			/**
			 *	@name Getter functions
			 *	@{
			 */
			/**
			 *	@brief Get the items CLSID.
			 *	@see SetCLSID()
			 *	@return A string containing the items CLSID.
			 */
			tstring GetCLSID();
			/**	@}	<!-- end of the group --> */

		private:
			tstring m_CLSID;	///<	The items CLSID.
		};

		inline Item::Item(Section *section)
			: Boot::Item(section), m_CLSID(_T(""))
		{
		}

		inline void Item::SetCLSID(const tstring &CLSID)
		{
			m_CLSID = CLSID;
		}

		inline Item::~Item()
		{
		}

		inline tstring Item::GetCLSID()
		{
			return m_CLSID;
		}

	}	// namespace BHO

}	// namespace Boot

#endif	// BTBHO_H
