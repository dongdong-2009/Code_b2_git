/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/SwitchInputToOutputCommand.cpp $
 * @author:  Rob Young
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2013/04/18 14:09:13 $
 * Last modified by:  $Author: huang.wenbo $
 * 
 * Object for requesting an input be switched to an output and for
 * processing the response.
 */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/COE/src/SwitchInputToOutputCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEVideoInput.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEVideoOutput.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/DebugUtil.h"

#include <sstream>

namespace TA_IRS_App
{
    //
    // SwitchInputToOutputCommand
    //
    SwitchInputToOutputCommand::SwitchInputToOutputCommand( unsigned long videoInputEntityKey, COEVideoOutput& output )
        : AbstractCOERequestCommand(), m_videoInputEntityKey( videoInputEntityKey ),
          m_videoOutput( output ), m_expectedResponseCmd( 0x00 )
    {
    }


	void SwitchInputToOutputCommand::buildCommand()
	{
        if ( m_videoInputEntityKey != 0 )
        {
			COESwitchManager::ECOEInputType inputType = COESwitchManager::getInstance().getInputType( m_videoInputEntityKey );
			if(COESwitchManager::CAMERA == inputType)
			{
                buildCamAndMonCmd();
			}
			else if(COESwitchManager::SEQUENCE == inputType)
			{
                buildSeqAndMonCmd();
			}
        }
        else
        {
            buildClearMonCmd();
        }
	}

    void SwitchInputToOutputCommand::buildCamAndMonCmd()
    {
        COECamera* cam = COESwitchManager::getInstance().getCamera( m_videoInputEntityKey );
        if ( 0 == cam )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST, "The InputType does not exist." ) );
        }

        m_data.erase( m_data.begin(), m_data.end() );
        std::pair< unsigned short, unsigned short > addrPr = cam->getParsedAddress();
        unsigned short monID;
        std::istringstream monAddr( m_videoOutput.getAddress() );  
		monAddr >> monID;
      //  std::vector< char > msg( 7, 0x00 );
		std::vector< char > msg( 8, 0x00 );		//20111013
        msg[ 0 ] = 0x06;
        msg[ 3 ] = 0x21;
        msg[ 4 ] = ( addrPr.first % 0x0100 );
        msg[ 5 ] = ( addrPr.second % 0x0100 );
		msg[ 6 ] = ( addrPr.second / 0x0100 );	//20111013
		msg[ 7 ] = ( monID % 0x0100 );
     //   msg[ 6 ] = ( monID % 0x0100 );

        m_data.insert( m_data.end(), msg.begin(), msg.end() );

        m_expectedResponseCmd = 0x21;
    }


    void SwitchInputToOutputCommand::buildSeqAndMonCmd()
    {
        COESequence* seq = COESwitchManager::getInstance().getSequence( m_videoInputEntityKey );
        if ( 0 == seq )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST, "The InputType does not exist." ) );
        }

        m_data.erase( m_data.begin(), m_data.end() );
        unsigned short seqID;
        std::istringstream seqAddr( ( *seq ).getAddress() );
        unsigned short monID;
        std::istringstream monAddr( m_videoOutput.getAddress() );  
		monAddr >> monID;
		seqAddr >> seqID;

        std::vector< char > msg( 6, 0x00 );
        msg[ 0 ] = 0x04;
        msg[ 3 ] = 0x22;
        msg[ 4 ] = ( seqID % 0x0100 );
        msg[ 5 ] = ( monID % 0x0100 );

        m_data.insert( m_data.end(), msg.begin(), msg.end() );

        m_expectedResponseCmd = 0x22;
    }


    void SwitchInputToOutputCommand::buildClearMonCmd()
    {
        m_data.erase( m_data.begin(), m_data.end() );
        unsigned short monID;
        std::istringstream monAddr( m_videoOutput.getAddress() );  
		monAddr >> monID;

        std::vector< char > msg( 5, 0x00 );
        msg[ 0 ] = 0x03;
        msg[ 3 ] = 0x06;
        msg[ 4 ] = ( monID % 0x0100 );

        m_data.insert( m_data.end(), msg.begin(), msg.end() );

        m_expectedResponseCmd = 0x06;
    }


    //
    // processResponse
    //
    bool SwitchInputToOutputCommand::processResponse( const std::vector< unsigned char >& data )
    { 
        if ( processGenericResponsePacketAttributes( data,  m_expectedResponseCmd, TA_COE::PACKET_HEADER_LENGTH,
            TA_COE::PACKET_HEADER_LENGTH ) )
        {
            if(SUCCEEDED == m_completionState)
			{
	            m_videoOutput.notifyInputSwitchedToOutput( m_videoInputEntityKey );
			}
			return true;   
        }
        else
        {
            return false;
        }
    }

}
