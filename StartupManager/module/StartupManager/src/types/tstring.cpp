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

#include "tstring.h"

#include <windows.h>
#include <cassert>

void trim(tstring &str)
{
	TCHAR space = _T(' ');
	std::string::size_type pos1 = str.find_first_not_of(space);
	std::string::size_type pos2 = str.find_last_not_of(space);
	
	if (pos1 == tstring::npos)
		pos1 = 0;
	
	if (pos2 == tstring::npos)
		pos2 = str.length() - 1;
	else
		pos2 = pos2 - pos1 + 1;
	
	str = str.substr(pos1, pos2);
}

std::wstring toWideString(const char* pStr, int len/*=-1*/)
{
	assert((len >= 0) || (len == -1));

	//figure out how many wide characters we are going to get 
	int nChars = MultiByteToWideChar(CP_ACP, 0, pStr, len, 0, 0); 
	if (len == -1)
		--nChars; 
	if (nChars == 0)
		return L"";

	//convert the narrow string to a wide string
	wchar_t *wchars = new wchar_t[nChars];
	MultiByteToWideChar(CP_ACP, 0, pStr, len, wchars, nChars);
	std::wstring buf(wchars, nChars);
	delete [] wchars;

	return buf;
}

std::string toNarrowString(const wchar_t* pStr, int len/*=-1*/)
{ 
	assert((len >= 0) || (len == -1));

	//figure out how many narrow characters we are going to get 
	int nChars = WideCharToMultiByte(CP_ACP, 0, pStr, len, 0, 0, 0, 0); 
	if (len == -1)
		--nChars; 
	if (nChars == 0)
		return "" ;

	//convert the wide string to a narrow string
	char *chars = new char[nChars];
	WideCharToMultiByte(CP_ACP, 0, pStr, len, chars, nChars, 0, 0); 
	std::string buf(chars, nChars);
	delete [] chars;

	return buf;
}
