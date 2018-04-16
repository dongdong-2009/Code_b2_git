/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanDataWriter.cpp $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #6 $
  *
  *  Last modification: $DateTime: 2011/03/30 10:46:09 $
  *  Last modified by:  $Author: builder $
  *
  *  A thread-safe Singleton factory for updating database and adding into database.
  *  
  */

#include "PlanAgent.h"
#include "DatabaseConstants.h"
#include "core/exceptions/src/DbConnectionFailed.h"
#include "bus/mathematical_evaluation/src/ExpressionEvaluator.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"
#include "BooleanContextImpl.h"

namespace TA_Base_App
{
    PlanDataWritingClass::PlanDataWritingClass()
    {
        /*
        1Day = 1.0, 1Hour = 1/24, 1Minute = 1/1440, 1Second=1/86400
        */
        UpdatePlansDBSequence();
        m_editPlanLockTimeout = PLANEDITLOCK_TIMEOUT_SECONDS/86400.0;
    }


    void PlanDataWritingClass::UpdatePlansDBSequence()
    {
		std::string sql = "call update_plans_seq('plans_seq')";
		TA_Base_Core::IDatabase* dataBase = getDatabase();
		dataBase->executeProcedure(sql);
        LOG(SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "The Database sequence[plans_seq] has been updated to the max of p_key[node_tree, step_details, step_parameters].");
    }


	void PlanDataWritingClass::deleteSteps(const std::string& session, TA_Base_Core::NodeId plan,
		const TA_Base_Core::StepIds& steps)
	{
		int nStepIdSeqSize = steps.length();
		
		for (int i=0; i < nStepIdSeqSize; i++)
		{

			
		}
	}


	void PlanDataWritingClass::getEditPlanLock( const std::string& session, const std::string& planManagerId, TA_Base_Core::NodeId plan, bool noDBAccess /*=false*/ )
    {
		if ( !TA_Base_Bus::PlanDataReader::instance()->existsNode(plan) )
		{
            TA_THROW(TA_Base_App::PlanNotFoundException("The plan does not exist.", TA_Base_Core::PNFE_NODE_NOT_EXIST));
		}

        std::string requestingLockOwner = session;
        requestingLockOwner += PLAN_EDIT_LOCK_SEPARATOR;
        requestingLockOwner += planManagerId;

        // TD8505: Merged fix from 4619.
        if (!noDBAccess)
        {
            std::string exisingLockOwner = setEditLockInDataBase(requestingLockOwner, plan);

            if ( exisingLockOwner.compare(requestingLockOwner) != 0 )
            {
                std::string owner = ActivePlanBulletinBoard::instance()->getOwner(exisingLockOwner);
            
                std::ostringstream what;
                what << "The plan is already being edited by the operator: " << owner;

                TA_THROW(TA_Base_Core::PlanAgentException(what.str(), TA_Base_Core::PAE_EDIT_LOCKED_BY_OPERATOR, ActivePlanBulletinBoard::instance()->getOwnerId(exisingLockOwner)));
            }
        }

        //Got the lock so generate various updates
#ifndef PROJECT_4669 // TDFAT 1068: Don't want lock audit events under 4669
		TA_Base_Core::DescriptionParameters descriptionParameters;
		//xufeng++ TD14927
		TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
		descriptionParameters.push_back ( &pair0 );
		//++xufeng TD14927
        TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(plan));
		descriptionParameters.push_back ( &pair1 );
		AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanEditLockAcquire, descriptionParameters);
#endif

        //Send StateUpdate for Plan being edited
		TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData;

        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
        TA_Base_Core::EditLockUpdate_var elu = new TA_Base_Core::EditLockUpdate;

        elu->session = CORBA::string_dup(session.c_str());
		elu->pmId = CORBA::string_dup(planManagerId.c_str());//CL-21338 PlanAgent wrong session and high CPU usage in monitor mode 
        elu->plan = plan;

        pasUpdateData->update.getLockUpdate(elu);

        ActivePlanBulletinBoard::instance()->sendStateUpdateMessage(pasUpdateData);
    }


	void PlanDataWritingClass::releaseEditPlanLock(const std::string& session,TA_Base_Core::NodeId plan, bool noDBAccess /*=false*/ )
    {
		if ( !TA_Base_Bus::PlanDataReader::instance()->existsNode(plan) )
		{
            TA_THROW(TA_Base_App::PlanNotFoundException("The plan does not exist.", TA_Base_Core::PNFE_NODE_NOT_EXIST));
		}

        // TD8505: Merged fix from 4619.
        if (!noDBAccess)
        {
            releaseEditLockInDatabase(session, plan );
        }

#ifndef PROJECT_4669 // TDFAT 1068: Don't want unlock audit events under 4669
		TA_Base_Core::DescriptionParameters descriptionParameters;
		//xufeng++ TD14927
		TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
		descriptionParameters.push_back ( &pair0 );
		//++xufeng TD14927
        TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(plan));
		descriptionParameters.push_back ( &pair1 );
		AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanEditLockRelease, descriptionParameters);
#endif

        //Send StateUpdate for release Plan being edited
		TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData;

        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
        TA_Base_Core::EditLockUpdate_var elu = new TA_Base_Core::EditLockUpdate;

        elu->session = CORBA::string_dup(session.c_str());
		elu->pmId = CORBA::string_dup("no need pmid.");//CL-21338 PlanAgent wrong session and high CPU usage in monitor mode 
        elu->plan = plan;

        pasUpdateData->update.releaseLockUpdate(elu);

        ActivePlanBulletinBoard::instance()->sendStateUpdateMessage(pasUpdateData);
    }

    std::string PlanDataWritingClass::getEditLockOwner(TA_Base_Core::NodeId plan)
    {
        std::string lockOwner = "";

		TA_Base_Core::IDatabase* database = getDatabase();

        std::ostringstream sql;

        sql << "select edit_lock_owner from node_tree where p_key = "
            << plan; 

		std::vector<std::string> columns;

		columns.push_back("edit_lock_owner");

		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( sql.str(), columns ) );
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(database); //  CL-21716
		if( (NULL == data.get() ) || (1 != data->getNumRows()) ) 
        {
            return "";
        }

        lockOwner = data->getStringData(0, columns[0] );
        return lockOwner;
    }

	bool PlanDataWritingClass::isPlanBeingEdited(TA_Base_Core::NodeId plan)
	{
		TA_Base_Core::IDatabase* database = getDatabase();

        std::ostringstream sql;

		sql << "select is_plan_edit_locked("
            << plan << ","
            << m_editPlanLockTimeout << ") isPlanEditLocked from dual";

		std::vector<std::string> columns;
		columns.push_back("isPlanEditLocked");

		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( sql.str(), columns ) );
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(database); //  CL-21716

		if( (NULL == data.get() ) || (1 != data->getNumRows()) ) 
        {
            TA_THROW(TA_Base_App::PlanNotFoundException("The plan does not exist.", TA_Base_Core::PNFE_NODE_NOT_EXIST));
        }

        int isPlanBeingEdited = data->getUnsignedLongData(0, columns[0] );

        if (isPlanBeingEdited) return true;

        return false;
	}


    std::string PlanDataWritingClass::setEditLockInDataBase (const std::string& session, TA_Base_Core::NodeId plan )
    {
		TA_Base_Core::IDatabase* database = getDatabase();


        std::ostringstream sql;
		sql << "call Set_Plan_Edit_Lock("
            << plan << ",'"
            << session.c_str() << "',"
            << m_editPlanLockTimeout << ")";

		std::string strSQL = sql.str();
		database->executeProcedure(sql.str());

        sql.str("");

        sql << "select edit_lock_owner from node_tree where p_key = "
            << plan; 

		std::vector<std::string> columns;

		columns.push_back("edit_lock_owner");

		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( sql.str(), columns ) );
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(database); //  CL-21716

		if( (NULL == data.get() ) || (1 != data->getNumRows()) ) 
        {
            TA_THROW(TA_Base_App::PlanNotFoundException("The plan does not exist.", TA_Base_Core::PNFE_NODE_NOT_EXIST));
        }

        std::string lockOwner = data->getStringData(0, columns[0] );
        return lockOwner;
    }


    void PlanDataWritingClass::releaseEditLockInDatabase(const std::string& session, TA_Base_Core::NodeId plan )
    {
		TA_Base_Core::IDatabase* database = getDatabase();

        std::ostringstream sql1;
        sql1 << "select nvl(edit_lock_owner, ' ') from node_tree where p_key = "
            << plan; 
          
		std::vector<std::string> columns;
		columns.push_back("edit_lock_owner");

		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( sql1.str(), columns ) );
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(database); //  CL-21716
		if( (NULL == data.get() ) || (1 != data->getNumRows()) ) 
        {
			std::ostringstream message;
			message << "The plan " << plan << "does not exist.";
            TA_THROW(TA_Base_App::PlanNotFoundException(message.str(), TA_Base_Core::PNFE_NODE_NOT_EXIST));
        }

        std::string lockOwner = data->getStringData(0, columns[0] );

        if (lockOwner.find(session) != 0)
        {
			std::ostringstream message;
			message << "Error: Cannot release the edit lock owned by " << lockOwner.c_str() << ".";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_EDIT_LOCKED_BY_OPERATOR, ActivePlanBulletinBoard::instance()->getOwnerId(lockOwner)));
        }

        std::ostringstream sql2;
        sql2 << "update node_tree set edit_lock_owner = NULL, edit_time_stamp = NULL where p_key = "
            << plan;

		database->executeProcedure(sql2.str());

        //Check whether the lock has really been released
		std::auto_ptr<TA_Base_Core::IData> data1( database->executeQuery( sql1.str(), columns ) );

		if( (NULL == data1.get() ) || (1 != data1->getNumRows()) ) 
        {
            TA_THROW(TA_Base_App::PlanNotFoundException("The plan does not exist.", TA_Base_Core::PNFE_NODE_NOT_EXIST));
        }

        lockOwner = data1->getStringData(0, columns[0] );

        if (lockOwner.compare(session) == 0)
        {
            TA_THROW(TA_Base_Core::PlanAgentException("Error: Could not release the edit lock. Check database connection", 
                TA_Base_Core::PAE_CANNOT_RELEASE_EDIT_LOCK));

        }
    }


    bool PlanDataWritingClass::refreshEditLockInDatabase(const std::string& session, TA_Base_Core::NodeId plan)
    {
        return true;
    }


	void PlanDataWritingClass::CheckPlanEditSession(TA_Base_Core::NodeId plan, const std::string& session)
	{
		TA_Base_Core::IDatabase* database = getDatabase();

        std::ostringstream sql;
        sql << "select nvl(edit_lock_owner, ' ') from node_tree where p_key = "
            << plan; 
         
		std::vector<std::string> columns;
		columns.push_back("edit_lock_owner");
		std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( sql.str(), columns ) );
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(database); //  CL-21716
		if( (NULL == data.get() ) || (1 != data->getNumRows()) ) 
        {
            TA_THROW(TA_Base_App::PlanNotFoundException("The plan does not exist.", TA_Base_Core::PNFE_NODE_NOT_EXIST));
        }

        std::string lockOwner = data->getStringData(0, columns[0] );

        if (lockOwner.find(session) == 0 ) return;

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

		if ( parentNodeType != TA_Base_Core::CATEGORY_NODE )
		{			
			message << "Error: The parent node is not of type CATEGORY.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_PARENT_NOT_CATEGORY));
		}

		std::string name = planDetail->name.in();
		std::string description = planDetail->description.in();
		TA_Base_Core::EApprovalState approvalState = planDetail->approvalState;

		if ( name.empty() )
		{
            TA_THROW(TA_Base_Core::PlanAgentException("The name of Plan cannot be blank.", TA_Base_Core::PAE_NO_PLAN_NAME));
		}

		//see if file has any file-path disallowed characters ( \ / : * ? " < > | )
		if (name.find ("\\") != std::string::npos ||
			 name.find ("/")  != std::string::npos ||
			 name.find (":")  != std::string::npos ||
			 name.find ("*")  != std::string::npos ||
		    name.find ("?")  != std::string::npos ||
			 name.find ("\"") != std::string::npos ||
			 name.find ("<")  != std::string::npos ||
			 name.find (">")  != std::string::npos ||
			 name.find ("|")  != std::string::npos )
		{
			TA_THROW(TA_Base_Core::PlanAgentException("The name of Plan cannot contain any of the reserved characters - \\ / : * ? \" < > |", TA_Base_Core::PAE_INVALID_PLAN_NAME));
		}
				
		
		if ( !TA_Base_Bus::PlanDataReader::instance()->canUseName(nodeId, parent, parseForApostrophe(name) ) )
		{
			message.str("");
			message << "Error: The name " << name.c_str() << " is already used by another Plan/Category.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NAME_NOT_UNIQUE));
		}

		if ( TA_Base_Core::UNAPPROVED_APPROVAL_STATE != approvalState )
		{
            TA_THROW(TA_Base_Core::PlanAgentException("Error: Plan can only be saved in an Unapproved state.", TA_Base_Core::PAE_CANNOT_SAVE_APPROVED_PLAN));
		}

		int numberOfSteps = plan.stepseq.length();
		const TA_Base_Core::StepDetail* stepDetail = NULL;
		const TA_Base_Core::StepParameters* stepParameters = NULL;

		for (int i = 0; i < numberOfSteps; i++)
		{
			stepDetail = &(plan.stepseq[i].detail);
			stepParameters = &(plan.stepseq[i].parameters);

			if ( stepDetail->type != stepParameters->_d() )
			{
				message.str("");
				message << "Error: Type mismatch between Step Type and its parameters for step position: "
					    << i+1;

				TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_STEP_TYPE_AND_PARAMETER_MISMATCH, i+1));				
			}

			std::string stepName = stepDetail->name.in();
			std::string stepDescription = stepDetail->description.in();
			unsigned long stepPosition = stepDetail->position;
			unsigned long skip = stepDetail->skip;
			TA_Base_Core::StepId step = stepDetail->step;
			TA_Base_Core::EStep type = stepDetail->type;
			unsigned long skippable = stepDetail->skippable;
			unsigned long delay = stepDetail->delay;

			if ( stepName.empty() )
			{
                TA_THROW(TA_Base_Core::PlanStepDetailException("Step name cannot be blank", TA_Base_Core::PSDT_NAME, TA_Base_Core::PSDE_UNSPECIFIED, i+1 ));
			}

			if ( stepPosition != i+1 )
			{
				message << "Error: The position parameter in step detail of step " << i+1 << " is " 
					    << stepPosition;
                TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_POSITION, TA_Base_Core::PSDE_INVALID, i+1 ));
			}

			if ( skip > 1 )
			{
				message << "Error: Invalid value (" << skip << ") of skip in step detail of step " << i+1;
                TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_SKIP, TA_Base_Core::PSDE_INVALID, i+1 ));
			}

			if ( skippable > 1 )
			{
				message << "Error: Invalid value (" << skippable << ") of skip in step detail of step " << i+1;
                TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_SKIP, TA_Base_Core::PSDE_INVALID, i+1 ));
			}

			if ( type < TA_Base_Core::END_STEP || type >= TA_Base_Core::UNDEFINED_STEP)
			{
				message << "Error: Invalid step type (" << type << ") in step detail of step " << i+1;
                TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_TYPE, TA_Base_Core::PSDE_INVALID, i+1 ));
			}
		}

		const TA_Base_Core::AbortParameters* abortParams = NULL;
		const TA_Base_Core::ActivePlanCheckParameters* planCheckParams = NULL;
		const TA_Base_Core::DataPointCheckParameters* dpCheckParams = NULL;
		const TA_Base_Core::DataPointSetParameters* dpSetParams = NULL;
		const TA_Base_Core::DecisionParameters* decisionParams = NULL;
		const TA_Base_Core::JumpParameters* jumpParams = NULL;
		const TA_Base_Core::LogParameters* logParams = NULL;
		const TA_Base_Core::PromptParameters* promptParams = NULL;
		const TA_Base_Core::RunPlanParameters* runPlanParams = NULL;
		const TA_Base_Core::TerminatePlanParameters* terminatePlanParams = NULL;
		const TA_Base_Core::DataPointValue* dpValue = NULL;
		const TA_Base_Core::NumericDataPoint* ndp = NULL;
        const TA_Base_Core::LaunchGuiAppParameters* lgap = NULL;
		const TA_Base_Core::EvaluateExpressionParameters* eExpressionParams = NULL;
		const TA_Base_Core::StationPublicAddressParameters* stationPAParams = NULL;
		const TA_Base_Core::TrainPublicAddressParameters* trainPAParams = NULL;
		const TA_Base_Core::FailureSummaryParameters* failureSummaryParams = NULL; //lizettejl (DP-Changes)
		
		for (int m = 0; m < numberOfSteps; m++)
		{
			stepParameters = &(plan.stepseq[m].parameters);

			switch(stepParameters->_d())
			{
			case TA_Base_Core::END_STEP:
				break;

			case TA_Base_Core::ABORT_STEP:
				break;

			case TA_Base_Core::ACTIVE_PLAN_CHECK_STEP:
				{
					planCheckParams = &(stepParameters->activePlanCheck());

					if (!(planCheckParams->plan) || !TA_Base_Bus::PlanDataReader::instance()->existsNode(planCheckParams->plan))
					{
						message << "Step " << m+1 << " - Parameter 'Plan Path' : " << " No plan has been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PLAN_PATH, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}

					if (TA_Base_Core::PLAN_NODE != TA_Base_Bus::PlanDataReader::instance()->getNodeType(planCheckParams->plan))
					{
						message << "Step " << m+1 << " - Parameter 'Plan Path' : " << " No plan has been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PLAN_PATH, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}

					if (getStepNumber(plan, planCheckParams->yesStep) == 0)
					{
						message << "Step " << m+1 << " - Parameter 'Yes Step' : " << " The step has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_YES, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}

					if (getStepNumber(plan, planCheckParams->noStep) == 0)
					{
						message << "Step " << m+1 << " - Parameter 'No Step' : " << " The step has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_NO, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}
				}
				break;

			case TA_Base_Core::ASSIGN_CAMERA_TO_MONITOR_STEP:
				{                   
					TA_Base_Core::AssignCameraToMonitorParameters cameraToMonitorParams = stepParameters->cameraMonitor();
                    if ( cameraToMonitorParams.camera == 0 )
                    {
						message << "Step " << m+1 << " - Parameter 'Camera Name' : " << " Camera name has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_CAMERA_NAME, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if ( cameraToMonitorParams.monitor == 0 )
                    {
						message << "Step " << m+1 << " - Parameter 'Monitor Name' : " << " Monitor name has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_MONITOR_NAME, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }
				}
				break;

			case TA_Base_Core::DATA_POINT_CHECK_STEP:
				{
					dpCheckParams = &(stepParameters->dataPointCheck());
					std::string dpName = dpCheckParams->dataPoint.in();

					if (dpName.empty())
					{
						message << "Step " << m+1 << " - Parameter 'Data Point Name' : " << " Data Point name has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DATA_POINT_NAME, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}

					dpValue = &(dpCheckParams->value);

					switch ( dpValue->_d() )
					{
					case TA_Base_Core:: BOOLEAN_DATA_POINT:
						{
							int boolValue = dpValue->booleanValue();
							if (boolValue > 1)
							{
						        message << "Step " << m+1 << " - Parameter 'Data Point Value' : " << " Incorrect boolean value";
                                TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DATA_POINT_VALUE, TA_Base_Core::PSDE_INVALID, m+1 ));
							}
						}
						break;

					case TA_Base_Core::NUMERIC_DATA_POINT:
						//Nothing to validate
						break;

					case TA_Base_Core::TEXT_DATA_POINT:
						{
							std::string text = dpValue->textValue();
							if (text.empty())
							{
						        message << "Step " << m+1 << " - Parameter 'Data Point Value' : " << " Text value of the data point has not been specified";
                                TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DATA_POINT_VALUE, TA_Base_Core::PSDE_INVALID, m+1 ));
							}
						}
						break;

                    case TA_Base_Core::UNDEFINED_DATA_POINT:
                        {
						    message << "Step " << m+1 << " - Parameter 'Data Point Value' : " << " Data Point value has not been specified";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DATA_POINT_VALUE, TA_Base_Core::PSDE_INVALID, m+1 ));
                        }
                        break;

					default:
                        {
						    message << "Step " << m+1 << " - Parameter 'Data Point Value' : " << " Data Point value has not been specified";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DATA_POINT_VALUE, TA_Base_Core::PSDE_INVALID, m+1 ));
                        }
						break;
					}

					switch (dpCheckParams->testType)
                    {
                    case TA_Base_Core::EQUAL_TO_TEST:
                    case TA_Base_Core::NOT_EQUAL_TO_TEST :
                    case TA_Base_Core::LESS_THAN_TEST:
                    case TA_Base_Core::GREATER_THAN_TEST:
                    case TA_Base_Core::LESS_THAN_OR_EQUAL_TO_TEST:
                    case TA_Base_Core::GREATER_THAN_OR_EQUAL_TO_TEST:
                        break;

                    case TA_Base_Core::UNDEFINED_TEST:
						message << "Step " << m+1 << " - Parameter 'Data Point Test Type' : " << " Data Point Test Type has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DATA_POINT_TEST_TYPE, TA_Base_Core::PSDE_INVALID, m+1 ));
                        break;

                    default:
						message << "Step " << m+1 << " - Parameter 'Data Point Test Type' : " << " Data Point Test Type has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DATA_POINT_TEST_TYPE, TA_Base_Core::PSDE_INVALID, m+1 ));
                        break;

                    }

					if (getStepNumber(plan, dpCheckParams->successStep) == 0)
					{
						message << "Step " << m+1 << " - Parameter 'Success Step' : " << " The success step has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_SUCCESS, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}

					if (getStepNumber(plan, dpCheckParams->failureStep) == 0)
					{
						message << "Step " << m+1 << " - Parameter 'Failure Step' : " << " The Failure step has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_FAILURE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}									
				}
				break;

			case TA_Base_Core::DATA_POINT_SET_STEP:
				{
					dpSetParams = &( stepParameters->dataPointSet() );
					std::string dpName = dpSetParams->dataPoint.in();
					if (dpName.empty())
					{
						message << "Step " << m+1 << " - Parameter 'Data Point Name' : " << " Data Point Name has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DATA_POINT_NAME, TA_Base_Core::PSDE_UNSPECIFIED, m+1) );
					}

					dpValue = &(dpSetParams->newValue);

					switch(dpValue->_d())
					{
					case TA_Base_Core::BOOLEAN_DATA_POINT:
						{
							int boolValue = dpValue->booleanValue();

							if (boolValue > 1)
							{
						        message << "Step " << m+1 << " - Parameter 'Data Point Value' : " << " Incorrect boolean value";
                                TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DATA_POINT_VALUE, TA_Base_Core::PSDE_INVALID, m+1 ));
							}
						}
						break;

					case TA_Base_Core::NUMERIC_DATA_POINT:						
						//Nothing to validate
						break;

					case TA_Base_Core::TEXT_DATA_POINT:
						{
							std::string text = dpValue->textValue();

							if (text.empty())
							{
						        message << "Step " << m+1 << " - Parameter 'Data Point Value' : " << " Text value of the data point has not been specified";
                                TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DATA_POINT_VALUE, TA_Base_Core::PSDE_INVALID, m+1 ));
							}
						}
						break;

                    case TA_Base_Core::UNDEFINED_DATA_POINT:
                        {
						    message << "Step " << m+1 << " - Parameter 'Data Point Value' : " << " Data Point value has not been specified";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DATA_POINT_VALUE, TA_Base_Core::PSDE_INVALID, m+1 ));
                        }
                        break;

                    default:
                        {
						    message << "Step " << m+1 << " - Parameter 'Data Point Value' : " << " Data Point value has not been specified";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DATA_POINT_VALUE, TA_Base_Core::PSDE_INVALID, m+1 ));
                        }
                        break;

					}
				}
				break;

			case TA_Base_Core::DECISION_STEP:
				{
					decisionParams = &( stepParameters->decision() );
					std::string decisionMessage = decisionParams->decisionMessage.in();

					if (decisionMessage.empty())
					{
						message << "Step " << m+1 << " - Parameter 'Decision Message' : " << " The message has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_DECISION_MESSAGE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}

					if (getStepNumber(plan, decisionParams->yesStep) == 0)
					{
						message << "Step " << m+1 << " - Parameter 'Yes Step' : " << " The Yes step has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_YES, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}

					if (getStepNumber(plan, decisionParams->noStep) == 0)
					{
						message << "Step " << m+1 << " - Parameter 'No Step' : " << " The No step has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_NO, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}									
					//DP-changes
					std::string yesButtonLabel = decisionParams->yesButtonLabel.in();
					if (yesButtonLabel.empty())
					{
						message << "Step " << m+1 << " - Parameter 'Yes Button Label' : " << " The label has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_YES_LABEL, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}	
					
					std::string noButtonLabel = decisionParams->noButtonLabel.in();
					if (noButtonLabel.empty())
					{
						message << "Step " << m+1 << " - Parameter 'No Button Label' : " << " The label has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_NO_LABEL, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}	
					
					std::string pauseButtonLabel = decisionParams->pauseButtonLabel.in();
					if (pauseButtonLabel.empty())
					{
						message << "Step " << m+1 << " - Parameter 'Pause Button Label' : " << " The label has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PAUSE_LABEL, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}	
					//++lizettejl
				}
				break;

			case TA_Base_Core::JUMP_STEP:
				{
					jumpParams = &( stepParameters->jump() );

					if (getStepNumber(plan, jumpParams->step) == 0)
					{
						message << "Step " << m+1 << " - Parameter 'Jump Step' : " << " The Jump step has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_JUMP, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}
				}
				break;

			case TA_Base_Core::LOG_STEP:
				{
					logParams = &(stepParameters->log());
					std::string logMessage = logParams->message.in();

					if (logMessage.empty())
					{
						message << "Step " << m+1 << " - Parameter 'Prompt Message' : " << " The message has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PROMPT_MESSAGE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}
				}
				break;

            case TA_Base_Core::ACTIVATE_CAMERA_PRESET_STEP:
                {
                    TA_Base_Core::ActivateCameraPresetParameters cameraPresetParams = stepParameters->cameraPreset();

                    if ( cameraPresetParams.camera == 0 )
                    {
						message << "Step " << m+1 << " - Parameter 'Camera Name' : " << " Camera name has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_CAMERA_NAME, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if ( cameraPresetParams.preset == 0 )
                    {
						message << "Step " << m+1 << " - Parameter 'Camera Preset' : " << " Camera preset has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_CAMERA_PRESET, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }
                }
                break;

			case TA_Base_Core::PROMPT_STEP:
				{
					promptParams = &(stepParameters->prompt());
					std::string promptMessage = promptParams->message.in();
					if (promptMessage.empty())
					{
						message << "Step " << m+1 << " - Parameter 'Prompt Message' : " << " The message has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PROMPT_MESSAGE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}
				}
				break;

			case TA_Base_Core::RUN_PLAN_STEP:
				{
					runPlanParams = &(stepParameters->runPlan());

					if (!(runPlanParams->plan) || !TA_Base_Bus::PlanDataReader::instance()->existsNode(runPlanParams->plan))
					{
						message << "Step " << m+1 << " - Parameter 'Plan Path' : " << " No plan has been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PLAN_PATH, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}

					if (TA_Base_Core::PLAN_NODE != TA_Base_Bus::PlanDataReader::instance()->getNodeType(runPlanParams->plan))
					{
						message << "Error: Step position " << m+1 << " Invalid node type for first parameter of Active Plan Check.";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PLAN_PATH, TA_Base_Core::PSDE_INVALID, m+1 ));
					}

				}

				break;

			case TA_Base_Core::TERMINATE_PLAN_STEP:
				{
					terminatePlanParams = &(stepParameters->terminatePlan());

					if (!(terminatePlanParams->plan) || !TA_Base_Bus::PlanDataReader::instance()->existsNode(terminatePlanParams->plan))
					{
						message << "Step " << m+1 << " - Parameter 'Plan Path' : " << " No plan has been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PLAN_PATH, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}

					if (TA_Base_Core::PLAN_NODE != TA_Base_Bus::PlanDataReader::instance()->getNodeType(terminatePlanParams->plan))
					{
						message << "Step " << m+1 << " - Parameter 'Plan Path' : " << " Specified plan is not a valid plan";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PLAN_PATH, TA_Base_Core::PSDE_INVALID, m+1 ));
					}
				}
				break;
			
			case TA_Base_Core::EVALUATE_EXPRESSION_STEP:
			{
					eExpressionParams = &(stepParameters->evaluateExpression());
					std::string eExpression = eExpressionParams->expression.in();

					if (eExpression.empty())
					{
						message << "Step " << m+1 << " - Parameter 'Expression' : " << " Expression to evaluate has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_EXPRESSION_VALUE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}
					else //check validity of expression 
					{
						try
						{
							//use ExpressionEvaluator class to see if the expression can be parsed
							std::auto_ptr <TA_Base_Bus::ExpressionEvaluator> expEval(new TA_Base_Bus::ExpressionEvaluator);
							BooleanContextImpl* context = new BooleanContextImpl ();
							expEval->setNewExpression(eExpression);
							expEval->addBooleanContext (context);
							expEval->evaluateBooleanExpression ();	
						}
						catch (const TA_Base_Core::MathematicalEvaluationException)
						{
							message << "Step " << m+1 << " - Parameter 'Expression Value' : " << " The expression cannot be evaluated !";
                            TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_EXPRESSION_VALUE, TA_Base_Core::PSDE_INVALID, m+1 ));
						}
					}

					if (getStepNumber(plan, eExpressionParams->successStep) == 0)
					{
						message << "Step " << m+1 << " - Parameter 'Success Step' : " << " The success step has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_SUCCESS, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}

					if (getStepNumber(plan, eExpressionParams->failureStep) == 0)
					{
						message << "Step " << m+1 << " - Parameter 'Failure Step' : " << " The Failure step has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_FAILURE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
					}									
				}
				break;

			case TA_Base_Core::VMS_SET_SCENE_STEP:
                {
                    TA_Base_Core::VmsSetSceneParameters vmsSetSceneParams = stepParameters->vmsSetScene();
                    if ( vmsSetSceneParams.vmsSign == 0 )
                    {
						message << "Step " << m+1 << " - Parameter 'VMS Sign' : " << " VMS Sign has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_VMS_SIGN, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if ( vmsSetSceneParams.scene == 0 )
                    {
						message << "Step " << m+1 << " - Parameter 'VMS Scene' : " << " VMS Scene has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_VMS_SCENE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }
                }
				break;

            case TA_Base_Core::LAUNCH_GUI_APP_STEP:
                lgap = &(stepParameters->guiAppParameters());

                if (lgap->app == 0)
                {
				    message << "Step " << m+1 << " - Parameter 'Application' : " << " This has not been specified or is invalid";
                    TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_APPLICATION, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                }

                if ( lgap->pos.width == 0 )
                {
				    message << "Step " << m+1 << " - Parameters 'Width' : " << " Width should be greater than 0";
                    TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_WIDTH, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                }

                if ( lgap->pos.height == 0 )
                {
				    message << "Step " << m+1 << " - Parameters 'Height' : " << " Height should be greater than 0";
                    TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_HEIGHT, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                }
                break;

            case TA_Base_Core::VMS_BLANKOUT_STEP:
                {
                    TA_Base_Core::VmsBlankoutParameters vmsBlankout = stepParameters->vmsBlankout();
                    if ( vmsBlankout.vmsSign == 0 )
                    {
						message << "Step " << m+1 << " - Parameter 'VMS Sign' : " << " VMS Sign has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_VMS_SIGN, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                }
                break;

            case TA_Base_Core::VMS_RESTORE_SCHEDULE_STEP:
                {
                    TA_Base_Core::VmsRestoreScheduleParameters vmsRestoreSchedule = stepParameters->vmsRestoreSchedule();

                    if ( vmsRestoreSchedule.vmsSign == 0 )
                    {
						message << "Step " << m+1 << " - Parameter 'VMS Sign' : " << " VMS Sign has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_VMS_SIGN, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }
                }
                break;

            case TA_Base_Core::VIDEO_WALL_STEP:
                {
                    TA_Base_Core::VideoWallParameters videoWall = stepParameters->videoWallParams();

                    if ( videoWall.wallBoard == 0 )
                    {
						message << "Step " << m+1 << " - Parameter 'Wall Board' : " << " Wall Board has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_WALLBOARD, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if ( videoWall.layout == 0 )
                    {
						message << "Step " << m+1 << " - Parameter 'Video Layout' : " << " Video Layout has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_VIDEO_LAYOUT, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }
                }
                break;

            case TA_Base_Core::SEND_SMS_MESSAGE_STEP:
                {
                    TA_Base_Core::SendSmsMessageParameters sendSmsMessageParams = stepParameters->sendSmsMessage();

                    if ( std::string(sendSmsMessageParams.recipientIdentifier).empty() )
                    {
                        message << "Step " << m+1 << " - Parameter 'SMS Recipient' : SMS Recipient has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_SMS_RECIPIENT, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if ( std::string(sendSmsMessageParams.textMessage).empty() )
                    {
                        message << "Step " << m+1 << " - Parameter 'SMS Message' : SMS Message has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_SMS_MESSAGE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }
                }
                break;

            case TA_Base_Core::LAUNCH_SCHEMATIC_STEP:
                {
                    TA_Base_Core::LaunchSchematicParameters launchSchematicParams = stepParameters->launchSchematic();

                    if ( std::string(launchSchematicParams.schematic.in()).empty() )
                    {
                        message << "Step " << m+1 << " - Parameter 'Schematic name' : Schematic has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_SCHEMATIC_NAME, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if ( launchSchematicParams.screen == 0 )
                    {
                        message << "Step " << m+1 << " - Parameter 'Display' : Specified screen number is not valid";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_SCREEN_NUMBER, TA_Base_Core::PSDE_INVALID, m+1 ));
                    }
                }
                break;

            case TA_Base_Core::STATION_TRAVELLER_INFO_SYSTEM_STEP:
                {
                    TA_Base_Core::StationTravellerInfoSystemParameters stationTisParams = stepParameters->stationTravellerInfoSystem();

                    if (std::string(stationTisParams.destination.station.in()).empty() && 
                        stationTisParams.destination.allStations == false)
                    {
                        message << "Step " << m+1 << " - Parameter 'Station' : Station name has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_STATION_NAME, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if (stationTisParams.message.messageId == TA_Base_Core::TIS_UNDEFINED_MESSAGE_ID)
                    {
                        message << "Step " << m+1 << " - Parameter 'Message' : Message has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_TIS_MESSAGE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if (stationTisParams.priority == 0)
                    {
                        message << "Step " << m+1 << " - Parameter 'Priority' : Message priority has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_TIS_PRIORITY, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if (stationTisParams.duration == 0)
                    {
                        message << "Step " << m+1 << " - Parameter 'Duration' : Message duration has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_TIS_DURATION, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }
                }
                break;

            case TA_Base_Core::TRAIN_TRAVELLER_INFO_SYSTEM_STEP:
                {
                    TA_Base_Core::TrainTravellerInfoSystemParameters trainTisParams = stepParameters->trainTravellerInfoSystem();

                    if (trainTisParams.destination.trainId == TA_Base_Core::TIS_UNDEFINED_TRAIN_ID && 
                        trainTisParams.destination.trainGroup == TA_Base_Core::TG_UNDEFINED)
                    {
                        message << "Step " << m+1 << " - Parameter 'Train ID' : Train ID has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_TRAIN_ID, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if (trainTisParams.message.messageId == TA_Base_Core::TIS_UNDEFINED_MESSAGE_ID)
                    {
                        message << "Step " << m+1 << " - Parameter 'Message' : Message has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_TIS_MESSAGE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if (trainTisParams.priority == 0)
                    {
                        message << "Step " << m+1 << " - Parameter 'Priority' : Message priority has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_TIS_PRIORITY, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if (trainTisParams.duration == 0)
                    {
                        message << "Step " << m+1 << " - Parameter 'Duration' : Message duration has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_TIS_DURATION, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }
                }
                break;
				
				case TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP:
					{
						stationPAParams = &(stepParameters->stationPublicAddress ());
						if (stationPAParams->message.messageId == TA_Base_Core::PA_UNDEFINED_MESSAGE_ID)
                  {
							message << "Step " << m+1 << " - Parameter 'Message' : Message has not been specified";
							TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_STATION_MESSAGE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                  }
						if (stationPAParams->zones.length () == 0)
						{
							message << "Step " << m+1 << " - Parameter 'Zone' : Zone has not been specified";
							TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_STATION_ZONE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
						}
						if (std::string(stationPAParams->station.station.in()).empty() && 
							stationPAParams->station.allStations == false)
						{
							message << "Step " << m+1 << " - Parameter 'Station' : Station name has not been specified";
							TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_STATION_NAME, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
						}
						
					}
					break;

				case TA_Base_Core::TRAIN_PUBLIC_ADDRESS_STEP:
					{
						trainPAParams  = &(stepParameters->trainPublicAddress ());
						if (trainPAParams->message.messageId == TA_Base_Core::PA_UNDEFINED_MESSAGE_ID)
                  {
							message << "Step " << m+1 << " - Parameter 'Message' : Message has not been specified";
							TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_STATION_MESSAGE, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                  }
						if (trainPAParams->destination.trainId == TA_Base_Core::TIS_UNDEFINED_TRAIN_ID && 
							trainPAParams->destination.trainGroup == TA_Base_Core::TG_UNDEFINED)
						{
							message << "Step " << m+1 << " - Parameter 'Train ID' : Train ID has not been specified";
							TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_TRAIN_ID, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
						}
						if (trainPAParams->duration == 0)
						{
							message << "Step " << m+1 << " - Parameter 'Duration' : Message duration has not been specified";
							TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_TIS_DURATION, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
						}
						
					}
					break;
				// lizettejl++ Failure Step Summary (DP-changes)
				case TA_Base_Core::FAILURE_SUMMARY_STEP:
					{
						failureSummaryParams = &(stepParameters->failureSummary());
						if ((failureSummaryParams->failureCategory != TA_Base_Core::SFC_ALL_STEPS) && 
							(failureSummaryParams->failureCategory != TA_Base_Core::SFC_SET_DATA_POINT_STEPS))
						{
							message << "Step " << m+1 << " - Parameter 'Failure Category' : Failure Category  has not been specified";
							TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_FAILURE_SUMMARY, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
						}
						
					}
					break;
				//++lizettejl

			default:
				break;

			}
		}
	}


	void PlanDataWritingClass::updatePlanDetail(const std::string& session, const TA_Base_Core::PlanDetail& detail)
	{
		std::ostringstream message;
		TA_Base_Core::NodeId nodeId = detail.plan;

		//Check whether the Plan is being edited by the current session user
		CheckPlanEditSession(nodeId, session);


		TA_Base_Core::ENode nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(nodeId);
		TA_Base_Core::NodeId parent = TA_Base_Bus::PlanDataReader::instance()->getParent(nodeId);

		if ( nodeType != TA_Base_Core::PLAN_NODE)
		{
			message << "Error: The node [" << detail.plan << "] is not of type PLAN."; 
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
		}

		TA_Base_Core::IDatabase* dataBase = getDatabase();

		std::ostringstream sql;

		sql << "update node_tree set name = '"
			<< parseForApostrophe(detail.name.in()).c_str()
			<< "',description = '"
			<< parseForApostrophe(detail.description.in()).c_str()
			<< "',fk_approval_state = "
			<< " ( select p_key from approval_state where state_value = "
			<< detail.approvalState << "), "
			<< "node_modified_by = '"
            << getOperatorName(session).c_str() << "', "
            << "date_modified = sysdate "
			<< " where p_key = " << nodeId;

		dataBase->executeModification(sql.str());
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(dataBase); //  CL-21716
		TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
		TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(nodeId);
		pcud->configUpdate.updatePlan(ns);

		ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( *pcud._retn() );

		TA_Base_Core::DescriptionParameters descriptionParameters;
		//xufeng++ TD14927
		TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
		descriptionParameters.push_back ( &pair0 );
		//++xufeng TD14927
        TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(nodeId));
		descriptionParameters.push_back ( &pair1 );
		AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanUpdate, descriptionParameters);
	}


	void PlanDataWritingClass::updatePlan(const std::string& session, const TA_Base_Core::Plan& plan)
	{
		std::ostringstream message;

		TA_Base_Core::NodeId nodeId = plan.detail.plan;

		//Check whether the Plan is being edited by the current session user
		CheckPlanEditSession(nodeId, session);

		TA_Base_Core::ENode nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(nodeId);
		TA_Base_Core::NodeId parent = TA_Base_Bus::PlanDataReader::instance()->getParent(nodeId);

		if ( nodeType != TA_Base_Core::PLAN_NODE)
		{
			message << "Error: The node [" << plan.detail.name << "] is not of type PLAN."; 
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.str().c_str());
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
		}

		//Make the Plan pass through some sanity checks before updating
		validatePlan(plan);

		// Delete all the steps of the plans
		// Update the details of the plan
		TA_Base_Core::IDatabase* dataBase = getDatabase();

		std::ostringstream sql;
		sql << "update node_tree set name = '"
			<< parseForApostrophe(plan.detail.name.in()).c_str() 
			<< "',description = '"
			<< parseForApostrophe(plan.detail.description.in()).c_str() 
			<< "',fk_approval_state = "
			<< " ( select p_key from approval_state where state_value = "
			<< plan.detail.approvalState << "), "
			<< "node_modified_by = '"
            << getOperatorName(session).c_str() << "', "
            << "date_modified = sysdate "
			<< " where p_key = " << nodeId;

		dataBase->executeModification(sql.str());
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(dataBase); //  CL-21716
		int numberOfSteps = plan.stepseq.length();

		TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
		TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(nodeId);
		pcud->configUpdate.updatePlan(ns);

		if ( numberOfSteps == 0 )
		{
			ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( *pcud._retn() );

		    TA_Base_Core::DescriptionParameters descriptionParameters;
			//xufeng++ TD14927
			TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
			descriptionParameters.push_back ( &pair0 );
			//++xufeng TD14927
            TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(nodeId));
		    descriptionParameters.push_back ( &pair1 );
		    AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanUpdate, descriptionParameters);
			return;			
		}

		//Delete all the existing step parameters and steps of this plan
		std::ostringstream sql1;
		sql1 << " delete from step_parameters where fk_step_details in "
			<< " (select p_key from step_details where fk_node_tree = " << nodeId << ")";

		std::ostringstream sql2;
		sql2 << " delete from step_details where fk_node_tree = " << nodeId;

		dataBase->executeModification(sql1.str()); //delete all step parameters
		dataBase->executeModification(sql2.str()); //delete all steps

		// We are here now to insert all the steps into step_details

////-------------

		const TA_Base_Core::AbortParameters* abortParams = NULL;
		const TA_Base_Core::ActivePlanCheckParameters* planCheckParams = NULL;
		const TA_Base_Core::AssignCameraToMonitorParameters* cameraToMonitorParams = NULL;
		const TA_Base_Core::DataPointCheckParameters* dpCheckParams = NULL;
		const TA_Base_Core::DataPointSetParameters* dpSetParams = NULL;
		const TA_Base_Core::DecisionParameters* decisionParams = NULL;
		const TA_Base_Core::JumpParameters* jumpParams = NULL;
		const TA_Base_Core::LogParameters* logParams = NULL;
		const TA_Base_Core::PromptParameters* promptParams = NULL;
		const TA_Base_Core::RunPlanParameters* runPlanParams = NULL;
		const TA_Base_Core::TerminatePlanParameters* terminatePlanParams = NULL;
		const TA_Base_Core::VmsSetSceneParameters* vmsSetSceneParams = NULL;
		const TA_Base_Core::DataPointValue* dpValue = NULL;
		const TA_Base_Core::NumericDataPoint* ndp = NULL;
		const TA_Base_Core::LaunchGuiAppParameters* lgap = NULL;
		const TA_Base_Core::SendSmsMessageParameters* sendSmsMessageParams = NULL;
		const TA_Base_Core::EvaluateExpressionParameters* evalExpressionParams = NULL;
		const TA_Base_Core::FailureSummaryParameters* failureSummaryParams = NULL;		

		for ( int m = 0; m < numberOfSteps ; m++ )
		{	//Add Step into step_details
			const TA_Base_Core::StepDetail* stepDetail = &(plan.stepseq[m].detail);
			const TA_Base_Core::StepParameters* stepParameters = &(plan.stepseq[m].parameters);

			if ( stepDetail->type != stepParameters->_d() )
			{
				//TODO: Throw exception
				//Mismatch between Step Type and its parameters 
				return;
			}

			std::ostringstream sqlstep;

			sqlstep << "insert into step_details ( p_key, fk_step_type, fk_node_tree,"
				    << "position, description, skip, skippable, delay, name, ignore_failure, no_wait ) "
					<< " values "
					<< "( " << stepDetail->step 
					<< ",(select p_key from step_type where type_value =" << stepDetail->type << ")"
					<< "," << nodeId
					<< "," << stepDetail->position
					<< ",'" << parseForApostrophe(stepDetail->description.in()).c_str()
					<< "'," << stepDetail->skip
					<< "," << stepDetail->skippable
					<< "," << stepDetail->delay
					<< ",'" << parseForApostrophe(stepDetail->name.in()).c_str()
                    << "'"
					<< "," << stepDetail->ignoreFailure
					<< "," << stepDetail->nowait
					<< ")";

			dataBase->executeModification(sqlstep.str());
			std::vector<std::string> valueVector; //To store the modified param values

			if ( stepDetail->type != stepParameters->_d() )
			{
				//TODO: Throw exception
				//Mismatch between Step Type and its parameters 
				return;
			}

            const std::string STEP_NUMBER_PREFIX("#");
            std::ostringstream ostrstrm;

			switch(stepParameters->_d())
			{
			case TA_Base_Core::END_STEP:
				break;

			case TA_Base_Core::ABORT_STEP:
				break;

			case TA_Base_Core::ACTIVE_PLAN_CHECK_STEP:
                // TD8736: Originally, the plan parameter was stored as a plan ID, and the yes/no step parameter
                //         as a step ID. These have been changed to plan path and step position, respectively.
                //         Plan Manager continues to support the Active Plan Check step in terms of plan ID and
                //         step ID. These parameters need to be converted before being written to database. 
                //         They have already been verified by the time we get here, so the conversion is safe.

				planCheckParams = &(stepParameters->activePlanCheck());
				ostrstrm << TA_Base_Bus::PlanDataReader::instance()->getNodePath(planCheckParams->plan);
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

				ostrstrm << STEP_NUMBER_PREFIX << getStepNumber(plan, planCheckParams->yesStep);
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

				ostrstrm << STEP_NUMBER_PREFIX << getStepNumber(plan, planCheckParams->noStep);
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");
				break;

			case TA_Base_Core::ASSIGN_CAMERA_TO_MONITOR_STEP:
            {
                #if defined ( STEPTYPE_ASSIGN_CAMERA_TO_MONITOR )
				cameraToMonitorParams = &( stepParameters->cameraMonitor() );
                ostrstrm << cameraToMonitorParams->camera;
				valueVector.push_back( ostrstrm.str() );
                ostrstrm.str("");
                ostrstrm << cameraToMonitorParams->monitor;
				valueVector.push_back( ostrstrm.str() );
                #endif

            }
			break;

			case TA_Base_Core::DATA_POINT_CHECK_STEP:
				dpCheckParams = &(stepParameters->dataPointCheck());
				valueVector.push_back(dpCheckParams->dataPoint.in());
				dpValue = &(dpCheckParams->value);

				switch ( dpValue->_d() )
				{
				case TA_Base_Core:: BOOLEAN_DATA_POINT:
					ostrstrm << TA_Base_Core::BOOLEAN_DATA_POINT << ",";
					ostrstrm << dpValue->booleanValue();				
					valueVector.push_back(ostrstrm.str());
					break;

				case TA_Base_Core::NUMERIC_DATA_POINT:
                    valueVector.push_back(makeNumericDataPointValueString( dpValue->numericValue() ));
					break;

				case TA_Base_Core::TEXT_DATA_POINT:
					ostrstrm << TA_Base_Core::TEXT_DATA_POINT << ",";
					ostrstrm << dpValue->textValue();
					valueVector.push_back(ostrstrm.str());
					break;

				default:
					//Throw exception
					break;
				}

				ostrstrm.str("");
				ostrstrm << dpCheckParams->testType;
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

                // TD8736: Originally, the success/failure step parameter was stored as a step ID. It has been changed 
                //         to step position. Plan Manager continues to support the Data Point Check step in terms of 
                //         step ID, so these parameters need to be converted before being written to database. 
                //         They have already been verified by the time we get here, so the conversion is safe.

				ostrstrm << STEP_NUMBER_PREFIX << getStepNumber(plan, dpCheckParams->successStep);
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

				ostrstrm << STEP_NUMBER_PREFIX << getStepNumber(plan, dpCheckParams->failureStep);
				valueVector.push_back(ostrstrm.str());
				break;

			case TA_Base_Core::DATA_POINT_SET_STEP:
				dpSetParams = &( stepParameters->dataPointSet() );
				valueVector.push_back(dpSetParams->dataPoint.in());
				dpValue = &(dpSetParams->newValue);

				switch(dpValue->_d())
				{
				case TA_Base_Core::BOOLEAN_DATA_POINT:
					ostrstrm << TA_Base_Core::BOOLEAN_DATA_POINT << ",";
					ostrstrm << dpValue->booleanValue();
					valueVector.push_back(ostrstrm.str());
					break;

				case TA_Base_Core::NUMERIC_DATA_POINT:
                    valueVector.push_back(makeNumericDataPointValueString( dpValue->numericValue() ));
					break;

				case TA_Base_Core::TEXT_DATA_POINT:
					ostrstrm << TA_Base_Core::TEXT_DATA_POINT << ",";
					ostrstrm << dpValue->textValue();
					valueVector.push_back(ostrstrm.str());
					break;
				}

				break;

			case TA_Base_Core::DECISION_STEP:
                // TD8736: Originally, the yes/no step parameter was stored as a step ID. It has been changed 
                //         to step position. Plan Manager continues to support the Decision step in terms of 
                //         step ID, so these parameters need to be converted before being written to database. 
                //         They have already been verified by the time we get here, so the conversion is safe.

				decisionParams = &( stepParameters->decision() );
				valueVector.push_back(parseForApostrophe(decisionParams->decisionMessage.in()));
				ostrstrm << decisionParams->responseTimeout;
				valueVector.push_back(ostrstrm.str());

				ostrstrm.str("");
				ostrstrm << STEP_NUMBER_PREFIX << getStepNumber(plan, decisionParams->yesStep);
				valueVector.push_back(ostrstrm.str());

				ostrstrm.str("");
				ostrstrm << STEP_NUMBER_PREFIX << getStepNumber(plan, decisionParams->noStep);
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

				//DP-changes
				// add support to the new parameters for the decision step
				// yesButtonLabel
				valueVector.push_back(parseForApostrophe(decisionParams->yesButtonLabel.in()));

				//noButtonLabel
				valueVector.push_back(parseForApostrophe(decisionParams->noButtonLabel.in()));

				//pauseButtonLabel
				valueVector.push_back(parseForApostrophe(decisionParams->pauseButtonLabel.in()));

				//noButtonEnabled
				ostrstrm.str("");
                if ( decisionParams->noButtonEnabled )                
                    ostrstrm << 1;
                else
                    ostrstrm << 0;
                valueVector.push_back(ostrstrm.str());

				//pauseButtonEnabled
				ostrstrm.str("");
                if ( decisionParams->pauseButtonEnabled )                
                    ostrstrm << 1;
                else
                    ostrstrm << 0;
                valueVector.push_back(ostrstrm.str());
				
				ostrstrm.str("");
				//++lizettejl
				break;

			case TA_Base_Core::JUMP_STEP:
                // TD8736: Originally, the target step parameter was stored as a step ID. It has been changed 
                //         to step position. Plan Manager continues to support the Jump step in terms of 
                //         step ID, so this parameter needs to be converted before being written to database. 
                //         It has already been verified by the time we get here, so the conversion is safe.

				jumpParams = &( stepParameters->jump() );
				ostrstrm << STEP_NUMBER_PREFIX << getStepNumber(plan, jumpParams->step);
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");
				break;

			case TA_Base_Core::LOG_STEP:
				logParams = &(stepParameters->log());
				valueVector.push_back(parseForApostrophe(logParams->message.in()));
                ostrstrm.str("");
                if ( logParams->logRequired )                
                    ostrstrm << 1;
                else
                    ostrstrm << 0;
                valueVector.push_back(ostrstrm.str());
                ostrstrm.str("");
				ostrstrm << logParams->responseTimeout;
				valueVector.push_back(ostrstrm.str());
				break;

            case TA_Base_Core::ACTIVATE_CAMERA_PRESET_STEP:
            {
				#if defined ( STEPTYPE_ACTIVATE_CAMERA_PRESET )
                TA_Base_Core::ActivateCameraPresetParameters presetParams= stepParameters->cameraPreset();
                ostrstrm << presetParams.camera;
				valueVector.push_back( ostrstrm.str() );
                ostrstrm.str("");
                ostrstrm << presetParams.preset;
				valueVector.push_back( ostrstrm.str() );
                #endif
            }
			break;

			case TA_Base_Core::PROMPT_STEP:
				promptParams = &(stepParameters->prompt());
				valueVector.push_back( parseForApostrophe(promptParams->message.in()) );
				ostrstrm << promptParams->dismissTimeout;
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");
				break;

			case TA_Base_Core::RUN_PLAN_STEP:
                // TD8736: Originally, the plan parameter was stored as a plan ID. This has been changed
                //         to be the plan path. The Run Plan step is still supported by the Plan Manager
                //         in terms of plan ID, so this needs to be converted to the corresponding path
                //         before being written to database. The plan ID has already been verified by the
                //         time we get here, so the conversion is safe.

				runPlanParams = &(stepParameters->runPlan());
				ostrstrm << TA_Base_Bus::PlanDataReader::instance()->getNodePath(runPlanParams->plan);
				valueVector.push_back(ostrstrm.str());

                ostrstrm.str("");
				ostrstrm << runPlanParams->manualLaunch;
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

				ostrstrm << runPlanParams->waitForCompletion;
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");
				break;

			case TA_Base_Core::TERMINATE_PLAN_STEP:
                // TD8736: Originally, the plan parameter was stored as a plan ID. This has been changed to
                //         be the plan path. The Terminate Plan step is still supported by the Plan Manager
                //         in terms of plan ID, so this needs to be converted to the corresponding path
                //         before being written to database. The plan ID has already been verified by the
                //         time we get here, so the conversion is safe.

				terminatePlanParams = &(stepParameters->terminatePlan());
				ostrstrm << TA_Base_Bus::PlanDataReader::instance()->getNodePath(terminatePlanParams->plan);
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");
				break;

			case TA_Base_Core::EVALUATE_EXPRESSION_STEP:
                // TD8736: Originally, the success/failure step parameter was stored as a step ID. It has been changed 
                //         to step position. Plan Manager continues to support the Evaluate Expression step in terms of 
                //         step ID, so these parameters need to be converted before being written to database. 
                //         They have already been verified by the time we get here, so the conversion is safe.

				evalExpressionParams = &(stepParameters->evaluateExpression());
				valueVector.push_back(evalExpressionParams->expression.in());
				ostrstrm.str("");

				ostrstrm << STEP_NUMBER_PREFIX << getStepNumber(plan, evalExpressionParams->successStep);
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

				ostrstrm << STEP_NUMBER_PREFIX << getStepNumber(plan, evalExpressionParams->failureStep);
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");
				break;


			case TA_Base_Core::VMS_SET_SCENE_STEP:
            {
                #if defined ( STEPTYPE_VMS_SET_SCENE )
				vmsSetSceneParams = &(stepParameters->vmsSetScene());
				ostrstrm << vmsSetSceneParams->vmsSign;
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

				ostrstrm << vmsSetSceneParams->scene;
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");
                #endif
            }
			break;

            case TA_Base_Core::LAUNCH_GUI_APP_STEP:
                lgap = &(stepParameters->guiAppParameters());
                ostrstrm << lgap->app;
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");

                valueVector.push_back(lgap->args.in());

                ostrstrm << lgap->pos.top   <<  ","
                         << lgap->pos.left  << ","
                         << lgap->pos.width << ","
                         << lgap->pos.height;

				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");
                break;

			case TA_Base_Core::VMS_BLANKOUT_STEP:
            {
                #if defined ( STEPTYPE_VMS_BLANKOUT )
                TA_Base_Core::VmsBlankoutParameters vms = stepParameters->vmsBlankout();
				ostrstrm << vms.vmsSign;
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");
                #endif
            }
			break;

			case TA_Base_Core::VMS_RESTORE_SCHEDULE_STEP:
            {
                #if defined ( STEPTYPE_VMS_RESTORE_SCHEDULE )
                TA_Base_Core::VmsRestoreScheduleParameters vms = stepParameters->vmsRestoreSchedule();
				ostrstrm << vms.vmsSign;
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");
                #endif
            }
			break;

			case TA_Base_Core::VIDEO_WALL_STEP:
            {
                #if defined ( STEPTYPE_VIDEOWALL )
                TA_Base_Core::VideoWallParameters videoWall = stepParameters->videoWallParams();
				ostrstrm << videoWall.wallBoard;
				valueVector.push_back(ostrstrm.str());

				ostrstrm.str("");
				ostrstrm << videoWall.layout;
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");
                #endif

            }
			break;

            case TA_Base_Core::SEND_SMS_MESSAGE_STEP:
            {
                #if defined (STEPTYPE_SEND_SMS_MESSAGE)
                TA_Base_Core::SendSmsMessageParameters sendSmsMessageParams = stepParameters->sendSmsMessage();
                ostrstrm << std::string(sendSmsMessageParams.recipientIdentifier);
                valueVector.push_back( ostrstrm.str() );
                ostrstrm.str("");

                ostrstrm << std::string(sendSmsMessageParams.textMessage);
                valueVector.push_back( ostrstrm.str() );
                ostrstrm.str("");
                #endif
            }
			break;

            case TA_Base_Core::LAUNCH_SCHEMATIC_STEP:
            {
                #if defined (STEPTYPE_LAUNCH_SCHEMATIC)
                TA_Base_Core::LaunchSchematicParameters launchSchematicParams = stepParameters->launchSchematic();

                ostrstrm.str("");
                ostrstrm << launchSchematicParams.schematic.in();
                valueVector.push_back( ostrstrm.str() );

                ostrstrm.str("");
                ostrstrm << launchSchematicParams.screen;
                valueVector.push_back( ostrstrm.str() );

                ostrstrm.str("");
                ostrstrm << launchSchematicParams.commands.in();
                valueVector.push_back( ostrstrm.str() );
                #endif
            }
			break;

            case TA_Base_Core::STATION_TRAVELLER_INFO_SYSTEM_STEP:
            {
                #if defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)
                TA_Base_Core::StationTravellerInfoSystemParameters stationTisParams = stepParameters->stationTravellerInfoSystem();
                const std::string SEPARATOR(",");

                // The station TIS message destination parameter is formatted like so:
                // "station,all_stations"
                ostrstrm.str("");
                ostrstrm << stationTisParams.destination.station.in() << SEPARATOR
                         << stationTisParams.destination.allStations;
                valueVector.push_back(ostrstrm.str());

                // Station level
                valueVector.push_back(stationTisParams.stationLevel.in());
            
                // The station TIS message is formatted like so:
                // "library_version,library_section,message_id"
                ostrstrm.str("");
                ostrstrm << stationTisParams.message.libraryVersion << SEPARATOR
                         << stationTisParams.message.librarySection << SEPARATOR
                         << stationTisParams.message.messageId;
                valueVector.push_back(ostrstrm.str());

                // Message priority
                ostrstrm.str("");
                ostrstrm << stationTisParams.priority;
                valueVector.push_back(ostrstrm.str());

                // Message duration
                ostrstrm.str("");
                ostrstrm << stationTisParams.duration;
                valueVector.push_back(ostrstrm.str());

                // Now the PIDs. It would have been nice to be able to stuff all the PIDs into a single
                // parameter, however, the parameter value column in the database has a limited size.
                // The PIDs are therefore split into a number of parameters rows, each holding a small 
                // subset of the PID values, in the format "pid1,pid2, ... ,pidN".
                // 
                // If no PIDs have been specified, then the Duration parameter will be the last row 
                // in the parameter table for this step.
                const unsigned long PIDS_PER_ROW(15);
                unsigned long pidIdx(0);

                while (pidIdx < stationTisParams.stationPids.length())
                {
                    ostrstrm.str("");

                    for (unsigned long i(0);
                         i < PIDS_PER_ROW && pidIdx < stationTisParams.stationPids.length();
                         i++, pidIdx++)
                    {
                        ostrstrm << stationTisParams.stationPids[pidIdx].in() << SEPARATOR;
                    }

                    valueVector.push_back(ostrstrm.str());
                }


                #endif
            }
			break;

            case TA_Base_Core::TRAIN_TRAVELLER_INFO_SYSTEM_STEP:
            {
                #if defined (STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM)
                TA_Base_Core::TrainTravellerInfoSystemParameters trainTisParams = stepParameters->trainTravellerInfoSystem();
                const std::string SEPARATOR(",");

                // The train TIS message destination parameter is formatted like so:
                // "train_id,train_group"

                ostrstrm.str("");
                ostrstrm << trainTisParams.destination.trainId << SEPARATOR
                         << trainTisParams.destination.trainGroup;
                 valueVector.push_back( ostrstrm.str() );

                // The train TIS message is formatted like so:
                // "library_version,library_section,message_id"
                
                ostrstrm.str("");
                ostrstrm << trainTisParams.message.libraryVersion << SEPARATOR
                         << trainTisParams.message.librarySection << SEPARATOR
                         << trainTisParams.message.messageId;
                valueVector.push_back( ostrstrm.str() );

                ostrstrm.str("");
                ostrstrm << trainTisParams.priority;
                valueVector.push_back( ostrstrm.str() );

                ostrstrm.str("");
                ostrstrm << trainTisParams.duration;
                valueVector.push_back( ostrstrm.str() );
                #endif
            }
			break;

			 case TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP:
            {
					 #if defined (STEPTYPE_STATION_PUBLIC_ADDRESS)
                TA_Base_Core::StationPublicAddressParameters stationPAParams = stepParameters->stationPublicAddress();
                const std::string SEPARATOR(",");

                // The station destination parameter is formatted like so:
                // "station,all_stations"
                ostrstrm.str("");
                ostrstrm << stationPAParams.station.station.in() << SEPARATOR
                         << stationPAParams.station.allStations;
                valueVector.push_back(ostrstrm.str());

               
                // The station PA message is formatted like so:
                // "library_version,library_section,message_id"
                ostrstrm.str("");
                ostrstrm << stationPAParams.message.libVersion << SEPARATOR
                         << stationPAParams.message.libSection << SEPARATOR
                         << stationPAParams.message.messageId;
                valueVector.push_back(ostrstrm.str());

               
					 //chime 
					 ostrstrm.str("");
                if ( stationPAParams.chime )                
                    ostrstrm << 1;
                else
                    ostrstrm << 0;
                valueVector.push_back(ostrstrm.str());

					 //cyclic
					 ostrstrm.str("");
                if ( stationPAParams.cyclic )                
                    ostrstrm << 1;
                else
                    ostrstrm << 0;
                valueVector.push_back(ostrstrm.str());

					 //TIS Sync
					 ostrstrm.str("");
                if ( stationPAParams.TISSynch )                
                    ostrstrm << 1;
                else
                    ostrstrm << 0;
                valueVector.push_back(ostrstrm.str());

                // Message duration
                ostrstrm.str("");
                ostrstrm << stationPAParams.duration;
                valueVector.push_back(ostrstrm.str());

					 //Message interval
					 ostrstrm.str("");
                ostrstrm << stationPAParams.interval;
                valueVector.push_back(ostrstrm.str());

                // the parameter value column in the database has a limited size.
                // The zones are therefore split into a number of parameters rows, each holding a small 
                // subset of the  zoneID values, in the format "zoneid1,zoneid2, ... ,zoneidN".
                const unsigned long ZONES_PER_ROW(15);
                unsigned long zoneIdx(0);

                while (zoneIdx < stationPAParams.zones.length())
                {
                    ostrstrm.str("");

                    for (unsigned long i(0);
                         i < ZONES_PER_ROW && zoneIdx < stationPAParams.zones.length();
                         i++, zoneIdx++)
                    {
                        ostrstrm << stationPAParams.zones[zoneIdx] << SEPARATOR;
                    }

                    valueVector.push_back(ostrstrm.str());
                }

                #endif
            }
			break;

			case TA_Base_Core::TRAIN_PUBLIC_ADDRESS_STEP:
            {
					 #if defined (STEPTYPE_TRAIN_PUBLIC_ADDRESS)
                TA_Base_Core::TrainPublicAddressParameters trainPAParams = stepParameters->trainPublicAddress();
                const std::string SEPARATOR(",");

                 // The train PA destination parameter is formatted like so:
                // "train_id,train_group"
                ostrstrm.str("");
                ostrstrm << trainPAParams.destination.trainId << SEPARATOR
                         << trainPAParams.destination.trainGroup;
                 valueVector.push_back( ostrstrm.str() );

               
                // The station PA message is formatted like so:
                // "library_version,library_section,message_id"
                ostrstrm.str("");
                ostrstrm << trainPAParams.message.libVersion << SEPARATOR
                         << trainPAParams.message.libSection << SEPARATOR
                         << trainPAParams.message.messageId;
                valueVector.push_back(ostrstrm.str());

               
					 //cyclic
					 ostrstrm.str("");
                if ( trainPAParams.cyclic )                
                    ostrstrm << 1;
                else
                    ostrstrm << 0;
                valueVector.push_back(ostrstrm.str());


                // Message duration
                ostrstrm.str("");
                ostrstrm << trainPAParams.duration;
                valueVector.push_back(ostrstrm.str());

                #endif
            }
			break;
			//lizettejl++ (DP-Changes)
			case TA_Base_Core::FAILURE_SUMMARY_STEP:
			{
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "*****$$$$$######INFO: stepParameters->_d()=%d",TA_Base_Core::FAILURE_SUMMARY_STEP );
				
				#if defined (STEPTYPE_FAILURE_SUMMARY_STEP) 				
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "STEPTYPE_FAILURE_SUMMARY_STEP");
				failureSummaryParams = &(stepParameters->failureSummary());
				ostrstrm.str("");				
				ostrstrm << failureSummaryParams->failureCategory;
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "*****$$$$$######INFO: failureSummaryParams->failureCategory=%d",failureSummaryParams->failureCategory);
				valueVector.push_back(ostrstrm.str());
				ostrstrm.str("");
				#endif // STEPTYPE_FAILURE_SUMMARY_STEP
			}
			break;
			//++lizettejl	
			default:
				break;
			}

	//	update step_parameters 
	//	set value = 'Decide MP' 
	//	where
	//	position = 1
	//	and 
	//	fk_step_details = (select step_details.p_key from step_details where fk_node_tree = 6 and position = 1);
			TA_Base_Core::IDatabase* dataBase = getDatabase();

			std::vector<std::string>::iterator it;
			int position = 1;

			for ( it = valueVector.begin(); it != valueVector.end(); it++)
			{
				std::ostringstream sql;
				sql << "insert into step_parameters (p_key, fk_step_details, position, value) "
					<< " values "
					<< "( plans_seq.nextval, " << stepDetail->step  
					<< "," 
					<< position++
					<< ",'" << parseForApostrophe(*it) << "')";

				std::string strSQL = sql.str();

				dataBase->executeModification(sql.str());
			}	
			TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(dataBase); //  CL-21716
		}

		TA_Base_Core::DescriptionParameters descriptionParameters;
		//xufeng++ TD14927
		TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
		descriptionParameters.push_back ( &pair0 );
		//++xufeng TD14927
        TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(nodeId));
		descriptionParameters.push_back ( &pair1 );
		AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanUpdate, descriptionParameters);

		ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( *pcud._retn() );	
	}

	void PlanDataWritingClass::copyPlan(const std::string& session, TA_Base_Core::NodeId plan,
		                                TA_Base_Core::NodeId parent, bool useForce)
	{
		std::ostringstream message;

		TA_Base_Core::NodeSummary_var oldSummary = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(plan);

		if ( oldSummary->type != TA_Base_Core::PLAN_NODE)
		{
			message << "Error: The node [" << plan << "] is not of type PLAN."; 
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
		}

		TA_Base_Core::ENode nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(parent);

		if ( nodeType != TA_Base_Core::CATEGORY_NODE)
		{
			message << "Error: The given parent node [" << parent << "] is not of type CATEGORY."; 
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
		}

		std::string name = oldSummary->name.in();
		bool overwriting = false;
		TA_Base_Core::NodeId existingPlan = 0;

		if ( oldSummary->parent != parent )
		{
			// if we have a conflicting plan
			if ( !TA_Base_Bus::PlanDataReader::instance()->canUseName(plan, parent, parseForApostrophe(name)) )
			{
				// get the ID of the existing plan
				existingPlan = getPlanToReplace( name, parent, useForce );		
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
            std::ostringstream sql;

			if (overwriting)
			{
		        sql << "call replace_plan("
                    << existingPlan << ","
			        << plan << ",'"
                    << getOperatorName(session) << "',"
                    << "1)"; // 1 = duplicate source plan
			}
			else
			{
		        sql << "call copy_plan("
			        << plan << ","
			        << parent << ",'"
			        << parseForApostrophe(name) << "','"
                    << getOperatorName(session)
                    << "')";
			}

		    std::cout << sql.str() << std::endl;

		    getDatabase()->executeProcedure(sql.str());

		    // We have successfully copied the plan, let us retrieve its NodeId and send the update	        
			newPlan = TA_Base_Bus::PlanDataReader::instance()->getNodeId( parent, parseForApostrophe(name) );
		}
		catch(...)
		{
		    TA_Base_Core::DescriptionParameters descriptionParameters;

			//xufeng++ TD14927
			TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "UNSUCCESSFUL");
			descriptionParameters.push_back ( &pair0 );
			//++xufeng TD14927

            TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(plan ));
		    descriptionParameters.push_back ( &pair1 );

            std::string destinationPlanName = TA_Base_Bus::PlanDataReader::instance()->getNodePath(parent);
            destinationPlanName += "/";
            destinationPlanName += TA_Base_Bus::PlanDataReader::instance()->getNodeName(plan );

            TA_Base_Core::NameValuePair pair2( "NewPlanName", destinationPlanName);
		    descriptionParameters.push_back ( &pair2 );


			 AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanCopyFailure, descriptionParameters);
			
			std::ostringstream alarmMessage;
			alarmMessage << "Plan Copy Failure:"
						 << TA_Base_Bus::PlanDataReader::instance()->getNodeName(plan).c_str()
						 << "[Id:"
						 << plan
						 << "] from Category[" 
						 << TA_Base_Bus::PlanDataReader::instance()->getNodeName(oldSummary->parent).c_str()
						 << "] to Category[" << TA_Base_Bus::PlanDataReader::instance()->getNodeName(parent).c_str() << "].";

			message << "Error: Failed to copy Plan [" << oldSummary->name.in() << "]."; 
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, alarmMessage.str().c_str());
			TA_THROW( TA_Base_Core::DataException( "Failed to copyPlan", TA_Base_Core::DataException::NO_VALUE, "" ) );
        }


		TA_Base_Core::PlanConfigUpdateData pcud;
		pcud.session = CORBA::string_dup(session.c_str());
        std::string sourcePlanPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(plan);
        std::string targetPlanPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(newPlan);

        // When replacing an existing plan, send a delete update before the copy update.
        if (overwriting)
        {
		    TA_Base_Core::DeleteNodeUpdate dnu;
		    dnu.parent = parent;
		    dnu.nodes.length(1);
            dnu.nodes[0] = existingPlan;

			if ( parent != TA_Base_Core::ROOT_NODE_ID )
            {
				pcud.configUpdate.deletePlans(dnu);
            }
			else
            {
				pcud.configUpdate.deletePlansFromRoot(dnu.nodes);
            }

            ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( pcud );

		    TA_Base_Core::DescriptionParameters descriptionParameters;
			//xufeng++ TD14927
			TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
			descriptionParameters.push_back ( &pair0 );
			//++xufeng TD14927
            TA_Base_Core::NameValuePair pair1( "PlanName", targetPlanPath);
		    descriptionParameters.push_back ( &pair1 );
		    AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanDelete, descriptionParameters);

		    std::ostringstream auditMessage;
		    auditMessage << targetPlanPath
					     << " [Id:"
					     << existingPlan
					     << "] has been deleted to make room for a copy of "
                         << sourcePlanPath
                         << " [Id:" 
					     << plan
					     << "].";

		    LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, auditMessage.str().c_str());
        }
            
		TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(newPlan);

		if ( parent != TA_Base_Core::ROOT_NODE_ID )
        {
			pcud.configUpdate.copyPlan(ns);
        }
		else
        {
			pcud.configUpdate.copyPlanToRoot( ns );
        }

		ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( pcud );

		TA_Base_Core::DescriptionParameters descriptionParameters;
		//xufeng++ TD14927
		TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
		descriptionParameters.push_back ( &pair0 );
		//++xufeng TD14927
		TA_Base_Core::NameValuePair pair1( "PlanName", sourcePlanPath);
		descriptionParameters.push_back ( &pair1 );
		TA_Base_Core::NameValuePair pair2( "NewPlanName", targetPlanPath);
		descriptionParameters.push_back ( &pair2 );
		AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanCopy, descriptionParameters);

		std::ostringstream auditMessage;
		auditMessage << sourcePlanPath
					 << " [Id:"
					 << plan
					 << "] has been copied to "
                     << targetPlanPath
                     << " [Id:" 
					 << newPlan
					 << "].";

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, auditMessage.str().c_str());
	}


	 TA_Base_Core::AuxPlanDetail* PlanDataWritingClass::exportPlan (const std::string& session, TA_Base_Core::NodeId& planId)
	 {
		 std::ostringstream message;
		 std::string nvlValue (" "); 
		 TA_Base_Core::ENode nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(planId);
		 TA_Base_Core::NodeId parent = TA_Base_Bus::PlanDataReader::instance()->getParent(planId);
		 
		 if ( nodeType != TA_Base_Core::PLAN_NODE)
		 {
			 message << "Error: The node being exported is not of type PLAN."; 
			 LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.str().c_str());
			 TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
		 }
		 
		 TA_Base_Core::AuxPlanDetail_var auxPlanDetail = new TA_Base_Core::AuxPlanDetail;
		 
		 //get plan details from DB
		 TA_Base_Core::IDatabase* database= getDatabase();

		 
		 std::ostringstream sql;
		 std::vector<std::string> columns;
		 
		 sql << "select name, nvl(description, '" << nvlValue.c_str() << "')"
          << " from plan_details where node_id = " << planId;
		 
		 columns.push_back( "name" );
		 columns.push_back( "description" );
       
		 std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( sql.str(), columns ) );
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(database); //  CL-21716
		 
		 if( (NULL == data.get()) || (0 == data->getNumRows()) ) 
		 {
			 TA_THROW( TA_Base_Core::DataException( "Failed to find node details", TA_Base_Core::DataException::NO_VALUE, sql.str() ) );
		 }
		 
		 
		 auxPlanDetail->name      = CORBA::string_dup (data->getStringData( 0, columns[0]).c_str());
		 
		 std::string description  = CORBA::string_dup (data->getStringData(0, columns[1]).c_str());
		 if ( description.compare(nvlValue) == 0 )
		 {
			 auxPlanDetail->description = CORBA::string_dup ("");
		 }
		 else
		 {
			 auxPlanDetail->description = CORBA::string_dup (description.c_str ());
		 }
		 
		 
		 //get step details
		 std::ostringstream stepSql;
		 
		 // Step details query
		 // Step details query
		stepSql << "select step_details.position, step_details.name,"
              << " step_type.type_value, nvl(step_details.description, '" << nvlValue.c_str() << "'),"
			     << " step_details.skip, step_details.skippable, step_details.delay,"
              << " nvl(step_details.ignore_failure, 0), nvl(step_details.no_wait, 0) "
			     << " from step_details, node_tree, step_type"
			     << " where step_details.fk_node_tree = node_tree.p_key"
			     << " and step_details.fk_step_type = step_type.p_key"
			     << " and step_details.fk_node_tree = " << planId;


		 stepSql << " order by step_details.position";
		 
		 columns.clear ();
		 columns.resize (0);
		 columns.push_back( "position" );
		 columns.push_back( "name" );
       columns.push_back( "type" );
       columns.push_back( "description" );
		 columns.push_back( "skip" );
		 columns.push_back( "skippable" );
		 columns.push_back( "delay" ); 
		 columns.push_back( "ignore_failure" ); 
		 columns.push_back( "no_wait" );

		 
		 TA_Base_Core::IData* stepData = database->executeQuery( stepSql.str(), columns ) ;
		 
		 if( (NULL == stepData) || (0 == stepData->getNumRows()) )
		 {
			 auxPlanDetail->steps.length(0);
			 return auxPlanDetail._retn();
		 }
		 
		 unsigned long rowsFetched = 0;
		 do
		 {
			 std::auto_ptr<TA_Base_Core::IData> dataPtr(stepData);
			 auxPlanDetail->steps.length( stepData->getNumRows() + rowsFetched );
			 
			 for( unsigned long dataIdx = 0; dataIdx < dataPtr->getNumRows(); dataIdx++ )
			 {
				auxPlanDetail->steps[rowsFetched + dataIdx].position    = dataPtr->getUnsignedLongData(dataIdx, columns[0]);
				auxPlanDetail->steps[rowsFetched + dataIdx].name        = CORBA::string_dup( dataPtr->getStringData( dataIdx, columns[1] ).c_str() );
				auxPlanDetail->steps[rowsFetched + dataIdx].type        = static_cast<TA_Base_Core::EStep>( dataPtr->getUnsignedLongData( dataIdx, columns[2]));
				//description can be a bit tricky
				 std::string description = CORBA::string_dup (dataPtr->getStringData(dataIdx, columns[3] ).c_str());
				 if ( description.compare(nvlValue) == 0 )
				 {
					 auxPlanDetail->steps[rowsFetched + dataIdx].description = CORBA::string_dup ("");
				 }
				 else
				 {
					 auxPlanDetail->steps[rowsFetched + dataIdx].description = CORBA::string_dup (description.c_str ());
				 } 
				
				auxPlanDetail->steps[rowsFetched + dataIdx].skip          = static_cast<CORBA::Boolean>( 0 != dataPtr->getUnsignedLongData( dataIdx, columns[4] ) );
				auxPlanDetail->steps[rowsFetched + dataIdx].skippable		 = static_cast<CORBA::Boolean>( 0 != dataPtr->getUnsignedLongData( dataIdx, columns[5] ) );
				auxPlanDetail->steps[rowsFetched + dataIdx].delay         = static_cast<TA_Base_Core::Seconds>( dataPtr->getUnsignedLongData( dataIdx, columns[6] ) );
				auxPlanDetail->steps[rowsFetched + dataIdx].ignoreFailure = static_cast<CORBA::Boolean>( 0 != dataPtr->getUnsignedLongData( dataIdx, columns[7] ) );
				auxPlanDetail->steps[rowsFetched + dataIdx].nowait = static_cast<CORBA::Boolean>( 0 != dataPtr->getUnsignedLongData( dataIdx, columns[8] ) ); 
			 }

			 rowsFetched += dataPtr->getNumRows();
			 dataPtr = std::auto_ptr<TA_Base_Core::IData>(NULL);		// ensure memory is released
			 stepData = NULL;
		 }
		 while ( database->moreData( stepData ) );

		 // TD20400 Yanrong++
		 // Fill up parameters for each step of plan
		 for( int stepIndex = 0; stepIndex < auxPlanDetail->steps.length(); ++stepIndex )
		 {
			 //get step params
			 std::ostringstream sqlParams;
			 std::vector<std::string> paramCol;
			 
			 sqlParams  << "select nvl(step_parameters.value, '" << nvlValue.c_str() << "')"
							<< " from step_parameters, step_details"
							<< " where step_parameters.fk_step_details = step_details.p_key"
							<< " and step_details.fk_node_tree = " << planId
							<< " and step_details.position = " << auxPlanDetail->steps[stepIndex].position
							<< " order by step_parameters.position";
			 
			 paramCol.push_back( "value" );
			 
			 std::auto_ptr<TA_Base_Core::IData> paramData( database->executeQuery(sqlParams.str(), paramCol));

			 //some steps don't have parameters and that is fine
			 if( NULL != paramData.get() ) 
			 {
				 unsigned long numParams = paramData->getNumRows ();
				 auxPlanDetail->steps[stepIndex].stepParams.length (numParams);
				 for (unsigned long paramIdx = 0; paramIdx < numParams; paramIdx++)
				 {
					 auxPlanDetail->steps[stepIndex].stepParams [paramIdx].position = paramIdx + 1;//parameters should start from 1 as do steps
					 auxPlanDetail->steps[stepIndex].stepParams [paramIdx].value    = CORBA::string_dup (paramData->getStringData (paramIdx, paramCol [0]).c_str ()); 
				 }
			 }

		 }
		 // ++Yanrong
		 
		 
		 return auxPlanDetail._retn();
		 
	 }

	void PlanDataWritingClass::importPlan (const std::string& session, const TA_Base_Core::AuxPlanDetail& plan, TA_Base_Core::NodeId parentId)
	{
		//first insert imported plan into selected category

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planInsertLock);
		std::string nvlValue (" "); 
		std::string name = plan.name.in ();
		std::string description = plan.description.in ();
		//every plan imported is to have UNAPPROVED state to start with
		TA_Base_Core::EApprovalState approvalState = TA_Base_Core::UNAPPROVED_APPROVAL_STATE;
		unsigned long importStep = 0;//when importing don't add an extra end step in

		std::ostringstream message;
		std::ostringstream sql;

		sql << "call insert_plan("
			 << TA_Base_Core::PLAN_NODE << ","
			 << parentId << ",'"
			 << name.c_str() << "','"
			 << description.c_str() << "',"
			 << approvalState << ",'"
          << getOperatorName(session).c_str() << "',"
			 << importStep 
          << ")";

		std::string strSQL = sql.str();
		std::cout << strSQL.c_str() << std::endl;

		TA_Base_Core::IDatabase* dataBase = getDatabase();

		dataBase->executeProcedure(sql.str());

		//We have successfully inserted the new plan, let us retrieve its NodeId

		sql.str("");
		sql << "select p_key from node_tree where parent_node = "
			 << parentId << " and name = '"
			 << name.c_str() << "'";

      std::vector<std::string> columns;
		columns.push_back( "p_key" );

		std::auto_ptr<TA_Base_Core::IData> data( dataBase->executeQuery( sql.str(), columns ) );
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(dataBase); //  CL-21716

		if( (NULL == data.get()) || (1 != data->getNumRows()) ) 
        {
			message << "Failed to import a new Plan. Please check the plans_seq value, the current"
                    << " value might be conflicting with the existing entries in node_tree or step_details tables.";
            std::string strMessage = message.str();
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_CANNOT_CREATE_PLAN));
        }

		//get the new plan id
		TA_Base_Core::NodeId planId = data->getUnsignedLongData( 0, columns[0] );
		
		//now update plan step details
		for ( int i = 0;  i < plan.steps.length () ; i++ )
		{	
			std::ostringstream sqlstep;
			TA_Base_Core::StepId stepId = TA_Base_Bus::PlanDataReader::instance()->getNextPlanSeqNumber();
			
			sqlstep << "insert into step_details ( p_key, fk_step_type, fk_node_tree,"
				    << "position, description, skip, skippable, delay, name, ignore_failure, no_wait ) "
					<< " values "
					<< "( " << stepId 
					<< ",(select p_key from step_type where type_value =" << plan.steps[i].type << ")"
					<< ","  << planId
					<< ","  << plan.steps[i].position
					<< ",'" << parseForApostrophe(plan.steps[i].description.in()).c_str()
					<< "'," << plan.steps[i].skip
					<< ","  << plan.steps[i].skippable
					<< ","  << plan.steps[i].delay
					<< ",'" << parseForApostrophe(plan.steps[i].name.in()).c_str()
               << "'"
					<< ","  << plan.steps[i].ignoreFailure
					<< ","  << plan.steps[i].nowait
					<< ")";

			dataBase->executeModification(sqlstep.str());

			//and step params
			
			for (int pIdx = 0; pIdx < plan.steps[i].stepParams.length (); pIdx++)
			{
				std::ostringstream sqlParams;
				sqlParams << "insert into step_parameters (p_key, fk_step_details, position, value) "
					       << " values "
							 << "( plans_seq.nextval, " << stepId  
							 << "," 
							 << plan.steps[i].stepParams[pIdx].position
							 << ",'" << parseForApostrophe (plan.steps[i].stepParams[pIdx].value.in ()) << "')";

				dataBase->executeModification(sqlParams.str());
			}

		}

		
		//Send Update for insertPlan
		TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
		pcud->session = CORBA::string_dup(session.c_str());

		TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(planId);

		if ( parentId != TA_Base_Core::ROOT_NODE_ID )
			pcud->configUpdate.insertPlan(ns);
		else
			pcud->configUpdate.insertPlanIntoRoot(ns);

		ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( *pcud._retn() );

		TA_Base_Core::DescriptionParameters descriptionParameters;
		//xufeng++ TD14927
		TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
		descriptionParameters.push_back ( &pair0 );
		//++xufeng TD14927
		TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(planId));
		descriptionParameters.push_back ( &pair1 );
		AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanInsert, descriptionParameters);

		std::ostringstream auditMessage;

		auditMessage << TA_Base_Bus::PlanDataReader::instance()->getNodeName(planId).c_str()
					    << "[Id:"
						 << planId
					    << "] has been inserted into Category[" 
					    << TA_Base_Bus::PlanDataReader::instance()->getNodeName(parentId).c_str()
					    << "].";
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, auditMessage.str().c_str());
	}
	void PlanDataWritingClass::movePlan(const std::string& session, TA_Base_Core::NodeId plan, 
                                        TA_Base_Core::NodeId parent, bool useForce)
	{
		TA_Base_Core::NodeSummary_var oldSummary = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(plan);
		std::ostringstream message;

		if ( oldSummary->type != TA_Base_Core::PLAN_NODE)
		{
			message << "Error: The node [" << plan << "] is not of type PLAN."; 
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
		}

		TA_Base_Core::ENode nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(parent);

		if ( nodeType != TA_Base_Core::CATEGORY_NODE)
		{
			message << "Error: The node [" << parent << "] is not of type CATEGORY."; 
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_CATEGORY));
		}

		if( isPlanBeingEdited(plan) )
		{
			std::string editLockSession = getEditLockOwner(plan);
			message << "Error: The plan with name [" << oldSummary->name.in()
					<< "] is being edited by "
					<<	ActivePlanBulletinBoard::instance()->getOwner(editLockSession);
			TA_THROW( TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_EDIT_LOCKED_BY_OPERATOR, ActivePlanBulletinBoard::instance()->getOwnerId(editLockSession)) );

		}

        std::string sourcePlanPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(plan);
		bool overwriting = false;
		TA_Base_Core::NodeId existingPlan = 0;

        // if we have a duplicate name
		if ( !TA_Base_Bus::PlanDataReader::instance()->canUseName(plan, parent, parseForApostrophe(oldSummary->name.in())) )
		{
			// get the ID of the existing plan
			existingPlan = getPlanToReplace( oldSummary->name.in(), parent, useForce );		
			overwriting = true;
		}


		if (overwriting)
		{
            std::ostringstream sql;
		      sql << "call replace_plan("
                << existingPlan << ","
			       << plan << ","
                << "NULL," // N/A
                << "0)";   // 0 = move source plan

		    std::cout << sql.str() << std::endl;

		    getDatabase()->executeProcedure(sql.str());
		}
		else
		{
			TA_Base_Core::IDatabase* dataBase = getDatabase();

			std::ostringstream sql;
			sql << "update node_tree set parent_node = "
				<< parent
				<< " where p_key = " << plan;

		    std::cout << sql.str() << std::endl;

			dataBase->executeModification(sql.str());
			TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(dataBase); //  CL-21716
		}

		TA_Base_Core::PlanConfigUpdateData pcud;
		pcud.session = CORBA::string_dup(session.c_str());
        std::string targetPlanPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(plan); // same plan ID different parent now

        // When replacing an existing plan, send a delete update before the move update.
        if (overwriting)
        {
		    TA_Base_Core::DeleteNodeUpdate dnu;
		    dnu.parent = parent;
		    dnu.nodes.length(1);
            dnu.nodes[0] = existingPlan;

			if ( parent != TA_Base_Core::ROOT_NODE_ID )
            {
				pcud.configUpdate.deletePlans(dnu);
            }
			else
            {
				pcud.configUpdate.deletePlansFromRoot(dnu.nodes);
            }

            ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( pcud );

		    TA_Base_Core::DescriptionParameters descriptionParameters;
			//xufeng++ TD14927
			TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
			descriptionParameters.push_back ( &pair0 );
			//++xufeng TD14927
            TA_Base_Core::NameValuePair pair1( "PlanName", targetPlanPath);
		    descriptionParameters.push_back ( &pair1 );
		    AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanDelete, descriptionParameters);

		    std::ostringstream auditMessage;
		    auditMessage << targetPlanPath
					     << " [Id:"
					     << existingPlan
					     << "] has been deleted to make room for "
                         << sourcePlanPath
                         << " [Id:" 
					     << plan
					     << "].";

		    LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, auditMessage.str().c_str());
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

		if ( parent == TA_Base_Core::ROOT_NODE_ID )
        {
			pcud.configUpdate.movePlanToRoot( tnu );
        }
		else if ( oldSummary->parent == TA_Base_Core::ROOT_NODE_ID )
        {
			pcud.configUpdate.movePlanFromRoot( tnu );
        }
		else
        {
			pcud.configUpdate.movePlan( tnu );
        }

		ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( pcud );

		TA_Base_Core::DescriptionParameters descriptionParameters;
		//xufeng++ TD14927
		TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
		descriptionParameters.push_back ( &pair0 );
		//++xufeng TD14927
        TA_Base_Core::NameValuePair pair1( "PlanName", sourcePlanPath);
		descriptionParameters.push_back ( &pair1 );
        TA_Base_Core::NameValuePair pair2( "NewPlanName", targetPlanPath);
		descriptionParameters.push_back ( &pair2 );
		AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanMove, descriptionParameters);

		std::ostringstream auditMessage;
		auditMessage << sourcePlanPath
					 << " [Id:"
					 << plan
					 << "] has been moved to "
                     << targetPlanPath
                     << ".";
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, auditMessage.str().c_str());
	}

	void PlanDataWritingClass::insertPlan(const std::string& session, const TA_Base_Core::NodeId parent)
	{
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_planInsertLock);
		std::string name = getNewNodeName(parent, TA_Base_Core::PLAN_NODE);
		std::string description = name;
		description +=  " Description";
		TA_Base_Core::EApprovalState approvalState = TA_Base_Core::UNAPPROVED_APPROVAL_STATE;

		std::ostringstream message;
		std::ostringstream sql;

		sql << "call insert_plan("
			<< TA_Base_Core::PLAN_NODE << ","
			<< parent << ",'"
			<< name.c_str() << "','"
			<< description.c_str() << "',"
			<< approvalState << ",'"
            << getOperatorName(session).c_str()
            << "')";

		std::string strSQL = sql.str();
		std::cout << strSQL.c_str() << std::endl;

		TA_Base_Core::IDatabase* dataBase = getDatabase();

		dataBase->executeProcedure(sql.str());

		//We have successfully inserted the new plan, let us retrieve its NodeId
		//and send the update

		sql.str("");
		sql << "select p_key from node_tree where parent_node = "
			<< parent << " and name = '"
			<< name.c_str() << "'";

        std::vector<std::string> columns;
		columns.push_back( "p_key" );

		std::auto_ptr<TA_Base_Core::IData> data( dataBase->executeQuery( sql.str(), columns ) );
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(dataBase); //  CL-21716
		if( (NULL == data.get()) || (1 != data->getNumRows()) ) 
        {
			message << "Failed to create a new Plan. Please check the plans_seq value, the current"
                    << " value might be conflicting with the existing entries in node_tree or step_details tables.";
            std::string strMessage = message.str();
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_CANNOT_CREATE_PLAN));
        }

		TA_Base_Core::NodeId plan = data->getUnsignedLongData( 0, columns[0] );

		//Send Update for insertPlan

		TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
		pcud->session = CORBA::string_dup(session.c_str());

		TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(plan);

		if ( parent != TA_Base_Core::ROOT_NODE_ID )
			pcud->configUpdate.insertPlan(ns);
		else
			pcud->configUpdate.insertPlanIntoRoot(ns);

		ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( *pcud._retn() );

		TA_Base_Core::DescriptionParameters descriptionParameters;
		//xufeng++ TD14927
		TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
		descriptionParameters.push_back ( &pair0 );
		//++xufeng TD14927
        TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(plan));
		descriptionParameters.push_back ( &pair1 );
		AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanInsert, descriptionParameters);

		std::ostringstream auditMessage;

		auditMessage << TA_Base_Bus::PlanDataReader::instance()->getNodeName(plan).c_str()
					 << "[Id:"
					 << plan
					 << "] has been inserted into Category[" 
					 << TA_Base_Bus::PlanDataReader::instance()->getNodeName(parent).c_str()
					 << "].";
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, auditMessage.str().c_str());
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

		for (int i=0; i < numberOfPlans; i++)
		{
			plan = plans[i];
			TA_Base_Core::ENode nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(plan);
			message << "Error: The plan ";

			if ( nodeType != TA_Base_Core::PLAN_NODE)
			{
				message << "is not of type PLAN.";
                TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
			}


			if ( TA_Base_Core::APPROVED_APPROVAL_STATE == TA_Base_Bus::PlanDataReader::instance()->getPlanApprovalState(plan) )
			{
				message << "is approved and cannot be deleted.";
                TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_CANNOT_DELETE_APPROVED_PLAN));
			}


			if ( isPlanBeingEdited(plan) )
			{
                std::string editLockSession = getEditLockOwner(plan);
				message << "is being edited by " << ActivePlanBulletinBoard::instance()->getOwner(editLockSession) << ".";
                TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_EDIT_LOCKED_BY_OPERATOR, ActivePlanBulletinBoard::instance()->getOwnerId(editLockSession)));
			}

			//dnu.operator [](i).parent = TA_Base_Bus::PlanDataReader::instance()->getParent(plan);
			dnu->nodes[i] = plan;
		}


		//We are here means we can delete all the given plans
		TA_Base_Core::IDatabase* dataBase = getDatabase();

		std::ostringstream notDeletedPlans;
		int undeletedPlans = 0;
		std::vector <int> VectorUndeletedPlans;

		for ( int j = 0; j < numberOfPlans; j++)
		{
            std::string planPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(plans[j]);
			std::ostringstream sql;

			plan = plans[j];

			sql << "call delete_plan(" << plan << ")";
			std::string strSQL = sql.str();

            // TD11505: Stored procedure DELETE_PLAN() will raise an application error if the plan
            // cannot be deleted for whatever reason. This will be thrown from executeProcedure() 
            // as a DatabaseException.
            try
            {
                dataBase->executeProcedure(sql.str());
				TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(dataBase); //  CL-21716
		        TA_Base_Core::DescriptionParameters descriptionParameters;
				//xufeng++ TD14927
				TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
				descriptionParameters.push_back ( &pair0 );
				//++xufeng TD14927
                TA_Base_Core::NameValuePair pair1( "PlanName", planPath);
		        descriptionParameters.push_back ( &pair1 );
		        AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanDelete, descriptionParameters);
            }
            catch (const TA_Base_Core::DatabaseException& ex)
            {
                // Check for error -20022.
                const std::string DELETE_PLAN_ERROR("-20022");
                std::string exceptionError(ex.what());

                if (exceptionError.find(DELETE_PLAN_ERROR) == std::string::npos)
                {
					TA_THROW(ex);
                }

				undeletedPlans++;
				VectorUndeletedPlans.push_back(j);

		        TA_Base_Core::DescriptionParameters descriptionParameters;
				//xufeng++ TD14927
				TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "UNSUCCESSFUL");
				descriptionParameters.push_back ( &pair0 );
				//++xufeng TD14927		        
                TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(plan ));
		        descriptionParameters.push_back ( &pair1 );

			   AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanDeleteFailure, descriptionParameters);
				
				//Log alarm
				std::ostringstream alarmMessage;
				alarmMessage << "PlanDeleteFailure:"
							 << TA_Base_Bus::PlanDataReader::instance()->getNodeName(plans[j]).c_str()
							 << "[Id:"
							 << plans[j]
							 << "] could not be deleted.";
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, alarmMessage.str().c_str());
			}
		}

		TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
		pcud->session = CORBA::string_dup(session.c_str());

		if ( undeletedPlans )
		{
			TA_Base_Core::DeleteNodeUpdate_var newDnu = new TA_Base_Core::DeleteNodeUpdate;
			dnu->parent = parent;
			int actualDeletedPlans = numberOfPlans-undeletedPlans;
			newDnu->nodes.length(actualDeletedPlans);
			int oldIndex = 0;
			int newIndex = 0;
			bool foundUndeletedPlan = false;

			std::vector<int>::iterator it;

			for ( it = VectorUndeletedPlans.begin(); it != VectorUndeletedPlans.end() ; it++)
			{
				int nUndeleted = *it;
				while(newIndex != nUndeleted)
				{
					newDnu->nodes[newIndex++] = dnu->nodes[oldIndex++];
				}

				oldIndex++;
			}

			if ( parent != TA_Base_Core::ROOT_NODE_ID )
				pcud->configUpdate.deletePlans(newDnu);
			else
				pcud->configUpdate.deletePlansFromRoot(newDnu->nodes);
		}
		else
		{
			if ( parent != TA_Base_Core::ROOT_NODE_ID )
				pcud->configUpdate.deletePlans(dnu);
			else
				pcud->configUpdate.deletePlansFromRoot(dnu->nodes);
		}

        if (undeletedPlans != numberOfPlans)
        {
		    //Finally send the update
		    ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( *pcud._retn() );
        }

		if (undeletedPlans )
		{
			std::string message = "Error: The Plans nodes could not be deleted: ";
			message += notDeletedPlans.str();
            TA_THROW(TA_Base_Core::PlanAgentException(message, TA_Base_Core::PAE_DATA_ERROR));
		}

	}

    void PlanDataWritingClass::updateCategory(const std::string& session, const TA_Base_Core::CategoryDetail& detail)
	{
		std::ostringstream message;

		TA_Base_Core::NodeId parent = detail.parent;

		TA_Base_Core::ENode nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(parent);

		if ( nodeType != TA_Base_Core::CATEGORY_NODE)
		{
			message << "Error: The parent[" << detail.parent << "] is not of type CATEGORY.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_PARENT_NOT_CATEGORY));
		}

		TA_Base_Core::NodeId category = detail.category;

		nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(category);

		if ( nodeType != TA_Base_Core::CATEGORY_NODE)
		{
			message << "Error: The category[" << category << "] is not of type CATEGORY.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_CATEGORY));
		}

		std::string name = parseForApostrophe(detail.name.in());
		std::string description = parseForApostrophe(detail.description.in());

		if ( !TA_Base_Bus::PlanDataReader::instance()->canUseName(category, parent, name) )
		{
			message << "Error: Category with name [" << detail.name.in() << "] already exists in the parent category."; 
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NAME_NOT_UNIQUE));
		}

        std::string oldPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(category);
		std::ostringstream sql;

		sql << "update node_tree set name = '"
			<< name.c_str() << "',"
			<< "description = '"
			<< description.c_str() << "' "
			<< "where p_key = " << category;

		TA_Base_Core::IDatabase* dataBase = getDatabase();

		dataBase->executeModification(sql.str());
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(dataBase); //  CL-21716
		//We have successfully updated the category.
		//Send Update for insertCategory
		TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
		pcud->session = CORBA::string_dup(session.c_str());

		TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(category);
		pcud->configUpdate.updateCategory(ns);
		ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( *pcud._retn() );

		TA_Base_Core::DescriptionParameters descriptionParameters;
		//xufeng++ TD14927
		TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
		descriptionParameters.push_back ( &pair0 );
		//++xufeng TD14927
        TA_Base_Core::NameValuePair pair1( "CategoryName", oldPath);
		descriptionParameters.push_back ( &pair1 );
        TA_Base_Core::NameValuePair pair2( "NewCategoryName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(category));
		descriptionParameters.push_back ( &pair2 );
		AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanCategoryUpdate, descriptionParameters);


		std::ostringstream auditMessage;
		auditMessage << "Category details of "
					 << TA_Base_Bus::PlanDataReader::instance()->getNodeName(category).c_str()
					 << "[Id:"
					 << category
					 << "] have been updated."; 
        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, auditMessage.str().c_str());
	}



    void PlanDataWritingClass::insertCategory(const std::string& session, TA_Base_Core::NodeId parent)
	{
		std::ostringstream message;
		TA_Base_Core::ENode nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(parent);

		if ( nodeType != TA_Base_Core::CATEGORY_NODE)
		{
			message << "Error: The parent [" << parent << "] is not of type CATEGORY.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_PARENT_NOT_CATEGORY));
		}

	  if ( parent != TA_Base_Core::ROOT_NODE_ID)
		{
			//TD15753++
			if ( TA_Base_Bus::PlanDataReader::instance()->getCategoryLevel(parent) > 2)
			//++TD15753
			{
				message << "Error: Category can not be created at a level deeper than 3.";
							   TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_EXCEED_MAX_CATAGORY_LEVEL));
			}
		}



		
		std::string name = getNewNodeName(parent, TA_Base_Core::CATEGORY_NODE);
		std::string description = name;
		description +=  " Description";
		std::ostringstream sql;

		sql << "insert into node_tree "
			<< " (p_key, fk_node_type, parent_node, name, description, fk_approval_state, node_created_by) "
			<< " values "
			<< " ( plans_seq.nextval, (select node_type.p_key from node_type where node_type.TYPE_VALUE = "
			<< TA_Base_Core::CATEGORY_NODE
			<< ") , " << parent << " ,'" 
			<< name.c_str()
			<< "','" << description.c_str() << "',"
			<< " (select approval_state.p_key from approval_state where approval_state.state_value = "
			<< TA_Base_Core::UNDEFINED_APPROVAL_STATE << "),'"
            << getOperatorName(session).c_str() 
            << "')";

		TA_Base_Core::IDatabase* dataBase = getDatabase();

		dataBase->executeModification(sql.str());

		//We have successfully inserted the new category, let us retrieve its NodeId
		sql.str("");
		sql << "select p_key from node_tree where parent_node = "
			<< parent << " and name = '"
			<< name.c_str() << "'";

        std::vector<std::string> columns;
		columns.push_back( "p_key" );

		std::auto_ptr<TA_Base_Core::IData> data( dataBase->executeQuery( sql.str(), columns ) );
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(dataBase); //  CL-21716
		if( (NULL == data.get()) || (1 != data->getNumRows()) ) 
        {
			message << "Error: Failed to insert category.";
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.str().c_str());
			TA_THROW( TA_Base_Core::DataException( "Failed to insertCategory", TA_Base_Core::DataException::NO_VALUE, columns[0] ) );
        }

		TA_Base_Core::NodeId category = data->getUnsignedLongData( 0, columns[0] );

		//Send Update for insertCategory
		TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
		pcud->session = CORBA::string_dup(session.c_str());

		TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(category);

		if ( parent != TA_Base_Core::ROOT_NODE_ID )
			pcud->configUpdate.insertCategory(ns);
		else
			pcud->configUpdate.insertCategoryIntoRoot(ns);

		ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( *pcud._retn() );

		TA_Base_Core::DescriptionParameters descriptionParameters;
		//xufeng++ TD14927
		TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
		descriptionParameters.push_back ( &pair0 );
		//++xufeng TD14927
		TA_Base_Core::NameValuePair pair1( "CategoryName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(category));
		descriptionParameters.push_back ( &pair1 );
		TA_Base_Core::NameValuePair pair2( "ParentCategory", TA_Base_Bus::PlanDataReader::instance()->getNodeName(parent).c_str());
		descriptionParameters.push_back ( &pair2 );
		
		AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanCategoryInsert, descriptionParameters);

		std::ostringstream auditMessage;

		auditMessage << "Category "
					 << TA_Base_Bus::PlanDataReader::instance()->getNodeName(category).c_str()
					 << "[Id:"
					 << category
					 << "] has been inserted into Category[" 
					 << TA_Base_Bus::PlanDataReader::instance()->getNodeName(parent).c_str()
					 << "].";
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, auditMessage.str().c_str());
	}

	void PlanDataWritingClass::deleteCategory(const std::string& session, TA_Base_Core::NodeId category)
	{
        std::string categoryPath = TA_Base_Bus::PlanDataReader::instance()->getNodePath(category);
		std::ostringstream message;

		TA_Base_Core::ENode nodeType = TA_Base_Bus::PlanDataReader::instance()->getNodeType(category);

		if ( nodeType != TA_Base_Core::CATEGORY_NODE)
		{
			message << "Error: The node[" << category << "] is not of type CATEGORY.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_CATEGORY));
		}

		TA_Base_Core::NodeSummary * categorySummary = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(category);

		if ( categorySummary->hasChildren )
		{
            TA_THROW(TA_Base_Core::PlanAgentException("Error: Non empty category cannot be deleted.", TA_Base_Core::PAE_CANNOT_DELETE_NON_EMPTY_CATAGORY));
		}

		TA_Base_Core::NodeId parent = categorySummary->parent;

		std::string parentName = "ROOT";

		if ( parent != TA_Base_Core::ROOT_NODE_ID )
		{
			TA_Base_Core::NodeSummary* parentSummary = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(parent);
			parentName = parentSummary->name.in();
		}

		std::ostringstream sql;
		sql << "delete from node_tree where p_key =" << category;

		TA_Base_Core::IDatabase* dataBase = getDatabase();

		std::string strSQL = sql.str();

		dataBase->executeModification(sql.str());
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(dataBase); //  CL-21716
		//Send Update
		TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
		pcud->session = CORBA::string_dup(session.c_str());

		if ( parent != TA_Base_Core::ROOT_NODE_ID )
		{
			TA_Base_Core::DeleteNodeUpdate_var dnu = new TA_Base_Core::DeleteNodeUpdate;
			dnu->parent = parent;
			dnu->nodes.length(1);
			dnu->nodes[0] = category;
			pcud->configUpdate.deleteCategory(dnu);
		}
		else
			pcud->configUpdate.deleteCategoryFromRoot(category);

		ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( *pcud._retn() );			

		TA_Base_Core::DescriptionParameters descriptionParameters;
		//xufeng++ TD14927
		TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
		descriptionParameters.push_back ( &pair0 );
		//++xufeng TD14927
        TA_Base_Core::NameValuePair pair1( "CategoryName", categoryPath);
		descriptionParameters.push_back ( &pair1 );
		AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanCategoryDelete, descriptionParameters);

		std::ostringstream auditMessage;
		auditMessage << categorySummary->name.in()
					 << "[Id:"
					 << category
					 << "] in Category" << parentName
					 << "["
					 << parent
					 << "] has been deleted.";
	    LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, auditMessage.str().c_str());
	}


    void PlanDataWritingClass::setPlanApprovalState(const std::string& session, TA_Base_Core::NodeId nodeId, TA_Base_Core::EApprovalState approvalState)
    {
        //Check that the Plan is not active now
        ActivePlanList* planList  = ActivePlanBulletinBoard::instance()-> getActiveInstances( nodeId );

        std::ostringstream what;

        if ( planList->size() != 0 )
        {
            what << "Plan: " << TA_Base_Bus::PlanDataReader::instance()->getNodeName(nodeId).c_str() <<" has at least one instance active";

			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, what.str().c_str());
            TA_THROW(TA_Base_Core::PlanAgentException(what.str(), TA_Base_Core::PAE_PLAN_HAS_ACTIVE_INSTANCE));
        }

        if ( isPlanBeingEdited(nodeId) )
        {
			std::string editLockSession = getEditLockOwner(nodeId);
            what << "The Plan cannot be approved while being edited by " << ActivePlanBulletinBoard::instance()->getOwner(editLockSession) << ".";
            TA_THROW(TA_Base_Core::PlanAgentException(what.str(), TA_Base_Core::PAE_EDIT_LOCKED_BY_OPERATOR, ActivePlanBulletinBoard::instance()->getOwnerId(editLockSession)));
        }


        TA_Base_Core::IDatabase* dataBase = getDatabase();

        unsigned long planPKey = nodeId;

        std::ostringstream sql;

//        sql << "update node_tree set fk_approval_state = ";
		sql << "update " << NODE_TREE_TABLE.c_str() << " set fk_approval_state = ";

        switch (approvalState)
        {
            case TA_Base_Core::APPROVED_APPROVAL_STATE:
            sql << "1 ";
            break;

            case TA_Base_Core::UNAPPROVED_APPROVAL_STATE:
            sql << "2 ";
            break;

            case TA_Base_Core::UNDEFINED_APPROVAL_STATE:
            sql << "3 ";
            break;

        default:
            sql << "3 ";
            break;
        }

        sql << ", approval_modified_by = '"
            << getOperatorName(session).c_str()
            << "', "
            << "approval_date_modified = sysdate "
            << " where p_key = " << nodeId;

        try
        {
		    dataBase->executeModification(sql.str());
			TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(dataBase); //  CL-21716
        }
		catch ( ... )
		{
			std::ostringstream auditMessage;
		    TA_Base_Core::DescriptionParameters descriptionParameters;
			//xufeng++ TD14927
			TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "UNSUCCESSFUL");
			descriptionParameters.push_back ( &pair0 );
			//++xufeng TD14927
            TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(nodeId ));
		    descriptionParameters.push_back ( &pair1 );

			if ( approvalState == TA_Base_Core::APPROVED_APPROVAL_STATE)
			{		       
			   AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanApprovalFailure, descriptionParameters);
				
				auditMessage << "PlanApprovalFailure:"
							 << TA_Base_Bus::PlanDataReader::instance()->getNodeName(nodeId).c_str()
							 << "[Id:"
							 << nodeId
							 << "] could not be approved.";

			    LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, auditMessage.str().c_str());
			}
			else if ( approvalState == TA_Base_Core::UNAPPROVED_APPROVAL_STATE)
			{
				auditMessage << "PlanUnApprovalFailure:"
							 << TA_Base_Bus::PlanDataReader::instance()->getNodeName(nodeId).c_str()
							 << "[Id:"
							 << nodeId
							 << "] could not be unapproved.";
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, auditMessage.str().c_str());
				std::string message = "Failed to unapprove a plan.";

				//Error using 'EXCEPTION_CATCH_LOG_TYPE'. cuilike++

				//LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, ERROR_LOG_LEVEL, message.c_str() );
				LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown Exception", message.c_str() );
				//++cuilike
				AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanUnApprovalFailure, descriptionParameters);
				TA_THROW(  TA_Base_Core::PlanAgentException (message.c_str() ,TA_Base_Core::PAE_PLAN_UNAPPROVE_ERROR) );
			}
            return;
		}

		TA_Base_Core::PlanConfigUpdateData_var pcud = new TA_Base_Core::PlanConfigUpdateData;
		pcud->session = CORBA::string_dup(session.c_str());

		TA_Base_Core::ApprovalStateUpdate_var asu = new TA_Base_Core::ApprovalStateUpdate;
		asu->plan = nodeId;
		asu->approvalState = approvalState;

		pcud->configUpdate.approvalState(asu);

		ActivePlanBulletinBoard::instance()->sendConfigUpdateMessage( *pcud._retn() );
        //Generate auditMessage for Approval/Unapproval


		TA_Base_Core::DescriptionParameters descriptionParameters;
		//xufeng++ TD14927
		TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
		descriptionParameters.push_back ( &pair0 );
		//++xufeng TD14927
        TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(nodeId));
		descriptionParameters.push_back ( &pair1 );


        std::ostringstream auditMessage;
        auditMessage << "has ";

		if ( approvalState == TA_Base_Core::APPROVED_APPROVAL_STATE)
		{
		    AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanApproval, descriptionParameters);
			auditMessage << "approved ";
		}
		else if ( approvalState == TA_Base_Core::UNAPPROVED_APPROVAL_STATE)
		{
		    AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanUnapproval, descriptionParameters);
			auditMessage << "unapproved ";
		}

        auditMessage << "the plan: "
                     << TA_Base_Bus::PlanDataReader::instance()->getNodeName(nodeId).c_str()
					 << "[Id:"
					 << nodeId
					 << "]";
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, auditMessage.str().c_str());
    }


	std::string PlanDataWritingClass::getNodeNameToCopy(TA_Base_Core::NodeId node, TA_Base_Core::NodeId parent)
	{
		TA_Base_Core::IDatabase* dataBase = getDatabase();

		TA_Base_Core::NodeSummary_var ns = TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(node);
		std::string originalName = ns->name.in();
		std::string copyNameSuffix = "of ";
		copyNameSuffix += originalName;

		std::string copyNamePrefix = "Copy ";
		std::string newName = copyNamePrefix + copyNameSuffix;

		std::ostringstream sql;
		sql << "select name from node_tree where parent_node = "
			<< parent
			<< " and ( name = '" << parseForApostrophe(originalName) << "' "
			<< " or name like '" << parseForApostrophe(copyNamePrefix.c_str()) << "%')";

		std::vector <std::string> columns;

		columns.push_back("name");
		std::auto_ptr<TA_Base_Core::IData> data( dataBase->executeQuery( sql.str(), columns ) );
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(dataBase); //  CL-21716

		if( (NULL == data.get()) || (0 == data->getNumRows()) ) 
        {
			return originalName;
        }

		int nRows =  data->getNumRows();
		std::string name;
		std::ostringstream newNameStrm;
		unsigned long dataIdx = 0;
		newNameStrm << newName.c_str();

		bool foundMatch = false;
		bool foundNewName = false;

		unsigned long Index = 2;

		while (!foundNewName)
		{
			for( dataIdx = 0; dataIdx < nRows; dataIdx++ )
			{
				name = data->getStringData( dataIdx, columns[0] );
				int result = name.compare(newName);

				if (result == 0)
				{
					foundMatch = true;
					break;
				}
			}

			if (foundMatch)
			{
				foundMatch = false;
				newNameStrm.str("");
				newNameStrm << copyNamePrefix.c_str()
							<< "(" << Index++ << ") "
							<< copyNameSuffix.c_str();				
			}
			else
			{
				foundNewName = true;
			}

			newName = newNameStrm.str();
		}
		return newName;
	}

	std::string PlanDataWritingClass::getNewNodeName(TA_Base_Core::NodeId parent, TA_Base_Core::ENode nodeType)
	{
		TA_Base_Core::IDatabase* dataBase = getDatabase();

		std::string nodeName = "";

		if (nodeType == TA_Base_Core::PLAN_NODE)
			nodeName = "New Plan";
		else if (nodeType == TA_Base_Core::CATEGORY_NODE)
			nodeName = "New Category";

		std::ostringstream sql;
		sql << "select name from node_tree where parent_node = "
			<< parent
			<< " and "
			<< " fk_node_type = (select p_key from node_type where type_value = "
			<< nodeType
			<< ") "
			<< "and name like '" << nodeName.c_str() << "%'";

		std::vector <std::string> columns;

		columns.push_back("name");

		std::auto_ptr<TA_Base_Core::IData> data( dataBase->executeQuery( sql.str(), columns ) );
		TA_Base_Bus::PlanDataBaseHelper dataBaseHelper(dataBase); //  CL-21716
		if( (NULL == data.get()) || (0 == data->getNumRows()) ) 
        {
			return nodeName;
        }

		int nRows =  data->getNumRows();

		std::string name;
		std::ostringstream newNameStrm;
		std::string newName;

		int iPos = -1;
		unsigned long dataIdx = 0;
		newNameStrm << nodeName.c_str();
		newName = nodeName;

		bool foundMatch = false;
		bool foundNewName = false;
		unsigned long Index = 2;

		while (!foundNewName)
		{
			for( dataIdx = 0; dataIdx < nRows; dataIdx++ )
			{
				name = data->getStringData( dataIdx, columns[0] );
				int result = name.compare(newName);

				if (result == 0)
				{
					foundMatch = true;
					break;
				}
			}

			if (foundMatch)
			{
				foundMatch = false;
				newNameStrm.str("");
				newNameStrm << nodeName.c_str();
				newNameStrm << " (" << Index++ << ")";				
			}
			else
			{
				foundNewName = true;
			}
			newName = newNameStrm.str();
		}
		return newName;
	}


	TA_Base_Core::IDatabase* PlanDataWritingClass::getDatabase() const
	{   
		TA_Base_Core::IDatabase* database = NULL;
		try
		{
            database = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Write);
		}
		catch ( ... )
		{
			std::string message = "Failed to connect to the database.";
			//Error using 'EXCEPTION_CATCH_LOG_TYPE'. cuilike++

			//LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, ERROR_LOG_LEVEL, message.c_str() );
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown Exception", message.c_str() );
			//++cuilike
			TA_THROW(  TA_Base_Core::PlanAgentException (message.c_str() ,TA_Base_Core::PAE_DATABASE_ERROR) );
		}

		return database;
	}


	std::string PlanDataWritingClass::parseForApostrophe(const std::string& inString)
	{
		std::string retValue="";
		int iPos = 0;
		int nextPos = 0;
		std::string temp="";

		while ( (nextPos = inString.find_first_of("'",iPos)) != std::string::npos ) 
		{
			temp = inString.substr(iPos, nextPos-iPos);
			retValue += temp;
			retValue += "''";
			iPos = nextPos+1;
		}

		if (retValue.empty())
			retValue = inString;
		else
			retValue += inString.substr(iPos, inString.length());

		return retValue;
	}


    std::string PlanDataWritingClass::getOperatorName(const std::string& session)
    {
        try
        {
            // The format is "<operator name> (<base profile>[ / <override profile>])"
            TA_Base_Bus::SessionInfo sessionInfo(m_authenticationLib.getSessionInfo(session, session));
		    std::auto_ptr<TA_Base_Core::IOperator> iOperator(
                TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));

            std::string profile("");
            
            for (std::vector<unsigned long>::const_iterator pIt(sessionInfo.ProfileId.begin());
                 pIt != sessionInfo.ProfileId.end(); pIt++)
            {
		        std::auto_ptr<TA_Base_Core::IProfile> iProfile(
                    TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(*pIt, false));
                
                if (!profile.empty())
                {
                    profile += " / ";
                }

                profile += iProfile->getName();
            }

            TA_ASSERT(!profile.empty(), "Session has no valid profile associated with it");

            std::ostringstream opNameAndProfile;
            opNameAndProfile << iOperator->getName() << " (" << profile << ")";

		    return opNameAndProfile.str();
        }
        catch(const TA_Base_Core::AuthenticationSecurityException& ex)
        {
            std::string message = "PlanDataWritingClass::getOperatorName. Caught TA_Base_Core::AuthenticationSecurityException: ";
            message += ex.what();
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.c_str());
        }
        catch(const TA_Base_Core::AuthenticationAgentException& ex)
        {
            std::string message = "PlanDataWritingClass::getOperatorName. Caught TA_Base_Core::AuthenticationAgentException: ";
            message += ex.what();
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.c_str());
        }
        catch(const TA_Base_Core::ObjectResolutionException& ex)
        {
            std::string message = "PlanDataWritingClass::getOperatorName. Caught TA_Base_Core::ObjectResolutionException: ";
            message += ex.what();
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.c_str());
        }
		catch (const TA_Base_Core::DataException& ex )
		{
            std::string message = "PlanDataWritingClass::getOperatorName. Caught TA_Base_Core::DataException: ";
            message += ex.what();
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.c_str());
		}
        catch ( ... )
        {
            std::string message = "PlanDataWritingClass::getOperatorName. Caught unknown exception.";
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.c_str());
        }

	    std::string defaultName = "session: " + session;
        return defaultName;
    }


    std::string PlanDataWritingClass::makeNumericDataPointValueString(const TA_Base_Core::NumericDataPoint& ndp)
    {
        std::stringstream strmValue;
        strmValue << ndp.value;
        std::string strValue = strmValue.str();
        unsigned long precision = 0;

		std::string::size_type beginPos = 0;
		beginPos = strValue.find_first_of(".", beginPos);

		if (beginPos == std::string::npos)
            precision = strValue.size();
        else
            precision = beginPos;

	    std::ostringstream ostrstrm;
		ostrstrm << TA_Base_Core::NUMERIC_DATA_POINT << ",";
		ostrstrm << std::setprecision(ndp.precision + precision) << ndp.value << ",";
		ostrstrm << ndp.precision;

        return ostrstrm.str();
    }


    TA_Base_Core::NodeId PlanDataWritingClass::getPlanToReplace(const std::string& planName, TA_Base_Core::NodeId parent, bool useForce)
    {
		// get the details for the duplicate plan
		TA_Base_Core::NodeId duplicatePlan = TA_Base_Bus::PlanDataReader::instance()->getNodeId( parent, parseForApostrophe(planName) );
	
		TA_Base_Core::NodeSummary_var dupSummary =
			TA_Base_Bus::PlanDataReader::instance()->getNodeSummary(duplicatePlan);

        // check that the existing plan is in fact a plan
        if (TA_Base_Core::CATEGORY_NODE == dupSummary->type)
        {
            std::ostringstream message;
			message << "Error: A subcategory with name ["
					<< planName
					<< "] already exists in the destination category.";
			TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_CANNOT_OVERWRITE_CATEGORY));
        }

		// check that the existing plan is not approved
		if (TA_Base_Core::APPROVED_APPROVAL_STATE == dupSummary->approvalState)
		{
            std::ostringstream message;
			message << "Error: An approved plan with name ["
					<< planName
					<< "] already exists in the destination category.";
			TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_CANNOT_OVERWRITE_APPROVED_PLAN));
		}

		// check that the existing plan is not being edited
		if (isPlanBeingEdited(duplicatePlan))
		{
            std::ostringstream message;
			std::string editLockSession = getEditLockOwner(duplicatePlan);
			message << "Error: The plan with name [" 
                    << planName
					<< "already exists in the destination category and it is being edited by "
					<<	ActivePlanBulletinBoard::instance()->getOwner(editLockSession);
			TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_CANNOT_OVERWRITE_EDITED_PLAN, ActivePlanBulletinBoard::instance()->getOwnerId(editLockSession)));
		}

		if (!useForce)
		{
            std::ostringstream message;
			message << "Error: Plan with name ["
					<< planName
					<< "] already exists in the destination category."; 
			TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NAME_NOT_UNIQUE));
		}

        return duplicatePlan;
    }


    TA_Base_Core::StepNumber PlanDataWritingClass::getStepNumber(const TA_Base_Core::Plan& plan, TA_Base_Core::StepId step) const
    {
        unsigned long stepCount(plan.stepseq.length());

        for (unsigned long i(0); i < stepCount; i++)
		{
            const TA_Base_Core::StepDetail& stepDetail = plan.stepseq[i].detail;

			if (stepDetail.step == step)
			{
				return stepDetail.position;
			}
		}

        return 0;
    }
}
