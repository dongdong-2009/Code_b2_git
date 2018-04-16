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

#include "bus/trains/TrainProtocolLibrary/src/TtisResetCommand.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType TtisResetCommand::MESSAGE_TYPE = 35;
    const unsigned short TtisResetCommand::MESSAGE_LENGTH = 9;
    const unsigned short TtisResetCommand::PID_POSITION = 5;
    const unsigned short TtisResetCommand::RESET_TYPE_POSITION = 6;


    TtisResetCommand::~TtisResetCommand()
    {
        FUNCTION_ENTRY( "~TtisResetCommand" );
        FUNCTION_EXIT;
    }


    TtisResetCommand::TtisResetCommand( CommonTypes::TrainIdType trainId,
                                        unsigned char origin,
                                        ProtocolTtisTypes::ETtisPid pidSelection,
                                        ProtocolTtisTypes::ETtisResetMessageType messageType )
        : AbstractTtisCommand( trainId, MESSAGE_LENGTH, MESSAGE_TYPE, origin, TtisResetMessage ),
          m_pidSelection( pidSelection ),
          m_messageType( messageType )
    {
        FUNCTION_ENTRY( "TtisResetCommand" );

        setByte( PID_POSITION, static_cast<unsigned char>( m_pidSelection ) );
        setByte( RESET_TYPE_POSITION, static_cast<unsigned char>( m_messageType ) );

        FUNCTION_EXIT;
    }


    std::string TtisResetCommand::getLogString()
    {
        FUNCTION_ENTRY( "getLogString" );

        // appends more information in the format:
        // "[Pid=X][Type=X]"
        std::ostringstream logStream;
        logStream << AbstractTtisCommand::getLogString();
        logStream << "[Pid=" << std::hex << static_cast<int>( m_pidSelection ) << "]"
                  << "[Type=" << static_cast<int>( m_messageType ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }

}
