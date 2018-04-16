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

#include "bus/trains/TrainProtocolLibrary/src/EndOfTtisDownloadEvent.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolException.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>
#include <iomanip>


namespace TA_IRS_Bus
{
    const TrainMessageType EndOfTtisDownloadEvent::MESSAGE_TYPE = 39;
    const unsigned short EndOfTtisDownloadEvent::RESULT_POSITION = 5;
    const unsigned short EndOfTtisDownloadEvent::REPETITION_POSITION = 6;


    EndOfTtisDownloadEvent::EndOfTtisDownloadEvent( const TrainMessageData& data,
                                                    const std::string& sendingTsi,
                                                    const std::string& destinationTsi )
            : TimsEvent( data, sendingTsi, destinationTsi )
    {
        FUNCTION_ENTRY( "EndOfTtisDownloadEvent" );

        m_result = static_cast<ETtisDownloadResult>( getByte( RESULT_POSITION ) );

        switch ( m_result )
        {
            // valid values

            case TtisDownloadOk:
                // fall through

            case TtisDownloadFailed:
                // fall through

            case TtisDownloadRepetition:
                break;

            // invalid values
            default:
                std::ostringstream invalidResult;
                invalidResult << static_cast<int>( m_result );
                TA_THROW( ProtocolException( "Invalid result received", "Result (Status 10)", invalidResult.str() ) );
        }

        if ( TtisDownloadRepetition == m_result )
        {
            for ( unsigned short i = REPETITION_POSITION; i < getUserDataLength(); i += 2 )
            {
                m_framesToRepeat.push_back( getWord( i ) );
            }

            if ( m_framesToRepeat.size() == 0 )
            {
                TA_THROW( ProtocolException( "Repetition requested, but no frames given.", "RepeatedFrames", "0" ) );
            }
        }

        FUNCTION_EXIT;
    }


    EndOfTtisDownloadEvent::~EndOfTtisDownloadEvent()
    {
        FUNCTION_ENTRY( "~EndOfTtisDownloadEvent" );
        FUNCTION_EXIT;
    }


    TrainMessageType EndOfTtisDownloadEvent::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;
    }


    EndOfTtisDownloadEvent::ETtisDownloadResult EndOfTtisDownloadEvent::getResult() const
    {
        FUNCTION_ENTRY( "getResult" );
        FUNCTION_EXIT;
        return m_result;
    }


    std::vector<unsigned short> EndOfTtisDownloadEvent::getFramesToRepeat() const
    {
        FUNCTION_ENTRY( "getFramesToRepeat" );
        FUNCTION_EXIT;
        return m_framesToRepeat;

    }


    std::string EndOfTtisDownloadEvent::getLogString() const
    {
        FUNCTION_ENTRY( "getLogString" );

        // appends more information in the format:
        // "[Result=X][FramesToRepeat=X,Y,Z...]"
        std::ostringstream logStream;
        logStream << TimsEvent::getLogString();
        logStream << "[Result=" << static_cast<int>( m_result ) << "]";

        if ( TtisDownloadRepetition == m_result )
        {
            logStream << "[FramesToRepeat=";

            for ( std::vector<unsigned short>::const_iterator frameIter = m_framesToRepeat.begin();
                  frameIter != m_framesToRepeat.end(); ++frameIter )
            {
                logStream << " " << std::setw(2) << std::setfill('0') << static_cast<unsigned int>( *frameIter );
            }
            
            logStream << "]";
        }

        FUNCTION_EXIT;
        return logStream.str();
    }

}

