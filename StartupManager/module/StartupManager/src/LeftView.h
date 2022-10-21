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

#ifndef LEFTVIEW_H
#define LEFTVIEW_H

#include "..\Win32++\GDI.h"
#include "TreeCtrl.h"
#include "boot\btcore.h"

using namespace Boot;

class CLeftView : public CTreeCtrl
{
public:
	CLeftView();
	virtual ~CLeftView();
	virtual void OnInitialUpdate();
	void SetCoreObj(Boot::Core *List) {m_BootList = List;}
	tstring GetSection();
	virtual LRESULT OnNotifyReflect(WPARAM wParam, LPARAM lParam);

protected:
	virtual LRESULT OnNcCalcSize(WPARAM wParam, LPARAM lParam);
	virtual void DrawTitleBar(CDC& cDC);
	virtual LRESULT OnNcPaint(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	RECT m_ncRect;

private:
	Boot::Core *m_BootList;
};

#endif // LEFTVIEW_H
