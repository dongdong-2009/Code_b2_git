/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/PolicySet.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/06/07 14:28:46 $
  *  Last modified by:  $Author: liwei.gong $
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
