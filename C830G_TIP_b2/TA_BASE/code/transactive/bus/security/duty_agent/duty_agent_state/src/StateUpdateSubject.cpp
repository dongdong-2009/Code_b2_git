/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/StateUpdateSubject.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #5 $
  *
  *  Last modification: $DateTime: 2011/06/20 18:29:56 $
  *  Last modified by:  $Author: shurui.liu $
  *
  *  Implements StateUpdateSubject.
  */

#include "DutyAgentState.h"

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
