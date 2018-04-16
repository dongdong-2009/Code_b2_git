/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File$
  * @author:   Robin Ashcroft
  * @version:  $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#ifdef _MSC_VER
#pragma warning( disable : 4503 )
#endif // _MSC_VER

#include "app/signs/tis_agent/src/StatusMonitor.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/signs/tis_agent/src/STISManager.h"
#include "app/signs/tis_agent/src/Exceptions.h"
#include "app/signs/tis_agent/src/TTISManager.h"//limin
#include "bus/signs_4669/TisManagerCorbaDef/src/ISTISManagerCorbaDef.h"
#include "app/signs/tis_agent/src/AgentModeMonitor.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/exceptions/src/DataException.h"
#include "core/message/types/ATSAgentComms_MessageTypes.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryAccessFactory.h"


namespace TA_IRS_App
{

    using TA_Base_Bus::DataPoint;

    using TA_Base_Bus::DataPointState;

    using TA_Base_Bus::DpValue;



    using TA_Base_Core::DebugUtil;

    using TA_Base_Core::PredefinedMessageLibraryAccessFactory;
    using TA_Base_Core::IPredefinedMessageLibrary;


	//
    // Initialise statics.
    //

	TA_Base_Core::ReEntrantThreadLockable StatusMonitor::m_singletonLock;
	TA_Base_Core::ReEntrantThreadLockable StatusMonitor::m_atsMessageLock;

	StatusMonitor* StatusMonitor::m_theClassInstance = NULL;

	const int sendplatforminterval = 4;  // send platform info every 4 seconds.
	StatusMonitor::StatusMonitor()
      : m_shouldBeRunning( true ),
        m_atsDataInitialised( false ),
        m_currentStationMessageLibrary(NULL),
        m_currentTrainMessageLibrary(NULL),
		m_timer( TA_Base_Core::SingletonTimerUtil::getInstance() )//TD15277
	{
		// Note - the library versions above are initialised for testing purposes only
	}

	StatusMonitor::~StatusMonitor()
	{
		std::vector< DataPointDetails* >::iterator dpIter;
		for ( dpIter = m_virtualDataPointList.begin(); 
			  dpIter != m_virtualDataPointList.end(); dpIter++)
		{
			delete (*dpIter);
		}

        // clean up message libraries
        if (m_currentStationMessageLibrary != NULL)
        {
            delete m_currentStationMessageLibrary;
            m_currentStationMessageLibrary = NULL;
        }

        if (m_currentTrainMessageLibrary != NULL)
        {
            delete m_currentTrainMessageLibrary;
            m_currentTrainMessageLibrary = NULL;
        }
	}

    //
    // getInstance
    //
    StatusMonitor* StatusMonitor::getInstance()
    {
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

		// If guard acquired then make sure the singleton is still NULL.

		if ( NULL == m_theClassInstance )
		{
			// Create the one & only object.
			m_theClassInstance = new StatusMonitor();
		}

		return m_theClassInstance;
    }

	void StatusMonitor::run()
	{
		FUNCTION_ENTRY("run()");

        // load the message libraries if not already loaded
        loadMessageLibrary( "TTIS" );
        loadMessageLibrary( "STIS" );
		
		//TD15277
		//start timer to send platform infomation periodically
		m_timer.startPeriodicTimeOutClock( this, sendplatforminterval*1000, false );

		

		while( m_shouldBeRunning )
		{
			if( AgentModeMonitor::getInstance().isInControlMode() )
			{
				if( m_locationType == TA_Base_Core::ILocation::OCC )
				{
					STISManager::getInstance()->submitOCCStatusRequestMessage();
				}
				else if( m_locationType != TA_Base_Core::ILocation::DPT ) // Do not want to poll the STIS server at the depot
				{
					STISManager::getInstance()->submitStationStatusRequestMessage();
				}				
			}

			this->sleep(m_statusPollInterval);
		}

		// Set this again, so that the next call to 'run()' will work
		m_shouldBeRunning = true;

		//TD15277
		// Stop the timers
		m_timer.stopPeriodicTimeOutClock(this);
		TA_Base_Core::SingletonTimerUtil::removeInstance();
		FUNCTION_EXIT;
	}

	void StatusMonitor::terminate()
	{
		FUNCTION_ENTRY("terminate");
			
		m_shouldBeRunning = false;

		TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

		FUNCTION_EXIT;
	}

	void StatusMonitor::initialise( TA_Base_Core::TISAgentEntityDataPtr agentData,
                                    TA_Base_Core::ILocation::ELocationType locationType )
	{
		m_location = agentData->getLocationName();

        m_locationType = locationType;

		m_statusPollInterval = agentData->getStatusPollInterval();

		m_agentAssetName = agentData->getAssetName();

		// TODO - remove hardcoding of agent name
		unsigned long locationKey = STISManager::getInstance()->getLocationKey();

        TA_Base_Core::CorbaNameList atsCorbaNameList = 
            TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation(
			TA_Base_Core::AtsAgentEntityData::getStaticType(),
                                                locationKey,true);

        // no ATS agent
        if ( atsCorbaNameList.size() < 1 )
        {
            TA_THROW(TA_Base_Core::DataException("No ATS agent at this location", TA_Base_Core::DataException::NO_VALUE, "") );
        }
        // multiple TIS agents
        else if ( atsCorbaNameList.size() > 1 )
        {
            TA_THROW(TA_Base_Core::DataException("Multiple ATS agents at this location", TA_Base_Core::DataException::NOT_UNIQUE, "") );
        }

		m_atsAgent.setCorbaName( atsCorbaNameList[0] );

		// Get the current TIS-related ATS info from the ATS agent
		// Note: if this fails due to unavailability of the ATS agent etc - since the TIS agent is subscribing
		// to ATS/TIS messages, the first time a message is broadcast by the
		// ATS agent (indicating that it is now available), getInitialATSInformation() will be called 
		getInitialATSInformation();
		
		try
		{
			subscribeToNotifications();
		}
		catch ( const TA_Base_Core::DatabaseException& dbe )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", dbe.what() );
		}
		catch ( const TA_Base_Core::DataException& de )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DataException", de.what() );
		}

	}

    int StatusMonitor::getNextSTISStationLibraryVersion()
    {
		try
		{
			DpValue dpValue = getCurrentDataPointValue( NEXT_STATION_LIBRARY_VERSION_DP_NAME, STIS_SERVER_EQUIPMENT_NAME );

			return (int)dpValue.getFloat();
		}
		catch (TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
		}

		return 0;
    } 
    
    int StatusMonitor::getNextSTISTrainLibraryVersion()
    {
		try
		{
 			DpValue dpValue = getCurrentDataPointValue( NEXT_TRAIN_LIBRARY_VERSION_DP_NAME, STIS_SERVER_EQUIPMENT_NAME );

			return (int)dpValue.getFloat();
		}
		catch (TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
		}

		return 0;
    }

	int StatusMonitor::getCurrentSTISStationLibraryVersion()
	{
		try
		{
			DpValue dpValue = getCurrentDataPointValue( CURRENT_STATION_LIBRARY_VERSION_DP_NAME, STIS_SERVER_EQUIPMENT_NAME );

			return (int)dpValue.getFloat();
		}
		catch (TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
		}

		return 0;
	}

	int StatusMonitor::getCurrentSTISTrainLibraryVersion()
	{
		try
		{
			DpValue dpValue = getCurrentDataPointValue( CURRENT_TRAIN_LIBRARY_VERSION_DP_NAME, STIS_SERVER_EQUIPMENT_NAME );

			return (int)dpValue.getFloat();
		}
		catch (TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
		}

		return 0;		
	}

    int StatusMonitor::getNextISCSStationLibraryVersion()
    {
		try
		{
			DpValue dpValue = getCurrentDataPointValue( NEXT_STATION_LIBRARY_VERSION_DP_NAME, ISCS_SERVER_EQUIPMENT_NAME );

			return (int)dpValue.getFloat();
		}
		catch (TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
		}

		return 0;
    } 
    
    int StatusMonitor::getNextISCSTrainLibraryVersion()
    {
		try
		{
 			DpValue dpValue = getCurrentDataPointValue( NEXT_TRAIN_LIBRARY_VERSION_DP_NAME, ISCS_SERVER_EQUIPMENT_NAME );

			return (int)dpValue.getFloat();
		}
		catch (TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
		}

		return 0;
    }

	int StatusMonitor::getCurrentISCSStationLibraryVersion()
	{
		try
		{
			DpValue dpValue = getCurrentDataPointValue( CURRENT_STATION_LIBRARY_VERSION_DP_NAME, ISCS_SERVER_EQUIPMENT_NAME );

			return (int)dpValue.getFloat();
		}
		catch (TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
		}

		return 0;
	}

	int StatusMonitor::getCurrentISCSTrainLibraryVersion()
	{
		try
		{
			DpValue dpValue = getCurrentDataPointValue( CURRENT_TRAIN_LIBRARY_VERSION_DP_NAME, ISCS_SERVER_EQUIPMENT_NAME );

			return (int)dpValue.getFloat();
		}
		catch (TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
		}

		return 0;
	}

	int StatusMonitor::getCurrentTrainTimeScheduleVersion()
	{
		try
		{
			DpValue dpValue = getCurrentDataPointValue( CURRENT_TRAIN_TIME_SCHEDULE_VERSION_DP_NAME, ISCS_SERVER_EQUIPMENT_NAME );

			return (int)dpValue.getFloat();
		}
		catch (TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
		}

		return 0;
	}

	int StatusMonitor::getCurrentTrainTimeScheduleKey()
	{
		try
		{
			DpValue dpValue = getCurrentDataPointValue( CURRENT_TRAIN_TIME_SCHEDULE_KEY_DP_NAME, ISCS_SERVER_EQUIPMENT_NAME );

			return (int)dpValue.getFloat();
		}
		catch (TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
		}

		return 0;
	}

	bool StatusMonitor::getStationLibrariesSynchronised()
	{
		try
		{
			DpValue dpValue = getCurrentDataPointValue( STATION_LIBRARIES_SYNCHRONISED_DP_NAME, ISCS_SERVER_EQUIPMENT_NAME );

			return dpValue.getBoolean();
		}
		catch (TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
		}

		return false;
	}
	
	bool StatusMonitor::getTrainLibrariesSynchronised()
	{
		try
		{
			DpValue dpValue = getCurrentDataPointValue( TRAIN_LIBRARIES_SYNCHRONISED_DP_NAME, ISCS_SERVER_EQUIPMENT_NAME );

			return dpValue.getBoolean();
		}
		catch (TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
		}

		return false;
	}

	std::string StatusMonitor::getNextSTISLibraryFilename()
	{
		try
		{
			DpValue dpValue = getCurrentDataPointValue( NEXT_STIS_LIBRARY_FILENAME_DP_NAME, ISCS_SERVER_EQUIPMENT_NAME );

			return dpValue.getText();
		}
		catch (TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
		}

		return "";
	}

	std::string StatusMonitor::getNextTTISLibraryFilename()
	{
		try
		{
			DpValue dpValue = getCurrentDataPointValue( NEXT_TTIS_LIBRARY_FILENAME_DP_NAME, ISCS_SERVER_EQUIPMENT_NAME );

			return dpValue.getText();
		}
		catch (TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
		}

		return "";
	}

	void StatusMonitor::setNextISCSStationLibraryVersion( int nextLibraryVersion )
    {

		// If this has been called by the Control mode agent
		// as opposed to being called by the Monitor mode agent
		// responding to a Datapoint status update
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			try
			{
				//write this value to the datapoint
				DpValue dpValue = getCurrentDataPointValue( NEXT_STATION_LIBRARY_VERSION_DP_NAME, ISCS_SERVER_EQUIPMENT_NAME); 
				dpValue.setFloat ( nextLibraryVersion );  
    
				DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE );
				updateDataPointState( NEXT_STATION_LIBRARY_VERSION_DP_NAME , newState, ISCS_SERVER_EQUIPMENT_NAME, true );
			}
			catch (TA_Base_Core::DataException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
			}
		}

    }

	void StatusMonitor::setNextISCSTrainLibraryVersion( int nextLibraryVersion )
    {

		// If this has been called by the Control mode agent
		// as opposed to being called by the Monitor mode agent
		// responding to a Datapoint status update
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			try
			{
				//write this value to the datapoint
				DpValue dpValue = getCurrentDataPointValue( NEXT_TRAIN_LIBRARY_VERSION_DP_NAME, ISCS_SERVER_EQUIPMENT_NAME); 
				dpValue.setFloat ( nextLibraryVersion );  
    
				DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE );
				updateDataPointState( NEXT_TRAIN_LIBRARY_VERSION_DP_NAME , newState, ISCS_SERVER_EQUIPMENT_NAME, true );
			}
			catch (TA_Base_Core::DataException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
			}
		}
    }

	void StatusMonitor::setNextSTISStationLibraryVersion( int nextLibraryVersion )
    {

		// If this has been called by the Control mode agent
		// as opposed to being called by the Monitor mode agent
		// responding to a Datapoint status update
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			try
			{
				//write this value to the datapoint
				DpValue dpValue = getCurrentDataPointValue( NEXT_STATION_LIBRARY_VERSION_DP_NAME, STIS_SERVER_EQUIPMENT_NAME); 
				dpValue.setFloat ( nextLibraryVersion );  
    
				DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE );
				updateDataPointState( NEXT_STATION_LIBRARY_VERSION_DP_NAME , newState, STIS_SERVER_EQUIPMENT_NAME, true );
			}
			catch (TA_Base_Core::DataException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
			}
		}
    }

	void StatusMonitor::setNextSTISTrainLibraryVersion( int nextLibraryVersion )
    {

		// If this has been called by the Control mode agent
		// as opposed to being called by the Monitor mode agent
		// responding to a Datapoint status update
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			try
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "CL-21505, setNextSTISTrainLibraryVersion: %d", nextLibraryVersion );
				//write this value to the datapoint
				DpValue dpValue = getCurrentDataPointValue( NEXT_TRAIN_LIBRARY_VERSION_DP_NAME, STIS_SERVER_EQUIPMENT_NAME); 
				dpValue.setFloat ( nextLibraryVersion );  
    
				DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE );
				updateDataPointState( NEXT_TRAIN_LIBRARY_VERSION_DP_NAME , newState, STIS_SERVER_EQUIPMENT_NAME, true );
			}
			catch (TA_Base_Core::DataException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
			}
		}
    }

	void StatusMonitor::setStationLibrariesSynchronised( bool synchronised )
	{
		// stop the synchroniser thread

		// If this has been called by the Control mode agent
		// as opposed to being called by the Monitor mode agent
		// responding to a Datapoint status update
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			try
			{
				//write this value to the datapoint
				DpValue dpValue = getCurrentDataPointValue( STATION_LIBRARIES_SYNCHRONISED_DP_NAME, ISCS_SERVER_EQUIPMENT_NAME); 
				dpValue.setBoolean ( synchronised );  
    
				DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE );
				updateDataPointState( STATION_LIBRARIES_SYNCHRONISED_DP_NAME , newState, ISCS_SERVER_EQUIPMENT_NAME, true );
			}
			catch (TA_Base_Core::DataException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
			}
		}
	}

	void StatusMonitor::setTrainLibrariesSynchronised( bool synchronised )
	{

		// stop the synchroniser thread

		// If this has been called by the Control mode agent
		// as opposed to being called by the Monitor mode agent
		// responding to a Datapoint status update
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			try
			{
				//write this value to the datapoint
				DpValue dpValue = getCurrentDataPointValue( TRAIN_LIBRARIES_SYNCHRONISED_DP_NAME, ISCS_SERVER_EQUIPMENT_NAME); 
				dpValue.setBoolean ( synchronised );  
    
				DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE );
				updateDataPointState( TRAIN_LIBRARIES_SYNCHRONISED_DP_NAME , newState, ISCS_SERVER_EQUIPMENT_NAME, true );
			}
			catch (TA_Base_Core::DataException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
			}
		}
	}

	void StatusMonitor::setCurrentISCSStationLibraryVersion( int libraryVersion )
	{

		// If this has been called by the Control mode agent
		// as opposed to being called by the Monitor mode agent
		// responding to a Datapoint status update
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			try
			{
				//write this value to the datapoint
				DpValue dpValue = getCurrentDataPointValue( CURRENT_STATION_LIBRARY_VERSION_DP_NAME, ISCS_SERVER_EQUIPMENT_NAME); 
				dpValue.setFloat ( libraryVersion );  
    
				DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE );
				updateDataPointState( CURRENT_STATION_LIBRARY_VERSION_DP_NAME , newState, ISCS_SERVER_EQUIPMENT_NAME, true );
			}
			catch (TA_Base_Core::DataException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
			}
		}

		m_currentISCSStationLibraryVersion = libraryVersion;

        // ensure we have the current library
        loadMessageLibrary( "STIS" );

	}

	void StatusMonitor::setCurrentISCSTrainLibraryVersion( int libraryVersion )
	{
		// If this has been called by the Control mode agent
		// as opposed to being called by the Monitor mode agent
		// responding to a Datapoint status update
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			try
			{
				//write this value to the datapoint
				DpValue dpValue = getCurrentDataPointValue( CURRENT_TRAIN_LIBRARY_VERSION_DP_NAME, ISCS_SERVER_EQUIPMENT_NAME); 
				dpValue.setFloat ( libraryVersion );  
    
				DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE );
				updateDataPointState( CURRENT_TRAIN_LIBRARY_VERSION_DP_NAME , newState, ISCS_SERVER_EQUIPMENT_NAME, true );
                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                    "StatusMonitor::setCurrentISCSTrainLibraryVersion_central set version: %d" , libraryVersion);

                TTISManager::getInstance()->verifyAllTrainDataVersions();
			}
			catch (TA_Base_Core::DataException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
			}
		}

        // ensure we have the current library
        loadMessageLibrary( "TTIS" );
	}

	void StatusMonitor::setCurrentSTISStationLibraryVersion( int libraryVersion )
	{

		// If this has been called by the Control mode agent
		// as opposed to being called by the Monitor mode agent
		// responding to a Datapoint status update
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			try
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "CL-21505, setCurrentSTISStationLibraryVersion: %d", libraryVersion );
				//write this value to the datapoint
				DpValue dpValue = getCurrentDataPointValue( CURRENT_STATION_LIBRARY_VERSION_DP_NAME, STIS_SERVER_EQUIPMENT_NAME); 
				dpValue.setFloat ( libraryVersion );  
    
				DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE );
				updateDataPointState( CURRENT_STATION_LIBRARY_VERSION_DP_NAME , newState, STIS_SERVER_EQUIPMENT_NAME, true );
			}
			catch (TA_Base_Core::DataException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
			}
		}
	}

	void StatusMonitor::setCurrentSTISTrainLibraryVersion( int libraryVersion )
	{
		// If this has been called by the Control mode agent
		// as opposed to being called by the Monitor mode agent
		// responding to a Datapoint status update
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			try
			{
				//write this value to the datapoint
				DpValue dpValue = getCurrentDataPointValue( CURRENT_TRAIN_LIBRARY_VERSION_DP_NAME, STIS_SERVER_EQUIPMENT_NAME); 
				dpValue.setFloat ( libraryVersion );  
    
				DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE );
				updateDataPointState( CURRENT_TRAIN_LIBRARY_VERSION_DP_NAME , newState, STIS_SERVER_EQUIPMENT_NAME, true );
			}
			catch (TA_Base_Core::DataException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
			}
		}
	}

	void StatusMonitor::setNextSTISLibraryFilename( const std::string& filename )
	{
		// If this has been called by the Control mode agent
		// as opposed to being called by the Monitor mode agent
		// responding to a Datapoint status update
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			try
			{
				//write this value to the datapoint
				DpValue dpValue = getCurrentDataPointValue( NEXT_STIS_LIBRARY_FILENAME_DP_NAME, ISCS_SERVER_EQUIPMENT_NAME); 
				dpValue.setText( filename );  
    
				DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE );
				updateDataPointState( NEXT_STIS_LIBRARY_FILENAME_DP_NAME , newState, ISCS_SERVER_EQUIPMENT_NAME, false );
			}
			catch (TA_Base_Core::DataException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
			}
		}
	}

	void StatusMonitor::setNextTTISLibraryFilename( const std::string& filename )
	{
		// If this has been called by the Control mode agent
		// as opposed to being called by the Monitor mode agent
		// responding to a Datapoint status update
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			try
			{
				//write this value to the datapoint
				DpValue dpValue = getCurrentDataPointValue( NEXT_TTIS_LIBRARY_FILENAME_DP_NAME, ISCS_SERVER_EQUIPMENT_NAME); 
				dpValue.setText( filename );  
    
				DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE );
				updateDataPointState( NEXT_TTIS_LIBRARY_FILENAME_DP_NAME , newState, ISCS_SERVER_EQUIPMENT_NAME, false );
			}
			catch (TA_Base_Core::DataException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
			}
		}
	}

	void StatusMonitor::setCurrentTrainTimeScheduleVersion( int version )
	{
		// If this has been called by the Control mode agent
		// as opposed to being called by the Monitor mode agent
		// responding to a Datapoint status update
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			try
			{
				//write this value to the datapoint
				DpValue dpValue = getCurrentDataPointValue( CURRENT_TRAIN_TIME_SCHEDULE_VERSION_DP_NAME, ISCS_SERVER_EQUIPMENT_NAME); 
				dpValue.setFloat( version );  
    
				DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE );
				updateDataPointState( CURRENT_TRAIN_TIME_SCHEDULE_VERSION_DP_NAME , newState, ISCS_SERVER_EQUIPMENT_NAME, true );
                TTISManager::getInstance()->verifyAllTrainDataVersions();
			}
			catch (TA_Base_Core::DataException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
			}
		}
	}

	void StatusMonitor::setCurrentTrainTimeScheduleKey( int key )
	{
		// If this has been called by the Control mode agent
		// as opposed to being called by the Monitor mode agent
		// responding to a Datapoint status update
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			try
			{
				//write this value to the datapoint
				DpValue dpValue = getCurrentDataPointValue( CURRENT_TRAIN_TIME_SCHEDULE_KEY_DP_NAME, ISCS_SERVER_EQUIPMENT_NAME); 
				dpValue.setFloat( key );  
    
				DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE );
				updateDataPointState( CURRENT_TRAIN_TIME_SCHEDULE_KEY_DP_NAME , newState, ISCS_SERVER_EQUIPMENT_NAME, true );
			}
			catch (TA_Base_Core::DataException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
			}
		}
	}


	void StatusMonitor::addDataPoint( DataPointDetails* theDataPoint )
	{
		LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Added datapoint to Status Monitor");
		m_virtualDataPointList.push_back( theDataPoint );
	}

	void StatusMonitor::setDataPointsToControlMode()
	{
		std::vector< DataPointDetails* >::iterator dpIter;
		for ( dpIter = m_virtualDataPointList.begin(); 
			  dpIter != m_virtualDataPointList.end(); dpIter++)
		{
			// Set this point to control mode
			try
			{
				const_cast<DataPoint*>((*dpIter)->dataPoint)->setToControlMode();
				if( ( (*dpIter)->address.compare("ISCS") == 0 ) && ((*dpIter)->dataPointName.find("Version") != std::string::npos) )
				{
					std::string sessionID = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );
					try
					{
						((*dpIter)->dataPoint)->SetIsForcedState(sessionID, true);
					}
					catch( ... )
					{
						std::ostringstream os;
						os << "Set ";
						os << (*dpIter)->dataPointName;
						os << " manuallydressed failed.";
						LOG_EXCEPTION_CATCH(SourceInfo, os.str().c_str(), "");
					}
				}
			}
			catch( ... )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "The Status Monitor caught an exception while attempting to switch a datapoint to Control Mode");
			}
		}


	}

	void StatusMonitor::setDataPointsToMonitorMode()
	{
		std::vector< DataPointDetails* >::iterator dpIter;
		for ( dpIter = m_virtualDataPointList.begin(); 
			  dpIter != m_virtualDataPointList.end(); dpIter++)
		{
			// Set this point to monitor mode
			try
			{
				const_cast<DataPoint*>((*dpIter)->dataPoint)->setToMonitorMode();
			}
			catch( ... )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "The Status Monitor caught an exception while attempting to switch a datapoint to Monitor Mode");
			}
		}


	}
		
	void StatusMonitor::updateDataPointState( const std::string& dpName, DataPointState& dataPointState, const std::string& equipmentName, bool manuallyDressed )
	{
		// Find the resource by the supplied data point name
		
		std::vector< DataPointDetails* >::iterator iter;
		for( iter = m_virtualDataPointList.begin() ; iter != m_virtualDataPointList.end() ; iter++ )
		{
			if( ((*iter)->dataPointName.compare(dpName) == 0 ) && ( (*iter)->shortPIDName.compare( equipmentName ) == 0 ))
			{
				if( dpName.compare((*iter)->dataPointName) == 0 )
				{
					try
					{
						if( manuallyDressed )
						{
							const_cast<DataPoint*>((*iter)->dataPoint)->updateForcedState( dataPointState );
						}
						else
						{
							const_cast<DataPoint*>((*iter)->dataPoint)->updateFieldState( dataPointState );
						}
					}
					catch( ... )
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Status Monitor caught an unknown exception while attempting to update a datapoint state");
					}
				}
			}
		}
	}

	void StatusMonitor::setBadDataPointQuality()
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Status Monitor setting quality of datapoints to bad");

		std::vector< DataPointDetails* >::iterator iter;
		for( iter = m_virtualDataPointList.begin() ; iter != m_virtualDataPointList.end() ; iter++ )
		{
			if( ( (*iter)->dataPointName.compare(CONNECTION_LINK_SUMMARY_ALARM_DP_NAME) == 0 ) || 
					((*iter)->dataPointName.compare(CLOCK_LINK_SUMMARY_ALARM_DP_NAME) == 0 ) ||
					((*iter)->dataPointName.compare(RXH_LINK_SUMMARY_ALARM_DP_NAME) == 0 ) ||
					((*iter)->dataPointName.compare(LED_DISPLAY_SUMMARY_ALARM_DP_NAME) == 0 ) ||
					((*iter)->dataPointName.compare(PLASMA_DISPLAY_SUMMARY_ALARM_DP_NAME) == 0 ) ||
					((*iter)->dataPointName.compare(TIS_OCC_SERVER_STATUS_DP_NAME) == 0 ) )
			{
                DataPointState dataPointState( (*iter)->dataPoint->getCurrentValue(), time(NULL), TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE );
				const_cast<DataPoint*>((*iter)->dataPoint)->updateFieldState( dataPointState );
			}
		}
	}

	DpValue StatusMonitor::getCurrentDataPointValue( const std::string& dpName, const std::string& equipmentName )
	{
		std::vector< DataPointDetails* >::iterator iter;
		for( iter = m_virtualDataPointList.begin() ; iter != m_virtualDataPointList.end() ; iter++ )
		{
			if( ((*iter)->dataPointName.compare( dpName ) == 0 ) && ( (*iter)->shortPIDName.compare( equipmentName ) == 0 ))
			{
				try
				{
					DpValue dpValue = const_cast<DataPoint*>((*iter)->dataPoint)->getCurrentValue();
					return dpValue;
				}
				catch( ... )
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Status Monitor caught an unknown exception while attempting to get a datapoint value");
				}
			}
		}
		
		std::ostringstream msg;
		msg << "Missing data point: " << equipmentName << " " << dpName;
		TA_THROW(TA_Base_Core::DataException(msg.str().c_str(), TA_Base_Core::DataException::MISSING_MANDATORY, dpName.c_str()));
	}

	void StatusMonitor::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& message )
	{
		TA_Base_Core::ThreadGuard guard( m_atsMessageLock );

		// if the message is the right type
		if( std::string(message.messageTypeKey) ==
			TA_Base_Core::ATSAgentComms::AtsTisUpdate.getTypeKey() )
		{
			// Only process this message if the ATS train/platform data
			// has already been successfully obtained from the ATS agent
			if( m_atsDataInitialised )
			{
				TA_IRS_Bus::IAtsTisCorbaDef::AtsTisMessageCorbaDef* atsTisMessage;
				
				//
				// Extract to the AtsTisMessage
				//
				if( ( message.messageState >>= atsTisMessage ) != 0 )
				{
                    // if this is the OCC, send all messages on to the hardware
                    // if not, then make sure the message is for the current location before
                    // sending it to the hardware
					//TD15277
					processAtsMessage(*atsTisMessage);			
					
				}
				else
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
								"Could not extract AtsTisMessageCorbaDef from CommsMessageCorbaDef");
					return;
				} 
			}
			else // Otherwise attempt to obtain the ATS train/platform info for the first time (the agent is now obviously alive)
			{
				// Get the current TIS-related ATS info from the ATS agent
				getInitialATSInformation();
			}
		}


	}

	void StatusMonitor::subscribeToNotifications()
	{
		//
		// Get the entity, subsystem and location key.
		// There should only be a single entity at a given location.
		//
		unsigned long locationKey = STISManager::getInstance()->getLocationKey();

		TA_Base_Core::IEntityDataList entityData =
			TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(
			TA_Base_Core::AtsAgentEntityData::getStaticType(), locationKey );


		if( entityData.size() != 1)
		{
			const char* msg = "There is no ATS agent at this location.";
			const char* fld = "ATS Agent";
			TA_THROW(TA_Base_Core::DataException(msg, TA_Base_Core::DataException::MISSING_MANDATORY, fld));
		}

		unsigned long entityKey    = entityData[0]->getKey();
		unsigned long subsystemKey = entityData[0]->getSubsystem();

		delete entityData[0];

		TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
			TA_Base_Core::ATSAgentComms::AtsTisUpdate,
			this,
			entityKey,
			subsystemKey,
			locationKey);

	}


	void StatusMonitor::getInitialATSInformation()
	{

		try
		{
			//TD15277
			TA_IRS_Bus::IAtsTisCorbaDef::TisInfoList_var list ;
			CORBA_CALL_RETURN( list,
			                   m_atsAgent,
			                   getTisInformationList,
			                   () );
            int i ;				
            
			for( i = 0; i < list->length(); ++i )
            {
                processAtsMessage( list[i] );				
            }

			m_atsDataInitialised = true;

		}
		catch ( const TA_Base_Core::ObjectResolutionException& ore )
		{
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException", ore.what() );
		}
        catch ( const TA_Base_Core::OperationModeException& ome )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", ome.reason.in() );
		}
        catch ( const TA_Base_Core::NotAvailableException& nae )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::NotAvailableException", nae.reason.in() );
		}
		catch ( const CORBA::Exception& ce )
		{
			std::string exceptionMsg = TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ce);

            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", exceptionMsg.c_str() );
		}
	}
	//TD15277++
	void StatusMonitor::processAtsMessage( const TA_IRS_Bus::IAtsTisCorbaDef::AtsTisMessageCorbaDef& atsTisMessage )
    {
        // if this is the OCC, send all messages on to the hardware
        // if not, then make sure the message is for the current location before
        // sending it to the hardware
		TA_Base_Core::ThreadGuard guard( m_atsListAccessLock );
		if( (TA_Base_Core::ILocation::OCC == STISManager::getInstance()->getLocationType()) ||
            (atsTisMessage.stationId == STISManager::getInstance()->getLocationKey()) )
		{
    		std::vector< AtsTisData >::iterator tisIter;
			for ( tisIter = m_atsInfoList.begin();  tisIter != m_atsInfoList.end(); ++tisIter)
			{			
				// If the platform is found, update with the information just received
				if( (atsTisMessage.stationId == tisIter->stationId ) && 
					(atsTisMessage.platformId == tisIter->platformId ) ) //&&
				//	(atsTisMessage->message._d() == tisIter->message._d()) )
				{
                    break;
				}		
			}

            // if not found, add a new entry
			if( tisIter == m_atsInfoList.end() )
			{
				// create a new entry in m_atsInfoList
                // set all the fields to blank

				AtsTisData atstisitem;
				
				atstisitem.stationId = atsTisMessage.stationId;
				atstisitem.platformId = atsTisMessage.platformId;
				atstisitem.tisPlatformId = atsTisMessage.tisPlatformId;
				
				if( atsTisMessage.message._d() == TA_IRS_Bus::IAtsTisCorbaDef::PLATFORM_INFORMATION)
				{
					
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Received a Platform information update message from ATS agent");
					
                    // set the platform info
					atstisitem.platformInformation = atsTisMessage.message.platformInformation();

                    // set all the fields to 0 in train info
                   
					atstisitem.trainInformation.firstTrain.valid = false;
					atstisitem.trainInformation.firstTrain.physicalTrainNumber = 0;
					atstisitem.trainInformation.firstTrain.serviceNumber = 0;
					atstisitem.trainInformation.firstTrain.destinationStationId = 0;
					atstisitem.trainInformation.firstTrain.lastTrain = false;
					atstisitem.trainInformation.firstTrain.predictedTime.year = 0;
					atstisitem.trainInformation.firstTrain.predictedTime.month = 0;
					atstisitem.trainInformation.firstTrain.predictedTime.day = 0;
					atstisitem.trainInformation.firstTrain.predictedTime.hour = 0;
					atstisitem.trainInformation.firstTrain.predictedTime.minute = 0;
					atstisitem.trainInformation.firstTrain.predictedTime.second = 0;
					atstisitem.trainInformation.firstTrain.stationType = false;

					atstisitem.trainInformation.secondTrain.valid = false;
					atstisitem.trainInformation.secondTrain.physicalTrainNumber = 0;
					atstisitem.trainInformation.secondTrain.serviceNumber = 0;
					atstisitem.trainInformation.secondTrain.destinationStationId = 0;
					atstisitem.trainInformation.secondTrain.lastTrain = false;
					atstisitem.trainInformation.secondTrain.predictedTime.year = 0;
					atstisitem.trainInformation.secondTrain.predictedTime.month = 0;
					atstisitem.trainInformation.secondTrain.predictedTime.day = 0;
					atstisitem.trainInformation.secondTrain.predictedTime.hour = 0;
					atstisitem.trainInformation.secondTrain.predictedTime.minute = 0;
					atstisitem.trainInformation.secondTrain.predictedTime.second = 0;
					atstisitem.trainInformation.secondTrain.stationType = false;

					
                }
				else if ( atsTisMessage.message._d() == TA_IRS_Bus::IAtsTisCorbaDef::TRAIN_INFORMATION)
				{
					
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Received a Train information update message from ATS agent");
					
                    // set the train info
					atstisitem.trainInformation = atsTisMessage.message.trainInformation(); 

                    // set the fields to 0 in platform info
                    
					atstisitem.platformInformation.preArrival = false;  
					atstisitem.platformInformation.arrival  = false;   
					atstisitem.platformInformation.preDeparture = false;
					atstisitem.platformInformation.departure = false;  
					atstisitem.platformInformation.skip = false;
					atstisitem.platformInformation.hold = false;
					atstisitem.platformInformation.evacuation = false;
					atstisitem.platformInformation.trainOverrun = false;
					atstisitem.platformInformation.validity = false; 

    			}			
				
				m_atsInfoList.push_back(atstisitem);
              
			}
            else
            {
			    // if found, should update the corresponding item in m_atsInfoList
                switch( atsTisMessage.message._d() )
			    {
			    case TA_IRS_Bus::IAtsTisCorbaDef::PLATFORM_INFORMATION:

				    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Received a Platform information update message from ATS agent");

				    tisIter->platformInformation = atsTisMessage.message.platformInformation() ;

	   			    break;

			    case TA_IRS_Bus::IAtsTisCorbaDef::TRAIN_INFORMATION:

				    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Received a Train information update message from ATS agent");

				    tisIter->trainInformation = atsTisMessage.message.trainInformation() ; 

				    break;

			    }
            }
		}
	}
//++TD15277
    
    std::string StatusMonitor::getStationPredefinedMessageText( TA_Base_Core::ELibrarySection librarySection,
                                                                unsigned short messageTag)
    {
        // ensure we have the current library
        loadMessageLibrary( "STIS" );

        return getPredefinedMessageText( m_currentStationMessageLibrary, librarySection, messageTag );
    }
     
    
    std::string StatusMonitor::getTrainPredefinedMessageText( TA_Base_Core::ELibrarySection librarySection,
                                                              unsigned short messageTag)
    {
        // ensure we have the current library
        loadMessageLibrary( "TTIS" );

        return getPredefinedMessageText( m_currentTrainMessageLibrary, librarySection, messageTag );
    }


    void StatusMonitor::loadMessageLibrary( std::string type )
    {
        TA_Base_Core::ThreadGuard guard( m_messageLibraryLock );

        bool loadNeeded = false;
        int newVersion = 0;

        // check the loaded version against the current version
        if ( type == "STIS" )
        {
            newVersion = getCurrentISCSStationLibraryVersion_central();

            // if not loaded or a different version is loaded
            if ( (m_currentStationMessageLibrary == NULL) ||
                 (m_currentStationMessageLibrary->getVersion() != newVersion) )
            {
                loadNeeded = true;
            }

        }
        else if ( type == "TTIS" )
        {
            newVersion = getCurrentISCSTrainLibraryVersion_central();

            // if not loaded or a different version is loaded
            if ( (m_currentTrainMessageLibrary == NULL) ||
                 (m_currentTrainMessageLibrary->getVersion() != newVersion) )
            {
                loadNeeded = true;
            }
        }

        // load the new if necessary
        if (loadNeeded)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Loading the current version (%d) of the %s message library",
                newVersion, type.c_str() );

            try
            {
                IPredefinedMessageLibrary* newLibrary =
                    PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessageLibrary( newVersion, type );

                // do the switcheroo and clean up the old library
                IPredefinedMessageLibrary* oldLibrary = NULL;
                
                if ( type == "STIS" )
                {
                    oldLibrary = m_currentStationMessageLibrary;
                    m_currentStationMessageLibrary = newLibrary;

                }
                else if ( type == "TTIS" )
                {
                    oldLibrary = m_currentTrainMessageLibrary;
                    m_currentTrainMessageLibrary = newLibrary;
                }

                if (oldLibrary != NULL)
                {
                    delete oldLibrary;
 oldLibrary = NULL;
                }
            }
            catch(TA_Base_Core::TransactiveException& te)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what() );
            }
        }
    }


    std::string StatusMonitor::getPredefinedMessageText( TA_Base_Core::IPredefinedMessageLibrary* messageLibrary,
                                                         TA_Base_Core::ELibrarySection librarySection,
                                                         unsigned short messageTag )
    {
        TA_Base_Core::ThreadGuard guard( m_messageLibraryLock );

        // get the message text from the message library
        if (messageLibrary != NULL)
        {
            TA_Base_Core::PredefinedMessage* message = 
                messageLibrary->getMessage( librarySection, messageTag);

            if (message != NULL)
            {
                return message->message;
            }
            else
            {
                // log the failure to find the message
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Unable to find message with tag %d, section %d in library %s",
                    messageTag, librarySection, messageLibrary->getLibraryType().c_str() );
            }
        }

        // return an empty string on failure
        return "";
    }


    TA_Base_Core::TTISLedAttributes StatusMonitor::getDefaultTTISLedAttributes()
    {
        TA_Base_Core::ThreadGuard guard( m_messageLibraryLock );

        TA_Base_Core::TTISLedAttributes attribs;
        // get the message text from the message library
        if (m_currentTrainMessageLibrary != NULL)
        {
            attribs = m_currentTrainMessageLibrary->getDefaultTTISLedAttributes();
        }

        // return an empty string on failure
        return attribs;
    }
//TD15277++
	void StatusMonitor::timerExpired(long timerId, void* userData)
	{	

		std::vector< AtsTisData > atsInfoList;
		{
			TA_Base_Core::ThreadGuard guard( m_atsListAccessLock );
			atsInfoList = m_atsInfoList;
		}
		std::vector< AtsTisData >::iterator tisIter;

		for ( tisIter = atsInfoList.begin();  tisIter != atsInfoList.end(); ++tisIter)		
		{
			try
			{
				// Send an ATS info message to the STIS hardware
				if (tisIter->platformInformation.validity)
				{	
					STISManager::getInstance()->submitTrainInformationMessage( tisIter->stationId, tisIter->tisPlatformId, tisIter->platformInformation, tisIter->trainInformation );					
				}
				else
				{
//					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "ATS Platform Information message not sent - Platform validity is false");
				}
			}
			catch( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& ex )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "ATS Platform Information message not sent - not connected to the STIS server");
			}
			catch( const TA_IRS_Core::TISAgentException& ex )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
					"ATS Platform Information message not sent - %s", ex.what() );
			}
			catch( ... )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
					"ATS Platform Information message not sent - unknown exception"  );
			}
		}

	}
//++TD15277
	
	//hongran++ TD17500
	void StatusMonitor::setNextISCSStationLibraryVersion_central( int nextVersionNumber_central )
	{
		// If this has been called by the Control mode agent
		// as opposed to being called by the Monitor mode agent
		// responding to a Datapoint status update
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			try
			{
				//write this value to the datapoint
				DpValue dpValue = getCurrentDataPointValue( CENTRAL_NEXT_STATION_LIBRARY_VERSION_DP_NAME , ISCS_SERVER_EQUIPMENT_NAME); 
				dpValue.setFloat ( nextVersionNumber_central );  
    
				DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE );
				updateDataPointState( CENTRAL_NEXT_STATION_LIBRARY_VERSION_DP_NAME  , newState, ISCS_SERVER_EQUIPMENT_NAME, true );

				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
					"StatusMonitor::setNextISCSStationLibraryVersion_central set version: %d" , nextVersionNumber_central);
			}
			catch (TA_Base_Core::DataException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
			}
		}

	}

	void StatusMonitor::setNextISCSTrainLibraryVersion_central( int nextVersionNumber_central )
	{
		// If this has been called by the Control mode agent
		// as opposed to being called by the Monitor mode agent
		// responding to a Datapoint status update
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			try
			{
				//write this value to the datapoint
				DpValue dpValue = getCurrentDataPointValue( CENTRAL_NEXT_TRAIN_LIBRARY_VERSION_DP_NAME , ISCS_SERVER_EQUIPMENT_NAME); 
				dpValue.setFloat ( nextVersionNumber_central );  
    
				DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE );
				updateDataPointState( CENTRAL_NEXT_TRAIN_LIBRARY_VERSION_DP_NAME , newState, ISCS_SERVER_EQUIPMENT_NAME, true );
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
					"StatusMonitor::setNextISCSTrainLibraryVersion_central set version: %d" , nextVersionNumber_central);
			}
			catch (TA_Base_Core::DataException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
			}
		}

	}

	void StatusMonitor::setCurrentISCSStationLibraryVersion_central( int libraryVersion_central )
	{
		// If this has been called by the Control mode agent
		// as opposed to being called by the Monitor mode agent
		// responding to a Datapoint status update
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			try
			{
				//write this value to the datapoint
				DpValue dpValue = getCurrentDataPointValue( CENTRAL_CURRENT_STATION_LIBRARY_VERSION_DP_NAME , ISCS_SERVER_EQUIPMENT_NAME); 
				dpValue.setFloat ( libraryVersion_central );  
    
				DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE );
				updateDataPointState( CENTRAL_CURRENT_STATION_LIBRARY_VERSION_DP_NAME , newState, ISCS_SERVER_EQUIPMENT_NAME, true );
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
					"StatusMonitor::setCurrentISCSStationLibraryVersion_central set version: %d" , libraryVersion_central);
			}
			catch (TA_Base_Core::DataException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
			}
		}
	}

	void StatusMonitor::setCurrentISCSTrainLibraryVersion_central( int libraryVersion_central )
	{
		// If this has been called by the Control mode agent
		// as opposed to being called by the Monitor mode agent
		// responding to a Datapoint status update
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			try
			{
				//write this value to the datapoint
				DpValue dpValue = getCurrentDataPointValue( CENTRAL_CURRENT_TRAIN_LIBRARY_VERSION_DP_NAME , ISCS_SERVER_EQUIPMENT_NAME); 
				dpValue.setFloat ( libraryVersion_central );  
    
				DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE );
				updateDataPointState( CENTRAL_CURRENT_TRAIN_LIBRARY_VERSION_DP_NAME , newState, ISCS_SERVER_EQUIPMENT_NAME, true );
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
					"StatusMonitor::setCurrentISCSTrainLibraryVersion_central set version: %d" , libraryVersion_central);
			}
			catch (TA_Base_Core::DataException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
			}
		}
	}

    int StatusMonitor::getNextISCSStationLibraryVersion_central()
	{
		try
		{
			DpValue dpValue = getCurrentDataPointValue( CENTRAL_NEXT_STATION_LIBRARY_VERSION_DP_NAME , ISCS_SERVER_EQUIPMENT_NAME );

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
				"StatusMonitor::getNextISCSStationLibraryVersion_central return version: %d" , (int)dpValue.getFloat());

			return (int)dpValue.getFloat();
		}
		catch (TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
		}

		return 0;
	}

    int StatusMonitor::getNextISCSTrainLibraryVersion_central()
	{
		try
		{
			DpValue dpValue = getCurrentDataPointValue( CENTRAL_NEXT_TRAIN_LIBRARY_VERSION_DP_NAME , ISCS_SERVER_EQUIPMENT_NAME );
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
				"StatusMonitor::getNextISCSTrainLibraryVersion_central return version: %d" , (int)dpValue.getFloat());

			return (int)dpValue.getFloat();
		}
		catch (TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
		}

		return 0;
	}

	int StatusMonitor::getCurrentISCSStationLibraryVersion_central()
	{
		try
		{
			DpValue dpValue = getCurrentDataPointValue( CENTRAL_CURRENT_STATION_LIBRARY_VERSION_DP_NAME , ISCS_SERVER_EQUIPMENT_NAME );
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
				"StatusMonitor::getCurrentISCSStationLibraryVersion_central return version: %d" , (int)dpValue.getFloat());

			return (int)dpValue.getFloat();
		}
		catch (TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
		}

		return 0;
	}

	int StatusMonitor::getCurrentISCSTrainLibraryVersion_central()
	{
		try
		{
			DpValue dpValue = getCurrentDataPointValue( CENTRAL_CURRENT_TRAIN_LIBRARY_VERSION_DP_NAME , ISCS_SERVER_EQUIPMENT_NAME );
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
				"StatusMonitor::getCurrentISCSTrainLibraryVersion_central return version: %d" , (int)dpValue.getFloat());

			return (int)dpValue.getFloat();
		}
		catch (TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
		}

		return 0;

	}
	//++hongran TD17500


}  // namespace TA_IRS_App
