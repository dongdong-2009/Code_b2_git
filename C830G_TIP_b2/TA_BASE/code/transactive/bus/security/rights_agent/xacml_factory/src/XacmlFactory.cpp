/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File$
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision$
 *
 * Last modification: $DateTime$
 * Last modified by:  $Author$
 *
 * Builds XACML contexts and policies.
 */

#include "XacmlFactoryInternal.h"

namespace TA_Base_Bus
{
	XacmlFactory::XacmlFactory() :
		m_contextFactory(NULL),
		m_policyFactory(NULL)
	{
		FUNCTION_ENTRY("XacmlContextFactory");
		try
		{
			m_contextFactory = new XacmlContextFactory;
			m_policyFactory  = new XacmlPolicyFactory;
		}
		catch( ... )
		{
			delete m_contextFactory;
			delete m_policyFactory;
			throw;
		}
		FUNCTION_EXIT;
	}

	XacmlFactory::~XacmlFactory()
	{
		delete m_contextFactory;
		delete m_policyFactory;
	}

	std::auto_ptr<Context> XacmlFactory::buildContext( const TA_Base_Bus::SessionInfo & session,
		unsigned long subject, unsigned long resource, unsigned long action ) const
	{
		TA_ASSERT( NULL != m_contextFactory, "m_contextFactory == NULL" );

		return( m_contextFactory->build( session, subject, resource, action ) );
	}
	
	std::auto_ptr<Context> XacmlFactory::buildContext( const TA_Base_Bus::SessionInfo & session,
		unsigned long subject, unsigned long location, unsigned long subsystem, unsigned long action ) const
	{
		TA_ASSERT( NULL != m_contextFactory, "m_contextFactory == NULL" );
		
		return( m_contextFactory->build( session, subject, location, subsystem, action ) );
	}
	
	std::auto_ptr<PolicySet> XacmlFactory::buildPolicy() const
	{
		TA_ASSERT( NULL != m_policyFactory, "m_policyFactory == NULL" );

		return( m_policyFactory->build() );
	}
}
