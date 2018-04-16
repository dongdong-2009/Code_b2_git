/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $2008-01-10$
  * Last modified by:  $LiangHua Yuan$
  *
  */

#include "bus/trains/PECCallBannerLibrary/src/PECCallFactory.h"
#include "bus/trains/PECCallBannerLibrary/src/PECCallEntry.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"
#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"
#include "bus/trains/TrainCommonLibrary/src/CctvException.h"

#include "bus/banner_pages/call_page_library/src/CallEntryManager.h"
#include "bus/banner_pages/call_page_library/src/CallEntryDoesNotExistException.h"
#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"

#include "core/utilities/src/RunParams.h"

#include <iomanip>
#include <vector>
#include <set>



namespace TA_IRS_Bus
{
    PECCallFactory::PECCallFactory( CallEntryManager& callManager )
        : m_callManager( callManager ),
          m_consoleKey( 0 ),
          m_cctvAvailable( true ),
          m_radioInFallback( false ),
		  m_CallFailureObserver(NULL)
    {
        FUNCTION_ENTRY( "PECCallFactory(callManager)" );

        // populate m_pecNumberLookupTable with the static data
        m_pecNumberLookupTable[ ProtocolPecTypes::PecOne ] = "B1";  //limin TD20582
        m_pecNumberLookupTable[ ProtocolPecTypes::PecTwo ] = "A2";  //limin TD20582
        m_pecNumberLookupTable[ ProtocolPecTypes::PecThree ] = "B3";//limin TD20582
        m_pecNumberLookupTable[ ProtocolPecTypes::PecFour ] = "A4"; //limin TD20582

		// populate m_pecCarNumberLookupTable with the static data
		m_pecCarNumberLookupTable[ ProtocolPecTypes::CarOne ] = "DM1";
		m_pecCarNumberLookupTable[ ProtocolPecTypes::CarTwo ] = "T-car";
		m_pecCarNumberLookupTable[ ProtocolPecTypes::CarThree ] = "DM2";

		// populate m_pecSourceLookupTable with the static data
		m_pecTrainSourceLookupTable[ PecTypes::LocalTrain ] = "(H)";
		m_pecTrainSourceLookupTable[ PecTypes::RemoteTrain ] = "(F)";

        // get the console this banner is running on
        std::string sessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

        TA_Base_Bus::AuthenticationLibrary authenticationLibrary;
        TA_Base_Bus::SessionInfo sessionInfo = authenticationLibrary.getSessionInfo( sessionId, sessionId );
        m_consoleKey = sessionInfo.WorkstationId;


        // get the location type this banner is running at
        TA_IRS_Bus::LocationCache& locationCache = TA_IRS_Bus::LocationCache::getInstance();
        TA_Base_Core::ILocation::ELocationType locationType = locationCache.getLocationTypeByKey( locationCache.getLocationKey() );

        // if it is the depot, cctv is not available
        // all other locations have cctv
        m_cctvAvailable = ( TA_Base_Core::ILocation::DPT != locationType );

        // initialise the proxies
        createProxies( sessionId );
			
		try
		{
			m_CallFailureObserver = new TA_IRS_Bus::CallFailureObserver (&m_callManager);
		}
		catch (TA_Base_Core::TransactiveException & ex)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Excpetion Encounter during initialization of CallFailure Observer : %s",ex.what());
			m_CallFailureObserver = NULL;
		}
		

        FUNCTION_EXIT;
    }


    PECCallFactory::~PECCallFactory()
    {
        FUNCTION_ENTRY( "~PECCallFactory" );

        // unsubscribe to updates from the proxies
        m_trainPecCorbaProxy->removeObserver( this );
        m_trainInformationProxy->removeObserver( this );

        FUNCTION_EXIT;
    }


    void PECCallFactory::processAgentOperationMode( const TrainInformationTypes::AgentOperationMode& event )
    {
        FUNCTION_ENTRY( "processAgentOperationMode" );

        // update fallback status
        m_radioInFallback = event.fallbackMode;
		
		// Get all PEC call entries from the call entry manager
		std::vector< std::string > type;
		type.push_back( PECCallEntry::getStaticType());

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"AgentOperation Mode for PEC");

		std::vector< ICallEntry* > calls = m_callManager.getCallEntriesOfType( type );

		// for each entry
		for ( std::vector<ICallEntry*>::iterator callEntryIter = calls.begin();
			callEntryIter != calls.end(); ++callEntryIter )
		{
			// dynamic cast to the PECCallEntry
			PECCallEntry* callEntry = dynamic_cast< PECCallEntry* >( *callEntryIter );

			TA_IRS_Bus::TrainInformationTypes::TrainDetails trainInfo = getDetailsForTrain( callEntry->getTrainId());

			if ((event.controlledLocations.find(trainInfo.currentLocation) == 
				event.controlledLocations.end())) 
			{
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					"Process Agent Operation Mode : Restting PEC for Train :%d, Location : %d, Location for this train is not anymore in control",
					trainInfo.trainId,trainInfo.currentLocation );
				
				// this is for deactivating PEC Acceptance dialog
 				if (callEntry->isCallActive())
 				{
 					callEntry->processReset();
 				}
			}

		}

        FUNCTION_EXIT;
    }


    void PECCallFactory::processTrainSelectionUpdate( const TrainInformationTypes::TrainSelectionUpdate& event )
    {
        FUNCTION_ENTRY( "processTrainSelectionUpdate" );

        // This is not used, do nothing

        FUNCTION_EXIT;
    }


    void PECCallFactory::processTrainDetails( const TrainInformationTypes::TrainDetails& event )
    {
        FUNCTION_ENTRY( "processTrainDetails" );

        {
            // acquire the m_trainDetailsLock
            TA_THREADGUARD( m_trainDetailsLock );

            // Find the corresponding entry in m_trainDetailsStore and update it
            // If it doesnt exist, add it.
            m_trainDetailsStore[ event.trainId ] = event;

            // release the lock
        }


        // Get all PEC call entries from the call entry manager
        std::vector< std::string > type;
        type.push_back( PECCallEntry::getStaticType() );
        std::vector< ICallEntry* > calls = m_callManager.getCallEntriesOfType( type );

        // for each entry
        for ( std::vector<ICallEntry*>::iterator callEntryIter = calls.begin();
              callEntryIter != calls.end(); ++callEntryIter )
        {
            // dynamic cast to the PECCallEntry
            PECCallEntry* callEntry = dynamic_cast< PECCallEntry* >( *callEntryIter );

            // get its train id, and if it matches the train ID that was just updated
            if ( callEntry->getTrainId() == event.trainId )
            {
                // call trainDetailsChanged so it can refresh itself
                callEntry->trainDetailsChanged();

                // tell m_callManager to refresh the call entry
                m_callManager.refreshCallEntry( callEntry->getId() );
            }
        }

        FUNCTION_EXIT;
    }


    void PECCallFactory::processPecUpdate( const PecTypes::PecUpdate& event )
    {
        FUNCTION_ENTRY( "processPecUpdate" );

        // depending on updateType, call one of: the update functions

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
			"processPecUpdate: event.updateType = %d pec valididity : %d", static_cast<PecTypes::EPecUpdateType>(event.updateType),event.pec.valid);
        switch ( event.updateType )
        {

            case PecTypes::PECUpdated:
                processPecUpdated( event );
                break;
			
			case PecTypes::PECHold:
            case PecTypes::PECReset:
                processPecReset( event );
                break;

            case PecTypes::PECMpuChangeover:
                processPecMpuChangeover( event );
                break;

            case PecTypes::PECCallLost:
                processPecCallLost( event );
                break;
                
            default:
                // this should never happen if our
                // corba conversion functions are correct
                TA_ASSERT( false, "Invalid PecUpdate type" );
        }

        // refresh all PEC for this train
        std::vector< std::string > type;
        type.push_back( PECCallEntry::getStaticType() );
        std::vector< ICallEntry* > calls = m_callManager.getCallEntriesOfType( type );

        // for each entry
        for ( std::vector<ICallEntry*>::iterator callEntryIter = calls.begin();
              callEntryIter != calls.end(); ++callEntryIter )
        {
            // dynamic cast to the PECCallEntry
            PECCallEntry* callEntry = dynamic_cast< PECCallEntry* >( *callEntryIter );

            // get its train id, and if it matches the train ID that was just updated
            if ( callEntry->getTrainId() == event.trainId )
            {
                // tell m_callManager to refresh the call entry
                m_callManager.refreshCallEntry( callEntry->getId() );
            }
        }

        FUNCTION_EXIT;
    }


    bool PECCallFactory::isOperatorBusy()
    {
        FUNCTION_ENTRY( "isOperatorBusy" );

        // Using m_callManager, get all PEC calls
        std::vector< std::string > type;
        type.push_back( PECCallEntry::getStaticType() );
        std::vector< ICallEntry* > calls = m_callManager.getCallEntriesOfType( type );

        // Get the operator session ID
        std::string operatorSessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

        // iterate through each pec call 
        for ( std::vector<ICallEntry*>::iterator callEntryIter = calls.begin();
              callEntryIter != calls.end(); ++callEntryIter )
        {
            // dynamic cast to the PECCallEntry
            PECCallEntry* callEntry = dynamic_cast< PECCallEntry* >( *callEntryIter );
            
            std::string sessionId = callEntry->getSessionId();

            // compare the session ID from the given PEC call entry
            // to the operator session ID
            if ( sessionId == operatorSessionId )
            {
                // if it matches, this means the operator
                // is involved in another PEC call
                return true;
            }
        }

        FUNCTION_EXIT;

        // no PEC matched the operator's session ID, so this operator
        // has not answered a PEC at this time
        return false;
    }


    bool PECCallFactory::trainHasPecAnsweredByDriver( CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "trainHasPecAnsweredByDriver" );

        std::vector< std::string > type;
        type.push_back( PECCallEntry::getStaticType() );
        std::vector< ICallEntry* > calls = m_callManager.getCallEntriesOfType( type );

        // for each entry
        for ( std::vector<ICallEntry*>::iterator callEntryIter = calls.begin();
              callEntryIter != calls.end(); ++callEntryIter )
        {
            // dynamic cast to the PECCallEntry
            PECCallEntry* callEntry = dynamic_cast< PECCallEntry* >( *callEntryIter );
            
            // check if it is answered by the driver
            if ( PecTypes::PecSourceDriver == callEntry->getPecSource() && callEntry->getTrainId() == trainId )
            {
                FUNCTION_EXIT;
                return true;
            }
        }

        FUNCTION_EXIT;
        return false;
    }


    TrainInformationTypes::TrainDetails PECCallFactory::getDetailsForTrain( CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "getDetailsForTrain" );

        // acquire the m_trainDetailsLock
        TA_THREADGUARD( m_trainDetailsLock );

        // Find the corresponding entry in m_trainDetailsStore and return it
        TrainInformationTypes::TrainDetailsMap::iterator ite = m_trainDetailsStore.find( trainId );

        if ( ite != m_trainDetailsStore.end() )
        {
            FUNCTION_EXIT;
            return ( ite->second );
        }

        TA_ASSERT( false, "Details not found!" );

    }


    bool PECCallFactory::isRadioInFallback()
    {
        FUNCTION_ENTRY( "isRadioInFallback" );
        FUNCTION_EXIT;
        return m_radioInFallback;
    }


    std::string PECCallFactory::getPecStringForPecId( ProtocolPecTypes::EPecNumber pecNumber )
    {
        FUNCTION_ENTRY( "getPecStringForPecId" );

        FUNCTION_EXIT;
        return m_pecNumberLookupTable[ pecNumber ];
    }


	std::string PECCallFactory::getPecStringForPecCarNum( ProtocolPecTypes::EPecCarNumber carNumber )
	{
		FUNCTION_ENTRY("getPecStringForCarNo");
		FUNCTION_EXIT;

		return m_pecCarNumberLookupTable[carNumber];
	}

	
	std::string PECCallFactory::getPecStringForPecTrainSource( PecTypes::EPecTrainSource pecSource )
	{
		FUNCTION_ENTRY("getPecStringForPecSource");
		FUNCTION_EXIT;

		return m_pecTrainSourceLookupTable[pecSource];
	}


    void PECCallFactory::createProxies( const std::string& sessionId )
    {
        FUNCTION_ENTRY( "createProxies" );

        // Using the TrainAgentProxyFactory, create m_trainInformationCorbaProxy and m_trainPecCorbaProxy
        
        TrainAgentProxyFactory::getInstance().guiInitialise( sessionId );

        m_trainInformationProxy = TrainAgentProxyFactory::getInstance().createTrainInformationCorbaProxy();
        m_trainPecCorbaProxy = TrainAgentProxyFactory::getInstance().createTrainPecCorbaProxy();
        
        // Add this object as an observer to both proxy objects
        m_trainPecCorbaProxy->addObserver( this );
        m_trainInformationProxy->addObserver( this );

        try
        {
            // using m_trainInformationCorbaProxy get all the initial train details
            // acquire the train details lock and then add each one to m_trainDetailsStore
            {
                TA_THREADGUARD( m_trainDetailsLock );

                TrainInformationTypes::TrainDetailsList trainDetailsList = m_trainInformationProxy->getTrainList();

                for ( TrainInformationTypes::TrainDetailsList::iterator trainDetailsIter = trainDetailsList.begin();
                      trainDetailsIter != trainDetailsList.end(); ++trainDetailsIter )
                {
                    m_trainDetailsStore.insert( 
                        TrainInformationTypes::TrainDetailsMap::value_type( trainDetailsIter->trainId,  *trainDetailsIter ) );
                }
            }


            PecTypes::PecUpdateList activePec = m_trainPecCorbaProxy->getAllActivePec();

            for ( PecTypes::PecUpdateList::iterator pecIter = activePec.begin();
                  pecIter != activePec.end(); ++pecIter )
            {
                processPecUpdate( *pecIter );
            }

            TA_IRS_Bus::TrainInformationTypes::AgentOperationMode operationMode = m_trainInformationProxy->getAgentOperationMode();
            m_radioInFallback = operationMode.fallbackMode;
        }
        catch( TA_IRS_Bus::AgentCommunicationException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );

            // agent not contactable, updates will be sent out as it starts
        }

        FUNCTION_EXIT;
    }


    void PECCallFactory::processPecUpdated( const PecTypes::PecUpdate& event )
    {
        FUNCTION_ENTRY( "processPecUpdated" );

        std::string pecId = getPecIdAsString( event );

        // Check if the PEC exists

        if ( true == doesCallIdExist( pecId ) )
        {
            try
            {
                unsigned long callId = retrieveCallId( pecId );

                // get the call entry
                PECCallEntry& callEntry = dynamic_cast< PECCallEntry& >( m_callManager.getCallEntry( callId ) );
                
                // update the entry with the new details
                callEntry.processUpdate( event );
				
				TA_IRS_Bus::ICallEntry& callEntrySound = m_callManager.getCallEntry(callId);
				
				// call close sound anytime there is an update on 
				// existing call entry
				if (&callEntrySound)
				{
					LOG_GENERIC (SourceInfo,
								 TA_Base_Core::DebugUtil::DebugInfo,
								 "Calling Call Sound");

					m_callManager.closeSound(&callEntrySound);

					LOG_GENERIC (SourceInfo,
						TA_Base_Core::DebugUtil::DebugInfo,
						"Done Calling");
				}

                // it will be refreshed by the calling function
            }
            catch ( const CallEntryDoesNotExistException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "CallEntryDoesNotExistException", pecId.c_str() );
            }
            catch ( const std::bad_cast& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "std::bad_cast", pecId.c_str() );
            }
            catch ( ... )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "...", pecId.c_str() );
            }
        }
        // otherwise if it is new
        else
        {
            // get a new call entry id
            unsigned long callId = getUniqueCallEntryId();

            // create a new PECCallEntry object
            ICallEntry* newEntry = new PECCallEntry( callId,
                                                     event,
                                                     *this,
                                                     m_trainPecCorbaProxy );

			LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
						"Test Log : Newly added Call Entry , call id : %d session ID  %s",callId,event.sessionId.c_str());

            // add the call entry to the m_callManager
            m_callManager.addCallEntry( newEntry );

            // save the call id
            saveCallId( pecId, callId );
        }

        FUNCTION_EXIT;
    }


    void PECCallFactory::processPecReset( const PecTypes::PecUpdate& event )
    {
        FUNCTION_ENTRY( "processPecReset" );

        std::string pecId = getPecIdAsString( event );

        try
        {
            unsigned long callId = retrieveCallId( pecId );

            // get the call entry
            PECCallEntry& callEntry = dynamic_cast< PECCallEntry& >( m_callManager.getCallEntry( callId ) );

            // tell the entry to process the reset
            callEntry.processReset();

            // tell m_callManager to delete the call entry
            m_callManager.deleteCallEntry( callId, PECCallEntry::getStaticType() );
        }
        catch ( const CallEntryDoesNotExistException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "CallEntryDoesNotExistException", pecId.c_str() );
        }
        catch ( const std::bad_cast& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::bad_cast", pecId.c_str() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", pecId.c_str() );
        }

        deleteCallId( pecId );

        FUNCTION_EXIT;
    }


    void PECCallFactory::processPecMpuChangeover( const PecTypes::PecUpdate event )
    {
        FUNCTION_ENTRY( "processPecMpuChangeover" );

        std::string pecId = getPecIdAsString( event );

        try
        {
            unsigned long callId = retrieveCallId( pecId );

            // get the call entry
            PECCallEntry& callEntry = dynamic_cast< PECCallEntry& >( m_callManager.getCallEntry( callId ) );
            
            // tell the entry to process the mpu changeover
            callEntry.processMpuChangeover();

            // tell m_callManager to delete the call entry
            m_callManager.deleteCallEntry( callId, PECCallEntry::getStaticType() );
        }
        catch ( const CallEntryDoesNotExistException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "CallEntryDoesNotExistException", pecId.c_str() );
        }
        catch ( const std::bad_cast& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::bad_cast", pecId.c_str() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", pecId.c_str() );
        }

        deleteCallId( pecId );

        FUNCTION_EXIT;
    }


    void PECCallFactory::processPecCallLost( const PecTypes::PecUpdate& event )
    {
        FUNCTION_ENTRY( "processPecCallLost" );

        std::string pecId = getPecIdAsString( event );

        try
        {
            unsigned long callId = retrieveCallId( pecId );

            // get the call entry
            PECCallEntry& callEntry = dynamic_cast< PECCallEntry& >( m_callManager.getCallEntry( callId ) );

            // tell the entry to process the call loss
            callEntry.processCallLost();

            // Bohong++ for CL-20793, tell m_callManager to delete the call entry
            m_callManager.deleteCallEntry( callId, PECCallEntry::getStaticType() );
            // ++Bohong
        }
        catch ( const CallEntryDoesNotExistException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "CallEntryDoesNotExistException", pecId.c_str() );
        }
        catch ( const std::bad_cast& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::bad_cast", pecId.c_str() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", pecId.c_str() );
        }

        //Bohong++ for CL-20793
        deleteCallId( pecId );

        FUNCTION_EXIT;
    }


    std::string PECCallFactory::getPecIdAsString( const PecTypes::PecUpdate& event )
    {
        FUNCTION_ENTRY( "getPecIdAsString" );

        // build an ID from the trainId, car number, and pec number
        std::ostringstream pecIdStream;
        pecIdStream << std::setw( 2 ) << std::setfill( '0' ) << static_cast<unsigned int>( event.trainId )
                    << std::setw( 2 ) << std::setfill( '0' ) << static_cast<unsigned int>( event.pec.car )
                    << std::setw( 2 ) << std::setfill( '0' ) << static_cast<unsigned int>( event.pec.pec );

        FUNCTION_EXIT;
        return pecIdStream.str();
    }


    bool PECCallFactory::isCctvAvailable()
    {
        FUNCTION_ENTRY( "isCctvAvailable" );

        FUNCTION_EXIT;
        return m_cctvAvailable;
    }


    void PECCallFactory::addImageToAlarmStack( CommonTypes::TrainIdType trainId,
                                               unsigned long location,
                                               const CctvTypes::TrainCameraSequence& cameraList,
                                               const std::string& sessionId )
    {
        FUNCTION_ENTRY( "addImageToAlarmStack" );

        //convert the camera list into the format required by the video switch agent, and use VideoSwitchAgentFactory to acknowledgeTrainCameraAlarm
        TA_Base_Bus::SwitchManagerCorbaDef::TrainCameraSequence switchAgentCamList;
        switchAgentCamList.length( cameraList.size() );

        unsigned int index = 0;
        CctvTypes::TrainCameraSequence::const_iterator trainCameraIter = cameraList.begin();
        while ( ( trainCameraIter != cameraList.end() ) &&
                ( index < cameraList.size() ) )
        {
            switchAgentCamList[index] = static_cast< unsigned long >( *trainCameraIter );

            ++trainCameraIter;
            ++index;
        }

        try
        {
            CORBA_CALL( TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getSwitchManager(),
                acknowledgeTrainCameraAlarm, ( location, trainId, switchAgentCamList, m_consoleKey ) );//limin
        }
        // catch any exceptions and wrap them in a TrainCctvException with the appropriate error message
        catch ( TA_Base_Bus::VideoSwitchAgentException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::VideoSwitchAgentException", e.what.in() );

            TA_THROW( CctvException( e.what.in(), ProtocolCctvTypes::CctvFailed ) );
        }
        catch ( CORBA::Exception& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::VideoSwitchAgentException",
                                 TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );

            TA_THROW( CctvException( "Error communicating with CCTV system", ProtocolCctvTypes::CctvFailed ) );
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            TA_THROW( CctvException( e.what(), ProtocolCctvTypes::CctvFailed ) );
        }

        FUNCTION_EXIT;
    }
	
}
