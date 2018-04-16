/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/DutyResponseSubject.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/06/06 17:21:08 $
  *  Last modified by:  $Author: jinmin.huang $
  *
  *  Implements DutyResponseSubject.
  */

#include "bus/security/duty_agent/duty_agent_state/src/DutyAgentState.h"

namespace TA_Base_Bus
{
	void DutyResponseSubjectImpl::update( const std::string& uniqueId, const IDutyAgentRespCorbaDef::EDutyResponse& response )
	{
		FUNCTION_ENTRY("update");
		m_uniqueId = uniqueId;
		m_response = response;

		notify(*this);
		FUNCTION_EXIT;
	}

	void DutyResponseSubjectImpl::getResponse( std::string& uniqueId, IDutyAgentRespCorbaDef::EDutyResponse& response ) const
	{
		FUNCTION_ENTRY("getResponse");
		uniqueId = m_uniqueId;
		response = m_response;
		FUNCTION_EXIT;
	}
}
