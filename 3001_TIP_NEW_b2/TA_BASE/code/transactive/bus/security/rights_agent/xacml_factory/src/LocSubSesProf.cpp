/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/rights_agent/xacml_factory/src/LocSubSesProf.cpp $
 * @author:   Nick Jardine
 * @version:  $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/02/06 16:15:14 $
 * Last modified by:  $Author: haijun.li $
 *
 * Implements the LocSubSesProf interface.
 *
 */

#include "XacmlFactoryInternal.h"

namespace TA_Base_Bus
{
	LocSubSesProf::LocSubSesProf( const unsigned long location, const unsigned long subsystem,
		const std::string& sessionId, const unsigned long profile ) :
		m_location (location ),
		m_subsystem(subsystem),
		m_sessionId(sessionId),
		m_profile  (profile  )
	{
	}


	bool LocSubSesProf::operator==( const LocSubSesProf& rhs ) const
	{
		if( m_location != rhs.m_location )
		{
			return false;
		}

		if( m_subsystem != rhs.m_subsystem )
		{
			return false;
		}

		if( m_sessionId != rhs.m_sessionId )
		{
			return false;
		}

		if( m_profile != rhs.m_profile )
		{
			return false;
		}
		return true;
	}

	bool LocSubSesProf::operator< (const LocSubSesProf& rhs) const
	{
		if( m_location < rhs.m_location )
		{
			return true;
		}

		if( m_location > rhs.m_location )
		{
			return false;
		}

		if( m_subsystem < rhs.m_subsystem )
		{
			return true;
		}

		if( m_subsystem > rhs.m_subsystem )
		{
			return false;
		}

		if( m_sessionId < rhs.m_sessionId )
		{
			return true;
		}

		if( m_sessionId > rhs.m_sessionId )
		{
			return false;
		}

		if( m_profile < rhs.m_profile )
		{
			return true;
		}

		return false;
	}
	
}

