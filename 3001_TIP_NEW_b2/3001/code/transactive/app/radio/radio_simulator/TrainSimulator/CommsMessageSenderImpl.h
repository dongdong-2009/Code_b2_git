#pragma once


#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/utilities/src/DebugUtil.h"



class CommsMessageSenderImpl
{
	public:
		CommsMessageSenderImpl(void);
		~CommsMessageSenderImpl(void);
		
		TA_Base_Core::CommsMessageSender* m_commsMessageSender;


		void sendStateUpdate (bool bRelease=false);
};
