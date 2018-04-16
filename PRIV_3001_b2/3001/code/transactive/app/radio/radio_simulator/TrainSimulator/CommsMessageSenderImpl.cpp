#include "StdAfx.h"
#include "CommsMessageSenderImpl.h"

#include "core/message/types/TrainAgentBroadcastComms_MessageTypes.h"
#include "core/message/types/TrainAgentComms_MessageTypes.h"
#include "bus/trains/TrainCorbaProxy/src/TrainInformationConversion.h"
#include "core/utilities/src/RunParams.h"
#include "core/corba/src/CorbaUtil.h"

#include "EtcTrainSimConfigHandler.h"

#include "bus\security\duty_agent\IDL\src\DutyAgentTypeCorbaDef.h"
#include "core\message\types\DutyAgentBroadcastComms_MessageTypes.h"

CommsMessageSenderImpl::CommsMessageSenderImpl(void)
{
	using TA_Base_Core::MessagePublicationManager;

	char * pszData = "1";
	char * pszHost = "localhost:6667";
	char * pszPort = "6667";
	
	TA_Base_Core::CorbaUtil::getInstance().initialise(6666);
	TA_Base_Core::CorbaUtil::getInstance().activate();

	TA_Base_Core::RunParams::getInstance().set(RPARAM_LOCATIONKEY,pszData);
	TA_Base_Core::RunParams::getInstance().set(RPARAM_NOTIFYHOSTS,pszHost);
	TA_Base_Core::RunParams::getInstance().set(RPARAM_NOTIFYPORT,pszPort);

	MessagePublicationManager& messagePublicationManager = MessagePublicationManager::getInstance();
	//m_commsMessageSender = messagePublicationManager.( TA_Base_Core::TrainAgentComms::Context );

	m_commsMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender(TA_Base_Core::DutyAgentBroadcastComms::Context);

	
}

CommsMessageSenderImpl::~CommsMessageSenderImpl(void)
{
	delete m_commsMessageSender;
	m_commsMessageSender = NULL;
	
	TA_Base_Core::CorbaUtil::getInstance().shutdown();
	TA_Base_Core::CorbaUtil::getInstance().stop();

}

void CommsMessageSenderImpl::sendStateUpdate (bool bRelease)
{
	// uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
	
// 	TA_IRS_Bus::TrainInformationTypes::AgentOperationMode update;
// 	
// 	std::vector <unsigned long> vctrDataBuf = EtcTrainSimConfigHandler::getInstance()->getControlledLocation();
// 	
// 	std::set<unsigned long> dataBuffer;
// 
// 	dataBuffer.insert(vctrDataBuf.begin(),vctrDataBuf.end());
// 
// 	update.localDuty = true;
// 	update.controlledLocations = dataBuffer;
// 	update.doubleAtsFailure = false;
// 	update.fallbackMode = false;
// 	update.groupOffline = false;
// 	
// 	
// 
// 	CORBA::Any data;
// 	data <<= TA_IRS_Bus::TrainInformationConversion::convertToCorba( update );
// 	
// 	m_commsMessageSender->sendCommsMessage(
// 											TA_Base_Core::TrainAgentComms::OperationModeUpdate,
// 											20000909,
// 											data,
// 											0, // not used
// 											1 );

	
	std::vector <unsigned long> vctrDataBuf = EtcTrainSimConfigHandler::getInstance()->getControlledLocation(bRelease);
	CORBA::Any data;
	TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence_var subsystemStates = new TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence();

	subsystemStates->length(2);
	
	vctrDataBuf.clear();
	vctrDataBuf.push_back(12);
	vctrDataBuf.push_back(13);

	if (bRelease)
	{
		
		unsigned long i;

		for ( i=0;i <vctrDataBuf.size(); i++ )
		{
			subsystemStates[i].currState = 2;
			subsystemStates[i].prevState = 0;
			subsystemStates[i].regionKey = vctrDataBuf.at(i);
			subsystemStates[i].subsystemKey = 75; // TrainComms

		}

		data <<= subsystemStates;
	}
	else
	{
		unsigned long i;

		for ( i=0;i <vctrDataBuf.size(); i++ )
		{
			subsystemStates[i].currState = 0;
			subsystemStates[i].prevState = 2;
			subsystemStates[i].regionKey = vctrDataBuf.at(i);
			subsystemStates[i].subsystemKey = 75; // TrainComms

		}

		data <<= subsystemStates;

	}
	
	m_commsMessageSender->sendBroadcastCommsMessage(TA_Base_Core::DutyAgentBroadcastComms::SubsystemStateChangeNotification, data);
}
