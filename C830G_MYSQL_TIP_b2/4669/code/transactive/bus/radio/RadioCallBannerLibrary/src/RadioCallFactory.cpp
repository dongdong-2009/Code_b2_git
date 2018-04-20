/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: PRIV_4669_TRAIN_ECP/CODE_4669/transactive/bus/radio/RadioCallBannerLibrary/src/RadioCallFactory.cpp $
  * @author:  oliverk
  * @version: $Revision: #3 $
  *
  * Last modification: $Date: 2009/04/23 $
  * Last modified by:  $Author: builder $
  *
  */


#include "bus/radio/RadioCallBannerLibrary/src/RadioCallFactory.h"
#include "bus/radio/RadioCallBannerLibrary/src/RadioCallEntry.h"
#include "bus/radio/RadioCallBannerLibrary/src/CallDriverCallEntry.h"
#include "bus/radio/RadioEntityAccess/src/RadioEntityAccess.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"
#include "bus/trains/TrainCommonLibrary/src/CallTypes.h"

#include "bus/banner_pages/call_page_library/src/CallEntryManager.h"
#include "bus/banner_pages/call_page_library/src/CallEntryDoesNotExistException.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/rights_library/src/RightsLibrary.h"

#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include <sstream>
#include <iomanip>


// forward declare helper functions
namespace
{
    std::string getCallCategoryAsString( TA_Base_Bus::IRadioCorbaDef::ECallCategory category );
}


namespace TA_IRS_Bus
{
    RadioCallFactory::RadioCallFactory( CallEntryManager& callManager,
                                        unsigned long locationKey )
        : m_bannerResourceId( 0 ),
          m_callManager( callManager ),
          m_consoleId( 0 ),
          m_locationKey( locationKey ),
          m_rightsLibrary( NULL )
    {
        FUNCTION_ENTRY( "RadioCallFactory" );

        initialise();

        FUNCTION_EXIT;
    }


    RadioCallFactory::~RadioCallFactory()
    {
        FUNCTION_ENTRY( "~RadioCallFactory" );

        // cancel the subscribe to message
        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages( this );

        // unsubscribe from proxy updates
        m_trainDriverCallCorbaProxy->removeObserver( this );
        m_trainInformationCorbaProxy->removeObserver( this );

        delete m_rightsLibrary;
        m_rightsLibrary = NULL;

        FUNCTION_EXIT;
    }


    RadioCallFactory::RadioAgentNamedObject& RadioCallFactory::getRadioAgent()
    {
        FUNCTION_ENTRY( "getRadioAgent" );
        FUNCTION_EXIT;
        return m_radioAgent;
    }


    void RadioCallFactory::processOccCallRequest( const CallTypes::OccCallRequest& event )
    {
        FUNCTION_ENTRY( "processOccCallRequest" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Received OCC Call [trainId: %d][timestamp: %d]",
                     event.trainId, event.timestamp );

        // check if the operator has rights
        if ( false == canPerformAction( TA_Base_Bus::aca_RADIO_MAKE_TRAIN_CALL ) )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "Operator has no OCC Call rights" );

            FUNCTION_EXIT;
            return;
        }

        TA_THREADGUARD( m_callUpdateLock );

        // find the existing OCC call if it exists
        std::string callIdString = getUnsignedLongAsString( event.trainId );
        
        bool callExists = doesCallIdExist( callIdString );

        if ( true == callExists )
        {
            updateCallDriverCallEntry( callIdString, event );
        }
        else
        {
            addCallDriverCallEntry( callIdString, event );
        }

        FUNCTION_EXIT;
    }


    void RadioCallFactory::processOccCallReset( const CallTypes::OccCallReset& event )
    {
        FUNCTION_ENTRY( "processOccCallReset" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Received OCC Call Removal [trainId: %d]",
                     event.trainId );

        TA_THREADGUARD( m_callUpdateLock );

        // find the existing OCC call if it exists
        std::string callIdString = getUnsignedLongAsString( event.trainId );

        bool callExists = doesCallIdExist( callIdString );

        if ( true == callExists )
        {
            unsigned long entryId = retrieveCallId( callIdString );

            m_callManager.deleteCallEntry( entryId, CallDriverCallEntry::getStaticType() );
            
            deleteCallId( callIdString );
        }

        FUNCTION_EXIT;
    }


    void RadioCallFactory::processTrainDetails( const TrainInformationTypes::TrainDetails& event )
    {
        FUNCTION_ENTRY( "processTrainDetails" );
        
        // update the internal map
        {
            TA_THREADGUARD( m_trainDetalsLock );
            
            m_trainDetailsStore[event.trainId] = event;
        }
        
        // see if there is a call for this train
        std::string callIdString = getUnsignedLongAsString( event.trainId );

        bool callExists = doesCallIdExist( callIdString );

        if ( true == callExists )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Received TrainDetails for OCC Call [trainId: %d][currentLocation: %d][communicatingLocation: %d]",
                     event.trainId, event.currentLocation, event.communicatingLocation );

            updateCallDriverCallEntry( callIdString, event );
        }

        FUNCTION_EXIT;
    }


    void RadioCallFactory::processTrainSelectionUpdate( const TrainInformationTypes::TrainSelectionUpdate& event )
    {
        FUNCTION_ENTRY( "processTrainSelectionUpdate" );

        // dont care about this update

        FUNCTION_EXIT;
    }


    void RadioCallFactory::processAgentOperationMode( const TrainInformationTypes::AgentOperationMode& event )
    {
        FUNCTION_ENTRY( "processAgentOperationMode" );

        // dont care about this update

        FUNCTION_EXIT;
    }


    void RadioCallFactory::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& message )
    {
        FUNCTION_ENTRY( "receiveSpecialisedMessage" );

        TA_THREADGUARD( m_callUpdateLock );

        // check for radio callstatusEvent
        TA_Base_Bus::IRadioCorbaDef::CallStatusEvent* callStatus;

	
		try	//CL-20796++: add function to catch std::exceptions.
		{
			if ( ( message.messageState >>= callStatus ) != 0 )
			{
				 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
							  "Received call status update [CallID: %d][SessionRef: %d][ConsoleKey: %d]"
							  "[ThisConsoleKey: %d][CallStatus: %s][CallCategory: %s][InvisibleToGui: %d]",
							  callStatus->callID,
							  callStatus->sessionRef,
							  callStatus->consoleKey,
							  m_consoleId,
							  RadioCallEntry::getCallStatusAsString( callStatus->callStatus ).c_str(),
							  getCallCategoryAsString( callStatus->callCategory ).c_str(),
							  callStatus->isInvisibleToGUIApps );
        
				//
				// If the notification is not addressed to this console, ignore it.
				if( callStatus->consoleKey != m_consoleId )
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
								 "Notification ConsoleKey (%lu) does not match this Console Id (%lu)",
								 callStatus->consoleKey, m_consoleId );

					FUNCTION_EXIT;
					return;
				}

				switch( callStatus->callStatus )
				{
					//
					// Ringing
					case TA_Base_Bus::IRadioCorbaDef::Ringing:
						receiveRingingNotification( callStatus->callID );
						break;
					
					//
					// Session Update
					// This indicates a re-login, all bets are off, all previous
					// entries are officially null and void
					case TA_Base_Bus::IRadioCorbaDef::SessionUpdate:
						clearAllEntries( RadioCallEntry::CALL_ENTRY_TYPE, m_callManager );
						clearAllEntries( RadioCallEntry::SDS_ENTRY_TYPE, m_callManager );
						break;

					//
					// Other
					default:
						receiveNonRingingNotification( callStatus->callID, callStatus->callCategory );
						break;
				}
			}
		}
		catch (const std::exception &ex) //CL-20796++: add function to catch std::exceptions.
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", ex.what() );
		}
		catch (...)  //CL-20796++: add function to catch std::exceptions.
		{
			LOG_EXCEPTION_CATCH(SourceInfo,"Unknown Exception","Caught an exception while receiving specialised message.");
		}

        FUNCTION_EXIT;
    }


    void RadioCallFactory::initialise()
    {
        FUNCTION_ENTRY( "initialise" );

        //
        // get the console key
        //
        m_consoleId = RadioEntityAccess::getConsoleKey();

        
        //
        // load the banner's details
        //
        TA_Base_Core::IEntityData* bannerEntity =
            TA_Base_Core::EntityAccessFactory::getInstance().getEntity( TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ) );
        
        m_bannerResourceId = bannerEntity->getKey();

        delete bannerEntity;
        bannerEntity = NULL;

        
        //
        // build the rights library
        //
        TA_Base_Bus::RightsLibraryFactory rightsLibraryFactory;
	m_rightsLibrary = rightsLibraryFactory.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL, TA_Base_Bus::RightsLibraryFactory::MANAGER);


        //
        // get the radio agent's entity data from database
        //
        TA_Base_Core::IEntityData* radioAgentEntity = RadioEntityAccess::getRadioEntity( m_locationKey );

        TA_ASSERT( NULL != radioAgentEntity, "entitydata for radio could not be retrieved" );
        
        unsigned long entityKey = radioAgentEntity->getKey();
        unsigned long subsystemKey = radioAgentEntity->getSubsystem();
        
        std::string entityName = radioAgentEntity->getName();
        std::string locationName = radioAgentEntity->getLocationName();

        delete radioAgentEntity;
        radioAgentEntity = NULL;


        //
        // populate the radio agent object
        //
        m_radioAgent.setEntityName( entityName,false );


        //
        // Create the train agent proxies
        //
        createProxies();
        

        //
        // Subscribe to updates
        //
        TA_THREADGUARD( m_callUpdateLock );

        // subscribe to Radio notifications from the Radio Agent
        // only subscribe the notification message for the class
        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
            TA_Base_Core::RadioComms::RadioCallStatus,
            this,
            entityKey,
            subsystemKey,
            m_locationKey );

        m_trainInformationCorbaProxy->addObserver( this );
        m_trainDriverCallCorbaProxy->addObserver( this );


        //
        // Load active calls from the agents
        //
        createExistingCalls();

        FUNCTION_EXIT;
    }


    void RadioCallFactory::createProxies()
    {
        FUNCTION_ENTRY("createProxies");

        std::string sessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

        // get a reference to the proxy factory
        TrainAgentProxyFactory& trainAgentProxyFactory = TrainAgentProxyFactory::getInstance();

        // initialise it in case it isnt already
        trainAgentProxyFactory.guiInitialise( sessionId );

        // get the proxy objects
        m_trainInformationCorbaProxy = trainAgentProxyFactory.createTrainInformationCorbaProxy();
        m_trainDriverCallCorbaProxy = trainAgentProxyFactory.createTrainDriverCallCorbaProxy();

        FUNCTION_EXIT;
    }


    void RadioCallFactory::createExistingCalls()
    {
        FUNCTION_ENTRY( "createExistingCalls" );


        //
        // First get all train details
        //
        try
        {
            TA_THREADGUARD( m_trainDetalsLock );
            
            TrainInformationTypes::TrainDetailsList trains = m_trainInformationCorbaProxy->getTrainList();

            for ( TrainInformationTypes::TrainDetailsList::iterator trainIter = trains.begin();
                  trainIter != trains.end(); ++trainIter )
            {
                m_trainDetailsStore[ trainIter->trainId ] = *trainIter;
            }
        }
        catch( AgentCommunicationException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "AgentCommunicationException", e.what() );
        }


        //
        // Second get the OCC Calls
        //
        try
        {
            CallTypes::OccCallRequestList occCalls = m_trainDriverCallCorbaProxy->getAllOccCallRequests();

            for ( CallTypes::OccCallRequestList::iterator callIter = occCalls.begin();
                  callIter != occCalls.end(); ++callIter )
            {
                processOccCallRequest( *callIter );
            }
        }
        catch( AgentCommunicationException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "AgentCommunicationException", e.what() );
        }


        //
        // Third, get the radio calls
        //
        try
        {
            TA_Base_Bus::IRadioCorbaDef::CallList_var calls;
            CORBA_CALL_RETURN( calls, m_radioAgent, getCallList, ( m_consoleId ) );//limin
            
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "RadioAgent->getCallList(%lu) returned %lu values",
                         m_consoleId, calls->length() );

            for ( unsigned long i = 0; i < calls->length(); ++i )
            {
                if ( ( calls[i].callStatus == TA_Base_Bus::IRadioCorbaDef::Ringing ) &&
                     ( calls[i].consoleKey == m_consoleId ) )
                {
                    receiveRingingNotification( calls[i].callID );
                }
            }
        }
        catch ( ... )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "RadioCallFactory::getExistingMessages(), Unable to get the call list from Radio Agent." );
        }

        FUNCTION_EXIT;
    }


    bool RadioCallFactory::canPerformAction( unsigned long action )
    {
        FUNCTION_ENTRY( "canPerformAction" );

        TA_ASSERT( NULL != m_rightsLibrary, "Rights library is NULL" );

        TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;

        bool canPerformAction = false;

        try
        {
            std::string reason;

            canPerformAction = m_rightsLibrary->isActionPermittedOnResource(
                    TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID ),
                    m_bannerResourceId,
                    action,
                    reason,
                    decisionModule );

            //
            // If the action couldn't be performed, log a message saying why
            //

            if ( false == canPerformAction )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "Access denied for action with id '%d' because '%s'",
                             action, reason.c_str() );
            }
        }
        catch ( const TA_Base_Core::RightsException& ex )
        {
            std::ostringstream message;
            message << "Could not determine rights for action with id '" << action
                    << "'. Exception: '" << ex.what() << "'";

            LOG_EXCEPTION_CATCH( SourceInfo, "RightsException", message.str().c_str() );
        }
        catch ( ... )
        {
            std::ostringstream message;
            message << "Could not determine rights for action with id '" << action << "'";

            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", message.str().c_str() );
        }


        //
        // If rights could not be determined (because an exception was caught)
        // access should be denied.
        //
        FUNCTION_EXIT;
        return canPerformAction;
    }


    void RadioCallFactory::receiveRingingNotification( long callId )
    {
        FUNCTION_ENTRY( "receiveRingingNotification" );

        std::string callIdStr = getUnsignedLongAsString( callId );

        try
        {
            TA_Base_Bus::IRadioCorbaDef::CallDetailsType_var details;
			CORBA_CALL_RETURN( details, m_radioAgent, getCallDetails,( callId, m_consoleId ) );

            if ( doesCallIdExist( callIdStr ) )
            {
                updateRadioCallEntry( callIdStr, details );
            }
            else
            {
                addRadioCallEntry( callIdStr, details );
            }
        }
        catch ( const TA_Base_Bus::IRadioCorbaDef::invalidCallIDException& )
        {
            std::string message = "Notification received for call that does not exist at Radio Agent. Radio call id: " + callIdStr;
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, message.c_str() );
        }

        FUNCTION_EXIT;
    }


    void RadioCallFactory::receiveNonRingingNotification( long callId,
                                                          TA_Base_Bus::IRadioCorbaDef::ECallCategory callCategory )
    {
        FUNCTION_ENTRY( "receiveNonRingingNotification" );

        //
        // Retrieve the entry id using the call id.
        //
        std::string callIdStr = getUnsignedLongAsString( callId );
        unsigned long entryId = retrieveCallId( callIdStr );

        try
        {
            std::string entryType = RadioCallEntry::CALL_ENTRY_TYPE;

            if ( callCategory == TA_Base_Bus::IRadioCorbaDef::TextMessage )
            {
                entryType = RadioCallEntry::SDS_ENTRY_TYPE;
            }

            //
            // The call banner doesn't track the call once it's stopped
            // ringing (ie it's been answered/deleted/etc) so tell the
            // CallEntryManager to delete the call entry.
            //
            m_callManager.deleteCallEntry( entryId, entryType );
        }
        catch ( const CallEntryDoesNotExistException& )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                "Notification received for call that does not exist. Radio call id: %s",
                callIdStr.c_str() );
        }

        //
        // The Radio entry has been deleted so delete the
        // entry id / call id association.
        //
        deleteCallId( callIdStr );

        FUNCTION_EXIT;
    }


    void RadioCallFactory::addRadioCallEntry( const std::string& callId,
                                              const TA_Base_Bus::IRadioCorbaDef::CallDetailsType& details )
    {
        FUNCTION_ENTRY( "addRadioCallEntry" );

        unsigned long entryId = getUniqueCallEntryId();

        ICallEntry* newEntry = new RadioCallEntry( entryId, m_consoleId, details, *this );


        //
        // Inform the CallEntryManager that a new entry has been created.
        // The CallEntryManager takes responsibility for the memory of the
        // call entry.
        //
        m_callManager.addCallEntry( newEntry );


        //
        // Save the entry id / call id association so that we can find this
        // call entry when other messages arrive for it.
        //
        saveCallId( callId, entryId );

        FUNCTION_EXIT;
    }


    void RadioCallFactory::updateRadioCallEntry( const std::string& callId,
                                                 const TA_Base_Bus::IRadioCorbaDef::CallDetailsType& details )
    {
        FUNCTION_ENTRY( "updateRadioCallEntry" );

        unsigned long entryId = retrieveCallId( callId );

        try
        {
            ICallEntry& existingEntry = m_callManager.getCallEntry( entryId );

            RadioCallEntry& radioEntry = dynamic_cast<RadioCallEntry&>( existingEntry );

            // update the call
            radioEntry.updateDetails( details );

            //refresh the display
            m_callManager.refreshCallEntry( entryId );

        }
        catch ( const CallEntryDoesNotExistException& )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                         "Updating a Call that does not exist %s",
                         callId.c_str() );

            // add it instead
            addRadioCallEntry( callId, details );
        }
        catch ( std::bad_cast& )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                         "Updating a Call that does not exist %s (bad cast)",
                         callId.c_str() );

            // add it instead
            addRadioCallEntry( callId, details );
        }
    }


    void RadioCallFactory::addCallDriverCallEntry( const std::string& callId,
                                                   const CallTypes::OccCallRequest& occCall )
    {
        FUNCTION_ENTRY( "addCallDriverCallEntry" );

        unsigned long entryId = getUniqueCallEntryId();

        // get the train details

        TrainInformationTypes::TrainDetails trainDetails;

        {
            TA_THREADGUARD( m_trainDetalsLock );
            
            TrainInformationTypes::TrainDetailsMap::iterator findIter = m_trainDetailsStore.find( occCall.trainId );

            if ( findIter != m_trainDetailsStore.end() )
            {
                trainDetails = findIter->second;
            }
            else
            {
                trainDetails.trainId = occCall.trainId;
                trainDetails.communicationValid = true; // must be to get this
                trainDetails.communicatingLocation = m_locationKey; // must be communicating with current agent
                trainDetails.primaryTsi = "";
                trainDetails.secondaryTsi = "";
                trainDetails.atsValid = false;
                trainDetails.isLocalisationValid = false;
                trainDetails.currentLocation = 0;
                trainDetails.currentCctvZone = 0;
                trainDetails.isMute = false;
                trainDetails.track = TrainInformationTypes::AtsTrackUnknown;
                
                std::ostringstream serviceNumberStream;
                serviceNumberStream << std::setw(2) << std::setfill('0')
                                    << static_cast<unsigned int>( occCall.trainId ) << "/_";
                trainDetails.serviceNumber = serviceNumberStream.str();
            }
        }

        ICallEntry* newEntry = new CallDriverCallEntry( entryId, occCall, trainDetails );

        //
        // Inform the CallEntryManager that a new entry has been created.
        // The CallEntryManager takes responsibility for the memory of the
        // call entry.
        //
        m_callManager.addCallEntry( newEntry );

        //
        // Save the entry id / call id association so that we can find this
        // call entry when other messages arrive for it.
        //
        saveCallId( callId, entryId );

        FUNCTION_EXIT;
    }


    void RadioCallFactory::updateCallDriverCallEntry( const std::string& callId,
                                                      const CallTypes::OccCallRequest& occCall )
    {
        unsigned long entryId = retrieveCallId( callId );

        try
        {
            ICallEntry& callEntry = m_callManager.getCallEntry( entryId );
            CallDriverCallEntry& calldriverEntry = dynamic_cast<CallDriverCallEntry&>( callEntry );

            // update its details
            calldriverEntry.updateDetails( occCall );

            // force the refresh
            m_callManager.refreshCallEntry( entryId );

        }
        catch ( const std::bad_cast& )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                         "Updating an OCC Call that does not exist %s (bad cast)",
                         callId.c_str() );

            addCallDriverCallEntry( callId, occCall );
        }
        catch ( const CallEntryDoesNotExistException& )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                         "Updating an OCC Call that does not exist %s",
                         callId.c_str() );

            addCallDriverCallEntry( callId, occCall );
        }
    }


    void RadioCallFactory::updateCallDriverCallEntry( const std::string& callId,
                                                      const TrainInformationTypes::TrainDetails& trainDetails )
    {
        unsigned long entryId = retrieveCallId( callId );

        try
        {
            ICallEntry& callEntry = m_callManager.getCallEntry( entryId );
            CallDriverCallEntry& calldriverEntry = dynamic_cast<CallDriverCallEntry&>( callEntry );

            // update its details
            calldriverEntry.updateDetails( trainDetails );

            // force the refresh
            m_callManager.refreshCallEntry( entryId );

        }
        catch ( const std::bad_cast& )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                         "Updating an OCC Call that does not exist %s (bad cast)",
                         callId.c_str() );
        }
        catch ( const CallEntryDoesNotExistException& )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                         "Updating an OCC Call that does not exist %s",
                         callId.c_str() );
        }
    }

}


namespace
{
    std::string getCallCategoryAsString( TA_Base_Bus::IRadioCorbaDef::ECallCategory category )
    {
        std::string categoryString;

        switch ( category )
        {

            case TA_Base_Bus::IRadioCorbaDef::HalfDuplex:
                categoryString = "HalfDuplex";
                break;

            case TA_Base_Bus::IRadioCorbaDef::FullDuplex:
                categoryString = "FullDuplex";
                break;

            case TA_Base_Bus::IRadioCorbaDef::TextMessage:
                categoryString = "TextMessage";
                break;

            case TA_Base_Bus::IRadioCorbaDef::CallDriver:
                TA_ASSERT( false, "This is deprecated and should never be sent by the agent" );
                break;

            default:
                categoryString = "Unknown";
                break;
        }

        return categoryString;
    }
}
