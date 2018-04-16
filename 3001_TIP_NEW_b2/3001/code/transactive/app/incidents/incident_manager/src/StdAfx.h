/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/incidents/incident_manager/src/StdAfx.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__3C492C26_B75D_40C2_94D1_F5B67A32F532__INCLUDED_)
#define AFX_STDAFX_H__3C492C26_B75D_40C2_94D1_F5B67A32F532__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x502
#endif
// #ifdef _WIN32_WINNT
// #pragma message("001:_WIN32_WINNT defined")
// #if (_WIN32_WINNT >= 0x0501)
// #pragma message("_WIN32_WINNT greater than 0x0501")
// #endif
// #endif

// #undef _WIN32_WINNT
// 
// #ifdef _WIN32_WINNT
// #pragma message("002:_WIN32_WINNT defined")
// #if (_WIN32_WINNT >= 0x0501)
// #pragma message("_WIN32_WINNT greater than 0x0501")
// #endif
// #else
// #pragma message("003 _WIN32_WINNT undefined")
// #define _WIN32_WINNT 0x0502
// // #define CCM_SETWINDOWTHEME	0x200b
// #endif


#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


// #ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
// #endif




#endif // !defined(AFX_STDAFX_H__3C492C26_B75D_40C2_94D1_F5B67A32F532__INCLUDED_)
