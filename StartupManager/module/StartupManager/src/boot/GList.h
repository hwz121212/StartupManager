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

#ifndef GLIST_H
#define GLIST_H

/// @file GList.h Declaration of a generic list.

/**
 *	@brief A generic list.
 *	@details GList is a doubly-linked generic list.
 */
class GList
{
public:
	/**
	 *	@brief A generic node.
	 *	@details GNode is the generic node used by GList.
	 */
    class GNode
    {
		friend class GList;
	
    public:
		/**
		 *	@name Construction/Destruction
		 *	@{
		 */
		/**
		 *	@brief The constructor.
		 *	@details This constructor sets the next and
		 *	prev pointers to 0.
		 */
		GNode();
		/**
		 *	@brief The destructor.
		 *	@details This destructor sets the next and
		 *	prev pointers to 0.
		 */
        virtual ~GNode();
		/**	@}	<!-- end of the group --> */
		
    private:
        GNode *m_pNext;	///<	Pointer to next item.
        GNode *m_pPrev;	///<	Pointer to previous item.
    };

	/**
	 *	@brief An enumeration of possible list status.
	 *	@details Possible list status are OK, EMPTY,
	 *	NOSELECT, NONEXT, NOPREV, NEWISNULL and FAILED.
	 */
	enum ListStatus
	{
		OK	      = 0,	///<	list is ok
		EMPTY     = 1,	///<	list is empty
		NOSELECT  = 2,	///<	there is no item selected
		NONEXT    = 3,	///<	the next item does not exist
		NOPREV    = 4,	///<	the previous item does not exist
		NEWISNULL = 5,	///<	tried to add an empty item
		FAILED    = 6	///<	action failed
	};

	/**
	 *	@name Construction/Destruction
	 *	@{
	 */
	///	The constructor.
	GList();
	/**
	 *	@brief The destructor.
	 *	@details Frees all items that are currently in
	 *	the list.
	 */
	virtual ~GList();
	/**	@}	<!-- end of the group --> */

	/**
	 *	@name Operations
	 *	@{
	 */
	/**
	 *	@brief Select the first node of the list.
	 *	@see SelLast()
	 *	@see SelNode()
	 *	@return The current status of the list.
	 *	Indicates success or failure.
	 */
	ListStatus SelFirst();
	/**
	 *	@brief Select the last node of the list.
	 *	@see SelFirst()
	 *	@see SelNode()
	 *	@return The current status of the list.
	 *	Indicates success or failure.
	 */
	ListStatus SelLast();
	/**
	 *	@brief Select a node of the list.
	 *	@param node A pointer to a node containing the
	 *	same information as the one to select.
	 *	@see SelFirst()
	 *	@see SelLast()
	 *	@return The current status of the list.
	 *	Indicates success or failure.
	 */
	virtual ListStatus SelNode(GNode *node);
	/**
	 *	@brief Select the next node in the list.
	 *	@see SelPrev()
	 *	@return The current status of the list.
	 *	Indicates success or failure.
	 */
	ListStatus SelNext();
	/**
	 *	@brief Select the previous node in the list.
	 *	@see SelNext()
	 *	@return The current status of the list.
	 *	Indicates success or failure.
	 */
	ListStatus SelPrev();
	/**
	 *	@brief Insert a node before the node that is
	 *	currently selected.
	 *	@param node A pointer to the node to insert.
	 *	@see InsertAfter()
	 *	@return The current status of the list.
	 *	Indicates success or failure.
	 */
	ListStatus InsertBefore(GNode *node);
	/**
	 *	@brief Insert a node after the node that is
	 *	currently selected.
	 *	@param node A pointer to the node to insert.
	 *	@see InsertBefore()
	 *	@return The current status of the list.
	 *	Indicates success or failure.
	 */
	ListStatus InsertAfter(GNode *node);
	/**
	 *	@brief Remove the node that is currently selected
	 *	from the list.
	 *	@return The current status of the list. Indicates
	 *	success or failure.
	 */
	ListStatus Delete();
	/**
	 *	@brief Retrieve a pointer to the node that is
	 *	currently selected.
	 *	@return A pionter to the node that is currently
	 *	selected.
	 */
	GNode* GetNode();
	/**
	 *	@brief Apply a function to each node.
	 *	@param proc The function that should be done for
	 *	each node.
	 */
	void Enumerate(void proc(GNode *node));
	/**	@}	<!-- end of the group --> */

	/**
	 *	@brief Get the number of nodes in the list.
	 *	@return The number of nodes in the list.
	 */
	int Count() const;

	/**
	 *	@brief Get the current status of the list.
	 *	@return The current status of the list.
	 */
	ListStatus Status() const;
  
protected:
	GNode *m_pFirst;		///<	A pointer to the first node.
	GNode *m_pCurrent;		///<	A pointer to the currently selected node.
	int m_nItems;			///<	The number of nodes.
	ListStatus m_Status;	///<	The current status of the list.
};

inline GList::GNode::GNode()
	: m_pNext(0), m_pPrev(0)
{
}

inline GList::GNode::~GNode()
{
	m_pPrev = 0;
	m_pNext = 0;
}

inline GList::GList()
	: m_pFirst(0), m_pCurrent(0), m_nItems(0), m_Status(EMPTY)
{
}

inline GList::GNode* GList::GetNode()
{
	return m_pCurrent;
}

inline int GList::Count() const
{
	return m_nItems;
}

inline GList::ListStatus GList::Status() const
{
	return m_Status;
}

#endif	// GLIST_H
