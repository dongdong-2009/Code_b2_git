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

#include "bus/trains/TrainProtocolLibrary/src/TtisFreeTextMessageCommand.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolException.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>
#include <iomanip>


namespace TA_IRS_Bus
{
    const TrainMessageType TtisFreeTextMessageCommand::MESSAGE_TYPE = 31;
    const unsigned short TtisFreeTextMessageCommand::MESSAGE_LENGTH = 15;
    const unsigned short TtisFreeTextMessageCommand::PID_POSITION = 5;
    const unsigned short TtisFreeTextMessageCommand::DISPLAY_ATTRIBUTES_POSITION = 6;
    const unsigned short TtisFreeTextMessageCommand::PRIORITY_POSITION = 7;
    const unsigned short TtisFreeTextMessageCommand::START_HOUR_POSITION = 8;
    const unsigned short TtisFreeTextMessageCommand::START_MINUTE_POSITION = 9;
    const unsigned short TtisFreeTextMessageCommand::STOP_HOUR_POSITION = 10;
    const unsigned short TtisFreeTextMessageCommand::STOP_MINUTE_POSITION = 11;
    const unsigned short TtisFreeTextMessageCommand::REPEAT_INTERVAL_POSITION = 12;
    const unsigned short TtisFreeTextMessageCommand::MESSAGE_POSITION = 13;
    const unsigned short TtisFreeTextMessageCommand::MAX_MESSAGE_LENGTH = 124;


    TtisFreeTextMessageCommand::TtisFreeTextMessageCommand( CommonTypes::TrainIdType trainId,
                                                            unsigned char origin,
                                                            ProtocolTtisTypes::ETtisPid pidSelection,
                                                            ProtocolTtisTypes::ETtisFontSize fontSize,
                                                            ProtocolTtisTypes::ETtisJustification justification,
                                                            ProtocolTtisTypes::ETtisIntensity intensity,
                                                            ProtocolTtisTypes::ETtisDisplayMode displayMode,
                                                            ProtocolTtisTypes::ETtisPriority priority,
                                                            const ProtocolTtisTypes::TtisTime& startTime,
                                                            const ProtocolTtisTypes::TtisTime& stopTime,
                                                            unsigned char repeatInterval,
                                                            const std::string& message )
        : AbstractTtisCommand( trainId, MESSAGE_LENGTH, MESSAGE_TYPE, origin, TtisFreeTextMessage ),
          m_pidSelection( pidSelection ),
          m_fontSize( fontSize ),
          m_justification( justification ),
          m_intensity( intensity ),
          m_displayMode( displayMode ),
          m_priority( priority ),
          m_startTime( startTime ),
          m_stopTime( stopTime ),
          m_repeatInterval( repeatInterval ),
          m_message( message ),
          m_displayAttributesByte( 0 )
    {
        FUNCTION_ENTRY( "TtisFreeTextMessageCommand" );

        if ( m_message.size() > MAX_MESSAGE_LENGTH )
        {
            std::ostringstream messageLengthStream;
            messageLengthStream << m_message.size();
            TA_THROW( ProtocolException( "Message too long", "MessageLength", messageLengthStream.str() ) );
        }

        // all those display options fit into one byte
        m_displayAttributesByte = combineDisplayAttributes();

        setByte( PID_POSITION, static_cast<unsigned char>( m_pidSelection ) );
        setByte( DISPLAY_ATTRIBUTES_POSITION, m_displayAttributesByte );
        setByte( PRIORITY_POSITION, static_cast<unsigned char>( m_priority ) );
        setByte( START_HOUR_POSITION, m_startTime.hour );
        setByte( START_MINUTE_POSITION, m_startTime.minute );
        setByte( STOP_HOUR_POSITION, m_stopTime.hour );
        setByte( STOP_MINUTE_POSITION, m_stopTime.minute );
        setByte( REPEAT_INTERVAL_POSITION, m_repeatInterval );

        setNewLength( getLength() + m_message.size() );

        unsigned short i = MESSAGE_POSITION;

        for ( std::string::iterator iter = m_message.begin();
              iter != m_message.end();
              ++iter, ++i )
        {
            setByte( i, *iter );
        }

        FUNCTION_EXIT;
    }


    TtisFreeTextMessageCommand::~TtisFreeTextMessageCommand()
    {
        FUNCTION_ENTRY( "~TtisFreeTextMessageCommand" );
        FUNCTION_EXIT;
    }


    std::string TtisFreeTextMessageCommand::getLogString()
    {
        FUNCTION_ENTRY( "getLogString" );
        // appends more information in the format:
        // "[Pid=X][FontSize=X][Justification=X][Intensity=X][DisplayMode=X][CombinedDisplayAttributes=X][Priority=X]
        //  [StartHour=X][StartMinute=X][StopHour=X][StopMinute=X][RepeatInterval=X][Message=X]"
        std::ostringstream logStream;
        logStream << AbstractTtisCommand::getLogString();
        logStream << "[Pid=" << std::hex << static_cast<int>( m_pidSelection )<< "]"
                  << "[FrontSize=" << static_cast<int>( m_fontSize ) << "]"
                  << "[Justification=" << static_cast<int>( m_justification ) << "]"
                  << "[Intensity=" << static_cast<int>( m_intensity ) << "]"
                  << "[DisplayMode=" << static_cast<int>( m_displayMode ) << "]"
                  << "[CombinedDisplayAttributes=" << std::hex << std::setw(2) << std::setfill('0')
                                                   << static_cast<int>( m_displayAttributesByte )<< "]"
                  << "[Priority=" << static_cast<int>( m_priority ) << "]"
                  << "[StartHour=" << std::setw(2) << std::setfill('0')
                                   << static_cast<unsigned int>( m_startTime.hour ) << "]"
                  << "[StartMinute" << std::setw(2) << std::setfill('0')
                                    << static_cast<unsigned int>( m_startTime.minute ) << "]"
                  << "[StopHour=" << std::setw(2) << std::setfill('0')
                                  << static_cast<unsigned int>( m_stopTime.hour ) << "]"
                  << "[StopMinute=" << std::setw(2) << std::setfill('0')
                                    << static_cast<unsigned int>( m_stopTime.minute ) << "]"
                  << "[RepeatInterval=" << static_cast<unsigned int>( m_repeatInterval ) << "]"
                  << "[Message=" << m_message << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }


    unsigned char TtisFreeTextMessageCommand::combineDisplayAttributes()
    {
        FUNCTION_ENTRY( "combineDisplayAttributes" );

        unsigned char displayAttributesByte;
        displayAttributesByte = 0x0;

        switch ( m_fontSize )
        {

            case ProtocolTtisTypes::TtisFontSmall:
                // Do nothing - already set to 0
                break;

            case ProtocolTtisTypes::TtisFontLarge:
                displayAttributesByte = displayAttributesByte | 0x80; // bits 8 and 7 (10 000000)
                break;

        }

        switch ( m_justification )
        {

            case ProtocolTtisTypes::TtisJustificationLeft:
                // Do nothing - already set to 0
                break;

            case ProtocolTtisTypes::TtisJustificationCentre:
                displayAttributesByte = displayAttributesByte | 0x10;  // bits 6 and 5 (00 01 0000)
                break;

            case ProtocolTtisTypes::TtisJustificationRight:
                displayAttributesByte = displayAttributesByte | 0x20; // bits 6 and 5 (00 10 0000)
                break;

        }

        switch ( m_intensity )
        {

            case ProtocolTtisTypes::TtisIntensityNormal:
                // Do nothing - already set to 0
                break;

            case ProtocolTtisTypes::TtisIntensityHigh:
                displayAttributesByte = displayAttributesByte | 0x08; // bit 4 (0000 1 000)
                break;

        }

        switch ( m_displayMode )
        {

            case ProtocolTtisTypes::TtisDisplayModeScrollLeft:
                // Do nothing - already set to 0
                break;

            case ProtocolTtisTypes::TtisDisplayModeScrollRight:
                displayAttributesByte = displayAttributesByte | 0x01; // bits 3, 2, 1 (00000 001)
                break;

            case ProtocolTtisTypes::TtisDisplayModeScrollUp:
                displayAttributesByte = displayAttributesByte | 0x02; // bits 3, 2, 1 (00000 010)
                break;

            case ProtocolTtisTypes::TtisDisplayModeScrollDown:
                displayAttributesByte = displayAttributesByte | 0x03; // bits 3, 2, 1 (00000 011)
                break;

            case ProtocolTtisTypes::TtisDisplayModeInstantOn:
                displayAttributesByte = displayAttributesByte | 0x04; // bits 3, 2, 1 (00000 100)
                break;

            case ProtocolTtisTypes::TtisDisplayModeBlinking:
                displayAttributesByte = displayAttributesByte | 0x05; // bits 3, 2, 1 (00000 101)
                break;

            case ProtocolTtisTypes::TtisDisplayModeWiping:
                displayAttributesByte = displayAttributesByte | 0x06; // bits 3, 2, 1 (00000 110)
                break;

            case ProtocolTtisTypes::TtisDisplayModeLikeSnow:
                displayAttributesByte = displayAttributesByte | 0x07; // bits 3, 2, 1 (00000 111)
                break;
        }

        FUNCTION_EXIT;
        return displayAttributesByte;
    }

}
