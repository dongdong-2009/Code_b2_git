
#include"app/radio/radio_mft_agent/src/RemoteAPICallBackMessage.h"
#include "app\radio\radio_mft_agent\src\ApiResponseHandlerHelper.h"

namespace TA_IRS_App
{

	ApiResponseHandlerHelper::ApiResponseHandlerHelper (RemoteApiConnection &remoteApiConnection)
		: m_remoteApiConnection (remoteApiConnection)
	{
		FUNCTION_ENTRY ("ApiResponseHandlerHelper");
		//start ();
		FUNCTION_EXIT;
	}

	ApiResponseHandlerHelper::~ApiResponseHandlerHelper ()
	{
		FUNCTION_ENTRY ("~ApiResponseHandlerHelper");
		FUNCTION_EXIT;
	}

	void ApiResponseHandlerHelper::processEventRequest (TA_IRS_App::RemoteAPIRadioEvent * eventObject)
	{
		FUNCTION_ENTRY ("processEventRequest");

		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					"Processing Event : MessageType : %d",eventObject->getMessageType());


		m_remoteApiConnection.processEvent(eventObject);

		FUNCTION_EXIT;
	}

}