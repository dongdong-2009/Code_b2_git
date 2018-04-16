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
#include "bus/trains/TrainProtocolLibrary/src/AbstractPecEvent.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolPecTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolException.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>
#include <iomanip>


namespace TA_IRS_Bus
{
    const unsigned short AbstractPecEvent::EXPECTED_MESSAGE_LENGTH = 8;
    const unsigned short AbstractPecEvent::PEC_NUMBER_POSITION = 5;
	const unsigned short AbstractPecEvent::PEC_RQSORACK_POSITION = 6;


    AbstractPecEvent::AbstractPecEvent( const TrainMessageData& data,
                                        const std::string& sendingTsi,
                                        const std::string& destinationTsi )
        : TimsEvent( data, sendingTsi, destinationTsi )
    {
        FUNCTION_ENTRY( "AbstractPecEvent" );

        m_combinedPecByte = getByte( PEC_NUMBER_POSITION );

		m_rqsOrAck = static_cast<ProtocolPecTypes::EPECFlagNumber>( getByte( PEC_RQSORACK_POSITION ) );

        // the CAR number is the upper half of the byte (then shift down 4 bits)
        m_carNumber = static_cast<ProtocolPecTypes::EPecCarNumber>( ( m_combinedPecByte & 0xf0 ) >> 4 );

        // the PEC number is the lower half of the byte
        m_pecNumber = static_cast<ProtocolPecTypes::EPecNumber>( m_combinedPecByte & 0x0f );

        switch ( m_carNumber )
        {
            // valid values

            case ProtocolPecTypes::AllCars:
                // fall through

            case ProtocolPecTypes::CarOne:
                // fall through

            case ProtocolPecTypes::CarTwo:
                // fall through

            case ProtocolPecTypes::CarThree:
                break;

            // invalid values
            default:
                std::ostringstream carNumberstr;
                carNumberstr << static_cast<int>( m_carNumber );
                TA_THROW( ProtocolException( "Invalid CAR number received", "CarNumber", carNumberstr.str() ) );
        }


        switch ( m_pecNumber )
        {
            // valid values

            case ProtocolPecTypes::AllPec:
                // fall through

            case ProtocolPecTypes::PecOne:
                // fall through

            case ProtocolPecTypes::PecTwo:
                // fall through

            case ProtocolPecTypes::PecThree:
                // fall through

            case ProtocolPecTypes::PecFour:
                break;

            // invalid values
            default:
                std::ostringstream PecNumberStr;
                PecNumberStr << static_cast<int>( m_pecNumber );
                TA_THROW( ProtocolException( "Invalid PEC number received", "PecNumber", PecNumberStr.str() ) );
        }

        FUNCTION_EXIT;
    }


    AbstractPecEvent::~AbstractPecEvent()
    {
        FUNCTION_ENTRY( "~AbstractPecEvent" );
        FUNCTION_EXIT;
    }


    std::string AbstractPecEvent::getLogString() const
    {
        FUNCTION_ENTRY( "getLogString" );

        // append the following:
        // "[Car=X][Pec=X][PecByte=X]"
        std::ostringstream logStream;
        logStream << TimsEvent::getLogString();
        logStream << "[Car=" << static_cast<int>( m_carNumber ) << "]"
                  << "[Pec=" << static_cast<int>( m_pecNumber ) << "]"
                  << "[Pec=" << std::hex << std::setw(2) << std::setfill('0')
                             << static_cast<unsigned int>( m_combinedPecByte ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }


    ProtocolPecTypes::EPecCarNumber AbstractPecEvent::getCar() const
    {
        FUNCTION_ENTRY( "getCar" );
        FUNCTION_EXIT;
        return  m_carNumber;
    }


    ProtocolPecTypes::EPecNumber AbstractPecEvent::getPec() const
    {
        FUNCTION_ENTRY( "getPec" );
        FUNCTION_EXIT;
        return  m_pecNumber;
    }


    unsigned short AbstractPecEvent::getExpectedMessageLength() const
    {
        FUNCTION_ENTRY( "getExpectedMessageLength" );
        FUNCTION_EXIT;
        return EXPECTED_MESSAGE_LENGTH;
    }

	ProtocolPecTypes::EPECFlagNumber AbstractPecEvent::getRqsOrAck() const
	{
		FUNCTION_ENTRY( "getRqsOrAck" );
        FUNCTION_EXIT;
        return  m_rqsOrAck;
	}

} //end of namespace

