/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioPatchManagerDialog/PatchDLTSMessageSubscriber.cpp $
  * @author:  Alexis Laredo
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * <description>
  */


#include "app/radio/radio_manager_gui_icdv4/src/RadioPatchManagerDialog/PatchDLTSMessageSubscriber.h"
//#include "app/radio/radio_manager_gui_icdv4/src/RadioPatchManagerDialog/PatchOperations.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentHelper.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/TelephonyComms_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"


TA_Base_Core::ReEntrantThreadLockable PatchDLTSMessageSubscriber::m_lock;

const std::string PatchDLTSMessageSubscriber::TELEPHONE_SERVER_AGENT_ENTITY_NAME = "TelephoneAgentCommand";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
PatchDLTSMessageSubscriber::PatchDLTSMessageSubscriber()
{
	initialise();
}

PatchDLTSMessageSubscriber::~PatchDLTSMessageSubscriber()
{
	TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
}


void PatchDLTSMessageSubscriber::initialise()
{
    FUNCTION_ENTRY("initialise");
    
    unsigned long occLocKey = TA_Base_Core::LocationAccessFactory::getInstance().getOccComLocationKey();
    //
    // Get the Telephone agent's entity data from its type and location.
    //
    std::vector<TA_Base_Core::IEntityData*> entities =
        TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(
            TELEPHONE_SERVER_AGENT_ENTITY_NAME, occLocKey);
    
    if(entities.size() != 1)
    {
        const char* msg = "There is no Telephone Agent at this location.";
        TA_THROW(TA_Base_Core::TransactiveException(msg));
    }


    std::string entityName     = "";
    std::string locationName   = "";
    unsigned long entityKey    = 0;
    unsigned long subsystemKey = 0;
    
    try
    {
        entityKey    = entities[0]->getKey();
        subsystemKey = entities[0]->getSubsystem();
        entityName   = entities[0]->getName();
    }
    catch(...)
    {
        delete entities[0];
        entities[0] = NULL;
        throw;
    }
    delete entities[0];
    entities[0] = NULL;

    //
    // Subscribe to Telephone notifications from the Telephone Agent
    //
	TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
        TA_Base_Core::TelephonyComms::DLTSResourceCallStatus,
        (TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*)(this),
        entityKey,
        subsystemKey,
        occLocKey);

	FUNCTION_EXIT;
}


void PatchDLTSMessageSubscriber::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& message)
{
	TA_Base_Core::ThreadGuard lock(m_lock);
	
    FUNCTION_ENTRY("receiveSpecialisedMessage");
    
    //
    // Make sure the message is a Telephone notification
    //
    if( std::string(message.messageTypeKey) !=
        TA_Base_Core::TelephonyComms::DLTSResourceCallStatus.getTypeKey() )
    {
        FUNCTION_EXIT;
        return;
    }

    //
    // Extract to the Telephone notification
    //
    //TA_IRS_Bus::TelephoneAgentIDL::DltsResourceCallStatusNotification* notification;
    //if( (message.messageState >>= notification) == 0 )
    //{
    //    FUNCTION_EXIT;
    //    return;
    //}


	/*
	 if (notification->status == TA_IRS_Bus::Telephony::DLTSCallEstablished)
     {
		 TA_IRS_Bus::TelephoneResource resource;
		 RadioMFTAgentHelper::convertTelephoneFromIDL(notification->resource,resource);

		 The_PatchOperations::instance()->onDLTSCallEstablishedEvent(resource);
	 }
	 else if (notification->status == TA_IRS_Bus::Telephony::DLTSCallOnHold)
	 {
		 TA_IRS_Bus::TelephoneResource resource;
		 RadioMFTAgentHelper::convertTelephoneFromIDL(notification->resource,resource);

		 The_PatchOperations::instance()->onDLTSCallHoldEvent(resource);
	 }
*/
	 FUNCTION_EXIT;
}