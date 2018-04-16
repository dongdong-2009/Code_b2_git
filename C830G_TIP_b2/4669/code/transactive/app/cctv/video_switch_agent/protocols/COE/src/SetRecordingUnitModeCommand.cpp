/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/SetRecordingUnitModeCommand.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Object for requesting a RecordingUnit be set to a particular recording speed, and for
  * processing the response.
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/COE/src/SetRecordingUnitModeCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COERecordingUnit.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/DebugUtil.h"

#include <sstream>

namespace TA_IRS_App
{

    //
    // SetRecordingUnitModeCommand
    //
    SetRecordingUnitModeCommand::SetRecordingUnitModeCommand( unsigned short recordingSpeedIndex, COERecordingUnit& recordingUnit )
        : AbstractCOERequestCommand(), m_recordingSpeedIndex( recordingSpeedIndex ),
          m_recordingUnit( recordingUnit), m_expectedResponseCmd( 0x00 )
    {
        m_data.erase( m_data.begin(), m_data.end() );
        
        unsigned short recID;
        std::istringstream recAddr( m_recordingUnit.getAddress() );  
        if ( false == (recAddr >> recID && recAddr.eof()) )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR, 
                "Invalid value set to Address" ) );            
        }
        std::vector< char > msg( 6, 0x00 );
        
        msg[ 0 ] = 0x04;
        msg[ 3 ] = 0x12;
        msg[ 4 ] = ( recID % 0x0100 );
        if ( m_recordingSpeedIndex == 2 )
        {
            msg[ 5 ] = 0x02;
        }
        else
        {
            msg[ 5 ] = 0x01;
        }

        m_data.insert( m_data.end(), msg.begin(), msg.end() );
        m_expectedResponseCmd = 0x12;
    }


    //
    // processResponse
    //
    bool SetRecordingUnitModeCommand::processResponse( const std::vector< unsigned char >& data )
    { 
        if ( processGenericResponsePacketAttributes( data,  TA_COE::REC_UNIT_MODE_COMMAND, TA_COE::REC_UNIT_MODE_MIN_RESPONSE_PACKET_LENGTH,
            TA_COE::REC_UNIT_MODE_MAX_RESPONSE_PACKET_LENGTH ) )
        {
            try
            {
				if(SUCCEEDED == m_completionState)
				{
					m_recordingUnit.notifyRecordingSpeedSet( m_recordingSpeedIndex );
				}
            }
            catch( const TA_Base_Core::DatabaseException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
                LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to notify clients of switch of input to output" );
                // Don't return false, because the problem was with the database, not with the received data.
            }
            catch( const TA_Base_Core::DataException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
                LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to notify clients of switch of input to output" );
                // Don't return false, because the problem was with the database, not with the received data.
            }
            return true;    
        }
        else
        {
            return false;
        }
    }

}
