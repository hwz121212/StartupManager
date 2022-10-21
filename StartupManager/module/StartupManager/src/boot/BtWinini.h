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

#ifndef BTWININI_H
#define BTWININI_H

/// @file BtWinini.h Declarations of the win.ini section classes.

#include "BtSection.h"
#include "BtWininiException.h"

namespace Boot
{
	/**
	 *	@namespace Boot::Winini
	 *	@brief Win.ini stuff.
	 *	@details This namespace contains definitions for win.ini
	 *	sections and items.
	 */
	namespace Winini
	{

		/**
		 *	@brief A win.ini section.
		 *	@details All win.ini sections can be represented using this
		 *	class.
		 */
		class Section : public Boot::Section
		{
		public:
			/**
			 *	@name Construction/Destruction
			 *	@{
			 */
			/**
			 *	@brief The constructor.
			 *	@param part Indicates which win.ini section is
			 *	represented (must be "load" or "run"!).
			 *	@param section The name of the represented section.
			 */
			Section(const tstring &part, const tstring &section);
			/// The destructor.
			virtual ~Section();
			/**	@} <!-- end of the group --> */

			/// Check whether this section was initialized properly.
			bool ProperlyInitialized();

			/**
			 *	@name Getter functions
			 *	{
			 */
			/**
			 *	@brief Get the long section name.
			 *	@return The long section name.
			 */
			virtual tstring GetLongSection() const;
			/**
			 *	@brief Get the part that indicates which win.ini section is
			 *	represented.
			 *	@return The part.
			 */
			tstring GetPart() const;
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
			 *	@param item A pointer to the item to add (must be of type
			 *	Boot::Winini::Item!).
			 *	@see Delete()
			 *	@exception Boot::Exception An exception that is thrown in
			 *	case of an error.
			 */
			virtual void Add(Boot::Item *item) throw (Boot::Exception);
			/**
			 *	@brief Delete an item from this section.
			 *	@param item A pointer to the item to delete (must be of type
			 *	Boot::Winini::Item!).
			 *	@see Add()
			 *	@exception Boot::Exception An exception that is thrown in
			 *	case of an error.
			 */
			virtual void Delete(Boot::Item *item) throw (Boot::Exception);
			/**
			 *	@brief Rename an item from this section.
			 *	@param item A pointer to the item to rename (must be of type
			 *	Boot::Winini::Item!).
			 *	@param newName The new name for the item.
			 *	@exception Boot::Exception An exception that is thrown in case
			 *	of an error.
			 */
			virtual void Rename(Boot::Item *item, const tstring &newName) throw (Boot::Exception);
			/**
			 *	@brief Enable an item from this section.
			 *	@param item A pointer to the item to enable (must be of type
			 *	Boot::Winini::Item!).
			 *	@pre The item is disabled.
			 *	@post The item is enabled.
			 *	@see Disable()
			 *	@exception Boot::Exception An exception that is thrown in case
			 *	of an error.
			 */
			virtual void Enable(Boot::Item *& item) throw (Boot::Exception);
			/**
			 *	@brief Disabled an item from this section.
			 *	@param item A pointer to the item to disable (must be of type
			 *	Boot::Winini::Item!).
			 *	@pre The item is enabled.
			 *	@post The item is disabled.
			 *	@see Enable()
			 *	@exception Boot::Exception An exception that is thrown in case
			 *	of an error.
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
			Section(const Section &w);
			/**
			 *	@brief Copy assignment operator.
			 *	@details Private to prevent copy assignment.
			 */
			Section& operator= (const Section &w);
			/**	@} <!-- end of the group --> */

			/// Insert an item that was read.
			void InsertReadItem(const tstring &path) throw (Boot::Exception);

			const tstring m_Part; ///<	The win.ini part: "load" or "run"
		};

		inline Section::~Section()
		{
		}

		inline tstring Section::GetPart() const
		{
			return m_Part;
		}

		/**
		 *	@brief A win.ini item.
		 *	@details A win.ini item can only be used with a win.ini section.
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
			 *	@param section A pointer to the section in which the item is
			 *	located.
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
			 *	@brief Set the path of this item.
			 *	@param file The path.
			 */
			virtual void SetPath(const tstring &file);
			/**	@} <!-- end of the group --> */
		};

		inline Item::Item(Section *section)
			: Boot::Item(section)
		{
		}

		inline Item::~Item()
		{
		}

	}	// namespace Winini
 
}	// namespace Boot
 
#endif	// BTWININI_H
