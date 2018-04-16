/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveEndStep.cpp $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Implementation of .
  */

#include "PlanAgent.h"

// namespace alias for Corba structures/types
 
namespace TA_Base_App
{

	ActiveEndStep::ActiveEndStep( const TA_Base_Core::StepDetail& stepDetail, 
									  IActivePlanFlow& flowControl ) :
		ActiveStep(stepDetail, flowControl),
		m_flowControl(flowControl)
	{
	}

	TA_Base_Core::EActiveStepState ActiveEndStep::executeImpl( void )
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "%s: Completed.", dumpContext().c_str() );
		m_flowControl.setIsActive( false );

        if ( m_flowControl.anyPlanStepFailed() )
		    m_remark = "Plan completed with failed step(s)";
        else
		    m_remark = "Plan completed successfully";

		ActivePlanBulletinBoard::instance()->update( m_flowControl.getPlan(),
														 TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE, m_remark );


		std::string session = ActivePlanBulletinBoard::instance()->getSession(m_flowControl.getPlan());
		if (ActivePlanBulletinBoard::instance()->isGRCPlan (m_flowControl.getPlan()))
		{
			if (ActivePlanBulletinBoard::instance()->isManualGRC (m_flowControl.getPlan()))
			{
				//TD18369++ zhaodongyang
				TA_Base_Core::DescriptionParameters descriptionParameters;
				TA_Base_Core::NameValuePair pair0( "ManualGRCPlan", "Executed");
                descriptionParameters.push_back ( &pair0 );

		    	TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(m_flowControl.getPlan().plan ));
		       	descriptionParameters.push_back ( &pair1 );
			
			    std::ostringstream tempStrm;
			    tempStrm << m_flowControl.getPlan().instance;
			    TA_Base_Core::NameValuePair pair2( "InstanceNumber", tempStrm.str());
			    descriptionParameters.push_back ( &pair2 );
				//++TD18369 zhaodongyang


				//TD14802++
				//Add dummy pair to set Value column in Event Viewer to "SUCCESSFUL"
				TA_Base_Core::NameValuePair pair3( "DataPointValue", "SUCCESSFUL");
				descriptionParameters.push_back ( &pair3 );
				//++TD14802
				
				AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanManualGRCSuccessful, descriptionParameters);	
			}
			else//automatic GRC
			{
				//TD18369++ zhaodongyang
				TA_Base_Core::DescriptionParameters descriptionParameters;
				TA_Base_Core::NameValuePair pair0( "AutoGRCPlan", "Executed");
				descriptionParameters.push_back ( &pair0 );

				TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(m_flowControl.getPlan().plan ));
		       	descriptionParameters.push_back ( &pair1 );
			
			    std::ostringstream tempStrm;
			    tempStrm << m_flowControl.getPlan().instance;
			    TA_Base_Core::NameValuePair pair2( "InstanceNumber", tempStrm.str());
			    descriptionParameters.push_back ( &pair2 );
				//++TD18369 zhaodongyang

				//TD14802++
				//Add dummy pair to set Value column in Event Viewer to "SUCCESSFUL"
				TA_Base_Core::NameValuePair pair3( "DataPointValue", "SUCCESSFUL");
				descriptionParameters.push_back ( &pair3 );
				//++TD14802
				
				AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanAutoGRCSuccessful, descriptionParameters);	
			}
		}
		else//DSS plans
		{
			TA_Base_Core::DescriptionParameters descriptionParameters;
			
			//xufeng++ TD14927
			TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
			descriptionParameters.push_back ( &pair0 );
			//++xufeng TD14927

			TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(m_flowControl.getPlan().plan ));
			descriptionParameters.push_back ( &pair1 );
			
			std::ostringstream tempStrm;
			tempStrm << m_flowControl.getPlan().instance;
			TA_Base_Core::NameValuePair pair2( "InstanceNumber", tempStrm.str());
			descriptionParameters.push_back ( &pair2 );
			
			
			
			AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanComplete, descriptionParameters);
		}



		std::ostringstream auditMessage;
		TA_Base_Core::NodeId parent = TA_Base_Bus::PlanDataReader::instance()->getParent(m_flowControl.getPlan().plan);

		auditMessage << TA_Base_Bus::PlanDataReader::instance()->getNodeName(m_flowControl.getPlan().plan).c_str()
					 << "[Id:"
					 << m_flowControl.getPlan().plan
					 << ",Instance:"
					 << m_flowControl.getPlan().instance
					 << "] in Category[" 
					 << TA_Base_Bus::PlanDataReader::instance()->getNodeName(parent).c_str()
					 << "]. has completed.";

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, auditMessage.str().c_str());

		//m_flowControl.setNextStep();
			return TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;
	}

} // TA_Base_App
