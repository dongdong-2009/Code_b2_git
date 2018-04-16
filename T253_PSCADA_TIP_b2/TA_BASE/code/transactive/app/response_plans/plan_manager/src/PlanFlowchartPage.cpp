//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanFlowchartPage.cpp $
// @author:  Bart Golab
// @version: $Revision: #35 $
//
// Last modification: $DateTime: 2015/10/30 17:07:49 $
// Last modified by:  $Author: huang.wenbo $
//
// <description>

#include "StdAfx.h"

#ifdef _MSC_VER
  #pragma warning(disable: 4250) // inherits via dominance
#endif // _MSC_VER

#include "PlanManager.h"
#include "PlanBrowserDoc.h"
#include "PlanFlowchartPage.h"
#include "PlanManagerCommonDefs.h"
#include "PlanAgentAccess.h"
#include "PlanNode.h"
#include "OperatorRights.h"
#include "MftStepImplementationFactory.h"
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include "core/utilities/src/DebugUtil.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IStepComponent.h"
#include "app/response_plans/plan_editor_plugin/workflow_editor/src/WorkflowCmd.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/WorkflowActivity.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanFlowchartPage dialog


CPlanFlowchartPage::CPlanFlowchartPage() : CPlanDetailPage(CPlanFlowchartPage::IDD),
m_oWorkflowEditor(),
m_bViewMode( false )
{
    FUNCTION_ENTRY( "CPlanFlowchartPage" );

    // {{AFX_DATA_INIT(CPlanFlowchartPage)
        // NOTE: the ClassWizard will add member initialization here
    EditorStepFactory::instance();
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPlanFlowchartPage::~CPlanFlowchartPage()
{
    FUNCTION_ENTRY( "~CPlanFlowchartPage" );
    FUNCTION_EXIT;
}


void CPlanFlowchartPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanDetailPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanFlowchartPage)
    DDX_Control( pDX, IDC_ACTIVE_INSTANCE_COMBO, m_planInstanceCombo );
    DDX_Control( pDX, IDC_ACTIVE_INSTANCE_LABEL, m_planInstanceComboLabel );
    DDX_Control( pDX, IDC_ACTIVE_INSTANCE_LIST, m_planInstanceListCtrl );
    DDX_Control( pDX, IDC_PLAN_STATUS_GRPBOX, m_planStatusGroupBox );
    DDX_Control( pDX, IDC_ALL_OWNERS_CHECK, m_allOwnersCheck );
    DDX_Control( pDX, IDC_ALL_LOCATIONS_CHECK, m_allLocationsCheck );
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanFlowchartPage, CPlanDetailPage)
    // {{AFX_MSG_MAP(CPlanFlowchartPage)
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_CBN_SELCHANGE( IDC_ACTIVE_INSTANCE_COMBO, OnPlanInstanceComboItemSelected )
    ON_MESSAGE( WM_SELECT_ACTIVE_PLAN_INSTANCE, OnSelectActivePlanInstance )
    ON_MESSAGE( WM_ACTIVE_PLAN_RESET, OnActivePlanReset )
    ON_MESSAGE( WM_ACTIVE_PLAN_DETAIL_UPDATE, OnActivePlanDetailUpdate )
    ON_MESSAGE( WM_ACTIVE_STEP_DETAIL_UPDATE, OnActiveStepDetailUpdate )
    ON_MESSAGE( WM_PLAN_SAVING, OnPlanSaving )
    ON_MESSAGE( WM_REFRESH_PLAN_STEP_LIST, OnRefreshPlanSteps )
    ON_MESSAGE( WM_RESET_TREE_NODE_VIEW, OnResetTreeNodeView )
    ON_MESSAGE( WM_WORK_FLOW_EDITOR_CMD, OnWorkFlowEditorCmd )
    ON_MESSAGE( WM_WORK_FLOW_EDITOR_CMD_UI_UPDATE, OnWorkFlowEditorCmdUIUpdate )
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanFlowchartPage message handlers

BOOL CPlanFlowchartPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );
    
    m_bViewMode = !GetDocument()->EditingEnabled();
    
    m_oWorkflowEditor.getWorkflowForm().attachParent( this );

    m_oWorkflowEditor.setReadOnly( m_bViewMode );

    CPlanDetailPage::OnInitDialog();

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CPlanFlowchartPage::OnSetActive()
{
    FUNCTION_ENTRY( "OnSetActive" );

    if (CPlanDetailPage::OnSetActive())
    {
        refreshWorkflowGraph();

        FUNCTION_EXIT;
        return TRUE;
    }

    FUNCTION_EXIT;
    return FALSE;
}

BOOL CPlanFlowchartPage::OnKillActive()
{
    FUNCTION_ENTRY( "OnSetActive" );

    if ( CPlanDetailPage::OnKillActive() )
    {
        if ( !m_bViewMode )
        {
            std::map<unsigned int, ActivityData> mapTypeToData;
            std::map<unsigned int, SequenceFlowData> mapSequenceFlows;
            m_oWorkflowEditor.save( mapTypeToData, mapSequenceFlows );

            GetPlanNode()->updateGraph( mapTypeToData, mapSequenceFlows );
        }

        FUNCTION_EXIT;
        return TRUE;
    }

    FUNCTION_EXIT;
    return FALSE;
}

void CPlanFlowchartPage::OnDestroy()
{
    m_oWorkflowEditor.getWorkflowForm().detachParent();

    CPlanDetailPage::OnDestroy();
}

LRESULT CPlanFlowchartPage::OnWorkFlowEditorCmd( WPARAM wParam, LPARAM lParam )
{
    WorkflowCmd cmdType = static_cast<WorkflowCmd>(wParam);

	switch (cmdType)
	{
	case eCmdDelete:
	case eCmdCopy:
	case eCmdPaste:
		if(!m_bViewMode)
		{
			m_oWorkflowEditor.onCmd(cmdType);
		}
		break;
	case eCmdZoomIn:
	case eCmdZoomOut:
	case eCmdZoomActual:
	case eCmdFitToWin:
		m_oWorkflowEditor.onCmd(cmdType);
		break;
	default:
		break;
	}
	return 0;
}

LRESULT CPlanFlowchartPage::OnWorkFlowEditorCmdUIUpdate( WPARAM wParam, LPARAM lParam )
{
    WorkflowCmd cmdType = static_cast<WorkflowCmd>(wParam);
	CCmdUI* pCmdUi = reinterpret_cast<CCmdUI*>(lParam);
	if( NULL == pCmdUi )
	{
		return 0;
	}

	switch (cmdType)
	{
		case eCmdZoomIn:
		case eCmdZoomOut:
		case eCmdZoomActual:
		case eCmdFitToWin:
			pCmdUi->Enable(TRUE);
			break;
		default:
			pCmdUi->Enable((m_oWorkflowEditor.isCmdEnable(cmdType) && !m_bViewMode && OperatorRights::getInstance().canEditPlans()) ? TRUE : FALSE);
			break;
	}
	return 0;
}

LRESULT CPlanFlowchartPage::OnRefreshPlanSteps(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("OnRefreshPlanSteps");

    try
    {
        CWaitCursor waitCursor;

        //refreshWorkflowGraph();
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, typeid(ex).name(), ex.what() );
    }

    FUNCTION_EXIT;
    return 0;
}

void CPlanFlowchartPage::refreshWorkflowGraph()
{
    FUNCTION_ENTRY("refreshWorkflowGraph");

    std::map<unsigned int, ActivityData> mapTypeToData;
    std::map<unsigned int, SequenceFlowData> mapSequenceFlows;
    GetPlanNode()->retrieveGraph( mapTypeToData, mapSequenceFlows );

    m_oWorkflowEditor.load( mapTypeToData, mapSequenceFlows );

    if (m_bViewMode)
    {
        PlanInstanceSharedPtr currentInstance = GetDocument()->GetCurrentInstance();
        m_planInstanceCombo.SetSelectedInstance(currentInstance);

        refreshInstanceStatus();
    }

    FUNCTION_EXIT;
}

void CPlanFlowchartPage::refreshInstanceStatus()
{
    FUNCTION_ENTRY( "refreshInstanceStatus" );

    PlanInstanceId planInstanceId = m_planInstanceCombo.GetSelectedInstance();
    bool isActiveInstance = (planInstanceId.m_instance != 0 && planInstanceId.m_location != 0);

    PlanInstanceSharedPtr planInstance;

    if ( isActiveInstance )
    {
        planInstance = GetPlanNode()->getPlanInstance( planInstanceId );
    }

    GetDocument()->SetCurrentInstance( planInstance );

    try
    {
        if ( NULL != planInstance.get() )
        {
            planInstance->setSinglePlanInstanceList( m_planInstanceListCtrl );

            std::vector<TA_Base_Core::ActiveStepDetail> stepExecDetails;
            planInstance->getStepExecDetails( stepExecDetails );

            std::map<unsigned int, int> mapStepIdToState;
            for ( std::vector<TA_Base_Core::ActiveStepDetail>::iterator itLoop = stepExecDetails.begin(); itLoop != stepExecDetails.end(); ++itLoop )
            {
                mapStepIdToState.insert( std::map<unsigned int, int>::value_type( itLoop->id, toActivityState( itLoop->state ) ) );
            }

            m_oWorkflowEditor.updateActivitiesStatus( mapStepIdToState );
        }
        else
        {
            static PlanInstanceNone oNonePlanInstance;
            oNonePlanInstance.setSinglePlanInstanceList( m_planInstanceListCtrl );

            std::map<unsigned int, int> mapStepIdToState;
            m_oWorkflowEditor.updateActivitiesStatus( mapStepIdToState );
        }
    }
    catch ( ... )
    {
        if ( isActiveInstance )
        {
            m_planInstanceCombo.SetSelectedInstance( PlanInstanceSharedPtr() );
        }
    }

    FUNCTION_EXIT;
}

int CPlanFlowchartPage::toActivityState( const int nStepStatus )
{
    int nActivityStatus = eUnknown;

    switch ( nStepStatus )
    {
    case TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE:
        nActivityStatus = eSucceed;
        break;
    case TA_Base_Core::SKIPPED_ACTIVE_STEP_STATE:
        nActivityStatus = eSkipped;
        break;
    case TA_Base_Core::UNEXECUTED_ACTIVE_STEP_STATE:
        nActivityStatus = eUnknown;
        break;
    case TA_Base_Core::FAILED_ACTIVE_STEP_STATE:
        nActivityStatus = eFailed;
        break;
    case TA_Base_Core::PAUSED_ACTIVE_STEP_STATE:
        nActivityStatus = ePaused;
        break;
    case TA_Base_Core::DELAYED_ACTIVE_STEP_STATE:
    case TA_Base_Core::LOADED_ACTIVE_STEP_STATE:
    case TA_Base_Core::WAITING_ACTIVE_STEP_STATE:
        nActivityStatus = eRunning;
        break;
    default:
        nActivityStatus = eUnknown;
        break;
    }

    return nActivityStatus;
}

void CPlanFlowchartPage::SetPlanFilterButtonStates()
{
    FUNCTION_ENTRY( "SetPlanFilterButtonStates" );

    // no need to do anything here

    FUNCTION_EXIT;
}


void CPlanFlowchartPage::OnSize(UINT nType, int cx, int cy)
{
    FUNCTION_ENTRY( "OnSize" );

    CPlanDetailPage::OnSize(nType, cx, cy);

    int nWorkflowEditorTop = 0;

    if ( ::IsWindow( m_planStatusGroupBox.GetSafeHwnd() ))
    {
        PlanNode *planNode = GetPlanNode();

        CRect statusBoxRect;
        statusBoxRect.bottom = 0;

        if ( planNode->isApproved() )
        {
            m_planStatusGroupBox.GetWindowRect( statusBoxRect );
            ScreenToClient( statusBoxRect );
            statusBoxRect.right = cx - 8;
            m_planStatusGroupBox.MoveWindow( statusBoxRect );

            CRect instanceListRect;
            m_planInstanceListCtrl.GetWindowRect( instanceListRect );
            ScreenToClient( instanceListRect );
            instanceListRect.right = statusBoxRect.right - 13;
            m_planInstanceListCtrl.MoveWindow( instanceListRect );

            nWorkflowEditorTop = (statusBoxRect.Height() + 5);
        }
    }

    m_oWorkflowEditor.getWorkflowForm().moveInParent( 0, nWorkflowEditorTop, cx, cy - nWorkflowEditorTop );

    FUNCTION_EXIT;
}


void CPlanFlowchartPage::ShowRelevantControls()
{
    FUNCTION_ENTRY( "ShowRelevantControls" );

    int showActivePlanControls = GetPlanNode()->isApproved() ? SW_SHOW : SW_HIDE;

    m_planStatusGroupBox.ShowWindow( showActivePlanControls );
    m_allOwnersCheck.ShowWindow( showActivePlanControls );
    m_allLocationsCheck.ShowWindow( showActivePlanControls );
    m_planInstanceComboLabel.ShowWindow( showActivePlanControls );
    m_planInstanceCombo.ShowWindow( showActivePlanControls );
    m_planInstanceListCtrl.ShowWindow( showActivePlanControls );

    FUNCTION_EXIT;
}


void CPlanFlowchartPage::RefreshControlContents()
{
    FUNCTION_ENTRY( "RefreshControlContents" );

	unsigned long ulNodeId = GetPlanNode()->getNodeId();

    m_oWorkflowEditor.attachWorkflowOwner( *GetPlanNode() );
    m_oWorkflowEditor.setReadOnly( m_bViewMode );

    try
    {
        CWaitCursor waitCursor;

        PlanNode *planNode = GetPlanNode();
        PlanInstanceFilter *planInstanceFilter = GetPlanInstanceFilter();

        if (planNode->isApproved())
        {
            m_allOwnersCheck.SetCheck( planInstanceFilter->allOwnersNeeded() ? BST_CHECKED : BST_UNCHECKED );
            m_allLocationsCheck.SetCheck( planInstanceFilter->allLocationsNeeded() ? BST_CHECKED : BST_UNCHECKED );

            // Instance "None" is always added to the plan instance combo, regardless of any active instances
            m_planInstanceCombo.ResetContent();
            planNode->addNoneInstance(m_planInstanceCombo);

            planNode->updatePlanInstanceCombo(m_planInstanceCombo, *planInstanceFilter);
        }

        refreshWorkflowGraph();
    }
    catch ( ... )
    {
    }

    FUNCTION_EXIT;
}


void CPlanFlowchartPage::RefreshActiveInstances()
{
    FUNCTION_ENTRY( "RefreshActiveInstances" );

    PlanNode *planNode = GetPlanNode();
    PlanInstanceFilter *planInstanceFilter = GetPlanInstanceFilter();

    planNode->updatePlanInstanceCombo( m_planInstanceCombo, *planInstanceFilter );

    FUNCTION_EXIT;
}


void CPlanFlowchartPage::PurgeActiveInstances()
{
    FUNCTION_ENTRY( "PurgeActiveInstances" );

    // The purge may occur as the operator activates the page. The instance that may have
    // been selected on the previous page should also be selected in the active instances
    // combo to make sure that current instance selection is not reset during the purge.
    m_planInstanceCombo.SetSelectedInstance( GetDocument()->GetCurrentInstance() );
    m_planInstanceCombo.Purge();

    FUNCTION_EXIT;
}

void CPlanFlowchartPage::OnPlanInstanceComboItemSelected( )
{
    FUNCTION_ENTRY( "OnPlanInstanceComboItemSelected" );

    refreshInstanceStatus();

    FUNCTION_EXIT;
}

LRESULT CPlanFlowchartPage::OnPlanSaving( WPARAM wParam, LPARAM lParam )
{
    FUNCTION_ENTRY( "OnPlanSaving" );

    if ( !m_bViewMode )
    {
        std::map<unsigned int, SequenceFlowData> mapSequenceFlows;
        std::map<unsigned int, ActivityData> mapTypeToData;
        m_oWorkflowEditor.save( mapTypeToData, mapSequenceFlows );
        m_oWorkflowEditor.resetModified();

        GetPlanNode()->updateGraph( mapTypeToData, mapSequenceFlows );
    }

    FUNCTION_EXIT;
    return 0u;
}

LRESULT CPlanFlowchartPage::OnSelectActivePlanInstance( WPARAM wParam, LPARAM lParam )
{
    FUNCTION_ENTRY( "OnSelectActivePlanInstance" );

    PlanInstanceId planInstanceId( LOWORD( lParam ), HIWORD( lParam ) );
    PlanInstanceSharedPtr planInstance = GetPlanNode()->getPlanInstance( planInstanceId );

    if ( NULL != planInstance.get() )
    {
        // The instance to be selected may not be visible under the current filter. If that is the case,
        // the filter needs to be reset to enable the instance of interest to appear in the instance combo.
        if ( !planInstance->ownedByCurrentSession() )
        {
            GetPlanInstanceFilter()->setAllOwnersNeeded( true );
        }
        if ( !planInstance->isLocalInstance() )
        {
            GetPlanInstanceFilter()->setAllLocationsNeeded( true );
        }

        // Now check the instance filter buttons as appropriate. This will cause the instance combo
        // to be populated with any missing instances.
        SetPlanFilterButtonStates();

        // Finally select the specified instance.
        m_planInstanceCombo.SetSelectedInstance( planInstance );
    }

    FUNCTION_EXIT;
    return 0;
}

LRESULT CPlanFlowchartPage::OnActivePlanReset( WPARAM wParam, LPARAM lParam )
{
    FUNCTION_ENTRY( "OnActivePlanReset" );

    unsigned long ulLocationKey = static_cast<unsigned long>(wParam);

    m_planInstanceCombo.RemoveInstances( ulLocationKey );

    FUNCTION_EXIT;
    return 0;
}

LRESULT CPlanFlowchartPage::OnActivePlanDetailUpdate( WPARAM wParam, LPARAM lParam )
{
    FUNCTION_ENTRY( "OnActivePlanDetailUpdate" );

    TA_Base_Core::ActivePlanDetail *activePlanDetail = reinterpret_cast<TA_Base_Core::ActivePlanDetail *>(wParam);

    if ( NULL == activePlanDetail )
    {
        return 0u;
    }

    PlanInstanceId planInstanceId( activePlanDetail->plan.instance, activePlanDetail->plan.location );
    PlanInstanceSharedPtr planInstance = GetPlanNode()->getPlanInstance( planInstanceId );

    if ( NULL != planInstance.get() )
    {
        if ( m_planInstanceCombo.IsNewInstance( planInstance->getInstanceId() ) )
        {
            m_planInstanceCombo.AddInstance( planInstance->getActivePlanId().plan, planInstance->getInstanceId() );

            // Automatically select every new instance activated by this session
            if ( planInstance->ownedByCurrentSession() )
            {
                m_planInstanceCombo.SetSelectedInstance( planInstance );
            }
        }
        else
        {
            if ( planInstance->getInstanceId() == m_planInstanceCombo.GetSelectedInstance() )
            {
                m_planInstanceListCtrl.UpdateInstance( *activePlanDetail );
            }

            // If there's been a change in execution state, the instance may need repositioning within the instance combo.
            m_planInstanceCombo.RepositionInstance( planInstance );
        }
    }
    else // if planInstance == NULL (i.e. the instance has expired)
    {
        m_planInstanceCombo.RemoveInstance( planInstanceId );
    }

    FUNCTION_EXIT;
    return 0;
}

LRESULT CPlanFlowchartPage::OnActiveStepDetailUpdate( WPARAM wParam, LPARAM lParam )
{
    FUNCTION_ENTRY( "OnActivePlanDetailUpdate" );

    TA_Base_Core::PlanExecuteStepUpdate *activeStepDetail = reinterpret_cast<TA_Base_Core::PlanExecuteStepUpdate *>(wParam);

    if ( NULL == activeStepDetail )
    {
        return 0u;
    }

    PlanInstanceId planInstanceId( activeStepDetail->plan.instance, activeStepDetail->plan.location );
    PlanInstanceSharedPtr planInstance = GetPlanNode()->getPlanInstance( planInstanceId );

    if ( NULL != planInstance.get() )
    {
        if ( planInstance->getInstanceId() == m_planInstanceCombo.GetSelectedInstance() )
        {
            try
            {
                m_oWorkflowEditor.setActivityStatus( activeStepDetail->detail.id, toActivityState( activeStepDetail->detail.state ));
            }
            catch ( std::exception& expWhat )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
            }
            catch ( ... )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "Caught unknown exception when setActivityStatus" );
            }
        }
    }

    FUNCTION_EXIT;
    return 0;
}


