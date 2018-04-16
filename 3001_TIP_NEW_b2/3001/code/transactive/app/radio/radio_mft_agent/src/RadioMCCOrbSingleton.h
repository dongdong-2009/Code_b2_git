
#pragma once

#include "core\synchronisation\src\ReEntrantThreadLockable.h"
#include "core\utilities\src\DebugUtil.h"
#include "ace/corba_macros.h"
#include "core/threads/src/Thread.h"
#include"app/radio/radio_mft_agent/src/RemoteAPIRadioEvent.h"
#include"app/radio/radio_mft_agent/src/RadioEventDispatcher.h"

namespace TA_IRS_App
{
	class RadioMCCOrbSingleton
	{

		public:
			virtual ~RadioMCCOrbSingleton();

			static RadioMCCOrbSingleton * getInstance ();
			static void removeInstance ();
		
			CORBA::ORB_var getOrb ();
			PortableServer::POA_var getRootPoa ();
			PortableServer::POAManager_var getPoaMngr ();
			PortableServer::POA_var getChildPoa ();
			CORBA::Object_var getObject();

			void intializeMCCORB (const std::string & strIor);

		private:
			RadioMCCOrbSingleton ();

			static RadioMCCOrbSingleton * m_singelonRadioMCCOrb;
			static TA_Base_Core::ReEntrantThreadLockable m_threadLock;
			
			// corba objects neccessary for init
			CORBA::ORB_var m_Orb;
			PortableServer::POA_var m_rootPoa;
			PortableServer::POAManager_var m_poaMngr;
			PortableServer::POA_var m_childPoa;
			CORBA::Object_var m_Obj;

			std::string m_strIor;
			bool m_bInit;

			// function helper to initialize corba objects
			void initialize ();
			

	};
}
