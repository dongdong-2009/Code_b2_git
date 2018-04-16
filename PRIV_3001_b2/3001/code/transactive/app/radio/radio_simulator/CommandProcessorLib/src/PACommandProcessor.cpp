/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\3001_TIP\3001\transactive\app\radio\radio_simulator\CommandProcessorLib\src\PACommandProcessor.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/16/08 10:05:25 $
  * Last modified by:  $Author: builder $
  *
  * This class is used for PA command processing.
  */

#include "app/radio/radio_simulator/CommandProcessorLib/src/PACommandProcessor.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/LivePaReponseThread.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/PreRecordedPaTimeOutThread.h"
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMHelper.h"
//#include "app/radio/radio_simulator/src/FrameCheckSequence.h"
#include "app/radio/radio_simulator/TrainSimulator/FrameCheckSequence.h"
#include "core/utilities/src/DebugUtil.h"


//#include "..\..\TrainSimulator\EtcTrainSimConfigHandler.h"
//#include "..\..\TrainSimulator\AutoSendDataModel.h"


namespace TA_IRS_App
{
	long PACommandProcessor::m_lTrainCount = 0;

	PACommandProcessor::PACommandProcessor( IPACMData& paCMData )
		: TrainCommandProcessorThread(paCMData),
		  m_bMaxReach (FALSE)
	{
		FUNCTION_ENTRY("PACommandProcessor");
		FUNCTION_EXIT;
	}

	PACommandProcessor::~PACommandProcessor()
	{
		FUNCTION_ENTRY("~TrainCommandProcessorThread");

		//Stop the thread
		terminateAndWait();

		FUNCTION_EXIT;
	}

	void PACommandProcessor::sendSDSMessage(boost::shared_ptr<SDSMessageData>& sdsMessage)
	{
		FUNCTION_ENTRY("sendSDSMessage");

		addTrainCommand(sdsMessage);

		FUNCTION_EXIT;
	}

	void PACommandProcessor::processTrainCommand(boost::shared_ptr<SDSMessageData>& sdsMessage)
	{
		FUNCTION_ENTRY("processTrainCommand");

		SDSMessageData::VMessage pData = sdsMessage->getData();
		SDSMessageData::VMessage resp = sdsMessage->getResponse();
		int userDataIndex = resp.size();
		bool isValidCommand = false;
		
		IConfigDataItemPtrType autoSendSharedPtr = EtcTrainSimConfigHandler::getInstance()->getAutoSendingData(EAutoSendType::EPAType);
		PAAutoSendInfo * autoSendPtr = (PAAutoSendInfo *)autoSendSharedPtr.get();
		
		IConfigDataItemPtrType autoSendSharedPtrCO = EtcTrainSimConfigHandler::getInstance()->getAutoSendingData(EAutoSendType::EChangeOver);
		IConfigDataItemPtrType autoSendSharedTrainCnt = EtcTrainSimConfigHandler::getInstance()->getAutoSendingData(EAutoSendType::ETrainCount);
		
		ChangeOverConfig * autoSendPtrCO = (ChangeOverConfig *)autoSendSharedPtrCO.get();
		TrainCountForAutoSend * trainCountCtrl = (TrainCountForAutoSend *) autoSendSharedTrainCnt.get();
		
		bool bMpuChangeOverResp = false;
		
		if (m_bMaxReach)
		{
			
			// the config was change becuase the flag already
			// set to max but conditions says the count number now is not
			// equal to our counter
			if (trainCountCtrl->getTrainCountNumber() != m_lTrainCount)
			{
				m_lTrainCount = 0;
				m_bMaxReach = false;
			}
		}

		switch( sdsMessage->getCommandType() )
		{
			case PA_LIVE_ANNOUNCEMENT:

			if (!autoSendPtrCO->getPALiveStatusFailure())
			{
				m_logger->setLogView("Receive PA Live Announcement : Command : 0x0B");
				resp.push_back(USER_DATA_FLAG);
				resp.push_back(PA_COMMAND_RECEIVED);
				resp.push_back( pData[23] ); // physical train number
				resp.push_back(15); // Length
				resp.push_back(0); // Origin, TIMS = 0
				resp.push_back( 0 ); // status 2
				resp.push_back(OC1_PA_LIVE_ANNOUNCEMENT);
				resp.push_back(1); // status 3, need simulated thread
				resp.push_back(0);
				resp.push_back(0);
				resp.push_back(0);
				resp.push_back(0);
				resp.push_back(pData[27]);
				// calculate check sum and send the message
				
				if (autoSendPtr->getAutoSendPALive())
				{
					isValidCommand = true;
// 					if (trainCountCtrl->getTrainCountNumber() != ALLTRAINNUM)
// 					{
// 						if (trainCountCtrl->getTrainCountNumber() != m_lTrainCount)
// 						{
// 							m_lTrainCount++;
// 							isValidCommand = true;
// 						}
// 						else
// 						{
// 							m_bMaxReach = true;
// 						}
// 
// 					}
// 					else
// 					{
// 						m_lTrainCount = 0;
// 						isValidCommand = true;
// 					}
				}
				
			}
			else
			{
				isValidCommand = false;

				m_logger->setLogView("Returning ChangeOver in response for the PA LiveCommand");					

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
		case PA_PRERECORDED_ANNOUNCEMENT:

			if (!autoSendPtrCO->getPALiveStatusFailure())
			{
				m_logger->setLogView("Receive Pre-recorded Announcement : Command : 0x0C");
				resp.push_back(USER_DATA_FLAG);
				resp.push_back(PA_COMMAND_RECEIVED);
				resp.push_back( pData[23] ); // physical train number
				resp.push_back(15); // Length
				resp.push_back(0); // Origin, TIMS = 0
				resp.push_back(0); // status 2
				resp.push_back(OC1_PA_PRERECORDED_ANNOUNCEMENT);
				resp.push_back(1); // status 3, need simulated thread
				resp.push_back(pData[30]);
				resp.push_back(pData[31]);
				resp.push_back(pData[32]);
				resp.push_back(pData[33]);
				resp.push_back(pData[34]); // announcement id
				// calculate check sum and send the message
				if (autoSendPtr->getAutoSendPAPreRecorded())
					isValidCommand = true;
			}
			else 
			{
				isValidCommand = false;

				m_logger->setLogView("Returning ChangeOver in response for the PA PreRecCommand");					

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

		case PA_LIVE_RESET:
			{

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "receive pa live reset, id = %d\n", pData[27] - 0);

				m_logger->setLogView("Receive PA Live Resest : Command : 0x0E");
				resp.push_back(USER_DATA_FLAG);
				resp.push_back(PA_COMMAND_RECEIVED);
				resp.push_back( pData[23] ); // physical train number
				resp.push_back(15); // Length
				resp.push_back(0); // Origin, TIMS = 0
				resp.push_back(0); // status 2
				resp.push_back(OC1_LIVE_PA_RESET);
				resp.push_back(5);
				resp.push_back(0);
				resp.push_back(0);
				resp.push_back(0);
				resp.push_back(0);
				resp.push_back(pData[27]); // announcement id
				// calculate check sum and send the message

				// Terminate Pa live thread
// 				std::vector<unsigned char> terminateResp;
// 				terminateResp.resize(24);
// 				terminateResp[23] = 0xFF;
// 				LivePaReponseThread::LiveRespThreadId threadId = std::make_pair(pData[27], pData[24]);
// 				LivePaReponseThread::insertRequestResetResponse(threadId, terminateResp);
				
				if (autoSendPtr->getAutoSendPALiveReset())
					isValidCommand = true;

				break;
			}

		case PA_PRERECORDED_RESET: // For new protocal
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "receive pa prerecord reset, id = %d\n", pData[27] - 0);

				m_logger->setLogView("Receive PA Pre-recorded reset : Command : 0x0F");
				resp.push_back(USER_DATA_FLAG);
				resp.push_back(PA_COMMAND_RECEIVED);
				resp.push_back( pData[23] ); // physical train number
				resp.push_back(15); // Length
				resp.push_back(0); // Origin, TIMS = 0
				resp.push_back(0); // status 2
				resp.push_back(OC1_DVA_PA_RESET);
				resp.push_back(5); // status 3, need simulated thread TODO: for new protocal change to 5
				resp.push_back(0);
				resp.push_back(0);
				resp.push_back(0);
				resp.push_back(0);
				resp.push_back(pData[26]); // announcement id
				
				if (autoSendPtr->getAutoSendPAPreRecordedReset())
					isValidCommand = true;

				
				break;
			}

		case PA_CONTINUE:

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Receive continue resp id = %d\n", pData[27] - 0);
			//resp.push_back(CMHelper::Hex2Int('0',1)+CMHelper::Hex2Int('7',0));

			m_logger->setLogView("Receive PA Continue : Command : 0x0F");
			resp.push_back(USER_DATA_FLAG);
			resp.push_back(READY_FOR_LIVE_ANNOUNCEMENT);
			resp.push_back(pData[23]); // physical train number
			resp.push_back(0); // Length
			resp.push_back(0); // Origin, TIMS = 0
			resp.push_back(pData[27]);
			// calculate check sum and send the message
			buildPALiveRequestResetResponse(pData);
			
			if (autoSendPtr->getAutoSendPAContinue())
				isValidCommand = true;

			break;
		}

		if ( true == isValidCommand)
		{
			int _trainID = static_cast<int>(pData[23]);

			//Check the Timer Strategy here
			if ( TIMER_SEND == getStrategyType(_trainID) || TIMER_DEFAULT == getStrategyType(_trainID) )
			{
				//apply timer strategy here
				applyTimerStategy(_trainID);
				
				processSendData(userDataIndex,resp,pData);

				//some command need to send two command response
				//put a little bit delay to process the second command
				TA_Base_Core::Thread::sleep(100);

				if (!autoSendPtr->getAutoSendPAPreRecCompl()&& 
					sdsMessage->getCommandType() == PA_PRERECORDED_ANNOUNCEMENT )
				{
				}
				else
				{	
					// for abort button to be enable

					processSecondResponse(pData);
				}
			}
		}
		else if (bMpuChangeOverResp)
		{
			int _trainID = static_cast<int>(pData[23]);
			processSendData(userDataIndex,resp,pData);
			
		}
		
		FUNCTION_EXIT;
	}

	void PACommandProcessor::processSecondResponse(const std::vector<unsigned char>& pData)
	{
		FUNCTION_ENTRY("processSecondResponse");

		switch (pData[USER_DATA_COMMAND_INDEX])
		{	
		case PA_LIVE_ANNOUNCEMENT:
			{
				buildPAReadyResponse(pData);
				//put a little bit delay
				TA_Base_Core::Thread::sleep(100);
				buildPALiveRequestResetResponse(pData);
				//buildLivePaReset(pData);
// 				LivePaReponseThread::LiveRespThreadId threadId = std::make_pair(pData[27], pData[23]);
// 				LivePaReponseThread* pThread = new LivePaReponseThread(threadId, *m_radioCommsObserver , livaPaReset);
// 				pThread->start();
				break ;
			}
		case PA_PRERECORDED_ANNOUNCEMENT:
			{
				if ( 1 == pData[26] ) // For normal announcement, reply directly
				{
					buildPAMessageCompleteResponse(pData, pData[34]);
				}
				else if ( 2 == pData[26]) // For emergency announcement, reply until timeout
				{
					//send first response
					// send second response for emergency 
					// when emergency auto send is enable
					IConfigDataItemPtrType autoSendSharedPtr = EtcTrainSimConfigHandler::getInstance()->getAutoSendingData(EAutoSendType::EPAType);
					PAAutoSendInfo * autoSendPtr = (PAAutoSendInfo *)autoSendSharedPtr.get();

					if (autoSendPtr->getAutoSendPAPreRecEmergency())
					{
						std::vector<unsigned char> PreRecordedPaTimeOutResp;
						buildPreRecordedPaTimeOutResponse(pData, PreRecordedPaTimeOutResp);
					}

					// New a thread to wait abort operation until timeout
// 					PreRecordedPaTimeoutThread::PaRecordedTimeoutThreadId threadId = std::make_pair(pData[34], pData[23]); // Nes protocal
// 					PreRecordedPaTimeoutThread* pThread = new PreRecordedPaTimeoutThread(threadId, m_radioCommsObserver, PreRecordedPaTimeOutResp);
// 					pThread->start();
				}
				break ;
			}
		}

		LivePaReponseThread::cleanUpUnusefulThread();
		PreRecordedPaTimeoutThread::cleanUpUnusefulThread();

		FUNCTION_EXIT;
	}

	void PACommandProcessor::buildPALiveRequestResetResponse(const std::vector<unsigned char>& pData)
	{
		FUNCTION_ENTRY("buildPALiveRequestResetResponse");

		std::vector<unsigned char> resp;
		std::vector<unsigned char> tempData;
		int userDataIndex = 0;
		unsigned char announcementId;

		// Get announcementId
		if ( PA_LIVE_ANNOUNCEMENT == pData[22])
		{
			announcementId = pData[27];
		} 
		else if ( PA_CONTINUE == pData[22])
		{
			announcementId = pData[26];
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Invaild command type\n");
		}

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

		//resp.push_back(CMHelper::Hex2Int('0',1)+CMHelper::Hex2Int('7',0));
		resp.push_back(USER_DATA_FLAG);
		resp.push_back(REQUEST_FOR_PA_RESET);
		resp.push_back( pData[23] ); // physical train number
		resp.push_back(8); // Length
		resp.push_back(0); // Origin, TIMS = 0
		resp.push_back(announcementId); // Announcement ID

		rawData.clear();
		rawData.insert(rawData.end(), resp.begin()+userDataIndex, resp.end());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing CRC\n");
		m_radioCommsObserver->printData(rawData, false);
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
		//resp.insert(resp.begin(), tempData.begin(), tempData.end());

		std::vector<unsigned char> finalData;
		finalData.insert(finalData.end(),tempData.begin(),tempData.end());

		finalData.insert(finalData.end(),resp.begin(),resp.end());

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Prepare request for PA reset id = %d\n", announcementId - 0);


		//printData(resp,false);

		//LivePaReponseThread::LiveRespThreadId threadId = std::make_pair(announcementId, pData[23]);
		//LivePaReponseThread::insertRequestResetResponse(threadId, finalData);

 		m_logger->setLogView("Sending Request for PA Reset to Radio Agent..");
// 
 		m_cmData.logger(finalData);
// 		// send the data
		m_radioCommsObserver->sendData(finalData);

		FUNCTION_EXIT;
	}

	void PACommandProcessor::buildPAReadyResponse(const std::vector<unsigned char>& pData)
	{
		FUNCTION_ENTRY("buildPAReadyResponse");

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

		//resp.push_back(CMHelper::Hex2Int('0',1)+CMHelper::Hex2Int('7',0));
		resp.push_back(USER_DATA_FLAG);
		resp.push_back(READY_FOR_LIVE_ANNOUNCEMENT);
		resp.push_back( pData[23] ); // physical train number
		resp.push_back(8); // Length
		resp.push_back(0); // Origin, TIMS = 0
		resp.push_back(pData[27]); // Announcement ID

		rawData.clear();
		rawData.insert(rawData.end(), resp.begin()+userDataIndex, resp.end());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing CRC\n");
		m_radioCommsObserver->printData(rawData, false);
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
		//resp.insert(resp.begin(), tempData.begin(), tempData.end());

		std::vector<unsigned char> finalData;
		finalData.insert(finalData.end(),tempData.begin(),tempData.end());

		finalData.insert(finalData.end(),resp.begin(),resp.end());


		//printf("\nSending Response (Message Type: %x):", resp[23]);
		//for(unsigned int j=0; j<resp.size(); j++)
		//{
		//printf("%3x ", resp[j]);
		//}
		
		printf("Sending PA Ready Response to Radio Agent:\n");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Sending PA Ready Response id = %d to Radio Agent:\n", pData[28] - 0);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Sending PA Ready Response to Radio Agent:\n");

		m_logger->setLogView("Sending PA Ready Response to Radio Agent..");

		m_cmData.logger(finalData);
		// send the data
		m_radioCommsObserver->sendData(finalData);

		FUNCTION_EXIT;
	}

	void PACommandProcessor::buildLivePaReset(const std::vector<unsigned char>& pData, bool badRespons)
	{
		FUNCTION_ENTRY("buildLivePaReset");

		std::vector<unsigned char> tempData;
		std::vector<unsigned char> resp;
		int userDataIndex = 0;
		unsigned char announcementId;

		// Get announcementId
		/*
		if ( PA_LIVE_ANNOUNCEMENT == pData[22])
		{
			announcementId = pData[27];
		} 
		else if ( PA_CONTINUE == pData[22])
		{
			announcementId = pData[27];
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Invaild command type\n");
		}
		*/

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

		//resp.push_back(CMHelper::Hex2Int('0',1)+CMHelper::Hex2Int('7',0));
		resp.push_back(USER_DATA_FLAG);
		resp.push_back(PA_LIVE_RESET);
		resp.push_back( pData[23] ); // physical train number
		resp.push_back(9); // Length
		resp.push_back(0); // Origin, TIMS = 0
		if(badRespons)
		{
			resp.push_back(6); // pa fail
		}
		else
		{
			resp.push_back(1);
		}
		resp.push_back(pData[27]); // Announcement ID

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
		//resp.insert( resp.begin(), tempData.begin(), tempData.end() );

		std::vector<unsigned char> finalData;
		finalData.insert(finalData.end(),tempData.begin(),tempData.end());

		finalData.insert(finalData.end(),resp.begin(),resp.end());

		resp.clear();
		resp.insert(resp.begin(),finalData.begin(),finalData.end());


		//printf("\nSending Response (Message Type: %x):", resp[23]);
		//for(unsigned int j=0; j<resp.size(); j++)
		//{
		//printf("%3x ", resp[j]);
		//}
		//printf("Sending PA Ready Response to Radio Agent:\n");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Prepare Live PA reset, id = %d\n", pData[28] - 0);

		m_logger->setLogView("Prepare Live PA reset..");

		m_cmData.logger(finalData);

		m_radioCommsObserver->sendData(finalData);

		FUNCTION_EXIT;
	}

	void PACommandProcessor::buildCommandReceived( const std::vector<unsigned char>& pData, bool badResponse /*= false*/ )
	{

		FUNCTION_ENTRY("buildLivePaReset");

		std::vector<unsigned char> tempData;
		std::vector<unsigned char> resp;
		int userDataIndex = 0;
		unsigned char announcementId;

		// Get announcementId
		/*
		if ( PA_LIVE_ANNOUNCEMENT == pData[22])
		{
			announcementId = pData[27];
		} 
		else if ( PA_CONTINUE == pData[22])
		{
			announcementId = pData[27];
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Invaild command type\n");
		}
		*/

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

		//resp.push_back(CMHelper::Hex2Int('0',1)+CMHelper::Hex2Int('7',0));

		resp.push_back(USER_DATA_FLAG);
		resp.push_back(PA_COMMAND_RECEIVED);
		resp.push_back( pData[23] ); // physical train number
		resp.push_back(15); // Length
		resp.push_back(0); // Origin, TIMS = 0
		resp.push_back(0); // status 2
		resp.push_back(OC1_LIVE_PA_RESET);
		if(badResponse)
		{
			resp.push_back(6);
		}
		else
		{
			resp.push_back(1);
		}
		resp.push_back(0);
		resp.push_back(0);
		resp.push_back(0);
		resp.push_back(0);
		resp.push_back(pData[27]); // announcement id

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
		//resp.insert( resp.begin(), tempData.begin(), tempData.end() );

		std::vector<unsigned char> finalData;
		finalData.insert(finalData.end(),tempData.begin(),tempData.end());

		finalData.insert(finalData.end(),resp.begin(),resp.end());

		resp.clear();
		resp.insert(resp.begin(),finalData.begin(),finalData.end());


		//printf("\nSending Response (Message Type: %x):", resp[23]);
		//for(unsigned int j=0; j<resp.size(); j++)
		//{
		//printf("%3x ", resp[j]);
		//}
		//printf("Sending PA Ready Response to Radio Agent:\n");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Prepare Live PA reset, id = %d\n", pData[28] - 0);

		m_logger->setLogView("Prepare Live PA reset..");

		m_cmData.logger(finalData);

		m_radioCommsObserver->sendData(finalData);

		FUNCTION_EXIT;
	}

	void PACommandProcessor::buildPAMessageCompleteResponse(const std::vector<unsigned char>& pData, unsigned char anouncementid)
	{
		FUNCTION_ENTRY("buildPAMessageCompleteResponse");

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

		//resp.push_back(CMHelper::Hex2Int('0',1)+CMHelper::Hex2Int('7',0));
		resp.push_back(USER_DATA_FLAG);
		resp.push_back(TRAIN_PA_MESSAGE_COMPLETE);
		resp.push_back( pData[23] ); // physical train number
		resp.push_back(9); // Length
		resp.push_back(0); // Origin, TIMS = 0
		resp.push_back(1); //status 4
		resp.push_back(anouncementid); // Announcement ID //TODO: different protocol

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


		//printf("\nSending Response (Message Type: %x):", resp[23]);
		//for(unsigned int j=0; j<resp.size(); j++)
		//{
		//printf("%3x ", resp[j]);
		//}
		
		printf("Sending PA Message Complete Response to Radio Agent\n");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Sending PA Message Complete Response to Radio Agent\n");


		m_logger->setLogView("Sending PA Message Complete Response to Radio Agent..");

		m_cmData.logger(finalData);
		// send the data
		m_radioCommsObserver->sendData(finalData);

		FUNCTION_EXIT;
	}

	void PACommandProcessor::buildPreRecordedPaTimeOutResponse(const std::vector<unsigned char>& pData, std::vector<unsigned char>& resp)
	{
		FUNCTION_ENTRY("buildPreRecordedPaTimeOutResponse");

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

		//resp.push_back(CMHelper::Hex2Int('0',1)+CMHelper::Hex2Int('7',0));
		resp.push_back(USER_DATA_FLAG);
		resp.push_back(TRAIN_PA_MESSAGE_COMPLETE);
		resp.push_back( pData[23] ); // physical train number
		resp.push_back(9); // Lengths
		resp.push_back(0); // Origin, TIMS = 0
		resp.push_back(1); // status 4 
		resp.push_back(pData[34]); // Announcement ID //TODO: different protocol

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

		//printf("\nSending Response (Message Type: %x):", resp[23]);
		//for(unsigned int j=0; j<resp.size(); j++)
		//{
		//printf("%3x ", resp[j]);
		//}
		//printf("Sending PA Ready Response to Radio Agent:\n");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Prepare Pre-recorded PA reset, id = %d(%d)\n", pData[35] - 0, pData[24] - 0);


		m_logger->setLogView("Sending PA Message Complete Response to Radio Agent..");

		m_cmData.logger(finalData);
		// send the data
		m_radioCommsObserver->sendData(finalData);

		FUNCTION_EXIT;
	}


}