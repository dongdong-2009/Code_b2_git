/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/bus/signalling/SignallingProtocolLibrary/src/IncomingTelegramProcessor.cpp $
  * @author:  Mark See
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of the Class IncomingTelegramProcessor
  */
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/alarm/public_interfaces/ItaAlarms.h"
#include "core/message/types/SignallingAgentAlarms_MessageTypes.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "bus/signalling/SignallingProtocolLibrary/src/IncomingTelegramProcessor.h"
#include "bus/signalling/SignallingProtocolLibrary/src/TelegramException.h"
// #include "bus/signalling/SignallingProtocolLibrary/src/AckTelegram.h"
 #include "bus/signalling/SignallingProtocolLibrary/src/IncomingTelegramConstants.h"
//Message Poll Telegram
#include "bus/signalling/SignallingProtocolLibrary/src/MessagePollTelegram.h"

//Train Location Telegram
#include "bus/signalling/SignallingProtocolLibrary/src/TrainLocationTelegram.h"

//Block Info Telegram
#include "bus/signalling/SignallingProtocolLibrary/src/BlockInfoTelegram.h"

//Dynamic Train Data Telegram
#include "bus/signalling/SignallingProtocolLibrary/src/DynamicTrainDataTelegram.h"

//Signal Status Telegram
#include "bus/signalling/SignallingProtocolLibrary/src/SignalStatusTelegram.h"

//Train Pis Info Telegram
#include "bus/signalling/SignallingProtocolLibrary/src/TrainPisInfoTelegram.h"

//Planned Telegram
#include "bus/signalling/SignallingProtocolLibrary/src/PlannedTelegramStart.h"
#include "bus/signalling/SignallingProtocolLibrary/src/PlannedTelegramContent.h"
#include "bus/signalling/SignallingProtocolLibrary/src/PlannedTelegramEnd.h"

//Practical Telegram
#include "bus/signalling/SignallingProtocolLibrary/src/PracticalTelegramStart.h"
#include "bus/signalling/SignallingProtocolLibrary/src/PracticalTelegramContent.h"
#include "bus/signalling/SignallingProtocolLibrary/src/PracticalTelegramEnd.h"

//Planned Telegram Interface
#include "bus/signalling/SignallingProtocolLibrary/src/IPlannedStartReceiver.h"
#include "bus/signalling/SignallingProtocolLibrary/src/IPlannedContentReceiver.h"
#include "bus/signalling/SignallingProtocolLibrary/src/IPlannedEndReceiver.h"

//Practical Telegram Interface
#include "bus/signalling/SignallingProtocolLibrary/src/IPracticalStartReceiver.h"
#include "bus/signalling/SignallingProtocolLibrary/src/IPracticalContentReceiver.h"
#include "bus/signalling/SignallingProtocolLibrary/src/IPracticalEndReceiver.h"

#include "bus/signalling/SignallingProtocolLibrary/src/ITelegramSender.h"
#include "bus/signalling/SignallingProtocolLibrary/src/IMessagePollReceiver.h"
#include "bus/signalling/SignallingProtocolLibrary/src/ITrainLocationReceiver.h"
#include "bus/signalling/SignallingProtocolLibrary/src/IBlockInfoReceiver.h"
#include "bus/signalling/SignallingProtocolLibrary/src/IDynamicTrainDataReceiver.h"
#include "bus/signalling/SignallingProtocolLibrary/src/ISignalStatusReceiver.h"
#include "bus/signalling/SignallingProtocolLibrary/src/ITrainPisInfoReceiver.h"


#include "bus/signalling/SignallingProtocolLibrary/src/TelegramFilter.h"
#include "bus/signalling/SignallingProtocolLibrary/src/HelperLogs.h"

using namespace std;
using namespace TA_IRS_Bus;


namespace
{
    typedef std::vector<boost::shared_ptr<Telegram> > TELEGRAM_PTR_LIST;
    typedef std::map<Telegram::EInterfaceType, TELEGRAM_PTR_LIST > TELEGRAM_MAP;
}

IncomingTelegramProcessor::IncomingTelegramProcessor
(TA_Base_Core::ItaAlarms& taAlarms,
 IMessagePollReceiver& messagePollReceiver,
 ITrainLocationReceiver& trainLocation,
 IBlockInfoReceiver&             blockInfo,
 IDynamicTrainDataReceiver&      dynamicTrainData,
 ISignalStatusReceiver&          signalStatus,
 ITrainPisInfoReceiver&          trainPisInfo,
 IPlannedStartReceiver& plannedStart,
 IPlannedContentReceiver& plannedContent,
 IPlannedEndReceiver& plannedEnd,
 IPracticalStartReceiver& practicalStart,
 IPracticalContentReceiver& practicalContent,
 IPracticalEndReceiver& practicalEnd,
 ITelegramSender&            telegramSender
 )
:m_taAlarms(taAlarms),
 m_invalidAlarmId(""),
 m_messagePollReceiver(messagePollReceiver),
 m_trainLocationReceiver(trainLocation),
 m_blockInfoReceiver(blockInfo),
 m_dynamicTrainDataReceiver(dynamicTrainData),
 m_signalStatusReceiver(signalStatus),
 m_trainPisInfoReceiver(trainPisInfo),
 m_telegramSender(telegramSender),
 m_isRunning(false),
 m_semaphore(0),
 m_agentInControl(false),
 m_plannedStartReceiver(plannedStart),
 m_plannedContentReceiver(plannedContent),
 m_plannedEndReceiver(plannedEnd),
 m_practicalStartReceiver(practicalStart),
 m_practicalContentReceiver(practicalContent),
 m_practicalEndReceiver(practicalEnd)
{
    FUNCTION_ENTRY("IncomingTelegramProcessor(const IThirdRailTelegramReceiver& thirdRailReceiver, const IRadioTelegramReceiver& radioReceiver, const IPidsTelegramReceiver& pidsReceiver, const ILessTelegramReceiver& lessReceiver,const IPtiTelegramReceiver& ptiReceiver, const ITelegramSender& telegramSender)");
    TELEGRAM_PTR_LIST tempList;
//     m_telegramMap.insert(TELEGRAM_MAP::value_type(Telegram::ThirdRailInterface, tempList));
//     m_telegramMap.insert(TELEGRAM_MAP::value_type(Telegram::RadioInterface,     tempList));
//     m_telegramMap.insert(TELEGRAM_MAP::value_type(Telegram::CongestionInterface,tempList));
//     m_telegramMap.insert(TELEGRAM_MAP::value_type(Telegram::PidsInterface,      tempList));

	m_telegramMap.insert(TELEGRAM_MAP::value_type(Telegram::Message_Polling_Interface, tempList));
	m_telegramMap.insert(TELEGRAM_MAP::value_type(Telegram::Train_Location_Interface, tempList));
	m_telegramMap.insert(TELEGRAM_MAP::value_type(Telegram::Block_Information_Interface, tempList));
	m_telegramMap.insert(TELEGRAM_MAP::value_type(Telegram::Dynamic_Train_Data_Interface, tempList));
	m_telegramMap.insert(TELEGRAM_MAP::value_type(Telegram::Signal_Status_Interface, tempList));
	m_telegramMap.insert(TELEGRAM_MAP::value_type(Telegram::Train_PIS_Info_Interface, tempList));
	m_telegramMap.insert(TELEGRAM_MAP::value_type(Telegram::Planned_Data_Start_Interface,tempList));
	m_telegramMap.insert(TELEGRAM_MAP::value_type(Telegram::Planned_Data_Content_Interface,tempList));
	m_telegramMap.insert(TELEGRAM_MAP::value_type(Telegram::Planned_Data_End_Interface,tempList));
	m_telegramMap.insert(TELEGRAM_MAP::value_type(Telegram::Practical_Data_Start_Interface,tempList));
	m_telegramMap.insert(TELEGRAM_MAP::value_type(Telegram::Practical_Data_Content_Interface,tempList));
	m_telegramMap.insert(TELEGRAM_MAP::value_type(Telegram::Practical_Data_End_Interface,tempList));
	

    FUNCTION_EXIT;
}

IncomingTelegramProcessor::~IncomingTelegramProcessor()
{
    FUNCTION_ENTRY("~IncomingTelegramProcessor");
    FUNCTION_EXIT;
}

void IncomingTelegramProcessor::processIncomingData(std::vector<unsigned char>& incomingData)
{
    FUNCTION_ENTRY("processIncomingData");

	{
		//copy data from cache
		TA_Base_Core::ThreadGuard guard(m_incomingDataLock);
		for(std::vector<unsigned char>::iterator it = incomingData.begin(); 
			it != incomingData.end() ; ++it)
		{
			m_incomingDataCache.push_back(*it);
		}

		incomingData.clear();
	}

	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Incoming Data, Size : %d", m_incomingDataCache.size());
	m_semaphore.post();
	
	FUNCTION_EXIT;
}

void IncomingTelegramProcessor::processMessagePollingTelegram(const Telegram& telegram)
{
	FUNCTION_ENTRY("processMessagePollingTelegram");

	boost::shared_ptr<MessagePollTelegram> messagePollTelegram(new MessagePollTelegram(telegram));
	m_telegramMap[Telegram::Message_Polling_Interface].push_back(messagePollTelegram);

	FUNCTION_EXIT;
}

void IncomingTelegramProcessor::processTrainLocationTelegram(const Telegram& telegram)
{
	FUNCTION_ENTRY("processTrainLocationTelegram");
	try
	{
		boost::shared_ptr<TrainLocationTelegram> trainLocation(new TrainLocationTelegram(telegram));
		m_telegramMap[Telegram::Train_Location_Interface].push_back(trainLocation);
	}
	catch (TelegramException &ex)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"%s",ex.what());
	}
	
	FUNCTION_EXIT;
}

void IncomingTelegramProcessor::processBlockInfoTelegram(const Telegram& telegram)
{
	FUNCTION_ENTRY("processBlockInfoTelegram");

	boost::shared_ptr<BlockInfoTelegram> blockInfo(new BlockInfoTelegram(telegram));
	m_telegramMap[Telegram::Block_Information_Interface].push_back(blockInfo);

	FUNCTION_EXIT;
}

void IncomingTelegramProcessor::processDynamicTrainDataTelegram(const Telegram& telegram)
{
	FUNCTION_ENTRY("processDynamicTrainDataTelegram");

	boost::shared_ptr<DynamicTrainDataTelegram> dynamicTrainData(new DynamicTrainDataTelegram(telegram));
	m_telegramMap[Telegram::Dynamic_Train_Data_Interface].push_back(dynamicTrainData);

	FUNCTION_EXIT;
}

void IncomingTelegramProcessor::processSignalStatusTelegram(const Telegram& telegram)
{
	FUNCTION_ENTRY("processSignalStatusTelegram");

	boost::shared_ptr<SignalStatusTelegram> signalStatus(new SignalStatusTelegram(telegram));
	m_telegramMap[Telegram::Signal_Status_Interface].push_back(signalStatus);

	FUNCTION_EXIT;
}

void IncomingTelegramProcessor::processTrainPisInfoTelegram(const Telegram& telegram)
{
	FUNCTION_ENTRY("processSignalStatusTelegram");

	boost::shared_ptr<TrainPisInfoTelegram> trainPisInfo(new TrainPisInfoTelegram(telegram));
	m_telegramMap[Telegram::Train_PIS_Info_Interface].push_back(trainPisInfo);

	FUNCTION_EXIT;
}

void IncomingTelegramProcessor::processPlannedTelegram(const Telegram& telegram)
{
	FUNCTION_ENTRY("processPlannedTelegram");

	ta_uint8 SUB_ID_POSITION = 4;
	try
	{
		int subID = telegram.getWord(SUB_ID_POSITION);

		switch(subID)
		{
		case Telegram::ISCS_DATA_START:
			{
				boost::shared_ptr<PlannedTelegramStart> plannedStart(new PlannedTelegramStart(telegram));
				m_telegramMap[Telegram::Planned_Data_Start_Interface].push_back(plannedStart);
				break;
			}

		case Telegram::ISCS_DATA_CONTENT:
			{
				boost::shared_ptr<PlannedTelegramContent> plannedContent(new PlannedTelegramContent(telegram));
				m_telegramMap[Telegram::Planned_Data_Content_Interface].push_back(plannedContent);
				break;
			}

		case Telegram::ISCS_DATA_END:
			{
				boost::shared_ptr<PlannedTelegramEnd> plannedEnd(new PlannedTelegramEnd(telegram));
				m_telegramMap[Telegram::Planned_Data_End_Interface].push_back(plannedEnd);
				break;
			}

		}
	}
	catch (TelegramException &ex)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"%s",ex.what());
	}
	
	FUNCTION_EXIT;
}

void IncomingTelegramProcessor::processPracticalTelegram(const Telegram& telegram)
{
	FUNCTION_ENTRY("processPracticalTelegram");

	ta_uint8 SUB_ID_POSITION = 4;
	try
	{
		int subID = telegram.getWord(SUB_ID_POSITION);

		switch(subID)
		{
		case Telegram::ISCS_DATA_START:
			{
				boost::shared_ptr<PracticalTelegramStart> practicalStart(new PracticalTelegramStart(telegram));
				m_telegramMap[Telegram::Practical_Data_Start_Interface].push_back(practicalStart);
				break;
			}

		case Telegram::ISCS_DATA_CONTENT:
			{
				boost::shared_ptr<PracticalTelegramContent> practicalContent(new PracticalTelegramContent(telegram));
				m_telegramMap[Telegram::Practical_Data_Content_Interface].push_back(practicalContent);
				break;
			}

		case Telegram::ISCS_DATA_END:
			{
				boost::shared_ptr<PracticalTelegramEnd> practicalEnd(new PracticalTelegramEnd(telegram));
				m_telegramMap[Telegram::Practical_Data_End_Interface].push_back(practicalEnd);
				break;
			}

		}
	}
	catch (TelegramException &ex)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"%s",ex.what());
	}

	FUNCTION_EXIT;
}


//TD 15503
//zhou yuan++
void IncomingTelegramProcessor::raiseInvalidMessageAlarm()
{
	FUNCTION_ENTRY("raiseInvalidMessageAlarm");
	if(m_agentInControl)
	{
		if(m_invalidAlarmId != "")
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
				"Ignoring, invalid msg alarm has already been raised");
			return;
		}
		
		//Raises an AtsInvalidMessageParameter.
		//
		//The description contains
		//SubSystem=SubsystemName
		TA_Base_Core::DescriptionParameters descriptionParameters;
		TA_Base_Core::NameValuePair pair1 ( "Interface", "");
		descriptionParameters.push_back ( &pair1 );
		
		m_invalidAlarmId = m_taAlarms.submitNonUniqueAlarm(TA_Base_Core::SignallingAgentAlarms::AtsInvalidMessageParameter,
											  descriptionParameters);
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
				"test++ raiseInvalidMessageAlarm");
	}	
    FUNCTION_EXIT;
}
//++zhou yuan

void IncomingTelegramProcessor::parseIncomingData()
{
	FUNCTION_ENTRY("parseIncomingData");

	bool isFinished = false;

	
	while( m_incomingData.size() != 0 && (!isFinished) )
	{
	
		//LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Slice incoming data, Data Cache Size: %d", 
		//				 m_incomingData.size());
		try
		{
			Telegram initialtelegram(m_incomingData);
			MessageDataContainer.push_back(initialtelegram.getPacketData());
			
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"MultiFlag:%d, Size after slice=%d, ContainerSize=%d, MsgSize=%d",
				initialtelegram.getMultiFlag(), m_incomingData.size(),MessageDataContainer.size(),initialtelegram.getLength());
			
			//Check MultiFlag
			if ( Telegram::Multiflag_Set == initialtelegram.getMultiFlag() )
			{
				//LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Multiflag=%d,MessageDataContainer=%d,  wait for the next message..",
				//	  initialtelegram.getMultiFlag(), MessageDataContainer.size() );
				//Wait for next message to complete
				continue;
			}

			//Log the Raw data information
			TA_IRS_Bus::HelperLogs::printRawData(initialtelegram.getPacketData(),"Telegram to send");

			//Process the Telegram
			processTelegram(MessageDataContainer);

		}
		catch( const TelegramException& ex)
		{
			//Check if the Exception case is not equal to IncompletePacket.
			if (ex.getError() != TelegramException::IncompletePacket)
			{
		
				//Telegram is invalid clear the Data Cache
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Invalid Telegram, Remove Data Cache, Size=%d",m_incomingData.size() );
				
				//Log the Invalid Telegram, Raw data information
				HelperLogs::printRawData(m_incomingData,"Invalid Telegram");
				
				//clear Data Cache
				m_incomingData.clear();

				//clear MessageDataContainer cache
				MessageDataContainer.clear();

				
			}
			//If incomplete packet, it means we need to wait and process for the next message
			else
			{
				//it is a IncompletePacket msg. 
				//so we should stop parsing incoming data and remain it for next use
				isFinished = true;
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Incomplete Packet, Size=%d",m_incomingData.size());
			}
			
			

		}
		catch(...)
		{
			//Use for unexpected exception occur			
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Unknown exception occur, Remove Data Cache, Size=%d",m_incomingData.size() );
			
			//Log the raw data information
			HelperLogs::printRawData(m_incomingData,"IncomingTelegramProcessor");

			m_incomingData.clear();
		}

		//LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Remaining Data in a Cache, Size : %d", m_incomingData.size() );

	}

	FUNCTION_EXIT;
}

void IncomingTelegramProcessor::onAgentControl() 
{
    FUNCTION_ENTRY("onAgentControl");
    m_agentInControl = true;
    FUNCTION_EXIT;
}

void IncomingTelegramProcessor::onAgentMonitor() 
{
    FUNCTION_ENTRY("onAgentMonitor");
    m_agentInControl = false;
    FUNCTION_EXIT;
}

void IncomingTelegramProcessor::proessSpecificTelegram (Telegram::ETelegramIdentification identification, const Telegram & initalTelegram)
{
	switch(identification)
	{
		case Telegram::MESSAGE_POLLING:
			{
				processMessagePollingTelegram(initalTelegram);
			}
			break;
		case Telegram::ATS_ISCS_TRAIN_LOCATION:
			{
				processTrainLocationTelegram(initalTelegram);
			}
			break;
		case Telegram::ATS_ISCS_BLOCK_INFO:
			{
				processBlockInfoTelegram(initalTelegram);
			}
			break;
		case Telegram::ATS_ISCS_DYNAMIC_TRAIN_DATA:
			{
				processDynamicTrainDataTelegram(initalTelegram);
			}
			break;
		case Telegram::ATS_ISCS_SIGNAL_STATUS:
			{
				processSignalStatusTelegram(initalTelegram);
			}
			break;
		case Telegram::ATS_ISCS_TRAIN_PIS_INFO:
			{
				processTrainPisInfoTelegram(initalTelegram);
			}
			break;
		case Telegram::ISCS_PLANNED_TRAIN_DATA:
			{
				processPlannedTelegram(initalTelegram);
			}
			break;
		case Telegram::ISCS_PRACTICAL_TRAIN_DATA:
			{
				processPracticalTelegram(initalTelegram);
			}
			break;
	}
}

void IncomingTelegramProcessor::terminate()
{
	FUNCTION_ENTRY("terminate");

	m_isRunning = false;
	m_semaphore.post();

	FUNCTION_EXIT;
}

void IncomingTelegramProcessor::run()
{
	FUNCTION_ENTRY("IncomingTelegramProcessor::run");

	m_isRunning = true;

	while(m_isRunning)
	{

		m_semaphore.wait();

		{
			//copy data from cache
			TA_Base_Core::ThreadGuard guard(m_incomingDataLock);
			for(std::vector<unsigned char>::iterator it = m_incomingDataCache.begin(); 
				it != m_incomingDataCache.end() ; ++it)
			{
				m_incomingData.push_back(*it);
			}

			m_incomingDataCache.clear();
			parseIncomingData();
			 		
		}
		
	}

	FUNCTION_EXIT;
}

void IncomingTelegramProcessor::processTelegram( std::list<MessageData>& messageData  )
{
	FUNCTION_ENTRY("processTelegram");

	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Total Number of FRAMES : %d", messageData.size() );

	MessageData _msgData = messageData.front();
	messageData.pop_front();

	//Copy first MessageData
	Telegram completeTelegram(_msgData);
	while( ! messageData.empty() )
	{
		Telegram telData( messageData.front() );
		completeTelegram.appendData(telData);
		messageData.pop_front();
	}

	//Get TelegramIdentification
	Telegram::ETelegramIdentification identification = completeTelegram.getSIGIdentification();
	std::vector<unsigned char> specificData = completeTelegram.getMessageContent();

	
	Telegram specificTelegram(specificData,false);

	proessSpecificTelegram(identification, specificTelegram);

	//LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Notify Telegram Processor..");
	notifyTelegramProcessors();
	
	FUNCTION_EXIT;
}

void IncomingTelegramProcessor::notifyTelegramProcessors()
{
	FUNCTION_ENTRY("notifyTelegramProcessors");

	if ( 0 != m_telegramMap[Telegram::Message_Polling_Interface].size())
 	{
 		m_messagePollReceiver.receiveTelegram(m_telegramMap[Telegram::Message_Polling_Interface]);
 		m_telegramMap[Telegram::Message_Polling_Interface].clear();
 	}

 	if ( 0 != m_telegramMap[Telegram::Train_Location_Interface].size())
 	{
 		m_trainLocationReceiver.receiveTelegram(m_telegramMap[Telegram::Train_Location_Interface]);
 		m_telegramMap[Telegram::Train_Location_Interface].clear();		
 	}
 
 	if ( 0 != m_telegramMap[Telegram::Block_Information_Interface].size())
 	{
 		m_blockInfoReceiver.receiveTelegram(m_telegramMap[Telegram::Block_Information_Interface]);
 		m_telegramMap[Telegram::Block_Information_Interface].clear();
 	}
 
 	if ( 0 != m_telegramMap[Telegram::Dynamic_Train_Data_Interface].size())
 	{
 		m_dynamicTrainDataReceiver.receiveTelegram(m_telegramMap[Telegram::Dynamic_Train_Data_Interface]);
 		m_telegramMap[Telegram::Dynamic_Train_Data_Interface].clear();
 	}
 
 	if ( 0 != m_telegramMap[Telegram::Signal_Status_Interface].size())
 	{
 		m_signalStatusReceiver.receiveTelegram(m_telegramMap[Telegram::Signal_Status_Interface]);
 		m_telegramMap[Telegram::Signal_Status_Interface].clear();
 	}
 
 	if ( 0 != m_telegramMap[Telegram::Train_PIS_Info_Interface].size())
 	{
 		m_trainPisInfoReceiver.receiveTelegram(m_telegramMap[Telegram::Train_PIS_Info_Interface]);
 		m_telegramMap[Telegram::Train_PIS_Info_Interface].clear();
 	}
 
 	if ( 0 != m_telegramMap[Telegram::Planned_Data_Start_Interface].size())
 	{
 		m_plannedStartReceiver.receiveTelegram(m_telegramMap[Telegram::Planned_Data_Start_Interface]);
 		m_telegramMap[Telegram::Planned_Data_Start_Interface].clear();
 	}
 
 	if ( 0 != m_telegramMap[Telegram::Planned_Data_Content_Interface].size())
 	{
 		m_plannedContentReceiver.receiveTelegram(m_telegramMap[Telegram::Planned_Data_Content_Interface]);
 		m_telegramMap[Telegram::Planned_Data_Content_Interface].clear();
 	}
 
 	if ( 0 != m_telegramMap[Telegram::Planned_Data_End_Interface].size())
 	{
 		m_plannedEndReceiver.receiveTelegram(m_telegramMap[Telegram::Planned_Data_End_Interface]);
 		m_telegramMap[Telegram::Planned_Data_End_Interface].clear();
 	}
 
 	//
 	if ( 0 != m_telegramMap[Telegram::Practical_Data_Start_Interface].size())
 	{
 		m_practicalStartReceiver.receiveTelegram(m_telegramMap[Telegram::Practical_Data_Start_Interface]);
 		m_telegramMap[Telegram::Practical_Data_Start_Interface].clear();
 	}
 
 	if ( 0 != m_telegramMap[Telegram::Practical_Data_Content_Interface].size())
 	{
 		m_practicalContentReceiver.receiveTelegram(m_telegramMap[Telegram::Practical_Data_Content_Interface]);
 		m_telegramMap[Telegram::Practical_Data_Content_Interface].clear();
 	}
 
 	if ( 0 != m_telegramMap[Telegram::Practical_Data_End_Interface].size())
 	{
 		m_practicalEndReceiver.receiveTelegram(m_telegramMap[Telegram::Practical_Data_End_Interface]);
 		m_telegramMap[Telegram::Practical_Data_End_Interface].clear();
 	}

	FUNCTION_EXIT;
}

void IncomingTelegramProcessor::connectionLost()
{
	FUNCTION_ENTRY("connectionLost");

	//Clear the MessageDataContainer Cache when there is connectionLost
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Receive Connection Lost, MessageDataContainer Cached will be cleared.");
	
	MessageDataContainer.clear();
	m_incomingData.clear();

	FUNCTION_EXIT;
}
