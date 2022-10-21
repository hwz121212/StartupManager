/*
 * Copyright (C) 2004-2008 Glenn Van Loon, cnx_glenn@users.sourceforge.net
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

#ifndef UTF8FSTREAM_H
#define UTF8FSTREAM_H

#include <fstream>
#include <locale>

// The UTF8Codecvt class was based on the utf8_codecvt_facet boost class.
// Copyright  2001 Ronald Garcia, Indiana University (garcia@osl.iu.edu)
// Andrew Lumsdaine, Indiana University (lums@osl.iu.edu). Permission to copy, 
// use, modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided "as is"
// without express or implied warranty, and with no claim as to its suitability
// for any purpose.

typedef std::codecvt<wchar_t, char, mbstate_t> WideToUTF8CodecvtBase;
class UTF8Codecvt : public WideToUTF8CodecvtBase
{
public:
	explicit WideToUTF8CodecvtBase(std::size_t no_locale_manage=0)
		: std::codecvt<wchar_t, char, std::mbstate_t>(no_locale_manage) 
	{}

protected:
	virtual std::codecvt_base::result do_in(
		std::mbstate_t& state, const char * from,
		const char * from_end, const char * & from_next,
		wchar_t * to, wchar_t * to_end, wchar_t * & to_next) const;

	virtual std::codecvt_base::result do_out(
		std::mbstate_t & state, const wchar_t * from,
		const wchar_t * from_end, const wchar_t*  & from_next,
		char * to, char * to_end, char * & to_next) const;

	bool invalid_continuing_octet(unsigned char octet_1) const
	{
		return ((octet_1 < 0x80) || (0xbf < octet_1));
	}

	bool invalid_leading_octet(unsigned char octet_1) const
	{
		return (((0x7f < octet_1) && (octet_1 < 0xc0)) || (octet_1 > 0xfd));
	}

	// continuing octets = octets except for the leading octet
	static unsigned int get_cont_octet_count(unsigned char lead_octet)
	{
		return (get_octet_count(lead_octet) - 1);
	}

	static unsigned int get_octet_count(unsigned char lead_octet);

	// How many "continuing octets" will be needed for this word
	// ==   total octets - 1.
	int get_cont_octet_out_count(wchar_t word) const;

	virtual bool do_always_noconv() const throw() {return false;}

	// UTF-8 isn't really stateful since we rewind on partial conversions
	virtual std::codecvt_base::result do_unshift(
		std::mbstate_t&, char * from, char * /*to*/, char * & next) const 
	{
		next = from;
		return ok;
	}

	virtual int do_encoding() const throw()
	{
		const int variable_byte_external_encoding = 0;
		return variable_byte_external_encoding;
	}

	// How many char objects can I process to get <= max_limit
	// wchar_t objects?
	virtual int do_length(
		std::mbstate_t &, const char * from, 
		const char * from_end, std::size_t max_limit) const throw();

	// Largest possible value do_length(state,from,from_end,1) could return.
	virtual int do_max_length() const throw ()
	{
		return 6; // largest UTF-8 encoding of a UCS-4 character
	}

private:
	UTF8Codecvt(const UTF8Codecvt&);
	UTF8Codecvt& operator=(const UTF8Codecvt&);
};

#define IMBUE_UTF8_CODECVT(outputFile) \
{ \
	std::locale loc(std::locale(), new UTF8Codecvt); \
	(outputFile).imbue(loc); \
}

class utf8ofstream : public std::wofstream
{
public:
	utf8ofstream() {}
	utf8ofstream(const char * filename, std::ios_base::openmode mode = std::ios_base::out)
	{
		open(filename, mode);
	}
	virtual ~utf8ofstream() {}
	virtual void open(const char * filename, std::ios_base::openmode mode = std::ios_base::out)
	{
		IMBUE_UTF8_CODECVT(*this);
		std::wofstream::open(filename, mode);
	}

private:
	utf8ofstream(const utf8ofstream &s);			//prevent copy construction
	utf8ofstream& operator=(const utf8ofstream &s);	//prevent copy assignment
};

class utf8ifstream : public std::wifstream
{
public:
	utf8ifstream() {}
	utf8ifstream(const char * filename, std::ios_base::openmode mode = std::ios_base::in)
	{
		open(filename, mode);
	}
	virtual ~utf8ifstream() {}
	virtual void open(const char * filename, std::ios_base::openmode mode = std::ios_base::in)
	{
		IMBUE_UTF8_CODECVT(*this);
		std::wifstream::open(filename, mode);
	}

private:
	utf8ifstream(const utf8ifstream &s);			//prevent copy construction
	utf8ifstream& operator=(const utf8ifstream &s);	//prevent copy assignment
};

#endif //UTF8FSTREAM_H
