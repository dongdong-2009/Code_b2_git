#include "StdAfx.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/bas/BASModeManager/src/SubSystemStatus.h"

namespace TA_IRS_App
{
	#define UnknownModeKey ULONG_MAX
	const unsigned long ModeSet::m_unknownMode = UnknownModeKey;
	ModeSet::ModeSet()
	:
	m_currentMode(UnknownModeKey), 
	m_runningMode(UnknownModeKey),
	m_modeAutoManul(UnknownModeKey),
	m_timeoutFlag(false)
	{
		FUNCTION_ENTRY("ModeSet");
		FUNCTION_EXIT;
	}
	ModeSet::~ModeSet()
	{
		FUNCTION_ENTRY("~ModeSet");
		FUNCTION_EXIT;
	}
	void ModeSet::updateModeInvalid(subsystemModeType modeType)
	{
		updateMode(m_unknownMode, modeType);
	}

	void ModeSet::updateMode(unsigned long value, subsystemModeType modeType)
	{
		FUNCTION_ENTRY("updateMode");
		//TA_THREADGUARD(m_modeLock);
		switch(modeType)
		{
		case modeType_RunningMode:
			m_runningMode = value;
			break;
		case modeType_CurrentMode:
			m_currentMode = value;
			if (m_runningMode == value)
			{
				m_runningMode = m_unknownMode;
			}
			break;
		case modeType_AutoManul:
			m_modeAutoManul = value;
			break;
		default:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown subSystemModeType.");
		}
		FUNCTION_EXIT;
	}

	void ModeSet::updateModeTimeoutFlag(bool isModeTimeout)
	{
	    FUNCTION_ENTRY("updateModeTimeoutFlag");
	    m_timeoutFlag = isModeTimeout;
	    FUNCTION_EXIT;
	}

	unsigned long ModeSet::getMode(subsystemModeType modeType)
	{
		FUNCTION_ENTRY("getMode");
		//TA_THREADGUARD(m_modeLock);
		unsigned long modeKey = 0;
		switch(modeType)
		{
		case modeType_RunningMode:
			modeKey = m_runningMode;
			break;
		case modeType_CurrentMode:
			modeKey = m_currentMode;
			break;
		case modeType_AutoManul:
			modeKey = m_modeAutoManul;
			break;
		default:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown subSystemModeType.");
		}
		FUNCTION_EXIT;
		return modeKey;
	}

	bool ModeSet::getModeTimeoutFlag()
	{
	    return m_timeoutFlag; 
	}

	unsigned long ModeSet::getUnknowModeKey()
	{
		return m_unknownMode;
	}
	////////////////////////////////////////////////////////////////////////
	SubSystemStatus::SubSystemStatus(void)
	:m_basManagerDlg(NULL)
	{
	}

	SubSystemStatus::~SubSystemStatus(void)
	{
	}

	void SubSystemStatus::updateSubsystemModeExecuteForClean(unsigned long locationKey, 
															unsigned long subsystemKey,
															ModeSet::subsystemModeType modeType)
	{
		FUNCTION_ENTRY("updateSubsystemModeExecuteForClean");
		using namespace TA_Base_Core;
 
		if (m_subsystemModes.end() == m_subsystemModes.find(locationKey))
		{
			SubsystemsModeInfo temp;
			m_subsystemModes[locationKey] = temp;
		}

		SubsystemModeIterator it = m_subsystemModes[locationKey].find(subsystemKey);
		if (it != m_subsystemModes[locationKey].end())
		{
			it->second.updateModeInvalid(modeType);
			m_basManagerDlg->updateSubsystemMode(locationKey, subsystemKey, modeType);
		}
		else
		{
			m_subsystemModes[subsystemKey][subsystemKey].updateModeInvalid(modeType);
		}

		FUNCTION_EXIT;
	}

	void SubSystemStatus::updateSubsystemMode(unsigned long locationKey, 
											unsigned long subsystemKey, 
											unsigned long value, 
											ModeSet::subsystemModeType modeType)
	{
		FUNCTION_ENTRY("updateSubsystemMode");
		using namespace TA_Base_Core;

		bool bNeedUpdate = false;
		{
			TA_THREADGUARD(m_subsystemModeLock);
			LocationSubsystemsModeInfo::iterator itFoundLocSubMode = m_subsystemModes.find(locationKey);
			if (m_subsystemModes.end() == itFoundLocSubMode)
			{
				SubsystemsModeInfo temp;
				itFoundLocSubMode = m_subsystemModes.insert(std::make_pair(locationKey, temp)).first;
			}

			SubsystemModeIterator it = itFoundLocSubMode->second.find(subsystemKey);
			if (it != itFoundLocSubMode->second.end())
			{
				it->second.updateMode(value, modeType);
				bNeedUpdate = true;
			}
			else
			{
				ModeSet modeTemp;
				it = itFoundLocSubMode->second.insert(std::make_pair(subsystemKey, modeTemp)).first;
				it->second.updateMode(value, modeType);
				bNeedUpdate = true;
			}
		}
	

		// there should update the listener
		if (true == bNeedUpdate)
		{
			if (NULL != m_basManagerDlg)
			{
				m_basManagerDlg->updateSubsystemMode(locationKey, subsystemKey, modeType);
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, "=== The BasManagerDialog invalid ===");
			}				
		}
		FUNCTION_EXIT;
	}

	void SubSystemStatus::updateSubsystemModeTimeoutFlag(unsigned long locationKey, unsigned long subsystemKey,bool isModeTimeout)
	{
	    FUNCTION_ENTRY("updateSubsystemModeTimeoutFlag");
	    using namespace TA_Base_Core;

	    LocationSubsystemsModeInfo::iterator itFoundLocSubMode = m_subsystemModes.find(locationKey);
	    if (m_subsystemModes.end() == itFoundLocSubMode)
	    {
	        SubsystemsModeInfo temp;
	        m_subsystemModes[locationKey] = temp;
	    }

	    SubsystemModeIterator it = m_subsystemModes[locationKey].find(subsystemKey);
	    if (it != m_subsystemModes[locationKey].end())
	    {
	        it->second.updateModeTimeoutFlag(isModeTimeout);
	    }
	    else
	    {
	        m_subsystemModes[subsystemKey][subsystemKey].updateModeTimeoutFlag(isModeTimeout);
	    }

	    FUNCTION_EXIT;
	}

	unsigned long SubSystemStatus::getSubsystemMode(unsigned long locationKey, unsigned long subsystemKey, ModeSet::subsystemModeType modeType)
	{
		FUNCTION_ENTRY("getSubsystemMode");
		unsigned long modeKey = 0;
		{
			TA_THREADGUARD(m_subsystemModeLock);
	        LocationSubsystemsModeInfo::iterator itFoundLocSubMode = m_subsystemModes.find(locationKey);
			if (m_subsystemModes.end() == itFoundLocSubMode)
			{
				SubsystemsModeInfo temp;
				itFoundLocSubMode = m_subsystemModes.insert(std::make_pair(locationKey,temp)).first;
			}

			SubsystemModeIterator it = itFoundLocSubMode->second.find(subsystemKey);
			if (it == itFoundLocSubMode->second.end())
			{
				ModeSet modeTemp;
				it = itFoundLocSubMode->second.insert(std::make_pair(subsystemKey,modeTemp)).first;
			}

			modeKey = it->second.getMode(modeType);
		}

		FUNCTION_EXIT;
		return modeKey;
	}

	bool SubSystemStatus::getSubsystemModeTimeoutFlag(unsigned long locationKey, unsigned long subsystemKey)
	{
	    FUNCTION_ENTRY("getSubsystemModeTimeoutFlag");
	    bool modeTimeoutFlag= false;
	    {
	        TA_THREADGUARD(m_subsystemModeLock);
	        LocationSubsystemsModeInfo::iterator itFoundLocSubMode = m_subsystemModes.find(locationKey);
	        if (m_subsystemModes.end() == itFoundLocSubMode)
	        {
	            SubsystemsModeInfo temp;
	            itFoundLocSubMode = m_subsystemModes.insert(std::make_pair(locationKey,temp)).first;
	        }

	        SubsystemModeIterator it = itFoundLocSubMode->second.find(subsystemKey);
	        if (it == itFoundLocSubMode->second.end())
	        {
	            ModeSet modeTemp;
	            it = itFoundLocSubMode->second.insert(std::make_pair(subsystemKey,modeTemp)).first;
	        }

	        modeTimeoutFlag = it->second.getModeTimeoutFlag();
	    }

	    FUNCTION_EXIT;
	    return modeTimeoutFlag;
	}

	void SubSystemStatus::registerListener(IsubsystemModeListenerInterface* listener)
	{
		FUNCTION_ENTRY("registerListener");
		if (NULL == m_basManagerDlg)
		{
			m_basManagerDlg = listener;
		}
		FUNCTION_EXIT;
	}
}

