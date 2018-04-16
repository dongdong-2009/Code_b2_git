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
    const unsigned short TtisPreStoredMessageCommand::MESSAGE_LENGTH = 15;
    const unsigned short TtisPreStoredMessageCommand::PID_POSITION = 5;
    const unsigned short TtisPreStoredMessageCommand::LIBRARY_VERSION_POSITION = 6;
    const unsigned short TtisPreStoredMessageCommand::PERRUN_POSITION = 10;
    const unsigned short TtisPreStoredMessageCommand::MESSAGEID_POSITION = 11;
 


    TtisPreStoredMessageCommand::TtisPreStoredMessageCommand( CommonTypes::TrainIdType trainId,
                                                              unsigned char origin,
                                                              ProtocolTtisTypes::ETtisPid pidSelection,
                                                              unsigned int libraryVersion,
															  unsigned char perRun,
                                                              unsigned short messageId
                                                              )
            : AbstractTtisCommand( trainId, MESSAGE_LENGTH, MESSAGE_TYPE, origin, TtisPreStoredMessage ),
            m_pidSelection( pidSelection ),
            m_libraryVersion( libraryVersion ),
            m_perRun( perRun ),
            m_messageId( messageId )           
    {
        FUNCTION_ENTRY( "TtisPreStoredMessageCommand" );

        setByte( PID_POSITION, static_cast<unsigned char>( m_pidSelection ) );
        setLong( LIBRARY_VERSION_POSITION, m_libraryVersion );
        setByte( PERRUN_POSITION, m_perRun );
        setWord( MESSAGEID_POSITION, messageId );   

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
                  << "[perRun=" << static_cast<unsigned short>( m_perRun ) << "]"
                  << "[message Id=" << static_cast<unsigned short>( m_messageId ) << "]"  ;

        FUNCTION_EXIT;
        return logStream.str();
    }

}
