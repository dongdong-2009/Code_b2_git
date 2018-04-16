/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_agent/src/STISMessage.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */


#include "ace/OS.h"

#include "app/signs/tis_agent/src/STISMessage.h"
#include "app/signs/tis_agent/src/STISManager.h"
#include "StringHelper.h"

#include <iomanip>

namespace TA_IRS_App
{

	STISMessage::STISMessage() : m_receivedResponse ( false ),
		m_nacked ( false ),
		m_dataIsSet( false ),
		m_nackRetries( 3 ),
		m_isMessageSentToSTIS( true )
	{
		m_timestamp = getNewTimestampString();
	}


    STISMessage::~STISMessage()
	{
	}


    bool STISMessage::hasReceivedResponse()
	{
		return m_receivedResponse;
	}


    bool STISMessage::receivedNackResponse()
	{
		return m_nacked;
	}


    void STISMessage::setResponse( const std::vector<unsigned char>& responseData, bool nack )
    {
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug , "Setting response data for message");

        m_responseMessage = responseData;
        
        m_receivedResponse = true;

		if(nack)
		{
			m_nacked = true;
		}

    }


	void STISMessage::setSequenceNumber( unsigned long seqNumber )
	{
		m_sequenceNumber = seqNumber;

        // convert it to a 4 character wide string
		std::stringstream sequenceNumStream;
        sequenceNumStream << std::setw(4) << std::setfill('0') << m_sequenceNumber;

		m_sequenceNumberString = sequenceNumStream.str();
	}


	int STISMessage::getSequenceNumber()
	{
		return m_sequenceNumber;
	}


    void STISMessage::setData()
    {
    }


	std::string STISMessage::getSequenceNumberAsString()
	{	
		return m_sequenceNumberString;
	}


	std::string STISMessage::getMessageID()
	{	
		return m_messageIDString;
	}


	std::string STISMessage::getNewTimestampString()
	{
		std::stringstream timeStamp;

		TA_Base_Core::ThreadGuard guard( m_timestampLock );

		time_t currentTime;
		tm localTime;

		currentTime = time(NULL);
        ACE_OS::localtime_r(&currentTime, &localTime);

        // the timestamp is in the format
        // YYYYMMDDHHmmSS

        // YYYY
		timeStamp << (localTime.tm_year + 1900);

        // MM
        // tm_mon is months since january, so 0 = january
        timeStamp << std::setw(2) << std::setfill('0') << ( localTime.tm_mon + 1 );
		
        // DD
        // tm_mday is the day of the month (1-31)
        timeStamp << std::setw(2) << std::setfill('0') << localTime.tm_mday;

        // HH
        // tm_hour is the hours past the day (0-23)
        timeStamp << std::setw(2) << std::setfill('0') << localTime.tm_hour;

        // MM
        // tm_min is the minutes past the hour (0-59)
		timeStamp << std::setw(2) << std::setfill('0') << localTime.tm_min;

        // SS
        // tm_sec is the seconds past the minute (0-60) (61 for leap seconds)
		timeStamp << std::setw(2) << std::setfill('0') << localTime.tm_sec;
	
		return timeStamp.str();
	}
	

	std::string STISMessage::getStationIdString(const std::string& locationName)
	{
		std::string station = locationName;
        station = StringHelper::replaceStr<char>(station, "OCC_", "");
		std::stringstream ss;
		ss << std::left << std::setw(6) << std::setfill(' ') << station << std::right;
		return ss.str();
	}

    std::string STISMessage::getMessageDestinationString(const TA_Base_Core::STISDestination& destination)
    {
        std::stringstream msgDest;

        // System ID
		msgDest << "DTL";

		std::string stationId(destination.station.in()); //station should be uppercase and length is 4 bytes
		msgDest << getStationIdString(stationId);

        // if AST is the destination
        // then the rest of the fields are not included
        if (stationId != "ASTN")
        {
		    // Number of PIDs this request is being sent to ( 00 - 50 )
		    // 00 is all PIDs at the station
		    int numPIDs = destination.pids.length();

            msgDest << std::setw(3) << std::setfill('0') << numPIDs;
		    
		    // List of selected PIDs
            for( int i = 0; i < numPIDs; i++ )
            {
                std::string pidName(destination.pids[i]);
                int pidAddress = STISManager::getInstance()->getPIDAddress(pidName, stationId);

			    // this field is three characters long, but the value can never be over 50
			    msgDest << std::setw(3) << std::setfill('0') << pidAddress;
            }
        }

        return msgDest.str();
    }

    std::string STISMessage::getTimestamp()
    {
        return m_timestamp;
    }

	void STISMessage::setMessageIsSentToSTIS( bool isSentToSTIS)
	{
		m_isMessageSentToSTIS = isSentToSTIS;
	}

	bool STISMessage::isMessageSentToSTIS()
	{
		return m_isMessageSentToSTIS;
	}

	std::string STISMessage::createLogMessage(const std::vector<unsigned char>& protocolMessage)
	{
		std::string tmpStr;
		std::copy(protocolMessage.begin(), protocolMessage.end(), std::back_inserter(tmpStr));
		return tmpStr;
	}
}
