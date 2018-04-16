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
#include "bus/trains/TrainProtocolLibrary/src/CallTrainCommandReceivedEvent.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolException.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>


namespace TA_IRS_Bus
{
    const TrainMessageType CallTrainCommandReceivedEvent::MESSAGE_TYPE = 78;
    const unsigned short CallTrainCommandReceivedEvent::EXPECTED_MESSAGE_LENGTH = 9;
    const unsigned short CallTrainCommandReceivedEvent::STATUS_POSITION = 5;
    const unsigned short CallTrainCommandReceivedEvent::AUDIO_MODE_POSITION = 6;


        CallTrainCommandReceivedEvent::CallTrainCommandReceivedEvent( const TrainMessageData& data,
                                                                  const std::string& sendingTsi,
                                                                  const std::string& destinationTsi )
            : TimsEvent( data, sendingTsi, destinationTsi )
    {
        FUNCTION_ENTRY( "CallTrainCommandReceivedEvent" );

        // read the fields
        m_commandStatus = static_cast<ProtocolCallTypes::ECallTrainCommandStatus>( getByte( STATUS_POSITION ) );
        m_audioMode = static_cast< EAudioMode >( getByte( AUDIO_MODE_POSITION ) );

        // validate the fields
        switch ( m_commandStatus )
        {
            // valid values

            case ProtocolCallTypes::CallTrainProceeding:
                // fall through

            case ProtocolCallTypes::CallTrainAudioRouteInUse:
                // fall through

            case ProtocolCallTypes::CallTrainAudioRouteInUseOptionToOverride:
                // fall through

            case ProtocolCallTypes::CallTrainReset:
                break;

            // invalid values
            default:
                std::ostringstream statusStr;
                statusStr << static_cast<int>( m_commandStatus );
                TA_THROW( ProtocolException( "Invalid status received", "Status 15", statusStr.str() ) );
				// fall through
        }

        switch ( m_audioMode )
        {
            // valid values

            case Normal:
                // fall through

            case Opposite:
                break;

            // invalid values
            default:
                std::ostringstream audioStr;
                audioStr << static_cast<int>( m_audioMode );
                TA_THROW( ProtocolException( "Invalid audio mode received", "AudioMode (Status 7)", audioStr.str() ) );
			// fall through
        }

        FUNCTION_EXIT;
    }


    CallTrainCommandReceivedEvent::~CallTrainCommandReceivedEvent()
    {
        FUNCTION_ENTRY( "~CallTrainCommandReceivedEvent" );
        FUNCTION_EXIT;
    }


    TrainMessageType CallTrainCommandReceivedEvent::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;
    }


    std::string CallTrainCommandReceivedEvent::getLogString() const
    {
        FUNCTION_ENTRY( "getLogString" );

        //appends more information in the format:
        //"[Result=X][AudioMode=Y]"
        std::ostringstream logStream;
        logStream << TimsEvent::getLogString();
        logStream << "[CommandStatus=" << static_cast<int>( m_commandStatus ) << "]"
                  << "[AudioMode=" << static_cast<int>( m_audioMode ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }


    ProtocolCallTypes::ECallTrainCommandStatus CallTrainCommandReceivedEvent::getStatus() const
    {
        FUNCTION_ENTRY( "getStatus" );
        FUNCTION_EXIT;
        return m_commandStatus;
    }


    CallTrainCommandReceivedEvent::EAudioMode CallTrainCommandReceivedEvent::getAudioMode() const
    {
        FUNCTION_ENTRY( "getAudioMode" );
        FUNCTION_EXIT;
        return m_audioMode;
    }


    unsigned short CallTrainCommandReceivedEvent::getExpectedMessageLength() const
    {
        FUNCTION_ENTRY( "getExpectedMessageLength" );
        FUNCTION_EXIT;
        return EXPECTED_MESSAGE_LENGTH;
    }

}

