/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/PeerStateChangeSubject.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2017/10/31 16:21:54 $
  *  Last modified by:  $Author: CM $
  *
  *  Implements PeerStateChangeSubject.
  */

#include "DutyAgentState.h"
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
