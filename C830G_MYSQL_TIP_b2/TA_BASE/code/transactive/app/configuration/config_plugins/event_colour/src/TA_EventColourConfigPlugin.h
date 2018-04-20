/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/configuration/config_plugins/event_colour/src/TA_EventColourConfigPlugin.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// TA_EventColourConfigPlugin.h : main header file for the TA_EVENTCOLOURCONFIGPLUGIN DLL
//

#if !defined(AFX_TA_EVENTCOLOURCONFIGPLUGIN_H__64F7B1CA_3543_435F_9B62_F42916EFE23B__INCLUDED_)
#define AFX_TA_EVENTCOLOURCONFIGPLUGIN_H__64F7B1CA_3543_435F_9B62_F42916EFE23B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTA_EventColourConfigPluginApp
// See TA_EventColourConfigPlugin.cpp for the implementation of this class
//

class CTA_EventColourConfigPluginApp : public CWinApp
{
public:
	CTA_EventColourConfigPluginApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTA_EventColourConfigPluginApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CTA_EventColourConfigPluginApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TA_EVENTCOLOURCONFIGPLUGIN_H__64F7B1CA_3543_435F_9B62_F42916EFE23B__INCLUDED_)
