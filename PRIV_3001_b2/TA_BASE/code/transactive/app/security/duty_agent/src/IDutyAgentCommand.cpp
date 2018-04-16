/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/PRIV_3001/TA_BASE/transactive/app/security/duty_agent/src/IDutyAgentCommand.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 *
 * 
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/IDutyAgentCommand.h"

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
