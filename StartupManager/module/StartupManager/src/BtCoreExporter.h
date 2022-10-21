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

#ifndef BTCOREEXPORTER_H
#define BTCOREEXPROTER_H

#include "Boot/BtCore.h"
#include "Types/tfstream.h"
#include "Types/utf16fstream.h"
#include "../Win32++/WinCore.h"

class ExportException : public CWinException
{
public:
	ExportException(LPCTSTR msg) : CWinException(msg) {}
};

class BtCoreExporter
{
public:
	BtCoreExporter(CWnd &hParent, Boot::Core &BtCore);
	~BtCoreExporter();

	void DumpHtml(tstring const& filename, bool bVertical = false) throw();
	void DumpXml(tstring const& filename, tstring const& xslfile = _T("")) throw();

private:
	BtCoreExporter(BtCoreExporter const& rhs);				// prevent copy-construction
	BtCoreExporter& operator=(BtCoreExporter const& rhs);	// prevent assignment

	void ExportItemsAsHtmlVertical(tofstream &htmlfile);
	void ExportItemsAsHtmlHorizontal(tofstream &htmlfile);
	void ExportItemsAsXml(tofstream &xmlfile);

	CWnd &m_hParent;
	Boot::Core &m_BtCore; 
};

inline BtCoreExporter::BtCoreExporter(CWnd &hParent, Boot::Core &BtCore)
	: m_hParent(hParent), m_BtCore(BtCore)
{
}

inline BtCoreExporter::~BtCoreExporter()
{
}

#endif	// BTCOREEXPORTER_H
