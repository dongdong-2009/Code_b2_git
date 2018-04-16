/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\app\ats\AtsAgent\ModBusProtocolHandler\src\ModBusProtocolManager.cpp $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This is a thread object which is responsible for handling RTU Library to receive the data for data processing.
  */

#include "app/ats/AtsAgent/ModBusProtocolHandler/src/ModBusProtocolManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/ats/ats_agent/CommonATS/src/IProtocolParser.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataProcessor.h"
#include "bus/ats/ats_agent/CommonATS/src/AgentOperationMode.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "bus/ats/ats_agent/CommonATS/src/DataCache.h"
#include "app/ats/AtsAgent/ModBusProtocolHandler/src/AtsConstants.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"

namespace TA_IRS_App
{
	ModBusProtocolManager::ModBusProtocolManager(TA_Base_Core::AtsAgentEntityDataPtr atsEntity, TA_Base_Bus::IProcessDataBlock& processDataBlock)
		: m_rawTable(MODBUS_START_ADDRESS,MODBUS_END_ADDRESS),
	      m_processDataBlock(processDataBlock),
		  m_dataNotif(NULL),
		  m_bPowerRequired(false),
		  m_semaphore(0),
		  m_keepRunning(false)
	{
		FUNCTION_ENTRY("ModBusProtocolManager");

		m_processDataBlock.registerDataBlockObserver(this);
	
		FUNCTION_EXIT;
	}

	ModBusProtocolManager::~ModBusProtocolManager()
	{
		FUNCTION_ENTRY("~ModBusProtocolManager");

		m_processDataBlock.removeDataBlockObserver(this);

		terminateAndWait();

		FUNCTION_EXIT;
	}

	TA_Base_Bus::DataBlock<WORD>& ModBusProtocolManager::getDataBlock()
	{
		FUNCTION_ENTRY("getDataBlock");

		TA_Base_Core::ThreadGuard guard(m_lockCache);

		m_rawTable.getDataBlock().setAll(0);

		FUNCTION_EXIT;
		return m_rawTable.getDataBlock();
	}

	void ModBusProtocolManager::updataDataBlock(const TA_Base_Bus::DataBlock<WORD> &dataBlock)
	{
		FUNCTION_ENTRY("updataDataBlock");

		{
			TA_Base_Core::ThreadGuard guard(m_lockCache);
			 m_rawTable.setDataBlock(dataBlock);

			 //Based on the RTUMnager implementation, once the updateDataBlock has been called
			 //it means that the Data Block Quality is GOOD.
			 //Sets the DataBlock Quality to good
			 m_rawTable.setDatablockQuality(TA_Base_Core::ITA_QUALITY_GOOD);

			 //Based on the ICD, the bit to identify the power status is 
			 //located in address 1.
			 //Check power bit and send power data if required
			 WORD word = dataBlock[1];
			 m_bPowerRequired = (word & POWER_BIT_POS) != 0;

			 std::ostringstream powerDataStr;
			 powerDataStr << "0x" << std::hex <<word;

			 LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"PowerData:%s, Is Power required? : %s",
				 powerDataStr.str().c_str(),m_bPowerRequired?"TRUE":"FALSE");
		}

		m_semaphore.post();

		FUNCTION_EXIT;
	}

	void ModBusProtocolManager::registerProtocolParser(TA_IRS_Bus::AtsMessageType type,TA_IRS_Bus::IProtocolParser* protocolParser)
	{
		FUNCTION_ENTRY("registerProtocolParser");

		TA_IRS_Bus::ParserProcessorMapIter iter = m_parserProcessors.find(type);
		if (iter == m_parserProcessors.end() )
		{
			m_parserProcessors[type] = protocolParser;
			protocolParser->setAtsDataNotification(m_dataNotif);
		}

		FUNCTION_EXIT;
	}

	void ModBusProtocolManager::deRegisterProtocolParser(TA_IRS_Bus::AtsMessageType type)
	{
		FUNCTION_ENTRY("FUNCTION_ENTRY");

		TA_IRS_Bus::ParserProcessorMapIter iter = m_parserProcessors.find(type);
		if (iter != m_parserProcessors.end() )
		{
			delete (*iter).second;
			(*iter).second = NULL;

			m_parserProcessors.erase(iter);
		}

		FUNCTION_EXIT;
	}

	void ModBusProtocolManager::setOperationMode(TA_Base_Core::EOperationMode eMode)
	{
		FUNCTION_ENTRY("setOperationMode");

		if (TA_Base_Core::Control == eMode)
		{
			startProcolProcessing();
		}
		else if (TA_Base_Core::Monitor == eMode)
		{
			stopProtocolProcessing();
		}

		FUNCTION_EXIT;
	}

	void ModBusProtocolManager::setAtsDataNotification(TA_IRS_Bus::IAtsDataNotification* atsDataNofic)
	{
		FUNCTION_ENTRY("setAtsDataNotification");

		m_dataNotif = atsDataNofic;

		FUNCTION_EXIT;
	}

	void ModBusProtocolManager::startProcolProcessing()
	{
		FUNCTION_ENTRY("startProcolProcessing");

		m_keepRunning = true;
		start();

		FUNCTION_EXIT;
	}

	void ModBusProtocolManager::stopProtocolProcessing()
	{
		FUNCTION_ENTRY("stopProtocolProcessing");

		m_keepRunning = false;
		terminateAndWait();

		FUNCTION_EXIT;
	}

	void ModBusProtocolManager::terminate()
	{
		FUNCTION_ENTRY("terminate");

		m_keepRunning = false;
		m_semaphore.post();

		FUNCTION_EXIT;
	}

	void ModBusProtocolManager::run()
	{
		FUNCTION_ENTRY("run");

		while(m_keepRunning)
		{
			m_semaphore.wait();

			if (true == TA_IRS_Bus::AgentOperationMode::getInstance().isInControlMode() )
			{
				TA_IRS_Bus::RawTable localrawTable(MODBUS_START_ADDRESS,MODBUS_END_ADDRESS);
				{
					TA_Base_Core::ThreadGuard guard(m_lockCache);
					localrawTable.setDataBlock(m_rawTable.getDataBlock());
					//Set the Data Block Quality
					localrawTable.setDatablockQuality(m_rawTable.getDatablockQuality());
				}

				//Displays log for raw data information
				TA_IRS_Bus::AtsHelper::printRawData(localrawTable.getDataBlock(),"ModBusProtocolManager");

				//Notify Parser observers
				TA_IRS_Bus::ParserProcessorMapIter iter = m_parserProcessors.begin();
				for (iter; iter != m_parserProcessors.end(); iter++ )
				{
					(*iter).second->parseAtsTable(localrawTable);
				}
				//only send out power status when the datablock is in good quality.
				if (TA_IRS_Bus::AtsHelper::isGoodDatapointQuality(localrawTable.getDatablockQuality()))
				{
					updatePowerStatus();
				}
				//Send power status if required
				
			}
		}

		FUNCTION_EXIT;
	}

	void ModBusProtocolManager::updatePowerStatus()
	{
		FUNCTION_ENTRY("updatePowerStatus");

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"Power required? : %s",
			m_bPowerRequired?"TRUE":"FALSE");

		//If power is not required, then there's no need to send Power status updates to ATS.
		if ( false == TA_IRS_Bus::DataCache::getInstance()->IsPowerMapConfiged() ||  false == m_bPowerRequired)
			return;

		try
		{
			// create and send the power data table
			TA_Base_Bus::DataBlock<WORD> dataBlock;
			dataBlock.setStartAndLength(POWER_START_ADDRESS,TA_IRS_Bus::POWER_STATUS_SIZE/2);
			//dataBlock.set(1,0);
			unsigned char* powerStatus = TA_IRS_Bus::DataCache::getInstance()->getPowerStatus();
			char data[16] = {0};
			std::string strPowerStatus="--";
			for(int i=0;i<TA_IRS_Bus::POWER_STATUS_SIZE/2;i++)
			{
				//powerStatus[i*2] is lower byte, powerStatus[i*2+1] is higher byte
				//The output from RTU is powerStatus[i*2+1], powerStatus[i*2+1], big endian
				//so we change the log as format of big endian,
				sprintf(data,"%x,%x ",powerStatus[i*2+1],powerStatus[i*2]);
				strPowerStatus+=data;
				WORD wdata=powerStatus[i*2]+(powerStatus[i*2+1]<<(TA_IRS_Bus::POWER_STATUS_SIZE/2));
				dataBlock.set(POWER_START_ADDRESS+i,wdata);
			}

			// reset the power status bit in SIG FEP
			//No such requirement in FZ
			//TA_Base_Bus::DataBlock<WORD> dbPowerSignal;
			//dbPowerSignal.setStartAndLength(1, 1);
			//dbPowerSignal.set(1, 0);
	
			////Send Power status updates
			int res = m_processDataBlock.sendDataBlock(dataBlock);
			LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugDebug, "set power status,return %d", res);	

			//Send reset Power bit,No such requirement in FZ
			//res = m_processDataBlock.sendDataBlock(dbPowerSignal);
			//LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugDebug, "set power signal bit,return:%d", res);
			LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugDebug, "set power status,value ( %s )", strPowerStatus.c_str());
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Encounter error while updatePowerStatus");
		}

		FUNCTION_EXIT;
	}

	void ModBusProtocolManager::updateQualityStatus(const TA_Base_Core::EDataPointQuality& quality)
	{
		FUNCTION_ENTRY("updateQualityStatus");
        
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"RTU status change, Receive, updateQualityStatus : %d",
			static_cast<int>(quality));

		//Check if the DataBlock Quality is not GOOD before processing.
		//This is to make sure that we only handle the BAD quality States.
		if(false == TA_IRS_Bus::AtsHelper::isGoodDatapointQuality(quality))
		{
			{
				TA_Base_Core::ThreadGuard guard(m_lockCache);
				//Use a dummy data for Data block
				TA_IRS_Bus::RawTable localrawTable(MODBUS_START_ADDRESS,MODBUS_END_ADDRESS);

				//Reset all the value for data block
				m_rawTable.setDataBlock(localrawTable.getDataBlock());

				//Sets the DataBlock Quality
				m_rawTable.setDatablockQuality(quality);

				//Set the Power Status flag to false.
				m_bPowerRequired = false;
			}

			m_semaphore.post();
		}
		
		FUNCTION_EXIT;
	}

}