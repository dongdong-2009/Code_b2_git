#if !defined(XACMLPOLICYFACTORY_H_INCLUDED)
#define XACMLPOLICYFACTORY_H_INCLUDED

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/rights_agent/xacml_factory/src/XacmlPolicyFactory.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #3 $
 *
 * Last modification: $DateTime: 2012/03/22 10:51:12 $
 * Last modified by:  $Author: xiaofeng.qian $
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
			DISPLAY_ONLY_RIGHTS_POLICY,
			DISPLAY_ONLY_DUTY_POLICY,
			DEFAULT_POLICY
		};

		enum ERule
		{

			PERMIT_VALID_RIGHTS_RULE,
			PERMIT_VALID_DUTY_RULE,
			PERMIT_VALID_DISPLAY_ONLY_RULE,
			DENY_RULE
		};	

		const Rule* XacmlPolicyFactory::buildRule( ERule rule ) const;
		const Policy* XacmlPolicyFactory::buildPolicy( EPolicy policy ) const;
	};
}

#endif // !defined(XACMLPOLICYFACTORY_H_INCLUDED)
