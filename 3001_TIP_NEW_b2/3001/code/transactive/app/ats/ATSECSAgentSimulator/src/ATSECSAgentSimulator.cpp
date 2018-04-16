/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/ATSECSAgentSimulator/src/ATSECSAgentSimulator.cpp $
  * @author:  
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/07/15 18:11:22 $
  * Last modified by:  $Author: qi.huang $
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

using TA_Base_Bus::TransActiveWinApp;

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

