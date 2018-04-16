#ifndef REMOTE_API_CONN_H_INCLUDED 
#define REMOTE_API_CONN_H_INCLUDED

#include "app/radio/radio_mft_agent/src/RemoteApiConnection.h"
#include "app/radio/radio_mft_agent/src/RemoteAPI.h"
#include "app/radio/radio_mft_agent/src/RadioEventDispatcher.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_IRS_App
{
	struct RemoteApiParamHolder
	{
		RemoteApiConnection *  apiConnection;
		MCC7500SDK::IAPIRequest_ptr IApiRequest;
		RadioEventDispatcher * eventDispatcher;
		TA_Base_Core::ReEntrantThreadLockable apiLock;
	};

	typedef RemoteApiParamHolder* RemoteApiParamHolderPtr;
}

#endif //REMOTE_API_CONN_H_INCLUDED
