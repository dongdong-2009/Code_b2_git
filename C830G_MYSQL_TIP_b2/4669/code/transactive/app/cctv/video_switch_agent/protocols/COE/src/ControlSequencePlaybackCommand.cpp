/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/ControlSequencePlaybackCommand.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Object for requesting the playback of a Sequence on a Monitor be set to a 
  * particular mode, and for processing the response.
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/COE/src/ControlSequencePlaybackCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESequence.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEVideoOutput.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/DebugUtil.h"

#include <sstream>

namespace TA_IRS_App
{

    //
    // ControlSequencePlaybackCommand
    //
    ControlSequencePlaybackCommand::ControlSequencePlaybackCommand( ESequencePlaybackCommandType sequencePlaybackCommandType, unsigned long videoOutputEntityKey)
        : AbstractCOERequestCommand(), m_sequencePlaybackCommandType( sequencePlaybackCommandType ), m_videoOutputEntityKey( videoOutputEntityKey ),
          m_expectedResponseCmd( 0x00 )
    {
        m_data.erase( m_data.begin(), m_data.end() );
        
        COEVideoOutput* videoOutput = COESwitchManager::getInstance().getVideoOutput( m_videoOutputEntityKey );
        if ( 0 == videoOutput )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST,
                "The VideoOutput does not exist." ) );
        }

        unsigned short monID;
        std::istringstream monAddr( videoOutput->getAddress() );  
        if ( false == (monAddr >> monID && monAddr.eof()) )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR, 
                "Invalid value set to Address" ) );            
        }

        std::vector< char > msg( 6, 0x00 );
        
        TA_COE::setPacketLength( 4, msg[ 0 ], msg[ 1 ] );
        msg[ 3 ] = 0x23;
        switch ( sequencePlaybackCommandType )
        {
        case SEQUENCE_HOLD:
            msg[ 4 ] =  0x01;
            break;
        case SEQUENCE_SKIP_FORWARD:
            msg[ 4 ] = 0x02;
            break;
        case SEQUENCE_SKIP_BACKWARD:
            msg[ 4 ] = 0x03;
            break;
        case SEQUENCE_PLAY:
            msg[ 4 ] = 0x04;
            break;
        default:
            break; // should never get here.
        }
        msg[ 5 ] = ( monID % 0x0100 );

        m_data.insert( m_data.end(), msg.begin(), msg.end() );
        m_expectedResponseCmd = 0x23;
    }


    //
    // processResponse
    //
    bool ControlSequencePlaybackCommand::processResponse( const std::vector< unsigned char >& data )
    { 
        if ( processGenericResponsePacketAttributes( data,  TA_COE::CTRL_SEQ_COMMAND, TA_COE::CTRL_SEQ_MIN_RESPONSE_PACKET_LENGTH,
            TA_COE::CTRL_SEQ_MAX_RESPONSE_PACKET_LENGTH ) )
        {
            // Nothing to notify.
            return true; 
        }
        else
        {
            return false;
        }
    }

}
