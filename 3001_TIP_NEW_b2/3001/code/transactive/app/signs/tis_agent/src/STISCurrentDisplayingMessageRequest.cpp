/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/STISCurrentDisplayingMessageRequest.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #5 $
  *
  * Last modification: $DateTime: 2014/07/01 11:16:12 $
  * Last modified by:  $Author: huang.wenbo $
  *
  */

#include "app/signs/tis_agent/src/STISCurrentDisplayingMessageRequest.h"
#include "app/signs/tis_agent/src/STISManager.h"
#include <iomanip>
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
	const wchar_t SEPARATOR = 0xFFFF;
	//save a unicode string to wstring
	std::wstring combinedStringToWstring(std::string src)
	{
		int length = src.size()/2;
		std::wstring dst(length, L'\0');
		for(int i = 0; i < length; ++i)
		{
            wchar_t ls = static_cast<wchar_t>((unsigned char)src[i*2]);
            wchar_t ms = (static_cast<wchar_t>((unsigned char)src[i*2+1]) << 8);
            dst[i] = ms | ls;
		}
		return dst;
	}
	
}

namespace TA_IRS_App
{
    const int STISCurrentDisplayingMessageRequest::DATA_LENGTH_START = 22;
    const int STISCurrentDisplayingMessageRequest::DATA_LENGTH_NUMBYTE = 4;
    const int STISCurrentDisplayingMessageRequest::REPORT_STATION_NUMBYTE = 6;
    const int STISCurrentDisplayingMessageRequest::REPORT_PID_NUMBYTE = 3;
	const int STISCurrentDisplayingMessageRequest::MESSAGE_STATUS_NUMBYTE = 2;
    const int STISCurrentDisplayingMessageRequest::MESSAGE_TAG_NUMBYTE = 12;
    const int STISCurrentDisplayingMessageRequest::START_TIME_NUMBYTE = 14;
    const int STISCurrentDisplayingMessageRequest::END_TIME_NUMBYTE = 14;
    const int STISCurrentDisplayingMessageRequest::PRIORITY_NUMBYTE = 1;
    const int STISCurrentDisplayingMessageRequest::MESSAGE_LENGTH_NUMBYTE = 4;
    const int STISCurrentDisplayingMessageRequest::MINIMUM_LENGTH = 38;
	const int STISCurrentDisplayingMessageRequest::MINIMUM_LENGTH_WITH_MESSAGE = 84;
	const std::wstring STISCurrentDisplayingMessageRequest::EMPTY_MESSAGE = L"There is no Display message in selected PID";
	

	STISCurrentDisplayingMessageRequest::STISCurrentDisplayingMessageRequest()
        : STISMessage(), m_responseProcessed(false)
	{
		m_messageIDString = "M50";

        // initialise
        m_currentDisplayingMessage.startTime = CORBA::string_dup("");
        m_currentDisplayingMessage.endTime = CORBA::string_dup("");
        m_currentDisplayingMessage.priority = 0;
        m_currentDisplayingMessage.messageContent = CORBA::wstring_dup(L"");
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

		// Now the data length - in this case it will always be 9 bytes
		messageStream << "0009";

		// Now the actual data

		// station ID, 4 bytes and 2 space reserved
        messageStream << getStationIdString(stationID);

		// PID id
		messageStream << std::setw(3) << std::setfill('0') << m_PID;

		std::string messageString = messageStream.str();

		messageVector.push_back( 0x02 );
		for(unsigned long i=0;i < messageString.size() ; i++)
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

		int index = DATA_LENGTH_START;
        unsigned long dataLength = atol(responseMessage.substr(index, DATA_LENGTH_NUMBYTE).c_str());
		index += DATA_LENGTH_NUMBYTE;
        // check that the size of the data actually corresponds to the data length
        if (responseMessage.size() - index - 1 != dataLength)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Actual data size is different to the data length sent in packet.  Ignoring.");
            return;
        }

        // Report Station
        std::string reportStation(responseMessage.substr(index, REPORT_STATION_NUMBYTE));
		index += REPORT_STATION_NUMBYTE;
        // Report PID
        std::string reportPid(responseMessage.substr(index, REPORT_PID_NUMBYTE));
		index += REPORT_PID_NUMBYTE;
		// Message status
		std::string messageStatus(responseMessage.substr(index, MESSAGE_STATUS_NUMBYTE));
		index += MESSAGE_STATUS_NUMBYTE;

        if (messageStatus == "00")
        {
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
						"Packet does not contain message. Current selected PID(%s, %s) doesn't display any message.",
						reportStation.c_str(), reportPid.c_str());
			
			m_currentDisplayingMessage.startTime = "";
			m_currentDisplayingMessage.endTime = "";
			m_currentDisplayingMessage.priority = 0;
			m_currentDisplayingMessage.messageContent = EMPTY_MESSAGE.c_str();
        }
		else if (responseMessage.size() >= MINIMUM_LENGTH_WITH_MESSAGE && messageStatus == "01")			
		{
			// Message Tag
			std::string messageTag(responseMessage.substr(index, MESSAGE_TAG_NUMBYTE));
			index += MESSAGE_TAG_NUMBYTE;
			// Start Time
			std::string startTime(responseMessage.substr(index, START_TIME_NUMBYTE));
			index += START_TIME_NUMBYTE;
			m_currentDisplayingMessage.startTime = startTime.c_str();
			// End Time
			std::string endTime(responseMessage.substr(index, END_TIME_NUMBYTE));
			index += END_TIME_NUMBYTE;
			m_currentDisplayingMessage.endTime = endTime.c_str();
			// Priority
			std::string priority(responseMessage.substr(index, PRIORITY_NUMBYTE));
			index += PRIORITY_NUMBYTE;
			m_currentDisplayingMessage.priority = static_cast<short>(atoi(priority.c_str()));
			// Message Length
			std::string messageLengthStr(responseMessage.substr(index, MESSAGE_LENGTH_NUMBYTE));
			index += MESSAGE_LENGTH_NUMBYTE;
			int messageLength = atol(messageLengthStr.c_str());
			// message content
			std::string message(responseMessage.substr(index, messageLength));
			// Actual Message,unicode
			std::wstring messageContent= combinedStringToWstring(message);
			// Remove the separator mark FFFF
			std::wstring::size_type pos = 0;
			while( (pos = messageContent.find(SEPARATOR, pos)) != std::wstring::npos )
			{
				messageContent.erase(pos, 1);
			}
			m_currentDisplayingMessage.messageContent = messageContent.c_str();
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Packet details:");
			LOGMORE(SourceInfo, "%s = %s", "Report Station", reportStation.c_str());
			LOGMORE(SourceInfo, "%s = %s", "Report PID", reportPid.c_str());
			LOGMORE(SourceInfo, "%s = %s", "Message Tag", messageTag.c_str());
			LOGMORE(SourceInfo, "%s = %s", "Start Time", startTime.c_str());
			LOGMORE(SourceInfo, "%s = %s", "End Time", endTime.c_str());
			LOGMORE(SourceInfo, "%s = %d", "Priority", m_currentDisplayingMessage.priority);
			//show message content as hex
			std::ostringstream hexMessageContent;
			for(int i = 0; i < message.size(); ++i)
			{
				hexMessageContent << "0x"; 
				hexMessageContent << std::setw(2) << std::setfill('0') << std::hex << message[i];
			}
			LOGMORE(SourceInfo, "%s = %s", "Content", hexMessageContent.str().c_str());
		}
		else
		{
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Actual data size is different to the data length sent in packet.  Ignoring.");
            return;
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