/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/DutyPolicyCombAlg.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #3 $
 *
 * Last modification: $DateTime: 2012/06/06 17:21:08 $
 * Last modified by:  $Author: jinmin.huang $
 *
 * Knows how to sum results from multiple policy queries resulting from multiple source
 * sessions being present for a givent duty change/query.
 */

#include "bus/security/duty_agent/duty_agent_state/src/DutyAgentState.h"

namespace TA_Base_Bus
{
	DutyPolicyCombAlg::DutyPolicyCombAlg() :
		m_combinedResult(IDutyPolicy::PR_DENY),
		m_lastResult(IDutyPolicy::PR_DENY),
		m_resultKnown(false)
	{
	}

	void DutyPolicyCombAlg::setResult( IDutyPolicy::POLICY_RESULT result )
	{
		m_lastResult = result;

		if( m_resultKnown )
		{
			return;
		}

		switch( result )
		{
			case IDutyPolicy::PR_DENY:
			{
				// DENY is the top of the pile
				m_combinedResult = result;
				m_resultKnown = true;
			}
			break;

			case IDutyPolicy::PR_REPLACE:
			case IDutyPolicy::PR_REPONLY:
			{
				// REPLACE and REPONLY are ony trumped by DENY
				if( false == m_resultKnown )
				{
					m_combinedResult = result;
				}
			}
			break;

			case IDutyPolicy::PR_ADD:
			case IDutyPolicy::PR_ADDONLY:
			{
				// ADD and ADDONLY beat REMOVE
				if( ((IDutyPolicy::PR_DENY   == m_combinedResult) ||
					 (IDutyPolicy::PR_REMOVE == m_combinedResult)) && (false == m_resultKnown) )
				{
					m_combinedResult = result;
				}
			}
			break;

			case IDutyPolicy::PR_REMOVE:
			{
				// REMOVE is the bottom of the pile
				if( (IDutyPolicy::PR_DENY == m_combinedResult ) && (false == m_resultKnown) )
				{
					m_combinedResult = result;
				}
			}
			break;
			
			default:
				TA_ASSERT(false,"unknown POLICY_RESULT");
		};
	}
	
	bool DutyPolicyCombAlg::isResultKnown() const
	{
		return( m_resultKnown );
	}

	IDutyPolicy::POLICY_RESULT DutyPolicyCombAlg::getCombinedResult() const
	{
		return( m_combinedResult );
	}

	IDutyPolicy::POLICY_RESULT DutyPolicyCombAlg::getLastResult() const
	{
		return( m_lastResult );
	}
}
