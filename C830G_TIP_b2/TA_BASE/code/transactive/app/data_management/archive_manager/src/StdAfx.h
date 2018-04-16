/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Alan Brims
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * stdafx.h : include file for standard system include files,
  * or project specific include files that are used frequently, but
  * are changed infrequently
  *
  */

#if !defined(AFX_STDAFX_H__E8BCC1F6_5A94_4E91_BC63_A72E5D3DB209__INCLUDED_)
#define AFX_STDAFX_H__E8BCC1F6_5A94_4E91_BC63_A72E5D3DB209__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#if defined( WIN32 )
#pragma warning ( disable : 4786 4250 )
#endif // defined( WIN32 )

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <shlwapi.h>  // for SHCreateStreamOnFileEx
#endif // _AFX_NO_AFXCMN_SUPPORT

#define WM_WRITE_COMPLETE           (WM_USER+1)
#define WM_CLOSE_DLG                (WM_USER+2)
#define WM_COMPLETE_DLG             (WM_USER+3)
#define WM_SERVER_STATUS_CHANGE     (WM_USER+4)
#define WM_SESSIONID_CHANGE         (WM_USER+5)
#define WM_DISABLE_DLG              (WM_USER+6)
#define WM_TERMINATE_WORKERTHREAD (WM_USER+7)

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__E8BCC1F6_5A94_4E91_BC63_A72E5D3DB209__INCLUDED_)
