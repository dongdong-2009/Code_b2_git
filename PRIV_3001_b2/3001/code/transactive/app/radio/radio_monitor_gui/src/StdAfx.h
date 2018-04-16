/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/radio/radio_monitor_gui/src/StdAfx.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// stdafx.h : include file for standard system include files (for PAManager App)
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A42DB050_CE42_4317_833D_19C8503C691E__INCLUDED_)
#define AFX_STDAFX_H__A42DB050_CE42_4317_833D_19C8503C691E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#pragma warning( disable : 4786 )
#pragma warning( disable : 4290 )   // "warning C4290: C++ Exception Specification ignored"
#pragma warning( disable : 4284 )   // For std::set warnings
#pragma warning( disable : 4250 )   // inherits via dominance

#include <list>
#include <string>
#include <algorithm>

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/naming/src/NamedObject.h"

#include "app/radio/radio_monitor_gui/src/RadioMonitorTypes.h"
#include "app/radio/radio_monitor_gui/src/RadioMonitorErrorHandler.h"
#include "app/radio/radio_monitor_gui/src/CachedConfig.h"

#define WM_RM_RIGHTS_CHANGE                 ( WM_APP + 0 )
#define WM_RM_ERROR_MESSAGE                 ( WM_APP + 1 )
#define WM_RM_MONITOR_CALL_UPDATE           ( WM_APP + 2 )
#define WM_RM_LISTEN_CALL_UPDATE            ( WM_APP + 3 )
#define TRAIN_INBOUND_MSG                   ( WM_APP + 4 )
#define TRAIN_OUTBOUND_MSG                  ( WM_APP + 5 )
#define TRAIN_UPDATE_SELECTION_MSG          ( WM_APP + 6 )
#define WM_RM_LISTEN_CALL_STATUS_UPDATE		( WM_APP + 7 )


/////////////////////////////
// Helper template function
template <class ArrayType> 
void deleteAllItemsInArray(ArrayType& array)
{
    typename
    ArrayType::iterator it = array.begin(); 
    for (; it != array.end(); ++it)
    {
        delete (*it);
    }

    array.clear();
}
/////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A42DB050_CE42_4317_833D_19C8503C691E__INCLUDED_)
