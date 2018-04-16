//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanControllerDoc.cpp $
// @author:  Bart Golab
// @version: $Revision: #15 $
//
// Last modification: $DateTime: 2015/10/15 18:28:56 $
// Last modified by:  $Author: qi.huang $
//
// <description>

#include "stdafx.h"
#include "core/exceptions/src/TransactiveException.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "PlanManager.h"
#include "PlanControllerDoc.h"
#include "FloatingDocTemplate.h"
#include "PlanNode.h"
#include "PlanInstance.h"
#include "PlanAgentUpdateProcessor.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/MessageBox.h"
#include "app/response_plans/plan_editor_plugin/workflow_editor/src/WorkflowCmd.h"
#include "OperatorRights.h"
#include "PlanstepEditorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanControllerDoc

IMPLEMENT_DYNCREATE(CPlanControllerDoc, CPlanBrowserDoc)

BEGIN_MESSAGE_MAP(CPlanControllerDoc, CPlanBrowserDoc)
    // {{AFX_MSG_MAP(CPlanControllerDoc)
    ON_UPDATE_COMMAND_UI(ID_PLAN_EXECUTE, OnUpdatePlanControlCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_CUSTOMISE, OnUpdatePlanControlCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_STOP, OnUpdatePlanControlCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_PAUSE, OnUpdatePlanControlCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_RESUME, OnUpdatePlanControlCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_RESUME_SELECTED_STEP, OnUpdatePlanControlCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_TAKECONTROL, OnUpdatePlanControlCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_RELEASECONTROL, OnUpdatePlanControlCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_SCHEDULE, OnUpdatePlanControlCmd)
    ON_UPDATE_COMMAND_UI(ID_STEP_CUSTOMISE, OnUpdatePlanControlCmd)
    ON_UPDATE_COMMAND_UI_RANGE(ID_FLOWCHAR_ZOOM_IN, ID_FLOWCHAR_FIT_WINDOW, OnUpdateFlowCharEditorCmdUI)
    ON_COMMAND_RANGE(ID_FLOWCHAR_ZOOM_IN, ID_FLOWCHAR_FIT_WINDOW, OnFlowCharEditorCmd)
    ON_COMMAND(ID_PLAN_EXECUTE, OnStartPlan)
    ON_COMMAND(ID_PLAN_CUSTOMISE, OnCustomisePlan)
    ON_COMMAND(ID_PLAN_STOP, OnStopPlan)
    ON_COMMAND(ID_PLAN_PAUSE, OnPausePlan)
    ON_COMMAND(ID_PLAN_RESUME, OnResumePlan)
    ON_COMMAND(ID_PLAN_RESUME_SELECTED_STEP, OnResumePlan)
    ON_COMMAND(ID_PLAN_TAKECONTROL, OnTakeControlOfPlan)
    ON_COMMAND(ID_PLAN_RELEASECONTROL, OnReleaseControlOfPlan)
    ON_COMMAND(ID_PLAN_SCHEDULE, OnSchedulePlan)
    ON_COMMAND(ID_STEP_CUSTOMISE, OnCustomiseStep)
    ON_COMMAND(ID_FILE_EXIT, OnFileExit)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanControllerDoc construction/destruction

CPlanControllerDoc::CPlanControllerDoc()
{
    FUNCTION_ENTRY( "CPlanControllerDoc" );
    FUNCTION_EXIT;
}


CPlanControllerDoc::~CPlanControllerDoc()
{
    FUNCTION_ENTRY( "~CPlanControllerDoc" );
    FUNCTION_EXIT;
}


BOOL CPlanControllerDoc::OnNewDocument()
{
    FUNCTION_ENTRY( "OnNewDocument" );

    if (!CPlanBrowserDoc::OnNewDocument())
    {
        FUNCTION_EXIT;
        return FALSE;
    }

    CFloatingDocTemplate* docTemplate = static_cast<CFloatingDocTemplate*>(GetDocTemplate());
    SetCurrentNode(docTemplate->getNode());

    PlanAgentUpdateProcessor::getInstance().subscribeForActivePlanUpdates(this);
    PlanAgentUpdateProcessor::getInstance().subscribeForPlanCustomisationUpdates(this);
    PlanAgentUpdateProcessor::getInstance().subscribeForPlanConfigUpdates(this);  // Need to observe plan move updates for
                                                                                  // the title to be refreshed

    // Register for SessionId runtime parameter updates. Operator rights may change with the session.
    TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);

    FUNCTION_EXIT;
    return TRUE;
}


void CPlanControllerDoc::OnCloseDocument()
{
    FUNCTION_ENTRY( "OnCloseDocument" );

    PlanAgentUpdateProcessor::getInstance().unsubscribeFromActivePlanUpdates(this);
    PlanAgentUpdateProcessor::getInstance().unsubscribeFromPlanCustomisationUpdates(this);
    PlanAgentUpdateProcessor::getInstance().unsubscribeFromPlanConfigUpdates(this);

    TA_Base_Core::RunParams::getInstance().deregisterRunParamUser(this);

    CPlanBrowserDoc::OnCloseDocument();

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// CPlanControllerDoc commands

void CPlanControllerDoc::OnFileExit()
{
    FUNCTION_ENTRY( "OnFileExit" );

    // TODO: Add your command handler code here
    AfxGetMainWnd()->SendMessage(WM_CLOSE);

    FUNCTION_EXIT;
}


void CPlanControllerDoc::OnUpdatePlanControlCmd(CCmdUI* pCmdUI)
{
    FUNCTION_ENTRY( "OnUpdatePlanControlCmd" );

    PlanNode *planNode = static_cast<PlanNode *>(GetCurrentNode());
    TA_ASSERT(planNode, "Plan controller document does not have a valid plan node");

    try
    {
        switch (pCmdUI->m_nID)
        {
            case ID_PLAN_EXECUTE:
            case ID_PLAN_CUSTOMISE:
            case ID_PLAN_SCHEDULE:
                pCmdUI->Enable( planNode->isEnable( pCmdUI->m_nID ) ? TRUE : FALSE );
                break;
            case ID_STEP_CUSTOMISE:
                // disable if no step selected or if the selected step is the End Step
                if (!GetCurrentStep() || (( NULL != GetCurrentStep() ) && !GetCurrentStep()->editable()))
                {
                    pCmdUI->Enable(FALSE);
                    break;
                }
                // fall through to allow the instance to handle the update
            case ID_PLAN_RESUME_SELECTED_STEP:
            case ID_PLAN_STOP:
            case ID_PLAN_PAUSE:
            case ID_PLAN_RESUME:
            case ID_PLAN_TAKECONTROL:
            case ID_PLAN_RELEASECONTROL:
                if ( NULL != GetCurrentInstance() )
                {
                    GetCurrentInstance()->updateCmdUI(*pCmdUI, *planNode);
                    break;
                }
                // fall through to default

            default:
                pCmdUI->Enable(FALSE);
        }
    }
    catch (...)
    {
        pCmdUI->Enable(FALSE);
    }

    FUNCTION_EXIT;
}


void CPlanControllerDoc::OnStartPlan()
{
    FUNCTION_ENTRY( "OnStartPlan" );

    // TODO: Add your command handler code here
    try
    {
        PlanNode *planNode = static_cast<PlanNode *>(GetCurrentNode());
        TA_ASSERT(planNode, "Plan controller document does not have a valid plan node");

        planNode->startNewInstance();
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_START_NEW_INSTANCE, ex);
    }

    FUNCTION_EXIT;
}


void CPlanControllerDoc::OnCustomisePlan()
{
    FUNCTION_ENTRY( "OnCustomisePlan" );

    // TODO: Add your command handler code here
    try
    {
        PlanNode *planNode = static_cast<PlanNode *>(GetCurrentNode());
        TA_ASSERT(planNode, "Plan controller document does not have a valid plan node");

        planNode->customiseNewInstance();
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_CUSTOMISE_INSTANCE, ex);
    }

    FUNCTION_EXIT;
}


void CPlanControllerDoc::OnStopPlan()
{
    FUNCTION_ENTRY( "OnStopPlan" );

    // TODO: Add your command handler code here
    try
    {
        PlanInstanceSharedPtr planInstance = GetCurrentInstance();
        if (NULL != planInstance.get())
        {
            planInstance->stop();
        }
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_STOP_INSTANCE, ex);
    }

    FUNCTION_EXIT;
}


void CPlanControllerDoc::OnPausePlan()
{
    FUNCTION_ENTRY( "OnPausePlan" );

    // TODO: Add your command handler code here
    try
    {
        PlanInstanceSharedPtr planInstance = GetCurrentInstance();
        if (NULL != planInstance.get())
        {
            planInstance->pause();
        }
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_PAUSE_INSTANCE, ex);
    }

    FUNCTION_EXIT;
}


void CPlanControllerDoc::OnResumePlan()
{
    FUNCTION_ENTRY( "OnResumePlan" );

    // TODO: Add your command handler code here
    try
    {
        PlanInstanceSharedPtr planInstance = GetCurrentInstance();
        if (NULL != planInstance.get())
        {
            planInstance->resume();
        }
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_RESUME_INSTANCE, ex);
    }

    FUNCTION_EXIT;
}


void CPlanControllerDoc::OnTakeControlOfPlan()
{
    FUNCTION_ENTRY( "OnTakeControlOfPlan" );

    // TODO: Add your command handler code here
    try
    {
        PlanInstanceSharedPtr planInstance = GetCurrentInstance();
        if (NULL != planInstance.get())
        {
            planInstance->takeControl();
        }
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_TAKE_CONTROL_OF_INSTANCE, ex);
    }

    FUNCTION_EXIT;
}


void CPlanControllerDoc::OnReleaseControlOfPlan()
{
    FUNCTION_ENTRY( "OnReleaseControlOfPlan" );

    // TODO: Add your command handler code here
    try
    {
        PlanInstanceSharedPtr planInstance = GetCurrentInstance();
        if (NULL != planInstance.get())
        {
            planInstance->releaseControl();
        }
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_RELEASE_CONTROL_OF_INSTANCE, ex);
    }

    FUNCTION_EXIT;
}


void CPlanControllerDoc::OnSchedulePlan()
{
    FUNCTION_ENTRY( "OnSchedulePlan" );

    // TODO: Add your command handler code here
    try
    {
        PlanNode *planNode = static_cast<PlanNode *>(GetCurrentNode());
        TA_ASSERT(planNode, "Plan controller document does not have a valid plan node");

        planNode->schedulePlan();
    }
    catch (TA_Base_Core::TransactiveException&)
    {
        CString applicationName;
        applicationName.LoadString(IDS_SCHEDULING_MANAGER);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << applicationName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_010002);
    }

    FUNCTION_EXIT;
}


void CPlanControllerDoc::OnCustomiseStep()
{
    FUNCTION_ENTRY( "OnCustomiseStep" );

    // TODO: Add your command handler code here
    try
    {
        PlanStep *planStep = GetCurrentStep();
        TA_ASSERT(planStep, "Plan controller document does not have a valid plan step");

        PlanInstanceSharedPtr planInstance = GetCurrentInstance();

        if (NULL != planInstance.get())
        {
            TA_Base_Core::StepCustomisableDetail customStepDetail = planInstance->getCustomStepDetail(planStep);

			CPlanStepEditorDlg dlg(planStep, &customStepDetail);
			bool canCustomiseStep = (dlg.DoModal() == IDOK);
            if ( canCustomiseStep )
            {
                planInstance->customiseStep(customStepDetail);
            }
        }
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_CUSTOMISE_STEP, ex);
    }

    FUNCTION_EXIT;
}


bool CPlanControllerDoc::ControlEnabled() const
{
    FUNCTION_ENTRY( "ControlEnabled" );
    FUNCTION_EXIT;
    return true;
}

void CPlanControllerDoc::OnUpdateFlowCharEditorCmdUI( CCmdUI* pCmdUI )
{
	switch (pCmdUI->m_nID)
	{
		case ID_FLOWCHAR_ZOOM_IN:
		case ID_FLOWCHAR_ZOOM_OUT:
		case ID_FLOWCHAR_ZOOM_NORMAL:
		case ID_FLOWCHAR_FIT_WINDOW:
			pCmdUI->Enable(TRUE);
			break;
		default:
			break;
	}

}

void CPlanControllerDoc::OnFlowCharEditorCmd( UINT id )
{
    WorkflowCmd cmdType = eCmdUnkown;

    switch( id )
    {
        case ID_FLOWCHAR_ZOOM_IN:
            cmdType = eCmdZoomIn;
            break;
        case ID_FLOWCHAR_ZOOM_OUT:
            cmdType = eCmdZoomOut;
            break;
        case ID_FLOWCHAR_ZOOM_NORMAL:
            cmdType = eCmdZoomActual;
            break;
        case ID_FLOWCHAR_FIT_WINDOW:
            cmdType = eCmdFitToWin;
            break;
        default:
            break;
    }

    AfxGetMainWnd()->SendMessageToDescendants( WM_WORK_FLOW_EDITOR_CMD, static_cast<WPARAM>( cmdType ) );
}

