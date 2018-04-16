
#include "app/radio/radio_simulator/CommandProcessorLib/src/ChangeModeCommandProcessor.h"
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMHelper.h"
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMTypes.h"

namespace TA_IRS_App
{
	ChangeModeCommandProcessor::ChangeModeCommandProcessor( IPACMData& paCMData )
		: TrainCommandProcessorThread(paCMData)
	{
		FUNCTION_ENTRY("ChangeModeCommandProcessor");
		FUNCTION_EXIT;
	}

	ChangeModeCommandProcessor::~ChangeModeCommandProcessor()
	{
		FUNCTION_ENTRY("~ChangeModeCommandProcessor");

		//stop the thread
		terminateAndWait();

		FUNCTION_EXIT;
	}

	void ChangeModeCommandProcessor::sendSDSMessage(boost::shared_ptr<SDSMessageData>& sdsMessage)
	{
		FUNCTION_ENTRY("sendSDSMessage");

		addTrainCommand(sdsMessage);

		FUNCTION_EXIT;
	}

	void ChangeModeCommandProcessor::processTrainCommand(boost::shared_ptr<SDSMessageData>& sdsMessage)
	{
		FUNCTION_ENTRY("processTrainCommand");

		SDSMessageData::VMessage pData = sdsMessage->getData();
		SDSMessageData::VMessage resp = sdsMessage->getResponse();
		int userDataIndex = resp.size();
		bool isValidCommand = false;

		switch( sdsMessage->getCommandType())
		{
		case TETRA_ISCS_MODE:
			m_logger->setLogView("Receive ChangeModeCommand : 0x49");
			resp.push_back(USER_DATA_FLAG);
			resp.push_back(CHANGEOVER_STATUS);
			resp.push_back(pData[23]); // physical train number
			resp.push_back(9); // Length
			resp.push_back(0); // Origin, TIMS = 0
			resp.push_back(0);
			resp.push_back(0);

			isValidCommand = true;
			break;

		}

		if ( true == isValidCommand )
		{
			int _trainID = static_cast<int>(pData[23]);

			//send first response
			//processSendData(userDataIndex,resp,pData);
		}

		FUNCTION_EXIT;
	}
}