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

#ifndef CTITLETIP_H
#define CTITLETIP_H

#include "ToolTip.h"

class CTitleTip : public CToolTip
{
public:
	CTitleTip() {}
	virtual ~CTitleTip() {}
	virtual void PreCreate(CREATESTRUCT &cs);
	void TrackActivate(BOOL bActivate = TRUE);
	void TrackPosition(POINT pPos);
	void UpdateRect(LPRECT rcNew);
	void SetMaxTipWidth(int iWidth = -1);
	void Hide();

protected:
	virtual void OnInitialUpdate();
    virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	TOOLINFO m_ToolTipStruct;
};

#endif //CTITLETIP_H
