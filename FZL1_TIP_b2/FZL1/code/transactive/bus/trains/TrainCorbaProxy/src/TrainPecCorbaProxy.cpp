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

#include "bus/trains/TrainCorbaProxy/src/TrainPecCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/CommonTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/PecTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/CctvTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/ExceptionConversion.h"
#include "bus/trains/TrainAgentCorba/src/TrainAgentObjectNames.h"

#include "bus/trains/TrainAgentCorba/idl/src/ITrainPecCorba.h"

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
     * @created 22-Oct-2007 4:46:59 PM
     */
    typedef TA_Base_Core::NamedObject < TA_Base_Bus::ITrainPecCorba,
                                        TA_Base_Bus::ITrainPecCorba_ptr,
                                        TA_Base_Bus::ITrainPecCorba_var > ITrainPecCorbaNamedObject;


    /**
     * The standard named object for access to the CORBA servant.
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:59 PM
     */
    typedef TA_Base_Core::FlexibleTimeoutNamedObject < TA_Base_Bus::ITrainPecCorba,
                                                       TA_Base_Bus::ITrainPecCorba_ptr,
                                                       TA_Base_Bus::ITrainPecCorba_var > ITrainPecCorbaFlexibleNamedObject;

    struct TrainPecCorbaProxy::PecCorbaObject
    {
        ITrainPecCorbaNamedObject namedObject;
        ITrainPecCorbaFlexibleNamedObject flexibleTimeoutNamedObject;
    };


    TrainPecCorbaProxy::TrainPecCorbaProxy( const std::string& agentName,
                                            unsigned long agentKey,
                                            unsigned long locationKey )
          : m_pecCorbaObject( new PecCorbaObject() ),
            m_subscribedToMessages( false ),
            m_trainAgentEntityName( agentName ),
            m_trainAgentEntityKey( agentKey ),
            m_trainAgentLocationKey( locationKey )
    {
        FUNCTION_ENTRY( "TrainPecCorbaProxy" );

        m_pecCorbaObject->namedObject.setCorbaName( agentName, TA_Base_Bus::TrainAgentObjectNames::TrainPecObjectName );
        m_pecCorbaObject->flexibleTimeoutNamedObject.setCorbaName( agentName, TA_Base_Bus::TrainAgentObjectNames::TrainPecObjectName );

        FUNCTION_EXIT;
    }


    TrainPecCorbaProxy::~TrainPecCorbaProxy()
    {
        FUNCTION_ENTRY( "~TrainPecCorbaProxy" );

        unsubscribeToMessages();

        delete m_pecCorbaObject;
        m_pecCorbaObject = NULL;

        FUNCTION_EXIT;
    }


    PecTypes::PecUpdateList TrainPecCorbaProxy::getAllActivePec()
    {
        FUNCTION_ENTRY( "getAllActivePec" );

        TA_Base_Bus::ITrainPecCorba::PecUpdateList_var pecUpdateList;

        // Use the normal named object
        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL_RETURN( pecUpdateList, m_pecCorbaObject->namedObject, getAllActivePec, () ) );//limin

        FUNCTION_EXIT;
        return PecTypesConversion::convertToCpp( pecUpdateList );
    }


    void TrainPecCorbaProxy::answerPec( CommonTypes::TrainIdType trainId,
                                        ProtocolPecTypes::EPecCarNumber car,
                                        ProtocolPecTypes::EPecNumber pec,
                                        const std::string& sessionId )
    {
        FUNCTION_ENTRY( "answerPec" );

        // Use the flexible timeout named object
        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_pecCorbaObject->flexibleTimeoutNamedObject, answerPec, ( trainId,
                                                                              PecTypesConversion::convertToCorba( car ),
                                                                              PecTypesConversion::convertToCorba( pec ),
                                                                              sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
    }


    CctvTypes::TrainCameraSequence TrainPecCorbaProxy::activatePecCamera( CommonTypes::TrainIdType trainId,
                                                                          ProtocolPecTypes::EPecCarNumber car,
                                                                          ProtocolPecTypes::EPecNumber pec,
                                                                          const std::string& sessionId )
    {
        FUNCTION_ENTRY( "activatePecCamera" );

        TA_Base_Bus::ITrainCctvCorba::TrainCameraSequence_var trainCameraSequence;

        // Use the flexible timeout named object
        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions

        CORBA_CATCH( CORBA_CALL_RETURN( trainCameraSequence,
            m_pecCorbaObject->flexibleTimeoutNamedObject, activatePecCamera, ( trainId,
                                                                             PecTypesConversion::convertToCorba( car ),
                                                                             PecTypesConversion::convertToCorba( pec ),
                                                                             sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
        return CctvTypesConversion::convertToCpp( trainCameraSequence );
    }


    CctvTypes::TrainCameraSequence TrainPecCorbaProxy::activatePecCameraDelocalisedTrain( CommonTypes::TrainIdType trainId,
                                                                                          ProtocolPecTypes::EPecCarNumber car,
                                                                                          ProtocolPecTypes::EPecNumber pec,
                                                                                          unsigned long trainLocation,
                                                                                          const std::string& sessionId )
    {
        FUNCTION_ENTRY( "activatePecCameraDelocalisedTrain" );

        TA_Base_Bus::ITrainCctvCorba::TrainCameraSequence_var trainCameraSequence;

        // Use the flexible timeout named object
        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions

        CORBA_CATCH( CORBA_CALL_RETURN( trainCameraSequence,
            m_pecCorbaObject->flexibleTimeoutNamedObject, activatePecCameraDelocalisedTrain, ( trainId,
                                                                                             PecTypesConversion::convertToCorba( car ),
                                                                                             PecTypesConversion::convertToCorba( pec ),
                                                                                             trainLocation,
                                                                                             sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
        return CctvTypesConversion::convertToCpp( trainCameraSequence );
    }


    void TrainPecCorbaProxy::resetPec( CommonTypes::TrainIdType trainId,
                                       ProtocolPecTypes::EPecCarNumber car,
                                       ProtocolPecTypes::EPecNumber pec,
                                       const std::string& sessionId )
    {
        FUNCTION_ENTRY( "resetPec" );

        // Use the flexible timeout named object
        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_pecCorbaObject->flexibleTimeoutNamedObject, resetPec, ( trainId,
                                                                              PecTypesConversion::convertToCorba( car ),
                                                                              PecTypesConversion::convertToCorba( pec ),
                                                                              sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
    }


    void TrainPecCorbaProxy::resetAllPec( CommonTypes::TrainIdType trainId,
                                          const std::string& sessionId )
    {
        FUNCTION_ENTRY( "resetAllPec" );

        // Use the flexible timeout named object
        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_pecCorbaObject->flexibleTimeoutNamedObject, resetAllPec, ( trainId, sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
    }


    void TrainPecCorbaProxy::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& payload )
    {
        FUNCTION_ENTRY( "receiveSpecialisedMessage" );

        //extract the message into its corba type, use the corba
        //conversion functions to convert to the native C++ type
        TA_Base_Bus::ITrainPecCorba::PecUpdate* pecUpdate = NULL;

        if ( payload.messageState >>= pecUpdate )
        {
            PecTypes::PecUpdate cppType = PecTypesConversion::convertToCpp( *pecUpdate );

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Received %s", cppType.getLogString().c_str() );

            // get the observers from the parent class (GenericObserverManager)
            GenericObserverManager<ITrainPecUpdateObserver>::ObserverList observerList = getObservers();

            // notify each observer of the message
            for ( GenericObserverManager<ITrainPecUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processPecUpdate( cppType );
            }
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "Unknown message type received." );
        }

        FUNCTION_EXIT;
    }


    void TrainPecCorbaProxy::firstObserverAdded()
    {
        FUNCTION_ENTRY( "firstObserverAdded" );

        // call subscribeToMessages
        subscribeToMessages();

        FUNCTION_EXIT;
    }


    void TrainPecCorbaProxy::lastObserverRemoved()
    {
        FUNCTION_ENTRY( "lastObserverRemoved" );

        // call unsubscribeToMessages
        unsubscribeToMessages();

        FUNCTION_EXIT;
    }


    void TrainPecCorbaProxy::subscribeToMessages()
    {
        FUNCTION_ENTRY( "subscribeToMessages" );

        TA_THREADGUARD( m_observerLock );

        // subscribe to messages from the train agent if not already subscribed
        // (as per the Train Agent Message Types package)

        if ( false == m_subscribedToMessages )
        {
            //Comms Messages:PECMessage
            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
                TA_Base_Core::TrainAgentComms::PECMessage, this,
                m_trainAgentEntityKey, 0, m_trainAgentLocationKey );

            m_subscribedToMessages = true;
        }

        FUNCTION_EXIT;
    }


    void TrainPecCorbaProxy::unsubscribeToMessages()
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
