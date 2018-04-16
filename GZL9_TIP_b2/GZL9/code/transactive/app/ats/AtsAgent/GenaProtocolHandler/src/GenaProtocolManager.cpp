
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaProtocolManager.h"

#include "bus/protocol/Gena/src/CommonDef.h"
#include "bus/ats/ats_agent/CommonATS/src/IProtocolParser.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataProcessor.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/DataCache.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"
#include "bus/ats/ats_agent/CommonATS/src/AgentOperationMode.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/CtaScadaExceptions.h"

#include "boost/algorithm/string.hpp"
#include <time.h>

namespace TA_IRS_App
{

	GenaProtocolManager::GenaProtocolManager(TA_Base_Bus::ItaScada* pScada)
		: m_semaphore(0),
		  m_isControlMode(false),
		  m_genaTable(TA_Base_Core::ITA_QUALITY_GOOD),
		  m_pScada(pScada)
	{
		FUNCTION_ENTRY("GenaProtocolManager");

		FUNCTION_EXIT;
	}

	GenaProtocolManager::~GenaProtocolManager()
	{
		FUNCTION_ENTRY("~GenaProtocolManager");

		FUNCTION_EXIT;
	}

	void GenaProtocolManager::registerProtocolParser(TA_IRS_Bus::AtsMessageType type, TA_IRS_Bus::IProtocolParser* protocolParser)
	{
		FUNCTION_ENTRY("registerProtocolParser");

		GenaBaseTableParser *pGenaParser = dynamic_cast<GenaBaseTableParser*> (protocolParser);

		GenaParserMapIter iter = m_parserProcessors.find(type);
		if (iter == m_parserProcessors.end() )
		{
			m_parserProcessors[type] = pGenaParser;
			pGenaParser->setAtsDataNotification(m_dataNotif);
		}

		FUNCTION_EXIT;
	}

	void GenaProtocolManager::deRegisterProtocolParser(TA_IRS_Bus::AtsMessageType type)
	{
		FUNCTION_ENTRY("deRegisterProtocolParser");

		GenaParserMapIter iter = m_parserProcessors.find(type);
		if (iter != m_parserProcessors.end() )
		{
			delete (*iter).second;
			(*iter).second = NULL;

			m_parserProcessors.erase(iter);
		}

		FUNCTION_EXIT;
	}

	void GenaProtocolManager::setOperationMode(TA_Base_Core::EOperationMode eMode)
	{
		FUNCTION_ENTRY("setOperationMode");

		if (TA_Base_Core::Control == eMode)
		{
			m_isControlMode = true;
		}
		else if (TA_Base_Core::Monitor == eMode)
		{
			m_isControlMode = false;
		}

		FUNCTION_EXIT;
	}


	void GenaProtocolManager :: updateDataImpl(const std::string& topicName, TA_Base_Bus::GENATopicDataPtr pData)
	{
		FUNCTION_ENTRY("updateDataImpl");

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Received %s data, update type %s", pData->getTopicName().c_str(), pData->getUpdateType().c_str() );

		if(m_isControlMode)
		{
			TA_Base_Core::ThreadGuard guard(m_lockCache);

			m_genaTable.setTopicData(pData);

			m_genaTable.setDataPointQuality(TA_Base_Core::ITA_QUALITY_GOOD);

			GenaParserMapIter itr = m_parserProcessors.begin();
			for (itr; itr != m_parserProcessors.end(); itr++ )
			{
				(*itr).second->parseGenaAtsTable(m_genaTable);
			}

		}

		FUNCTION_EXIT;
	}

	void GenaProtocolManager :: updataConnectStatDp(const std::string& dpAddress, unsigned long dpValue)
	{
		FUNCTION_ENTRY("updataConnectStatDp");

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Update Connection Status, Address : %s, Value : %d",
			dpAddress.c_str(),dpValue );

		if (dpAddress.empty()) 
		{
			return;
		}

		try
		{
			::time_t timestamp;
			::time(&timestamp);

			//Update the Datapoint
			m_pScada->setLocalDataPointStateByAddress(dpAddress,dpValue,
				TA_Base_Core::ITA_QUALITY_GOOD,timestamp); 
		}
		catch(TA_Base_Core::CtaScadaInvalidConfigurationException& mssge)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				"%s", mssge.what());
		}

		catch(TA_Base_Core::CtaScadaWriteException& e)
		{
			std::ostringstream reason;
			reason << "Setting Local Data Point Error "
				<< dpAddress.c_str()
				<< ":"
				<< e.what();
			LOG_EXCEPTION_CATCH(SourceInfo, "CtaScadaWriteException", reason.str().c_str());
		}

		FUNCTION_EXIT;
	}

	void GenaProtocolManager :: onStateChanged(const std::string& strDpAddress, const GENAConnectionStatus& status )
	{	
		FUNCTION_ENTRY("onStateChanged");

		if(!m_isControlMode)
		{
			return;
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Received sever dpAddress %s, connection status changed to %d", strDpAddress.c_str(), status );
        //if status=BAD_ALL  then udpate all SIG datapoint to bad quality
		//if status=PASSIVE  the  update connection status to Passive
		//if status=BAD  the  update connection status to BAD
		if (GENA_CONN_StatusBad_ALL == status) 
		{
			TA_Base_Core::ThreadGuard guard(m_lockCache);
			m_genaTable.setDataPointQuality(TA_Base_Core::ITA_QUALITY_BAD);
			setAllDataPointQuality();
		}
		else if (GENA_CONN_StatusBad == status)
		{
			updataConnectStatDp(strDpAddress, ConnStat_BAD);
		}
		else if (GENA_CONN_StatusGood_Passive == status)
		{
			updataConnectStatDp(strDpAddress, ConnStat_PASSIVE);
		}
		else if (GENA_CONN_StatusGood_Active == status)
		{
			updataConnectStatDp(strDpAddress, ConnStat_GOOD);
		}
			
		FUNCTION_EXIT;
	}

	void GenaProtocolManager :: setAtsDataNotification(TA_IRS_Bus::IAtsDataNotification* atsDataNofic)
	{
		FUNCTION_ENTRY("setAtsDataNotification");

		m_dataNotif = atsDataNofic;

		FUNCTION_EXIT;
	}

	void GenaProtocolManager :: synchronizeBegin(const std::string& topicName)
	{
		FUNCTION_ENTRY("synchronizeBegin");

		TA_IRS_Bus::DataCache::getInstance()->syncDataBegin(topicName);

		FUNCTION_EXIT;
	}

	void GenaProtocolManager :: synchronizeEnd(const std::string& topicName)
	{
		FUNCTION_ENTRY("synchronizeEnd");

		TA_IRS_Bus::DataCache::getInstance()->syncDataEnd(topicName);

		FUNCTION_EXIT;
	}

	void GenaProtocolManager :: setAllDataPointQuality()
	{	
		FUNCTION_ENTRY("setAllDataPointQuality");

		if(m_isControlMode)
		{
			GenaParserMapIter itr = m_parserProcessors.begin();
			for (itr; itr != m_parserProcessors.end(); itr++ )
			{
				(*itr).second->parseGenaAtsTable(m_genaTable);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "AtsMessageType %d, Set DataPoint Quality to Bad", itr->first);
			}
		}

		FUNCTION_EXIT;
	}

}