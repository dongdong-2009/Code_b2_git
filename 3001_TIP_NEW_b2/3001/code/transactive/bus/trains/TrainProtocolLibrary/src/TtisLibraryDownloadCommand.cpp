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
#include "bus/trains/TrainProtocolLibrary/src/TtisLibraryDownloadCommand.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType TtisLibraryDownloadCommand::MESSAGE_TYPE = 33;
    const unsigned short TtisLibraryDownloadCommand::MESSAGE_LENGTH = 7;
    const unsigned short TtisLibraryDownloadCommand::COMMAND_POSITION = 5;


    TtisLibraryDownloadCommand::TtisLibraryDownloadCommand( CommonTypes::TrainIdType trainId,
                                                            unsigned char origin,
                                                            ETtisLibraryDownloadCommand commandType )
        : AbstractTtisCommand( trainId,
                               MESSAGE_LENGTH,
                               MESSAGE_TYPE,
                               origin,
                               TtisLibraryDownload ),
          m_commandType( commandType )
    {
        FUNCTION_ENTRY ( "TtisLibraryDownloadCommand" );

        setByte( COMMAND_POSITION, static_cast<unsigned char>( m_commandType ) );

        FUNCTION_EXIT;
    }


    TtisLibraryDownloadCommand::~TtisLibraryDownloadCommand()
    {
        FUNCTION_ENTRY( "~TtisLibraryDownloadCommand" );
        FUNCTION_EXIT;
    }


    std::string TtisLibraryDownloadCommand::getLogString()
    {
        FUNCTION_ENTRY( "getLogString" );

        //appends more information in the format:
        //"[CommandType=X]"
        std::ostringstream logStream;
        logStream << AbstractTtisCommand::getLogString();
        logStream << "[CommandType=" << static_cast<int>( m_commandType ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }

}
