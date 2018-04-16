#include "StdAfx.h"
#include "core/data_access_interface/entity_access/src/StationBASAgentEntityData.h"
#include "bus/bas/bas_agent_library/src/BasAgentLibraryException.h"
#include "app/bas/TimeTableManager/src/AgentAccessFactory.h"
#include "app/bas/TimeTableManager/src/CommonDefine.h"
#include "app/bas/TimeTableManager/src/TimeTableManagerUtility.h"
#include "app/bas/TimeTableManager/src/TimeTableManagerDataHelper.h"

namespace TA_IRS_App
{

	#define EntityParameterName_Today_TimeTable_ModeCode_Token "CurrentDayTimeTableModeCodeFeedbackToken"
	#define EntityParameterName_Today_TimeTable_BeginTime_Token "CurrentDayTimeTableBeginTimeFeedbackToken"
	#define EntityParameterName_Tomorrow_TimeTable_ModeCode_Token "NextDayTimeTableModeCodeFeedbackToken"
	#define EntityParameterName_Tomorrow_TimeTable_BeginTime_Token "NextDayTimeTableBeginTimeFeedbackToken"

	AgentAccessFactory* AgentAccessFactory::m_AgentAccessFactoryInstance = NULL;
	TA_Base_Core::NonReEntrantThreadLockable AgentAccessFactory::m_instanceLock;
	AgentAccessFactory::AgentAccessFactory(void)
	:m_strBasAgentEntityName("")
	{
		FUNCTION_ENTRY("AgentAccessFactory");
		FUNCTION_EXIT;
	}

	AgentAccessFactory::~AgentAccessFactory(void)
	{
		FUNCTION_ENTRY("~AgentAccessFactory");
		FUNCTION_EXIT;
	}

	AgentAccessFactory& AgentAccessFactory::getInstance()
	{
		FUNCTION_ENTRY("getInstance");
		TA_THREADGUARD(m_instanceLock);
		if (NULL == m_AgentAccessFactoryInstance)
		{
			m_AgentAccessFactoryInstance = new AgentAccessFactory;
		}
		FUNCTION_EXIT;
		return *m_AgentAccessFactoryInstance;
	}

	std::string AgentAccessFactory::getAgentEntityName(unsigned long locationKey)
	{
		FUNCTION_ENTRY("getAgentEntityName");

		TA_Base_Core::IEntityDataList agentList;
		getEntityDataList( locationKey, false, agentList);
		std::string strEntityName = "";
		if (agentList.size() > 0)
		{
			strEntityName = agentList[0]->getName();
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "There not find the Agent Entity of locatoin : %d", locationKey);
		}
		
		// delete the data avoid the memory leak
		TA_Base_Core::IEntityDataList::iterator it;
		for (it = agentList.begin(); it != agentList.end(); it++)
		{
			delete (*it);
			(*it) = NULL;
		}

		if (strEntityName.empty())
		{
            TA_THROW(TA_IRS_Bus::BasNotDeployStationException("Can not found the Agent Entity Data"));
		}
		FUNCTION_EXIT;
		return strEntityName;
	}

	void AgentAccessFactory::getEntityDataList(unsigned long locationKey, bool loadParameter, TA_Base_Core::IEntityDataList& dataList)
	{
		FUNCTION_ENTRY("getEntityDataList");
		using namespace TA_Base_Core;
		// get the location key and stationBasAgent type name
		// prepare to get the StationBASAgent entity name
		//unsigned long locationKey = TimeTableManagerUtility::stringToUnsignedLong(RunParams::getInstance().get(RPARAM_LOCATIONKEY));
		std::string strStationBasAgentTypeName = TA_Base_Core::StationBASAgentEntityData::getStaticType();
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
			"create BasAgent using locationKey=%d, BasAgentTypeName=%s",
			locationKey, strStationBasAgentTypeName.c_str());

		// get the StationBASAgent entityName
		dataList =	TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(strStationBasAgentTypeName, locationKey, loadParameter);
		FUNCTION_EXIT;
	}

	BASAgentObject& AgentAccessFactory::getDefaultBasAgent()
	{
		FUNCTION_ENTRY("getDefaultBasAgent");
		using namespace TA_Base_Core;
		unsigned long locationKey = TimeTableManagerUtility::stringToUnsignedLong(RunParams::getInstance().get(RPARAM_LOCATIONKEY));
		FUNCTION_EXIT;
		return getBasAgentByLocation(locationKey);
	}

	BASAgentObject& AgentAccessFactory::getBasAgentByLocation(unsigned long locationKey)
	{
		FUNCTION_ENTRY("getBasAgentByLocation");
		BasAgentMapIterator it;
		{
			TA_THREADGUARD(m_agentMapLock);
			it = m_basAgentMap.find(locationKey);
			if (it != m_basAgentMap.end())
			{
				return *(it->second);
			}
		}
		
		std::string strAgentEntityName = getAgentEntityName( locationKey );
		BASAgentObject* agentPtr = new BASAgentObject(strAgentEntityName, true);
		if (NULL != agentPtr)
		{
			TA_THREADGUARD(m_agentMapLock);
			m_basAgentMap[locationKey] = agentPtr;
		} 
		else
		{
			std::string strError = "can not get the BASAgent with entityname:";
			strError += strAgentEntityName;
            TA_THROW(TA_IRS_Bus::BasNotDeployStationException(strError));
		}
		
		FUNCTION_EXIT;
		return *agentPtr;
	}

	void AgentAccessFactory::getTimeTableDatapointNamesByLocation(unsigned long locationKey, TimeTableDatapointNameInfos& datapointsName)
	{
		FUNCTION_ENTRY("getTimeTableDatapointNamesByLocation");

		DataPointNameMap::iterator itFound = m_datapointsNames.find(locationKey);
		if (m_datapointsNames.end() == itFound)
		{
			std::string strEntityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
			ILocation_SmartPtr ptrLocal = TimeTableManagerDataHelperInstance::instance()->getLocationByKey(locationKey);
			if (NULL == ptrLocal.get())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot get this location information. locationKey=%d",locationKey);
				return;
			}

			std::string strLocationName = ptrLocal->getName();
			datapointsName.clear();
			std::string CurrentDayTimeTableModeCodeFeedbackToken = getEntityParameter(strEntityName, EntityParameterName_Today_TimeTable_ModeCode_Token);
			std::string CurrentDayTimeTableBeginTimeFeedbackToken = getEntityParameter(strEntityName, EntityParameterName_Today_TimeTable_BeginTime_Token);
			std::string NextDayTimeTableModeCodeFeedbackToken = getEntityParameter(strEntityName,EntityParameterName_Tomorrow_TimeTable_ModeCode_Token);
			std::string NextDayTimeTableBeginTimeFeedbackToken = getEntityParameter(strEntityName,EntityParameterName_Tomorrow_TimeTable_BeginTime_Token);
			for (int i = 1; i <= MaxModesSendPerLocation; i++)
			{
				TimeTableDatapointNameInfo datapointName;
				datapointName.todayTimeTableModeCodeFeedback = strLocationName + CurrentDayTimeTableModeCodeFeedbackToken + TimeTableManagerUtility::unsignedLongToString(i);
				datapointName.todayTimeTableBeginTimeFeedback = strLocationName + CurrentDayTimeTableBeginTimeFeedbackToken + TimeTableManagerUtility::unsignedLongToString(i);
				datapointName.tomorrowTimeTableModeCodeFeedback = strLocationName + NextDayTimeTableModeCodeFeedbackToken + TimeTableManagerUtility::unsignedLongToString(i);
				datapointName.tomorrowTimeTableBeginTimeFeedback = strLocationName + NextDayTimeTableBeginTimeFeedbackToken + TimeTableManagerUtility::unsignedLongToString(i);
				datapointsName.push_back(datapointName);
			}
			m_datapointsNames.insert(std::make_pair(locationKey, datapointsName));
			return;
		}

		datapointsName = itFound->second;
		FUNCTION_EXIT;
	}



	std::string AgentAccessFactory::getEntityParameter(const std::string& strEntityName, const std::string& strParameterName)
	{
		FUNCTION_ENTRY("getEntityParameter");
		std::string strParameterValue = "";
		if(strEntityName.empty())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "the entity name is empty");
			return strParameterValue;
		}
		if(strParameterName.empty())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "the parameter name is empty");
			return strParameterValue;
		}

		TA_Base_Core::IData* data = TA_Base_Core::EntityAccessFactory::getInstance().getParameter(strEntityName, strParameterName);
		if(NULL == data)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "the entity data is NULL");
			return strParameterValue;
		}

		if(data->getNumRows() != 1)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The entity this parameter is not only one");
			return strParameterValue;
		}
		strParameterValue = data->getStringData(0, "VALUE");
		
		delete data;
		data = NULL;
		FUNCTION_EXIT;
		return strParameterValue;
	}
}

