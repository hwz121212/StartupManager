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

#include <tchar.h>      //Support for _tWinMain
#include <new.h>
#include "FrameApp.h"




int NewHandler(size_t /*size*/)
{
	throw std::bad_alloc();
}

int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int /*iCmdShow*/)
{
	try
	{
		_set_new_handler(NewHandler);

		// The one and only CStartupMgrApp object
	    CStartupMgrApp Stm;

	    //Run the application
	    return Stm.Run();
	}

	catch (std::bad_alloc & /*ba*/)
	{
		MessageBox(0, _T("Error: Out of memory."), _T("Startup Manager"), MB_OK | MB_ICONERROR);
	}

	catch (...)
	{
	#ifdef DEBUG
		MessageBox(0, _T("_tWinMain :: Unknown Exception..."), _T("Startup Manager"), MB_OK | MB_ICONWARNING);
	#endif
	}

	return 1;
}
