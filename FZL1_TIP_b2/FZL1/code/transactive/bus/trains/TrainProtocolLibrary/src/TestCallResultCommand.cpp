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
#include "bus/trains/TrainProtocolLibrary/src/TestCallResultCommand.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType TestCallResultCommand::MESSAGE_TYPE = 72;
    const unsigned short TestCallResultCommand::MESSAGE_LENGTH = 8;
    const unsigned short TestCallResultCommand::AREA_POSITION = 5;


    TestCallResultCommand::TestCallResultCommand( CommonTypes::TrainIdType trainId,
                                                  unsigned char origin,
                                                  ETrainArea area )
            : TimsCommand( trainId, MESSAGE_LENGTH, MESSAGE_TYPE, origin, false )
    {
        FUNCTION_ENTRY( "TestCallResultCommand" );

        m_area = area;
        setByte( AREA_POSITION, static_cast<unsigned char>( m_area ) );

        FUNCTION_EXIT;
    }


    TestCallResultCommand::~TestCallResultCommand()
    {
        FUNCTION_ENTRY( "~TestCallResultCommand" );
        FUNCTION_EXIT;
    }


    std::string TestCallResultCommand::getLogString()
    {
        FUNCTION_ENTRY( "getLogString" );

        std::ostringstream logStream;
        logStream << TimsCommand::getLogString();
        logStream << "[Area=" << static_cast<int>( m_area ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }


    TrainCommand::TrainResponseResult TestCallResultCommand::setResponse( TrainEventPtr response )
    {
        FUNCTION_ENTRY( "setResponse" );
        FUNCTION_EXIT;
        return TrainCommand::TrainResponseResult( false, false );
    }

}
