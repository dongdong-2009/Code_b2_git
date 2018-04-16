/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL9_TIP/TA_BASE/transactive/app/scada/wfinhibitor/src/WFInhibitor.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides the base application class for the inspector panel.  It is implemented
  * as a managed process.
  */

#include "app/scada/WFInhibitor/src/stdafx.h"

#include "app/scada/WFInhibitor/src/WFInhibitor.h"
#include "app/scada/WFInhibitor/src/WFInhibitorDlg.h"
#include "app/scada/WFInhibitor/src/WFInhibitorGUI.h"

#include "core\utilities\src\DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::TransActiveWinApp;

namespace TA_Base_App
{
	/////////////////////////////////////////////////////////////////////////////
	// CWFInhibitorApp

	BEGIN_MESSAGE_MAP(CWFInhibitorApp, TransActiveWinApp)
		//{{AFX_MSG_MAP(CWFInhibitorApp)
			// NOTE - the ClassWizard will add and remove mapping macros here.
			//    DO NOT EDIT what you see in these blocks of generated code!
		//}}AFX_MSG
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CWFInhibitorApp construction

	CWFInhibitorApp::CWFInhibitorApp() : TransActiveWinApp( new WFInhibitorGUI, "WF Inhibitor" )
	{
	}

	/////////////////////////////////////////////////////////////////////////////
	// The one and only CWFInhibitorApp object

	CWFInhibitorApp theApp;
}
