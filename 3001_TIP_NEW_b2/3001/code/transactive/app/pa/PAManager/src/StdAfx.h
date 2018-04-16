/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/StdAfx.h $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/11/29 22:02:45 $
  * Last modified by:  $Author: qi.huang $
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


// "warning C4290: C++ Exception Specification ignored"
#pragma warning( disable : 4290 ) 


#pragma warning(disable:4284)   // For std::set warnings

#include <set>
#include <list>
#include <string>
#include <algorithm>

#include "core/utilities/src/DebugUtil.h"
#include "bus/pa_4669/pa_agent/IDL/src/IStationPaMethod.h"
#include "core/naming/src/NamedObject.h"
#include "app/pa/PAManager/src/resource.h"		// main symbols
#include "app/pa/PAManager/src/PaErrorHandler.h"

// wParam defines zone key
#define WM_PA_STN_ZONE_UPDATE			    ( WM_APP + 0  )
// wParam defines group key (use WM_PA_STN_HMI_ZONE_SEL_CHANGE for HMI group)
#define WM_PA_STN_ZONE_GROUP_UPDATE         ( WM_APP + 1  )
#define WM_PA_STN_HMI_ZONE_SEL_CHANGE		( WM_APP + 2  )
#define WM_PA_PRIORITY_SCHEME_UPDATE	    ( WM_APP + 3  )
#define WM_PA_STN_STATUS_CONFIG_UPDATE		( WM_APP + 4  )
#define WM_PA_STN_STATUS_REPORT_UPDATE		( WM_APP + 5  )
#define WM_PA_STN_STATUS_NAME_UPDATE        ( WM_APP + 6  )
#define WM_PA_DVA_MESSAGE_UPDATE            ( WM_APP + 7  )
#define WM_PA_DVA_VERSION_UPDATE            ( WM_APP + 8  )
#define WM_PA_SWITCH_SELECTED_DVA_MESSAGE   ( WM_APP + 9 )
#define WM_PA_ERROR_MESSAGE                 ( WM_APP + 12 )
#define WM_PA_HMI_DISPLAYSEL_CHANGED        ( WM_APP + 13 )
// Indicates there was a change in some right (non-specific)
#define WM_PA_RIGHTS_CHANGE                 ( WM_APP + 16 )
#define WM_PA_ATTEMPT_APPLICATION_RESTART   ( WM_APP + 17 )
#define WM_PA_MANAGER_ACTIVE   ( WM_APP + 20)

typedef std::set<unsigned long> DvaMessageUpdateData;
typedef DvaMessageUpdateData::iterator DvaMessageUpdateDataIt;

/// TD #3152
/// Defines origins of broadcasts within PA Manager application
/// - important so we know which broadcasts we are allowed to terminate
/// even if the privelege isn't present (for example, live broadcasts, 
/// dva playbacks / recording broadcasts all need to be terminated
/// locally, even if priveleges aren't present..)
enum EBroadcastOrigin
{
    BO_RECORDING_BROADCAST,
    BO_LIVE_BROADCAST,
    BO_PLAYBACK_BROADCAST,
    BO_UNKNOWN      // Valid for general purpose use, when we don't know where originated
};

const std::string   STATION_PA_AGENT_ENTITY_TYPE_NAME = "StationPAAgent";
const std::string   MASTER_PA_AGENT_ENTITY_TYPE_NAME = "MasterPAAgent";

#ifndef STUBBED_PA_AGENT

// Unit test classes define an agent stub class
typedef TA_Base_Core::NamedObject<TA_Base_Bus::IStationPaMethod,
	   					     TA_Base_Bus::IStationPaMethod_ptr, 
	           			     TA_Base_Bus::IStationPaMethod_var> PAAgentNamedObject;

#else

// Otherwise we're building the test code, so include stub directly
#include "app/pa/PAManager/test/src/PAAgentNamedObjectStub.h"

#endif

/**
 * castPointerToReference
 *
 * Casts the input pointer to an equivalent reference defined by template specification
 *
 * For exception Apple& apple = castPointerToReference<Apple>(pFruit);
 *
 * @param input the input pointer to perform the cast on
 *
 * @return reference to the casted object
 *
 * @exception bad_cast if the input pointer is null, or the cast is invalid
 *
 */
template <class TYPE, class ANY>
TYPE& castPointerToReference(ANY* input)
{
    if (0 == input)
    {
        TA_THROW(std::bad_cast("Attempting to cast null pointer to reference"));
    }

    TYPE* result = dynamic_cast<TYPE*>(input);
    
    if (0 == input)
    {
        TA_THROW(std::bad_cast("Invalid type cast"));
    }

    return *result;  
}


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




#endif // !defined(AFX_STDAFX_H__A42DB050_CE42_4317_833D_19C8503C691E__INCLUDED_)
