/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/Policy.cpp $
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

	Policy::Policy( const Target* target, const std::list<const Rule*>& ruleList ) :
		m_target(target),
		m_ruleList(ruleList)
	{
	}

	Policy::~Policy()
	{
		RuleListIt it;

		for( it = m_ruleList.begin(); it != m_ruleList.end(); it++ )
		{
			delete *it;
		}
	}

	void Policy::evaluate( const Context& context, PolicyCombiningAlg& result ) const
	{
		// check the target
		if( NULL != m_target.get() )
		{
			if( 0 < m_target->size() )
			{
				PdpConst::EMatchResult targetResult = m_target->evaluate( context );

				if( PdpConst::MATCH_MR != targetResult )
				{
					result.setResult( UnmatchedTarget::evaluate(targetResult) );
					return;
				}
			}
		}

		RuleListConstIt it;
		RuleCombiningAlg ruleResult;

		it = m_ruleList.begin();

		// evaluate rules using the "deny overrides" policy-combining-
		// algorithm
		while( (!ruleResult.resultKnown()) && (m_ruleList.end() != it) )
		{
			(*it)->evaluate( context, ruleResult );
			it++;
		}

		result.setResult( ruleResult.combinedResult() );
	}



} // TA_Base_Bus
