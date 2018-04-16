/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/maintenance_management/job_request_gui/src/JobRequestMgr.cpp $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  *
  */

#include "stdafx.h"
#include "JobRequestMgr.h"
#include "JobRequestMgrDlg.h"
#include "JobRequestMgrGUI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::TransActiveWinApp;

namespace TA_IRS_App
{

	// CJobRequestMgrApp

	BEGIN_MESSAGE_MAP(CJobRequestMgrApp, TransActiveWinApp)
		//{{AFX_MSG_MAP(CJobRequestMgrApp)
			// NOTE - the ClassWizard will add and remove mapping macros here.
			//    DO NOT EDIT what you see in these blocks of generated code!
		//}}AFX_MSG
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CJobRequestMgrApp construction

	CJobRequestMgrApp::CJobRequestMgrApp() : TransActiveWinApp(new JobRequestMgrGUI(), "Service Request Manager")
	{
		// TODO: add construction code here,
		// Place all significant initialization in InitInstance
	}

	/////////////////////////////////////////////////////////////////////////////
	// The one and only CJobRequestMgrApp object

	CJobRequestMgrApp theApp;
} //end namespace TA_IRS_App
