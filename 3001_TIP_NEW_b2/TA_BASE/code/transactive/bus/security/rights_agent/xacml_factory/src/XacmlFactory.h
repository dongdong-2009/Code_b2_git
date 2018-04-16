#if !defined(XACMLFACTORY_H_INCLUDED)
#define XACMLFACTORY_H_INCLUDED

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/rights_agent/xacml_factory/src/XacmlFactory.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #3 $
 *
 * Last modification: $DateTime: 2014/07/11 18:24:37 $
 * Last modified by:  $Author: haijun.li $
 *
 * Acts as a wrapper around the context and policy factories to allow them to
 * use classes as members that that we don't want to expose to the caller through
 * our header includes. The idea is the minimise the interface to make it as
 * re-implementable as possible. For this reason the internal factories are stored
 * as pointers to allow a forward declaration to suffice instead of dragging in the
 * headers.
 */


#include "bus/security/rights_agent/policy_decision_point/src/PolicyDecisionPoint.h"
#include  "bus/security/authentication_library/src/SessionInfo.h"

namespace TA_Base_Bus
{
	class XacmlContextFactory;
	class XacmlPolicyFactory;
}

namespace TA_Base_Bus
{
	class XacmlFactory
	{
	public:
		XacmlFactory();
		~XacmlFactory();
		
		std::auto_ptr<Context>   buildContext( const TA_Base_Bus::SessionInfo &  session,
			unsigned long subject, unsigned long resource, unsigned long action ) const;

		std::auto_ptr<Context>   buildContext( const TA_Base_Bus::SessionInfo &  session,
			unsigned long subject, unsigned long location, unsigned long subsystem, unsigned long action ) const;

		std::auto_ptr<Context>   buildContextWoDuty( const TA_Base_Bus::SessionInfo & session,
			unsigned long subject, unsigned long resource, unsigned long action ) const;

		std::auto_ptr<PolicySet> buildPolicy() const;

		void stationOnLine(unsigned long locaitonKey);
		void stationOffLine(unsigned long locationKey);

	private:
		XacmlFactory( const XacmlFactory& x );
		XacmlFactory& operator=( const XacmlFactory& x );

		XacmlContextFactory* m_contextFactory;
		XacmlPolicyFactory*  m_policyFactory;
	};
}

#endif // !defined(XACMLFACTORY_H_INCLUDED)
