/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/SetSequenceConfigCommand.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Object for requesting new sequence configuration and processing the response.
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/COE/src/SetSequenceConfigCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
//#include "app/cctv/video_switch_agent/protocols/COE/src/COEQuad.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{

    //
    // SetSequenceConfigCommand
    //
    SetSequenceConfigCommand::SetSequenceConfigCommand( unsigned short dwellTime, const std::vector< unsigned long >& videoInputs, COESequence& sequence )
        : AbstractCOERequestCommand(), m_expectedResponseCmd( 0x00 ), m_dwellTime( dwellTime ), m_videoInputs( videoInputs ), m_sequence( sequence )
    {
        m_data.erase( m_data.begin(), m_data.end() );
        unsigned short sequenceID;
        std::istringstream seqAddr( m_sequence.getAddress() );  
		seqAddr >> sequenceID;
        unsigned int packetLength( 0 );
        if ( 0 < m_videoInputs.size() )
        {
          //  packetLength = 4 + ( 2 * m_videoInputs.size() );
			  packetLength = 4 + ( 3 * m_videoInputs.size() );	//20111013
        }
        else
        {
            packetLength = 4;
            m_dwellTime = 0;
        }
        std::vector< char > msg( 5, 0x00 );
        msg[ 0 ] = packetLength % 0x0100; // LSB
        msg[ 1 ] = packetLength / 0x0100; // MSB
        msg[ 3 ] = 0x17;
        msg[ 4 ] = ( sequenceID % 0x0100 );

    //    if ( m_dwellTime != 0 )		//20111013
        {
            msg.push_back( dwellTime % 0x0100 );
        }

        COECamera* pCOECamera = 0; // Only Cameras can be configured for sequences in this protocol.
        std::pair< unsigned short, unsigned short > camAddr( 0,0 );
        std::vector< unsigned long >::iterator videoInputsIt = m_videoInputs.begin();
        for ( ; videoInputsIt != m_videoInputs.end(); videoInputsIt++ )
        {
            pCOECamera = COESwitchManager::getInstance().getCamera( *videoInputsIt );
            if ( 0 == pCOECamera )
            {
                TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST, "The InputType does not exist." ) );
            }
            camAddr = pCOECamera->getParsedAddress();
            msg.push_back( camAddr.first % 0x0100 );
            msg.push_back( camAddr.second % 0x0100 );
			msg.push_back( camAddr.second / 0x100 );	//20111013
        }

        TA_ASSERT( ( msg.size() == ( packetLength + 2 ) ), "Incorrect length bytes have been set!" );

        m_data.insert( m_data.end(), msg.begin(), msg.end() );

        m_expectedResponseCmd = 0x17;
    }


    //
    // processResponse
    //
    bool SetSequenceConfigCommand::processResponse( const std::vector< unsigned char >& data )
    { 
        if ( processGenericResponsePacketAttributes( data,  TA_COE::PROG_SEQ_COMMAND, TA_COE::PROG_SEQ_MIN_RESPONSE_PACKET_LENGTH,
            TA_COE::PROG_SEQ_MAX_RESPONSE_PACKET_LENGTH ) )
        {
            if(SUCCEEDED == m_completionState)
			{
				m_sequence.notifySequenceConfigUpdated( m_dwellTime, m_videoInputs );
			}
            return true; 
        }
        else
        {
            return false;
        }
    }

}
