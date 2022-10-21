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

#ifndef HOTIMAGE_H
#define HOTIMAGE_H

#include "CImage.h"
#include "types\tstring.h"

#ifndef IDC_HAND
#define IDC_HAND  MAKEINTRESOURCE(32649)
#endif

class CHotImage : private CImage
{
public:
	CHotImage();
	virtual ~CHotImage();
	void Create(HWND hWndParent, int ImageID, HINSTANCE hInst);
	void SetTarget(const tstring &target);
	void OnLButtonDown();
	void OnLButtonUp(LPARAM lParam);

protected:
	virtual void OpenUrl();
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	BOOL	m_bUrlVisited;
	BOOL	m_bClicked;
	HCURSOR m_hCursor;
	tstring m_Target;
};

#endif //HOTIMAGE_H
