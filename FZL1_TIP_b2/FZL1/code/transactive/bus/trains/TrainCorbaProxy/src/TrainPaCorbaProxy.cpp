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


#include "bus/trains/TrainCorbaProxy/src/TrainPaCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/CommonTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/PaTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/ExceptionConversion.h"
#include "bus/trains/TrainAgentCorba/src/TrainAgentObjectNames.h"

#include "bus/trains/TrainAgentCorba/idl/src/ITrainPaCorba.h"

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
     * @created 22-Oct-2007 4:46:58 PM
     */
    typedef TA_Base_Core::NamedObject< TA_Base_Bus::ITrainPaCorba,
                                       TA_Base_Bus::ITrainPaCorba_ptr,
                                       TA_Base_Bus::ITrainPaCorba_var > ITrainPaCorbaNamedObject;


    struct TrainPaCorbaProxy::PaCorbaObject
    {
        ITrainPaCorbaNamedObject namedObject;
    };


    TrainPaCorbaProxy::TrainPaCorbaProxy( const std::string& agentName,
                                          unsigned long agentKey,
                                          unsigned long locationKey )
         :  m_paCorbaObject( new PaCorbaObject() ),
            m_subscribedToMessages( false ),
            m_trainAgentEntityName( agentName ),
            m_trainAgentEntityKey( agentKey ),
            m_trainAgentLocationKey( locationKey )
    {
        FUNCTION_ENTRY( "TrainPaCorbaProxy" );

        m_paCorbaObject->namedObject.setCorbaName( agentName, TA_Base_Bus::TrainAgentObjectNames::TrainPaObjectName );

        FUNCTION_EXIT;
    }


    TrainPaCorbaProxy::~TrainPaCorbaProxy()
    {
        FUNCTION_ENTRY( "~TrainPaCorbaProxy" );

        unsubscribeToMessages();

        delete m_paCorbaObject;
        m_paCorbaObject = NULL;

        FUNCTION_EXIT;
    }


    unsigned long TrainPaCorbaProxy::getCommandTimeout()
    {
        FUNCTION_ENTRY( "getCommandTimeout" );

        unsigned long result = 20; // 20s is the default corba timeout

        // Do parameter conversion using the
        // appropriate convertToCorba methods call the corba method, catching and
        // wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL_RETURN( result, m_paCorbaObject->namedObject, getCommandTimeout, () ) );//limin

        FUNCTION_EXIT;
        return result;
    }


    unsigned short TrainPaCorbaProxy::getTrainPaLibraryVersion( CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "getTrainPaLibraryVersion" );

        unsigned short paLibversion = 0;
        
        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL_RETURN( paLibversion, m_paCorbaObject->namedObject, getTrainPaLibraryVersion, ( trainId ) ) );//limin;

        FUNCTION_EXIT;
        return paLibversion;
    }


    PaTypes::TrainLibraryInfoList TrainPaCorbaProxy::getTrainPaLibraryVersionList()
    {
        FUNCTION_ENTRY( "getTrainPaLibraryVersionList" );

        TA_Base_Bus::ITrainPaCorba::TrainLibraryInfoList_var trainLibraryInfoList;

        CORBA_CATCH( CORBA_CALL_RETURN( trainLibraryInfoList, m_paCorbaObject->namedObject, getTrainPaLibraryVersionList, () ) );//limin

        FUNCTION_EXIT;
        return PaTypesConversion::convertToCpp( trainLibraryInfoList );
    }


    void TrainPaCorbaProxy::cyclicAnnouncement( const CommonTypes::TrainIdList& trainIdList,
                                                PaTypes::AnnouncementId announcementId,
                                                unsigned char messageNumber,
                                                unsigned short libraryVersion,
                                                const PaTypes::MessageSchedule& announcementPeriod,
                                                bool override,
                                                const std::string& sessionId )
    {
        FUNCTION_ENTRY( "cyclicAnnouncement" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_paCorbaObject->namedObject, cyclicAnnouncement, ( CommonTypesConversion::convertToCorba( trainIdList ),
                                                                       announcementId,
                                                                       messageNumber,
                                                                       libraryVersion,
                                                                       PaTypesConversion::convertToCorba( announcementPeriod ),
                                                                       override,
                                                                       sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
    }


    void TrainPaCorbaProxy::preRecordedAnnouncement( const CommonTypes::TrainIdList& trainIdList,
                                                     PaTypes::AnnouncementId announcementId,
                                                     unsigned char messageNumber,
                                                     unsigned short libraryVersion,
                                                     bool override,
                                                     const std::string& sessionId )
    {
        FUNCTION_ENTRY( "preRecordedAnnouncement" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_paCorbaObject->namedObject, preRecordedAnnouncement, ( CommonTypesConversion::convertToCorba( trainIdList ),
                                                                            announcementId,
                                                                            messageNumber,
                                                                            libraryVersion,
                                                                            override,
                                                                            sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
    }


    void TrainPaCorbaProxy::beginLiveAnnouncement( PaTypes::AnnouncementId announcementId,
                                                   const std::string& sessionId )
    {
        FUNCTION_ENTRY( "beginLiveAnnouncement" );

        //Do parameter conversion using the appropriate convertToCorba methods
        //call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_paCorbaObject->namedObject, beginLiveAnnouncement, ( announcementId, sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
    }


    void TrainPaCorbaProxy::setupLiveAnnouncement( const CommonTypes::TrainIdList& trainIdList,
                                                   PaTypes::AnnouncementId announcementId,
                                                   bool override,
                                                   const std::string& staticGroup,
                                                   const std::string& sessionId )
    {
        FUNCTION_ENTRY( "setupLiveAnnouncement" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_paCorbaObject->namedObject, setupLiveAnnouncement, ( CommonTypesConversion::convertToCorba( trainIdList ),
                                                                          announcementId,
                                                                          override,
                                                                          staticGroup.c_str(),
                                                                          sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
    }


    void TrainPaCorbaProxy::continueLiveAnnouncement( PaTypes::AnnouncementId announcementId,
                                                      const std::string& sessionId )
    {
        FUNCTION_ENTRY( "continueLiveAnnouncement" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_paCorbaObject->namedObject, continueLiveAnnouncement, ( announcementId, sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
    }


    void TrainPaCorbaProxy::endLiveAnnouncement( PaTypes::AnnouncementId announcementId,
                                                 const std::string& sessionId )
    {
        FUNCTION_ENTRY( "endLiveAnnouncement" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_paCorbaObject->namedObject, endLiveAnnouncement, ( announcementId, sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
    }


    void TrainPaCorbaProxy::endBroadcast( const CommonTypes::TrainIdList& trainIdList,
                                          PaTypes::AnnouncementId announcementId,
                                          const std::string& sessionId )
    {
        FUNCTION_ENTRY( "endBroadcast" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_paCorbaObject->namedObject, endBroadcast, ( CommonTypesConversion::convertToCorba( trainIdList ),
                                                                 announcementId,
                                                                 sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
    }


    void TrainPaCorbaProxy::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& payload )
    {
        FUNCTION_ENTRY( "receiveSpecialisedMessage" );

        // extract the message into its corba type, use the corba
        // conversion functions to convert to the native C++ type
        TA_Base_Bus::ITrainPaCorba::PaAnnouncementUpdate* paAnnouncementUpdate = NULL;
        TA_Base_Bus::ITrainPaCorba::PaUpdateEvent* paUpdateEvent = NULL;
        TA_Base_Bus::ITrainPaCorba::TrainLibraryInfo* trainLibraryInfo = NULL;

        if ( payload.messageState >>= paAnnouncementUpdate )
        {
            PaTypes::PaAnnouncementUpdate cppType = PaTypesConversion::convertToCpp( *paAnnouncementUpdate );

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Received %s", cppType.getLogString().c_str() );

            // get the observers from the parent class (GenericObserverManager)
            GenericObserverManager<ITrainPaUpdateObserver>::ObserverList observerList = getObservers();

            // notify each observer of the message
            for ( GenericObserverManager<ITrainPaUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processPaAnnouncementUpdate( cppType );
            }
        }
        else if ( payload.messageState >>= paUpdateEvent )
        {
            PaTypes::PaUpdateEvent cppType = PaTypesConversion::convertToCpp( *paUpdateEvent );

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Received %s", cppType.getLogString().c_str() );

            // get the observers from the parent class (GenericObserverManager)
            GenericObserverManager<ITrainPaUpdateObserver>::ObserverList observerList = getObservers();

            // notify each observer of the message
            for ( GenericObserverManager<ITrainPaUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processPaUpdateEvent( cppType );
            }
        }
        else if ( payload.messageState >>= trainLibraryInfo )
        {
            PaTypes::TrainLibraryInfo cppType = PaTypesConversion::convertToCpp( *trainLibraryInfo );

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Received %s", cppType.getLogString().c_str() );

            // get the observers from the parent class (GenericObserverManager)
            GenericObserverManager<ITrainPaUpdateObserver>::ObserverList observerList = getObservers();

            // notify each observer of the message
            for ( GenericObserverManager<ITrainPaUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processTrainLibraryInfo( cppType );
            }
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "Unknown message type received." );
        }

        FUNCTION_EXIT;
    }


    void TrainPaCorbaProxy::firstObserverAdded()
    {
        FUNCTION_ENTRY( "firstObserverAdded" );

        // call subscribeToMessages
        subscribeToMessages();

        FUNCTION_EXIT;
    }


    void TrainPaCorbaProxy::lastObserverRemoved()
    {
        FUNCTION_ENTRY( "lastObserverRemoved" );

        //c all unsubscribeToMessages
        unsubscribeToMessages();

        FUNCTION_EXIT;
    }


    void TrainPaCorbaProxy::subscribeToMessages()
    {
        FUNCTION_ENTRY( "subscribeToMessages" );

        TA_THREADGUARD( m_observerLock );

        // subscribe to messages from the train agent if not already subscribed
        // (as per the Train Agent Message Types package)

        if ( false == m_subscribedToMessages )
        {
            // Comms Messages: PAMessage
            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
                TA_Base_Core::TrainAgentComms::PAMessage, this, 
                m_trainAgentEntityKey, 0, m_trainAgentLocationKey );

            // Comms Messages: LiveAnnouncementUpdate
            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
                TA_Base_Core::TrainAgentComms::LiveAnnouncementUpdate, this,
                m_trainAgentEntityKey, 0, m_trainAgentLocationKey );

            // Broadcast Comms Messages: LiveAnnouncementUpdate
            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
                TA_Base_Core::TrainAgentBroadcastComms::TrainPaLibraryUpdate, this );

            m_subscribedToMessages = true;
        }

        FUNCTION_EXIT;
    }


    void TrainPaCorbaProxy::unsubscribeToMessages()
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
