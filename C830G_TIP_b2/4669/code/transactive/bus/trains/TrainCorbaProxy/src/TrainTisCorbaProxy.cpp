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

#include "bus/trains/TrainCorbaProxy/src/TrainTisCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/CommonTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/TisTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/ExceptionConversion.h"
#include "bus/trains/TrainAgentCorba/src/TrainAgentObjectNames.h"

#include "bus/trains/TrainAgentCorba/idl/src/ITrainTisCorba.h"

#include "core/corba/src/VisitType.h"
#include "core/message/types/TrainAgentComms_MessageTypes.h"
#include "core/message/types/TrainAgentBroadcastComms_MessageTypes.h"
#include "core/naming/src/NamedObject.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{

    /**
     * The standard named object for access to the CORBA servant.
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:47:02 PM
     */
    typedef TA_Base_Core::NamedObject < TA_Base_Bus::ITrainTisCorba,
                                        TA_Base_Bus::ITrainTisCorba_ptr,
                                        TA_Base_Bus::ITrainTisCorba_var > ITrainTisCorbaNamedObject;

    struct TrainTisCorbaProxy::TisCorbaObject
    {
        ITrainTisCorbaNamedObject namedObject;
    };


    TrainTisCorbaProxy::TrainTisCorbaProxy( const std::string& agentName,
                                            unsigned long agentKey,
                                            unsigned long locationKey )
          : m_tisCorbaObject( new TisCorbaObject() ),
            m_subscribedToMessages( false ),
            m_trainAgentEntityName( agentName ),
            m_trainAgentEntityKey( agentKey ),
            m_trainAgentLocationKey( locationKey )
    {
        FUNCTION_ENTRY( "TrainTisCorbaProxy" );

        m_tisCorbaObject->namedObject.setCorbaName( agentName, TA_Base_Bus::TrainAgentObjectNames::TrainTisObjectName );

        FUNCTION_EXIT;
    }


    TrainTisCorbaProxy::~TrainTisCorbaProxy()
    {
        FUNCTION_ENTRY( "~TrainTisCorbaProxy" );

        unsubscribeToMessages();

        delete m_tisCorbaObject;
        m_tisCorbaObject = NULL;

        FUNCTION_EXIT;
    }


    TtisTypes::TisLibraryVersionUpdate TrainTisCorbaProxy::getLibraryVersions( CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "getLibraryVersions( TrainIdType )" );

        TA_Base_Bus::ITrainTisCorba::TisLibraryVersionUpdate_var tisLibraryVersionUpdate;
        
        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++
        // exceptions CORBA_CATCH( result = namedObject->operation( parameters ) )
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL_RETURN( tisLibraryVersionUpdate, m_tisCorbaObject->namedObject, getLibraryVersionsForTrain, ( trainId ) ) );//limin

        FUNCTION_EXIT;
        return TisTypesConversion::convertToCpp( tisLibraryVersionUpdate );
    }


    TtisTypes::TisLibraryVersionList TrainTisCorbaProxy::getLibraryVersions()
    {
        FUNCTION_ENTRY( "getLibraryVersions" );

        TA_Base_Bus::ITrainTisCorba::TisLibraryVersionList_var tisLibraryVersionList;

        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++
        CORBA_CATCH( CORBA_CALL_RETURN( tisLibraryVersionList, m_tisCorbaObject->namedObject, getLibraryVersions, () ) );

        FUNCTION_EXIT;
        return TisTypesConversion::convertToCpp( tisLibraryVersionList );
    }


    void TrainTisCorbaProxy::displayFreeTextMessage( const CommonTypes::TrainIdList& trains,
                                                     const TtisTypes::TtisFreeTextDisplayParameters& displayParameters,
                                                     time_t timestamp,
                                                     const std::string& sessionId )
    {
        FUNCTION_ENTRY( "displayFreeTextMessage" );

        // Do parameter conversion using the
        // appropriate convertToCorba methods call the corba method, catching and
        // wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_tisCorbaObject->namedObject, displayFreeTextMessage, ( CommonTypesConversion::convertToCorba( trains ),
                                                                            TisTypesConversion::convertToCorba( displayParameters ),
                                                                            timestamp,
                                                                            sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
    }


    void TrainTisCorbaProxy::displayStoredMessage( const CommonTypes::TrainIdList& trains,
                                                   const TtisTypes::TtisPredefinedDisplayParameters& displayParameters,
                                                   time_t timestamp,
                                                   const std::string& sessionId )
    {
        FUNCTION_ENTRY( "displayStoredMessage" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // Use the flexible timeout named object Do parameter conversion using the
        // appropriate convertToCorba methods call the corba method, catching and
        // wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_tisCorbaObject->namedObject, displayStoredMessage, ( CommonTypesConversion::convertToCorba( trains ),
                                                                          TisTypesConversion::convertToCorba( displayParameters ),
                                                                          timestamp,
                                                                          sessionId.c_str()  ) ) );//limin

        FUNCTION_EXIT;
    }


    void TrainTisCorbaProxy::resetMessage( const CommonTypes::TrainIdList& trains,
                                           const TtisTypes::TtisResetParameters& displayParameters,
                                           time_t timestamp,
                                           const std::string& sessionId )
    {
        FUNCTION_ENTRY( "resetMessage" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // Use the flexible timeout named object Do parameter conversion using the
        // appropriate convertToCorba methods call the corba method, catching and
        // wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_tisCorbaObject->namedObject, resetMessage, ( CommonTypesConversion::convertToCorba( trains ),
                                                                  TisTypesConversion::convertToCorba( displayParameters ),
                                                                  timestamp,
                                                                  sessionId.c_str()  ) ) );//limin

        FUNCTION_EXIT;
    }


    void TrainTisCorbaProxy::libraryDownload( const CommonTypes::TrainIdList& trains,
                                              const TtisTypes::LibraryDataType& libraryData,
                                              unsigned short libraryVersion,
                                              const std::string& sessionId )
    {
        FUNCTION_ENTRY( "libraryDownload" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // Use the flexible timeout named object Do parameter conversion using the
        // appropriate convertToCorba methods call the corba method, catching and
        // wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_tisCorbaObject->namedObject, libraryDownload, ( CommonTypesConversion::convertToCorba( trains ),
                                                                     TisTypesConversion::convertToCorba( libraryData ),
                                                                     libraryVersion,
                                                                     sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
    }


    void TrainTisCorbaProxy::libraryUpgrade( const CommonTypes::TrainIdList& trains,
                                             const std::string& sessionId )
    {
        FUNCTION_ENTRY( "libraryUpgrade" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // Use the flexible timeout named object Do parameter conversion using the
        // appropriate convertToCorba methods call the corba method, catching and
        // wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_tisCorbaObject->namedObject, libraryUpgrade, ( CommonTypesConversion::convertToCorba( trains ),
                                                                    sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
    }


    void TrainTisCorbaProxy::scheduleDownload( const CommonTypes::TrainIdList& trains,
                                               const TtisTypes::LibraryDataType& scheduleData,
                                               unsigned short scheduleVersion,
                                               const std::string& sessionId )
    {
        FUNCTION_ENTRY( "scheduleDownload" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // Use the flexible timeout named object Do parameter conversion using the
        // appropriate convertToCorba methods call the corba method, catching and
        // wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_tisCorbaObject->namedObject, scheduleDownload, ( CommonTypesConversion::convertToCorba( trains ),
                                                                      TisTypesConversion::convertToCorba( scheduleData ),
                                                                      scheduleVersion,
                                                                      sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
    }


    void TrainTisCorbaProxy::scheduleUpgrade( const CommonTypes::TrainIdList& trains,
                                              const std::string& sessionId )
    {
        FUNCTION_ENTRY( "scheduleUpgrade" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // Use the flexible timeout named object Do parameter conversion using the
        // appropriate convertToCorba methods call the corba method, catching and
        // wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_tisCorbaObject->namedObject, scheduleUpgrade, ( CommonTypesConversion::convertToCorba( trains ),
                                                                     sessionId.c_str()  ) ) );//limin

        FUNCTION_EXIT;
    }


    unsigned long TrainTisCorbaProxy::getMaximumWaitTime()
    {
        FUNCTION_ENTRY( "getMaximumWaitTime" );

        unsigned long result = 20; // 20s is the default corba timeout

        // Do parameter conversion using the
        // appropriate convertToCorba methods call the corba method, catching and
        // wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL_RETURN( result, m_tisCorbaObject->namedObject, getFlexibleTimeOutValue, () ) );//limin

        FUNCTION_EXIT;
        return result;
    }


    void TrainTisCorbaProxy::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& payload )
    {
        FUNCTION_ENTRY( "receiveSpecialisedMessage" );

        // extract the message into its corba type, use the corba
        // conversion functions to convert to the native C++ type
        TA_Base_Bus::ITrainTisCorba::TisLibraryUpdate* tisLibraryUpdate = NULL;
        TA_Base_Bus::ITrainTisCorba::TisLibraryVersionUpdate* tisLibraryVersionUpdate = NULL;
        TA_Base_Bus::ITrainTisCorba::TisReturnStatus* tisReturnStatus = NULL;

        if ( payload.messageState >>= tisLibraryUpdate )
        {
            TtisTypes::TisLibraryUpdate cppType = TisTypesConversion::convertToCpp( *tisLibraryUpdate );

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Received %s", cppType.getLogString().c_str() );

            // get the observers from the parent class (GenericObserverManager)
            GenericObserverManager<ITrainTisUpdateObserver>::ObserverList observerList = getObservers();

            // notify each observer of the message
            for ( GenericObserverManager<ITrainTisUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processTisLibraryUpdate( cppType );
            }
        }
        else if ( payload.messageState >>= tisLibraryVersionUpdate )
        {
            TtisTypes::TisLibraryVersionUpdate cppType = TisTypesConversion::convertToCpp( *tisLibraryVersionUpdate );

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Received %s", cppType.getLogString().c_str() );

            // get the observers from the parent class (GenericObserverManager)
            GenericObserverManager<ITrainTisUpdateObserver>::ObserverList observerList = getObservers();

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Sent to observer TIS Agent %s", cppType.getLogString().c_str() );

            // notify each observer of the message
            for ( GenericObserverManager<ITrainTisUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processTisLibraryVersionUpdate( cppType );
            }
        }
        else if ( payload.messageState >>= tisReturnStatus )
        {
            TtisTypes::TisReturnStatus cppType = TisTypesConversion::convertToCpp( *tisReturnStatus );

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Received %s", cppType.getLogString().c_str() );

            // get the observers from the parent class (GenericObserverManager)
            GenericObserverManager<ITrainTisUpdateObserver>::ObserverList observerList = getObservers();

            // notify each observer of the message
            for ( GenericObserverManager<ITrainTisUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processTisReturnStatus( cppType );
            }
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "Unknown message type received." );
        }

        FUNCTION_EXIT;
    }


    void TrainTisCorbaProxy::firstObserverAdded()
    {
        FUNCTION_ENTRY( "firstObserverAdded" );

        // call subscribeToMessages
        subscribeToMessages();

        FUNCTION_EXIT;
    }


    void TrainTisCorbaProxy::lastObserverRemoved()
    {
        FUNCTION_ENTRY( "lastObserverRemoved" );

        // call unsubscribeToMessages
        unsubscribeToMessages();

        FUNCTION_EXIT;
    }


    void TrainTisCorbaProxy::subscribeToMessages()
    {
        FUNCTION_ENTRY( "subscribeToMessages" );

        // subscribe to messages from the train agent if not already subscribed
        // (as per the Train Agent Message Types package)

        TA_THREADGUARD( m_observerLock );

        if ( false == m_subscribedToMessages )
        {
            // Comms Messages: TISMessage
            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
                TA_Base_Core::TrainAgentComms::TISMessage, this,
                m_trainAgentEntityKey, 0, m_trainAgentLocationKey );

            // Broadcast Comms Messages: TrainTisLibraryUpdate
            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
                TA_Base_Core::TrainAgentBroadcastComms::TrainTisLibraryUpdate, this );

            m_subscribedToMessages = true;
        }

        FUNCTION_EXIT;
    }


    void TrainTisCorbaProxy::unsubscribeToMessages()
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
