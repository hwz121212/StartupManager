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

#include "BtOrderDlg.h"
#include "resource.h"
#include "types\tfstream.h"

BtOrderDlg::CreateBatchDlg::CreateBatchDlg(Boot::Core &BootList, HWND hWndParent)
	: CDialog(IDD_NEWBAT, hWndParent), m_BootList(BootList), m_File(_T("")), m_Place(_T(""))
{
}

BOOL BtOrderDlg::CreateBatchDlg::OnInitDialog()
{
	m_TT.Create(GetHwnd());
	m_TT.Add(LoadString(DLGBTORDERC_TT_IDC_NAME), IDC_NAME);
	m_TT.Add(LoadString(DLGBTORDERC_TT_IDC_SECTION), IDC_SECTION);
	
	//populate combobox with boot sections
	int i = 0;
	for (Boot::Core::const_reverse_iterator iter = m_BootList.rbegin(); iter != m_BootList.rend(); ++iter, i++)
		if (!(((*iter)->GetSection().find(_T("Userinit")) != std::string::npos)
			|| ((*iter)->GetSection().find(_T("Shell")) != std::string::npos)
			|| ((*iter)->GetSection().find(_T("BHO")) != std::string::npos)))
				::SendDlgItemMessage(GetHwnd(), IDC_SECTION, CB_ADDSTRING, i, (LPARAM)(*iter)->GetSection().c_str());
	::SendDlgItemMessage(GetHwnd(), IDC_SECTION, CB_SETCURSEL, 2, 0);
	
	//limit item name length to 30 characters
	::SendDlgItemMessage(GetHwnd(), IDC_NAME, EM_LIMITTEXT, (WPARAM)30, 0);
	
	::SetDlgItemText(GetHwnd(), IDC_NAME, _T("order"));
	
	return TRUE;
}

void BtOrderDlg::CreateBatchDlg::OnOK()
{
	int length = ::GetWindowTextLength(::GetDlgItem(GetHwnd(), IDC_NAME));
	TCHAR *buffer = new TCHAR[length + 1];
	::GetDlgItemText(GetHwnd(), IDC_NAME, buffer, length + 1);
	m_File = buffer;
	delete [] buffer;
	if(m_File.length() > 0)
		m_File += _T(".bat");
	else
		m_File = _T("order.bat");
		
	length = ::GetWindowTextLength(::GetDlgItem(GetHwnd(), IDC_SECTION));
	buffer = new TCHAR[length + 1];
	::GetDlgItemText(GetHwnd(), IDC_SECTION, buffer, length + 1);
	m_Place = buffer;
	delete [] buffer;
	
	CDialog::OnOK();	
}

void BtOrderDlg::CreateBatchDlg::GetData(tstring &name, tstring &section) const
{
	name    = m_File;
	section = m_Place;
}

/******************************************************************************/

BtOrderDlg::AddToBatchDlg::AddToBatchDlg(HWND hWndParent) : CDialog(IDD_ADDTOBATCH, hWndParent)
{
}

BOOL BtOrderDlg::AddToBatchDlg::OnInitDialog()
{
	m_TT.Create(GetHwnd());
	m_TT.Add(LoadString(DLGBTORDERA_TT_IDOK), IDOK);
	m_TT.Add(LoadString(DLGBTORDERA_TT_IDC_FLAGS), IDC_FLAGS);

	m_Browse.AttachDlgItem(IDC_PATH, this);
	tstring EditTip = LoadString(DLGBTORDERA_TT_IDC_PATH);
	tstring ButtonTip = LoadString(DLGBTORDERA_TT_IDBROWSE);
	m_Browse.SetToolTips(EditTip.c_str(), ButtonTip.c_str());
	m_Browse.SetBrowseInfo(_T("Executable (*.exe; *.com; *.bat; *.scr; *.jar; *.cgi)\0*.exe;*.com;*.bat;*.scr;*.jar;*.cgi\0Shortcut (*.lnk; *.pif)\0*.lnk;*.pif\0Library (*.dll; *.a; *.lib)\0*.dll;*.a;*.lib\0All Files (*.*)\0*.*\0"));
	
	return TRUE;
}

void BtOrderDlg::AddToBatchDlg::OnOK()
{
	int length = ::GetWindowTextLength(::GetDlgItem(GetHwnd(), IDC_PATH));
	if (length > 0)
	{
		TCHAR *buffer = new TCHAR[length + 1];
		::GetDlgItemText(GetHwnd(), IDC_PATH, buffer, length + 1);
		m_Command = buffer;
		delete [] buffer;
		if (m_Command.find(_T(" ")) != std::string::npos)
			m_Command = _T("\"") + m_Command + _T("\"");

		length = ::GetWindowTextLength(::GetDlgItem(GetHwnd(), IDC_FLAGS));
		if (length > 0)
		{
			buffer = new TCHAR[length + 1];
			::GetDlgItemText(GetHwnd(), IDC_FLAGS, buffer, length + 1);
			m_Command = m_Command + _T(" ") + buffer;
			delete [] buffer;
		}
		
		CDialog::OnOK();
	}
	else
	{
		tstring msg_information = LoadString(MSG_INFORMATION);
		::MessageBox(GetHwnd(), LoadString(DLGBTORDERA_MSG_EMPTYFIELD), msg_information.c_str(), MB_OK | MB_ICONINFORMATION);
	}
}

/******************************************************************************/

BtOrderDlg::BtOrderDlg(Boot::Core &BootList, BOOL edit, HWND hWndParent)
	: CDialog(IDD_ORDER, hWndParent), m_BootList(BootList), m_bBrowse(edit), m_Path(_T("")),
	m_Section(_T("")), m_bBrowsed(false)
{
	TCHAR buffer[100];
	::GetEnvironmentVariable(_T("OS"), buffer, 100);
	if (_tcscmp(_T("Windows_NT"), buffer) == 0)
	{
		m_bIsWinNT = true;
		m_BatchLocation = GetFolder(CSIDL_COMMON_APPDATA);
	}
	else
	{
		m_bIsWinNT = false;
		m_BatchLocation = GetFolder(CSIDL_COMMON_DOCUMENTS);
	}
}

tstring BtOrderDlg::GetFolder(UINT nID)
{
	IMalloc *gpMalloc;
	SHGetMalloc(&gpMalloc);
	
	TCHAR tmp[1024];
	::SHGetSpecialFolderPath(GetHwnd(), tmp, nID, 0);
	tstring path = tmp;
	path += _T("\\Startup Manager\\batch\\");
	
	gpMalloc->Release();

	return path;
}

tstring BtOrderDlg::OnBrowse(HWND hWndParent, LPTSTR types, tstring folder)
{
	TCHAR *szFileName = new TCHAR[MAX_PATH + 1];
	szFileName[0] = 0;
	
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner   = hWndParent;
	ofn.lpstrFilter = types;
	ofn.lpstrFile   = szFileName;
	ofn.nMaxFile    = MAX_PATH;
	ofn.Flags       = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	
	if (folder.length() > 0)
	{
		const tstring &rfolder = folder;
		ofn.lpstrInitialDir = rfolder.c_str();
	}
		
	tstring retVal;
	if (GetOpenFileName(&ofn))
		retVal = szFileName;
	else
		retVal = _T("");
	 
	delete [] szFileName;
	
	return retVal;
}

void BtOrderDlg::ReadBatchFile()
{
	tifstream iBatch(toNarrowString(m_Path).c_str(), std::ios::in);
	if (iBatch)
	{   
        tstring item;
        int i = 0;
		while (!iBatch.eof())
		{
			getline(iBatch, item);
                  
			if (item.length() > 0)
			{
				if (item.find(_T("start /wait")) != std::string::npos)
				{
					::SendDlgItemMessage(GetHwnd(), IDC_CHWAIT, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
					item.erase(0, 12); //erase "start /wait "
				}
				else
					item.erase(0, 6); //erase "start "
				
				if (m_bIsWinNT)
					item.erase(0, 3); //erase empty title and space after it
				
				m_List.AddItem(item.c_str(), i, 0);
				i++;
			}
		}
	}
	else
	{
		tstring msg_information = LoadString(MSG_INFORMATION);
		::MessageBox(GetHwnd(), LoadString(DLGBTORDER_MSG_NOBATCH), msg_information.c_str(), MB_OK | MB_ICONINFORMATION);
	}
		
	iBatch.close();
}

void BtOrderDlg::OnNew()
{
	CreateBatchDlg CreateBatchDlg(m_BootList, GetHwnd());
	CreateBatchDlg.DoModal();
	tstring name = _T("");
	CreateBatchDlg.GetData(name, m_Section);
	
	if (name.length() > 0)
	{
		m_Path = m_BatchLocation + name;
		
		::EnableWindow(::GetDlgItem(GetHwnd(), IDOK), true);
		::EnableWindow(::GetDlgItem(GetHwnd(), IDADD), true);
		::EnableWindow(::GetDlgItem(GetHwnd(), IDC_CHWAIT), true);
		::EnableWindow(::GetDlgItem(GetHwnd(), IDNEW), false);
		::EnableWindow(::GetDlgItem(GetHwnd(), IDOPEN), false);
        
		int length = ::GetWindowTextLength(GetHwnd());
		TCHAR *tmp = new TCHAR[length + 1];
		::GetWindowText(GetHwnd(), tmp, length + 1);
		tstring title = tmp;
		delete [] tmp;
		title = title + _T(" [") + m_Path.substr(m_Path.find_last_of(_T("\\/:")) + 1) + _T("]");
		::SetWindowText(GetHwnd(), title.c_str());
		
		::SendDlgItemMessage(GetHwnd(), IDC_CHWAIT, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
	}
}

void BtOrderDlg::OnOpen()
{
	m_Path = OnBrowse(GetHwnd(), _T("Batch File\0*.bat\0"), m_BatchLocation);
	if (m_Path.length() > 0)
	{
		m_bBrowsed = true;
		ReadBatchFile();
        
		::EnableWindow(::GetDlgItem(GetHwnd(), IDOK), true);
		::EnableWindow(::GetDlgItem(GetHwnd(), IDADD), true);
		::EnableWindow(::GetDlgItem(GetHwnd(), IDC_CHWAIT), true);
		::EnableWindow(::GetDlgItem(GetHwnd(), IDNEW), false);
		::EnableWindow(::GetDlgItem(GetHwnd(), IDOPEN), false);
        int length = ::GetWindowTextLength(GetHwnd());
		
		TCHAR *tmp = new TCHAR[length + 1];
		::GetWindowText(GetHwnd(), tmp, length + 1);
		tstring title = tmp;
		delete [] tmp;
		title = title + _T(" [") + m_Path.substr(m_Path.find_last_of(_T("\\/:")) + 1) + _T("]");
		::SetWindowText(GetHwnd(), title.c_str());
	}
}

BOOL BtOrderDlg::OnInitDialog()
{
	m_TT.Create(GetHwnd());
	m_TT.Add(LoadString(DLGBTORDER_TT_IDUP), IDUP);
	m_TT.Add(LoadString(DLGBTORDER_TT_IDDOWN), IDDOWN);
	m_TT.Add(LoadString(DLGBTORDER_TT_IDADD), IDADD);
	m_TT.Add(LoadString(DLGBTORDER_TT_IDDELETE), IDDELETE);
	m_TT.Add(LoadString(DLGBTORDER_TT_IDNEW), IDNEW);
	m_TT.Add(LoadString(DLGBTORDER_TT_IDOPEN), IDOPEN);
	m_TT.Add(LoadString(DLGBTORDER_TT_IDOK), IDOK);
	m_TT.Add(LoadString(DLGBTORDER_TT_IDCANCEL), IDCANCEL);
	m_TT.Add(LoadString(DLGBTORDER_TT_IDC_CHWAIT), IDC_CHWAIT);

	m_List.Create(GetHwnd());

	::SetWindowLongPtr(m_List.GetHwnd(), GWLP_ID, IDC_BATCONTENT);
	
	::MoveWindow(m_List.GetHwnd(), 10, 40, 390, 140, TRUE);
	m_List.InsertColumn(LoadString(DLGBTORDER_STR_LVCOLUMN), 0, 0);
	::SendMessage(m_List.GetHwnd(), LVM_SETCOLUMNWIDTH, 0, LVSCW_AUTOSIZE_USEHEADER);
	
	if (m_bBrowse)
		OnOpen();
	else
		OnNew();
	
	return TRUE;
}

void BtOrderDlg::OnOK()
{
	try
	{
		tstring msg_information = LoadString(MSG_INFORMATION);
		
		int j = ListView_GetItemCount(m_List.GetHwnd());
		if(j > 0)
		{
			tofstream oBatch(toNarrowString(m_Path).c_str(), std::ios::out);
			if (oBatch)
			{
				//add listview content to batch file
				for (int i=0; i<j; i++)
				{
					oBatch << _T("start ");
					if (::IsDlgButtonChecked(GetHwnd(), IDC_CHWAIT) == BST_CHECKED)
						oBatch << _T("/wait ");
					if (m_bIsWinNT)
						oBatch << _T("\"\" ");
					
					oBatch << m_List.GetItemStr(i) << std::endl;
				}
				oBatch.close();

				bool added = false;

				//add batch file to boot sections (if not already in it)
				if ((!m_bBrowsed && (m_Section.length() > 0)) || m_bBrowsed)
				{
					tstring name = _T("");
					added = true;

					if (m_bBrowsed)
					{
						if (!m_BootList.Find(m_Path))
						{
							tstring msg_question = LoadString(MSG_QUESTION);
							if (::MessageBox(GetHwnd(), LoadString(DLGBTORDER_MSG_QADD), msg_question.c_str(), MB_YESNO | MB_ICONQUESTION) == IDYES)
							{
								CreateBatchDlg CreateBatchDlg(m_BootList, GetHwnd());
								if (CreateBatchDlg.DoModal() == IDCANCEL)
									added = false;
								CreateBatchDlg.GetData(name, m_Section);
							}
							else
								added = false;
						}
						else
							CDialog::OnOK();
					}
					
					if (added)
					{						
						if (name.length() <= 0)
						{
							File file;
							file.SetPath(m_Path);
							name = file.GetName().erase(file.GetName().length() - file.GetExt().length() - 1, file.GetExt().length() + 1);
						}

						Boot::WorkItem wItem(m_Path, name, m_Section);
						try
						{
							m_BootList.Add(wItem);
						}

						catch (Boot::Exception const& e)
						{
							::MessageBox(GetHwnd(), (LoadString(DLGBTORDER_MSG_ADDFAILED) + (_T("\n\t") + e.GetMsg())).c_str(), msg_information.c_str(), MB_OK | MB_ICONINFORMATION);
						}

						catch (...)
						{
							::MessageBox(GetHwnd(), LoadString(DLGBTORDER_MSG_ADDFAILED), msg_information.c_str(), MB_OK | MB_ICONINFORMATION);
						}
					}
				}
				else if (!m_bBrowsed)
				{
					::MessageBox(GetHwnd(), _T("Couldn't find section"), _T("Warning"), MB_OK | MB_ICONWARNING);
					return;
				}
				
				//log changes (in case of a new boot order file, Boot::Core::Add logs the changes)
				if (!added)
				{
					Boot::Log::LogInfo info;
					File file;
					file.SetPath(m_Path);
					info.item    = file.GetName().erase(file.GetName().length() - file.GetExt().length() - 1, file.GetExt().length() + 1);
					info.section = _T("-");
					info.action  = Boot::Log::MODIFIED;
					Boot::LOG(info);
				}
				
				CDialog::OnOK();
			}
			else
			{
				oBatch.close();
				::MessageBox(GetHwnd(), LoadString(DLGBTORDER_MSG_BATCH), msg_information.c_str(), MB_OK | MB_ICONINFORMATION);
			}
		}
		else
			::MessageBox(GetHwnd(), LoadString(DLGBTORDER_MSG_EMPTY), msg_information.c_str(), MB_OK | MB_ICONINFORMATION);
	}

	catch (Boot::Exception const& e)
	{
		::MessageBox(GetHwnd(), e.GetMsg().c_str(), LoadString(MSG_INFORMATION), MB_OK | MB_ICONINFORMATION);
	}

	catch (...)
	{
		DebugWarnMsg(_T("BtOrderDlg::OnOK... Unknown error"));
	}
}

void BtOrderDlg::OnMoveUp()
{
	int pos = m_List.GetSelPos();
	if (pos != -1)
	{
		m_List.AddItem(m_List.GetItemStr(pos), pos-1, 0);
		m_List.Delete(pos+1);
	}
	m_List.Select(pos-1);
}

void BtOrderDlg::OnMoveDown()
{
	int pos = m_List.GetSelPos();
	if (pos != -1)
	{
		m_List.AddItem(m_List.GetItemStr(pos), pos+2, 0);
		m_List.Delete(pos);
	}
	m_List.Select(pos+1);
}

void BtOrderDlg::OnAdd()
{
	AddToBatchDlg AddToBatchDlg(GetHwnd());
	AddToBatchDlg.DoModal();
	tstring item = AddToBatchDlg.GetData();
	
	if (item.length() > 0)
	{
		int pos = ListView_GetItemCount(m_List.GetHwnd());
		m_List.AddItem(item.c_str(), pos, 0);
	}
}

void BtOrderDlg::OnDelete()
{
	int pos = m_List.GetSelPos();
	if (pos != -1)
		m_List.Delete(pos);
}

LRESULT BtOrderDlg::OnNotify(WPARAM wParam, LPARAM /*lParam*/)
{
	if (LOWORD(wParam) == IDC_BATCONTENT)
	{		
		int pos = m_List.GetSelPos();
		if (pos != -1)
		{
			if (pos > 0)
        		::EnableWindow(::GetDlgItem(GetHwnd(), IDUP), true);
        	else
        		::EnableWindow(::GetDlgItem(GetHwnd(), IDUP), false);
        		
        	if (pos+1 < ListView_GetItemCount(m_List.GetHwnd()))
        		::EnableWindow(::GetDlgItem(GetHwnd(), IDDOWN), true);
			else
        		::EnableWindow(::GetDlgItem(GetHwnd(), IDDOWN), false);
        		
			::EnableWindow(::GetDlgItem(GetHwnd(), IDDELETE), true);
			
		}
		else
		{
        	::EnableWindow(::GetDlgItem(GetHwnd(), IDUP), false);
        	::EnableWindow(::GetDlgItem(GetHwnd(), IDDOWN), false);
        	::EnableWindow(::GetDlgItem(GetHwnd(), IDDELETE), false);
		}
	}
	
	return TRUE;
}

BOOL BtOrderDlg::DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{			
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDNEW: OnNew(); break;
		case IDOPEN: OnOpen(); break;
		case IDUP: OnMoveUp(); break;
		case IDDOWN: OnMoveDown(); break;
		case IDADD: OnAdd(); break;
		case IDDELETE: OnDelete(); break;
		}
		break;
		
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		break;
	} // switch (uMsg)

	//default dialog procedure for other messages
	return CDialog::DialogProc(hwnd, uMsg, wParam, lParam);	
} // BOOL CALLBACK DialogProc(...)
