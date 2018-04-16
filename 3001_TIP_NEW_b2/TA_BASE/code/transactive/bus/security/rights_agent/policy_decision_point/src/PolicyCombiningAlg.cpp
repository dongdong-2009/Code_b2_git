/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/PolicyCombiningAlg.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2012/02/06 16:15:14 $
  *  Last modified by:  $Author: haijun.li $
  *
  *  Implementation of .
  */

#include "PolicyDecisionPoint.h"

namespace TA_Base_Bus
{
	PolicyCombiningAlg::PolicyCombiningAlg() :
		m_permit(false),
		m_atLeastOneError(false),
		m_atLeastOneDeny(false)
	{
	}

	void PolicyCombiningAlg::setResult(  PdpConst::EPolicyResult result )
	{
		switch( result )
		{
			case PdpConst::DENY_PR:
			{
				m_atLeastOneDeny = true;
			}
			break;

			case PdpConst::PERMIT_PR:
			{
				m_permit = true;
			}
			break;

			case PdpConst::NOT_APPLICABLE_PR:
				break;

			case PdpConst::INDETERMINATE_PR:
			{
				m_atLeastOneError = true;
			}
			break;

			default:
			{
				std::ostringstream msg;
				msg << "result == " << result;
				TA_ASSERT(false, msg.str().c_str());
			}
			break;
		}
	}

	bool PolicyCombiningAlg::resultKnown() const
	{
		return( m_permit );
	}

	PdpConst::EPolicyResult PolicyCombiningAlg::combinedResult() const
	{
		if( m_permit )
		{
			return( PdpConst::PERMIT_PR );
		}
		if( m_atLeastOneDeny )
		{
			return( PdpConst::DENY_PR );
		}
		if( m_atLeastOneError )
		{
			return( PdpConst::INDETERMINATE_PR );
		}

		return( PdpConst::NOT_APPLICABLE_PR );
	}
} // TA_Base_Bus
