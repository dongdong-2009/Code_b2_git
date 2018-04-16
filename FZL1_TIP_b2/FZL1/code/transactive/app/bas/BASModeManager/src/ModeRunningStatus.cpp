#include "StdAfx.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/bas/BASModeManager/src/ModeRunningStatus.h"
#include "app/bas/BASModeManager/src/ModeStatusListenerManager.h"
#include "app/bas/BASModeManager/src/BasManagerDataHelper.h"
#include "app/bas/BASModeManager/src/SubSystemStatus.h"
#include "app/bas/BASModeManager/src/BasManagerDataHelper.h"

Namespace_TA_IRS_APP_Begin
const std::string ModeRunningStatusConfigManager::m_MRS_Unknown = "停止";
const std::string ModeRunningStatusConfigManager::m_MRS_Running = "执行中";
const std::string ModeRunningStatusConfigManager::m_MRS_Fail = "执行失败";
const std::string ModeRunningStatusConfigManager::m_MRS_Success = "执行成功";
ModeRunningStatusConfigManager::ModeRunningStatusConfigManager(void)
{
}

ModeRunningStatusConfigManager::~ModeRunningStatusConfigManager(void)
{
}

std::string ModeRunningStatusConfigManager::getModeRunningStatusByType(int modeStatus)
{
	FUNCTION_ENTRY("getModeRunningStatusByType");
	std::string strStatusValue = "";
	switch (modeStatus)
	{
	case eModeStateUnknown:
		strStatusValue = m_MRS_Unknown;
		break;
	case eModeExecuting:
		strStatusValue = m_MRS_Running;
		break;
	case eModeExecutionFail:
		strStatusValue = m_MRS_Fail;
		break;
	case eModeExecutionSuccess:
		strStatusValue = m_MRS_Success;
		break;
	default:
		strStatusValue = m_MRS_Unknown;
		break;
	}
	FUNCTION_EXIT;
	return  strStatusValue;
}

int ModeRunningStatusConfigManager::getModeStatusByName(const std::string& modeStatus)
{
	int status = eModeStateUnknown;
	if (m_MRS_Unknown == modeStatus)
	{
		status = eModeStateUnknown;
	}
	else if (m_MRS_Running == modeStatus)
	{
		status = eModeExecuting;
	}
	else if (m_MRS_Success == modeStatus)
	{
		status = eModeExecutionSuccess;
	}
	else if (m_MRS_Fail == modeStatus)
	{
		status = eModeExecutionFail;
	}
	return status;
}
//////////////////////////////////////////////////////////////////////////////
ModeStatusManager::ModeStatusManager(void)
{
	FUNCTION_ENTRY("ModeStatusManager");
	FUNCTION_EXIT;
}

ModeStatusManager::~ModeStatusManager(void)
{
	FUNCTION_ENTRY("~ModeStatusManager");
	FUNCTION_EXIT;
}

void ModeStatusManager::updateModeStatus(unsigned long modeKey, int status)
{
	FUNCTION_ENTRY("UpdateModeStatus");
	bool bNeedUpdate = false;
	bool bFoundCache = false;

    unsigned long locationKey = ULONG_MAX;
    unsigned long subsystemKey = ULONG_MAX;
	Mode_SmartPtr mode = BasManagerDataHelperInstance::instance()->getModeByKey(modeKey);
	if (NULL != mode.get())
	{
		TA_THREADGUARD(m_modeStatusLock);
		locationKey = mode->getLocationKey();
		subsystemKey = mode->getSubsystemKey();

		LocationModeStatusIterator it = m_modeStatus.find(locationKey);	
		if(it != m_modeStatus.end())
		{
			SubsystemModeIterator subIt = it->second.find(subsystemKey);
			if (subIt != it->second.end())
			{
				cleanLocationSubsystemModesStatus(locationKey, subsystemKey); // clean this subsystem other mode status to unknown.
				ModeStatusIterator modeIt = subIt->second.find(modeKey);
				if (modeIt != subIt->second.end())
				{
                    bFoundCache = true;

					modeIt->second = status;
					bNeedUpdate = true;
				}
			}
		}
		
		if (false == bFoundCache)
		{
			m_modeStatus[locationKey][subsystemKey][modeKey] = status;
			bNeedUpdate = true;
		}
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "can not found the mode now, mode value = %lu", modeKey);
		return;
	}

	// there should be update other listeners to update the GUI
	if (false != bNeedUpdate)
	{
        //Reset mode timeout flag 
        SubSystemStatusInstance::instance()->updateSubsystemModeTimeoutFlag(locationKey, subsystemKey, false);

		BAS_TRY
		{
            ModeStatusListenerManagerInstance::instance()->updateModeStatus(modeKey, status);

            // if the status if running, update the begin time
            if (NULL != mode.get())
            {
                SubSystemStatusInstance::instance()->updateSubsystemMode(mode->getLocationKey(), mode->getSubsystemKey(),modeKey, ModeSet::modeType_CurrentMode);
                if ( eModeExecuting == status )
                {
                    {
                        TA_THREADGUARD(m_RunningModeLock);
                        m_modeExecuteTime[modeKey] = time(0);
                    }
                    SubSystemStatusInstance::instance()->updateSubsystemMode(mode->getLocationKey(), mode->getSubsystemKey(), modeKey, ModeSet::modeType_RunningMode);
                }
                else
                {
                    {
                        TA_THREADGUARD(m_RunningModeLock);
                        m_modeExecuteTime.erase(modeKey);
                    }
                    SubSystemStatusInstance::instance()->updateSubsystemModeExecuteForClean(mode->getLocationKey(), mode->getSubsystemKey(), ModeSet::modeType_RunningMode);
                }

                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Mode status update, ModeKey:%d ModeName:%s Subsystem:%s  Status:%s ",
                    modeKey, mode->getName().c_str(), mode->getSubsystemName().c_str(), ModeRunningStatusConfigManager::getModeRunningStatusByType(status).c_str());
            }
            else
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "can not found the mode now, mode value = %lu", modeKey);
            }
		}
		BAS_CATCH;

	}
	FUNCTION_EXIT;
}

void ModeStatusManager::updateModeStatusForClean(unsigned long locationKey, unsigned long subsystemKey)
{
    FUNCTION_ENTRY("UpdateModeStatus");
    cleanLocationSubsystemModesStatus(locationKey, subsystemKey);
    SubSystemStatusInstance::instance()->updateSubsystemModeExecuteForClean(locationKey, subsystemKey, ModeSet::modeType_RunningMode);
    SubSystemStatusInstance::instance()->updateSubsystemModeExecuteForClean(locationKey, subsystemKey, ModeSet::modeType_CurrentMode);
    FUNCTION_EXIT;
}

// get the mode current status 
int ModeStatusManager::getModeStatus(unsigned long modeKey)
{
	FUNCTION_ENTRY("getModeStatus");
	int status = eModeStateUnknown;
	TA_THREADGUARD(m_modeStatusLock);
	Mode_SmartPtr mode = BasManagerDataHelperInstance::instance()->getModeByKey(modeKey);
	if (NULL != mode.get())
	{
		LocationModeStatusIterator locationIt = m_modeStatus.find(mode->getLocationKey());
		if (locationIt != m_modeStatus.end())
		{
			SubsystemModeIterator subIt = locationIt->second.find(mode->getSubsystemKey());
			if (subIt != locationIt->second.end())
			{
				ModeStatusIterator it = subIt->second.find(modeKey);
				if (it != subIt->second.end())
				{
					status = it->second; 
				}
			}			
		}
	}
	FUNCTION_EXIT;
	return status;
}

std::vector<unsigned long> ModeStatusManager::getRunningMode()
{
	FUNCTION_ENTRY("getRunningMode");
	TA_THREADGUARD(m_RunningModeLock);
	std::vector<unsigned long> runningModeKey;
	std::map<unsigned long, time_t>::iterator it;
	for (it = m_modeExecuteTime.begin(); it != m_modeExecuteTime.end(); it++)
	{
		runningModeKey.push_back((*it).first);
	}
	FUNCTION_EXIT;
	return runningModeKey;
}

bool ModeStatusManager::isModeExecuteSuccessInTime(unsigned long modeKey)
{
	FUNCTION_ENTRY("isModeExecuteSuccessInTime");
	TA_THREADGUARD(m_RunningModeLock);
	static const unsigned long timeInterval = BasManagerDataHelperInstance::instance()->getModeExecuteTimeOutTime();
	bool bSuccess = true; //default status is success
	time_t beginExecuteTime = 0;
	if (m_modeExecuteTime.end() != m_modeExecuteTime.find(modeKey))
	{
        if ((( time(0) - m_modeExecuteTime[modeKey] ) >= timeInterval ) && ( getModeStatus(modeKey) != eModeExecutionSuccess ))
        {
            bSuccess = false;
        }
        beginExecuteTime = m_modeExecuteTime[modeKey];
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "ModeKey:%lu BeginRunningTime:%lu  CurrentTime %lu IsExpired:%s",
		modeKey, beginExecuteTime, time(0), bSuccess ? "False" : "True");
	FUNCTION_EXIT;
	return bSuccess;
}

void ModeStatusManager::cleanLocationSubsystemModesStatus(unsigned long locationKey, unsigned long subsystemKey)
{
	LocationModeStatusIterator locationIt = m_modeStatus.find(locationKey);
	if (locationIt != m_modeStatus.end())
	{
		SubsystemModeIterator subIt = locationIt->second.find(subsystemKey);
		if (subIt != locationIt->second.end())
		{
			ModeStatusListenerManager* modeStatusListenerManager = ModeStatusListenerManagerInstance::instance();
			for (ModeStatusIterator it = subIt->second.begin(); it != subIt->second.end(); it++)
			{
					it->second = eModeStateUnknown;
					modeStatusListenerManager->updateModeStatus(it->first, it->second);
			}
		}
	}
}

Namespace_TA_IRS_APP_End
