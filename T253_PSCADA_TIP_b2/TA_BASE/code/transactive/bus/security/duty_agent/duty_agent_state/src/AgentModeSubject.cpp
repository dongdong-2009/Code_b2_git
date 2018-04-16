/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/AgentModeSubject.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2017/10/31 16:21:54 $
  *  Last modified by:  $Author: CM $
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
