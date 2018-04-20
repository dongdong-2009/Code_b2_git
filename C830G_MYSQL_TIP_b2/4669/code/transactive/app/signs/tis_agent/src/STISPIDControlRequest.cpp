/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/STISPIDControlRequest.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/signs/tis_agent/src/STISPIDControlRequest.h"
#include "app/signs/tis_agent/src/STISManager.h"

#include <iomanip>

namespace TA_IRS_App
{

	STISPIDControlRequest::STISPIDControlRequest()
	{
		m_messageIDString = "M21";
	}


	STISPIDControlRequest::~STISPIDControlRequest()
	{
	}


	std::vector<unsigned char> STISPIDControlRequest::createProtocolMessage()
	{
		std::vector<unsigned char> messageVector;
		std::stringstream messageStream;

		// Add the sequence number

		messageStream << getSequenceNumberAsString();

		// Add the timestamp to the message
		messageStream << m_timestamp;

		// Now the message ID
		messageStream << m_messageIDString; // M21

		// Now the actual data - this needs to be created separately
		// because the length of the data needs to be indicated 
		// before the data itself (i.e we need to calculate the size of it)

		std::stringstream dataStream;

		// System ID
		dataStream << "CCL";

		// Station or Train ID indicator
		// When this message is sent in response to the STIS GUI
		// this can only ever be the Station ID

        // split the PID name up into station id and pid name
        std::vector<std::string> parts = 
            STISManager::getInstance()->tokenizePIDName( m_destination );

        TA_ASSERT(parts.size() == 4, "The PID name validity should have been checked in STISManager.");

        // first part is the station name
		dataStream << parts[0];

		// DESTINATION 'FIELD'

		// Number of PIDs this request is being sent to ( 00 - 50 )
		// 00 is all PIDs at the station
		dataStream << "001";

		// List of selected PIDs
        // the last part is the PID name
        int pid = STISManager::getInstance()->getPIDAddress(parts[3], parts[0]);
		dataStream << std::setw(3) << std::setfill('0') << pid;

		// Control fields
		
		if( m_controlCommand == TA_Base_Core::TURN_ON )
		{
			dataStream << "1";
			dataStream << "0";
    		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Turning %d on", pid);
		}
		else // if( m_controlCommand == TA_IRS_Core::TURN_OFF )
		{
			dataStream << "0";
			dataStream << "1";
    		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Turning %d off", pid);
		}


		// Calculate the data length - this is variable
		std::string dataString = dataStream.str();
        int dataSize = dataString.size();

        messageStream << std::setw(4) << std::setfill('0') << dataSize;
		
		// Add the data stream to the actual message stream after calculating its size
		messageStream << dataString;	

		// No train data

		std::string messageString = messageStream.str();

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Sending Control Request: %s", messageString.c_str());

        messageVector.push_back( 0x02 );
		for(unsigned int i=0;i < messageString.size() ; i++)
		{
			messageVector.push_back( messageString[i] );
		}
		messageVector.push_back( 0x03 );
				
		return messageVector;
	}


	void STISPIDControlRequest::log(bool sendSuccess)
	{
	}


	void STISPIDControlRequest::setData( const std::string& destination, TA_Base_Core::EPIDControl controlCommand )
	{
		m_controlCommand = controlCommand;	
		m_destination = destination;
	}

	
}