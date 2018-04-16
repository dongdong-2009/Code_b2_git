#pragma once

#include "app/radio/radio_manager_gui/src/Stdafx.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui/src/RadioErrorMsg.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentHelper.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"
#include "bus/radio/radio_agent/idl/src/RadioServerAgentIDL.h"
#include "bus/radio/radio_agent/idl/src/CommonRadioAgentIDL.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainDriverCallUpdateObserver.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainInformationUpdateObserver.h"
#include "bus/trains/TrainCorbaProxy/src/TrainDriverCallCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#include "bus\trains\TrainCorbaProxy\src\TrainInformationCorbaProxy.h"

#include "bus\trains\traincommonlibrary\src\calltypes.h"
#include "bus\trains\TrainCommonLibrary\src\CommonTypes.h"
#include "bus\trains\TrainCommonLibrary\src\TrainInformationTypes.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/src/ISession.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/entity_access/src/RadioEntityData.h"

#include "boost/algorithm/string/case_conv.hpp"

using namespace TA_IRS_Bus;

class RadioTetraAgentCallbackSubscriber : public ITrainDriverCallUpdateObserver
										,public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
{
public: 
	RadioTetraAgentCallbackSubscriber();
	~RadioTetraAgentCallbackSubscriber();


	virtual void processOccCallRequest( const CallTypes::OccCallRequest& event );
	virtual void processOccCallReset( const CallTypes::OccCallReset& event );        
	virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);
	void createProxies();
	void initialise();

private:
	void processProfileUpdate(const TA_IRS_Bus::RadioServerAgentIDL::ProfileTgSync& event);
	void processDbUpdate(const TA_IRS_Bus::RadioServerAgentIDL::DbResSync& event);


	TrainDriverCallCorbaProxyPtr m_trainDriverCallCorbaProxy;
	TrainInformationCorbaProxyPtr m_trainInformationCorbaProxy;
	TA_Base_Core::NonReEntrantThreadLockable m_notificationsLock;
	unsigned long m_profileKey;
};