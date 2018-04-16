#include "MCCRestartMngr.h"

namespace TA_IRS_App
{

	MCCRestartMngr::MCCRestartMngr(TA_IRS_App::RemoteApiParamHolder* paramHolder, string ior,string username,string password) : 
		m_semaphore(0),
		m_bTerminate (false),
		m_signal(false),
		m_strIor(ior),
		m_strPassword(password),
		m_strUserName(username),
		m_pParamHolder(paramHolder)
	{
		start ();
	}

	MCCRestartMngr::~MCCRestartMngr(void)
	{
		terminate();
		m_semaphore.post ();
		terminateAndWait();
	}

	void MCCRestartMngr::run ()
	{		

		while (!m_bTerminate)
		{
			m_semaphore.wait();

			if (!m_bTerminate)
			{
 
				TA_Base_Core::ThreadGuard guard( m_lock );
				
				//Get the RadioMFT Connection State to know when
				// to descontruct the RadioApiConnection.
				RadioMFTAgentStatusManager::RadioMFTState radioMftState = The_StatusManager::instance()->getRadioMFTState();

				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"RadioMFT Connection State : %d",
					static_cast<RadioMFTAgentStatusManager::RadioMFTState>(radioMftState));

				//Make sure that RadioMFT connection state is not equal to Registration state
				//before calling destructor of RadioApiConnection.
				
				if(NULL != m_pParamHolder)
				{
					if (RadioMFTAgentStatusManager::REGISTRATION_STATE != radioMftState )
					{
						if(NULL != m_pParamHolder->apiConnection)
						{
							delete m_pParamHolder->apiConnection;
							m_pParamHolder->apiConnection = NULL;
						}
						
						LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
							"Done Deleting RemoteApi Obj : %p",m_pParamHolder->apiConnection);

						m_signal = false;
						m_pParamHolder->apiConnection = new RemoteApiConnection ( m_pParamHolder->IApiRequest,
																					m_strIor.c_str(),
																					m_strUserName.c_str(),
																					m_strPassword.c_str(),
																					*m_pParamHolder->eventDispatcher,
																					m_pParamHolder->apiLock);


						int mccWaitingTime = RadioMFTAgentHelper::getMCCReconnectionWaitingTime();
						LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Waiting for MCC at %d seconds before calling MCC Registration..", ( mccWaitingTime/1000) );
						TA_Base_Core::Thread::sleep(mccWaitingTime);

						LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"m_signal : %s",
							m_signal ?"TRUE":"FALSE");

						if ( false == m_signal )
						{
							//Set the RadioMFT State to Registration
							The_StatusManager::instance()->setRadioMFTState(RadioMFTAgentStatusManager::REGISTRATION_STATE);
							// attempt again to reconnect to MCC Radio
							m_pParamHolder->apiConnection->connect();
						}

					}
				}
				
			}	
		}
	}

	void MCCRestartMngr::terminate ()
	{
		m_bTerminate = true;
	}	

	void MCCRestartMngr::signalRestart ()
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Receive Signal from Gateway..");
	
		m_signal = true;
		m_semaphore.post();
	}
}