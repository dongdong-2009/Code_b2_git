/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/StateUpdateSubject.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
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
