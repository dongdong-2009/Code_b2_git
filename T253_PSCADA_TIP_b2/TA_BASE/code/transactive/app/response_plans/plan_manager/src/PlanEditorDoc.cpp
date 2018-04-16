/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanEditorDoc.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #26 $
  *
  * Last modification: $DateTime: 2015/11/02 18:24:29 $
  * Last modified by:  $Author: qi.huang $
  *
  * Plan Editor GUI is based on an SDI. This class is a representation of the document associated with the
  * plan editor frame and its plan node view.
  *
  **/

#include "stdafx.h"

#include <set>

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "PlanManager.h"
#include "PlanEditorDoc.h"
#include "ViewUpdate.h"
#include "Timer.h"
#include "FloatingDocTemplate.h"
#include "PlanNode.h"
#include "NewPlanStepDlg.h"
#include "PlanStepPastePositionDlg.h"
#include "OperatorRights.h"
#include "PlanManagerCommonDefs.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/MessageBox.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_editor_plugin/workflow_editor/src/WorkflowCmd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanEditorDoc

IMPLEMENT_DYNCREATE( CPlanEditorDoc, CPlanBrowserDoc )

CPlanEditorDoc::CPlanEditorDoc() : m_copyStepAction( SCA_NO_ACTION )
{
    FUNCTION_ENTRY( "CPlanEditorDoc" );
    FUNCTION_EXIT;
}


BOOL CPlanEditorDoc::OnNewDocument()
{
    FUNCTION_ENTRY( "OnNewDocument" );

    CFloatingDocTemplate* floatDocTemplate = dynamic_cast<CFloatingDocTemplate*>( GetDocTemplate() );
    SetCurrentNode( floatDocTemplate->getNode() );

    if( !CDocument::OnNewDocument() )
    {
        FUNCTION_EXIT;
        return FALSE;
    }

    // Lock the plan for editing
    FUNCTION_EXIT;
    return TRUE;
}


void CPlanEditorDoc::SetCurrentNode( TreeNode* node )
{
    FUNCTION_ENTRY( "SetCurrentNode" );

    TA_ASSERT( node != NULL, "No node specified." );

    // Delete any existing plan copy
    delete GetCurrentNode();

    // Make a copy of the specified plan for editing
    PlanNode*   planNode = dynamic_cast<PlanNode*>( node );
    TA_ASSERT( planNode != NULL, "Not a plan node." );
    PlanNode*   planNodeCopy  = new PlanNode( *planNode );
    CPlanBrowserDoc::SetCurrentNode( planNodeCopy );

    FUNCTION_EXIT;
}


CPlanEditorDoc::~CPlanEditorDoc()
{
    FUNCTION_ENTRY( "~CPlanEditorDoc" );

    m_copyStepList.clear();

    delete GetCurrentNode();

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP( CPlanEditorDoc, CPlanBrowserDoc )
    // {{AFX_MSG_MAP(CPlanEditorDoc)
    ON_UPDATE_COMMAND_UI( ID_PLAN_SAVE, OnUpdatePlanSave )
	ON_UPDATE_COMMAND_UI( ID_EDITOR_UPGRADE, OnUpdatePlanUpgrade )
    ON_UPDATE_COMMAND_UI( ID_STEP_EDIT, OnUpdateCmdUI )
    ON_UPDATE_COMMAND_UI( ID_STEP_DELETE, OnUpdateCmdUI )
    ON_UPDATE_COMMAND_UI( ID_STEP_NEW, OnUpdateCmdUI )
    ON_UPDATE_COMMAND_UI( ID_STEP_COPY, OnUpdateCmdUI )
    ON_UPDATE_COMMAND_UI( ID_STEP_CUT, OnUpdateCmdUI )
    ON_UPDATE_COMMAND_UI( ID_STEP_PASTE, OnUpdateCmdUI )
    ON_COMMAND( ID_PLAN_SAVE, OnPlanSave )
    ON_COMMAND( ID_STEP_EDIT, OnStepEdit )
    ON_COMMAND( ID_STEP_DELETE, OnStepDelete )
    ON_COMMAND( ID_STEP_NEW, OnStepNew )
    ON_COMMAND( ID_STEP_COPY, OnStepCopy )
    ON_COMMAND( ID_STEP_CUT, OnStepCut )
    ON_COMMAND( ID_STEP_PASTE, OnStepPaste )
    ON_COMMAND( ID_FILE_EXIT, OnFileExit )
	ON_COMMAND( ID_EDITOR_UPGRADE, OnPlanUpgrade )
    ON_UPDATE_COMMAND_UI_RANGE( ID_FLOWCHAR_DELETE, ID_FLOWCHAR_PASTE, OnUpdateFlowCharEditorCmdUI )
    ON_COMMAND_RANGE( ID_FLOWCHAR_DELETE, ID_FLOWCHAR_PASTE, OnFlowCharEditorCmd )
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPlanEditorDoc::OnFileExit()
{
    FUNCTION_ENTRY( "OnFileExit" );

    AfxGetMainWnd()->SendMessage( WM_CLOSE );

    FUNCTION_EXIT;
}


void CPlanEditorDoc::OnPlanSave()
{
    FUNCTION_ENTRY( "OnPlanSave" );

    DoFileSave();

    FUNCTION_EXIT;
}


BOOL CPlanEditorDoc::DoFileSave()
{
    FUNCTION_ENTRY( "DoFileSave" );

    AfxGetMainWnd()->SendMessageToDescendants( WM_PLAN_SAVING );

    PlanNode*   planNode = dynamic_cast<PlanNode*>( GetCurrentNode() );
    TA_ASSERT( planNode != NULL, "Not a plan node." );

    try
    {
        CWaitCursor waitCursor;

        planNode->savePlan();

        RefreshTitle(); // TD#2555: Update plan name in title bar

        FUNCTION_EXIT;
        return TRUE;
    }
    catch( TA_Base_Core::TransactiveException& ex )
    {
        MessageBox::error( IDS_SAVE_PLAN, ex );
    }
    catch( ... )
    {
        MessageBox::error( IDS_SAVE_PLAN );
    }

    FUNCTION_EXIT;
    return FALSE;
}

void CPlanEditorDoc::OnPlanUpgrade()
{
	FUNCTION_ENTRY( "OnEditorUpgrade" );
  
    PlanNode* planNode = dynamic_cast<PlanNode*>( GetCurrentNode() );

    //Update database
    planNode->upgradePlan();

	//Notify plan node type change
    AfxGetMainWnd()->SendMessageToDescendants( WM_PLAN_NODE_TYPE_UPGRADE, 0, static_cast<LPARAM>(planNode->getNodeId()) );

	FUNCTION_EXIT;
}

void CPlanEditorDoc::OnUpdateCmdUI( CCmdUI* pCmdUI )
{
    FUNCTION_ENTRY( "OnUpdateCmdUI" );

    PlanStepList currentSteps = GetCurrentSteps();

    if( currentSteps.size() == 0 )
    {
        pCmdUI->Enable( FALSE );

        FUNCTION_EXIT;
        return;
    }

    bool bEnable = true;

    switch( pCmdUI->m_nID )
    {
        case ID_STEP_PASTE:
            bEnable &= ( m_copyStepAction != SCA_NO_ACTION );

        case ID_STEP_NEW:
        case ID_STEP_EDIT:
            bEnable &= ( currentSteps.size() == 1 );
    }

    for( PlanStepList::iterator iter = currentSteps.begin(); iter != currentSteps.end(); iter++ )
    {
        bEnable &= isEnable( *( *iter ), pCmdUI->m_nID );
    }

    pCmdUI->Enable( bEnable ? TRUE : FALSE );

    FUNCTION_EXIT;
}


void CPlanEditorDoc::OnUpdatePlanSave( CCmdUI* pCmdUI )
{
    FUNCTION_ENTRY( "OnUpdatePlanSave" );

    // Enable when plan is modified
    bool stepUpdated = false;
    PlanNode* planNode = dynamic_cast<PlanNode*>( GetCurrentNode() );
    if( NULL != planNode )
    {
        stepUpdated = planNode->isUpdated();
    }

    SetModifiedFlag( stepUpdated ? TRUE : FALSE );

    pCmdUI->Enable( stepUpdated && OperatorRights::getInstance().canEditPlans() );

    FUNCTION_EXIT;
}

void CPlanEditorDoc::OnUpdatePlanUpgrade( CCmdUI* pCmdUI )
{
	FUNCTION_ENTRY( "OnUpdateEditorUpgrade" );

	// Disable when plan is upgraded
	TA_Base_Core::ENode nodeType = TA_Base_Core::PLAN_NODE;
    bool planUpdated = false;
	PlanNode* planNode = dynamic_cast<PlanNode*>( GetCurrentNode() );
    if( NULL != planNode )
    {
        nodeType = planNode->getType();
        planUpdated = planNode->isUpdated();
    }

	pCmdUI->Enable(( TA_Base_Core::PLAN_NODE == nodeType ) && !planUpdated );

	FUNCTION_EXIT;
}

void CPlanEditorDoc::OnStepEdit()
{
    FUNCTION_ENTRY( "OnStepEdit" );

    try
    {
        // Start the step editor
        PlanStep* planStep = GetCurrentStep();
        if( planStep )
        {
            PlanNode* planNode = dynamic_cast<PlanNode*>( GetCurrentNode() );
            TA_ASSERT( planNode != NULL, "Not a plan node." );

            planNode->editPlanStep( planStep );
        }
    }
    catch( TA_Base_Core::TransactiveException& ex )
    {
        MessageBox::error( IDS_EDIT_PLAN_STEP, ex );
    }
    catch( ... )
    {
        MessageBox::error( IDS_EDIT_PLAN_STEP );
    }

    FUNCTION_EXIT;
}


void CPlanEditorDoc::OnStepNew()
{
    FUNCTION_ENTRY( "OnStepNew" );

    PlanNode* planNode = dynamic_cast<PlanNode*>( GetCurrentNode() );
    TA_ASSERT( planNode != NULL, "Not a plan node." );

    PlanStep* planStep = GetCurrentStep();
    TA_ASSERT( planStep != NULL, "Plan step is null" );

    CNewPlanStepDlg dlg( !planStep->hasIncoming(), !planStep->hasOutgoing() );

    if( dlg.DoModal() == IDOK )
    {
        // Get insert position
        CNewPlanStepDlg::EInsertPositionType insertPosition = dlg.getInsertPosition();
        PlanStepNumber stepNumber = ( insertPosition == CNewPlanStepDlg::IPT_INSERT_BEFORE ) ? planStep->getStepPosition() : planStep->getStepPosition() + 1;

        // Get step type
        int stepType = dlg.getStepType();

        try
        {
            CWaitCursor waitCursor;

            // Create the new step
            planNode->insertPlanStep( stepType, stepNumber );
        }
        catch( TA_Base_Core::TransactiveException&  ex )
        {
            MessageBox::error( IDS_CREATE_NEW_STEP, ex );
        }
    }

    FUNCTION_EXIT;
}


void CPlanEditorDoc::OnStepDelete()
{
    FUNCTION_ENTRY( "OnStepDelete" );

    PlanStepList currentSteps = GetCurrentSteps();

    TA_Base_Bus::TransActiveMessage userMsg;
    CString actionName;
    actionName.LoadString( ( currentSteps.size() == 1 ) ? IDS_DELETE_SINGLE_STEP : IDS_DELETE_MULTIPLE_STEPS );
    userMsg << actionName;
    UINT selectedButton = userMsg.showMsgBox( IDS_UW_010003 );

    // Check if the user really wishes to do this
    if( selectedButton == IDYES )
    {
        CWaitCursor waitCursor;

        // Delete the step(s)
        PlanNode* planNode = dynamic_cast<PlanNode*>( GetCurrentNode() );
        TA_ASSERT( planNode != NULL, "Not a plan node." );

        // Remove deleted step from copy step list
        if( !m_copyStepList.empty() )
        {
            std::set<PlanStep*> setCurSteps;
            for( PlanStepList::iterator itLoop = currentSteps.begin(); currentSteps.end() != itLoop; ++itLoop )
            {
                setCurSteps.insert( *itLoop );
            }

            for( PlanStepList::iterator itLoop = m_copyStepList.begin(); m_copyStepList.end() != itLoop; )
            {
                std::set<PlanStep*>::iterator itFound = setCurSteps.find( *itLoop );
                if( setCurSteps.end() == itFound )
                {
                    ++itLoop;
                    continue;
                }

                itLoop = m_copyStepList.erase( itLoop );

                if( m_copyStepList.empty() )
                {
                    m_copyStepAction = SCA_NO_ACTION;
                    break;
                }

                setCurSteps.erase( itFound );

                if( setCurSteps.empty() )
                {
                    break;
                }
            }

            setCurSteps.clear();
        }

        planNode->deletePlanSteps( currentSteps );
        TA_ASSERT( currentSteps.empty(), "Deleted step list is not empty" );
    };

    FUNCTION_EXIT;
}


void CPlanEditorDoc::OnStepCopy()
{
    FUNCTION_ENTRY( "OnStepCopy" );

    m_copyStepAction = SCA_COPY_STEP;
    m_copyStepList = GetCurrentSteps();

    FUNCTION_EXIT;
}


void CPlanEditorDoc::OnStepCut()
{
    FUNCTION_ENTRY( "OnStepCut" );

    m_copyStepAction = SCA_CUT_STEP;
    m_copyStepList = GetCurrentSteps();

    FUNCTION_EXIT;
}


void CPlanEditorDoc::OnStepPaste()
{
    FUNCTION_ENTRY( "OnStepPaste" );

    if( m_copyStepList.empty() )
    {
        m_copyStepAction = SCA_NO_ACTION;
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", "No plan has been copied or cut - cannot paste" );

        return;
    }

    if( m_copyStepAction != SCA_CUT_STEP && m_copyStepAction != SCA_COPY_STEP )
    {
        m_copyStepAction = SCA_NO_ACTION;
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid paste action %d", m_copyStepAction );

        return;
    }

    PlanStep* planStep = GetCurrentStep();

    CPlanStepPastePositionDlg dlg( !planStep->hasIncoming(), !planStep->hasOutgoing() );

    if( dlg.DoModal() == IDOK )
    {
        // Get paste position
        CPlanStepPastePositionDlg::EPastePositionType pastePosition = dlg.getPastePosition();
        PlanStepNumber stepNumber = ( pastePosition == CPlanStepPastePositionDlg::PPT_PASTE_BEFORE ) ?
                                    planStep->getStepPosition() : planStep->getStepPosition() + 1;

        try
        {
            CWaitCursor waitCursor;

            PlanNode* planNode = dynamic_cast<PlanNode*>( GetCurrentNode() );
            TA_ASSERT( planNode != NULL, "Not a plan node." );

            if( m_copyStepAction == SCA_CUT_STEP )
            {
                // Shift the step(s) to their new position
                planNode->movePlanSteps( m_copyStepList, stepNumber );
                m_copyStepAction = SCA_NO_ACTION; // Can paste only once for any given cut.
                m_copyStepList.clear();
            }
            else if( m_copyStepAction == SCA_COPY_STEP )
            {
                // Duplicate the step(s)
                planNode->copyPlanSteps( m_copyStepList, stepNumber );
            }
        }
        catch( TA_Base_Core::TransactiveException&  ex )
        {
            MessageBox::error( IDS_PASTE_STEP, ex );
        }
    }

    FUNCTION_EXIT;
}

bool CPlanEditorDoc::EditingEnabled() const
{
    FUNCTION_ENTRY( "EditingEnabled" );
    FUNCTION_EXIT;
    return true;
}

bool CPlanEditorDoc::isEnable( PlanStep& planStep, const int nAction )
{
    FUNCTION_ENTRY( "isEnable" );

    bool bEnable = false;

    switch( nAction )
    {
        case ID_STEP_NEW:
        case ID_STEP_PASTE:  // Plan will disable if no step copied or cut.
            bEnable = OperatorRights::getInstance().canEditPlans();
            break;

        case ID_STEP_EDIT:
        case ID_STEP_COPY:   // Cannot cut or copy the end step!
        case ID_STEP_CUT:
        case ID_STEP_DELETE:
            bEnable = ( planStep.editable() && OperatorRights::getInstance().canEditPlans() );
            break;

        default:
            bEnable = false;
    }

    FUNCTION_EXIT;
    return bEnable;
}

void CPlanEditorDoc::OnUpdateFlowCharEditorCmdUI( CCmdUI* pCmdUI )
{
    WorkflowCmd cmdType = eCmdUnkown;
    switch( pCmdUI->m_nID )
    {
        case ID_FLOWCHAR_DELETE:
            cmdType = eCmdDelete;
            break;
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
        case ID_FLOWCHAR_COPY:
            cmdType = eCmdCopy;
            break;
        case ID_FLOWCHAR_PASTE:
            cmdType = eCmdPaste;
            break;
        default:
            break;
    }

	AfxGetMainWnd()->SendMessageToDescendants( WM_WORK_FLOW_EDITOR_CMD_UI_UPDATE, static_cast<WPARAM>(cmdType), reinterpret_cast<LPARAM>(pCmdUI) );
}

void CPlanEditorDoc::OnFlowCharEditorCmd( UINT id )
{
    WorkflowCmd cmdType = eCmdUnkown;

    switch( id )
    {
        case ID_FLOWCHAR_DELETE:
            cmdType = eCmdDelete;
            break;
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
        case ID_FLOWCHAR_COPY:
            cmdType = eCmdCopy;
            break;
        case ID_FLOWCHAR_PASTE:
            cmdType = eCmdPaste;
            break;
        default:
            break;
    }

    AfxGetMainWnd()->SendMessageToDescendants( WM_WORK_FLOW_EDITOR_CMD, static_cast<WPARAM>( cmdType ));
}
