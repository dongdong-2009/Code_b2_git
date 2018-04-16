/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/security_utilities/src/SessionUpdateSubject.cpp $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/06/07 14:28:46 $
  * Last modified by:  $Author: liwei.gong $
  *
  * Receives session updates from the Authentication Agent and publishes
  * the update using the observer pattern.
  *
  */

#ifdef _MSC_VER
	#pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif // _MSC_VER

#include "SecurityUtilitiesInternal.h"

namespace TA_Base_Bus
{
	SessionUpdateSubject::SessionUpdateSubject() :
		m_session(NULL),
		m_updateFn(NULL)
	{
		FUNCTION_ENTRY("SessionUpdateSubject");

        // Register the subscriber with the MessageSubscribtionManager
		TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage
            ( TA_Base_Core::AuthenticationAgentStateUpdateBroadcast::AuthenticationAgentDistributedUpdate, this, NULL );

        FUNCTION_EXIT;
	}

	SessionUpdateSubject::~SessionUpdateSubject()
	{
        FUNCTION_ENTRY("~SessionUpdateSubject");
		TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
        FUNCTION_EXIT;
	}

	void SessionUpdateSubject::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
	{
        FUNCTION_ENTRY("receiveSpecialisedMessage");

		// check message type
        if( 0 == std::string(message.messageTypeKey).compare(TA_Base_Core::AuthenticationAgentStateUpdateBroadcast::AuthenticationAgentDistributedUpdate.getTypeKey()) )
        {
			// message state not NULL
            if (0 != ( message.messageState >>= m_session ) )
            {
				// session not NULL
				if (m_session == NULL)
				{
					LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "m_session == NULL " );
				}

				// check update type
                switch( m_session->updateType )
				{
					case TA_Base_Bus::TA_Security::SessionStart:
					{
						m_updateFn = &SessionUpdateSubject::begin;
					}
					break;

					case TA_Base_Bus::TA_Security::ProfileUpdate:
					{
						// in case of override
						if( m_session->additionalProfiles.length() > 0 )
						{
							m_updateFn = &SessionUpdateSubject::overrideBegin;
						}
						else
						{
							m_updateFn = &SessionUpdateSubject::overrideEnd;
						}
					}
					break;

					case TA_Base_Bus::TA_Security::SessionEnd:
					{
						m_updateFn = &SessionUpdateSubject::end;
					}
					break;

					default:
						LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Received unexpected session update type: %d", m_session->updateType );
						m_updateFn = NULL;
					break;
				}
            }
        }
        else
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Received unexpected auth agent message: %s", message.messageTypeKey.in() );
        }

		// update
		if( NULL != m_updateFn )
		{
			notify(*this);
		}
		m_session = NULL;

        FUNCTION_EXIT;
	}

	void SessionUpdateSubject::interpretUpdate( ISessionUpdateObserver& observer )
	{
		FUNCTION_ENTRY("interpretUpdate");

		// update
		if( NULL != m_updateFn )
		{
			(this->*m_updateFn)(observer);
		}

		FUNCTION_EXIT;
	}

	void SessionUpdateSubject::begin( ISessionUpdateObserver& observer )
	{
		FUNCTION_ENTRY("begin");

		// session not NULL
		if( NULL != m_session )
		{
			observer.sessionBegin( m_session->SessionId.in(),
				                   m_session->UserId,
								   m_session->ProfileId,
								   m_session->LocationId,
								   m_session->WorkstationId );
		}
		FUNCTION_EXIT;
	}

	void SessionUpdateSubject::end( ISessionUpdateObserver& observer )
	{
		FUNCTION_ENTRY("end");
		observer.sessionEnd( m_session->SessionId.in() );
		FUNCTION_EXIT;
	}

	void SessionUpdateSubject::overrideBegin( ISessionUpdateObserver& observer )
	{
		FUNCTION_ENTRY("overrideBegin");
		observer.sessionOverrideBegin( m_session->SessionId.in(),
			                           m_session->additionalProfiles[m_session->additionalProfiles.length()-1] );
		FUNCTION_EXIT;
	}

	void SessionUpdateSubject::overrideEnd( ISessionUpdateObserver& observer )
	{
		FUNCTION_ENTRY("overrideEnd");
		observer.sessionOverrideEnd( m_session->SessionId.in(), m_session->ProfileId );
		FUNCTION_EXIT;
	}

}
