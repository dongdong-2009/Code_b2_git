
#include "app/ats/AtsAgent/ModBusProtocolHandler/src/ModBusFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/ats/AtsAgent/ModBusProtocolHandler/src/PATableParser.h"
#include "app/ats/AtsAgent/ModBusProtocolHandler/src/PIDSTableParser.h"
#include "app/ats/AtsAgent/ModBusProtocolHandler/src/TrainTableParser.h"
#include "app/ats/AtsAgent/ModBusProtocolHandler/src/TrainPISTableParser.h"
#include "bus/ats/ats_agent/CommonATS/src/PIDSDispatcher.h"
#include "bus/ats/ats_agent/CommonATS/src/PADispatcher.h"
#include "bus/ats/ats_agent/CommonATS/src/PADataSynch.h"

#include "bus/ats/ats_agent/CommonATS/src/IProtocolManager.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataDispatcher.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataSync.h"
#include "bus/ats/ats_agent/CommonATS/src/DataProcessorThread.h"
#include "bus/ats/ats_agent/CommonATS/src/PADataProcessor.h"
#include "bus/ats/ats_agent/CommonATS/src/TrainDataProcessor.h"
#include "bus/ats/ats_agent/CommonATS/src/PIDSDataProcessor.h"
#include "bus/ats/ats_agent/CommonATS/src/TrainPISDataProcessor.h"


namespace TA_IRS_App
{
	TA_Base_Core::ReEntrantThreadLockable ModBusFactory::m_singletonLock;
	ModBusFactory* ModBusFactory::m_instance = NULL;

	ModBusFactory::ModBusFactory()
	{
		FUNCTION_ENTRY("ModBusFactory");
		FUNCTION_EXIT;
	}

	ModBusFactory::~ModBusFactory()
	{
		FUNCTION_ENTRY("~ModBusFactory");
		FUNCTION_EXIT;
	}

	ModBusFactory* ModBusFactory::getInstance()
	{
		FUNCTION_ENTRY("getInstance");

		if ( 0 == m_instance )
		{
			// Double checking to prevent multiple threads
			// creating multiple instances.

			// Any lock is okay at this stage.
			TA_Base_Core::ThreadGuard guard( m_singletonLock );

			if ( 0 == m_instance )
			{
				m_instance = new ModBusFactory();
			}
		}

		FUNCTION_EXIT;
		return m_instance;
	}

	TA_IRS_Bus::IProtocolParser* ModBusFactory::createParser(TA_IRS_Bus::AtsMessageType type,TA_Base_Core::AtsAgentEntityDataPtr EntityData)
	{
		FUNCTION_ENTRY("createParser");

		TA_IRS_Bus::IProtocolParser* retParser = NULL;
		switch (type)
		{
		case TA_IRS_Bus::ePA:
			retParser = new PATableParser(EntityData);
			break;

		case TA_IRS_Bus::ePIDS:
			retParser = new PIDSTableParser(EntityData);
			break;

		case  TA_IRS_Bus::eTRAIN:
			retParser = new TrainTableParser(EntityData);
			break;

		case  TA_IRS_Bus::ePIS:
			retParser = new TrainPISTableParser(EntityData);
			break;
		}

		FUNCTION_EXIT;
		return retParser;
	}

	TA_IRS_Bus::IDataDispatcher* ModBusFactory::createDispatcher(TA_IRS_Bus::AtsMessageType type)
	{
		FUNCTION_ENTRY("createParser");

		TA_IRS_Bus::IDataDispatcher* retDispatcher = NULL;
		switch (type)
		{
		case TA_IRS_Bus::ePA:
			retDispatcher = new TA_IRS_Bus::PADispatcher();
			break;

		case TA_IRS_Bus::ePIDS:
			retDispatcher = new TA_IRS_Bus::PIDSDispatcher();
			break;
		}

		FUNCTION_EXIT;
		return retDispatcher;
	}

	TA_IRS_Bus::IDataSync* ModBusFactory::createDataSynch(TA_IRS_Bus::AtsMessageType type,TA_Base_Core::AtsAgentEntityDataPtr EntityData)
	{
		FUNCTION_ENTRY("createParser");

		TA_IRS_Bus::IDataSync* retDataSync = NULL;
		switch (type)
		{
		case TA_IRS_Bus::ePA:
			retDataSync = new TA_IRS_Bus::PADataSynch(EntityData);
			break;
		}

		FUNCTION_EXIT;
		return retDataSync;
	}

	TA_IRS_Bus::DataProcessorThread* ModBusFactory::createAtsDataProcessor(TA_IRS_Bus::AtsMessageType type, TA_Base_Core::AtsAgentEntityDataPtr EntityData)
	{
		FUNCTION_ENTRY("createAtsDataProcessor");

		TA_IRS_Bus::DataProcessorThread* retAtsProcessor = NULL;

		switch(type)
		{
		case TA_IRS_Bus::ePA:
			retAtsProcessor = new TA_IRS_Bus::PADataProcessor(EntityData);
			break;

		case TA_IRS_Bus::eTRAIN:
			retAtsProcessor = new TA_IRS_Bus::TrainDataProcessor(EntityData);
			break;

		case TA_IRS_Bus::ePIDS:
			retAtsProcessor = new TA_IRS_Bus::PIDSDataProcessor(EntityData);
			break;

		case TA_IRS_Bus::ePIS:
			retAtsProcessor = new TA_IRS_Bus::TrainPISDataProcessor(EntityData);
			break;
		}

		FUNCTION_EXIT;
		return retAtsProcessor;
	}

}