//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditShutdownSystemPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#include "StdAfx.h"
#include "resource.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"
#include "PlanStepEditShutdownSystemPage.h"
#include "ShutdownSystemStep.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditShutdownSystemPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditShutdownSystemPage, CPlanStepEditParameterPage)

CPlanStepEditShutdownSystemPage::CPlanStepEditShutdownSystemPage(ShutdownSystemStep* step)
: CPlanStepEditParameterPage(IDD_PLAN_STEP_EDIT_SHUTDOWN_SYSTEM_DLG,step)
{
    FUNCTION_ENTRY( "CPlanStepEditShutdownSystemPage" );
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
    DDX_Control(pDX, IDC_NOTIFY_PEER_CHECK, m_nofityPeer);
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

    m_nofityPeer.EnableWindow(enable);

    FUNCTION_EXIT;
}


void CPlanStepEditShutdownSystemPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

	fillCheckbox( ParamNotifyPeer, m_nofityPeer );

    FUNCTION_EXIT;
}


void CPlanStepEditShutdownSystemPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

	updateBooleanValue( ParamNotifyPeer, m_nofityPeer );

    FUNCTION_EXIT;
}

}
