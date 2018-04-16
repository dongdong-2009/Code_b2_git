/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ats/AtsComBridge/src/AtsSoundManager.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */

// AtsSoundManager.cpp : Implementation of CAtsComBridgeApp and DLL registration.

#include "stdafx.h"
#include "AtsComBridge.h"
#include "AtsSoundManager.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/exceptions/src/SoundPlayerException.h"

#include "bus/sound/sound_client/src/SoundManagerProxy.h"

/////////////////////////////////////////////////////////////////////////////
//
const std::string ATS_ALARM_CATEGORY = "AtsAlarm";
const std::string ISCS_ALARM_CATEGORY = "Alarm";

AtsSoundManager::AtsSoundManager() : m_soundInterface(NULL)
{
	initialiseDebugUtil();
	initialiseCorba(0);
	TA_Base_Core::RunParams::getInstance().set(RPARAM_OPERATIONMODE, RPARAM_CONTROL);

	initSoundInterface();
}

AtsSoundManager::~AtsSoundManager()
{
	delete m_soundInterface;
	m_soundInterface = NULL;

	cleanupCorba();
}

STDMETHODIMP AtsSoundManager::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IAtsSoundManager,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

void AtsSoundManager::initSoundInterface()
{
	if (m_soundInterface == NULL)
	{
		try
		{
			m_soundInterface = new TA_Base_Bus::SoundManagerProxy(this);
		}
		catch(TA_Base_Core::SoundPlayerException& ex)
		{
			std::string error = "Unable to initialise SoundManager: ";
			error += ex.what();
			LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", error.c_str());
		}
	}

	if (m_soundInterface != NULL)
	{
		try
		{
			m_soundInterface->joinCategories(ISCS_ALARM_CATEGORY, ATS_ALARM_CATEGORY);
		}
		catch(TA_Base_Core::SoundPlayerException& ex)
		{
			std::string error = "Unable to initialise SoundManager: ";
			error += ex.what();
			LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", error.c_str());
		}
	}
}

STDMETHODIMP AtsSoundManager::setAlarmSeverity(long alarmSeverity)
{
	const int NUM_SEVERITIES = 4;

	// Need to convert severities (1 highest, descending order) into priorities 
	// (1 lowest, ascending order).
	long alarmPriority = 0;
	if (alarmSeverity != 0)
	{
		alarmPriority = NUM_SEVERITIES + 1 - alarmSeverity;
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "setAlarmSeverity() - set severity to %d, priority is %d", 
		        alarmSeverity, alarmPriority );

	initSoundInterface();

	if (m_soundInterface != NULL)
	{
		try
		{
			m_soundInterface->setPriority(ATS_ALARM_CATEGORY, alarmPriority);
		}
		catch(TA_Base_Core::SoundPlayerException& ex)
		{
			std::string error = "Error setting priority on SoundManager: ";
			error += ex.what();
			LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", error.c_str());
		}
	}

	return S_OK;
}

void AtsSoundManager::handleCategorySilenced(const std::string& category, bool isSilenced)
{
}


void AtsSoundManager::handleAllSilenced(bool isSilenced)
{
}


void AtsSoundManager::initialiseDebugUtil()
{
    // Set the debug file name
    std::string param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGFILE);
    if (!param.empty())
    {
        TA_Base_Core::DebugUtil::getInstance().setFile(param.c_str());
    }
	else
	{
		TA_Base_Core::DebugUtil::getInstance().setFile("c:/transactive/logs/Log_AtsComBridge.log");
	}

    // Set the debug level
    param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVEL);
    if (!param.empty())
    {
        TA_Base_Core::DebugUtil::getInstance().setLevel(TA_Base_Core::DebugUtil::getDebugLevelFromString(param.c_str()));
    }
	else
	{
		TA_Base_Core::DebugUtil::getInstance().setLevel(TA_Base_Core::DebugUtil::getDebugLevelFromString("INFO"));
	}

    // Set the maximum size of a debug file in bytes
    param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGFILEMAXSIZE);
    if (!param.empty())
    {
        TA_Base_Core::DebugUtil::getInstance().setMaxSize(atoi(param.c_str()));
    }

    // Set the maximum number of log files
    param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGMAXFILES);
    if (!param.empty())
    {
        TA_Base_Core::DebugUtil::getInstance().setMaxFiles(atoi(param.c_str()));
    }
}


void AtsSoundManager::initialiseCorba(int portNum)
{
	int port = portNum;

	// If a different port is set we should use that
	if ( TA_Base_Core::RunParams::getInstance().isSet( "PortNumber" ) )
	{
		port = atoi( TA_Base_Core::RunParams::getInstance().get( "PortNumber" ).c_str());
	}

	// Initialise on the default, or specified, port
    if ( false == TA_Base_Core::CorbaUtil::getInstance().initialise( port ) )
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to initialise corba");
		
		// Kill initialisation of DLL
		throw;
    }

	try
	{
		TA_Base_Core::CorbaUtil::getInstance().activate();
	}
	catch( ... )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to activate corba" );
		
		// Kill initialisation of DLL
		throw;
	}
}

void AtsSoundManager::cleanupCorba()
{
	TA_Base_Core::CorbaUtil::getInstance().shutdown();

	TA_Base_Core::CorbaUtil::cleanup();
}