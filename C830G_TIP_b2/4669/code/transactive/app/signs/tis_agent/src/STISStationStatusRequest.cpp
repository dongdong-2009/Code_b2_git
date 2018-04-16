/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/STISStationStatusRequest.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/signs/tis_agent/src/STISStationStatusRequest.h"
#include "app/signs/tis_agent/src/STISManager.h"
#include "app/signs/tis_agent/src/AgentModeMonitor.h"
#include "app/signs/tis_agent/src/StatusMonitor.h"

namespace TA_IRS_App
{
    using TA_Base_Bus::DataPoint;
    using TA_Base_Bus::DataPointState;
    using TA_Base_Bus::DpValue;


	STISStationStatusRequest::STISStationStatusRequest()
	{
		m_messageIDString = "M30";
	}


	STISStationStatusRequest::~STISStationStatusRequest()
	{
	}


	std::vector<unsigned char> STISStationStatusRequest::createProtocolMessage()
	{
		std::vector<unsigned char> messageVector;
		std::stringstream messageStream;

		// Add the sequence number
    	messageStream << getSequenceNumberAsString();

		// Add the timestamp to the message
		messageStream << m_timestamp;

		// Now the message ID
		messageStream << m_messageIDString; // M30

		// Now the data length - in this case it will always be 3 bytes
		messageStream << "0003";

		// Now the actual data
		// This message only contains the 3-character station ID
		messageStream << STISManager::getInstance()->getLocationName();

		std::string messageString = messageStream.str();

		messageVector.push_back( 0x02 );
		for(unsigned int i=0;i < messageString.size() ; i++)
		{
			messageVector.push_back( messageString[i] );
		}
		messageVector.push_back( 0x03 );
				
		return messageVector;
	}


	void STISStationStatusRequest::log(bool sendSuccess)
	{
	}


	void STISStationStatusRequest::processResponse()
	{
		int newStationLibraryVersion;
		int newTrainLibraryVersion;
		int currentStationLibraryVersion;
		int currentTrainLibraryVersion;
		int numberOfPIDs;
		bool clockLinkAlarm;
		bool connectionLinkAlarm;
		bool ledSummaryAlarm;
		bool plasmaSummaryAlarm;
 
		// length (in bytes) of the actual data in the response message
		int dataLength;

		// Extract the fields from the data section


		// Data length 
		std::string dataLengthStr(m_responseMessage.begin()+22, m_responseMessage.begin()+26 );
		// Report Station
		std::string reportStationStr(m_responseMessage.begin()+26, m_responseMessage.begin()+29 );
		// LED Summary alarm
		std::string ledSummaryStr(m_responseMessage.begin()+29, m_responseMessage.begin()+30 );
		// Plasma Summary alarm
		std::string plasmaSummaryStr(m_responseMessage.begin()+30, m_responseMessage.begin()+31 );
		// Connection link Summary alarm
		std::string connectionLinkStr(m_responseMessage.begin()+31, m_responseMessage.begin()+32 );
		// Clock link Summary alarm
		std::string clockLinkStr(m_responseMessage.begin()+32, m_responseMessage.begin()+33 );
		// current STIS library version
		std::string currentSTISLibraryVersionStr(m_responseMessage.begin()+33, m_responseMessage.begin()+36 );
		// new STIS library version
		std::string newSTISLibraryVersionStr(m_responseMessage.begin()+36, m_responseMessage.begin()+39 );
		// Number of report PIDs
		std::string numberOfPIDsStr(m_responseMessage.begin()+39, m_responseMessage.begin()+41 );

		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			// Now read the status of each PID
			// The assumption is made that the status of all PIDs is supplied
			// and they are in order of address 1 - n

			std::stringstream statusStream;
			
			statusStream << "Status of PIDS: ";

			int statusPos = 41; // Position of last character above
			numberOfPIDs = atoi( numberOfPIDsStr.c_str() );	
			char pidStatus[1];
			int i;
			int status;
			for( i = 0; i < numberOfPIDs ; i++ )
			{
				std::copy(m_responseMessage.begin()+(statusPos),m_responseMessage.begin()+(statusPos+1),pidStatus);
				status = atoi( pidStatus );

				// submit this status to the STIS Manager, the address is simply the array
				// index +1, since PIDs are 1 - n
				STISManager::getInstance()->updatePIDStatus(i+1,status);

				statusStream << status;

				statusPos++;
			}

			std::string statusString;
			statusString = statusStream.str();

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, statusString.c_str());

			dataLength = atoi(dataLengthStr.c_str());
			clockLinkAlarm = (bool)atoi(clockLinkStr.c_str());
			connectionLinkAlarm = (bool)atoi(connectionLinkStr.c_str());
			ledSummaryAlarm = (bool)atoi(ledSummaryStr.c_str());
			plasmaSummaryAlarm = (bool)atoi(plasmaSummaryStr.c_str());
			currentStationLibraryVersion = atoi(currentSTISLibraryVersionStr.c_str());
			newStationLibraryVersion = atoi(newSTISLibraryVersionStr.c_str());

			std::stringstream resourceStatus;
			resourceStatus << "Status of Resources: ";
			resourceStatus << "LED Summary:";
			resourceStatus << ledSummaryAlarm;
			resourceStatus << " Plasma Summary:";
			resourceStatus << plasmaSummaryAlarm;
			resourceStatus << " Connection link:";
			resourceStatus << connectionLinkAlarm;
			resourceStatus << " Clock link:";
			resourceStatus << clockLinkAlarm;

			std::string resourceStr;

			resourceStr = resourceStatus.str();

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, resourceStr.c_str());

			// Update the state of the resource datapoints

			// Clock Link Summary Point
			try
			{
				DpValue resourcePointValue = StatusMonitor::getInstance()->getCurrentDataPointValue(CLOCK_LINK_SUMMARY_ALARM_DP_NAME, STIS_SERVER_EQUIPMENT_NAME);
				DataPointState resourcePointState(resourcePointValue, time(NULL), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );

				resourcePointValue.setBoolean( clockLinkAlarm );  
				resourcePointState.setValue(resourcePointValue );
				resourcePointState.setStatus( TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
				resourcePointState.setTimestamp( time( NULL ) );
				StatusMonitor::getInstance()->updateDataPointState( CLOCK_LINK_SUMMARY_ALARM_DP_NAME , resourcePointState, STIS_SERVER_EQUIPMENT_NAME, false );
			}
			catch (TA_Base_Core::DataException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
			}

			// Connection Link Summary Point
			try
			{
				DpValue resourcePointValue = StatusMonitor::getInstance()->getCurrentDataPointValue(CONNECTION_LINK_SUMMARY_ALARM_DP_NAME, STIS_SERVER_EQUIPMENT_NAME);
				DataPointState resourcePointState(resourcePointValue, time(NULL), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );

				resourcePointValue.setBoolean( connectionLinkAlarm );  
				resourcePointState.setValue(resourcePointValue );
				resourcePointState.setStatus( TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
				resourcePointState.setTimestamp( time( NULL ) );
				StatusMonitor::getInstance()->updateDataPointState( CONNECTION_LINK_SUMMARY_ALARM_DP_NAME , resourcePointState, STIS_SERVER_EQUIPMENT_NAME, false );
			}
			catch (TA_Base_Core::DataException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
			}

			// LED Display Summary Point
			try
			{
				DpValue resourcePointValue = StatusMonitor::getInstance()->getCurrentDataPointValue(LED_DISPLAY_SUMMARY_ALARM_DP_NAME, STIS_SERVER_EQUIPMENT_NAME);
				DataPointState resourcePointState(resourcePointValue, time(NULL), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );

				resourcePointValue.setBoolean( ledSummaryAlarm );  
				resourcePointState.setValue( resourcePointValue );
				resourcePointState.setStatus( TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
				resourcePointState.setTimestamp( time( NULL ) );
				StatusMonitor::getInstance()->updateDataPointState( LED_DISPLAY_SUMMARY_ALARM_DP_NAME , resourcePointState, STIS_SERVER_EQUIPMENT_NAME, false ); 
			}
			catch (TA_Base_Core::DataException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
			}

			// Plasma Display Summary Point
			try
			{
				DpValue resourcePointValue = StatusMonitor::getInstance()->getCurrentDataPointValue(PLASMA_DISPLAY_SUMMARY_ALARM_DP_NAME, STIS_SERVER_EQUIPMENT_NAME);
				DataPointState resourcePointState(resourcePointValue, time(NULL), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );

				resourcePointValue.setBoolean( plasmaSummaryAlarm );  
				resourcePointState.setValue(resourcePointValue );
				resourcePointState.setStatus( TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
				resourcePointState.setTimestamp( time( NULL ) );
				StatusMonitor::getInstance()->updateDataPointState( PLASMA_DISPLAY_SUMMARY_ALARM_DP_NAME , resourcePointState, STIS_SERVER_EQUIPMENT_NAME, false );
			}
			catch (TA_Base_Core::DataException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
			}

			StatusMonitor::getInstance()->setCurrentSTISStationLibraryVersion( currentStationLibraryVersion );
			StatusMonitor::getInstance()->setNextSTISStationLibraryVersion( newStationLibraryVersion );

		}

	}
	
}