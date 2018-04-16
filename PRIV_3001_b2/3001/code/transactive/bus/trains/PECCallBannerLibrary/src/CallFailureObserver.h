#pragma once

//#include "bus\banner_pages\call_page_library\src\CallEntryManager.h"
#include "bus\trains\PECCallBannerLibrary\src\PECCallEntry.h"
#include "bus\banner_pages\call_page_library\src\CallEntryManager.h"

//#include "bus/trains/PECCallBannerLibrary/src/ICallFailureObserver.h"

//#include "core/message/IDL/src/CommsMessageCorbaDef.h"

namespace TA_IRS_Bus
{
	class CallEntryManager;

	class CallFailureObserver : public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
			//						
								  
	{
		public:
			CallFailureObserver(CallEntryManager * callEntryMngr);
			~CallFailureObserver(void);
			
			void notifyCallTermination (unsigned long callId);
			void notifyCallSetupFailed ();
			void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);


		private:
			CallEntryManager * m_callManager;
	};

}
