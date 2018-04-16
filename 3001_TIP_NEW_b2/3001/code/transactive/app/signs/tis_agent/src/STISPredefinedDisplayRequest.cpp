/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/STISPredefinedDisplayRequest.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/03/19 12:35:30 $
  * Last modified by:  $Author: huang.wenbo $
  *
  */

#include "app/signs/tis_agent/src/STISPredefinedDisplayRequest.h"
#include "app/signs/tis_agent/src/STISManager.h"
#include "app/signs/tis_agent/src/StatusMonitor.h"
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
		// TYY T:message library section YY:message NO.
		std::stringstream tagStream;
		//tagStream << m_librarySection;
        // 4 byte hex number, 0001 to 1FFF
        tagStream << std::hex << std::setw(4) << std::setfill('0') << m_tag;
		// fill 8 space
		tagStream << std::setw(8) << std::setfill(' ') << " ";
		dataStream << tagStream.str();

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

		unsigned long j;

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
	}


    void STISPredefinedDisplayRequest::setData( const TA_Base_Core::STISDestination& destination, TA_Base_Core::ELibrarySection librarySection, 
		unsigned long libraryVersion, unsigned short messageTag, const std::string& startTime, 
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
