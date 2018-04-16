#ifndef __TAPIRESPONSE__
#define __TAPIRESPONSE__
#include"app/radio/radio_mft_agent/src/RemoteAPI.h"
#include<iostream>
#include "ace/Get_Opt.h"
#include "core/utilities/src/DebugUtil.h"
#include"app/radio/radio_mft_agent/src/RemoteAPICallBackMessage.h"
#include"app/radio/radio_mft_agent/src/RemoteAPIRadioEvent.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentStatusManager.h"
#include "app\radio\radio_mft_agent\src\RemoteApiConnection.h"
#include "app\radio\radio_mft_agent\src\ApiResponseHandler.h"
#include "core\synchronisation\src\ReEntrantThreadLockable.h"

/*
#include<omniORB4/CORBA_Any.h>
*/
using namespace std;
using namespace TA_IRS_App;

//class RemoteApiConnection;

namespace MotorolaRemoteAPI
{

class TAPIResponse : public virtual POA_MCC7500SDK::IAPIResponse
{
	//RemoteApiConnection &api_connection;
	ApiResponseHandler * m_apiResponseHandler;

	//provides a thread lock
	TA_Base_Core::ReEntrantThreadLockable m_lock;

public:
	TAPIResponse         (RemoteApiConnection &con):m_apiResponseHandler (NULL)//:api_connection(con)
    {
		m_apiResponseHandler = new ApiResponseHandler (con);
    }
    virtual ~TAPIResponse()
    {
		if (m_apiResponseHandler)
		{
			delete m_apiResponseHandler;
			m_apiResponseHandler = NULL;
		}
    }
    virtual void  CallBack(const ::MCC7500SDK::Parameters & message)
    {
		TA_Base_Core::ThreadGuard guard( m_lock );

		LOG_GENERIC (SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing Log : CallBack from MCC hardware");
		
		const ::MCC7500SDK::Parameters localMessage = message;

		if(localMessage.length()==0)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Received Keep-ALIVE message");

			if (The_StatusManager::instance()->getMccConnectionStatus())
			{
				The_StatusManager::instance()->startTimer();
			}						
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Before processing MCC7500 message");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Testing Log : MCC7500 SERVER -> CLIENT : Callback invoked from server. Parameter len: %d Message TYPE: %u(0x%x) | Name : %s",message.length(),message[0].V_UINT16(),message[0].V_UINT16(),RemoteAPICallBackMessage::getResponseName(message[0].V_UINT16()));
			
			if (The_StatusManager::instance()->getMccConnectionStatus())
			{
				The_StatusManager::instance()->startTimer();
				m_apiResponseHandler->sendResult(localMessage);
			}
			

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "After processing MCC7500 message");

		}
    }
};
}
#endif