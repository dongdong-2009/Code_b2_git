/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/security/duty_agent/src/RightsLibraryUpdateSender.cpp $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Listens to PeerStateChangeSubject and sends updates to the Rights Library.
  *
  */

#include "DutyAgent.h"
#include "RightsLibraryUpdateSender.h"
#include "RightsLibraryUpdateConverter.h"

namespace TA_Base_App
{
	
	RightsLibraryUpdateSender::RightsLibraryUpdateSender() :
		TA_Base_Bus::SeDutyStateObserver<TA_Base_Bus::PeerStateChangeSubjectImpl>(*TA_Base_Bus::PeerStateChangeSubject::instance()),
		TA_Base_Bus::SeObserver<TA_Base_Bus::AgentTerminateSubjectImpl> (*TA_Base_Bus::AgentTerminateSubject::instance() ),
		m_commsSender(TA_Base_Core::MessagePublicationManager::getInstance().
			getCommsMessageSender(TA_Base_Core::DutyAgentBroadcastComms::Context))
	{
	}
	
		void RightsLibraryUpdateSender::update( TA_Base_Bus::PeerStateChangeSubjectImpl& peerStateChange, const TA_Base_Bus::DutyState & state )
	{
        FUNCTION_ENTRY("PeerStateChangeSubjectImpl::update");

		//TA_Base_Bus::DutyState* state = peerStateChange.getStateUpdate();

        // TD12082: Don't bother processing the update if no duty information is contained in it.
        if ( state.isEmpty() )
        {
            FUNCTION_EXIT;
            return;
        }

		RightsLibraryUpdateConverter rluc;

		state.convert(rluc);
	
		CORBA::Any commsData;
		commsData <<= rluc.getUpdate();

		//m_commsSender->sendBroadcastCommsMessage( TA_Base_Core::DutyAgentBroadcastComms::RightsLibraryCacheUpdate, commsData, NULL );

        FUNCTION_EXIT;
	}

	void RightsLibraryUpdateSender::update( TA_Base_Bus::AgentTerminateSubjectImpl&  terminateUpdate )
	{
        FUNCTION_ENTRY("AgentTerminateSubjectImpl::update");

		PeerStateParent::detach();
		AgentTerminateParent::detach();

        FUNCTION_EXIT;
	}
}
