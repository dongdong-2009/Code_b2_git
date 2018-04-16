#if !defined(DATA_POINT_UPDATE_SUBSCRIBER_CPP)
#define DATA_POINT_UPDATE_SUBSCRIBER_CPP

/**
  * DataPointUpdateStateSubscriber.h
  *
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/DataPointUpdateStateSubscriber.cpp $
  * @author:  ismard
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/01/08 11:35:48 $
  * Last modified by:  $Author: builder $
  *
  * Plan Agent's subscriber of Datapoint updates.
  * Currently it is only to listen to Comms messages resulting from 
  * writes to Data points as implemented in ActiveDataPointSetStep.cpp 
  */

#include "PlanAgent.h"

#include "DataPointUpdateStateSubscriber.h"

#include "core/message/types/DataPointStateUpdate_MessageTypes.h"
#include "core/message/src/MessageSubscriptionManager.h"

#include "core/utilities/src/DebugUtil.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"

#include "bus/scada/common_library/src/CommonDefs.h"

TA_Base_App::DataPointUpdateStateSubscriber* TA_Base_App::DataPointUpdateStateSubscriber::s_instance = NULL;
TA_Base_Core::NonReEntrantThreadLockable TA_Base_App::DataPointUpdateStateSubscriber::s_singletonLock;

namespace TA_Base_App
{

    //
    // DataPointUpdateStateSubscriber
    //
    DataPointUpdateStateSubscriber::DataPointUpdateStateSubscriber()
    {
        m_proxyMap.clear();
    }

	 DataPointUpdateStateSubscriber::~DataPointUpdateStateSubscriber() 
	 {
		m_activeBRC.clear ();
	 }

	 DataPointUpdateStateSubscriber &DataPointUpdateStateSubscriber::getInstance()
	 {
		 if (!s_instance)
		 {
			 TA_Base_Core::ThreadGuard guard(s_singletonLock);
			 
			 if (!s_instance)
			 {
				 s_instance = new DataPointUpdateStateSubscriber();
			 }
		 }
		 
		 return *s_instance;
	 }
	 
	 void DataPointUpdateStateSubscriber::destroyInstance()
	 {
		 TA_Base_Core::ThreadGuard guard(s_singletonLock);
		 
		 delete s_instance;

	 }


    //
    // processEntityUpdateEvent
    //
    //void DataPointUpdateStateSubscriber::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
     void DataPointUpdateStateSubscriber::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
    {
		 FUNCTION_ENTRY("processEntityUpdateEvent");
		 
         // only interested on Control update
         if (updateType != TA_Base_Bus::ControlUpdate)
         {
             return;
         }

         std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*>::iterator it;
         it = m_proxyMap.find(entityKey);

         TA_ASSERT(it != m_proxyMap.end(), "DataPoint Proxy was not created yet");

         TA_Base_Bus::DataPointProxySmartPtr *dpProxy = m_proxyMap[entityKey];
         
		 try
		 {
             int i = 0;
             for (i = 0; i < m_activeBRC.size(); i++)
             {
                 if (m_activeBRC[i].entityKey  == entityKey)
                 {
                     break;
                 }
             }
             
             if (i == m_activeBRC.size())
             {//not from this subscriber
                 LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                     "DataPointInternalStateUpdate received NOT from this subscriber (agent). Message not processed !");
                 return;
             }
             
             std::string  equipmentDescription = (*dpProxy)->getDescription();
             TA_Base_Core::DescriptionParameters descriptionParameters;
             descriptionParameters.clear ();
             
			 TA_Base_Bus::EDataPointControlState controlState = (*dpProxy)->getControlState();
			 if (controlState == TA_Base_Bus::success ||
				 controlState == TA_Base_Bus::written)
			 {//send control successful audit message

				 if (ActivePlanBulletinBoard::instance()->isManualGRC (m_activeBRC [i].planID))
				 {//for manual GRC we're interested in the BRC messages  
				  //successful GRC messages are handeled elsewhere 	
				 TA_Base_Core::NameValuePair pair ("EquipmentDescription", equipmentDescription);
				 descriptionParameters.push_back (&pair);

				 //TD14802++
				//Add dummy pair to set Value column in Event Viewer to "SUCCESSFUL"
				TA_Base_Core::NameValuePair pair1( "DataPointValue", "SUCCESSFUL");
				descriptionParameters.push_back ( &pair1 );
				//++TD14802
				 
				 AAMessageSender::instance()->sendAuditMessage(m_activeBRC [i].sessionID, TA_Base_Core::PlanAgentAudit::PlanBRCStepSuccessful, descriptionParameters);
				 
				 //remove processed entity so we don't have to wait for another message
				 //sent by the same datapoints (they can send more then one message for a single datapoint set operation)
				 m_activeBRC.erase (m_activeBRC.begin () + i);
				 }
			 }
			 else if (controlState == TA_Base_Bus::LCC_failed) //TD14802
			 {//send control unsuccessful due to Launching condition failed
				 //for LCC_failed we're interested in GRC as well as manual BRC messages
				 TA_Base_Core::NameValuePair pair1 ("EquipmentDescription", equipmentDescription);
				 descriptionParameters.push_back (&pair1);
				 TA_Base_Core::NameValuePair pair2 ("EquipmentDescription", equipmentDescription);
				 descriptionParameters.push_back (&pair2);
				 TA_Base_Core::NameValuePair pair3 ("LaunchingConditionString", (*dpProxy)->getLaunchingConditionResultString());
				 descriptionParameters.push_back (&pair3);

				 //TD14802++
				//Add dummy pair to set Value column in Event Viewer to "BAD LAUNCH"
				TA_Base_Core::NameValuePair pair4( "DataPointValue", "BAD LAUNCH");
				descriptionParameters.push_back ( &pair4 );
				//++TD14802
				 
				 if (ActivePlanBulletinBoard::instance()->isManualGRC (m_activeBRC [i].planID))
				 {
					//manual BRC
					AAMessageSender::instance()->sendAuditMessage(m_activeBRC [i].sessionID, TA_Base_Core::PlanAgentAudit::PlanBRCStepBadLaunch, descriptionParameters);
				 
				   //also one for manual GRC (with same parametres)
					AAMessageSender::instance()->sendAuditMessage(m_activeBRC [i].sessionID, TA_Base_Core::PlanAgentAudit::PlanManualGRCBadLaunch, descriptionParameters);
				 }
				 else//auto GRC
				 {
					AAMessageSender::instance()->sendAuditMessage(m_activeBRC [i].sessionID, TA_Base_Core::PlanAgentAudit::PlanAutoGRCBadLaunch, descriptionParameters);
				 }

				 m_activeBRC.erase (m_activeBRC.begin () + i);
			 }
			 else if (controlState == TA_Base_Bus::RCC_failed) //TD14802
			 {//send control unsuccessful due to Launching condition failed
         //for RCC_failed we're interested in GRC as well as manual BRC messages
				 TA_Base_Core::NameValuePair pair1 ("EquipmentDescription", equipmentDescription);
				 descriptionParameters.push_back (&pair1);

				 if (ActivePlanBulletinBoard::instance()->isManualGRC (m_activeBRC [i].planID))
				 {
					 {
						//TD14802++
						//Add dummy pair to set Value column in Event Viewer to "BAD RETURN"
						TA_Base_Core::NameValuePair pair2( "DataPointValue", "BAD RETURN");
						descriptionParameters.push_back ( &pair2 );
						//++TD14802

						 //one message for BRC 
						 AAMessageSender::instance()->sendAuditMessage(m_activeBRC [i].sessionID, TA_Base_Core::PlanAgentAudit::PlanBRCStepBadReturn, descriptionParameters);
					 }
				 
					 //one for GRC
					 TA_Base_Core::NameValuePair pair2 ("EquipmentDescription", equipmentDescription);
					 descriptionParameters.push_back (&pair2);
                     //weimin: should this be ReturnConditionString?
					 TA_Base_Core::NameValuePair pair3 ("LaunchingConditionString", (*dpProxy)->getLaunchingConditionResultString());
					 descriptionParameters.push_back (&pair3);

					//TD14802++
					//Add dummy pair to set Value column in Event Viewer to "BAD RETURN"
					TA_Base_Core::NameValuePair pair4( "DataPointValue", "BAD RETURN");
					descriptionParameters.push_back ( &pair4 );
					//++TD14802
					 
					 AAMessageSender::instance()->sendAuditMessage(m_activeBRC [i].sessionID, TA_Base_Core::PlanAgentAudit::PlanManualGRCBadReturn, descriptionParameters);
				 }
				 else //auto GRC
				 {
					TA_Base_Core::NameValuePair pair2 ("EquipmentDescription", equipmentDescription);
					descriptionParameters.push_back (&pair2);
                    //weimin: should this be ReturnConditionString?
                    TA_Base_Core::NameValuePair pair3 ("LaunchingConditionString", (*dpProxy)->getLaunchingConditionResultString());
					descriptionParameters.push_back (&pair3);

					//TD14802++
					//Add dummy pair to set Value column in Event Viewer to "BAD RETURN"
					TA_Base_Core::NameValuePair pair4( "DataPointValue", "BAD RETURN");
					descriptionParameters.push_back ( &pair4 );
					//++TD14802

					AAMessageSender::instance()->sendAuditMessage(m_activeBRC [i].sessionID, TA_Base_Core::PlanAgentAudit::PlanAutoGRCBadReturn, descriptionParameters);
				 }

				 m_activeBRC.erase (m_activeBRC.begin () + i);
			 }
		 }
         catch(TA_Base_Core::ScadaProxyException& e)
         {
             LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
                 "ScadaProxyException caught in processEntityUpdateEvent - %s", e.what());
         }
         catch (...)
		 {
			 LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");
			 LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, "Exception thrown while receiving event");
		 }
		 
		 FUNCTION_EXIT;
    }


    //
    // subscribeToMessage
	 // make sure this method is only called for manually launched GRCs
	 // so that we only receive messages for those types of plans
    //
    void DataPointUpdateStateSubscriber::subscribeToMessages(unsigned long entityKey, std::string sessionID, 
			TA_Base_Core::ActivePlanId planID)
    {
		 FUNCTION_ENTRY("subscribeToMessage");
		 TA_ASSERT(entityKey != 0, "Entity key is 0");

         //use proxy to subscribe datapoint update message
         if (m_proxyMap.find(entityKey) == m_proxyMap.end())
         {
             TA_Base_Core::IEntityData* entity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityKey);
             TA_Base_Core::DataPointEntityData* dpEntity = dynamic_cast<TA_Base_Core::DataPointEntityData*>(entity);
             TA_ASSERT(dpEntity != NULL, "The entity for subcribe message is not a DataPoint");
             
             TA_Base_Bus::ScadaProxyFactory::getInstance().setProxiesToControlMode();
             
             TA_Base_Bus::DataPointProxySmartPtr *dpProxy = new TA_Base_Bus::DataPointProxySmartPtr();
			 TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy(
                 boost::shared_ptr<TA_Base_Core::DataPointEntityData>(dpEntity), *this, * dpProxy);
             
             m_proxyMap[entityKey] = dpProxy;
         }
		 
		 ActiveBRC actBRC;
		 actBRC.planID				  = planID;
		 actBRC.entityKey             = entityKey;
		 actBRC.sessionID             = sessionID;
		 m_activeBRC.push_back(actBRC);
		 
	 }

    //
    // unsubscribeToMessage
    //
    void DataPointUpdateStateSubscriber::unsubscribeToMessages()
    {
        FUNCTION_ENTRY("unsubscribeToMessage");
        
        TA_Base_Bus::ScadaProxyFactory::getInstance().setProxiesToMonitorMode();
        
        FUNCTION_EXIT;
    }

}   // TA_Base_App

#endif //DATA_POINT_UPDATE_SUBSCRIBER_CPP
