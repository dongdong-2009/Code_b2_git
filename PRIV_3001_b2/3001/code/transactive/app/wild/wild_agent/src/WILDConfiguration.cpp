/* 
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/PRIV_3001/3001/transactive/app/wild/wild_agent/src/WILDConfiguration.cpp $
 * @author:  <Andy Siow>
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 * 
 * Implementation file
 */

#include "app/wild/wild_agent/src/WILDConfiguration.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_IRS_App
{
	WILDConfiguration* WILDConfiguration::m_instance = 0;
	TA_Base_Core::NonReEntrantThreadLockable WILDConfiguration::m_configLock;

	WILDConfiguration::WILDConfiguration()
		: m_agentKey(0),
		m_agentLocationKey(0),
		m_agentName(""),
		m_agentAssetName(""),
		m_agentLocationName(""),
		m_agentSubsystemName(""),
		m_agentSubsystemKey(0),
		m_agentTypeKey(0),
		m_correlationThreshold(0),
		m_agentATSWILDQueueSize(0),
		m_correlationTimeoutSecs(30),
		m_wildDetailsToken("")
	{

	}


	WILDConfiguration::~WILDConfiguration()
	{

	}

	
	WILDConfiguration* WILDConfiguration::getInstance()
	{
		if ( 0 == m_instance )
		{
			// Double checking to prevent multiple threads
			// creating multiple instances.

			// Any lock is okay at this stage.
			TA_Base_Core::ThreadGuard guard( m_configLock );

			if ( 0 == m_instance )
			{
				m_instance = new WILDConfiguration();
    		}
		}

		return m_instance;
	}


	void WILDConfiguration::removeInstance()
	{
		// 
		// Guard this to prevent multiple threads atempting
		// to delete/create simultaneously
		//
		TA_Base_Core::ThreadGuard guard( m_configLock );  // Any lock will do
		if ( m_instance != 0 )
		{
			delete m_instance;
			m_instance = 0;
		}
	}


	void WILDConfiguration::setAgentKey(unsigned long param)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "setAgentKey");

		TA_Base_Core::ThreadGuard guard( m_configLock );
		m_agentKey = param;

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "setAgentKey");
	}


	unsigned long WILDConfiguration::getAgentKey() const
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "getAgentKey");

		TA_Base_Core::ThreadGuard guard( m_configLock );
		return m_agentKey;

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "getAgentKey");
	}


	void WILDConfiguration::setAgentLocationKey(unsigned long param)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "setAgentLocationKey");

		TA_Base_Core::ThreadGuard guard( m_configLock );
		m_agentLocationKey = param;

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "setAgentLocationKey");
	}


	unsigned long WILDConfiguration::getAgentLocationKey() const
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "getAgentLocationKey");

		TA_Base_Core::ThreadGuard guard( m_configLock );
		return m_agentLocationKey;

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "getAgentLocationKey");
	}


	void WILDConfiguration::setAgentName(const std::string& param)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "setAgentName");

		TA_Base_Core::ThreadGuard guard( m_configLock );
		m_agentName = param;

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "setAgentName");
	}


	std::string WILDConfiguration::getAgentName() const
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "getAgentName");

		TA_Base_Core::ThreadGuard guard( m_configLock );
		return m_agentName;

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "getAgentName");
	}


	void WILDConfiguration::setAgentAssetName(const std::string& param)
	{
		FUNCTION_ENTRY("setAgentAssetName");

		TA_Base_Core::ThreadGuard guard( m_configLock );
		m_agentAssetName = param;

		FUNCTION_EXIT;
	}


	std::string WILDConfiguration::getAgentAssetName() const
	{
		FUNCTION_ENTRY("getAgentAssetName");

		TA_Base_Core::ThreadGuard guard( m_configLock );
		return m_agentAssetName;

		FUNCTION_EXIT;
	}

	void WILDConfiguration::setAgentSubsystemKey(unsigned long param)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "setAgentSubsystemKey");

		TA_Base_Core::ThreadGuard guard( m_configLock );
		m_agentSubsystemKey = param;

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "setAgentSubsystemKey");
	}


	unsigned long WILDConfiguration::getAgentSubsystemKey() const 
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "getAgentSubsystemKey");

		TA_Base_Core::ThreadGuard guard( m_configLock );
		return m_agentSubsystemKey;

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "getAgentSubsystemKey");
	}


	void WILDConfiguration::setAgentTypeKey(unsigned long param)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "setAgentTypeKey");

		TA_Base_Core::ThreadGuard guard( m_configLock );
		m_agentTypeKey = param;

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "setAgentTypeKey");
	}


	unsigned long WILDConfiguration::getAgentTypeKey() const
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "getAgentTypeKey");

		TA_Base_Core::ThreadGuard guard( m_configLock );
		return m_agentTypeKey;

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "getAgentTypeKey");
	}


	void WILDConfiguration::setCorrelationThreshold(unsigned long param)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "setCorrelationThreshold");

		TA_Base_Core::ThreadGuard guard( m_configLock );
		m_correlationThreshold = param;

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "setCorrelationThreshold");
	}


	unsigned long WILDConfiguration::getCorrelationThreshold() const
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "getCorrelationThreshold");

		TA_Base_Core::ThreadGuard guard( m_configLock );
		return m_correlationThreshold;

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "getCorrelationThreshold");
	}


	void WILDConfiguration::setAgentLocationName(const std::string& param)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "setAgentLocationName");

		TA_Base_Core::ThreadGuard guard( m_configLock );
		m_agentLocationName = param;

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "setAgentLocationName");
	}


	std::string WILDConfiguration::getAgentLocationName() const
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "getAgentLocationName");

		TA_Base_Core::ThreadGuard guard( m_configLock );
		return m_agentLocationName;

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "getAgentLocationName");
	}


	void WILDConfiguration::setAgentSubsystemName(const std::string& param)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "getAgentKey");

		TA_Base_Core::ThreadGuard guard( m_configLock );
		m_agentSubsystemName = param;

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "setAgentSubsystemName");
	}


	std::string WILDConfiguration::getAgentSubsystemName() const
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "getAgentSubsystemName");

		TA_Base_Core::ThreadGuard guard( m_configLock );
		return m_agentSubsystemName;

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "getAgentSubsystemName");
	}


	void WILDConfiguration::setAgentATSWILDQueueSize(unsigned int param)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "setAgentATSWILDQueueSize");

		TA_Base_Core::ThreadGuard guard( m_configLock );
		m_agentATSWILDQueueSize = param;

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "setAgentATSWILDQueueSize");
	}


	unsigned int WILDConfiguration::getAgentATSWILDQueueSize() const
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "getAgentATSWILDQueueSize");

		TA_Base_Core::ThreadGuard guard( m_configLock );
		return m_agentATSWILDQueueSize;

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "getAgentATSWILDQueueSize");
	}
	
	void WILDConfiguration::setCorrelationThresholdTimeoutSecs(long param)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "setAgentATSWILDQueueSize");

		TA_Base_Core::ThreadGuard guard( m_configLock );
		m_correlationTimeoutSecs = param;

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "setAgentATSWILDQueueSize");
	}


	long WILDConfiguration::getCorrelationThresholdTimeoutSecs() const
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "getAgentATSWILDQueueSize");

		TA_Base_Core::ThreadGuard guard( m_configLock );
		return m_correlationTimeoutSecs;

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "getAgentATSWILDQueueSize");
	}


	void WILDConfiguration::setWILDDetailsToken(const std::string& param) 
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "setWILDDetailsToken");

		TA_Base_Core::ThreadGuard guard( m_configLock );
		m_wildDetailsToken = param;

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "setWILDDetailsToken");
	}


	std::string WILDConfiguration::getWILDDetailsToken() const
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "getWILDDetailsToken");

		TA_Base_Core::ThreadGuard guard( m_configLock );
		return m_wildDetailsToken;

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "getWILDDetailsToken");
	}

} // TA_App
