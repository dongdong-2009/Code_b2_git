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

#include "bus/trains/TrainCorbaProxy/src/TrainCctvCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/CommonTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/CctvTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/ExceptionConversion.h"
#include "bus/trains/TrainAgentCorba/src/TrainAgentObjectNames.h"

#include "bus/trains/TrainAgentCorba/idl/src/ITrainCctvCorba.h"

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
     * @created 22-Oct-2007 4:46:52 PM
     */
    typedef TA_Base_Core::NamedObject < TA_Base_Bus::ITrainCctvCorba,
                                        TA_Base_Bus::ITrainCctvCorba_ptr,
                                        TA_Base_Bus::ITrainCctvCorba_var > ITrainCctvCorbaNamedObject;


    /**
     * The standard named object for access to the CORBA servant.
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:52 PM
     */
    typedef TA_Base_Core::FlexibleTimeoutNamedObject < TA_Base_Bus::ITrainCctvCorba,
                                                       TA_Base_Bus::ITrainCctvCorba_ptr,
                                                       TA_Base_Bus::ITrainCctvCorba_var > ITrainCctvCorbaFlexibleNamedObject;

    struct TrainCctvCorbaProxy::CctvCorbaObject
    {
        ITrainCctvCorbaNamedObject namedObject;
        ITrainCctvCorbaFlexibleNamedObject flexibleTimeoutNamedObject;
    };


    TrainCctvCorbaProxy::TrainCctvCorbaProxy( const std::string& agentName,
                                              unsigned long agentKey,
                                              unsigned long locationKey )
        :  m_cctvCorbaObject( new CctvCorbaObject() ),
           m_subscribedToMessages( false ),
           m_trainAgentEntityName( agentName ),
           m_trainAgentEntityKey( agentKey ),
           m_trainAgentLocationKey( locationKey )

    {
        FUNCTION_ENTRY( "TrainCctvCorbaProxy" );

        m_cctvCorbaObject->namedObject.setCorbaName( agentName, TA_Base_Bus::TrainAgentObjectNames::TrainCctvObjectName );
        m_cctvCorbaObject->flexibleTimeoutNamedObject.setCorbaName( agentName, TA_Base_Bus::TrainAgentObjectNames::TrainCctvObjectName );

        FUNCTION_EXIT;
    }


    TrainCctvCorbaProxy::~TrainCctvCorbaProxy()
    {
        FUNCTION_ENTRY( "~TrainCctvCorbaProxy" );

        unsubscribeToMessages();

        delete m_cctvCorbaObject;
        m_cctvCorbaObject = NULL;

        FUNCTION_EXIT;
    }


    void TrainCctvCorbaProxy::setCarrierOn( CommonTypes::TrainIdType trainId,
                                            const std::string& sessionId )
    {
        FUNCTION_ENTRY( "setCarrierOn" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_cctvCorbaObject->flexibleTimeoutNamedObject, setCarrierOn, ( trainId, sessionId.c_str() ) ) );

        FUNCTION_EXIT;
    }


    void TrainCctvCorbaProxy::setCarrierOnDelocalisedTrain( CommonTypes::TrainIdType trainId,
                                                            unsigned long trainLocation,
                                                            const std::string& sessionId )
    {
        FUNCTION_ENTRY( "setCarrierOnDelocalisedTrain" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_cctvCorbaObject->flexibleTimeoutNamedObject, setCarrierOnDelocalisedTrain, ( trainId, trainLocation, sessionId.c_str() ) ) );

        FUNCTION_EXIT;
    }


    void TrainCctvCorbaProxy::setCarrierOff( CommonTypes::TrainIdType trainId,
                                             const std::string& sessionId )
    {
        FUNCTION_ENTRY( "setCarrierOff" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_cctvCorbaObject->flexibleTimeoutNamedObject, setCarrierOff, ( trainId, sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
    }


    void TrainCctvCorbaProxy::setSingleScreen( CommonTypes::TrainIdType trainId,
                                               ProtocolCctvTypes::ECctvCamera camera,
                                               const std::string& sessionId )
    {
        FUNCTION_ENTRY( "setSingleScreen" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_cctvCorbaObject->flexibleTimeoutNamedObject, setSingleScreen, ( trainId,
                                                                                     CctvTypesConversion::convertToCorba( camera ),
                                                                                     sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
    }


    void TrainCctvCorbaProxy::setQuadScreen( CommonTypes::TrainIdType trainId,
                                             ProtocolCctvTypes::ECctvCamera cameraW,
                                             ProtocolCctvTypes::ECctvCamera cameraX,
                                             ProtocolCctvTypes::ECctvCamera cameraY,
                                             ProtocolCctvTypes::ECctvCamera cameraZ,
                                             const std::string& sessionId )
    {
        FUNCTION_ENTRY( "setQuadScreen" );

        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_cctvCorbaObject->flexibleTimeoutNamedObject, setQuadScreen, ( trainId,
                                                                          CctvTypesConversion::convertToCorba( cameraW ),
                                                                          CctvTypesConversion::convertToCorba( cameraX ),
                                                                          CctvTypesConversion::convertToCorba( cameraY ),
                                                                          CctvTypesConversion::convertToCorba( cameraZ ),
                                                                          sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
    }


    void TrainCctvCorbaProxy::setSequence( CommonTypes::TrainIdType trainId,
                                           ProtocolCctvTypes::ECctvSequenceNumber sequenceNo,
                                           ProtocolCctvTypes::ECctvSequenceMode sequenceMode,
                                           const std::string& sessionId )
    {
        FUNCTION_ENTRY( "setSequence" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_cctvCorbaObject->flexibleTimeoutNamedObject, setSequence, ( trainId,
                                                                                 CctvTypesConversion::convertToCorba( sequenceNo ),
                                                                                 CctvTypesConversion::convertToCorba( sequenceMode ),
                                                                                 sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;

    }


    void TrainCctvCorbaProxy::addAlarmImage( CommonTypes::TrainIdType trainId,
                                             const CctvTypes::TrainCameraSequence& cameraIds,
                                             const std::string& sessionId )
    {
        FUNCTION_ENTRY( "addAlarmImage" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_cctvCorbaObject->flexibleTimeoutNamedObject, addAlarmImage, ( trainId,
                                                                                   CctvTypesConversion::convertToCorba( cameraIds ),
                                                                                   sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
    }


    void TrainCctvCorbaProxy::addAlarmImageDelocalisedTrain( CommonTypes::TrainIdType trainId,
                                                             const CctvTypes::TrainCameraSequence& cameraIds,
                                                             unsigned long trainLocation,
                                                             const std::string& sessionId )
    {
        FUNCTION_ENTRY( "addAlarmImageDelocalisedTrain" );

        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_cctvCorbaObject->flexibleTimeoutNamedObject, addAlarmImageDelocalisedTrain, ( trainId,
                                                                                                   CctvTypesConversion::convertToCorba( cameraIds ),
                                                                                                   trainLocation,
                                                                                                   sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
    }


    void TrainCctvCorbaProxy::removeAlarmImage( CommonTypes::TrainIdType trainId,
                                                ProtocolCctvTypes::ECctvCamera cameraId,
                                                const std::string& sessionId )
    {
        FUNCTION_ENTRY( "removeAlarmImage" );

        // Use the flexible timeout named object
        // Do parameter conversion using the appropriate convertToCorba methods
        // call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL( m_cctvCorbaObject->flexibleTimeoutNamedObject, removeAlarmImage, ( trainId,
                                                                                      CctvTypesConversion::convertToCorba( cameraId ),
                                                                                      sessionId.c_str() ) ) );//limin

        FUNCTION_EXIT;
    }


    CctvTypes::CctvSwitchState TrainCctvCorbaProxy::getActiveTrainForStage( unsigned long stageID )
    {
        FUNCTION_ENTRY( "getActiveTrainForStage" );

        TA_Base_Bus::ITrainCctvCorba::CctvSwitchState_var cctvSwitchState;

        //Do parameter conversion using the appropriate convertToCorba methods
        //Use the normal named object
        //call the corba method, catching and wrapping any CORBA exceptions in C++ exceptions
        CORBA_CATCH( CORBA_CALL_RETURN( cctvSwitchState, m_cctvCorbaObject->namedObject, getActiveTrainForStage, ( stageID ) ) );//limin

        FUNCTION_EXIT;
        return CctvTypesConversion::convertToCpp( cctvSwitchState );
    }


    void TrainCctvCorbaProxy::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& payload )
    {
        FUNCTION_ENTRY( "receiveSpecialisedMessage" );

        // extract the message into its corba type, use the corba
        // conversion functions to convert to the native C++ type
        TA_Base_Bus::ITrainCctvCorba::CctvSwitchState *cctvSwitchState = NULL;

        if ( payload.messageState >>= cctvSwitchState )
        {
            CctvTypes::CctvSwitchState cppType = CctvTypesConversion::convertToCpp( *cctvSwitchState );

            // Logs the full details of the received message
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Received %s", cppType.getLogString().c_str() );

            // get the observers from the parent class (GenericObserverManager)
            GenericObserverManager<ITrainCctvUpdateObserver>::ObserverList observerList = getObservers();

            // notify each observer of the message
            for ( GenericObserverManager<ITrainCctvUpdateObserver>::ObserverList::iterator iteObsList = observerList.begin();
                  iteObsList != observerList.end(); ++iteObsList )
            {
                (*iteObsList)->processCctvSwitchState( cppType );
            }
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                         "Unknown message type received." );
        }

        FUNCTION_EXIT;
    }


    void TrainCctvCorbaProxy::firstObserverAdded()
    {
        FUNCTION_ENTRY( "firstObserverAdded" );

        // call subscribeToMessages
        subscribeToMessages();

        FUNCTION_EXIT;
    }


    void TrainCctvCorbaProxy::lastObserverRemoved()
    {
        FUNCTION_ENTRY( "lastObserverRemoved" );

        // call unsubscribeToMessages
        unsubscribeToMessages();

        FUNCTION_EXIT;
    }


    void TrainCctvCorbaProxy::subscribeToMessages()
    {
        FUNCTION_ENTRY( "subscribeToMessages" );

        TA_THREADGUARD( m_observerLock );

        if ( false == m_subscribedToMessages )
        {
            // BroadcastCommsMessages:CctvUpdate
            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
                TA_Base_Core::TrainAgentComms::CctvUpdate, this,
                m_trainAgentEntityKey, 0, m_trainAgentLocationKey );

            m_subscribedToMessages = true;
        }

        FUNCTION_EXIT;
    }


    void TrainCctvCorbaProxy::unsubscribeToMessages()
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
