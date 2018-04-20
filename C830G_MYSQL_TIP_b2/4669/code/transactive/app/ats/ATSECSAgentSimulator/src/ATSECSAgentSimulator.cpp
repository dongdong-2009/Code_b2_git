/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ats/ATSECSAgentSimulator/src/ATSECSAgentSimulator.cpp $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  *
  */

#include "stdafx.h"
#include "app/ats/ATSECSAgentSimulator/src/ATSECSAgentSimulator.h"
#include "app/ats/ATSECSAgentSimulator/src/ATSECSAgentSimulatorDlg.h"
#include "app/ats/ATSECSAgentSimulator/src/ATSECSAgentSimulatorGUI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Bus::TransActiveWinApp;

/////////////////////////////////////////////////////////////////////////////
// CATSECSAgentSimulatorApp

BEGIN_MESSAGE_MAP(CATSECSAgentSimulatorApp, TransActiveWinApp)
	//{{AFX_MSG_MAP(CATSECSAgentSimulatorApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CATSECSAgentSimulatorApp construction

CATSECSAgentSimulatorApp::CATSECSAgentSimulatorApp()
 : TransActiveWinApp(new ATSECSAgentSimulatorGUI(), "ATS ECS Agent Simulator")
{
}

int CATSECSAgentSimulatorApp::ExitInstance()
{
    int retval = TransActiveWinApp::ExitInstance();

    return retval;
}


/////////////////////////////////////////////////////////////////////////////
// The one and only CATSECSAgentSimulatorApp object

CATSECSAgentSimulatorApp theApp;

