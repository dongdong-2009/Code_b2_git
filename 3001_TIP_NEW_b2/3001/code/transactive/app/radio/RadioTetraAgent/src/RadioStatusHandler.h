/**
 *
 * To access gateway to retrieve data and synchronize with ISCS database 
 */
#ifndef RADIO_STATUS_HANDLER_H_INCLUDED
#define RADIO_STATUS_HANDLER_H_INCLUDED
#include "app/radio/RadioTetraAgent/RadioTcpCommunication/src/BaseTcpServer.h"

#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethodSystemStatus.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventSystemStatus.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethod.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioProtocol.h"
#include "app/radio/RadioTetraAgent/src/RadioDpStatusUpdateThread.h"

#include "bus/trains/TrainCorbaProxy/src/TrainOperationModeCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"

#include "core/data_access_interface/radio/src/IRadioSubscriber.h"
#include "bus/radio/radio_bus/src/radiotypes.h"

#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/exceptions/src/DataException.h"

#include "ace/Time_Value.h"
#include "ace/OS_NS_unistd.h"
#include "ace/Task.h"
#include <vector>

namespace TA_IRS_App {
	class BaseTcpServer;

	class RadioStatusHandler : public virtual ACE_Task_Base {
	public:
		RadioStatusHandler(BaseTcpServer* sender, TA_Base_Bus::DataPoint* dp);
		~RadioStatusHandler();

		void initialize();
		void start();
		void stop();
		virtual int svc();

		void notifyMftAgent(short status);
		void notifyTrainAgent(bool status);

		bool getStatus();

	protected:
		bool createFallbackDp();

	private:
		bool m_terminate;
		BaseTcpServer* m_tcpSender;
		TA_Base_Core::CommsMessageSender* m_commsSender;
		RadioDpStatusUpdateThread* m_dpUpdateThread;

		int m_status;
		TA_Base_Core::IEntityData* m_radioEntity;
		unsigned long m_entityKey;
		unsigned long m_locationKey;
		unsigned long m_subsystemKey;
		TA_Base_Bus::DataPoint* m_dataPoint;

		// to notify train agent
		TA_IRS_Bus::TrainOperationModeCorbaProxyPtr m_trainAgentProxy;

		// status command for OCC (0) or STATION (1)
		unsigned short m_messageType; 
		
		static unsigned short OCC_SYSTEM_STATUS;
		static unsigned short STATION_SYSTEM_STATUS;
	};
}
#endif