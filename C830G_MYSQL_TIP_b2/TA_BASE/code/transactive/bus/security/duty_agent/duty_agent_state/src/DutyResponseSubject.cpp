/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/DutyResponseSubject.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Implements DutyResponseSubject.
  */

#include "DutyAgentState.h"

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
