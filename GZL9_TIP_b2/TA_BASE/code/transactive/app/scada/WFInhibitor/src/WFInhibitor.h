/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL9_TIP/TA_BASE/transactive/app/scada/wf_inhibitor/src/WFInhibitor.h $
  * @author:  Liu Yong
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/07/05 10:04:52 $
  * Last modified by:  $Author: jinmin.huang $
  * 
  * This class provides the base application class for the inspector panel.  It is implemented
  * as a managed process.
  */

#ifndef WF_INHIBITOR_H
#define WF_INHIBITOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "app/scada/WFInhibitor/src/resource.h"		// main symbols
#include "app/scada/WFInhibitor/src/WFInhibitorGUI.h"

#include "bus/generic_gui/src/TransActiveWinApp.h"



namespace TA_Base_App
{
	/////////////////////////////////////////////////////////////////////////////
	// CWFInhibitorApp:
	// See WFInhibitor.cpp for the implementation of this class
	//

	class CWFInhibitorApp : public TA_Base_Bus::TransActiveWinApp
	{
	public:
		CWFInhibitorApp();
		virtual ~CWFInhibitorApp() {};

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CWFInhibitorApp)
		public:
		//}}AFX_VIRTUAL

	// Implementation

		//{{AFX_MSG(CWFInhibitorApp)
			// NOTE - the ClassWizard will add and remove member functions here.
			//    DO NOT EDIT what you see in these blocks of generated code !
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	};


	/////////////////////////////////////////////////////////////////////////////

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}

#endif // ifndef WF_INHIBITOR_H
