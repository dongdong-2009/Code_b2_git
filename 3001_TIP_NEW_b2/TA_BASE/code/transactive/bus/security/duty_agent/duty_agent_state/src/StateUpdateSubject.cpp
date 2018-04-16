/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/StateUpdateSubject.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/06/06 17:21:08 $
  *  Last modified by:  $Author: jinmin.huang $
  *
  *  Implements StateUpdateSubject.
  */

#include "bus/security/duty_agent/duty_agent_state/src/DutyAgentState.h"

namespace TA_Base_Bus
{
	void StateUpdateSubjectImpl::update( const DutyState& state )
	{
		FUNCTION_ENTRY("update");
		if( state.isEmpty() )
		{
			FUNCTION_EXIT;
			return;
		}
		
		notify(*this,state);
		FUNCTION_EXIT;
	}
}
