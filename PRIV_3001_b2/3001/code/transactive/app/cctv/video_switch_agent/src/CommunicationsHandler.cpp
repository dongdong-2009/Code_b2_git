/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/src/CommunicationsHandler.cpp $
  * @author:   Katherine Thomson 
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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

        m_pdCommunicationsHandler = std::auto_ptr< IPDCommunicationsHandler >( PDInterfaceFactory::createCommunicationsHandler( videoSwitchAgentData ) );
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
		FUNCTION_ENTRY("connect");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "For unit testing: connect");
        // If the agent is in Control mode, connect to the hardware.

        if ( AgentModeMonitor::getInstance().isInControlMode() )
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "For unit testing: if_branch");
            TA_ASSERT( 0 != m_pdCommunicationsHandler.get(), "The protocol dependent CommunicationsHandler has not been initialised." ); 
            m_pdCommunicationsHandler->connect();
        }
		FUNCTION_EXIT;
    }

    
    //
    // disconnect
    //
    void CommunicationsHandler::disconnect() 
    {
		FUNCTION_ENTRY("disconnect");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "For unit testing: disconnect");

        TA_ASSERT( 0 != m_pdCommunicationsHandler.get(), "The protocol dependent CommunicationsHandler has not been initialised." ); 
        
        // No matter what mode the agent is in, disconnect from the hardware.

        m_pdCommunicationsHandler->disconnect();
		FUNCTION_EXIT;
    }

	void CommunicationsHandler::setToControl()
	{
		m_pdCommunicationsHandler->setToControl();
	}

} // TA_IRS_App