/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/AAMessageSender.cpp $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2008/12/26 18:32:31 $
  *  Last modified by:  $Author: builder $
  *
  *  A thread-safe Singleton factory for updating database and adding into database.
  *  
  */

#include "PlanAgent.h"

namespace TA_Base_App
{
    AAMessageSenderImpl::AAMessageSenderImpl()
		//TD18093,jianghp:m_alarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper())
    {
		m_auditMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender( TA_Base_Core::PlanAgentAudit::Context );
		
    }

	AAMessageSenderImpl::~AAMessageSenderImpl()
	{
		deleteMessageSenders ();
	}

    void AAMessageSenderImpl::deleteMessageSenders(void)
    {
		if (m_auditMessageSender != NULL)
		{
			delete m_auditMessageSender;
			m_auditMessageSender = NULL;
		}
		
		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
		
    }

    void AAMessageSenderImpl::sendAuditMessage(const std::string& session, const TA_Base_Core::MessageType& auditMessageType, const TA_Base_Core::DescriptionParameters& description)
    {
        if ( !ActivePlanBulletinBoard::instance()->isControlMode() ) return;

		/**
		* sendAuditMessage
		*
		* Used to send an AuditMessage
		*
		* @param    messageType    	The specific type of message
		* @param    entityKey    	The key of an entity that is associated with this Message
		* @param    description    	The strings that will fill in the parameterised description string that is stored in the database. There must be one string per parameter.
		* @param    details    		Any additional information associated with the Message.
		* @param    sessionID  	  	The sessionID of the operator whose action initiated this Audit Message
		* @param    alarmID    		The UUID of an alarm whose instance required that this audit message be generated.
		* @param    incidentID    	The UUID of an incident that this event should be associated with
		* @param    parentEventID   If this event needs to be associated with another audit event (ie plan steps), to display a related event tree in the viewer, put the key here
		* @param    filterableData  Any additional filterable information that you wish to include in the event.
		
		void sendAuditMessage( const MessageType& messageType,
							   unsigned long entityKey,
							   const DescriptionParameters& description,
							   const std::string& details,
							   const std::string& sessionID,
							   const std::string& alarmID,
							   const std::string& incidentID,
							   const std::string& parentEventID,
							   const FilterData* filterableData = NULL );
*/

		m_auditMessageSender->sendAuditMessage	( 
								auditMessageType,					// message type
								PlanAgentUser::getInstance()->getAgentEntityKey(),	// entity key
								description,			    // description
								"",							        // details
								session,							// session ID
								"",							        // alarm ID
								"",								    // incidentID
								""									// parentEventID
												);
        

    }

	std::string AAMessageSenderImpl::raiseAlarm(const TA_Base_Core::MessageType& alarmType, const TA_Base_Core::DescriptionParameters& description)
    {
		/**
		* submitAlarmWithAsset
		*
		* Use this to submit an alarm that is unique in the system
		*
		* @param    alarmType    The type of alarm as defined in the specific Message Type header file
		* @param    entityKey    The entity that has raised the alarm
		* @param    entityTypeKey    The entityType of the entity that has raised the alarm
		* @param    description    The parameters to be inserted into the description as defined in the AlarmType table
		* @param    entityName    The name of the entity
		* @param    regionKey    The region the entity belongs to - 0 if don't care
		* @param    subsystemKey    The subsystem the entity belongs to - 0 if don't care
		void submitAlarmWithAsset( const TA_Base_Core::MessageType& alarmType,
						  unsigned long entityKey,
						  unsigned long entityTypeKey,
						  const TA_Base_Core::DescriptionParameters& description,
						  const std::string& entityName, 
						  unsigned long regionKey, 
						  unsigned long subsystemKey );
		*/
        if ( !ActivePlanBulletinBoard::instance()->isControlMode() ) return "";

        return TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarmWithAsset(
                                             alarmType,								//alarm message type
                                             PlanAgentUser::getInstance()->getAgentEntityKey(),		//
                                             PlanAgentUser::getInstance()->getAgentEntityTypeKey(),	//
                                             description,					//
                                             PlanAgentUser::getInstance()->getAgentEntityName(),	//
                                             PlanAgentUser::getInstance()->getAgentLocationKey(),		//regionkey does not exists, check!
                                             PlanAgentUser::getInstance()->getAgentSubsystemKey(),	//
											 PlanAgentUser::getInstance()->getAgentAssetName(),
											 TA_Base_Core::AlarmConstants::defaultTime
                                         );

    }


    void AAMessageSenderImpl::closeAlarm(const std::string& alarmId)
    {
        TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().closeAlarmAtLocation(alarmId, PlanAgentUser::getInstance()->getAgentLocationKey());
    }


	std::string AAMessageSenderImpl::getOperatorNameNid(const std::string& session)
	{   
        std::string retValue = "OperatorWithSession:";
        retValue += session;
        try
        {
            TA_Base_Bus::SessionInfo sessionInfo = m_authenticationLib.getSessionInfo(session, session);
		    std::auto_ptr<TA_Base_Core::IOperator> iOperator(
                TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
		    std::ostringstream detail;
		    detail << iOperator->getName() << "[Id:" << iOperator->getKey() <<"] ";
		    return detail.str();
        }
        catch(const TA_Base_Core::AuthenticationSecurityException& ex)
        {
            std::string message = "AAMessageSenderImpl::getOperatorNameNid. Caught TA_Base_Core::AuthenticationSecurityException: ";
            message += ex.what();
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.c_str());
            return retValue;
        }
        catch(const TA_Base_Core::AuthenticationAgentException& ex)
        {
            std::string message = "AAMessageSenderImpl::getOperatorNameNid. Caught TA_Base_Core::AuthenticationAgentException: ";
            message += ex.what();
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.c_str());
            return retValue;
        }
        catch(const TA_Base_Core::ObjectResolutionException& ex)
        {
            std::string message = "AAMessageSenderImpl::getOperatorNameNid. Caught TA_Base_Core::ObjectResolutionException: ";
            message += ex.what();

            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.c_str());
            return retValue;
        }
		catch (const TA_Base_Core::DataException& de )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "DataException", de.what() );
			return retValue;
		}
        catch ( ... )
        {
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Unknown exception caught in getOperatorNameNid() in AAMessageSender.cpp" );
			return retValue;

        }
	}
}


