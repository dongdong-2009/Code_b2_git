/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/PolicySet.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
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

} // TA_Base_Bus
