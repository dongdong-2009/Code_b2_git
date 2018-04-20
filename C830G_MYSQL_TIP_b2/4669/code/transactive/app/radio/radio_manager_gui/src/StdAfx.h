/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/StdAfx.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__AB68D999_4E17_11D7_B14B_0050BAB1D931__INCLUDED_)
#define AFX_STDAFX_H__AB68D999_4E17_11D7_B14B_0050BAB1D931__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*
#pragma message("This is for debug mode")
//#define RUN_IN_DEBUG_MODE
#define RUN_RIGHTS_IN_DEBUG_MODE
*/

//liqiang++
#pragma warning ( disable : 4244 )
#pragma warning ( disable : 4101 )
#pragma warning ( disable : 4800 )
//++liqiang

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#pragma warning ( disable : 4284 4786 )

#include "RadioManagerErrorHandler.h"


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__AB68D999_4E17_11D7_B14B_0050BAB1D931__INCLUDED_)
