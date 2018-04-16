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
#include "bus/trains/TrainProtocolLibrary/src/PaContinueCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/PaCommandReceivedEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PaTrainMessageCompletedEvent.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType PaContinueCommand::MESSAGE_TYPE = 18;
    const unsigned short PaContinueCommand::MESSAGE_LENGTH = 8;
    const unsigned short PaContinueCommand::ANNOUNCEMENT_ID_POSITION = 5;


    PaContinueCommand::PaContinueCommand( CommonTypes::TrainIdType trainId,
                                          unsigned char origin,
                                          unsigned char announcementId )
        : TimsCommand( trainId,
                       MESSAGE_LENGTH,
                       MESSAGE_TYPE,
                       origin,
                       false ),
           m_announcementId( announcementId )

    {
        FUNCTION_ENTRY ( "PaContinueCommand" );

        setByte( ANNOUNCEMENT_ID_POSITION, m_announcementId );

        FUNCTION_EXIT;
    }


    PaContinueCommand::~PaContinueCommand()
    {
        FUNCTION_ENTRY( "~PaContinueCommand" );
        FUNCTION_EXIT;
    }


    TrainCommand::TrainResponseResult PaContinueCommand::setResponse( TrainEventPtr response )
    {
        FUNCTION_ENTRY( "setResponse" );
        FUNCTION_EXIT;
        return TrainResponseResult( false, false );
    }


    std::string PaContinueCommand::getLogString()
    {
        FUNCTION_ENTRY( "getLogString" );

        std::ostringstream logStream;
        logStream << TimsCommand::getLogString();
        logStream << "[AnnounceMentId=" << static_cast<unsigned int>( m_announcementId ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }

}
