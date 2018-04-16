#ifndef RULE_H_INCLUDED
#define RULE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/Rule.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/06/07 14:28:46 $
  *  Last modified by:  $Author: liwei.gong $
  *
  *  See http://www.oasis-open.org/committees/xacml/repository/oasis-xacml-1.0.pdf
  */

namespace TA_Base_Bus
{
	//Class declaration
	class Rule
	{
	public:

		Rule( const std::string& id, const Target* target, PdpConst::EPolicyResult effect );
		Rule( const std::string& id, const Target* target, const Condition* condition, PdpConst::EPolicyResult effect );
		~Rule()
		{
		}

		void evaluate( const Context& context, RuleCombiningAlg& result ) const;
		
	private:

		Rule( const Rule &x );
		Rule& operator=( const Rule &x );

		const std::string m_id;
		std::auto_ptr<const Target>    m_target;
		std::auto_ptr<const Condition> m_condition;

		PdpConst::EPolicyResult m_effect;
		
	}; // Rule
}; // TA_Base_Bus

#endif // RULE_H_INCLUDED
