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

#ifndef UTF16FSTREAM_H
#define UTF16FSTREAM_H

#include <fstream>
#include <locale>

typedef std::codecvt<wchar_t, char, mbstate_t> WideCodecvtBase;
class UTF16Codecvt : public WideCodecvtBase
{
public:
	explicit UTF16Codecvt(size_t _R=0) : WideCodecvtBase(_R) {}

protected:
	virtual result do_in(mbstate_t&, const char*, const char*,
		const char*&, wchar_t*, wchar_t*, wchar_t*&) const
	{
		return noconv;
	}
	virtual result do_out(mbstate_t&, const wchar_t*, const wchar_t*,
		const wchar_t*&, char*, wchar_t*, char*&) const
	{
		return noconv;
	}
	virtual result do_unshift(mbstate_t&, char*, char*, char*&) const
	{
		return noconv;
	}
	virtual int do_length(mbstate_t&, const char* _F1, const char* _L1,
		size_t _N2)
	{
		return static_cast<int>((_N2 < (size_t)(_L1 - _F1)) ? _N2 : _L1 - _F1);
	}
	virtual bool do_always_noconv()
	{
		return true;
	}
	virtual int do_max_length()
	{
		return 2;
	}
	virtual int do_encoding()
	{
		return 2;
	}

private:
	UTF16Codecvt(const UTF16Codecvt&);
	UTF16Codecvt& operator=(const UTF16Codecvt&);
};

#define IMBUE_UTF16_CODECVT(outputFile) \
{ \
	std::locale loc(std::locale(), new UTF16Codecvt); \
	(outputFile).imbue(loc); \
}

class utf16ofstream : public std::wofstream
{
public:
	utf16ofstream() {}
	utf16ofstream(const char * filename, std::ios_base::openmode mode = std::ios_base::out)
	{
		open(filename, mode);
	}
	virtual ~utf16ofstream() {}
	virtual void open(const char * filename, std::ios_base::openmode mode = std::ios_base::out)
	{
		IMBUE_UTF16_CODECVT(*this);
		std::wofstream::open(filename, mode);
	}

private:
	utf16ofstream(const utf16ofstream &s);				//prevent copy construction
	utf16ofstream& operator=(const utf16ofstream &s);	//prevent copy assignment
};

class utf16ifstream : public std::wifstream
{
public:
	utf16ifstream() {}
	utf16ifstream(const char * filename, std::ios_base::openmode mode = std::ios_base::in)
	{
		open(filename, mode);
	}
	virtual ~utf16ifstream() {}
	virtual void open(const char * filename, std::ios_base::openmode mode = std::ios_base::in)
	{
		IMBUE_UTF16_CODECVT(*this);
		std::wifstream::open(filename, mode);
	}

private:
	utf16ifstream(const utf16ifstream &s);				//prevent copy construction
	utf16ifstream& operator=(const utf16ifstream &s);	//prevent copy assignment
};

#endif //UTF16FSTREAM_H
