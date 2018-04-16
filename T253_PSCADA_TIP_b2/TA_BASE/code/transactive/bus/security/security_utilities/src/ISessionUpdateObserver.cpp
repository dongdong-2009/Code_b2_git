/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/security/security_utilities/src/ISessionUpdateObserver.cpp $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * Implements the update() method of ISessionUpdateObserver.
  *
  */

#ifdef _MSC_VER
	#pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif // _MSC_VER

#include "SecurityUtilitiesInternal.h"

namespace TA_Base_Bus
{
	ISessionUpdateObserver::ISessionUpdateObserver( SessionUpdateSubject& sessionUpdate ) :
		SeObserver<SessionUpdateSubject>(sessionUpdate)
	{
	}

	void ISessionUpdateObserver::update( SessionUpdateSubject& sessionUpdate )
	{
		sessionUpdate.interpretUpdate(*this);
	}

	void ISessionUpdateObserver::sessionBegin( const std::string& session, unsigned long user,
			                       unsigned long profile, unsigned long location,
								   unsigned long workstation )
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Session begin ignored (%s)", session.c_str() );
	}

	void ISessionUpdateObserver::sessionEnd( const std::string& session )
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Session end ignored (%s)", session.c_str() );
	}

	void ISessionUpdateObserver::sessionOverrideBegin( const std::string& session, unsigned long profile )
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Session override begin ignored (%s)", session.c_str() );
	}

	void ISessionUpdateObserver::sessionOverrideEnd( const std::string& session, unsigned long profile )
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Session override end ignored (%s)", session.c_str() );
	}
}
