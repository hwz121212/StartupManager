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

#ifndef BTSECTION_H
#define BTSECTION_H

/// @file BtSection.h Declarations of the generic start section classes.

#include "GList.h"
#include "File.h"
#include "BtException.h"
#include "RegKey.h"
#include "..\types\tstring.h"

#include <cassert>

/**
 *	@mainpage Startup Manager Boot Core documentation
 *	These pages contain detailed information on the implementation of
 *	the Startup Manager %Boot Core. It contains documentation of the
 *	used namespaces classes and sometimes even on the implementation.
 */

/**
 *	@namespace Boot
 *	@brief Contains all start section and item classes.
 *	@details This namespace contains all classes that are necessary
 *	to work with the start sections as well as a wrapper class that
 *	provides an interface for easy access to the sections and their
 *	items.
 */
namespace Boot
{

	class Section;	// forward declaration of the Section class.

	/**
	 *	@defgroup BtSectionGenerics Generic definitions for the start sections.
	 *	@details This group contains all generic classes that are
	 *	needed by the start sections and their items.
	 *	@{
	 */

	/**
	 *	@brief A generic start item.
	 *	@details All section-specific start items inherit from this
	 *	class.
	 */
	class Item : public GList::GNode, public File
	{
	public:
		///	Enumeration of possible start item attributes.
		enum
		{
			attrErrNotInit = 0L,	///<	item is not yet initialized.
			attrErrCorrupt = 1L,	///<	item has empty command/name.
			attrSystem     = 2L,	///<	item is an important system file.
			attrDisabled   = 4L,	///<	item is currently disabled.
			attrRegular    = 8L,	///<	regular item.
		};

		/**
		 *	@name Construction/Destruction
		 *	@{
		 */
		/**
		 *	@brief The constructur.
		 *	@param section The Section in which the item is listed.
		 *	The parameter cannot be 0.
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
		 *	@brief Set the name of the item.
		 *	@param name The items name.
		 */
		virtual void SetiName(const tstring &name);
		/**
		 *	@brief Set the items attributes.
		 *	@param attr A bit-sequence (DWORD) containing the
		 *	attributes.
		 */
		void SetAttributes(const DWORD attr);
		/**	@} <!-- end of the group --> */

		/**
		 *	@name Getter functions
		 *	@{
		 */
		/**
		 *	@brief Get the name of the item.
		 *	@return The items name.
		 */
		tstring GetiName() const;
		/**
		 *	@brief Get the parameters that are passed in the item.
		 *	@return The parameters.
		 */
		virtual tstring GetFlags() const;
		/**
		 *	@brief Get the items attributes.
		 *	@return The items attributes.
		 */
		DWORD GetAttributes() const;
		/**
		 *	@brief Get the section in which the item is listed.
		 *	@return A reference to the section in which the item
		 *	is listed.
		 */
		Section*& GetSection();
		/**	@} <!-- end of the group --> */

		/**
		 *	@brief Check whether an item is enabled or not.
		 *	@return A boolean indicating if the item is enabled
		 *	(true) or not (false).
		 *	@see IsDisabled
		 */
		bool IsEnabled() const;
		/**
		 *	@brief Check whether an item is disabled or not.
		 *	@return A boolean indicating if the item is disabled
		 *	(true) or not (false).
		 *	@see Enabled
		 */
		bool IsDisabled() const;
       
	protected:
		tstring m_Item;		///<	The items name.
		tstring m_Flags;	///<	The parameters that are passed to the item.
	
	private:
		/**
		 *	@name Construction/Destruction
		 *	@{
		 */
		/**
		 *	@brief Copy constructor.
		 *	@details Private to prevent copy construction.
		 */
		Item(const Item &item);
		/**
		 *	@brief Copy assignment operator.
		 *	@details Private to prevent copy assignment.
		 */
		Item& operator=(const Item &item);
		/**	@} <!-- end of the group --> */

		DWORD m_dwAttr;		///<	The items attributes.
		Section *m_section;	///<	Reference to a pointer to the section of which this item is part.
	};

	inline Item::Item(Section *section)
		: m_Item(_T("")), m_Flags(_T("")), m_dwAttr(attrErrNotInit), m_section(section)
	{
		assert(section != 0);
	}

	inline Item::~Item()
	{
	}

	inline void Item::SetiName(const tstring &name)
	{
		m_Item = name;
	}

	inline tstring Item::GetiName() const
	{
		return m_Item;
	}

	inline tstring Item::GetFlags() const
	{
		return m_Flags;
	}

	inline bool Item::IsEnabled() const
	{
		return !(m_dwAttr & attrDisabled);
	}

	inline bool Item::IsDisabled() const
	{
		return !IsEnabled();
	}

	inline void Item::SetAttributes(const DWORD attr)
	{
		m_dwAttr = attr;
	}

	inline DWORD Item::GetAttributes() const
	{
		return m_dwAttr;
	}

	inline Section*& Item::GetSection()
	{
		return m_section;
	}

	/// A list that contains Item nodes.
	class List : public GList
	{
	public:
		/**
		 *	@name Construction/Destruction
		 *	@{
		 */
		///	The constructor.
		List();
		/// The destructor.
		virtual ~List();
		/**	@} <!-- end of the group --> */

		/**
		 *	@name Operations
		 *	@{
		 */
		/**
		 *	@brief Select a node of the list.
		 *	@param node A pointer to a node containing the same
		 *	information as the one to select.
		 *	@return The current status of the list. Indicates
		 *	success or failure.
		 */
		virtual ListStatus SelNode(GNode *node);
		/**
		 *	@brief Retrieve a pointer to the node that is currently
		 *	selected.
		 *	@return A pionter to the node that is currently selected.
		 */
		Item* GetNode();
		/**	@} <!-- end of the group --> */
	};

	inline List::List()
	{
	}

	inline List::~List()
	{
	}

	inline Item* List::GetNode()
	{
		return dynamic_cast<Item *>(m_pCurrent);
	}

	/**
	 *	@brief A generic start section.
	 *	@details All start section inherit from this abstract class and
	 *	implement its methods.
	 */
	class Section : public List
	{
		friend class Item;

	public:
		/**
		 *	@name Construction/Destruction
		 *	@{
		 */
		/**
		 *	@brief The constructor.
		 *	@param section The name of the section. Its length must be
		 *	greater than zero.
		 */
		explicit Section(const tstring &section);
		/// The destructor.
		virtual ~Section();
		/**	@} <!-- end of the group --> */

		/**
		 *	@name Operations
		 *	@{
		 */
		/**
		 *	@brief Read the items in this section and list them.
		 *	@details Each section must implement this function because
		 *	it is pure virtual.
		 */
		virtual void Read() = 0;
		/**
		 *	@brief Add an item to this section.
		 *	@details Each section must implement this function because
		 *	it is pure virtual.
		 *	@param item A pointer to the item that should be added.
		 *	@exception Exception An exception thrown in case of an error.
		 */
		virtual void Add(Item *item) throw (Exception) = 0;
		/**
		 *	@brief Delete an item from this section.
		 *	@details Each section must implement this function because
		 *	it is pure virtual.
		 *	@param item A pointer to the item that should be added.
		 *	@exception Exception An exception thrown in case of an error.
		 */
		virtual void Delete(Item *item) throw (Exception) = 0;
		/**
		 *	@brief Rename an item in this section.
		 *	@details Each section must implement this function because
		 *	it is pure virtual.
		 *	@param item A pointer to the item that should be renamed.
		 *	@param newName The new name for the item.
		 *	@exception Exception An exception thrown in case of an error.
		 */
		virtual void Rename(Item *item, const tstring &newName) throw (Exception) = 0;
		/**
		 *	@brief Enable an item in this section.
		 *	@details Each section must implement this function because
		 *	it is pure virtual.
		 *	@pre The item is disabled.
		 *	@post The item is enabled.
		 *	@param item A pointer to the item that should be enabled.
		 *	@exception Exception An exception thrown in case of an error.
		 */
		virtual void Enable(Item *& item) throw (Exception) = 0;
		/**
		 *	@brief Disable an item in this section.
		 *	@details Each section must implement this function because
		 *	it is pure virtual.
		 *	@pre The item is enabled.
		 *	@post The item is disabled.
		 *	@param item A pointer to the item that should be disabled.
		 *	@exception Exception An exception thrown in case of an error.
		 */
		virtual void Disable(Item *& item) throw (Exception) = 0;
		/**	@} <!-- end of the group --> */

		/**
		 *	@name Getter functions
		 *	@{
		 */
		/**
		 *	@brief Get the name of this section.
		 *	@details Each section must implement this function because
		 *	it is pure virtual.
		 *	@return The name of this section.
		 */
		tstring GetSection() const;
		/**
		 *	@brief Get the long name of this section (For example the
		 *	registry key in case of a registry section).
		 *	@details Each section must implement this function because
		 *	it is pure virtual.
		 *	@return The long name of this section.
		 */
		virtual tstring GetLongSection() const;
		/**	@} <!-- end of the group --> */

	protected:

		const tstring m_Section;	///<	The sections name.
		tstring m_Diskey;			///<	%Registry key where disabled items are stored.

	private:
		/**
		 *	@name Construction/Destruction
		 *	@{
		 */
		/**
		 *	@brief Copy constructor.
		 *	@details Private to prevent copy construction.
		 */
		Section(const Section &s);
		/**
		 *	@brief Copy assignment operator.
		 *	@details Private to prevent copy assignment.
		 */
		Section& operator=(const Section &s);
		/**	@} <!-- end of the group --> */
	};

	inline Section::Section(const tstring &section) : m_Section(section), m_Diskey(_T(""))
	{
		assert(section.length() > 0);
	}

	inline Section::~Section()
	{
	}

	inline tstring Section::GetSection() const
	{
		return m_Section;
	}

	inline tstring Section::GetLongSection() const
	{
		return m_Section;
	}

	/**	@} <!-- end of the BtSectionGenerics group --> */

} // namespace Boot
 
#endif	// BTSECTION_H
