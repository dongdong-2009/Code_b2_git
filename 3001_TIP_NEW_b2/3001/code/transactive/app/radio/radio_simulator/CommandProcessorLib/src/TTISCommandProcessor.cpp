/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\3001_TIP\3001\transactive\app\radio\radio_simulator\CommandProcessorLib\src\TTISCommandProcessor.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/16/08 10:05:25 $
  * Last modified by:  $Author: builder $
  *
  * This class is used for TTIS command processing.
  */

#include "app/radio/radio_simulator/CommandProcessorLib/src/TTISCommandProcessor.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMHelper.h"

#include "app/radio/radio_simulator/TrainSimulator/SimulatorConfig.h"
#include <vector>

//#include "app/radio/radio_simulator/TrainSimulator/stdafx.h"

#include "..\..\TrainSimulator\EtcTrainSimConfigHandler.h"
#include "..\..\TrainSimulator\AutoSendDataModel.h"


namespace TA_IRS_App
{
	int TTISCommandProcessor::_commandProtocol[5] = { 0x1F,0x20,0x21,0x23,0x2B };
	TTISCommandProcessor::TTISCommandProcessor( IPACMData& paCMData )
		: TrainCommandProcessorThread(paCMData),
		  m_model(NULL)
	{
		FUNCTION_ENTRY("TTISCommandProcessor");
		FUNCTION_EXIT;
	}

	TTISCommandProcessor::~TTISCommandProcessor()
	{
		FUNCTION_ENTRY("~TTISCommandProcessor");

		//Stop the Thread
		terminateAndWait();

		FUNCTION_EXIT;
	}

	void TTISCommandProcessor::attachSocketManager(IModel* model)
	{
		FUNCTION_ENTRY("attachSocketManager");

		m_model = model;

		FUNCTION_EXIT
	}
	void TTISCommandProcessor::sendSDSMessage(boost::shared_ptr<SDSMessageData>& sdsMessage)
	{
		FUNCTION_ENTRY("sendSDSMessage");

		addTrainCommand(sdsMessage);

		FUNCTION_EXIT;
	}

	void TTISCommandProcessor::processTrainCommand(boost::shared_ptr<SDSMessageData>& sdsMessage)
	{
		FUNCTION_ENTRY("processTrainCommand");


		SDSMessageData::VMessage pData = sdsMessage->getData();
		SDSMessageData::VMessage resp = sdsMessage->getResponse();
		int userDataIndex = resp.size();

		//Get the library Version No. to be upgraded
		SimulatorConfig* config = SimulatorConfig::getInstance();
		config->ReadConfig();
		std::vector<int> libraryVersionUpgrade = config->getLibraryVersion();

		bool commandExist = false;
		//Check if the command exist else return
		for ( int command=0; command < 5; command++)
		{
			if ( sdsMessage->getCommandType() == _commandProtocol[command])
			{
				commandExist = true;
				break;
			}
		}

		IConfigDataItemPtrType autoSendSharedPtr = EtcTrainSimConfigHandler::getInstance()->getAutoSendingData(EAutoSendType::ETTISType);
		IConfigDataItemPtrType autoSendSharedPtrCO = EtcTrainSimConfigHandler::getInstance()->getAutoSendingData(EAutoSendType::EChangeOver);
		TTISAutoSendInfo * autoSendPtr = (TTISAutoSendInfo *)autoSendSharedPtr.get();
		ChangeOverConfig * autoSendPtrCO = (ChangeOverConfig *)autoSendSharedPtrCO.get();
		bool isValidCommand = false;

		if ( commandExist )
		{
			m_logger->setLogView("TTIS Command REceived");
			//m_cmData.logger(pData);


			char v1,v2,v3,v4,v5,v6,v7,v8;
			v1 = m_cmData.getTTISLibVersion().at(0);
			v2 = m_cmData.getTTISLibVersion().at(1);
			v3 = m_cmData.getTTISLibVersion().at(2);
			v4 = m_cmData.getTTISLibVersion().at(3);
			v5 = m_cmData.getTTISLibVersion().at(4);
			v6 = m_cmData.getTTISLibVersion().at(5);
			v7 = m_cmData.getTTISLibVersion().at(6);
			v8 = m_cmData.getTTISLibVersion().at(7);
			int a1,a2,a3,a4 = 0;
			a1 = CMHelper::Hex2Int(v1,1) + CMHelper::Hex2Int(v2,0);
			a2 = CMHelper::Hex2Int(v3,1) + CMHelper::Hex2Int(v4,0);
			a3 = CMHelper::Hex2Int(v5,1) + CMHelper::Hex2Int(v6,0);
			a4 = CMHelper::Hex2Int(v7,1) + CMHelper::Hex2Int(v8,0);

			bool _isTTISDownload = false;

			switch( sdsMessage->getCommandType())
			{
			case TTIS_FREE_TEXT_MESSAGE:

				m_logger->setLogView("Receive TTIS Free Text Message : Command : 0x1F  ");
				resp.push_back(USER_DATA_FLAG);
				resp.push_back(TTIS_COMMAND_RECEIVED);
				resp.push_back( pData[23] ); // physical train number
				resp.push_back(13); // Length
				resp.push_back(0); // Origin, TIMS = 0
				
				if (!autoSendPtr->getSendTtisCmdRecvFailed())
				{
					m_logger->setLogView("Setting Status for TTIS Command Recieved to 1 (PR)");
					resp.push_back(1); // status 1 PR
				}
				else
				{
					m_logger->setLogView("Setting Status for TTIS Command Recieved to 5 (FAILED)");
					resp.push_back(5); // status 5 Failed
				}
				
				resp.push_back(OC2_TTIS_FREE_TEXT_MESSAGE);
				resp.push_back(a1); // library version number
				resp.push_back(a2);
				resp.push_back(a3);
				resp.push_back(a4);
				// calculate check sum and send the message

				break;
			case TTIS_PRESTORED_MESSAGE:

				m_logger->setLogView("Receive TTIS Prestored Message : Command : 0x20");
				resp.push_back(USER_DATA_FLAG);
				resp.push_back(TTIS_COMMAND_RECEIVED);
				resp.push_back( pData[23] ); // physical train number
				resp.push_back(13); // Length
				resp.push_back(0); // Origin, TIMS = 0
				resp.push_back(1); // status 5, to do
				resp.push_back(OC2_TTIS_PRESTORED_MESSAGE);
				resp.push_back(a1); // library version number
				resp.push_back(a2);
				resp.push_back(a3);
				resp.push_back(a4);
				// calculate check sum and send the message

				break;
			case TTIS_LIBRARY_DOWNLOAD:
				
				if (!autoSendPtrCO->getTtisStatusFailure())
				{
					m_logger->setLogView("Receive TTIS Library Download : Command : 0x21");
					resp.push_back(USER_DATA_FLAG);
					resp.push_back(TTIS_COMMAND_RECEIVED);
					resp.push_back( pData[23] ); // physical train number
					resp.push_back(13); // Length
					resp.push_back(0); // Origin, TIMS = 0

					if (!autoSendPtr->getSendTtisCmdRecvFailed())
					{
						m_logger->setLogView("Setting Status for TTIS Command Recieved to 1 (PR)");
						resp.push_back(1); // status 1 PR
					}
					else
					{
						m_logger->setLogView("Setting Status for TTIS Command Recieved to 5 (FAILED)");
						resp.push_back(5); // status 5 Failed
					}

					resp.push_back(OC2_TTIS_LIBRARY_DOWNLOAD);
					resp.push_back(a1); // library version number
					resp.push_back(a2);
					resp.push_back(a3);
					resp.push_back(a4);
					
					_isTTISDownload = true;
					if (!autoSendPtr->getAutoSendTTISLib())
					{
						isValidCommand = false;
					}
					else
					{
						isValidCommand = true;
					}
				}
				else
				{
					m_logger->setLogView("Returning ChangeOver in response for the TTIS Lib Command");					
					resp.push_back(USER_DATA_FLAG);
					resp.push_back(0x41);
					resp.push_back( pData[23] ); // physical train number
					resp.push_back(9); // Length
					resp.push_back(0); // Origin, TIMS = 0
					resp.push_back(0); // status 5, to do
					resp.push_back(1);

					isValidCommand = true;
				}
				// calculate check sum and send the message
				
				

				break;
			case TTIS_MESSAGE_RESET:

				m_logger->setLogView("Receive TTIS Message Reset : Command : 0x23");
				resp.push_back(USER_DATA_FLAG);
				resp.push_back(TTIS_COMMAND_RECEIVED);
				resp.push_back( pData[23] ); // physical train number
				resp.push_back(13); // Length
				resp.push_back(0); // Origin, TIMS = 0
				resp.push_back(1); // status 5, to do
				resp.push_back(OC2_TTIS_MESSAGE_RESET);
				resp.push_back(a1); // library version number
				resp.push_back(a2);
				resp.push_back(a3);
				resp.push_back(a4);
				// calculate check sum and send the message

				break;
				//case END_OF_TTIS_DOWNLOAD:
			case TTIS_LIBRARY_UPGRADE:
				
				if (!autoSendPtrCO->getUpgradeTtisStatusFailure())
				{
					m_logger->setLogView("Receive TTIS Library Upgrade : Command : 0x2B");
					resp.push_back(USER_DATA_FLAG);
					resp.push_back(TTIS_COMMAND_RECEIVED);
					resp.push_back( pData[23] ); // physical train number
					resp.push_back(13); // Length
					resp.push_back(0); // Origin, TIMS = 0
					//resp.push_back(1); // status 5, to do
					resp.push_back(3); // status 5, to do
					resp.push_back(OC2_TTIS_LIBRARY_UPGRADE);
					resp.push_back(libraryVersionUpgrade.at(3)); // library version number
					resp.push_back(libraryVersionUpgrade.at(2));
					resp.push_back(libraryVersionUpgrade.at(1));
					resp.push_back(libraryVersionUpgrade.at(0));
				}
				else
				{
					m_logger->setLogView("Returning ChangeOver in response for the TTIS Version Upgrade Command");					
					resp.push_back(USER_DATA_FLAG);
					resp.push_back(0x41);
					resp.push_back( pData[23] ); // physical train number
					resp.push_back(9); // Length
					resp.push_back(0); // Origin, TIMS = 0
					resp.push_back(0); // status 5, to do
					resp.push_back(1);

					isValidCommand = true;
				}
				

				//Testing purposes
// 				resp.push_back(0x58); // library version number
// 				resp.push_back(0x2E);
// 				resp.push_back(0x32);
// 				resp.push_back(0x0C);

				// calculate check sum and send the message

				break;


			}
			
			int _trainID = static_cast<int>(pData[23]);
			//Check the Timer Strategy here
			if ( TIMER_SEND == getStrategyType(_trainID) || TIMER_DEFAULT == getStrategyType(_trainID) )
			{
				//apply timer strategy here
				if (_isTTISDownload)
				{
					if (isValidCommand)
					{
						applyTimerStategy(_trainID);
						processSendData(userDataIndex,resp,pData);
					}
					else
					{
						m_logger->setLogView("Not Sending response for TTIS Lib Donwload request");
					}

				}
				else
				{
					applyTimerStategy(_trainID);
					processSendData(userDataIndex,resp,pData);
				}

				
			}

		}
		
			FUNCTION_EXIT;
	}

}