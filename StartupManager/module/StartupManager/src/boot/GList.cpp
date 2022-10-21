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

///	@file GList.cpp Definition of a generic list.

#include "GList.h"

#include <cassert>

GList::~GList()
{
	SelFirst();
	GNode *tmp = GetNode();
	while (tmp)
	{
		Delete();
		SelFirst();
		tmp = GetNode();
	}

	m_pFirst   = 0;
	m_pCurrent = 0;
}

GList::ListStatus GList::SelFirst()
{
	m_pCurrent = m_pFirst;
	if (m_pFirst)
		m_Status = OK;
	else
		m_Status = EMPTY;

	return m_Status;
}
 
GList::ListStatus GList::SelLast()
{
	m_pCurrent = m_pFirst;
	if (m_pFirst)
	{
		while (m_pCurrent->m_pNext)
			m_pCurrent = m_pCurrent->m_pNext;
		m_Status = OK;
	}
	else
		m_Status = EMPTY;

	return m_Status;
}
 
GList::ListStatus GList::SelNode(GNode *node)
{
	m_pCurrent = m_pFirst;
	while (m_pCurrent && (m_pCurrent != node))
		m_pCurrent = m_pCurrent->m_pNext;

	if (m_pCurrent && (m_pCurrent == node))
		m_Status = OK;
	else if (m_pCurrent)
	{
		m_pCurrent = 0;
		m_Status = NOSELECT;
	}
	else
		m_Status = NOSELECT;

	return m_Status;
}
 
GList::ListStatus GList::SelNext()
{
	if (m_pCurrent)
	{
		if (m_pCurrent->m_pNext)
		{
			m_pCurrent = m_pCurrent->m_pNext;
			m_Status = OK;
		}
		else
			m_Status = NONEXT;
	}
	else
		m_Status = NOSELECT;

	return m_Status;
}

GList::ListStatus GList::SelPrev()
{
	if (m_pCurrent)
	{
		if (m_pCurrent == m_pFirst)
			m_Status = NOPREV;
		else
		{
			m_pCurrent = m_pCurrent->m_pPrev;
			m_Status = OK;
		}
	}
	else
		m_Status = NOSELECT;

	return m_Status;
}
 
GList::ListStatus GList::InsertBefore(GNode *node)
{
	if (!node)
	{
		m_Status = NEWISNULL;
	}
	else if (!m_pFirst)
	{
		node->m_pNext = 0;
		node->m_pPrev = 0;
		m_pFirst = node;
	    m_pCurrent = m_pFirst;
	    m_Status = OK;
		m_nItems++;
	}
	else if (!m_pCurrent)
	{	
		m_Status = NOSELECT;
	}
	else
	{
		SelPrev();
	    if (m_Status == NOPREV)
   		{
   			node->m_pNext = m_pFirst;
   			node->m_pPrev = 0;
   			m_pCurrent->m_pPrev = node;
   			m_pFirst = node;
   			m_pCurrent = m_pFirst;
		}
		else
		{
		    node->m_pNext = m_pCurrent->m_pNext;
		    node->m_pPrev = m_pCurrent;
		    m_pCurrent->m_pNext->m_pPrev = node;
		    m_pCurrent->m_pNext = node;
		    m_pCurrent = m_pCurrent->m_pNext;
	    }
		m_Status = OK;
		m_nItems++;
	}

	assert(GetNode() == node);

	return m_Status;
}
 
GList::ListStatus GList::InsertAfter(GNode *node)
{
	if (!node)
	{
		m_Status = NEWISNULL;
	}
	else if (!m_pFirst)
	{
		node->m_pNext = 0;
		node->m_pPrev = 0;
		m_pFirst = node;
		m_pCurrent = m_pFirst;
		m_Status = OK;
		m_nItems++;
	}
	else if (!m_pCurrent)
	{
		m_Status = NOSELECT;
	}
	else
	{
		node->m_pNext = m_pCurrent->m_pNext;
		node->m_pPrev = m_pCurrent;
		if (m_pCurrent->m_pNext)
			m_pCurrent->m_pNext->m_pPrev = node;
		m_pCurrent->m_pNext = node;
		m_pCurrent = m_pCurrent->m_pNext;
		m_Status = OK;
		m_nItems++;
	}
	
	assert(GetNode() == node);

	return m_Status;
}
 
GList::ListStatus GList::Delete()
{	
	GNode *pt;
		
	if (m_pCurrent == m_pFirst)
	{
		if (m_pFirst->m_pNext)
		{
			pt = m_pCurrent;
			SelNext();
			m_pFirst = m_pCurrent;
			m_Status = OK;
			if (pt) delete pt;
		}
		else
		{
			pt = m_pCurrent;
			m_pFirst = 0;
			m_pCurrent = 0;
			m_Status = EMPTY;
			if (pt) delete pt;
		}
		m_nItems--;
	}
	else if (m_pCurrent)
	{
		pt = m_pCurrent;
		SelPrev();
		if (m_pCurrent->m_pNext->m_pNext)
			m_pCurrent->m_pNext->m_pNext->m_pPrev = m_pCurrent;
		m_pCurrent->m_pNext = m_pCurrent->m_pNext->m_pNext;
		m_Status = OK;
		if (pt) delete pt;
		m_nItems--;
	}
	else
	{
		m_Status = NOSELECT;
	}
	 
	return m_Status;
}
 
void GList::Enumerate(void proc(GNode *Node))
{
	for (GNode *pNode = m_pFirst; pNode != 0; pNode = pNode->m_pNext)
		proc(pNode);
}
