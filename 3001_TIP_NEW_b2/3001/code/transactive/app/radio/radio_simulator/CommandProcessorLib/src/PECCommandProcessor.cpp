/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\3001_TIP\3001\transactive\app\radio\radio_simulator\CommandProcessorLib\src\PECCommandProcessor.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/16/08 10:05:25 $
  * Last modified by:  $Author: builder $
  *
  * This class is used for PEC command processing.
  */

#include "app/radio/radio_simulator/CommandProcessorLib/src/PECCommandProcessor.h"
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMHelper.h"
//#include "app/radio/radio_simulator/src/FrameCheckSequence.h"
#include "app/radio/radio_simulator/TrainSimulator/FrameCheckSequence.h"

#include "core/utilities/src/DebugUtil.h"

#include "..\..\TrainSimulator\EtcTrainSimConfigHandler.h"
#include "..\..\TrainSimulator\AutoSendDataModel.h"

namespace TA_IRS_App
{
	PECCommandProcessor::PECCommandProcessor(IPACMData& paCMData)
		:TrainCommandProcessorThread(paCMData)
	{
		FUNCTION_ENTRY("PECCommandProcessor");
		FUNCTION_EXIT;
	}

	PECCommandProcessor::~PECCommandProcessor()
	{
		FUNCTION_ENTRY("~TrainCommandProcessorThread");

		//Stop the thread
		terminateAndWait();

		FUNCTION_EXIT;
	}

	void PECCommandProcessor::sendSDSMessage(boost::shared_ptr<SDSMessageData>& sdsMessage)
	{
		FUNCTION_ENTRY("sendSDSMessage");

		addTrainCommand(sdsMessage);

		FUNCTION_EXIT;
	}

	void PECCommandProcessor::processTrainCommand(boost::shared_ptr<SDSMessageData>& sdsMessage)
	{
		FUNCTION_ENTRY("processTrainCommand");

	
			SDSMessageData::VMessage pData = sdsMessage->getData();
			SDSMessageData::VMessage resp = sdsMessage->getResponse();
			int userDataIndex = resp.size();
			bool isValidCommand = false;
			
			IConfigDataItemPtrType autoSendSharedPtr = EtcTrainSimConfigHandler::getInstance()->getAutoSendingData(EAutoSendType::EPecType);
			PecAutoSendInfo * autoSendPtr = (PecAutoSendInfo *)autoSendSharedPtr.get();
			
			IConfigDataItemPtrType autoSendSharedPtrCO = EtcTrainSimConfigHandler::getInstance()->getAutoSendingData(EAutoSendType::EChangeOver);
			ChangeOverConfig * autoSendPtrCO = (ChangeOverConfig *)autoSendSharedPtrCO.get();
			
			bool bMpuChangeOverResp = false;

			switch( sdsMessage->getCommandType())
			{
			case PEC_ANSWER:
				
				if(!autoSendPtrCO->getPECAnsStatusFailure())
				{
					m_logger->setLogView("Receive PEC Answer : Command : 0x33");
					resp.push_back(USER_DATA_FLAG);
					resp.push_back(PEC_COMMAND_RECEIVED);
					resp.push_back( pData[23] ); // physical train number
					resp.push_back(10); // Length
					resp.push_back(0); // Origin, TIMS = 0
					resp.push_back(pData[26]);
					resp.push_back(pData[27]);
					resp.push_back(1); // status 6, 1=proceeding

					if (autoSendPtr->getAutoSendPecAnswer())
					{	
						isValidCommand = true;
					}
					else
					{
						m_logger->setLogView("Not Sending Auto Response for PEC Answer Command");
					}

				}
				else
				{
					isValidCommand = false;

					m_logger->setLogView("Returning ChangeOver in response for the PECCommandRecv");					
					
					resp.push_back(USER_DATA_FLAG);
					resp.push_back(0x41);
					resp.push_back( pData[23] ); // physical train number
					resp.push_back(9); // Length
					resp.push_back(0); // Origin, TIMS = 0
					resp.push_back(0); // status 5, to do
					resp.push_back(1);

					bMpuChangeOverResp = true;
				}
				
				break;

			case PEC_RESET:
				m_logger->setLogView("Receive PEC Reset: Command : 0x34");
				resp.push_back(USER_DATA_FLAG);
				resp.push_back(PEC_RESET_RECEIVED);
				resp.push_back( pData[23] ); // physical train number
				resp.push_back(10); // Length
				resp.push_back(0); // Origin, TIMS = 0
				resp.push_back(pData[26]);
				resp.push_back(pData[27]);
				resp.push_back(1); // status 6, 1=proceeding
				
				if (autoSendPtr->getAutoSendPecReset())
				{
					isValidCommand = true;
				}
				else
				{
					m_logger->setLogView("Not Sending Auto Response for PEC Answer Command");
				}

				break;
				//case PEC_ACTIVATED:
				//case PEC_ANSWERED_BY_DRIVER:
				//case PEC_COMMAND_RECEIVED:
				//case READY_FOR_PEC_CONVERSATION:
			case PEC_HOLD:
				m_logger->setLogView("Receive PEC Hold : Command : 0x3C");
				resp.push_back(USER_DATA_FLAG);
				resp.push_back(PEC_ACTIVATED);
				resp.push_back( pData[23] ); // physical train number
				resp.push_back(9); // Length
				resp.push_back(0); // Origin, TIMS = 0
				resp.push_back(pData[26]);
				resp.push_back(pData[27]);
				//	resp.push_back(1); // status 6, 1=proceeding
				if (autoSendPtr->getAutoSendPecHold())
				{
					isValidCommand = true;
				}
				else
				{
					m_logger->setLogView("Not Sending Auto Response for PEC Answer Command");
				}

				break ;
			}

			
			if ( true == isValidCommand )
			{
				int _trainID = static_cast<int>(pData[23]);

				//send first response
				processSendData(userDataIndex,resp,pData);

				//Check the Timer Strategy here
				if ( TIMER_SEND == getStrategyType(_trainID) || TIMER_DEFAULT == getStrategyType(_trainID) )
				{
					//apply timer strategy here
					applyTimerStategy(_trainID);

					//some command need to send two command response
					//give a lil bit delay to process the second comand
					TA_Base_Core::Thread::sleep(100);
					processSecondResponse(pData);
				}
						
			}
			else if (bMpuChangeOverResp)
			{
				int _trainID = static_cast<int>(pData[23]);

				//send first response
				processSendData(userDataIndex,resp,pData);
			}

		FUNCTION_EXIT;
	}


	void PECCommandProcessor::buildCommandReceived(int userDataIndex, std::vector<unsigned char>& resp, const std::vector<unsigned char>& pData)
	{

		std::vector<unsigned char> rawData;
		std::vector<unsigned char> tempData;

		rawData.insert(rawData.end(), resp.begin()+userDataIndex, resp.end());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing CRC\n");
		m_radioCommsObserver->printData(rawData, false);

		unsigned short calcCRC = TA_IRS_Bus::FrameCheckSequence::calculate(rawData,rawData.size());
		tempData = CMHelper::encode_u16(calcCRC);
		//resp.insert(resp.end(), tempData.begin(), tempData.end());
		resp.insert(resp.end(), tempData.at(1));
		resp.insert(resp.end(), tempData.at(0));

		// calculate TL Length
		int decLength = (resp.size()-userDataIndex +6 )*8;

		std::vector<unsigned char>::iterator iter;
		iter = resp.begin() + CM_SDTS_DATA_TLLENGTH_INDEX -2;
		resp.insert(iter, (unsigned char)  (decLength & 0xFF00) >> 8);
		iter = resp.begin() + CM_SDTS_DATA_TLLENGTH_INDEX -2+1;
		//resp[CM_SDTS_DATA_TLLENGTH_INDEX] = (decLength & 0xFF00) >> 8;
		resp.insert(iter, (unsigned char)  (decLength & 0x00FF));
		//resp[CM_SDTS_DATA_TLLENGTH_INDEX+1] = (decLength & 0x00FF);	
		// Adaptation length, 2 bytes at beginning
		unsigned int nAdaptation = resp.size();
		tempData = CMHelper::encode_u16(nAdaptation);
		resp.insert( resp.begin(), tempData.begin(), tempData.end() );


		m_logger->setLogView("Sending PEC Command Received Response to Radio Agent..");

		m_cmData.logger(resp);
		// send the data
		m_radioCommsObserver->sendData(resp);

	}



	void PECCommandProcessor::buildPecReadyResponse(const std::vector<unsigned char>& pData)
	{
		FUNCTION_ENTRY("buildPecReadyResponse");

		std::vector<unsigned char> resp;
		std::vector<unsigned char> tempData;
		int userDataIndex = 0;

		// CM PDU Type
		tempData = CMHelper::encodeCM_PDUType(CMPDUTYPE_DATA_REQ);
		resp.insert( resp.end(), tempData.begin(), tempData.end() );

		// Source Address
		tempData = CMHelper::encodeSSI( m_cmData.getCMData().getDstAddr() );
		resp.insert( resp.end(), tempData.begin(), tempData.end() );

		// Destination Address
		tempData = CMHelper::encodeSSI( m_cmData.getCMData().getSrcAddr() );
		resp.insert( resp.end(), tempData.begin(), tempData.end() );

		// Protocol Identifier
		tempData = CMHelper::encodeCM_ProtocolIdentifier( m_cmData.getCMData().getProtocolId() );
		resp.insert( resp.end(), tempData.begin(), tempData.end() );

		// Flags
		resp.push_back(m_cmData.getCMData().getFlags());
		//resp.insert( resp.end(), m_cmData.getFlags());

		// Message Reference
		resp.push_back(m_cmData.getCMData().getMsgRef());
		//resp.insert( resp.end(), m_cmData.getMsgRef());

		// Area Slection
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Preparing Area Selection");
		resp.push_back(m_cmData.getCMData().getAreaSel());
		//resp.insert( resp.end(), m_cmData.getAreaSel());

		// TL Length = 0, to be determined later
		//resp.push_back(0); resp.push_back(0);


		// TL Header
		//resp.push_back(0);
		//resp.push_back(0);
		for (int i = 0; i < m_cmData.getTLHeader().size(); i+=2) {
			resp.push_back(CMHelper::Hex2Int(m_cmData.getTLHeader().at(i),1)+CMHelper::Hex2Int(m_cmData.getTLHeader().at(i+1),0));
		}

		// check CRC16
		std::vector<unsigned char> rawData;
		unsigned short calcCRC;
		userDataIndex = resp.size();

		resp.push_back(USER_DATA_FLAG);
		resp.push_back(READY_FOR_PEC_CONVERSATION);
		resp.push_back( pData[23] ); // physical train number
		resp.push_back(9); // Length
		resp.push_back(0); // Origin, TIMS = 0
		resp.push_back(pData[26]);
		resp.push_back(pData[27]);
		//resp.push_back(CMHelper::Hex2Int('0',1)+CMHelper::Hex2Int('7',0));

		rawData.clear();
		rawData.insert(rawData.end(), resp.begin()+userDataIndex, resp.end());
		calcCRC = TA_IRS_Bus::FrameCheckSequence::calculate(rawData,rawData.size());
		tempData = CMHelper::encode_u16(calcCRC);
		//resp.insert(resp.end(), tempData.begin(), tempData.end());
		resp.insert(resp.end(), tempData.at(1));
		resp.insert(resp.end(), tempData.at(0));

		// calculate TL Length
		int decLength = (resp.size()-userDataIndex +6 )*8;

		std::vector<unsigned char>::iterator iter;
		iter = resp.begin() + CM_SDTS_DATA_TLLENGTH_INDEX -2;
		resp.insert(iter, (unsigned char)  (decLength & 0xFF00) >> 8);
		iter = resp.begin() + CM_SDTS_DATA_TLLENGTH_INDEX -2+1;
		//resp[CM_SDTS_DATA_TLLENGTH_INDEX] = (decLength & 0xFF00) >> 8;
		resp.insert(iter, (unsigned char)  (decLength & 0x00FF));
		//resp[CM_SDTS_DATA_TLLENGTH_INDEX+1] = (decLength & 0x00FF);	
		// Adaptation length, 2 bytes at beginning
		unsigned int nAdaptation = resp.size();
		tempData = CMHelper::encode_u16(nAdaptation);
		//resp.insert(resp.end(), tempData.begin(), tempData.end());

		std::vector<unsigned char> finalData;
		finalData.insert(finalData.end(),tempData.begin(),tempData.end());

		finalData.insert(finalData.end(),resp.begin(),resp.end());

		printf("Sending PEC Reset Response to Radio Agent\n");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Sending PEC Ready Response to Radio Agent\n");


		m_logger->setLogView("Sending PEC Ready for Conversation Response to Radio Agent");

		m_cmData.logger(finalData);
		// send the data
		m_radioCommsObserver->sendData(finalData);

		FUNCTION_EXIT;
	}

	void PECCommandProcessor::processSecondResponse(const std::vector<unsigned char>& pData)
	{
		FUNCTION_ENTRY("processSecondResponse");

		switch (pData[USER_DATA_COMMAND_INDEX]) 
		{
		case PEC_ANSWER:
			buildPecReadyResponse(pData);
			break;
		}

		FUNCTION_EXIT;
	}

}