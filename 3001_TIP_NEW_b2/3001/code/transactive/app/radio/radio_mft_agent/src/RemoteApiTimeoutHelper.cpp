#include "app/radio/radio_mft_agent/src/RemoteApiTimeoutHelper.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentStatusManager.h"

void RemoteApiTimeoutHelper::executeAlarm(TA_Base_Core::AlarmClock* clock)
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Keep alive message not received in 60 sec. MCC Connection disconnected. Reconnect in 25 sec");
	
	// timer expire need to set timer expiration flag to true and call failToConnectRadio )
	The_StatusManager::instance()->failConnectToRadio();
}

void RemoteApiTimeoutHelper::notifyTimerExpire(ETimerStatus timerStatus)
{
	if (ETimerExpire == timerStatus)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Keep alive message not received in 60 sec. MCC Connection disconnected. Reconnect in 25 sec");
		
		if (!The_StatusManager::instance()->getMccConnectionStatus())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Notify Timeout not needed mcc is not connected");			
			return;
		}

		The_StatusManager::instance()->failConnectToRadio();
		
	}
	else if (ETimerStop == timerStatus)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Timeout Timer was stop");
	}
	
}