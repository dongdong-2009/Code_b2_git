/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_agent/src/STISClearRequest.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#include "app/signs/tis_agent/src/STISClearRequest.h"
#include "app/signs/tis_agent/src/STISManager.h"

#include <iomanip>

namespace TA_IRS_App
{


	STISClearRequest::STISClearRequest()
        : STISMessage(), 
          m_destination(""),
          m_tag("")
	{
		m_messageIDString = "M20";
	}


	STISClearRequest::~STISClearRequest()
	{
	}


	std::vector<unsigned char> STISClearRequest::createProtocolMessage()
	{
		std::vector<unsigned char> messageVector;
		std::stringstream messageStream;

		// Add the sequence number
		messageStream << getSequenceNumberAsString();

		// Add the timestamp to the message
		messageStream << m_timestamp;

		// Now the message ID
		messageStream << m_messageIDString; // M20

		// Now the actual data - this needs to be created separately
		// because the length of the data needs to be indicated 
		// before the data itself (i.e we need to calculate the size of it)

		std::stringstream dataStream;

		// DESTINATION (MSGDEST) 'FIELDS'

        // Message destination (MSGDEST)
        dataStream << m_destination;

		// Message Tag - this is actually the timestamp for M20 messages
		// When this message is originated at the STIS Manager,
		// the timestamp is unknown and is filled with 12 space characters
        dataStream << std::setw(12) << std::setfill(' ') << std::left << m_tag << std::right;

		// Priority clearing info
		for(int i = 0; i < 6; ++i)
		{
			if( (i >= m_highPriority-1) && ( i < m_lowPriority))
			{
				dataStream << "1";
			}
			else
			{
				dataStream << "0";
			}
		}

		// Calculate the data length - this is variable
		std::string dataString = dataStream.str();
		int dataSize = dataString.size();
        messageStream << std::setw(4) << std::setfill('0') << dataSize;
		
		// Add the data stream to the actual message stream after calculating it
		messageStream << dataString;

		// No train data

		std::string messageString = messageStream.str();

		messageVector.push_back( 0x02 );
		for(unsigned long z=0;z < messageString.size() ; z++)
		{
			messageVector.push_back( messageString[z] );
		}
		messageVector.push_back( 0x03 );
				
		return messageVector;
	}


	void STISClearRequest::log(bool sendSuccess)
	{
	}


    void STISClearRequest::setData( const TA_Base_Core::STISDestination& destination,
		                            const unsigned short highPriority,
									const unsigned short lowPriority )
	{
		m_destination = getMessageDestinationString(destination);
		m_highPriority = highPriority;
		m_lowPriority = lowPriority;
	}

    void STISClearRequest::setData( const std::string& destination,
		                            const unsigned short priority,
									const std::string& messageTag )
	{
        m_tag = messageTag;
		m_destination = destination;
		m_lowPriority = priority;
		m_highPriority = priority;
	}
}
