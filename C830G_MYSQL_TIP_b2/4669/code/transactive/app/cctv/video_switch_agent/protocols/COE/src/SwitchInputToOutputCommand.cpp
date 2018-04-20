/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File$
 * @author:  Rob Young
 * @version: $Revision$
 *
 * Last modification: $DateTime$
 * Last modified by:  $Author$
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
        if ( videoInputEntityKey != 0 )
        {
            switch ( COESwitchManager::getInstance().getInputType( videoInputEntityKey ) )
            {
            case COESwitchManager::CAMERA:
                buildCamAndMonCmd();
                break;
            case COESwitchManager::QUAD:
                buildQuadAndMonCmd();
                break;
            case COESwitchManager::SEQUENCE:
                buildSeqAndMonCmd();
                break;
            case COESwitchManager::BVS_STAGE:
                buildBVSStageAndMonCmd();
                break;
            default:
                break;
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
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST,
                "The InputType does not exist." ) );
        }

        m_data.erase( m_data.begin(), m_data.end() );
        std::pair< unsigned short, unsigned short > addrPr = cam->getParsedAddress();
        unsigned short monID;
        std::istringstream monAddr( m_videoOutput.getAddress() );  
        if ( false == (monAddr >> monID && monAddr.eof()) )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR, 
                "Invalid value set to Monitor Address" ) );            
        }
        std::vector< char > msg( 8, 0x00 );
        msg[ 0 ] = 0x06;
        msg[ 3 ] = 0x21;
        msg[ 4 ] = ( addrPr.first % 0x0100 );
        unsigned short camId = ( addrPr.second % 0x0201 );
		msg[ 5 ] = camId % 0x0100; //LSB
		msg[ 6 ] = camId / 0x0100; //MSB

        msg[ 7 ] = ( monID % 0x0100 );

        m_data.insert( m_data.end(), msg.begin(), msg.end() );

        m_expectedResponseCmd = 0x21;
    }


    void SwitchInputToOutputCommand::buildQuadAndMonCmd()
    {
        COEQuad* quad = COESwitchManager::getInstance().getQuad( m_videoInputEntityKey );
        if ( 0 == quad )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST,
                "The InputType does not exist." ) );
        }

        m_data.erase( m_data.begin(), m_data.end() );
        unsigned short quadID;
        std::istringstream quadAddr( ( *quad ).getAddress() );
        unsigned short monID;
        std::istringstream monAddr( m_videoOutput.getAddress() );  
        if ( ( false == (monAddr >> monID && monAddr.eof()) ) || ( false == (quadAddr >> quadID && quadAddr.eof()) ) )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR, 
                "Invalid value set to Monitor or Quad Address" ) );            
        }

        std::vector< char > msg( 6, 0x00 );
        msg[ 0 ] = 0x04;
        msg[ 3 ] = 0x24;
        msg[ 4 ] = ( quadID % 0x0100 );
        msg[ 5 ] = ( monID % 0x0100 );

        m_data.insert( m_data.end(), msg.begin(), msg.end() );

        m_expectedResponseCmd = 0x24;
    }


    void SwitchInputToOutputCommand::buildBVSStageAndMonCmd()
    {
        COEBVSStage* bvsStage = COESwitchManager::getInstance().getBVSStage( m_videoInputEntityKey );
        if ( 0 == bvsStage )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST,
                "The InputType does not exist." ) );
        }

        m_data.erase( m_data.begin(), m_data.end() );
        unsigned short bvsStageID;
        std::istringstream bvsStageAddr( ( *bvsStage ).getAddress() );
        unsigned short monID;
        std::istringstream monAddr( m_videoOutput.getAddress() );  
        if ( ( false == (monAddr >> monID && monAddr.eof()) ) || ( false == (bvsStageAddr >> bvsStageID && bvsStageAddr.eof()) ) )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR, 
                "Invalid value set to Monitor or BVSStage Address" ) );            
        }

        std::vector< char > msg( 6, 0x00 );
        msg[ 0 ] = 0x04;
        msg[ 3 ] = 0x25;
        msg[ 4 ] = ( bvsStageID % 0x0100 );
        msg[ 5 ] = ( monID % 0x0100 );

        m_data.insert( m_data.end(), msg.begin(), msg.end() );

        m_expectedResponseCmd = 0x25;
    }


    void SwitchInputToOutputCommand::buildSeqAndMonCmd()
    {
        COESequence* seq = COESwitchManager::getInstance().getSequence( m_videoInputEntityKey );
        if ( 0 == seq )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST,
                "The InputType does not exist." ) );
        }

        m_data.erase( m_data.begin(), m_data.end() );
        unsigned short seqID;
        std::istringstream seqAddr( ( *seq ).getAddress() );
        unsigned short monID;
        std::istringstream monAddr( m_videoOutput.getAddress() );  
        if ( ( false == (monAddr >> monID && monAddr.eof()) ) || ( false == (seqAddr >> seqID && seqAddr.eof()) ) )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR, 
                "Invalid value set to Monitor or Sequence Address" ) );            
        }

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
        if ( false == (monAddr >> monID && monAddr.eof()) )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR, 
                "Invalid value set to Address" ) );            
        }

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
