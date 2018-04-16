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
#include "bus/trains/TrainProtocolLibrary/src/PecCommandReceivedEvent.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolException.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>


namespace TA_IRS_Bus
{
    const TrainMessageType PecCommandReceivedEvent::MESSAGE_TYPE = 55;
    const unsigned short PecCommandReceivedEvent::EXPECTED_MESSAGE_LENGTH = 9;
    const unsigned short PecCommandReceivedEvent::RESULT_POSITION = 6;


    PecCommandReceivedEvent::PecCommandReceivedEvent( const TrainMessageData& data,
                                                      const std::string& sendingTsi,
                                                      const std::string& destinationTsi )

            : AbstractPecEvent( data, sendingTsi, destinationTsi )
    {
        FUNCTION_ENTRY( "PecCommandReceivedEvent" );

        m_pecResult = static_cast<ProtocolPecTypes::EPecCommandResult>( getByte( RESULT_POSITION ) );

        switch ( m_pecResult )
        {
            // valid values

            case ProtocolPecTypes::PecProceeding:
                // fall through

            case ProtocolPecTypes::PecInUse:
                // fall through

            case ProtocolPecTypes::PecFailed:
                break;

            // invalid values
            default:
                std::ostringstream pecStr;
                pecStr << static_cast<int>( m_pecResult );
                TA_THROW( ProtocolException( "Invalid PEC result received", "Result (Status 6)", pecStr.str() ) );
        }

        FUNCTION_EXIT;
    }


    PecCommandReceivedEvent::~PecCommandReceivedEvent()
    {
        FUNCTION_ENTRY( "~PecCommandReceivedEvent" );
        FUNCTION_EXIT;
    }


    TrainMessageType PecCommandReceivedEvent::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;
    }


    std::string PecCommandReceivedEvent::getLogString() const
    {
        FUNCTION_ENTRY( "getLogString" );

        // append the following:
        // "[Status=X]"
        std::ostringstream logStream;
        logStream << AbstractPecEvent::getLogString();
        logStream << "[Status=" << static_cast<int>( m_pecResult );

        FUNCTION_EXIT;
        return logStream.str();
    }


    ProtocolPecTypes::EPecCommandResult PecCommandReceivedEvent::getResult() const
    {
        FUNCTION_ENTRY( "getResult" );
        FUNCTION_EXIT;
        return m_pecResult;
    }


    unsigned short PecCommandReceivedEvent::getExpectedMessageLength() const
    {
        FUNCTION_ENTRY( "getExpectedMessageLength" );
        FUNCTION_EXIT;
        return EXPECTED_MESSAGE_LENGTH;
    }

}

