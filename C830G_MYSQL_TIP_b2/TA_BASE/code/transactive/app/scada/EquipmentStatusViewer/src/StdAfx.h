/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/StdAfx.h $
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

#if !defined(AFX_STDAFX_H__E8BCC1F6_5A94_4E91_BC63_A72E5D3DB209__INCLUDED_)
#define AFX_STDAFX_H__E8BCC1F6_5A94_4E91_BC63_A72E5D3DB209__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#define NOMINMAX            // disable windows.h defining min and max as macros
#include "boost/config.hpp" // include boosts compiler-specific "fixes"
//using std::min;             // make them globally available
//using std::max;

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

namespace TA_Base_Bus
{
    class DataPointProxySmartPtr;
    class DataNodeProxySmartPtr;
}

#include <map>
#include <vector>

namespace TA_Base_App
{
    typedef std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr> DataPointProxyMap;
    typedef std::map<unsigned long, TA_Base_Bus::DataNodeProxySmartPtr> DataNodeProxyMap;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__E8BCC1F6_5A94_4E91_BC63_A72E5D3DB209__INCLUDED_)
