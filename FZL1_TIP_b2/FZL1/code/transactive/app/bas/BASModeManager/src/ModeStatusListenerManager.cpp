#include "StdAfx.h"
#include <algorithm>
#include "core/utilities/src/DebugUtil.h"
#include "app/bas/BASModeManager/src/ModeStatusListenerManager.h"
#include "app/bas/BASModeManager/src/BasManagerSubscriber.h"
#include "app/bas/BASModeManager/src/ModeRunningStatus.h"

Namespace_TA_IRS_APP_Begin
ModeStatusListenerManager::ModeStatusListenerManager(void)
{
}

ModeStatusListenerManager::~ModeStatusListenerManager(void)
{
}

unsigned long ModeStatusListenerManager::addListener(IModeStatusListenerInterface* listener)
{
	FUNCTION_ENTRY("addListener");
	TA_THREADGUARD(m_listenerLock);
	if (NULL != listener )
	{
		if( m_vctLister.end() == std::find(m_vctLister.begin(), m_vctLister.end(), listener))
		{
			m_vctLister.push_back(listener);
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "you have add a same listener");
		}
	}
	else
	{
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "listener is NULL");
	}

	FUNCTION_EXIT;
	return static_cast<unsigned long>(m_vctLister.size());
}

unsigned long ModeStatusListenerManager::removeListener(IModeStatusListenerInterface* listener)
{
	FUNCTION_ENTRY("removeListener");
	TA_THREADGUARD(m_listenerLock);
	if (NULL != listener )
	{
		std::vector<IModeStatusListenerInterface*>::iterator it;
		it = std::find(m_vctLister.begin(), m_vctLister.end(), listener);
		if( m_vctLister.end() != it)
		{
			m_vctLister.erase(it);
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "do not find this listener, did you add it ?");
		}
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "listener is NULL");
	}

	FUNCTION_EXIT;
	return static_cast<unsigned long>(m_vctLister.size());
}

void ModeStatusListenerManager::updateModeStatus(unsigned long modeKey, int status)
{
	FUNCTION_ENTRY("updateModeStatus");
	//TA_THREADGUARD(m_listenerLock);
	std::vector<IModeStatusListenerInterface*>::iterator it;
	for (it = m_vctLister.begin(); it != m_vctLister.end(); it++)
	{
		(*it)->updateModeStatus(modeKey, status);
	}
	FUNCTION_EXIT;
}
Namespace_TA_IRS_APP_End
