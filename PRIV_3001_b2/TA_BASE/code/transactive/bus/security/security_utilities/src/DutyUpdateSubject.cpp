/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/security/security_utilities/src/DutyUpdateSubject.cpp $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Receives session updates from the Authentication Agent and publishes
  * the update using the observer pattern.
  *
  */

#ifdef _MSC_VER
	#pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif // _MSC_VER

#include "bus/security/security_utilities/src/SecurityUtilitiesInternal.h"

namespace TA_Base_Bus
{
	DutyUpdateSubject::DutyUpdateSubject() :
        m_sessionId(NULL),
		m_updateFn(NULL)
	{
		FUNCTION_ENTRY("DutyUpdateSubject");

        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
            TA_Base_Core::DutyAgentBroadcastComms::DutyChangedNotification, this, NULL);

        FUNCTION_EXIT;
	}

	DutyUpdateSubject::~DutyUpdateSubject()
	{
        FUNCTION_ENTRY("~DutyUpdateSubject");
		TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
        FUNCTION_EXIT;
	}

	void DutyUpdateSubject::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
	{
        FUNCTION_ENTRY("receiveSpecialisedMessage");

		DutyAgentTypeCorbaDef::DutyPopup* data = NULL;

		// process DutyChangedNotification message
        if( 0 == std::string(message.messageTypeKey).compare(TA_Base_Core::DutyAgentBroadcastComms::DutyChangedNotification.getTypeKey()) )
        {

			// received expected duty agent message
            if (0 != ( message.messageState >>= data ) )
            {
				m_sessionId = data->targetSession;
                m_updateFn  = &DutyUpdateSubject::changed;
            }
        }
        else
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Received unexpected duty agent message: %s", message.messageTypeKey.in() );
        }

		// notify duty update
		if( NULL != m_updateFn )
		{
			notify(*this);
		}
		m_sessionId = NULL;

        FUNCTION_EXIT;
	}

	void DutyUpdateSubject::interpretUpdate( IDutyUpdateObserver& observer )
	{
		FUNCTION_ENTRY("interpretUpdate");

		// notify duty update
		if( NULL != m_updateFn )
		{
			(this->*m_updateFn)(observer);
		}

		FUNCTION_EXIT;
	}

	void DutyUpdateSubject::changed( IDutyUpdateObserver& observer )
	{
		FUNCTION_ENTRY("begin");

		// notify session update
		if( NULL != m_sessionId )
		{
			observer.dutyChanged( m_sessionId );
		}

		FUNCTION_EXIT;
	}

}
