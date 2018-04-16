/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/TTISMessage.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/signs/tis_agent/src/TTISMessage.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

#include <ace/OS.h>

#include <iomanip>
#include <sstream>

namespace TA_IRS_App
{

	TTISMessage::TTISMessage() : m_dataIsSet( false )
	{
        FUNCTION_ENTRY( "TTISMessage" );
		m_timestamp = getNewTimestampString();
        FUNCTION_EXIT;
	}


	TTISMessage::~TTISMessage()
	{
        FUNCTION_ENTRY( "~TTISMessage" );
        FUNCTION_EXIT;
	}


	std::string TTISMessage::getNewTimestampString()
	{
        FUNCTION_ENTRY( "getNewTimestampString" );
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

        FUNCTION_EXIT;
		return timeStamp.str();
	}


	void TTISMessage::setData()
	{
        FUNCTION_ENTRY( "setData" );
        FUNCTION_EXIT;
	}

	
    std::string TTISMessage::getTimestamp()
    {
        FUNCTION_ENTRY( "getTimestamp" );
        FUNCTION_EXIT;
        return m_timestamp;
    }

}
