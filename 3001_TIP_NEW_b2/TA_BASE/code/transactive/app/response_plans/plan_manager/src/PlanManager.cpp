//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanManager.cpp $
// @author:  Bart Golab
// @version: $Revision: #5 $
//
// Last modification: $DateTime: 2014/03/17 11:18:09 $
// Last modified by:  $Author: huangjian $
//
// <description>

#include "StdAfx.h"
#include "core/utilities/src/RunParams.h"
#include "PlanManager.h"
#include "PlanAgentUpdateProcessor.h"
#include "TreeNodeFactory.h"
#include "OperatorRights.h"
#include "PlanAgentAccess.h"
#include "FloatingWindowThreadRegister.h"
#include "PlanManagerGUI.h"
#include "app/response_plans/plan_manager/src/ProfileConfigAccess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;
using TA_Base_Bus::TransActiveWinApp;

/////////////////////////////////////////////////////////////////////////////
// The one and only CPlanManagerApp object

CPlanManagerApp planManagerApp;


/////////////////////////////////////////////////////////////////////////////
// CPlanManagerApp

BEGIN_MESSAGE_MAP(CPlanManagerApp, TransActiveWinApp)
    // {{AFX_MSG_MAP(CPlanManagerApp)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        // DO NOT EDIT what you see in these blocks of generated code!
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanManagerApp construction/destruction

CPlanManagerApp::CPlanManagerApp() : TransActiveWinApp(new PlanManagerGUI, "Plan Manager")
{
    FUNCTION_ENTRY( "CPlanManagerApp" );

    // TODO: add construction code here,
    // Place all significant initialization in InitInstance

    FUNCTION_EXIT;
}


CPlanManagerApp::~CPlanManagerApp()
{
    FUNCTION_ENTRY( "~CPlanManagerApp" );
    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// CPlanManagerApp initialization

BOOL CPlanManagerApp::InitInstance()
{
    FUNCTION_ENTRY( "InitInstance" );

    // Standard initialization
    AfxInitRichEdit();

    FUNCTION_EXIT;
    return TransActiveWinApp::InitInstance();
}


int CPlanManagerApp::ExitInstance()
{
    FUNCTION_ENTRY( "ExitInstance" );

    FloatingWindowThreadRegister::destroyInstance();
    TreeNodeFactory::destroyInstance();
    PlanAgentAccess::destroyInstance();
    PlanAgentUpdateProcessor::destroyInstance();
    OperatorRights::destroyInstance();
    ProfileConfigAccess::close();

    FUNCTION_EXIT;
    return TransActiveWinApp::ExitInstance();
}


