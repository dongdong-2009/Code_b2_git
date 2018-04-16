/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanNode.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #70 $
  *
  * Last modification: $DateTime: 2016/01/28 15:17:03 $
  * Last modified by:  $Author: xiaoxia.li $
  *
  * This is a representation of a plan node within the plan tree.
  *
  **/

#include "StdAfx.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/generic_gui/src/applauncher.h"
#include "bus/application_types/src/apptypes.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "Resource.h"
#include "PlanNode.h"
#include "RootNode.h"
#include "TreeNodeFactory.h"
#include "PlanInstance.h"
#include "PlanInstanceMap.h"
#include "SequenceFlow.h"
#include "SequenceFlowMap.h"
#include "PlanStepMap.h"
#include "PlanAgentAccess.h"
#include "PlanNodeView.h"
#include "Utilities.h"
#include "LabelMaker.h"
#include "OperatorRights.h"
#include "PlanManagerCommonDefs.h"
#include "PlanInstanceFilter.h"
#include "PlanGeneralDetailPage.h"
#include "PlanStepDetailPage.h"
#include "PlanEditorThread.h"
#include "PlanControllerThread.h"
#include "FloatingWindowThreadRegister.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/MessageBox.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include "PlanAgentAccessException.h"
#include "PlanStepEditorDlg.h"


namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// PlanNode

PlanNode::PlanNode( const TA_Base_Core::NodeSummary& nodeSummary ) :
TreeNode(nodeSummary),
m_isCopiedPlan(false),
m_isInfoUpdated(false),
m_areStepsUpdated(false),
m_localPlanInstanceMap(NULL),
m_remotePlanInstanceMap(NULL),
m_planStepMap(NULL),
m_sequenceFlowMap(NULL)
{
    FUNCTION_ENTRY( "PlanNode" );
    FUNCTION_EXIT;
}


PlanNode::PlanNode(const PlanNode& planNode) :
TreeNode(const_cast<PlanNode&>(planNode).getNodeSummary()),
m_isCopiedPlan(true),
m_isInfoUpdated(false),
m_areStepsUpdated(false),
m_localPlanInstanceMap(NULL),
m_remotePlanInstanceMap(NULL),
m_planStepMap(NULL),
m_sequenceFlowMap(NULL)
{
    FUNCTION_ENTRY( "PlanNode" );
    FUNCTION_EXIT;
}


PlanNode::~PlanNode()
{
    FUNCTION_ENTRY( "~PlanNode" );

    delete m_localPlanInstanceMap;
    delete m_remotePlanInstanceMap;
    delete m_planStepMap;
    delete m_sequenceFlowMap;

    FUNCTION_EXIT;
}


const std::string& PlanNode::getNodeType()
{
    FUNCTION_ENTRY( "getNodeType" );

    static const std::string PLAN_NODE_TYPE = (LPCTSTR) LabelMaker::getPlanNodeTypeLabel();

    FUNCTION_EXIT;
    return PLAN_NODE_TYPE;
}


const std::string& PlanNode::getNodeStatus()
{
    FUNCTION_ENTRY( "getNodeStatus" );

    static const std::string APPROVED_PLAN_STATUS = Utilities::convertToString(TA_Base_Core::APPROVED_APPROVAL_STATE);
    static const std::string UNAPPROVED_PLAN_STATUS = Utilities::convertToString(TA_Base_Core::UNAPPROVED_APPROVAL_STATE);

    FUNCTION_EXIT;
    return (isApproved()? APPROVED_PLAN_STATUS : UNAPPROVED_PLAN_STATUS);
}


PlanInstanceSharedPtr PlanNode::getPlanInstance(const PlanInstanceId &instanceId)
{
    FUNCTION_ENTRY( "getPlanInstance" );

    // This method may be called in the course of processing a Plan Controller launch request from
    // the Plan Agent. The agent identifies the instance of interest, however, it may be one that
    // it created moments earlier and it's possible that the Plan Manager has not yet received an
    // active plan update for it.
    // 
    // The procedure therefore is to first look up the local plan instance store. If the instance
    // cannot be found, we refresh the store contents by retrieving all the active instances for
    // the plan from the agent.

    PlanInstanceSharedPtr instance = getPlanInstanceMap(instanceId.m_location).getPlanInstance(instanceId);

    if (NULL == instance.get())
    {
        try
        {
            TA_Base_Core::ActivePlanDetails_var activePlanDetails =
                PlanAgentAccess::getInstance().getActivePlanDetailsAtLocation(getNodeId(), instanceId.m_location);

            getPlanInstanceMap(instanceId.m_location).addPlanInstances(activePlanDetails);
        }
        catch (TA_Base_Core::TransactiveException &ex)
        {
            MessageBox::error(IDS_RETRIEVE_PLAN, ex);
        }

        instance = getPlanInstanceMap(instanceId.m_location).getPlanInstance(instanceId);
    }

    FUNCTION_EXIT;
    return instance;
}


CRuntimeClass *PlanNode::getNodeViewClass()
{
    FUNCTION_ENTRY( "getNodeViewClass" );
    FUNCTION_EXIT;
    return RUNTIME_CLASS(CPlanNodeView);
}


bool PlanNode::isEnable(const int nAction)
{
    FUNCTION_ENTRY( "isEnable" );

    bool bEnable = false;

    switch (nAction)
    {
        case ID_PLAN_SCHEDULE:
            bEnable = isApproved() && OperatorRights::getInstance().canSchedulePlans();
            break;

        case ID_PLAN_EXECUTE:
        case ID_PLAN_CONTROL:
            bEnable = isApproved() && OperatorRights::getInstance().canControlPlans();
            break;

        case ID_PLAN_CUSTOMISE:
            bEnable = isApproved() && OperatorRights::getInstance().canCustomisePlans();
            break;

        case ID_PLAN_APPROVE:
            bEnable = !isApproved() && OperatorRights::getInstance().canApprovePlans();
            break;

        case ID_PLAN_UNAPPROVE:
            bEnable = isApproved() && OperatorRights::getInstance().canApprovePlans();
            break;

        case ID_PLAN_EDIT:
        case ID_PLAN_CUT:
        case ID_PLAN_DELETE:
        case ID_CATEGORY_OR_PLAN_DELETE:
            bEnable = !isApproved() && OperatorRights::getInstance().canEditPlans();
            break;

        case ID_PLAN_NEW:
        case ID_PLAN_COPY:
        case ID_PLAN_PASTE:
            bEnable = OperatorRights::getInstance().canEditPlans();
            break;

        case ID_CATEGORY_NEW:
        case ID_EXPORT_PLAN:
              bEnable = OperatorRights::getInstance().canEditCategories();
            break;
        case ID_PLAN_VISIBLE:
            bEnable = !isVisible();
            break;
        case ID_PLAN_INVISIBLE:
            bEnable = isVisible();
            break;
        default:
            bEnable = false;
    }

    FUNCTION_EXIT;
    return bEnable;
}


ETreeNodeImageType PlanNode::getNodeImageType(bool isExpanded)
{
    FUNCTION_ENTRY( "getNodeImageType" );

    TA_ASSERT(!isExpanded, "Plan nodes cannot be expanded");

    FUNCTION_EXIT;
    return (isApproved()? TNIT_APPROVED_PLAN : TNIT_UNAPPROVED_PLAN);
}


void PlanNode::addChildrenToTree(CTreeCtrl &treeCtrl, HTREEITEM parent, ITreeNodeFilter *filter)
{
    FUNCTION_ENTRY( "addChildrenToTree" );

    TA_ASSERT(false, "Plan nodes do not have children");

    FUNCTION_EXIT;
}


void PlanNode::addChildrenToList(CListCtrl &listCtrl, ITreeNodeFilter *filter)
{
    FUNCTION_ENTRY( "addChildrenToList" );

    TA_ASSERT(false, "Plan nodes do not have children");

    FUNCTION_EXIT;
}


void PlanNode::renameNode(std::string nodeName)
{
    FUNCTION_ENTRY( "renameNode" );

    TA_Base_Core::PlanDetail detail = getPlanDetail();
    std::string oldName = detail.name.in();
    std::string newName = nodeName;

    // as per TD 14880
    if (oldName.compare(newName) == 0)
    {
        FUNCTION_EXIT;
        return;
    }

    detail.name = nodeName.c_str();

    PlanAgentAccess::getInstance().updatePlanDetail( detail );

    // Reset node information used in updating plan tree items. The agent will be requried and should respond
    // with the new name. This means that after the name of a plan has been edited in the tree it will not reset
    // to its old value before a plan change notification comes back from the agent.
    invalidateNodeName();
    invalidateNodeSummary();
    invalidateNodeDetail();

    FUNCTION_EXIT;
}


void PlanNode::updatePlanView(CPlanGeneralDetailPage &generalDetailPage, const PlanInstanceFilter &filter)
{
    FUNCTION_ENTRY( "updatePlanView" );

    generalDetailPage.m_nameEdit.SetWindowText(_T(getPlanDetail().name));
    generalDetailPage.m_descriptionEdit.SetWindowText(_T(getPlanDetail().description));
    generalDetailPage.m_approvalStatusEdit.SetWindowText(_T(Utilities::convertToString(getPlanDetail().approvalState).c_str()));
    generalDetailPage.m_planModByEdit.SetWindowText(_T(getPlanDetail().modifiedBy));
    generalDetailPage.m_planModTimeEdit.SetWindowText(_T(getPlanDetail().modifiedTime));
    generalDetailPage.m_approvalModByEdit.SetWindowText(_T(getPlanDetail().approvalChangedBy));
    generalDetailPage.m_approvalModTimeEdit.SetWindowText(_T(getPlanDetail().approvalChangedTime));

    if (isApproved())
    {
        generalDetailPage.m_allOwnersCheck.SetCheck(filter.allOwnersNeeded()? BST_CHECKED : BST_UNCHECKED);
        generalDetailPage.m_allLocationsCheck.SetCheck(filter.allLocationsNeeded()? BST_CHECKED : BST_UNCHECKED);

        generalDetailPage.m_planInstanceListCtrl.DeleteAllItems();
        updatePlanInstanceList(generalDetailPage.m_planInstanceListCtrl, filter);
    }

    FUNCTION_EXIT;
}


void PlanNode::updatePlanView(CPlanStepDetailPage &stepDetailPage, const PlanInstanceFilter &filter)
{
    FUNCTION_ENTRY( "updatePlanView" );

    stepDetailPage.m_stepParametersListCtrl.DeleteAllItems();

    addStepsToStepList(stepDetailPage.m_planStepsListCtrl);

    if (isApproved())
    {
        stepDetailPage.m_allOwnersCheck.SetCheck(filter.allOwnersNeeded()? BST_CHECKED : BST_UNCHECKED);
        stepDetailPage.m_allLocationsCheck.SetCheck(filter.allLocationsNeeded()? BST_CHECKED : BST_UNCHECKED);

        // Instance "None" is always added to the plan instance combo, regardless of any active instances
        stepDetailPage.m_planInstanceCombo.ResetContent();
        addNoneInstance( stepDetailPage.m_planInstanceCombo );

        updatePlanInstanceCombo(stepDetailPage.m_planInstanceCombo, filter);
    }

    FUNCTION_EXIT;
}


void PlanNode::updatePlanInstanceList(CPlanInstanceListCtrl &instanceListCtrl, const PlanInstanceFilter &filter)
{
    FUNCTION_ENTRY( "updatePlanInstanceList" );

    if (isApproved())
    {
        PlanInstanceList planInstances;
        getLocalPlanInstanceMap().getPlanInstances(planInstances);

        // If plans at all locations are required
        if (filter.allLocationsNeeded())
        {
            PlanInstanceList remotePlanInstances;
            getRemotePlanInstanceMap().getPlanInstances(remotePlanInstances);

            planInstances.insert(planInstances.end(), remotePlanInstances.begin(), remotePlanInstances.end());
        }

        instanceListCtrl.SetItemCount(planInstances.size());

        for (PlanInstanceList::const_iterator iiter = planInstances.begin(); iiter != planInstances.end(); iiter++)
        {
            PlanInstanceSharedPtr planInstance = *iiter;

            if ( !TreeNodeFactory::getInstance().canTreeNodeBeShowed(planInstance->getActivePlanId().plan) )
            {
                continue;
            }
            planInstance->addToPlanInstanceList(instanceListCtrl);
        }
    }

    FUNCTION_EXIT;
}


void PlanNode::updatePlanInstanceCombo(CPlanInstanceComboBox &instanceComboBox, const PlanInstanceFilter &filter)
{
    FUNCTION_ENTRY( "updatePlanInstanceCombo" );

    if (isApproved())
    {
        PlanInstanceList planInstances;
        getLocalPlanInstanceMap().getPlanInstances(planInstances);

        // If plans at all locations are required
        if (filter.allLocationsNeeded())
        {
            PlanInstanceList remotePlanInstances;
            getRemotePlanInstanceMap().getPlanInstances(remotePlanInstances);

            planInstances.insert(planInstances.end(), remotePlanInstances.begin(), remotePlanInstances.end());
        }

        for (PlanInstanceList::const_iterator iiter = planInstances.begin(); iiter != planInstances.end(); iiter++)
        {
            PlanInstanceSharedPtr planInstance = *iiter;
            planInstance->addToPlanInstanceCombo(instanceComboBox);
        }
    }

    FUNCTION_EXIT;
}


void PlanNode::addStepsToStepList(CPlanStepListCtrl& planStepsListCtrl)
{
    FUNCTION_ENTRY( "addStepsToStepList" );

    planStepsListCtrl.DeleteAllItems();

    PlanStepList planSteps;
    getPlanStepMap().getPlanSteps(planSteps);

    planStepsListCtrl.SetItemCount(planSteps.size());

    for (PlanStepList::const_iterator siter = planSteps.begin(); siter != planSteps.end(); siter++)
    {
        PlanStep* planStep = *siter;
        if ( NULL != planStep )
        {
            planStepsListCtrl.AddStep( *planStep );
        }
    }

    FUNCTION_EXIT;
}

void PlanNode::clearPlanInstances( const unsigned long ulLocationKey )
{
    FUNCTION_ENTRY( "clearPlanInstances" );

    getPlanInstanceMap(ulLocationKey).clearInstances();

    FUNCTION_EXIT;
}

void PlanNode::activityCreated( const int nType, const unsigned int unId )
{
    try
    {
        getPlanStepMap().insertPlanStep( nType, getPlanStepMap().getNewPostion(), unId );

        m_areStepsUpdated = true;
    }
    catch (...)
    {
    }

    notifyPlanStepUpdate( unId, 1 );
}

void PlanNode::activityDeleted( const unsigned int unId )
{
    try
    {
        getPlanStepMap().deletePlanStep( unId );

        m_areStepsUpdated = true;
    }
    catch ( ... )
    {
    }

    notifyPlanStepUpdate( unId, 1 );
}

void PlanNode::activityUpdated( const unsigned int unId )
{
    m_areStepsUpdated = true;
}

void PlanNode::activityCopied( const unsigned int unFrom, const unsigned int unTo )
{
    PlanStep* pStepFrom = getPlanStepMap().getPlanStepById( unFrom );
    PlanStep* pStepTo = getPlanStepMap().getPlanStepById( unTo );
    
    if ( NULL == pStepFrom || NULL == pStepTo )
    {
        return;
    }

    pStepFrom->reloadParameters();

    std::vector<std::string> vecParameters = pStepFrom->compactParameter();

    pStepTo->copyParameter( vecParameters );

    pStepTo->setIgnoreFailure( pStepFrom->ignoreFailure() );
    pStepTo->setStepDelay( pStepFrom->getStepDelay() );
    pStepTo->setStepDescription( pStepFrom->getStepDescription() );
    pStepTo->setStepName( pStepFrom->getStepName() );
    pStepTo->setStepSkippable( pStepFrom->isStepSkippable() );
    pStepTo->setStepSkipped( pStepFrom->isStepSkipped() );

    m_areStepsUpdated = true;
}

void PlanNode::flowCreated( const unsigned int unId )
{
    try
    {
        getSequenceFlowMap().insertSequenceFlow( unId );

        m_areStepsUpdated = true;
    }
    catch ( ... )
    {
    }
}

void PlanNode::flowDeleted( const unsigned int unId )
{
    SequenceFlow* pFlow = getSequenceFlowMap().getSequenceFlow( unId );

    if ( NULL == pFlow )
    {
        return;
    }

    if ( 0u != pFlow->outgoingStep() )
    {
        PlanStep* pStep = getPlanStepMap().getPlanStepById( pFlow->outgoingStep() );

        if ( NULL != pStep )
        {
            pStep->linkIncomingStep( pFlow->incomingStep(), 0u );
        }
    }

    if ( 0u != pFlow->incomingStep() )
    {
        PlanStep* pStep = getPlanStepMap().getPlanStepById( pFlow->incomingStep() );

        if ( NULL != pStep )
        {
            pStep->linkOutgoingStep( pFlow->outgoingStep(), 0u );
        }
    }

    getSequenceFlowMap().deleteSequenceFlow( unId );

    m_areStepsUpdated = true;
}

void PlanNode::linkFlowFrom( const unsigned int unFlow, const unsigned int unActivity )
{
    unsigned int unPrevIncoming = 0u;
    unsigned int unCurOutgoing = 0u;
    bool bLinkUpdated = getSequenceFlowMap().linkFlowFrom( unFlow, unActivity, unPrevIncoming, unCurOutgoing );

    m_areStepsUpdated |= bLinkUpdated;

    if ( bLinkUpdated )
    {
        if ( 0u != unCurOutgoing )
        {
            PlanStep* pOutStep = getPlanStepMap().getPlanStepById( unCurOutgoing );

            if ( NULL != pOutStep )
            {
                pOutStep->linkIncomingStep( unPrevIncoming, unActivity );
            }
            
            PlanStep* pCurStep = getPlanStepMap().getPlanStepById( unActivity );

            if ( NULL != pCurStep )
            {
                pCurStep->linkOutgoingStep( 0u, unCurOutgoing );
            }
        }

        if ( 0u != unPrevIncoming )
        {
            PlanStep* pStep = getPlanStepMap().getPlanStepById( unPrevIncoming );

            if ( NULL != pStep )
            {
                pStep->linkOutgoingStep( unCurOutgoing, 0u );
            }
        }
    }
}

void PlanNode::linkFlowTo( const unsigned int unFlow, const unsigned int unActivity )
{
    unsigned int unPrevOutgoing = 0u;
    unsigned int unCurIncoming = 0u;
    bool bLinkUpdated = getSequenceFlowMap().linkFlowTo( unFlow, unActivity, unPrevOutgoing, unCurIncoming );

    m_areStepsUpdated |= bLinkUpdated;

    if ( bLinkUpdated )
    {
        if ( 0u != unCurIncoming )
        {
            PlanStep* pInStep = getPlanStepMap().getPlanStepById( unCurIncoming );

            if ( NULL != pInStep )
            {
                pInStep->linkOutgoingStep( unPrevOutgoing, unActivity );
            }
            
            PlanStep* pCurStep = getPlanStepMap().getPlanStepById( unActivity );

            if ( NULL != pCurStep )
            {
                pCurStep->linkIncomingStep( 0u, unCurIncoming );
            }
        }

        if ( 0u != unPrevOutgoing )
        {
            PlanStep* pStep = getPlanStepMap().getPlanStepById( unPrevOutgoing );

            if ( NULL != pStep )
            {
                pStep->linkIncomingStep( unCurIncoming, 0u );
            }
        }
    }
}

void PlanNode::updatePlanInstance(const TA_Base_Core::ActivePlanDetail &activePlanDetail)
{
    FUNCTION_ENTRY( "updatePlanInstance" );

    TA_ASSERT(activePlanDetail.plan.plan == getNodeId(), "Plan node ID mismatch");

    getPlanInstanceMap(activePlanDetail.plan.location).updatePlanInstance(activePlanDetail);

    FUNCTION_EXIT;
}


void PlanNode::updatePlanInstance(const TA_Base_Core::PlanExecuteStepUpdate &activeStepDetail)
{
    FUNCTION_ENTRY( "updatePlanInstance" );

    TA_ASSERT(activeStepDetail.plan.plan == getNodeId(), "Plan node ID mismatch");

    getPlanInstanceMap(activeStepDetail.plan.location).updatePlanInstance(activeStepDetail);

    FUNCTION_EXIT;
}


void PlanNode::updatePlanInstance(const TA_Base_Core::PlanCustomisationUpdate &planCustomDetail)
{
    FUNCTION_ENTRY( "updatePlanInstance" );

    TA_ASSERT(planCustomDetail.plan.plan == getNodeId(), "Plan node ID mismatch");

    getPlanInstanceMap(planCustomDetail.plan.location).updatePlanInstance(planCustomDetail);

    FUNCTION_EXIT;
}


void PlanNode::updateNode(const TA_Base_Core::PlanConfigUpdateData &planConfigData)
{
    FUNCTION_ENTRY( "updateNode" );

    TreeNode::updateNode(planConfigData);

    switch (planConfigData.configUpdate._d())
    {
        case TA_Base_Core::PCUT_APPROVAL_STATE:
        {
            if (!isApproved())
            {
                // If a Plan Controller is running for this plan, then tell it the plan's been unapproved.
                CPlanControllerThread *planControllerThread = FloatingWindowThreadRegister::getInstance().getPlanControllerThread(getNodeId());
                if (NULL != planControllerThread)
                {
                    planControllerThread->PostThreadMessage(WM_CONTROLLED_PLAN_APPROVAL_STATE_CHANGED, getNodeId(), 0);
                }
            }
            break;
        }

        case TA_Base_Core::PCUT_UPDATE_PLAN:
        {
            CPlanControllerThread *planControllerThread = FloatingWindowThreadRegister::getInstance().getPlanControllerThread(getNodeId());
            if (NULL != planControllerThread)
            {
                planControllerThread->PostThreadMessage(WM_CONTROLLED_PLAN_APPROVAL_STATE_CHANGED, getNodeId(), 0);
            }
            invalidatePlanStepMap();
            break;
        }

        default:
            break;
    }

    FUNCTION_EXIT;
}

void PlanNode::updateChildNodeList(const TA_Base_Core::PlanConfigUpdateData &planConfigData)
{
    FUNCTION_ENTRY( "updateChildNodeList" );

    TA_ASSERT(false, "Plan nodes do not have children");

    FUNCTION_EXIT;
}

void PlanNode::setPlanType(const TA_Base_Core::ENode eType)
{
    FUNCTION_ENTRY("setPlanName");

    TA_ASSERT(m_isCopiedPlan, "Attempting to update an original plan.");
    m_isInfoUpdated |= (eType != getPlanDetail().nodeType);
    getPlanDetail().nodeType = eType;
    setType(eType);

    FUNCTION_EXIT;
}

void PlanNode::setPlanName(const std::string& name)
{
    FUNCTION_ENTRY( "setPlanName" );

    TA_ASSERT(m_isCopiedPlan,"Attempting to update an original plan.");
    m_isInfoUpdated |= ( 0 != name.compare( getPlanDetail().name.in() ));
    getPlanDetail().name = name.c_str();

    FUNCTION_EXIT;
}


void PlanNode::setPlanDescription(const std::string& description)
{
    FUNCTION_ENTRY( "setPlanDescription" );

    TA_ASSERT(m_isCopiedPlan,"Attempting to update an original plan.");
    m_isInfoUpdated |= ( 0 != description.compare( getPlanDetail().description.in() ));
    getPlanDetail().description = description.c_str();

    FUNCTION_EXIT;
}


void PlanNode::schedulePlan()
{
    FUNCTION_ENTRY( "schedulePlan" );

    if (OperatorRights::getInstance().canSchedulePlans())
    {
        // Get the full path of this plan.
        std::string planPath("");

        try
        {
            planPath = PlanAgentAccess::getInstance().getNodePath(getNodeId());
        }
        catch (...)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                "Failed to determine the path of node ID %lu", getNodeId());

            CString actionName;
            actionName.LoadString(IDS_LAUNCH_SCHEDULING_MANAGER);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            userMsg.showMsgBox(IDS_UE_010028);

            FUNCTION_EXIT;
            return;
        }

        std::ostringstream commandLine;
        commandLine << "--job-info=Plan:PlanPath/" << planPath;

        CPoint pt = GetMessagePos();
        RECT newRect = TA_Base_Bus::AppLauncher::getInstance().getRect(
            TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT, TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC, pt.x);
        // Run the Plan Scheduler
        TA_Base_Bus::AppLauncher::getInstance().launchApplication(SCHEDULING_MANAGER, commandLine.str(),
            TA_Base_Bus::TA_GenericGui::POS_BOUNDED, TA_Base_Bus::TA_GenericGui::ALIGN_FIT, NULL, &newRect);
    }
    else
    {
        CString actionName;
        actionName.LoadString(IDS_LAUNCH_SCHEDULING_MANAGER);
        TA_Base_Bus::TransActiveMessage userMsg;
          std::string userName;
          std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
          TA_Base_Bus::AuthenticationLibrary authLibrary(true);
          TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
          std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
          userName = iOperator->getName();
          userMsg << userName;
        userMsg << actionName;
        userMsg.showMsgBox(IDS_UE_010021);
    }

    FUNCTION_EXIT;
}


void PlanNode::approvePlan(bool isApproved)
{
    FUNCTION_ENTRY( "approvePlan" );

    PlanAgentAccess::getInstance().setPlanApprovalState(getNodeId(), (isApproved? TA_Base_Core::APPROVED_APPROVAL_STATE : TA_Base_Core::UNAPPROVED_APPROVAL_STATE));

    FUNCTION_EXIT;
}


void PlanNode::savePlan()
{
    FUNCTION_ENTRY( "savePlan" );

    TA_ASSERT(m_isCopiedPlan,"Attempting to save a non-editted file");
    
    TA_Base_Core::Plan plan;
    int stepNumber = 0;

    // Create plan structure
    plan.detail = getPlanDetail();

    if ( m_areStepsUpdated )
    {
        // Add all the plan steps
        PlanStepList planSteps;
        getPlanStepMap().getPlanSteps( planSteps );

        for (PlanStepList::iterator siter = planSteps.begin(); siter != planSteps.end(); siter++)
        {
            (*siter)->reloadParameters();
            (*siter)->resetFlowParameters();
        }

        createClassicSequenceFlows( planSteps );

        plan.stepseq.length(planSteps.size());
        for (PlanStepList::iterator siter = planSteps.begin(); siter != planSteps.end(); siter++)
        {
            TA_Base_Core::Step& refStep = plan.stepseq[stepNumber];
            (*siter)->updateFlowParameters();

            refStep.detail.delay = (*siter)->getStepDelay();
            refStep.detail.description = (*siter)->getStepDescription().c_str();
            refStep.detail.graph = (*siter)->getStepGraph().c_str();
            refStep.detail.id = (*siter)->getStepId();
            refStep.detail.ignoreFailure = (*siter)->ignoreFailure();
            refStep.detail.name = (*siter)->getStepName().c_str();
            refStep.detail.pkey = (*siter)->getStepPkey();
            refStep.detail.position = (*siter)->getStepPosition();
            refStep.detail.skip = (*siter)->isStepSkipped();
            refStep.detail.skippable = (*siter)->isStepSkippable();
            refStep.detail.type = (*siter)->getStepType();

            // Validate parameters
            std::vector<std::string> vecInvalidParams;
            if ( hasInvalidParameter( *(*siter), vecInvalidParams ))
            {
                TA_THROW( TA_Base_Core::PlanStepDetailException( "Invalid Step parameter", vecInvalidParams.front(), TA_Base_Core::PSDE_INVALID, refStep.detail.position ));
            }

            // Validate sequence flows
            std::vector<std::string> vecInvalidFlows;
            if ( hasInvalidFlow( *(*siter), vecInvalidFlows ))
            {
                TA_THROW( TA_Base_Core::PlanStepDetailException( "Invalid Sequence flow", vecInvalidFlows.front(), TA_Base_Core::PSDE_INVALID, refStep.detail.position ));
            }

            std::vector<std::string> vecParameters = (*siter)->compactParameter();
            refStep.parameters.length( vecParameters.size() );

            int nParamIndex = 0;
            for ( std::vector<std::string>::const_iterator itParamLoop = vecParameters.begin(); vecParameters.end() != itParamLoop; ++itParamLoop )
            {
                refStep.parameters[nParamIndex++] = itParamLoop->c_str();
            }

            stepNumber++;
        }  // For each plan step

        // Add all the plan steps
        SequenceFlowList sequenceFlows;

        getSequenceFlowMap().getSequenceFlows( sequenceFlows );

        plan.flowseq.length( sequenceFlows.size() );
        stepNumber = 0;
        for ( SequenceFlowList::const_iterator siter = sequenceFlows.begin(); siter != sequenceFlows.end(); siter++ )
        {
            TA_Base_Core::SequenceFlow& rerFlow = plan.flowseq[stepNumber];
            rerFlow.graph = (*siter)->graph( ).c_str( );
            rerFlow.pkey = (*siter)->pkey();
            rerFlow.id = (*siter)->id();
            rerFlow.incomingStep = (*siter)->incomingStep();
            rerFlow.outgoingStep = (*siter)->outgoingStep();
            rerFlow.planKey = plan.detail.plan;
            stepNumber++;
        }  // For each sequence flow
    }

    // Save this plan
    PlanAgentAccess::getInstance().updatePlan(plan);

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[Pointer] Plan [%lu] saved", getNodeId() );

    // Plan modification author and time must be refreshed.
    invalidateNodeDetail();
    notifyPlanGeneralDetailUpdate();

    m_areStepsUpdated = false;
    m_isInfoUpdated = false;

    FUNCTION_EXIT;
}

void PlanNode::upgradePlan()
{
    setPlanType(TA_Base_Core::WORKFLOW_NODE);
}

TA_Base_Core::AuxPlanDetail* PlanNode::exportPlan()
{
    FUNCTION_ENTRY( "exportPlan" );

    TA_Base_Core::AuxPlanDetail_var auxPlanVar;
    auxPlanVar = PlanAgentAccess::getInstance().exportPlan(getNodeId());

    FUNCTION_EXIT;
    return auxPlanVar._retn();
}


void PlanNode::deletePlan()
{
    FUNCTION_ENTRY( "deletePlan" );

    PlanAgentAccess::getInstance().deletePlan(getNodeId());

    FUNCTION_EXIT;
}


void PlanNode::startNewInstance()
{
    FUNCTION_ENTRY( "startNewInstance" );

    PlanAgentAccess::getInstance().startPlan(getNodeId());

    FUNCTION_EXIT;
}


void PlanNode::customiseNewInstance()
{
    FUNCTION_ENTRY( "customiseNewInstance" );

    PlanAgentAccess::getInstance().customisePlan(getNodeId());

    FUNCTION_EXIT;
}


void PlanNode::executeInteractiveStep(InteractiveStep& interactiveStep)
{
    FUNCTION_ENTRY( "executeInteractiveStep" );

    // Open a Plan Controller for the interactive step to execute within. Ignore any limits that may be
    // placed on the number of simultaneously open Plan Controllers. They should not prevent the step's
    // successful execution.
    controlPlan(true);

    FUNCTION_EXIT;
}


void PlanNode::controlPlan(bool ignoreActivePlanControllerLimit)
{
    FUNCTION_ENTRY( "controlPlan" );

    if (OperatorRights::getInstance().canControlPlans())
    {
        // Create a new Plan Controller or activate the existing one if already open
        FloatingWindowThreadRegister::getInstance().createPlanControllerThread(getNodeId(), ignoreActivePlanControllerLimit);
    }
    else
    {
        CString actionName;
        actionName.LoadString(IDS_LAUNCH_PLAN_CONTROLLER);
        TA_Base_Bus::TransActiveMessage userMsg;
          std::string userName;
          std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
          TA_Base_Bus::AuthenticationLibrary authLibrary(true);
          TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
          std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
          userName = iOperator->getName();
          userMsg << userName;
        userMsg << actionName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_010021);
    }

    FUNCTION_EXIT;
}


void PlanNode::controlPlan(const PlanInstanceId &planInstanceId)
{
    FUNCTION_ENTRY( "controlPlan" );

    // Open a Plan Controller.
    controlPlan();

    // Ask the Plan Controller to select the specified plan instance.
    CPlanControllerThread *planControllerThread = FloatingWindowThreadRegister::getInstance().getPlanControllerThread(getNodeId());
    if (planControllerThread)
    {
        planControllerThread->PostThreadMessage(WM_SELECT_ACTIVE_PLAN_INSTANCE, getNodeId(), MAKELPARAM(planInstanceId.m_instance, planInstanceId.m_location));
    }

    FUNCTION_EXIT;
}


void PlanNode::editPlan()
{
    FUNCTION_ENTRY( "editPlan" );

    if (OperatorRights::getInstance().canEditPlans())
    {
        // Create a new Plan Editor or activate the existing one if already open.
        // Note that createPlanEditorThread() returns NULL if plan edit lock cannot be obtained.
        FloatingWindowThreadRegister::getInstance().createPlanEditorThread(getNodeId());
    }
    else
    {
        CString actionName;
        actionName.LoadString(IDS_LAUNCH_PLAN_EDITOR);
        TA_Base_Bus::TransActiveMessage userMsg;
          std::string userName;
          std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
          TA_Base_Bus::AuthenticationLibrary authLibrary(true);
          TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
          std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
          userName = iOperator->getName();
          userMsg << userName;
        userMsg << actionName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_010021);
    }

    FUNCTION_EXIT;
}


// Returns true if this plan node precedes the specified sibling node on a plan tree branch.
bool PlanNode::hasPrecedence(TreeNode &siblingNode)
{
    FUNCTION_ENTRY( "hasPrecedence" );

    PlanNode *planNode = dynamic_cast<PlanNode *>(&siblingNode);

    // Plan nodes trail category nodes. If the other node is not a plan (i.e. must be a category),
    // then it should actually precede this node.
    if (!planNode)
    {
        FUNCTION_EXIT;
        return false;
    }

    // If both this and the sibling are plan nodes, then compare them on name basis.
    std::string thisNodeName = getNodeName();
    std::string otherNodeName = planNode->getNodeName();

    FUNCTION_EXIT;
    return std::lexicographical_compare(thisNodeName.begin(), thisNodeName.end(),
                                        otherNodeName.begin(), otherNodeName.end(),
                                        CaseInsensitiveCompare());
}


TA_Base_Core::PlanDetail& PlanNode::getPlanDetail()
{
    FUNCTION_ENTRY( "getPlanDetail" );
    FUNCTION_EXIT;
    return getNodeDetail().plan();
}


PlanInstanceMap &PlanNode::getPlanInstanceMap(LocationKey location)
{
    FUNCTION_ENTRY( "getPlanInstanceMap" );

    if (location == TA_Base_Bus::PlanAgentLocationAccess::getInstance().getDefaultPlanAgentLocation())
    {
        FUNCTION_EXIT;
        return getLocalPlanInstanceMap();
    }

    FUNCTION_EXIT;
    return getRemotePlanInstanceMap();
}


PlanInstanceMap &PlanNode::getLocalPlanInstanceMap()
{
    FUNCTION_ENTRY( "getLocalPlanInstanceMap" );

    if (!m_localPlanInstanceMap)
    {
        // Contact the plan agent at the profile location and get its active plan details
        TA_Base_Core::ActivePlanDetails_var activePlanDetails = PlanAgentAccess::getInstance().getActivePlanDetails(getNodeId());

        m_localPlanInstanceMap = new PlanInstanceMap(activePlanDetails);
    }

    FUNCTION_EXIT;
    return *m_localPlanInstanceMap;
}


PlanInstanceMap &PlanNode::getRemotePlanInstanceMap()
{
    FUNCTION_ENTRY( "getRemotePlanInstanceMap" );

    if (!m_remotePlanInstanceMap)
    {
        m_remotePlanInstanceMap = new PlanInstanceMap();

        // Get all the remote locations
        typedef std::vector<unsigned long> LocationList;
        LocationList locations = TA_Base_Bus::PlanAgentLocationAccess::getInstance().getNonDefaultPlanAgentLocations();

        // Contact the plan agent at each of the remote locations and get its active plan details.
        // getActivePlanDetailsAtLocation() will skip a location if no Plan Agent entity is configured for it.
        for (LocationList::iterator iter = locations.begin(); iter != locations.end(); iter++)
        {
            LocationKey location = *iter;

            try
            {
                TA_Base_Core::ActivePlanDetails_var activePlanDetails =
                    PlanAgentAccess::getInstance().getActivePlanDetailsAtLocation(getNodeId(), location);

                m_remotePlanInstanceMap->addPlanInstances(activePlanDetails);
            }
            catch (PlanAgentAccessException& paex)
            {
                CString actionName;
                std::string locationName;
                TA_Base_Bus::TransActiveMessage userMsg;
                unsigned long location = atol (paex.getDetail ().c_str ());
                locationName = TA_Base_Bus::PlanAgentLocationAccess::getInstance().getLocationName (location).c_str();
                userMsg << locationName;
                actionName.LoadString(IDS_RETRIEVE_PLAN);
                userMsg << actionName;
                // userMsg.showMsgBox(IDS_UE_210011);
            }
            catch (TA_Base_Core::TransactiveException &ex)
            {
                MessageBox::error(IDS_RETRIEVE_PLAN, ex);
            }
        }
    }

    FUNCTION_EXIT;
    return *m_remotePlanInstanceMap;
}

void PlanNode::createMapsIfRequired()
{
    FUNCTION_ENTRY( "createMapsIfRequired" );

    if ( NULL == m_planStepMap )
    {
		TA_Base_Core::StepDetails_var stepDetails = PlanAgentAccess::getInstance().getStepDetails( getNodeId() );

        m_planStepMap = new PlanStepMap( this, stepDetails );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[Pointer] PlanStepMap created for plan [%lu]", getNodeId() );
    }

    if ( NULL == m_sequenceFlowMap )
    {
        TA_Base_Core::SequenceFlows_var sequenceFlows = PlanAgentAccess::getInstance().getSequenceFlows( getNodeId() );

        m_sequenceFlowMap = new SequenceFlowMap( this );

        for ( size_t szLoop = 0u; szLoop < sequenceFlows->length(); ++szLoop )
        {
            TA_Base_Core::SequenceFlow& refFlow = sequenceFlows[szLoop];
            m_sequenceFlowMap->insertSequenceFlow( refFlow.pkey, refFlow.id, refFlow.incomingStep, refFlow.outgoingStep, refFlow.condition.in(), refFlow.graph.in() );
        }
    }

    FUNCTION_EXIT;
}

PlanStepMap& PlanNode::getPlanStepMap()
{
    FUNCTION_ENTRY( "getPlanStepMap" );

    createMapsIfRequired();

    FUNCTION_EXIT;
    return *m_planStepMap;
}


SequenceFlowMap& PlanNode::getSequenceFlowMap()
{
    FUNCTION_ENTRY( "getSequenceFlowMap" );

    createMapsIfRequired();

    FUNCTION_EXIT;
    return *m_sequenceFlowMap;
}



void PlanNode::invalidatePlanStepMap()
{
    FUNCTION_ENTRY( "invalidatePlanStepMap" );

    delete m_planStepMap;
    m_planStepMap = NULL;

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[Pointer] PlanStepMap destroyed for plan [%lu]", getNodeId() );

    delete m_sequenceFlowMap;
    m_sequenceFlowMap = NULL;

    FUNCTION_EXIT;
}

//PlanStep* PlanNode::getPlanStepByPos( unsigned long stepNum )
//{
//    FUNCTION_ENTRY( "getPlanStepByPos" );
//
//    FUNCTION_EXIT;
//    return getPlanStepMap().getPlanStepByPos( stepNum );
//}

PlanStep* PlanNode::getPlanStepByStepId(unsigned long stepId)
{
	FUNCTION_ENTRY( "getPlanStepById" );

	FUNCTION_EXIT;
	return getPlanStepMap().getPlanStepById(stepId);
}

std::vector<PlanStep*> PlanNode::getPlanSteps()
{
	FUNCTION_ENTRY( "getPlanSteps" );

	PlanStepList planSteps;
	getPlanStepMap().getPlanSteps(planSteps);

	FUNCTION_EXIT;
	return planSteps;
}

std::map<unsigned int, UintSet> PlanNode::getSequenceFlows()
{
    FUNCTION_ENTRY("getPlanSteps");

    SequenceFlowList sequenceFlows;
    getSequenceFlowMap().getSequenceFlows(sequenceFlows);

    std::map<unsigned int, UintSet> mapStepToOutgoings;

    for ( SequenceFlowList::const_iterator itLoop = sequenceFlows.begin(); sequenceFlows.end() != itLoop; ++itLoop )
    {
        SequenceFlow* pFlow = *itLoop;

        if ( NULL == pFlow )
        {
            continue;
        }

        unsigned int unIncoming = pFlow->incomingStep();
        unsigned int unOutgoing = pFlow->outgoingStep();

        if ( 0u != unIncoming && 0u != unOutgoing )
        {
            mapStepToOutgoings[unIncoming].insert( unOutgoing );
        }
    }

    FUNCTION_EXIT;
    return mapStepToOutgoings;
}

unsigned long PlanNode::getPlanNodeId()
{
	FUNCTION_ENTRY( "getPlanNodeId" );
	FUNCTION_EXIT;
	return getNodeDetail().plan().plan;
}

void PlanNode::notifyPlanGeneralDetailUpdate()
{
    FUNCTION_ENTRY( "notifyPlanGeneralDetailUpdate" );

    CPlanEditorThread *planEditorThread = FloatingWindowThreadRegister::getInstance().getPlanEditorThread(getNodeId());
    if (planEditorThread)
    {
        planEditorThread->PostThreadMessage(WM_REFRESH_PLAN_GENERAL_DETAIL, 0, 0);
    }

    FUNCTION_EXIT;
}


void PlanNode::notifyPlanStepUpdate(unsigned long stepNumber, unsigned long stepCount)
{
    FUNCTION_ENTRY( "notifyPlanStepUpdate" );

    CPlanEditorThread *planEditorThread = FloatingWindowThreadRegister::getInstance().getPlanEditorThread(getNodeId());
    if (NULL != planEditorThread)
    {
        planEditorThread->PostThreadMessage(WM_REFRESH_PLAN_STEP_LIST, (WPARAM) stepNumber, (LPARAM) stepCount);
    }

    FUNCTION_EXIT;
}


bool PlanNode::editPlanStep(PlanStep* planStep)
{
    FUNCTION_ENTRY( "editPlanStep" );

    TA_ASSERT(planStep!=NULL, "No plan step specified.");

	CPlanStepEditorDlg dlg(planStep,true);

	if (dlg.DoModal() == IDOK)
	{
        m_areStepsUpdated = true;
		notifyPlanStepUpdate(planStep->getStepPosition(), 1);

		FUNCTION_EXIT;
		return true;
	}

	FUNCTION_EXIT;
	return false;
}

void PlanNode::insertPlanStep( const int stepType, unsigned long position )
{
    FUNCTION_ENTRY( "insertPlanStep" );

    unsigned long ulNewStepId = getPlanStepMap().getNewId();
    getPlanStepMap().insertPlanStep( stepType, position, ulNewStepId );

    // Refresh the step list in the editor. Request selection of the new step added at the specified position.
    m_areStepsUpdated = true;
    notifyPlanStepUpdate(position, 1);

    FUNCTION_EXIT;
}


void PlanNode::deletePlanSteps(PlanStepList &deleteSteps)
{
    FUNCTION_ENTRY( "deletePlanSteps" );

    TA_ASSERT(deleteSteps.size() > 0, "No plan step specified.");
    
    unsigned long firstRemovedPosition = getPlanStepMap().deletePlanSteps(deleteSteps);
    TA_ASSERT(deleteSteps.empty(), "Deleted step list is not empty");

    // Refresh the step list in the editor. Request selection of the step that now has the position
    // of the first of the deleted steps.
    m_areStepsUpdated = true;
    notifyPlanStepUpdate(firstRemovedPosition, 1);

    FUNCTION_EXIT;
}


void PlanNode::copyPlanSteps(const PlanStepList &copySteps, unsigned long position)
{
    FUNCTION_ENTRY( "copyPlanSteps" );

    TA_ASSERT(copySteps.size() > 0, "No plan step specified.");

    unsigned long ulInId = 0u;
    PlanStep* pCurStep = getPlanStepMap().getPlanStepByPos( position - 1u );
    if ( NULL != pCurStep )
    {
        ulInId = pCurStep->getStepId();
    }

    unsigned long ulOutId = 0u;
    PlanStep* pNextStep = getPlanStepMap().getPlanStepByPos( position );
    if ( NULL != pNextStep )
    {
        ulOutId = pNextStep->getStepId();
    }

    std::vector<unsigned int> newStepIds;
    getPlanStepMap().copyPlanSteps( copySteps, position, newStepIds );
    
    bool bFound = false;
    unsigned int unInToOut = 0u;
    unsigned int unOutToIn = 0u;
    getSequenceFlowMap().getSequenceFlowId( ulInId, ulOutId, unInToOut, bFound );
    getSequenceFlowMap().getSequenceFlowId( ulOutId, ulInId, unOutToIn, bFound );

    if ( 0u != unInToOut )
    {
        getSequenceFlowMap().deleteSequenceFlow( unInToOut );
    }
    if ( 0u != unOutToIn )
    {
        getSequenceFlowMap().deleteSequenceFlow( unOutToIn );
    }

    // Refresh the step list in the editor. Request selection of the copied steps added at the specified position.
    m_areStepsUpdated = true;
    notifyPlanStepUpdate( position, copySteps.size() );

    FUNCTION_EXIT;
}


void PlanNode::movePlanSteps(const PlanStepList &moveSteps, unsigned long position)
{
    FUNCTION_ENTRY( "movePlanSteps" );

    TA_ASSERT(moveSteps.size() > 0, "No plan step specified.");

    unsigned long adjustedPosition = getPlanStepMap().movePlanSteps(moveSteps, position);

    // Refresh the step list in the editor. Request selection of the moved steps at their final position.
    // This position will not be the same as the specified target position if steps were moved from before
    // the target position.
    m_areStepsUpdated = true;
    notifyPlanStepUpdate(adjustedPosition, moveSteps.size());

    FUNCTION_EXIT;
}


unsigned long PlanNode::getLastStepPosition()
{
    FUNCTION_ENTRY( "getLastStepPosition" );
    FUNCTION_EXIT;
    return getPlanStepMap().getLastStepPosition();
}

void PlanNode::addNoneInstance( CPlanInstanceComboBox &instanceComboBox )
{
    m_planInstanceNone.addToPlanInstanceCombo( instanceComboBox );
}

IActivityParameter& PlanNode::getActivityParameter( const unsigned int unId )
{
    PlanStep* pStep = getPlanStepMap().getPlanStepById( unId );

    if ( NULL == pStep )
    {
        throw PlanManagerException( "Invalid activity id" );
    }

    return pStep->getActivityParameter();
}

void PlanNode::updateGraph( const std::map<unsigned int, ActivityData>& mapGraph, const std::map<unsigned int, SequenceFlowData>& mapFlow )
{
    for ( std::map<unsigned int, ActivityData>::const_iterator itLoop = mapGraph.begin(); mapGraph.end() != itLoop; ++itLoop )
    {
        PlanStep* pStep = getPlanStepMap().getPlanStepById( itLoop->first );

        if ( NULL == pStep )
        {
            continue;
        }

        pStep->setStepGraph( itLoop->second.Graph );
    }

    for ( std::map<unsigned int, SequenceFlowData>::const_iterator itLoop = mapFlow.begin(); mapFlow.end() != itLoop; ++itLoop )
    {
        SequenceFlow* pFlow = getSequenceFlowMap().getSequenceFlow( itLoop->first );

        if ( NULL == pFlow )
        {
            continue;
        }

        pFlow->graph( itLoop->second.Graph );
    }
}

void PlanNode::retrieveGraph( std::map<unsigned int, ActivityData>& mapGraph, std::map<unsigned int, SequenceFlowData>& mapFlow )
{
    PlanStepList planSteps;
    getPlanStepMap().getPlanSteps( planSteps );

    for ( PlanStepList::iterator itLoop = planSteps.begin(); planSteps.end() != itLoop; ++itLoop )
    {
        PlanStep* pStep = *itLoop;

        if ( NULL == pStep )
        {
            continue;
        }
        
        ActivityData stData = { 0u, "", "" };
        std::map<unsigned int, ActivityData>::iterator itNew = mapGraph.insert( std::map<unsigned int, ActivityData>::value_type( pStep->getStepId(), stData )).first;
                
        if ( mapGraph.end() != itNew )
        {
            itNew->second.Type = pStep->getStepType();
            itNew->second.Graph = pStep->getStepGraph();
            itNew->second.InstanceName = pStep->getStepName();
        }
    }

    SequenceFlowList sequenceFlows;
    getSequenceFlowMap().getSequenceFlows( sequenceFlows );

    for ( SequenceFlowList::iterator itLoop = sequenceFlows.begin(); sequenceFlows.end() != itLoop; ++itLoop )
    {
        SequenceFlow* pFlow = *itLoop;

        if ( NULL == pFlow )
        {
            continue;
        }
        
        SequenceFlowData stData = { 0u, 0u, "", "" };
        std::map<unsigned int, SequenceFlowData>::iterator itNew = mapFlow.insert( std::map<unsigned int, SequenceFlowData>::value_type( pFlow->id(), stData )).first;
                
        if ( mapFlow.end() != itNew )
        {
            itNew->second.Graph = pFlow->graph();
            itNew->second.Condition = pFlow->condition();
            itNew->second.Incoming = pFlow->incomingStep();
            itNew->second.Outgoing = pFlow->outgoingStep();
        }
    }
}

bool PlanNode::hasInvalidParameter( PlanStep& refStep, std::vector<std::string>& vecParamNames )
{
    vecParamNames.clear();

    std::vector<ItemPair> vecParams;

    refStep.getActivityParameter().getNameValuePairs( vecParams );

    vecParamNames.clear();
    for ( std::vector<ItemPair>::iterator itLoop = vecParams.begin(); vecParams.end() != itLoop; ++itLoop )
    {
        if ( !itLoop->Valid )
        {
            vecParamNames.push_back( itLoop->DisplayName );
        }
    }

    return !vecParamNames.empty();
}

bool PlanNode::hasInvalidFlow( PlanStep& refStep, std::vector<std::string>& vecFlowErrors )
{
    vecFlowErrors.clear();

    std::map<unsigned int, unsigned int> mapFlowIdToIncoming;
    std::map<unsigned int, unsigned int> mapFlowIdToOutgoing;

    getSequenceFlowMap().getSequenceFlows( refStep.getStepId(), mapFlowIdToIncoming, mapFlowIdToOutgoing );

    if ( mapFlowIdToIncoming.empty() && refStep.hasIncoming() )
    {
        vecFlowErrors.push_back( "Incoming step missed" );
    }
    else if (( mapFlowIdToIncoming.size() > 1u ) && !refStep.allowMultipleIncoming() )
    {
        // Multiple incoming steps allowed since branched flow exist.
        //vecFlowErrors.push_back( "Multiple incoming steps" );
    }
    else
    {
        for ( std::map<unsigned int, unsigned int>::iterator itLoop = mapFlowIdToIncoming.begin(); mapFlowIdToIncoming.end() != itLoop; ++itLoop )
        {
            if ( 0u == itLoop->second )
            {
                vecFlowErrors.push_back( "Incoming step missed" );
                break;
            }
        }
    }

    if ( mapFlowIdToOutgoing.empty()  && refStep.hasOutgoing() )
    {
        vecFlowErrors.push_back( "Outgoing step missed" );
    }
    else if (( mapFlowIdToOutgoing.size() > 1u ) && !refStep.allowMultipleOutgoing() )
    {
        vecFlowErrors.push_back( "Multiple outgoing steps" );
    }
    else
    {
        for ( std::map<unsigned int, unsigned int>::iterator itLoop = mapFlowIdToOutgoing.begin(); mapFlowIdToOutgoing.end() != itLoop; ++itLoop )
        {
            if ( 0u == itLoop->second )
            {
                vecFlowErrors.push_back( "Outgoing step missed" );
                break;
            }
        }
    }

    return !vecFlowErrors.empty();
}

bool PlanNode::isUpdated()
{
    return m_areStepsUpdated || m_isInfoUpdated;
}

bool PlanNode::isCopiedPlan()
{
    return m_isCopiedPlan;
}

void PlanNode::createClassicSequenceFlows( PlanStepList& vecSteps )
{
    if ( getType() != TA_Base_Core::PLAN_NODE )
    {
        return;
    }

    // Clear all sequence flow; Solution for compatible with classic plan step editor
    getSequenceFlowMap().clearSequenceFlow();

    unsigned int unIncomingStep = 0u;
    unsigned int unParallelIncomingStep = 0u;
    std::vector<unsigned int> vecFlowSteps;
    PlanStep* pParallelStep = NULL;

    for ( PlanStepList::iterator itLoop = vecSteps.begin(); vecSteps.end() != itLoop; ++itLoop )
    {
        PlanStep* pCurStep = *itLoop;

        if ( NULL == pCurStep )
        {
            continue;
        }

        unsigned int unStepId = pCurStep->getStepId();

        // Create sequence flow between previous step and current step
        if ( 0u != unIncomingStep )
        {
            getSequenceFlowMap().insertSequenceFlow( getSequenceFlowMap().getNewId(), unIncomingStep, unStepId );
        }

        unIncomingStep = 0u;

        // Handling parallel block
        if ( NULL != pParallelStep )
        {
            // Check whether is parallel end step
            if ( pCurStep->canParallelHandle() )
            {
                // Update parameters for parallel steps
                pCurStep->clearFlowParameters();
                pParallelStep->clearFlowParameters();

                for ( std::vector<unsigned int>::iterator itStepId = vecFlowSteps.begin(); vecFlowSteps.end() != itStepId; ++itStepId )
                {
                    if ( 0u == *itStepId )
                    {
                        // Do not create sequence flow for invalid step
                        continue;
                    }

                    // Insert outgoing flow for start parallel gateway step
                    getSequenceFlowMap().insertSequenceFlow( getSequenceFlowMap().getNewId(), pParallelStep->getStepId(), *itStepId );

                    // Update outgoing parameter
                    pParallelStep->outgoingStepChanged( 0u, *itStepId );

                    // Insert incoming flow for end parallel gateway step
                    getSequenceFlowMap().insertSequenceFlow( getSequenceFlowMap().getNewId(), *itStepId, unStepId );

                    // Update incoming parameter
                    pCurStep->incomingStepChanged( 0u, *itStepId );
                }

                PlanStepList::iterator itOutgoing = itLoop + 1;
                if (( vecSteps.end() != itOutgoing ) && ( NULL != *itOutgoing ))
                {
                    // Update outgoing parameter
                    pCurStep->outgoingStepChanged( 0u, (*itOutgoing)->getStepId() );
                }

                if ( 0u != unParallelIncomingStep )
                {
                    // Update incoming parameter
                    pParallelStep->incomingStepChanged( 0u, unParallelIncomingStep );
                    unParallelIncomingStep = 0u;
                }

                unIncomingStep = unStepId;

                // Current is parallel end step, reset parallel start step
                pParallelStep = NULL;
                vecFlowSteps.clear();
            }
            else
            {
                if ( !pCurStep->hasCustomOutgoingStep() )
                {
                    // Current is step required parallel unless it has custom outgoing step
                    vecFlowSteps.push_back( unStepId );
                }
                else
                {
                    pCurStep->resetFlowParameters();
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot insert step [%u] with multiple outgoing between two parallel gateway steps", unStepId );
                }
            }
        }
        // Check whether parallel block starts
        else if( pCurStep->canParallelHandle() )
        {
            // Set current step as parallel start step
            pParallelStep = pCurStep;

            PlanStepList::iterator itIncoming = itLoop - 1;
            if (( vecSteps.end() != itIncoming ) && ( NULL != *itIncoming ))
            {
                unParallelIncomingStep = (*itIncoming)->getStepId();
            }
        }
        // Handling normal steps
        else
        {
            std::vector<unsigned int> vecOutgoings;
            if ( pCurStep->retrieveOutgoings( vecOutgoings ))
            {
                // Create custom outgoing step sequence flow
                for ( std::vector<unsigned int>::iterator itOutLoop = vecOutgoings.begin(); vecOutgoings.end() != itOutLoop; ++itOutLoop )
                {
                    if ( 0u == *itOutLoop )
                    {
                        // Do not create sequence flow for invalid step
                        continue;
                    }

                    getSequenceFlowMap().insertSequenceFlow( getSequenceFlowMap().getNewId(), unStepId, *itOutLoop );
                }
            }
            else
            {
                unIncomingStep = unStepId;
            }
        }
    }
}

}