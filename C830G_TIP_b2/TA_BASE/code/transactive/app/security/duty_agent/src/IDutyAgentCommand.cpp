/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/app/security/duty_agent/src/IDutyAgentCommand.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * 
 */

#include "DutyAgent.h"
#include "IDutyAgentCommand.h"

namespace TA_Base_App
{
	IDutyAgentCommand::IDutyAgentCommand( const TA_Base_Bus::DutyState& state ) :
		m_state(state)
	{
	}

	bool IDutyAgentCommand::rightsOk( TA_Base_Bus::RightsLibrary& rightsLib, unsigned long resource,
			std::string& reason ) const
	{
		if( ULONG_MAX == getActionKey() )
		{
			return( true );
		}

		TA_Base_Bus::EDecisionModule decisionModule;

		return( rightsLib.isActionPermittedOnResource( m_state.getAccessControlSession(), resource,
			getActionKey(), reason, decisionModule ) );
	}

}
