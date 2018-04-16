
#include "bus/ats/ats_agent/CommonATS/src/DataProcessorManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "bus/ats/ats_agent/CommonATS/src/PADataProcessor.h"

namespace TA_IRS_Bus
{
	DataProcessorManager::DataProcessorManager(TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: m_atsEntityData(EntityData)
	{
		FUNCTION_ENTRY("DataProcessorManager");
		FUNCTION_EXIT;
	}

	DataProcessorManager::~DataProcessorManager()
	{
		FUNCTION_ENTRY("~DataProcessorManager");	
		FUNCTION_EXIT;
	}

	void DataProcessorManager::processDataDispatcher(TA_IRS_Bus::AtsMessageType type)
	{
		FUNCTION_ENTRY("processUpdtes");

		TA_IRS_Bus::DispatcherProcessorMapIter iter = m_dataDispatchMap.find(type);
		if (iter != m_dataDispatchMap.end())
		{
			(*iter).second->sendDataUpdate();
		}

		FUNCTION_EXIT;
	}

	void DataProcessorManager::processDataSync(TA_IRS_Bus::AtsMessageType type)
	{
		FUNCTION_ENTRY("processDataSync");

		TA_IRS_Bus::DataSyncProcessorMapIter iter = m_dataSyncMap.find(type);
		if (iter != m_dataSyncMap.end())
		{
			(*iter).second->processDataSync();
		}

		FUNCTION_EXIT;
	}

	void DataProcessorManager::registerDataSynch(TA_IRS_Bus::AtsMessageType type, TA_IRS_Bus::IDataSync* dataSynch)
	{
		FUNCTION_ENTRY("registerDataSynch");

		TA_IRS_Bus::DataSyncProcessorMapIter iter = m_dataSyncMap.find(type);
		if (iter == m_dataSyncMap.end() )
		{
			dataSynch->setDataUpdates(this);
			m_dataSyncMap[type] = dataSynch;
		}


		FUNCTION_EXIT;
	}
	void DataProcessorManager::deRegisterDataSynch(TA_IRS_Bus::AtsMessageType type)
	{
		FUNCTION_ENTRY("registerDataSynch");

		TA_IRS_Bus::DataSyncProcessorMapIter iter = m_dataSyncMap.find(type);
		if (iter != m_dataSyncMap.end() )
		{
			delete (*iter).second;
			(*iter).second = NULL;

			m_dataSyncMap.erase(iter);
		}

		FUNCTION_EXIT;
	}

	void DataProcessorManager::registerDataDispatch(TA_IRS_Bus::AtsMessageType type, TA_IRS_Bus::IDataDispatcher* dataDispatch)
	{
		FUNCTION_ENTRY("registerDataSynch");

		TA_IRS_Bus::DispatcherProcessorMapIter iter = m_dataDispatchMap.find(type);
		if (iter == m_dataDispatchMap.end() )
		{
			m_dataDispatchMap[type] = dataDispatch;
		}

		FUNCTION_EXIT;
	}

	void DataProcessorManager::deRegisterDataDispatch(TA_IRS_Bus::AtsMessageType type)
	{
		FUNCTION_ENTRY("registerDataSynch");

		TA_IRS_Bus::DispatcherProcessorMapIter iter = m_dataDispatchMap.find(type);
		if (iter != m_dataDispatchMap.end() )
		{
			delete (*iter).second;
			(*iter).second = NULL;

			m_dataDispatchMap.erase(iter);
		}

		FUNCTION_EXIT;
	}

	void DataProcessorManager::processDataNotification(DataNotificationPtr& newEvent)
	{
		FUNCTION_ENTRY("processDataNotification");

		newEvent->setDataUpdater(this);
		AtsDataProcessorMapIter iter = m_atsDataProcessorMap.begin();
		for (iter; iter != m_atsDataProcessorMap.end(); iter++)
		{
			(*iter).second->processAtsData(newEvent);
		}
		
		FUNCTION_EXIT;
	}

	void DataProcessorManager::registerAtsDataProcessor(AtsMessageType type, DataProcessorThread* dataProcessor)
	{
		FUNCTION_ENTRY("registerDataSynch");

		AtsDataProcessorMapIter iter = m_atsDataProcessorMap.find(type);
		if (iter == m_atsDataProcessorMap.end() )
		{
			m_atsDataProcessorMap[type] = dataProcessor;
		}

		FUNCTION_EXIT;
	}
	void DataProcessorManager::deRegisterAtsDataProcessor(TA_IRS_Bus::AtsMessageType type)
	{
		FUNCTION_ENTRY("deRegisterAtsDataProcessor");

		AtsDataProcessorMapIter iter = m_atsDataProcessorMap.find(type);
		if (iter != m_atsDataProcessorMap.end() )
		{
			delete (*iter).second;
			(*iter).second = NULL;

			m_atsDataProcessorMap.erase(iter);
		}

		FUNCTION_EXIT;
	}

	DataProcessorThread* DataProcessorManager::getAtsDataProcessor(AtsMessageType type)
	{
		FUNCTION_ENTRY("registerDataSynch");

		DataProcessorThread* retDataProcessor = NULL;
		AtsDataProcessorMapIter iter = m_atsDataProcessorMap.find(type);
		if (iter != m_atsDataProcessorMap.end() )
		{
			retDataProcessor = (*iter).second;
		}

		FUNCTION_EXIT;
		return retDataProcessor;
	}


}