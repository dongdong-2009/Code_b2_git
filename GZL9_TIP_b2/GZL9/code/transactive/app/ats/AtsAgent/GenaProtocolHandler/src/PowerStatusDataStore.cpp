
#include "app/ats/AtsAgent/GenaProtocolHandler/src/PowerStatusDataStore.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaAtsDefinitions.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TA_String.h"
#include "bus/ats/ats_agent/CommonATS/src/DataCache.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/protocol/Gena/src/GENALibrary.h"

namespace TA_IRS_App
{

	PowerStatusDataStore::PowerStatusDataStore()
	{
		FUNCTION_ENTRY("PowerStatusDataStore");

		m_powerStatusMap.clear ();
		m_GenaPowerDataList.clear ();

		FUNCTION_EXIT;

	}

	PowerStatusDataStore::~PowerStatusDataStore()
	{
		FUNCTION_ENTRY("~PowerStatusDataStore");

		m_powerStatusMap.clear ();
		m_GenaPowerDataList.clear ();

		FUNCTION_EXIT;

	}

	void PowerStatusDataStore::updateDataImpl(const std::string& topicName, TA_Base_Bus::GENATopicDataPtr pData)
	{
		FUNCTION_ENTRY("updateDataImpl");

		FUNCTION_EXIT;
	}


	TA_Base_Bus::T_GENATopicDataList PowerStatusDataStore::getAllData(const std::string& topicName )
	{
		FUNCTION_ENTRY("getAllData");
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Get all Power Status data");

		m_GenaPowerDataList.clear ();

		m_powerStatusMap.clear ();
		m_powerStatusMap = TA_IRS_Bus::DataCache::getInstance()->getAllPowerSectionStatus();


		if(!m_powerStatusMap.empty())
		{
			TA_IRS_Bus::PowerStatusMapIt itr = m_powerStatusMap.begin();

			for( ; itr != m_powerStatusMap.end(); itr ++)
			{
				m_GenaPowerDataList.push_back(convertToGENATopicData((itr->first), (itr->second)));
			}

			return m_GenaPowerDataList;
		}	
		
		FUNCTION_EXIT;
		return m_GenaPowerDataList;	
	}

	void PowerStatusDataStore::onPowerStatusUpdate(const unsigned short& sectionID, const unsigned char& powerStatus)
	{
		FUNCTION_ENTRY("onPowerStatusUpdate");

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Power Status Update sectionID %d, power status %d", sectionID, powerStatus);

		//received update message, and then pass the data to Gena
		TA_Base_Bus::GENATopicDataPtr pGenaTopicData = convertToGENATopicData(sectionID, powerStatus);
		//updateData(POWER_SECTION_TOPIC_NAME,pGenaTopicData);

		try
		{
			TA_Base_Bus::GENALibrary::getInstance().getServerInstance()->publishUpdate(pGenaTopicData);
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Publish power data error");
		}
		FUNCTION_EXIT;
	}

	TA_Base_Bus::GENATopicDataPtr PowerStatusDataStore::convertToGENATopicData(const unsigned short& sectionID, const unsigned char& powerStatus)
	{
		FUNCTION_ENTRY("convertToGENATopicData");

		std::string uri, strPowerStatus;
		std::string objectID=TA_Base_Core::toString(sectionID);
		 
		switch(powerStatus)
		{
		case TA_IRS_Bus::ENERGIZED:
			strPowerStatus = "Energized";
			break;
		case TA_IRS_Bus::DEENERGIZED:
			strPowerStatus = "Deenergized";
			break;
		default:
			strPowerStatus = "Unknown";
			break;
		}

		TA_Base_Bus::GENATopicDataPtr pGenaTopicData = TA_Base_Bus::GENATopicDataPtr(new TA_Base_Bus::GENATopicData(objectID,POWER_SECTION_TOPIC_NAME));

		pGenaTopicData->setAttributeValue(POWER_STATUS_ATTR, strPowerStatus);
		pGenaTopicData->setTopicURI(POWER_TOPIC_URL);
		pGenaTopicData->setUpdateType(GENA_UPDATE);

		FUNCTION_EXIT;
		return pGenaTopicData;
	}

	TA_Base_Bus::GENATopicDataPtr PowerStatusDataStore::convertToGENATopicData(const unsigned short& sectionID, const std::string& powerStatus)
	{
		FUNCTION_ENTRY("convertToGENATopicData");

		std::string uri;
		std::string objectID=TA_Base_Core::toString(sectionID);

		TA_Base_Bus::GENATopicDataPtr pGenaTopicData = TA_Base_Bus::GENATopicDataPtr(new TA_Base_Bus::GENATopicData(objectID,POWER_SECTION_TOPIC_NAME));

		pGenaTopicData->setAttributeValue(POWER_STATUS_ATTR, powerStatus);
		pGenaTopicData->setTopicURI(POWER_TOPIC_URL);
		pGenaTopicData->setUpdateType(GENA_BIRTH);

		FUNCTION_EXIT;
		return pGenaTopicData;
	}

}