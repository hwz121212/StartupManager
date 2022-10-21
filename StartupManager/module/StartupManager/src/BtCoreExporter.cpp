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

#include "BtCoreExporter.h"
#include "resource.h"

#ifdef UNICODE
  #define NEWLINE _T("\r\n")
#else
  #define NEWLINE std::endl
#endif

void BtCoreExporter::ExportItemsAsHtmlVertical(tofstream &htmlfile)
{
	if (!htmlfile || !htmlfile.is_open())
		throw (ExportException(_T("ExportItemsAsHtmlVetical... htmlfile is not a valid file handle")));

	tstring item     = m_hParent.LoadString(STR_COL1);
	tstring section  = m_hParent.LoadString(STR_COL2);
	tstring flags    = m_hParent.LoadString(STR_COL3);
	tstring filepath = m_hParent.LoadString(STR_COL4);
	tstring disabled = m_hParent.LoadString(STR_DISABLED);

	htmlfile << _T("\t\t<table border=\"1\">") << NEWLINE;

	Boot::Core::const_iterator it = m_BtCore.begin();
	for (; it != m_BtCore.end(); ++it)
	{
		(*it)->SelFirst();
		for (int j = 0; j < (*it)->Count(); (*it)->SelNext(), j++)
		{
			Boot::Item *btItem = (*it)->GetNode();

			htmlfile << _T("\t\t\t<tr><td class=\"hed\">") << item.c_str() << _T("</td>");
			htmlfile << _T("<td class=\"hed\">") << btItem->GetiName().c_str() << _T("</td></tr>") << NEWLINE;
			htmlfile << _T("\t\t\t<tr><td><b>") << disabled.c_str() << _T("</b></td><td>");
			htmlfile << (btItem->IsEnabled() ? _T("No") : _T("Yes")) << _T("&nbsp;</td></tr>") << NEWLINE;
			htmlfile << _T("\t\t\t<tr><td><b>") << section.c_str() << _T("</b></td><td>");
			htmlfile << btItem->GetSection()->GetSection().c_str() << _T("&nbsp;</td></tr>") << NEWLINE;
			htmlfile << _T("\t\t\t<tr><td><b>") << flags.c_str() << _T("</b></td><td>");
			htmlfile << btItem->GetFlags().c_str() << _T("&nbsp;</td></tr>") << NEWLINE;
			htmlfile << _T("\t\t\t<tr><td><b>") << filepath.c_str() << _T("</b></td><td>");
			htmlfile << btItem->GetPath().c_str() << _T("&nbsp;</td></tr>") << NEWLINE;
		}
	}

	htmlfile << _T("\t\t</table>") << NEWLINE;
}

void BtCoreExporter::ExportItemsAsHtmlHorizontal(tofstream &htmlfile)
{
	if (!htmlfile || !htmlfile.is_open())
		throw (ExportException(_T("ExportItemsAsHtmlHorizontal... htmlfile is not a valid file handle")));

	tstring item     = m_hParent.LoadString(STR_COL1);
	tstring section  = m_hParent.LoadString(STR_COL2);
	tstring flags    = m_hParent.LoadString(STR_COL3);
	tstring filepath = m_hParent.LoadString(STR_COL4);
	tstring disabled = m_hParent.LoadString(STR_DISABLED);
				
	htmlfile << _T("\t\t<p/>") << NEWLINE;
	htmlfile << _T("\t\t<table border=\"1\">") << NEWLINE;
	htmlfile << _T("\t\t\t<tr>") << NEWLINE;
	htmlfile << _T("\t\t\t\t<th><b>") << item.c_str() << _T("</b></th>") << NEWLINE;
	htmlfile << _T("\t\t\t\t<th><b>") << disabled.c_str() << _T("</b></th>") << NEWLINE;
	htmlfile << _T("\t\t\t\t<th><b>") << section.c_str() << _T("</b></th>") << NEWLINE;
	htmlfile << _T("\t\t\t\t<th><b>") << flags.c_str() << _T("</b></th>") << NEWLINE;
	htmlfile << _T("\t\t\t\t<th><b>") << filepath.c_str() << _T("</b></th>") << NEWLINE;
	htmlfile << _T("\t\t\t</tr>") << NEWLINE;

	Boot::Core::const_iterator it = m_BtCore.begin();
	for (; it != m_BtCore.end(); ++it)
	{
		(*it)->SelFirst();
		for (int j = 0; j < (*it)->Count(); (*it)->SelNext(), j++)
		{
			Boot::Item *btItem = (*it)->GetNode();

			htmlfile << _T("\t\t\t<tr>");
			htmlfile << _T("<td>&nbsp;");
			htmlfile << btItem->GetiName().c_str() << _T("</td>");
			htmlfile << _T("<td>&nbsp;");
			htmlfile << (btItem->IsEnabled() ? _T("No") : _T("Yes")) << _T("</td>");
			htmlfile << _T("<td>&nbsp;");
			htmlfile << btItem->GetSection()->GetSection().c_str() << _T("</td>");
			htmlfile << _T("<td>&nbsp;");
			htmlfile << btItem->GetFlags().c_str() << _T("</td>");
			htmlfile << _T("<td>&nbsp;");
			htmlfile << btItem->GetPath().c_str() << _T("</td>");
			htmlfile << _T("</tr>");
			htmlfile << NEWLINE;
		}
	}

	htmlfile << _T("\t\t</table>") << NEWLINE;
}

void BtCoreExporter::DumpHtml(tstring const& path, bool bVertical) throw()
{
#ifdef UNICODE
	utf16ofstream htmlfile;
#else
	tofstream htmlfile;
#endif

	try
	{
	#ifdef UNICODE
		htmlfile.open(toNarrowString(path).c_str(), std::ios::out | std::ios::binary);
		
		if (htmlfile && htmlfile.is_open())
		{
			//Write ucs-2 little endian (utf-16) byte order marker to file.
			//Notice: little endian => to get 0xFF 0xFE, we write 0xFE 0xFF
			wchar_t bom = static_cast<wchar_t>(0xFEFF);
			htmlfile.write(&bom, 1);

			htmlfile << _T("<?xml version=\"1.0\" encoding=\"unicode\"?>") << NEWLINE;
	#else
		htmlfile.open(path.c_str(), std::ios::out);

		if (htmlfile && htmlfile.is_open())
		{
			htmlfile << _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>") << NEWLINE;
	#endif

			htmlfile << _T("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"") << NEWLINE;
			htmlfile << _T("\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">") << NEWLINE;
			htmlfile << _T("<html xmlns=\"http://www.w3.org/1999/xhtml\">") << NEWLINE;
			htmlfile << _T("\t<head>") << NEWLINE;
			htmlfile << _T("\t\t<title>Startup Programs List</title>") << NEWLINE;
			htmlfile << _T("\t\t<style type=\"text/css\" media=\"screen\">") << NEWLINE;
			htmlfile << _T("table {") << NEWLINE;
			htmlfile << _T("\twidth: 95%;") << NEWLINE;
			htmlfile << _T("\ttext-align: center;") << NEWLINE;
			htmlfile << _T("\tfont-family: Verdana, Geneva, Arial, Helvetica, sans-serif;") << NEWLINE;
			htmlfile << _T("\tfont-weight: normal;") << NEWLINE;
			htmlfile << _T("\tfont-size: 11px;") << NEWLINE;
			htmlfile << _T("\tcolor: #fff;") << NEWLINE;
			htmlfile << _T("\tbackground-color: #666;") << NEWLINE;
			htmlfile << _T("\tborder: 0px;") << NEWLINE;
			htmlfile << _T("\tborder-collapse: collapse;") << NEWLINE;
			htmlfile << _T("\tborder-spacing: 0px;") << NEWLINE;
			htmlfile << _T("}") << NEWLINE << NEWLINE;
			htmlfile << _T("table td {") << NEWLINE;
			htmlfile << _T("\tbackground-color: #CCC;") << NEWLINE;
			htmlfile << _T("\tcolor: #000;") << NEWLINE;
			htmlfile << _T("\tpadding: 4px;") << NEWLINE;
			htmlfile << _T("\ttext-align: left;") << NEWLINE;
			htmlfile << _T("\tborder: 1px #fff solid;") << NEWLINE;
			htmlfile << _T("}") << NEWLINE;
			
			if (bVertical)
			{
				htmlfile << _T("table td.hed {") << NEWLINE;
				htmlfile << _T("\tbackground-color: #666;") << NEWLINE;
				htmlfile << _T("\tcolor: #fff;") << NEWLINE;
				htmlfile << _T("\tpadding: 4px;") << NEWLINE;
				htmlfile << _T("\ttext-align: left;") << NEWLINE;
				htmlfile << _T("\tborder-bottom: 2px #fff solid;") << NEWLINE;
				htmlfile << _T("\tfont-size: 12px;") << NEWLINE;
				htmlfile << _T("\tfont-weight: bold;") << NEWLINE;
				htmlfile << _T("}") << NEWLINE;
			}
			
			htmlfile << _T("\t\t</style>") << NEWLINE;
			htmlfile << _T("\t</head>") << NEWLINE;
			htmlfile << _T("\t<body>") << NEWLINE;
			htmlfile << _T("\t\t<h1>Startup Programs List</h1>") << NEWLINE;
			htmlfile << _T("\t\t<p/>") << NEWLINE;
			htmlfile << _T("\t\t<h2>Created with <a href=\"http://startupmanager.org/\" target=\"blank\">Startup Manager</a></h2>") << NEWLINE;
			
			if (bVertical)
				ExportItemsAsHtmlVertical(htmlfile);
			else		
				ExportItemsAsHtmlHorizontal(htmlfile);

			htmlfile << _T("\t</body>") << NEWLINE;
			htmlfile << _T("</html>") << NEWLINE;
	
			htmlfile.close();
		}
		else
			throw (ExportException(_T("ExportAsHtml... Couldn't create Html file")));
	}
	
	catch (ExportException const &e)
	{
		if (htmlfile && htmlfile.is_open())
			htmlfile.close();

		e.MessageBox();
	}
	
	catch (...)
	{
		if (htmlfile && htmlfile.is_open())
			htmlfile.close();

	#ifdef DEBUG
		::MessageBox(0, _T("ExportAsHtml... Unknown exception"), _T("Exception"), MB_OK | MB_ICONWARNING);
	#endif
	}
}

void BtCoreExporter::ExportItemsAsXml(tofstream &xmlfile)
{
	if (!xmlfile || !xmlfile.is_open())
		throw (ExportException(_T("ExportItemsAsXml... xmlfile is not a valid file handle")));

	tstring item     = m_hParent.LoadString(STR_COL1);
	tstring section  = m_hParent.LoadString(STR_COL2);
	tstring flags    = m_hParent.LoadString(STR_COL3);
	tstring filepath = m_hParent.LoadString(STR_COL4);
	tstring disabled = m_hParent.LoadString(STR_DISABLED);

	xmlfile << _T("\t<columns>") << NEWLINE;
	xmlfile << _T("\t\t<name>") << item.c_str() << _T("</name>") << NEWLINE;
	xmlfile << _T("\t\t<disabled>") << disabled.c_str() << _T("</disabled>") << NEWLINE;
	xmlfile << _T("\t\t<section>") << section.c_str() << _T("</section>") << NEWLINE;
	xmlfile << _T("\t\t<flags>") << flags.c_str() << _T("</flags>") << NEWLINE;
	xmlfile << _T("\t\t<filepath>") << filepath.c_str() << _T("</filepath>") << NEWLINE;
	xmlfile << _T("\t</columns>") << NEWLINE;

	Boot::Core::const_iterator it = m_BtCore.begin();
	for (; it != m_BtCore.end(); ++it)
	{
		(*it)->SelFirst();
		for (int j = 0; j < (*it)->Count(); (*it)->SelNext(), j++)
		{
			Boot::Item *btItem = (*it)->GetNode();

			xmlfile << _T("\t<item>") << NEWLINE;
			xmlfile << _T("\t\t<name>") << btItem->GetiName().c_str() << _T("</name>") << NEWLINE;
			xmlfile << _T("\t\t<disabled>") << (btItem->IsEnabled() ? _T("No") : _T("Yes")) << _T("</disabled>") << NEWLINE;
			xmlfile << _T("\t\t<section>") << btItem->GetSection()->GetSection().c_str() << _T("</section>") << NEWLINE;
			xmlfile << _T("\t\t<flags>") << btItem->GetFlags().c_str() << _T("</flags>") << NEWLINE;
			xmlfile << _T("\t\t<filepath>") << btItem->GetPath().c_str() << _T("</filepath>") << NEWLINE;
			xmlfile << _T("\t</item>") << NEWLINE;
		}
	}
}

void BtCoreExporter::DumpXml(tstring const& path, tstring const& xslfile) throw()
{
#ifdef UNICODE
	utf16ofstream xmlfile;
#else
	tofstream xmlfile;
#endif 

	try
	{
	#ifdef UNICODE
		xmlfile.open(toNarrowString(path).c_str(), std::ios::out | std::ios::binary);
		
		if (xmlfile && xmlfile.is_open())
		{
			//Write ucs-2 little endian (utf-16) byte order marker to file.
			//Notice: little endian => to get 0xFF 0xFE, we write 0xFE 0xFF
			wchar_t bom = static_cast<wchar_t>(0xFEFF);
			xmlfile.write(&bom, 1);

			xmlfile << _T("<?xml version=\"1.0\" encoding=\"unicode\"?>") << NEWLINE;

			//Check whether xsl stylesheet should be added
			if (xslfile.length() > 0)
			{
				tstring topath = path.substr(0, path.find_last_of(_T("\\/:"))) + _T("/st-m_items_utf16.xsl");
				if (::CopyFile(xslfile.c_str(), topath.c_str(), FALSE) != 0)
					xmlfile << _T("<?xml-stylesheet type=\"text/xsl\" href=\"st-m_items_utf16.xsl\"?>") << NEWLINE;
			}
			
			xmlfile << _T("<st-m_items xmlns=\"http://startupmanager.org\"") << NEWLINE;
			xmlfile << _T("xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"") << NEWLINE;
			xmlfile << _T("xsi:schemaLocation=\"http://startupmanager.org http://startupmanager.org/XMLSchema/st-m_items_utf16.xsd\">") << NEWLINE;
	#else
		xmlfile.open(path.c_str(), std::ios::out);

		if (xmlfile && xmlfile.is_open())
		{
			xmlfile << _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>") << NEWLINE;

			//Check whether xsl stylesheet should be added
			if (xslfile.length() > 0)
			{
				tstring topath = path.substr(0, path.find_last_of(_T("\\/:"))) + _T("/st-m_items.xsl");
				if (::CopyFile(xslfile.c_str(), topath.c_str(), FALSE) != 0)
					xmlfile << _T("<?xml-stylesheet type=\"text/xsl\" href=\"st-m_items.xsl\"?>") << NEWLINE;
			}
			
			xmlfile << _T("<st-m_items xmlns=\"http://startupmanager.org\"") << NEWLINE;
			xmlfile << _T("xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"") << NEWLINE;
			xmlfile << _T("xsi:schemaLocation=\"http://startupmanager.org http://startupmanager.org/XMLSchema/st-m_items.xsd\">") << NEWLINE;
	#endif

			//write items to file now
			ExportItemsAsXml(xmlfile);

			xmlfile << _T("</st-m_items>") << NEWLINE;
	
			xmlfile.close();
		}
		else
			throw(ExportException(_T("ExportAsXml... Couldn't create Xml file")));
	}
	
	catch (ExportException const &e)
	{
		if (xmlfile && xmlfile.is_open())
			xmlfile.close();

		e.MessageBox();
	}
	
	catch (...)
	{
		if (xmlfile && xmlfile.is_open())
			xmlfile.close();

	#ifdef DEBUG
		::MessageBox(0, _T("ExportAsXML... Unknown exception"), _T("Exception"), MB_OK | MB_ICONWARNING);
	#endif
	}
}
