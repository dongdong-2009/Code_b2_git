/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/PolicySet.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2017/10/31 16:21:54 $
  *  Last modified by:  $Author: CM $
  *
  *  Implementation of .
  */

#include "PolicyDecisionPoint.h"

namespace TA_Base_Bus
{

	PolicySet::PolicySet( const std::list<const Policy*>& policyList ) :
		m_policyList(policyList)
	{
	}

	PolicySet::~PolicySet()
	{
		PolicyListIt it;

		for( it = m_policyList.begin(); it != m_policyList.end(); it++ )
		{
			delete *it;
			*it = NULL;
		}
	}

	PdpConst::EPolicyResult PolicySet::evaluate( const Context& context ) const
	{
		PolicyListConstIt it;

		PolicyCombiningAlg result;

		it = m_policyList.begin();

		// evaluate policies using the "allow overrides" policy-combining-
		// algorithm
		while( (!result.resultKnown()) && (m_policyList.end() != it) )
		{
			(*it)->evaluate( context, result );
			it++;
		}

		return( result.combinedResult() );
	}
	
	PdpConst::EPolicyResult PolicySet::evaluateWithoutDuty( const Context& context ) const
	{
		PolicyListConstIt it;

		PolicyCombiningAlg result;

		it = m_policyList.begin();

		// evaluate policies using the "allow overrides" policy-combining-
		// algorithm, but skip the duty policy
		while( (!result.resultKnown()) && (m_policyList.end() != it) )
		{
			if(false == (*it)->isDutyPolicy())
			{
				(*it)->evaluate( context, result );
			}
			it++;
		}

		return( result.combinedResult() );
	}

} // TA_Base_Bus
