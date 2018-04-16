/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#include "bus/trains/TrainProtocolLibrary/src/TestCallEvent.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType TestCallEvent::MESSAGE_TYPE = 71;
    const unsigned short TestCallEvent::EXPECTED_MESSAGE_LENGTH = 13;
    const unsigned short TestCallEvent::TTIS_VERSION_POSITION = 5;
    const unsigned short TestCallEvent::PA_VERSION_POSITION = 7;
    const unsigned short TestCallEvent::SCHEDULE_VERSION_POSITION = 9;


    TestCallEvent::TestCallEvent( const TrainMessageData& data,
                                  const std::string& sendingTsi,
                                  const std::string& destinationTsi )
            : TimsEvent( data, sendingTsi, destinationTsi )
    {
        FUNCTION_ENTRY( "TestCallEvent" );

        // reads the versions, and sets the members
        m_ttisVersion = getWord( TTIS_VERSION_POSITION );
        m_paVersion = getWord( PA_VERSION_POSITION );
        m_scheduleVersion = getWord( SCHEDULE_VERSION_POSITION );

        FUNCTION_EXIT;
    }


    TestCallEvent::~TestCallEvent()
    {
        FUNCTION_ENTRY( "~TestCallEvent" );
        FUNCTION_EXIT;
    }


    TrainMessageType TestCallEvent::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;
    }


    std::string TestCallEvent::getLogString() const
    {
        FUNCTION_ENTRY( "getLogString" );

        //appends more information in the format:
        //"[TTIS library=X][PA library=X][Message schedule=X]"
        std::ostringstream logStream;
        logStream << TimsEvent::getLogString();
        logStream << "[TTIS library= " << m_ttisVersion << "]"
                  << "[PA library=" << m_paVersion << "]"
                  << "Message schedule=" << m_scheduleVersion << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }


    unsigned short TestCallEvent::getTtisVersion() const
    {
        FUNCTION_ENTRY( "getTtisVersion" );
        FUNCTION_EXIT;
        return m_ttisVersion;
    }


    unsigned short TestCallEvent::getPaVersion() const
    {
        FUNCTION_ENTRY( "getPaVersion" );
        FUNCTION_EXIT;
        return m_paVersion;
    }


    unsigned short TestCallEvent::getScheduleVersion() const
    {
        FUNCTION_ENTRY( "getScheduleVersion" );
        FUNCTION_EXIT;
        return m_scheduleVersion;
    }


    unsigned short TestCallEvent::getExpectedMessageLength() const
    {
        FUNCTION_ENTRY ( "getExpectedMessageLength" );
        FUNCTION_EXIT;
        return EXPECTED_MESSAGE_LENGTH;
    }

}//end of namespace

