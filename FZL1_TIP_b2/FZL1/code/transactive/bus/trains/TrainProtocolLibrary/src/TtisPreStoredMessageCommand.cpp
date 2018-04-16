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

#include "bus/trains/TrainProtocolLibrary/src/TtisPreStoredMessageCommand.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType TtisPreStoredMessageCommand::MESSAGE_TYPE = 32;
    const unsigned short TtisPreStoredMessageCommand::MESSAGE_LENGTH = 17;
    const unsigned short TtisPreStoredMessageCommand::PID_POSITION = 5;
    const unsigned short TtisPreStoredMessageCommand::LIBRARY_VERSION_POSITION = 6;
    const unsigned short TtisPreStoredMessageCommand::IDENTIFIER_TYPE_POSITION = 8;
    const unsigned short TtisPreStoredMessageCommand::IDENTIFIER_OFFSET_POSITION = 9;
    const unsigned short TtisPreStoredMessageCommand::PRIORITY_POSITION = 10;
    const unsigned short TtisPreStoredMessageCommand::START_HOUR_POSITION = 11;
    const unsigned short TtisPreStoredMessageCommand::START_MINUTE_POSITION = 12;
    const unsigned short TtisPreStoredMessageCommand::STOP_HOUR_POSITION = 13;
    const unsigned short TtisPreStoredMessageCommand::STOP_MINUTE_POSITION = 14;


    TtisPreStoredMessageCommand::TtisPreStoredMessageCommand( CommonTypes::TrainIdType trainId,
                                                              unsigned char origin,
                                                              ProtocolTtisTypes::ETtisPid pidSelection,
                                                              unsigned short libraryVersion,
                                                              unsigned char identifierType,
                                                              unsigned char identifierOffset,
                                                              ProtocolTtisTypes::ETtisPriority priority,
                                                              const ProtocolTtisTypes::TtisTime& startTime,
                                                              const ProtocolTtisTypes::TtisTime& stopTime )
            : AbstractTtisCommand( trainId, MESSAGE_LENGTH, MESSAGE_TYPE, origin, TtisPreStoredMessage ),
            m_pidSelection( pidSelection ),
            m_libraryVersion( libraryVersion ),
            m_identifierType( identifierType ),
            m_identifierOffset( identifierOffset ),
            m_priority( priority ),
            m_startTime( startTime ),
            m_stopTime( stopTime )
    {
        FUNCTION_ENTRY( "TtisPreStoredMessageCommand" );

        setByte( PID_POSITION, static_cast<unsigned char>( m_pidSelection ) );
        setWord( LIBRARY_VERSION_POSITION, m_libraryVersion );
        setByte( IDENTIFIER_TYPE_POSITION, m_identifierType );
        setByte( IDENTIFIER_OFFSET_POSITION, m_identifierOffset );
        setByte( PRIORITY_POSITION, static_cast<unsigned char>( m_priority ) );
        setByte( START_HOUR_POSITION, m_startTime.hour );
        setByte( START_MINUTE_POSITION, m_startTime.minute );
        setByte( STOP_HOUR_POSITION, m_stopTime.hour );
        setByte( STOP_MINUTE_POSITION, m_stopTime.minute );

        FUNCTION_EXIT;
    }


    TtisPreStoredMessageCommand::~TtisPreStoredMessageCommand()
    {
        FUNCTION_ENTRY( "~TtisPreStoredMessageCommand" );
        FUNCTION_EXIT;
    }


    std::string TtisPreStoredMessageCommand::getLogString()
    {
        FUNCTION_ENTRY( "getLogString" );

        std::ostringstream logStream;
        logStream << AbstractTtisCommand::getLogString();
        logStream << "[Pid=" << std::hex << static_cast<int>( m_pidSelection ) << "]"
                  << "[LibraryVersion=" << static_cast<unsigned int>( m_libraryVersion ) << "]"
                  << "[IdentifierType=" << static_cast<unsigned int>( m_identifierType ) << "]"
                  << "[IdentifierOffset" << static_cast<unsigned int>( m_identifierOffset ) << "]"
                  << "[Priority=" << static_cast<unsigned int>( m_priority ) << "]"
                  << "[StartHour=" << static_cast<unsigned int>( m_startTime.hour ) << "]"
                  << "[StartMinute" << static_cast<unsigned int>( m_startTime.minute ) << "]"
                  << "[StopHour=" << static_cast<unsigned int>( m_stopTime.hour ) << "]"
                  << "[StopMinute=" << static_cast<unsigned int>( m_stopTime.minute ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }

}
