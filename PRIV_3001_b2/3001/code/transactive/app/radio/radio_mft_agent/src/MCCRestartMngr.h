#pragma once

#include "core\threads\src\Thread.h"
#include "core\synchronisation\src\Semaphore.h"
#include "app\radio\radio_mft_agent\src\RemoteApiConnection.h"
#include "core\utilities\src\DebugUtil.h"
#include <string>
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "app/radio/radio_mft_agent/src/RemoteApiParamHolder.h"

namespace TA_IRS_App
{
	class MCCRestartMngr : public TA_Base_Core::Thread
	{
		public:
			
			MCCRestartMngr(TA_IRS_App::RemoteApiParamHolder* paramHolder,string ior,string username,string password );
			~MCCRestartMngr(void);

			void run ();
			void terminate ();

			void signalRestart ();
			
			

		private:
			bool m_bTerminate;
			TA_Base_Core::Semaphore m_semaphore;
			TA_IRS_App::RemoteApiParamHolder* m_pParamHolder;
			
			std::string m_strIor;
			std::string m_strUserName;
			std::string m_strPassword;

			//provides a thread lock
			TA_Base_Core::ReEntrantThreadLockable m_lock;
			bool m_signal;
	};

}
