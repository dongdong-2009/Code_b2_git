/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/AgentModeSubject.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Implements AgentModeSubject.
  */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	void AgentModeSubjectImpl::update( const std::string& mode )
	{
		m_mode = mode;

		notify(*this);
	}
}
