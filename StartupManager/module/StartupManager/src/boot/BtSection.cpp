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

/// @file BtSection.cpp Definitions of the generic start section classes.

#include "BtSection.h"

namespace Boot
{

	GList::ListStatus List::SelNode(GNode *node)
	{
		SelFirst();

		Item *btNode = dynamic_cast<Item *>(node);
		Item *btCurrent = dynamic_cast<Item *>(m_pCurrent);

		while ((m_Status == OK) && ((btCurrent->GetiName() != btNode->GetiName())
			|| (btCurrent->GetPath() != btNode->GetPath())))
		{
			SelNext();
			btCurrent = dynamic_cast<Item *>(m_pCurrent);
		}

		if (m_pCurrent && (btCurrent->GetiName() == btNode->GetiName())
			&& (btCurrent->GetPath() == btNode->GetPath()))
			m_Status = OK;
		else if (m_pCurrent)
		{
			m_pCurrent = 0;
			m_Status   = NOSELECT;
		}
		else
		{
			m_Status = NOSELECT;
		}

		return m_Status;
	}

} //namespace Boot
