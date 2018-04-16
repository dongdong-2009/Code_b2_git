#ifndef PLANAGENTCORBADEFIMPL_H_INCLUDED
#define PLANAGENTCORBADEFIMPL_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanAgentCorbaDefImpl.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #16 $
  *
  *  Last modification: $DateTime: 2015/08/27 16:34:07 $
  *  Last modified by:  $Author: huangjian $
  *
  *  <description>
  */

#include "core/corba/src/ServantBase.h"
#include "bus/response_plans/plan_agent/IDL/src/IPlanAgentCorbaDef.h"

namespace TA_Base_App
{
	class PlanAgentUser;
	//Class declaration
    class PlanAgentCorbaDefImpl : public virtual POA_TA_Base_Bus::IPlanAgentCorbaDef, public virtual TA_Base_Core::ServantBase
	{
	// Public methods
	public:
		PlanAgentCorbaDefImpl(PlanAgentUser * parentClass);
		virtual ~PlanAgentCorbaDefImpl();

		// CORBA interface
        virtual CORBA::Boolean isOwner(const char* session, const TA_Base_Core::ActivePlanId& plan);
        virtual void processStepResponse(const char* session, const ::TA_Base_Core::ActivePlanId& plan, const ::TA_Base_Core::InteractiveStepData& stepDetail);
        virtual void runPlan(const char* session, TA_Base_Core::ActivePlanId& plan);
        virtual unsigned long runPlanByName(const char* session, const char* path, const char* param = NULL);
		virtual unsigned long runPlanByNameWithoutActionPermittedCheck(const char* session, const char* path, const char* param = NULL); // CL-20368
		virtual unsigned long runPlanByNameWithEntityRestriction( const char* session, const char* path, unsigned long entitykey); // CL-13527
        virtual void runOperatorTriggeredPlan(const char* session, TA_Base_Core::ActivePlanId& plan);
        virtual void customisePlan(const char* session, TA_Base_Core::ActivePlanId& plan);
        virtual void pausePlan(const char* session, const TA_Base_Core::ActivePlanId& plan);
        virtual void resumePlan(const char* session, const TA_Base_Core::ActivePlanId& plan);
        virtual void stopPlan(const char* session, const TA_Base_Core::ActivePlanId& plan);
        virtual void takeControl(const char* session, const TA_Base_Core::ActivePlanId& plan);
        virtual void customiseStep(const char* session, const TA_Base_Core::ActivePlanId& plan, const TA_Base_Core::StepCustomisableDetail& detail);
        virtual void getPlanExecutionHistory(const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::ExecutionHistory_out history);
        virtual void getPlanCustomisationHistory(const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::CustomisationHistory_out history);
        virtual void setPlanApprovalState(const char* session, TA_Base_Core::NodeId plan, TA_Base_Core::EApprovalState state);
        virtual void setPlanType(const char* session, TA_Base_Core::NodeId plan, TA_Base_Core::EPlanType state);
        virtual void getRootNodeChildren(TA_Base_Core::SiblingNodes_out children);
        virtual void getChildren(TA_Base_Core::NodeId parent, TA_Base_Core::SiblingNodes_out children);
        virtual void getNodeDetail(TA_Base_Core::NodeId node, TA_Base_Core::NodeDetail_out detail);
        virtual void getNodeSummary(TA_Base_Core::NodeId node, TA_Base_Core::NodeSummary_out summary);
        virtual void getNodePath(TA_Base_Core::NodeId node, TA_Base_Core::NodePath_out path);
        virtual void getNodeIdsInNodePath(TA_Base_Core::NodeId plan, TA_Base_Core::NodeIds_out plans);
        virtual TA_Base_Core::NodeId getNodeId(const char* path);
        virtual void getActivePlanDetail(TA_Base_Core::NodeId plan, TA_Base_Core::ActivePlanDetails_out details);
        virtual void getAssocAlarmPlanDetail(const char* alarmId, TA_Base_Core::ActivePlanDetail_out detail);
        virtual void getActivePlanDetailByName(const char* path, TA_Base_Core::ActivePlanDetails_out details);
        virtual void getAllActivePlanDetails(TA_Base_Core::ActivePlanStepsDetails_out details);
        virtual void getEditPlanLock(const char* session, TA_Base_Core::NodeId plan);
        virtual void releaseEditPlanLock(const char* session, TA_Base_Core::NodeId plan);
        virtual void getStepNames(TA_Base_Core::StepNames_out names);
        virtual void getAllStepDetails(TA_Base_Core::NodeId plan, TA_Base_Core::StepDetails_out details);
        virtual void getSpecifiedStepDetails(TA_Base_Core::NodeId plan, const TA_Base_Core::StepIds& steps, TA_Base_Core::StepDetails_out details);
        virtual void getStepPosition( TA_Base_Core::NodeId planId, TA_Base_Core::StepId stepId, TA_Base_Core::StepNumber& position );
        virtual void getSequenceFlows( TA_Base_Core::NodeId plan, TA_Base_Core::SequenceFlows_out flows );
        virtual void getParameterNames( CORBA::Long stepType, TA_Base_Core::ParameterNames_out names );
        virtual void getParametersByStepNumber( TA_Base_Core::NodeId plan, TA_Base_Core::StepNumber position, TA_Base_Core::StepParameters_out parameters );
        virtual void getParametersByStepId( TA_Base_Core::StepId step, TA_Base_Core::StepParameters_out parameters );
        virtual void updateCategoryDetail(const char* session, const TA_Base_Core::CategoryDetail& detail);
        virtual void insertCategory(const char* session, TA_Base_Core::NodeId parent);
        virtual void insertCategoryIntoRoot(const char* session);
        virtual void deleteCategory(const char* session, TA_Base_Core::NodeId category);
        virtual void updatePlanDetail(const char* session, const TA_Base_Core::PlanDetail& detail);
        virtual void updatePlan(const char* session, const TA_Base_Core::Plan& plan);
        virtual void insertPlan(const char* session, TA_Base_Core::NodeId parent, TA_Base_Core::ENode nodeType);
        virtual void insertPlanIntoRoot(const char* session, TA_Base_Core::ENode nodeType);
        virtual void deletePlans(const char* session, const TA_Base_Core::NodeIds& plans);
        virtual void movePlan(const char* session, TA_Base_Core::NodeId fromPlan, TA_Base_Core::NodeId toParent, bool useForce);
        virtual void movePlanToRoot(const char* session, TA_Base_Core::NodeId plan, bool useForce);
        virtual void copyPlanToRoot(const char* session, TA_Base_Core::NodeId plan, bool useForce);
        virtual void copyPlan(const char* session, TA_Base_Core::NodeId fromPlan, TA_Base_Core::NodeId toParent, bool useForce);
        virtual void getNextStepId(const char* session, TA_Base_Core::StepId& step);
		virtual void exportPlan (const char* session, TA_Base_Core::AuxPlanDetail_out plan, TA_Base_Core::NodeId planId);
		virtual void importPlan (const char* session, const TA_Base_Core::AuxPlanDetail& plan, TA_Base_Core::NodeId parentId);
	private:
		//TD18706,jianghp.
		void checkOperationMode();
        void runPlan( const char* session, TA_Base_Core::ActivePlanId& plan, const unsigned long ulTriggerEntity, const bool bUniqueTrigger, const bool checkActionPermitted );
	
	private:
		PlanAgentCorbaDefImpl& operator=( const PlanAgentCorbaDefImpl& );
		PlanAgentCorbaDefImpl( const PlanAgentCorbaDefImpl& );
		PlanAgentUser * m_parentClass;
	}; // PlanAgentCorbaDefImpl

}; // TA_Base_App

#endif // PLANAGENTCORBADEFIMPL_H_INCLUDED
