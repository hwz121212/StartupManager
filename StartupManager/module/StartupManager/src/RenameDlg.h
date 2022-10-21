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
 
#ifndef RENAMEDLG_H
#define RENAMEDLG_H

#include "..\Win32++\dialog.h"
#include "boot\btcore.h"
#include "ToolTip.h"

using namespace Boot;

class RenameDlg : public CDialog
{
public:
    RenameDlg(Boot::Core &BootList, const Boot::WorkItem &item, HWND hWndParent = 0);
    virtual ~RenameDlg() {}
      
private:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
	
    Boot::Core &m_BootList;
    const Boot::WorkItem &m_Old;
	CToolTip m_TT;
};

#endif //RENAMEDLG_H
