/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/radio/RadioCallBannerLibrary/src/RadioCallFactory.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #13 $
  *
  * Last modification: $DateTime: 2016/09/15 18:33:09 $
  * Last modified by:  $Author: Noel $
  * 
  * This factory listens for Radio notifications from the
  * Radio Agent.
  *
  * If a notification of type Ringing is received, a new call entry is
  * created. Any other types of notification cause the call entry to be
  * deleted.
  *
  */



// Disable "identifier truncated" warning
#if defined(_MSC_VER)
#pragma warning(disable:4786 4250)
#endif // _MSC_VER


#include "bus/radio/RadioCallBannerLibrary/src/RadioCallFactory.h"
#include "bus/radio/RadioCallBannerLibrary/src/RadioCallEntry.h"
#include "bus/radio/RadioCallBannerLibrary/src/CallDriverCallEntry.h"
#include "bus/banner_pages/call_page_library/src/CallEntryDoesNotExistException.h"


#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/RadioEntityData.h"
#include "core/data_access_interface/entity_access/src/TrainBorneManagerEntityData.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "core/utilities/src/RunParams.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/radio/radio_agent/idl/src/CommonRadioAgentIDL.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"

#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "core/exceptions/src/ApplicationException.h"

#include <sstream>
#include <iomanip>
#include "core/data_access_interface/src/GuiAccessFactory.h"
#include "core/data_access_interface/src/IGui.h"

using TA_Base_Core::CommsMessageCorbaDef;
using TA_Base_Core::MessageSubscriptionManager;
using TA_Base_Core::SpecialisedMessageSubscriber;
using TA_Base_Core::EntityAccessFactory;
using TA_Base_Core::RadioEntityData;
using TA_Base_Core::IEntityData;
using TA_Base_Core::ThreadGuard;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::DataException;
using TA_Base_Core::RunParams;




namespace TA_IRS_Bus
{

	const char* RadioCallFactory::LAUNCH_RADIOMANAGER = "LaunchRadiomanager";
	const char* RadioCallFactory::LAUNCH_NOT_ALLOWED  = "0";

    RadioCallFactory::RadioCallFactory(
        TA_IRS_Bus::CallEntryManager& callManager,
        unsigned long locationKey,
        long radioSessionKey)
        :
        m_callManager(callManager),
        m_radioSessionKey(radioSessionKey),
        m_lastCallId(0),
		m_locationKey(locationKey)
    {
        try
        {
            initialise(locationKey);
			//Initialize RightsLibrary
			m_rightsLibrary = m_rightsLibraryFactoryObj.buildRightsLibrary();
        }
        catch(...)
        {
            //
            // Unsubscribe to messages before leaving the constructor
            //
            MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
            
            throw;
        }
    }


    void RadioCallFactory::initialise(unsigned long locationKey)
    {
		// launch radio manager
		std::string bannerName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);

		//Check Runparam if Radio Manager is allowed to launch
		std::string _lauchRadioManager = TA_Base_Core::RunParams::getInstance().get(LAUNCH_RADIOMANAGER);

		//Check if the the RadioManager is not allowed to launch on the MFT
		if ( 0 == _lauchRadioManager.compare(LAUNCH_NOT_ALLOWED) )
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"RadioManager is not allowed to launch on this MFT.");
			return;
		}

		if (bannerName.compare("LeftCallBannerPage") == 0 && 
			(!TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DISABLE_AUTOLAUNCH_RADIOMGR))) 
		{
			std::ostringstream runParams;
			runParams << "--entityName=RadioManager --debug-level=INFO --EndPointPublish --ShowWindow=0"; 


			std::string message = "Launching Radio Manager with run params '";
			message            += runParams.str();
			message            += "'";

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, message.c_str());

			using namespace TA_Base_Bus::TA_GenericGui;
			CPoint pt(GetMessagePos());
			RECT position = TA_Base_Bus::AppLauncher::getInstance().getRect(SCREEN_CURRENT,
				AREA_SCHEMATIC,
				pt.x);
			try
			{
				CWaitCursor wait;

				TA_Base_Bus::AppLauncher::getInstance().launchApplication(RADIO_CONTROLLER_GUI_APPTYPE,
					runParams.str(),
					POS_BOUNDED,
					ALIGN_FIT,
					NULL,
					&position);
			}
			catch(const TA_Base_Core::ApplicationException& ae)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "ApplicationException (Radio Manager)", ae.what());
				AfxMessageBox("Error launching Radio Manager");
			}
			catch (...)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown (Radio Manager)", "Launching Radio Manager");
				AfxMessageBox("Unknown error launching Radio Manager");
			}

		} else {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Not LeftCallBannerPage entity. Not launching Radio Manager");
		}

		int BulkPeriod = 500;
		TA_Base_Core::TrainBorneManagerEntityData* trainBorneManagerEntityData = NULL; // TrainBorneManager configuration
		TA_Base_Core::IEntityData* guiEntity;
		guiEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity( "TrainBorneManager" );
		if ( guiEntity != NULL)
		{
			trainBorneManagerEntityData = dynamic_cast<TA_Base_Core::TrainBorneManagerEntityData*>(guiEntity);
			if(trainBorneManagerEntityData != NULL)
			{
				BulkPeriod = trainBorneManagerEntityData->getBulkingPollPeriod();
			}
		}
		if (bannerName.compare("LeftCallBannerPage") == 0) {

			std::ostringstream runParams;
			runParams << "--entityName=TrainBorneManager --ShowWindow=0 --BulkPeriod=" << BulkPeriod; 


			std::string message = "Launching TrainBorne Manager with run params '";
			message            += runParams.str();
			message            += "'";

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, message.c_str());

			using namespace TA_Base_Bus::TA_GenericGui;
			CPoint pt(GetMessagePos());
			RECT position = TA_Base_Bus::AppLauncher::getInstance().getRect(SCREEN_CURRENT,
				AREA_SCHEMATIC,
				pt.x);
			try
			{
				CWaitCursor wait;

				TA_Base_Bus::AppLauncher::getInstance().launchApplication(100,
					runParams.str(),
					POS_BOUNDED,
					ALIGN_FIT,
					NULL,
					&position);
			}
			catch(const TA_Base_Core::ApplicationException& ae)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "ApplicationException (TrainBorne Manager)", ae.what());
				AfxMessageBox("Error launching TrainBorne Manager");
			}
			catch (...)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown (TrainBorne Manager)", "Launching TrainBorne Manager");
				AfxMessageBox("Unknown error launching TrainBorne Manager");
			}

		} else {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Not LeftCallBannerPage entity. Not launching TrainBorne Manager");
		}
        //
        // Get the radio agent's entity data from its type and location.
        //
		std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
		TA_Base_Core::IConsole* console = NULL;
		std::string entityName;

		try
		{ 
			console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId);
			entityName = console->getName() + "RadioMFTAgent";
			delete console;
		}
		catch(TA_Base_Core::TransactiveException& e)
		{
			std::string err = e.what();
		}

        IEntityData* entities =
            EntityAccessFactory::getInstance().getEntity(entityName);

        if(entities == NULL)
        {
            const char* msg = "There is no Radio MFTAgent at this location.";
            const char* fld = "Radio Agent";
            TA_THROW(DataException(msg, DataException::MISSING_MANDATORY, fld));
        }

        std::string locationName   = "";
        unsigned long entityKey    = 0;
        unsigned long subsystemKey = 0;
        
        try
        {
            entityKey    = entities->getKey();
            subsystemKey = entities->getSubsystem();
            locationName = entities->getLocationName();
        }
        catch(...)
        {
            delete entities;
            entities = NULL;
            throw;
        }
        delete entities;
        entities = NULL;


	    m_locationName = locationName;


		// Create the train agent proxies
		//
		createProxies();

        //
        // Acquire the notifications lock before subscribing to messages
        // so that any notifications are received after the initial batch of
        // authorisation requests are retrieved.
        //
        ThreadGuard guard(m_notificationsLock);

	    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"Location Key = %d",locationKey);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"Entity Key = %d",entityKey);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"SubsystemKey Key = %d",subsystemKey);

        //
        // Subscribe to Radio notifications from the Radio Agent
        //
        MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
            TA_Base_Core::RadioComms::RadioCallStatus,
            (SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*)(this),
            entityKey,
            subsystemKey,
            locationKey);

		m_trainInformationCorbaProxy->addObserver( this );
		m_trainDriverCallCorbaProxy->addObserver( this );
	

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
		
		//
		// Load all the train details
		//
		getAllTrainDetails();

		FUNCTION_EXIT;

	}

	void RadioCallFactory::getAllTrainDetails()
	{
		FUNCTION_ENTRY("getAllTrainDetails");
		
		//
		// Get all train details
		//
		try
		{
			TA_THREADGUARD(m_notificationsLock);

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

		FUNCTION_EXIT;
	}

	void RadioCallFactory::processOccCallRequest(const TA_IRS_Bus::CallTypes::OccCallRequest &event)
	{
		FUNCTION_ENTRY("processOccCallRequest");

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
		FUNCTION_ENTRY("processOccCallReset");

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
			
			TA_THREADGUARD(m_notificationsLock);

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
	
	void RadioCallFactory::processAgentOperationMode( const TrainInformationTypes::AgentOperationMode& event )
	{
		FUNCTION_ENTRY( "processAgentOperationMode" );

		// dont care about this update

		FUNCTION_EXIT;
	}

	void RadioCallFactory::processTrainSelectionUpdate( const TrainInformationTypes::TrainSelectionUpdate& event )
	{
		FUNCTION_ENTRY( "processTrainSelectionUpdate" );

		// dont care about this update

		FUNCTION_EXIT;
	}


	void RadioCallFactory::updateCallDriverCallEntry(const std::string &callId, const TA_IRS_Bus::CallTypes::OccCallRequest &occCall)
	{
		FUNCTION_ENTRY("updateCallDriverCallEntry");

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

		FUNCTION_EXIT;

	}

	void RadioCallFactory::updateCallDriverCallEntry( const std::string& callId, const TrainInformationTypes::TrainDetails& trainDetails )
	{
		FUNCTION_ENTRY("updateCallDriverCallEntry");

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

		FUNCTION_EXIT;
	}

	void RadioCallFactory::addCallDriverCallEntry( const std::string& callId, const CallTypes::OccCallRequest& occCall )
	{
		FUNCTION_ENTRY("addCallDriverCallEntry");

		unsigned long entryId = getUniqueCallEntryId();

		// get the train details

		TrainInformationTypes::TrainDetails trainDetails;

		{
	
			TA_THREADGUARD(m_notificationsLock);

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

		ICallEntry* newEntry = new CallDriverCallEntry( entryId, occCall, trainDetails,m_trainDriverCallCorbaProxy );

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

    RadioCallFactory::~RadioCallFactory()
    {
        //
        // Unsubscribe to messages
        //
        MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

		//unsubscribe from proxy updates
		m_trainDriverCallCorbaProxy->removeObserver(this);
		m_trainInformationCorbaProxy->removeObserver(this);
    }

	bool RadioCallFactory::canPerformAction(unsigned long action)
	{
        FUNCTION_ENTRY("canPerformAction");
		TA_Base_Core::IResource* resource = NULL;
		TA_Base_Core::IEntityData* entity = NULL;
		//TA_Base_Bus::RightsLibraryFactory rightsLibraryFactory;

        TA_ASSERT(
            m_rightsLibrary != NULL,
            "The rights library was not constructed correctly so we "
            "cannot determine rights");
		//m_rightsLibrary = rightsLibraryFactory.buildRightsLibrary();
            entity =
                TA_Base_Core::EntityAccessFactory::getInstance().getEntity(
                    RunParams::getInstance().get(RPARAM_ENTITYNAME) );
		            resource =
             TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(
                    entity->getKey() );
            m_bannerResourceId = resource->getKey();
		//tianxiang fixing call banner memory leak problem	
		try
		{
			delete entity;
			entity = NULL;

			delete resource;
			resource = NULL;
		}
		catch(...)
		{

		}
        //tianxiang fixing call banner memory leak problem
        TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        std::string reason;

        try
        {
            bool canPerformAction =
                m_rightsLibrary->isActionPermittedOnResource( 
                    RunParams::getInstance().get(RPARAM_SESSIONID),
                    m_bannerResourceId,
                    action,
                    reason,
                    decisionModule );

            //
            // If the action couldn't be performed, log a message saying why
            //
            if( !canPerformAction )
            {
                LOG_GENERIC(
                    SourceInfo, DebugUtil::DebugNorm,
                    "Access denied for action with id '%d' because '%s'",
					action, reason.c_str());
            }

            FUNCTION_EXIT;
            return canPerformAction;
        }
        catch(const TA_Base_Core::RightsException& ex)
        {
            std::ostringstream message;
            message 
                << "Could not determine rights for action with id '" << action
                << "'. Exception: '" << ex.what() << "'";

            LOG_EXCEPTION_CATCH(
                SourceInfo, "RightsException", message.str().c_str());
        }
        catch(...)
        {
            std::ostringstream message;
            message 
                << "Could not determine rights for action with id '"
                << action
                << "'";

            LOG_EXCEPTION_CATCH(
                SourceInfo, "Unknown Exception", message.str().c_str());
        }


        //
        // If rights could not be determined (because an exception was caught)
        // access should be denied.
        //
        FUNCTION_EXIT;
        return false;
    }

    void RadioCallFactory::receiveSpecialisedMessage(
        const CommsMessageCorbaDef& message)
    {
        FUNCTION_ENTRY("receiveSpecialisedMessage");

        
        //
        // Acquire the notifications lock so that other threads can acquire
        // this lock (when released) to avoid being interrupted by incoming
        // notifications.
        //
        ThreadGuard lock(m_notificationsLock);


        //
        // Make sure the message is a Radio notification
        //
        if( std::string(message.messageTypeKey) !=
            TA_Base_Core::RadioComms::RadioCallStatus.getTypeKey() )
        {
            FUNCTION_EXIT;
            return;
        }


        //
        // Extract to the Radio notification
        //
        TA_IRS_Bus::RadioMFTAgentCallbackIDL::IncomingCallEvent* notification;
        if( (message.messageState >>= notification) == 0 )
        {
            FUNCTION_EXIT;
            return;
        }

        //
        // Log a message with the details of the notification.
        //
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
			getNotificationAsString( *notification ).c_str() );

		processEvent(*notification);


        FUNCTION_EXIT;
    }


	void RadioCallFactory::processEvent(const TA_IRS_Bus::RadioMFTAgentCallbackIDL::IncomingCallEvent& event)
	{
		FUNCTION_ENTRY("processEvent");

		try
		{
			std::string callIdStr = getUnsignedLongAsString(event.caller.id);
			LOG_GENERIC(
				SourceInfo, DebugUtil::DebugInfo,"Even status: %d",event.status);

            switch (event.status)
            {
            case TA_IRS_Bus::Radio::PC_INCOMING:
                if (event.caller.id == 0)
                {
                    // don't know how to display this
                    break;
                }

				// check if call is existing
				// if yes just update
				if (doesCallIdExist(callIdStr))
				{
					updateRadioCallEntry(callIdStr, TA_IRS_Bus::Radio::PC_INCOMING);
					m_lastCallId = event.caller.id;
				}
				// if not add
				else
				{
					addRadioCallEntry(callIdStr, event);
					m_lastCallId = event.caller.id;
				}
			
                break;

			case TA_IRS_Bus::Radio::PC_NOT_INVOLVED:

				{
					LOG_GENERIC(
						SourceInfo, DebugUtil::DebugInfo,"Even caller id : %d",event.caller.id);

					if (event.caller.id == 0)
					{
						if (m_lastCallId == 0)
						{
							// don't know how to clear the call
							break;
						}

						callIdStr = getUnsignedLongAsString(event.caller.id);
					}
					//Get the entryId
					unsigned long entryId = retrieveCallId(callIdStr);

					if (doesCallIdExist(callIdStr))
					{
						updateRadioCallEntry(callIdStr, TA_IRS_Bus::Radio::PC_NOT_INVOLVED);
						m_lastCallId = event.caller.id;
					}
					break;
				}

			case TA_IRS_Bus::Radio::PC_OUTGOING:
			case TA_IRS_Bus::Radio::PC_ESTABLISHED:

				{
					LOG_GENERIC(
						SourceInfo, DebugUtil::DebugInfo,"Even caller id : %d",event.caller.id);

					if (event.caller.id == 0)
					{
						if (m_lastCallId == 0)
						{
							// don't know how to clear the call
							break;
						}

						callIdStr = getUnsignedLongAsString(event.caller.id);
					}

					//Get the entryId
					unsigned long entryId = retrieveCallId(callIdStr);
					ICallEntry& callEntry = m_callManager.getCallEntry(entryId);

					//Get the type to check whether it is a driver call
					std::string entryType = callEntry.getType();


					int compareTrain = entryType.compare(CallDriverCallEntry::DRIVER_ENTRY_TYPE);

					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
						"EntryId :%d, EntryType :%s, entryTypeCompare : %d",entryId,entryType.c_str(),compareTrain);

					//Check whether the callEntry is driver call
					if(entryType.compare(CallDriverCallEntry::DRIVER_ENTRY_TYPE) == 0)
					{

						//Get the sessionId
						std::string sessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

						LOG_GENERIC(
							SourceInfo, DebugUtil::DebugInfo,"answerOccCallRequest : callEntryId : %d, TrainID : %d, sessionId : %s",callEntry.getId(),event.caller.id,sessionId.c_str());

						m_trainDriverCallCorbaProxy->answerOccCallRequest(event.caller.id,sessionId);

					}
					else 
					{


						if (doesCallIdExist(callIdStr))
						{
							m_callManager.deleteCallEntry(entryId,RadioCallEntry::CALL_ENTRY_TYPE);
							deleteCallId(callIdStr);
						}
					}

					break;
				}
			}
		}
		catch(...)
		{
			LOG_GENERIC(
				SourceInfo, DebugUtil::DebugInfo,"Exception in processing event");
		}
		

		FUNCTION_EXIT;
	}


    void RadioCallFactory::addRadioCallEntry(
        const std::string& callId,
        const TA_IRS_Bus::RadioMFTAgentCallbackIDL::IncomingCallEvent& event)
    {
        unsigned long entryId = getUniqueCallEntryId();
        TA_IRS_Bus::ICallEntry* newEntry = new RadioCallEntry(entryId, event);
    

        //
        // Inform the CallEntryManager that a new entry has been created.
        // The CallEntryManager takes responsibility for the memory of the
        // call entry.
        //
        m_callManager.addCallEntry(newEntry);


        //
        // Save the entry id / call id association so that we can find this
        // call entry when other messages arrive for it.
        //
        saveCallId(callId, entryId);
    }


    std::string RadioCallFactory::getNotificationAsString(const TA_IRS_Bus::RadioMFTAgentCallbackIDL::IncomingCallEvent& event)
    {
        std::ostringstream asStream;
        asStream <<
            " 'Incoming radio notification (" << event.caller.id<< ")"
            " from caller " << event.caller.alias.in() << "'";

        return asStream.str();
    }

    std::string RadioCallFactory::getCallInformation(const TA_IRS_Bus::RadioMFTAgentCallbackIDL::IncomingCallEvent& event)
    {
        std::ostringstream ostr;

        if (event.isEmergencyMode)
        {
            ostr << "EMERGENCY ";
        }
		else
		{
			ostr << "Private Call";	
		}
        
        return ostr.str();
    }


    std::string RadioCallFactory::getCallSourceAsString( 
            const TA_IRS_Bus::RadioMFTAgentCallbackIDL::IncomingCallEvent& event)
    {
        std::ostringstream ostr;
        ostr << event.caller.alias.in() << " (" 
             << event.caller.ssi << ")";

        return ostr.str();
    }

	void RadioCallFactory::updateRadioCallEntry(const std::string &callId, Radio::EPrivateCallState status)
	{
		FUNCTION_ENTRY("updateCallDriverCallEntry");

		unsigned long entryId = retrieveCallId( callId );

		try
		{
			ICallEntry& callEntry = m_callManager.getCallEntry( entryId );
			RadioCallEntry& radioEntry = dynamic_cast<RadioCallEntry&>( callEntry );

			// update its details
			radioEntry.updateStatus(status);

			// force the refresh
			m_callManager.refreshCallEntry( entryId );

		}
		catch ( const std::bad_cast& )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"Updating a Radio Call that does not exist %s (bad cast)",
				callId.c_str() );
		}
		catch ( const CallEntryDoesNotExistException& )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"Updating a Radio Call that does not exist %s",
				callId.c_str() );
		}

		FUNCTION_EXIT;

	}

	unsigned long RadioCallFactory::removeRadioCallEntry(std::string callId)
	{
		FUNCTION_ENTRY("removeCallEntry");

		unsigned long entryId = 0;
		if ( doesCallIdExist(callId) ) {
			entryId = retrieveCallId(callId);
			deleteCallId(callId);

			// Inform Radio Manager
			std::ostringstream runParams;
			runParams << "--ResourceSelection=" << "ID-" << callId <<","
				<< "Type-3"; 


			std::string message = "Launching Radio Manager with run params '";
			message            += runParams.str();
			message            += "'";

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, message.c_str());

			using namespace TA_Base_Bus::TA_GenericGui;
			CPoint pt(GetMessagePos());
			RECT position = TA_Base_Bus::AppLauncher::getInstance().getRect(SCREEN_CURRENT,
				AREA_SCHEMATIC,
				pt.x);
			try
			{
				CWaitCursor wait;

				TA_Base_Bus::AppLauncher::getInstance().launchApplication(RADIO_CONTROLLER_GUI_APPTYPE,
					runParams.str(),
					POS_BOUNDED,
					ALIGN_FIT,
					NULL,
					&position);
			}
			catch(const TA_Base_Core::ApplicationException& ae)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "ApplicationException (Radio Manager)", ae.what());
				AfxMessageBox("Error launching Radio Manager");
			}
			catch (...)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown (Radio Manager)", "Launching Radio Manager");
				AfxMessageBox("Unknown error launching Radio Manager");
			}
		}

		FUNCTION_EXIT;

		return entryId;
	}

	std::string RadioCallFactory::getCallEntryStatus(std::string callId) 
	{
		FUNCTION_ENTRY("getCallEntryStatus");
		std::string status = "";
		if ( doesCallIdExist( callId ) ) 
		{
			unsigned long entryId = retrieveCallId( callId );
			ICallEntry& callEntry = m_callManager.getCallEntry( entryId );
			
			RadioCallEntry& radioEntry = dynamic_cast<RadioCallEntry&>( callEntry );
			status = radioEntry.getStatusStr();
		}

		FUNCTION_EXIT;

		return status;
	}

}// TA_Bus
