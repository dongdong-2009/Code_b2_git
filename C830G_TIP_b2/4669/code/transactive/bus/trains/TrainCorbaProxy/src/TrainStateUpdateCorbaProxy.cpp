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

#include "bus/trains/TrainCorbaProxy/src/TrainStateUpdateCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/CommonTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/RadioGroupTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/TrainInformationConversion.h"
#include "bus/trains/TrainCorbaProxy/src/StateUpdateTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/ExceptionConversion.h"
#include "bus/trains/TrainAgentCorba/src/TrainAgentObjectNames.h"

#include "bus/trains/TrainAgentCorba/idl/src/ITrainStateUpdateCorba.h"

#include "core/corba/src/VisitType.h"
#include "core/message/types/TrainAgentComms_MessageTypes.h"
#include "core/message/types/TrainAgentBroadcastComms_MessageTypes.h"
#include "core/naming/src/NamedObject.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/message/types/TrainAgentStateUpdate_MessageTypes.h" //limin, TD20554


namespace TA_IRS_Bus
{

    /**
     * The standard named object for access to the CORBA servant.
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:47:01 PM
     */
    typedef TA_Base_Core::NamedObject < TA_Base_Bus::ITrainStateUpdateCorba,
                                        TA_Base_Bus::ITrainStateUpdateCorba_ptr,
                                        TA_Base_Bus::ITrainStateUpdateCorba_var > ITrainStateUpdateCorbaNamedObject;


    struct TrainStateUpdateCorbaProxy::StateUpdateCorbaObject
    {
        ITrainStateUpdateCorbaNamedObject namedObject;
    };


    TrainStateUpdateCorbaProxy::TrainStateUpdateCorbaProxy( const std::string& agentName,
                                                            unsigned long agentKey,
                                                            unsigned long locationKey )
            : m_stateUpdateCorbaObject( new StateUpdateCorbaObject() ),
              m_subscribedToMessages( false ),
              m_trainAgentEntityName( agentName ),
              m_trainAgentEntityKey( agentKey ),
              m_trainAgentLocationKey( locationKey )
    {
        FUNCTION_ENTRY( "TrainStateUpdateCorbaProxy" );

        m_stateUpdateCorbaObject->namedObject.setCorbaName( agentName, TA_Base_Bus::TrainAgentObjectNames::TrainStateUpdateObjectName );

        FUNCTION_EXIT;
    }


    TrainStateUpdateCorbaProxy::TrainStateUpdateCorbaProxy( const std::string& agentName )
            : m_stateUpdateCorbaObject( new StateUpdateCorbaObject() ),
              m_trainAgentEntityName( agentName ),
              m_trainAgentEntityKey( 0 ),
              m_trainAgentLocationKey( 0 )
    {
        FUNCTION_ENTRY( "TrainStateUpdateCorbaProxy" );

        m_stateUpdateCorbaObject->namedObject.setCorbaName( agentName, TA_Base_Bus::TrainAgentObjectNames::TrainStateUpdateObjectName );

        FUNCTION_EXIT;
    }


    TrainStateUpdateCorbaProxy::~TrainStateUpdateCorbaProxy()
    {
        FUNCTION_ENTRY( "~TrainStateUpdateCorbaProxy" );

        unsubscribeToMessages();

        delete m_stateUpdateCorbaObject;
        m_stateUpdateCorbaObject = NULL;

        FUNCTION_EXIT;
    }


    StateUpdateTypes::FullLocalState TrainStateUpdateCorbaProxy::getLocalState()
    {
        FUNCTION_ENTRY( "getAllActivePec" );

        TA_Base_Bus::ITrainStateUpdateCorba::FullLocalState_var fullLocalState;

        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL_RETURN( fullLocalState, m_stateUpdateCorbaObject->namedObject, getLocalState, () ) );//limin

        FUNCTION_EXIT;
        return StateUpdateTypesConversion::convertToCpp( fullLocalState );
    }


    StateUpdateTypes::FullGlobalState TrainStateUpdateCorbaProxy::getGlobalState()
    {
        FUNCTION_ENTRY( "getAllActivePec" );

        TA_Base_Bus::ITrainStateUpdateCorba::FullGlobalState_var fullGlobalState;

        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL_RETURN( fullGlobalState, m_stateUpdateCorbaObject->namedObject, getGlobalState, () ) );

        FUNCTION_EXIT;
        return StateUpdateTypesConversion::convertToCpp( fullGlobalState );
    }


    void TrainStateUpdateCorbaProxy::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& payload )
    {
        FUNCTION_ENTRY( "receiveSpecialisedMessage" );

        // get the observers from the parent class (GenericObserverManager)
        GenericObserverManager<ITrainStateUpdateObserver>::ObserverList observerList = getObservers();

        // extract the message into its corba type, use the corba
        // conversion functions to convert to the native C++ type
        
        TA_Base_Bus::ITrainStateUpdateCorba::CommsState* trainCommsSyncState = NULL;
        TA_Base_Bus::ITrainStateUpdateCorba::CctvState* trainCctvSyncState = NULL;
        TA_Base_Bus::ITrainRadioGroupCorba::TrainRadioGroup* trainRadioGroup = NULL;
        TA_Base_Bus::ITrainStateUpdateCorba::NonDutyTrainTSI* nonDutyTrainTSI = NULL;
        
        if ( payload.messageState >>= trainCommsSyncState )
        {
            StateUpdateTypes::TrainCommsSyncState cppType = StateUpdateTypesConversion::convertToCpp( *trainCommsSyncState );

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Received %s", cppType.getLogString().c_str() );

            // notify each observer of the message also pass in payload.assocEntityKey
            for ( GenericObserverManager<ITrainStateUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processTrainCommsSyncState( cppType, payload.assocEntityKey );
            }
        }
        else if ( payload.messageState >>= trainCctvSyncState )
        {
            StateUpdateTypes::TrainCctvSyncState cppType = StateUpdateTypesConversion::convertToCpp( *trainCctvSyncState );

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Received %s", cppType.getLogString().c_str() );

            // notify each observer of the message also pass in payload.assocEntityKey
            for ( GenericObserverManager<ITrainStateUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processTrainCctvSyncState( cppType, payload.assocEntityKey );
            }
        }
        else if ( payload.messageState >>= trainRadioGroup )
        {
            RadioGroupTypes::TrainRadioGroup cppType = RadioGroupTypesConversion::convertToCpp( *trainRadioGroup );

            std::ostringstream locationCoverage;
            RadioGroupTypes::GroupLocationList::iterator ite = cppType.locationCoverage.begin();
            locationCoverage << *( ite++ );
            for ( ; ite != cppType.locationCoverage.end(); ++ite )
            {
                locationCoverage << ", " << *ite;
            }

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Received %s", cppType.getLogString().c_str() );

            // notify each observer of the message also pass in payload.assocEntityKey
            for ( GenericObserverManager<ITrainStateUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processTrainRadioGroup( cppType, payload.assocEntityKey );
            }
        }
        else if ( payload.messageState >>= nonDutyTrainTSI )
        {
            StateUpdateTypes::NonDutyTrainTSI cppType = StateUpdateTypesConversion::convertToCpp( *nonDutyTrainTSI );

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Received %s", cppType.getLogString().c_str() );

            // notify each observer of the message also pass in payload.assocEntityKey
            for ( GenericObserverManager<ITrainStateUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processNonDutyTrainTSI( cppType, payload.assocEntityKey );
            }
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "Unknown message type received." );
        }

        FUNCTION_EXIT;
    }


    void TrainStateUpdateCorbaProxy::receiveSpecialisedMessage( const TA_Base_Core::StateUpdateMessageCorbaDef& payload )
    {
        FUNCTION_ENTRY( "receiveSpecialisedMessage" );

        // get the observers from the parent class (GenericObserverManager)
        GenericObserverManager<ITrainStateUpdateObserver>::ObserverList observerList = getObservers();

        // extract the message into its corba type, use the corba
        // conversion functions to convert to the native C++ type
        TA_Base_Bus::ITrainInformationCorba::AgentOperationMode* agentOperationMode = NULL;
        TA_Base_Bus::ITrainStateUpdateCorba::RadioCallDetailsList* radioCallDetailsSet = NULL;
        TA_Base_Bus::ITrainStateUpdateCorba::SavedTrainSelectionSequence* trainSelectionMap = NULL;
        TA_Base_Bus::ITrainStateUpdateCorba::AlarmState* trainAlarmSyncState = NULL;
        TA_Base_Bus::ITrainStateUpdateCorba::CallState* trainCallSyncState = NULL;
        TA_Base_Bus::ITrainStateUpdateCorba::PaState* trainPaSyncState = NULL;
        TA_Base_Bus::ITrainStateUpdateCorba::PecState* trainPecSyncState = NULL;
        TA_Base_Bus::ITrainStateUpdateCorba::TisState* trainTisSyncState = NULL;

        if ( payload.stateUpdateInfo >>= agentOperationMode )
        {
            TrainInformationTypes::AgentOperationMode cppType = TrainInformationConversion::convertToCpp( *agentOperationMode );

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Received %s", cppType.getLogString().c_str() ); //limin, CL-20691

            // notify each observer of the message also pass in payload.assocEntityKey
            for ( GenericObserverManager< ITrainStateUpdateObserver >::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processAgentOperationMode( cppType, payload.assocEntityKey );
            }
        }
        else if ( payload.stateUpdateInfo >>= radioCallDetailsSet )
        {
            StateUpdateTypes::RadioCallDetailsSet cppType = StateUpdateTypesConversion::convertToCpp( *radioCallDetailsSet );

            for ( StateUpdateTypes::RadioCallDetailsSet::iterator ite = cppType.begin();
                  ite != cppType.end(); ++ite )
            {
                // Logs the full details of the received message
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                             "Received %s",
                             StateUpdateTypes::radioCallDetailsSetToString( cppType ).c_str() );
            }

            // notify each observer of the message also pass in payload.assocEntityKey
            for ( GenericObserverManager<ITrainStateUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processRadioCallDetailsSet( cppType, payload.assocEntityKey );
            }
        }
        else if ( payload.stateUpdateInfo >>= trainSelectionMap )
        {
            StateUpdateTypes::TrainSelectionMap cppType = StateUpdateTypesConversion::convertToCpp( *trainSelectionMap );

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Received %s",
                         StateUpdateTypes::trainSelectionMapToString( cppType ).c_str() );

            // notify each observer of the message also pass in payload.assocEntityKey
            for ( GenericObserverManager<ITrainStateUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processTrainSelectionMap( cppType, payload.assocEntityKey );
            }
        }
        else if ( payload.stateUpdateInfo >>= trainAlarmSyncState )
        {
            StateUpdateTypes::TrainAlarmSyncState cppType = StateUpdateTypesConversion::convertToCpp( *trainAlarmSyncState );

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Received %s", cppType.getLogString().c_str() );

            // notify each observer of the message also pass in payload.assocEntityKey
            for ( GenericObserverManager<ITrainStateUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processTrainAlarmSyncState( cppType, payload.assocEntityKey );
            }
        }
        else if ( payload.stateUpdateInfo >>= trainCallSyncState )
        {
            StateUpdateTypes::TrainCallSyncState cppType = StateUpdateTypesConversion::convertToCpp( *trainCallSyncState );

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Received %s", cppType.getLogString().c_str() );

            // notify each observer of the message also pass in payload.assocEntityKey
            for ( GenericObserverManager<ITrainStateUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processTrainCallSyncState( cppType, payload.assocEntityKey );
            }
        }
        else if ( payload.stateUpdateInfo >>= trainPaSyncState )
        {
            StateUpdateTypes::TrainPaSyncState cppType = StateUpdateTypesConversion::convertToCpp( *trainPaSyncState );

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Received %s", cppType.getLogString().c_str() );

            // notify each observer of the message also pass in payload.assocEntityKey
            for ( GenericObserverManager<ITrainStateUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processTrainPaSyncState( cppType, payload.assocEntityKey );
            }
        }
        else if ( payload.stateUpdateInfo >>= trainPecSyncState )
        {
            StateUpdateTypes::TrainPecSyncState cppType = StateUpdateTypesConversion::convertToCpp( *trainPecSyncState );

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Received %s", cppType.getLogString().c_str() );

            // notify each observer of the message also pass in payload.assocEntityKey
            for ( GenericObserverManager<ITrainStateUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processTrainPecSyncState( cppType, payload.assocEntityKey );
            }
        }
        else if ( payload.stateUpdateInfo >>= trainTisSyncState )
        {
            StateUpdateTypes::TrainTisSyncState cppType = StateUpdateTypesConversion::convertToCpp( *trainTisSyncState );

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Received %s", cppType.getLogString().c_str() );

            // notify each observer of the message also pass in payload.assocEntityKey
            for ( GenericObserverManager<ITrainStateUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processTrainTisSyncState( cppType, payload.assocEntityKey );
            }
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "Unknown message type received." );
        }

        FUNCTION_EXIT;
    }


    void TrainStateUpdateCorbaProxy::firstObserverAdded()
    {
        FUNCTION_ENTRY( "firstObserverAdded" );

        // call subscribeToMessages
        subscribeToMessages();

        FUNCTION_EXIT;
    }


    void TrainStateUpdateCorbaProxy::lastObserverRemoved()
    {
        FUNCTION_ENTRY( "lastObserverRemoved" );

        // call unsubscribeToMessages
        unsubscribeToMessages();

        FUNCTION_EXIT;
    }


    void TrainStateUpdateCorbaProxy::subscribeToMessages()
    {
        FUNCTION_ENTRY( "subscribeToMessages" );

        if ( ( 0 == m_trainAgentEntityKey ) ||
             ( 0 == m_trainAgentLocationKey ) )
        {

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                         "Cannot use a proxy constructed without entity information to subscribe to state updates" );

            FUNCTION_EXIT;
            return;
        }


        // subscribe to messages from the train agent if not already subscribed
        // (as per the Train Agent Message Types package)

        TA_THREADGUARD( m_observerLock );

        if ( false == m_subscribedToMessages )
        {
            // Broadcast Comms Messages: TrainAgentSyncState
            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
                TA_Base_Core::TrainAgentBroadcastComms::TrainAgentSyncState,
                static_cast< TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>* >( this ) );

            // Broadcast Comms Messages: TrainCommunicationError
            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
                TA_Base_Core::TrainAgentBroadcastComms::TrainCommunicationError,
                static_cast< TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>* >( this ) );

            // State Update Messages: TrainAgentStateUpdate
            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
                TA_Base_Core::TrainAgentStateUpdate::TrainAgentSyncStateUpdate,
                static_cast< TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>* >( this ),
                m_trainAgentEntityName,
                NULL );

            m_subscribedToMessages = true;
        }

        FUNCTION_EXIT;
    }


    void TrainStateUpdateCorbaProxy::unsubscribeToMessages()
    {
        FUNCTION_ENTRY( "unsubscribeToMessages" );

        // unsubscribe to messages from the train agent if subscribed
        TA_THREADGUARD( m_observerLock );

        if ( true == m_subscribedToMessages )
        {
            TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(
                static_cast< TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>* >( this ) );
            
            TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(
                static_cast< TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>* >( this ) );
            
            m_subscribedToMessages = false;
        }

        FUNCTION_EXIT;
    }

}

