
#pragma once 


#include "core/utilities/src/DebugUtil.h"
#include "core\threads\src\Thread.h"
#include "core\synchronisation\src\Semaphore.h"
#include "core\synchronisation\src\ReEntrantThreadLockable.h"

#include"app/radio/radio_mft_agent/src/RemoteAPIRadioEvent.h"
#include "app\radio\radio_mft_agent\src\ApiResponseHandlerHelper.h"

#include "boost\shared_ptr.hpp"
#include "core/threads/src/QueueProcessor.h"
#include"app/radio/radio_mft_agent/src/RemoteAPIRadioEvent.h"

namespace TA_IRS_App
{
	class ApiResponseHandler : public TA_Base_Core::QueueProcessor<TA_IRS_App::RemoteAPIRadioEvent>
	{
		public:
			ApiResponseHandler (RemoteApiConnection & remoteApiConnection);
			~ApiResponseHandler ();
			
			void sendResult (const ::MCC7500SDK::Parameters & message);

		protected:
			void processEvent (boost::shared_ptr<TA_IRS_App::RemoteAPIRadioEvent> eventItem);
	
		private:
			
			ApiResponseHandlerHelper m_responseHelper;
			TA_IRS_App::RemoteAPIRadioEvent * m_eventItem;
			::MCC7500SDK::Parameters m_message;
			TA_IRS_App::RemoteAPIRadioEvent * creteEventFromResponse (const ::MCC7500SDK::Parameters & message);

	};
}
