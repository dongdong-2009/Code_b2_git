/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/STISStationStatusRequest.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #5 $
  *
  * Last modification: $DateTime: 2014/03/31 14:20:27 $
  * Last modified by:  $Author: huang.wenbo $
  *
  */

#include "app/signs/tis_agent/src/STISStationStatusRequest.h"
#include "app/signs/tis_agent/src/STISManager.h"
#include "app/signs/tis_agent/src/AgentModeMonitor.h"
#include "app/signs/tis_agent/src/StatusMonitor.h"
#include "app/signs/tis_agent/src/GlobalConstant.h"

namespace TA_IRS_App
{
    using TA_Base_Bus::DataPoint;
    using TA_Base_Bus::DataPointState;
    using TA_Base_Bus::DpValue;


	STISStationStatusRequest::STISStationStatusRequest(const std::string& location)
	{
		m_location = location;
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

		// Now the data length - in this case it will always be 6 bytes
		messageStream << "0006";

		// Now the actual data,4 byte station name and two space reserved
		messageStream << getStationIdString(m_location);
		std::string messageString = messageStream.str();
		messageVector.push_back( 0x02 );
		for(unsigned long i=0;i < messageString.size() ; i++)
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
		unsigned long newStationLibraryVersion;
		unsigned long currentStationLibraryVersion;
		int numberOfPIDs;
		bool connectionLinkAlarm;

		// length (in bytes) of the actual data in the response message
		int dataLength;

		// Extract the fields from the data section

		// Data length 
		std::string dataLengthStr(m_responseMessage.begin()+22, m_responseMessage.begin()+26 );
		// Report Station
		std::string reportStationStr(m_responseMessage.begin()+26, m_responseMessage.begin()+32 );
		// Connection link Summary alarm
		std::string connectionLinkStr(m_responseMessage.begin()+32, m_responseMessage.begin()+33 );
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
			char pidStatus[2] = {0};
			int i;
			int status;
			for( i = 0; i < numberOfPIDs ; i++ )
			{
				std::copy(m_responseMessage.begin()+(statusPos),m_responseMessage.begin()+(statusPos+1),pidStatus);
				status = atoi( pidStatus );

				// submit this status to the STIS Manager, the address is simply the array
				// index +1, since PIDs are 1 - n
				StatusMonitor::getInstance()->updatePidStatus(i+1, status);
				statusStream << status;
				statusPos++;
			}

			std::string statusString;
			statusString = statusStream.str();

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, statusString.c_str());

			dataLength = atoi(dataLengthStr.c_str());
			connectionLinkAlarm = (bool)atoi(connectionLinkStr.c_str());
			currentStationLibraryVersion = strtoul(currentSTISLibraryVersionStr.c_str(), NULL, 10);
			newStationLibraryVersion = strtoul(newSTISLibraryVersionStr.c_str(), NULL, 10);

			std::stringstream resourceStatus;
			resourceStatus << "Status of Resources: ";
			resourceStatus << " Connection link:";
			resourceStatus << connectionLinkAlarm;

			std::string resourceStr;

			resourceStr = resourceStatus.str();

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, resourceStr.c_str());

			// Update the state of the resource datapoints

			// Connection Link Summary Point
			StatusMonitor::getInstance()->updateConnectionLinkAlarmDataPointState(connectionLinkAlarm);
			StatusMonitor::getInstance()->setCurrentSTISStationLibraryVersion( currentStationLibraryVersion );
			StatusMonitor::getInstance()->setNextSTISStationLibraryVersion( newStationLibraryVersion );
		}
	}
}
