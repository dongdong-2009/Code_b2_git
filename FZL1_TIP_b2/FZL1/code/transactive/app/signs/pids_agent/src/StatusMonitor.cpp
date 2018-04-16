/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3002_TIP/3002/transactive/app/signs/pids_agent/src/StatusMonitor.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/10/01 16:36:38 $
  * Last modified by:  $Author: grace.koh $
  *
  */
#ifdef _MSC_VER
#pragma warning( disable : 4503 )
#endif // _MSC_VER

#include "app/signs/pids_agent/src/StatusMonitor.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/signs/pids_agent/src/PIDSManager.h"
#include "app/signs/pids_agent/src/Exceptions.h"
#include "app/signs/pids_agent/src/AgentModeMonitor.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/exceptions/src/DataException.h"
#include "core/message/types/ATSAgentComms_MessageTypes.h"
#include "core/message/src/MessageSubscriptionManager.h"


namespace TA_IRS_App
{

//    using TA_Base_Bus::DataPoint;

//    using TA_Base_Bus::DataPointState;

//    using TA_Base_Bus::DpValue;



    using TA_Base_Core::DebugUtil;

	//
    // Initialise statics.
    //

	TA_Base_Core::ReEntrantThreadLockable StatusMonitor::m_singletonLock;
	TA_Base_Core::ReEntrantThreadLockable StatusMonitor::m_atsMessageLock;

	StatusMonitor* StatusMonitor::m_theClassInstance = NULL;

	const int sendplatforminterval = 4;  // send platform info every 4 seconds.
	StatusMonitor::StatusMonitor()
      : m_shouldBeRunning( true ),
//        m_atsDataInitialised( false ),
		m_timer( TA_Base_Core::SingletonTimerUtil::getInstance() )//TD15277
	{
		// Note - the library versions above are initialised for testing purposes only
	}

	StatusMonitor::~StatusMonitor()
	{
/*		std::vector< DataPointDetails* >::iterator dpIter;
		for ( dpIter = m_virtualDataPointList.begin(); 
			  dpIter != m_virtualDataPointList.end(); dpIter++)
		{
			delete (*dpIter);
		}
*/
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

		//TD15277
		//start timer to send platform infomation periodically
		m_timer.startPeriodicTimeOutClock( this, sendplatforminterval*1000, false );

		
/*
		while( m_shouldBeRunning )
		{
			if( AgentModeMonitor::getInstance().isInControlMode() )
			{
				if( m_locationType == TA_Base_Core::ILocation::OCC )
				{
					PIDSManager::getInstance()->submitOCCStatusRequestMessage();
				}
				else if( m_locationType != TA_Base_Core::ILocation::DPT ) // Do not want to poll the PIDS server at the depot
				{
					PIDSManager::getInstance()->submitStationStatusRequestMessage();
				}				
			}

			this->sleep(m_statusPollInterval);
		}
*/
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
/*
	void StatusMonitor::initialise( TA_Base_Core::PIDSAgentEntityDataPtr agentData,
                                    TA_Base_Core::ILocation::ELocationType locationType )
	{
		m_location = agentData->getLocationName();

        m_locationType = locationType;

		m_agentAssetName = agentData->getAssetName();

		// TODO - remove hardcoding of agent name
		unsigned long locationKey = PIDSManager::getInstance()->getLocationKey();

        TA_Base_Core::CorbaNameList atsCorbaNameList = 
            TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation(
			TA_Base_Core::AtsAgentEntityData::getStaticType(),
                                                locationKey,true);

        // no ATS agent
        if ( atsCorbaNameList.size() < 1 )
        {
            TA_THROW(TA_Base_Core::DataException("No ATS agent at this location", TA_Base_Core::DataException::NO_VALUE, "") );
        }
        // multiple PIDS agents
        else if ( atsCorbaNameList.size() > 1 )
        {
            TA_THROW(TA_Base_Core::DataException("Multiple ATS agents at this location", TA_Base_Core::DataException::NOT_UNIQUE, "") );
        }

		m_atsAgent.setCorbaName( atsCorbaNameList[0] );

		// Get the current PIDS-related ATS info from the ATS agent
		// Note: if this fails due to unavailability of the ATS agent etc - since the PIDS agent is subscribing
		// to ATS/PIDS messages, the first time a message is broadcast by the
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
*/


/*
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
					((*iter)->dataPointName.compare(PIDS_OCC_SERVER_STATUS_DP_NAME) == 0 ) )
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
*/
	void StatusMonitor::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& message )
	{
/*		TA_Base_Core::ThreadGuard guard( m_atsMessageLock );

		// if the message is the right type
		if( std::string(message.messageTypeKey) ==
			TA_Base_Core::ATSAgentComms::AtsPidsUpdate.getTypeKey() )
		{
			// Only process this message if the ATS train/platform data
			// has already been successfully obtained from the ATS agent
			if( m_atsDataInitialised )
			{
				TA_IRS_Bus::IAtsPidsCorbaDef::AtsPidsMessageCorbaDef* atsPidsMessage;
				
				//
				// Extract to the AtsPidsMessage
				//
				if( ( message.messageState >>= atsPidsMessage ) != 0 )
				{
                    // if this is the OCC, send all messages on to the hardware
                    // if not, then make sure the message is for the current location before
                    // sending it to the hardware
					//TD15277
					processAtsMessage(*atsPidsMessage);			
					
				}
				else
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
								"Could not extract AtsPidsMessageCorbaDef from CommsMessageCorbaDef");
					return;
				} 
			}
			else // Otherwise attempt to obtain the ATS train/platform info for the first time (the agent is now obviously alive)
			{
				// Get the current PIDS-related ATS info from the ATS agent
				getInitialATSInformation();
			}
		}

*/
	}

	void StatusMonitor::subscribeToNotifications()
	{
		//
		// Get the entity, subsystem and location key.
		// There should only be a single entity at a given location.
		//
/*		unsigned long locationKey = PIDSManager::getInstance()->getLocationKey();

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
			TA_Base_Core::ATSAgentComms::AtsPidsUpdate,
			this,
			entityKey,
			subsystemKey,
			locationKey);
*/
	}


	void StatusMonitor::getInitialATSInformation()
	{
/*
		try
		{
			//TD15277
			TA_IRS_Bus::IAtsPidsCorbaDef::PidsInfoList_var list ;
			CORBA_CALL_RETURN( list,
			                   m_atsAgent,
			                   getPidsInformationList,
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
*/
	}

/*
//TD15277++
void StatusMonitor::processAtsMessage( const TA_IRS_Bus::IAtsPidsCorbaDef::AtsPidsMessageCorbaDef& atsPidsMessage )
    {
        // if this is the OCC, send all messages on to the hardware
        // if not, then make sure the message is for the current location before
        // sending it to the hardware
		if( (TA_Base_Core::ILocation::OCC == PIDSManager::getInstance()->getLocationType()) ||
            (atsPidsMessage.stationId == PIDSManager::getInstance()->getLocationKey()) )
		{
    		std::vector< AtsPidsData >::iterator PidsIter;
			for ( PidsIter = m_atsInfoList.begin();  PidsIter != m_atsInfoList.end(); ++PidsIter)
			{			
				// If the platform is found, update with the information just received
				if( (atsPidsMessage.stationId == PidsIter->stationId ) && 
					(atsPidsMessage.platformId == PidsIter->platformId ) ) //&&
				//	(atsPidsMessage->message._d() == PidsIter->message._d()) )
				{
                    break;
				}		
			}

            // if not found, add a new entry
			if( PidsIter == m_atsInfoList.end() )
			{
				// create a new entry in m_atsInfoList
                // set all the fields to blank

				AtsPidsData atsPidsitem;
				
				atsPidsitem.stationId = atsPidsMessage.stationId;
				atsPidsitem.platformId = atsPidsMessage.platformId;
				atsPidsitem.PidsPlatformId = atsPidsMessage.PidsPlatformId;
				
				if( atsPidsMessage.message._d() == TA_IRS_Bus::IAtsPidsCorbaDef::PLATFORM_INFORMATION)
				{
					
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Received a Platform information update message from ATS agent");
					
                    // set the platform info
					atsPidsitem.platformInformation = atsPidsMessage.message.platformInformation();

                    // set all the fields to 0 in train info
                   
					atsPidsitem.trainInformation.firstTrain.valid = false;
					atsPidsitem.trainInformation.firstTrain.physicalTrainNumber = 0;
					atsPidsitem.trainInformation.firstTrain.serviceNumber = 0;
					atsPidsitem.trainInformation.firstTrain.destinationStationId = 0;
					atsPidsitem.trainInformation.firstTrain.lastTrain = false;
					atsPidsitem.trainInformation.firstTrain.predictedTime.year = 0;
					atsPidsitem.trainInformation.firstTrain.predictedTime.month = 0;
					atsPidsitem.trainInformation.firstTrain.predictedTime.day = 0;
					atsPidsitem.trainInformation.firstTrain.predictedTime.hour = 0;
					atsPidsitem.trainInformation.firstTrain.predictedTime.minute = 0;
					atsPidsitem.trainInformation.firstTrain.predictedTime.second = 0;
					atsPidsitem.trainInformation.firstTrain.stationType = false;

					atsPidsitem.trainInformation.secondTrain.valid = false;
					atsPidsitem.trainInformation.secondTrain.physicalTrainNumber = 0;
					atsPidsitem.trainInformation.secondTrain.serviceNumber = 0;
					atsPidsitem.trainInformation.secondTrain.destinationStationId = 0;
					atsPidsitem.trainInformation.secondTrain.lastTrain = false;
					atsPidsitem.trainInformation.secondTrain.predictedTime.year = 0;
					atsPidsitem.trainInformation.secondTrain.predictedTime.month = 0;
					atsPidsitem.trainInformation.secondTrain.predictedTime.day = 0;
					atsPidsitem.trainInformation.secondTrain.predictedTime.hour = 0;
					atsPidsitem.trainInformation.secondTrain.predictedTime.minute = 0;
					atsPidsitem.trainInformation.secondTrain.predictedTime.second = 0;
					atsPidsitem.trainInformation.secondTrain.stationType = false;

					
                }
				else if ( atsPidsMessage.message._d() == TA_IRS_Bus::IAtsPidsCorbaDef::TRAIN_INFORMATION)
				{
					
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Received a Train information update message from ATS agent");
					
                    // set the train info
					atsPidsitem.trainInformation = atsPidsMessage.message.trainInformation(); 

                    // set the fields to 0 in platform info
                    
					atsPidsitem.platformInformation.preArrival = false;  
					atsPidsitem.platformInformation.arrival  = false;   
					atsPidsitem.platformInformation.preDeparture = false;
					atsPidsitem.platformInformation.departure = false;  
					atsPidsitem.platformInformation.skip = false;
					atsPidsitem.platformInformation.hold = false;
					atsPidsitem.platformInformation.evacuation = false;
					atsPidsitem.platformInformation.trainOverrun = false;
					atsPidsitem.platformInformation.validity = false; 

    			}			
				
				m_atsInfoList.push_back(atsPidsitem);
              
			}
            else
            {
			    // if found, should update the corresponding item in m_atsInfoList
                switch( atsPidsMessage.message._d() )
			    {
			    case TA_IRS_Bus::IAtsPidsCorbaDef::PLATFORM_INFORMATION:

				    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Received a Platform information update message from ATS agent");

				    PidsIter->platformInformation = atsPidsMessage.message.platformInformation() ;

	   			    break;

			    case TA_IRS_Bus::IAtsPidsCorbaDef::TRAIN_INFORMATION:

				    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Received a Train information update message from ATS agent");

				    PidsIter->trainInformation = atsPidsMessage.message.trainInformation() ; 

				    break;

			    }
            }
		}
	}
*/
	void StatusMonitor::timerExpired(long timerId, void* userData)
	{	
/*
		std::vector< AtsPidsData >::iterator PidsIter;

		for ( PidsIter = m_atsInfoList.begin();  PidsIter != m_atsInfoList.end(); ++PidsIter)		
		{
			try
			{
				// Send an ATS info message to the PIDS hardware
				if (PidsIter->platformInformation.validity)
				{	
//					PIDSManager::getInstance()->submitTrainInformationMessage( PidsIter->stationId, PidsIter->PidsPlatformId, PidsIter->platformInformation, PidsIter->trainInformation );					
				}
				else
				{
//					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "ATS Platform Information message not sent - Platform validity is false");
				}
			}
			catch( const TA_Base_Bus::IPIDSManagerCorbaDef::PIDSServerNotConnectedException& ex )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "ATS Platform Information message not sent - not connected to the PIDS server");
			}
			catch( const TA_IRS_Core::PIDSAgentException& ex )
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
*/
	}
//++TD15277

}  // namespace TA_IRS_App
