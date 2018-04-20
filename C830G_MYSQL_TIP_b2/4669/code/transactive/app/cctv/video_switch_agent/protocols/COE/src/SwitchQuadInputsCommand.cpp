/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Robert Young
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Object for requesting inputs be switched to a quad and for
  * processing the response.
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/COE/src/SwitchQuadInputsCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEQuad.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/DebugUtil.h"

#include <sstream>

namespace TA_IRS_App
{

    //
    // SwitchQuadInputsCommand
    //
    SwitchQuadInputsCommand::SwitchQuadInputsCommand( unsigned long topLeftVideoInput, unsigned long topRightVideoInput, unsigned long bottomLeftVideoInput, unsigned long bottomRightVideoInput,  COEQuad& quad )
        : AbstractCOERequestCommand(), m_expectedResponseCmd( 0x00 ), m_topLeftVideoInput( topLeftVideoInput ), m_topRightVideoInput( topRightVideoInput ),
          m_bottomLeftVideoInput( bottomLeftVideoInput ), m_bottomRightVideoInput( bottomRightVideoInput ), m_quad( quad )
    {
        COECamera* pCOECamera = 0;
        std::pair< unsigned short, unsigned short > topLeftAddr( 0,0 ), topRightAddr( 0,0 ), bottomLeftAddr( 0,0 ), bottomRightAddr( 0,0 );
        if ( 0 != topLeftVideoInput )
        {
            pCOECamera = COESwitchManager::getInstance().getCamera( m_topLeftVideoInput );
            if ( 0 == pCOECamera )
            {
                TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                    TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST,
                    "The InputType does not exist." ) );
            }
            topLeftAddr = pCOECamera->getParsedAddress();
        }
        if ( 0 != topRightVideoInput )
        {
            pCOECamera = COESwitchManager::getInstance().getCamera( m_topRightVideoInput );
            if ( 0 == pCOECamera )
            {
                TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                    TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST,
                    "The InputType does not exist." ) );
            }
            topRightAddr = pCOECamera->getParsedAddress();
        }
        if ( 0 != bottomLeftVideoInput )
        {
            pCOECamera = COESwitchManager::getInstance().getCamera( m_bottomLeftVideoInput );
            if ( 0 == pCOECamera )
            {
                TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                    TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST,
                    "The InputType does not exist." ) );
            }
            bottomLeftAddr = pCOECamera->getParsedAddress();
        }
        if ( 0 != bottomRightVideoInput )
        {
            pCOECamera = COESwitchManager::getInstance().getCamera( m_bottomRightVideoInput );
            if ( 0 == pCOECamera )
            {
                TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                    TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST,
                    "The InputType does not exist." ) );
            }
            bottomRightAddr = pCOECamera->getParsedAddress();
        }

        m_data.erase( m_data.begin(), m_data.end() );
        unsigned short quadID;
        std::istringstream quadAddr( m_quad.getAddress() );  
        if ( false == (quadAddr >> quadID && quadAddr.eof()) )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR, 
                "Invalid value set to Monitor Address" ) );            
        }
        std::vector< char > msg( 17, 0x00 );
		unsigned short camId;
        msg[ 0 ] = 0x0f;
        msg[ 3 ] = 0x18;
        msg[ 4 ] = ( quadID % 0x0100 );

        msg[ 5 ] = ( topLeftAddr.first % 0x0100 );
		camId = topLeftAddr.second % 0x0201;
		msg[ 6 ] = camId % 0x0100;
		msg[ 7 ] = camId / 0x0100;
     
        msg[ 8 ] = ( topRightAddr.first % 0x0100 );
		camId = topRightAddr.second % 0x0201;
		msg[ 9 ] = camId % 0x0100;
		msg[ 10 ] = camId / 0x0100;
        
        msg[ 11 ] = ( bottomLeftAddr.first % 0x0100 );
		camId = bottomLeftAddr.second % 0x0201;
		msg[ 12 ] = camId % 0x0100;
		msg[ 13 ] = camId / 0x0100;
      
        msg[ 14 ] = ( bottomRightAddr.first % 0x0100 );
		camId = bottomRightAddr.second % 0x0201;
		msg[ 15 ] = camId % 0x0100;
		msg[ 16 ] = camId / 0x0100;

        m_data.insert( m_data.end(), msg.begin(), msg.end() );

        m_expectedResponseCmd = 0x18;
    }


    //
    // processResponse
    //
    bool SwitchQuadInputsCommand::processResponse( const std::vector< unsigned char >& data )
    { 
        if ( processGenericResponsePacketAttributes( data,  TA_COE::PROG_QUAD_COMMAND, TA_COE::PROG_QUAD_MIN_RESPONSE_PACKET_LENGTH,
            TA_COE::PROG_QUAD_MAX_RESPONSE_PACKET_LENGTH ) )
        {
            if(SUCCEEDED == m_completionState)
			{
				m_quad.notifyInputsSwitchedToQuad( m_topLeftVideoInput, m_topRightVideoInput, m_bottomLeftVideoInput, m_bottomRightVideoInput );
			}
			return true;
        }
        else
        {
            return false;
        }
    }

}
