#ifndef _TRAIN_BORNE_DISPLAY_MANAGER_SWITCHAGENT_COMMUNICATOR_
#define _TRAIN_BORNE_DISPLAY_MANAGER_SWITCHAGENT_COMMUNICATOR_
//#include "bus/trains/TrainCorbaProxy/src/TrainCctvCorbaProxy.h"
#include "bus/trains/traincommonlibrary/src/commontypes.h"
#include "bus/cctv/tvss_agent/TVSSManager/IDL/src/TVSSManagerCorbaDef.h"
#include "core/naming/src/namedobject.h"

namespace TA_IRS_App
{
	typedef TA_Base_Core::NamedObject< TA_Base_Bus::TVSSManagerCorbaDef,
										TA_Base_Bus::TVSSManagerCorbaDef_ptr,
									    TA_Base_Bus::TVSSManagerCorbaDef_var >		TVSSManagerNamedObject;
	class SwitchAgentCommunicator
	{
	public:
		
		SwitchAgentCommunicator();
		~SwitchAgentCommunicator();
		bool mappingTrainBorneMonitor(unsigned long monitorKey);
		bool setSingleScreen(long trainID,unsigned char monitorID,unsigned char cameraID);

		//bool setQuad(long trainID,unsigned char monitorID,unsigned char cameraW,unsigned char cameraX,unsigned char cameraY,unsigned char cameraZ);
		bool setQuad(long trainID,unsigned char monitorID,unsigned char quadId);

		bool setSequence(long trainID,unsigned char monitorID,unsigned char sequenceID);

		bool clearMonitor(long trainID,unsigned char monitorID);

		bool vssClearMonitor(unsigned long monitorKey);

		bool trainBorneAlarmEvent(long trainID,const char* eventStr);

		bool trainBorneAcknowleageAlarm(unsigned long entiytKey,const char* sessionID);

		bool setAcknowledgeEvent(long trainID,unsigned char monitorID,unsigned char cameraW,unsigned char cameraX);

		bool equipmentAlarmQuery();
		//void setSingleScreen(TA_IRS_Bus::CommonTypes::TrainIdType trainID,TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera camera);
		//void setQuadScreen(TA_IRS_Bus::CommonTypes::TrainIdType trainID,TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraW,TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraX,TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraY,TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraZ);
		//void setSequence(TA_IRS_Bus::CommonTypes::TrainIdType trainID,TA_IRS_Bus::ProtocolCctvTypes::ECctvSequenceNumber sequenceNo,TA_IRS_Bus::ProtocolCctvTypes::ECctvSequenceMode sequenceMode=TA_IRS_Bus::ProtocolCctvTypes::SequenceHold);
		static SwitchAgentCommunicator& getInstance();
		static void setLocationName(std::string locationName);
	private:
//		TA_IRS_Bus::TrainCctvCorbaProxyPtr m_trainCorbaProxy;
		static SwitchAgentCommunicator* m_theInstance;
        static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;
		static std::string m_locationName;
		TVSSManagerNamedObject m_tvssManagerNameObj;
	};
}

#endif