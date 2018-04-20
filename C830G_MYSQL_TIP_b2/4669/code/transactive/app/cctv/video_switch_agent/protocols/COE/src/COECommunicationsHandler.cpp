/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsHandler.cpp $
  * @author:   Rob Young
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2010/08/03 14:18:50 $
  * Last modified by:  $Author: builder $
  *
  * Handles the protocol dependent sections of the operations for a COECommunicationsHandler
  * using the COE protocol.
  * This interface is used by the COECommunicationsHandler class. Before the methods
  * in this interface are called, the user privileges have been checked and
  * that the protocol supports the required features.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 4503)
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/COE/src/TriggeringEventCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsHandler.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsChannel.h"
#include "app/cctv/video_switch_agent/src/AddressHelper.h"
#include "app/cctv/video_switch_agent/src/SwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/src/IAgentRequestCommand.h"
#include "app/cctv/video_switch_agent/protocols/src/IHardwareNotificationCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/ReceivedDataProcessor.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/StateValidationCycle.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/TriggeringEventAckCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/TriggeringEventClearCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/TrainCameraAlarmAckCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/SetStageSwitchModeCommand.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/VideoMonitor.h"
#include "core/data_access_interface/entity_access/src/BVSStage.h"
#include "core/data_access_interface/video_switch_agent/src/VideoTriggeringEventAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/types/MessageTypes.h"
#include "core/message/types/VisualAlarms_MessageTypes.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/alarm/src/NonUniqueAlarmHelper.h"
#include "core/alarm/src/AlarmConstants.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
// TD 15200
#include "core/data_access_interface/entity_access/src/EntityHelper.h"


#include <algorithm>

namespace TA_IRS_App
{
    using TA_Base_Bus::DataPoint;
    using TA_Base_Bus::DpValue;
    using TA_Base_Bus::DataPointState;


    const unsigned short COECommunicationsHandler::NO_ACTIVE_SWITCH_CYCLE_ALLOWANCE = 3;
    const std::string COECommunicationsHandler::VIDEO_SWITCH_TYPE = "VideoSwitch";
    
    const unsigned short COECommunicationsHandler::SYSTEM_DATAPOINT_ID = 7;
    const unsigned short COECommunicationsHandler::TRANSMISSION_DATAPOINT_ID = 6;
    const unsigned short COECommunicationsHandler::CAMERA_DATAPOINT_ID = 5;
    const unsigned short COECommunicationsHandler::ANCILLARY_DATAPOINT_ID = 4;
	//TD19444
	const unsigned short COECommunicationsHandler::DVR_DATAPOINT_ID = 3;	
	//TD19444

	// TD 15200
 	static const std::string INTRUDER_ALM = "CET - Intruder Alm";
	static const std::string INTERCOM_ACT_ALM = "CET - Intercom Act Alm";
	static const std::string BLS_ACT_ALM = "CET - BLS Act Alm";
	static const std::string ESP_ALM = "CET - ESP";
	static const std::string PAT_ALM = "CET - PAT";
	// TD 15200

    //
    // COECommunicationsHandler
    //
    COECommunicationsHandler::COECommunicationsHandler( TA_Base_Core::VideoSwitchAgentPtr videoSwitchAgentData )
        : m_videoSwitchAgentData( videoSwitchAgentData ),
          m_switchConnectTimeout( 0 ),
          m_switchPollInterval( 0 ),
          m_maxSwitchResponseTime( 0 ),
          m_lowPriorityStateValidationCycle( 0 ),
          m_highPriorityStateValidationCycle( 0 ),
          m_activeSwitchChannelIndex( 0 ),
          m_exceededNoActiveSwitchCycleAllowance( false ),
          m_doTerminate( false ), 
          m_processingToBeDoneSemaphore( 0 ),
          m_receivedDataProcessor( std::auto_ptr< ReceivedDataProcessor >( new ReceivedDataProcessor() ) )
          //m_alarmHelper( std::auto_ptr< TA_Base_Core::AlarmHelper >( TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper() ) )
    {
        try
        {
            // Gather comms related parameters from Agent Entity Config
            m_switchConnectTimeout = m_videoSwitchAgentData->getSwitchConnectTimeout();
            m_maxSwitchResponseTime = m_videoSwitchAgentData->getMaxSwitchResponseTime();
            m_switchPollInterval = m_videoSwitchAgentData->getSwitchPollInterval();
            m_lowPriorityStateValidationCycle = m_videoSwitchAgentData->getLowPriorityStateValidationCycle();
            m_highPriorityStateValidationCycle = m_videoSwitchAgentData->getHighPriorityStateValidationCycle();
			//close all previous triggering event alarms, we dont know whats valid anymore
			
			TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(
				TA_Base_Core::VisualAlarms::CctvTriggeringAlarm, m_videoSwitchAgentData->getKey(), m_videoSwitchAgentData->getLocation());
			
			m_stationIdToLocationMap = TA_IRS_Core::VideoStationIdAccessFactory::getInstance().getStationIdToLocationKeyMap();
			m_locationToStationIdMap = TA_IRS_Core::VideoStationIdAccessFactory::getInstance().getLocationKeyToStationIdMap(m_stationIdToLocationMap);
			
			//need to create a map of console keys to videoMonitor Ids if we are the OCC
			m_locationType = TA_Base_Core::LocationAccessFactory::getInstance().getLocationType(videoSwitchAgentData->getLocationName());
			
			TA_Base_Core::IEntityDataList videoMonitorEntities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::VideoMonitor::getStaticType(), videoSwitchAgentData->getLocation());
			TA_Base_Core::IEntityDataList::iterator videoMonitorEntitiesIt;
			
			
			for(videoMonitorEntitiesIt = videoMonitorEntities.begin(); videoMonitorEntitiesIt != videoMonitorEntities.end(); videoMonitorEntitiesIt++)
			{
				TA_Base_Core::VideoMonitor* monitor = dynamic_cast<TA_Base_Core::VideoMonitor*>(*videoMonitorEntitiesIt);
				if(monitor != NULL && m_consoleKeyToMonitorIdMap.find(monitor->getConsoleKey()) == m_consoleKeyToMonitorIdMap.end())
				{
					unsigned long monitorId = atoi(monitor->getAddress().c_str());
					if(monitorId > 0)
					{
						if(monitor->getConsoleKey()>0 && monitor->isAlarmMonitor())
						{
							m_consoleKeyToMonitorIdMap[monitor->getConsoleKey()] = monitorId;
						}
					}
					else
					{
						std::stringstream message;
						message << "Monitor addresses must be a valid number: "<< monitor->getName();
						throw( TA_Base_Core::DataException(message.str().c_str(), TA_Base_Core::DataException::INVALID_VALUE, monitor->getName()));
					}
				}
				else if(monitor!=NULL && monitor->isAlarmMonitor())
				{
					std::stringstream message;
					message << "Console keys must be unique: "<< monitor->getName();
					throw( TA_Base_Core::DataException(message.str().c_str(), TA_Base_Core::DataException::NOT_UNIQUE, monitor->getName()));
				}
			}
			


            TA_IRS_Bus::AtsZoneUtils::CctvZoneToLocationMap cctvZoneToLocationMap;
            TA_IRS_Bus::AtsZoneUtils::getCctvConfiguration( m_locationKeyToStageIdMap, cctvZoneToLocationMap );

			
			TA_IRS_Core::IVideoTriggeringEvents allEvents = TA_IRS_Core::VideoTriggeringEventAccessFactory::getInstance().getAllVideoTriggeringEvents(false);
			TA_IRS_Core::IVideoTriggeringEvents::iterator trigIt;

			for(trigIt = allEvents.begin(); trigIt!=allEvents.end(); trigIt++)
			{
				TA_COE::AlarmState alarm;
				alarm.eventId = (*trigIt)->getEventId();
				alarm.stationId = (*trigIt)->getStationId();

				// TD16066
				if ( m_triggeringEventsConfigMap.find(alarm) == m_triggeringEventsConfigMap.end())
				{
					VideoTriggeringEventsList trigEventList;
					trigEventList.push_back(*trigIt);

					m_triggeringEventsConfigMap[alarm] = trigEventList;
				}
				else
				{
					m_triggeringEventsConfigMap[alarm].push_back(*trigIt);
				}
			}
            COESwitchManager::getInstance().getStateValidationCycle().setValidationCyclePeriods( m_lowPriorityStateValidationCycle, m_highPriorityStateValidationCycle );

            // Get the VideoSwitch data, so we can get the addresses for the switching hardware.
            unsigned long locationKey = strtoul( TA_Base_Core::RunParams::getInstance().get( RPARAM_LOCATIONKEY ).c_str() , NULL, 10 );
            m_videoSwitchData = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation( VIDEO_SWITCH_TYPE, locationKey);
            loadCommunicationsChannels();
			SwitchManager::getInstance().setAlarmInterface(this);
            SwitchManager::getInstance().setStageModeInterface(this);

        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", 
                "Failed to retrieve mandatory VideoSwitchAgent parameters, or addresses of any configured VideoSwitch entities, from the database." );
            
            cleanUp();
            
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR, 
                "Failed to retrieve mandatory VideoSwitchAgent parameters, or addresses of any configured VideoSwitch entities, from the database." ) );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", 
                "Failed to retrieve mandatory VideoSwitchAgent parameters, or addresses of any configured VideoSwitch entities, from the database." );
            
            cleanUp();
            
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR, 
                "Failed to retrieve mandatory VideoSwitchAgent parameters, or addresses of any configured VideoSwitch entities, from the database." ) );            
        }
    }


    //
    // ~COECommunicationsHandler
    //
    COECommunicationsHandler::~COECommunicationsHandler()
    {
        FUNCTION_ENTRY( "Destructor" );

        // If an exception is thrown here, there is nothing we can do, 
        // but we should make sure no exceptions are thrown out of this destructor.
        try
        {
            // Clean up this thread if still running, and any queued commands.
            disconnect();

            // Clean up heap objects.
            cleanUp();

            // As this is last COE Protocol object to destruct, here we manually destruct
            // the COESwitvhManager singleton also.
            COESwitchManager::getInstance().removeInstance();
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        }
        catch( ... )
        {
             LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Reason unknown" );           
        }

        FUNCTION_EXIT;
    }


    /// 
    // submitRequestCommand
    //
    void COECommunicationsHandler::submitRequestCommand( RequestCommandSharedPtr command, bool isClientRequest )
    {
        FUNCTION_ENTRY( "submitRequestCommand" );

        TA_Base_Core::ThreadGuard threadStateGuard( m_threadStateLock );

        //
        // Make sure the communications handler is connected (i.e. thread active), else
        // do not queue the command, otherwise it may never be processed.
        //
        if ( false == isConnected() )
        {
            commandNotToBeProcessed( command );
            FUNCTION_EXIT;
            return;
        }

        //
        // Since we are connected, queue the command as specified by isClientRequest,
        // and post the semaphore to indicate there is work to do.
        //
        if ( isClientRequest )
        {
            m_clientRequestCommandQueue.push( command );
        }
        else
        {
            m_internalRequestCommandQueue.push( command );
        }
        m_processingToBeDoneSemaphore.post();

        FUNCTION_EXIT;
    }


    // 
    // submitNotificationCommand
    //
    void COECommunicationsHandler::submitNotificationCommand( NotificationCommandSharedPtr command )
    {
        FUNCTION_ENTRY( "submitRequestCommand" );

        //
        // Important not to acquire the m_threadStateLock here, as may result in
        // deadlock if a channel is trying to submit a NotificationCommand while we
        // are trying to deactivate it. Incidentally, its of no consequence if we 
        // queue a notification command while this thread in process of terminating 
        // (because no thread of execution is waiting for a notification command to
        // be processed, and client code isn't interested in hardware notifcations 
        // after calling disconnect).
        //

        m_notificationCommandQueue.push( command );
        m_processingToBeDoneSemaphore.post();

        FUNCTION_EXIT;
    }


    //
    // processCommunicationsChannelState
    //
    void COECommunicationsHandler::processCommunicationsChannelState( unsigned int channelIndex, ECommunicationChannelState state )
    {
        FUNCTION_ENTRY( "processCommunicationsChannelStateUpdate" );
        
        TA_ASSERT( channelIndex < m_communicationsChannelsInfo.size(), "Invalid channelIndex passed to processCommunicationsChannelStateUpdate!" );
        
        TA_Base_Core::ThreadGuard commsChannelsStateGuard( m_commsChannelsStateLock );

        switch ( state )
        {
        case COMMUNICATIONS_FAILURE:
            receivedChannelStateCommsFailure( channelIndex );
            receivedChannelStateNotActive( channelIndex );
            m_communicationsChannelsInfo[ channelIndex ].state = COMMUNICATIONS_FAILURE;
            break;
        case IDLE_SWITCH_CHANNEL:
            receivedChannelStateNotCommsFailure( channelIndex );
            receivedChannelStateNotActive( channelIndex );
            m_communicationsChannelsInfo[ channelIndex ].state = IDLE_SWITCH_CHANNEL;
            break;
        case ACTIVE_SWITCH_CHANNEL:
            receivedChannelStateNotCommsFailure( channelIndex );
            receivedChannelStateActive( channelIndex );
            m_communicationsChannelsInfo[ channelIndex ].state = ACTIVE_SWITCH_CHANNEL;
            break;
        default:
            TA_ASSERT( false, "processCommunicationsChannelStateUpdate passed illegal state update!" );
            break;
        }

        FUNCTION_EXIT;
    }


    //
    // processSummaryAlarmsState
    //
	//19444
    void COECommunicationsHandler::processSummaryAlarmsState( bool systemSummaryAlarm, bool transmissionSummaryAlarm, bool cameraSummaryAlarm, bool ancillarySummaryAlarm, bool dvrSummaryAlarm )
    {
        FUNCTION_ENTRY( "processSummaryAlarmsStateUpdate" );		

        DataPoint* systemDP = COESwitchManager::getInstance().getDataPointFromId( SYSTEM_DATAPOINT_ID );
        DataPoint* transmissionDP = COESwitchManager::getInstance().getDataPointFromId( TRANSMISSION_DATAPOINT_ID );
        DataPoint* cameraDP = COESwitchManager::getInstance().getDataPointFromId( CAMERA_DATAPOINT_ID );
        DataPoint* ancillaryDP = COESwitchManager::getInstance().getDataPointFromId( ANCILLARY_DATAPOINT_ID );
		DataPoint* dvrDP = COESwitchManager::getInstance().getDataPointFromId( DVR_DATAPOINT_ID );

        time_t now = time( 0 );

        if ( 0 != systemDP )
        {
            DpValue dpValue( systemDP->getDataPointDataType(), systemDP->getEnumLabelsMap(), systemDP->getBooleanLabels() );
            dpValue.setBoolean( systemSummaryAlarm );

            DataPointState dpState( dpValue, now, TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
            dpState.setValue( dpValue );

            systemDP->updateFieldState( dpState );
        }
        if ( 0 != transmissionDP )
        {
            DpValue dpValue( transmissionDP->getDataPointDataType(), transmissionDP->getEnumLabelsMap(), transmissionDP->getBooleanLabels() );
            dpValue.setBoolean( transmissionSummaryAlarm );
            
            DataPointState dpState( dpValue, now, TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
            dpState.setValue( dpValue );

            transmissionDP->updateFieldState( dpState );
        }
        if ( 0 != cameraDP )
        {
            DpValue dpValue( cameraDP->getDataPointDataType(), cameraDP->getEnumLabelsMap(), cameraDP->getBooleanLabels() );
            dpValue.setBoolean( cameraSummaryAlarm );
            
            DataPointState dpState( dpValue, now, TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
            dpState.setValue( dpValue );

            cameraDP->updateFieldState( dpState );
        }
        if ( 0 != ancillaryDP )
        {
            DpValue dpValue( ancillaryDP->getDataPointDataType(), ancillaryDP->getEnumLabelsMap(), ancillaryDP->getBooleanLabels() );
            dpValue.setBoolean( ancillarySummaryAlarm );
            
            DataPointState dpState( dpValue, now, TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
            dpState.setValue( dpValue );

            ancillaryDP->updateFieldState( dpState );
        }
        if ( 0 != dvrDP )
        {
            DpValue dpValue( dvrDP->getDataPointDataType(), dvrDP->getEnumLabelsMap(), dvrDP->getBooleanLabels() );
            dpValue.setBoolean( dvrSummaryAlarm );
            
            DataPointState dpState( dpValue, now, TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
            dpState.setValue( dpValue );
			
            dvrDP->updateFieldState( dpState );
        }

        FUNCTION_EXIT;
    }


	void COECommunicationsHandler::processAlarmStack( TA_COE::AlarmStack alarmStack )
	{
		TA_COE::AlarmStack alarmsToOpen;
		TA_COE::AlarmStack alarmsToClose;

		//this is the only place that m_alarmStack is altered so we know its already sorted
		//so we only need to sort the new stack
		std::sort(alarmStack.begin(), alarmStack.end());
		
		std::set_difference(alarmStack.begin(), alarmStack.end(), 
                            m_alarmStack.begin(), m_alarmStack.end(),
                            std::insert_iterator<TA_COE::AlarmStack>
                            (alarmsToOpen, alarmsToOpen.begin()));

		if(alarmsToOpen.size()!=0)
		{
			openTriggeringEventAlarms(alarmsToOpen);
		}

		std::set_difference(m_alarmStack.begin(), m_alarmStack.end(),
							alarmStack.begin(), alarmStack.end(), 
                            std::insert_iterator<TA_COE::AlarmStack>
                            (alarmsToClose, alarmsToClose.begin()));

		if(alarmsToClose.size()!=0)
		{
			closeTriggeringEventAlarms(alarmsToClose);
		}

		m_alarmStack = alarmStack;

		StateUpdateBroadcaster::getInstance().notifyAlarmStackUpdate();


	}


    ///////////////////////////////////////////////////////////////////////
    //
    // IPDCommunicationsHandler Methods to be overridden
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // connect
    //
    void COECommunicationsHandler::connect()
    {
        FUNCTION_ENTRY( "connect" );

        TA_Base_Core::ThreadGuard threadStateGuard( m_threadStateLock );

        if ( TA_Base_Core::Thread::THREAD_STATE_UNKNOWN != getCurrentState() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                "connect() called while already active. Thread will be restarted." );

            terminateAndWait();            
        }

        activateCommunicationsChannels();
        start();

        // Activate State Validation Thread.
        COESwitchManager::getInstance().getStateValidationCycle().startValidationCycle();
		
		FUNCTION_EXIT;
    }


    //
    // disconnect
    //
    void COECommunicationsHandler::disconnect()
    {
        FUNCTION_ENTRY( "disconnect" );

        //
        // IMPORTANT - Must deactivate State Validation Thread before we obtain
        // m_threadStateLock. Otherwise deadlock may occur because the validation
        // thread acquires this lock during its run method (via calling
        // submitRequestCommand).
        //
        COESwitchManager::getInstance().getStateValidationCycle().stopValidationCycle();

        TA_Base_Core::ThreadGuard threadStateGuard( m_threadStateLock ); 

        if ( TA_Base_Core::Thread::THREAD_STATE_UNKNOWN != getCurrentState() )
        {
            terminateAndWait();
        }

        // Empty anything left in the queues as these may never be processed now, 
        clearCommandQueues();

        // Ensure flag reset so thread will run correctly again when reactivated.
        m_doTerminate = false;
    
        FUNCTION_EXIT;
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Thread Methods
    //
    ///////////////////////////////////////////////////////////////////////
    

    //
    // run
    //
	void COECommunicationsHandler::run()
	{
        // If an exception is thrown here, there is nothing we can do, 
        // but we should make sure no exceptions are thrown out of this method.

        try
        {
		    FUNCTION_ENTRY( "run" );
    
		    while( false == m_doTerminate )
		    {
                try //if something roots up we dont want to give up and have the thread die
				{
					m_processingToBeDoneSemaphore.wait();
                
					//
					// Prioritise as follows:
					// (1) Hardware Notifications;
					// (2) Client Request Commands;
					// (3) Internal Request Commands (State Validation)
					//
					if ( false == m_notificationCommandQueue.empty() )
					{
						( m_notificationCommandQueue.front() )->processNotification();
						m_notificationCommandQueue.pop();
					}
					else if ( false == m_clientRequestCommandQueue.empty() )
					{
						processRequestCommand( m_clientRequestCommandQueue.front() );
						m_clientRequestCommandQueue.pop();
					}
					else if ( false == m_internalRequestCommandQueue.empty() )
					{
						processRequestCommand( m_internalRequestCommandQueue.front() );
						m_internalRequestCommandQueue.pop();
					}

				}
		        catch( const TA_Base_Core::TransactiveException& e )
				{
					LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
				}
				catch( ... )
				{
					 LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Reason unknown" );           
				}

			} // end while( false == m_doTerminate)

		    deactivateCommunicationsChannels();

		    FUNCTION_EXIT;
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        }
        catch( ... )
        {
             LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Reason unknown" );           
        }
	}


    //
    // terminate
    //
	void COECommunicationsHandler::terminate()
	{
		m_doTerminate = true;
        m_processingToBeDoneSemaphore.post();
	}
	
    
    ///////////////////////////////////////////////////////////////////////
    //
    // Private Methods
    //
    ///////////////////////////////////////////////////////////////////////
  	

    //
    // loadCommunicationChannels
    //
    void COECommunicationsHandler::loadCommunicationsChannels()
    {
        FUNCTION_ENTRY( "loadCommunicationsChannels" );

        // Get the addresses of all the VideoSwitches and create CommsChannel for each.  This may throw database related exceptions which are documented
        // in the method comment.

        CommunicationsChannelInfo defaultChannelInfo;
        defaultChannelInfo.state = NOT_CURRENTLY_ESTABLISHED;
        defaultChannelInfo.numberCyclesWithoutActiveChannel = 0;

        VideoSwitchData::iterator it = m_videoSwitchData.begin();
        for ( ; it != m_videoSwitchData.end(); it++ )
        {
			// obtain the asset name
			std::auto_ptr<TA_Base_Core::IData> entityParam = std::auto_ptr<TA_Base_Core::IData>(TA_Base_Core::EntityAccessFactory::getInstance().getParameter((*it)->getName(), "AssetName"));
			std::string assetName("");
			if (entityParam.get() != 0 && entityParam->getNumRows() > 0 && entityParam->isNull(0, "VALUE") == false)
			{
				assetName = entityParam->getStringData(0, "VALUE");
			}

            AddressHelper::Address switchAddress = AddressHelper::splitAddress( ( *it )->getAddress() );                                      
            m_communicationsChannels.push_back( new COECommunicationsChannel( ( it - m_videoSwitchData.begin() ), 
                switchAddress.host, switchAddress.port, m_switchConnectTimeout, m_maxSwitchResponseTime, m_switchPollInterval, *this, *m_receivedDataProcessor.get(), ( *it )->getName(), ( *it )->getKey(), (* it )->getLocation(), ( *it )->getTypeKey(), ( *it )->getSubsystem(), assetName ) );
            m_communicationsChannelsInfo.push_back( defaultChannelInfo );           
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                "COECommunications channel created for VideoSwitch at '%s:%s'", switchAddress.host.c_str(), switchAddress.port.c_str() );
        }
        m_activeSwitchChannelIndex = m_communicationsChannels.size(); // invalid index indicates no current active switch channel
        
        TA_ASSERT( m_communicationsChannelsInfo.size() == m_communicationsChannels.size(), 
            "The number of CommunicationsChannels and CommunicationsChannelsInfo structs is not equal!" );

        FUNCTION_EXIT;
    }


    // 
    // activateCommunicationsChannels
    //
    void COECommunicationsHandler::activateCommunicationsChannels()
    {
        FUNCTION_ENTRY( "activateCommunicationsChannels" );

        CommunicationsChannels::iterator commsChannelIt = m_communicationsChannels.begin();
        for ( ; commsChannelIt != m_communicationsChannels.end(); commsChannelIt++ )
        {
            ( *commsChannelIt )->activateChannel();
        }

        FUNCTION_EXIT;
    }


    // 
    // deactivateCommuncationsChannels
    //
    void COECommunicationsHandler::deactivateCommunicationsChannels()
    {
        FUNCTION_ENTRY( "deactivateCommuncationsChannels" );

        TA_ASSERT( m_communicationsChannelsInfo.size() == m_communicationsChannels.size(), 
            "The number of CommunicationsChannels and CommunicationsChannelsInfo structs is not equal!" );

        CommunicationsChannels::iterator commsChannelIt = m_communicationsChannels.begin();
        CommunicationsChannelsInfo::iterator commsChannelInfoIt = m_communicationsChannelsInfo.begin();
        for ( ; commsChannelIt != m_communicationsChannels.end(); commsChannelIt++ )
        {
            ( *commsChannelIt )->deactivateChannel();

            //
            // Important to only acquire lock while channel not in process of deactivating
            // else potential for deadlock.
            //
            TA_Base_Core::ThreadGuard commsChannelsStateGuard( m_commsChannelsStateLock );
                ( *commsChannelInfoIt ).state = NOT_CURRENTLY_ESTABLISHED;
                ( *commsChannelInfoIt ).numberCyclesWithoutActiveChannel = 0;
            commsChannelInfoIt++;
        }
        m_activeSwitchChannelIndex = m_communicationsChannels.size(); // invalid index indicates no current active switch channel
        m_exceededNoActiveSwitchCycleAllowance = false;

        FUNCTION_EXIT;
    }


    //
    // cleanUp
    //
    void COECommunicationsHandler::cleanUp()
    {
        FUNCTION_ENTRY( "cleanUpVideoSwitchData" );

        VideoSwitchData::iterator videoSwitchDataIt = m_videoSwitchData.begin();
        for ( ; videoSwitchDataIt != m_videoSwitchData.end(); videoSwitchDataIt++ )
        {
            delete ( *videoSwitchDataIt );
        }
        m_videoSwitchData.clear();

        CommunicationsChannels::iterator commsChannelIt = m_communicationsChannels.begin();
        for ( ; commsChannelIt != m_communicationsChannels.end(); commsChannelIt++ )
        {
            delete ( *commsChannelIt );
        }
        m_communicationsChannels.clear();
        m_communicationsChannelsInfo.clear();
        
        FUNCTION_EXIT;
    }


    //
    // processRequestCommand
    //
    void COECommunicationsHandler::processRequestCommand( RequestCommandSharedPtr command )
    {
        FUNCTION_ENTRY( "processRequestCommand" );

        //
        // Important that this thread of execution should not acquire the m_commsChannelsStateLock
        // while calling "sendCommand" on the channel. Otherwise deadlock can occur because the
        // channel's thread may be trying to acquire thwe same lock when processing it's polling 
        // commands. This is why this method uses "isThereActiveSwitchChannel" to determine this
        // information.
        //

        unsigned int activeChannelIndex( 0 );
        if ( isThereActiveSwitchChannel( activeChannelIndex ) )
        {
            TA_ASSERT( 0 != command.get(), "Request command shared ptr is null!" );

            m_communicationsChannels[ m_activeSwitchChannelIndex ]->
                sendCommand( *command.get() );
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                "Request command could not be sent as no connection to active switch!" );

            command->signalCouldNotBeSent();
        }

        FUNCTION_EXIT;
    }


    //
    // receivedChannelStateCommsFailure
    //
    void COECommunicationsHandler::receivedChannelStateCommsFailure( unsigned int channelIndex )
    {
        if ( m_communicationsChannelsInfo[ channelIndex ].state != COMMUNICATIONS_FAILURE )
        {
            // TODO Raise Alarm
        }
    }


    //
    // receivedChannelStateNotCommsFailure
    //
    void COECommunicationsHandler::receivedChannelStateNotCommsFailure( unsigned int channelIndex )
    {
        if ( ( m_communicationsChannelsInfo[ channelIndex ].state == COMMUNICATIONS_FAILURE )
            || ( m_communicationsChannelsInfo[ channelIndex ].state == NOT_CURRENTLY_ESTABLISHED ) )
        {
            // TODO Ensure Alarm Closed
        }
    }
    

    //
    // receivedChannelStateActive
    // 
    void COECommunicationsHandler::receivedChannelStateActive( unsigned int channelIndex )
    {
        if ( m_activeSwitchChannelIndex != channelIndex )
        {
            // TODO Ensure No Active Switch Alarm Closed
            m_activeSwitchChannelIndex = channelIndex;
            CommunicationsChannelsInfo::iterator it = m_communicationsChannelsInfo.begin();
            for ( ; it != m_communicationsChannelsInfo.end(); it++ )
            {
                ( *it ).numberCyclesWithoutActiveChannel = 0;
            }
            m_exceededNoActiveSwitchCycleAllowance = false;
        }
    }


    //
    // receivedChannelStateNotActive
    //
    void COECommunicationsHandler::receivedChannelStateNotActive( unsigned int channelIndex )
    {
        if ( m_activeSwitchChannelIndex == channelIndex )
        {
            // As no longer connected to active switch, ensure DataPoints reflect bad quality
            // due to comms failure.
            setDataPointsAsBadValue();

            m_activeSwitchChannelIndex = m_communicationsChannels.size();
        }

        if ( ( m_activeSwitchChannelIndex == m_communicationsChannels.size() )
            && ( false == m_exceededNoActiveSwitchCycleAllowance ) )
        {
            m_communicationsChannelsInfo[ channelIndex ].numberCyclesWithoutActiveChannel++;
            CommunicationsChannelsInfo::iterator it = m_communicationsChannelsInfo.begin();
            unsigned short minNumberCyclesWithoutActiveChannel = ( *it ).numberCyclesWithoutActiveChannel;
            for ( ; it != m_communicationsChannelsInfo.end(); it++ )
            {
                if ( ( *it ).numberCyclesWithoutActiveChannel < minNumberCyclesWithoutActiveChannel )
                {
                    minNumberCyclesWithoutActiveChannel = ( *it ).numberCyclesWithoutActiveChannel;
                }                
            }
            if ( minNumberCyclesWithoutActiveChannel > NO_ACTIVE_SWITCH_CYCLE_ALLOWANCE )
            {
                // As could not connect to active switch, ensure DataPoints reflect bad quality
                // due to comms failure.
                setDataPointsAsBadValue();

                // TODO Raise No Active Switch Alarm
                m_exceededNoActiveSwitchCycleAllowance = true;
            }
        }
    }


    //
    // setDataPointsAsBadValue
    //
    void COECommunicationsHandler::setDataPointsAsBadValue()
    {
        DataPoint* systemDP = COESwitchManager::getInstance().getDataPointFromId( SYSTEM_DATAPOINT_ID );
        DataPoint* transmissionDP = COESwitchManager::getInstance().getDataPointFromId( TRANSMISSION_DATAPOINT_ID );
        DataPoint* cameraDP = COESwitchManager::getInstance().getDataPointFromId( CAMERA_DATAPOINT_ID );
        DataPoint* ancillaryDP = COESwitchManager::getInstance().getDataPointFromId( ANCILLARY_DATAPOINT_ID );
		//TD19444
		DataPoint* dvrDP = COESwitchManager::getInstance().getDataPointFromId(DVR_DATAPOINT_ID);

        time_t now = time( 0 );

        if ( 0 != systemDP )
        {
            DpValue dpValue( systemDP->getDataPointDataType(), systemDP->getEnumLabelsMap(), systemDP->getBooleanLabels() );
            DataPointState badState( dpValue, time( 0 ), TA_Base_Bus::QUALITY_BAD_COMM_FAILURE );                
            systemDP->updateFieldState( badState );
        }
        if ( 0 != transmissionDP )
        {
            DpValue dpValue( transmissionDP->getDataPointDataType(), transmissionDP->getEnumLabelsMap(), transmissionDP->getBooleanLabels() );
            DataPointState badState( dpValue, time( 0 ), TA_Base_Bus::QUALITY_BAD_COMM_FAILURE );                
            transmissionDP->updateFieldState( badState );
        }
        if ( 0 != cameraDP )
        {
            DpValue dpValue( cameraDP->getDataPointDataType(), cameraDP->getEnumLabelsMap(), cameraDP->getBooleanLabels() );
            DataPointState badState( dpValue, time( 0 ), TA_Base_Bus::QUALITY_BAD_COMM_FAILURE );                
            cameraDP->updateFieldState( badState );
        }
        if ( 0 != ancillaryDP )
        {
            DpValue dpValue( ancillaryDP->getDataPointDataType(), ancillaryDP->getEnumLabelsMap(), ancillaryDP->getBooleanLabels() );
            DataPointState badState( dpValue, time( 0 ), TA_Base_Bus::QUALITY_BAD_COMM_FAILURE );                
            ancillaryDP->updateFieldState( badState );
        }
        if ( 0 != dvrDP )
        {
            DpValue dpValue( dvrDP->getDataPointDataType(), dvrDP->getEnumLabelsMap(), dvrDP->getBooleanLabels() );
            DataPointState badState( dpValue, time( 0 ), TA_Base_Bus::QUALITY_BAD_COMM_FAILURE );                
            dvrDP->updateFieldState( badState );
        }
		//TD19444
    }


    //
    // isConnected
    //
    bool COECommunicationsHandler::isConnected()
    {
        TA_Base_Core::ThreadGuard threadStateGuard( m_threadStateLock );
        
        return ( ( TA_Base_Core::Thread::THREAD_STATE_RUNNING == getCurrentState() ) 
                    || ( TA_Base_Core::Thread::THREAD_STATE_NEW == getCurrentState() ) );
    }


    //
    // isThereActiveSwitchChannel
    //
    bool COECommunicationsHandler::isThereActiveSwitchChannel( unsigned int& activeChannelIndex )
    {
        FUNCTION_ENTRY( "isThereActiveSwitchChannel" );

        TA_Base_Core::ThreadGuard commsChannelsStateGuard( m_commsChannelsStateLock );

        activeChannelIndex = m_activeSwitchChannelIndex;

        FUNCTION_EXIT;
        return ( m_activeSwitchChannelIndex < m_communicationsChannels.size() );
    }

    
    //
    // commandNotToBeProcessed
    // 
    void  COECommunicationsHandler::commandNotToBeProcessed( RequestCommandSharedPtr command )
    {
        FUNCTION_ENTRY( "commandNotProcessed" );

        //
        // Log the fact the request command was never processed by CommsHandler (as disconnected)
        // and signal that it could not be sent so the process waiting for completion will wake
        // up. This should not happen as all servant bases are deactivated before the comms handler
        // is disconnected, so log as warning.
        //
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
            "Request command could not be processed as COECommunications handler was disconnected." );

        command->signalCouldNotBeSent();

        FUNCTION_EXIT;
    }


    //
    // commandNotToBeProcessed 
    // 
    void COECommunicationsHandler::commandNotToBeProcessed( NotificationCommandSharedPtr command )
    {
        FUNCTION_ENTRY( "commandNotProcessed" );

        //
        // A notification command may be received by one of our channels during 
        // the process of disconnecting. Can't do anything but this logs the
        // fact the command was received but never processed. This is not an error.
        //
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "Notification command could not be processed as COECommunications handler was disconnected." );

        FUNCTION_EXIT;
    }


    //
    // clearCommandQueues
    //
    void COECommunicationsHandler::clearCommandQueues()
    {
        FUNCTION_ENTRY( "clearCommandQueues" );

        while ( false == m_notificationCommandQueue.empty() )
        {
            commandNotToBeProcessed( m_notificationCommandQueue.front() );
            m_notificationCommandQueue.pop();
        }

        while ( false == m_clientRequestCommandQueue.empty() )
        {
            commandNotToBeProcessed( m_clientRequestCommandQueue.front() );
            m_clientRequestCommandQueue.pop();
        }

        while ( false == m_internalRequestCommandQueue.empty() )
        {
            commandNotToBeProcessed( m_internalRequestCommandQueue.front() );
            m_internalRequestCommandQueue.pop();
        }

        // Ensure semaphore reset back to zero as there is no
        // processing to be done now that the queues have been cleared.
        while ( m_processingToBeDoneSemaphore.tryWait() )
        {
            // Nothing to do inside loop.
        }

        FUNCTION_EXIT;
    }

	// TD TD 15200
	/*
	void COECommunicationsHandler::openTriggeringEventAlarms(const TA_COE::AlarmStack& alarmsToBeOpened)
	{
		std::auto_ptr<TA_Base_Core::NonUniqueAlarmHelper> alarmHelper( 
		TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper());
		
		TA_COE::AlarmStack::const_iterator alarmStackItr;
		for(alarmStackItr = alarmsToBeOpened.begin(); alarmStackItr != alarmsToBeOpened.end(); alarmStackItr++)
		{
			if((*alarmStackItr).eventId!=0)
			{
				std::map<TA_COE::AlarmState, TA_IRS_Core::IVideoTriggeringEvent*>::iterator eventsConfigItr = m_triggeringEventsConfigMap.find(*alarmStackItr);

				std::string location;
				std::string triggeringEvent;
				
				if(eventsConfigItr != m_triggeringEventsConfigMap.end())
				{
					TA_IRS_Core::IVideoTriggeringEvent* eventConfig = (*eventsConfigItr).second;
					if(eventConfig!=NULL)
					{
						location = eventConfig->getLocation();
						triggeringEvent = eventConfig->getCctvTriggering();
					}
					else
					{
						location = "Unknown";
						triggeringEvent = "Unknown";
					}
				}
				else
				{
					location = "Unknown";
					triggeringEvent = "Unknown Event: "+(*alarmStackItr).eventId;
				}
			
				TA_IRS_Core::VideoStationIdAccessFactory::StationIdToLocationKeyMap::const_iterator stationId = m_stationIdToLocationMap.find((*alarmStackItr).stationId);
				std::string stationName = "Unknown Station: "+(*alarmStackItr).stationId;

				if(stationId!=m_stationIdToLocationMap.end())
				{
					try
					{
						stationName = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey((*stationId).second)->getName();
					}
					catch(...)
					{
					}
				}

				TA_Base_Core::DescriptionParameters dp;
				TA_Base_Core::NameValuePair pair1 ("Location", location);
				dp.push_back(&pair1);
			
				TA_Base_Core::NameValuePair pair3 ("Station", stationName);
				dp.push_back(&pair3);
				
				TA_Base_Core::NameValuePair pair2 ("Event", triggeringEvent);
				dp.push_back(&pair2);

				timeb timestamp = TA_Base_Core::AlarmConstants::defaultTime;

				std::string alarmId = alarmHelper->submitAlarmWithAsset(TA_Base_Core::VisualAlarms::CctvTriggeringAlarm, 
													m_videoSwitchAgentData.getKey(),
													m_videoSwitchAgentData.getTypeKey(),
													dp,
													m_videoSwitchAgentData.getName(), 
													//m_stationIdToLocationMap[(*alarmStackItr).stationId], 
													m_videoSwitchAgentData.getLocation(),
													m_videoSwitchAgentData.getSubsystem(),
													m_videoSwitchAgentData.getAssetName(),
													timestamp);
				
				TA_Base_Core::ThreadGuard threadStateGuard( m_openAlarmsLock );

				std::map<TA_COE::AlarmState, std::string>::iterator openAlarmsItr = m_openAlarms.find(*alarmStackItr);
				if(openAlarmsItr != m_openAlarms.end())
				{
							alarmHelper->closeAlarmAtLocation((*openAlarmsItr).second, m_videoSwitchAgentData.getLocation());
				}
				m_openAlarms[*alarmStackItr] = alarmId;
			}
		}
	}

	void COECommunicationsHandler::closeTriggeringEventAlarms(const TA_COE::AlarmStack& alarmsToBeClosed)
	{
		std::auto_ptr<TA_Base_Core::NonUniqueAlarmHelper> alarmHelper( 
		TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper());
		
		TA_COE::AlarmStack::const_iterator alarmStackItr;
		for(alarmStackItr = alarmsToBeClosed.begin(); alarmStackItr != alarmsToBeClosed.end(); alarmStackItr++)
		{
			if((*alarmStackItr).eventId!=0)
			{
				TA_Base_Core::ThreadGuard threadStateGuard( m_openAlarmsLock );

				std::map<TA_COE::AlarmState, std::string>::iterator openAlarmsItr = m_openAlarms.find(*alarmStackItr);
				if(openAlarmsItr != m_openAlarms.end())
				{
					alarmHelper->closeAlarmAtLocation((*openAlarmsItr).second, m_videoSwitchAgentData.getLocation());
					m_openAlarms.erase(openAlarmsItr);
				}
			}
		}
	}
	*/

	void COECommunicationsHandler::openTriggeringEventAlarms(const TA_COE::AlarmStack& alarmsToBeOpened)
	{
		TA_COE::AlarmStack::const_iterator alarmStackItr;
		for(alarmStackItr = alarmsToBeOpened.begin(); alarmStackItr != alarmsToBeOpened.end(); alarmStackItr++)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[TD17255] openTriggeringEventAlarms, eventId=%lu, stationId=%lu",
				(*alarmStackItr).eventId, (*alarmStackItr).stationId);
			if((*alarmStackItr).eventId ==0) continue;

			std::map<TA_COE::AlarmState, VideoTriggeringEventsList>::iterator eventsConfigItr = m_triggeringEventsConfigMap.find(*alarmStackItr);
			
			std::string location;
			std::string triggeringEvent;
			unsigned long entityKey;
			TA_Base_Core::IEntityData* entityData = NULL;
			
			if(eventsConfigItr != m_triggeringEventsConfigMap.end())
			{
				// TD16066
				TA_IRS_Core::IVideoTriggeringEvent* eventConfig = NULL;

				// loop through the VideoTriggeringEventsList to get CctvTriggeringAlarm entity for this location
				VideoTriggeringEventsList eventList = (*eventsConfigItr).second;
				for (VideoTriggeringEventsList::iterator it = eventList.begin(); it != eventList.end(); it++)
				{
					entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity((*it)->getEntityKey());
					if(0 == entityData)
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[TD16066]Cannot get the entity data to raise the alarms. eventId %d", (*alarmStackItr).eventId);
						continue;
					}

					if (entityData->getLocation() == m_videoSwitchAgentData->getLocation())
					{
						eventConfig = *it;
						break;
					}
					else
					{
						delete entityData;
						entityData = NULL;
					}
				}
				// TD16066
				
				if(eventConfig!=NULL)
				{
					location = eventConfig->getLocation();
					triggeringEvent = eventConfig->getCctvTriggering();
					entityKey = eventConfig->getEntityKey();
				}
				else
				{
					location = "Unknown";
					triggeringEvent = "Unknown";
					entityKey = -1;
				}
			}
			else
			{
				location = "Unknown";
				triggeringEvent = "Unknown Event: "+(*alarmStackItr).eventId;
				entityKey = -1;
			}
			
			if(-1 == entityKey)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot find the entity to raise the alarms. eventId %d", (*alarmStackItr).eventId);
				continue;
			}
			
			TA_IRS_Core::VideoStationIdAccessFactory::StationIdToLocationKeyMap::const_iterator stationId = m_stationIdToLocationMap.find((*alarmStackItr).stationId);
			std::string stationName = "Unknown Station: "+(*alarmStackItr).stationId;

			if(stationId!=m_stationIdToLocationMap.end())
			{
				try
				{
					stationName = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey((*stationId).second)->getName();
				}
				catch(...)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[TD17255] openTriggeringEventAlarms, getLocationByKey failed.");
				}
			}

			TA_Base_Core::DescriptionParameters dp;
			TA_Base_Core::NameValuePair pair1 ("Location", location);
			dp.push_back(&pair1);
		
			TA_Base_Core::NameValuePair pair3 ("Station", stationName);
			dp.push_back(&pair3);
			
			TA_Base_Core::NameValuePair pair2 ("Event", triggeringEvent);
			dp.push_back(&pair2);
			// get the entity data
			//TA_Base_Core::IEntityData* entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityKey);
			if(0 == entityData)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot get the entity data to raise the alarms. eventId %d", (*alarmStackItr).eventId);
				continue;
			}
			
			// jinhua++ TD17255
			std::string assetName("C");
			//assetName = entityData->getLocationName(); 
			assetName += stationName; // yanrong++
			assetName += "/SCS/CCTV/";
			// ++jinhua TD17255

			// get the alarm type
			const TA_Base_Core::MessageType* alarmType = 0;
			std::string subsystemName = entityData->getPhysicalSubsystemName();
			
			if (subsystemName == INTRUDER_ALM)
			{
				alarmType = &(TA_Base_Core::VisualAlarms::CctvTriggeringAlarmAMS);
				assetName += "AMS"; //jinhua TD17255
			}
			else if (subsystemName == INTERCOM_ACT_ALM)
			{
				alarmType = &(TA_Base_Core::VisualAlarms::CctvTriggeringAlarmIntercom);
				assetName += "ICOM"; //jinhua TD17255
			}
			else if (subsystemName == BLS_ACT_ALM)
			{
				alarmType = &(TA_Base_Core::VisualAlarms::CctvTriggeringAlarmBLS);
				assetName += "BLS"; //jinhua TD17255
			}
			else if (subsystemName == ESP_ALM)
			{
				alarmType = &(TA_Base_Core::VisualAlarms::CctvTriggeringAlarmESP);
				assetName += "ESP"; //jinhua TD17255
			}
			else if (subsystemName == PAT_ALM)
			{
				alarmType = &(TA_Base_Core::VisualAlarms::CctvTriggeringAlarmPAT);
				assetName += "EPAX"; //jinhua TD17255
			}
			else
			{
				alarmType = 0;
			}

			//jinhua++ TD17255
			if( m_locationType == TA_Base_Core::ILocation::OCC )
			{
				assetName += "_OCC"; 
			}
			//++jinhua TD17255

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[TD17255] openTriggeringEventAlarms, assetName=%s, locationType=%d", assetName.c_str(), m_locationType);

			// submit alarm
			if (0 != alarmType)
			{
				timeb timestamp = TA_Base_Core::AlarmConstants::defaultTime;
				std::string alarmIDStr = TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarmWithAsset(
					*alarmType, 
					entityData->getKey(),
					entityData->getTypeKey(),
					dp,
					entityData->getName(), 
					entityData->getLocation(),
					entityData->getSubsystem(),
					assetName, //jinhua TD17255
					timestamp);

				// TD15853 - add to open alarms cache 
				AlarmID alarm_id;
				alarm_id.alarmType = alarmType->getTypeKey();
				alarm_id.entityKey = entityData->getKey();
				alarm_id.alarmID   = alarmIDStr;
				/*
				 * TD16164 - close alarm when it already exists
				 */
				TA_Base_Core::ThreadGuard threadStateGuard( m_openAlarmsLock );
				std::map<TA_COE::AlarmState,AlarmID>::iterator openAlarmsItr = m_openAlarms.find(*alarmStackItr);
				if(openAlarmsItr != m_openAlarms.end())
				{
					AlarmID alarm_id = (*openAlarmsItr).second;
					// yanrong++ CL-17255
					std::string closeValue = "NORMAL";
					// ++yanrong

					std::ostringstream logStr;
					logStr << "[TD16164] Alarm exists. Close AlarmID = " << alarm_id.alarmID
						   << " Type = " << alarmType->getTypeKey() << " EntityKey = %d " << entityData->getKey()
						   << " closeValue = " << closeValue;
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, logStr.str().c_str());

					TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().closeAlarmAtLocation(alarm_id.alarmID, m_videoSwitchAgentData->getLocation(), closeValue);
				}
				std::ostringstream data;
				data << "[Add to OpenAlarms] AlarmID = " << alarmIDStr
					 << " EventID = " << (*alarmStackItr).eventId
					 << " StationID = " << (*alarmStackItr).stationId
					 << " Alarm Type = " << alarm_id.alarmType
					 << " Entity Key = " << alarm_id.entityKey;

				m_openAlarms[*alarmStackItr] = alarm_id;
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, data.str().c_str());
				// TD15853
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot get the correct alarm type. eventId %d", (*alarmStackItr).eventId);
			}
		}
	}
	
	void COECommunicationsHandler::closeTriggeringEventAlarms(const TA_COE::AlarmStack& alarmsToBeClosed)
	{
		TA_COE::AlarmStack::const_iterator alarmStackItr;
		for(alarmStackItr = alarmsToBeClosed.begin(); alarmStackItr != alarmsToBeClosed.end(); alarmStackItr++)
		{
			if((*alarmStackItr).eventId == 0) continue;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
				"Close Triggering Event Alarm with Event ID = %d ", (*alarmStackItr).eventId);
			
			std::map<TA_COE::AlarmState, VideoTriggeringEventsList>::iterator eventsConfigItr = m_triggeringEventsConfigMap.find(*alarmStackItr);
			unsigned long entityKey; 
			
			TA_Base_Core::IEntityData* entityData = NULL;
			if(eventsConfigItr != m_triggeringEventsConfigMap.end())
			{
				//TA_IRS_Core::IVideoTriggeringEvent* eventConfig = (*eventsConfigItr).second;

				// TD16066
				TA_IRS_Core::IVideoTriggeringEvent* eventConfig = NULL;

				// loop through the VideoTriggeringEventsList to get CctvTriggeringAlarm entity for this location
				VideoTriggeringEventsList eventList = (*eventsConfigItr).second;
				for (VideoTriggeringEventsList::iterator it = eventList.begin(); it != eventList.end(); it++)
				{
					entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity((*it)->getEntityKey());
					if(0 == entityData)
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[TD16066]Cannot get the entity data to raise the alarms. eventId %d", (*alarmStackItr).eventId);
						continue;
					}

					if (entityData->getLocation() == m_videoSwitchAgentData->getLocation())
					{
						eventConfig = *it;
						break;
					}
					else
					{
						delete entityData;
						entityData = NULL;
					}
				}
				// TD16066


				if(eventConfig!=NULL)
				{
					entityKey = eventConfig->getEntityKey();
				}
				else
				{
					entityKey = -1;
				}
			}
			else
			{
				entityKey = -1;
			}
			
			if(-1 == entityKey)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Can't find the entity to close the alarms. eventId %d", (*alarmStackItr).eventId);
				continue;
			}
			
			// get the entity data
			//TA_Base_Core::IEntityData* entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityKey);
			if(0 == entityData)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot get the entity data to close the alarms. eventId %d", (*alarmStackItr).eventId);
				continue;
			}
			
			// get the alarm type
			const TA_Base_Core::MessageType* alarmType = 0;
			std::string subsystemName = entityData->getPhysicalSubsystemName();
			
			if (subsystemName == INTRUDER_ALM)
			{
				alarmType = &(TA_Base_Core::VisualAlarms::CctvTriggeringAlarmAMS);
			}
			else if (subsystemName == INTERCOM_ACT_ALM)
			{
				alarmType = &(TA_Base_Core::VisualAlarms::CctvTriggeringAlarmIntercom);
			}
			else if (subsystemName == BLS_ACT_ALM)
			{
				alarmType = &(TA_Base_Core::VisualAlarms::CctvTriggeringAlarmBLS);
			}
			else if (subsystemName == ESP_ALM)
			{
				alarmType = &(TA_Base_Core::VisualAlarms::CctvTriggeringAlarmESP);
			}
			else if (subsystemName == PAT_ALM)
			{
				alarmType = &(TA_Base_Core::VisualAlarms::CctvTriggeringAlarmPAT);
			}
			else
			{
				alarmType = 0;
			}

			std::ostringstream logStr;
			logStr << "Close Alarm @ Location = " << entityData->getLocation() 
				   << " Type = " << alarmType->getTypeKey() << " EntityKey = %d " << entityData->getKey();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, logStr.str().c_str());

			if(0 != alarmType)
			{
				// TD15853
				std::map<TA_COE::AlarmState, AlarmID>::iterator openAlarmsItr = m_openAlarms.find(*alarmStackItr);
				if (openAlarmsItr != m_openAlarms.end())
				{
					// yanrong++ CL-17255
					std::string closeValue = "NORMAL";
					// ++yanrong
					TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().closeAlarmAtLocation(
						(*openAlarmsItr).second.alarmID, m_videoSwitchAgentData->getLocation(), closeValue);

					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
						"Remove from OpenAlarms cache EventID = %d StationID = %d, closeValue=%s",
						(*openAlarmsItr).first.eventId,(*openAlarmsItr).first.stationId, closeValue.c_str());

					m_openAlarms.erase(openAlarmsItr); 
				}
				// TD15853
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot get the correct alarmtype to close the alarms. eventId %d", (*alarmStackItr).eventId);
			}
		}
	}
	// TD 15200

	void COECommunicationsHandler::acknowledgeAlarm( const std::string& alarmKey,/* unsigned long entityKey,*/ unsigned long consoleId )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Console to Monitor = %d ",consoleId);
		//acknowledging alarm on a switch is only valid at the occ
		//where the default alarm monitors are on the wall not on a console
		if( m_locationType == TA_Base_Core::ILocation::OCC )
		{
			//std::string alarmIdString(alarmId);
		
			std::map<unsigned long, unsigned long>::iterator consoleToMonitorIdIt;
			consoleToMonitorIdIt = m_consoleKeyToMonitorIdMap.find(consoleId);
			if(consoleToMonitorIdIt!=m_consoleKeyToMonitorIdMap.end())
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MonitorID = %d ", (*consoleToMonitorIdIt).second);

				std::map<TA_COE::AlarmState, AlarmID>::iterator it;
				
				TA_Base_Core::ThreadGuard threadStateGuard( m_openAlarmsLock );

				for(it = m_openAlarms.begin(); it!=m_openAlarms.end(); it++)
				{
					AlarmID alarm_id = (*it).second;
					if (alarm_id.alarmID == alarmKey)
					{
						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Sending TriggeringEventAckCommand...");
						RequestCommandSharedPtr command = COECommunicationsHandler::RequestCommandSharedPtr(new TriggeringEventAckCommand((*it).first, (*consoleToMonitorIdIt).second));
						submitRequestCommand(command, false);
					}
				}
			}
			else
			{
				TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST, 
					"Console does not exsist" ) );            
			}
		}	
	}

	void COECommunicationsHandler::acknowledgeAlarm(unsigned long stationId, unsigned long eventId, unsigned long consoleId)
	{
		//acknowledging alarm on a switch is only valid at the occ
		//where the default alarm monitors are on the wall not on a console
		if( m_locationType == TA_Base_Core::ILocation::OCC )
		{
			std::map<unsigned long, unsigned long>::iterator consoleToMonitorIdIt;
			consoleToMonitorIdIt = m_consoleKeyToMonitorIdMap.find(consoleId);
			if(consoleToMonitorIdIt!=m_consoleKeyToMonitorIdMap.end())
			{
				TA_COE::AlarmState state;
				state.stationId = stationId;
				state.eventId = eventId;
				RequestCommandSharedPtr command = COECommunicationsHandler::RequestCommandSharedPtr(new TriggeringEventAckCommand(state, (*consoleToMonitorIdIt).second));
				submitRequestCommand(command, false);
			}
			else
			{
				TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST, 
						"Console does not exsist" ) );
			}
		}
	}

	void COECommunicationsHandler::acknowledgeTrainCameraAlarm(unsigned long locationId, unsigned long trainId, const std::vector<unsigned long>& cameras, unsigned long consoleId )
	{
		std::map<unsigned long, unsigned long>::iterator consoleToMonitorIdIt;
		consoleToMonitorIdIt = m_consoleKeyToMonitorIdMap.find(consoleId);
		std::map<unsigned long, unsigned long>::iterator locationIdToStageIdIt;
		locationIdToStageIdIt = m_locationKeyToStageIdMap.find(locationId);

		if(consoleToMonitorIdIt!=m_consoleKeyToMonitorIdMap.end())
		{
			if(locationIdToStageIdIt!=m_locationKeyToStageIdMap.end())
			{
				RequestCommandSharedPtr command = COECommunicationsHandler::RequestCommandSharedPtr(new TrainCameraAlarmAckCommand((*locationIdToStageIdIt).second, (*consoleToMonitorIdIt).second));
				submitRequestCommand(command, false);
			}
			else
			{
				TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST, 
					"location does not exsist" ) );
			}
		}
		else
		{
			TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST, 
					"Console does not exsist" ) );
		}	
	}

	void COECommunicationsHandler::closeAlarm(const std::string& alarmKey/*, unsigned long entityKey*/)
	{
		//std::string alarmIdString(alarmId);

		std::map<TA_COE::AlarmState, AlarmID>::iterator it;

		TA_Base_Core::ThreadGuard threadStateGuard( m_openAlarmsLock );

		for(it = m_openAlarms.begin(); it!=m_openAlarms.end(); it++)
		{
			AlarmID alarm_id = (*it).second;
			if (alarm_id.alarmID == alarmKey)
			{
				if ( canbeManuallyClosed((*it).first) ) // TD16201
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Sending TriggeringEventClearCommand...");
					RequestCommandSharedPtr command = COECommunicationsHandler::RequestCommandSharedPtr(new TriggeringEventClearCommand((*it).first));
					submitRequestCommand(command, false);
				}
				else
				{
					TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR, 
						"The alarm not configured to be manually closed" ) );					
				}
			}
		}
	}

	void COECommunicationsHandler::closeAlarm(unsigned long stationId, unsigned long eventId)
	{
		TA_COE::AlarmState state;
		state.stationId = stationId;
		state.eventId = eventId;
		RequestCommandSharedPtr command = COECommunicationsHandler::RequestCommandSharedPtr(new TriggeringEventClearCommand(state));
		submitRequestCommand(command, false);
	}

	TA_Base_Bus::SwitchManagerCorbaDef::AlarmStack COECommunicationsHandler::getAlarmStack()
	{
		TA_Base_Bus::SwitchManagerCorbaDef::AlarmStack stack;
		TA_Base_Core::ThreadGuard threadStateGuard( m_openAlarmsLock );

		stack.length(m_alarmStack.size());
		std::map<TA_COE::AlarmState, AlarmID>::iterator itr;
		unsigned int index(0);
		for(itr=m_openAlarms.begin(); itr!=m_openAlarms.end(); itr++)
		{
			TA_Base_Bus::SwitchManagerCorbaDef::AlarmState state;
			state.eventId = (*itr).first.eventId;
			state.stationId = (*itr).first.stationId;
			state.alarmType = CORBA::string_dup((*itr).second.alarmType.c_str());
			state.eventId   = (*itr).second.entityKey;
			state.alarmID   = CORBA::string_dup((*itr).second.alarmID.c_str()); // TD16164
			stack[index++] = state;
		}

		return stack;
	}

	void COECommunicationsHandler::updateAlarmStack(TA_Base_Bus::SwitchManagerCorbaDef::AlarmStack& stack)
	{
		TA_Base_Core::ThreadGuard threadStateGuard( m_openAlarmsLock );
		m_alarmStack.clear();
		m_openAlarms.clear();

		for(unsigned int i=0; i<stack.length(); i++)
		{
			TA_COE::AlarmState state;
			state.eventId = stack[i].eventId;
			state.stationId = stack[i].stationId;
			m_alarmStack.push_back(state);

			// TD15853
			AlarmID alarm_id;
			alarm_id.alarmType = stack[i].alarmType;
			alarm_id.entityKey = stack[i].entityKey;
			alarm_id.alarmID = stack[i].alarmID.in(); // TD16164

			m_openAlarms[state] = alarm_id;
		}
	}

	unsigned long COECommunicationsHandler::getEntityKey()
	{
		return m_videoSwitchAgentData->getKey();
	}


	// TD16201
	bool COECommunicationsHandler::canbeManuallyClosed(const TA_COE::AlarmState& alarmState)
	{
		std::map<TA_COE::AlarmState, VideoTriggeringEventsList>::iterator eventsConfigItr = m_triggeringEventsConfigMap.find(alarmState);
		if(eventsConfigItr != m_triggeringEventsConfigMap.end())
		{
			TA_IRS_Core::IVideoTriggeringEvent* eventConfig = NULL;
			// loop through the VideoTriggeringEventsList to get CctvTriggeringAlarm entity for this location
			VideoTriggeringEventsList eventList = (*eventsConfigItr).second;
			for (VideoTriggeringEventsList::iterator it = eventList.begin(); it != eventList.end(); it++)
			{
				TA_Base_Core::IEntityData* entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity((*it)->getEntityKey());
				if(0 == entityData)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot get the entity data for eventId %d",alarmState.eventId);
					continue;
				}

				if (entityData->getLocation() == m_videoSwitchAgentData->getLocation())
				{
					return (*it)->getManualClosing();
				}
				else
				{
					delete entityData;
					entityData = NULL;
				}
			}
		}

		return false;
	}

    void COECommunicationsHandler::setStageSwitchMode( unsigned long originatingStage, unsigned long destinationStage, bool allowSwitch )
    {
        FUNCTION_ENTRY( "setStageSwitchMode" );

        RequestCommandSharedPtr command = COECommunicationsHandler::RequestCommandSharedPtr( new SetStageSwitchModeCommand( originatingStage,
                                                                                                                            destinationStage,
                                                                                                                            allowSwitch ) );
        submitRequestCommand(command, false);

        FUNCTION_EXIT;
    }

} // TA_IRS_App
