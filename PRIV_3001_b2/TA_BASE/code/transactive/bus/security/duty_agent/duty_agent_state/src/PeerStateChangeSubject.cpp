/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/PeerStateChangeSubject.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
  *
  *  Implements PeerStateChangeSubject.
  */

#include "bus/security/duty_agent/duty_agent_state/src/DutyAgentState.h"
using namespace TA_Base_Core;
namespace TA_Base_Bus
{
	void PeerStateChangeSubjectImpl::update( const DutyState& state )
	{
		FUNCTION_ENTRY("update");

        // TD12082: Don't limit the update here, instead let the observers decide if they will ignore
        //          the state data or not.	
		notify(*this, state);	
		FUNCTION_EXIT;
	}
}
