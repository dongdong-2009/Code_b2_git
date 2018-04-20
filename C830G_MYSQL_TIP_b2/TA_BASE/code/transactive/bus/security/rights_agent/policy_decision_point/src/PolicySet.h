#ifndef POLICYSET_H_INCLUDED
#define POLICYSET_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/PolicySet.h $
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
	class PolicySet
	{
	public:

		PolicySet( const std::list<const Policy*>& policyList );
		~PolicySet();

		PdpConst::EPolicyResult evaluate( const Context& context ) const;
		
	private:

		PolicySet( const PolicySet &x );
		PolicySet& operator=( const PolicySet &x );

		typedef std::list<const Policy*> PolicyList;
		typedef PolicyList::iterator PolicyListIt;
		typedef PolicyList::const_iterator PolicyListConstIt;

		PolicyList m_policyList;

	}; // PolicySet
}; // TA_Base_Bus

#endif // POLICYSET_H_INCLUDED
