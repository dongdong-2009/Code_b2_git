#ifndef RADIO_MFT_AGENT_REMOTEAPI_EVENT_IMPL_H_INCLUDED
#define RADIO_MFT_AGENT_REMOTEAPI_EVENT_IMPL_H_INCLUDED

#include"app/radio/radio_mft_agent_icdv4/src/RemoteAPIRadioEvent.h"

#include "app/radio/radio_mft_agent_icdv4/src/IRadioMFTAgentCallback.h"
#include "app/radio/radio_mft_agent_icdv4/src/IRadioMFTAgentCommand.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentStatusManager.h"
#include "app/radio/radio_mft_agent_icdv4/src/FutureManager.h"
#include "app/radio/radio_mft_agent_icdv4/src/FutureResult.h"
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/ConsoleDispatchEvent.h"
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/CDI_Event.h"
#include "bus/radio/Radio_Agent/idl/src/RadioMFTAgentCallbackIDL.h"
#include "bus/radio/Radio_Agent/idl/src/RadioServerAgentIDL.h"
#include "core/naming/src/NamedObject.h"
#include "app/radio/radio_mft_agent_icdv4/src/IMcdbRefreshCallback.h"
typedef unsigned long ResourceIdentifier;

class RadioMFTAgentRemoteApiEventImpl : public ICDV4::IMcdbRefreshCallback
{

	public:

		RadioMFTAgentRemoteApiEventImpl(MCC7500SDK::IAPIRequest_ptr &ptr);

		virtual ~RadioMFTAgentRemoteApiEventImpl();

	public:
		void closeAlarm(const TA_Base_Core::MessageType& pAlarm);
		
		void submitAlarm(const TA_Base_Core::MessageType& pAlarm);
		
		void onMcdiDispatchReady();

		void onMcdiDispatchNotReady();

		void onMcdiRetrieveRCDatabaseRequest();

		void onMcdiRetrieveRCDatabaseResponseAck();

        void onMcdiSynchronisationStart();

		void onMcdiSyncCompletionStatus();

		void onMcdiSynchronisationComplete();

		void onResourceTransmitStatus(ICDV4::RemoteAPIRadioEvent* pConEvent);

		void onEmergencyAlarmStatus(ICDV4::RemoteAPIRadioEvent* pConEvent);

		void onEmergencyCallStatus(ICDV4::RemoteAPIRadioEvent* pConEvent);

		bool onGeneralTransmitStatus(ICDV4::RemoteAPIRadioEvent* pConEvent, ACE_Future<FutureResultT>& future); // return true if BeginGeneralTransmit 
																		 //	was invoked on answering private call

		void onInboundCallStatus(ICDV4::RemoteAPIRadioEvent* pConEvent);

		void onInboundPttIdStatus(ICDV4::RemoteAPIRadioEvent* pConEvent);

		void onInboundPrivateCallStatus(ICDV4::RemoteAPIRadioEvent* pConEvent);

		void onInboundRadioMessageStatus(ICDV4::RemoteAPIRadioEvent* pConEvent);

		void onRefreshStatus(ICDV4::RemoteAPIRadioEvent* pConEvent);

		void onResourceVolumeStatus(ICDV4::RemoteAPIRadioEvent* pConEvent);

		bool onResourceSelectStatus(ICDV4::RemoteAPIRadioEvent* pConEvent);  // return true if the selected resource is a PrivateCall resource

		bool onAssignmentStatus(ICDV4::RemoteAPIRadioEvent* pConEvent);  // return true if Assignment command is done by this console operator

		void onAssignResourceErr(ICDV4::RemoteAPIRadioEvent* pConEvent, bool bAssign);

		bool onPrivateCallStatus(ICDV4::RemoteAPIRadioEvent* pConEvent);

		void onAmbienceListeningCallStatus(ICDV4::RemoteAPIRadioEvent* pConEvent);

		void onGetFreePatchGroup(ICDV4::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id);

		void onPatchGroupStatus(ICDV4::RemoteAPIRadioEvent* pConEvent);

		void onPatchMemberStatus(ICDV4::RemoteAPIRadioEvent* pConEvent);

		void onPatchMemberRefresh(ICDV4::RemoteAPIRadioEvent* pConEvent);

		void onGetFreeMselGroup(ICDV4::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id);

		void onMselGroupStatus(ICDV4::RemoteAPIRadioEvent* pConEvent);

		void onMselMemberStatus(ICDV4::RemoteAPIRadioEvent* pConEvent);

		void onMselMemberRefresh(ICDV4::RemoteAPIRadioEvent* pConEvent);

        void onMcdbRefreshComplete();

		void onSelectResourceError(ICDV4::RemoteAPIRadioEvent* pConEvent);

		void onOpenMselError(ICDV4::RemoteAPIRadioEvent* pConEvent);

		void onCloseMselError(ICDV4::RemoteAPIRadioEvent* pConEvent);

        // ++TD14322
		void onAddMselMemberWrn(ICDV4::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id);
		void onAddMselMemberErr(ICDV4::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id);
		void onDeleteMselMemberErr(ICDV4::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id);
		void onAddPatchMemberWrn(ICDV4::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id);
		void onAddPatchMemberErr(ICDV4::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id);
		void onDeletePatchMemberErr(ICDV4::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id);
		void onBeginGeneralXmitWrn(ICDV4::RemoteAPIRadioEvent* pConEvent);
		void onBeginGeneralXmitErr(ICDV4::RemoteAPIRadioEvent* pConEvent);

        void sendErrorMessage(const std::string msg);
        // TD14322++

		void onEmergencyAlarmTone(ICDV4::RemoteAPIRadioEvent* pConEvent);

		bool addCallback(TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr pCallback);

		void setMFTCommand(IRadioMFTAgentCommand* pCommand);

		void updateGroupResource(ICDV4::RemoteAPIRadioEvent* pConEvent, ACE_Future<FutureResultT>& future);

		void processPrivateCallFunc(ACE_Future<FutureResultT>& future);

		//++TD14294
		//void processPhoneCallFunc(ACE_Future<FutureResultT>& future);
		//bool isPhoneSelected(ICDV4::RemoteAPIRadioEvent* pConEvent);
        // TD14322++

		void reconnect() {};

	private:
        // disabled methods
		RadioMFTAgentRemoteApiEventImpl(const RadioMFTAgentRemoteApiEventImpl&);
		RadioMFTAgentRemoteApiEventImpl& operator=(const RadioMFTAgentRemoteApiEventImpl&);



		IRadioMFTAgentCallback::ResourceActivityState convertResourceTXStatus (
				TA_IRS_App::Mca::ResourceTransmitStatus::RegT status );


	private:
		/**
		*	Used for safe acess;
		*/
		TA_Base_Core::ReEntrantThreadLockable m_lock;

		IRadioMFTAgentCallback* m_pCallback;

		IRadioMFTAgentCommand* m_pCommand;

		
		TA_Base_Core::NamedObject<TA_IRS_Bus::RadioServerAgentIDL,
							  TA_IRS_Bus::RadioServerAgentIDL_ptr,
					  		  TA_IRS_Bus::RadioServerAgentIDL_var> m_serverAgent;
		
		RadioApplicationStateManager* m_pApStateMgr;
		
		bool m_bOperationMode;

};
#endif
