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
#include "bus/trains/TrainProtocolLibrary/src/BadCommandEvent.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolException.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>
#include <iomanip>


namespace TA_IRS_Bus
{
    const TrainMessageType BadCommandEvent::MESSAGE_TYPE = 66;
    const unsigned short BadCommandEvent::EXPECTED_MESSAGE_LENGTH = 9;
    const unsigned short BadCommandEvent::ORIGINAL_POSITION = 5;
    const unsigned short BadCommandEvent::ERROR_CODE_POSITION = 6;


    BadCommandEvent::BadCommandEvent( const TrainMessageData& data,
                                      const std::string& sendingTsi,
                                      const std::string& destinationTsi )
            : TimsEvent( data, sendingTsi, destinationTsi )
    {
        FUNCTION_ENTRY( "BadCommandEvent" );

        // extract the original command number
        m_originalCommand = getByte( ORIGINAL_POSITION );

        // extract the error code
        m_errorCode = static_cast<ProtocolCommonTypes::EBadCommandError>( getByte( ERROR_CODE_POSITION ) );

        // validate the error code
        switch ( m_errorCode )
        {
            // valid error codes

            case ProtocolCommonTypes::BadChecksum:
                // fall through

            case ProtocolCommonTypes::UnknownCommand:
                // fall through

            case ProtocolCommonTypes::BadParameter:
                // fall through

            case ProtocolCommonTypes::UnexpectedCommand:
                break;

            // invalid value
            default:
                std::ostringstream errorValue;
                errorValue << static_cast<int>( m_errorCode );
                TA_THROW( ProtocolException( "Invalid Error Code", "ErrorCode", errorValue.str()  ) );
        }

        FUNCTION_EXIT;
    }


    BadCommandEvent::~BadCommandEvent()
    {
        FUNCTION_ENTRY( "~BadCommandEvent" );
        FUNCTION_EXIT;
    }


    TrainMessageType BadCommandEvent::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;
    }


    TrainMessageType BadCommandEvent::getOriginalCommand() const
    {
        FUNCTION_ENTRY( "getOriginalCommand" );
        FUNCTION_EXIT;
        return m_originalCommand;
    }


    ProtocolCommonTypes::EBadCommandError BadCommandEvent::getErrorCode() const
    {
        FUNCTION_ENTRY( "getErrorCode" );
        FUNCTION_EXIT;
        return m_errorCode;
    }


    std::string BadCommandEvent::getLogString() const
    {
        FUNCTION_ENTRY( "getLogString" );

        // appends more information in the format:
        // "[Original Command=X][Error Code=X]"
        std::ostringstream logStream;
        logStream << TimsEvent::getLogString();
        logStream << "[OriginalCommand=" << std::setw(2) << std::setfill('0')
                                         << static_cast<unsigned int>( m_originalCommand ) << "]"
                  << "[ErrorCode=" << std::setw(2) << std::setfill('0')
                                   << static_cast<int>( m_errorCode ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }


    unsigned short BadCommandEvent::getExpectedMessageLength() const
    {
        FUNCTION_ENTRY( "getExpectedMessageLength" );
        FUNCTION_EXIT;
        return EXPECTED_MESSAGE_LENGTH;
    }

} // end of namespace
