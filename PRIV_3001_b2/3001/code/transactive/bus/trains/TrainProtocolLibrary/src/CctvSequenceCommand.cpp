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
#include "bus/trains/TrainProtocolLibrary/src/CctvSequenceCommand.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolException.h"

#include "core/utilities/src/DebugUtilInit.h"


namespace TA_IRS_Bus
{
    const TrainMessageType CctvSequenceCommand::MESSAGE_TYPE = 5;
    const unsigned short CctvSequenceCommand::MESSAGE_LENGTH = 9;
    const unsigned short CctvSequenceCommand::SEQUENCE_POSITION = 5;
    const unsigned short CctvSequenceCommand::SEQUENCE_MODE_POSITION = 6;


    CctvSequenceCommand::CctvSequenceCommand( CommonTypes::TrainIdType trainId,
                                              unsigned char origin,
                                              ProtocolCctvTypes::ECctvSequenceNumber sequence,
                                              ProtocolCctvTypes::ECctvSequenceMode sequenceMode )
        : AbstractCctvCommand( trainId, MESSAGE_LENGTH, MESSAGE_TYPE, origin ),
          m_sequence( sequence ),
          m_sequenceMode( sequenceMode )
    {
        FUNCTION_ENTRY( "CctvSequenceCommand" );

        setByte( SEQUENCE_POSITION, static_cast<unsigned char>( m_sequence ) );

        setByte( SEQUENCE_MODE_POSITION, static_cast<unsigned char>( m_sequenceMode ) );

        FUNCTION_EXIT;
    }


    CctvSequenceCommand::~CctvSequenceCommand()
    {
        FUNCTION_ENTRY( "~CctvSequenceCommand" );
        FUNCTION_EXIT;
    }


    ProtocolCctvTypes::ECctvSequenceNumber CctvSequenceCommand::getSequenceNumber()
    {
        FUNCTION_ENTRY( "getSequenceNumber" );
        FUNCTION_EXIT;
        return m_sequence;
    }


    ProtocolCctvTypes::ECctvSequenceMode CctvSequenceCommand::getSequenceMode()
    {
        FUNCTION_ENTRY( "getSequenceMode" );
        FUNCTION_EXIT;
        return m_sequenceMode;
    }


    std::string CctvSequenceCommand::getLogString()
    {
        FUNCTION_ENTRY( "getLogString" );

        std::ostringstream logStream;
        logStream << TimsCommand::getLogString();
        logStream << "[Sequence=" << static_cast<int>( m_sequence ) << "]"
                  << "[Mode=" << static_cast<int>( m_sequenceMode ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }

}
