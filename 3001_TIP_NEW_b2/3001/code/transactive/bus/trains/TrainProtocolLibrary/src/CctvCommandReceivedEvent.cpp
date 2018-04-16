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
#include "bus/trains/TrainProtocolLibrary/src/CctvCommandReceivedEvent.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolException.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>


namespace TA_IRS_Bus
{
    const TrainMessageType CctvCommandReceivedEvent::MESSAGE_TYPE = 9;
    const unsigned short CctvCommandReceivedEvent::EXPECTED_MESSAGE_LENGTH = 8;
    const unsigned short CctvCommandReceivedEvent::RESULT_POSITION = 5;


    CctvCommandReceivedEvent::CctvCommandReceivedEvent( const TrainMessageData& data,
                                                        const std::string& sendingTsi,
                                                        const std::string& destinationTsi )
            : TimsEvent( data, sendingTsi, destinationTsi )
    {
        FUNCTION_ENTRY( "CctvCommandReceivedEvent" );
        
        // read the result
        m_result = static_cast< ProtocolCctvTypes::ECctvCommandStatus >( getByte( RESULT_POSITION ) );

        // verify the result
        switch ( m_result )
        {
            // valid values

            case ProtocolCctvTypes::CctvProceeding:
                // fall through

            case ProtocolCctvTypes::CctvInUse:
                // fall through

            case ProtocolCctvTypes::CctvFailed:
                break;

            // invalid values
            default:
                std::ostringstream resultStr;
                resultStr << static_cast<int>( m_result );
                TA_THROW( ProtocolException( "Invalid Command Result (Status 1)", "Result (Status 1)", resultStr.str() ) );
			// fall through
        }

        FUNCTION_EXIT;
    }


    CctvCommandReceivedEvent::~CctvCommandReceivedEvent()
    {
        FUNCTION_ENTRY( "~CctvCommandReceivedEvent" );
        FUNCTION_EXIT;
    }


    TrainMessageType CctvCommandReceivedEvent::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;
    }


    ProtocolCctvTypes::ECctvCommandStatus CctvCommandReceivedEvent::getResult() const
    {
        FUNCTION_ENTRY( "getResult" );
        FUNCTION_EXIT;
        return m_result;
    }


    std::string CctvCommandReceivedEvent::getLogString() const
    {
        FUNCTION_ENTRY( "getLogString" );
        std::ostringstream logStream;
        logStream << TimsEvent::getLogString();
        logStream << "[Result=" << static_cast<int>( m_result ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }


    unsigned short CctvCommandReceivedEvent::getExpectedMessageLength() const
    {
        FUNCTION_ENTRY( "getExpectedMessageLength" );
        FUNCTION_EXIT;
        return EXPECTED_MESSAGE_LENGTH;
    }

}  //end of namespace
