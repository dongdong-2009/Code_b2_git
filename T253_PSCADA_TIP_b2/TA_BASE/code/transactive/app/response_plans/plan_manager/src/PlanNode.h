/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanNode.h $
  * @author:  Bart Golab
  * @version: $Revision: #36 $
  *
  * Last modification: $DateTime: 2015/11/02 18:24:29 $
  * Last modified by:  $Author: qi.huang $
  *
  * This is a representation of a plan node within the plan tree.
  *
  **/

#if !defined(AFX_PLANNODE_H__7FC18459_AC18_4BBF_B5A7_39BA7FD76656__INCLUDED_)
#define AFX_PLANNODE_H__7FC18459_AC18_4BBF_B5A7_39BA7FD76656__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_editor_plugin/workflow_editor/src/IWorkflowOwner.h"
#include "app/response_plans/plan_editor_plugin/workflow_editor/src/WorkflowStructure.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "TreeNode.h"
#include "PlanInstanceNone.h"


namespace TA_Base_App
{
    class CPlanGeneralDetailPage;
    class CPlanStepDetailPage;
    class CPlanStepListCtrl;
    class CPlanInstanceComboBox;
    class InteractiveStep;
    class PlanStep;
    class PlanStepMap;
    class SequenceFlowMap;
    class PlanInstanceMap;
    class PlanInstanceFilter;
	class IPlanNode;
	class PlanStep;


    /////////////////////////////////////////////////////////////////////////////
    // PlanNode

    class PlanNode : public TreeNode, public IWorkflowOwner, public IPlanNode
    {
    // Operations
    public:
        // Constructors and destructors
        PlanNode(const TA_Base_Core::NodeSummary& nodeSummary);
        PlanNode(const PlanNode& planNode); // Copy constructor (deep copy)
        virtual ~PlanNode();

        // Summary info getters
        virtual const std::string &getNodeType();   // Returned by reference to enable setting CategoryNodeView item labels by callback
        virtual const std::string &getNodeStatus(); // Returned by reference to enable setting CategoryNodeView item labels by callback

        // Get plan step details
        //PlanStep* getPlanStepByPos( unsigned long stepNum );
		virtual PlanStep* getPlanStepByStepId(unsigned long stepId);
		virtual std::vector<PlanStep*> getPlanSteps();
        virtual std::map<unsigned int, UintSet> getSequenceFlows();
        virtual unsigned long getPlanNodeId();
        unsigned long getLastStepPosition();

        bool isUpdated();

        // Plan management functions
        void approvePlan(bool isApproved);
        void schedulePlan();
        void deletePlan();
        void savePlan();
        void upgradePlan();
        TA_Base_Core::AuxPlanDetail* exportPlan();
        void insertPlanStep(const int stepType, unsigned long position);
        void deletePlanSteps(PlanStepList &deleteSteps);
        void copyPlanSteps(const PlanStepList &copySteps, unsigned long position);
        void movePlanSteps(const PlanStepList &moveSteps, unsigned long position);

        // Plan execution control
        void startNewInstance();
        void customiseNewInstance();
        void executeInteractiveStep(InteractiveStep& interactiveStep);
        void controlPlan(bool ignoreActivePlanControllerLimit = false);
        void controlPlan(const PlanInstanceId &planInstanceId);

        // Plan editing
        virtual bool isCopiedPlan();
        void editPlan();
        void setPlanType(const TA_Base_Core::ENode eType);
        void setPlanName(const std::string& name);
        void setPlanDescription(const std::string& description);
        bool editPlanStep(PlanStep* planStep);
        void notifyPlanGeneralDetailUpdate();
        void notifyPlanStepUpdate(unsigned long stepNumber, unsigned long stepCount);

        void updateGraph( const std::map<unsigned int, ActivityData>& mapGraph, const std::map<unsigned int, SequenceFlowData>& mapFlow );
        void retrieveGraph( std::map<unsigned int, ActivityData>& mapGraph, std::map<unsigned int, SequenceFlowData>& mapFlow );

        // Active instance access
        PlanInstanceSharedPtr getPlanInstance(const PlanInstanceId &planInstanceId);

        // Return runtime class of the view used to display plan node details
        virtual CRuntimeClass *getNodeViewClass();

        // Enable actions that can be performed on this plan
        virtual bool isEnable(const int nAction);

        // Return an image type for the corresponding tree item
        virtual ETreeNodeImageType getNodeImageType(bool isExpanded);

        // Window controls modifiers
        virtual void addChildrenToTree(CTreeCtrl &treeCtrl, HTREEITEM parent, ITreeNodeFilter *filter = NULL);
        virtual void addChildrenToList(CListCtrl &listCtrl, ITreeNodeFilter *filter = NULL);
        virtual void updatePlanView(CPlanGeneralDetailPage &generalDetailPage, const PlanInstanceFilter &filter);
        virtual void updatePlanView(CPlanStepDetailPage &stepDetailPage, const PlanInstanceFilter &filter);
        virtual void updatePlanInstanceList(CPlanInstanceListCtrl &instanceListCtrl, const PlanInstanceFilter &filter);
        virtual void updatePlanInstanceCombo( CPlanInstanceComboBox &instanceComboBox, const PlanInstanceFilter &filter );
        virtual void addNoneInstance( CPlanInstanceComboBox &instanceComboBox );
        virtual void addStepsToStepList(CPlanStepListCtrl& planStepsListCtrl);

        // Facilitate in-tree renaming of nodes.
        virtual void renameNode(std::string nodeName);

        // Active plan, plan config and plan customization update handlers
        virtual void clearPlanInstances( const unsigned long ulLocationKey );
        virtual void updatePlanInstance(const TA_Base_Core::ActivePlanDetail &activePlanDetail);
        virtual void updatePlanInstance(const TA_Base_Core::PlanExecuteStepUpdate &activeStepDetail);
        virtual void updatePlanInstance(const TA_Base_Core::PlanCustomisationUpdate &planCustomDetail);
        virtual void updateNode(const TA_Base_Core::PlanConfigUpdateData &planConfigData);
        virtual void updateChildNodeList(const TA_Base_Core::PlanConfigUpdateData &planConfigData);

        // interface for IWorkflowOwner
        virtual void activityCreated( const int nType, const unsigned int unId );
        virtual void activityDeleted( const unsigned int unId );
        virtual void activityUpdated( const unsigned int unId );
        virtual void activityCopied( const unsigned int unFrom, const unsigned int unTo );
        virtual void flowCreated( const unsigned int unId );
        virtual void flowDeleted( const unsigned int unId );
        virtual void linkFlowFrom( const unsigned int unFlow, const unsigned int unActivity );
        virtual void linkFlowTo( const unsigned int unFlow, const unsigned int unActivity );
        virtual IActivityParameter& getActivityParameter( const unsigned int unId );

    protected:
        virtual bool hasPrecedence(TreeNode &siblingNode);
        virtual TA_Base_Core::PlanDetail &getPlanDetail();
        virtual PlanInstanceMap &getPlanInstanceMap(LocationKey location);
        virtual PlanInstanceMap &getLocalPlanInstanceMap();
        virtual PlanInstanceMap &getRemotePlanInstanceMap();
        virtual PlanStepMap &getPlanStepMap();
        virtual SequenceFlowMap &getSequenceFlowMap();
        virtual void createMapsIfRequired();
        virtual void invalidatePlanStepMap();

        bool hasInvalidParameter( PlanStep& refStep, std::vector<std::string>& vecParamNames );
        bool hasInvalidFlow( PlanStep& refStep, std::vector<std::string>& vecFlowErrors );

        void createClassicSequenceFlows( PlanStepList& refSteps );

    private:
        PlanNode &operator=(const PlanNode &other);

    // Attributes
    private:
        bool m_isCopiedPlan;
        bool m_isInfoUpdated;
        bool m_areStepsUpdated;
        PlanInstanceMap* m_localPlanInstanceMap;   // instances at the profile locations
        PlanInstanceMap* m_remotePlanInstanceMap;  // instances at all other locations
        PlanStepMap* m_planStepMap;
        SequenceFlowMap* m_sequenceFlowMap;
        PlanInstanceNone m_planInstanceNone;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_PLANNODE_H__7FC18459_AC18_4BBF_B5A7_39BA7FD76656__INCLUDED_)
