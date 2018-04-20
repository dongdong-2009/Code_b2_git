#if !defined(XACMLPOLICYFACTORY_H_INCLUDED)
#define XACMLPOLICYFACTORY_H_INCLUDED

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/rights_agent/xacml_factory/src/XacmlPolicyFactory.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * Defines a set of rules and policies and builds an XACML "Policy Set".
 */

namespace TA_Base_Bus
{
	class XacmlPolicyFactory
	{
	public:

		XacmlPolicyFactory(){}

		std::auto_ptr<PolicySet> build() const;

	private:
		
		XacmlPolicyFactory( const XacmlPolicyFactory& x );
		XacmlPolicyFactory& operator=( const XacmlPolicyFactory& x );

		enum EPolicy
		{
			RIGHTS_POLICY,
			DUTY_POLICY,
			DEFAULT_POLICY
		};

		enum ERule
		{

			PERMIT_VALID_RIGHTS_RULE,
			PERMIT_VALID_DUTY_RULE,
			DENY_RULE
		};	

		const Rule* XacmlPolicyFactory::buildRule( ERule rule ) const;
		const Policy* XacmlPolicyFactory::buildPolicy( EPolicy policy ) const;
	};
}

#endif // !defined(XACMLPOLICYFACTORY_H_INCLUDED)
