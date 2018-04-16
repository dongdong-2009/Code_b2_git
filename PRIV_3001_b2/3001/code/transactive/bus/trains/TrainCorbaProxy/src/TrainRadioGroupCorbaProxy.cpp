/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Robert V
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "bus/trains/TrainCorbaProxy/src/TrainRadioGroupCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/CommonTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/RadioGroupTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/ExceptionConversion.h"
#include "bus/trains/TrainAgentCorba/src/TrainAgentObjectNames.h"

#include "bus/trains/TrainAgentCorba/idl/src/ITrainRadioGroupCorba.h"

#include "core/corba/src/VisitType.h"
#include "core/message/types/TrainAgentBroadcastComms_MessageTypes.h"
#include "core/naming/src/NamedObject.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{

    /**
     * The standard named object for access to the CORBA servant.
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:47:00 PM
     */
    typedef TA_Base_Core::NamedObject < TA_Base_Bus::ITrainRadioGroupCorba,
                                        TA_Base_Bus::ITrainRadioGroupCorba_ptr,
                                        TA_Base_Bus::ITrainRadioGroupCorba_var > ITrainRadioGroupCorbaNamedObject;


    struct TrainRadioGroupCorbaProxy::RadioGroupCorbaObject
    {
        ITrainRadioGroupCorbaNamedObject namedObject;
    };


    TrainRadioGroupCorbaProxy::TrainRadioGroupCorbaProxy( const std::string& agentName,
                                                          unsigned long agentKey,
                                                          unsigned long locationKey )
         :  m_radioGroupCorbaObject( new RadioGroupCorbaObject() ),
            m_trainAgentEntityKey( agentKey ),
            m_trainAgentEntityName( agentName ),
            m_trainAgentLocationKey( locationKey ),
            m_subscribedToMessages( false )
    {
        FUNCTION_ENTRY( "TrainRadioGroupCorbaProxy" );

        m_radioGroupCorbaObject->namedObject.setCorbaName( agentName, TA_Base_Bus::TrainAgentObjectNames::TrainRadioGroupObjectName );

        FUNCTION_EXIT;
    }


    TrainRadioGroupCorbaProxy::~TrainRadioGroupCorbaProxy()
    {
        FUNCTION_ENTRY( "~TrainRadioGroupCorbaProxy" );

        unsubscribeToMessages();

        delete m_radioGroupCorbaObject;
        m_radioGroupCorbaObject = NULL;

        FUNCTION_EXIT;
    }


    RadioGroupTypes::TrainRadioGroupMap TrainRadioGroupCorbaProxy::getTrainRadioGroups( unsigned long location )
    {
        FUNCTION_ENTRY( "getAllActivePec" );

        // Use the normal named object
        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        TA_Base_Bus::ITrainRadioGroupCorba::TrainRadioGroupList_var trainRadioGroupMap;

        CORBA_CATCH( CORBA_CALL_RETURN( trainRadioGroupMap, m_radioGroupCorbaObject->namedObject, getTrainRadioGroups, () ) );//limin

        FUNCTION_EXIT;
        return RadioGroupTypesConversion::convertToCpp( trainRadioGroupMap );
    }


    void TrainRadioGroupCorbaProxy::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& payload )
    {
        FUNCTION_ENTRY( "receiveSpecialisedMessage" );

        // extract the message into its corba type, use the corba
        // conversion functions to convert to the native C++ type
        TA_Base_Bus::ITrainRadioGroupCorba::TrainRadioGroup* pecUpdate = NULL;

        if ( payload.messageState >>= pecUpdate )
        {
            RadioGroupTypes::TrainRadioGroup cppType = RadioGroupTypesConversion::convertToCpp( *pecUpdate );

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Received %s", cppType.getLogString().c_str() );

            // get the observers from the parent class (GenericObserverManager)
            GenericObserverManager<ITrainRadioGroupUpdateObserver>::ObserverList observerList = getObservers();

            // notify each observer of the message
            for ( GenericObserverManager<ITrainRadioGroupUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processTrainRadioGroup( cppType );
            }
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "Unknown message type received." );
        }

        FUNCTION_EXIT;
    }


    void TrainRadioGroupCorbaProxy::firstObserverAdded()
    {
        FUNCTION_ENTRY( "firstObserverAdded" );

        // call subscribeToMessages
        subscribeToMessages();

        FUNCTION_EXIT;
    }


    void TrainRadioGroupCorbaProxy::lastObserverRemoved()
    {
        FUNCTION_ENTRY( "lastObserverRemoved" );

        // call unsubscribeToMessages
        unsubscribeToMessages();

        FUNCTION_EXIT;
    }


    void TrainRadioGroupCorbaProxy::subscribeToMessages()
    {
        FUNCTION_ENTRY( "subscribeToMessages" );

        // subscribe to messages from the train agent if not already subscribed
        // (as per the Train Agent Message Types package)

        TA_THREADGUARD( m_observerLock );

        if ( false == m_subscribedToMessages )
        {
            // Broadcast Comms Messages: RadioGroupUpdate
            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
                TA_Base_Core::TrainAgentBroadcastComms::RadioGroupUpdate, this );

            m_subscribedToMessages = true;
        }

        FUNCTION_EXIT;
    }


    void TrainRadioGroupCorbaProxy::unsubscribeToMessages()
    {
        FUNCTION_ENTRY( "unsubscribeToMessages" );

        // unsubscribe to messages from the train agent if subscribed
        TA_THREADGUARD( m_observerLock );

        if ( true == m_subscribedToMessages )
        {
            TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages( this );
            m_subscribedToMessages = false;
        }

        FUNCTION_EXIT;
    }

}
