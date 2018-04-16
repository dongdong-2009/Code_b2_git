/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/SwitchQuadInputsCommand.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by:  $Author: grace.koh $
  *
  * Object for requesting inputs be switched to a quad and for
  * processing the response.
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/COE/src/MappingTrainBorneMonitorCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
//#include "app/cctv/video_switch_agent/protocols/COE/src/COEQuad.h"

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
    MappingTrainBorneMonitorCommand::MappingTrainBorneMonitorCommand(  COEVideoOutput& monitor)
        : AbstractCOERequestCommand(), m_expectedResponseCmd( 0x00 ), m_monitor( monitor )
    {
     
        m_data.erase( m_data.begin(), m_data.end() );
        unsigned short monitorID;
        std::istringstream monitorAddr( m_monitor.getAddress() );  
		monitorAddr >> monitorID;
        std::vector< char > msg( 5, 0x00 );
        msg[ 0 ] = 0x3;
        msg[ 3 ] = 0x25;
        msg[ 4 ] = ( monitorID % 0x0100 );


        m_data.insert( m_data.end(), msg.begin(), msg.end() );

        m_expectedResponseCmd = 0x25;
    }


    //
    // processResponse
    //
    bool MappingTrainBorneMonitorCommand::processResponse( const std::vector< unsigned char >& data )
    { 
        if ( processGenericResponsePacketAttributes( data,  TA_COE::TRAIN_AND_MON_COMMAND, TA_COE::TRAIN_AND_MON_MIN_RESPONSE_PACKET_LENGTH,
            TA_COE::TRAIN_AND_MON_MAX_RESPONSE_PACKET_LENGTH ) )
        {
            if(SUCCEEDED == m_completionState)
			{
				m_monitor.notifyMappingTrainBorneMonitor( );
			}
			return true;
        }
        else
        {
            return false;
        }
    }

}
