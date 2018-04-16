/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Robert van Hugten
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */


#include "bus/trains/TrainCorbaProxy/src/TrainDriverCallCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/CommonTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/CallTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/ExceptionConversion.h"
#include "bus/trains/TrainAgentCorba/src/TrainAgentObjectNames.h"

#include "core/corba/src/VisitType.h"
#include "core/message/types/TrainAgentComms_MessageTypes.h"
#include "core/naming/src/NamedObject.h"
#include "core/naming/src/FlexibleTimeoutNamedObject.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{

    /**
     * The standard named object for access to the CORBA servant.
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:54 PM
     */
    typedef TA_Base_Core::NamedObject < TA_Base_Bus::ITrainDriverCallCorba,
                                        TA_Base_Bus::ITrainDriverCallCorba_ptr,
                                        TA_Base_Bus::ITrainDriverCallCorba_var > ITrainDriverCallCorbaNamedObject;

    /**
     * The standard named object for access to the CORBA servant.
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:54 PM
     */
    typedef TA_Base_Core::FlexibleTimeoutNamedObject < TA_Base_Bus::ITrainDriverCallCorba,
                                                       TA_Base_Bus::ITrainDriverCallCorba_ptr,
                                                       TA_Base_Bus::ITrainDriverCallCorba_var > ITrainDriverCallCorbaFlexibleNamedObject;

    struct TrainDriverCallCorbaProxy::DriverCallCorbaObject
    {
        ITrainDriverCallCorbaNamedObject namedObject;
        ITrainDriverCallCorbaFlexibleNamedObject flexibleTimeoutNamedObject;
    };


    TrainDriverCallCorbaProxy::TrainDriverCallCorbaProxy( const std::string& agentName,
                                                          unsigned long agentKey,
                                                          unsigned long locationKey )
            : m_driverCallCorbaObject( new DriverCallCorbaObject() ),
              m_subscribedToMessages( false ),
              m_trainAgentEntityName( agentName ),
              m_trainAgentEntityKey( agentKey ),
              m_trainAgentLocationKey( locationKey )
    {
        FUNCTION_ENTRY( "TrainDriverCallCorbaProxy" );

        m_driverCallCorbaObject->namedObject.setCorbaName( agentName, TA_Base_Bus::TrainAgentObjectNames::TrainDriverCallObjectName );
        m_driverCallCorbaObject->flexibleTimeoutNamedObject.setCorbaName( agentName, TA_Base_Bus::TrainAgentObjectNames::TrainDriverCallObjectName );

        FUNCTION_EXIT;
    }


    TrainDriverCallCorbaProxy::~TrainDriverCallCorbaProxy()
    {
        FUNCTION_ENTRY( "~TrainDriverCallCorbaProxy" );

        unsubscribeToMessages();

        delete m_driverCallCorbaObject;
        m_driverCallCorbaObject = NULL;

        FUNCTION_EXIT;
    }


    CallTypes::OccCallRequestList TrainDriverCallCorbaProxy::getAllOccCallRequests()
    {
        FUNCTION_ENTRY( "getAllOccCallRequests" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        TA_Base_Bus::ITrainDriverCallCorba::OccCallRequestList_var occCallRequestList;

        CORBA_CATCH( CORBA_CALL_RETURN( occCallRequestList, m_driverCallCorbaObject->namedObject, getAllOccCallRequests, () ) );//limin

        FUNCTION_EXIT;
        return CallTypesConversion::convertToCpp( occCallRequestList );
    }



    void TrainDriverCallCorbaProxy::answerOccCallRequest( CommonTypes::TrainIdType trainId,
                                                          const std::string& sessionId )
    {
        FUNCTION_ENTRY( "answerOccCallRequest" );

        // Use the flexible timeout named object
        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_driverCallCorbaObject->flexibleTimeoutNamedObject, answerOccCallRequest, ( trainId, sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
    }


    CallTypes::DriverCallResult TrainDriverCallCorbaProxy::makeDriverCall( const CommonTypes::TrainIdList& trainList,
                                                                           const CallTypes::ItsiList& individualSubscribers,
                                                                           bool override,
                                                                           CallTypes::ECallType callType,
                                                                           CallTypes::ECallPriority callPriority,
                                                                           const std::string& sessionId )
    {
        FUNCTION_ENTRY( "makeDriverCall" );


        TA_Base_Bus::ITrainDriverCallCorba::DriverCallResult_var driverCallResult;

        // Use the flexible timeout named object
        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL_RETURN( driverCallResult, m_driverCallCorbaObject->flexibleTimeoutNamedObject, makeDgnaDriverCall, ( CommonTypesConversion::convertToCorba( trainList ),
                                                                                                                 CallTypesConversion::convertToCorba( individualSubscribers ),
                                                                                                                 override,
                                                                                                                 CallTypesConversion::convertToCorba( callType ),
                                                                                                                 CallTypesConversion::convertToCorba( callPriority ),
                                                                                                                 sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
        return CallTypesConversion::convertToCpp( driverCallResult );
    }


    CallTypes::DriverCallResult TrainDriverCallCorbaProxy::makeDriverCall( const CommonTypes::TrainIdList& trainList,
                                                                           const std::string& groupTsi,
                                                                           bool override,
                                                                           CallTypes::ECallType callType,
                                                                           CallTypes::ECallPriority callPriority,
                                                                           const std::string& sessionId )
    {
        FUNCTION_ENTRY( "makeDriverCall" );

        TA_Base_Bus::ITrainDriverCallCorba::DriverCallResult_var driverCallResult;

        // Do parameter conversion using the appropriate convertToCorba methods
        // Use the flexible timeout named object
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL_RETURN( driverCallResult, m_driverCallCorbaObject->flexibleTimeoutNamedObject, makeStaticGroupDriverCall, ( CommonTypesConversion::convertToCorba( trainList ),
                                                                                                                        groupTsi.c_str(),
                                                                                                                        override,
                                                                                                                        CallTypesConversion::convertToCorba( callType ),
                                                                                                                        CallTypesConversion::convertToCorba( callPriority ),
                                                                                                                        sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
        return CallTypesConversion::convertToCpp( driverCallResult );
    }


    void TrainDriverCallCorbaProxy::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& payload )
    {
        FUNCTION_ENTRY( "receiveSpecialisedMessage" );

        // extract the message into its corba type, use the corba
        // conversion functions to convert to the native C++ type
        TA_Base_Bus::ITrainDriverCallCorba::OccCallRequest* occCallRequest = NULL;
        TA_Base_Bus::ITrainDriverCallCorba::OccCallReset* occCallReset = NULL;

        if ( payload.messageState >>= occCallRequest )
        {
            CallTypes::OccCallRequest cppType = CallTypesConversion::convertToCpp( *occCallRequest );

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Received %s", cppType.getLogString().c_str() );

            // get the observers from the parent class (GenericObserverManager)
            GenericObserverManager<ITrainDriverCallUpdateObserver>::ObserverList observerList = getObservers();

            // notify each observer of the message

            for ( GenericObserverManager<ITrainDriverCallUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processOccCallRequest( cppType );
            }
        }
        else if ( payload.messageState >>= occCallReset )
        {
            CallTypes::OccCallReset cppType = CallTypesConversion::convertToCpp( *occCallReset );

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Received %s", cppType.getLogString().c_str() );

            // get the observers from the parent class (GenericObserverManager)
            GenericObserverManager<ITrainDriverCallUpdateObserver>::ObserverList observerList = getObservers();

            //notify each observer of the message

            for ( GenericObserverManager<ITrainDriverCallUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processOccCallReset( cppType );
            }
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "Unknown message type received." );
        }

        FUNCTION_EXIT;
    }


    void TrainDriverCallCorbaProxy::firstObserverAdded()
    {
        FUNCTION_ENTRY( "firstObserverAdded" );

        // call subscribeToMessages
        subscribeToMessages();

        FUNCTION_EXIT;
    }


    void TrainDriverCallCorbaProxy::lastObserverRemoved()
    {
        FUNCTION_ENTRY( "lastObserverRemoved" );
        // call unsubscribeToMessages
        unsubscribeToMessages();

        FUNCTION_EXIT;
    }


    void TrainDriverCallCorbaProxy::subscribeToMessages()
    {
        FUNCTION_ENTRY( "subscribeToMessages" );

        TA_THREADGUARD( m_observerLock );

        // subscribe to messages from the train agent if not already subscribed
        // (as per the Train Agent Message Types package) Comms Messages::CallDriverMessage

        if ( false == m_subscribedToMessages )
        {
            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
                TA_Base_Core::TrainAgentComms::CallDriverMessage,
                this, m_trainAgentEntityKey, 0, m_trainAgentLocationKey );

            m_subscribedToMessages = true;
        }

        FUNCTION_EXIT;
    }


    void TrainDriverCallCorbaProxy::unsubscribeToMessages()
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
