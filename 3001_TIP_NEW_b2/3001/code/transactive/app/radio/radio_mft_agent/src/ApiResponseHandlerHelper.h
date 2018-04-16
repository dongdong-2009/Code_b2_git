
#pragma once 

#include "boost\shared_ptr.hpp"
#include "core/threads/src/QueueProcessor.h"
#include "core/utilities/src/DebugUtil.h"
#include"app/radio/radio_mft_agent/src/RemoteAPIRadioEvent.h"
#include "app\radio\radio_mft_agent\src\RemoteApiConnection.h"

namespace TA_IRS_App
{
	class ApiResponseHandlerHelper
	{
		public:
			~ApiResponseHandlerHelper ();
			ApiResponseHandlerHelper (RemoteApiConnection &remoteApiConnection);	
		
			void processEventRequest (TA_IRS_App::RemoteAPIRadioEvent * eventObject);

		private:
			RemoteApiConnection & m_remoteApiConnection;

	};
}
