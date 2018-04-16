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


#include "bus/trains/TrainCorbaProxy/src/TrainInformationCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/CommonTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/TrainInformationConversion.h"
#include "bus/trains/TrainCorbaProxy/src/ExceptionConversion.h"
#include "bus/trains/TrainAgentCorba/src/TrainAgentObjectNames.h"

#include "bus/trains/TrainAgentCorba/idl/src/ITrainInformationCorba.h"

#include "core/corba/src/VisitType.h"
#include "core/message/types/TrainAgentComms_MessageTypes.h"
#include "core/naming/src/NamedObject.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{

    /**
     * The standard named object for access to the CORBA servant.
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:56 PM
     */
    typedef TA_Base_Core::NamedObject <  TA_Base_Bus::ITrainInformationCorba,
                                         TA_Base_Bus::ITrainInformationCorba_ptr,
                                         TA_Base_Bus::ITrainInformationCorba_var > ITrainInformationCorbaNamedObject;

    
    struct TrainInformationCorbaProxy::TrainInformationCorbaObject
    {
        ITrainInformationCorbaNamedObject namedObject;
    };


    TrainInformationCorbaProxy::TrainInformationCorbaProxy( const std::string& agentName,
                                                            unsigned long agentKey,
                                                            unsigned long locationKey )
            : m_trainInformationCorbaObject( new TrainInformationCorbaObject() ),
              m_subscribedToMessages( false ),
              m_trainAgentEntityName( agentName ),
              m_trainAgentEntityKey( agentKey ),
              m_trainAgentLocationKey( locationKey )

    {
        FUNCTION_ENTRY( "TrainInformationCorbaProxy" );

        m_trainInformationCorbaObject->namedObject.setCorbaName( agentName, TA_Base_Bus::TrainAgentObjectNames::TrainInformationObjectName );

        FUNCTION_EXIT;
    }


    TrainInformationCorbaProxy::~TrainInformationCorbaProxy()
    {
        FUNCTION_ENTRY( "~TrainInformationCorbaProxy" );

        unsubscribeToMessages();

        delete m_trainInformationCorbaObject;
        m_trainInformationCorbaObject = NULL;

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::TrainInformationTypes::TrainDetails TrainInformationCorbaProxy::getTrainDetails( CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "getTrainDetails" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        TA_Base_Bus::ITrainInformationCorba::TrainDetails_var trainDetails;

        CORBA_CATCH( CORBA_CALL_RETURN( trainDetails, m_trainInformationCorbaObject->namedObject, getTrainDetails, ( trainId ) ) );//limin

        FUNCTION_EXIT;
        return TrainInformationConversion::convertToCpp( trainDetails );
    }


    TA_IRS_Bus::TrainInformationTypes::TrainDetailsList TrainInformationCorbaProxy::getTrainList()
    {
        FUNCTION_ENTRY( "getTrainList" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        TA_Base_Bus::ITrainInformationCorba::TrainDetailsList_var trainDetailsList;

        CORBA_CATCH( CORBA_CALL_RETURN( trainDetailsList, m_trainInformationCorbaObject->namedObject, getTrainList, () ) );//limin

        FUNCTION_EXIT;
        return TrainInformationConversion::convertToCpp( trainDetailsList );
    }


    TA_IRS_Bus::TrainInformationTypes::TrainSelectionList TrainInformationCorbaProxy::getTrainSelectionList()
    {
        FUNCTION_ENTRY( "getTrainSelectionList" );

        TA_Base_Bus::ITrainInformationCorba::TrainSelectionList_var trainSelectionList;

        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL_RETURN( trainSelectionList, m_trainInformationCorbaObject->namedObject, getTrainSelectionList, () ) );//limin

        FUNCTION_EXIT;
        return TrainInformationConversion::convertToCpp( trainSelectionList );
    }


    CommonTypes::TrainIdList TrainInformationCorbaProxy::getTrainSelection( const std::string& selectionName )
    {
        FUNCTION_ENTRY( "getTrainSelection" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        TA_Base_Bus::ITrainCommonCorba::TrainList_var trainIdList;
        
        CORBA_CATCH( CORBA_CALL_RETURN( trainIdList, m_trainInformationCorbaObject->namedObject, getTrainSelection, ( selectionName.c_str() ) ) );//limin

        FUNCTION_EXIT;
        return CommonTypesConversion::convertToCpp( trainIdList );
    }


    void TrainInformationCorbaProxy::deleteTrainSelection( const std::string& selectionName, const std::string& sessionId )
    {
        FUNCTION_ENTRY( "deleteTrainSelection" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_trainInformationCorbaObject->namedObject, deleteTrainSelection, ( selectionName.c_str(), sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
    }


    void TrainInformationCorbaProxy::saveTrainSelection( const std::string& selectionName,
                                                         const CommonTypes::TrainIdList& trainIDs,
                                                         const std::string& sessionId )
    {
        FUNCTION_ENTRY( "saveTrainSelection" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_trainInformationCorbaObject->namedObject, saveTrainSelection, ( selectionName.c_str(),
                                                                                     CommonTypesConversion::convertToCorba( trainIDs ),
                                                                                     sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::TrainInformationTypes::AgentOperationMode TrainInformationCorbaProxy::getAgentOperationMode()
    {
        FUNCTION_ENTRY( "getAgentOperationMode" );

        TA_Base_Bus::ITrainInformationCorba::AgentOperationMode_var agentOperationMode;

        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL_RETURN( agentOperationMode, m_trainInformationCorbaObject->namedObject, getAgentOperationMode, () ) );

        FUNCTION_EXIT;
        return TrainInformationConversion::convertToCpp( agentOperationMode );
    }


    void TrainInformationCorbaProxy::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& payload )
    {
        FUNCTION_ENTRY( "receiveSpecialisedMessage" );

        // extract the message into its corba type, use the corba
        // conversion functions to convert to the native C++ type
        TA_Base_Bus::ITrainInformationCorba::AgentOperationMode* agentOperationMode = NULL;
        TA_Base_Bus::ITrainInformationCorba::TrainDetails* trainDetails = NULL;
        TA_Base_Bus::ITrainInformationCorba::TrainSelectionUpdate* trainSelectionUpdate = NULL;

        if ( payload.messageState >>= agentOperationMode )
        {
            TrainInformationTypes::AgentOperationMode cppType = TrainInformationConversion::convertToCpp( *agentOperationMode );

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Received %s", cppType.getLogString().c_str() );

            // get the observers from the parent class (GenericObserverManager)
            GenericObserverManager<ITrainInformationUpdateObserver>::ObserverList observerList = getObservers();

            // notify each observer of the message
            for ( GenericObserverManager<ITrainInformationUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processAgentOperationMode( cppType );
            }
        }
        else if ( payload.messageState >>= trainDetails )
        {
            TrainInformationTypes::TrainDetails cppType = TrainInformationConversion::convertToCpp( *trainDetails );

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Received %s", cppType.getLogString().c_str() );

            // get the observers from the parent class (GenericObserverManager)
            GenericObserverManager<ITrainInformationUpdateObserver>::ObserverList observerList = getObservers();

            // notify each observer of the message
            for ( GenericObserverManager<ITrainInformationUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processTrainDetails( cppType );
            }
        }
        else if ( payload.messageState >>= trainSelectionUpdate )
        {
            TrainInformationTypes::TrainSelectionUpdate cppType = TrainInformationConversion::convertToCpp( *trainSelectionUpdate );

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Received %s", cppType.getLogString().c_str() );

            // get the observers from the parent class (GenericObserverManager)
            GenericObserverManager<ITrainInformationUpdateObserver>::ObserverList observerList = getObservers();

            // notify each observer of the message
            for ( GenericObserverManager<ITrainInformationUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processTrainSelectionUpdate( cppType );
            }
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "Unknown message type received." );
        }

        FUNCTION_EXIT;
    }


    void TrainInformationCorbaProxy::firstObserverAdded()
    {
        FUNCTION_ENTRY( "firstObserverAdded" );

        // call subscribeToMessages
        subscribeToMessages();

        FUNCTION_EXIT;
    }


    void TrainInformationCorbaProxy::lastObserverRemoved()
    {
        FUNCTION_ENTRY( "lastObserverRemoved" );

        // call unsubscribeToMessages
        unsubscribeToMessages();

        FUNCTION_EXIT;
    }


    void TrainInformationCorbaProxy::subscribeToMessages()
    {
        FUNCTION_ENTRY( "subscribeToMessages" );

        TA_THREADGUARD( m_observerLock );

        // subscribe to messages from the train agent if not already subscribed
        // Comms Messages: TrainDetailsUpdate, TrainSelectionUpdate, OperationModeUpdate
        // (as per the Train Agent Message Types package)

        if ( false == m_subscribedToMessages )
        {
            // TrainDetailsUpdate
            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
                TA_Base_Core::TrainAgentComms::TrainDetailsUpdate, this,
                m_trainAgentEntityKey, 0, m_trainAgentLocationKey );

            // TrainSelectionUpdate
            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
                TA_Base_Core::TrainAgentComms::TrainSelectionUpdate, this,
                m_trainAgentEntityKey, 0, m_trainAgentLocationKey );

            // OperationModeUpdate
            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
                TA_Base_Core::TrainAgentComms::OperationModeUpdate, this,
                m_trainAgentEntityKey, 0, m_trainAgentLocationKey );

            m_subscribedToMessages = true;
        }

        FUNCTION_EXIT;

    }


    void TrainInformationCorbaProxy::unsubscribeToMessages()
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
