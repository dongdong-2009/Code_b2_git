#ifndef POLICY_H_INCLUDED
#define POLICY_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/Policy.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  See http://www.oasis-open.org/committees/xacml/repository/oasis-xacml-1.0.pdf
  */

namespace TA_Base_Bus
{
	//Class declaration
	class Policy
	{
	public:
		
		Policy( const Target* target, const std::list<const Rule*>& ruleList );
		~Policy();

		void evaluate( const Context& context, PolicyCombiningAlg& result ) const;

	private:

		Policy( const Policy &x );
		Policy& operator=( const Policy &x );

		typedef std::list<const Rule* > RuleList;
		typedef RuleList::iterator RuleListIt;
		typedef RuleList::const_iterator RuleListConstIt;

		std::auto_ptr<const Target> m_target;
		RuleList m_ruleList;

	}; // Policy
}; // TA_Base_Bus

#endif // POLICY_H_INCLUDED
