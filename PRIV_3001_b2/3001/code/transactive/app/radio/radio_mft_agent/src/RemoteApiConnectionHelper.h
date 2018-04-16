#pragma once

#include"app/radio/radio_mft_agent/src/RemoteAPIRadioEvent.h"
#include"app/radio/radio_mft_agent/src/RadioEventDispatcher.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include <string>

class RemoteApiConnection;

namespace TA_IRS_App
{
	class RemoteApiConnectionHelper
	{
		public:
			RemoteApiConnectionHelper(RemoteApiConnection * remoteApiConnection);
			~RemoteApiConnectionHelper(void);

			bool establishConnectionToMcc (MCC7500SDK::IAPIRequest_ptr &request, TA_Base_Core::ReEntrantThreadLockable &apiLock);
			
		private:
			CORBA::ORB_var orb;
			PortableServer::POA_var rootpoa;
			PortableServer::POAManager_var pman;
			PortableServer::POA_var m_child_poa;
			CORBA::Object_var obj;
			RemoteApiConnection * m_remoteApiConnection;
			bool m_bResult;

			void setOrbObjects ();
			MCC7500SDK::IAPIRequestServer_var m_server;
			BYTE m_clientid;
	};

}
