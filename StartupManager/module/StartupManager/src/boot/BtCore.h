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

#ifndef BTCORE_H
#define BTCORE_H

/// @file BtCore.h Declaration of the start section wrapper classes.

#include "BtSection.h"
#include "BtFolder.h"
#include "BtWinLogon.h"
#include "BtWinini.h"
#include "BtReg.h"
#include "BtBHO.h"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4702) // Unreachable code warnings in xtree/vector
#endif
#include <vector>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

namespace Boot
{

	/**
	 *	@defgroup BtWrapper Boot wrapper classes.
	 *	These classes form a interface the user can use to access
	 *	the start sections.
	 *	@{
	 */

	/**
	 *	@brief Class that represents a start item to work with.
	 *	@details This class is used to prevent the direct usage
	 *	of Item objects.
	 */
	class WorkItem
	{
	public:
		/**
		 *	@name Construction/Destruction
		 *	@{
		 */
		/**
		 *	@brief The constructor.
		 *	@param command The items command.
		 *	@param name The items name.
		 *	@param section The name of the sections in which the
		 *	item is located.
		 */
		WorkItem(const tstring &command, const tstring &name, const tstring &section);
		/**
		 *	@brief Copy constructor.
		 *	@param item The WorkItem to copy.
		 */
		WorkItem (const WorkItem &item);
		/// The destructor.
		~WorkItem();
		/**
		 *	@brief Copy assignment.
		 *	@param item The item to copy.
		 *	@return A reference to this item.
		 */
		WorkItem& operator=(const WorkItem &item);
		/**	@}	<!-- end of the group --> */

		/**
		 *	@name Getter functions
		 *	@{
		 */
		/**
		 *	@brief Get the items command.
		 *	@return The command.
		 */
		tstring GetCommand() const;
		/**
		 *	@brief Get the items name.
		 *	@return The name.
		 */
		tstring GetName() const;
		/**
		 *	@brief Get the name of the section in which the item
		 *	is located.
		 *	@return The name of the section in which the item is
		 *	located.
		 */
		tstring GetSection() const;
		/**	@}	<!-- end of the group --> */

	private:
		tstring m_command;	///<	The items command.
		tstring m_name;		///<	The items name.
		tstring m_section;	///<	The name of the section in which the item is located.
	};

	inline WorkItem::WorkItem(const tstring &command, const tstring &name, const tstring &section)
		: m_command(command), m_name(name), m_section(section)
	{
	}

	inline WorkItem::WorkItem (const WorkItem &item)
		: m_command(item.GetCommand()), m_name(item.GetName()), m_section(item.GetSection())
	{
	}

	inline WorkItem::~WorkItem()
	{
	}

	inline tstring WorkItem::GetCommand() const
	{
		return m_command;
	}

	inline tstring WorkItem::GetName() const
	{
		return m_name;
	}

	inline tstring WorkItem::GetSection() const
	{
		return m_section;
	}

	/**
	 *	@brief A wrapper class for all start sections.
	 *	@details This class provides an interface the user can
	 *	use to access the start sections.
	 */
	class Core
	{
	public:
		/**
		 *	@name Iteration
		 *	@{
		 */
		/**
		 *	@brief A typedef to give the user read-only access
		 *	to the BtSectionContainer
		 */
		typedef std::vector<Section *>::const_iterator const_iterator;
		/**
		 *	@brief A typedef to give the user read-only access
		 *	to the BtSectionContainer
		 */
		typedef std::vector<Section *>::const_reverse_iterator const_reverse_iterator;
		/**	@}	<!-- end of the group --> */

		/**
		 *	@name Construction/Destruction
		 *	@{
		 */
		/**
		 *	@brief The constructor.
		 *	@param bDisplayUnused Should unused sections also
		 *	be included?
		 */
		Core(bool bDisplayUnused) throw (Boot::Exception);
		/// The destructor.
		~Core();
		/**	@}	<!-- end of the group --> */

		/// Read all sections and store the items.
		void Init() throw (Boot::Exception);

		/**
		 *	@name Operations
		 *	@{
		 */
		/**
		 *	@brief Get the total number of start items.
		 *	@return The total number of start items.
		 *	@throw Exception An exception thrown in case of an error.
		 */
		int GetItemCount() const throw (Boot::Exception);
		/**
		 *	@brief Get the number of items in a certain
		 *	sections.
		 *	@param section The name of the section.
		 *	@return The number of items in this section.
		 *	@throw Exception An exception thrown in case of an error.
		 */
		int GetItemCount(const tstring &section) const throw (Boot::Exception);
		/**
		 *	@brief Get the long name of a certain section.
		 *	@param section The name of the section.
		 *	@return The long name of this section.
		 *	@throw Exception An exception thrown in case of an error.
		 */
		tstring GetLongSection(const tstring &section) const throw (Boot::Exception);
		/**
		 *	@brief Check if an item exists.
		 *	@param path The path of the items to check whether
		 *	such an item exists.
		 *	@return A boolean indicating whether an items
		 *	exists (true) or not (false).
		 *	@throw Exception An exception thrown in case of an error.
		 */
		bool Find(const tstring &path) const throw (Boot::Exception);
		/**
		 *	@brief Get a start item.
		 *	@param item The WorkItem that represents the item
		 *	to retrieve.
		 *	@return A pointer to the start item.
		 *	@throw Exception An exception thrown in case of an error.
		 */
		Item* Get(const WorkItem &item) const throw(Boot::Exception);
		/**	@}	<!-- end of the group --> */

		/**
		 *	@name Start section and item manipulation.
		 *	@{
		 *	@brief Manipulate the start sections and their
		 *	items.
		 *	@details These functions provide an interface to
		 *	easily manipulate the start sections and their
		 *	items.
		 */
		/**
		 *	@brief Add an item.
		 *	@param item The WorkItem that contains the necessary
		 *	information.
		 *	@see Delete()
		 *	@throw Exception An exception thrown in case of an error.
		 */
		void Add(const WorkItem &item) throw (Boot::Exception);
		/**
		 *	@brief Delete an item.
		 *	@param item The WorkItem that represents the item to
		 *	delete.
		 *	@see Add()
		 *	@throw Exception An exception thrown in case of an error.
		 */
		void Delete(const WorkItem &item) throw (Boot::Exception);
		/**
		 *	@brief Rename an item.
		 *	@param item The WorkItem that represents the item to
		 *	rename.
		 *	@param newName The new name for the item.
		 *	@throw Exception An exception thrown in case of an error.
		 */
		void Rename(const WorkItem &item, const tstring &newName) throw (Boot::Exception);
		/**
		 *	@brief Enable an item.
		 *	@param item The WorkItem that represents the item to
		 *	rename.
		 *	@see Disable()
		 *	@throw Exception An exception thrown in case of an error.
		 */
		void Enable(const WorkItem &item) throw (Boot::Exception);
		/**
		 *	@brief Disable an item.
		 *	@param item The WorkItem that represents the item to
		 *	rename.
		 *	@see Enable()
		 *	@throw Exception An exception thrown in case of an error.
		 */
		void Disable(const WorkItem &item) throw (Boot::Exception);
		/**	@} <!-- end of the group --> */

		/**
		 *	@name Section access functions.
		 *	@{
		 *	@brief Functions that provide access to the start
		 *	sections.
		 *	@details These functions provide read access to the
		 *	start sections by returning a constant iterator.
		 */
		/**
		 *	@brief Get the begin() iterator of the sections.
		 *	@see end()
		 *	@return A constant iterator referring to the first
		 *	section.
		 */
		const_iterator begin() const;
		/**
		 *	@brief Get the end() iterator of the sections.
		 *	@see begin()
		 *	@return A constant iterator referring past the last
		 *	section.
		 */
		const_iterator end() const;
		/**
		 *	@brief Get the rbegin() iterator of the sections.
		 *	@see rend()
		 *	@return A constant reverse iterator referring to the
		 *	last section.
		 */
		const_reverse_iterator rbegin() const;
		/**
		 *	@brief Get the rend() iterator of the sections.
		 *	@see rbegin()
		 *	@return A constant reverse iterator referring before
		 *	the first section.
		 */
		const_reverse_iterator rend() const;
		/**	@} <!-- end of the group --> */

	private:
		/**
		 *	@name Iteration
		 *	@{
		 */
		/**
		 *	@brief A typedef to give the user read and write access
		 *	to the BtSectionContainer
		 */
		typedef std::vector<Section *>::iterator iterator;
		/**	@} <!-- end of the group --> */

		/// Convert a WorkItem to a Item (needed for Add).
		Item *Convert(const WorkItem &item, Section *section) throw(Boot::Exception);

		/**
		 *	@brief A section container class.
		 *	@details This class contains pointers to the start
		 *	sections and is only available to the Boot::Core class.
		 */
		class SectionContainer : public std::vector<Section *>	// std::map<std::string, Section *>
		{
		public:
			/**
			 *	@name Construction/Destruction
			 *	@{
			 */
			/**
			 *	@brief The constructor.
			 *	@details Private so that only Boot::Core has access
			 *	to it.
			 *	@param bDisplayUnused Boolean indicating whether
			 *	unused sections should belisted or not.
			 */
			SectionContainer(bool bDisplayUnused);
			/// The destructor.
			~SectionContainer();
			/**	@}	<!-- end of the group --> */

			/// Initialize the pointer list.
			void Init() throw (...);

			/// Add the win.ini sections.
			void AddWininiSections();

			/// Check whether a registry section should be added or not.
			BOOL ShouldAdd(Registry::Section *registry);

			/// Add a registry section.
			void AddRegSection(int i, HKEY root, const tstring &regkey, const tstring &section);

			/// Add the registry sections.
			void AddRegistrySections();

			/// Add the start folder sections.
			void AddFolderSections();

			/// Get the location of disabled items of some section.
			tstring GetDisabledLocation(LPCTSTR section) throw(Boot::Exception);

			/// Read disabled registry items.
			void ReadDisabledRegistry();

			/// Read disabled start folder items.
			void ReadDisabledFolder();

			/// Read disabled win.ini items.
			void ReadDisabledWinini();

			/// Read disabled winlogon items.
			void ReadDisabledWinLogon();

			bool m_bDisplayUnused;	///<	List unused sections?
		} m_StartSections;	///<	The start section container.
	};

	inline Core::~Core()
	{
	}

	inline Core::const_iterator Core::begin() const
	{
		return m_StartSections.begin();
	}

	inline Core::const_iterator Core::end() const
	{
		return m_StartSections.end();
	}

	inline Core::const_reverse_iterator Core::rbegin() const
	{
		return m_StartSections.rbegin();
	}

	inline Core::const_reverse_iterator Core::rend() const
	{
		return m_StartSections.rend();
	}

	inline Core::SectionContainer::SectionContainer(bool bDisplayUnused)
		: m_bDisplayUnused(bDisplayUnused)
	{
		Init();
	}

	/** @} <!-- end of the BtWrapper group --> */

	/**
	 *	@brief A logging class.
	 *	@details This class is used to log the actions that were
	 *	performed on the start sections.
	 */
	class Log
	{
	public:
		/**
		 *	@name Construction/Destruction
		 *	@{
		 */
		/// The destructor.
		~Log();
		/**	@}	<!-- end of the group --> */

		/**
		 *	@brief Retrieve the logging object.
		 *	@return The logging object.
		 */
		static Log* GetLog(); 

		/// An enumeration containing the possible actions.
		enum Action { NONE = -1, ADDED = 0, DELETED = 1, RENAMED = 2,
			MODIFIED = 3, BTENABLED = 4, BTDISABLED = 5 };

		///	A class containing logging information.
		class LogInfo
		{ 
		public:
			/**
			 *	@name Construction/Destruction
			 *	@{
			 */
			///	The constructor.
			LogInfo();
			/**	@}	<!-- end of the group --> */
		
			tstring item;		///<	The item on which the action was performed.
			tstring section;	///<	The section in which the item is located.
			Action action;		///<	The action that was performed.
		};
		
		/**
		 *	@brief %Log an action.
		 *	@param info The logging information.
		 */
		void DoLog(LogInfo info) throw (Boot::Exception);

	private:
		tstring path;	///<	The path of the log file.

		/**
		 *	@name Construction/Destruction
		 *	@{
		 */
		/// Prevent normal construction.
		Log();
		/// Prevent copy construction.
		Log(const Log&);
		/// Prevent copy assignment.
		Log& operator= (const Log&);
		/**	@}	<!-- end of the group --> */

		/// Retrieve the current users name.
		tstring GetUserName();

		static Log* st_pLog;	///<	The logging object.
	};

	inline Log::LogInfo::LogInfo()
		: item(_T("")), section(_T("")), action(Log::NONE)
	{
	}

	/**
	 *	@brief A global logging function.
	 *	@details This function retrieves the logging object and
	 *	logs the information that is passed to this function.
	 *	@param info The logging information.
	 */
	inline static void LOG(Log::LogInfo info)
	{
		Log::GetLog()->DoLog(info);
	}

} //namespace Boot
 
#endif
