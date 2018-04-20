/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/STISOCCStatusRequest.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/signs/tis_agent/src/STISOCCStatusRequest.h"
#include "app/signs/tis_agent/src/STISManager.h"
#include "app/signs/tis_agent/src/StatusMonitor.h"
#include "app/signs/tis_agent/src/MessageLibraryManager.h"
#include "app/signs/tis_agent/src/AgentModeMonitor.h"
#include "app/signs/tis_agent/src/Exceptions.h"

#include "core/data_access_interface/tis_agent/src/RATISMessageAccessFactory.h"

#include <iomanip>

#include "boost/regex.hpp"

namespace
{
    //
    // Utility for trimming a string (uses boost)
    //
    static const boost::regex e("\\A\\s+|\\s+\\z");
    std::string trim(const std::string & s)
    {
        return boost::regex_replace(s, e, "");
    }
}

namespace TA_IRS_App
{

    using TA_Base_Bus::DataPoint;
    using TA_Base_Bus::DataPointState;
    using TA_Base_Bus::DpValue;

    // initialise it to not having received an RXH message 
    int STISOCCStatusRequest::m_previousRXHMessageAck = 0;

	STISOCCStatusRequest::STISOCCStatusRequest()
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
		int nextSTISStationLibraryVersion;
		int currentSTISStationLibraryVersion;
		int nextSTISTrainLibraryVersion;

		// ISCS server library versions
		int currentISCSStationLibraryVersion;
		int nextISCSStationLibraryVersion;
		int currentISCSTrainLibraryVersion;
		int nextISCSTrainLibraryVersion;

		bool iscsStationLibrariesSynchronised;
		bool iscsTrainLibrariesSynchronised;

		bool rxhLinkAlarm;
		bool clockLinkAlarm;
		bool connectionLinkAlarm;
		bool tisOccServerAlarm;
 
		bool newRATISMessage;

		// length (in bytes) of the actual data in the response message
		int dataLength;

		// Extract the fields from the data section

		// Data length 
		std::string dataLengthStr(m_responseMessage.begin()+22, m_responseMessage.begin()+26 );
		// RXH link status
		std::string rxhLinkStr(m_responseMessage.begin()+26,m_responseMessage.begin()+27 );
		// Clock link status
		std::string clockLinkStr(m_responseMessage.begin()+27,m_responseMessage.begin()+28 );
		// Connection link status
		std::string connectionLinkStr(m_responseMessage.begin()+28,m_responseMessage.begin()+29 );
		// TIS OCC server status
		std::string tisOccServerStr(m_responseMessage.begin()+29,m_responseMessage.begin()+30 );
		// stis message library filename
		std::string stisLibraryFilename(m_responseMessage.begin()+30,m_responseMessage.begin()+46 );
		// current STIS library version
		std::string currentSTISLibraryVersionStr(m_responseMessage.begin()+46,m_responseMessage.begin()+49 );
		// new STIS library version
		std::string newSTISLibraryVersionStr(m_responseMessage.begin()+49,m_responseMessage.begin()+52 );
		// ttis message library filename
		std::string ttisLibraryFilename(m_responseMessage.begin()+52,m_responseMessage.begin()+68 );
		// new TTIS library version
		std::string newTTISLibraryVersionStr(m_responseMessage.begin()+68,m_responseMessage.begin()+71 );
		// RATIS message flag
		std::string newRATISMessageStr(m_responseMessage.begin()+71,m_responseMessage.begin()+72 ); 

		// trim whitespace off the filenames
		stisLibraryFilename = trim(stisLibraryFilename);
		ttisLibraryFilename = trim(ttisLibraryFilename);

        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
			"Common info: \ndata length = %s, \nrxh link = %s, \nclock link = %s, \nconnection link = %s, \nocc server link = %s, \nstis library[Curr v%s][Next v%s] = %s, \nttis library[Next v%s] = %s, \nratis message = %s",
            dataLengthStr.c_str(),
            rxhLinkStr.c_str(),
            clockLinkStr.c_str(),
            connectionLinkStr.c_str(),
            tisOccServerStr.c_str(),
            currentSTISLibraryVersionStr.c_str(),
            newSTISLibraryVersionStr.c_str(),
            stisLibraryFilename.c_str(),
            newTTISLibraryVersionStr.c_str(),
            ttisLibraryFilename.c_str(),
            newRATISMessageStr.c_str());

        m_previousRXHMessageAck = atoi(newRATISMessageStr.c_str());
        newRATISMessage = (m_previousRXHMessageAck == 1);

		if( newRATISMessage )
		{
            m_previousRXHMessageAck = true;
			int endOfDestinationMarker = 0;
            std::string msgdest;  // this should contain the entire MSGDEST section

			std::string systemIdStr(m_responseMessage.begin()+72, m_responseMessage.begin()+75 );
			std::string destinationStr(m_responseMessage.begin()+75, m_responseMessage.begin()+78 );

			if ((0 == destinationStr.compare("AST")) || (0 == destinationStr.compare("ATR")))
			{
				// All stations/trains
				endOfDestinationMarker = 78;
                msgdest = systemIdStr + destinationStr;
			}
			else
			{
				// A list of train numbers/station IDs
				std::string numberOfIDStr(m_responseMessage.begin()+78, m_responseMessage.begin()+81 );
				int numID = atoi( numberOfIDStr.c_str() );

				endOfDestinationMarker = 81 + ( 3 * numID );

				std::string trainListStr(m_responseMessage.begin()+81, m_responseMessage.begin()+ endOfDestinationMarker );
                msgdest = systemIdStr + destinationStr + numberOfIDStr + trainListStr;
			}

			std::string ratisTagStr(m_responseMessage.begin()+endOfDestinationMarker, m_responseMessage.begin()+endOfDestinationMarker+12 );
			std::string startTimeStr(m_responseMessage.begin()+endOfDestinationMarker+12, m_responseMessage.begin()+endOfDestinationMarker+26 );
			std::string endTimeStr(m_responseMessage.begin()+endOfDestinationMarker+26, m_responseMessage.begin()+endOfDestinationMarker+40 );
			std::string priorityStr(m_responseMessage.begin()+endOfDestinationMarker+40, m_responseMessage.begin()+endOfDestinationMarker+41 );
			std::string requiresVettingStr(m_responseMessage.begin()+endOfDestinationMarker+41, m_responseMessage.begin()+endOfDestinationMarker+42 );
			std::string overridableStr(m_responseMessage.begin()+endOfDestinationMarker+42, m_responseMessage.begin()+endOfDestinationMarker+43 );
			std::string clearStr(m_responseMessage.begin()+endOfDestinationMarker+43, m_responseMessage.begin()+endOfDestinationMarker+44 );
			std::string messageLengthStr(m_responseMessage.begin()+endOfDestinationMarker+44, m_responseMessage.begin()+endOfDestinationMarker+47 );

			int messageLength = atoi( messageLengthStr.c_str() );

			std::string messageTextStr( m_responseMessage.begin()+endOfDestinationMarker+47, m_responseMessage.begin()+endOfDestinationMarker+47+messageLength );

            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"Incoming RATIS info: \nTag = %s, \nDestination = %s, \nStart Time = %s, \nEnd Time = %s, \nPriority = %s, \nVetting = %s, \nOverride = %s, \nClear = %s, \nMessage = %s", 
                ratisTagStr.c_str(),
                msgdest.c_str(),
                startTimeStr.c_str(),
                endTimeStr.c_str(),
                priorityStr.c_str(),
                requiresVettingStr.c_str(),
                overridableStr.c_str(),
                clearStr.c_str(),
                messageTextStr.c_str());

            TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageDetails incomingRATISMessage;
				
			incomingRATISMessage.startTime = startTimeStr.c_str();
			incomingRATISMessage.endTime = endTimeStr.c_str();
			incomingRATISMessage.requiresVetting = (atoi( requiresVettingStr.c_str() ) == 1);
			incomingRATISMessage.overridable = (atoi(overridableStr.c_str()) == 1);
			bool clearType =(atoi( clearStr.c_str()) == 1);

			if( clearType )
			{
				incomingRATISMessage.type = TA_Base_Core::RATIS_IN_CLEAR;
			}
			else
			{
				incomingRATISMessage.type = TA_Base_Core::RATIS_IN_NEW;
                // We'll set this to new first and will decide whether it is a new or update in STISManager's
                // processIncomingRATISMessage method.
			}

			incomingRATISMessage.status = TA_Base_Core::NOT_APPROVED;
			//incomingRATISMessage.timeCreated = atol( getNewTimestampString().c_str() );
			incomingRATISMessage.timeCreated = time(NULL); // TODO - switch to ACE equivalent
			incomingRATISMessage.priority = atoi( priorityStr.c_str() );
			incomingRATISMessage.tag = ratisTagStr.c_str();
			incomingRATISMessage.destination = msgdest.c_str(); // store the entire msgdest structure
			incomingRATISMessage.messageText = messageTextStr.c_str();

			STISManager::getInstance()->processIncomingRATISMessage( incomingRATISMessage );

		}
		
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			dataLength = atoi(dataLengthStr.c_str());
			rxhLinkAlarm = (bool)atoi(rxhLinkStr.c_str());
			clockLinkAlarm = (bool)atoi(clockLinkStr.c_str());
			connectionLinkAlarm = (bool)atoi(connectionLinkStr.c_str());
			tisOccServerAlarm = (bool)atoi(tisOccServerStr.c_str());
			currentSTISStationLibraryVersion = atoi(currentSTISLibraryVersionStr.c_str());
			nextSTISStationLibraryVersion = atoi(newSTISLibraryVersionStr.c_str());
			nextSTISTrainLibraryVersion = atoi(newTTISLibraryVersionStr.c_str());
			
			std::stringstream resourceStatus;
			resourceStatus << "Status of Resources: ";
			resourceStatus << "RXH Link:";
			resourceStatus << rxhLinkAlarm;
			resourceStatus << " Clock Link:";
			resourceStatus << clockLinkAlarm;
			resourceStatus << " Connection link:";
			resourceStatus << connectionLinkAlarm;
			resourceStatus << " TIS Server:";
			resourceStatus << tisOccServerAlarm;

			std::string resourceStr;

			resourceStr = resourceStatus.str();

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, resourceStr.c_str());

			// Update the state of the resource datapoints

			// RXH Link Summary Point
			try
			{
				DpValue resourcePointValue = StatusMonitor::getInstance()->getCurrentDataPointValue(RXH_LINK_SUMMARY_ALARM_DP_NAME, STIS_SERVER_EQUIPMENT_NAME);
				DataPointState resourcePointState(resourcePointValue, time(NULL), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );

				resourcePointValue.setBoolean( rxhLinkAlarm );  
				resourcePointState.setValue(resourcePointValue );
				resourcePointState.setStatus( TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
				resourcePointState.setTimestamp( time( NULL ) );
				StatusMonitor::getInstance()->updateDataPointState( RXH_LINK_SUMMARY_ALARM_DP_NAME , resourcePointState, STIS_SERVER_EQUIPMENT_NAME, false );
			}
			catch (TA_Base_Core::DataException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
			}

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

			// OCC TIS Server Summary Point
			try
			{
				DpValue resourcePointValue = StatusMonitor::getInstance()->getCurrentDataPointValue(TIS_OCC_SERVER_STATUS_DP_NAME, STIS_SERVER_EQUIPMENT_NAME);
				DataPointState resourcePointState(resourcePointValue, time(NULL), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );

				resourcePointValue.setBoolean( tisOccServerAlarm );  
				resourcePointState.setValue(resourcePointValue );
				resourcePointState.setStatus( TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
				resourcePointState.setTimestamp( time( NULL ) );
				StatusMonitor::getInstance()->updateDataPointState( TIS_OCC_SERVER_STATUS_DP_NAME , resourcePointState, STIS_SERVER_EQUIPMENT_NAME, false ); 
			}
			catch (TA_Base_Core::DataException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what() );
			}

			StatusMonitor::getInstance()->setCurrentSTISStationLibraryVersion( currentSTISStationLibraryVersion );
			StatusMonitor::getInstance()->setNextSTISStationLibraryVersion( nextSTISStationLibraryVersion );
			StatusMonitor::getInstance()->setNextSTISTrainLibraryVersion( nextSTISTrainLibraryVersion );
			
			// Grab a copy of the ISCS server library versions to make the checks below more readable
			currentISCSStationLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSStationLibraryVersion_central();
			nextISCSStationLibraryVersion = StatusMonitor::getInstance()->getNextISCSStationLibraryVersion_central();
			currentISCSTrainLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSTrainLibraryVersion_central();
			nextISCSTrainLibraryVersion = StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion_central();
			iscsStationLibrariesSynchronised = StatusMonitor::getInstance()->getStationLibrariesSynchronised();

			std::stringstream stisVersionMessage;

			stisVersionMessage << "\nCurrent STIS server station library version: " << currentSTISStationLibraryVersion << "\n";
			stisVersionMessage << "Next STIS server station library version:    " << nextSTISStationLibraryVersion << "\n";
			stisVersionMessage << "Current ISCS station library version: " << currentISCSStationLibraryVersion << "\n";
			stisVersionMessage << "Next ISCS server station library version: " << nextISCSStationLibraryVersion << "\n";
			stisVersionMessage << "Station libraries ";
			if( iscsStationLibrariesSynchronised )
			{
				stisVersionMessage << "ARE synchronised \n";
			}
			else
			{
				stisVersionMessage << "ARE NOT synchronised \n";
			}

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						stisVersionMessage.str().c_str() );

			// See if there is a new STIS or TTIS message library file to process
			if( currentISCSStationLibraryVersion != currentSTISStationLibraryVersion )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
						"The STIS server has a different STIS predefined message library to the TISAgent's current STIS predefined message library");
			}

			if( ( currentISCSStationLibraryVersion != nextSTISStationLibraryVersion ) &&
				( nextISCSStationLibraryVersion != nextSTISStationLibraryVersion) )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						"A new STIS predefined message library is available for download");

				StatusMonitor::getInstance()->setNextSTISLibraryFilename( stisLibraryFilename );

				try
				{
					if( ( STISManager::getInstance()->stationLibrarySynchInProgress() == false ) /*&& ( iscsStationLibrariesSynchronised )*/)
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						"Attempting to process new STIS message library version");

						STISManager::getInstance()->processNewMessageLibrary( TA_IRS_App::STATION_LIBRARY, stisLibraryFilename, nextSTISStationLibraryVersion );
					}
					else
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
						"however, a previous station message library synchronisation is in progress");
					}
				}
				catch( const TA_IRS_Core::TISAgentException& e)
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
						"Could not process new STIS predefined message library");
				}
				catch(...)
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
						"Could not process new STIS predefined message library");
				}
			}

			// Only need to compare our current version to the indicated new version - the STIS doesnt have a current
			// version of the TTIS library
			currentISCSTrainLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSTrainLibraryVersion_central();
			nextISCSTrainLibraryVersion = StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion_central();
			iscsTrainLibrariesSynchronised = StatusMonitor::getInstance()->getTrainLibrariesSynchronised();

			std::stringstream ttisVersionMessage;

			ttisVersionMessage << "\nNext STIS server train library version:    " << nextSTISTrainLibraryVersion << "\n";
			ttisVersionMessage << "Current ISCS train library version: " << currentISCSTrainLibraryVersion << "\n";
			ttisVersionMessage << "Next ISCS train library version: " << nextISCSTrainLibraryVersion << "\n";
			ttisVersionMessage << "Train libraries ";
			if( iscsTrainLibrariesSynchronised )
			{
				ttisVersionMessage << "ARE synchronised \n";
			}
			else
			{
				ttisVersionMessage << "ARE NOT synchronised \n";
			}

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						ttisVersionMessage.str().c_str() );


			// We're only interested in a new library file,
			// if the previous one has been synchronised and upgraded, and the
			// version that the STIS is telling us about is different from our current library version
			if( ( currentISCSTrainLibraryVersion != nextSTISTrainLibraryVersion ) &&
				( nextISCSTrainLibraryVersion != nextSTISTrainLibraryVersion ))
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						"A new TTIS predefined message library is available for download");

				StatusMonitor::getInstance()->setNextTTISLibraryFilename( ttisLibraryFilename );

				try
				{
					if( ( STISManager::getInstance()->trainLibrarySynchInProgress() == false ) /*&&( iscsTrainLibrariesSynchronised )*/)
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						"Attempting to process new TTIS message library version");

						STISManager::getInstance()->processNewMessageLibrary( TA_IRS_App::TRAIN_LIBRARY, ttisLibraryFilename, nextSTISTrainLibraryVersion );
					}
					else
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
						"however, a previous train message library synchronisation is in progress");
					}
				}
				catch( const TA_IRS_Core::TISAgentException& e)
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
						"Could not process new TTIS predefined message library");
				}
				catch(...)
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
						"Could not process new TTIS predefined message library");
				}
			}

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

		// Now the data length - in this case it will always be 1 byte
		messageStream << "0001";

		// Now the actual data
		// This message only contains the RATIS message receival acknowledgement flag - TODO
		messageStream << m_previousRXHMessageAck;

		std::string messageString = messageStream.str();

		messageVector.push_back( 0x02 );
		for(unsigned int i=0;i < messageString.size() ; i++)
		{
			messageVector.push_back( messageString[i] );
		}
		messageVector.push_back( 0x03 );
				
		return messageVector;
    }

	
}