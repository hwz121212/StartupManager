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
 
#ifndef ABOUTDLG_H
#define ABOUTDLG_H

#include "..\Win32++\dialog.h"
#include "CImage.h"
#include "types\tstring.h"
#include "ToolTip.h"
#include "HyperLink.h"
#include "HotImage.h"

class AboutDlg : public CDialog
{
public:
    AboutDlg(HWND hWndParent = 0);
    virtual ~AboutDlg() {}
      
private:
	tstring GetLicenseLocation();
    virtual BOOL OnInitDialog();
     
    CImage m_Header;
	CToolTip m_TT;
	CHyperlink m_Home;
	CHyperlink m_License;
	CHyperlink m_Win32xx;
	CHotImage m_Donate;
};

#endif //ABOUTDLG_H
