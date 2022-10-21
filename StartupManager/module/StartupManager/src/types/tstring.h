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

//UNICODE STL string definitions and some conversion functions.

#ifndef TSTRING_H
#define TSTRING_H

#include <string>
#include <tchar.h>

typedef std::basic_string<TCHAR> tstring;

void trim(tstring &str);

std::wstring toWideString(const char* pStr, int len=-1); 
inline std::wstring toWideString(const std::string& str)
{
	return toWideString(str.c_str(), static_cast<int>(str.length()));
}
inline std::wstring toWideString(const wchar_t* pStr, int len=-1)
{
	return ((len < 0) ? pStr : std::wstring(pStr, len));
}
inline std::wstring toWideString(const std::wstring& str)
{
	return str;
}
std::string toNarrowString(const wchar_t* pStr, int len=-1); 
inline std::string toNarrowString(const std::wstring& str)
{
	return toNarrowString(str.c_str(), static_cast<int>(str.length()));
}
inline std::string toNarrowString(const char* pStr, int len=-1)
{
	return ((len < 0) ? pStr : std::string(pStr,len));
}
inline std::string toNarrowString(const std::string& str)
{
	return str;
}

#ifdef _UNICODE
	inline TCHAR toTchar(char ch) {return (wchar_t)ch;}
	inline TCHAR toTchar(wchar_t ch) {return ch;}
	inline tstring toTstring(const std::string& s) {return toWideString(s);}
	inline tstring toTstring(const char* p, int len=-1) {return toWideString(p,len);}
	inline tstring toTstring(const std::wstring& s) {return s;}
	inline tstring toTstring(const wchar_t* p, int len=-1) {return ((len < 0) ? p : std::wstring(p, len));}
#else 
	inline TCHAR toTchar(char ch) {return ch;}
	inline TCHAR toTchar(wchar_t ch) {return (((ch >= 0) && (ch <= 0xFF)) ? (char)ch : '?');} 
	inline tstring toTstring(const std::string& s) {return s;}
	inline tstring toTstring(const char* p, int len=-1) {return ((len < 0) ? p : std::string(p, len));}
	inline tstring toTstring(const std::wstring& s) {return toNarrowString(s);}
	inline tstring toTstring(const wchar_t* p, int len=-1) {return toNarrowString(p, len);}
#endif // _UNICODE

#endif //TSTRING_H
