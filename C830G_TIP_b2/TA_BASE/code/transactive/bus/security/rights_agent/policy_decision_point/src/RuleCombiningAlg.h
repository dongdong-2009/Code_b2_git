#ifndef RULECOMBININGALG_H_INCLUDED
#define RULECOMBININGALG_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/RuleCombiningAlg.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Implements the "deny-overrides" rule combining algorithm.
  *
  *  See http://www.oasis-open.org/committees/xacml/repository/oasis-xacml-1.0.pdf
  */

namespace TA_Base_Bus
{
	//Class declaration
	class RuleCombiningAlg
	{
	public:
		
		RuleCombiningAlg();

		void setResult( PdpConst::EPolicyResult effect, PdpConst::EPolicyResult result );

		bool resultKnown() const;

		PdpConst::EPolicyResult combinedResult() const;

	private:

		RuleCombiningAlg& operator=( const RuleCombiningAlg& );
		RuleCombiningAlg( const RuleCombiningAlg& );

		bool m_deny;
		bool m_atLeastOneError;
		bool m_potentialDeny;
		bool m_atLeastOnePermit;

	}; // RuleCombiningAlg
}; // TA_Base_Bus

#endif // RULECOMBININGALG_H_INCLUDED
