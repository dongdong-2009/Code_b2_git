/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/Policy.cpp $
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

	Policy::Policy( const Target* target, const std::list<const Rule*>& ruleList ) :
		m_target(target),
		m_ruleList(ruleList),
		isDuty_(false)
	{
	}

	Policy::Policy( const Target* target, const std::list<const Rule*>& ruleList, bool isDuty ):
	m_target(target),
	m_ruleList(ruleList),
	isDuty_(isDuty)
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
	bool Policy::isDutyPolicy() const
	{
		return this->isDuty_;
	}

} // TA_Base_Bus
