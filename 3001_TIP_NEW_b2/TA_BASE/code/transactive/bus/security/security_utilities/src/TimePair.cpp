/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/security_utilities/src/TimePair.cpp $
  *  @author:  <your name>
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2012/02/06 16:15:14 $
  *  Last modified by:  $Author: haijun.li $
  *
  *  <description>
  */

#ifdef _MSC_VER
	#pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif // _MSC_VER

#include "SecurityUtilitiesInternal.h"

namespace TA_Base_Bus
{
	TimePair::TimePair() :
		m_lower(0),
		m_upper(0)
	{
	}

	TimePair::TimePair( time_t time ) :
		m_lower(time),
		m_upper(time)
	{
	}

	TimePair::TimePair( const TimePair& copyThis ) :
		m_lower(copyThis.m_lower),
		m_upper(copyThis.m_upper)
	{
			TA_ASSERT( m_upper >= m_lower, "Upper time must be >= lower time" );
	}

	void TimePair::setUpper( time_t upper )
	{
		TA_ASSERT( upper >= m_lower, "Upper time must be >= lower time" );

		m_upper = upper;
	}

	bool TimePair::operator< ( const TimePair& rhs ) const
	{
		if( (m_lower != m_upper) || (rhs.m_lower != rhs.m_upper) )
		{
			if( (m_lower < rhs.m_lower) && (m_upper <= rhs.m_lower) )
			{
				return( true );
			}
		}

		return( false);
	}
}
