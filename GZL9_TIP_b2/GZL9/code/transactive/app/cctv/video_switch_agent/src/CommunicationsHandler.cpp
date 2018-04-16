/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/src/CommunicationsHandler.cpp $
  * @author:   Katherine Thomson 
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Implements the ICommunicationsHandler interface.  Handles Agent specific
  * tasks required on start up and shutdown, as well as process 
  * management messages and instructions originating from the System Controller.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include <algorithm>
#include <sstream>

#include "app/cctv/video_switch_agent/src/CommunicationsHandler.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/protocols/src/PDInterfaceFactory.h"

#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{
    //
    // CommunicationsHandler
    //
    CommunicationsHandler::CommunicationsHandler( TA_Base_Core::VideoSwitchAgentPtr videoSwitchAgentData )
        : m_pdCommunicationsHandler( 0 )
    {
        // Create an object to handle the protocol dependent work for the CommunicationsHandler.

        m_pdCommunicationsHandler = std::auto_ptr< IPDCommunicationsHandler >( getPDInterfaceFactory().createCommunicationsHandler( videoSwitchAgentData ) );
        if ( 0 == m_pdCommunicationsHandler.get() )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CREATION_FAILED, "Failed to create IPDCommunicationsHandler." ) );
        }
    }


    //
    // connect
    //
    void CommunicationsHandler::connect() 
    {
        // If the agent is in Control mode, connect to the hardware.

        if ( AgentModeMonitor::getInstance().isInControlMode() )
        {
            TA_ASSERT( 0 != m_pdCommunicationsHandler.get(), "The protocol dependent CommunicationsHandler has not been initialised." ); 
            m_pdCommunicationsHandler->connect();
        }
    }

    
    //
    // disconnect
    //
    void CommunicationsHandler::disconnect() 
    {
        TA_ASSERT( 0 != m_pdCommunicationsHandler.get(), "The protocol dependent CommunicationsHandler has not been initialised." ); 
        
        // No matter what mode the agent is in, disconnect from the hardware.

        m_pdCommunicationsHandler->disconnect();
    }

} // TA_IRS_App