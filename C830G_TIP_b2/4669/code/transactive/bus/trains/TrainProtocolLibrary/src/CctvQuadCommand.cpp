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
#include "bus/trains/TrainProtocolLibrary/src/CctvQuadCommand.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType CctvQuadCommand::MESSAGE_TYPE = 3;
    const unsigned short CctvQuadCommand::MESSAGE_LENGTH = 11;
    const unsigned short CctvQuadCommand::CAMERA_W_POSITION = 5;
    const unsigned short CctvQuadCommand::CAMERA_X_POSITION = 6;
    const unsigned short CctvQuadCommand::CAMERA_Y_POSITION = 7;
    const unsigned short CctvQuadCommand::CAMERA_Z_POSITION = 8;


    CctvQuadCommand::CctvQuadCommand( CommonTypes::TrainIdType trainId,
                                      unsigned char origin,
                                      ProtocolCctvTypes::ECctvCamera cameraW,
                                      ProtocolCctvTypes::ECctvCamera cameraX,
                                      ProtocolCctvTypes::ECctvCamera cameraY,
                                      ProtocolCctvTypes::ECctvCamera cameraZ )
        : AbstractCctvCommand( trainId, MESSAGE_LENGTH, MESSAGE_TYPE, origin ),
          m_cameraW( cameraW ),
          m_cameraX( cameraX ),
          m_cameraY( cameraY ),
          m_cameraZ( cameraZ )
    {
        FUNCTION_ENTRY( "CctvQuadCommand" );

        setByte( CAMERA_W_POSITION, static_cast<unsigned char>( m_cameraW ) );
        setByte( CAMERA_X_POSITION, static_cast<unsigned char>( m_cameraX ) );
        setByte( CAMERA_Y_POSITION, static_cast<unsigned char>( m_cameraY ) );
        setByte( CAMERA_Z_POSITION, static_cast<unsigned char>( m_cameraZ ) );

        FUNCTION_EXIT;
    }


    CctvQuadCommand::~CctvQuadCommand()
    {
        FUNCTION_ENTRY( "~CctvQuadCommand" );
        FUNCTION_EXIT;
    }


    ProtocolCctvTypes::ECctvCamera CctvQuadCommand::getCameraW()
    {
        FUNCTION_ENTRY( "getCameraW" );
        FUNCTION_EXIT;
        return m_cameraW;
    }


    ProtocolCctvTypes::ECctvCamera CctvQuadCommand::getCameraX()
    {
        FUNCTION_ENTRY( "getCameraX" );
        FUNCTION_EXIT;
        return m_cameraX;
    }


    ProtocolCctvTypes::ECctvCamera CctvQuadCommand::getCameraY()
    {
        FUNCTION_ENTRY( "getCameraY" );
        FUNCTION_EXIT;
        return m_cameraY;
    }


    ProtocolCctvTypes::ECctvCamera CctvQuadCommand::getCameraZ()
    {
        FUNCTION_ENTRY( "getCameraZ" );
        FUNCTION_EXIT;
        return m_cameraZ;
    }


    std::string CctvQuadCommand::getLogString()
    {
        FUNCTION_ENTRY( "getLogString" );

        // append the following:
        // "[CameraW=X][CameraX=X][CameraY=X][CameraZ=X]"
        std::ostringstream logStream;
        logStream << TimsCommand::getLogString();
        logStream << "[CameraW=" << static_cast<int>( m_cameraW ) << "]"
                  << "[CameraX=" << static_cast<int>( m_cameraX ) << "]"
                  << "[CameraY=" << static_cast<int>( m_cameraY ) << "]"
                  << "[CameraZ=" << static_cast<int>( m_cameraZ ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }

}
