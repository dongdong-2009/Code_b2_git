#include "StdAfx.h"
#include "app/bas/BASModeManager/src/BASAgentHelper.h"
#include "bus/bas/bas_agent_library/src/BasAgentLibraryException.h"
#include "core/data_access_interface/entity_access/src/StationBASAgentEntityData.h"
#include "core/data_access_interface/src/IData.h"

Namespace_TA_IRS_APP_Begin

BASAgentHelper::BASAgentHelper(unsigned long locationKey)
:m_strBasAgentEntityName("")
,m_strCurrentExcuteModeDataPointTokenName("")
,m_strCurrentExcuteModeBitDataPointTokenName("")
,m_strCurrentExcuteModeStatusDataPointTokenName("")
,m_BasAgentEntityKey(0)
,m_BasAgentEntityTypeKey(0)
,m_BasAgentSubsystemKey(0)
,m_locationKey(locationKey)
{
	FUNCTION_ENTRY("BASAgentHelper");
	using namespace TA_Base_Core;
	std::string entityName = getAgentEntityName();
	m_BasAgentPtr = std::auto_ptr<BASAgentObject>(new BASAgentObject(entityName, true));
	FUNCTION_EXIT;
}

BASAgentHelper::~BASAgentHelper(void)
{
	FUNCTION_ENTRY("~BASAgentHelper");
	FUNCTION_EXIT;
}

void BASAgentHelper::getEntityDataList(bool loadParameter, TA_Base_Core::IEntityDataList& dataList)
{
	FUNCTION_ENTRY("getEntityDataList");
	using namespace TA_Base_Core;
	// get the location key and stationBasAgent type name
	// prepare to get the StationBASAgent entity name
	unsigned long locationKey = m_locationKey;
	std::string strStationBasAgentTypeName = TA_Base_Core::StationBASAgentEntityData::getStaticType();
	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
		"create BasAgent using locationKey=%d, BasAgentTypeName=%s",
		locationKey, strStationBasAgentTypeName.c_str());

	// get the StationBASAgent entityName
	dataList =	TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(strStationBasAgentTypeName, locationKey, loadParameter);
	FUNCTION_EXIT;
}

std::string BASAgentHelper::getEntityParameterValue(const std::string strParameterName)
{
	FUNCTION_ENTRY("getEntityParameterValue");
	using namespace TA_Base_Core;
	bool bInvalid = true;
	if (StationBASAgentEntityData::MY_MODE_VALUE_SQL_TOKEN == strParameterName)
	{
		if (false == m_strCurrentExcuteModeDataPointTokenName.empty())
		{
			bInvalid = false;
		}
	}
	else if (StationBASAgentEntityData::MY_MODE_RIGHT_STATUS_TOKEN == strParameterName)
	{
		if (false == m_strCurrentExcuteModeBitDataPointTokenName.empty())
		{
			bInvalid = false;
		}
	}
	else if (StationBASAgentEntityData::MY_MODE_EXEC_STATUS_TOKEN == strParameterName)
	{
		if (false == m_strCurrentExcuteModeStatusDataPointTokenName.empty())
		{
			bInvalid = false;
		}
	}
	
	std::string strValue = "";
	if (bInvalid)
	{
		std::string strEntityName = getAgentEntityName();
		TA_Base_Core::IData* data = TA_Base_Core::EntityAccessFactory::getInstance().getParameter(strEntityName, strParameterName);
		//TA_ASSERT(NULL != data, "can not find the BASAgent entity object or more Entity data in this location");
		if( NULL == data)
		{
            TA_THROW(TA_IRS_Bus::BasNotDeployStationException(m_strBasAgentEntityName + " - not deploy, please contact administrator"));
		}
		strValue = data->getStringData(0, "VALUE"); // "VALUE" is the entityparametervalue table column name
		delete data;
		data = NULL;
	}

	if (StationBASAgentEntityData::MY_MODE_VALUE_SQL_TOKEN == strParameterName)
	{
		if (false == m_strCurrentExcuteModeDataPointTokenName.empty())
		{
			 m_strCurrentExcuteModeDataPointTokenName = strValue;
		}
	}
	else if (StationBASAgentEntityData::MY_MODE_RIGHT_STATUS_TOKEN == strParameterName)
	{
		if (false == m_strCurrentExcuteModeBitDataPointTokenName.empty())
		{
			 m_strCurrentExcuteModeBitDataPointTokenName = strValue;
		}
	}
	else if (StationBASAgentEntityData::MY_MODE_EXEC_STATUS_TOKEN == strParameterName)
	{
		if (false == m_strCurrentExcuteModeStatusDataPointTokenName.empty())
		{
			 m_strCurrentExcuteModeStatusDataPointTokenName = strValue;
		}
	}
	FUNCTION_EXIT;
	return strValue;
}
std::string BASAgentHelper::getAgentEntityName()
{
	FUNCTION_ENTRY("getAgentEntityName");
	if (false != m_strBasAgentEntityName.empty())
	{
		TA_Base_Core::IEntityDataList agentList;
		getEntityDataList(false, agentList);
		//TA_ASSERT(1 == agentList.size(), "can not find the BASAgent entity object or more Entity data in this location");
		if( 1 != agentList.size())
		{
            TA_THROW(TA_IRS_Bus::BasNotDeployStationException(m_strBasAgentEntityName + " - not deploy, please contact administrator"));
		}
		m_strBasAgentEntityName = agentList[0]->getName();
	}
	FUNCTION_EXIT;
	return m_strBasAgentEntityName;
}

unsigned long BASAgentHelper::getAgentEntityKey()
{
	FUNCTION_ENTRY("getAgentEntityKey");
	if (0 == m_BasAgentEntityKey)
	{
		TA_Base_Core::IEntityDataList agentList;
		getEntityDataList(false, agentList);
		//TA_ASSERT(1 == agentList.size(), "can not find the BASAgent entity object or more Entity data in this location");
		if( 1 != agentList.size())
		{
            TA_THROW(TA_IRS_Bus::BasNotDeployStationException(m_strBasAgentEntityName + " - not deploy, please contact administrator"));
		}
		m_BasAgentEntityKey = agentList[0]->getKey();
	}
	FUNCTION_EXIT;
	return m_BasAgentEntityKey;
}

unsigned long BASAgentHelper::getAgentEntityTypeKey()
{
	FUNCTION_ENTRY("getAgentEntityTypeKey");
	if (0 == m_BasAgentEntityTypeKey)
	{
		TA_Base_Core::IEntityDataList agentList;
		getEntityDataList(false, agentList);
		//TA_ASSERT(1 == agentList.size(), "can not find the BASAgent entity object or more Entity data in this location");
		if( 1 != agentList.size())
		{
            TA_THROW(TA_IRS_Bus::BasNotDeployStationException(m_strBasAgentEntityName + " - not deploy, please contact administrator"));
		}
		m_BasAgentEntityTypeKey = agentList[0]->getTypeKey();
	}
	FUNCTION_EXIT;
	return m_BasAgentEntityTypeKey;
}

unsigned long BASAgentHelper::getAgentEntitySubsystemKey()
{
	FUNCTION_ENTRY("getAgentEntitySubsystemKey");
	if (0 == m_BasAgentSubsystemKey)
	{
		TA_Base_Core::IEntityDataList agentList;
		getEntityDataList(false, agentList);
		//TA_ASSERT(1 == agentList.size(), "can not find the BASAgent entity object or more Entity data in this location");
		if( 1 != agentList.size())
		{
            TA_THROW(TA_IRS_Bus::BasNotDeployStationException(m_strBasAgentEntityName + " - not deploy, please contact administrator"));
		}
		m_BasAgentSubsystemKey = agentList[0]->getSubsystem();
	}
	FUNCTION_EXIT;
	return m_BasAgentSubsystemKey;
}

std::string BASAgentHelper::getStationName()
{
	FUNCTION_ENTRY("getStationName");
	if (false != m_strStationName.empty())
	{
		m_strStationName = getEntityParameterValue(TA_Base_Core::StationBASAgentEntityData::STATIONNAME);
		if (m_strStationName.empty())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[No Data]No Found the parameter value, parameter name :%s",TA_Base_Core::StationBASAgentEntityData::STATIONNAME.c_str());
		}
	}
	FUNCTION_EXIT;
	return m_strStationName;
}
std::string BASAgentHelper::getCurrentExcuteModeDataPointName(TA_IRS_Core::SubSystem_SmartPtr& subsystemPtr)
{
	FUNCTION_ENTRY("getCurrentExcuteModeDataPointName");
	using namespace TA_Base_Core;
	
	unsigned long subsystemKey = subsystemPtr->getKey();
	DataPointNameMapIterator it = m_CurrentExcuteModeDataPointName.find(subsystemKey);

	if (it == m_CurrentExcuteModeDataPointName.end())
	{
		std::string strLocationName = getStationName();
		std::string strSubsystemAsserName = subsystemPtr->getAssetName();
		std::string strCurrentExcuteModeName = getEntityParameterValue(TA_Base_Core::StationBASAgentEntityData::MY_MODE_VALUE_SQL_TOKEN);
		if (strCurrentExcuteModeName.empty())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[No Data]No Found the parameter value, parameter name :%s",TA_Base_Core::StationBASAgentEntityData::MY_MODE_VALUE_SQL_TOKEN.c_str());
		}

		m_CurrentExcuteModeDataPointName[subsystemKey] = 
			strLocationName + strSubsystemAsserName + strCurrentExcuteModeName;
	}
	FUNCTION_EXIT;
	return m_CurrentExcuteModeDataPointName[subsystemKey];
}

std::string BASAgentHelper::getCurrentExcuteModeBitDataPointName(TA_IRS_Core::SubSystem_SmartPtr& subsystemPtr)
{
	FUNCTION_ENTRY("getCurrentExcuteModeBitDataPointName");
	using namespace TA_Base_Core;
	
	unsigned long subsystemKey = subsystemPtr->getKey();
	DataPointNameMapIterator it = m_CurrentExcuteModeBitDataPointName.find(subsystemKey);

	if (it == m_CurrentExcuteModeBitDataPointName.end())
	{
		std::string strLocationName = getStationName();
		std::string strSubsystemAsserName = subsystemPtr->getAssetName();
		std::string strCurrentModeBitName = getEntityParameterValue(TA_Base_Core::StationBASAgentEntityData::MY_MODE_RIGHT_STATUS_TOKEN);
		if (strCurrentModeBitName.empty())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[No Data]No Found the parameter value, parameter name :%s",TA_Base_Core::StationBASAgentEntityData::MY_MODE_RIGHT_STATUS_TOKEN.c_str());
		}

		m_CurrentExcuteModeBitDataPointName[subsystemKey] = 
			strLocationName + strSubsystemAsserName + strCurrentModeBitName;
	}
	FUNCTION_EXIT;
	return m_CurrentExcuteModeBitDataPointName[subsystemKey];
}


std::string BASAgentHelper::getCurrentExcuteModeBitSetDataPointName(TA_IRS_Core::SubSystem_SmartPtr& subsystemPtr)
{
	FUNCTION_ENTRY("getCurrentExcuteModeBitDataPointName");
	using namespace TA_Base_Core;

	unsigned long subsystemKey = subsystemPtr->getKey();
	DataPointNameMapIterator it = m_CurrentExcuteModeBitSetDataPointName.find(subsystemKey);

	if (it == m_CurrentExcuteModeBitSetDataPointName.end())
	{
		std::string strLocationName = getStationName();
		std::string strSubsystemAsserName = subsystemPtr->getAssetName();
		std::string strCurrentModeBitName = getEntityParameterValue(TA_Base_Core::StationBASAgentEntityData::MY_MODE_RIGHT_CONTROL_TOKEN);
		if (strCurrentModeBitName.empty())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[No Data]No Found the parameter value, parameter name :%s",TA_Base_Core::StationBASAgentEntityData::MY_MODE_RIGHT_CONTROL_TOKEN.c_str());
		}

		m_CurrentExcuteModeBitSetDataPointName[subsystemKey] = 
			strLocationName + strSubsystemAsserName + strCurrentModeBitName;
	}
	FUNCTION_EXIT;
	return m_CurrentExcuteModeBitSetDataPointName[subsystemKey];
}

std::string BASAgentHelper::getCurrentExcuteModeStatusDataPointName(TA_IRS_Core::SubSystem_SmartPtr& subsystemPtr)
{
	FUNCTION_ENTRY("getCurrentExcuteModeStatusDataPointName");
	using namespace TA_Base_Core;

	unsigned long subsystemKey = subsystemPtr->getKey();
	DataPointNameMapIterator it = m_CurrentExcuteModeStatusDataPointName.find(subsystemKey);

	if (it == m_CurrentExcuteModeStatusDataPointName.end())
	{
		std::string strLocationName = getStationName();
		std::string strSubsystemAsserName = subsystemPtr->getAssetName();
		std::string strCurrentModeStatusName = getEntityParameterValue(TA_Base_Core::StationBASAgentEntityData::MY_MODE_EXEC_STATUS_TOKEN);
		if (strCurrentModeStatusName.empty())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[No Data]No Found the parameter value, parameter name :%s",TA_Base_Core::StationBASAgentEntityData::MY_MODE_EXEC_STATUS_TOKEN.c_str());
		}

		m_CurrentExcuteModeStatusDataPointName[subsystemKey] = 
			strLocationName + strSubsystemAsserName + strCurrentModeStatusName;
	}
	FUNCTION_EXIT;
	return m_CurrentExcuteModeStatusDataPointName[subsystemKey];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned long BasAgentHelperMap::getAgentEntityKey(unsigned long locationKey)
{
	return getBasAgentHelperByLocationKey(locationKey)->getAgentEntityKey();
}

std::string BasAgentHelperMap::getAgentEntityName(unsigned long locationKey)
{
	return getBasAgentHelperByLocationKey(locationKey)->getAgentEntityName();
}

unsigned long BasAgentHelperMap::getAgentEntityTypeKey(unsigned long locationKey)
{
	return getBasAgentHelperByLocationKey(locationKey)->getAgentEntityTypeKey();
}

unsigned long BasAgentHelperMap::getAgentEntitySubsystemKey(unsigned long locationKey)
{
	return getBasAgentHelperByLocationKey(locationKey)->getAgentEntitySubsystemKey();
}

std::string BasAgentHelperMap::getStationName(unsigned long locationKey)
{
	return getBasAgentHelperByLocationKey(locationKey)->getStationName();
}

std::string BasAgentHelperMap::getCurrentExcuteModeDataPointName(unsigned long locationKey, TA_IRS_Core::SubSystem_SmartPtr& subsystemPtr)
{
	return getBasAgentHelperByLocationKey(locationKey)->getCurrentExcuteModeDataPointName(subsystemPtr);
}

std::string BasAgentHelperMap::getCurrentExcuteModeBitDataPointName(unsigned long locationKey, TA_IRS_Core::SubSystem_SmartPtr& subsystemPtr)
{
	return getBasAgentHelperByLocationKey(locationKey)->getCurrentExcuteModeBitDataPointName(subsystemPtr);
}

std::string BasAgentHelperMap::getCurrentExcuteModeBitSetDataPointName(unsigned long locationKey, TA_IRS_Core::SubSystem_SmartPtr& subsystemPtr)
{
	return getBasAgentHelperByLocationKey(locationKey)->getCurrentExcuteModeBitSetDataPointName(subsystemPtr);
}

std::string BasAgentHelperMap::getCurrentExcuteModeStatusDataPointName(unsigned long locationKey, TA_IRS_Core::SubSystem_SmartPtr& subsystemPtr)
{
	return getBasAgentHelperByLocationKey(locationKey)->getCurrentExcuteModeStatusDataPointName(subsystemPtr);
}

BASAgentHelper_SmartPtr BasAgentHelperMap::getBasAgentHelperByLocationKey(unsigned long locationKey)
{
	if (m_basAgentHelperMap.end() == m_basAgentHelperMap.find(locationKey))
	{
		BASAgentHelper_SmartPtr helper = BASAgentHelper_SmartPtr(new BASAgentHelper(locationKey));
		m_basAgentHelperMap[locationKey] = helper;
	}
	return m_basAgentHelperMap[locationKey];
}
Namespace_TA_IRS_APP_End
