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
#include "bus/trains/TrainProtocolLibrary/src/CctvSingleCommand.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType CctvSingleCommand::MESSAGE_TYPE = 4;
    const unsigned short CctvSingleCommand::MESSAGE_LENGTH = 8;
    const unsigned short CctvSingleCommand::CAMERA_POSITION = 5;


    CctvSingleCommand::CctvSingleCommand( CommonTypes::TrainIdType trainId,
                                          unsigned char origin,
                                          ProtocolCctvTypes::ECctvCamera camera )
        : AbstractCctvCommand( trainId, MESSAGE_LENGTH, MESSAGE_TYPE, origin ),
          m_camera( camera )
    {
        FUNCTION_ENTRY( "CctvSingleCommand" );

        setByte( CAMERA_POSITION, static_cast<unsigned char>( m_camera ) );

        FUNCTION_EXIT;
    }


    CctvSingleCommand::~CctvSingleCommand()
    {
        FUNCTION_ENTRY( "~CctvSingleCommand" );
        FUNCTION_EXIT;

    }


    ProtocolCctvTypes::ECctvCamera CctvSingleCommand::getCamera()
    {
        FUNCTION_ENTRY( "getCamera" );
        FUNCTION_EXIT;
        return m_camera;
    }


    std::string CctvSingleCommand::getLogString()
    {
        FUNCTION_ENTRY( "getLogString" );

        //append the following:
        //"[Camera=X]"
        std::ostringstream logStream;
        logStream << TimsCommand::getLogString();
        logStream << "[Camera=" << static_cast<int>( m_camera ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }

}
