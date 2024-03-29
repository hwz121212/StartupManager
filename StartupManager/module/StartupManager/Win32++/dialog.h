/*
 * Startup Manager    Version 2.4.2
 * Copyright (C) 2004-2008 Glenn Van Loon, glenn@startupmanager.org
 *
 * Win32++            Version 6.3
 * Copyright (c) 2005-2008  David Nash
 *
 * This file is part of Startup Manager
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


////////////////////////////////////////////////////////
// dialog.h
//  Declaration of the CDialog class

// CDialog adds support for dialogs to Win32++. Dialogs are specialised
// windows which are a parent window for common controls. Common controls
// are special window types such as buttons, edit controls, tree views,
// list views, static text etc.

// The layout of a dialog is typically defined in a resource script file
// (often Resource.rc). While this script file can be constructed manually,
// it is often created using a resource editor. If your compiler doesn't
// include a resource editor, you might find ResEdit useful. It is a free
// resource editor available for download at:
// http://www.resedit.net/

// CDialog supports modal and modeless dialogs. It also supports the creation
// of dialogs defined in a resource script file, as well as those defined in
// a dialog template.

// Use the Dialog generic program as the starting point for your own dialog
// applications.
// The DlgSubclass sample demonstrates how to use subclassing to customise
// the behaviour of common controls in a dialog.


#ifndef DIALOG_H
#define DIALOG_H

#include "wincore.h"


namespace Win32xx
{

	class CDialog : public CWnd
	{
	public:
		CDialog(UINT nResID, HWND hWndParent = NULL);
		CDialog(LPCTSTR lpszResName, HWND hWndParent = NULL);
		CDialog(LPCDLGTEMPLATE lpTemplate, HWND hWndParent = NULL);
		virtual ~CDialog();

		// You probably won't need to override these functions
		virtual HWND Create(HWND hWndParent);
		virtual INT_PTR DoModal();
		virtual HWND DoModeless();

	protected:
		// These are the functions you might wish to override
		virtual BOOL DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual BOOL DialogProcDefault(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual BOOL PreTranslateMessage(MSG* pMsg);
		virtual void EndDialog(INT_PTR nResult);
		virtual void OnCancel();
		virtual BOOL OnInitDialog();
		virtual void OnOK();

		// Can't override this function
		static BOOL CALLBACK StaticDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
		BOOL IsIndirect;				// a flag for Indirect dialogs
		BOOL IsModal;					// a flag for modal dialogs
		LPCTSTR m_lpszResName;			// the resource name for the dialog
		LPCDLGTEMPLATE m_lpTemplate;	// the dialog template for indirect dialogs

	};

	inline CDialog::CDialog(LPCTSTR lpszResName, HWND hWndParent/* = NULL*/)
		: IsIndirect(FALSE), IsModal(TRUE), m_lpszResName(lpszResName), m_lpTemplate(NULL)
	{
		m_hWndParent = hWndParent;
		::InitCommonControls();
	}

	inline CDialog::CDialog(UINT nResID, HWND hWndParent/* = NULL*/)
		: IsIndirect(FALSE), IsModal(TRUE), m_lpszResName(MAKEINTRESOURCE (nResID)), m_lpTemplate(NULL)
	{
		m_hWndParent = hWndParent;
		::InitCommonControls();
	}

	//For indirect dialogs - created from a dialog box template in memory.
	inline CDialog::CDialog(LPCDLGTEMPLATE lpTemplate, HWND hWndParent/* = NULL*/)
		: IsIndirect(TRUE), IsModal(TRUE), m_lpszResName(NULL), m_lpTemplate(lpTemplate)
	{
		m_hWndParent = hWndParent;
		::InitCommonControls();
	}

	inline CDialog::~CDialog()
	{
		if (m_hWnd != NULL)
		{
			if (IsModal)
				::EndDialog(m_hWnd, 0);
			else
				Destroy();
		}
	}

	inline HWND CDialog::Create(HWND hWndParent = 0)
	{
		// Allow a dialog to be used as a child window
		SetParent(hWndParent);
		return DoModeless();
	}

	inline BOOL CDialog::DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		// Override this function in your class derrived from CDialog if you wish to handle messages
		// A typical function might look like this:

		//	switch (uMsg)
		//	{
		//	case MESSAGE1:		// Some Windows API message
		//		OnMessage1();	// A user defined function
		//		break;			// Also do default processing
		//	case MESSAGE2:
		//		OnMessage2();
		//		return x;		// Don't do default processing, but instead return
		//						//  a value recommended by the Windows API documentation
		//	}

		// Always pass unhandled messages on to DialogProcDefault
		return DialogProcDefault(hWnd, uMsg, wParam, lParam);
	}

	inline BOOL CDialog::DialogProcDefault(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		// All unhandled dialog messages end up here

		switch (uMsg)
	    {
	    case WM_INITDIALOG:
			{
				m_hWnd = hWnd;
				// Center the dialog
				CenterWindow();
			}
		    return OnInitDialog();
	    case WM_COMMAND:
	        switch (LOWORD (wParam))
	        {
	        case IDOK:
				OnOK();
				return TRUE;
			case IDCANCEL:
				OnCancel();
				return TRUE;
			default:
				{
					OnCommand(wParam, lParam);

					// Refelect this message if it's from a control
					CWnd* Wnd = FromHandle((HWND)lParam);
					if (Wnd != NULL)
						Wnd->OnMessageReflect(uMsg, wParam, lParam);
				}
				break;  // Some commands require default processing
	        }
	        break;
		case WM_NOTIFY:
			{
				// Do Notification reflection if it came from a CWnd object
				HWND hwndFrom = ((LPNMHDR)lParam)->hwndFrom;
				CWnd* WndFrom = FromHandle(hwndFrom);
				if (WndFrom != NULL)
				{
					BOOL bReturn = (BOOL)WndFrom->OnNotifyReflect(wParam, lParam);
					if (bReturn) return TRUE;
				}

				if (m_hWnd != ::GetParent(hwndFrom))
				{
					// Some controls (eg ListView) have child windows.
					// Reflect those notifications too.
					CWnd* WndFromParent = FromHandle(GetParent(hwndFrom));
					if (WndFromParent != NULL)
					{
						BOOL bReturn = (BOOL)WndFromParent->OnNotifyReflect(wParam, lParam);
						if (bReturn) return TRUE;
					}
				}
			}
			return (TRUE == OnNotify(wParam, lParam) );

		// A set of messages to be reflected back to the control that generated them
		case WM_CTLCOLORBTN:
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORDLG:
		case WM_CTLCOLORLISTBOX:
		case WM_CTLCOLORSCROLLBAR:
		case WM_CTLCOLORSTATIC:
		case WM_DRAWITEM:
		case WM_MEASUREITEM:
		case WM_DELETEITEM:
		case WM_COMPAREITEM:
		case WM_CHARTOITEM:
		case WM_VKEYTOITEM:
		case WM_HSCROLL:
		case WM_VSCROLL:
		case WM_PARENTNOTIFY:
			return (BOOL) MessageReflect(hWnd, uMsg, wParam, lParam);

	    } // switch(uMsg)
	    return FALSE;

	} // LRESULT CALLBACK CDialog::DialogProc(...)

	inline INT_PTR CDialog::DoModal()
	{
		// Create a modal dialog
		// A modal dialog box must be closed by the user before the application continues
		try
		{
			if (IsWindow())
				throw CWinException(_T("CDialog::DoModal ... Window already exists"));

			IsModal=TRUE;

			// Ensure this thread has the TLS index set
			TLSData* pTLSData = GetApp()->SetTlsIndex();

			// Store the CWnd pointer in Thread Local Storage
			pTLSData->pCWnd = this;

			HINSTANCE hInstance = GetApp()->GetInstanceHandle();

			// Create a modal dialog
			INT_PTR nResult;
			if (IsIndirect)
				nResult = ::DialogBoxIndirect(hInstance, m_lpTemplate, m_hWndParent, (DLGPROC)CDialog::StaticDialogProc);
			else
			{
				if (::FindResource(GetApp()->GetResourceHandle(), m_lpszResName, RT_DIALOG))
					hInstance = GetApp()->GetResourceHandle();
				nResult = ::DialogBox(hInstance, m_lpszResName, m_hWndParent, (DLGPROC)CDialog::StaticDialogProc);
			}
			// Tidy up
			m_hWnd = NULL;
			pTLSData->pCWnd = NULL;

			if (nResult == -1)
				throw CWinException(_T("Failed to create modal dialog box"));

			return nResult;
		}

		catch (const CWinException &e )
		{
			e.MessageBox();
			return -1L;
		}

	}

	inline HWND CDialog::DoModeless()
	{
		// Modeless dialog
		try
		{
			if (IsWindow())
				throw CWinException(_T("CDialog::DoModeless ... Window already exists"));

			IsModal=FALSE;

			// Ensure this thread has the TLS index set
			TLSData* pTLSData = GetApp()->SetTlsIndex();

			// Store the CWnd pointer in Thread Local Storage
			pTLSData->pCWnd = this;

			HINSTANCE hInstance = GetApp()->GetInstanceHandle();

			// Create a modeless dialog
			if (IsIndirect)
				m_hWnd = ::CreateDialogIndirect(hInstance, m_lpTemplate, m_hWndParent, (DLGPROC)CDialog::StaticDialogProc);
			else
			{
				if (::FindResource(GetApp()->GetResourceHandle(), m_lpszResName, RT_DIALOG))
					hInstance = GetApp()->GetResourceHandle();

				m_hWnd = ::CreateDialog(hInstance, m_lpszResName, m_hWndParent, (DLGPROC)CDialog::StaticDialogProc);
			}

			// Tidy up
			pTLSData->pCWnd = NULL;

			// Now handle dialog creation failure
			if (!m_hWnd)
				throw CWinException(_T("CDialog::DoModeless ... Failed to create dialog"));
		}

		catch (const CWinException &e )
		{
			e.MessageBox();
		}

		return m_hWnd;
	}

	inline void CDialog::EndDialog(INT_PTR nResult)
	{
		if (::IsWindow(m_hWnd))
		{
			if (IsModal)
				::EndDialog(m_hWnd, nResult);
			else
				Destroy();
		}
		m_hWnd = NULL;
	}

	inline void CDialog::OnCancel()
	{
		// Override to customize OnCancel behaviour
		EndDialog(IDCANCEL);
	}

	inline BOOL CDialog::OnInitDialog()
	{
		// Called when the dialog is initialized
		// Override it in your derived class to automatically perform tasks
		// The return value is used by WM_INITDIALOG

		return TRUE;
	}

	inline void CDialog::OnOK()
	{
		// Override to customize OnOK behaviour
		EndDialog(IDOK);
	}

	inline BOOL CDialog::PreTranslateMessage(MSG* pMsg)
	{
		// allow the dialog to translate Tab, Left, Right, Up, and Down arrow keys
		if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_TAB || pMsg->wParam == VK_LEFT
			|| pMsg->wParam == VK_UP || pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_DOWN))
		{
			if (IsDialogMessage(m_hWnd, pMsg))
				return TRUE;
		}

		return CWnd::PreTranslateMessage(pMsg);
	}

	inline BOOL CALLBACK CDialog::StaticDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		try
		{
			// Find the CWnd pointer mapped to this HWND
			CDialog* w = (CDialog*)GetApp()->GetCWndFromMap(hWnd);
			if (0 != w)
			{
				// CDialog pointer found, so call the CDialog's DialogProc
				return w->DialogProc(hWnd, uMsg, wParam, lParam);
			}

			else
			{
				// The HWND wasn't in the map, so add it now

				TLSData* pTLSData = (TLSData*)TlsGetValue(GetApp()->GetTlsIndex());
				if (NULL == pTLSData)
					throw CWinException(_T("CWnd::StaticCBTProc ... Unable to get TLS"));

				// Retrieve pointer to CWnd object from Thread Local Storage TLS
				w = (CDialog*)pTLSData->pCWnd;
				if (NULL == w)
					throw CWinException(_T("CWnd::StaticWindowProc .. Failed to route message"));

				pTLSData->pCWnd = NULL;

				// Store the Window pointer into the HWND map
				w->m_hWnd = hWnd;
				w->AddToMap();

				return w->DialogProc(hWnd, uMsg, wParam, lParam);
			}
		}

		catch (const CWinException &e )
		{
			e.MessageBox();
			return FALSE;
		}

	} // LRESULT CALLBACK CDialog::StaticDialogProc(...)

} // namespace Win32xx

#endif // DIALOG_H

