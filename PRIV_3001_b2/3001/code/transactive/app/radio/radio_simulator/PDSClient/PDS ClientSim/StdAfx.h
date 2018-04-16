// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__8912783F_B35E_4F55_BF86_649735CBF74D__INCLUDED_)
#define AFX_STDAFX_H__8912783F_B35E_4F55_BF86_649735CBF74D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#define PARAM_FILE_NAME			"ClientInfo.ini"

#define SECTIONNAME_IP			"IPAddress"
#define SECTIONNAME_PORT		"PORT"
#define COMMON_KEY_NAME			"1"

#define BEGIN					1
#define REPEAT					2

#define FRAME_RECV				1
#define FRAME_SEND				2

#pragma warning (disable:4786)	

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__8912783F_B35E_4F55_BF86_649735CBF74D__INCLUDED_)
