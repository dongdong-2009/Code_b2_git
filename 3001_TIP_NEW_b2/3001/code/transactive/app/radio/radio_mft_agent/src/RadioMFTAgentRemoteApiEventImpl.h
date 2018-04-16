#ifndef RADIO_MFT_AGENT_REMOTEAPI_EVENT_IMPL_H_INCLUDED
#define RADIO_MFT_AGENT_REMOTEAPI_EVENT_IMPL_H_INCLUDED
#include"app/radio/radio_mft_agent/src/RemoteAPIRadioEvent.h"

#include "app/radio/radio_mft_agent/src/IRadioMFTAgentCallback.h"
#include "app/radio/radio_mft_agent/src/IRadioMFTAgentCommand.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentStatusManager.h"
#include "app/radio/radio_mft_agent/src/FutureManager.h"
#include "app/radio/radio_mft_agent/src/FutureResult.h"
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/CDI_Event.h"
#include "bus/radio/Radio_Agent/idl/src/RadioMFTAgentCallbackIDL.h"
#include "bus/radio/Radio_Agent/idl/src/RadioServerAgentIDL.h"
#include "core/naming/src/NamedObject.h"
#include"app/radio/radio_mft_agent/src/IMcdbRefreshCallback.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
typedef unsigned long ResourceIdentifier;

#include <map>

using namespace TA_IRS_App;

#define MCA_PENDING 0x20
#define MCA_PENDING_AND_IN_USE 0x21

class RadioMFTAgentRemoteApiEventImpl : public IMcdbRefreshCallback
{

	public:

		RadioMFTAgentRemoteApiEventImpl(MCC7500SDK::IAPIRequest_ptr& ptr, TA_Base_Core::ReEntrantThreadLockable& apiLock);

		virtual ~RadioMFTAgentRemoteApiEventImpl();

	public:
		void closeAlarm(const TA_Base_Core::MessageType& pAlarm);
		
		void submitAlarm(const TA_Base_Core::MessageType& pAlarm);
		
		void onMccStatus(short index);

		void onMcdiDispatchReady();

		void onMcdiLocalTrunking();

		void onMcdiNormalMode();

		void onMcdiDispatchNotReady();

		void onMcdiRetrieveRCDatabaseRequest();

		void onMcdiRetrieveRCDatabaseResponseAck();

        void onMcdiSynchronisationStart();

		void onMcdiSyncCompletionStatus();

		void onMcdiSynchronisationComplete();

		void onResourceTransmitStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);

		void onEmergencyAlarmStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);

		void onEmergencyCallStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);

		bool onGeneralTransmitStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, ACE_Future<FutureResultT>& future); // return true if BeginGeneralTransmit 
																		 //	was invoked on answering private call

		void onInboundCallStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);

		void onInboundPttIdStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);

		void onInboundPrivateCallStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);

		void onInboundRadioMessageStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);

		void onRefreshStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);

		void onResourceVolumeStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);

		bool onResourceSelectStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);  // return true if the selected resource is a PrivateCall resource

		bool onAssignmentStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);  // return true if Assignment command is done by this console operator

		void onAssignResourceErr(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, bool bAssign);

		bool onPrivateCallStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);

		void onAmbienceListeningCallStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);

		void onGetFreePatchGroup(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id);

		void onGetFreePatchGroupError();

		void onPatchGroupStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);

		void onPatchMemberStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);

		void onPatchMemberRefresh(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);

		void onGetFreeMselGroup(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id);

		void onMselGroupStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);

		void onMselMemberStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);

		void onMselMemberRefresh(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);

        void onMcdbRefreshComplete(short mode);

		void onSelectResourceError(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, ACE_Future<FutureResultT>& future);

		void onDeselectResourceError(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, ACE_Future<FutureResultT>& future);

		void onOpenMselError(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);

		void onCloseMselError(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);

        // ++TD14322
		void onAddMselMemberWrn(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id);
		void onAddMselMemberErr(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id);
		void onDeleteMselMemberErr(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id);
		void onAddPatchMemberWrn(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id);
		void onAddPatchMemberErr(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id);
		void onDeletePatchMemberErr(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id);
		void onBeginGeneralXmitWrn(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);
		void onBeginGeneralXmitErr(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);

		void onNewDatabaseAvailable(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);

		void onDatabaseUpdateNotification(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);

		void onResourceRejectStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);
		
		void onEndCallTerimination (TA_IRS_App::RemoteAPIRadioEvent * pConEvent);

        void sendErrorMessage(const std::string msg);

		long sendErrorMessage(long messageID);
        // TD14322++

		void onEmergencyAlarmTone(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);

		bool addCallback(TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr pCallback);

		void setMFTCommand(IRadioMFTAgentCommand* pCommand);

		void updateGroupResource(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, ACE_Future<FutureResultT>& future);

		void processSelectResourceFunc(ACE_Future<FutureResultT>& future);

		void reconnect();

		void resyncDbIscs();

		void resyncDbMcc();

		void onSetupCallFailure ();

		bool onEndPrivCallErrHandler(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);
		void onResendEndPrvCall();

		//++TD14294
		//void processPhoneCallFunc(ACE_Future<FutureResultT>& future);
		//bool isPhoneSelected(TA_IRS_App::RemoteAPIRadioEvent* pConEvent);
        // TD14322++

		static const short DB_STATUS = 0;
		static const short AUDIO_STATUS = 1; 
		static const short PERIPHERAL_STATUS = 2;
	private:
        // disabled methods
		RadioMFTAgentRemoteApiEventImpl(const RadioMFTAgentRemoteApiEventImpl&);
		RadioMFTAgentRemoteApiEventImpl& operator=(const RadioMFTAgentRemoteApiEventImpl&);



		IRadioMFTAgentCallback::ResourceActivityState convertResourceTXStatus (
				int status );


	private:
		/**
		*	Used for safe acess;
		*/
		TA_Base_Core::ReEntrantThreadLockable m_lock;

		IRadioMFTAgentCallback* m_pCallback;

		IRadioMFTAgentCommand* m_pCommand;

		
		/*TA_Base_Core::NamedObject<TA_IRS_Bus::RadioServerAgentIDL,
							  TA_IRS_Bus::RadioServerAgentIDL_ptr,
					  		  TA_IRS_Bus::RadioServerAgentIDL_var> m_serverAgent;*/
		
		RadioApplicationStateManager* m_pApStateMgr;
		
		bool m_bOperationMode;

		// to fulfill 3 conditions before any API function can be called to MCC
		// RadioManager Initialization only needs 2 messages from MCC Server
		// DB_READY and AUDIO_PROCESSING
		bool mccReady[2];
		bool initialized;
		RadioResourceType m_curUpdateType;

};
#endif
