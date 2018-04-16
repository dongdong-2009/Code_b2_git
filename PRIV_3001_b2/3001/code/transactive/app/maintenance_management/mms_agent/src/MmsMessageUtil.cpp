/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/maintenance_management/mms_agent/src/MmsMessageUtil.cpp $
  * @author: christiandw
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * Impliimentation of the MmsMessageUtil class
  */
#pragma warning(disable:4786)
#include "app/maintenance_management/mms_agent/src/MmsMessageUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/entity_access/src/MmsAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/utilities/src/RunParams.h"
#include "bus/maintenance_management/mmsHelper/src/MMSDatabaseCache.h"

namespace TA_IRS_App
{
	//init static instance member
	MmsMessageUtil* MmsMessageUtil::m_instance = 0;
	TA_Base_Core::ReEntrantThreadLockable MmsMessageUtil::m_lock;

	MmsMessageUtil* MmsMessageUtil::getInstance()
	{
		//Thread guard this method
		TA_Base_Core::ThreadGuard guard( m_lock );

		FUNCTION_ENTRY("getInstance");
		
		if(0 == m_instance)
		{
			m_instance = new MmsMessageUtil();
		}

		FUNCTION_EXIT;
		return m_instance;
	}

	/**
	* getAgentData
	*
	* @return the updated data for the agent
	*/
	TA_Base_Core::MmsAgentEntityData* MmsMessageUtil::getAgentData()
	{
		//Thread guard this method
		TA_Base_Core::ThreadGuard guard( m_lock );

		FUNCTION_ENTRY("getAgentData");

		if(NULL == m_agentData)
		{
			std::string AgentEntityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);

			TA_Base_Core::IEntityData* entityData = 
				TA_Base_Core::EntityAccessFactory::getInstance().getEntity(AgentEntityName);

			m_agentData = dynamic_cast < TA_Base_Core::MmsAgentEntityData* > ( entityData );

			//TA_ASSERT(0 != m_agentData, "dynamic_cast < TA_Base_Core::MmsAgentEntityData* > ( entityData ) failed");
			if (NULL == m_agentData)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "dynamic_cast < TA_Base_Core::MmsAgentEntityData* > ( entityData ) failed");
                TA_THROW(TA_Base_Core::TransactiveException("dynamic_cast < TA_Base_Core::MmsAgentEntityData* > ( entityData ) failed"));
			}

		}

		FUNCTION_EXIT;
		return m_agentData;
	}

// 	unsigned long MmsMessageUtil::getNodeKey(unsigned long dataPointKey)
// 	{
// 
// 		//Thread guard this method
// 		TA_Base_Core::ThreadGuard guard( m_lock );
// 
// 		FUNCTION_ENTRY("getNodeKey");
// 
// 		unsigned long nodeKey = 0;
// 		try
// 		{
// 			std::auto_ptr<TA_Base_Core::IEntityData> pointData(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(dataPointKey));
// 			if (NULL != pointData.get())
// 			{
// 				nodeKey = pointData->getParent();
// 			}
// 			else
// 			{
// 				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Can not get the datapoint[%lu] parentKey", dataPointKey);
// 			}
// 		}
// 		catch(const TA_Base_Core::TransactiveException& ex)
// 		{
// 			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
// 		}
// 		catch(...)
// 		{
// 			LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception when datapoint get parentkey");
// 		}
// 		FUNCTION_EXIT;
// 		return nodeKey;
// 	}

	MmsMessageUtil::MmsMessageUtil()
		: m_agentData(0)
	{
		FUNCTION_ENTRY("MmsMessageUtil");

		FUNCTION_EXIT;
	}

	MmsMessageUtil::~MmsMessageUtil()
	{
		FUNCTION_ENTRY("~MmsMessageUtil");

		if(0 != m_instance)
		{
			delete m_instance;
		}
		if(0 != m_agentData)
		{
			delete m_agentData;
		}

		FUNCTION_EXIT;
	}

	std::string MmsMessageUtil::getOperatorName(unsigned long key) 
	{
		return TA_Base_Bus::MMSDatabaseCache::getInstance()->getOperatorName(key);
	}
 
	std::string MmsMessageUtil::getSubsystemName(unsigned long key) 
	{
		return TA_Base_Bus::MMSDatabaseCache::getInstance()->getSubsystemName(key);
	}

	std::string MmsMessageUtil::getAlarmSeverityName(const TA_Base_Core::AlarmDetailCorbaDef& p_alarm)
	{

		//Thread guard this method
		TA_Base_Core::ThreadGuard guard( m_lock );

		FUNCTION_ENTRY("getAlarmSeverityName");

		std::string severity;
		try
		{
			severity = TA_Base_Bus::MMSDatabaseCache::getInstance()->getAlarmSeverityName(p_alarm.alarmSeverity);
		}
		catch(...)
		{
			
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"m_dbCache.getAlarmSeverityName() threw an exception");

			severity = "";
		}

		FUNCTION_EXIT;
		return severity;
	}

} //namespace TA_IRS_App