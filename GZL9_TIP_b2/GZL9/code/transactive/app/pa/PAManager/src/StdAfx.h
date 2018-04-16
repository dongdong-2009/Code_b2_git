/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
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

#pragma warning(disable:4005)   // For std::set warnings

#include <list>
#include <string>
#include <algorithm>
#include <set>
#include <sstream>

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "app/pa/PAManager/src/resource.h"		// main symbols

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
