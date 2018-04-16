/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/STISOCCStatusRequest.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #10 $
  *
  * Last modification: $DateTime: 2014/07/23 13:32:29 $
  * Last modified by:  $Author: huang.wenbo $
  *
  */

#include "app/signs/tis_agent/src/STISOCCStatusRequest.h"
#include "app/signs/tis_agent/src/STISManager.h"
#include "app/signs/tis_agent/src/StatusMonitor.h"
#include "app/signs/tis_agent/src/MessageLibraryManager.h"
#include "app/signs/tis_agent/src/AgentModeMonitor.h"
#include "app/signs/tis_agent/src/Exceptions.h"
#include "app/signs/tis_agent/src/GlobalConstant.h"
#include <iomanip>

namespace TA_IRS_App
{

    using TA_Base_Bus::DataPoint;
    using TA_Base_Bus::DataPointState;
    using TA_Base_Bus::DpValue;

    // initialise it to not having received an RXH message 
    int STISOCCStatusRequest::m_previousRXHMessageAck = 0;

	STISOCCStatusRequest::STISOCCStatusRequest(bool isTimeScheduleValid):m_isTimeScheduleValid(isTimeScheduleValid)
	{
		m_messageIDString = "M31";
	}


	STISOCCStatusRequest::~STISOCCStatusRequest()
	{
	}


	void STISOCCStatusRequest::log(bool sendSuccess)
	{
    }


	void STISOCCStatusRequest::processResponse()
	{
		// STIS server library versions
		unsigned long nextSTISStationLibraryVersion;
		unsigned long currentSTISStationLibraryVersion;

		// ISCS server library versions
		unsigned long currentISCSStationLibraryVersion;
		unsigned long nextISCSStationLibraryVersion;

		bool iscsStationLibrariesSynchronised;

		bool connectionLinkAlarm;
		bool stisOccServerAlarm;
 
		// length (in bytes) of the actual data in the response message
		int dataLength;

		// Extract the fields from the data section

		// Data length 
		std::string dataLengthStr(m_responseMessage.begin()+22, m_responseMessage.begin()+26 );
		// Connection link status
		std::string connectionLinkStr(m_responseMessage.begin()+26,m_responseMessage.begin()+27 );
		// STIS OCC server status
		std::string stisOccServerStr(m_responseMessage.begin()+27,m_responseMessage.begin()+28 );
		// Next stis message library file id
		std::string stisLibraryFileName(m_responseMessage.begin()+28,m_responseMessage.begin()+44 );
		// current STIS library version
		std::string currentSTISLibraryVersionStr(m_responseMessage.begin()+44,m_responseMessage.begin()+47 );
		// next STIS library version
		std::string nextSTISLibraryVersionStr(m_responseMessage.begin()+47,m_responseMessage.begin()+50 );
		// time schedule download status
		std::string timeScheduleDownloadStatusStr(m_responseMessage.begin()+50, m_responseMessage.begin()+51);

        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
			"Common info: \ndata length = %s, \nconnection link = %s, \nstis occ server link = %s, \nstis library[Curr v%s][Next v%s], \nnext stis library file name = %s",
            dataLengthStr.c_str(),
            connectionLinkStr.c_str(),
            stisOccServerStr.c_str(),
            currentSTISLibraryVersionStr.c_str(),
            nextSTISLibraryVersionStr.c_str(),
            stisLibraryFileName.c_str());

		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			dataLength = atoi(dataLengthStr.c_str());
			connectionLinkAlarm = (bool)atoi(connectionLinkStr.c_str());
			stisOccServerAlarm = (bool)atoi(stisOccServerStr.c_str());
			currentSTISStationLibraryVersion = strtoul(currentSTISLibraryVersionStr.c_str(), NULL, 10);
			nextSTISStationLibraryVersion = strtoul(nextSTISLibraryVersionStr.c_str(), NULL, 10);
			
			std::stringstream resourceStatus;
			resourceStatus << "Status of Resources: ";
			resourceStatus << " Connection link:";
			resourceStatus << connectionLinkAlarm;
			resourceStatus << " STIS Server:";
			resourceStatus << stisOccServerAlarm;

			std::string resourceStr;
			resourceStr = resourceStatus.str();
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, resourceStr.c_str());

			//update time schedule download status, equal 0 mean timeschedule download in progress
			StatusMonitor::getInstance()->setTimeScheduleDownloadStatus( "0" == timeScheduleDownloadStatusStr );
			

			// Update the state of the resource datapoints

			// Connection Link Summary Point
			StatusMonitor::getInstance()->updateConnectionLinkAlarmDataPointState(connectionLinkAlarm);

			// OCC TIS Server Summary Point
			StatusMonitor::getInstance()->updateOccServerStatusAlarmDataPointState(stisOccServerAlarm);

			// STIS library version DataPoint
			StatusMonitor::getInstance()->setCurrentSTISStationLibraryVersion( currentSTISStationLibraryVersion );
			StatusMonitor::getInstance()->setNextSTISStationLibraryVersion( nextSTISStationLibraryVersion );
			
			// Grab a copy of the ISCS server library versions to make the checks below more readable
			currentISCSStationLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSStationLibraryVersion_central();
			nextISCSStationLibraryVersion = StatusMonitor::getInstance()->getNextISCSStationLibraryVersion_central();
			iscsStationLibrariesSynchronised = StatusMonitor::getInstance()->getStationLibrariesSynchronised();

			std::stringstream stisVersionMessage;

			stisVersionMessage << "\nCurrent STIS server station library version: " << currentSTISStationLibraryVersion << "\n";
			stisVersionMessage << "Next STIS server station library version:    " << nextSTISStationLibraryVersion << "\n";
			stisVersionMessage << "Current ISCS station library version: " << currentISCSStationLibraryVersion << "\n";
			stisVersionMessage << "Next ISCS server station library version: " << nextISCSStationLibraryVersion << "\n";
			stisVersionMessage << "Station libraries ";
			stisVersionMessage << (iscsStationLibrariesSynchronised ? "ARE synchronised \n" : "ARE NOT synchronised \n");

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						stisVersionMessage.str().c_str() );

			// See if there is a new STIS or TTIS message library file to process
			if( currentISCSStationLibraryVersion != currentSTISStationLibraryVersion )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
						"The STIS server has a different STIS predefined message library to the TISAgent's current STIS predefined message library");
			}

			if(nextISCSStationLibraryVersion < nextSTISStationLibraryVersion
				|| (nextISCSStationLibraryVersion == 255 && nextSTISStationLibraryVersion == 1))
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						"A new STIS predefined message library is available for download");
				try
				{
					if( ( STISManager::getInstance()->stationLibrarySynchInProgress() == false ) )
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						"Attempting to process new STIS message library version");

						STISManager::getInstance()->processNewMessageLibrary( TA_IRS_App::STATION_LIBRARY, stisLibraryFileName, nextSTISStationLibraryVersion );
					}
					else
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
						"however, a previous station message library synchronisation is in progress");
					}
				}
				catch( const TA_IRS_Core::TISAgentException& e)
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, e.what()); 
				}
				catch(...)
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
						"Could not process new STIS predefined message library");
				}
			}

			// Only need to compare our current version to the indicated new version - the STIS doesnt have a current
			// version of the TTIS library


			// We're only interested in a new library file,
			// if the previous one has been synchronised and upgraded, and the
			// version that the STIS is telling us about is different from our current library version
		}
	}


    std::vector<unsigned char> STISOCCStatusRequest::createProtocolMessage()
    {
		std::vector<unsigned char> messageVector;
		std::stringstream messageStream;

		// Add the sequence number
    	messageStream << getSequenceNumberAsString();
	
		// Add the timestamp to the message
		messageStream << m_timestamp;

		// Now the message ID
		messageStream << m_messageIDString; // M31

		// Now the data length - in this case it will always be 7 byte
		messageStream << "0007";

		// Now the actual data
		messageStream << getStationIdString("KOCC");
		if(m_isTimeScheduleValid || StatusMonitor::getInstance()->isTimeScheduleInDownload())
		{
		    messageStream << "1";
		}
		else
		{
			messageStream << "0";
		}
		
		std::string messageString = messageStream.str();

		messageVector.push_back( 0x02 );
		for(unsigned long i=0;i < messageString.size() ; i++)
		{
			messageVector.push_back( messageString[i] );
		}
		messageVector.push_back( 0x03 );
				
		return messageVector;
    }

	
}