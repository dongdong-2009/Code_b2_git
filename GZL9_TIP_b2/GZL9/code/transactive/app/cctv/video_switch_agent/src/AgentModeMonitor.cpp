/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/src/AgentModeMonitor.cpp $
  * @author:   Katherine Thomson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class is a singleton which maintains the mode of the agent.
  * All methods that are mode dependent can use the instance of this
  * class to verify that the agent is in Control mode.  
  */

#include <sstream>

#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "bus/cctv_gzl9/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;


namespace TA_IRS_App
{
    //
    // Initialise statics.
    //

	TA_Base_Core::ReEntrantThreadLockable AgentModeMonitor::m_singletonLock;
	AgentModeMonitor* AgentModeMonitor::m_theClassInstance = NULL;


    //
    // getInstance
    //
    AgentModeMonitor& AgentModeMonitor::getInstance()
    {
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

		// If guard acquired then make sure the singleton is still NULL.

		if ( NULL == m_theClassInstance )
		{
			// Create the one & only object.

			m_theClassInstance = new AgentModeMonitor();
		}

		return *m_theClassInstance;
    }


    //
    // isInControlMode
    //
    bool AgentModeMonitor::isInControlMode()
    {
        TA_Base_Core::ThreadGuard guard( m_modeLock );
        return m_isInControlMode;
    }

    
    //
    // setToMonitorMode
    //
    void AgentModeMonitor::setToMonitorMode()
    {
        TA_Base_Core::ThreadGuard guard( m_modeLock );
        m_isInControlMode = false;
    }

    
    //
    // setToControlMode
    //
    void AgentModeMonitor::setToControlMode()
    {
        TA_Base_Core::ThreadGuard guard( m_modeLock );
        m_isInControlMode = true;
    }

    
    //
    // verifyInControlMode
    //
    void AgentModeMonitor::verifyInControlMode( const std::string& action )
    {
        TA_Base_Core::ThreadGuard guard( m_modeLock );
        if ( !m_isInControlMode )
        {
            std::stringstream error;
            error << "The agent (" << TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME );
            error << ") is in Monitor mode so it is unable to process the request to " << action;
            error << ". Please try again.";
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( error.str().c_str() );
        }
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Private methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // AgentModeMonitor
    //
    AgentModeMonitor::AgentModeMonitor()
    {
        if ( RPARAM_CONTROL == TA_Base_Core::RunParams::getInstance().get( RPARAM_OPERATIONMODE ) )
        {
            setToControlMode();
        }
        else
        {
            setToMonitorMode();
        }
    }

} // TA_IRS_App
