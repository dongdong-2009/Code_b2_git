/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ats/ATSECSAgentSimulator/src/StdAfx.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__E8BCC1F6_5A94_4E91_BC63_A72E5D3DB209__INCLUDED_)
#define AFX_STDAFX_H__E8BCC1F6_5A94_4E91_BC63_A72E5D3DB209__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "resource.h"

#define WM_SETTEXT_TEMPERATURE  ( WM_APP + 0  )
#define WM_SETTEXT_STALL_TIME   ( WM_APP + 1  )
#define WM_SELECT_ECS_ZONE_ID   ( WM_APP + 2  )
#define WM_SETCHECK_EMERGENCY   ( WM_APP + 3  )
#define WM_UPDATE_LIST          ( WM_APP + 4  )

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__E8BCC1F6_5A94_4E91_BC63_A72E5D3DB209__INCLUDED_)
