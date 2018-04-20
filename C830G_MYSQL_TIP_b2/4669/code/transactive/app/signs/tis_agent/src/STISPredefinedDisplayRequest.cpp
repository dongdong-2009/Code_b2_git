/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/STISPredefinedDisplayRequest.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/signs/tis_agent/src/STISPredefinedDisplayRequest.h"
#include "app/signs/tis_agent/src/STISManager.h"
#include "app/signs/tis_agent/src/StatusMonitor.h"

#include "core/data_access_interface/tis_agent/src/DisplayRequestAccessFactory.h"
#include "core/data_access_interface/tis_agent/src/DisplayRequestData.h"

#include <iomanip>

namespace TA_IRS_App
{

	STISPredefinedDisplayRequest::STISPredefinedDisplayRequest()
	{
		m_messageIDString = "M10";
	}

	STISPredefinedDisplayRequest::~STISPredefinedDisplayRequest()
	{


	}


	std::vector<unsigned char> STISPredefinedDisplayRequest::createProtocolMessage()
	{
		std::vector<unsigned char> messageVector;
		std::stringstream messageStream;

		// Add the sequence number
		messageStream << getSequenceNumberAsString();

		// Add the timestamp to the message
		messageStream << m_timestamp;

		// Now the message ID
		messageStream << m_messageIDString; // M10

		// Now the actual data - this needs to be created separately
		// because the length of the data needs to be indicated 
		// before the data itself (i.e we need to calculate the size of it)

		std::stringstream dataStream;

		// Message destination (MSGDEST)
        dataStream << getMessageDestinationString(m_destination);

		// MESSAGE 'FIELDS'
		
		// Message Tag - always padded out to 12 bytes (empty space added)
		std::stringstream tagStream;
		std::string tagString;

		switch( m_librarySection )
		{
        case TA_Base_Core::EMERGENCY_SECTION:
			tagStream << "0";
			break;

        case TA_Base_Core::NORMAL_SECTION:
			tagStream << "1";
			break;
		}

        // 2 byte hex number, 01 to FF
        tagStream << std::hex << std::setw(2) << std::setfill('0') << m_tag;

		tagString = tagStream.str();
		int tagSize = tagString.size();

		dataStream << tagString;

		// Pad the remaining characters with " "
		for(int i=0 ; i < (12 - tagSize) ; i++)
		{
			dataStream << " ";
		}

		// Start Time
		dataStream << m_startTime;

		// End Time
		dataStream << m_endTime;

		// Priority
		dataStream << m_priority;

		// Calculate the data length - this is variable
		std::string dataString = dataStream.str();
		int dataSize = dataString.size();
        messageStream << std::setw(4) << std::setfill('0') << dataSize;
		
		// Add the data stream to the actual message stream after calculating its size
		messageStream << dataString;	

		// No train data

		std::string messageString = messageStream.str();

		unsigned int j;

		messageVector.push_back( 0x02 );
		for( j=0;j < messageString.size() ; j++)
		{
			messageVector.push_back( messageString[j] );
		}
		messageVector.push_back( 0x03 );
				
		return messageVector;
	}


	void STISPredefinedDisplayRequest::log(bool sendSuccess)
	{
		TA_ASSERT(m_dataIsSet, "Log() called for Predefined display request that has no data");

        TA_IRS_Core::IDisplayRequest* displayRequest = TA_IRS_Core::DisplayRequestAccessFactory::getInstance().createNewDisplayRequest();

		displayRequest->setMessageTag( m_tag );
		displayRequest->setLibrarySection( m_librarySection );
		displayRequest->setLibraryVersion( m_libraryVersion );
		displayRequest->setDestinationSystem( "STIS" );
		displayRequest->setStartTime( m_startTime );
		displayRequest->setEndTime( m_endTime );
		displayRequest->setPriority( m_priority );
		displayRequest->setTimestamp( m_timestamp );
        displayRequest->setMessageType( TA_IRS_Core::IDisplayRequest::PREDEFINED );
		displayRequest->setSubmittedBy( m_submittedBy );
		//hongran++ TD17502
		displayRequest->setIfSendSuccess(sendSuccess ? TA_Base_Core::Pass : TA_Base_Core::Fail);
		//++hongran TD17502

		displayRequest->setStisDestinationStation( m_destination.station.in() );

        // set the adhoc content to the predefined message text in case this library is
        // upgraded later - they will still be able to see what was sent.
        displayRequest->setAdhocMessageContent(
            StatusMonitor::getInstance()->getStationPredefinedMessageText( m_librarySection, m_tag ) );
		
		std::stringstream levelStream;

		unsigned int i;
		for( i = 0 ; i < m_destination.levels.length() ; i++ )
		{
			// Add a space before the next level name
			// this ensures that we don't add one at the end
			if( i > 0 )
			{
				levelStream << " ";
			}

			levelStream << m_destination.levels[i].in();
		}

		displayRequest->setStisDestinationLevels( levelStream.str() );

		std::stringstream pidStream;
		for( i = 0 ; i < m_destination.pids.length() ; i++ )
		{
			// Add a space before the next level name
			// this ensures that we don't add one at the end
			if( i > 0 )
			{
				pidStream << " ";
			}

			pidStream << m_destination.pids[i].in();
		}

		displayRequest->setStisDestinationPids( pidStream.str() );

		std::string wtf = pidStream.str();

		// write this display request to the database
		displayRequest->applyChanges();

		delete displayRequest;
		displayRequest = NULL;
	}


    void STISPredefinedDisplayRequest::setData( const TA_Base_Core::STISDestination& destination, TA_Base_Core::ELibrarySection librarySection, 
		unsigned short libraryVersion, unsigned short messageTag, const std::string& startTime, 
		const std::string& endTime, int priority, const std::string& operatorName )
	{

		// No need to check the validity of the actual data here
		// the STIS Manager should have done that
		m_destination = destination;
		m_tag = messageTag;
		m_librarySection = librarySection;
		m_libraryVersion = libraryVersion;
		m_startTime = startTime;
		m_endTime = endTime;
		m_priority = priority;
		m_submittedBy = operatorName;

		m_dataIsSet = true;

	}
	
}