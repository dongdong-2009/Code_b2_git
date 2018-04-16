
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaFactory.h"
#include "bus/ats/ats_agent/CommonATS/src/IProtocolManager.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataDispatcher.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataSync.h"
#include "bus/ats/ats_agent/CommonATS/src/DataProcessorThread.h"
#include "core/utilities/src/DebugUtil.h"

#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaPATableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaTrainTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/SignalStatusTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaBlkTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaEsbTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaFGTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaSwitchTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaPISTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaPlatformTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaPlatformDoorTableParser.h"

#include "bus/ats/ats_agent/CommonATS/src/PADispatcher.h"
#include "bus/ats/ats_agent/CommonATS/src/PADataSynch.h"

#include "bus/ats/ats_agent/CommonATS/src/PADataProcessor.h"
#include "bus/ats/ats_agent/CommonATS/src/TrainDataProcessor.h"
#include "bus/ats/ats_agent/CommonATS/src/TrainPISDataProcessor.h"
#include "bus/ats/ats_agent/CommonATS/src/PlatformDataProcessor.h"
#include "bus/ats/ats_agent/CommonATS/src/SignalDataProcessor.h"
#include "bus/ats/ats_agent/CommonATS/src/BlkDataProcessor.h"
#include "bus/ats/ats_agent/CommonATS/src/EsbDataProcessor.h"
#include "bus/ats/ats_agent/CommonATS/src/FGDataProcessor.h"
#include "bus/ats/ats_agent/CommonATS/src/SwitchDataProcessor.h"
#include "bus/ats/ats_agent/CommonATS/src/PlatformDoorDataProcessor.h"


namespace TA_IRS_App
{
	TA_Base_Core::ReEntrantThreadLockable GenaFactory::m_singletonLock;

	GenaFactory* GenaFactory::m_instance = NULL;

	GenaFactory::GenaFactory()
	{
		FUNCTION_ENTRY("GenaFactory");
		FUNCTION_EXIT;
	}

	GenaFactory::~GenaFactory()
	{
		FUNCTION_ENTRY("~GenaFactory");
		FUNCTION_EXIT;
	}

	GenaFactory* GenaFactory::getInstance()
	{
		FUNCTION_ENTRY("getInstance");

		if ( 0 == m_instance )
		{
			// Double checking to prevent multiple threads
			// creating multiple instances.

			// Any lock is ok at this stage.
			TA_Base_Core::ThreadGuard guard( m_singletonLock );

			if ( 0 == m_instance )
			{
				m_instance = new GenaFactory();
			}
		}

		FUNCTION_EXIT;
		return m_instance;
	}

	TA_IRS_App::GenaBaseTableParser* GenaFactory::createParser(TA_IRS_Bus::AtsMessageType type)
	{
		FUNCTION_ENTRY("createParser");

		GenaBaseTableParser* retParser = NULL;
		switch (type)
		{
		case TA_IRS_Bus::ePA:
			retParser = new GenaPATableParser();
			break;

		case  TA_IRS_Bus::eTRAIN:
			retParser = new GenaTrainTableParser();
			break;

		case TA_IRS_Bus::eSIG:
			retParser = new SignalStatusTableParser();
			break;

		case TA_IRS_Bus::eBLK:
			retParser = new GenaBlkTableParser();
			break;

		case TA_IRS_Bus::eFG:
			retParser = new GenaFGTableParser();
			break;

		case TA_IRS_Bus::eESB:
			retParser = new GenaEsbTableParser();
			break;

		case TA_IRS_Bus::eSW:
			retParser = new GenaSwitchTableParser();
			break;

		case TA_IRS_Bus::ePIS:
			retParser = new GenaPISTableParser();
			break;
			
		case TA_IRS_Bus::ePlatform:
			retParser = new GenaPlatformTableParser();
			break;

		case TA_IRS_Bus::ePlatformDoor:
			retParser = new GenaPlatformDoorTableParser();
			break;

		default:
			break;
		}

		FUNCTION_EXIT;
		return retParser;
	}

	TA_IRS_Bus::IDataDispatcher* GenaFactory::createDispatcher(TA_IRS_Bus::AtsMessageType type)
	{
		FUNCTION_ENTRY("createParser");

		TA_IRS_Bus::IDataDispatcher* retDispatcher = NULL;
		switch (type)
		{
		case TA_IRS_Bus::ePA:
			retDispatcher = new TA_IRS_Bus::PADispatcher();
			break;

		default:
			break;
		}

		FUNCTION_EXIT;
		return retDispatcher;
	}

	TA_IRS_Bus::IDataSync* GenaFactory::createDataSynch(TA_IRS_Bus::AtsMessageType type,TA_Base_Core::AtsAgentEntityDataPtr EntityData)
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

	TA_IRS_Bus::DataProcessorThread* GenaFactory::createAtsDataProcessor(TA_IRS_Bus::AtsMessageType type, TA_Base_Core::AtsAgentEntityDataPtr EntityData)
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

		case TA_IRS_Bus::ePIS:
			retAtsProcessor = new TA_IRS_Bus::TrainPISDataProcessor(EntityData);
			break;
			
		case TA_IRS_Bus::ePlatform:
			retAtsProcessor = new TA_IRS_Bus::PlatformDataProcessor(EntityData);
			break;
			
		case TA_IRS_Bus::eSIG:
			retAtsProcessor = new TA_IRS_Bus::SignalDataProcessor(EntityData);
			break;

		case TA_IRS_Bus::eBLK:
			retAtsProcessor = new TA_IRS_Bus::BlkDataProcessor(EntityData);
			break;

		case TA_IRS_Bus::eFG:
			retAtsProcessor = new TA_IRS_Bus::FGDataProcessor(EntityData);
			break;

		case TA_IRS_Bus::eESB:
			retAtsProcessor = new TA_IRS_Bus::EsbDataProcessor(EntityData);
			break;

		case TA_IRS_Bus::eSW:
			retAtsProcessor = new TA_IRS_Bus::SwitchDataProcessor(EntityData);
			break;

		case TA_IRS_Bus::ePlatformDoor:
			retAtsProcessor = new TA_IRS_Bus::PlatformDoorDataProcessor(EntityData);
			break;

		default:
			break;
		}

		FUNCTION_EXIT;
		return retAtsProcessor;
	}

}