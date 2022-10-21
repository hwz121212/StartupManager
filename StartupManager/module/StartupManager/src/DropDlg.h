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
 
#ifndef DROPDLG_H
#define DROPDLG_H

#include "..\Win32++\dialog.h"
#include "boot\btcore.h"
#include "boot\ShortCut.h"
#include "ToolTip.h"

using namespace Boot;

class DropDlg : public CDialog
{
public:
    DropDlg(Boot::Core &BootList, TCHAR *newfile, HWND hWndParent = 0);
    virtual ~DropDlg();
      
private:
    ShortCut *m_File;
    Boot::Core &m_BootList;
    bool m_bExp;
    int m_nWidth, m_nHeight, m_nTopl, m_nTopc;
	CToolTip m_TT;
          
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual BOOL DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void Resize();
};

#endif //DROPDLG_H
