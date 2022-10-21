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

#ifndef WINTOOLS_H
#define WINTOOLS_H

/// @file WinTools.h Declarations of some helper functions.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "..\types\tstring.h"

/**
 *	@brief Get the windows version
 *	@return A string containing the windows version.
 */
tstring GetWindowsVersion();

/**
 *	@brief Check whether the windows version is 64-bit.
 *	@return A boolean indicating whether the windows version is
 *	64-bit (true) or not (false).
 */
BOOL IsWindows64();

#endif
