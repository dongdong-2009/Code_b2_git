/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/PeerStateChangeSubject.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/06/06 17:21:08 $
  *  Last modified by:  $Author: jinmin.huang $
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
