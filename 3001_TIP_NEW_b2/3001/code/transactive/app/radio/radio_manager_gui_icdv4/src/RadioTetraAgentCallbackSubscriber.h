#pragma once

#include "app/radio/radio_manager_gui_icdv4/src/Stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"

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

using namespace TA_IRS_Bus;

class RadioTetraAgentCallbackSubscriber : public ITrainDriverCallUpdateObserver
{
public: 
		RadioTetraAgentCallbackSubscriber();
		~RadioTetraAgentCallbackSubscriber();
	

        virtual void processOccCallRequest( const CallTypes::OccCallRequest& event );
        virtual void processOccCallReset( const CallTypes::OccCallReset& event );
		void createProxies();
		void initialise();

private:
	
        TrainDriverCallCorbaProxyPtr m_trainDriverCallCorbaProxy;
        TrainInformationCorbaProxyPtr m_trainInformationCorbaProxy;
		TA_Base_Core::NonReEntrantThreadLockable m_notificationsLock;
};