/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanDataWriter.cpp $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #24 $
  *
  *  Last modification: $DateTime: 2015/11/02 18:24:29 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  A thread-safe Singleton factory for updating database and adding into database.
  *
  */
#include <iomanip>

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/PlanAgentException.h"
#include "core/exceptions/src/PlanStepDetailException.h"
#include "bus/mathematical_evaluation/src/ExpressionEvaluator.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataWriter.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataReader.h"
#include "app/response_plans/plan_agent/src/PlanNotFoundException.h"
#include "app/response_plans/plan_agent/src/AAMessageSender.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/PlanDataWriter.h"
#include "app/response_plans/plan_agent/src/StepImplementationFactory.h"

namespace TA_Base_App
{
    const unsigned long PLANEDITLOCK_TIMEOUT_SECONDS = 60;
    const char* const PLAN_EDIT_LOCK_SEPARATOR = ":";

    PlanDataWritingClass::PlanDataWritingClass()
    {
        updatePlansDBSequence();
    }


    void PlanDataWritingClass::updatePlansDBSequence()
    {
        TA_Base_Bus::PlanDataWriter::instance()->UpdatePlansDBSequence();
    }

    void PlanDataWritingClass::getEditPlanLock(const std::string& session, TA_Base_Core::NodeId plan, bool noDBAccess /*=false*/)
    {
        if(!TA_Base_Bus::PlanDataReader::instance()->existsNode(plan))
        {
            TA_THROW(TA_Base_App::PlanNotFoundException("The plan does not exist.", TA_Base_Core::PNFE_NODE_NOT_EXIST));
        }

        std::string requestingLockOwner = session;

        // TD8505: Merged fix from 4619.
        if(!noDBAccess)
        {
            std::string exisingLockOwner = setEditLockInDataBase(requestingLockOwner, plan);

            if(exisingLockOwner.compare(requestingLockOwner) != 0)
            {
                std::string owner = ActivePlanBulletinBoard::instance()->getOwner(exisingLockOwner);

                std::ostringstream what;
                what << "The plan is already being edited by the operator: " << owner;

                TA_THROW(TA_Base_Core::PlanAgentException(what.str(), TA_Base_Core::PAE_EDIT_LOCKED_BY_OPERATOR, ActivePlanBulletinBoard::instance()->getOwnerId(exisingLockOwner)));
            }
        }

        //Send StateUpdate for Plan being edited
        TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData;

        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
        TA_Base_Core::EditLockUpdate_var elu = new TA_Base_Core::EditLockUpdate;

        elu->session = CORBA::string_dup(session.c_str());
        elu->plan = plan;

        pasUpdateData->update.getLockUpdate(elu);

        ActivePlanBulletinBoard::instance()->sendStateUpdateMessage(pasUpdateData);
    }


    void PlanDataWritingClass::releaseEditPlanLock(const std::string& session, TA_Base_Core::NodeId plan, bool noDBAccess /*=false*/)
    {
        if(!TA_Base_Bus::PlanDataReader::instance()->existsNode(plan))
        {
            TA_THROW(TA_Base_App::PlanNotFoundException("The plan does not exist.", TA_Base_Core::PNFE_NODE_NOT_EXIST));
        }

        // TD8505: Merged fix from 4619.
        if(!noDBAccess)
        {
            releaseEditLockInDatabase(session, plan);
        }

        //Send StateUpdate for release Plan being edited
        TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData;

        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
        TA_Base_Core::EditLockUpdate_var elu = new TA_Base_Core::EditLockUpdate;

        elu->session = CORBA::string_dup(session.c_str());
        elu->plan = plan;

        pasUpdateData->update.releaseLockUpdate(elu);

        ActivePlanBulletinBoard::instance()->sendStateUpdateMessage(pasUpdateData);
    }

    std::string PlanDataWritingClass::getEditLockOwner(TA_Base_Core::NodeId plan)
    {
        return TA_Base_Bus::PlanDataWriter::instance()->getEditLockOwner(plan);
    }

    bool PlanDataWritingClass::isPlanBeingEdited(TA_Base_Core::NodeId plan)
    {
        try
        {
            return TA_Base_Bus::PlanDataWriter::instance()->isPlanBeingEdited(plan);
        }
        catch(TA_Base_Core::DataException& e)
        {
            if(e.getFailType() == TA_Base_Core::DataException::NO_VALUE)
            {
                TA_THROW(TA_Base_App::PlanNotFoundException("The plan does not exist.", TA_Base_Core::PNFE_NODE_NOT_EXIST));
            }
            TA_THROW(e);
        }
    }


    std::string PlanDataWritingClass::setEditLockInDataBase(const std::string& session, TA_Base_Core::NodeId plan)
    {
        try
        {
            return TA_Base_Bus::PlanDataWriter::instance()->setEditLockInDataBase(session, plan);
        }
        catch(TA_Base_Core::DataException& e)
        {
            if(e.getFailType() == TA_Base_Core::DataException::NO_VALUE)
            {
                TA_THROW(TA_Base_App::PlanNotFoundException("The plan does not exist.", TA_Base_Core::PNFE_NODE_NOT_EXIST));
            }
            TA_THROW(e);
        }
    }


    void PlanDataWritingClass::releaseEditLockInDatabase(const std::string& session, TA_Base_Core::NodeId plan)
    {
        try
        {
            TA_Base_Bus::PlanDataWriter::instance()->releaseEditLockInDatabase(session, plan);
        }
        catch(TA_Base_Core::DataException& e)
        {
            if(e.getFailType() == TA_Base_Core::DataException::NO_VALUE)
            {
                std::ostringstream message;
                message << "The plan " << plan << "does not exist.";
                TA_THROW(TA_Base_App::PlanNotFoundException(message.str(), TA_Base_Core::PNFE_NODE_NOT_EXIST));
            }
            TA_THROW(e);
        }
        catch(TA_Base_Core::PlanAgentException& e)
        {
            if(e.getErrorType() == TA_Base_Core::PAE_EDIT_LOCKED_BY_OPERATOR)
            {
                std::string lockOwner = e.what();
                std::ostringstream message;
                message << "Error: Cannot release the edit lock owned by " << e.what() << ".";
                TA_THROW(TA_Base_Core::PlanAgentException(lockOwner, TA_Base_Core::PAE_EDIT_LOCKED_BY_OPERATOR, ActivePlanBulletinBoard::instance()->getOwnerId(lockOwner)));
            }
            TA_THROW(e);
        }

    }


    bool PlanDataWritingClass::refreshEditLockInDatabase(const std::string& session, TA_Base_Core::NodeId plan)
    {
        return true;
    }


    void PlanDataWritingClass::checkPlanEditSession(TA_Base_Core::NodeId plan, const std::string& session)
    {
        std::string lockOwner;
        try
        {
            lockOwner = TA_Base_Bus::PlanDataWriter::instance()->getPlanEditSession(plan, session);
        }
        catch(TA_Base_Core::DataException& e)
        {
            if(e.getFailType() == TA_Base_Core::DataException::NO_VALUE)
            {
                TA_THROW(TA_Base_App::PlanNotFoundException("The plan does not exist.", TA_Base_Core::PNFE_NODE_NOT_EXIST));
            }
            TA_THROW(e);
        }

        if(lockOwner.find(session) == 0) return;

        std::string owner = ActivePlanBulletinBoard::instance()->getOwner(lockOwner);

        std::ostringstream what;
        what << "The plan is being edited by another operator: " << owner;

        TA_THROW(TA_Base_Core::PlanAgentException(what.str(), TA_Base_Core::PAE_EDIT_LOCKED_BY_OPERATOR, ActivePlanBulletinBoard::instance()->getOwnerId(lockOwner)));
    }



    void PlanDataWritingClass::validatePlan(const TA_Base_Core::Plan& plan)
    {
        std::ostringstream message;
        TA_Base_Core::NodeId nodeId = plan.detail.plan;
        TA_Base_Core::NodeId parent = plan.detail.parent;

        const TA_Base_Core::PlanDetail* planDetail = &(plan.detail);
        TA_Base_Core::ENode parentNodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(planDetail->parent);

        if(parentNodeType != TA_Base_Core::CATEGORY_NODE)
        {
            message << "Error: The parent node is not of type CATEGORY.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_PARENT_NOT_CATEGORY));
        }

        std::string name = planDetail->name.in();
        std::string description = planDetail->description.in();
        TA_Base_Core::EApprovalState approvalState = planDetail->approvalState;

        if(name.empty())
        {
            TA_THROW(TA_Base_Core::PlanAgentException("The name of Plan cannot be blank.", TA_Base_Core::PAE_NO_PLAN_NAME));
        }

        //see if file has any file-path disallowed characters ( \ / : * ? " < > | )
        if(name.find("\\") != std::string::npos || name.find("/") != std::string::npos ||
           name.find(":") != std::string::npos || name.find("*") != std::string::npos ||
           name.find("?") != std::string::npos || name.find("\"") != std::string::npos ||
           name.find("<") != std::string::npos || name.find(">") != std::string::npos || name.find("|") != std::string::npos)
        {
            TA_THROW(TA_Base_Core::PlanAgentException("The name of Plan cannot contain any of the reserved characters - \\ / : * ? \" < > |", TA_Base_Core::PAE_INVALID_PLAN_NAME));
        }

        if(!TA_Base_Bus::PlanDataReader::instance()->canUseName(nodeId, parent, parseForApostrophe(name)))
        {
            message.str("");
            message << "Error: The name " << name.c_str() << " is already used by another Plan/Category.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NAME_NOT_UNIQUE));
        }

        if(TA_Base_Core::UNAPPROVED_APPROVAL_STATE != approvalState)
        {
            TA_THROW(TA_Base_Core::PlanAgentException("Error: Plan can only be saved in an Unapproved state.", TA_Base_Core::PAE_CANNOT_SAVE_APPROVED_PLAN));
        }

        int numberOfSteps = plan.stepseq.length();
        const TA_Base_Core::StepDetail* stepDetail = NULL;
        const TA_Base_Core::StepParameters* stepParameters = NULL;

        for(int i = 0; i < numberOfSteps; i++)
        {
            stepDetail = &(plan.stepseq[i].detail);
            stepParameters = &(plan.stepseq[i].parameters);

            std::string stepName = stepDetail->name.in();
            unsigned long stepPosition = stepDetail->position;

            if(stepName.empty())
            {
                TA_THROW(TA_Base_Core::PlanStepDetailException("Step name cannot be blank", "step name", TA_Base_Core::PSDE_UNSPECIFIED, i + 1));
            }

            if(stepPosition != i + 1)
            {
                message << "Error: The position parameter in step detail of step " << i + 1 << " is " << stepPosition;
                TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), "postion", TA_Base_Core::PSDE_INVALID, i + 1));
            }
        }

        for(int m = 0; m < numberOfSteps; m++)
        {
            const TA_Base_Core::Step& refStep = plan.stepseq[m];

            std::vector<std::string> vecParameters;
            for ( size_t szLoop = 0u; szLoop < refStep.parameters.length(); ++szLoop )
            {
                vecParameters.push_back( refStep.parameters[szLoop].in() );
            }

            ActiveStepFactory::instance()->validateStepParameter( refStep.detail.type, vecParameters );
        }
    }


    void PlanDataWritingClass::updatePlanDetail(const std::string& session, const TA_Base_Core::PlanDetail& detail)
    {
        std::ostringstream message;
        TA_Base_Core::NodeId nodeId = detail.plan;

        //Check whether the Plan is being edited by the current session user
        checkPlanEditSession(nodeId, session);

        TA_Base_Core::ENode nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(nodeId);

        if (( nodeType != TA_Base_Core::PLAN_NODE ) && ( nodeType != TA_Base_Core::WORKFLOW_NODE ))
        {
            message << "Error: The node [" << detail.plan << "] is not of type PLAN.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
        }

        TA_Base_Bus::PlanDataWriter::instance()->updatePlanDetail(session, detail);

        TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
        TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(nodeId);
        pcud->configUpdate.updatePlan(ns);

        ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage(*pcud._retn());

        std::string strPlanName = TA_Base_Bus::PlanDataReader::instance()->getNodePath(nodeId);
        AAMessageSender::instance()->submitAuditPlanUpdate(session, strPlanName);
    }


    void PlanDataWritingClass::updatePlan(const std::string& session, const TA_Base_Core::Plan& plan)
    {
        std::ostringstream message;

        TA_Base_Core::NodeId nodeId = plan.detail.plan;

        //Check whether the Plan is being edited by the current session user
        checkPlanEditSession(nodeId, session);

        TA_Base_Core::ENode nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(nodeId);

        if (( nodeType != TA_Base_Core::PLAN_NODE ) && ( nodeType != TA_Base_Core::WORKFLOW_NODE ))
        {
            message << "Error: The node [" << plan.detail.name << "] is not of type PLAN.";
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", message.str().c_str());
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
        }

        //Make the Plan pass through some sanity checks before updating
        validatePlan(plan);

        TA_Base_Bus::PlanDataWriter::instance()->updatePlanDetail(session, plan.detail);

		size_t numberOfSteps = plan.stepseq.length();

		if(numberOfSteps > 0)
		{
			TA_Base_Bus::PlanDataWriter::instance()->deleteAllStepParameters(session, nodeId);
			TA_Base_Bus::PlanDataWriter::instance()->deleteAllSteps(session, nodeId);

			// We are here now to insert all the steps into step_details
			for ( size_t m = 0u; m < numberOfSteps; m++ )
			{
				//Add Step into step_details
				const TA_Base_Core::StepDetail& stepDetail = plan.stepseq[m].detail;

				TA_Base_Bus::PlanDataWriter::instance()->insertStepDetail(nodeId, stepDetail);

				const TA_Base_Core::StepParameters& stepParameters = plan.stepseq[m].parameters;
				std::vector<std::string> valueVector; //To store the modified param values
				for(size_t nParaLoop = 0u; nParaLoop < stepParameters.length(); ++nParaLoop)
				{
					valueVector.push_back(stepParameters[nParaLoop].in());
				}

				//  update step_parameters
				TA_Base_Bus::PlanDataWriter::instance()->insertStepParameters(stepDetail.pkey, valueVector);
			}

			TA_Base_Bus::PlanDataWriter::instance()->deleteAllSequenceFlows( session, nodeId );

			for ( size_t m = 0u; m < plan.flowseq.length(); m++ )
			{
				TA_Base_Bus::PlanDataWriter::instance()->insertSequenceFlow( nodeId, plan.flowseq[m] );
			}
		}

        std::string strPlanName = TA_Base_Bus::PlanDataReader::instance()->getNodePath( nodeId );
        AAMessageSender::instance()->submitAuditPlanUpdate(session, strPlanName);

        TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
        TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary( nodeId );
        pcud->configUpdate.updatePlan( ns );
        ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage(*pcud._retn());
    }

    void PlanDataWritingClass::copyPlan(const std::string& session, TA_Base_Core::NodeId plan, TA_Base_Core::NodeId parent, bool useForce)
    {
        std::ostringstream message;

        TA_Base_Core::NodeSummary_var oldSummary = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(plan);

        if (( oldSummary->type != TA_Base_Core::PLAN_NODE ) && ( oldSummary->type != TA_Base_Core::WORKFLOW_NODE ))
        {
            message << "Error: The node [" << plan << "] is not of type PLAN.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
        }

        TA_Base_Core::ENode nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(parent);

        if(nodeType != TA_Base_Core::CATEGORY_NODE)
        {
            message << "Error: The given parent node [" << parent << "] is not of type CATEGORY.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
        }

        std::string name = oldSummary->name.in();
        bool overwriting = false;
        TA_Base_Core::NodeId existingPlan = 0;

        if(oldSummary->parent != parent)
        {
            // if we have a conflicting plan
            if(!TA_Base_Bus::PlanDataReader::instance()->canUseName(plan, parent, parseForApostrophe(name)))
            {
                // get the ID of the existing plan
                existingPlan = getPlanToReplace(name, parent, useForce);
                overwriting = true;
            }
        }
        else
        {
            name = getNodeNameToCopy(plan, parent);
        }

        TA_Base_Core::NodeId newPlan = 0;
        try
        {
            if(overwriting)
            {
                TA_Base_Bus::PlanDataWriter::instance()->replacePlan(session, existingPlan, plan);
            }
            else
            {
                TA_Base_Bus::PlanDataWriter::instance()->copyPlan(session, plan, parent, name);
            }

            // We have successfully copied the plan, let us retrieve its NodeId and send the update
            newPlan = TA_Base_Bus::PlanDataReader::instance()->getNodeId(parent, parseForApostrophe(name));
        }
        catch(...)
        {
            std::string strPlanName = TA_Base_Bus::PlanDataReader::instance()->getNodePath(plan);
            std::string destinationPlanName = TA_Base_Bus::PlanDataReader::instance()->getNodePath(parent);
            destinationPlanName += "/";
            destinationPlanName += TA_Base_Bus::PlanDataReader::instance()->getNodeName(plan);
            AAMessageSender::instance()->submitAuditPlanCopyFailure(session, strPlanName, destinationPlanName);

            std::ostringstream alarmMessage;
            alarmMessage << "Plan Copy Failure:" << strPlanName << "[Id:" << plan << "]";
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", alarmMessage.str().c_str());

            TA_THROW(TA_Base_Core::DataException("Failed to copyPlan", TA_Base_Core::DataException::NO_VALUE, ""));
        }


        TA_Base_Core::PlanConfigUpdateData pcud;
        pcud.session = CORBA::string_dup(session.c_str());
        std::string sourcePlanPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(plan);
        std::string targetPlanPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(newPlan);

        // When replacing an existing plan, send a delete update before the copy update.
        if(overwriting)
        {
            TA_Base_Core::DeleteNodeUpdate dnu;
            dnu.parent = parent;
            dnu.nodes.length(1);
            dnu.nodes[0] = existingPlan;

            if(parent != TA_Base_Core::ROOT_NODE_ID)
            {
                pcud.configUpdate.deletePlans(dnu);
            }
            else
            {
                pcud.configUpdate.deletePlansFromRoot(dnu.nodes);
            }

            ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage(pcud);

            AAMessageSender::instance()->submitAuditPlanDelete(session, targetPlanPath);

            std::ostringstream auditMessage;
            auditMessage << targetPlanPath << " [Id:" << existingPlan << "] has been deleted by overwriting " << sourcePlanPath << " [Id:"  << plan << "].";
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", auditMessage.str().c_str());
        }

        TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(newPlan);

        if(parent != TA_Base_Core::ROOT_NODE_ID)
        {
            pcud.configUpdate.copyPlan(ns);
        }
        else
        {
            pcud.configUpdate.copyPlanToRoot(ns);
        }

        ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage(pcud);

        AAMessageSender::instance()->submitAuditPlanCopy(session, sourcePlanPath, targetPlanPath);

        std::ostringstream auditMessage;
        auditMessage << sourcePlanPath << " [Id:" << plan << "] has been copied to " << targetPlanPath << " [Id:" << newPlan << "].";

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", auditMessage.str().c_str());
    }


    TA_Base_Core::AuxPlanDetail* PlanDataWritingClass::exportPlan(const std::string& session, TA_Base_Core::NodeId& planId)
    {
        return TA_Base_Bus::PlanDataWriter::instance()->exportPlan(session, planId);
    }

    void PlanDataWritingClass::importPlan(const std::string& session, const TA_Base_Core::AuxPlanDetail& plan, TA_Base_Core::NodeId parentId)
    {
        TA_Base_Core::NodeId planId = TA_Base_Bus::PlanDataWriter::instance()->importPlan(session, plan, parentId);

        //Send Update for insertPlan
        TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
        pcud->session = CORBA::string_dup(session.c_str());

        TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(planId);

        if(parentId != TA_Base_Core::ROOT_NODE_ID)
        {
            pcud->configUpdate.insertPlan(ns);
        }
        else
        {
            pcud->configUpdate.insertPlanIntoRoot(ns);
        }

        ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage(*pcud._retn());

        std::string strPlanName = TA_Base_Bus::PlanDataReader::instance()->getNodePath(planId);
        AAMessageSender::instance()->submitAuditPlanInsert(session, strPlanName);

        std::ostringstream auditMessage;
        auditMessage << strPlanName << "[Id:" << planId << "] has been inserted.";
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", auditMessage.str().c_str());
    }

    void PlanDataWritingClass::movePlan(const std::string& session, TA_Base_Core::NodeId plan,
                                        TA_Base_Core::NodeId parent, bool useForce)
    {
        TA_Base_Core::NodeSummary_var oldSummary = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(plan);
        std::ostringstream message;

        if (( oldSummary->type != TA_Base_Core::PLAN_NODE ) && ( oldSummary->type != TA_Base_Core::WORKFLOW_NODE ))
        {
            message << "Error: The node [" << plan << "] is not of type PLAN.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
        }

        TA_Base_Core::ENode nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(parent);

        if(nodeType != TA_Base_Core::CATEGORY_NODE)
        {
            message << "Error: The node [" << parent << "] is not of type CATEGORY.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_CATEGORY));
        }

        if(isPlanBeingEdited(plan))
        {
            std::string editLockSession = getEditLockOwner(plan);
            message << "Error: The plan with name [" << oldSummary->name.in()
                    << "] is being edited by "
                    <<  ActivePlanBulletinBoard::instance()->getOwner(editLockSession);
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_EDIT_LOCKED_BY_OPERATOR, ActivePlanBulletinBoard::instance()->getOwnerId(editLockSession)));

        }

        std::string sourcePlanPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(plan);
        bool overwriting = false;
        TA_Base_Core::NodeId existingPlan = 0;

        // if we have a duplicate name
        if(!TA_Base_Bus::PlanDataReader::instance()->canUseName(plan, parent, parseForApostrophe(oldSummary->name.in())))
        {
            // get the ID of the existing plan
            existingPlan = getPlanToReplace(oldSummary->name.in(), parent, useForce);
            overwriting = true;
        }

        if(overwriting)
        {
            TA_Base_Bus::PlanDataWriter::instance()->replacePlan(session, existingPlan, plan);
        }
        else
        {
            TA_Base_Bus::PlanDataWriter::instance()->updateNodeParent(session, parent, plan);
        }

        TA_Base_Core::PlanConfigUpdateData pcud;
        pcud.session = CORBA::string_dup(session.c_str());
        std::string targetPlanPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(plan); // same plan ID different parent now

        // When replacing an existing plan, send a delete update before the move update.
        if(overwriting)
        {
            TA_Base_Core::DeleteNodeUpdate dnu;
            dnu.parent = parent;
            dnu.nodes.length(1);
            dnu.nodes[0] = existingPlan;

            if(parent != TA_Base_Core::ROOT_NODE_ID)
            {
                pcud.configUpdate.deletePlans(dnu);
            }
            else
            {
                pcud.configUpdate.deletePlansFromRoot(dnu.nodes);
            }

            ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage(pcud);

            AAMessageSender::instance()->submitAuditPlanDelete(session, targetPlanPath);

            std::ostringstream auditMessage;
            auditMessage << targetPlanPath << " [Id:" << existingPlan << "] has been deleted by overwriting " << sourcePlanPath << " [Id:"  << plan << "].";
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", auditMessage.str().c_str());
        }

        TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(plan);
        TA_Base_Core::TransferNodeUpdate tnu;

        tnu.oldParent = oldSummary->parent;
        tnu.summary.node = ns->node;
        tnu.summary.parent = parent;
        tnu.summary.name = CORBA::string_dup(ns->name);
        tnu.summary.type = ns->type;
        tnu.summary.approvalState = ns->approvalState;
        tnu.summary.hasChildren = ns->hasChildren;
        tnu.summary.hasApprovedDescendant = ns->hasApprovedDescendant;
        tnu.summary.hasUnapprovedDescendant = ns->hasUnapprovedDescendant;

        if(parent == TA_Base_Core::ROOT_NODE_ID)
        {
            pcud.configUpdate.movePlanToRoot(tnu);
        }
        else if(oldSummary->parent == TA_Base_Core::ROOT_NODE_ID)
        {
            pcud.configUpdate.movePlanFromRoot(tnu);
        }
        else
        {
            pcud.configUpdate.movePlan(tnu);
        }

        ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage(pcud);

        AAMessageSender::instance()->submitAuditPlanMove(session, sourcePlanPath, targetPlanPath);

        std::ostringstream auditMessage;
        auditMessage << sourcePlanPath << " [Id:" << plan << "] has been moved to " << targetPlanPath << ".";
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", auditMessage.str().c_str());
    }

    void PlanDataWritingClass::insertPlan(const std::string& session, const TA_Base_Core::NodeId parent, TA_Base_Core::ENode nodeType)
    {
        TA_Base_Core::NodeId plan = TA_Base_Bus::PlanDataWriter::instance()->insertPlan(session, parent, nodeType);

        //Send Update for insertPlan
        TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
        pcud->session = CORBA::string_dup(session.c_str());

        TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(plan);

        if(parent != TA_Base_Core::ROOT_NODE_ID)
        {
            pcud->configUpdate.insertPlan(ns);
        }
        else
        {
            pcud->configUpdate.insertPlanIntoRoot(ns);
        }

        ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage(*pcud._retn());

        std::string strPlanName = TA_Base_Bus::PlanDataReader::instance()->getNodePath(plan);
        AAMessageSender::instance()->submitAuditPlanInsert(session, strPlanName);

        std::ostringstream auditMessage;
        auditMessage << strPlanName << "[Id:" << plan << "] has is moved.";
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", auditMessage.str().c_str());
    }

    //This method used to delete plans which could belong to different parents. But now it is being
    //changed so that all plans are assumed to belong to the same parent.

    void PlanDataWritingClass::deletePlans(const std::string& session, const TA_Base_Core::NodeIds& plans)
    {
        int numberOfPlans = plans.length();
        TA_Base_Core::NodeId parent = TA_Base_Bus::PlanDataReader::instance()->getParent(plans[0]);

        TA_Base_Core::DeleteNodeUpdate_var dnu = new TA_Base_Core::DeleteNodeUpdate;
        dnu->parent = parent;
        dnu->nodes.length(numberOfPlans);

        TA_Base_Core::NodeId plan = 0;

        std::ostringstream message;

        for(int i = 0; i < numberOfPlans; i++)
        {
            plan = plans[i];
            TA_Base_Core::ENode nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(plan);
            message << "Error: The plan ";

            if (( nodeType != TA_Base_Core::PLAN_NODE ) && ( nodeType != TA_Base_Core::WORKFLOW_NODE ))
            {
                message << "is not of type PLAN.";
                TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
            }

            if(TA_Base_Core::APPROVED_APPROVAL_STATE == TA_Base_Bus::PlanDataReader::instance()->getPlanApprovalState(plan))
            {
                message << "is approved and cannot be deleted.";
                TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_CANNOT_DELETE_APPROVED_PLAN));
            }


            if(isPlanBeingEdited(plan))
            {
                std::string editLockSession = getEditLockOwner(plan);
                message << "is being edited by " << ActivePlanBulletinBoard::instance()->getOwner(editLockSession) << ".";
                TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_EDIT_LOCKED_BY_OPERATOR, ActivePlanBulletinBoard::instance()->getOwnerId(editLockSession)));
            }

            //dnu.operator [](i).parent = TA_Base_Bus::PlanDataReader::instance()->getParent(plan);
            dnu->nodes[i] = plan;
        }

        //We are here means we can delete all the given plans
        int undeletedPlans = 0;
        std::vector <int> VectorUndeletedPlans;

        for(int j = 0; j < numberOfPlans; j++)
        {
            std::string planPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(plans[j]);
            plan = plans[j];
            // TD11505: Stored procedure DELETE_PLAN() will raise an application error if the plan
            // cannot be deleted for whatever reason. This will be thrown from executeProcedure()
            // as a DatabaseException.
            try
            {
                TA_Base_Bus::PlanDataWriter::instance()->deletePlan(session, plan);

                AAMessageSender::instance()->submitAuditPlanDelete(session, planPath);
            }
            catch(const TA_Base_Core::DatabaseException& ex)
            {
                // Check for error -20022.
                const std::string DELETE_PLAN_ERROR("-20022");
                std::string exceptionError(ex.what());

                if(exceptionError.find(DELETE_PLAN_ERROR) == std::string::npos)
                {
                    TA_THROW(ex);
                }

                undeletedPlans++;
                VectorUndeletedPlans.push_back(j);

                AAMessageSender::instance()->submitAuditPlanDeleteFailure(session, planPath);

                //Log alarm
                std::ostringstream alarmMessage;
                alarmMessage << "PlanDeleteFailure:" << planPath << "[Id:" << plans[j] << "] could not be deleted.";
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", alarmMessage.str().c_str());
            }

            try
            {
                TA_Base_Bus::PlanDataWriter::instance()->deletePlanType(plan);
            }
            catch(...)
            {
            }
        }

        TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
        pcud->session = CORBA::string_dup(session.c_str());

        if(undeletedPlans)
        {
            TA_Base_Core::DeleteNodeUpdate_var newDnu = new TA_Base_Core::DeleteNodeUpdate;
            dnu->parent = parent;
            int actualDeletedPlans = numberOfPlans - undeletedPlans;
            newDnu->nodes.length(actualDeletedPlans);
            int oldIndex = 0;
            int newIndex = 0;
            bool foundUndeletedPlan = false;

            std::vector<int>::iterator it;

            for(it = VectorUndeletedPlans.begin(); it != VectorUndeletedPlans.end() ; it++)
            {
                int nUndeleted = *it;
                while(newIndex != nUndeleted)
                {
                    newDnu->nodes[newIndex++] = dnu->nodes[oldIndex++];
                }

                oldIndex++;
            }

            if(parent != TA_Base_Core::ROOT_NODE_ID)
            {
                pcud->configUpdate.deletePlans(newDnu);
            }
            else
            {
                pcud->configUpdate.deletePlansFromRoot(newDnu->nodes);
            }
        }
        else
        {
            if(parent != TA_Base_Core::ROOT_NODE_ID)
            {
                pcud->configUpdate.deletePlans(dnu);
            }
            else
            {
                pcud->configUpdate.deletePlansFromRoot(dnu->nodes);
            }
        }

        if(undeletedPlans != numberOfPlans)
        {
            //Finally send the update
            ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage(*pcud._retn());
        }

        if(undeletedPlans)
        {
            std::string message = "Error: The Plans nodes could not be deleted: ";
            TA_THROW(TA_Base_Core::PlanAgentException(message, TA_Base_Core::PAE_DATA_ERROR));
        }
    }

    void PlanDataWritingClass::updateCategory(const std::string& session, const TA_Base_Core::CategoryDetail& detail)
    {
        std::ostringstream message;

        TA_Base_Core::NodeId parent = detail.parent;

        TA_Base_Core::ENode nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(parent);

        if(nodeType != TA_Base_Core::CATEGORY_NODE)
        {
            message << "Error: The parent[" << detail.parent << "] is not of type CATEGORY.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_PARENT_NOT_CATEGORY));
        }

        TA_Base_Core::NodeId category = detail.category;

        nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(category);

        if(nodeType != TA_Base_Core::CATEGORY_NODE)
        {
            message << "Error: The category[" << category << "] is not of type CATEGORY.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_CATEGORY));
        }

        std::string name = parseForApostrophe(detail.name.in());
        std::string description = parseForApostrophe(detail.description.in());

        if(!TA_Base_Bus::PlanDataReader::instance()->canUseName(category, parent, name))
        {
            message << "Error: Category with name [" << detail.name.in() << "] already exists in the parent category.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NAME_NOT_UNIQUE));
        }

        std::string oldPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(category);

        TA_Base_Bus::PlanDataWriter::instance()->updateCategory(session, name, description, category);
        //We have successfully updated the category.
        //Send Update for insertCategory
        TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
        pcud->session = CORBA::string_dup(session.c_str());

        TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(category);
        pcud->configUpdate.updateCategory(ns);
        ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage(*pcud._retn());

        std::string strNewName = TA_Base_Bus::PlanDataReader::instance()->getNodePath(category);
        AAMessageSender::instance()->submitAuditCategoryUpdate(session, oldPath, strNewName);

        std::ostringstream auditMessage;
        auditMessage << "Category details of " << strNewName << "[Id:" << category << "] have been updated.";
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", auditMessage.str().c_str());
    }


    void PlanDataWritingClass::insertCategory(const std::string& session, TA_Base_Core::NodeId parent)
    {
        std::string name = getNewNodeName(parent, TA_Base_Core::CATEGORY_NODE);
        TA_Base_Core::NodeId category = TA_Base_Bus::PlanDataWriter::instance()->insertCategory(session, parent);

        //Send Update for insertCategory
        TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
        pcud->session = CORBA::string_dup(session.c_str());

        TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(category);

        if(parent != TA_Base_Core::ROOT_NODE_ID)
        {
            pcud->configUpdate.insertCategory(ns);
        }
        else
        {
            pcud->configUpdate.insertCategoryIntoRoot(ns);
        }

        ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage(*pcud._retn());

        std::string strName = TA_Base_Bus::PlanDataReader::instance()->getNodePath(category);
        std::string strParent = TA_Base_Bus::PlanDataReader::instance()->getNodeName(parent);
        AAMessageSender::instance()->submitAuditCategoryInsert(session, name, strParent);

        std::ostringstream auditMessage;
        auditMessage << "Category " << strName << "[Id:" << category << "] has been inserted into Category["  << strParent << "].";
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", auditMessage.str().c_str());
    }

    void PlanDataWritingClass::deleteCategory(const std::string& session, TA_Base_Core::NodeId category)
    {
        std::string categoryPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(category);
        std::ostringstream message;

        TA_Base_Core::ENode nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(category);

        if(nodeType != TA_Base_Core::CATEGORY_NODE)
        {
            message << "Error: The node[" << category << "] is not of type CATEGORY.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_CATEGORY));
        }

        TA_Base_Core::NodeSummary* categorySummary = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(category);

        if(categorySummary->hasChildren)
        {
            TA_THROW(TA_Base_Core::PlanAgentException("Error: Non empty category cannot be deleted.", TA_Base_Core::PAE_CANNOT_DELETE_NON_EMPTY_CATAGORY));
        }

        TA_Base_Core::NodeId parent = categorySummary->parent;

        std::string parentName = "ROOT";

        if(parent != TA_Base_Core::ROOT_NODE_ID)
        {
            TA_Base_Core::NodeSummary* parentSummary = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(parent);
            parentName = parentSummary->name.in();
        }

        TA_Base_Bus::PlanDataWriter::instance()->deleteCategory(category);
        //Send Update
        TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
        pcud->session = CORBA::string_dup(session.c_str());

        if(parent != TA_Base_Core::ROOT_NODE_ID)
        {
            TA_Base_Core::DeleteNodeUpdate_var dnu = new TA_Base_Core::DeleteNodeUpdate;
            dnu->parent = parent;
            dnu->nodes.length(1);
            dnu->nodes[0] = category;
            pcud->configUpdate.deleteCategory(dnu);
        }
        else
        {
            pcud->configUpdate.deleteCategoryFromRoot(category);
        }

        ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage(*pcud._retn());

        AAMessageSender::instance()->submitAuditCategoryDelete(session, categoryPath);

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s[Id:%lu] in Category %s[%lu] has been deleted.", 
            categorySummary->name.in(), category, parentName.c_str(), parent );
    }


    void PlanDataWritingClass::setPlanApprovalState(const std::string& session, TA_Base_Core::NodeId nodeId, TA_Base_Core::EApprovalState approvalState)
    {
        //Check that the Plan is not active now
        std::list<TA_Base_Core::ActivePlanId> planList;
        ActivePlanBulletinBoard::instance()->getActiveInstances(nodeId, planList);

        std::ostringstream what;

        if(planList.size() != 0)
        {
            what << "Plan: " << TA_Base_Bus::PlanDataReader::instance()->getNodeName(nodeId).c_str() << " has at least one instance active";

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", what.str().c_str());
            TA_THROW(TA_Base_Core::PlanAgentException(what.str(), TA_Base_Core::PAE_PLAN_HAS_ACTIVE_INSTANCE));
        }

        if(isPlanBeingEdited(nodeId))
        {
            std::string editLockSession = getEditLockOwner(nodeId);
            what << "The Plan cannot be approved while being edited by " << ActivePlanBulletinBoard::instance()->getOwner(editLockSession) << ".";
            TA_THROW(TA_Base_Core::PlanAgentException(what.str(), TA_Base_Core::PAE_EDIT_LOCKED_BY_OPERATOR, ActivePlanBulletinBoard::instance()->getOwnerId(editLockSession)));
        }

        unsigned long planPKey = nodeId;

        try
        {
            TA_Base_Bus::PlanDataWriter::instance()->setPlanApprovalState(session, nodeId, approvalState);
        }
        catch(...)
        {
            std::ostringstream auditMessage;
            std::string strPlanName = TA_Base_Bus::PlanDataReader::instance()->getNodePath(nodeId);

            if(approvalState == TA_Base_Core::APPROVED_APPROVAL_STATE)
            {
                AAMessageSender::instance()->submitAuditPlanApprovalFailure(session, strPlanName);

                auditMessage << "PlanApprovalFailure:" << strPlanName << "[Id:" << nodeId << "] could not be approved.";
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", auditMessage.str().c_str());
            }
            else if(approvalState == TA_Base_Core::UNAPPROVED_APPROVAL_STATE)
            {
                auditMessage << "PlanUnApprovalFailure:" << strPlanName << "[Id:" << nodeId << "] could not be unapproved.";
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", auditMessage.str().c_str());
                AAMessageSender::instance()->submitAuditPlanUnapprovalFailure(session, strPlanName);
                std::string message = "Failed to unapprove a plan.";
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", message.c_str());
                TA_THROW(TA_Base_Core::PlanAgentException(message.c_str() , TA_Base_Core::PAE_PLAN_UNAPPROVE_ERROR));
            }
            return;
        }

        TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
        pcud->session = CORBA::string_dup(session.c_str());

        TA_Base_Core::ApprovalStateUpdate_var asu = new TA_Base_Core::ApprovalStateUpdate;
        asu->plan = nodeId;
        asu->approvalState = approvalState;

        pcud->configUpdate.approvalState(asu);

        ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage(*pcud._retn());
        //Generate auditMessage for Approval/Unapproval

        std::string strName = TA_Base_Bus::PlanDataReader::instance()->getNodePath(nodeId);

        std::ostringstream auditMessage;
        auditMessage << "has ";

        if(approvalState == TA_Base_Core::APPROVED_APPROVAL_STATE)
        {
            AAMessageSender::instance()->submitAuditPlanApproval(session, strName);
            auditMessage << "approved ";
        }
        else if(approvalState == TA_Base_Core::UNAPPROVED_APPROVAL_STATE)
        {
            AAMessageSender::instance()->submitAuditPlanUnapproval(session, strName);
            auditMessage << "unapproved ";
        }

        auditMessage << "the plan: " << strName << "[Id:" << nodeId << "]";
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", auditMessage.str().c_str());
    }


    std::string PlanDataWritingClass::getNodeNameToCopy(TA_Base_Core::NodeId node, TA_Base_Core::NodeId parent)
    {
        return TA_Base_Bus::PlanDataWriter::instance()->getNodeNameToCopy(node, parent);
    }

    std::string PlanDataWritingClass::getNewNodeName(TA_Base_Core::NodeId parent, TA_Base_Core::ENode nodeType)
    {
        return TA_Base_Bus::PlanDataWriter::instance()->getNewNodeName(parent, nodeType);
    }


    std::string PlanDataWritingClass::parseForApostrophe(const std::string& inString)
    {
        std::string retValue = "";
        int iPos = 0;
        int nextPos = 0;
        std::string temp = "";

        while((nextPos = inString.find_first_of("'", iPos)) != std::string::npos)
        {
            temp = inString.substr(iPos, nextPos - iPos);
            retValue += temp;
            retValue += "''";
            iPos = nextPos + 1;
        }

        if(retValue.empty())
            retValue = inString;
        else
            retValue += inString.substr(iPos, inString.length());

        return retValue;
    }

    TA_Base_Core::NodeId PlanDataWritingClass::getPlanToReplace(const std::string& planName, TA_Base_Core::NodeId parent, bool useForce)
    {
        // get the details for the duplicate plan
        TA_Base_Core::NodeId duplicatePlan = TA_Base_Bus::PlanDataReader::instance()->getNodeId(parent, parseForApostrophe(planName));

        TA_Base_Core::NodeSummary_var dupSummary =
            TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(duplicatePlan);

        // check that the existing plan is in fact a plan
        if(TA_Base_Core::CATEGORY_NODE == dupSummary->type)
        {
            std::ostringstream message;
            message << "Error: A subcategory with name ["
                    << planName
                    << "] already exists in the destination category.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_CANNOT_OVERWRITE_CATEGORY));
        }

        // check that the existing plan is not approved
        if(TA_Base_Core::APPROVED_APPROVAL_STATE == dupSummary->approvalState)
        {
            std::ostringstream message;
            message << "Error: An approved plan with name ["
                    << planName
                    << "] already exists in the destination category.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_CANNOT_OVERWRITE_APPROVED_PLAN));
        }

        // check that the existing plan is not being edited
        if(isPlanBeingEdited(duplicatePlan))
        {
            std::ostringstream message;
            std::string editLockSession = getEditLockOwner(duplicatePlan);
            message << "Error: The plan with name ["
                    << planName
                    << "already exists in the destination category and it is being edited by "
                    <<  ActivePlanBulletinBoard::instance()->getOwner(editLockSession);
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_CANNOT_OVERWRITE_EDITED_PLAN, ActivePlanBulletinBoard::instance()->getOwnerId(editLockSession)));
        }

        if(!useForce)
        {
            std::ostringstream message;
            message << "Error: Plan with name ["
                    << planName
                    << "] already exists in the destination category.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NAME_NOT_UNIQUE));
        }

        return duplicatePlan;
    }

    void PlanDataWritingClass::setPlanType(const std::string& session, TA_Base_Core::NodeId nodeId, TA_Base_Core::EPlanType visibleState)
    {
        try
        {
            TA_Base_Bus::PlanDataWriter::instance()->deletePlanType(nodeId);
        }
        catch(...)
        {
        }

        bool bSuccess = false;

        try
        {
            bSuccess = TA_Base_Bus::PlanDataWriter::instance()->insertPlanType(session, nodeId, visibleState);
        }
        catch(...)
        {
        }

        if(!bSuccess)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Set plan type fail for node %d and type %d", nodeId, visibleState);
        }
    }
}
