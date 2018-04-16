/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/STISCurrentDisplayingMessageRequest.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/signs/tis_agent/src/STISCurrentDisplayingMessageRequest.h"
#include "app/signs/tis_agent/src/STISManager.h"
#include <iomanip>
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    const int STISCurrentDisplayingMessageRequest::DATA_LENGTH_START = 22;
    const int STISCurrentDisplayingMessageRequest::DATA_LENGTH_NUMBYTE = 4;
    const int STISCurrentDisplayingMessageRequest::REPORT_STATION_NUMBYTE = 3;
    const int STISCurrentDisplayingMessageRequest::REPORT_PID_NUMBYTE = 2;
    const int STISCurrentDisplayingMessageRequest::NUM_MESSAGE_LIST_NUMBYTE = 2;
    const int STISCurrentDisplayingMessageRequest::MESSAGE_TAG_NUMBYTE = 12;
    const int STISCurrentDisplayingMessageRequest::START_TIME_NUMBYTE = 14;
    const int STISCurrentDisplayingMessageRequest::END_TIME_NUMBYTE = 14;
    const int STISCurrentDisplayingMessageRequest::PRIORITY_NUMBYTE = 1;
    const int STISCurrentDisplayingMessageRequest::MESSAGE_LENGTH_NUMBYTE = 3;
    const int STISCurrentDisplayingMessageRequest::MINIMUM_LENGTH = 33;
	const int STISCurrentDisplayingMessageRequest::MINIMUM_LENGTH_WITH_MESSAGE = 78;
	const std::string STISCurrentDisplayingMessageRequest::EMPTY_MESSAGE = "There is no Display message in selected PID";
	

	STISCurrentDisplayingMessageRequest::STISCurrentDisplayingMessageRequest()
        : STISMessage(), m_responseProcessed(false)
	{
		m_messageIDString = "M50";

        // initialise
        m_currentDisplayingMessage.startTime = CORBA::string_dup("");
        m_currentDisplayingMessage.endTime = CORBA::string_dup("");
        m_currentDisplayingMessage.priority = 0;
        m_currentDisplayingMessage.messageContent = CORBA::string_dup("");
	}


	STISCurrentDisplayingMessageRequest::~STISCurrentDisplayingMessageRequest()
	{
	}


	std::vector<unsigned char> STISCurrentDisplayingMessageRequest::createProtocolMessage()
	{
		std::vector<unsigned char> messageVector;
		std::stringstream messageStream;

		// Add the sequence number
        messageStream << getSequenceNumberAsString();

		// Add the timestamp to the message
		messageStream << m_timestamp;

		// Now the message ID
		messageStream << m_messageIDString; // M50

		// Now the data length - in this case it will always be 5 bytes
		messageStream << "0005";

		// Now the actual data

		// 3 character station ID
		messageStream << stationID;

		// PID id - 2 digits
		messageStream << std::setw(2) << std::setfill('0') << m_PID;

		std::string messageString = messageStream.str();

		messageVector.push_back( 0x02 );
		for(unsigned int i=0;i < messageString.size() ; i++)
		{
			messageVector.push_back( messageString[i] );
		}
		messageVector.push_back( 0x03 );
				
		return messageVector;
	}


	void STISCurrentDisplayingMessageRequest::log(bool sendSuccess)
	{
    }


	void STISCurrentDisplayingMessageRequest::processResponse()
	{
		m_responseProcessed = true;

        // Convert the vector<unsigned char> into string so that it's easier to play around
		std::string responseMessage(m_responseMessage.begin(), m_responseMessage.end());

        // check to see if the packet is has at least the minimum length.  If it doesn't then just ignore it.
        if (responseMessage.size() < MINIMUM_LENGTH)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Packet is shorter than minimum length.  Ignoring.");
            return;
        }

        int i = DATA_LENGTH_START;
        // Data length
        int size = DATA_LENGTH_NUMBYTE;
        unsigned int dataLength = atol(responseMessage.substr(i, size).c_str());
        i += size;
        // check that the size of the data actually corresponds to the data length
        if (responseMessage.size() - i - 1 != dataLength)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Actual data size is different to the data length sent in packet.  Ignoring.");
            return;
        }

        // Report Station
        size = REPORT_STATION_NUMBYTE;
        std::string reportStation = responseMessage.substr(i, size);
        i += size;

        // Report PID
        size = REPORT_PID_NUMBYTE;
        std::string reportPid = responseMessage.substr(i, size);
        i += size;

        // Number of Message List
        size = NUM_MESSAGE_LIST_NUMBYTE;
        int numMessageList = atol(responseMessage.substr(i, size).c_str());
        i += size;

        // this could only be 0 or 1.  If it is 0, just return.
        if (numMessageList == 0)
        {
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
						"Packet does not contain message. Current selected PID(%s, %s) doesn't display any message.",
						reportStation.c_str(), reportPid.c_str());
			
			m_currentDisplayingMessage.startTime = "";
			m_currentDisplayingMessage.endTime = "";
			m_currentDisplayingMessage.priority = 0;
			m_currentDisplayingMessage.messageContent = EMPTY_MESSAGE.c_str();
        }
		else if (responseMessage.size() >= MINIMUM_LENGTH_WITH_MESSAGE && numMessageList == 1)			
		{
			// Message Tag
			size = MESSAGE_TAG_NUMBYTE;
			std::string messageTag = responseMessage.substr(i, size);
			i += size;

			// Start Time
			size = START_TIME_NUMBYTE;
			std::string startTime = responseMessage.substr(i, size);
			m_currentDisplayingMessage.startTime = startTime.c_str(); //startTime.c_str();
			i += size;
        
			// End Time
			size = END_TIME_NUMBYTE;
			std::string endTime = responseMessage.substr(i, size);
			m_currentDisplayingMessage.endTime = endTime.c_str();
			i += size;

			// Priority
			size = PRIORITY_NUMBYTE;
			m_currentDisplayingMessage.priority = static_cast<short>(atoi(responseMessage.substr(i, size).c_str()));
			i += size;

			// Message Length
			size = MESSAGE_LENGTH_NUMBYTE;
			int messageLength = atol(responseMessage.substr(i, size).c_str());
			i += size;

			// Actual Message
			std::string messageContent = responseMessage.substr(i, messageLength);
			m_currentDisplayingMessage.messageContent = messageContent.c_str();
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Packet details:");
			LOGMORE(SourceInfo, "%s = %s", "Report Station", reportStation.c_str());
			LOGMORE(SourceInfo, "%s = %s", "Report PID", reportPid.c_str());
			LOGMORE(SourceInfo, "%s = %s", "Message Tag", messageTag.c_str());
			LOGMORE(SourceInfo, "%s = %s", "Start Time", startTime.c_str());
			LOGMORE(SourceInfo, "%s = %s", "End Time", endTime.c_str());
			LOGMORE(SourceInfo, "%s = %d", "Priority", m_currentDisplayingMessage.priority);
			LOGMORE(SourceInfo, "%s = %s", "Content", messageContent.c_str());
		}
		else
		{
			
		}
	}


	TA_Base_Core::CurrentDisplayingMessage STISCurrentDisplayingMessageRequest::getMessage()
	{
		TA_ASSERT( m_responseProcessed==true, "Cannot get currently displaying message - response not yet processed");

		return m_currentDisplayingMessage;
	}


    void STISCurrentDisplayingMessageRequest::setData( const std::string& station, int pidAddress )
    {
		stationID = station;
		m_PID = pidAddress;
    }
	
}