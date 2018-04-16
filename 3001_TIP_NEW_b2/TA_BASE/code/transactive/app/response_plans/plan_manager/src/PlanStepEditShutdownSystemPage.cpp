//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditShutdownSystemPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/01/11 18:06:11 $
// Last modified by:  $Author: huangjian $
//
// <description>

#include "StdAfx.h"

#if defined (STEPTYPE_SHUTDOWN_SYSTEM)

#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "PlanManager.h"
#include "PlanStepEditShutdownSystemPage.h"
#include "ShutdownSystemStep.h"
#include "YesNoStepParameter.h"
#include "PlanNode.h"
#include "PlanManagerCommonDefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditShutdownSystemPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditShutdownSystemPage, CPlanStepEditParameterPage)

CPlanStepEditShutdownSystemPage::CPlanStepEditShutdownSystemPage(ShutdownSystemStep* step)
: CPlanStepEditParameterPage(CPlanStepEditShutdownSystemPage::IDD,step)
{
    FUNCTION_ENTRY( "CPlanStepEditShutdownSystemPage" );

    // {{AFX_DATA_INIT(CPlanStepEditShutdownSystemPage)
    m_nofityPeer = TRUE;
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPlanStepEditShutdownSystemPage::~CPlanStepEditShutdownSystemPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditShutdownSystemPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditShutdownSystemPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditShutdownSystemPage)

    DDX_Check(pDX, IDC_NOTIFY_PEER_CHECK, m_nofityPeer);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditShutdownSystemPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditShutdownSystemPage)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditShutdownSystemPage message handlers

BOOL CPlanStepEditShutdownSystemPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    if (!CPlanStepEditParameterPage::OnInitDialog())
    {
        FUNCTION_EXIT;
        return FALSE;
    }

    FUNCTION_EXIT;
    return TRUE;
}


void CPlanStepEditShutdownSystemPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    CWnd* notifyPeer = GetDlgItem(IDC_NOTIFY_PEER_CHECK);
    if (notifyPeer)
    {
        notifyPeer->EnableWindow(enable);
    }

    FUNCTION_EXIT;
}


void CPlanStepEditShutdownSystemPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    UpdateData();

    ShutdownSystemStep* step = dynamic_cast<ShutdownSystemStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the log step page");

    m_nofityPeer = step->getNotifyPeerParameter()->getAffirmative();

    UpdateData(FALSE);

    FUNCTION_EXIT;
}


void CPlanStepEditShutdownSystemPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    ShutdownSystemStep* step = dynamic_cast<ShutdownSystemStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the log step page");

    UpdateData();

    step->getNotifyPeerParameter()->setAffirmative(m_nofityPeer != 0);

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_SHUTDOWN_SYSTEM)
